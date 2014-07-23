#pragma once
#include "UIlib.h"
#include <vlc/vlc.h>

class CJPPlayerTopBar:
	public DuiLib::WindowImplBase
{
public:
	CJPPlayerTopBar(void);
	~CJPPlayerTopBar(void);
	//duilib²¿·Ö
	LPCTSTR GetWindowClassName() const
	{
		return JUSTPLAYIT_TOPBAR_NAME;
	}
	UINT GetClassStyle() const
	{
		return UI_CLASSSTYLE_FRAME | CS_DBLCLKS;
	}
	void Notify(DuiLib::TNotifyUI& msg);
	DuiLib::CDuiString GetSkinFile();
	DuiLib::CDuiString GetSkinFolder();
	void Init(libvlc_media_player_t* player, HWND hParent);
	void SetTitle(const char* szTitle);
protected:
	virtual LRESULT HandleMessage( UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

	void OnCloseClick();
	void OnMinClick();
	void OnMaxClick();
	void OnRestoreClick();
private:
	libvlc_media_player_t*			m_vlcplayer;
	HWND							m_hParent;
	DuiLib::CButtonUI*				m_btnVideoName;
	DuiLib::CButtonUI*				m_btnClose;
	DuiLib::CButtonUI*				m_btnMin;
	DuiLib::CButtonUI*				m_btnMax;
	DuiLib::CButtonUI*				m_btnRestore;
};
