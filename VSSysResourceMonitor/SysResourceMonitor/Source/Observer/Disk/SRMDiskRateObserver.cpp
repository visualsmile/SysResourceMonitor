#include "Observer\Disk\SRMDiskRateObserver.h"
#include <QMessageBox>
#include <QMenu>
#include <QAction>
#include "SRMScopeOnExit.h"
#include "SRMConfig.h"
#include "Observer\SRMModuleSubject.h"

#define _WIN32_DCOM
#include <comdef.h>
#include <Wbemidl.h>

#pragma comment(lib, "wbemuuid.lib")

const QString c_sDiskTrayMenu = QString::fromStdWString(L"磁盘使用");
const QString c_sDiskTrayAvg = QString::fromStdWString(L"显示平均");
const QString c_sDiskTrayBusiest = QString::fromStdWString(L"显示最忙");

REG_SRM_OBSERVER(SRMDiskRateObserver);
SRMDiskRateObserver::SRMDiskRateObserver()
	: SRMObserverBase("SRMDiskRateObserver")
	, m_pWbemRefresher(nullptr)
	, m_pWbemHiPerfEnum(nullptr)
{
}

SRMDiskRateObserver::~SRMDiskRateObserver()
{
	foreach(SRMDiskRateSubObserver *p, m_oSubObserverHash)
	{
		delete p;
	}

	if (m_pWbemRefresher)
		m_pWbemRefresher->Release();
	if (m_pWbemHiPerfEnum)
		m_pWbemHiPerfEnum->Release();
}



void SRMDiskRateObserver::update(int nIndex, VSSharedMemStruct* pSharedMemStruct, QList<QLabel*>& oLabelList)
{
	updatePartitionInfo();

	foreach(const PartitionInfo & oInfo, m_oPartitionInfoList)
	{
		SRMDiskRateSubObserver* pSubObserver = findSubObserver(oInfo);
		int nChildCheckedCount = pSubObserver->checkedCount();
		if (nChildCheckedCount > 0)
		{
			pSubObserver->updateInfo(oInfo.sPartitionName, oInfo.nUsingRate);
			pSubObserver->update(nIndex, pSharedMemStruct, oLabelList);
			nIndex += nChildCheckedCount;
		}		
	}
}

void SRMDiskRateObserver::regTrayMenu(QMenu* pTrayMenu)
{
	QMenu* pDiskTrayMenu = new QMenu(c_sDiskTrayMenu, pTrayMenu);
	pTrayMenu->addMenu(pDiskTrayMenu);
	//pDiskTrayMenu->setIcon(QIcon(":/Images/DiskTrayMenu.png"));

	foreach(const PartitionInfo & oInfo, m_oPartitionInfoList)
	{
		SRMDiskRateSubObserver* pSubObserver = findSubObserver(oInfo);
		pSubObserver->regTrayMenu(pDiskTrayMenu);
	}
}

int SRMDiskRateObserver::order()
{
	return 65;
}

QString SRMDiskRateObserver::customSettingDescription()
{
	return QString::fromStdWString(L"磁盘使用：");
}

int SRMDiskRateObserver::checkedCount() const
{
	int nCount = 0;
	foreach(SRMDiskRateSubObserver *p, m_oSubObserverHash)
	{
		nCount += p->checkedCount();
	}
	return nCount;
}

QList<ISRMCustomSettingInf*> SRMDiskRateObserver::customSettingInfList()
{
	QList<ISRMCustomSettingInf*> oResultList;
	foreach(const PartitionInfo &oInfo, m_oPartitionInfoList)
	{
		SRMDiskRateSubObserver* pSubObserver = findSubObserver(oInfo);
		oResultList.append(pSubObserver->customSettingInfList());
	}
	return oResultList;
}

