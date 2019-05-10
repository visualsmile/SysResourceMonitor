#include "SysResourceMonitorService.h"
#include "Observer/SRMServiceSubject.h"
static std::wstring lsSysResourceMonitorServiceMutex = L"Global\\VS_SysResourceMonitorServiceMutex";
static QString c_sServiceName = QString::fromStdWString(L"SysResourceMonitor Service");
static QString c_sServiceDescription = QString::fromStdWString(L"SysResourceMonitor 用于读取部分高权限系统资源信息的服务");

SysResourceMonitorService::SysResourceMonitorService(int argc, char** argv)
	: QtService<QCoreApplication>(argc, argv, c_sServiceName)
	, m_hSysResourceMonitorServiceMutex(nullptr)
{
	setServiceDescription(c_sServiceDescription);
	setServiceFlags(QtServiceBase::Default);
	setStartupType(QtServiceController::AutoStartup);
}

SysResourceMonitorService::~SysResourceMonitorService()
{

}

void SysResourceMonitorService::start()
{
	// 服务运行的权限比普通进程要高，需要一个完整的 安全描述符 来允许普通进程访问
	SECURITY_DESCRIPTOR oSecutityDese;
	::InitializeSecurityDescriptor(&oSecutityDese, SECURITY_DESCRIPTOR_REVISION);
	::SetSecurityDescriptorDacl(&oSecutityDese, TRUE, NULL, FALSE);
	SECURITY_ATTRIBUTES oSecurityAttr;
	oSecurityAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	oSecurityAttr.bInheritHandle = FALSE;
	oSecurityAttr.lpSecurityDescriptor = &oSecutityDese;
	HANDLE hResult = OpenMutex(SEMAPHORE_ALL_ACCESS, FALSE, lsSysResourceMonitorServiceMutex.c_str());
	if (hResult)
	{
		CloseHandle(hResult);
		return;
	}

	m_hSysResourceMonitorServiceMutex = CreateMutex(&oSecurityAttr, FALSE, lsSysResourceMonitorServiceMutex.c_str());
	connect(&m_oTimer, &QTimer::timeout, this, &SysResourceMonitorService::update, Qt::UniqueConnection);
	m_oTimer.start(1000);
	logMessage(QString::fromStdWString(L"SysResourceMonitorService服务启动成功"));
}

void SysResourceMonitorService::stop()
{
	CloseHandle(m_hSysResourceMonitorServiceMutex);
	m_hSysResourceMonitorServiceMutex = nullptr;
	m_oTimer.stop();
	logMessage(QString::fromStdWString(L"SysResourceMonitorService服务停止成功"));
}

void SysResourceMonitorService::update()
{
	SRMServiceSubject::getInstance()->doOntify();
}
