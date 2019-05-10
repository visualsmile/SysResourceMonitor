#ifndef SRMCONFIG_H_E1A6D371_1629_4B70_B480_1E0DF73581BF
#define SRMCONFIG_H_E1A6D371_1629_4B70_B480_1E0DF73581BF
/************************************************************************/
/* ≈‰÷√π‹¿Ì                                                                     */
/************************************************************************/
#include <QString>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonDocument>
#include <QFile>
#include <QSet>

class SRMConfig
{
public:
	static SRMConfig* getInstance();
	~SRMConfig();

public:
	void saveConfig(QString sMainKey, QString sSubkey, QJsonValue oValue);
	QJsonValue readConfig(QString sMainKey, QString sSubKey);

private:
	SRMConfig();
	void readConfigFromFile();
	void saveConfigToFile();

private:
	void copyFromDefault(QString sDestPath);

private:
	QJsonObject m_oConfigJsonObject;
	QFile m_oFile;
};

#endif //SRMCONFIG_H_E1A6D371_1629_4B70_B480_1E0DF73581BF