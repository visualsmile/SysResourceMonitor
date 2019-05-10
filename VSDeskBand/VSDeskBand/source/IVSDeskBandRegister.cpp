#include <ShObjIdl.h>
#include <ComCat.h>
#include <ShlGuid.h>
#include <strsafe.h>
#include "Def/VSDeskBandGlobalDef.h"
#include "VSDeskBandFactory.h"

HINSTANCE g_hDeskBandGlobalInstanceModule = nullptr;    // dll单例 
ULONG g_l_DllInstanceRef = 0;                           // dll全局引用，为0时dll会被系统释放

// dllmain.cpp : 定义 DLL 应用程序的入口点。
BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved)
{
    g_hDeskBandGlobalInstanceModule = hModule;
    UNREFERENCED_PARAMETER(lpReserved);
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

HRESULT myRegisterServer();
HRESULT myUnRegisterServer();
HRESULT myCanUnloadNow();
HRESULT myGetClassObject(__in REFCLSID rclsid, __in REFIID riid, LPVOID FAR* ppv);
void updatepTrayDeskBandMenu();

// 注册，将该COM组件的UUID、文件路径写入注册表中
STDAPI DllRegisterServer() 
{
    if (myRegisterServer() != S_OK)
    {
        MessageBox(nullptr, L"注册表填写错误", L"Error", S_OK);
        return S_FALSE;
    }

    // 注册band类型
    ICatRegister  *pcr;
    HRESULT        hr = S_OK;
    hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr, NULL, CLSCTX_INPROC_SERVER, IID_ICatRegister, (LPVOID*)&pcr);
    if (SUCCEEDED(hr))
    {
        CATID catid[] = { CATID_DeskBand };
        hr = pcr->RegisterClassImplCategories(CLSID_VSDeskBandCOM, 1, catid);
        pcr->Release();
    }

    // 要求刷新菜单
    updatepTrayDeskBandMenu();

    return S_OK;
}

// 反注册
STDAPI DllUnregisterServer()
{
    if (myUnRegisterServer() != S_OK)
    {
        return S_FALSE;
    }

    // 要求刷新菜单
    updatepTrayDeskBandMenu();
    return S_OK;
}

//用于创建类厂并返回所需接口，由CoGetClassObject函数调用  
STDAPI DllGetClassObject(__in REFCLSID rclsid, __in REFIID riid, LPVOID FAR* ppv)
{
    return myGetClassObject(rclsid, riid, ppv);
}

//用于判断是否可以卸载本组件, 由CoFreeUnusedLibraries函数调用，一般被系统调用
STDAPI DllCanUnloadNow()
{
    return myCanUnloadNow();
}


// 具体定义
HRESULT myRegisterServer()
{
    // 分为三步 

	// 1. 拿到dll文件路径
    WCHAR lsFullFilePath[MAX_PATH] = { 0 };
    DWORD dwResult = GetModuleFileName(g_hDeskBandGlobalInstanceModule, lsFullFilePath, MAX_PATH); //获取本组件(dll)所在路径
    if (0 == dwResult)
    {
        MessageBox(NULL, L"无法获取当前dll所在目录", L"注册失败", MB_OK);
        return S_FALSE;
    }

	// 2. 将CLSID + 路径写入注册表里面
    HKEY regkeyCLSID, regkeyDeskBandCOM_GUID, regkeyInProcServer32;
    // 打开键注册表 HKEY_CLASSES_ROOT\CLSID,  
    if (ERROR_SUCCESS == RegOpenKey(HKEY_CLASSES_ROOT, L"CLSID", &regkeyCLSID))
    {
        // 新键: VSCom的CLSID
        if (ERROR_SUCCESS == RegCreateKey(regkeyCLSID, lsCLSID_VSDeskBandCOM_GUID, &regkeyDeskBandCOM_GUID))
        {
            RegSetValue(regkeyDeskBandCOM_GUID, NULL, REG_SZ, lsVSDeskBandMenuName, DWORD(wcslen(lsVSDeskBandMenuName) * 2));
            // 新建: InProcServer32
            if (ERROR_SUCCESS == RegCreateKey(regkeyDeskBandCOM_GUID, L"InProcServer32", &regkeyInProcServer32))
            {
                // 将dll文件路径写为该键的默认值  
                RegSetValue(regkeyInProcServer32, NULL, REG_SZ, lsFullFilePath, DWORD(wcslen(lsFullFilePath) * 2));
                // RegSetValue只能改默认值，所以用RegSetValueEx
                WCHAR const szModel[] = L"Apartment";
                RegSetValueEx(regkeyInProcServer32, L"ThreadingModel", 0, REG_SZ, LPBYTE(szModel), sizeof(szModel));
                RegCloseKey(regkeyInProcServer32);
            }
            RegCloseKey(regkeyDeskBandCOM_GUID);
        }
        RegCloseKey(regkeyCLSID);
    }

	// 3. 将ObjectName + CLSID（可选）注册进去
    HKEY regkeyMyComObject, regkeyMyComObjectCLISID;
    //在HKEY_CLASSES_ROOT 下创建 CLSID
    if (ERROR_SUCCESS == RegCreateKey(HKEY_CLASSES_ROOT, lsVSDeskBandCOM_Name, &regkeyMyComObject))
    {
        RegSetValue(regkeyMyComObject, NULL, REG_SZ, lsVSDeskBandCOM_Name, DWORD(wcslen(lsVSDeskBandCOM_Name) * 2));
        // 创建子键 CLSID
        if (ERROR_SUCCESS == RegCreateKey(regkeyMyComObject, L"CLSID", &regkeyMyComObjectCLISID))
        {
            // 将MyCom的CLSID写为该键的默认值  
            RegSetValue(regkeyMyComObjectCLISID, NULL, REG_SZ, lsCLSID_VSDeskBandCOM_GUID, DWORD(wcslen(lsCLSID_VSDeskBandCOM_GUID) * 2));
            RegCloseKey(regkeyMyComObjectCLISID);
        }
        RegCloseKey(regkeyMyComObject);
    }

    //这样的话一个客户端程序如果想要使用本组件，首先可以以MyCom.ICustomInf为参数调用CLSIDFromProgID函数  
    //来获取MyCom.ICustomInf的CLSID，再以这个CLSID为参数调用CoCreateInstance创建COM对象  
    // MessageBox(NULL, lsFullFilePath, L"注册成功，已将以下路径写入注册表", MB_OK);
    return S_OK;
}

