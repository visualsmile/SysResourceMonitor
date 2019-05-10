#ifndef VSDESKBANDOPERATION_H_82665874_F285_416B_8BC4_A3B486313F87
#define VSDESKBANDOPERATION_H_82665874_F285_416B_8BC4_A3B486313F87

#include "Interface/IVSDeskBandOperation.h"
#include "Def/VSSharedMemStructDef.h"

class VSDeskBandOperation : public IVSDeskBandOperation
{
public:
	static IVSDeskBandOperation* getInstance();
	~VSDeskBandOperation();

public:
	virtual bool isRegistered() override;
	virtual bool doReg() override;
	virtual bool doUnreg() override;
	virtual bool visible() override;
	virtual bool show(bool bAutoClick = true) override;
	virtual bool hide() override;
	virtual bool updateMessage(VSSharedMemStruct* pMessageArray) override;

private: // 自动点击添加DeskBand时的系统弹窗
	void autoClickOk();
	bool findaConfirmWindow();

private:
	VSDeskBandOperation();

private:
	void initSharedMemory();
	void freeSharedMemory();
	void* m_pSharedBuffer;
	HANDLE m_hMapFile;
	HANDLE m_hWriteSemaphore;
};



#endif //VSDESKBANDOPERATION_H_82665874_F285_416B_8BC4_A3B486313F87s
