#ifndef _IWEBBROWSEREVENTIMPL_H
#define _IWEBBROWSEREVENTIMPL_H

#include <exdispid.h>
namespace pccontrol
{

	//SINK_ENTRY_EX(ctlID,DIID_DWebBrowserEvents2,DISPID_FILEDOWNLOAD,OnFileDownload) 
#define DWEBBROWSEREVENTS2_SINK_ENTRY(ctlID) \
	SINK_ENTRY_EX(ctlID,DIID_DWebBrowserEvents2,DISPID_STATUSTEXTCHANGE,OnStatusTextChange) \
	SINK_ENTRY_EX(ctlID,DIID_DWebBrowserEvents2,DISPID_DOWNLOADCOMPLETE,OnDownloadComplete) \
	SINK_ENTRY_EX(ctlID,DIID_DWebBrowserEvents2,DISPID_COMMANDSTATECHANGE,OnCommandStateChange) \
	SINK_ENTRY_EX(ctlID,DIID_DWebBrowserEvents2,DISPID_DOWNLOADBEGIN,OnDownloadBegin) \
	SINK_ENTRY_EX(ctlID,DIID_DWebBrowserEvents2,DISPID_PROGRESSCHANGE,OnProgressChange) \
	SINK_ENTRY_EX(ctlID,DIID_DWebBrowserEvents2,DISPID_PROPERTYCHANGE,OnPropertyChange) \
	SINK_ENTRY_EX(ctlID,DIID_DWebBrowserEvents2,DISPID_TITLECHANGE,OnTitleChange) \
	SINK_ENTRY_EX(ctlID,DIID_DWebBrowserEvents2,DISPID_PRINTTEMPLATEINSTANTIATION,OnPrintTemplateInstantiation) \
	SINK_ENTRY_EX(ctlID,DIID_DWebBrowserEvents2,DISPID_PRINTTEMPLATETEARDOWN,OnPrintTemplateTeardown) \
	SINK_ENTRY_EX(ctlID,DIID_DWebBrowserEvents2,DISPID_UPDATEPAGESTATUS,OnUpdatePageStatus) \
	SINK_ENTRY_EX(ctlID,DIID_DWebBrowserEvents2,DISPID_BEFORENAVIGATE2,OnBeforeNavigate2) \
	SINK_ENTRY_EX(ctlID,DIID_DWebBrowserEvents2,DISPID_NEWWINDOW2,OnNewWindow2) \
	SINK_ENTRY_EX(ctlID,DIID_DWebBrowserEvents2,DISPID_NEWWINDOW3,OnNewWindow3) \
	SINK_ENTRY_EX(ctlID,DIID_DWebBrowserEvents2,DISPID_NAVIGATECOMPLETE,OnNavigateComplete) \
	SINK_ENTRY_EX(ctlID,DIID_DWebBrowserEvents2,DISPID_NAVIGATECOMPLETE2,OnNavigateComplete2) \
	SINK_ENTRY_EX(ctlID,DIID_DWebBrowserEvents2,DISPID_QUIT,OnQuit) \
	SINK_ENTRY_EX(ctlID,DIID_DWebBrowserEvents2,DISPID_DOCUMENTCOMPLETE,OnDocumentComplete) \
	SINK_ENTRY_EX(ctlID,DIID_DWebBrowserEvents2,DISPID_WINDOWCLOSING,OnWindowClosing) \
	SINK_ENTRY_EX(ctlID,DIID_DWebBrowserEvents2,DISPID_CLIENTTOHOSTWINDOW,OnClientToHostWindow) \
	SINK_ENTRY_EX(ctlID,DIID_DWebBrowserEvents2,DISPID_SETSECURELOCKICON,OnSetSecureLockIcon) \
	SINK_ENTRY_EX(ctlID,DIID_DWebBrowserEvents2,DISPID_NAVIGATEERROR,OnNavigateError) \
	SINK_ENTRY_EX(ctlID,DIID_DWebBrowserEvents2,DISPID_PRIVACYIMPACTEDSTATECHANGE,OnPrivacyImpactedStateChange) 


	
}









/*
class IWebBrowserEventImpl
{
public:
	virtual void STDMETHODCALLTYPE OnStatusTextChange( BSTR Text ){return ;}
	virtual void STDMETHODCALLTYPE  OnDownloadComplete(){return ;}
    virtual void STDMETHODCALLTYPE  OnCommandStateChange( long Command, BOOL Enable){return ;}
    virtual void STDMETHODCALLTYPE  OnDownloadBegin(){return ;}
	virtual void STDMETHODCALLTYPE  OnProgressChange( long x, long y ){return ;}
    virtual void STDMETHODCALLTYPE  OnPropertyChange( LPCSTR szProperty ){return ;}
    virtual void STDMETHODCALLTYPE  OnTitleChange( LPCSTR Text){return ;}

    virtual void STDMETHODCALLTYPE  OnPrintTemplateInstantiation( IDispatch* pDisp){return ;}
    virtual void STDMETHODCALLTYPE  OnPrintTemplateTeardown( IDispatch* pDisp ){return ;}
    virtual void STDMETHODCALLTYPE  OnUpdatePageStatus(
                    IDispatch* pDisp,
                    VARIANT* nPage,
                    VARIANT* fDone) {return ;}
    virtual void STDMETHODCALLTYPE  OnBeforeNavigate2(
                    IDispatch* pDisp,
                    VARIANT* URL,
                    VARIANT* Flags,
                    VARIANT* TargetFrameName,
                    VARIANT* PostData,
                    VARIANT* Headers,
                    BOOL* Cancel ){return ;}
    virtual void STDMETHODCALLTYPE  OnNewWindow2(
                    IDispatch** ppDisp,
                    BOOL* Cancel){return ;}
	virtual void STDMETHODCALLTYPE  OnNavigateComplete( IDispatch* dispatchPtr, VARIANT* URL ){return ;}
    virtual void STDMETHODCALLTYPE  OnQuit(){return ;}
    virtual void STDMETHODCALLTYPE  OnDocumentComplete(
                    IDispatch* pDisp,
                    VARIANT* URL) {return ;}
    virtual void STDMETHODCALLTYPE  OnWindowClosing( BOOL IsChildWindow, BOOL* Cancel ){return ;}
    virtual void STDMETHODCALLTYPE  OnClientToHostWindow( long* CX, long* CY ){return ;}
    virtual void STDMETHODCALLTYPE  OnSetSecureLockIcon( long SecureLockIcon ){return ;}
    virtual void STDMETHODCALLTYPE  OnFileDownload( VARIANT_BOOL* Cancel ){return ;}
    virtual void STDMETHODCALLTYPE  OnNavigateError(
                    IDispatch* pDisp,
                    VARIANT* URL,
                    VARIANT* Frame,
                    VARIANT* StatusCode,
                    BOOL* Cancel){return ;}
    virtual void STDMETHODCALLTYPE OnPrivacyImpactedStateChange( BOOL bImpacted ) {return ;}



};
*/

#endif