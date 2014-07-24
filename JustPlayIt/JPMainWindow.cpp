#include "StdAfx.h"
#include "JPMainWindow.h"
#include "Util.h"
#include <boost/bind.hpp>
#include <atlapp.h>
#include <atldlgs.h>

#pragma comment(lib,"libvlc.lib")
#pragma comment(lib,"libvlccore.lib")

#define ID_TIMER_HOOK_VLC			(5002)
#define ID_TIMER_VIDEO_DBCLICK		(5003)
#define ID_TIMER_HIDE_BAR			(5004)
#define ID_TIMER_SHOW_BAR			(5005)

#define DURATION_TIMER_SHOW_BAR		200
#define DURATION_TIMER_HIDE_BAR		2000
#define DURATION_TIMER_HOOK_VLC		2000
#define ALPHA_PLAYER_BAR			150
HHOOK g_Hook = NULL;

RECT rcDefault = {0, 0, 1000, 600};

LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	POINT ptScreen = {0};
	::GetCursorPos(&ptScreen);
	HWND hTopWnd = ::WindowFromPoint(ptScreen);
	TCHAR szClassName[MAX_PATH] = {0};
	::GetClassName(hTopWnd,szClassName,MAX_PATH);
	if(wcscmp(szClassName,JUSTPLAYIT_WINDOW_NAME) != 0 && wcscmp(szClassName,JUSTPLAYIT_BOTTOMBAR_NAME) != 0 && wcscmp(szClassName,JUSTPLAYIT_TOPBAR_NAME) != 0)
	{
		return CallNextHookEx(g_Hook, nCode, wParam, lParam);
	}
    MSLLHOOKSTRUCT* pHookStruct = (MSLLHOOKSTRUCT*)lParam;
    if (!pHookStruct)
    {
        return CallNextHookEx(g_Hook, nCode, wParam, lParam);
    }
    POINT pt = pHookStruct->pt;

    if (wParam == WM_LBUTTONDOWN)
    {
        HWND hWnd = CJPMainWindow::Instance()->GetHWND();
        if (IsWindow(hWnd) && IsWindowVisible(hWnd) && IsWindowEnabled(hWnd))
        {
            RECT mainRect = {0};
            GetWindowRect(hWnd, &mainRect);
            if (PtInRect(&mainRect, pt))
            {
                CJPMainWindow::Instance()->SetDoubleClick();
                SetTimer(hWnd, ID_TIMER_VIDEO_DBCLICK, 200, NULL);
                if (CJPMainWindow::Instance()->IsDoubleClick())
                {
                    ::PostMessage(hWnd, WM_VIDEO_DBCLICK, 0, MAKELPARAM(pt.x, pt.y));
                }
            }
        }
    }
    else if (wParam == WM_MOUSEMOVE)
    {
        HWND hWnd = CJPMainWindow::Instance()->GetHWND();
        if (IsWindow(hWnd) && IsWindowVisible(hWnd) && IsWindowEnabled(hWnd))
        {
            RECT mainRect = {0};
            GetWindowRect(hWnd, &mainRect);
			if (PtInRect(&mainRect, pt))
            {
				//只有鼠标左键不是按下状态时才显示playerbar，防止移动窗口或者改变大小时闪烁playerbar
				if(!GetAsyncKeyState(VK_LBUTTON) && CJPMainWindow::Instance()->IsAutoShowBar())
				{
					//需要延迟显示，否则windowposchange时显示playerbar会导致卡顿
					::SetTimer(hWnd,ID_TIMER_SHOW_BAR,DURATION_TIMER_SHOW_BAR,NULL);
				}
            }
        }
    }
    return CallNextHookEx(g_Hook, nCode, wParam, lParam);
}

BOOL CALLBACK EnumVlcWndProc(HWND hWnd, LPARAM lParam)
{
    EnableWindow(hWnd, FALSE);
    DWORD dwVlcThreadId = 0;
    TCHAR className[MAX_PATH] = {0};
    GetClassName(hWnd, className, MAX_PATH);
    if (IsWindow(hWnd) && g_Hook == NULL)
    {
        //使用WH_MOUSE失败，改用全局HOOK WH_MOUSE_LL
        g_Hook = SetWindowsHookEx(WH_MOUSE_LL, HookProc, GetModuleHandle(NULL), 0);
		HWND hParent = GetParent(hWnd);
		if (IsWindow(hParent))
		{
			if(g_Hook != NULL)
			{
				::KillTimer(hParent, ID_TIMER_HOOK_VLC);
			}
		}
    }
    return TRUE;
}

