#ifndef SRMGPUINFO_H_03C2435D_09EF_49D3_BF69_2F0801F0F81F
#define SRMGPUINFO_H_03C2435D_09EF_49D3_BF69_2F0801F0F81F
/************************************************************************/
/* 显卡数据读取接口定义                                                                     */
/************************************************************************/

class SRMGPUInfoInf
{
public:
	virtual int getGPUCount() = 0;
	virtual int getGPURate(int nIndex) = 0;
	virtual int getMemRate(int nIndex) = 0;
	virtual int getTemperature(int nIndex) = 0;
};

#endif //SRMGPUINFO_H_03C2435D_09EF_49D3_BF69_2F0801F0F81F