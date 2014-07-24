#include "StdAfx.h"
#include "JPPlayerBottomBar.h"
#include "JPMainWindow.h"

using namespace DuiLib;

#define ID_TIMER_UPDATE_PROGRESS		(5005)
#define DURATION_TIMER_UPDATE_PROGRESS  (1000.0/60.0)

CJPPlayerBottomBar::CJPPlayerBottomBar(void)
{
	m_btnPlay = NULL;
	m_btnPause = NULL;
	m_btnStop = NULL;
	m_btnForward = NULL;
	m_btnBackward = NULL;
	m_btnVolume = NULL;
	m_btnVolumeZero = NULL;
	m_slideVolumn = NULL;
	m_slideProgress = NULL;
	m_vlcplayer = NULL;
	m_btnTopMost = NULL;
	m_btnNoTopMost = NULL;
	m_labelTotalTime = NULL;
	m_labelCurTime = NULL;
	m_nCurPos = 0;
	m_nVolume = 50;
	m_duration = 0;
}

CJPPlayerBottomBar::~CJPPlayerBottomBar(void)
{
	SetUpdateProgress(false);
}

bool CJPPlayerBottomBar::Resume()
{
	SetUpdateProgress(true);
	if(m_vlcplayer)
	{
		libvlc_media_player_set_pause(m_vlcplayer,0);
		if(m_btnPlay)
		{
			m_btnPlay->SetVisible(false);
		}
		if(m_btnPause)
		{
			m_btnPause->SetVisible(true);
		}
		return true;
	}
	return false;
}

bool CJPPlayerBottomBar::Stop()
{
	Seek(0.0);
	SetDuration(0);
	SetUpdateProgress(false);
	if(m_vlcplayer)
	{
		libvlc_media_player_stop(m_vlcplayer);
		if(m_btnPlay)
		{
			m_btnPlay->SetVisible(true);
		}
		if(m_btnPause)
		{
			m_btnPause->SetVisible(false);
		}
	}
	if(m_hParent && IsWindow(m_hParent))
	{
		::PostMessage(m_hParent,WM_CMD_PLAYEND,0,0);
	}
	return false;
}

bool CJPPlayerBottomBar::Pause()
{
	SetUpdateProgress(false);
	if(m_vlcplayer)
	{
		libvlc_media_player_set_pause(m_vlcplayer,1);
		if(m_btnPlay)
		{
			m_btnPlay->SetVisible(true);
		}
		if(m_btnPause)
		{
			m_btnPause->SetVisible(false);
		}
		return true;
	}
	return false;
}

bool CJPPlayerBottomBar::Seek( double pos )
{
	if(pos < 0.0 || pos > 100.0)
	{
		return false;
	}
	if(m_vlcplayer)
	{
		double progress = pos/100.0;
		m_nCurPos = pos;
		libvlc_media_player_set_position(m_vlcplayer,progress);
		UpdateProgress(true);
		return true;
	}
	return false;
}

bool CJPPlayerBottomBar::SetVolumn( int num )
{
	if(num != 0)
	{
		m_nVolume = num;
	}
	if(m_vlcplayer)
	{
		libvlc_audio_set_volume(m_vlcplayer,num);
	}
	if(m_slideVolumn)
	{
		m_slideVolumn->SetValue(num);
	}
	return false;
}

void CJPPlayerBottomBar::FastForward()
{
	Seek(m_nCurPos + 1.0);
}

void CJPPlayerBottomBar::FastBackward()
{
	Seek(m_nCurPos - 1.0);
}

