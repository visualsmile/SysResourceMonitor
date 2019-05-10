#include "VSDeskBandOperation.h"
#include <windows.h>
#include "ShObjIdl.h"
#include "Shlwapi.h"
#include "Def/VSDeskBandGlobalDef.h"

void VSDeskBandOperation::autoClickOk()
{
	int nRetryTimes = 1000;
	while (nRetryTimes--)
	{
		if (findaConfirmWindow())
			break;
	}
}

namespace VSDeskBandOperationWindowFind
{
	static bool s_bFound = false;
	static HWND s_hTargetWindow = nullptr;
	static LPCTSTR s_pFindWindowClassText = L"";
	static LPCTSTR s_pFindWindowTitleText = L"";

	static BOOL compareWidow(HWND hCur, LPARAM lParam)
	{
		if (s_bFound)
			return FALSE;

		TCHAR szResult[MAX_PATH] = { 0 };
		if (IsWindow(hCur))
		{
			GetClassName(hCur, szResult, MAX_PATH);
			if (0 == lstrcmp(szResult, s_pFindWindowClassText))
			{
				GetWindowText(hCur, szResult, MAX_PATH);
				if (0 == lstrcmp(szResult, s_pFindWindowTitleText))
				{
					s_hTargetWindow = hCur;
					s_bFound = true;
					return FALSE;
				}
			}

			EnumChildWindows(hCur, compareWidow, lParam);
		}
		return TRUE;
	}

	static HWND FindWindow(LPCTSTR pClassName, LPCTSTR pWindowTitleName)
	{
		s_bFound = false;
		s_pFindWindowClassText = pClassName;
		s_pFindWindowTitleText = pWindowTitleName;
		EnumWindows(VSDeskBandOperationWindowFind::compareWidow, NULL);
		return s_hTargetWindow;
	}
}

bool VSDeskBandOperation::findaConfirmWindow()
{
	// 先找到Explorer生成的VSDeskBand窗体
	HWND hTargetWindow = VSDeskBandOperationWindowFind::FindWindow(L"#32770", lsVSDeskBandMenuName);
	if (hTargetWindow)
	{
		HWND hAffirmWindow = FindWindowEx(hTargetWindow, NULL, L"DirectUIHWND", NULL);
		if (hAffirmWindow)
		{
			HWND hSink = nullptr;
			int nRetryTimes = 1000;
			do
			{
				hSink = FindWindowEx(hAffirmWindow, hSink, L"CtrlNotifySink", NULL);
				if (hSink)
				{
					HWND hButton = FindWindowEx(hSink, NULL, L"Button", NULL);
					if (hButton)
					{
						WCHAR lsCaption[MAX_PATH] = { 0 };
						GetWindowText(hButton, lsCaption, MAX_PATH);
						if (0 == lstrcmp(lsCaption, L"是(&Y)"))
						{
							SendMessage(hSink, WM_COMMAND, BN_CLICKED, (LPARAM)hButton);
							ShowWindow(hAffirmWindow, SW_HIDE);
							return true;
						}
					}
				}
			} while (hSink != nullptr && nRetryTimes--);
		}
	}
	return false;
}

VSDeskBandOperation::VSDeskBandOperation()
{
	CoInitialize(NULL);
	initSharedMemory();
}

void VSDeskBandOperation::initSharedMemory()
{
	m_hMapFile = ::CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, c_nSharedMemSize, c_lsSRMShareMemoryName);
	m_pSharedBuffer = ::MapViewOfFile(m_hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	m_hWriteSemaphore = CreateSemaphore(nullptr, 1, 1, c_lsSRMMutexSemaphoreName);
}

void VSDeskBandOperation::freeSharedMemory()
{
	VSSharedMemStruct oSharedMemStructObject;
	oSharedMemStructObject.setMessageID(0);
	WaitForSingleObject(m_hWriteSemaphore, INFINITE); // （信号量句柄，等待时间）
	oSharedMemStructObject.writeToStream(m_pSharedBuffer);
	ReleaseSemaphore(m_hWriteSemaphore, 1, NULL); // （信号量句柄，释放的数量，out释放之前资源数量）

	UnmapViewOfFile(m_pSharedBuffer);
	CloseHandle(m_hMapFile);
	CloseHandle(m_hWriteSemaphore);
}

VSDeskBandOperation::~VSDeskBandOperation()
{
	freeSharedMemory();
	CoUninitialize();
}

IVSDeskBandOperation* VSDeskBandOperation::getInstance()
{
	static VSDeskBandOperation s_oVSDeskBandOperation;
	return &s_oVSDeskBandOperation;
}