void OnPlayingStart( const struct libvlc_event_t *pEvent, void *pUserData )
{
	CJPMainWindow* pMainWnd = (CJPMainWindow*)pUserData;
	if(pMainWnd)
	{
		pMainWnd->OnPlayBegin();
	}
}

void OnPlayingStop( const struct libvlc_event_t *pEvent, void *pUserData )
{
	CJPMainWindow* pMainWnd = (CJPMainWindow*)pUserData;
	if(pMainWnd)
	{
		pMainWnd->OnPlayEnd(true);
	}
}

void OnPlayingEnd( const struct libvlc_event_t *pEvent, void *pUserData )
{
	CJPMainWindow* pMainWnd = (CJPMainWindow*)pUserData;
	if(pMainWnd)
	{
		pMainWnd->OnPlayEnd(false);
	}
}

void OnPlayingError( const struct libvlc_event_t *pEvent, void *pUserData )
{
	CJPMainWindow* pMainWnd = (CJPMainWindow*)pUserData;
	if(pMainWnd)
	{
		pMainWnd->OnPlayFailed();
	}
}


CJPMainWindow::CJPMainWindow(void)
{
    m_vlc_instance = NULL;
    m_vlc_player = NULL;
    m_vlc_media = NULL;
    m_vlc_log = NULL;
	m_vlc_event_manager = NULL;

    m_bottomBar = NULL;
	m_topBar = NULL;
    m_nDoubleClick = 0;
    m_bFullScreen = false;
    m_lastRect = rcDefault;
	m_oldStyle = 0;
	m_sourceType = EMediaType_Unknown;
	m_editUri = NULL;
	m_btnOpenFile = NULL;
	m_playerActive = false;
	m_bAutoShowBar = false;

	m_btnWelcomeClose = NULL;
	m_btnWelcomeMax = NULL;
	m_btnWelcomeMin = NULL;
	m_btnWelcomeRestore = NULL;
	m_labelPlayerError = NULL;

	//InitPlayer();
}

CJPMainWindow::~CJPMainWindow(void)
{
}

void CJPMainWindow::Notify(DuiLib::TNotifyUI& msg)
{
	if(msg.sType == DUI_MSGTYPE_CLICK)
	{
		if(msg.pSender->GetName() == UI_NAME_BUTTON_OPENFILE)
		{
			if(m_editUri && !m_editUri->GetText().IsEmpty())
			{
				OnOpenFileClick(LPCTSTR(m_editUri->GetText()));
			}
			else
			{
				WTL::CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
					_T("媒体类型\0*.mp4; *.3gp; *.mov; *.qt; *.3gpp; *.avi; *.asf; *.wmv; *.rm; *.rmvb; *.ram; *.flv; *.f4v; *.flc; \
					   *.fli; *.mkv; *.mpg; *.ts; *.m2t; *.mpeg; *.m2v; *.m2p; *.m4v; *.vob;*.m2ts; *.mp3; *.ogg; *.wma; *.amr; *.wav; *.aac; *.flac; *.mp2\0")
					   _T("QT媒体(QuickTime)(*.mp4; *.3gp; *.mov; *.qt; *.3gpp)\0*.mp4; *.3gp; *.mov; *.qt; *.3gpp\0")
					   _T("Windows媒体(*.avi; *.asf; *.wmv)\0*.avi; *.asf; *.wmv\0")
					   _T("Real媒体(*.rm; *.rmvb; *.ram)\0*.rm; *.rmvb; *.ram\0")
					   _T("Flash媒体(*.flv; *.swf; *.f4v; *.flc; *.fli)\0*.flv; *.f4v; *.flc; *.fli\0")
					   _T("Matroska媒体(*.mkv)\0*.mkv\0")
					   _T("Mpeg媒体(*.mpg; *.ts; *.m2t; *.mpeg; *.m2v; *.m2p; *.m4v;)\0(*.mpg; *.ts; *.m2t; *.mpeg; *.m2v; *.m2p; *.m4v;\0")
					   _T("DVD和蓝光媒体(*.vob;*.m2ts)\0*.vob;*.m2ts\0")
					   _T("音频媒体(*.mp3; *.ogg; *.wma; *.amr; *.wav; *.aac; *.flac; *.mp2)\0*.mp3; *.ogg; *.wma; *.amr; *.wav; *.aac; *.flac; *.mp2\0")
					   _T("所有文件(*.*)\0*.*\0\0"),GetHWND());
				if(dlg.DoModal() == IDOK)
				{
					OnOpenFileClick(dlg.m_szFileName,true);
				}
			}
		}
		if(msg.pSender->GetName() == UI_NAME_BUTTON_WELCOMECLOSE)
		{
			OnCloseClick();
		}
		else if(msg.pSender->GetName() == UI_NAME_BUTTON_WELCOMEMIN)
		{
			OnMinClick();
		}
		else if(msg.pSender->GetName() == UI_NAME_BUTTON_WELCOMEMAX)
		{
			OnMaxClick();
		}
		else if(msg.pSender->GetName() == UI_NAME_BUTTON_WELCOMERESTORE)
		{
			OnRestoreClick();
		}
	}
    __super::Notify(msg);
}

