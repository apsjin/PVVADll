#include "CUDAPhysicalOptics.h"
#include "CUDAPOCalculation.h"
#include "GraphTrans.h"
#include "Field.h"
#include "STLMirror.h"
#include "Position3D.h"//λ��
#include "Matrix4D.h"	//ת�ƾ���
#include "Vector3D.h"	//����
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
		Vector3 rotate_axis = Vector3(0, 0, 1).Cross(new_n); // ��ת��
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
	//��ȡ���볡�ֲ�-�ļ�
	Field inputField;
	inputField.setFileAddress(FieldName);
	inputField.readData();
	//��ȡ��������-�ļ�
	STLMirror stlMirror;
	stlMirror.setNameFile(STLName);
	stlMirror.readData();
	
	GraphTrans gt_inc;//�������λ�úͷ�����Ϣ
	int M, N;
	double ds;//��ɢ���
	Vector3 u_input;	u_input.set(1.0, 0.0, 0.0);
	Vector3 v_input;	v_input.set(0.0, 1.0, 0.0);
	Vector3 n_input;	n_input.set(0.0, 0.0, 1.0);
	Vector3 p_cen_in;
	Vector3 p_center;
	inputField.getSourcePara(gt_inc, M, N, ds);
	//��������λ��
	p_cen_in.set(gt_inc.getTrans_x(), gt_inc.getTrans_y(), gt_inc.getTrans_z());

	//������ת��
	Vector3D  RotateAxis_in(gt_inc.getRotate_x(), gt_inc.getRotate_y(), gt_inc.getRotate_z());
	Matrix4D  RotateMatrix_in = Matrix4D::getRotateMatrix(gt_inc.getRotate_theta(), RotateAxis_in);

	u_input = RotateMatrix_in*u_input;
	v_input = RotateMatrix_in*v_input;
	n_input = RotateMatrix_in*n_input;

	vector<vector<complex<double>>> Eu;
	vector<vector<complex<double>>> Ev;

	Eu = inputField.getEx();	Ev = inputField.getEy();
	
	
	CUDAPOCalculation cudapo;
	// ���õ�λ
	cudapo.SetReturnFloat(returnFloat, user);
	// ����Ƶ��
	cudapo.setFrequency(fre);
	//�������䳡
	cudapo.setPlaneApertureEField_D(Eu,Ev,u_input,v_input,p_cen_in,float(ds),M,N);
	//���÷�������
	cudapo.setReflectSTL(stlMirror.getPolyData());

	if (returnFloat) // ���û��ע���򲻻ص�
	{
		returnFloat(30, user);
	}

	//���㳡������
	cudapo.calculateF2S();
	//��ȡ����ı������
	vector<complex<double>> Hx_Current;
	vector<complex<double>> Hy_Current;
	vector<complex<double>> Hz_Current;
	int NumCurrent = stlMirror.getPolyData()->GetNumberOfCells();
	Hx_Current.resize(NumCurrent);
	Hy_Current.resize(NumCurrent);
	Hz_Current.resize(NumCurrent);
	//��ȡ��һ�������� �������
	cudapo.getSTLlistHfield(Hx_Current,Hy_Current,Hz_Current);
	//���õڶ�����������-�������
	cudapo.setSTLCurrentSourceZeroOrder(stlMirror.getPolyData(), Hx_Current, Hy_Current, Hz_Current);
	
	if (returnFloat) // ���û��ע���򲻻ص�
	{
		returnFloat(60, user);
	}

	//���³��䳡��λ��
	p_center = p_cen_in;

	Vector3 tempReflect;	//�������
	Vector3 InterPoint;		//����
	GraphTrans gt_out; gt_out = gt_inc;
	RayTracing RT(&stlMirror);
	bool tempIsIntersect = false;
	RT.calcReflect(p_center, n_input, tempReflect, InterPoint, tempIsIntersect);
	tempReflect.Normalization();
	double z0 = CalDistance(p_center,InterPoint);
	
	//����ƽ�� �������浽�����潻��
	p_center = p_center + n_input*z0;
	gt_out.updateTranslate(p_center);
	//������ת
	updateSource_n(tempReflect, gt_out);
	//���·����ƽ�� �ӷ����潻�㵽������
	p_center = p_center + tempReflect*dis;
	gt_out.updateTranslate(p_center);
	Vector3 u_out;	u_out.set(1.0, 0.0, 0.0);
	Vector3 v_out;	v_out.set(0.0, 1.0, 0.0);
	Vector3 n_out;	n_out.set(0.0, 0.0, 1.0);
	Vector3 p_cen_out;	p_cen_out = p_center;

	//������ת��
	Vector3D  RotateAxis_out(gt_out.getRotate_x(), gt_out.getRotate_y(), gt_out.getRotate_z());
	Matrix4D  RotateMatrix_out = Matrix4D::getRotateMatrix(gt_out.getRotate_theta(), RotateAxis_out);

	u_out = RotateMatrix_out*u_out;
	v_out = RotateMatrix_out*v_out;
	n_out = RotateMatrix_out*n_out;
	//���ó��䳡λ��
	cudapo.setOutputAperture(u_out,v_out,p_cen_out,float(ds),M,N);
	//CUDAPO����
	cudapo.calculateS2F();

	if (returnFloat) // ���û��ע���򲻻ص�
	{
		returnFloat(90, user);
	}
	//�ļ�������
	complex<double>** Eu_out = NULL;	Eu_out = Allocate_2D(Eu_out, M, N);
	complex<double>** Ev_out = NULL;	Ev_out = Allocate_2D(Ev_out, M, N);
	cudapo.getOutApertureE(Eu_out,Ev_out);
	//�ļ������
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
	//д�ļ�����������
	if (returnFloat) // ���û��ע���򲻻ص�
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