void CJPPlayerBottomBar::Notify( DuiLib::TNotifyUI& msg )
{
	if(msg.sType == DUI_MSGTYPE_CLICK)
	{
		if(msg.pSender->GetName() == UI_NAME_BUTTON_PAUSE)
		{
			Pause();
		}
		else if(msg.pSender->GetName() == UI_NAME_BUTTON_PLAY)
		{
			Resume();
		}
		else if(msg.pSender->GetName() == UI_NAME_BUTTON_FORWARD)
		{
			FastForward();
		}
		else if(msg.pSender->GetName() == UI_NAME_BUTTON_BACKWARD)
		{
			FastBackward();
		}
		else if(msg.pSender->GetName() == UI_NAME_BUTTON_STOP)
		{
			Stop();
		}
		else if(msg.pSender->GetName() == UI_NAME_BUTTON_MUTE)
		{
			SetMute(true);
		}
		else if(msg.pSender->GetName() == UI_NAME_BUTTON_MUTE_ZERO)
		{
			SetMute(false);
		}
		else if(msg.pSender->GetName() == UI_NAME_BUTTON_TOPMOST)
		{
			SetTopMost(true);
		}
		else if(msg.pSender->GetName() == UI_NAME_BUTTON_NOTOPMOST)
		{
			SetTopMost(false);
		}
	}
	else if(msg.sType == DUI_MSGTYPE_VALUECHANGED)
	{
		if(msg.pSender->GetName() == UI_NAME_SLIDER_PROGRESS)
		{
			if(m_slideProgress)
			{
				Seek(m_slideProgress->GetValue());
			}
		}
		else if(msg.pSender->GetName() == UI_NAME_SLIDER_VOL)
		{
			if(m_slideVolumn)
			{
				SetVolumn(m_slideVolumn->GetValue());
			}
		}
	}
	return __super::Notify(msg);
}

DuiLib::CDuiString CJPPlayerBottomBar::GetSkinFile()
{
	return _T("UIBottomBar.xml");
}

DuiLib::CDuiString CJPPlayerBottomBar::GetSkinFolder()
{
	return _T("skins");
}

LRESULT CJPPlayerBottomBar::HandleMessage( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if(uMsg == WM_MOUSEMOVE)
	{
		UpdateHoverTime();
	}
	return __super::HandleMessage(uMsg,wParam,lParam);
}

LRESULT CJPPlayerBottomBar::OnCreate( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	LRESULT result = __super::OnCreate(uMsg,wParam,lParam,bHandled);
	if(!m_btnVolume)
	{
		m_btnVolume = static_cast<CButtonUI*>(m_PaintManager.FindControl(UI_NAME_BUTTON_MUTE));
	}
	if(!m_btnVolumeZero)
	{
		m_btnVolumeZero = static_cast<CButtonUI*>(m_PaintManager.FindControl(UI_NAME_BUTTON_MUTE_ZERO));
	}
	if(!m_btnStop)
	{
		m_btnStop = static_cast<CButtonUI*>(m_PaintManager.FindControl(UI_NAME_BUTTON_STOP));
	}
	if(!m_btnBackward)
	{
		m_btnBackward = static_cast<CButtonUI*>(m_PaintManager.FindControl(UI_NAME_BUTTON_BACKWARD));
	}
	if(!m_btnPlay)
	{
		m_btnPlay = static_cast<CButtonUI*>(m_PaintManager.FindControl(UI_NAME_BUTTON_PLAY));
	}
	if(!m_btnPause)
	{
		m_btnPause = static_cast<CButtonUI*>(m_PaintManager.FindControl(UI_NAME_BUTTON_PAUSE));
	}
	if(!m_btnForward)
	{
		m_btnForward = static_cast<CButtonUI*>(m_PaintManager.FindControl(UI_NAME_BUTTON_FORWARD));
	}
	if(!m_slideVolumn)
	{
		m_slideVolumn = static_cast<CSliderUI*>(m_PaintManager.FindControl(UI_NAME_SLIDER_VOL));
		m_nVolume = m_slideVolumn->GetValue();
	}
	if(!m_slideProgress)
	{
		m_slideProgress = static_cast<CSliderUI*>(m_PaintManager.FindControl(UI_NAME_SLIDER_PROGRESS));
	}
	if(!m_btnTopMost)
	{
		m_btnTopMost = static_cast<CButtonUI*>(m_PaintManager.FindControl(UI_NAME_BUTTON_TOPMOST));
	}
	if(!m_btnNoTopMost)
	{
		m_btnNoTopMost = static_cast<CButtonUI*>(m_PaintManager.FindControl(UI_NAME_BUTTON_NOTOPMOST));
	}
	if(!m_labelCurTime)
	{
		m_labelCurTime = static_cast<CLabelUI*>(m_PaintManager.FindControl(UI_NAME_LABEL_CURTIME));
	}
	if(!m_labelTotalTime)
	{
		m_labelTotalTime = static_cast<CLabelUI*>(m_PaintManager.FindControl(UI_NAME_LABEL_TOTALTIME));
	}
	return result;
}

