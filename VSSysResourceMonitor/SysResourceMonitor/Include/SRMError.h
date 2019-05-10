#ifndef SRMERROR_H_52A7B72E_A4CF_423B_A57E_9640516FCB94
#define SRMERROR_H_52A7B72E_A4CF_423B_A57E_9640516FCB94
/************************************************************************/
/* 异常定义                                                                     */
/************************************************************************/
#include <QString>

enum EnSRMError
{
	esrmeUnKnown = -1,
	esrmeNoError = 0,			// 0
	esrmeConfigError = 10,		// 10 配置文件不可写或创建失败
	esrmeGpuReadError,			// 10 显卡数据读取错误
};

class SRMError
{
public:
	SRMError(EnSRMError enError, QString &sMessage = QString("")) 
		: m_enErrorCode(enError), m_sErrorMessage(sMessage){
	}
	~SRMError(){

	}

	EnSRMError errorCode() {
		return m_enErrorCode;
	}
	QString errorMessage() {
		return m_sErrorMessage;
	}

private:
	EnSRMError m_enErrorCode;
	QString m_sErrorMessage;
};

#endif //SRMERROR_H_52A7B72E_A4CF_423B_A57E_9640516FCB94