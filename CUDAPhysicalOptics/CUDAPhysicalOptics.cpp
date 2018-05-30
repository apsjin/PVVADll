#include "CUDAPhysicalOptics.h"
#include "CUDAPOCalculation.h"
#include "GraphTrans.h"
#include "Field.h"
#include "STLMirror.h"
#include "Position3D.h"//位置
#include "Matrix4D.h"	//转移矩阵
#include "Vector3D.h"	//向量
#include "Raytracing.h"
#include "Constant_Val.h"
#include "GEMS_Memory.h"
using namespace calculation;
//using namespace std;

double CalDistance(const Vector3 &a, const Vector3 &b)
{
	return pow(pow((a.x - b.x), 2) + pow((a.y - b.y), 2) + pow((a.z - b.z), 2), 0.5);
}
void updateSource_n(const Vector3& new_n, GraphTrans & _gt)
{
	//new_n.Normalization();
	if (new_n.x != 0 || new_n.y != 0 || new_n.z != 1)
	{
		Vector3 rotate_axis = Vector3(0, 0, 1).Cross(new_n); // 旋转轴
		double rotate_theta = acos(Vector3(0, 0, 1).Dot(new_n));
		rotate_theta = rotate_theta / Pi * 180;
		_gt.updateRotate(rotate_axis, rotate_theta);
	}
	else
	{
		_gt.updateRotate(Vector3(0, 0, 1), 0);
	}
}


CUDAPhysicalOptics::CUDAPhysicalOptics()
{
	this->returnFloat = NULL;
	this->user = NULL;
	//FieldName = NULL;
	//STLName = NULL;
	//this->FieldName.resize(100);
	//this->STLName.resize(100);
}

CUDAPhysicalOptics::~CUDAPhysicalOptics()
{
	//if (FieldName) delete[] FieldName;
	//if (STLName) delete[] STLName;
}

void CUDAPhysicalOptics::setInputFile(const std::string & file)
{	
	
	FieldName = file;
	/*
	int size = file.size();
	FieldName = new char[size];
	for (int i = 0; i < size; i++) {
		FieldName[i] = file.c_str()[i];
	}*/
}

void CUDAPhysicalOptics::setModelFile(const std::string & file)
{
	STLName = file;
	/*
	int size = file.size();
	STLName = new char[size];
	for (int i = 0; i < size; i++) {
		STLName[i] = file.c_str()[i];
	}*/

}

