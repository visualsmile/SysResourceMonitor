#ifndef SRMREGMANAGER_H_9E8B0119_AA06_4728_82FD_974C40FF05A1
#define SRMREGMANAGER_H_9E8B0119_AA06_4728_82FD_974C40FF05A1

#include "SRMRegManager.h"
#include <QSettings>
#include <QDir>
#include <QCoreApplication>
#include <windows.h>

const QString s_sAutoRunRegPath = "HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Run";

SRMRegManager* SRMRegManager::getInstance()
{
	static SRMRegManager s_oSRMRegManager;
	return &s_oSRMRegManager;
}


SRMRegManager::~SRMRegManager()
{

}

bool SRMRegManager::setAutoRun()
{
	HINSTANCE hRegister = ShellExecute(0, L"runas", L"SysResourceMonitor_1_设置开机自启.bat", NULL, NULL, SW_HIDE);
	if ((__int64)hRegister < 32)
	{
		//MessageBox(NULL, L"注册表修改失败。\n请重试", L"ERROR", MB_OK);
		return false;
	}
	WaitForSingleObject(hRegister, -1);
	return true;
}

bool SRMRegManager::cancelAutoRun()
{
	HINSTANCE hRegister = ShellExecute(0, L"runas", L"SysResourceMonitor_2_取消开机自启.bat", NULL, NULL, SW_HIDE);
	if ((__int64)hRegister < 32)
	{
		//MessageBox(NULL, L"注册表修改失败。\n请重试", L"ERROR", MB_OK);
		return false;
	}
	WaitForSingleObject(hRegister, -1);
	return true;
}

bool SRMRegManager::isAutoRun()
{
	QSettings reg(s_sAutoRunRegPath, QSettings::NativeFormat);
	QString strAppPath = "\"" + QDir::toNativeSeparators(QCoreApplication::applicationFilePath()) + "\"" + " AutoRun";
	QString sRegValue = reg.value("SysResourceMonitorAutoRun").toString();
	if (strAppPath == sRegValue)
		return true;
	else
		return false;
}

bool SRMRegManager::saveCustomInfo(QString& sKey, QString& sValue)
{
	return true;
}

QString SRMRegManager::readCustomInfo(QString& sKey)
{
	return "";
}

bool SRMRegManager::regWrite(QString sRegPath, QString sKey, QString sValue)
{
	QString sParam = sRegPath + " " + sKey + " " + sValue;
	HINSTANCE hRegister = ShellExecute(0, L"runas", L"SysResourceMonitor_0_RegEditor.bat", sParam.toStdWString().c_str(), NULL, SW_HIDE);
	if ((__int64)hRegister < 32)
	{
		//MessageBox(NULL, L"注册表修改失败。\n请重试", L"ERROR", MB_OK);
		return false;
	}
	WaitForSingleObject(hRegister, -1);
	return true;
}

SRMRegManager::SRMRegManager()
{

}

#endif //SRMREGMANAGER_H_9E8B0119_AA06_4728_82FD_974C40FF05A1

