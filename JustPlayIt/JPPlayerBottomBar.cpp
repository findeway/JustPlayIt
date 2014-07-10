#include "StdAfx.h"
#include "JPPlayerBottomBar.h"

CJPPlayerBottomBar::CJPPlayerBottomBar(void)
{
}

CJPPlayerBottomBar::~CJPPlayerBottomBar(void)
{
}

bool CJPPlayerBottomBar::Play()
{
	return false;
}

bool CJPPlayerBottomBar::Stop()
{
	return false;
}

bool CJPPlayerBottomBar::Pause()
{
	return false;
}

bool CJPPlayerBottomBar::Seek( int pos )
{
	return false;
}

bool CJPPlayerBottomBar::VolumnUp( int num )
{
	return false;
}

bool CJPPlayerBottomBar::VolumnDown( int num )
{
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
