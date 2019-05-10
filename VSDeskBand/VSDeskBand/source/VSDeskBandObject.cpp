#include "Def/VSDeskBandGlobalDef.h"
#include "VSDeskBandObject.h"
#include "window/VSDeskBandWindow.h"

extern HMODULE g_hDeskBandGlobalInstanceModule;
extern ULONG g_l_DllInstanceRef;

VSDeskBandObject::VSDeskBandObject()
{
    m_lRef = 1;
    m_pInputObjSite = nullptr;
    m_pDeskBandWindowInf = new VSDeskBandWindow();
    InterlockedIncrement(&g_l_DllInstanceRef);
}

VSDeskBandObject::~VSDeskBandObject()
{
    if (m_pDeskBandWindowInf)
        delete m_pDeskBandWindowInf;
    InterlockedDecrement(&g_l_DllInstanceRef);
}

HRESULT STDMETHODCALLTYPE VSDeskBandObject::QueryInterface(const IID& riid, void** ppvObject)
{
    if (riid == IID_VSDeskBandObject)
    {
        *ppvObject = static_cast<VSDeskBandObject*>(this);
    }
    else if (riid == IID_IDeskBand)
    {
        *ppvObject = static_cast<IDeskBand*>(this);
    }
    else if (riid == IID_IDeskBand2)
    {
        *ppvObject = static_cast<IDeskBand2*>(this);
    }
    else if (riid == IID_IDockingWindow)
    {
        *ppvObject = static_cast<IDockingWindow*>(this);
    }
    else if (riid == IID_IPersist)
    {
        *ppvObject = static_cast<IPersist*>(this);
    }
    else if (riid == IID_IPersistStream)
    {
        *ppvObject = static_cast<IPersistStream*>(this);
    }
    else if (riid == IID_IInputObject)
    {
        *ppvObject = static_cast<IInputObject*>(this);
    }
    else if (riid == IID_IOleWindow)
    {
        *ppvObject = static_cast<IOleWindow*>(this);
    }
    else if (riid == IID_IObjectWithSite)
    {
        *ppvObject = static_cast<IObjectWithSite*>(this);
    }
    else
    {
        *ppvObject = nullptr;
        return E_NOINTERFACE;
    }

    static_cast<IUnknown*>(*ppvObject)->AddRef();
    return S_OK;
}

STDMETHODIMP_(ULONG)VSDeskBandObject::AddRef()
{
    return InterlockedIncrement(&m_lRef);
}

STDMETHODIMP_(ULONG)VSDeskBandObject::Release()
{
    ULONG lRef = InterlockedDecrement(&m_lRef);
    if (0 == lRef)
    {
        delete this;
    }

    return lRef;
}

STDMETHODIMP VSDeskBandObject::GetWindow(HWND *phwnd)
{
    if (NULL == phwnd)
    {
        return E_INVALIDARG;
    }

    *phwnd = m_pDeskBandWindowInf->getDeskBandWindowHandle();

    return S_OK;
}

STDMETHODIMP VSDeskBandObject::ContextSensitiveHelp(BOOL fEnterMode)
{
    UNREFERENCED_PARAMETER(fEnterMode);
    return E_NOTIMPL;
}

STDMETHODIMP VSDeskBandObject::ShowDW(BOOL fShow)
{
    if (m_pDeskBandWindowInf->showDeskBandWindow(fShow))
        return S_OK;
    else
        return S_FALSE;
}

STDMETHODIMP VSDeskBandObject::CloseDW(DWORD dwReserved)
{
    UNREFERENCED_PARAMETER(dwReserved);
    if (m_pDeskBandWindowInf->closeDeskBandWindow())
        return S_OK;
    else 
        return S_FALSE;
}

STDMETHODIMP VSDeskBandObject::ResizeBorderDW(LPCRECT prcBorder, IUnknown *punkToolbarSite, BOOL fReserved)
{
    UNREFERENCED_PARAMETER(prcBorder);
    UNREFERENCED_PARAMETER(punkToolbarSite);
    UNREFERENCED_PARAMETER(fReserved);

    return E_NOTIMPL;
}

STDMETHODIMP VSDeskBandObject::GetBandInfo(DWORD dwBandID, DWORD dwViewMode, DESKBANDINFO *pdbi)
{
    if (NULL == pdbi)
    {
        return E_INVALIDARG;
    }

    m_dwBandID = dwBandID;
    m_dwViewMode = dwViewMode;

    SIZE deskBandSize = m_pDeskBandWindowInf->getDeskBandWindowMinSize((DBIF_VIEWMODE_VERTICAL == dwViewMode) ? FALSE : TRUE);

    POINTL ptMinSize = { deskBandSize.cx, deskBandSize.cy };
    POINTL ptActual = { deskBandSize.cx, deskBandSize.cy };

    if (pdbi->dwMask & DBIM_MINSIZE)
    {
        pdbi->ptMinSize = ptMinSize;
    }

    if (pdbi->dwMask & DBIM_ACTUAL)
    {
        pdbi->ptActual = ptActual;
    }

    if (pdbi->dwMask & DBIM_MAXSIZE)
    {
        // There is no limit for the maximum height.
        pdbi->ptMaxSize.y = -1;
    }

    if (pdbi->dwMask & DBIM_INTEGRAL)
    {
        // The sizing step value of the band object.
        pdbi->ptIntegral.y = 1;
    }

    if (pdbi->dwMask & DBIM_TITLE)
    {
        // Do NOT show the title by removing the flag.
        pdbi->dwMask &= ~DBIM_TITLE;
    }

    if (pdbi->dwMask & DBIM_MODEFLAGS)
    {
        pdbi->dwModeFlags = DBIMF_NORMAL | DBIMF_VARIABLEHEIGHT;
    }

    if (pdbi->dwMask & DBIM_BKCOLOR)
    {
        // Use the default background color by removing this flag.
        pdbi->dwMask &= ~DBIM_BKCOLOR;
    }

    return S_OK;
}

