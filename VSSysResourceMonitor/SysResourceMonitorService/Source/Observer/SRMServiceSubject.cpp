
#include "Observer/SRMServiceSubject.h"
#include "Observer/SRMServiceObserverBase.h"

SRMServiceSubject* SRMServiceSubject::getInstance()
{
	static SRMServiceSubject s_oSRMServiceSubject;
	return &s_oSRMServiceSubject;
}

void SRMServiceSubject::doOntify()
{
	foreach(SRMServiceObserverBase *p, m_oObserverList)
	{
		p->update();
	}
}

void SRMServiceSubject::regObserver(SRMServiceObserverBase* pObserver)
{
	if (!pObserver->init())
	{
		delete pObserver;
		return;
	}		

	m_oObserverList.push_back(pObserver);
}

SRMServiceSubject::SRMServiceSubject()
{

}

SRMServiceSubject::~SRMServiceSubject()
{
	foreach(SRMServiceObserverBase *p, m_oObserverList)
	{
		delete p;
	}
}
