#ifndef IM_IDOCHOSTUIHANDLERIMPL_H
#define IM_IDOCHOSTUIHANDLERIMPL_H

namespace shcontrol
{


class IDocHostUIHandlerImpl : public IDocHostUIHandlerDispatch
{
public:
// IDispatch
    STDMETHOD( GetTypeInfoCount )( UINT* pctinfo )
    {
        return E_FAIL;
    }
    STDMETHOD( GetTypeInfo )( UINT itinfo, LCID lcid, ITypeInfo** pptinfo )
    {
        return E_FAIL;
    }
    STDMETHOD( GetIDsOfNames )( REFIID riid, LPOLESTR* rgszNames, UINT cNames,
                                LCID lcid, DISPID* rgdispid )
    {
        return E_FAIL;
    }
    STDMETHOD( Invoke )( DISPID dispidMember, REFIID riid,
                         LCID lcid, WORD wFlags, DISPPARAMS* pdispparams, VARIANT* pvarResult,
                         EXCEPINFO* pexcepinfo, UINT* puArgErr )
    {
        return E_FAIL;
    }


    STDMETHOD( ShowContextMenu )(
        DWORD dwID,
        DWORD x,
        DWORD y,
        IUnknown* pcmdtReserved,
        IDispatch* pdispReserved,
        HRESULT* dwRetVal )
    {
        return E_NOTIMPL;
    }
    STDMETHOD( GetHostInfo )(
        DWORD* pdwFlags,
        DWORD* pdwDoubleClick )
    {
        return E_NOTIMPL;
    }
    STDMETHOD( ShowUI )(
        DWORD dwID,
        IUnknown* pActiveObject,
        IUnknown* pCommandTarget,
        IUnknown* pFrame,
        IUnknown* pDoc,
        HRESULT* dwRetVal )
    {
        return E_NOTIMPL;

    }
    STDMETHOD( HideUI )()
    {
        return E_NOTIMPL;
    }
    STDMETHOD( UpdateUI )()
    {
        return E_NOTIMPL;
    }
    STDMETHOD( EnableModeless )( VARIANT_BOOL fEnable )
    {
        return E_NOTIMPL;
    }
    STDMETHOD( OnDocWindowActivate )(
        VARIANT_BOOL fActivate )
    {
        return E_NOTIMPL;
    }
    STDMETHOD( OnFrameWindowActivate )(
        VARIANT_BOOL fActivate )
    {
        return E_NOTIMPL;
    }
    STDMETHOD( ResizeBorder )(
        long left,
        long top,
        long right,
        long bottom,
        IUnknown * pUIWindow,
        VARIANT_BOOL fFrameWindow )
    {
        return E_NOTIMPL;
    }
    STDMETHOD( TranslateAccelerator )(
        DWORD hWnd,
        DWORD nMessage,
        DWORD wParam,
        DWORD lParam,
        BSTR bstrGuidCmdGroup,
        DWORD nCmdID,
        HRESULT* dwRetVal )
    {
        return E_NOTIMPL;
    }
    STDMETHOD( GetOptionKeyPath )(
        BSTR* pbstrKey,
        DWORD dw )
    {
        return E_NOTIMPL;
    }
    STDMETHOD( GetDropTarget )(
        IUnknown* pDropTarget,
        IUnknown** ppDropTarget )
    {
        return E_NOTIMPL;
    }
    STDMETHOD( GetExternal )(
        IDispatch **ppDispatch )
    {
        return E_NOTIMPL;
    }
    STDMETHOD( TranslateUrl )(
        DWORD dwTranslate,
        BSTR bstrURLIn,
        BSTR* pbstrURLOut )
    {
        return E_NOTIMPL;
    }
    STDMETHOD( FilterDataObject )(
        IUnknown *pDO,
        IUnknown **ppDORet )
    {
        return E_NOTIMPL;
    }




    /*
    	STDMETHOD(ShowContextMenu)(DWORD dwID, POINT FAR* ppt, IUnknown FAR* pcmdtReserved,
    		IDispatch FAR* pdispReserved)
    	{
    		//return peer_->showContextMenu( dwID, ppt, pcmdtReserved, pdispReserved );
    		return E_NOTIMPL;

    	}

    	STDMETHOD(GetHostInfo)(DOCHOSTUIINFO FAR *pInfo)
    	{
    		//peer_->getHostInfo( pInfo );
    		return S_OK;
    	}

    	STDMETHOD(ShowUI)(DWORD dwID, IOleInPlaceActiveObject FAR* pActiveObject,
    		IOleCommandTarget FAR* pCommandTarget,
    		IOleInPlaceFrame  FAR* pFrame,
    		IOleInPlaceUIWindow FAR* pDoc)	{
    		return E_NOTIMPL;
    	}

    	STDMETHOD(HideUI)(void)	{
    		return E_NOTIMPL;
    	}

    	STDMETHOD(UpdateUI)(void)	{
    		return E_NOTIMPL;
    	}

    	STDMETHOD(EnableModeless)(BOOL fEnable)	{
    		return E_NOTIMPL;
    	}

    	STDMETHOD(OnDocWindowActivate)(BOOL fActivate)	{
    		return E_NOTIMPL;
    	}

    	STDMETHOD(OnFrameWindowActivate)(BOOL fActivate)	{
    		return E_NOTIMPL;
    	}

    	STDMETHOD(ResizeBorder)(LPCRECT prcBorder, IOleInPlaceUIWindow FAR* pUIWindow,
    		BOOL fRameWindow)	{
    		return E_NOTIMPL;
    	}

    	STDMETHOD(TranslateAccelerator)(LPMSG lpMsg, const GUID FAR* pguidCmdGroup,
    		DWORD nCmdID)	{
    		return E_NOTIMPL;
    	}

    	STDMETHOD(GetOptionKeyPath)(LPOLESTR FAR* pchKey, DWORD dw)	{
    		return E_NOTIMPL;
    	}

    	STDMETHOD(GetDropTarget)(IDropTarget* pDropTarget, IDropTarget** ppDropTarget)	{
    		return E_NOTIMPL;
    	}

    	STDMETHOD(GetExternal)(IDispatch** ppDispatch) {
    		return E_NOTIMPL;
    	}

    	STDMETHOD(TranslateUrl)(DWORD dwTranslate, OLECHAR* pchURLIn,
    		OLECHAR** ppchURLOut)	{
    		return E_NOTIMPL;
    	}

    	STDMETHOD(FilterDataObject)(IDataObject* pDO, IDataObject** ppDORet) {
    		return E_NOTIMPL;
    	}
    */

};

}


#endif