DuiLib::CDuiString CJPMainWindow::GetSkinFile()
{
    return _T("UIMainWindow.xml");
}

DuiLib::CDuiString CJPMainWindow::GetSkinFolder()
{
    return _T("skins");
}

bool CJPMainWindow::InitPlayer(const wchar_t* argv[] /*= NULL*/, int argc /*= 0*/)
{
    //初始化vlc
    const char* vlcArgs[] =
    {
        "-I",
        "dummy",
        "--verbose=2",
    };
    char** userVlcArgs = NULL;
    if (argv != NULL && argc > 0)
    {
        userVlcArgs = new char*[argc];
        for (int i = 0; i < argc; i++)
        {
            userVlcArgs[i] = new char[MAX_PATH];
            memset(userVlcArgs[i], 0, MAX_PATH);
        }
        for (int j = 0; j < argc; j++)
        {
            strcpy_s(userVlcArgs[j], MAX_PATH, W2Utf8(argv[j]).c_str());
        }
    }

    int nVlcArgs = 3;
    if (argv == NULL || argc <= 0)
    {
        if (!m_vlc_instance)
        {
            m_vlc_instance = libvlc_new(nVlcArgs, vlcArgs);
        }
    }
    else
    {
        if (!m_vlc_instance)
        {
            m_vlc_instance = libvlc_new(argc, userVlcArgs);
        }
    }
    if (m_vlc_instance == NULL)
    {
        return false;
    }
    m_vlc_log = libvlc_log_open(m_vlc_instance);
    libvlc_log_close(m_vlc_log);
	
	m_vlc_player = libvlc_media_player_new(m_vlc_instance);
	if (m_vlc_player == NULL)
	{
		return false;
	}
	
	//注册播放底层回调
	m_vlc_event_manager = libvlc_media_player_event_manager(m_vlc_player);
	libvlc_event_attach(m_vlc_event_manager,libvlc_MediaPlayerPlaying,&OnPlayingStart,this);
	libvlc_event_attach(m_vlc_event_manager,libvlc_MediaPlayerStopped,&OnPlayingStop,this);
	libvlc_event_attach(m_vlc_event_manager,libvlc_MediaPlayerEndReached,&OnPlayingEnd,this);
	libvlc_event_attach(m_vlc_event_manager,libvlc_MediaPlayerEncounteredError,&OnPlayingError,this);

    //创建播放窗口
    Create(NULL, _T("JustPlayIt"), UI_WNDSTYLE_FRAME & (~WS_VISIBLE), WS_EX_WINDOWEDGE);
    

   
    //创建bottombar
    if (m_bottomBar == NULL)
    {
        m_bottomBar = new CJPPlayerBottomBar();
        m_bottomBar->Create(NULL, JUSTPLAYIT_BOTTOMBAR_NAME, UI_WNDSTYLE_DIALOG & (~WS_VISIBLE), WS_EX_LAYERED | WS_EX_TOOLWINDOW & ~WS_EX_APPWINDOW );
        m_bottomBar->ShowWindow(false,false);
		SetLayeredWindowAttributes(m_bottomBar->GetHWND(),NULL,ALPHA_PLAYER_BAR,LWA_ALPHA);
		m_bottomBar->Init(m_vlc_player,GetHWND());
    }
	//创建topbar
	if(m_topBar == NULL)
	{
		m_topBar = new CJPPlayerTopBar();
		m_topBar->Create(NULL,JUSTPLAYIT_TOPBAR_NAME, UI_WNDSTYLE_DIALOG& (~WS_VISIBLE), WS_EX_LAYERED | WS_EX_TOOLWINDOW & ~WS_EX_APPWINDOW);
		m_topBar->ShowWindow(false,false);
		SetLayeredWindowAttributes(m_topBar->GetHWND(),NULL,ALPHA_PLAYER_BAR,LWA_ALPHA);
		m_topBar->Init(m_vlc_player,GetHWND());
	}
    SetupVlcHook();
    return true;
}

