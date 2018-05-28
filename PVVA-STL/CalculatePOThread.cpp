#include "CalculatePoThread.h"


void CalculatePoThread::run()
{
	poDll->SetReturnFloat(setSlaverValue, this);
	poDll->calculate(fre, dis);
}


void CalculatePoThread::setSlaverValue(float val, void *user)
{
	((CalculatePoThread*)user)->sendSlaverValue(val);
}

void CalculatePoThread::killFDTD()
{
	deleteLater();
}
