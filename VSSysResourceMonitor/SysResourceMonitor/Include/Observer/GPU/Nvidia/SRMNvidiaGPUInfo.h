#ifndef SRMNVIDIAGPUINFO_H_B9DFEF68_12C7_4604_A025_8E75E8BA74B6
#define SRMNVIDIAGPUINFO_H_B9DFEF68_12C7_4604_A025_8E75E8BA74B6

/********************************************************************
 Nvidia显卡信息，需要用到 nvapi64.dll
 GPU信息读取，这是一个单例
*********************************************************************/
#include <Windows.h>
#include "SRMError.h"
#include "Observer/GPU/SRMGPUInfoInf.h"

#define MAX_GPU_NUM						4L		// 监控的GPU个数（NVIDIA定义的最多GPU个数是64，这里最多只监控4个）
#define MAX_DISPLAY_CARDS               4       // 最多监控4块显卡（暂时应该够了吧）  
#define MAX_THERMAL_SENSORS_PER_GPU     3  
#define MAX_USAGES_PER_GPU              33  
#define MAX_MEMORY_VALUES_PER_GPU       5  
#define MAX_PHYSICAL_GPUS               64 
#define MY_PROCESS_ERROR(Condition) do { if (!(Condition)) throw SRMError(esrmeGpuReadError); } while (false)  

typedef int NvDisplayHandle;
typedef int NvPhysicalGpuHandle;
//*****************************以下是类内部使用的结构体************************************//  
/*
* @brief GPU信息
*/
typedef struct _GPU_INFO
{
	NvPhysicalGpuHandle     nvGpuHandle;        // GPU句柄  
	int                     nUsage;             // GPU占用率  
}GPU_INFO;

/*
* @brief 显卡信息
*/
typedef struct _DISPLAY_CARD_INFO
{
	NvDisplayHandle nvDisplayHandle;            // 显卡句柄  
	int             nGpuCount;                  // Gpu个数  
	DWORD           dwTotalMemory;              // 总显存大小（KB）  
	DWORD           dwFreeMemory;               // 空闲显存大小（KB）  

	GPU_INFO        sGpuInfo[MAX_GPU_NUM];      // GPU信息  
}DISPLAY_CARD_INFO;
//************************以上是类内部使用的结构体************************************//  

/*
* @brief 显卡相关信息（可用于导出的结构体）
*/
typedef struct _DISPLAY_INFO
{
	int         nGpuCount;                  // Gpu个数  
	int         nGpuUsages[MAX_GPU_NUM];    // Gpu占用率  
	DWORD       dwTotalMemory;              // 总显存大小（KB）  
	DWORD       dwFreeMemory;               // 空闲显存大小（KB）  
}DISPLAY_INFO;

