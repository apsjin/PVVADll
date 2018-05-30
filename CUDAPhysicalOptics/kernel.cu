//CUDA Core for Physical Optics Calculation
//v0.0, 2018.05
//Ming Jin jinmingaps@163.com
#include "cuda_runtime.h"
#include "cuda.h"

#include "./helper_cuda.h"
#include "device_launch_parameters.h"
#include "device_functions.h"
#include <stdio.h>
#include "stdlib.h"
#include "stdio.h"
#include "stdlib.h"
#include "CUDAfunction.h"
#include "cuVector3.h"
#include "cuComplexVector3.h"
#include "Constant_Val.h"

//Total amount of constant memory:          65536 bytes			常量内存 相当于每个线程128个Float（4byte） 128threads
//Total amount of shared memory per block : 49152 bytes			共享内存 相当于每个线程96个Float (4byte) 128threds
//Total number of registers available per block : 65536			寄存器 相当于每个线程128个Float (4byte) 128threds

//每个Block有128个线程
const int threadsPerBlock = 128;

#if CUDART_VERSION < 5000

// CUDA-C includes
#include <cuda.h>


// This function wraps the CUDA Driver API into a template function
#include
template <class T>
inline void getCudaAttribute(T *attribute, CUdevice_attribute device_attribute, int device)
{
	CUresult error = cuDeviceGetAttribute(attribute, device_attribute, device);

	if (CUDA_SUCCESS != error)
	{
		fprintf(stderr, "cuSafeCallNoSync() Driver API error = %04d from file <%s>, line %i.\n",
			error, __FILE__, __LINE__);

		exit(EXIT_FAILURE);
	}
}

#endif /* CUDART_VERSION < 5000 */

