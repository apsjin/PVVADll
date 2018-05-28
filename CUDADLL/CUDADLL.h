//这个.h的作用是声明 从 API(DLL) 到 CUDA调用函数 的调用接口

#include<vector>
#include<complex>
using namespace std;

//extern "C" CUDADLL_API int vectorAdd(int c[], int a[], int b[], int size);
//这个函数返回当前的CUDA设置
extern "C"  void DeviceInf(void);

//这个函数利用CUDA计算 输入光线组 与 STL面元组的交点，然后返回：1，是否相交，2，距交点传播距离，3，与第几个STL相交，4，交点位置x,y,z
//这个版本直接使用float*
extern "C" void RunReflectionLine(int _NumPoints, float* _psourcex, float* _psourcey, float*  _psourcez,
	float* _pdirx, float*  _pdiry, float*  _pdirz,
	bool* &_intersected, float* &_prot, int* &_STLIndex,
	float* &_inter_x, float* &_inter_y, float* &_inter_z,
	int _NumSTL, float* _stlp1x, float* _stlp1y, float* _stlp1z,
	float* _stlp2x, float* _stlp2y, float* _stlp2z,
	float* _stlp3x, float* _stlp3y, float* _stlp3z);
//这个版本中先将传入float*转化为cuVector3,然后再用CUDA计算-练习用
extern "C" void RunReflectionLineS(int _NumPoints, float* _psourcex, float* _psourcey, float*  _psourcez,
	float* _pdirx, float*  _pdiry, float*  _pdirz,
	bool* &_intersected, float* &_prot, int* &_STLIndex,
	float* &_inter_x, float* &_inter_y, float* &_inter_z,
	int _NumSTL, float* _stlp1x, float* _stlp1y, float* _stlp1z,
	float* _stlp2x, float* _stlp2y, float* _stlp2z,
	float* _stlp3x, float* _stlp3y, float* _stlp3z);

//这里给PO用

//定义一个从任意面电场传播至反射面表面电流的函数
extern "C" void RunProFromEfieldToSurfaceCurrents(int _NumPoints, float* _px, float *_py, float * _pz, //点数，源点位置
									   float* _nx, float* _ny, float* _nz, float* s_ds,		//源法向，位置，面元面积 
									   complex<float>* _Ex, complex<float>* _Ey, complex<float>* _Ez, //源电场强度
									   int _NumSTL, float* _centerx, float* _centery, float* _centerz, //面元数，中心点坐标
									   float* _nxstl, float* _nystl, float* _nzstl,	float* stl_ds,	   //法向，面元面积	
									   complex<float>* &_Jx, complex<float>* &_Jy, complex<float>* &_Jz);  //结果 表面电流

//定义一个从反射面表面电流传播到任意位置电场的函数
extern "C" void RunProFromSurfaceCurrentsToEfield( 
	int _NumSTL, float* _centerx, float* _centery, float* _centerz, //面元数，中心点坐标
	float* _nxstl, float* _nystl, float* _nzstl, float* stl_ds,	   //法向，面元面积	
	complex<float>* _Jx, complex<float>* _Jy, complex<float>* _Jz, //表面电流
	int _NumPoints, float* _px, float* _py, float* _pz,			   //输出点位置， 
	complex<float>* &Ex, complex<float>* &Ey, complex<float>* &Ez);//输出点场强

//定义一个从反射面表面电流传播到下一个反射面表面电流的函数
extern "C" void RunProFromSurfaceCurrentsToSurfaceCurrents(
	int s_NumSTL, float* s_centerx, float* s_centery, float* s_centerz, //源面元数，中心点坐标
	float* s_nxstl, float* s_nystl, float* s_nzstl, float* s_stl_ds,	    //法向，面元面积	
	complex<float>* s_Jx, complex<float>* s_Jy, complex<float>* s_Jz,      //表面电流
	int o_NumSTL, float* o_centerx, float* o_centery, float* o_centerz, //目标面面元数，中心点坐标
	float* o_nxstl, float* o_nystl, float* o_nzstl,						//法向
	complex<float>* o_Jx, complex<float>* o_Jy, complex<float>* o_Jz);      //结果 表面电流

//这里给FDTD的惠更斯面外推用
//定义一个从惠更斯面(表面电磁场)计算到电场的函数