enum NvStatus
{
	enumNvStatus_OK = 0,
	enumNvStatus_ERROR = -1,
	enumNvStatus_LIBRARY_NOT_FOUND = -2,
	enumNvStatus_NO_IMPLEMENTATION = -3,
	enumNvStatus_API_NOT_INTIALIZED = -4,
	enumNvStatus_INVALID_ARGUMENT = -5,
	enumNvStatus_NVIDIA_DEVICE_NOT_FOUND = -6,
	enumNvStatus_END_ENUMERATION = -7,
	enumNvStatus_INVALID_HANDLE = -8,
	enumNvStatus_INCOMPATIBLE_STRUCT_VERSION = -9,
	enumNvStatus_HANDLE_INVALIDATED = -10,
	enumNvStatus_OPENGL_CONTEXT_NOT_CURRENT = -11,
	enumNvStatus_NO_GL_EXPERT = -12,
	enumNvStatus_INSTRUMENTATION_DISABLED = -13,
	enumNvStatus_EXPECTED_LOGICAL_GPU_HANDLE = -100,
	enumNvStatus_EXPECTED_PHYSICAL_GPU_HANDLE = -101,
	enumNvStatus_EXPECTED_DISPLAY_HANDLE = -102,
	enumNvStatus_INVALID_COMBINATION = -103,
	enumNvStatus_NOT_SUPPORTED = -104,
	enumNvStatus_PORTID_NOT_FOUND = -105,
	enumNvStatus_EXPECTED_UNATTACHED_DISPLAY_HANDLE = -106,
	enumNvStatus_INVALID_PERF_LEVEL = -107,
	enumNvStatus_DEVICE_BUSY = -108,
	enumNvStatus_NV_PERSIST_FILE_NOT_FOUND = -109,
	enumNvStatus_PERSIST_DATA_NOT_FOUND = -110,
	enumNvStatus_EXPECTED_TV_DISPLAY = -111,
	enumNvStatus_EXPECTED_TV_DISPLAY_ON_DCONNECTOR = -112,
	enumNvStatus_NO_ACTIVE_SLI_TOPOLOGY = -113,
	enumNvStatus_SLI_RENDERING_MODE_NOTALLOWED = -114,
	enumNvStatus_EXPECTED_DIGITAL_FLAT_PANEL = -115,
	enumNvStatus_ARGUMENT_EXCEED_MAX_SIZE = -116,
	enumNvStatus_DEVICE_SWITCHING_NOT_ALLOWED = -117,
	enumNvStatus_TESTING_CLOCKS_NOT_SUPPORTED = -118,
	enumNvStatus_UNKNOWN_UNDERSCAN_CONFIG = -119,
	enumNvStatus_TIMEOUT_RECONFIGURING_GPU_TOPO = -120,
	enumNvStatus_DATA_NOT_FOUND = -121,
	enumNvStatus_EXPECTED_ANALOG_DISPLAY = -122,
	enumNvStatus_NO_VIDLINK = -123,
	enumNvStatus_REQUIRES_REBOOT = -124,
	enumNvStatus_INVALID_HYBRID_MODE = -125,
	enumNvStatus_MIXED_TARGET_TYPES = -126,
	enumNvStatus_SYSWOW64_NOT_SUPPORTED = -127,
	enumNvStatus_IMPLICIT_SET_GPU_TOPOLOGY_CHANGE_NOT_ALLOWED = -128,
	enumNvStatus_REQUEST_USER_TO_CLOSE_NON_MIGRATABLE_APPS = -129,
	enumNvStatus_OUT_OF_MEMORY = -130,
	enumNvStatus_WAS_STILL_DRAWING = -131,
	enumNvStatus_FILE_NOT_FOUND = -132,
	enumNvStatus_TOO_MANY_UNIQUE_STATE_OBJECTS = -133,
	enumNvStatus_INVALID_CALL = -134,
	enumNvStatus_D3D10_1_LIBRARY_NOT_FOUND = -135,
	enumNvStatus_FUNCTION_NOT_FOUND = -136
};

enum NvThermalController
{
	enumNvThermalController_NONE = 0,
	enumNvThermalController_GPU_INTERNAL,
	enumNvThermalController_ADM1032,
	enumNvThermalController_MAX6649,
	enumNvThermalController_MAX1617,
	enumNvThermalController_LM99,
	enumNvThermalController_LM89,
	enumNvThermalController_LM64,
	enumNvThermalController_ADT7473,
	enumNvThermalController_SBMAX6649,
	enumNvThermalController_VBIOSEVT,
	enumNvThermalController_OS,
	enumNvThermalController_UNKNOWN = -1,
};

enum NvThermalTarget
{
	enumNvThermalTarget_NONE = 0,
	enumNvThermalTarget_GPU = 1,
	enumNvThermalTarget_MEMORY = 2,
	enumNvThermalTarget_POWER_SUPPLY = 4,
	enumNvThermalTarget_BOARD = 8,
	enumNvThermalTarget_ALL = 15,
	enumNvThermalTarget_UNKNOWN = -1
};

enum NV_THERMAL_TARGET
{
	NVAPI_THERMAL_TARGET_NONE = 0,
	NVAPI_THERMAL_TARGET_GPU = 1,     //!< GPU core temperature requires NvPhysicalGpuHandle
	NVAPI_THERMAL_TARGET_MEMORY = 2,     //!< GPU memory temperature requires NvPhysicalGpuHandle
	NVAPI_THERMAL_TARGET_POWER_SUPPLY = 4,     //!< GPU power supply temperature requires NvPhysicalGpuHandle
	NVAPI_THERMAL_TARGET_BOARD = 8,     //!< GPU board ambient temperature requires NvPhysicalGpuHandle
	NVAPI_THERMAL_TARGET_VCD_BOARD = 9,     //!< Visual Computing Device Board temperature requires NvVisualComputingDeviceHandle
	NVAPI_THERMAL_TARGET_VCD_INLET = 10,    //!< Visual Computing Device Inlet temperature requires NvVisualComputingDeviceHandle
	NVAPI_THERMAL_TARGET_VCD_OUTLET = 11,    //!< Visual Computing Device Outlet temperature requires NvVisualComputingDeviceHandle

	NVAPI_THERMAL_TARGET_ALL = 15,
	NVAPI_THERMAL_TARGET_UNKNOWN = -1,
};

typedef struct _NvMemoryInfo
{
	unsigned int Version;
	unsigned int Values[MAX_MEMORY_VALUES_PER_GPU];
}NvMemoryInfo;

typedef struct _NvSensor
{
	NvThermalController Controller;
	unsigned int DefaultMinTemp;
	unsigned int DefaultMaxTemp;
	unsigned int CurrentTemp;
	NvThermalTarget Target;
}NvSensor;

