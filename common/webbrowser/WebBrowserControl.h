#pragma once
#include "IOleClientSiteImpl.h"
#include "IOleInPlaceSiteImpl.h"
#include "IDocHostUIHandlerImpl.h"
#include "IWebBrowserEventImpl.h"
#include <atlwin.h>
#include <bitset>
#include <atlapp.h>
#include <atlmisc.h>
#include <shlguid.h>
#include <map>
#include <atlstr.h>
#include <shellapi.h>
#include <boost/function.hpp>
#include <comutil.h>
using namespace std;

namespace shcontrol
{
#define IUNKNOWNREFCOUNT_FAKE_IMPL() \
	virtual ULONG STDMETHODCALLTYPE AddRef( void){ \
		return 1;\
	}\
	\
	virtual ULONG STDMETHODCALLTYPE Release( void){\
		return 1; \
	}

#define BEGIN_FAKE_IUNKNOWN(_class)\
STDMETHOD(QueryInterface)( REFIID riid, void** obj ) {\
	HRESULT hr = E_NOINTERFACE;\
	if ( riid == IID_IUnknown ) {\
		*obj = this;\
		hr = S_OK;\
	} \
	if ( riid == IID_IDispatch ) {\
		*obj = this;\
		hr = S_OK;\
	}


#define FAKE_INTERFACE_ENTRY( BaseClass ) \
	else if ( riid == __uuidof(BaseClass)  ) { \
		*obj = (BaseClass*)this;\
		return S_OK;\
	}

#define CHAIN_FAKE_IUNKNOWN(ParentClass) \
	hr = ParentClass::QueryInterface(riid,obj);\
	if( hr == S_OK ) return hr;
#define END_FAKE_IUNKNOWN() \
	return hr; } \
	IUNKNOWNREFCOUNT_FAKE_IMPL()


///////////////////////////////////////////////

#define WEBCTRL_ID (UINT) - 1
#define MAX_WEBINTERFACE_BITS 100

#define IMPL_IOLECLIENTSITE					0
#define IMPL_IOLEINPLACESITEIMPL			1
#define IMPL_IDOCHOSTUIHANDLERIMPL			2
#define IMPL_DWEBBROWSEREVENTS2				3



template <class DeriveT>
class CWebBrowserImpl:
            public IOleClientSiteImpl,
            public IOleInPlaceSiteImpl,
            public IDocHostUIHandlerImpl,
            public CWindowImpl<CWebBrowserImpl<DeriveT>, CWindow>,
            public IDispEventImpl< WEBCTRL_ID , CWebBrowserImpl<DeriveT>, &DIID_DWebBrowserEvents2, &LIBID_SHDocVw, 1, 0 >

{
public:
    typedef IDispEventImpl< WEBCTRL_ID , CWebBrowserImpl, &DIID_DWebBrowserEvents2, &LIBID_SHDocVw, 1, 0 > BrowserEvents;
	static map<HWND,ULONG_PTR>	m_procMap;
//构造函数
public:
	CWebBrowserImpl()
	{
		m_bitInterfaceImpl.reset();
		m_pCustomFuncDisp=NULL;
		//m_bGoBackEnable		= FALSE;
		//m_bGoForwardEnable	= FALSE;
		m_hIEWnd			= NULL;
	}

	virtual ~CWebBrowserImpl()
	{

	}

	static ATL::CWndClassInfo& GetWndClassInfo() 
	{ 
		static ATL::CWndClassInfo wc = 
		{ 
			{ sizeof(WNDCLASSEX), /*CS_HREDRAW | CS_VREDRAW | */CS_DBLCLKS, StartWindowProc, 
			  0, 0, NULL, NULL, NULL, (HBRUSH)(COLOR_WINDOW + 1), NULL, NULL, NULL }, 
			NULL, NULL, IDC_ARROW, TRUE, 0, _T("") 
		}; 
		return wc; 
	}
	static LRESULT CALLBACK WindowProc(  HWND hwnd,   UINT uMsg,   WPARAM wParam, 
		LPARAM lParam )
	{
		if(uMsg == WM_PRINTCLIENT)
		{	
			CRect rt;
			::GetClientRect(hwnd, rt );
			HDC hDC    = ::GetDC(hwnd);
			HDC hDstDC = (HDC)wParam;
			HBITMAP hBmp = CopyDCToBitmap(hDC,(LPRECT)rt);
			if(hBmp)
			{
				HDC		hSrcDC		= ::CreateCompatibleDC(hDC);
				HBITMAP hOldBitmap	= (HBITMAP)::SelectObject(hSrcDC,(HGDIOBJ)hBmp);
				::BitBlt(hDstDC,rt.left,rt.top,rt.Width(),rt.Height(),hSrcDC,0,0,SRCCOPY);
				(HBITMAP)::SelectObject(hSrcDC,(HGDIOBJ)hOldBitmap);
				::SelectObject(hSrcDC,(HGDIOBJ)hOldBitmap);
				::DeleteObject(hBmp);
				::DeleteDC(hSrcDC);
			}
			::ReleaseDC(hwnd,hDC);
			return TRUE;
		}
		else
		{
			return CallWindowProc((WNDPROC)m_procMap.find(hwnd)->second,hwnd,uMsg,wParam,lParam);
		}
	}
//方法
public:
    IWebBrowser2* GetWebBrowser2();
	void Navigate2( const ATL::CString &szURL );
	void Refresh();
    HWND GetIEHwnd();
	void SetCustomFuncDisp(LPDISPATCH lpDisp)
	{
		m_pCustomFuncDisp = lpDisp;
		m_wndWebBrowserContainer.SetExternalDispatch( m_pCustomFuncDisp.p );
	}
	STDMETHOD(GetExternal)(IDispatch **ppDispatch )
    {
		if (m_pCustomFuncDisp==NULL)
			return E_NOTIMPL;

		m_pCustomFuncDisp.QueryInterface(ppDispatch);
		//m_pCustomFuncDisp->AddRef();
        return S_OK;
    }
    BOOL InvokeScript(const ATL::CString strFunc, const CSimpleArray<ATL::CString>& paramArray, CComVariant* pVarResult);
	BOOL IsGoBackEnable()const
	{
		return m_bGoBackEnable;
	}
	BOOL IsGoForwardEnable()const
	{
		return m_bGoForwardEnable;
	}
	void GoBack()
	{
		try
		{
			m_spWebBrowser->GoBack();
		}
		catch (...)
		{
		}
	}
	void GoForward()
	{
		try
		{
			m_spWebBrowser->GoForward();
		}
		catch (...)
		{
		}
	}
	ATL::CString GetUrl()
	{
		try
		{
			_bstr_t url;
			m_spWebBrowser->get_LocationURL(url.GetAddress());
			return (LPCTSTR)url;
		}
		catch (...)
		{
			return _T("");
		}
	}
	virtual void OnGoBackStatusChanged(BOOL enable)
	{

	}

	virtual void OnGoFrowardStatusChanged(BOOL enable)
	{

	}
public:
    BEGIN_SINK_MAP( CWebBrowserImpl )
		DWEBBROWSEREVENTS2_SINK_ENTRY( WEBCTRL_ID )
    END_SINK_MAP()

public:
    BEGIN_FAKE_IUNKNOWN( DeriveT )
		FAKE_INTERFACE_ENTRY( IOleClientSite )
		FAKE_INTERFACE_ENTRY( IOleInPlaceSite )
		FAKE_INTERFACE_ENTRY( IDocHostUIHandler )
    END_FAKE_IUNKNOWN()

protected:
    BEGIN_MSG_MAP( CWebBrowserImpl )
		MESSAGE_HANDLER( WM_CREATE, OnCreate )
		MESSAGE_HANDLER( WM_SIZE, OnSize )
		MESSAGE_HANDLER( WM_DESTROY, OnDestroy )
		MESSAGE_HANDLER( WM_ERASEBKGND, OnEraseBkgnd )
		MESSAGE_HANDLER( WM_PRINTCLIENT, OnPrintClient)
    END_MSG_MAP()

protected:
	#pragma region [msg handler]
    LRESULT OnCreate( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
    {
        DefWindowProc( uMsg, wParam, lParam );
        LPOLESTR pStrClassID = NULL;
        HRESULT hr = StringFromCLSID( CLSID_WebBrowser , &pStrClassID );
        //COLE2A pClsid(pStrClassID);

        //hr = _Module.Init( NULL, _Module.GetModuleInstance(),&LIBID_SHDocVw );

        CRect rc;
        GetClientRect( rc );
        HWND h = m_wndWebBrowserContainer.Create( m_hWnd, rc, NULL, WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE/*| WS_VISIBLE */, 0 , WEBCTRL_ID );
        ATLASSERT( h != NULL );

        hr = m_wndWebBrowserContainer.CreateControl( pStrClassID );
        CoTaskMemFree( pStrClassID );

        hr = m_wndWebBrowserContainer.QueryControl( &m_spWebBrowser );
        ATLASSERT( hr == S_OK );

        if ( m_bitInterfaceImpl.test( IMPL_IOLECLIENTSITE ) )
        {
            CComPtr<IOleObject> spOleOb;
            hr = m_spWebBrowser->QueryInterface( IID_IOleObject, ( void ** ) & spOleOb );
            if ( SUCCEEDED( hr ) )
            {
                hr = spOleOb->SetClientSite( static_cast<IOleClientSite *>( this ) );
            }
        }

        if ( m_bitInterfaceImpl.test( IMPL_IDOCHOSTUIHANDLERIMPL ) )
        {
            m_wndWebBrowserContainer.SetExternalUIHandler( static_cast<IDocHostUIHandlerDispatch *>( this ) );
            ATLASSERT( SUCCEEDED( hr ) ) ;
        }


        if ( m_bitInterfaceImpl.test( IMPL_DWEBBROWSEREVENTS2 ) )
        {
            hr = BrowserEvents::DispEventAdvise( m_spWebBrowser , &DIID_DWebBrowserEvents2 );
            ATLASSERT( SUCCEEDED( hr ) ) ;
        }
		m_spWebBrowser->put_RegisterAsDropTarget(VARIANT_FALSE);
		m_spWebBrowser->put_Silent(VARIANT_TRUE);
		HWND hIEWnd   = GetIEHwnd();
		if(hIEWnd)
		{
			LONG_PTR proc = ::SetWindowLongPtr(hIEWnd,GWLP_WNDPROC,(LONG_PTR)&CWebBrowserImpl::WindowProc);
			m_procMap.insert(std::make_pair(GetIEHwnd(),proc));
		}
        return 1L;
    }

    LRESULT OnSize( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
    {
        DefWindowProc( uMsg, wParam, lParam );
        int cx = GET_X_LPARAM( lParam );
        int cy = GET_Y_LPARAM( lParam );
        BOOL b = ::MoveWindow( m_wndWebBrowserContainer.m_hWnd, 0, 0, cx, cy, TRUE );
        return 1L;
    }
    LRESULT OnDestroy( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
    {
		HWND hIEWnd   = GetIEHwnd();
		if(hIEWnd && m_procMap.find(hIEWnd) != m_procMap.end())
		{
			::SetWindowLongPtr(hIEWnd,GWLP_WNDPROC,m_procMap.find(hIEWnd)->second);
			m_procMap.erase(hIEWnd);
		}
        if ( m_bitInterfaceImpl.test( IMPL_IDOCHOSTUIHANDLERIMPL ) )
        {
            HRESULT hr = m_wndWebBrowserContainer.SetExternalUIHandler( NULL );
            ATLASSERT( SUCCEEDED( hr ) ) ;
        }

        if ( m_bitInterfaceImpl.test( IMPL_DWEBBROWSEREVENTS2 ) )
        {
            HRESULT hr = BrowserEvents::DispEventUnadvise( m_spWebBrowser , &DIID_DWebBrowserEvents2 );
            ATLASSERT( SUCCEEDED( hr ) ) ;
        }

        m_spWebBrowser.Release();
        m_spWebBrowser = NULL;


        bHandled = FALSE;
        return 1L;
    }

    LRESULT OnEraseBkgnd( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
	{
		bHandled = TRUE;
		return S_OK;
	}

	static HBITMAP CopyDCToBitmap(HDC hScrDC, LPRECT lpRect)
	{
		HDC hMemDC; 
		// 屏幕和内存设备描述表
		HBITMAP hBitmap,hOldBitmap; 
		// 位图句柄
		int nX, nY, nX2, nY2; 
		// 选定区域坐标
		int nWidth, nHeight; 
		// 位图宽度和高度
		// 确保选定区域不为空矩形
		if (IsRectEmpty(lpRect))
			return NULL;

		// 获得选定区域坐标
		nX = lpRect->left;
		nY = lpRect->top;
		nX2 = lpRect->right;
		nY2 = lpRect->bottom;
		nWidth = nX2 - nX;
		nHeight = nY2 - nY;
		//为指定设备描述表创建兼容的内存设备描述表
		hMemDC = CreateCompatibleDC(hScrDC);
		// 创建一个与指定设备描述表兼容的位图
		hBitmap = CreateCompatibleBitmap(hScrDC, nWidth, nHeight);
		// 把新位图选到内存设备描述表中
		hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
		// 把屏幕设备描述表拷贝到内存设备描述表中
		StretchBlt(hMemDC,0,0,nWidth,nHeight,hScrDC,nX,nY,nWidth,nHeight,SRCCOPY);
		//BitBlt(hMemDC, 0, 0, nWidth, nHeight,hScrDC, nX, nY, SRCCOPY);
		//得到屏幕位图的句柄 
		hBitmap = (HBITMAP)SelectObject(hMemDC, hOldBitmap);
		//清除 

		DeleteDC(hMemDC);
		DeleteObject(hOldBitmap);
		//
		// 返回位图句柄
		return hBitmap;
	}

	//把HBITMAP保存成位图
	static BOOL SaveBmp(HBITMAP hBitmap, ATL::CString FileName)
	{
		HDC hDC;
		//当前分辨率下每象素所占字节数
		int iBits;
		//位图中每象素所占字节数
		WORD wBitCount;
		//定义调色板大小， 位图中像素字节大小 ，位图文件大小 ， 写入文件字节数 
		DWORD dwPaletteSize=0, dwBmBitsSize=0, dwDIBSize=0, dwWritten=0; 
		//位图属性结构 
		BITMAP Bitmap; 
		//位图文件头结构
		BITMAPFILEHEADER bmfHdr; 
		//位图信息头结构 
		BITMAPINFOHEADER bi; 
		//指向位图信息头结构 
		LPBITMAPINFOHEADER lpbi; 
		//定义文件，分配内存句柄，调色板句柄 
		HANDLE fh, hDib, hPal,hOldPal=NULL; 

		//计算位图文件每个像素所占字节数 
		hDC = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
		iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES); 
		DeleteDC(hDC); 
		if (iBits <= 1) wBitCount = 1; 
		else if (iBits <= 4) wBitCount = 4; 
		else if (iBits <= 8) wBitCount = 8; 
		else wBitCount = 24; 

		GetObject(hBitmap, sizeof(Bitmap), (LPSTR)&Bitmap);
		bi.biSize = sizeof(BITMAPINFOHEADER);
		bi.biWidth = Bitmap.bmWidth;
		bi.biHeight = Bitmap.bmHeight;
		bi.biPlanes = 1;
		bi.biBitCount = wBitCount;
		bi.biCompression = BI_RGB;
		bi.biSizeImage = 0;
		bi.biXPelsPerMeter = 0;
		bi.biYPelsPerMeter = 0;
		bi.biClrImportant = 0;
		bi.biClrUsed = 0;

		dwBmBitsSize = ((Bitmap.bmWidth * wBitCount + 31) / 32) * 4 * Bitmap.bmHeight;

		//为位图内容分配内存 
		hDib = GlobalAlloc(GHND,dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER)); 
		lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib); 
		*lpbi = bi; 
		// 处理调色板 
		hPal = GetStockObject(DEFAULT_PALETTE); 
		if (hPal) 
		{ 
			hDC = ::GetDC(NULL); 
			hOldPal = ::SelectPalette(hDC, (HPALETTE)hPal, FALSE); 
			RealizePalette(hDC); 
		}
		// 获取该调色板下新的像素值 
		GetDIBits(hDC, hBitmap, 0, (UINT) Bitmap.bmHeight, (LPSTR)lpbi + sizeof(BITMAPINFOHEADER) 
			+dwPaletteSize, (BITMAPINFO *)lpbi, DIB_RGB_COLORS); 

		//恢复调色板 
		if (hOldPal) 
		{ 
			::SelectPalette(hDC, (HPALETTE)hOldPal, TRUE); 
			RealizePalette(hDC); 
			::ReleaseDC(NULL, hDC); 
		} 
		//创建位图文件 
		fh = CreateFile(FileName, GENERIC_WRITE,0, NULL, CREATE_ALWAYS, 
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL); 

		if (fh == INVALID_HANDLE_VALUE) return FALSE; 

		// 设置位图文件头 
		bmfHdr.bfType = 0x4D42; // "BM" 
		dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize; 
		bmfHdr.bfSize = dwDIBSize; 
		bmfHdr.bfReserved1 = 0; 
		bmfHdr.bfReserved2 = 0; 
		bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize; 
		// 写入位图文件头 
		WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL); 
		// 写入位图文件其余内容 
		WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL); 
		//清除 
		GlobalUnlock(hDib); 
		GlobalFree(hDib); 
		CloseHandle(fh); 
		return TRUE;
	}

