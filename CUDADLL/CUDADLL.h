//���.h������������ �� API(DLL) �� CUDA���ú��� �ĵ��ýӿ�

#include<vector>
#include<complex>
using namespace std;

//extern "C" CUDADLL_API int vectorAdd(int c[], int a[], int b[], int size);
//����������ص�ǰ��CUDA����
extern "C"  void DeviceInf(void);

//�����������CUDA���� ��������� �� STL��Ԫ��Ľ��㣬Ȼ�󷵻أ�1���Ƿ��ཻ��2���ཻ�㴫�����룬3����ڼ���STL�ཻ��4������λ��x,y,z
//����汾ֱ��ʹ��float*
extern "C" void RunReflectionLine(int _NumPoints, float* _psourcex, float* _psourcey, float*  _psourcez,
	float* _pdirx, float*  _pdiry, float*  _pdirz,
	bool* &_intersected, float* &_prot, int* &_STLIndex,
	float* &_inter_x, float* &_inter_y, float* &_inter_z,
	int _NumSTL, float* _stlp1x, float* _stlp1y, float* _stlp1z,
	float* _stlp2x, float* _stlp2y, float* _stlp2z,
	float* _stlp3x, float* _stlp3y, float* _stlp3z);
//����汾���Ƚ�����float*ת��ΪcuVector3,Ȼ������CUDA����-��ϰ��
extern "C" void RunReflectionLineS(int _NumPoints, float* _psourcex, float* _psourcey, float*  _psourcez,
	float* _pdirx, float*  _pdiry, float*  _pdirz,
	bool* &_intersected, float* &_prot, int* &_STLIndex,
	float* &_inter_x, float* &_inter_y, float* &_inter_z,
	int _NumSTL, float* _stlp1x, float* _stlp1y, float* _stlp1z,
	float* _stlp2x, float* _stlp2y, float* _stlp2z,
	float* _stlp3x, float* _stlp3y, float* _stlp3z);

//�����PO��

//����һ����������糡�������������������ĺ���
extern "C" void RunProFromEfieldToSurfaceCurrents(int _NumPoints, float* _px, float *_py, float * _pz, //������Դ��λ��
									   float* _nx, float* _ny, float* _nz, float* s_ds,		//Դ����λ�ã���Ԫ��� 
									   complex<float>* _Ex, complex<float>* _Ey, complex<float>* _Ez, //Դ�糡ǿ��
									   int _NumSTL, float* _centerx, float* _centery, float* _centerz, //��Ԫ�������ĵ�����
									   float* _nxstl, float* _nystl, float* _nzstl,	float* stl_ds,	   //������Ԫ���	
									   complex<float>* &_Jx, complex<float>* &_Jy, complex<float>* &_Jz);  //��� �������

//����һ���ӷ���������������������λ�õ糡�ĺ���
extern "C" void RunProFromSurfaceCurrentsToEfield( 
	int _NumSTL, float* _centerx, float* _centery, float* _centerz, //��Ԫ�������ĵ�����
	float* _nxstl, float* _nystl, float* _nzstl, float* stl_ds,	   //������Ԫ���	
	complex<float>* _Jx, complex<float>* _Jy, complex<float>* _Jz, //�������
	int _NumPoints, float* _px, float* _py, float* _pz,			   //�����λ�ã� 
	complex<float>* &Ex, complex<float>* &Ey, complex<float>* &Ez);//����㳡ǿ

//����һ���ӷ�������������������һ���������������ĺ���
extern "C" void RunProFromSurfaceCurrentsToSurfaceCurrents(
	int s_NumSTL, float* s_centerx, float* s_centery, float* s_centerz, //Դ��Ԫ�������ĵ�����
	float* s_nxstl, float* s_nystl, float* s_nzstl, float* s_stl_ds,	    //������Ԫ���	
	complex<float>* s_Jx, complex<float>* s_Jy, complex<float>* s_Jz,      //�������
	int o_NumSTL, float* o_centerx, float* o_centery, float* o_centerz, //Ŀ������Ԫ�������ĵ�����
	float* o_nxstl, float* o_nystl, float* o_nzstl,						//����
	complex<float>* o_Jx, complex<float>* o_Jy, complex<float>* o_Jz);      //��� �������

//�����FDTD�Ļݸ�˹��������
//����һ���ӻݸ�˹��(�����ų�)���㵽�糡�ĺ���


