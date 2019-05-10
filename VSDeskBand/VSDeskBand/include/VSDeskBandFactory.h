#ifndef VSDeskBandFactory_H
#define VSDeskBandFactory_H
/************************************************************************/
/* COM组件工程类
   系统会调用该工厂，来创建对应的object    
   这是com组件必须实现的接口
   */
/************************************************************************/
#include <Unknwn.h>

class VSDeskBandFactory : public IClassFactory
{
public:
    VSDeskBandFactory();
    ~VSDeskBandFactory();

public:
    //IUnknown 
    STDMETHODIMP QueryInterface(const IID& riid, void** ppvObject);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

    //IClassFactory
    STDMETHODIMP CreateInstance(IUnknown *pUnkOuter, const IID& riid, void **ppvObject);
    STDMETHODIMP LockServer(BOOL bLock);

private:
    ULONG m_lRef;
};

#endif //VSDeskBandFactory_H