HRESULT myUnRegisterServer()
{
	// 先关闭任务栏显示
	ITrayDeskBand* pTrayDeskBand = NULL;
	HRESULT hr = CoCreateInstance(CLSID_TrayDeskBand, NULL, CLSCTX_ALL, IID_PPV_ARGS(&pTrayDeskBand));
	// Vista and higher operating system
	if (SUCCEEDED(hr))
	{
		int nRetryTimes = 5;
		do
		{
			hr = pTrayDeskBand->HideDeskBand(CLSID_VSDeskBandCOM);
		} while (--nRetryTimes && !SUCCEEDED(hr));

		if (pTrayDeskBand)
			pTrayDeskBand->Release();
	}	

    // 然后要求释放dll
    // Find handle to the task bar.
    HWND hTaskbarWnd = FindWindow(L"Shell_TrayWnd", NULL);
    // If task bar receives this message, it will call CoFreeUnusedLibraries function
    // immediately to free unused libraries.
    PostMessage(hTaskbarWnd, WM_TIMER, 24, 0);


    // 最后清理注册表信息
	// 1. 清理CLSID + 文件路径
    HKEY regkeyCLSID;
    if (ERROR_SUCCESS == RegOpenKey(HKEY_CLASSES_ROOT, L"CLSID", &regkeyCLSID))
    {
        // RegDeleteKey不能删除含有子项的节点 所以删除的东西比较多。直接使用RegDeleteTree, 小心使用，别把注册表全删了     
        RegDeleteTree(regkeyCLSID, lsCLSID_VSDeskBandCOM_GUID);

        // 删除VSDeskBandCOM_CLSID
        //deleteKey(regkeyCLSID, lsCLSID_VSDeskBandCOM_GUID);
        RegCloseKey(regkeyCLSID);
    }

	// 2. 清理ObjectName + CLSID
    RegDeleteTree(HKEY_CLASSES_ROOT, lsVSDeskBandCOM_Name);  

    return S_OK;
}

void updatepTrayDeskBandMenu()
{
    // 以下代码就是让Toolbars子菜单项中立即出现我们注册的Band对象菜单项
    ITrayDeskBand *pTrayDeskBand = NULL;
    HRESULT hr = ::CoCreateInstance(CLSID_TrayDeskBand, NULL, CLSCTX_ALL, IID_PPV_ARGS(&pTrayDeskBand));
    if (SUCCEEDED(hr))
    {
        pTrayDeskBand->DeskBandRegistrationChanged();
        pTrayDeskBand->Release();
    }        

    if (FAILED(hr))
    {
        MessageBox(nullptr, L"立即显示DeskBand失败", L"Error", S_OK);
    }
}

HRESULT myGetClassObject(__in REFCLSID rclsid, __in REFIID riid, LPVOID FAR* ppv)
{
    if (CLSID_VSDeskBandCOM == rclsid)
    {
        VSDeskBandFactory* pFactory = new VSDeskBandFactory();//创建类厂对象
        if (NULL == pFactory)
        {
            return E_OUTOFMEMORY;
        }

        HRESULT oResult = pFactory->QueryInterface(riid, ppv);
        pFactory->Release();

        return oResult; //获取所需接口
    }
    else
    {
        return CLASS_E_CLASSNOTAVAILABLE;
    }
}

HRESULT myCanUnloadNow()
{
    //如果对象个数为0，则可以卸载
    if (0 == g_l_DllInstanceRef)
    {
        return S_OK;
    }
    else
    {
        return S_FALSE;
    }
}
