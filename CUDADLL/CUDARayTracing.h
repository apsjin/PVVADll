/*
*	created by liyun 2018/5/13
*   function 封装cuda 对外API
*   version 2.0 
	注意一次完整的镜面反射PO计算包括：1，计算表面电流，2，计算出射场或者下一个镜面点的表面电流
*/
#ifndef CUDARAYTRACING_H
#define CUDARAYTRACING_H

#include "Vector3.h"

class __declspec(dllexport) CUDARayTracing
{
public:
	CUDARayTracing();
	~CUDARayTracing();

	static int getCUDAInfo(); // 得到cuda的信息，若返回非0则说明不支持cuda

	int run(); // 计算光线追踪 出错返回非0

	void setSTL(void* polyData);
	void setRays(Vector3 **Plane, Vector3 ** n_Plane, int n, int m);

	bool* getIintersected() const { return intersected; }
	float* getProt() const { return prot; }
	float* getInter_x() const { return inter_x; }
	float* getInter_y() const { return inter_y; }
	float* getInter_z() const { return inter_z; }
	int * getSTLIndex() const { return STLIndex; }

private:
	int numPoints;
	int numSTL;

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

