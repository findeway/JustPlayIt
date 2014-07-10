#include "StdAfx.h"
#include "JPMainWindow.h"
#include "Util.h"
#include <boost/bind.hpp>

#pragma comment(lib,"libvlc.lib")
#pragma comment(lib,"libvlccore.lib")

#define WM_VIDEO_DBCLICK			(WM_USER+500)
#define ID_TIMER_HOOK_VLC			(5002)
#define ID_TIMER_VIDEO_DBCLICK		(5003)
#define ID_TIMER_HIDE_BOTTOM_BAR	(5004)
#define DURATION_TIMER_HOOK_VLC	2000

HHOOK g_Hook = NULL;

RECT rcDefault = {0, 0, 1000, 600};

LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    MSLLHOOKSTRUCT* pHookStruct = (MSLLHOOKSTRUCT*)lParam;
    if (!pHookStruct)
    {
        CallNextHookEx(g_Hook, nCode, wParam, lParam);
    }
    POINT pt = pHookStruct->pt;

    if (wParam == WM_LBUTTONDOWN)
    {
        HWND hWnd = CJPMainWindow::Instance().GetHWND();
        if (IsWindow(hWnd) && IsWindowVisible(hWnd) && IsWindowEnabled(hWnd))
        {
            RECT mainRect = {0};
            GetWindowRect(hWnd, &mainRect);
            if (PtInRect(&mainRect, pt))
            {
                CJPMainWindow::Instance().SetDoubleClick();
                SetTimer(hWnd, ID_TIMER_VIDEO_DBCLICK, 200, NULL);
                if (CJPMainWindow::Instance().IsDoubleClick())
                {
                    ::PostMessage(hWnd, WM_VIDEO_DBCLICK, 0, MAKELPARAM(pt.x, pt.y));
                }
            }
        }
    }
    else if (wParam == WM_MOUSEMOVE)
    {
        HWND hWnd = CJPMainWindow::Instance().GetHWND();
        if (IsWindow(hWnd) && IsWindowVisible(hWnd) && IsWindowEnabled(hWnd))
        {
            RECT mainRect = {0};
            GetWindowRect(hWnd, &mainRect);
            if (PtInRect(&mainRect, pt))
            {
                CJPMainWindow::Instance().ShowBottombar(true);
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
        HWND hParent = GetParent(hWnd);
        if (IsWindow(hParent))
        {
            ::KillTimer(hParent, ID_TIMER_HOOK_VLC);
        }
        GetWindowThreadProcessId(hWnd, &dwVlcThreadId);
        if (dwVlcThreadId != 0)
        {
            //使用WH_MOUSE失败，改用全局HOOK WH_MOUSE_LL
            g_Hook = SetWindowsHookEx(WH_MOUSE_LL, HookProc, GetModuleHandle(NULL), 0);
        }
    }
    return TRUE;
}

CJPMainWindow::CJPMainWindow(void)
{
    m_vlc_instance = NULL;
    m_vlc_player = NULL;
    m_vlc_media = NULL;
    m_vlc_log = NULL;
    m_bottomBar = NULL;
    m_nDoubleClick = 0;
    m_bFullScreen = false;
    m_lastRect = rcDefault;
	m_oldStyle = 0;
	InitPlayer();
}

CJPMainWindow::~CJPMainWindow(void)
{
    UnInitPlayer();
}

void CJPMainWindow::Notify(DuiLib::TNotifyUI& msg)
{
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

    //创建播放窗口
    Create(NULL, _T("JustPlayIt"), UI_WNDSTYLE_FRAME & (~WS_VISIBLE), WS_EX_WINDOWEDGE);
    CenterWindow();
    ShowWindow(true, true);
    SetShadowVisible(false);

    m_vlc_player = libvlc_media_player_new(m_vlc_instance);
    if (m_vlc_player == NULL)
    {
        return false;
    }
    libvlc_media_player_set_hwnd(m_vlc_player, GetHWND());

    //创建bottombar
    if (m_bottomBar == NULL)
    {
        m_bottomBar = new CJPPlayerBottomBar();
		m_bottomBar->Init(m_vlc_player,GetHWND());
        m_bottomBar->Create(GetHWND(), _T("JPPlayerBottomBar"), UI_WNDSTYLE_FRAME & (~WS_VISIBLE), WS_EX_WINDOWEDGE);
        m_bottomBar->ShowWindow(false);
    }
    SetupVlcHook();
    return true;
}

bool CJPMainWindow::UnInitPlayer()
{
    UnHookVlc();
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
    switch (mediaType)
    {
        case EMediaType_Url:
            PlayOnLineVideo(uri);
            break;
        case EMediaType_Local:
            PlayLocalVideo(uri);
            break;
        default:
            PlayLocalVideo(uri);
            break;
    }
    ShowBottombar(false);
}

void CJPMainWindow::PlayOnLineVideo(const wchar_t* uri)
{
    if (m_vlc_instance)
    {
        m_vlc_media = libvlc_media_new_location(m_vlc_instance, W2Utf8(uri).c_str());
    }
    else
    {
        return;
    }
    if (m_vlc_media != NULL)
    {
        libvlc_media_parse(m_vlc_media);
        libvlc_time_t duration = libvlc_media_get_duration(m_vlc_media);
        libvlc_media_track_info_t* media_tracks = NULL;
        int trackCount = libvlc_media_get_tracks_info(m_vlc_media, &media_tracks);
        libvlc_media_player_set_media(m_vlc_player, m_vlc_media);
        libvlc_media_player_play(m_vlc_player);
    }
}

void CJPMainWindow::PlayLocalVideo(const wchar_t* uri)
{
    if (uri != NULL && _waccess(uri, 0) == 0)
    {
        if (m_vlc_instance)
        {
            m_vlc_media = libvlc_media_new_path(m_vlc_instance, W2Utf8(uri).c_str());
            libvlc_media_parse(m_vlc_media);
            libvlc_time_t duration = libvlc_media_get_duration(m_vlc_media);
            libvlc_media_track_info_t* media_tracks = NULL;
            int trackCount = libvlc_media_get_tracks_info(m_vlc_media, &media_tracks);
            libvlc_media_player_set_media(m_vlc_player, m_vlc_media);
            libvlc_media_player_play(m_vlc_player);
        }
    }
}

LRESULT CJPMainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_TIMER)
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
        else if (wParam == ID_TIMER_HIDE_BOTTOM_BAR)
        {
			POINT curPt = {0};
			::GetCursorPos(&curPt);
			if(!PtInRect(&GetBottomBarRect(),curPt))
			{
				ShowBottombar(false);
				KillTimer(GetHWND(), ID_TIMER_HIDE_BOTTOM_BAR);
			}
        }
    }
    if (uMsg == WM_VIDEO_DBCLICK)
    {
        SetFullScreen(!m_bFullScreen);
    }
    return __super::HandleMessage(uMsg, wParam, lParam);
}