void CUDAPhysicalOptics::calculate(double fre, double dis)
{	
	//读取输入场分布-文件
	Field inputField;
	inputField.setFileAddress(FieldName);
	inputField.readData();
	//读取计算网格-文件
	STLMirror stlMirror;
	stlMirror.setNameFile(STLName);
	stlMirror.readData();
	
	GraphTrans gt_inc;//入射面的位置和方向信息
	int M, N;
	double ds;//离散间隔
	Vector3 u_input;	u_input.set(1.0, 0.0, 0.0);
	Vector3 v_input;	v_input.set(0.0, 1.0, 0.0);
	Vector3 n_input;	n_input.set(0.0, 0.0, 1.0);
	Vector3 p_cen_in;
	Vector3 p_center;
	inputField.getSourcePara(gt_inc, M, N, ds);
	//口面中心位置
	p_cen_in.set(gt_inc.getTrans_x(), gt_inc.getTrans_y(), gt_inc.getTrans_z());

	//设置旋转：
	Vector3D  RotateAxis_in(gt_inc.getRotate_x(), gt_inc.getRotate_y(), gt_inc.getRotate_z());
	Matrix4D  RotateMatrix_in = Matrix4D::getRotateMatrix(gt_inc.getRotate_theta(), RotateAxis_in);

	u_input = RotateMatrix_in*u_input;
	v_input = RotateMatrix_in*v_input;
	n_input = RotateMatrix_in*n_input;

	vector<vector<complex<double>>> Eu;
	vector<vector<complex<double>>> Ev;

	Eu = inputField.getEx();	Ev = inputField.getEy();
	
	
	CUDAPOCalculation cudapo;
	// 设置单位
	cudapo.SetReturnFloat(returnFloat, user);
	// 设置频率
	cudapo.setFrequency(fre);
	//设置入射场
	cudapo.setPlaneApertureEField_D(Eu,Ev,u_input,v_input,p_cen_in,float(ds),M,N);
	//设置反射网格
	cudapo.setReflectSTL(stlMirror.getPolyData());

	if (returnFloat) // 如果没有注册则不回调
	{
		returnFloat(30, user);
	}

	//计算场到电流
	cudapo.calculateF2S();
	//提取计算的表面电流
	vector<complex<double>> Hx_Current;
	vector<complex<double>> Hy_Current;
	vector<complex<double>> Hz_Current;
	int NumCurrent = stlMirror.getPolyData()->GetNumberOfCells();
	Hx_Current.resize(NumCurrent);
	Hy_Current.resize(NumCurrent);
	Hz_Current.resize(NumCurrent);
	//读取第一步计算结果 表面电流
	cudapo.getSTLlistHfield(Hx_Current,Hy_Current,Hz_Current);
	//设置第二部计算输入-表面电流
	cudapo.setSTLCurrentSourceZeroOrder(stlMirror.getPolyData(), Hx_Current, Hy_Current, Hz_Current);
	
	if (returnFloat) // 如果没有注册则不回调
	{
		returnFloat(60, user);
	}

	//更新出射场的位置
	p_center = p_cen_in;

	Vector3 tempReflect;	//反射光线
	Vector3 InterPoint;		//交点
	GraphTrans gt_out; gt_out = gt_inc;
	RayTracing RT(&stlMirror);
	bool tempIsIntersect = false;
	RT.calcReflect(p_center, n_input, tempReflect, InterPoint, tempIsIntersect);
	tempReflect.Normalization();
	double z0 = CalDistance(p_center,InterPoint);
	
	//更新平移 从入射面到反射面交点
	p_center = p_center + n_input*z0;
	gt_out.updateTranslate(p_center);
	//更新旋转
	updateSource_n(tempReflect, gt_out);
	//更新反射后平移 从反射面交点到出射面
	p_center = p_center + tempReflect*dis;
	gt_out.updateTranslate(p_center);
	Vector3 u_out;	u_out.set(1.0, 0.0, 0.0);
	Vector3 v_out;	v_out.set(0.0, 1.0, 0.0);
	Vector3 n_out;	n_out.set(0.0, 0.0, 1.0);
	Vector3 p_cen_out;	p_cen_out = p_center;

	//设置旋转：
	Vector3D  RotateAxis_out(gt_out.getRotate_x(), gt_out.getRotate_y(), gt_out.getRotate_z());
	Matrix4D  RotateMatrix_out = Matrix4D::getRotateMatrix(gt_out.getRotate_theta(), RotateAxis_out);

	u_out = RotateMatrix_out*u_out;
	v_out = RotateMatrix_out*v_out;
	n_out = RotateMatrix_out*n_out;
	//设置出射场位置
	cudapo.setOutputAperture(u_out,v_out,p_cen_out,float(ds),M,N);
	//CUDAPO计算
	cudapo.calculateS2F();

	if (returnFloat) // 如果没有注册则不回调
	{
		returnFloat(90, user);
	}
	//文件输出输出
	complex<double>** Eu_out = NULL;	Eu_out = Allocate_2D(Eu_out, M, N);
	complex<double>** Ev_out = NULL;	Ev_out = Allocate_2D(Ev_out, M, N);
	cudapo.getOutApertureE(Eu_out,Ev_out);
	//文件输出：
	ofstream outfile("./outPO.txt");
	outfile << gt_out.getTrans_x() << " "
		<< gt_out.getTrans_y() << " "
		<< gt_out.getTrans_z() << " "
		<< gt_out.getRotate_x() << " "
		<< gt_out.getRotate_y() << " "
		<< gt_out.getRotate_z() << " "
		<< gt_out.getRotate_theta() << " "
		<< M << " " << N << " " << ds << endl;
	for (int i = 0; i < M; i++)
		for (int j = 0; j < N; j++)
		{
			outfile
				<< abs(Eu_out[i][j]) << " " << arg(Eu_out[i][j]) * 180 / Pi << " "
				<< abs(Ev_out[i][j]) << " " << arg(Ev_out[i][j]) * 180 / Pi << " " << endl;
		}
	//写文件结束，结束
	if (returnFloat) // 如果没有注册则不回调
	{
		returnFloat(100, user);
	}

	Free_2D(Eu_out);
	Free_2D(Ev_out);
}

void CUDAPhysicalOptics::SetReturnFloat(void(*returnFloat)(float, void *), void * _user)
{
	this->returnFloat = returnFloat;
	this->user = _user;
}
