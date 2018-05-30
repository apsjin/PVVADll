//����ļ� ���� ��CUDA��ʹ�õ� ��άʸ��(λ��)  ���� v1.0
#pragma once
#if !defined(CUVECTOR3_H_)
#define CUVECTOR3_H_


#include <math.h>       /* import fabsf, sqrt */
#include <cuComplex.h>

#if defined(__cplusplus)
extern "C" {

#include "vector_types.h"
	
	//����CUDA����ά���������ڱ�ʾ�����λ��
	typedef struct cuVector3 {
		float x,y,z;
		//���������
		cuVector3 operator=(const cuVector3 &c);
		cuVector3 operator=(const float &c);
	};

	__host__ __device__ __inline__ cuVector3 cuVector3::operator=(const cuVector3 & a)
	{
		x = a.x; y = a.y; z = a.z;
		return *this;
	}

	__host__ __device__ __inline__ cuVector3 cuVector3::operator=(const float &a)
	{
		x = a; y = a; z = a;
		return *this;
	}

	//�趨ֵ
	__host__ __device__ __inline__ cuVector3 SetcuVector3(const float _x, const float _y, const float _z) {
		cuVector3 result;
		result.x = _x;
		result.y = _y;
		result.z = _z;
		return result;
	}
	//�趨ֵ-
	__host__ __device__ __inline__ void SetcuVector3S(const float _x, const float _y, const float _z, cuVector3 &result) {
		result.x = _x;
		result.y = _y;
		result.z = _z;
	}

	//����һ����ֵ
	__host__ __device__ __inline__ cuVector3 cuVector3MinusR(const cuVector3 _a) {
		cuVector3 result;
		result.x = -_a.x;
		result.y = -_a.y;
		result.z = -_a.z;
		return result;
	}
	//�Լ�ȡ��
	__host__ __device__ __inline__ void cuVector3MinusS(cuVector3 &_a) {
		_a.x = -_a.x;
		_a.y = -_a.y;
		_a.z = -_a.z;
	}
	//����һ����������������
	__host__ __device__ __inline__ cuVector3 cuVector3MulR(const cuVector3 _a, const float _b) {
		cuVector3 result;
		result.x = _a.x*_b;
		result.y = _a.y*_b;
		result.z = _a.z*_b;
		return result;
	}
	//����һ����������������_ָ�����
	__host__ __device__ __inline__ void cuVector3MulRS(const cuVector3 _a, const float _b, cuVector3 &result) {
		result.x = _a.x*_b;
		result.y = _a.y*_b;
		result.z = _a.z*_b;
	}
	//�Լ���һ����
	__host__ __device__ __inline__ void cuVector3MulS( cuVector3 &_a, const float _b) {
		_a.x = _a.x*_b;
		_a.y = _a.y*_b;
		_a.z = _a.z*_b;
	}
	//�����������
	__host__ __device__ __inline__ cuVector3 cuVector3Add(const cuVector3 _a, const cuVector3 _b) {
		cuVector3 result;
		result.x = _a.x + _b.x;
		result.y = _a.y + _b.y;
		result.z = _a.z + _b.z;
		return result;
	}
	//����������� ָ��
	__host__ __device__ __inline__ void cuVector3AddS(const cuVector3 _a, const cuVector3 _b, cuVector3 & result ) {
		result.x = _a.x + _b.x;
		result.y = _a.y + _b.y;
		result.z = _a.z + _b.z;
	}
	//�����������
	__host__ __device__ __inline__ cuVector3 cuVector3Sub(const cuVector3 _a, const cuVector3 _b) {
		cuVector3 result;
		result.x = _a.x - _b.x;
		result.y = _a.y - _b.y;
		result.z = _a.z - _b.z;
		return result;
	}
	//�����������
	__host__ __device__ __inline__ void cuVector3SubS(const cuVector3 _a, const cuVector3 _b, cuVector3 &result) {
		result.x = _a.x - _b.x;
		result.y = _a.y - _b.y;
		result.z = _a.z - _b.z;
	}
	//�����������
	__host__ __device__ __inline__ float cuVector3Dot(const cuVector3 _a, const cuVector3 _b) {
		return _a.x*_b.x + _a.y*_b.y + _a.z*_b.z;
	}
	//�����������-ָ��
	__host__ __device__ __inline__ void cuVector3DotS(const cuVector3 _a, const cuVector3 _b, float &result) {
		result = _a.x*_b.x + _a.y*_b.y + _a.z*_b.z;
	}
	//�����������
	__host__ __device__ __inline__ cuVector3 cuVector3Cross(const cuVector3 _a, const cuVector3 _b) {
		cuVector3 result;
		result.x = _a.y*_b.z - _a.z*_b.y;
		result.y = _a.z*_b.x - _a.x*_b.z;
		result.z = _a.x*_b.y - _a.y*_b.x;
		return result;
		//x3 = y1*z2 - z1*y2;
		//y3 = z1*x2 - x1*z2;
		//z3 = x1*y2 - y1*x2;
	}
	//�����������-ָ��
	__host__ __device__ __inline__ void cuVector3CrossS(const cuVector3 _a, const cuVector3 _b, cuVector3 &result) {
		result.x = _a.y*_b.z - _a.z*_b.y;
		result.y = _a.z*_b.x - _a.x*_b.z;
		result.z = _a.x*_b.y - _a.y*_b.x;
	}
	//����ȡģֵ
	__host__ __device__ __inline__ float cuVector3Abs(const cuVector3 _a) {
		float result;
		result = _a.x*_a.x + _a.y*_a.y + _a.z*_a.z;
		result = sqrt(result);
		return result;
	}
	//����ȡģֵ��ָ��
	__host__ __device__ __inline__ void cuVector3AbsS(const cuVector3 _a, float &result) {
		result = _a.x*_a.x + _a.y*_a.y + _a.z*_a.z;
		result = sqrt(result);
	}

#endif /* __cplusplus */
	/*
	typedef float3 cuFloatVector3;

	__host__ __device__ static __inline__ float cuVec3GetX(cuFloatVector3 _x)
	{
		return _x.x;
	}
	*/
	/*
	typedef cuFloatComplex[3] cuCFloatVector3;
	__host__ __device__ static __inline__ float cuVec3GetX(cuFloatVector3 _x)
	{
		return _x.x;
	}
	*/

}
#endif /* !defined(CU_COMPLEX_H_) */