void CJPPlayerBottomBar::Init( libvlc_media_player_t* player, HWND hParent )
{
	m_vlcplayer = player;
	m_hParent = hParent;
	SetUpdateProgress(true);
	::RegisterHotKey(GetHWND(),WM_HOTKEY_FORWARD,MOD_CONTROL|MOD_ALT,VK_RIGHT);
	::RegisterHotKey(GetHWND(),WM_HOTKEY_BACKWARD,MOD_CONTROL|MOD_ALT,VK_LEFT);
	::RegisterHotKey(GetHWND(),WM_HOTKEY_VOLUP,MOD_ALT,VK_UP);
	::RegisterHotKey(GetHWND(),WM_HOTKEY_VOLDOWN,MOD_ALT,VK_DOWN);
}

bool CJPPlayerBottomBar::SetMute( bool bMute )
{
	if(bMute)
	{
		SetVolumn(0);
		if(m_btnVolume)
		{
			m_btnVolume->SetVisible(false);
		}
		if(m_btnVolumeZero)
		{
			m_btnVolumeZero->SetVisible(true);
		}
	}
	else
	{
		SetVolumn(m_nVolume);
		if(m_btnVolume)
		{
			m_btnVolume->SetVisible(true);
		}
		if(m_btnVolumeZero)
		{
			m_btnVolumeZero->SetVisible(false);
		}
	}
	return bMute;
}

bool CJPPlayerBottomBar::SetTopMost( bool bTopMost )
{
	if(bTopMost)
	{
		if(m_btnTopMost)
		{
			m_btnTopMost->SetVisible(false);
		}
		if(m_btnNoTopMost)
		{
			m_btnNoTopMost->SetVisible(true);
		}
	}
	else
	{
		if(m_btnTopMost)
		{
			m_btnTopMost->SetVisible(true);
		}
		if(m_btnNoTopMost)
		{
			m_btnNoTopMost->SetVisible(false);
		}
	}
	return CJPMainWindow::Instance()->SetTopMost(bTopMost);
}

bool CJPPlayerBottomBar::SetProgress( double num )
{
	if(m_vlcplayer)
	{
		m_nCurPos = num;
		double progress = num/100.0;
		libvlc_media_player_set_position(m_vlcplayer,progress);
		if(m_slideProgress)
		{
			m_slideProgress->SetValue(num);
		}
	}
	return true;
}

void CJPPlayerBottomBar::UpdateProgress( bool bUpdate )
{
	if(bUpdate)
	{
		m_nCurPos = libvlc_media_player_get_position(m_vlcplayer) * 100.0;
		if(m_slideProgress)
		{
			m_slideProgress->SetValue(m_nCurPos);
		}
		if(m_duration <= 0)
		{
			libvlc_time_t time = libvlc_media_player_get_length(m_vlcplayer);
			SetDuration(libvlc_media_player_get_length(m_vlcplayer) / 1000.0);
			m_duration = time/1000;
		}
		//调用vlc的libvlc_media_player_get_time针对hls会返回0
		libvlc_time_t time = libvlc_media_player_get_time(m_vlcplayer);
		if(time != 0)
		{
			SetCurTime(time/1000);
		}
		else
		{
			int time = m_duration*libvlc_media_player_get_position(m_vlcplayer);
			SetCurTime(time);
		}
	}
}

void CJPPlayerBottomBar::SetUpdateProgress( bool bUpdate )
{
	if(GetHWND() && IsWindow(GetHWND()))
	{
		if(bUpdate)
		{
			SetTimer(GetHWND(),ID_TIMER_UPDATE_PROGRESS,DURATION_TIMER_UPDATE_PROGRESS,NULL);
		}
		else
		{
			KillTimer(GetHWND(),ID_TIMER_UPDATE_PROGRESS);
		}
	}
}

bool CJPPlayerBottomBar::SetDuration( int duration )
{
	m_duration = duration;
	if(m_labelTotalTime)
	{
		m_labelTotalTime->SetText(FormatTime(duration).c_str());
		return true;
	}
	return false;
}

bool CJPPlayerBottomBar::SetCurTime( int time )
{
	if(m_labelCurTime)
	{
		m_labelCurTime->SetText(FormatTime(time).c_str());
		return true;
	}
	return false;
}

std::wstring CJPPlayerBottomBar::FormatTime( int time )
{
	//防止时间出现负数
	if(time < 0)
	{
		time = 0;
	}
	DuiLib::CDuiString strTime;
	int second = time % 60;
	int minute = (time / 60) % 60;
	int hour = (time / (60 * 60)) % 60;
	if(hour > 0)
	{
		strTime.Format(_T("%02d:%02d:%02d"),hour,minute,second);
	}
	else
	{
		strTime.Format(_T("%02d:%02d"),minute,second);
	}
	return LPCTSTR(strTime);
}

