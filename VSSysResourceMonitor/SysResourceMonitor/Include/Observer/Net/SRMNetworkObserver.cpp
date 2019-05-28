
#include "Observer/Net/SRMNetworkObserver.h"
#include "Observer/SRMModuleSubject.h"

const std::wstring lsNetworkRegPath = L"SYSTEM\\CurrentControlSet\\Control\\Network\\{4D36E972-E325-11CE-BFC1-08002BE10318}";
#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

REG_SRM_OBSERVER(SRMNetworkObserver);
SRMNetworkObserver::SRMNetworkObserver()
	: SRMObserverBase("SRMNetworkObserver")
{

}

SRMNetworkObserver::~SRMNetworkObserver()
{
	if (m_pAdapterTable)
	{
		FREE(m_pAdapterTable);
		m_pAdapterTable = nullptr;
	}
}

bool SRMNetworkObserver::init()
{
	return initAdapterTable();
}

void SRMNetworkObserver::update(int nIndex, VSSharedMemStruct* pSharedMemStruct, QList<QLabel*>& oLabelList)
{
	readNetValue();

	auto translateUnit = [&](double& nCurSpeed) -> QString
	{
		QString sCurUnit = " B/s";
		if (nCurSpeed > 1024)
		{
			nCurSpeed /= 1024.0;
			if (nCurSpeed > 1024)
			{
				nCurSpeed /= 1024.0;
				sCurUnit = " M/s";
			}
			else
			{
				sCurUnit = " K/s";
			}
		}

		return sCurUnit;
	};

	auto getEmptyStr = [](int nCount) -> QString
	{
		return QString(nCount, ' ');
	};
	
	double dCurDownloadSpeed = m_dCurDownloadSpeed;
	// 下载（值位置）
	QString sDownloadUnit = translateUnit(m_dCurDownloadSpeed);
	QString sCurDownloadSpeed = QString::number(m_dCurDownloadSpeed, 'f', 1);
	sCurDownloadSpeed = getEmptyStr(6 - sCurDownloadSpeed.size()) + sCurDownloadSpeed + sDownloadUnit;
	static VSShareMemTextNode oValueNode(0, 0, 0, L"0 B/s↓");
	QColor oCurColor = fillCollorByValue(dCurDownloadSpeed / 1024.0 / 1024.0, &oValueNode);	
	oValueNode.m_sInfo = QString::fromStdWString(L"%1↓  ").arg(sCurDownloadSpeed).toStdWString();
	pSharedMemStruct->setTextNode(0, nIndex, oValueNode);

	// 上传（名称位置）
	QString sUploadUnit = translateUnit(m_dCurUploadSpeed);
	QString sCurUploadSpeed = QString::number(m_dCurUploadSpeed, 'f', 1);
	sCurUploadSpeed = getEmptyStr(6 - sCurUploadSpeed.size()) + sCurUploadSpeed + sUploadUnit;
	static VSShareMemTextNode oNameNode(0, 0, 0, L"0 B/s↑");
	fillCollorByValue(dCurDownloadSpeed / 1024.0 / 1024.0, &oNameNode);// 用下载的值着色，不考虑上传速度对颜色的影响
	oNameNode.m_sInfo = QString::fromStdWString(L"%1↑  ").arg(sCurUploadSpeed).toStdWString();
	pSharedMemStruct->setTextNode(1, nIndex, oNameNode);

	// 浮窗
	QString sFloatingWidgetText = QString::fromStdWString(L"%1↓ %2↑").arg(sCurDownloadSpeed).arg(sCurUploadSpeed);
	static QPalette oPalette = oLabelList[nIndex]->palette();
	oPalette.setColor(QPalette::WindowText, oCurColor);
	oLabelList[nIndex]->setPalette(oPalette);
	oLabelList[nIndex]->setText(sFloatingWidgetText);
}

void SRMNetworkObserver::regTrayMenu(QMenu* pTrayMenu)
{
	__super::regTrayMenu(pTrayMenu, QString::fromStdWString(L"网络使用"));
}

int SRMNetworkObserver::order()
{
	return 70;
}

QString SRMNetworkObserver::customSettingDescription()
{
	return QString::fromStdWString(L"网络使用: 数据颜色以下载速率为准, 单位为 MB");
}