void SRMDiskRateObserver::addAvgAndBusiest()
{
	if (m_oPartitionInfoList.size() == 0)
		return;

	// 平均
	int nTotal = 0;
	foreach(const PartitionInfo & oCurInfo, m_oPartitionInfoList)
	{
		nTotal += oCurInfo.nUsingRate;
	}
	PartitionInfo oPartitionInfoAvg;
	oPartitionInfoAvg.sPartitionName = "Avg:";
	oPartitionInfoAvg.sTrayMenuName = c_sDiskTrayAvg;
	oPartitionInfoAvg.nUsingRate = nTotal / m_oPartitionInfoList.size();

	// 最忙
	PartitionInfo oPartitionInfoBusiest = m_oPartitionInfoList[0];
	foreach(const PartitionInfo & oCurInfo, m_oPartitionInfoList)
	{
		if (oCurInfo.nUsingRate > oPartitionInfoBusiest.nUsingRate)
		{
			oPartitionInfoBusiest = oCurInfo;
		}
	}
	oPartitionInfoBusiest.sTrayMenuName = c_sDiskTrayBusiest;

	m_oPartitionInfoList.push_back(oPartitionInfoAvg);
	m_oPartitionInfoList.push_back(oPartitionInfoBusiest);
}

SRMDiskRateSubObserver* SRMDiskRateObserver::findSubObserver(const PartitionInfo& oPartitionInfo)
{
	SRMDiskRateSubObserver* pSubObserver = m_oSubObserverHash.value(oPartitionInfo.sTrayMenuName, nullptr);
	if (!pSubObserver)
	{
		pSubObserver = new SRMDiskRateSubObserver(oPartitionInfo.sPartitionName, oPartitionInfo.sTrayMenuName);
		m_oSubObserverHash.insert(oPartitionInfo.sTrayMenuName, pSubObserver);
	}

	return pSubObserver;
}


bool SRMDiskRateObserver::init()
{
	// Step 1: --------------------------------------------------
	// Initialize COM. ------------------------------------------
	// com组件再main函数中统一初始化

	// Step 2: --------------------------------------------------
	// Set general COM security levels --------------------------
	if (S_OK != CoInitializeSecurity(nullptr,
		-1,
		nullptr,
		nullptr,
		RPC_C_AUTHN_LEVEL_NONE,
		RPC_C_IMP_LEVEL_IMPERSONATE,
		nullptr,
		EOAC_NONE,
		0))
		return false;

	// Step 3: ---------------------------------------------------
	// Obtain the initial locator to WMI -------------------------
	IWbemLocator* pWbemLocator = nullptr;
	SCOPE_EXIT{ if (pWbemLocator) pWbemLocator->Release(); };
	if (S_OK != CoCreateInstance(CLSID_WbemLocator,
		nullptr,
		CLSCTX_INPROC_SERVER, IID_IWbemLocator,
		(void**)& pWbemLocator))
		return false;

	// Step 4: -----------------------------------------------------
	// Connect to WMI through the IWbemLocator::ConnectServer method
	// Connect to the root\cimv2 namespace with
	// the current user and obtain pointer pSvc
	// to make IWbemServices calls.
	IWbemServices* pWbemServices = nullptr;
	SCOPE_EXIT{ if (pWbemServices) pWbemServices->Release(); };
	if (S_OK != pWbemLocator->ConnectServer(L"ROOT\\CIMV2",
		nullptr,
		nullptr,
		nullptr,
		0L,
		nullptr,
		nullptr,
		&pWbemServices))
		return false;

	// Step 5: --------------------------------------------------
	// Set security levels on the proxy -------------------------
	if (S_OK != CoSetProxyBlanket(pWbemServices,							// Indicates the proxy to set
		RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
		RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
		NULL,                        // Server principal name 
		RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
		RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
		NULL,                        // client identity
		EOAC_NONE))                  // proxy capabilities
		return false;

	// Step 6: --------------------------------------------------
	if (S_OK != CoCreateInstance(CLSID_WbemRefresher,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_IWbemRefresher,
		(void**)& m_pWbemRefresher))
		return false;

	IWbemConfigureRefresher* pConfig = nullptr;
	SCOPE_EXIT{ pConfig->Release(); };
	if (S_OK != m_pWbemRefresher->QueryInterface(IID_IWbemConfigureRefresher, (void**)& pConfig))
		return false;

	// Step 7: --------------------------------------------------
	// Add an enumerator to the refresher.
	long nWbemHiPerfEnumID = 0;
	if (S_OK != pConfig->AddEnum(pWbemServices,
		L"Win32_PerfFormattedData_PerfDisk_LogicalDisk",
		0,
		nullptr,
		&m_pWbemHiPerfEnum,
		&nWbemHiPerfEnumID))
		return false;

	// 初始化一下，看看能不能读到数据
	updatePartitionInfo();
	if (m_oPartitionInfoList.size() == 0)
		return false;

	return true;
}