__global__ void //对输入输出量进行清0 _确保补余的量经计算后也为0_这样的计算中少一个判断
kernel_SetZero(cuVector3* _d_p_in, float* _d_ds, cuComplexVector3* _d_J_in) {
	int i = blockDim.x * blockIdx.x + threadIdx.x;
	//int shift = blockDim.x * blockIdx.x;
	int ii = threadIdx.x;
	_d_p_in[i] = SetcuVector3(100, 100, 100);
	_d_ds[i] = 0;
	_d_J_in[i] = SetcuComplexVector3f(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
}

//Kernel 与 外调函数配对
__global__ void	//包涵累加归约操作 有点麻烦，先将Block内的计算结果进行累加，然后每个Block的值存在一个数组的元素里。
kernel_ZeroOrderJM2H(const cuComplex _coe, const float _k, const cuVector3* _d_p_in, const float* _d_ds, const cuComplexVector3* _d_J_in,
	const cuVector3 _d_p_out, cuComplexVector3* _d_H_out_blocks) {
	int i = blockDim.x * blockIdx.x + threadIdx.x;
	int blockid = int(i / threadsPerBlock);
	//int shift = blockDim.x * blockIdx.x;
	int ii = threadIdx.x;
	
	//不选择了，直接全算，补余区域计算结果自然为0
	__shared__ cuComplexVector3 BlockResults[threadsPerBlock];//用于存储块内计算完成的结果
	cuVector3 R = cuVector3Sub(_d_p_out, _d_p_in[i]); //3	//输出位置到输入位置的矢量
	float ds = _d_ds[i];	//输入位置的面积
	cuComplexVector3 J = _d_J_in[i];	//输入电流
	float absR = cuVector3Abs(R);
	float absR2 = absR*absR;	float absR3 = absR*absR2;
	cuComplex cc = _coe;	//coe should be:
	float kk = _k;

	cuComplexVector3 CV1 = cuComplexVector3Crossfc(R, J);		CV1 = cuComplexVector3Crossfc(R, CV1);
	cuComplexVector3 CV2 = J;
	cuComplexVector3 result;
	cuComplex item1;
	cuComplex item2;
	float kr = -kk*absR;
	cuComplex iteme = make_cuComplex(cos(kr), sin(kr));
	iteme = cuCmulf(cuCmulfcf(iteme, ds), cc);
	item1 = cuCaddf(make_cuComplex(3 - kk*kk*absR2, 0), make_cuComplex(0, -3.0*kr));
	item1 = cuCdivfcf(item1, absR2*absR3);
	item1 = cuCmulf(item1, iteme);
	item2 = cuCaddf(make_cuComplex(float(-1), 0), make_cuComplex(0, kr));
	item2 = cuCmulfcf(item2, float(2.0));
	item2 = cuCmulf(item2, iteme);
	cuComplexVector3AddS(cuComplexVector3Mul(CV1, item1), cuComplexVector3Mul(CV2, item2), result);
	//计算完成，下面需要进行累加 首先在共享内存内累加
	BlockResults[ii] = result;
	__syncthreads();//进行至此进行同步，一个块内所有线程进行同步

	//if (ii < 128) { BlockResults[ii] = cuComplexVector3Add(BlockResults[ii], BlockResults[ii + 128]); }
	//__syncthreads();//进行至此进行同步，一个块内所有线程进行同步
	if (ii < 64) { BlockResults[ii] = cuComplexVector3Add(BlockResults[ii], BlockResults[ii + 64]); }
	__syncthreads();//进行至此进行同步，一个块内所有线程进行同步
	if (ii < 32) { BlockResults[ii] = cuComplexVector3Add(BlockResults[ii], BlockResults[ii + 32]); }
	__syncthreads();//进行至此进行同步，一个块内所有线程进行同步
	if (ii < 16) { BlockResults[ii] = cuComplexVector3Add(BlockResults[ii], BlockResults[ii + 16]); }
	__syncthreads();//进行至此进行同步，一个块内所有线程进行同步
	if (ii < 8) { BlockResults[ii] = cuComplexVector3Add(BlockResults[ii], BlockResults[ii + 8]); }
	__syncthreads();//进行至此进行同步，一个块内所有线程进行同步
	if (ii < 4) { BlockResults[ii] = cuComplexVector3Add(BlockResults[ii], BlockResults[ii + 4]); }
	__syncthreads();//进行至此进行同步，一个块内所有线程进行同步
	if (ii < 2) { BlockResults[ii] = cuComplexVector3Add(BlockResults[ii], BlockResults[ii + 2]); }
	__syncthreads();//进行至此进行同步，一个块内所有线程进行同步
	if (ii < 1) {
		BlockResults[ii] = cuComplexVector3Add(BlockResults[ii], BlockResults[ii + 1]);
		_d_H_out_blocks[blockid] = BlockResults[ii];//输出
	}
}

__global__ void
kernel_ZeroOrderJ2E(const cuComplex _coe, const float _k, const cuVector3* _d_p_in, const float* _d_ds, const cuComplexVector3* _d_J_in,
	const cuVector3 _d_p_out, cuComplexVector3* _d_E_out_blocks) {
	int i = blockDim.x * blockIdx.x + threadIdx.x;
	int blockid = int(i / threadsPerBlock);
	//int shift = blockDim.x * blockIdx.x;
	int ii = threadIdx.x;

	//不选择了，直接全算，补余区域计算结果自然为0

	__shared__ cuComplexVector3 BlockResults[threadsPerBlock];//用于存储块内计算完成的结果
	cuVector3 R = cuVector3Sub(_d_p_out, _d_p_in[i]); //3	//输出位置到输入位置的矢量
	float ds = _d_ds[i];	//输入位置的面积
	cuComplexVector3 J = _d_J_in[i];	//输入电流
	float absR = cuVector3Abs(R);
	float absR2 = absR*absR;	float absR3 = absR*absR2;
	cuComplex cc = _coe;	//coe should be:
	float kk = _k;

	cuComplexVector3 CV1 = cuComplexVector3Crossfc(R, J);		CV1 = cuComplexVector3Crossfc(R, CV1);
	cuComplexVector3 CV2 = J;
	cuComplexVector3 result;
	cuComplex item1;
	cuComplex item2;
	float kr = -kk*absR;
	cuComplex iteme = make_cuComplex(cos(kr), sin(kr));
	iteme = cuCmulf(cuCmulfcf(iteme, ds), cc);
	item1 = cuCaddf(make_cuComplex(3 - kk*kk*absR2, 0), make_cuComplex(0, -3.0*kr));
	item1 = cuCdivfcf(item1, absR2*absR3);
	item1 = cuCmulf(item1, iteme);
	item2 = cuCaddf(make_cuComplex(float(1), 0), make_cuComplex(0, -kr));
	item2 = cuCmulfcf(item2, float(2.0));
	item2 = cuCmulf(item2, iteme);
	cuComplexVector3AddS(cuComplexVector3Mul(CV1, item1), cuComplexVector3Mul(CV2, item2), result);
	//计算完成，下面需要进行累加 首先在共享内存内累加
	BlockResults[ii] = result;
	__syncthreads();//进行至此进行同步，一个块内所有线程进行同步

	//if (ii < 128) { BlockResults[ii] = cuComplexVector3Add(BlockResults[ii], BlockResults[ii + 128]); }
	//__syncthreads();//进行至此进行同步，一个块内所有线程进行同步
	if (ii < 64) { BlockResults[ii] = cuComplexVector3Add(BlockResults[ii], BlockResults[ii + 64]); }
	__syncthreads();//进行至此进行同步，一个块内所有线程进行同步
	if (ii < 32) { BlockResults[ii] = cuComplexVector3Add(BlockResults[ii], BlockResults[ii + 32]); }
	__syncthreads();//进行至此进行同步，一个块内所有线程进行同步
	if (ii < 16) { BlockResults[ii] = cuComplexVector3Add(BlockResults[ii], BlockResults[ii + 16]); }
	__syncthreads();//进行至此进行同步，一个块内所有线程进行同步
	if (ii < 8) { BlockResults[ii] = cuComplexVector3Add(BlockResults[ii], BlockResults[ii + 8]); }
	__syncthreads();//进行至此进行同步，一个块内所有线程进行同步
	if (ii < 4) { BlockResults[ii] = cuComplexVector3Add(BlockResults[ii], BlockResults[ii + 4]); }
	__syncthreads();//进行至此进行同步，一个块内所有线程进行同步
	if (ii < 2) { BlockResults[ii] = cuComplexVector3Add(BlockResults[ii], BlockResults[ii + 2]); }
	__syncthreads();//进行至此进行同步，一个块内所有线程进行同步
	if (ii < 1) {
		BlockResults[ii] = cuComplexVector3Add(BlockResults[ii], BlockResults[ii + 1]);
		_d_E_out_blocks[blockid] = BlockResults[ii];//输出
	}
}

int RunJM2H(float _freq,int _NumSource, float* _px_in, float* _py_in, float* _pz_in,
	float* _ds_in, cuComplex* Jmx_in, cuComplex* Jmy_in, cuComplex* Jmz_in,
	int _NumOut, float* _px_out, float* _py_out, float* _pz_out,
	cuComplex* &Hx_out, cuComplex* &Hy_out, cuComplex* &Hz_out) {

	cudaDeviceReset;
	cudaSetDevice(0);
	FILE* cudalog;
	cudalog = fopen("./cudalog_calculationJM2H.txt", "w");
	fprintf(cudalog, "This is log file for Cuda Calculation \n");
	fclose(cudalog); cudalog = fopen("./cudalog_calculationJM2H.txt", "a");
	cudaError_t err = cudaSuccess;

	int NumSource = _NumSource;
	//多少个Block
	int blocksPerGrid = (NumSource + threadsPerBlock - 1) / threadsPerBlock;
	//在申请GPU内存时自动补余
	int NumMalloc = blocksPerGrid*threadsPerBlock;

	//define Host Memory - 输入
	cuVector3* h_p_in = nullptr;			h_p_in = new cuVector3[NumSource];
	float* h_ds_in = nullptr;				h_ds_in = new float[NumSource];
	cuComplexVector3* h_J_in = nullptr;		h_J_in = new cuComplexVector3[NumSource];

	for (int i = 0; i < NumSource; i++) {
		h_p_in[i] = SetcuVector3(_px_in[i], _py_in[i], _pz_in[i]);
		h_ds_in[i] = _ds_in[i];
		h_J_in[i] = SetcuComplexVector3c(Jmx_in[i],Jmy_in[i],Jmz_in[i]);
	}

	//define and malloc GPU Memory, consider NumMalloc other than NumSource!!!!
	cuVector3* d_p_in = NULL;		err = cudaMalloc((void**)&d_p_in, sizeof(cuVector3)*NumMalloc);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_p_in!\n", cudaGetErrorString(err));
		fclose(cudalog);
		return EXIT_FAILURE;
	}
	float* d_ds_in = NULL;		err = cudaMalloc((void**)&d_ds_in, sizeof(float)*NumMalloc);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_ds_in!\n", cudaGetErrorString(err));
		fclose(cudalog);
		return EXIT_FAILURE;
	}
	cuComplexVector3* d_J_in = NULL;		err = cudaMalloc((void**)&d_J_in, sizeof(cuComplexVector3)*NumMalloc);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_J_in!\n", cudaGetErrorString(err));
		fclose(cudalog);
		return EXIT_FAILURE;
	}
	fprintf(cudalog, "First, CUDA kernel will launch %d blocks of %d threads for device Input values to Zero on GPU.\n", blocksPerGrid, threadsPerBlock);
	fclose(cudalog); cudalog = fopen("./cudalog_calculationJM2H.txt", "a");
	kernel_SetZero <<< blocksPerGrid, threadsPerBlock >>>
		(d_p_in, d_ds_in, d_J_in);

	//Copy Host Memory into GPU Memory. Notice!! Here should be NumSource
	err = cudaMemcpy(d_p_in, h_p_in, sizeof(cuVector3)*NumSource, cudaMemcpyHostToDevice);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to copy memory from host h_p_in to device d_p_in!\n", cudaGetErrorString(err));
		fclose(cudalog);
		return EXIT_FAILURE;
	}
	err = cudaMemcpy(d_ds_in, h_ds_in, sizeof(float)*NumSource, cudaMemcpyHostToDevice);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to copy memory from host h_ds_in to device d_ds_in!\n", cudaGetErrorString(err));
		fclose(cudalog);
		return EXIT_FAILURE;
	}
	err = cudaMemcpy(d_J_in, h_J_in, sizeof(cuComplexVector3)*NumSource, cudaMemcpyHostToDevice);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to copy memory from host h_J_in to device d_J_in!\n", cudaGetErrorString(err));
		fclose(cudalog);
		return EXIT_FAILURE;
	}

	//Start Physical Optic Computations

	//define CPU and GPU Memory for Buffer storing output results
	//CPU
	cuComplexVector3* h_H_out = nullptr;	h_H_out = new cuComplexVector3[blocksPerGrid];	//完成CPU端的申请
	//GPU
	cuComplexVector3* d_H_out = NULL;		err = cudaMalloc((void**)&d_H_out, sizeof(cuComplexVector3)*blocksPerGrid);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device buff d_H_out!\n", cudaGetErrorString(err));
		fclose(cudalog);
		return EXIT_FAILURE;
	}
	err = cudaMemcpy(h_H_out, d_H_out, sizeof(cuComplexVector3)*blocksPerGrid, cudaMemcpyDeviceToHost);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to copy memory from device d_H_out to host h_H_out,  !\n", cudaGetErrorString(err));
		fclose(cudalog);
		return EXIT_FAILURE;
	}

	int NumOut = _NumOut;

	fprintf(cudalog, "Second, CUDA kernel will launch %d blocks of %d threads for Computing Field to Current PO.\n", blocksPerGrid, threadsPerBlock);
	fclose(cudalog); cudalog = fopen("./cudalog_calculationJM2H.txt", "a");

	//computation parameters
	float freq = _freq;
	float lambda = C_Speed / freq;
	float k0 = 2 * Pi / lambda;
	cuVector3 p_out;
	//注意不同的流-场对，这个值不一样哦
	cuComplex coe;	coe = make_cuComplex(0, 4 * Pi * 2 * Pi*freq*Mu0);
	cuComplexVector3 HResult;
	coe = cuCdivf(make_cuComplex(1.0,0.0),coe);
	for (int i = 0; i < NumOut; i++) {
		p_out = SetcuVector3(_px_out[i], _py_out[i], _pz_out[i]);
		
		kernel_ZeroOrderJM2H <<< blocksPerGrid, threadsPerBlock >>>
			(coe,k0,d_p_in,d_ds_in,d_J_in,p_out,d_H_out);
		//计算完成了，每个Block会完成计算出一个cuComplexVector3 的积分值，将其传回（多少个Block,这个数组就有多长）

		err = cudaMemcpy(h_H_out, d_H_out, sizeof(cuComplexVector3)*blocksPerGrid, cudaMemcpyDeviceToHost);
		if (err != cudaSuccess) {
			fprintf(cudalog, "Failed to copy memory from device d_H_out to host h_H_out, at PO iteration step: %d !\n",i, cudaGetErrorString(err));
			fclose(cudalog);
			return EXIT_FAILURE;
		}
		//在CPU端对返回的H数组进行累加（GPU上做累加太累了）
		HResult = SetcuComplexVector3d(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
		for (int j = 0; j < blocksPerGrid; j++) {
			HResult = cuComplexVector3Add(HResult, h_H_out[j]);
		}
		//累加后返回磁场数据
		Hx_out[i] = HResult.x;
		Hy_out[i] = HResult.y;
		Hz_out[i] = HResult.z;

		if(i%1000 == 0){
			fprintf(cudalog, "     CUDA kernel performs %d times of %d for Computing Field to Current PO.\n", i, NumOut, threadsPerBlock);
			fclose(cudalog); cudalog = fopen("./cudalog_calculationJM2H.txt", "a");
		}
	}
	
	//清空cudaArray
	err = cudaFree(d_p_in);	err = cudaFree(d_ds_in);	err = cudaFree(d_J_in);
	err = cudaFree(d_H_out);

	delete[] h_H_out;	h_H_out = nullptr;
	delete[] h_p_in;	h_p_in = nullptr;
	delete[] h_ds_in;	h_ds_in = nullptr;
	delete[] h_J_in;	h_J_in = nullptr;

	return 0;
}

