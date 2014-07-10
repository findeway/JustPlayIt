#pragma once

#include "UIlib.h"
#include <vlc/vlc.h>
#include "JPPlayerBottomBar.h"
#include "Singleton.h"

enum EMediaType
{
	EMediaType_Local = 0,
	EMediaType_Url
};

class CJPMainWindow:
	public DuiLib::WindowImplBase,
	public SohuTool::SingletonImpl<CJPMainWindow>
{
public:
	CJPMainWindow(void);
	~CJPMainWindow(void);
	//duilib����
	LPCTSTR GetWindowClassName() const
	{
		return JUSTPLAYIT_WINDOW_NAME;
	}
	UINT GetClassStyle() const
	{
		return UI_CLASSSTYLE_FRAME | CS_DBLCLKS;
	}

	virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT HandleMessage( UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
	
	void Notify(DuiLib::TNotifyUI& msg);
	DuiLib::CDuiString GetSkinFile();
	DuiLib::CDuiString GetSkinFolder();

	//�������ӿ�: ֧��m3u8,����MP4�뱾����Ƶ
	void Play(const wchar_t* uri,EMediaType mediaType = EMediaType_Local);
	
	bool InitPlayer(const wchar_t* argv[] = NULL,int argc = 0);
	bool UnInitPlayer();
	bool SetupVlcHook();				//hook vlc windows
	bool UnHookVlc();
	bool ShowBottombar(bool bShow);

	//���������Ϣʵ��
	bool SetDoubleClick();
	bool IsDoubleClick();

	//����ȫ��
	bool SetFullScreen(bool bFullScreen);

	bool IsBottomBarVisible();
	HWND GetBottomBar();
	RECT GetBottomBarRect();
protected:
	void PlayOnLineVideo(const wchar_t* uri);
	void PlayLocalVideo(const wchar_t* uri);

	RECT GetDesktopRect(bool bWorkArea);
	virtual LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
private:
	libvlc_instance_t*				m_vlc_instance;
	libvlc_media_player_t*			m_vlc_player;
	libvlc_media_t*					m_vlc_media;
	libvlc_log_t*					m_vlc_log;

	CJPPlayerBottomBar*				m_bottomBar;
	bool							m_bFullScreen;
	LONG_PTR						m_oldStyle;
	int								m_nDoubleClick;		//0Ϊ���״̬��2��ʾ˫��
	RECT							m_lastRect;
};