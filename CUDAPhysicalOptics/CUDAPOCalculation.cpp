// CUDAPO.cpp : 定义 DLL 应用程序的导出函数。
//
#include "CUDAPOCalculation.h"
#include "CUDAFunction.h"
#include <vtkPoints.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include "cuVector3.h"
#include "cuComplexVector3.h"

//这个用来求解三角形的面积
double CalSquare(const Vector3 & A, const Vector3 & B,
	const Vector3 & C)
{
	double AB = (B - A).Length();
	double AC = (C - A).Length();
	double BC = (B - C).Length();
	double p1 = (AB + AC + BC) / 2;

	return sqrt(p1*(p1 - AB)*(p1 - BC)*(p1 - AC));
}

CUDAPOCalculation::CUDAPOCalculation() :
	freq(0.0),
	numSource(0),
	numOut(0),
	N1_in(0),
	N2_in(0),
	N1_out(0),
	N2_out(0),
	Jx_in(nullptr),
	Jy_in(nullptr),
	Jz_in(nullptr),
	Jmx_in(nullptr),
	Jmy_in(nullptr),
	Jmz_in(nullptr),
	px_in(nullptr),
	py_in(nullptr),
	pz_in(nullptr),
	nx_in(nullptr),
	ny_in(nullptr),
	nz_in(nullptr),
	ds_in(nullptr),
	px_out(nullptr),
	py_out(nullptr),
	pz_out(nullptr),
	Ex_out(nullptr),
	Ey_out(nullptr),
	Ez_out(nullptr),
	Hx_out(nullptr),
	Hy_out(nullptr),
	Hz_out(nullptr) {

}

CUDAPOCalculation::~CUDAPOCalculation() {
	if (Jx_in) { delete[] Jx_in;		Jx_in = nullptr; }
	if (Jy_in) { delete[] Jy_in;		Jy_in = nullptr; }
	if (Jz_in) { delete[] Jz_in;		Jz_in = nullptr; }

	if (Jmx_in) { delete[] Jmx_in;	Jmx_in = nullptr; }
	if (Jmy_in) { delete[] Jmy_in;	Jmy_in = nullptr; }
	if (Jmz_in) { delete[] Jmz_in;	Jmz_in = nullptr; }

	if (px_in) { delete[] px_in;		px_in = nullptr; }
	if (py_in) { delete[] py_in;		py_in = nullptr; }
	if (pz_in) { delete[] pz_in;		pz_in = nullptr; }

	if (nx_in) { delete[] nx_in;		nx_in = nullptr; }
	if (ny_in) { delete[] ny_in;		ny_in = nullptr; }
	if (nz_in) { delete[] nz_in;		nz_in = nullptr; }

	if (ds_in) { delete[] ds_in;		ds_in = nullptr; }

	if (px_out) { delete[] px_out;	px_out = nullptr; }
	if (py_out) { delete[] py_out;	py_out = nullptr; }
	if (pz_out) { delete[] pz_out;	pz_out = nullptr; }

	if (Ex_out) { delete[] Ex_out;	Ex_out = nullptr; }
	if (Ey_out) { delete[] Ey_out;	Ey_out = nullptr; }
	if (Ez_out) { delete[] Ez_out;	Ez_out = nullptr; }

	if (Hx_out) { delete[] Hx_out;	Hx_out = nullptr; }
	if (Hy_out) { delete[] Hy_out;	Hy_out = nullptr; }
	if (Hz_out) { delete[] Hz_out;	Hz_out = nullptr; }

}

void CUDAPOCalculation::setFrequency(double _freq) {
	CUDAPOCalculation::freq = float(_freq);
}
void CUDAPOCalculation::setFrequency(float _freq) {
	CUDAPOCalculation::freq = _freq;
}

