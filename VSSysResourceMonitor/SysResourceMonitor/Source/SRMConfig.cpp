
#include <QDir>
#include <QApplication>
#include <QJsonParseError>
#include <windows.h>
#include "SRMError.h"
#include "SRMConfig.h"

SRMConfig* SRMConfig::getInstance()
{
	static SRMConfig s_oSRMConfig;
	return &s_oSRMConfig;
}

SRMConfig::~SRMConfig()
{
	m_oFile.flush();
	m_oFile.close();
}

void SRMConfig::saveConfig(QString sMainKey, QString sSubkey, QJsonValue oValue)
{
	QJsonObject oJsonObject = m_oConfigJsonObject[sMainKey].toObject();
	if (oJsonObject[sSubkey] == oValue)
		return;

	oJsonObject[sSubkey] = oValue;
	m_oConfigJsonObject[sMainKey] = oJsonObject;
	saveConfigToFile();
}

QJsonValue SRMConfig::readConfig(QString sMainKey, QString sSubkey)
{
	return m_oConfigJsonObject[sMainKey].toObject()[sSubkey];
}

SRMConfig::SRMConfig()
{
	QString sUserConfigDir = QDir::homePath() + "/Documents/SysResourceMonitor/";
	QDir oUserDir(sUserConfigDir);
	if (!oUserDir.exists() && !oUserDir.mkpath(sUserConfigDir))
		throw SRMError(esrmeConfigError);

	QString sUserConfigFile = sUserConfigDir + "/SRMUserConfig.json";
	if (!QFile::exists(sUserConfigFile))
	{
		MessageBox(nullptr, L"配置文件丢失，将使用默认配置", L"SysResourceMonitorWarning", S_OK);
		copyFromDefault(sUserConfigFile);
	}

	m_oFile.setFileName(sUserConfigFile);
	if (!m_oFile.open(QIODevice::ReadWrite))
		throw SRMError(esrmeConfigError);

	readConfigFromFile();
}

void SRMConfig::readConfigFromFile()
{
	QByteArray oJsonByte = m_oFile.readAll();

	QJsonParseError oError;
	QJsonDocument oJsonDocument = QJsonDocument::fromJson(oJsonByte, &oError);
	if (oError.error != QJsonParseError::NoError)
	{
		MessageBox(nullptr, L"配置文件损坏，将使用默认配置", L"SysResourceMonitorWarning", S_OK);
		m_oFile.close();
		copyFromDefault(m_oFile.fileName());
		if (!m_oFile.open(QIODevice::ReadWrite))
			throw SRMError(esrmeConfigError);

		// 再读一次
		oJsonByte = m_oFile.readAll();
		oJsonDocument = QJsonDocument::fromJson(oJsonByte, &oError);
	}

	if (oError.error != QJsonParseError::NoError || oJsonDocument.isNull() || !oJsonDocument.isObject())
	{		
		return;
	}

	m_oConfigJsonObject = oJsonDocument.object();
}

void SRMConfig::saveConfigToFile()
{
	QJsonDocument oJsonDocument(m_oConfigJsonObject);
	QByteArray oJsonByte = oJsonDocument.toJson(QJsonDocument::Indented);
	m_oFile.resize(0);
	m_oFile.write(oJsonByte);
	m_oFile.flush();	
}

void SRMConfig::copyFromDefault(QString sDestPath)
{
	QString sDefaultConfigFile = qApp->applicationDirPath() + "/SRMDefaultConfig.json";
	if (QFile::exists(sDefaultConfigFile))
	{
		QFile::copy(sDefaultConfigFile, sDestPath);
	}
}
