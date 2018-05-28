#pragma once
//Written by Ming Jin, 2018
//version 0.0
//Physical Optics (Zero Order Current Description on STL Triangles) by CUDA Computation
//Requires dense mesh!  1/8 lambda

//输入源：场 会转化成电流
//输出：场

#ifndef CUDAPOCALCULATION_H
#define CUDAPOCALCULATION_H

#include "Vector3.h"
#include <complex>
#include <vector>
#include <cuComplex.h>
//#include "cuVector3.h"

using namespace std;

class CUDAPOCalculation
{
public:
	CUDAPOCalculation();//构造函数
	~CUDAPOCalculation();//析构函数

	void setModelFile(const std::string & file);


	static int getCUDAInfo(); // 得到cuda的信息，若返回非0则说明不支持cuda

	int calculateSTL(double _freq, double _dis); // 计算

												 //MirrorReflectionMode;
	int calculateF2S();
	int calculateS2S();
	int calculateS2F();
	//ApertureEField to anywhere EField;
	int calculateF2F();

	//ComputeHuygensAperture
	int calculateHuygens();


	int calculateField2Current();
	int calculateCurrent2Field();

	//输入关系
	void setFrequency(float _freq);
	void setFrequency(double _freq); //重载
									 //反射网格
	void setReflectSTL(void* _polyData);

	//入射场分布作为激励 -- 与Fields 相匹配
	void setPlaneApertureEField_D(vector<vector<complex<double>>> _Eu, vector<vector<complex<double>>> _Ev, Vector3 _u, Vector3 _v, Vector3 _poscen, float _ds, int _Nu, int _Nv);
	//表面电流作为激励//注意是一列
	void setSTLCurrentSourceZeroOrder(void* _polyData, vector<complex<double>> _Hx, vector<complex<double>> _Hy, vector<complex<double>> _Hz);
	//设置输出口面场分布
	void setOutputAperture(Vector3 _u, Vector3 _v, Vector3 _poscen, float _ds, int _Nu, int _Nv);


	//输出关系
	//口面场式的调用
	void setOutputAperture_2D(void);
	void getOutputField_2D(void);
	//位置列表式的输出 接口未定义
	void setOutputField_1D(void);
	void getOutputField_1D(void);

	//输出口面场结果
	//void getOutApertureEx(complex<double>** &_Ex);
	//void getOutApertureEy(complex<double>** &_Ey);
	//void getOutApertureEz(complex<double>** &_Ez);
	void getOutApertureE(complex<double>** &_Eu, complex<double>** &_Ev);

	//输出表面电流结果
	void getSTLlistHfield(vector<complex<double>> &_Hx, vector<complex<double>> &_Hy, vector<complex<double>> &_Hz);

	void SetReturnFloat(void(*returnFloat)(float, void*), void*_user);// 注册回调函数

private:
	//关键参数频率
	float freq;
	//三角网格数目
	int numSource;
	//入射场格点
	int numOut;

	//一维二维转换
	int N1_in, N2_in;
	int N1_out, N2_out;

	//这里是不是也可以是用void* + 强制类型转换的方式？

	//入射场
	//等效电流对应磁场
	cuComplex* Jx_in;	cuComplex* Jy_in;	cuComplex* Jz_in;
	//等效磁流对应电场
	cuComplex* Jmx_in;	cuComplex* Jmy_in;	cuComplex* Jmz_in;
	//0阶电流表述
	//位置
	float* px_in;	float* py_in;	float* pz_in;
	//法向量
	float* nx_in;	float* ny_in;	float* nz_in;
	//面片面积
	float* ds_in;

	//输出位置
	float* px_out;	float* py_out;	float* pz_out;
	Vector3 u_out, v_out;

	//出射场
	cuComplex* Ex_out;	cuComplex* Ey_out;	cuComplex* Ez_out;
	cuComplex* Hx_out;	cuComplex* Hy_out;	cuComplex* Hz_out;

	//文件输入
	std::string inputFieldFile;
	std::string stlMirrorFile;


	//注册回调函数
	void(*returnFloat)(float, void*);
	void *user; // 回调函数的类指针
};

#endif // CUDAPOCalculation_H