//设置作为输出位置的STL
void CUDAPOCalculation::setReflectSTL(void* _polyData) {
	vtkPolyData* polyData = (vtkPolyData*)(_polyData);
	numOut = polyData->GetNumberOfCells();

	N1_out = numOut; N2_out = 1;

	px_out = new float[numOut];
	py_out = new float[numOut];
	pz_out = new float[numOut];

	vtkIdList * p;
	double * point;
	double x1, y1, z1, x2, y2, z2, x3, y3, z3;
	//读取各点的位置
	for (int i = 0; i < numOut; i++) {
		p = polyData->GetCell(i)->GetPointIds();
		//点1
		point = polyData->GetPoint(p->GetId(0));
		x1 = point[0]; y1 = point[1]; z1 = point[2];
		//点2
		point = polyData->GetPoint(p->GetId(1));
		x2 = point[0]; y2 = point[1]; z2 = point[2];
		//点3
		point = polyData->GetPoint(p->GetId(2));
		x3 = point[0]; y3 = point[1]; z3 = point[2];

		px_out[i] = float((x1 + x2 + x3) / 3.0);
		py_out[i] = float((y1 + y2 + y3) / 3.0);
		pz_out[i] = float((z1 + z2 + z3) / 3.0);

	}

	//计算结果数组
	Hx_out = new cuComplex[numOut];
	Hy_out = new cuComplex[numOut];
	Hz_out = new cuComplex[numOut];

}
//设置作为输出的口面场位置
void CUDAPOCalculation::setOutputAperture(Vector3 _u, Vector3 _v, Vector3 _poscen, float _ds, int _Nu, int _Nv) {
	//数目
	N1_out = _Nu;
	N2_out = _Nv;
	numOut = N1_out*N2_out;
	//方向向量
	u_out = _u;
	v_out = _v;
	//申请内存：场量
	Ex_out = new cuComplex[numOut];
	Ey_out = new cuComplex[numOut];
	Ez_out = new cuComplex[numOut];
	//申请内存：位置
	px_out = new float[numOut];
	py_out = new float[numOut];
	pz_out = new float[numOut];

	int ii = 0;
	for (int i = 0; i < N1_out; i++) {
		for (int j = 0; j < N2_out; j++) {
			//设置位置
			px_out[ii] = float(_poscen.x + (-(_Nu - 1) / 2.0 + i)*_ds*_u.x + (-(_Nv - 1) / 2.0 + j)*_ds*_v.x);
			py_out[ii] = float(_poscen.y + (-(_Nu - 1) / 2.0 + i)*_ds*_u.y + (-(_Nv - 1) / 2.0 + j)*_ds*_v.y);
			pz_out[ii] = float(_poscen.z + (-(_Nu - 1) / 2.0 + i)*_ds*_u.z + (-(_Nv - 1) / 2.0 + j)*_ds*_v.z);
			ii = ii + 1;
		}
	}
}

//设置作为输入的STL及场结果
void CUDAPOCalculation::setSTLCurrentSourceZeroOrder(void* _polyData, vector<complex<double>> _Hx, vector<complex<double>> _Hy, vector<complex<double>> _Hz) {

	vtkPolyData* polyData = (vtkPolyData*)(_polyData);
	//源点数目
	numSource = polyData->GetNumberOfCells();
	N1_in = numSource; N2_in = 1;

	px_in = new float[numSource];
	py_in = new float[numSource];
	pz_in = new float[numSource];
	nx_in = new float[numSource];
	ny_in = new float[numSource];
	nz_in = new float[numSource];

	vtkIdList * p;
	double * point;
	Vector3 P1;
	Vector3 P2;
	Vector3 P3;
	Vector3 normal;

	//读取各点的位置并产生 填充位置、法向、以及面积数组
	for (int i = 0; i < numSource; i++) {
		p = polyData->GetCell(i)->GetPointIds();
		//点1
		point = polyData->GetPoint(p->GetId(0));
		P1.set(point[0], point[1], point[2]);
		//点2
		point = polyData->GetPoint(p->GetId(1));
		P2.set(point[0], point[1], point[2]);
		//点3
		point = polyData->GetPoint(p->GetId(2));
		P3.set(point[0], point[1], point[2]);
		//法向
		normal = (P1 - P2).Cross(P1 - P3);
		normal.Normalization();

		px_in[i] = float((P1.x + P2.x + P3.x) / 3.0);
		py_in[i] = float((P1.y + P2.y + P3.y) / 3.0);
		pz_in[i] = float((P1.z + P2.z + P3.z) / 3.0);

		nx_in[i] = float(normal.x);
		ny_in[i] = float(normal.y);
		nz_in[i] = float(normal.z);

		//polyData->GetCell(i)
		//计算面元面积
		ds_in[i] = float(CalSquare(P1, P2, P3));
	}

	//申请电流源分布
	Jx_in = new cuComplex[numSource];	Jy_in = new cuComplex[numSource];	Jz_in = new cuComplex[numSource];

	//利用叉乘关系，从输入的场分布中提取表面电流
	// J = 2 n cross H
	cuComplexVector3 TempCVH;
	cuComplexVector3 TempCVJ;
	cuVector3 TempN;
	for (int i = 0; i < numSource; i++) {
		TempN = SetcuVector3(nx_in[i], ny_in[i], nz_in[i]);
		TempCVH = SetcuComplexVector3f(_Hx[i].real(), _Hx[i].imag(),
			_Hy[i].real(), _Hy[i].imag(),
			_Hz[i].real(), _Hz[i].imag());
		TempCVJ = cuComplexVector3Crossfc(TempN, TempCVH);
		TempCVJ = cuComplexVector3Mulcf(TempCVJ, float(2.0));
		Jx_in[i] = TempCVJ.x;
		Jy_in[i] = TempCVJ.y;
		Jz_in[i] = TempCVJ.z;
	}
}

