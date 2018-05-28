#pragma once
//Written by Ming Jin, 2018
//version 0.0
//Physical Optic (Zero Order STL) by CUDA Computation
//Requires dense mesh!

#ifndef CUDAPHYSICALOPTICS_H
#define CUDAPHYSICALOPTICS_H

#include "Vector3.h"

class __declspec(dllexport) CUDAPhysicalOptics
{
public:
	CUDAPhysicalOptics();
	CUDAPhysicalOptics();

	static int getCUDAInfo(); // 得到cuda的信息，若返回非0则说明不支持cuda

	int run(); // 计算

	//MirrorReflectionMode;
	int calculateF2S();
	int calculateS2S();
	int calculateS2F();

	int calculateF2F();



	int calculateField2Current();
	int calculateCurrent2Field();

	//输入关系
	//反射网格
	void setSTL(void* polyData);

	//入射场分布作为激励
	void setIncidentFieldSource(void);
	//表面电流作为激励
	void setSurfaceCurrentSource(void);

	//输出关系
	//口面场式的调用
	void setOutputAperture_2D(void);
	void getOutputField_2D(void);
	//位置列表式的输出
	void setOutputField_1D(void);
	void getOutputField_1D(void);
	//位置列表式的输出
	void setOutputCurrent(void);
	void getOutputCurrent(void);

	void setRays(Vector3 **Plane, Vector3 ** n_Plane, int n, int m);

	bool* getIintersected() const { return intersected; }
	float* getProt() const { return prot; }
	float* getInter_x() const { return inter_x; }
	float* getInter_y() const { return inter_y; }
	float* getInter_z() const { return inter_z; }
	int * getSTLIndex() const { return STLIndex; }

private:
	//三角网格数目
	int numSTL;
	//入射场格点
	int numPoints;

	//这里是不是也可以是用void* + 强制类型转换的方式？

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

#endif // CUDAPhysicalOptics_H