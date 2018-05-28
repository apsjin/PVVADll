//这个.h的作用是声明 从 API(DLL) 到 CUDA调用函数 的调用接口
#include<vector>
#include<complex>
#include <cuComplex.h>
using namespace std;

//这个函数返回当前的CUDA设置
extern "C"  void DeviceInf(void);
//这个函数由电场分布(表面磁流)计算得到表面电流(磁场分布)
extern "C"  int RunJM2H(float _freq, int _NumSource, float* _px_in, float* _py_in, float* _pz_in,
						float* _ds_in, cuComplex* Jmx_in, cuComplex* Jmy_in, cuComplex* Jmz_in,
						int _NumOut, float* _px_out, float* _py_out, float* _pz_out,
						cuComplex* &Hx_out, cuComplex* &Hy_out, cuComplex* &Hz_out);

//这个函数由电场分布(表面磁流)计算得到电场分布
extern "C"  int RunJM2E(float _freq, int _NumSource, float* _px_in, float* _py_in, float* _pz_in,
						float* _ds_in, cuComplex* Jmx_in, cuComplex* Jmy_in, cuComplex* Jmz_in,
						int _NumOut, float* _px_out, float* _py_out, float* _pz_out,
						cuComplex* &Ex_out, cuComplex* &Ey_out, cuComplex* &Ez_out);

//这个函数由表面电流(磁场分布)计算得到电场分布
extern "C" int RunJ2E(float _freq, int _NumSource, float* _px_in, float* _py_in, float* _pz_in,
					  float* _ds_in, cuComplex* Jx_in, cuComplex* Jy_in, cuComplex* Jz_in,
					  int _NumOut, float* _px_out, float* _py_out, float* _pz_out,
	                  cuComplex* &Ex_out, cuComplex* &Ey_out, cuComplex* &Ez_out);

//这个函数由表面电流(磁场分布)计算得到表面电流(磁场分布)
extern "C" int RunJ2H(float _freq, int _NumSource, float* _px_in, float* _py_in, float* _pz_in,
					  float* _ds_in, cuComplex* Jx_in, cuComplex* Jy_in, cuComplex* Jz_in,
					  int _NumOut, float* _px_out, float* _py_out, float* _pz_out,
					  cuComplex* &Hx_out, cuComplex* &Hy_out, cuComplex* &Hz_out);





