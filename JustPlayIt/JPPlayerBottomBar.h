#pragma once

#include "UIlib.h"
#include <vlc/vlc.h>

class CJPPlayerBottomBar:
	public DuiLib::WindowImplBase
{
public:
	CJPPlayerBottomBar(void);
	~CJPPlayerBottomBar(void);
	//duilib部分
	LPCTSTR GetWindowClassName() const
	{
		return JUSTPLAYIT_BOTTOMBAR_NAME;
	}
	UINT GetClassStyle() const
	{
		return UI_CLASSSTYLE_FRAME | CS_DBLCLKS;
	}
	void Notify(DuiLib::TNotifyUI& msg);
	DuiLib::CDuiString GetSkinFile();
	DuiLib::CDuiString GetSkinFolder();

	virtual LRESULT HandleMessage( UINT uMsg, WPARAM wParam, LPARAM lParam );
	//播放控制接口
	bool Resume();
	bool Stop();
	bool Pause();
	//pos 0-100
	bool Seek(double pos);
	bool SetMute(bool bMute);
	void FastForward();
	void FastBackward();
	//初始化播放实例
	void Init(libvlc_media_player_t* player, HWND hParent);

	//设置视频物理时长,以秒为单位
	bool SetDuration(int duration);
protected:
	//num:0-100
	bool SetVolumn(int num);
	//num:0-100
	bool SetProgress(double num);
	//设置当前时间
	bool SetCurTime(int time);

	bool SetTopMost(bool bTopMost);
	void SetUpdateProgress(bool bUpdate);
	void UpdateProgress(bool bUpdate);

	void UpdateHoverTime();
	
	//格式化时间,以秒为单位
	std::wstring FormatTime(int time);

	virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
private:
	//控件
	DuiLib::CButtonUI* m_btnPlay;
	DuiLib::CButtonUI* m_btnPause;

	DuiLib::CButtonUI* m_btnStop;
	
	DuiLib::CButtonUI* m_btnForward;
	DuiLib::CButtonUI* m_btnBackward;

	DuiLib::CButtonUI* m_btnVolume;
	DuiLib::CButtonUI* m_btnVolumeZero;

	DuiLib::CButtonUI* m_btnTopMost;
	DuiLib::CButtonUI* m_btnNoTopMost;

	DuiLib::CSliderUI* m_slideVolumn;
	DuiLib::CSliderUI* m_slideProgress;

	DuiLib::CLabelUI*  m_labelCurTime;
	DuiLib::CLabelUI*  m_labelTotalTime;

	//播放控制相关
	libvlc_media_player_t*			m_vlcplayer;
	int								m_nVolume;		//0-100
	double							m_nCurPos;		//0-100
	HWND							m_hParent;
	int								m_duration;
};
