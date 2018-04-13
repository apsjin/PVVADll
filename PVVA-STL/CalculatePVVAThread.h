#ifndef CalculatePVVAThread_H
#define CalculatePVVAThread_H
#include "../PVVADll/PVVADll.h"
#include <QThread>


class CalculatePVVAThread : public QThread
{
	Q_OBJECT

public:
	CalculatePVVAThread(double fre, double dis, PVVADll* pVVADll)
	{
		this->fre = fre;
		this->dis = dis;
		this->pVVADll = pVVADll;
	}
	//~CalculatePVVAThread();
	static void setSlaverValue(float, void*);

signals:
	void sendSlaverValue(int);

	public slots:

	void killFDTD();

protected:
	void run();

private:
	PVVADll* pVVADll;
	double fre;
	double dis;

};

#endif // CalculatePVVAThread_H