bool CJPMainWindow::UnInitPlayer()
{
	m_bottomBar->Close();
	m_bottomBar = NULL;
	m_topBar->Close();
	m_topBar = NULL;
    UnHookVlc();

	//解注册播放器底层回调
	libvlc_event_detach(m_vlc_event_manager,libvlc_MediaPlayerPlaying,&OnPlayingStart,this);
	libvlc_event_detach(m_vlc_event_manager,libvlc_MediaPlayerStopped,&OnPlayingStop,this);
	libvlc_event_detach(m_vlc_event_manager,libvlc_MediaPlayerEndReached,&OnPlayingEnd,this);

    if (m_vlc_player != NULL)
    {
        libvlc_media_player_stop(m_vlc_player);
        libvlc_media_player_release(m_vlc_player);
        m_vlc_player = NULL;
    }
    if (m_vlc_instance != NULL)
    {
        libvlc_release(m_vlc_instance);
        m_vlc_instance = NULL;
    }
    if (m_bottomBar != NULL)
    {
        delete m_bottomBar;
        m_bottomBar = NULL;
        return true;
    }
    return false;
}

void CJPMainWindow::Play(const wchar_t* uri, EMediaType mediaType)
{
	//清空错误提示
	if(m_labelPlayerError)
	{
		m_labelPlayerError->SetText(L"");
	}

	m_uri = uri;
	m_sourceType = mediaType;
	int status = -1;
	switch (mediaType)
    {
        case EMediaType_HttpStream:
            status = PlayNetVideo(uri);
            break;
        case EMediaType_Local:
            status = PlayLocalVideo(uri);
            break;
		case EMediaType_HLS:
			status = PlayHLSStream(uri);
			break;
        default:
            //未知的类型
			status = PlayNetVideo(uri);
            break;
    }
    ShowBottombar(false);
	ShowTopBar(false);
	if(status < 0)
	{
		OnPlayFailed();
	}
}

int CJPMainWindow::PlayNetVideo(const wchar_t* uri)
{
    if (m_vlc_instance)
    {
        m_vlc_media = libvlc_media_new_location(m_vlc_instance, W2Utf8(uri).c_str());
    }
    else
    {
        return -1;
    }
    if (m_vlc_media != NULL)
    {
        libvlc_media_parse(m_vlc_media);
        libvlc_media_player_set_media(m_vlc_player, m_vlc_media);
        return libvlc_media_player_play(m_vlc_player);
    }
	return -1;
}

int CJPMainWindow::PlayLocalVideo(const wchar_t* uri)
{
    if (uri != NULL && _waccess(uri, 0) == 0)
    {
        if (m_vlc_instance)
        {
            m_vlc_media = libvlc_media_new_path(m_vlc_instance, W2Utf8(uri).c_str());
            libvlc_media_parse(m_vlc_media);
            libvlc_media_player_set_media(m_vlc_player, m_vlc_media);
            return libvlc_media_player_play(m_vlc_player);
        }
    }
	return -1;
}

LRESULT CJPMainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_VIDEO_DBCLICK)
    {
        SetFullScreen(!m_bFullScreen);
    }
	if(uMsg == WM_WINDOWPOSCHANGED)
	{
		POINT pt = {0};
		::GetCursorPos(&pt);
		if (!PtInRect(&GetBottomBarRect(), pt) && !PtInRect(&GetBottomBarRect(), pt))
		{
			//移动或改变大小时先隐藏playerbar
			ShowBottombar(false);
			ShowTopBar(false);
		}
	}
	/*if(uMsg == WM_CMD_PLAYEND)
	{
		OnPlayEnd();
	}*/
    return __super::HandleMessage(uMsg, wParam, lParam);
}