LRESULT CJPMainWindow::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
    if (uMsg == WM_KEYDOWN)
    {
        if (wParam == VK_ESCAPE)
        {
            Close();
            PostQuitMessage(0);
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
                DuiLib::CDuiRect bottomBarRect = GetBottomBarRect();
                MoveWindow(m_bottomBar->GetHWND(), bottomBarRect.left, bottomBarRect.top, bottomBarRect.GetWidth(), bottomBarRect.GetHeight(), FALSE);
                m_bottomBar->ShowWindow(bShow);
                if (bShow)
                {
                    SetTimer(GetHWND(), ID_TIMER_HIDE_BOTTOM_BAR, 1000, NULL);
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
    if (bDBClick)
    {
//        m_nDoubleClick = 0;
    }
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
        //全屏隐藏bottombar
        ShowBottombar(false);
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
    UnInitPlayer();
    return __super::OnClose(uMsg, wParam, lParam, bHandled);
}

RECT CJPMainWindow::GetBottomBarRect()
{
    RECT rectPlayer = {0};
    GetWindowRect(GetHWND(), &rectPlayer);
    RECT bottomBarRect = {0};
    bottomBarRect.bottom = rectPlayer.bottom;
    bottomBarRect.left = rectPlayer.left + 3;
    bottomBarRect.right = rectPlayer.right - 3;
	bottomBarRect.top = bottomBarRect.bottom - 70;
    return bottomBarRect;
}

LRESULT CJPMainWindow::OnNcHitTest( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
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
	return __super::OnNcHitTest(uMsg,wParam,lParam,bHandled);
}