//设置作为输入的口面场分布
void CUDAPOCalculation::setPlaneApertureEField_D(vector<vector<complex<double>>> _Eu, vector<vector<complex<double>>> _Ev, Vector3 _u, Vector3 _v, Vector3 _poscen, float _ds, int _Nu, int _Nv) {
	//点数目
	N1_in = _Nu;
	N2_in = _Nv;
	numSource = N1_in*N2_in;
	//法向
	Vector3 facenormal = _u.Cross(_v);
	facenormal.Normalization();
	Jmx_in = new cuComplex[numSource];
	Jmy_in = new cuComplex[numSource];
	Jmz_in = new cuComplex[numSource];

	px_in = new float[numSource];
	py_in = new float[numSource];
	pz_in = new float[numSource];

	nx_in = new float[numSource];
	ny_in = new float[numSource];
	nz_in = new float[numSource];

	ds_in = new float[numSource];

	complex<double> Ex_in;
	complex<double> Ey_in;
	complex<double> Ez_in;
	cuVector3 Normal;
	Normal = SetcuVector3(float(facenormal.x), float(facenormal.y), float(facenormal.z));
	cuComplexVector3 EField;
	cuComplexVector3 Jm;
	int ii = 0;
	for (int i = 0; i < N1_in; i++) {
		for (int j = 0; j < N2_in; j++) {
			//电场化为等效磁流
			// Jm = -2*n cross E
			Ex_in = _Eu[i][j] * _u.x + _Ev[i][j] * _v.x;
			Ey_in = _Eu[i][j] * _u.y + _Ev[i][j] * _v.y;
			Ez_in = _Eu[i][j] * _u.z + _Ev[i][j] * _v.z;
			EField = SetcuComplexVector3f(Ex_in.real(), Ex_in.imag(), Ey_in.real(), Ey_in.imag(), Ez_in.real(), Ez_in.imag());
			Jm = cuComplexVector3Crossfc(Normal, EField);
			Jm = cuComplexVector3Mulcf(Jm, float(-2.0));
			Jmx_in[ii] = Jm.x;
			Jmy_in[ii] = Jm.y;
			Jmz_in[ii] = Jm.z;
			//设置位置
			px_in[ii] = float(_poscen.x + (-(_Nu - 1) / 2.0 + i)*_ds*_u.x + (-(_Nv - 1) / 2.0 + j)*_ds*_v.x);
			py_in[ii] = float(_poscen.y + (-(_Nu - 1) / 2.0 + i)*_ds*_u.y + (-(_Nv - 1) / 2.0 + j)*_ds*_v.y);
			pz_in[ii] = float(_poscen.z + (-(_Nu - 1) / 2.0 + i)*_ds*_u.z + (-(_Nv - 1) / 2.0 + j)*_ds*_v.z);
			//设置法向
			nx_in[ii] = Normal.x;
			ny_in[ii] = Normal.y;
			nz_in[ii] = Normal.z;
			//设置面积
			ds_in[ii] = _ds*_ds;
			ii = ii + 1;
		}
	}
}

//从场分布计算表面电流
int CUDAPOCalculation::calculateF2S() {
	//这是在kernal.cu里写的调用函数
	return RunJM2H(freq, numSource, px_in, py_in, pz_in, ds_in, Jmx_in, Jmy_in, Jmz_in, numOut, px_out, py_out, pz_out, Hx_out, Hy_out, Hz_out);
}
//从表面电流计算场分布
int CUDAPOCalculation::calculateS2F() {
	//这是在kernal里写的函数
	return RunJ2E(freq, numSource, px_in, py_in, pz_in, ds_in, Jx_in, Jy_in, Jz_in, numOut, px_out, py_out, pz_out, Ex_out, Ey_out, Ez_out);
}

//获取输出结果输出 口面场分布
void CUDAPOCalculation::getOutApertureE(complex<double>** &_Eu, complex<double>** &_Ev) {
	complex<double> tempEx;
	complex<double> tempEy;
	complex<double> tempEz;
	int ii = 0;
	for (int i = 0; i < N1_out; i++) {
		for (int j = 0; j < N2_out; j++) {
			tempEx = complex<double>(Ex_out[ii].x, Ex_out[ii].y);
			tempEy = complex<double>(Ey_out[ii].x, Ey_out[ii].y);
			tempEz = complex<double>(Ez_out[ii].x, Ez_out[ii].y);
			_Eu[i][j] = tempEx*u_out.x + tempEy*u_out.y + tempEz*u_out.z;
			_Ev[i][j] = tempEx*v_out.x + tempEy*v_out.y + tempEz*v_out.z;
			ii = ii + 1;
		}
	}
}

void CUDAPOCalculation::getSTLlistHfield(vector<complex<double>> &_Hx, vector<complex<double>> &_Hy, vector<complex<double>> &_Hz) {
	for (int i = 0; i < numOut; i++) {
		_Hx[i] = complex<double>(Hx_out[i].x, Hx_out[i].y);
		_Hy[i] = complex<double>(Hy_out[i].x, Hy_out[i].y);
		_Hz[i] = complex<double>(Hz_out[i].x, Hz_out[i].y);
	}
}

void CUDAPOCalculation::SetReturnFloat(void(*returnFloat)(float, void *), void * _user)
{
	this->returnFloat = returnFloat;
	this->user = _user;
}

