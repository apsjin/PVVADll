#pragma once
//Written by Ming Jin, 2018
//version 0.0
//Physical Optics (Zero Order Current Description on STL Triangles) by CUDA Computation
//Requires dense mesh!  1/8 lambda

//����Դ���� ��ת���ɵ���
//�������

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
	CUDAPOCalculation();//���캯��
	~CUDAPOCalculation();//��������

	void setModelFile(const std::string & file);


	static int getCUDAInfo(); // �õ�cuda����Ϣ�������ط�0��˵����֧��cuda

	int calculateSTL(double _freq, double _dis); // ����

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

	//�����ϵ
	void setFrequency(float _freq);
	void setFrequency(double _freq); //����
									 //��������
	void setReflectSTL(void* _polyData);

	//���䳡�ֲ���Ϊ���� -- ��Fields ��ƥ��
	void setPlaneApertureEField_D(vector<vector<complex<double>>> _Eu, vector<vector<complex<double>>> _Ev, Vector3 _u, Vector3 _v, Vector3 _poscen, float _ds, int _Nu, int _Nv);
	//���������Ϊ����//ע����һ��
	void setSTLCurrentSourceZeroOrder(void* _polyData, vector<complex<double>> _Hx, vector<complex<double>> _Hy, vector<complex<double>> _Hz);
	//����������泡�ֲ�
	void setOutputAperture(Vector3 _u, Vector3 _v, Vector3 _poscen, float _ds, int _Nu, int _Nv);


	//�����ϵ
	//���泡ʽ�ĵ���
	void setOutputAperture_2D(void);
	void getOutputField_2D(void);
	//λ���б�ʽ����� �ӿ�δ����
	void setOutputField_1D(void);
	void getOutputField_1D(void);

	//������泡���
	//void getOutApertureEx(complex<double>** &_Ex);
	//void getOutApertureEy(complex<double>** &_Ey);
	//void getOutApertureEz(complex<double>** &_Ez);
	void getOutApertureE(complex<double>** &_Eu, complex<double>** &_Ev);

	//�������������
	void getSTLlistHfield(vector<complex<double>> &_Hx, vector<complex<double>> &_Hy, vector<complex<double>> &_Hz);

	void SetReturnFloat(void(*returnFloat)(float, void*), void*_user);// ע��ص�����

private:
	//�ؼ�����Ƶ��
	float freq;
	//����������Ŀ
	int numSource;
	//���䳡���
	int numOut;

	//һά��άת��
	int N1_in, N2_in;
	int N1_out, N2_out;

	//�����ǲ���Ҳ��������void* + ǿ������ת���ķ�ʽ��

	//���䳡
	//��Ч������Ӧ�ų�
	cuComplex* Jx_in;	cuComplex* Jy_in;	cuComplex* Jz_in;
	//��Ч������Ӧ�糡
	cuComplex* Jmx_in;	cuComplex* Jmy_in;	cuComplex* Jmz_in;
	//0�׵�������
	//λ��
	float* px_in;	float* py_in;	float* pz_in;
	//������
	float* nx_in;	float* ny_in;	float* nz_in;
	//��Ƭ���
	float* ds_in;

	//���λ��
	float* px_out;	float* py_out;	float* pz_out;
	Vector3 u_out, v_out;

	//���䳡
	cuComplex* Ex_out;	cuComplex* Ey_out;	cuComplex* Ez_out;
	cuComplex* Hx_out;	cuComplex* Hy_out;	cuComplex* Hz_out;

	//�ļ�����
	std::string inputFieldFile;
	std::string stlMirrorFile;


	//ע��ص�����
	void(*returnFloat)(float, void*);
	void *user; // �ص���������ָ��
};

#endif // CUDAPOCalculation_H