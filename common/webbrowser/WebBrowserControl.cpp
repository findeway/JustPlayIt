#include "StdAfx.h"
#include "WebBrowserControl.h"

namespace shcontrol
{
	CWebBrowserControl::CWebBrowserControl()
	{
		m_bitInterfaceImpl.set( IMPL_DWEBBROWSEREVENTS2 );
		m_bitInterfaceImpl.set( IMPL_IDOCHOSTUIHANDLERIMPL );
		m_bScroll		 = TRUE;	
	}

	CWebBrowserControl::~CWebBrowserControl()
	{

	}

	STDMETHODIMP CWebBrowserControl::ShowContextMenu( DWORD dwID, DWORD x, DWORD y, IUnknown* pcmdtReserved,
		IDispatch* pdispReserved, HRESULT* dwRetVal )
	{
		*dwRetVal = S_OK;
		return S_OK;
	}

	STDMETHODIMP CWebBrowserControl::GetHostInfo( DWORD* pdwFlags, DWORD* pdwDoubleClick )
	{
		*pdwDoubleClick = DOCHOSTUIDBLCLK_DEFAULT;
		if(pdwFlags)
		{
			*pdwFlags |= (FALSE ? 0 : DOCHOSTUIFLAG_NO3DBORDER);
			*pdwFlags |= (m_bScroll ? 0 : DOCHOSTUIFLAG_SCROLL_NO);
			*pdwFlags |= DOCHOSTUIFLAG_NO3DBORDER /*| DOCHOSTUIFLAG_SCROLL_NO */;
		}
		return S_OK;
	}


	BOOL CWebBrowserControl::PreTranslateMessage( MSG* pMsg )
	{
		if(  ::IsChild( m_hWnd , pMsg->hwnd )  )
		{
			if ( ( pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST ) &&
				( pMsg->message < WM_MOUSEFIRST || pMsg->message > WM_MOUSELAST ) )
				return FALSE;

			if ( pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST )
			{
				if ( HIWORD( GetKeyState( VK_CONTROL ) ) && HIWORD( GetKeyState( 'N' ) ) )
				{
					if ( !HIWORD( GetKeyState( 'G' ) ) )
						return FALSE;
				}
			}

			if ( ::SendMessage( m_wndWebBrowserContainer.m_hWnd, WM_FORWARDMSG, 0, ( LPARAM )pMsg ) != 0 )
				return TRUE;
		}
		return FALSE;
	}

	LRESULT CWebBrowserControl::OnCreate( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
	{
		if(!m_addMessageFilterFun.empty())
			m_addMessageFilterFun(this);
		bHandled = FALSE;
		return 1L;
	}

	LRESULT CWebBrowserControl::OnDestroy( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
	{
		if(!m_removeMessageFilterFun.empty())
			m_removeMessageFilterFun(this);
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, EM_SETHANDLE, (WPARAM)NULL, 0L);
		bHandled = FALSE;
		return 1L;
	}

	void CWebBrowserControl::OnDocumentComplete(IDispatch* pDisp, VARIANT* URL)
	{
		if(!m_documentCompleteFun.empty())
			m_documentCompleteFun();
	}

	void CWebBrowserControl::OnGoBackStatusChanged(BOOL enable)
	{
		if(!m_gobackStatusChangedFun.empty())
			m_gobackStatusChangedFun(enable);
	}

	void CWebBrowserControl::OnGoFrowardStatusChanged(BOOL enable)
	{
		if(!m_goFrowardStatusChangedFun.empty())
			m_goFrowardStatusChangedFun(enable);
	}

}