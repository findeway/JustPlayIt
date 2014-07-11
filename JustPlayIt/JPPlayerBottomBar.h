#pragma once

#include "UIlib.h"
#include <vlc/vlc.h>

class CJPPlayerBottomBar:
	public DuiLib::WindowImplBase
{
public:
	CJPPlayerBottomBar(void);
	~CJPPlayerBottomBar(void);
	//duilib����
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
	//���ſ��ƽӿ�
	bool Resume();
	bool Stop();
	bool Pause();
	bool Seek(int pos);
	bool SetMute(bool bMute);
	void FastForward();
	void FastBackward();
	//��ʼ������ʵ��
	void Init(libvlc_media_player_t* player, HWND hParent);
protected:
	bool SetVolumn(int num);
	bool SetTopMost(bool bTopMost);
	virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
private:
	//�ؼ�
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

	//���ſ������
	libvlc_media_player_t*			m_vlcplayer;
	int								m_nVolume;
	HWND							m_hParent;
};