LRESULT CJPMainWindow::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
    if (uMsg == WM_KEYDOWN)
    {
        if (wParam == VK_ESCAPE)
        {
           if(m_bFullScreen)
		   {
			   SetFullScreen(false);
		   }
		   return 1L;
        }
		else if(wParam == VK_RETURN)
		{
			if(m_editUri && !m_editUri->GetText().IsEmpty())
			{
				OnOpenFileClick(LPCTSTR(m_editUri->GetText()));
			}
		}
    }
    return __super::MessageHandler(uMsg, wParam, lParam, bHandled);
}

bool CJPMainWindow::SetupVlcHook()
{
    ::SetTimer(GetHWND(), ID_TIMER_HOOK_VLC, DURATION_TIMER_HOOK_VLC, NULL);
    return true;
}

bool CJPMainWindow::ShowBottombar(bool bShow)
{
    HWND hWnd = GetHWND();
    if (hWnd && IsWindow(hWnd) && IsWindowVisible(hWnd) && IsWindowEnabled(hWnd))
    {
        if (m_bottomBar != NULL)
        {
            if (m_bottomBar->GetHWND() && IsWindow(m_bottomBar->GetHWND()))
            {
				m_bottomBar->SetMouseTransparent(false);
                DuiLib::CDuiRect bottomBarRect = GetBottomBarRect();
				m_bottomBar->ShowWindow(bShow);
				if (bShow)
                {
					::SetWindowPos(m_bottomBar->GetHWND(), NULL, bottomBarRect.left, bottomBarRect.top, bottomBarRect.GetWidth(), bottomBarRect.GetHeight(), SWP_NOZORDER|SWP_NOREDRAW);
					::SetTimer(GetHWND(), ID_TIMER_HIDE_BAR, DURATION_TIMER_HIDE_BAR, NULL);
                }
                return true;
            }
        }
    }
    return false;
}

bool CJPMainWindow::IsDoubleClick()
{
    bool bDBClick = (m_nDoubleClick >= 2);
    return bDBClick;
}

bool CJPMainWindow::SetDoubleClick()
{
    m_nDoubleClick++;
    return true;
}

bool CJPMainWindow::UnHookVlc()
{
    if (g_Hook)
    {
        return UnhookWindowsHookEx(g_Hook);
    }
    return false;
}

bool CJPMainWindow::SetFullScreen(bool bFullScreen)
{
    m_bFullScreen = bFullScreen;
    if (bFullScreen)
    {
        //全屏隐藏bottombar,topbar
        ShowBottombar(false);
		ShowTopBar(false);
        //双击鼠标操作全屏
        RECT desktopRect = GetDesktopRect(true);
        desktopRect.bottom += 40;
		m_oldStyle = SetWindowLongPtr(GetHWND(), GWL_STYLE, WS_SYSMENU | WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE);
		MoveWindow(GetHWND(), 0, 0, desktopRect.right, desktopRect.bottom, TRUE);
	}
    else
    {
		SetWindowLongPtr(GetHWND(), GWL_STYLE, m_oldStyle);
		MoveWindow(GetHWND(), m_lastRect.left, m_lastRect.top, m_lastRect.right-m_lastRect.left, m_lastRect.bottom-m_lastRect.top, TRUE);
    }
    return m_bFullScreen;
}

LRESULT CJPMainWindow::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    //窗口大小改变时改变caption区域，保持整个视频窗口都可以拖拽移动
    if (!m_bFullScreen)
    {
        RECT wndRect = {0};
        ::GetWindowRect(GetHWND(), &wndRect);
        m_lastRect = wndRect;
		//移动或改变大小时先隐藏playerbar
		ShowBottombar(false);
		ShowTopBar(false);
    }
    return __super::OnSize(uMsg, wParam, lParam, bHandled);
}

