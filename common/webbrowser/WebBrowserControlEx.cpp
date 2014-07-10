#include "StdAfx.h"
#include "WebBrowserControlEx.h"
#include "SHPlayerDef.h"
#include <comutil.h>

CWebBrowserControlEx::CWebBrowserControlEx(void)
{
	m_dispInDefaultWindow = FALSE;
}

CWebBrowserControlEx::~CWebBrowserControlEx(void)
{

}

void CWebBrowserControlEx::OnDocumentComplete(IDispatch* pDisp, VARIANT* URL)
{
	::SendMessage(::GetParent(m_hWnd), WM_SH_LOAD_URL_SUCCESS, 0, (LPARAM)this);
}

void CWebBrowserControlEx::OnGoBackStatusChanged(BOOL enable)
{
	::SendMessage(::GetParent(m_hWnd), WM_SH_BACK_STATUS_CHANGED, WPARAM(enable), (LPARAM)this);
}

void CWebBrowserControlEx::OnGoFrowardStatusChanged(BOOL enable)
{
	::SendMessage(::GetParent(m_hWnd), WM_SH_FRONT_STATUS_CHANGED, WPARAM(enable), (LPARAM)this);
}

void CWebBrowserControlEx::OnNewWindow3(IDispatch **ppDisp,
													 VARIANT_BOOL *Cancel,
													 DWORD dwFlags,
													 BSTR bstrUrlContext,
													 BSTR bstrUrl
													 )
{
	if(m_dispInDefaultWindow && _tcsstr((wchar_t*)_bstr_t(bstrUrl),_T("about:blank")) == NULL)
	{
		if(Cancel)
			*Cancel = VARIANT_TRUE;
		if(m_openNewWindowFunc.empty() || !m_openNewWindowFunc((wchar_t*)_bstr_t(bstrUrl)))
		{
			Navigate2((wchar_t*)_bstr_t(bstrUrl));
		}	
	}
}

BOOL CWebBrowserControlEx::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->hwnd != GetIEHwnd())
		return FALSE;
	if (pMsg->message == WM_MOUSEMOVE)
	{
		TRACKMOUSEEVENT trkMouse;
		trkMouse.cbSize = sizeof( TRACKMOUSEEVENT );
		trkMouse.dwFlags = TME_LEAVE;
		trkMouse.hwndTrack = pMsg->hwnd;
		_TrackMouseEvent( &trkMouse );
	}	
	else if(pMsg->message == WM_MOUSELEAVE)
	{
		HWND hParent = GetParent();
		if(hParent == NULL)
			return 0;
		POINT pt;
		CRect rect;
		GetCursorPos(&pt);
		GetWindowRect(&rect);
		rect.DeflateRect(1,1,1,1);
		if (PtInRect(&rect,pt))
		{
			return 0;
		}
		::SendMessage(hParent,WM_SH_BROWSER_MOUSE_LEAVE,0,0);
	}
	return shcontrol::CWebBrowserControl::PreTranslateMessage(pMsg);
}