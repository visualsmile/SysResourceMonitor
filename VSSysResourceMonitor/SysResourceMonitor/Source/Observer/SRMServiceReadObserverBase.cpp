#include "Observer/SRMServiceReadObserverBase.h"
#include "SRMServiceOperation.h"

SRMServiceReadObserverBase::SRMServiceReadObserverBase(QString sReadMutexSemaphoreName, QString sShareMemoryName, QString sObserverClassName)
	: m_sReadMutexSemaphoreName(sReadMutexSemaphoreName)
	, m_sShareMemoryName(sShareMemoryName)
	, SRMObserverBase(sObserverClassName)
	, m_hFile(nullptr)
	, m_hReadMutexSemaphore(nullptr)
	, m_pBuffer(nullptr)
{

}

SRMServiceReadObserverBase::~SRMServiceReadObserverBase()
{
	UnmapViewOfFile(m_pBuffer);
	CloseHandle(m_hFile);
	CloseHandle(m_hReadMutexSemaphore);
}

bool SRMServiceReadObserverBase::init()
{
	if (!SRMServiceOperation::getInstance()->serviceIsRunning())
		return false;

	m_hFile = ::OpenFileMapping(FILE_MAP_ALL_ACCESS, 0, m_sShareMemoryName.toStdWString().c_str());
	int n = GetLastError();
	if (!m_hFile)
		return false;

	m_pBuffer = ::MapViewOfFile(m_hFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (!m_pBuffer)
		return false;

	m_hReadMutexSemaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, m_sReadMutexSemaphoreName.toStdWString().c_str());
	if (!m_hReadMutexSemaphore)
		return false;

	return true;
}

int SRMServiceReadObserverBase::readServiceData()
{
	WaitForSingleObject(m_hReadMutexSemaphore, INFINITE);	// （信号量句柄，等待时间）
	int nResult = -1;
	memcpy(&nResult, m_pBuffer, sizeof(int));
	ReleaseSemaphore(m_hReadMutexSemaphore, 1, NULL);		// （信号量句柄，释放的数量，out释放之前资源数量）
	return nResult;
}
