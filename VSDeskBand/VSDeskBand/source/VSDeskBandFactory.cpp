#include "Def/VSDeskBandGlobalDef.h"
#include "VSDeskBandObject.h"
#include "VSDeskBandFactory.h"

extern HMODULE g_hDeskBandGlobalInstanceModule;
extern ULONG g_l_DllInstanceRef;

VSDeskBandFactory::VSDeskBandFactory()
{
    m_lRef = 1;
    InterlockedIncrement(&g_l_DllInstanceRef);
}

VSDeskBandFactory::~VSDeskBandFactory()
{
    InterlockedDecrement(&g_l_DllInstanceRef);
}

STDMETHODIMP VSDeskBandFactory::QueryInterface(const IID& riid, void** ppvObject)
{
    if (IID_IUnknown == riid)
    {
        *ppvObject = static_cast<IUnknown*>(this);
    }
    else if (IID_IClassFactory == riid)
    {
        *ppvObject = (IClassFactory*)this;
    }
    else
    {
        *ppvObject = NULL;
        return E_NOINTERFACE;
    }

    this->AddRef();
    return S_OK;
}

STDMETHODIMP_(ULONG) VSDeskBandFactory::AddRef()
{
    return InterlockedIncrement(&m_lRef);
}

STDMETHODIMP_(ULONG) VSDeskBandFactory::Release()
{
    ULONG lRef = InterlockedDecrement(&m_lRef);
    if (0 == lRef)
    {
        delete this;
    }
    return lRef;
}

STDMETHODIMP VSDeskBandFactory::CreateInstance(IUnknown *pUnkOuter, const IID& riid, void **ppvObject)
{
    HRESULT hr = CLASS_E_NOAGGREGATION;
    if (NULL == pUnkOuter)
    {
        hr = E_OUTOFMEMORY;
        VSDeskBandObject *pObj = new VSDeskBandObject();
        if (NULL != pObj)
        {
            hr = pObj->QueryInterface(riid, ppvObject);
            pObj->Release();
        }         
    }
    return hr;
}

STDMETHODIMP VSDeskBandFactory::LockServer(BOOL bLock)
{
    if (TRUE == bLock)
    {
        // Add the reference count of dll. This is a
        // global variable defined in SdkDeskBandDef.h file.
        InterlockedIncrement(&g_l_DllInstanceRef);
    }
    else
    {
        // Subtract the reference count of dll. This is
        // a global variable defined in SdkDeskBandDef.h file.
        InterlockedDecrement(&g_l_DllInstanceRef);
    }
    return S_OK;
}

