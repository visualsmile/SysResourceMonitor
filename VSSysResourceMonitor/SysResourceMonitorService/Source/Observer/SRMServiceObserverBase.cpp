
#include "Observer/SRMServiceObserverBase.h"

SRMServiceObserverBase::SRMServiceObserverBase(QString sWriteMutexSemaphoreName, QString sShareMemoryName)
	: m_sMutexSemaphoreName(sWriteMutexSemaphoreName)
	, m_sShareMemoryName(sShareMemoryName)
{
	initSharedMemory();
}

SRMServiceObserverBase::~SRMServiceObserverBase()
{
	freeSharedMemory();
}

bool SRMServiceObserverBase::init()
{
	if (!m_pSharedBuffer
		|| !m_hMapFile
		|| !m_hWriteSemaphore)
		return false;

	return true;
}

void SRMServiceObserverBase::writeValue(int nValue)
{
	WaitForSingleObject(m_hWriteSemaphore, INFINITE);	// （信号量句柄，等待时间）
	memcpy(m_pSharedBuffer, &nValue, sizeof(int));
	ReleaseSemaphore(m_hWriteSemaphore, 1, NULL);		// （信号量句柄，释放的数量，out释放之前资源数量）
}

void SRMServiceObserverBase::initSharedMemory()
{
	// 服务运行的权限比普通进程要高，需要一个完整的 安全描述符 来允许普通进程访问
	SECURITY_DESCRIPTOR oSecutityDese;
	::InitializeSecurityDescriptor(&oSecutityDese, SECURITY_DESCRIPTOR_REVISION);
	::SetSecurityDescriptorDacl(&oSecutityDese, TRUE, NULL, FALSE);
	SECURITY_ATTRIBUTES oSecurityAttr;
	oSecurityAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	oSecurityAttr.bInheritHandle = FALSE;
	oSecurityAttr.lpSecurityDescriptor = &oSecutityDese;
	m_hMapFile = ::CreateFileMapping(INVALID_HANDLE_VALUE, &oSecurityAttr, PAGE_READWRITE, 0, 1024, m_sShareMemoryName.toStdWString().c_str());
	m_pSharedBuffer = ::MapViewOfFile(m_hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	m_hWriteSemaphore = CreateSemaphore(&oSecurityAttr, 1, 1, m_sMutexSemaphoreName.toStdWString().c_str());
}

void SRMServiceObserverBase::freeSharedMemory()
{
	UnmapViewOfFile(m_pSharedBuffer);
	CloseHandle(m_hMapFile);
	CloseHandle(m_hWriteSemaphore);
}

