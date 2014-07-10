#ifndef IM_IOLEINPLACESITEIMPL_H_
#define IM_IOLEINPLACESITEIMPL_H_

namespace shcontrol
{

class IOleInPlaceSiteImpl : public  IOleInPlaceSite 
{
public :
	STDMETHOD(GetWindow)( HWND * phwnd )
	{
	//	*phwnd = peer_->hwnd_;
		return S_OK;
	
	}

	STDMETHOD(ContextSensitiveHelp)( BOOL fEnterMode ) {
		return E_NOTIMPL;
	}

	STDMETHOD(CanInPlaceActivate)(void)	{
		return S_OK;
	}

	STDMETHOD(OnInPlaceActivate)(void) {
		inPlaceActive_ = true;
		return S_OK;
	}

	STDMETHOD(OnUIActivate)(void) {
		return E_NOTIMPL;
	}

	STDMETHOD(GetWindowContext)(IOleInPlaceFrame **ppFrame, IOleInPlaceUIWindow **ppDoc,
		LPRECT lprcPosRect, LPRECT lprcClipRect,
		LPOLEINPLACEFRAMEINFO lpFrameInfo)	
	{
		/*
		Rect r = peer_->getBounds();

		lprcPosRect->left = r.left_;
		lprcPosRect->right = r.right_;
		lprcPosRect->top = r.top_;
		lprcPosRect->bottom = r.bottom_;

		lprcClipRect->left = r.left_;
		lprcClipRect->right = r.right_;
		lprcClipRect->top = r.top_;
		lprcClipRect->bottom = r.bottom_;
		*/
		return S_OK;
	}

	STDMETHOD(Scroll)(SIZE scrollExtant)	{
		return E_NOTIMPL;
	}

	STDMETHOD(OnUIDeactivate)(BOOL fUndoable) {
		return E_NOTIMPL;
	}

	STDMETHOD(OnInPlaceDeactivate)( void) {
		inPlaceActive_ = false;
		return S_OK;
	}

	STDMETHOD(DiscardUndoState)( void) {
		return E_NOTIMPL;
	}

	STDMETHOD(DeactivateAndUndo)( void)	{
		return E_NOTIMPL;
	}

	STDMETHOD(OnPosRectChange)(LPCRECT lprcPosRect)	{
		return E_NOTIMPL;
	}

	bool inPlaceActive_;
};


}

#endif