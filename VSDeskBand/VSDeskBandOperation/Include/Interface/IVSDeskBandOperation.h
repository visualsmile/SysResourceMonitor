#ifndef IVSDESKBANDOPERATION_H_74B0AB51_EA37_4C79_B336_1EDE7DF1744E
#define IVSDESKBANDOPERATION_H_74B0AB51_EA37_4C79_B336_1EDE7DF1744E

class VSSharedMemStruct;

class IVSDeskBandOperation
{
public:
	//************************************
	// Mark:       是否注册过
	//			DeskBand需要注册后，才可以出现在工具栏选项中
	// FullName:  IVSDeskBandOperation::registered
	// Access:    virtual public 
	// Returns:   bool
	// Qualifier:
	//************************************
	virtual bool isRegistered() = 0;

	//************************************
	// Mark:      注册（将DeskBand信息写入注册表）
	// FullName:  IVSDeskBandOperation::reg
	// Access:    virtual public 
	// Returns:   bool
	// Qualifier:
	//************************************
	virtual bool doReg() = 0;

	//************************************
	// Mark:      反注册(从注册表中移除DeskBand)
	// FullName:  IVSDeskBandOperation::unreg
	// Access:    virtual public 
	// Returns:   bool
	// Qualifier:
	//************************************
	virtual bool doUnreg() = 0;

	//************************************
	// Mark:      是否可见
	// FullName:  IVSDeskBandOperation::visible
	// Access:    virtual public 
	// Returns:   bool
	// Qualifier:
	//************************************
	virtual bool visible() = 0;
	//************************************
	// Mark:      显示
	// FullName:  IVSDeskBandOperation::show
	// Access:    virtual public 
	// Returns:   bool
	// Parameter: bool bAutoClick	自动点击确定按钮（更多时候会使用hook，往资源管理器上挂hook）
	//************************************
	virtual bool show(bool bAutoClick) = 0;

	//************************************
	// Mark:      隐藏
	// FullName:  IVSDeskBandOperation::hide
	// Access:    virtual public 
	// Returns:   bool
	// Qualifier:
	//************************************
	virtual bool hide() = 0;

	//************************************
	// Mark:      更新显示数据
	// FullName:  IVSDeskBandOperation::updateMessage
	// Access:    virtual public 
	// Returns:   bool
	// Parameter: VSSharedMemStruct * pMessageArray
	//************************************
	virtual bool updateMessage(VSSharedMemStruct *pMessageArray) = 0;
};

#endif //IVSDESKBANDOPERATION_H_74B0AB51_EA37_4C79_B336_1EDE7DF1744E