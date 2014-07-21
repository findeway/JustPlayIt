#include "StdAfx.h"
#include "JPPlayerTopBar.h"
#include "Util.h"
#include "JPMainWindow.h"

CJPPlayerTopBar::CJPPlayerTopBar(void)
{
	m_vlcplayer = NULL;
	m_hParent = NULL;
	m_btnMin = NULL;
	m_btnClose = NULL;
	m_btnVideoName = NULL;
}

CJPPlayerTopBar::~CJPPlayerTopBar(void)
{
}

void CJPPlayerTopBar::Notify( DuiLib::TNotifyUI& msg )
{
	if(msg.sType == DUI_MSGTYPE_CLICK)
	{
		if(msg.pSender->GetName() == UI_NAME_BUTTON_CLOSE)
		{
			OnCloseClick();
		}
		else if(msg.pSender->GetName() == UI_NAME_BUTTON_MIN)
		{
			OnMinClick();
		}
		else if(msg.pSender->GetName() == UI_NAME_BUTTON_VIDEONAME)
		{
		}
	}
	__super::Notify(msg);
}

DuiLib::CDuiString CJPPlayerTopBar::GetSkinFile()
{
	return _T("UITopBar.xml");
}

DuiLib::CDuiString CJPPlayerTopBar::GetSkinFolder()
{
	return _T("skins");
}

LRESULT CJPPlayerTopBar::HandleMessage( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	return __super::HandleMessage(uMsg,wParam,lParam);
}

LRESULT CJPPlayerTopBar::OnCreate( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	LRESULT result = __super::OnCreate(uMsg,wParam,lParam,bHandled);
	if(!m_btnVideoName)
	{
		m_btnVideoName = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl(UI_NAME_BUTTON_VIDEONAME));
	}
	if(!m_btnMin)
	{
		m_btnMin = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl(UI_NAME_BUTTON_CLOSE));
	}
	if(!m_btnClose)
	{
		m_btnClose = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl(UI_NAME_BUTTON_MIN));
	}
	return result;
}

void CJPPlayerTopBar::Init( libvlc_media_player_t* player, HWND hParent )
{
	m_vlcplayer = player;
	m_hParent = hParent;
}

void CJPPlayerTopBar::OnCloseClick()
{
	if(m_hParent && ::IsWindow(m_hParent))
	{
		CJPMainWindow::Instance()->QuitApplication();
	}
}

void CJPPlayerTopBar::OnMinClick()
{
	if(m_hParent && ::IsWindow(m_hParent))
	{
		::ShowWindow(m_hParent,SW_MINIMIZE);
	}
}

void CJPPlayerTopBar::SetTitle( const char* szTitle )
{
	std::wstring strTitle = Utf82W(szTitle);
	if(m_btnVideoName)
	{
		m_btnVideoName->SetText(strTitle.c_str());
	}
}
