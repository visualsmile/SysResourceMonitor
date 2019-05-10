
#include "SRMServiceOperation.h"
#include <string>
#include <QStringList>
#include <QString>
#include <QApplication>

SRMServiceOperation* SRMServiceOperation::getInstance()
{
	static SRMServiceOperation s_oSRMServiceOperation;
	return &s_oSRMServiceOperation;
}

bool SRMServiceOperation::serviceIsRunning()
{
	if (!m_bServiceIsRunning)
		m_bServiceIsRunning = chekService();

	return m_bServiceIsRunning;
}

bool SRMServiceOperation::unregService()
{
	return execOp(1, QString::fromStdWString(L"卸载"));
}

bool SRMServiceOperation::regService()
{
	return execOp(0, QString::fromStdWString(L"安装"));
}

bool SRMServiceOperation::startService()
{
	return execOp(2, QString::fromStdWString(L"启动"));
}

bool SRMServiceOperation::stopService()
{
	return execOp(3, QString::fromStdWString(L"停止"));
}

bool SRMServiceOperation::chekService()
{	
	bool bIsAutoRun = false;
	QStringList sArgumentList = qApp->arguments();
	if (sArgumentList.size() > 1 && sArgumentList[1] == "AutoRun")
	{
		bIsAutoRun = true;
	}
	
	int nRetryTimes = bIsAutoRun ? 60 * 1000  : 0;	
	HANDLE hServiceMutex = openServiceMutex(nRetryTimes);
	if (hServiceMutex)
	{
		CloseHandle(hServiceMutex);
		return true;
	}

	if (bIsAutoRun)
	{
		return false;
	}

	if (regService())
	{
		hServiceMutex = openServiceMutex(60 * 1000);
		if (hServiceMutex)
		{
			CloseHandle(hServiceMutex);
			return true;
		}
	}

	return false;
}

bool SRMServiceOperation::execOp(int nIndex, QString sOperationName)
{
	QString sBatName = QString::fromStdWString(L"SysResourceMonitorService_%1_%2服务.bat").arg(nIndex).arg(sOperationName);
	HINSTANCE hRegister = ShellExecute(0, L"runas", sBatName.toStdWString().c_str(), NULL, NULL, SW_HIDE);
	if ((__int64)hRegister < 32)
	{
		QString sErrorMsg = QString::fromStdWString(L"无法%1SysResourceMonitorService。\n请重试 或手动执行 SysResourceMonitorService_%2_%3服务.bat。").arg(sOperationName).arg(nIndex).arg(sOperationName);
		MessageBox(NULL, sErrorMsg.toStdWString().c_str(), L"ERROR", MB_OK);
		return false;
	}
	WaitForSingleObject(hRegister, -1);
	Sleep(1000);
	return true;
};

HANDLE SRMServiceOperation::openServiceMutex(int nRetryTimes)
{
	HANDLE hServiceMutex = nullptr;
	do
	{
		hServiceMutex = OpenMutex(SEMAPHORE_ALL_ACCESS, FALSE, L"Global\\VS_SysResourceMonitorServiceMutex");

		Sleep(1);
	} while (!hServiceMutex && nRetryTimes--);

	return hServiceMutex;
}

SRMServiceOperation::SRMServiceOperation()
	:m_bServiceIsRunning(false)
{
}

SRMServiceOperation::~SRMServiceOperation()
{

}
