/*
*	created by liyun 2018/5/13
*   function ��װcuda ����API
*   version 2.0 
*/
#ifndef CUDARAYTRACING_H
#define CUDARAYTRACING_H

#include "Vector3.h"

class __declspec(dllexport) CUDARayTracing
{
public:
	CUDARayTracing();
	~CUDARayTracing();

	static int getCUDAInfo(); // �õ�cuda����Ϣ�������ط�0��˵����֧��cuda

	int run(); // �������׷�� �����ط�0

	void setSTL(void* polyData);
	void setRays(Vector3 **Plane, Vector3 ** n_Plane, int n, int m);

	bool* getIintersected() const { return intersected; }
	float* getProt() const { return prot; }
	float* getInter_x() const { return inter_x; }
	float* getInter_y() const { return inter_y; }
	float* getInter_z() const { return inter_z; }
	int * getSTLIndex() const { return STLIndex; }

private:
	int numSTL;	
	int numPoints;

	bool* intersected;
	int* STLIndex;
	float* prot;
	float* inter_x;
	float* inter_y;
	float* inter_z;
	float* psourcex;
	float* psourcey;
	float* psourcez;
	float* pdirx;
	float* pdiry;
	float* pdirz;

	float* stlp1x;
	float* stlp1y;
	float* stlp1z;
	float* stlp2x;
	float* stlp2y;
	float* stlp2z;
	float* stlp3x;
	float* stlp3y;
	float* stlp3z;
	
	
};

#endif // CUDARAYTRACING_H

