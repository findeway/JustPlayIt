#pragma once

#include "UIlib.h"
#include <vlc/vlc.h>
#include "JPPlayerBottomBar.h"
#include "JPPlayerTopBar.h"
#include "SingletonImpl.h"
#include "HLSMetaParser.h"

enum EMediaType
{
	EMediaType_Unknown = -1,
	EMediaType_Local = 0,
	EMediaType_HttpStream,
	EMediaType_HLS
};

class CJPMainWindow:
	public DuiLib::WindowImplBase,
	public SingletonImpl<CJPMainWindow>
{
public:
	CJPMainWindow(void);
	~CJPMainWindow(void);
	//duilib部分
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

	//播放器接口: 支持m3u8,在线MP4与本地视频
	void Play(const wchar_t* uri,EMediaType mediaType = EMediaType_Local);
	
	bool InitPlayer(const wchar_t* argv[] = NULL,int argc = 0);
	bool UnInitPlayer();
	bool SetupVlcHook();				//hook vlc windows
	bool UnHookVlc();
	bool ShowBottombar(bool bShow);
	bool ShowTopBar(bool bShow);

	//部分鼠标消息实现
	bool SetDoubleClick();
	bool IsDoubleClick();

	//设置全屏
	bool SetFullScreen(bool bFullScreen);

	bool IsBottomBarVisible();
	CJPPlayerBottomBar* GetBottomBar();
	RECT GetBottomBarRect();
	RECT GetTopBarRect();

	void QuitApplication();
	bool SetTopMost(bool bTopMost);
	bool IsAutoShowBar();

	//播放开始通知
	void OnPlayBegin();
	/*
	 *	bStop为true代表手动停止，否则代表播放完毕
	 */
	void OnPlayEnd(bool bStop);
	void OnPlayFailed();
protected:
	/*
	 *	return 0 if playback started (and was already started), or -1 on error
	 */
	int PlayNetVideo(const wchar_t* uri);
	int PlayLocalVideo(const wchar_t* uri);
	int PlayHLSStream(const wchar_t* uri);

	RECT GetDesktopRect(bool bWorkArea);
	virtual LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void OnOpenFileClick(const wchar_t* uri,bool bLocal = false);

	void AdjustRatio();	

	void OnCloseClick();
	void OnMinClick();
	void OnMaxClick();
	void OnRestoreClick();

	//解析m3u8元数据
	static void OnGetM3u8Meta(const HLSMetaData& metaData,void* userdata);
private:
	libvlc_instance_t*				m_vlc_instance;
	libvlc_media_player_t*			m_vlc_player;
	libvlc_media_t*					m_vlc_media;
	libvlc_log_t*					m_vlc_log;
	libvlc_event_manager_t*			m_vlc_event_manager;

	CJPPlayerBottomBar*				m_bottomBar;
	CJPPlayerTopBar*				m_topBar;
	bool							m_bFullScreen;
	LONG_PTR						m_oldStyle;
	int								m_nDoubleClick;		//0为清空状态，2表示双击
	RECT							m_lastRect;
	EMediaType						m_sourceType;
	std::wstring					m_uri;
	bool							m_playerActive;
	bool							m_bAutoShowBar;

	//Welcome Scene
	DuiLib::CRichEditUI*			m_editUri;
	DuiLib::CButtonUI*				m_btnOpenFile;
	DuiLib::CButtonUI*				m_btnWelcomeMin;
	DuiLib::CButtonUI*				m_btnWelcomeMax;
	DuiLib::CButtonUI*				m_btnWelcomeClose;
	DuiLib::CButtonUI*				m_btnWelcomeRestore;
	DuiLib::CLabelUI*				m_labelPlayerError;
};
