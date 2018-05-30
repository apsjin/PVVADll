//这个文件 定义 在CUDA中使用的 三维复矢量(场值)  金铭 v1.0
//注意：C是不能重载的！
#pragma once
#if !defined(CUCOMPLEXVECTOR3_H_)
#define CUCOMPLEXVECTOR3_H_


#include <math.h>       /* import fabsf, sqrt */
#include <cuComplex.h>
#include "cuVector3.h"

#if defined(__cplusplus)
extern "C" {

#include "vector_types.h"

	//用于CUDA的三维复数向量，用于表示场
	typedef struct cuComplexVector3 {
		cuComplex x, y, z;
		//重载运算符
		cuComplexVector3 operator=(const cuComplexVector3 &a);
		cuComplexVector3 operator=(const cuComplex &a);
		cuComplexVector3 operator=(const float &a);
	};
	//直接赋值
	__host__ __device__ __inline__ cuComplexVector3 cuComplexVector3::operator=(const cuComplexVector3 & a)
	{
		x = a.x; y = a.y; z = a.z;
		return *this;
	}
	//等于复数
	__host__ __device__ __inline__ cuComplexVector3 cuComplexVector3::operator=(const cuComplex & a)
	{
		x = a; y = a; z = a;
		return *this;
	}
	//等于实数
	__host__ __device__ __inline__ cuComplexVector3 cuComplexVector3::operator=(const float &a)
	{
		x = make_cuFloatComplex(a, 0); y = make_cuFloatComplex(a, 0); z = make_cuFloatComplex(a, 0);
		return *this;
	}
	//直接设置时部虚部
	__host__ __device__ __inline__ cuComplexVector3 SetcuComplexVector3d(const double _x_r, const double _x_i, const double _y_r, const double  _y_i, const double _z_r, const double _z_i) {
		cuComplexVector3 result;
		result.x = make_cuFloatComplex(_x_r, _x_i);
		result.y = make_cuFloatComplex(_y_r, _y_i);
		result.z = make_cuFloatComplex(_z_r, _z_i);
		return result;
	}
	//直接设置实部虚部，float
	__host__ __device__ __inline__ cuComplexVector3 SetcuComplexVector3f(const float _x_r, const float _x_i, const float _y_r, const float _y_i, const float _z_r, const float _z_i) {
		cuComplexVector3 result;
		result.x = make_cuFloatComplex(_x_r, _x_i);
		result.y = make_cuFloatComplex(_y_r, _y_i);
		result.z = make_cuFloatComplex(_z_r, _z_i);
		return result;
	}
	//设置三个分量 -- 
	__host__ __device__ __inline__ cuComplexVector3 SetcuComplexVector3c(const cuComplex _x, const cuComplex _y, const cuComplex _z) {
		cuComplexVector3 result;
		result.x = _x;
		result.y = _y;
		result.z = _z;
		return result;
	}
	//设置三个分量，指针
	__host__ __device__ __inline__ void SetcuComplexVector3S(const cuComplex _x, const cuComplex _y, const cuComplex _z, cuComplexVector3 &result) {
		result.x = _x;
		result.y = _y;
		result.z = _z;
	}
	//复数乘个复数
	__host__ __device__ static __inline__ cuComplex cuCmulffc(float x, cuComplex y)
	{
		return make_cuFloatComplex(x * cuCrealf(y), (x)* cuCimagf(y));
	}
	//复数乘个实数 
	__host__ __device__ static __inline__ cuComplex cuCmulfcf(cuComplex x, float y)
	{
		return make_cuFloatComplex(cuCrealf(x) * y, cuCimagf(x) * y);
	}
	//复数除个实数
	__host__ __device__ static __inline__ cuComplex cuCdivfcf(cuComplex x, float y)
	{
		return make_cuFloatComplex(cuCrealf(x) / y, cuCimagf(x) / y);
	}

	//共轭
	__host__ __device__ __inline__ cuComplexVector3 cuComplexVector3Conj(const cuComplexVector3 _a) {
		return SetcuComplexVector3c(cuConjf(_a.x), cuConjf(_a.y), cuConjf(_a.z));
	}
	//加
	__host__ __device__ __inline__ cuComplexVector3 cuComplexVector3Add(const cuComplexVector3 _a, const cuComplexVector3 _b) {
		return SetcuComplexVector3c(cuCaddf(_a.x, _b.x), cuCaddf(_a.y, _b.y), cuCaddf(_a.z, _b.z));
	}
	__host__ __device__ __inline__ void cuComplexVector3AddS(const cuComplexVector3 _a, const cuComplexVector3 _b, cuComplexVector3 &result) {
		SetcuComplexVector3S(cuCaddf(_a.x, _b.x), cuCaddf(_a.y, _b.y), cuCaddf(_a.z, _b.z),result);
	}
	//减
	__host__ __device__ __inline__ cuComplexVector3 cuComplexVector3Sub(const cuComplexVector3 _a, const cuComplexVector3 _b) {
		return SetcuComplexVector3c(cuCsubf(_a.x, _b.x), cuCsubf(_a.y, _b.y), cuCsubf(_a.z, _b.z));
	}
	__host__ __device__ __inline__ void cuComplexVector3SubS(const cuComplexVector3 _a, const cuComplexVector3 _b, cuComplexVector3 &result) {
		SetcuComplexVector3S(cuCsubf(_a.x, _b.x), cuCsubf(_a.y, _b.y), cuCsubf(_a.z, _b.z),result);
	}
	//乘个数
	__host__ __device__ __inline__ cuComplexVector3 cuComplexVector3Mulcvf(const cuComplexVector3 _a, const float _b) {
		return SetcuComplexVector3c(cuCmulfcf(_a.x, _b), cuCmulfcf(_a.y, _b), cuCmulfcf(_a.z, _b));
	}
	//乘个数，指针
	__host__ __device__ __inline__ void cuComplexVector3MulScvf(const cuComplexVector3 _a, const float _b, cuComplexVector3 &result) {
		SetcuComplexVector3S(cuCmulfcf(_a.x, _b), cuCmulfcf(_a.y, _b), cuCmulfcf(_a.z, _b),result);
	}

	//点乘 实向量和复向量
	__host__ __device__ __inline__ cuComplex cuComplexVector3Dotfc(const cuVector3 _a, const cuComplexVector3 _b) {
		return cuCaddf(cuCaddf(cuCmulffc(_a.x, _b.x), cuCmulffc(_a.y, _b.y)), cuCmulffc(_a.z, _b.z));
	}

	__host__ __device__ __inline__ void cuComplexVector3DotSfc(const cuVector3 _a, const cuComplexVector3 _b, cuComplex &result) {
		result = cuCaddf(cuCaddf(cuCmulffc(_a.x, _b.x), cuCmulffc(_a.y, _b.y)), cuCmulffc(_a.z, _b.z));
	}
	//点乘 复向量和复向量
	__host__ __device__ __inline__ cuComplex cuComplexVector3Dot(const cuComplexVector3 _a, const cuComplexVector3 _b) {
		return cuCaddf(cuCaddf(cuCmulf(_a.x, _b.x), cuCmulf(_a.y, _b.y)), cuCmulf(_a.z, _b.z));
	}
	__host__ __device__ __inline__ void cuComplexVector3DotS(const cuComplexVector3 _a, const cuComplexVector3 _b, cuComplex &result) {
		result = cuCaddf(cuCaddf(cuCmulf(_a.x, _b.x), cuCmulf(_a.y, _b.y)), cuCmulf(_a.z, _b.z));
	}
	//叉乘
	__host__ __device__ __inline__ cuComplexVector3 cuComplexVector3Crossfc(const cuVector3 _a, const cuComplexVector3 _b) {
		cuComplexVector3 result;
		result.x = cuCsubf(cuCmulffc(_a.y, _b.z), cuCmulffc(_a.z, _b.y));
		result.y = cuCsubf(cuCmulffc(_a.z, _b.x), cuCmulffc(_a.x, _b.z));
		result.z = cuCsubf(cuCmulffc(_a.x, _b.y), cuCmulffc(_a.y, _b.x));
		return result;
	}
	__host__ __device__ __inline__ void cuComplexVector3CrossSfc(const cuVector3 _a, const cuComplexVector3 _b, cuComplexVector3 &result) {
		result.x = cuCsubf(cuCmulffc(_a.y, _b.z), cuCmulffc(_a.z, _b.y));
		result.y = cuCsubf(cuCmulffc(_a.z, _b.x), cuCmulffc(_a.x, _b.z));
		result.z = cuCsubf(cuCmulffc(_a.x, _b.y), cuCmulffc(_a.y, _b.x));
	}

	__host__ __device__ __inline__ cuComplexVector3 cuComplexVector3Cross(const cuComplexVector3 _a, const cuComplexVector3 _b) {
		cuComplexVector3 result;
		result.x = cuCsubf(cuCmulf(_a.y, _b.z), cuCmulf(_a.z, _b.y));
		result.y = cuCsubf(cuCmulf(_a.z, _b.x), cuCmulf(_a.x, _b.z));
		result.z = cuCsubf(cuCmulf(_a.x, _b.y), cuCmulf(_a.y, _b.x));
		return result;
	}

	__host__ __device__ __inline__ void cuComplexVector3CrossS(const cuComplexVector3 _a, const cuComplexVector3 _b, cuComplexVector3 &result) {
		result.x = cuCsubf(cuCmulf(_a.y, _b.z), cuCmulf(_a.z, _b.y));
		result.y = cuCsubf(cuCmulf(_a.z, _b.x), cuCmulf(_a.x, _b.z));
		result.z = cuCsubf(cuCmulf(_a.x, _b.y), cuCmulf(_a.y, _b.x));
	}

	__host__ __device__ __inline__ cuComplexVector3 cuComplexVector3Crosscf(const cuComplexVector3 _a, const cuVector3 _b) {
		cuComplexVector3 result;
		result.x = cuCsubf(cuCmulfcf(_a.y, _b.z), cuCmulfcf(_a.z, _b.y));
		result.y = cuCsubf(cuCmulfcf(_a.z, _b.x), cuCmulfcf(_a.x, _b.z));
		result.z = cuCsubf(cuCmulfcf(_a.x, _b.y), cuCmulfcf(_a.y, _b.x));
		return result;
	}
	__host__ __device__ __inline__ void cuComplexVector3CrossScf(const cuComplexVector3 _a, const cuVector3 _b, cuComplexVector3 &result) {
		result.x = cuCsubf(cuCmulfcf(_a.y, _b.z), cuCmulfcf(_a.z, _b.y));
		result.y = cuCsubf(cuCmulfcf(_a.z, _b.x), cuCmulfcf(_a.x, _b.z));
		result.z = cuCsubf(cuCmulfcf(_a.x, _b.y), cuCmulfcf(_a.y, _b.x));
	}
	//场量乘个数
	__host__ __device__ __inline__ cuComplexVector3 cuComplexVector3Mulcf(const cuComplexVector3 _a, float _b) {
		cuComplexVector3 result;
		result.x = cuCmulfcf(_a.x, _b);
		result.y = cuCmulfcf(_a.y, _b);
		result.z = cuCmulfcf(_a.z, _b);
		return result;
	}
	__host__ __device__ __inline__ void cuComplexVector3MulScf(const cuComplexVector3 _a, float _b, cuComplexVector3 &result) {
		result.x = cuCmulfcf(_a.x, _b);
		result.y = cuCmulfcf(_a.y, _b);
		result.z = cuCmulfcf(_a.z, _b);
	}

	__host__ __device__ __inline__ cuComplexVector3 cuComplexVector3Mul(const cuComplexVector3 _a, cuComplex _b) {
		cuComplexVector3 result;
		result.x = cuCmulf(_a.x, _b);
		result.y = cuCmulf(_a.y, _b);
		result.z = cuCmulf(_a.z, _b);
		return result;
	}
	__host__ __device__ __inline__ void cuComplexVector3MulS(const cuComplexVector3 _a, cuComplex _b, cuComplexVector3 &result) {
		result.x = cuCmulf(_a.x, _b);
		result.y = cuCmulf(_a.y, _b);
		result.z = cuCmulf(_a.z, _b);
	}


#endif 

}
#endif /* !defined(CUCOMPLEXVECTOR3_H_) */
