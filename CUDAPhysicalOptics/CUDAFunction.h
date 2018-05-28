//���.h������������ �� API(DLL) �� CUDA���ú��� �ĵ��ýӿ�
#include<vector>
#include<complex>
#include <cuComplex.h>
using namespace std;

//����������ص�ǰ��CUDA����
extern "C"  void DeviceInf(void);
//��������ɵ糡�ֲ�(�������)����õ��������(�ų��ֲ�)
extern "C"  int RunJM2H(float _freq, int _NumSource, float* _px_in, float* _py_in, float* _pz_in,
						float* _ds_in, cuComplex* Jmx_in, cuComplex* Jmy_in, cuComplex* Jmz_in,
						int _NumOut, float* _px_out, float* _py_out, float* _pz_out,
						cuComplex* &Hx_out, cuComplex* &Hy_out, cuComplex* &Hz_out);

//��������ɵ糡�ֲ�(�������)����õ��糡�ֲ�
extern "C"  int RunJM2E(float _freq, int _NumSource, float* _px_in, float* _py_in, float* _pz_in,
						float* _ds_in, cuComplex* Jmx_in, cuComplex* Jmy_in, cuComplex* Jmz_in,
						int _NumOut, float* _px_out, float* _py_out, float* _pz_out,
						cuComplex* &Ex_out, cuComplex* &Ey_out, cuComplex* &Ez_out);

//��������ɱ������(�ų��ֲ�)����õ��糡�ֲ�
extern "C" int RunJ2E(float _freq, int _NumSource, float* _px_in, float* _py_in, float* _pz_in,
					  float* _ds_in, cuComplex* Jx_in, cuComplex* Jy_in, cuComplex* Jz_in,
					  int _NumOut, float* _px_out, float* _py_out, float* _pz_out,
	                  cuComplex* &Ex_out, cuComplex* &Ey_out, cuComplex* &Ez_out);

//��������ɱ������(�ų��ֲ�)����õ��������(�ų��ֲ�)
extern "C" int RunJ2H(float _freq, int _NumSource, float* _px_in, float* _py_in, float* _pz_in,
					  float* _ds_in, cuComplex* Jx_in, cuComplex* Jy_in, cuComplex* Jz_in,
					  int _NumOut, float* _px_out, float* _py_out, float* _pz_out,
					  cuComplex* &Hx_out, cuComplex* &Hy_out, cuComplex* &Hz_out);





