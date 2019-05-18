
#include "Observer/SRMObserverBase.h"
#include "Observer/SRMModuleSubject.h"

SRMModuleSubject* SRMModuleSubject::getInstance()
{
	static SRMModuleSubject s_oSRMModuleSubject;
	return &s_oSRMModuleSubject;
}

void SRMModuleSubject::release()
{
	foreach(SRMObserverBase* p, m_oObserverList)
	{
		delete p;
	}
	m_oObserverList.clear();

	foreach(SRMObserverFactory* p, m_oObserverFactoryList)
	{
		delete p;
	}
	m_oObserverFactoryList.clear();
}

void SRMModuleSubject::init()
{
	static bool bInited = false;

	if (!bInited)
	{

		foreach (SRMObserverFactory* p, m_oObserverFactoryList)
		{
			m_oObserverList.push_back(p->createObject());
		}

		for (auto it = m_oObserverList.begin(); it != m_oObserverList.end();)
		{
			if ((*it)->init())
			{
				it++;
			}
			else
			{
				delete (*it);
				it = m_oObserverList.erase(it);
			}
		}

		std::sort(m_oObserverList.begin(), m_oObserverList.end(),
			[&](SRMObserverBase* pLeft, SRMObserverBase* pRight)->bool
			{
				return pLeft->order() < pRight->order();
			});

		bInited = true;
	}
}

int SRMModuleSubject::doCheckedCount()
{
	int nCount = 0;
	foreach(SRMObserverBase * p, m_oObserverList)
	{
		nCount += p->checkedCount();
	}

	return nCount;
}

SRMModuleSubject::SRMModuleSubject()
{

}

SRMModuleSubject::~SRMModuleSubject()
{
}

void SRMModuleSubject::doOntify(VSSharedMemStruct* pSharedMemStruct, QList<QLabel*>& oLabelList)
{
	int nIndex = 0;
	foreach(SRMObserverBase* p, m_oObserverList)
	{
		int nCheckedCount = p->checkedCount();
		if (nCheckedCount > 0)
		{
			p->update(nIndex, pSharedMemStruct, oLabelList);
			nIndex += nCheckedCount;
		}	
	}
}

void SRMModuleSubject::doRegTrayMenu(QMenu* pTrayMenu)
{
	foreach(SRMObserverBase * p, m_oObserverList)
	{
		p->regTrayMenu(pTrayMenu);
	}
}

QList<ISRMCustomSettingInf*> SRMModuleSubject::doCustomSettingInfList()
{
	QList<ISRMCustomSettingInf*> oResultList;
	foreach(SRMObserverBase* p, m_oObserverList)
	{
		oResultList.append(p->customSettingInfList());
	}
	return oResultList;
}

void SRMModuleSubject::regObserverFactory(SRMObserverFactory* pObserverFactory)
{
	m_oObserverFactoryList.push_back(pObserverFactory);
}

