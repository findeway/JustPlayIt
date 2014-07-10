#pragma once

#include "UIlib.h"

class CJPPlayerBottomBar:
	public DuiLib::WindowImplBase
{
public:
	CJPPlayerBottomBar(void);
	~CJPPlayerBottomBar(void);
	//duilib部分
	LPCTSTR GetWindowClassName() const
	{
		return L"JPBottomBar";
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
	bool Play();
	bool Stop();
	bool Pause();
	bool Seek(int pos);
	bool VolumnUp(int num);
	bool VolumnDown(int num);
	void FastForward();
	void FastBackward();
};
