#ifndef SRMATIGPUINFO_H_8ADE5879_B99F_46F9_BC02_D750992626DB
#define SRMATIGPUINFO_H_8ADE5879_B99F_46F9_BC02_D750992626DB
/************************************************************************/
/* ATI显卡信息 
依赖 atiadlxx.dll
关于 atiadlxx.dll 和 atiadlxy.dll:
xx永远都是与系统保持一致的。32位系统上，xx就是32位，64位系统上，xx就是64位。
xy的用途是：64位系统上，运行32位程序时，xx没法用，得用32位的xy。
最好先加载xx，如果失败了，再加载xy
ADL 没有提供显存信息读取接口
*/
/************************************************************************/
#include "Observer\GPU\ATI\ADL\adl_sdk.h"
#include "Observer\GPU\SRMGPUInfoInf.h"
#include <windows.h>

// API定义
typedef int (*ADL_MAIN_CONTROL_CREATE) (ADL_MAIN_MALLOC_CALLBACK, int);
typedef int (*ADL_MAIN_CONTROL_DESTROY) ();
typedef int (*ADL_ADAPTER_NUMBEROFADAPTERS_GET) (int*);
typedef int (*ADL_ADAPTER_ADAPTERINFO_GET) (LPAdapterInfo, int);
typedef int (*ADL_DISPLAY_COLORCAPS_GET) (int, int, int*, int*);
typedef int (*ADL_DISPLAY_COLOR_GET) (int, int, int, int*, int*, int*, int*, int*);
typedef int (*ADL_DISPLAY_COLOR_SET) (int, int, int, int);
typedef int (*ADL_DISPLAY_DISPLAYINFO_GET)	(int, int*, ADLDisplayInfo**, int);
typedef int (*ADL_OVERDRIVE5_TEMPERATURE_GET) (int, int, ADLTemperature*);
typedef int (*ADL_OVERDRIVE5_FANSPEED_GET) (int, int, ADLFanSpeedValue*);
typedef int (*ADL_OVERDRIVE5_CURRENTACTIVITY_GET) (int, ADLPMActivity*);
typedef int (*ADL_ADAPTER_MEMORYINFO_GET) (int, ADLMemoryInfo*);

class SRMATIGPUInfo : public SRMGPUInfoInf
{
public:
    static SRMGPUInfoInf* createGPUInfoInf();

public:
    virtual int getGPUCount() override;
    virtual int getGPURate(int nIndex) override;
    virtual int getMemRate(int nIndex) override;
    virtual int getTemperature(int nIndex) override;

private:
    SRMATIGPUInfo();
    ~SRMATIGPUInfo();
    void init();
    void unInit();

private:// API接口指针
    ADL_MAIN_CONTROL_CREATE				m_pFun_ADL_Main_Control_Create;
    ADL_MAIN_CONTROL_DESTROY			m_pFun_ADL_Main_Control_Destroy;
    ADL_ADAPTER_NUMBEROFADAPTERS_GET	m_pFun_ADL_Adapter_NumberOfAdapters_Get;
    ADL_ADAPTER_ADAPTERINFO_GET		    m_pFun_ADL_Adapter_AdapterInfo_Get;
    ADL_OVERDRIVE5_CURRENTACTIVITY_GET	m_pFun_ADL_Overdrive5_CurrentActivity_Get;
    ADL_OVERDRIVE5_TEMPERATURE_GET		m_pFun_ADL_Overdrive5_Temperature_Get;
    ADL_ADAPTER_MEMORYINFO_GET          m_pFun_ADL_Adapter_MemoryInfo_Get;
    //ADL_DISPLAY_COLORCAPS_GET			m_pFun_ADL_Display_ColorCaps_Get;
    //ADL_DISPLAY_COLOR_GET				m_pFun_ADL_Display_Color_Get;
    //ADL_DISPLAY_COLOR_SET				m_pFun_ADL_Display_Color_Set;
    //ADL_DISPLAY_DISPLAYINFO_GET		    m_pFun_ADL_Display_DisplayInfo_Get;    
    //ADL_OVERDRIVE5_FANSPEED_GET			m_pFun_ADL_Overdrive5_FanSpeed_Get;

private:
    bool m_bIsValid;
    int m_AdaptersNumber;               // 显卡数量
    AdapterInfo* m_pAdapterInfos;       // 显卡信息, 这是一个数组
    HINSTANCE m_hATIApiDll;             // API dll 句柄
};
#endif //SRMATIGPUINFO_H_8ADE5879_B99F_46F9_BC02_D750992626DB