void CJPPlayerBottomBar::UpdateHoverTime()
{
	POINT pt = {0};
	::GetCursorPos(&pt);
	::ScreenToClient(GetHWND(),&pt);
	if(m_slideProgress)
	{
		RECT rect = m_slideProgress->GetPos();
		if(PtInRect(&rect,pt))
		{
			DuiLib::CDuiRect thumbRect = m_slideProgress->GetThumbRect();
			//保留两位小数
			float seekPercent = int(float(pt.x - rect.left - thumbRect.GetWidth() / 2)/(rect.right-rect.left - thumbRect.GetWidth()) * 100) / 100.0;
			int time = seekPercent * m_duration;
			m_slideProgress->SetToolTip(FormatTime(time).c_str());
		}
	}
}

LRESULT CJPPlayerBottomBar::OnTimer( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	if(wParam == ID_TIMER_UPDATE_PROGRESS)
	{
		if(m_vlcplayer)
		{
			libvlc_state_t playerState = libvlc_media_player_get_state(m_vlcplayer);
			if(playerState == libvlc_Playing)
			{
				UpdateProgress(true);
			}
		}
	}
	return __super::OnTimer(uMsg,wParam,lParam,bHandled);
}

LRESULT CJPPlayerBottomBar::OnNcHitTest( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	POINT pt;
	pt.x = GET_X_LPARAM(lParam);
	pt.y = GET_Y_LPARAM(lParam);
	::ScreenToClient(*this, &pt);

	RECT rcClient;
	::GetClientRect(*this, &rcClient);

	RECT rcSizeBox = m_PaintManager.GetSizeBox();
	if (pt.y > rcClient.bottom - rcSizeBox.bottom)
	{
		SetMouseTransparent(true);
		return __super::OnNcHitTest(uMsg,wParam,lParam,bHandled);
	}
	if (pt.x > rcClient.right - rcSizeBox.right)
	{
		SetMouseTransparent(true);
		return __super::OnNcHitTest(uMsg,wParam,lParam,bHandled);
	}
	return __super::OnNcHitTest(uMsg,wParam,lParam,bHandled);
}

void CJPPlayerBottomBar::SetMouseTransparent( bool bTrans )
{
	if(bTrans)
	{
		::SetWindowLong(GetHWND(),GWL_EXSTYLE,GetWindowLong(GetHWND(),GWL_EXSTYLE) | WS_EX_TRANSPARENT);
	}
	else
	{
		::SetWindowLong(GetHWND(),GWL_EXSTYLE,GetWindowLong(GetHWND(),GWL_EXSTYLE) & (~WS_EX_TRANSPARENT));
	}
}

LRESULT CJPPlayerBottomBar::MessageHandler( UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled )
{
	if (uMsg == WM_KEYDOWN)
	{
		if (wParam == VK_ESCAPE)
		{
			return 1L;
		}
	}
	if(uMsg == WM_HOTKEY)
	{
		if(wParam == WM_HOTKEY_BACKWARD)
		{
			FastBackward();
		}
		else if(wParam == WM_HOTKEY_FORWARD)
		{
			FastForward();
		}
		else if(wParam == WM_HOTKEY_VOLUP)
		{
			VolumnUp();
		}
		else if(wParam == WM_HOTKEY_VOLDOWN)
		{
			VolumnDown();
		}
	}
	return __super::MessageHandler(uMsg, wParam, lParam, bHandled);
}

void CJPPlayerBottomBar::VolumnUp()
{
	m_nVolume += 10;
	SetVolumn(m_nVolume);
}

void CJPPlayerBottomBar::VolumnDown()
{
	m_nVolume -= 10;
	SetVolumn(m_nVolume);
}

LRESULT CJPPlayerBottomBar::OnClose( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	::UnregisterHotKey(GetHWND(),WM_HOTKEY_VOLDOWN);
	::UnregisterHotKey(GetHWND(),WM_HOTKEY_VOLUP);
	::UnregisterHotKey(GetHWND(),WM_HOTKEY_FORWARD);
	::UnregisterHotKey(GetHWND(),WM_HOTKEY_BACKWARD);
	return __super::OnClose(uMsg,wParam,lParam,bHandled);
}
