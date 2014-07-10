#include "StdAfx.h"
#include "JPPlayerBottomBar.h"

using namespace DuiLib;

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
	m_nVolume = 50;
}

CJPPlayerBottomBar::~CJPPlayerBottomBar(void)
{
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

bool CJPPlayerBottomBar::Seek( int pos )
{
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
		if(msg.pSender->GetName() == UI_NAME_SLIDER_VOL)
		{

		}
		else if(msg.pSender->GetName() == UI_NAME_BUTTON_PAUSE)
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
	return result;
}

void CJPPlayerBottomBar::Init( libvlc_media_player_t* player, HWND hParent )
{
	m_vlcplayer = player;
	m_hParent = hParent;
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
