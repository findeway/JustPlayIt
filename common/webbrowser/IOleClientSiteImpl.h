#ifndef IM_IOLECLIENTSITEIMPL_H_
#define IM_IOLECLIENTSITEIMPL_H_

namespace shcontrol
{

	class IOleClientSiteImpl : public IOleClientSite 
	{
	public:
		STDMETHOD(SaveObject)(void)	
		{
			return E_NOTIMPL;
		}

		STDMETHOD(GetMoniker)(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker **ppmk)
		{
			return E_NOTIMPL;
		}

		STDMETHOD(GetContainer)(IOleContainer **ppContainer)
		{
			return E_NOTIMPL;
		}

		STDMETHOD(ShowObject)(void)
		{
			return E_NOTIMPL;
		}

		STDMETHOD(OnShowWindow)(BOOL fShow)
		{
			return E_NOTIMPL;
		}

		STDMETHOD(RequestNewObjectLayout)(void)
		{
			return E_NOTIMPL;
		}
	};

}
#endif