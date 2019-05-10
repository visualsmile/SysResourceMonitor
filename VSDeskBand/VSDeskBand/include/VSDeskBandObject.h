#ifndef VSDeskBandObject_H
#define VSDeskBandObject_H
#define SRM_DESK_BAND_DLL_INTERNAL

/************************************************************************/
/*  DeskBandObject
	com组件的Object；
	通过Com组件拿到的，就是这个Object
	可以从这个object中，访问一些接口
	但是，这个对象是供
/************************************************************************/

#include <Unknwn.h>
#include <ShObjIdl.h>
#include <OCIdl.h>
#include "window/IVSDeskBandWindowInf.h"

class VSDeskBandObject : public IDeskBand2,
    IObjectWithSite,
    IPersistStream,
    IInputObject
{
public:
    VSDeskBandObject();
    ~VSDeskBandObject();

public:
    // IUnknown 
    STDMETHODIMP QueryInterface(const IID& riid, void** ppvObject);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

    // IDeskBand2 : IOleWindow
    STDMETHODIMP GetWindow(HWND *phwnd);
    STDMETHODIMP ContextSensitiveHelp(BOOL fEnterMode);

    // IDeskBand2 : IDockingWindow
    STDMETHODIMP ShowDW(BOOL fShow);
    STDMETHODIMP CloseDW(DWORD dwReserved);
    STDMETHODIMP ResizeBorderDW(LPCRECT prcBorder, IUnknown *punkToolbarSite, BOOL fReserved);

    // IDeskBand2 : IDeskBand (needed for all desk bands)
    STDMETHODIMP GetBandInfo(DWORD dwBandID, DWORD dwViewMode, DESKBANDINFO *pdbi);

    // IDeskBand2 (needed for glass desk band)
    STDMETHODIMP CanRenderComposited(BOOL *pfCanRenderComposited);
    STDMETHODIMP SetCompositionState(BOOL fCompositionEnabled);
    STDMETHODIMP GetCompositionState(BOOL *pfCompositionEnabled);

    // IPersistStream : IPersist
    STDMETHODIMP GetClassID(CLSID *pclsid);

    // IPersistStream
    STDMETHODIMP IsDirty();
    STDMETHODIMP Load(IStream *pStm);
    STDMETHODIMP Save(IStream *pStm, BOOL fClearDirty);
    STDMETHODIMP GetSizeMax(ULARGE_INTEGER *pcbSize);

    // IObjectWithSite
    STDMETHODIMP SetSite(IUnknown *pUnkSite);
    STDMETHODIMP GetSite(REFIID riid, void **ppv);

    // IInputObject
    STDMETHODIMP UIActivateIO(BOOL fActivate, MSG *pMsg);
    STDMETHODIMP HasFocusIO();
    STDMETHODIMP TranslateAcceleratorIO(MSG *pMsg);

    // IDeskBandObject
    IVSDeskBandWindowInf* getDeskBandWindowInf();     // 获取窗口
private:
    ULONG				m_lRef;
    DWORD               m_dwViewMode;           // The view mode of the band object
    DWORD               m_dwBandID;             // The ID of desk band
    BOOL                m_fCompositionEnabled;  // Whether glass is currently enabled in desk band
    BOOL                m_fHasFocus;            // Whether desk band has focus
    BOOL                m_fIsDirty;             // Whether desk band setting has changed
    IInputObjectSite   *m_pInputObjSite;        // Parent site that contains desk band
    IVSDeskBandWindowInf *m_pDeskBandWindowInf;   // 窗口
};



#endif //VSDeskBandObject_H