void SRMDiskRateObserver::updatePartitionInfo()
{
	m_oPartitionInfoList.clear();

	// Step 8: --------------------------------------------------
	// Get a property handle for the VirtualBytes property.
	// Refresh the object ten times and retrieve the value.
	if (S_OK != m_pWbemRefresher->Refresh(0L))
		return;

	std::vector<IWbemObjectAccess*> oEnumAccessVec;
	DWORD nResultCount = 0;
	HRESULT hResult = m_pWbemHiPerfEnum->GetObjects(0L, 0, nullptr, &nResultCount);
	if (hResult == WBEM_E_BUFFER_TOO_SMALL)
	{
		if(nResultCount > oEnumAccessVec.size())
			oEnumAccessVec.resize(nResultCount, nullptr);

		if (S_OK != m_pWbemHiPerfEnum->GetObjects(0L, (DWORD)oEnumAccessVec.size(), oEnumAccessVec.data(), &nResultCount))
			return;
	}
	else
		return;
	
	long hNameHandle = 0;
	long hPercentIdleTimeHandle = 0;
	CIMTYPE enNameType = 0;
	CIMTYPE enPercentIdleTimeType = 0;
	if (S_OK != oEnumAccessVec[0]->GetPropertyHandle(L"Name", &enNameType, &hNameHandle))
		return;
	if (S_OK != oEnumAccessVec[0]->GetPropertyHandle(L"PercentIdleTime", &enPercentIdleTimeType, &hPercentIdleTimeHandle))
		return;

	PartitionInfo oCurPartitionInfo;
	DWORD dwVirtualBytes = 0;
	DWORD dwIDProcess = 0;
	for (DWORD nIndex = 0; nIndex < nResultCount; nIndex++)
	{
		// 1. 读盘符
		static WCHAR sTempByte[1024] = {0};
		static long nTempSize;
		memset(sTempByte, 0, 1024);

		if (S_OK != oEnumAccessVec[nIndex]->ReadPropertyValue(hNameHandle, 1024, &nTempSize, (byte*)sTempByte))
			continue;
		oCurPartitionInfo.sPartitionName = QString::fromStdWString(sTempByte);
		oCurPartitionInfo.sTrayMenuName = oCurPartitionInfo.sPartitionName;

		// 2. 读空闲时间占比
		DWORD nIdleTime = 0;
		if (S_OK != oEnumAccessVec[nIndex]->ReadDWORD(hPercentIdleTimeHandle,  &nIdleTime))
			continue;
		oCurPartitionInfo.nUsingRate = 100 - nIdleTime;

		// 3. 只要标准盘符的, 其他不要；另外，这样会把最后一个total过滤掉
		if (oCurPartitionInfo.sPartitionName.size() == 2 && oCurPartitionInfo.sPartitionName.contains(":"))
			m_oPartitionInfoList.push_back(oCurPartitionInfo);
	}

	// 把平均和最忙加进去
	addAvgAndBusiest();
}