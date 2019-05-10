#include <iostream>

#include "VSDeskBandOperation.h"

using namespace std;

int main()
{
	IVSDeskBandOperation* pOperationInf = VSDeskBandOperation::getInstance();
	bool bResult = pOperationInf->isRegistered();
	if (!bResult)
	{
		bResult = pOperationInf->doReg();
	}

	bResult = pOperationInf->isRegistered();
	bResult = pOperationInf->visible();
	bResult = pOperationInf->show(false);
	bResult = pOperationInf->visible();
	bResult = pOperationInf->hide();
	bResult = pOperationInf->visible();
	bResult = pOperationInf->show(true);
	bResult = pOperationInf->hide();
	bResult = pOperationInf->show(true);
	bResult = pOperationInf->doUnreg();
	bResult = pOperationInf->isRegistered();
	return 0;
}