typedef struct _NvGPUThermalSettings
{
	unsigned int Version;
	unsigned int Count;
	NvSensor Sensor[MAX_THERMAL_SENSORS_PER_GPU];
}NvGPUThermalSettings;


typedef struct _NvUsages
{
	unsigned int Version;
	unsigned int Usages[MAX_USAGES_PER_GPU];
}NvUsages;

// 接口ID值
#define ID_NvAPI_Initialize                     0x0150E828  
#define ID_NvAPI_GPU_GetUsages                  0x189A1FDF
#define ID_NvAPI_GPU_GetMemoryInfo              0x774AA982
#define ID_NvAPI_GetPhysicalGPUsFromDisplay     0x34EF9506  
#define ID_NvAPI_EnumNvidiaDisplayHandle        0x9ABDD40D  
#define ID_NvAPI_GPU_GetThermalSettings         0xE3640A56

// 版本号参数定义
#define GPU_THERMAL_SETTINGS_VER                (sizeof(NvGPUThermalSettings) | 0x10000) 
#define GPU_USAGES_VER                          (sizeof(NvUsages) | 0x10000)  
#define GPU_MEMORY_INFO_VER                     (sizeof(NvMemoryInfo) | 0x20000)  

// 函数定义
typedef NvStatus(*NvAPI_InitializeType)();
typedef void* (*Nvapi_QueryInterfaceType)(unsigned int uiInterfaceID);
typedef NvStatus(*NvAPI_GPU_GetThermalSettingsType)(const NvPhysicalGpuHandle gpuHandle, int sensorIndex, NvGPUThermalSettings* pnvGPUThermalSettings);
typedef NvStatus(*NvAPI_EnumNvidiaDisplayHandleType)(const int thisEnum, NvDisplayHandle* pDisplayHandle);
typedef NvStatus(*NvAPI_GPU_GetUsagesType)(const NvPhysicalGpuHandle gpuHandle, NvUsages* pnvUsages);
typedef NvStatus(*NvAPI_GPU_GetMemoryInfoType)(const NvDisplayHandle displayHandle, NvMemoryInfo* pnvMemoryInfo);
typedef NvStatus(*NvAPI_GetPhysicalGPUsFromDisplayType)(const NvDisplayHandle displayHandle, NvPhysicalGpuHandle* pGpuHandles, unsigned int* pGpuCount);

// 类
class SRMNvidiaGPUInfo : public SRMGPUInfoInf
{
public:
	virtual int getGPUCount() override;
	virtual int getGPURate(int nIndex) override;
	virtual int getMemRate(int nIndex) override;
	virtual int getTemperature(int nIndex) override;

public:
	static SRMGPUInfoInf* createGPUInfoInf();

private:
	bool isValid();

private:
	bool GetDisplayInfo(const int nCardIndex, DISPLAY_INFO* pDisplayInfo);
	bool GetThermal(const int nCardIndex, NvGPUThermalSettings* info);

private:
	void Init();
	void Unit();
	bool GetDisplayCardGpuUsages(const NvDisplayHandle nvDisplayHandle, DISPLAY_CARD_INFO* pCardInfo);
	bool GetDisplayCardMemoryInfo(const NvDisplayHandle nvDisplayHandle, DISPLAY_CARD_INFO* pCardInfo);
	int  EnumDisplayCards();
	bool GetGpuHandles(const NvDisplayHandle nvDisplayHandle, DISPLAY_CARD_INFO* pCardInfo);

private:
	NvAPI_InitializeType m_pfnNvAPI_Initialize;
	NvAPI_GPU_GetThermalSettingsType m_pfnNvAPI_GPU_GetThermalSettings;
	NvAPI_GetPhysicalGPUsFromDisplayType m_pfnNvAPI_GetPhysicalGPUsFromDisplay;
	NvAPI_EnumNvidiaDisplayHandleType m_pfnNvAPI_EnumNvidiaDisplayHandle;
	Nvapi_QueryInterfaceType m_pfnNvapi_QueryInterface;
	NvAPI_GPU_GetUsagesType m_pfnNvAPI_GPU_GetUsages;
	NvAPI_GPU_GetMemoryInfoType m_pfnNvAPI_GPU_GetMemoryInfo;

private:
	SRMNvidiaGPUInfo();
	~SRMNvidiaGPUInfo();

private:
	bool m_bIsValid;        // 是否有效
	HMODULE             m_hNvApiDll;
	int                 m_nDisplayCardCount;
	DISPLAY_CARD_INFO* m_pDisplayCards;
};
#endif //SRMNVIDIAGPUINFO_H_B9DFEF68_12C7_4604_A025_8E75E8BA74B6