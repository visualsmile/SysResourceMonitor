#include <QMessageBox>
#include "Observer/GPU/Nvidia/SRMNvidiaGPUInfo.h"
#include "SRMScopeOnExit.h"

int SRMNvidiaGPUInfo::getGPUCount()
{
	return m_nGPUCount;
}

int SRMNvidiaGPUInfo::getGPURate(int nIndex)
{
    if (nIndex >= m_nGPUCount)
        return 0;

    NvUsages oNvUsages;
    oNvUsages.Version = GPU_USAGES_VER;
    if (m_pFun_NvAPI_GPU_GetUsages(m_hGPUHandles[nIndex], &oNvUsages) != NVAPI_OK)
        return 0;

    return oNvUsages.Usages[2];
}

int SRMNvidiaGPUInfo::getMemRate(int nIndex)
{
    if (nIndex >= m_nGPUCount)
        return 0;

    NV_DISPLAY_DRIVER_MEMORY_INFO oMemoryInfo = { 0 };
    oMemoryInfo.version = NV_DISPLAY_DRIVER_MEMORY_INFO_VER_3;
    NvAPI_Status enResult = NvAPI_GPU_GetMemoryInfo(m_hGPUHandles[nIndex], &oMemoryInfo);
    if (enResult == NVAPI_INCOMPATIBLE_STRUCT_VERSION)
    {
        oMemoryInfo.version = NV_DISPLAY_DRIVER_MEMORY_INFO_VER_2;
        enResult = NvAPI_GPU_GetMemoryInfo(m_hGPUHandles[nIndex], &oMemoryInfo);
    }

    if (enResult == NVAPI_INCOMPATIBLE_STRUCT_VERSION)
    {
        oMemoryInfo.version = NV_DISPLAY_DRIVER_MEMORY_INFO_VER_1;
        enResult = NvAPI_GPU_GetMemoryInfo(m_hGPUHandles[nIndex], &oMemoryInfo);
    }

    if (enResult != NVAPI_OK)
        return 0;

    int nTotalMemory = oMemoryInfo.dedicatedVideoMemory;
    int nEmptyMemory = oMemoryInfo.version == NV_DISPLAY_DRIVER_MEMORY_INFO_VER_1 
        ? oMemoryInfo.availableDedicatedVideoMemory : oMemoryInfo.curAvailableDedicatedVideoMemory;

	return 1.0* (nTotalMemory - nEmptyMemory) / nTotalMemory * 100;
}

int SRMNvidiaGPUInfo::getTemperature(int nIndex)
{
    if (nIndex >= m_nGPUCount)
        return 0;

    NV_GPU_THERMAL_SETTINGS oThermalSettingsInfo;
    oThermalSettingsInfo.version = NV_GPU_THERMAL_SETTINGS_VER_2;
    NvAPI_Status enResult = NvAPI_GPU_GetThermalSettings(m_hGPUHandles[nIndex], NVAPI_THERMAL_TARGET_ALL, &oThermalSettingsInfo);
    if (enResult == NVAPI_INCOMPATIBLE_STRUCT_VERSION)
    {
        oThermalSettingsInfo.version = NV_GPU_THERMAL_SETTINGS_VER_1;
        NvAPI_Status enResult = NvAPI_GPU_GetThermalSettings(m_hGPUHandles[nIndex], NVAPI_THERMAL_TARGET_ALL, &oThermalSettingsInfo);
    }

    if (enResult != NVAPI_OK)
        return 0;

    return oThermalSettingsInfo.sensor[0].currentTemp;
}

SRMGPUInfoInf* SRMNvidiaGPUInfo::createGPUInfoInf()
{
	static SRMNvidiaGPUInfo s_oSRMGPUInfo;
	if (s_oSRMGPUInfo.m_bIsValid)
		return &s_oSRMGPUInfo;
	else
		return nullptr;
}

//************************************
// Method:    SRMGPUInfo
// FullName:  SRMGPUInfo::SRMGPUInfo
// Access:    private 
// Returns:   
// Qualifier:
// Note:      
//************************************
SRMNvidiaGPUInfo::SRMNvidiaGPUInfo()
    : m_bIsValid(false)
    , m_nGPUCount(0)
    , m_hNvApiDll(nullptr)
    , m_pFun_NvAPI_GPU_GetUsages(nullptr)
    , m_pFun_NvAPI_QueryInterface(nullptr)
{
	init();
}

//************************************
// Method:    ~SRMGPUInfo
// FullName:  SRMGPUInfo::~SRMGPUInfo
// Access:    private 
// Returns:   
// Qualifier:
// Note:      
//************************************
SRMNvidiaGPUInfo::~SRMNvidiaGPUInfo()
{
	unInit();
}

//************************************
// Method:    Init
// FullName:  SRMGPUInfo::Init
// Access:    private 
// Returns:   void
// Qualifier:
// Note:      
//************************************
void SRMNvidiaGPUInfo::init()
{
    m_hNvApiDll = LoadLibrary(L"nvapi64.dll");
    if (!m_hNvApiDll)
        return;

    m_pFun_NvAPI_QueryInterface = (NvAPI_QueryInterface)GetProcAddress(m_hNvApiDll, "nvapi_QueryInterface");
    if (!m_pFun_NvAPI_QueryInterface)
        return;

    m_pFun_NvAPI_GPU_GetUsages = (NvAPI_GPU_GetUsages)m_pFun_NvAPI_QueryInterface(ID_NvAPI_GPU_GetUsages);
    if (!m_pFun_NvAPI_GPU_GetUsages)
        return;

    if (NvAPI_Initialize() != NVAPI_OK)
        return;

    memset(m_hGPUHandles, 0, sizeof(NvPhysicalGpuHandle) * NVAPI_MAX_PHYSICAL_GPUS);
    if (NvAPI_EnumPhysicalGPUs(m_hGPUHandles, &m_nGPUCount) != NVAPI_OK)
        return;

    if (m_nGPUCount <= 0)
        return;

    m_bIsValid = true;
}

//************************************
// Method:    Unit
// FullName:  SRMGPUInfo::Unit
// Access:    private 
// Returns:   void
// Qualifier:
// Note:      
//************************************
void SRMNvidiaGPUInfo::unInit()
{
    m_bIsValid = false;
    m_nGPUCount = 0;
    m_pFun_NvAPI_QueryInterface = nullptr;
    m_pFun_NvAPI_GPU_GetUsages = nullptr;

    if (m_hNvApiDll)
    {
        FreeLibrary(m_hNvApiDll);
        m_hNvApiDll = nullptr;
    }        
}


