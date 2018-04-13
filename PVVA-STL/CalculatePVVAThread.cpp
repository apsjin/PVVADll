#include "CalculatePVVAThread.h"


void CalculatePVVAThread::run()
{
	pVVADll->SetReturnFloat(setSlaverValue, this);
	pVVADll->calculate(fre, dis);
}


void CalculatePVVAThread::setSlaverValue(float val, void *user)
{
	((CalculatePVVAThread*)user)->sendSlaverValue(val);
}

void CalculatePVVAThread::killFDTD()
{
	deleteLater();
}