RECT CJPMainWindow::GetDesktopRect(bool bWorkArea)
{
    HMONITOR hMonitor = MonitorFromWindow(GetHWND(), MONITOR_DEFAULTTONEAREST);
    if (NULL == hMonitor)
    {
        return m_PaintManager.GetCaptionRect();
    }
    MONITORINFO monitorInfo = {sizeof(MONITORINFO)};
    GetMonitorInfo(hMonitor, &monitorInfo);
    return bWorkArea ? monitorInfo.rcWork : monitorInfo.rcMonitor;
}

bool CJPMainWindow::IsBottomBarVisible()
{
    if (m_bottomBar)
    {
        if (m_bottomBar->GetHWND() && IsWindowVisible(m_bottomBar->GetHWND()))
        {
            return true;
        }
    }
    return false;
}

HWND CJPMainWindow::GetBottomBar()
{
    if (m_bottomBar)
    {
        return m_bottomBar->GetHWND();
    }
    return NULL;
}

LRESULT CJPMainWindow::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    return __super::OnClose(uMsg, wParam, lParam, bHandled);
}

RECT CJPMainWindow::GetBottomBarRect()
{
    RECT rectPlayer = {0};
    GetWindowRect(GetHWND(), &rectPlayer);
    RECT bottomBarRect = {0};
    bottomBarRect.bottom = rectPlayer.bottom;
    bottomBarRect.left = rectPlayer.left;
    bottomBarRect.right = rectPlayer.right;
	DuiLib::CDuiRect bottomWinRect;
	if(m_bottomBar)
	{
		GetClientRect(m_bottomBar->GetHWND(),&bottomWinRect);
	}
	bottomBarRect.top = bottomBarRect.bottom - bottomWinRect.GetHeight();
    return bottomBarRect;
}

LRESULT CJPMainWindow::OnNcHitTest( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	if(m_playerActive)
	{
		if(!IsDoubleClick() && !m_bFullScreen)
		{
			POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
			::ScreenToClient(*this, &pt);

			RECT rcClient;
			::GetClientRect(*this, &rcClient);

			RECT rcSizeBox = m_PaintManager.GetSizeBox();
			if( pt.x >= rcClient.left + rcSizeBox.left && pt.x < rcClient.right - rcSizeBox.right \
				&& pt.y >= rcClient.top + rcSizeBox.top && pt.y < rcClient.bottom - rcSizeBox.bottom ) {
					return HTCAPTION;
			}
		}
		else
		{
			return HTCLIENT;
		}
	}
	return __super::OnNcHitTest(uMsg,wParam,lParam,bHandled);
}

bool CJPMainWindow::ShowTopBar( bool bShow )
{
	HWND hWnd = GetHWND();
	if (hWnd && IsWindow(hWnd) && IsWindowVisible(hWnd) && IsWindowEnabled(hWnd))
	{
		if (m_topBar != NULL)
		{
			if (m_topBar->GetHWND() && IsWindow(m_topBar->GetHWND()))
			{
				DuiLib::CDuiRect topBarRect = GetTopBarRect();
				m_topBar->ShowWindow(bShow);
				if (bShow)
				{
					::SetWindowPos(m_topBar->GetHWND(), NULL, topBarRect.left, topBarRect.top, topBarRect.GetWidth(), topBarRect.GetHeight(), SWP_NOZORDER|SWP_NOREDRAW);
					::SetTimer(GetHWND(), ID_TIMER_HIDE_BAR, DURATION_TIMER_HIDE_BAR, NULL);
				}
				return true;
			}
		}
	}
	return false;
}

RECT CJPMainWindow::GetTopBarRect()
{
	RECT rectPlayer = {0};
	GetWindowRect(GetHWND(), &rectPlayer);
	RECT topBarRect = {0};
	topBarRect.left = rectPlayer.left;
	topBarRect.right = rectPlayer.right;
	topBarRect.top = rectPlayer.top;
	DuiLib::CDuiRect topWinRect;
	if(m_topBar)
	{
		GetClientRect(m_topBar->GetHWND(),&topWinRect);
	}
	topBarRect.bottom = topBarRect.top + topWinRect.GetHeight();
	return topBarRect;
}

void CJPMainWindow::QuitApplication()
{
	UnInitPlayer();
	ShowWindow(false);
	Close();
	PostQuitMessage(0);
}