int RunJ2E(float _freq, int _NumSource, float* _px_in, float* _py_in, float* _pz_in,
	float* _ds_in, cuComplex* Jx_in, cuComplex* Jy_in, cuComplex* Jz_in,
	int _NumOut, float* _px_out, float* _py_out, float* _pz_out,
	cuComplex* &Ex_out, cuComplex* &Ey_out, cuComplex* &Ez_out) {

	cudaDeviceReset;
	cudaSetDevice(0);
	FILE* cudalog;
	cudalog = fopen("./cudalog_calculationJ2E.txt", "w");
	fprintf(cudalog, "This is log file for Cuda Calculation \n");
	fclose(cudalog); cudalog = fopen("./cudalog_calculationJ2E.txt", "a");
	cudaError_t err = cudaSuccess;

	int NumSource = _NumSource;
	//多少个Block
	int blocksPerGrid = (NumSource + threadsPerBlock - 1) / threadsPerBlock;
	//在申请GPU内存时自动补余
	int NumMalloc = blocksPerGrid*threadsPerBlock;

	//define Host Memory - 输入
	cuVector3* h_p_in = nullptr;			h_p_in = new cuVector3[NumSource];
	float* h_ds_in = nullptr;				h_ds_in = new float[NumSource];
	cuComplexVector3* h_J_in = nullptr;		h_J_in = new cuComplexVector3[NumSource];

	for (int i = 0; i < NumSource; i++) {
		h_p_in[i] = SetcuVector3(_px_in[i], _py_in[i], _pz_in[i]);
		h_ds_in[i] = _ds_in[i];
		h_J_in[i] = SetcuComplexVector3c(Jx_in[i], Jy_in[i], Jz_in[i]);
	}

	//define and malloc GPU Memory, consider NumMalloc other than NumSource!!!!
	cuVector3* d_p_in = NULL;		err = cudaMalloc((void**)&d_p_in, sizeof(cuVector3)*NumMalloc);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_p_in!\n", cudaGetErrorString(err));
		fclose(cudalog);
		return EXIT_FAILURE;
	}
	float* d_ds_in = NULL;		err = cudaMalloc((void**)&d_ds_in, sizeof(float)*NumMalloc);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_ds_in!\n", cudaGetErrorString(err));
		fclose(cudalog);
		return EXIT_FAILURE;
	}
	cuComplexVector3* d_J_in = NULL;		err = cudaMalloc((void**)&d_J_in, sizeof(cuComplexVector3)*NumMalloc);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_J_in!\n", cudaGetErrorString(err));
		fclose(cudalog);
		return EXIT_FAILURE;
	}
	fprintf(cudalog, "First, CUDA kernel will launch %d blocks of %d threads for device Input values to Zero on GPU.\n", blocksPerGrid, threadsPerBlock);
	fclose(cudalog); cudalog = fopen("./cudalog_calculationJ2E.txt", "a");
	kernel_SetZero << < blocksPerGrid, threadsPerBlock >> >
		(d_p_in, d_ds_in, d_J_in);

	//Copy Host Memory into GPU Memory. Notice!! Here should be NumSource
	err = cudaMemcpy(d_p_in, h_p_in, sizeof(cuVector3)*NumSource, cudaMemcpyHostToDevice);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to copy memory from host h_p_in to device d_p_in!\n", cudaGetErrorString(err));
		fclose(cudalog);
		return EXIT_FAILURE;
	}
	err = cudaMemcpy(d_ds_in, h_ds_in, sizeof(float)*NumSource, cudaMemcpyHostToDevice);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to copy memory from host h_ds_in to device d_ds_in!\n", cudaGetErrorString(err));
		fclose(cudalog);
		return EXIT_FAILURE;
	}
	err = cudaMemcpy(d_J_in, h_J_in, sizeof(cuComplexVector3)*NumSource, cudaMemcpyHostToDevice);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to copy memory from host h_J_in to device d_J_in!\n", cudaGetErrorString(err));
		fclose(cudalog);
		return EXIT_FAILURE;
	}

	//Start Physical Optic Computations

	//define CPU and GPU Memory for Buffer storing output results
	//CPU
	cuComplexVector3* h_E_out = nullptr;	h_E_out = new cuComplexVector3[blocksPerGrid];
	//GPU
	cuComplexVector3* d_E_out = NULL;		err = cudaMalloc((void**)&d_E_out, sizeof(cuComplexVector3)*blocksPerGrid);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device buff d_H_out!\n", cudaGetErrorString(err));
		fclose(cudalog);
		return EXIT_FAILURE;
	}
	int NumOut = _NumOut;

	fprintf(cudalog, "Second, CUDA kernel will launch %d blocks of %d threads for Computing Field to Current PO.\n", blocksPerGrid, threadsPerBlock);
	fclose(cudalog); cudalog = fopen("./cudalog_calculationJ2E.txt", "a");

	//computation parameters
	float freq = _freq;
	float lambda = C_Speed / freq;
	float k0 = 2 * Pi / lambda;
	cuVector3 p_out;
	//注意不同的流-场对，这个值不一样哦
	cuComplex coe;	coe = make_cuComplex(0, 4 * Pi * 2 * Pi*freq*Eps0);
	cuComplexVector3 EResult;
	coe = cuCdivf(make_cuComplex(1.0, 0.0), coe);
	for (int i = 0; i < NumOut; i++) {
		p_out = SetcuVector3(_px_out[i], _py_out[i], _pz_out[i]);

		kernel_ZeroOrderJ2E << < blocksPerGrid, threadsPerBlock >> >
			(coe, k0, d_p_in, d_ds_in, d_J_in, p_out, d_E_out);
		//计算完成了，每个Block会完成计算出一个cuComplexVector3 的积分值，将其传回（多少个Block,这个数组就有多长）
		err = cudaMemcpy(h_E_out, d_E_out, sizeof(cuComplexVector3)*blocksPerGrid, cudaMemcpyDeviceToHost);
		if (err != cudaSuccess) {
			fprintf(cudalog, "Failed to copy memory from device d_H_out to host h_H_out, at PO iteration step: %d !\n", i, cudaGetErrorString(err));
			fclose(cudalog);
			return EXIT_FAILURE;
		}
		//在CPU端对返回的H数组进行累加（GPU上做累加太累了）
		EResult = SetcuComplexVector3d(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
		for (int j = 0; j < blocksPerGrid; j++) {
			EResult = cuComplexVector3Add(EResult, h_E_out[j]);
		}
		//累加后返回磁场数据
		Ex_out[i] = EResult.x;
		Ey_out[i] = EResult.y;
		Ez_out[i] = EResult.z;

		if (i % 1000 == 0) {
			fprintf(cudalog, "     CUDA kernel performs %d times of %d for Computing Current to Field PO.\n", i, NumOut, threadsPerBlock);
			fclose(cudalog); cudalog = fopen("./cudalog_calculationJ2E.txt", "a");
		}
	}

	//清空cudaArray
	err = cudaFree(d_p_in);	err = cudaFree(d_ds_in);	err = cudaFree(d_J_in);
	err = cudaFree(d_E_out);

	delete[] h_E_out;	h_E_out = nullptr;
	delete[] h_p_in;	h_p_in = nullptr;
	delete[] h_ds_in;	h_ds_in = nullptr;
	delete[] h_J_in;	h_J_in = nullptr;

	return 0;
}