STDMETHODIMP VSDeskBandObject::CanRenderComposited(BOOL *pfCanRenderComposited)
{
    if (NULL == pfCanRenderComposited)
    {
        return E_INVALIDARG;
    }

    *pfCanRenderComposited = TRUE;
    return S_OK;
}

STDMETHODIMP VSDeskBandObject::SetCompositionState(BOOL fCompositionEnabled)
{
    m_fCompositionEnabled = fCompositionEnabled;
    m_pDeskBandWindowInf->updateDeskBandWindow();
    return S_OK;
}

STDMETHODIMP VSDeskBandObject::GetCompositionState(BOOL *pfCompositionEnabled)
{
    if (NULL == pfCompositionEnabled)
    {
        return E_INVALIDARG;
    }

    *pfCompositionEnabled = m_fCompositionEnabled;
    return S_OK;
}

STDMETHODIMP VSDeskBandObject::GetClassID(CLSID *pclsid)
{
    if (NULL == pclsid)
    {
        return E_INVALIDARG;
    }

    *pclsid = CLSID_VSDeskBandCOM;
    return S_OK;
}

STDMETHODIMP VSDeskBandObject::IsDirty()
{
    return (TRUE == m_fIsDirty) ? S_OK : S_FALSE;
}

STDMETHODIMP VSDeskBandObject::Load(IStream *pStm)
{
    UNREFERENCED_PARAMETER(pStm);
    return E_NOTIMPL;
}

STDMETHODIMP VSDeskBandObject::Save(IStream *pStm, BOOL fClearDirty)
{
    UNREFERENCED_PARAMETER(pStm);
    if (TRUE == fClearDirty)
    {
        m_fIsDirty = FALSE;
    }

    return E_NOTIMPL;
}

STDMETHODIMP VSDeskBandObject::GetSizeMax(ULARGE_INTEGER *pcbSize)
{
    UNREFERENCED_PARAMETER(pcbSize);

    return E_NOTIMPL;
}

STDMETHODIMP VSDeskBandObject::SetSite(IUnknown *pUnkSite)
{
    //If a site is being held, release it.
    if (m_pInputObjSite)
    {
        m_pInputObjSite->Release();
        m_pInputObjSite = NULL;
    }

    //If punkSite is not NULL, a new site is being set.
    if (pUnkSite)
    {
        //Get the parent window.
        HWND hParentWnd = NULL;
        IOleWindow  *pOleWindow;
        if (SUCCEEDED(pUnkSite->QueryInterface(IID_IOleWindow, (LPVOID*)&pOleWindow)))
        {

            pOleWindow->GetWindow(&hParentWnd);
            pOleWindow->Release();
        }

        if (!hParentWnd)
            return E_FAIL;

        if (!m_pDeskBandWindowInf->createNewDeskBandWindow(hParentWnd, g_hDeskBandGlobalInstanceModule))
            return E_FAIL;

        //Get and keep the IInputObjectSite pointer.
        if (SUCCEEDED(pUnkSite->QueryInterface(IID_IInputObjectSite, (LPVOID*)&m_pInputObjSite)))
        {
            return S_OK;
        }

        return E_FAIL;
    }

    return S_OK;
}

STDMETHODIMP VSDeskBandObject::GetSite(REFIID riid, void **ppv)
{
    *ppv = NULL;

    if (m_pInputObjSite)
        return m_pInputObjSite->QueryInterface(riid, ppv);

    return E_FAIL;
}

STDMETHODIMP VSDeskBandObject::UIActivateIO(BOOL fActivate, MSG *pMsg)
{
    UNREFERENCED_PARAMETER(pMsg);

    if (TRUE == fActivate)
    {
        SetFocus(m_pDeskBandWindowInf->getDeskBandWindowHandle());
    }

    return S_OK;
}

STDMETHODIMP VSDeskBandObject::HasFocusIO()
{
    if (m_fHasFocus)
        return S_OK;
    else
        return S_FALSE;
}

STDMETHODIMP VSDeskBandObject::TranslateAcceleratorIO(MSG *pMsg)
{
    UNREFERENCED_PARAMETER(pMsg);
    return S_FALSE;
}

IVSDeskBandWindowInf* VSDeskBandObject::getDeskBandWindowInf()
{
    return m_pDeskBandWindowInf;
}