int CJPMainWindow::PlayHLSStream( const wchar_t* uri )
{
	if (m_vlc_instance)
	{
		m_vlc_media = libvlc_media_new_location(m_vlc_instance, W2Utf8(uri).c_str());
	}
	else
	{
		return -1;
	}
	if (m_vlc_media != NULL)
	{
		libvlc_media_parse(m_vlc_media);
		libvlc_media_player_set_media(m_vlc_player, m_vlc_media);
		return libvlc_media_player_play(m_vlc_player);
	}
	return -1;
}

void CJPMainWindow::OnPlayBegin()
{
	if(m_vlc_player)
	{
		libvlc_media_player_set_hwnd(m_vlc_player, GetHWND());
	}
	//显示标题
	if(m_topBar)
	{
		m_topBar->SetTitle(libvlc_media_get_meta(m_vlc_media,libvlc_meta_Title));
	}
	//解析m3u8时长信息
	if(m_sourceType == EMediaType_HLS)
	{
		m_hlsMetaData = CHLSMetaParser::Instance()->parseHLSStream(m_uri.c_str());
		m_bottomBar->SetDuration(m_hlsMetaData.duration);
	}
	//自适应宽度
	int height = libvlc_video_get_height(m_vlc_player);
	int width = libvlc_video_get_width(m_vlc_player);
	if(height != 0 && width != 0)
	{
		//目前只有本地播放能够获取到正常的宽高信息
		::SetWindowPos(GetHWND(),NULL,0,0,width,height,SWP_NOZORDER|SWP_NOMOVE);
		CenterWindow();
	}
	//AdjustRatio();
	m_playerActive = true;
	m_bAutoShowBar = true;
}

void CJPMainWindow::AdjustRatio()
{
	CStringA strRatio;
	DuiLib::CDuiRect rect;
	GetClientRect(GetHWND(),&rect);
	strRatio.Format("%d:%d",rect.GetWidth(),rect.GetHeight());
	libvlc_video_set_aspect_ratio(m_vlc_player,LPCSTR(strRatio));
}

bool CJPMainWindow::SetTopMost( bool bTopMost )
{
	if(GetHWND() && IsWindow(GetHWND()))
	{
		if(bTopMost)
		{
			::SetWindowPos(GetHWND(),HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOREPOSITION|SWP_NOREDRAW|SWP_NOMOVE);
			::SetWindowPos(m_bottomBar->GetHWND(),GetHWND(),0,0,0,0,SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOREPOSITION|SWP_NOMOVE);
			::SetWindowPos(m_topBar->GetHWND(),GetHWND(),0,0,0,0,SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOREPOSITION|SWP_NOMOVE);
		}
		else
		{
			::SetWindowPos(GetHWND(),HWND_NOTOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOREPOSITION|SWP_NOMOVE);
			::SetWindowPos(m_bottomBar->GetHWND(),HWND_NOTOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOREPOSITION|SWP_NOREDRAW|SWP_NOMOVE);
			::SetWindowPos(m_topBar->GetHWND(),HWND_NOTOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOREPOSITION|SWP_NOREDRAW|SWP_NOMOVE);
		}
	}
	return bTopMost;
}

LRESULT CJPMainWindow::OnTimer( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	if (wParam == ID_TIMER_HOOK_VLC)
	{
		if (IsWindow(GetHWND()))
		{
			EnumChildWindows(GetHWND(), EnumVlcWndProc, NULL);
		}
	}
	else if (wParam == ID_TIMER_VIDEO_DBCLICK)
	{
		m_nDoubleClick = 0;
		KillTimer(GetHWND(), ID_TIMER_VIDEO_DBCLICK);
	}
	else if (wParam == ID_TIMER_HIDE_BAR)
	{
		POINT curPt = {0};
		::GetCursorPos(&curPt);
		if(!PtInRect(&GetBottomBarRect(),curPt) && !PtInRect(&GetTopBarRect(),curPt))
		{
			ShowBottombar(false);
			ShowTopBar(false);
			KillTimer(GetHWND(), ID_TIMER_HIDE_BAR);
		}
	}
	else if(wParam == ID_TIMER_SHOW_BAR)
	{
		ShowBottombar(true);
		ShowTopBar(true);
		KillTimer(GetHWND(), ID_TIMER_SHOW_BAR);
	}
	return __super::OnTimer(uMsg,wParam,lParam,bHandled);
}