bool VSDeskBandOperation::isRegistered()
{
	WCHAR lsFullFilePath[MAX_PATH] = { 0 };
	DWORD dwDataSize = 0;
	HKEY regkeyCLSID, regkeyDeskBandCOM_GUID, regkeyInProcServer32;
	if (ERROR_SUCCESS == RegOpenKey(HKEY_CLASSES_ROOT, L"CLSID", &regkeyCLSID))
	{
		if (ERROR_SUCCESS == RegOpenKey(regkeyCLSID, lsCLSID_VSDeskBandCOM_GUID, &regkeyDeskBandCOM_GUID))
		{
			if (ERROR_SUCCESS == RegOpenKey(regkeyDeskBandCOM_GUID, L"InProcServer32", &regkeyInProcServer32))
			{
				// 先读size，然后把size传进去读value；坑爹的win api
				RegGetValue(regkeyInProcServer32, NULL, NULL, REG_SZ, NULL, NULL, &dwDataSize);
				RegGetValue(regkeyInProcServer32, NULL, NULL, REG_SZ, NULL, lsFullFilePath, &dwDataSize);
				RegCloseKey(regkeyInProcServer32);
			}
			RegCloseKey(regkeyDeskBandCOM_GUID);
		}
		RegCloseKey(regkeyCLSID);
	}

	if (PathFileExists(lsFullFilePath))
		return true;
	else
		return false;
}

bool VSDeskBandOperation::doReg()
{
	HINSTANCE hRegister = ShellExecute(0, L"runas", L"VSDeskBand_0_注册.bat", NULL, NULL, SW_HIDE);
	if ((__int64)hRegister < 32)
	{
		MessageBox(NULL, L"无法注册任务栏标尺组件。\n请重试 或手动执行 VSDeskBand_0_注册.bat以完成注册。", L"ERROR", MB_OK);
		return false;
	}
	WaitForSingleObject(hRegister, -1);
	Sleep(1000);
	return true;
}

bool VSDeskBandOperation::doUnreg()
{
	hide();
	HINSTANCE hRegister = ShellExecute(0, L"runas", L"VSDeskBand_1_注销.bat", NULL, NULL, SW_HIDE);
	if ((__int64)hRegister < 32)
	{
		MessageBox(NULL, L"无法注销任务栏标尺组件。\n请重试 或手动执行 VSDeskBand_1_注销.bat以完成注销。", L"ERROR", MB_OK);
		return false;
	}
	WaitForSingleObject(hRegister, -1);
	Sleep(1000);
	return true;
}

bool VSDeskBandOperation::visible()
{
	ITrayDeskBand* pTrayDeskBand = NULL;
	HRESULT hr = CoCreateInstance(CLSID_TrayDeskBand, NULL, CLSCTX_ALL, IID_PPV_ARGS(&pTrayDeskBand));
	// Vista and higher operating system
	if (!SUCCEEDED(hr))
		return false;

	int nRetryTimes = 1000;
	do 
	{
		hr = pTrayDeskBand->IsDeskBandShown(CLSID_VSDeskBandCOM);
	} while (--nRetryTimes && !SUCCEEDED(hr));

	if (pTrayDeskBand)
		pTrayDeskBand->Release();

	return S_OK == hr;
}

bool VSDeskBandOperation::show(bool bAutoClick /*=true*/)
{
	if (!isRegistered())
		doReg();

	ITrayDeskBand* pTrayDeskBand = NULL;
	HRESULT hr = CoCreateInstance(CLSID_TrayDeskBand, NULL, CLSCTX_ALL, IID_PPV_ARGS(&pTrayDeskBand));
	// Vista and higher operating system
	if (!SUCCEEDED(hr))
		return false;

	int nRetryTimes = 1000;
	do
	{
		hr = pTrayDeskBand->ShowDeskBand(CLSID_VSDeskBandCOM);
	} while (--nRetryTimes && !SUCCEEDED(hr));

	if (pTrayDeskBand)
		pTrayDeskBand->Release();

	if (S_OK == hr && bAutoClick)
		autoClickOk();

	return S_OK == hr;
}

bool VSDeskBandOperation::hide()
{
	ITrayDeskBand* pTrayDeskBand = NULL;
	HRESULT hr = CoCreateInstance(CLSID_TrayDeskBand, NULL, CLSCTX_ALL, IID_PPV_ARGS(&pTrayDeskBand));
	// Vista and higher operating system
	if (!SUCCEEDED(hr))
		return false;

	int nRetryTimes = 1000;
	do
	{
		hr = pTrayDeskBand->HideDeskBand(CLSID_VSDeskBandCOM);
	} while (--nRetryTimes && !SUCCEEDED(hr));

	if (pTrayDeskBand)
		pTrayDeskBand->Release();

	return S_OK == hr;
}

bool VSDeskBandOperation::updateMessage(VSSharedMemStruct* pMessageArray)
{
	static int s_nID = 0;
	s_nID++;

	//if (pMessageArray->colCount() == 0)
	//{
	//	pMessageArray->reset();
	//	pMessageArray->setRowCount(1);
	//	pMessageArray->setColCount(1);
	//	VSShareMemTextNode oTextNode(255, 0, 0, wstring(L"来自主程序的空消息"));
	//	pMessageArray->setTextNode(0, 0, oTextNode);
	//}

	pMessageArray->setMessageID(s_nID);
	WaitForSingleObject(m_hWriteSemaphore, INFINITE); // （信号量句柄，等待时间）
	pMessageArray->writeToStream(m_pSharedBuffer);
	ReleaseSemaphore(m_hWriteSemaphore, 1, NULL); // （信号量句柄，释放的数量，out释放之前资源数量）
	return true;
}
