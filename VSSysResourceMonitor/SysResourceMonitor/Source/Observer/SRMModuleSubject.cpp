
#include "Observer/SRMObserverBase.h"
#include "Observer/SRMModuleSubject.h"

SRMModuleSubject* SRMModuleSubject::getInstance()
{
	static SRMModuleSubject s_oSRMModuleSubject;
	return &s_oSRMModuleSubject;
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

int SRMModuleSubject::getCheckedCount()
{
	int nCount = 0;
	foreach(SRMObserverBase * p, m_oObserverList)
	{
		if (p->isChecked())
			nCount++;
	}

	return nCount;
}

SRMModuleSubject::SRMModuleSubject()
{

}

SRMModuleSubject::~SRMModuleSubject()
{
	foreach(SRMObserverBase * p, m_oObserverList)
	{
		delete p;
	}

	foreach(SRMObserverFactory* p, m_oObserverFactoryList)
	{
		delete p;
	}
}

void SRMModuleSubject::doOntify(VSSharedMemStruct* pSharedMemStruct, QList<QLabel*>& oLabelList)
{
	int nIndex = 0;
	foreach(SRMObserverBase* p, m_oObserverList)
	{
		if (p->isChecked())
			p->update(nIndex++, pSharedMemStruct, oLabelList);
	}
}

void SRMModuleSubject::regTrayMenu(QMenu* pTrayMenu)
{
	foreach(SRMObserverBase * p, m_oObserverList)
	{
		p->regTrayMenu(pTrayMenu);
	}
}

QList<SRMObserverBase*>& SRMModuleSubject::getObserverList()
{
	return m_oObserverList;
}

void SRMModuleSubject::regObserverFactory(SRMObserverFactory* pObserverFactory)
{
	m_oObserverFactoryList.push_back(pObserverFactory);
}

