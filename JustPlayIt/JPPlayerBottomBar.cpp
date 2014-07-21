#include "StdAfx.h"
#include "JPPlayerBottomBar.h"

using namespace DuiLib;

#define ID_TIMER_UPDATE_PROGRESS		(5005)
#define DURATION_TIMER_UPDATE_PROGRESS  (1000/60)

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
	return false;
}

bool CJPPlayerBottomBar::Pause()
{
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
	if(m_vlcplayer)
	{
		double progress = pos/100.0;
		m_nCurPos = pos;
		libvlc_media_player_set_position(m_vlcplayer,progress);
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
	return false;
}

void CJPPlayerBottomBar::FastForward()
{

}

void CJPPlayerBottomBar::FastBackward()
{

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
	if(uMsg == WM_TIMER)
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
	if(m_hParent && IsWindow(m_hParent))
	{
		if(bTopMost)
		{
			::SetWindowPos(m_hParent,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOREPOSITION|SWP_NOREDRAW|SWP_NOMOVE);
			::SetWindowPos(GetHWND(),m_hParent,0,0,0,0,SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOREPOSITION|SWP_NOMOVE);
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
			::SetWindowPos(GetHWND(),HWND_NOTOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOREPOSITION|SWP_NOMOVE);
			::SetWindowPos(m_hParent,HWND_NOTOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOREPOSITION|SWP_NOREDRAW|SWP_NOMOVE);
			if(m_btnTopMost)
			{
				m_btnTopMost->SetVisible(true);
			}
			if(m_btnNoTopMost)
			{
				m_btnNoTopMost->SetVisible(false);
			}
		}
	}
	return bTopMost;
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
		m_nCurPos = libvlc_media_player_get_position(m_vlcplayer)*100.0;
		if(m_slideProgress)
		{
			m_slideProgress->SetValue(m_nCurPos);
		}
		SetCurTime(int(m_duration*m_nCurPos/100));
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
	DuiLib::CDuiString strTime;
	int hour = time/(60*60);
	int minute = time/60;
	int second = time%60;
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
