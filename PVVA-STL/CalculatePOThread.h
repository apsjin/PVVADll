#pragma once
#ifndef CalculatePOThread_H
#define CalculatePOThread_H
#include "../CUDAPhysicalOptics/CUDAPhysicalOptics.h"
#include <QThread>


class CalculatePoThread : public QThread
{
	Q_OBJECT

public:
	CalculatePoThread(double fre, double dis, CUDAPhysicalOptics* _poDll)
	{
		this->fre = fre;
		this->dis = dis;
		this->poDll = _poDll;
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
	CUDAPhysicalOptics* poDll;
	double fre;
	double dis;

};

#endif // CalculatePoThread_H