	LRESULT OnPrintClient( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
	{
 		bHandled = TRUE;
		return 0;
	}

	#pragma endregion

protected:
    CAxWindow m_wndWebBrowserContainer;
    CComPtr<IWebBrowser2> m_spWebBrowser;
    std::bitset< MAX_WEBINTERFACE_BITS > m_bitInterfaceImpl;
    CComPtr<IDispatch> m_pCustomFuncDisp;
	static BOOL m_bGoBackEnable;
	static BOOL m_bGoForwardEnable;
	HWND m_hIEWnd;
protected:
	#pragma region [event]
    virtual void STDMETHODCALLTYPE	OnStatusTextChange( BSTR Text );
    virtual void STDMETHODCALLTYPE  OnDownloadComplete();
    virtual void STDMETHODCALLTYPE  OnCommandStateChange( long Command, VARIANT_BOOL Enable );
    virtual void STDMETHODCALLTYPE  OnDownloadBegin();
    virtual void STDMETHODCALLTYPE  OnProgressChange( long x, long y );
    virtual void STDMETHODCALLTYPE  OnPropertyChange( BSTR szProperty );
    virtual void STDMETHODCALLTYPE  OnTitleChange( BSTR Text );
    virtual void STDMETHODCALLTYPE  OnPrintTemplateInstantiation( IDispatch* pDisp );
    virtual void STDMETHODCALLTYPE  OnPrintTemplateTeardown( IDispatch* pDisp );
    virtual void STDMETHODCALLTYPE  OnUpdatePageStatus(
        IDispatch* pDisp,
        VARIANT* nPage,
        VARIANT* fDone ) ;
    virtual void STDMETHODCALLTYPE  OnBeforeNavigate2(
        IDispatch* pDisp,
        VARIANT* URL,
        VARIANT* Flags,
        VARIANT* TargetFrameName,
        VARIANT* PostData,
        VARIANT* Headers,
        VARIANT_BOOL* Cancel );
    virtual void STDMETHODCALLTYPE  OnNewWindow2(
        IDispatch** ppDisp,
        VARIANT_BOOL* Cancel );
	virtual void STDMETHODCALLTYPE  OnNewWindow3(IDispatch **ppDisp,
		VARIANT_BOOL *Cancel,
		DWORD dwFlags,
		BSTR bstrUrlContext,
		BSTR bstrUrl
		);

    virtual void STDMETHODCALLTYPE  OnNavigateComplete( IDispatch* dispatchPtr, VARIANT* URL );
    virtual void STDMETHODCALLTYPE  OnNavigateComplete2( IDispatch* dispatchPtr, VARIANT* URL );
    virtual void STDMETHODCALLTYPE  OnQuit();
    virtual void STDMETHODCALLTYPE  OnDocumentComplete(
        IDispatch* pDisp,
        VARIANT* URL ) ;
    virtual void STDMETHODCALLTYPE  OnWindowClosing( VARIANT_BOOL IsChildWindow, VARIANT_BOOL* Cancel );
    virtual void STDMETHODCALLTYPE  OnClientToHostWindow( long* CX, long* CY );
    virtual void STDMETHODCALLTYPE  OnSetSecureLockIcon( long SecureLockIcon );
    virtual void STDMETHODCALLTYPE  OnFileDownload( VARIANT_BOOL* Cancel );
    virtual void STDMETHODCALLTYPE  OnNavigateError(
        IDispatch* pDisp,
        VARIANT* URL,
        VARIANT* Frame,
        VARIANT* StatusCode,
        VARIANT_BOOL* Cancel );
    virtual void STDMETHODCALLTYPE OnPrivacyImpactedStateChange( VARIANT_BOOL bImpacted ) ;
	#pragma endregion

};

template <class DeriveT>
BOOL shcontrol::CWebBrowserImpl<DeriveT>::m_bGoBackEnable = FALSE;

template <class DeriveT>
BOOL shcontrol::CWebBrowserImpl<DeriveT>::m_bGoForwardEnable = FALSE;

template<class DeriveT >
map<HWND,ULONG_PTR>	CWebBrowserImpl<DeriveT >::m_procMap;

template<class DeriveT >
void STDMETHODCALLTYPE CWebBrowserImpl<DeriveT >::OnStatusTextChange( BSTR Text )
{
    return ;
}
template<class DeriveT >
void STDMETHODCALLTYPE  CWebBrowserImpl<DeriveT >::OnDownloadComplete()
{
    return ;
}
template<class DeriveT >
void STDMETHODCALLTYPE  CWebBrowserImpl<DeriveT >::OnCommandStateChange( long Command, VARIANT_BOOL Enable )
{
	if(Command == 1)
	{
		if(m_bGoForwardEnable != (Enable == VARIANT_TRUE))
		{
			m_bGoForwardEnable = Enable == VARIANT_TRUE;
			OnGoFrowardStatusChanged(m_bGoForwardEnable);
		}
	}
	else if(Command == 2)
	{
		if(m_bGoBackEnable != (Enable == VARIANT_TRUE))
		{
			m_bGoBackEnable	   = Enable == VARIANT_TRUE;
			OnGoBackStatusChanged(m_bGoBackEnable);
		}
	}
}
template<class DeriveT >
void STDMETHODCALLTYPE  CWebBrowserImpl<DeriveT >::OnDownloadBegin()
{
    return ;
}
template<class DeriveT >
void STDMETHODCALLTYPE  CWebBrowserImpl<DeriveT >::OnProgressChange( long x, long y )
{
    return ;
}
template<class DeriveT >
void STDMETHODCALLTYPE  CWebBrowserImpl<DeriveT >::OnPropertyChange( BSTR szProperty )
{
    return ;
}
template<class DeriveT >
void STDMETHODCALLTYPE  CWebBrowserImpl<DeriveT >::OnTitleChange( BSTR Text )
{
    return ;
}
template<class DeriveT >
void STDMETHODCALLTYPE  CWebBrowserImpl<DeriveT >::OnPrintTemplateInstantiation( IDispatch* pDisp )
{
    return ;

}
template<class DeriveT >
void STDMETHODCALLTYPE  CWebBrowserImpl<DeriveT >::OnPrintTemplateTeardown( IDispatch* pDisp )
{
    return ;
}
template<class DeriveT >
void STDMETHODCALLTYPE  CWebBrowserImpl<DeriveT >::OnUpdatePageStatus(
    IDispatch* pDisp,
    VARIANT* nPage,
    VARIANT* fDone )
{
    return ;
}
template<class DeriveT >
void STDMETHODCALLTYPE  CWebBrowserImpl<DeriveT >::OnBeforeNavigate2(
    IDispatch* pDisp,
    VARIANT* URL,
    VARIANT* Flags,
    VARIANT* TargetFrameName,
    VARIANT* PostData,
    VARIANT* Headers,
    VARIANT_BOOL* Cancel )
{

    return ;

}
template<class DeriveT >
void STDMETHODCALLTYPE CWebBrowserImpl<DeriveT >::OnNewWindow2(
    IDispatch** ppDisp,
    VARIANT_BOOL* Cancel )
{

	return;
	//点击链接用默认浏览器打开   而不是IE
	
	*Cancel=TRUE;

	CComPtr<IDispatch> pDisp;
	HRESULT hr=m_spWebBrowser->get_Document( &pDisp );

	if(FAILED(hr)||pDisp==NULL) return;

	CComPtr<IHTMLDocument2> pDocument2 = NULL;

	hr = pDisp->QueryInterface(IID_IHTMLDocument2, (void**)&pDocument2);

	if(FAILED(hr)||pDocument2==NULL) return;


	CComPtr<IHTMLElement>  pElement = NULL;
	hr=pDocument2->get_activeElement(&pElement);
	if(FAILED(hr)||pElement==NULL) 
	{
		return;
	}

	
	CComPtr<IHTMLAnchorElement> pLinkElement;
	hr=pElement->QueryInterface(IID_IHTMLAnchorElement,(void**)&pLinkElement);
	if(FAILED(hr)||pLinkElement==NULL) 
	{
		return;
	}
   
	

	CComBSTR url(1000);
	pLinkElement->get_href(&url);
	
	ShellExecute(NULL,_T("open"),url,NULL,NULL,SW_SHOW);
	
}

template<class DeriveT >
void STDMETHODCALLTYPE CWebBrowserImpl<DeriveT >::OnNewWindow3(IDispatch **ppDisp,VARIANT_BOOL *Cancel,DWORD dwFlags,BSTR bstrUrlContext,BSTR bstrUrl)
{
	//*Cancel=TRUE;
  
	//ShellExecute(NULL,_T("open"),bstrUrl,NULL,NULL,SW_SHOW);

	//pcutil::MiscHelper::GotoURL(bstrUrl);
}

template<class DeriveT >
void STDMETHODCALLTYPE  CWebBrowserImpl<DeriveT >::OnNavigateComplete( IDispatch* dispatchPtr, VARIANT* URL )
{
    return ;
}
template<class DeriveT >
void STDMETHODCALLTYPE  CWebBrowserImpl<DeriveT >::OnNavigateComplete2( IDispatch* dispatchPtr, VARIANT* URL )
{
    return ;
}
template<class DeriveT >
void STDMETHODCALLTYPE  CWebBrowserImpl<DeriveT >::OnQuit()
{
    return ;
}
template<class DeriveT >
void STDMETHODCALLTYPE  CWebBrowserImpl<DeriveT >::OnDocumentComplete(
    IDispatch* pDisp,
    VARIANT* URL )
{
    return ;
}
template<class DeriveT >
void STDMETHODCALLTYPE  CWebBrowserImpl<DeriveT >::OnWindowClosing( VARIANT_BOOL IsChildWindow, VARIANT_BOOL* Cancel )
{
    return ;
}
template<class DeriveT >
void STDMETHODCALLTYPE  CWebBrowserImpl<DeriveT >::OnClientToHostWindow( long* CX, long* CY )
{
    return ;
}
template<class DeriveT >
void STDMETHODCALLTYPE  CWebBrowserImpl<DeriveT >::OnSetSecureLockIcon( long SecureLockIcon )
{
    return ;
}
template<class DeriveT >
void STDMETHODCALLTYPE  CWebBrowserImpl<DeriveT >::OnFileDownload( VARIANT_BOOL* Cancel )
{
    return ;
}
template<class DeriveT >
void STDMETHODCALLTYPE  CWebBrowserImpl<DeriveT >::OnNavigateError(
    IDispatch* pDisp,
    VARIANT* URL,
    VARIANT* Frame,
    VARIANT* StatusCode,
    VARIANT_BOOL* Cancel )
{
    return ;
}
template<class DeriveT >
void STDMETHODCALLTYPE CWebBrowserImpl<DeriveT >::OnPrivacyImpactedStateChange( VARIANT_BOOL bImpacted )
{
    return ;
}

template<class DeriveT >
IWebBrowser2* CWebBrowserImpl<DeriveT >::GetWebBrowser2()
{
	return m_spWebBrowser.p;
}

template<class DeriveT >
void CWebBrowserImpl<DeriveT >::Navigate2( const ATL::CString &szURL )
{
    ATLASSERT( m_spWebBrowser != NULL );
    if ( !m_spWebBrowser ) return;
    CComVariant vtURL( szURL ), vt;
	//CComVariant flag(navNoReadFromCache);
	CComVariant flag;
    m_spWebBrowser->Navigate2( &vtURL, &flag, &vt, &vt, &vt );
}

template<class DeriveT >
void CWebBrowserImpl<DeriveT >::Refresh()
{
	ATLASSERT( m_spWebBrowser != NULL );
	if ( !m_spWebBrowser ) return;
	m_spWebBrowser->Refresh();
}

template<class DeriveT >
HWND CWebBrowserImpl<DeriveT >::GetIEHwnd()
{
	if(m_hIEWnd == NULL)
	{
		if ( m_spWebBrowser.p != NULL )
		{
			CComPtr<IDispatch> pDisp;
			HRESULT hr=m_spWebBrowser->get_Document( &pDisp );
			if(FAILED(hr)||pDisp==NULL) return NULL;

			CComPtr<IOleWindow> spWnd;
			pDisp->QueryInterface( IID_IOleWindow , (void **)&spWnd );
			if ( spWnd ) 
			{	
				spWnd->GetWindow( &m_hIEWnd );
			}
		}
	}
	return m_hIEWnd;
	
// 	HWND hWnd = NULL;
// 	IServiceProvider* pServiceProvider = NULL;
// 	if (SUCCEEDED(m_spWebBrowser->QueryInterface(
// 		IID_IServiceProvider, 
// 		(void**)&pServiceProvider)))
// 	{
// 		IOleWindow* pWindow = NULL;
// 		if (SUCCEEDED(pServiceProvider->QueryService(
// 			SID_SShellBrowser, 
// 			IID_IOleWindow,
// 			(void**)&pWindow)))
// 		{
// 			if (SUCCEEDED(pWindow->GetWindow(&hWnd)))
// 			{
// 				// hwndBrowser is the handle of TabWindowClass
// 			}
// 
// 			pWindow->Release();
// 		}
// 
// 		pServiceProvider->Release();
// 	} 
	return m_hIEWnd;
}

template<class DeriveT >
BOOL CWebBrowserImpl<DeriveT >::InvokeScript(const ATL::CString strFunc, const CSimpleArray<ATL::CString>& paramArray, CComVariant* pVarResult)
{
	if ( m_spWebBrowser.p == NULL )
	  return FALSE;

	CComPtr<IDispatch> pDisp;
	HRESULT hr=m_spWebBrowser->get_Document( &pDisp );

	if(FAILED(hr)||pDisp==NULL) return FALSE;

	CComPtr<IHTMLDocument2> pDocument2 = NULL;
	
	hr = pDisp->QueryInterface(IID_IHTMLDocument2, (void**)&pDocument2);

	if(FAILED(hr)||pDocument2==NULL) return FALSE;

	CComPtr<IDispatch> spScript;

	hr = pDocument2->get_Script(&spScript);
	if(!SUCCEEDED(hr))
	{
		return FALSE;
	}
	CComBSTR bstrMember(strFunc);
	DISPID dispid = NULL;
	hr = spScript->GetIDsOfNames(IID_NULL,&bstrMember,1,
		LOCALE_SYSTEM_DEFAULT,&dispid);
	if(FAILED(hr))
	{
		return FALSE;
	}

	const int arraySize = paramArray.GetSize();
	DISPPARAMS dispparams;
	memset(&dispparams, 0, sizeof dispparams);
	dispparams.cArgs = arraySize;
	dispparams.cNamedArgs = 0;

    if (arraySize > 0)
	{
		dispparams.rgvarg = new VARIANT[dispparams.cArgs];

		for( int i = 0; i < arraySize; i++)
		{
			CComBSTR bstr = paramArray[arraySize - 1 - i]; // back reading
			bstr.CopyTo(&dispparams.rgvarg[i].bstrVal);
			dispparams.rgvarg[i].vt = VT_BSTR;
		}
	}


	EXCEPINFO excepInfo;
	memset(&excepInfo, 0, sizeof excepInfo);
	CComVariant vaResult;
	UINT nArgErr = (UINT)-1;  // initialize to invalid arg

	hr = spScript->Invoke(dispid,IID_NULL,0,
		DISPATCH_METHOD,&dispparams,&vaResult,&excepInfo,&nArgErr);

	delete [] dispparams.rgvarg;
	if(FAILED(hr))
	{
		return FALSE;
	}

	if(pVarResult!=NULL)
		*pVarResult = vaResult;
	return TRUE;
}




class CWebBrowserControl
	:public CWebBrowserImpl< CWebBrowserControl >,
	public CMessageFilter
{
public:
	typedef CWebBrowserImpl< CWebBrowserControl > _baseWebBrowser;
    CWebBrowserControl(  );
    virtual ~CWebBrowserControl(  );

public:
	BEGIN_SINK_MAP( CWebBrowserControl )
	END_SINK_MAP()

	BEGIN_MSG_MAP( CWebBrowserControl )
		MESSAGE_HANDLER( WM_CREATE, OnCreate )
		MESSAGE_HANDLER( WM_DESTROY, OnDestroy )
		CHAIN_MSG_MAP( _baseWebBrowser )
	END_MSG_MAP()

public:
	STDMETHOD( ShowContextMenu )( DWORD dwID, DWORD x, DWORD y, IUnknown* pcmdtReserved, IDispatch* pdispReserved, HRESULT* dwRetVal );
    STDMETHOD( GetHostInfo )( DWORD* pdwFlags, DWORD* pdwDoubleClick );
	LRESULT OnDestroy( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	LRESULT OnCreate( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	void	ShowScrollBar(BOOL show){m_bScroll  = show;}
	virtual void STDMETHODCALLTYPE  OnDocumentComplete(IDispatch* pDisp, VARIANT* URL);
	virtual void OnGoBackStatusChanged(BOOL enable);
	virtual void OnGoFrowardStatusChanged(BOOL enable);
public:
	virtual BOOL PreTranslateMessage( MSG* pMsg );
protected:
	BOOL	m_bScroll;
public:
	boost::function<void(void)>	m_documentCompleteFun;
	boost::function<void(BOOL)>	m_gobackStatusChangedFun;
	boost::function<void(BOOL)>	m_goFrowardStatusChangedFun;
	boost::function<void(CMessageFilter*)> m_addMessageFilterFun;
	boost::function<void(CMessageFilter*)> m_removeMessageFilterFun;
};


}