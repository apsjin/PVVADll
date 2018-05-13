// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� CUDADLL_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// CUDADLL_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�

//���洴��һ��CUDA Kernel ����������DLL
#include<vector>
using namespace std;

#ifdef CUDADLL_EXPORTS
#define CUDADLL_API __declspec(dllexport)
#else
#define CUDADLL_API __declspec(dllimport)
#endif

//extern "C" CUDADLL_API int vectorAdd(int c[], int a[], int b[], int size);
extern "C"  void DeviceInf(void);


//�����������CUDA���� ��������� �� STL��Ԫ��Ľ��㣬Ȼ�󷵻أ�1���Ƿ��ཻ��2���ཻ�㴫�����룬3����ڼ���STL�ཻ��4������λ��x,y,z
extern "C" void RunReflectionLine(int _NumPoints, float* _psourcex, float* _psourcey, float*  _psourcez,
	float* _pdirx, float*  _pdiry, float*  _pdirz,
	bool* &_intersected, float* &_prot, int* &_STLIndex,
	float* &_inter_x, float* &_inter_y, float* &_inter_z,
	int _NumSTL, float* _stlp1x, float* _stlp1y, float* _stlp1z,
	float* _stlp2x, float* _stlp2y, float* _stlp2z,
	float* _stlp3x, float* _stlp3y, float* _stlp3z);

/*
// �����Ǵ� CUDADLL.dll ������
class CUDADLL_API CCUDADLL {
public:
	CCUDADLL(void);
	// TODO:  �ڴ�������ķ�����
};

extern CUDADLL_API int nCUDADLL;

CUDADLL_API int fnCUDADLL(void);
*/