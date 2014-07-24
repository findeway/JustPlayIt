#pragma once

#define JUSTPLAYIT_WINDOW_NAME  _T("{5E10ADBE-9B72-4684-AE43-F6C6CDA5BD94}")
#define JUSTPLAYIT_BOTTOMBAR_NAME	_T("{CD1135B2-FE08-4d4f-BAB7-9AFFC521E996}")
#define JUSTPLAYIT_TOPBAR_NAME		_T("{8CA0F3EC-8F13-425e-A29C-1C2CCB86096E}")

//主窗口
//Welcome 窗口
#define UI_NAME_EDIT_URI					_T("editUri")
#define UI_NAME_BUTTON_OPENFILE				_T("btnOpenFile")
#define UI_NAME_BUTTON_WELCOMEMIN			_T("btnWelcomeMin")
#define UI_NAME_BUTTON_WELCOMECLOSE			_T("btnWelcomeClose")
#define UI_NAME_BUTTON_WELCOMEMAX			_T("btnWelcomeMax")
#define UI_NAME_BUTTON_WELCOMERESTORE		_T("btnWelcomeRestore")
#define UI_NAME_LABEL_PLAYERROR				_T("labelPlayerError")

//播放控制面板
#define UI_NAME_BUTTON_MUTE			_T("btnVolume")
#define UI_NAME_BUTTON_MUTE_ZERO	_T("btnVolumeZero")
#define UI_NAME_BUTTON_STOP			_T("btnStop")
#define UI_NAME_BUTTON_FORWARD		_T("btnForward")
#define UI_NAME_BUTTON_BACKWARD		_T("btnBackward")
#define UI_NAME_BUTTON_PLAY			_T("btnPlay")
#define UI_NAME_BUTTON_PAUSE		_T("btnPause")
#define UI_NAME_SLIDER_VOL			_T("sliderVol")
#define UI_NAME_SLIDER_PROGRESS		_T("sliderProgress")
#define UI_NAME_BUTTON_TOPMOST		_T("btnTopMost")
#define UI_NAME_BUTTON_NOTOPMOST	_T("btnNoTopMost")
#define UI_NAME_LABEL_CURTIME		_T("labelCurTime")
#define UI_NAME_LABEL_TOTALTIME		_T("labelTotalTime")

//topbar
#define UI_NAME_BUTTON_MIN			_T("btnMin")
#define UI_NAME_BUTTON_CLOSE		_T("btnClose")
#define UI_NAME_BUTTON_VIDEONAME	_T("btnVideoName")
#define UI_NAME_BUTTON_MAX			_T("btnMax")
#define UI_NAME_BUTTON_RESTORE		_T("btnRestore")

#define WM_VIDEO_DBCLICK			(WM_USER + 500)
#define WM_CMD_PLAYEND				(WM_USER + 501)
#define WM_HOTKEY_FORWARD			(WM_USER + 502)
#define WM_HOTKEY_BACKWARD			(WM_USER + 503)
#define WM_HOTKEY_VOLUP				(WM_USER + 504)
#define WM_HOTKEY_VOLDOWN			(WM_USER + 505)