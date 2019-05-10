
#include <windows.h>
#include "Observer/CPU/OlsApi.h"
#include "Observer/SRMServiceSubject.h"
#include "Observer/CPU/SRMCPUTemperatureServiceObserver.h"
#include "qtservice.h"

#define IA32_PACKAGE_THERM_STATUS 0x1B1     // CPU package温度(16~22)位
#define IA32_THERM_STATUS_MSR 0x019C        // CPU 核心温度(16~22)位
#define IA32_TEMPERATURE_TARGET 0x01A2      // TjunctionMax CPU 保护温度  (16~23)位

REG_SRM_SERVICE_OBSERVER(SRMCPUTemperatureServiceObserver);

SRMCPUTemperatureServiceObserver::SRMCPUTemperatureServiceObserver()
	: SRMServiceObserverBase(s_sCPUTemperatureServiceWriteMutexSemaphoreName, s_sCPUTemperatureServiceShareMemoryName)
{
}

SRMCPUTemperatureServiceObserver::~SRMCPUTemperatureServiceObserver()
{
	DeinitializeOls();
}

bool SRMCPUTemperatureServiceObserver::init()
{
	if (!__super::init())
		return false;

	if (!InitializeOls())
	{
		QtServiceBase::instance()->logMessage(QString::fromStdWString(L"WinRing0x64初始化失败"), QtServiceBase::Warning);
		//switch (GetDllStatus())
		//{
		//case OLS_DLL_NO_ERROR:
		//    QMessageBox::warning(this, QStringLiteral("没有找到驱动"), QStringLiteral("OLS_DLL_NO_ERROR"));
		//    break;
		//case OLS_DLL_UNSUPPORTED_PLATFORM:
		//    QMessageBox::warning(this, QStringLiteral("没有找到驱动"), QStringLiteral("OLS_DLL_UNSUPPORTED_PLATFORM"));
		//    break;
		//case OLS_DLL_DRIVER_NOT_LOADED:
		//    QMessageBox::warning(this, QStringLiteral("没有找到驱动"), QStringLiteral("OLS_DLL_DRIVER_NOT_LOADED"));
		//    break;
		//case OLS_DLL_DRIVER_NOT_FOUND:
		//    QMessageBox::warning(this, QStringLiteral("没有找到驱动"), QStringLiteral("OLS_DLL_DRIVER_NOT_FOUND"));
		//    break;
		//case OLS_DLL_DRIVER_UNLOADED:
		//    QMessageBox::warning(this, QStringLiteral("没有找到驱动"), QStringLiteral("OLS_DLL_DRIVER_UNLOADED"));
		//    break;
		//case OLS_DLL_DRIVER_NOT_LOADED_ON_NETWORK:
		//    QMessageBox::warning(this, QStringLiteral("没有找到驱动"), QStringLiteral("OLS_DLL_DRIVER_NOT_LOADED_ON_NETWORK"));
		//    break;
		//case OLS_DLL_UNKNOWN_ERROR:
		//default:
		//    QMessageBox::warning(this, QStringLiteral("没有找到驱动"), QStringLiteral("OLS_DLL_UNKNOWN_ERROR"));
		//    return;
		//    break;
		//}
		return false;
	}
	else
	{
		readRduTjunctionMax();
		return true;
	}
}

void SRMCPUTemperatureServiceObserver::update()
{
	writeValue(getCPUTemperature());
}

int SRMCPUTemperatureServiceObserver::getCPUTemperature()
{
	DWORD uHigh, uLow;
	//RdmsrTx(IA32_THERM_STATUS_MSR, &uLow, &uHigh, 1LU << 0);  // 1, 2, 4, 8  不同核心的温度 
	RdmsrTx(IA32_PACKAGE_THERM_STATUS, &uLow, &uHigh, 1LU << 0); // CPU包温度

	DWORD uCur = (uLow & 0x7FFFFF) >> 16;                  // (16~22)位
	return m_uRduTjunctionMax - uCur;
}

//************************************
// Method:    readRduTjunctionMax
// FullName:  SRMCpuTemperatureObserver::readRduTjunctionMax
// Access:    private 
// Returns:   void
// Qualifier:
// Note:      读取保护温度
//************************************
void SRMCPUTemperatureServiceObserver::readRduTjunctionMax()
{
	DWORD uHigh, uLow;
	RdmsrTx(IA32_TEMPERATURE_TARGET, &uLow, &uHigh, 1LU << 0);   // 保护温度
	m_uRduTjunctionMax = (uLow & 0xFFFFFF) >> 16;                // (16~23)位
}

