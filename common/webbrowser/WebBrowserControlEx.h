#pragma once
#include "WebBrowserControl.h"

#define WM_SH_LOAD_URL_SUCCESS		(WM_USER + 1104)
#define WM_SH_BACK_STATUS_CHANGED   (WM_USER + 1107)
#define WM_SH_FRONT_STATUS_CHANGED  (WM_USER + 1108)
#define WM_SH_BROWSER_MOUSE_LEAVE   (WM_USER + 1109)

class CWebBrowserControlEx : public shcontrol::CWebBrowserControl
{
public:
	CWebBrowserControlEx(void);
	~CWebBrowserControlEx(void);

protected:
	//virtual void STDMETHODCALLTYPE  OnDownloadComplete();
	virtual void STDMETHODCALLTYPE  OnDocumentComplete(IDispatch* pDisp, VARIANT* URL);
	virtual void OnGoBackStatusChanged(BOOL enable);
	virtual void OnGoFrowardStatusChanged(BOOL enable);
	void STDMETHODCALLTYPE  OnNewWindow3(IDispatch **ppDisp,
		VARIANT_BOOL *Cancel,
		DWORD dwFlags,
		BSTR bstrUrlContext,
		BSTR bstrUrl
		);
	BOOL PreTranslateMessage(MSG* pMsg);
public:
	BOOL m_dispInDefaultWindow;
	boost::function<BOOL(LPCTSTR)> m_openNewWindowFunc;
};
