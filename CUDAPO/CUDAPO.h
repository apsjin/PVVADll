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

	static int getCUDAInfo(); // �õ�cuda����Ϣ�������ط�0��˵����֧��cuda

	int run(); // ����

	//MirrorReflectionMode;
	int calculateF2S();
	int calculateS2S();
	int calculateS2F();

	int calculateF2F();



	int calculateField2Current();
	int calculateCurrent2Field();

	//�����ϵ
	//��������
	void setSTL(void* polyData);

	//���䳡�ֲ���Ϊ����
	void setIncidentFieldSource(void);
	//���������Ϊ����
	void setSurfaceCurrentSource(void);

	//�����ϵ
	//���泡ʽ�ĵ���
	void setOutputAperture_2D(void);
	void getOutputField_2D(void);
	//λ���б�ʽ�����
	void setOutputField_1D(void);
	void getOutputField_1D(void);
	//λ���б�ʽ�����
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
	//����������Ŀ
	int numSTL;
	//���䳡���
	int numPoints;

	//�����ǲ���Ҳ��������void* + ǿ������ת���ķ�ʽ��

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