/*
*	created by liyun 2017/12/27
*   function ��ʾ�� 3D 2D ��λ ���� dB
*   version 2.0 �ع��˱������ݵ����ݽṹ
*/
#ifndef FIELD_H
#define FIELD_H

#include <vector>
#include <fstream>
#include <complex>
#include <String>

#include "GraphTrans.h"

#include "Vector3.h"

class Field
{
public:
	Field();
	~Field();

	void init();

	void freeMemory();
	void allocateMemory();

	void setNM(int N, int M);
	void getNM(int& N, int &M) const;
	void setPlane(const GraphTrans & _graphTransPara, double _ds);

	void setField(complex<double> ** _Ex, complex<double> ** _Ey, complex<double> ** _Ez,
		complex<double> ** _Hx, complex<double> ** _Hy, complex<double> ** _Hz);

	const vector<vector<complex<double>>>& getEx() const;
	const vector<vector<complex<double>>>& getEy() const;

	void setDs(double _Ds);
	double getDs() const;

	//�õ����Ĳ���
	void getSourcePara(GraphTrans & _graphTransPara, 
		int & _N_width, int & _M_depth, double &_ds) const;

	double getWidth() const { return data[7]; }
	double getDepth() const { return data[8]; }

	void save(const std::string &fileName) const;

	void readData();

	void setFileAddress(const string & file);

protected:
	GraphTrans graphTrans; // ��תƽ�Ʋ���
	vector<double> data;

	// ������
	vector<vector<complex<double>>> Ex, Ey, Ez;
	vector<vector<complex<double>>> Hx, Hy, Hz;

	int N_width, M_depth;  //N_width = para[0] /ds

	double ds;
	string fileAddress;
	bool isSource;

};

#endif // !DRAW_H