bool SRMNetworkObserver::initAdapterTable()
{
	m_oAdapterIDSet.clear();

	m_pAdapterTable = (MIB_IFTABLE*)MALLOC(sizeof(MIB_IFTABLE));
	ULONG nSize = sizeof(MIB_IFTABLE);
	// 尝试分配
	if (GetIfTable(m_pAdapterTable, &nSize, FALSE) != ERROR_INSUFFICIENT_BUFFER)
		return false;

	// 重新分配内存后重新获取
	FREE(m_pAdapterTable);
	m_pAdapterTable = (MIB_IFTABLE*)MALLOC(nSize);
	GetIfTable(m_pAdapterTable, &nSize, FALSE);

	MIB_IFROW oMibIfrowInfo;
	for (DWORD nIndex = 0; nIndex < m_pAdapterTable->dwNumEntries; ++nIndex)
	{
		oMibIfrowInfo.dwIndex = nIndex;
		if (GetIfEntry(&oMibIfrowInfo) != NO_ERROR)
		{
			continue;
		}

		QSet<std::wstring> oSet = getPCI();
		foreach(auto lsNetWorkName, oSet)
		{
			if (std::wstring(oMibIfrowInfo.wszName).find(lsNetWorkName) != std::wstring::npos)
			{
				m_oAdapterIDSet.insert(oMibIfrowInfo.dwIndex);
			}
		}
	}

	return m_oAdapterIDSet.size() > 0;
}

void SRMNetworkObserver::readNetValue()
{
	static bool bFirst = true;
	static DWORD nPreDownload = INT_MAX;
	static DWORD nPreUpload = INT_MAX;

	DWORD nCurDownload = 0;
	DWORD nCurUpload = 0;
	MIB_IFROW oMibIfrowInfo;
	for (auto it = m_oAdapterIDSet.begin(); it != m_oAdapterIDSet.end(); ++it)
	{
		oMibIfrowInfo.dwIndex = *it;
		if (GetIfEntry(&oMibIfrowInfo) != NO_ERROR)
		{
			continue;
		}

		if (oMibIfrowInfo.dwOperStatus == MIB_IF_OPER_STATUS_NON_OPERATIONAL
			|| oMibIfrowInfo.dwOperStatus == MIB_IF_OPER_STATUS_UNREACHABLE
			|| oMibIfrowInfo.dwOperStatus == MIB_IF_OPER_STATUS_DISCONNECTED)
		{
			continue;
		}

		//if (oMibIfrowInfo.dwType != MIB_IF_TYPE_ETHERNET)
		//{
		//	continue;
		//}

		nCurDownload += oMibIfrowInfo.dwInOctets;
		nCurUpload += oMibIfrowInfo.dwOutOctets;
	}

	m_dCurDownloadSpeed = nCurDownload - nPreDownload;
	m_dCurUploadSpeed = nCurUpload - nPreUpload;

	if (bFirst)
	{
		m_dCurDownloadSpeed = 0;
		m_dCurUploadSpeed = 0;
		bFirst = false;
	}

	nPreDownload = nCurDownload;
	nPreUpload = nCurUpload;
}

QSet<std::wstring> SRMNetworkObserver::getPCI()
{
	QSet<std::wstring> oPCISet;
	HKEY Key_Network;
	RegCreateKeyExW(HKEY_LOCAL_MACHINE,
		lsNetworkRegPath.c_str(),
		0,
		NULL,
		REG_OPTION_NON_VOLATILE,
		KEY_READ,
		NULL,
		&Key_Network,
		NULL);


	INT nPathIndex = 0;
	WCHAR Path_Name[MAX_PATH];
	DWORD nNameSize = MAX_PATH;
	while (RegEnumKeyExW(Key_Network, nPathIndex, Path_Name, &nNameSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
	{
		if (Path_Name[0] == L'{')
		{
			std::wstring lsPathNew = lsNetworkRegPath + L"\\" + Path_Name + L"\\Connection";
			WCHAR Value[MAX_PATH];
			DWORD Value_Size = MAX_PATH;
			RegGetValueW(
				HKEY_LOCAL_MACHINE,
				lsPathNew.c_str(),
				L"PnPInstanceId",
				RRF_RT_REG_SZ,
				NULL,
				Value,
				&Value_Size
			);

			if (std::wstring(Value).find(L"PCI") == 0)
			{
				oPCISet.insert(Path_Name);
			}
		}
		nPathIndex++;
		nNameSize = MAX_PATH;
	}
	RegCloseKey(Key_Network);

	return oPCISet;
}