LRESULT CJPMainWindow::OnCreate( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	LRESULT result = __super::OnCreate(uMsg,wParam,lParam,bHandled);
	if(!m_editUri)
	{
		m_editUri = static_cast<DuiLib::CRichEditUI*>(m_PaintManager.FindControl(UI_NAME_EDIT_URI));
	}
	if(!m_btnOpenFile)
	{
		m_btnOpenFile = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl(UI_NAME_BUTTON_OPENFILE));
	}
	if(!m_btnWelcomeClose)
	{
		m_btnWelcomeClose = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl(UI_NAME_BUTTON_WELCOMECLOSE));
	}
	if(!m_btnWelcomeMax)
	{
		m_btnWelcomeMax = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl(UI_NAME_BUTTON_WELCOMEMAX));
	}
	if(!m_btnWelcomeMin)
	{
		m_btnWelcomeMin = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl(UI_NAME_BUTTON_WELCOMEMIN));
	}
	if(!m_btnWelcomeRestore)
	{
		m_btnWelcomeRestore = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl(UI_NAME_BUTTON_WELCOMERESTORE));
	}
	if(!m_labelPlayerError)
	{
		m_labelPlayerError = static_cast<DuiLib::CLabelUI*>(m_PaintManager.FindControl(UI_NAME_LABEL_PLAYERROR));
	}
	CenterWindow();
	ShowWindow(true,true);
	return result;
}

void CJPMainWindow::OnPlayEnd(bool bStop)
{
	if(m_playerActive)
	{
		//如果是自然停止需要调用playerbar的停止来执行清理操作
		if(!bStop && m_bottomBar)
		{
			m_bottomBar->Stop();
		}
		if(m_vlc_player)
		{
			libvlc_media_player_set_hwnd(m_vlc_player, NULL);
		}
		if(m_topBar)
		{
			m_topBar->SetTitle("");
		}
		KillTimer(GetHWND(), ID_TIMER_HIDE_BAR);
		ShowBottombar(false);
		ShowTopBar(false);
		m_playerActive = false;
		m_bAutoShowBar = false;
	}
}

void CJPMainWindow::OnOpenFileClick(const wchar_t* uri,bool bLocal)
{
	DuiLib::CDuiString strUri = uri;
	if(!bLocal)
	{
		std::wstring netUri = LPCTSTR(strUri);
		if(strUri.Find(L"m3u8") >= 0)
		{
			if(strUri.Find(L"http://") < 0)
			{
				netUri = L"http://";
				netUri += LPCTSTR(strUri);
			}
			Play(netUri.c_str(),EMediaType_HLS);
		}
		else if(strUri.Find(L"http://") >= 0)
		{
			Play(LPCTSTR(strUri),EMediaType_HttpStream);
		}
		else
		{
			if(strUri.Find(L":\\") >= 0)
			{
				Play(LPCTSTR(strUri),EMediaType_Local);
			}
			else
			{
				netUri = L"http://";
				netUri += LPCTSTR(strUri);
				Play(LPCTSTR(strUri),EMediaType_HttpStream);
			}
		}
	}
	else
	{
		Play(LPCTSTR(strUri),EMediaType_Local);
	}
}

bool CJPMainWindow::IsAutoShowBar()
{
	return m_bAutoShowBar;
}

void CJPMainWindow::OnCloseClick()
{
	QuitApplication();
}

void CJPMainWindow::OnMinClick()
{
	::ShowWindow(GetHWND(),SW_MINIMIZE);
}

void CJPMainWindow::OnMaxClick()
{
	::ShowWindow(GetHWND(),SW_MAXIMIZE);
	if(m_btnWelcomeRestore)
	{
		m_btnWelcomeRestore->SetVisible(true);
	}
	if(m_btnWelcomeMax)
	{
		m_btnWelcomeMax->SetVisible(false);
	}
}

void CJPMainWindow::OnRestoreClick()
{
	::ShowWindow(GetHWND(),SW_RESTORE);
	if(m_btnWelcomeRestore)
	{
		m_btnWelcomeRestore->SetVisible(false);
	}
	if(m_btnWelcomeMax)
	{
		m_btnWelcomeMax->SetVisible(true);
	}
}

void CJPMainWindow::OnPlayFailed()
{
	if(m_labelPlayerError)
	{
		m_labelPlayerError->SetText(L"播放媒体失败");
	}
}
