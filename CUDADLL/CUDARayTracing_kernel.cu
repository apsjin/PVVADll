#include "cuda_runtime.h"
#include "cuda.h"
#include "./helper_cuda.h"
#include "device_launch_parameters.h"
#include "device_functions.h"
#include "stdio.h"
#include "stdlib.h"
#include "CUDADLL.h"
//#include "thrust/device_vector.h"
//#include "thrust/host_vector.h"
#include "cuVector3.cuh"


//using namespace thrust;
using namespace std;



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

//Total Shared 
//Shared 49152 bytes
__device__ //这个可以从Device 调
inline void Vector3Add(const float x1, const float y1, const float z1, const float x2, const float y2, const float z2, float & x3, float & y3, float & z3) {
	x3 = x1 + x2;
	y3 = y1 + y2;
	z3 = z1 + z2;
}
__device__
inline void Vector3Sub(const float x1, const float y1, const float z1, const float x2, const float y2, const float z2, float & x3, float & y3, float & z3) {
	x3 = x1 - x2;
	y3 = y1 - y2;
	z3 = z1 - z2;
}
__device__
inline void Vector3Dot(const float x1, const float y1, const float z1, const float x2, const float y2, const float z2, float & dotresult) {
	dotresult = x1*x2 + y1*y2 + z1*z2;
}
__device__
inline void Vector3Cross(const float x1, const float y1, const float z1, const float x2, const float y2, const float z2, float & x3, float & y3, float & z3) {
	x3 = y1*z2 - z1*y2;
	y3 = z1*x2 - x1*z2;
	z3 = x1*y2 - y1*x2;

	//y * v.z - z * v.y,
	//z * v.x - x * v.z,
	//x * v.y - y * v.x
}

__global__ void
FindInterSection_register(const float _psx, const float _psy, const float _psz, const float _dsx, const float _dsy, const float _dsz,
	const int _sourceIndex,
	const float* f_stlp1x, const float* f_stlp1y, const float* f_stlp1z,
	const float* f_stlp2x, const float* f_stlp2y, const float* f_stlp2z,
	const float* f_stlp3x, const float* f_stlp3y, const float* f_stlp3z,
	bool* f_intersecb, float* f_prot, int* f_STLIndex,
	float* f_inter_x, float* f_inter_y, float* f_inter_z,
	const int numElements)
{
	int i = blockDim.x * blockIdx.x + threadIdx.x;
	//int shift = blockDim.x * blockIdx.x;
	int ii = threadIdx.x;

	if (i < numElements)
	{

		//C[i] = A[i] + B[i];
		float s_stlp1x = f_stlp1x[i];		float s_stlp2x = f_stlp2x[i];		float s_stlp3x = f_stlp3x[i];
		float s_stlp1y = f_stlp1y[i];		float s_stlp2y = f_stlp2y[i];		float s_stlp3y = f_stlp3y[i];
		float s_stlp1z = f_stlp1z[i];		float s_stlp2z = f_stlp2z[i];		float s_stlp3z = f_stlp3z[i];

		float s_psx = _psx;	float s_psy = _psy;	float s_psz = _psz;
		float s_dsx = _dsx;	float s_dsy = _dsy;	float s_dsz = _dsz;
		float E1_x = 0;	float E1_y = 0;	float E1_z = 0;
		float E2_x = 0; float E2_y = 0; float E2_z = 0;
		float P_x = 0;	float P_y = 0;	float P_z = 0;
		float Q_x = 0;	float Q_y = 0;	float Q_z = 0;
		float T_x = 0;	float T_y = 0;	float T_z = 0;
		float u = 0;	float v = 0;	float det = 0;
		float s_prot = 0;
		bool s_intersected = false;
		int s_STLIndex = 0;
		float inter_x = 0;
		float inter_y = 0;
		float inter_z = 0;

		Vector3Sub(s_stlp2x, s_stlp2y, s_stlp2z, s_stlp1x, s_stlp1y, s_stlp1z, E1_x, E1_y, E1_z);
		Vector3Sub(s_stlp3x, s_stlp3y, s_stlp3z, s_stlp1x, s_stlp1y, s_stlp1z, E2_x, E2_y, E2_z);
		Vector3Cross(s_dsx, s_dsy, s_dsz, E2_x, E2_y, E2_z, P_x, P_y, P_z);
		Vector3Dot(E1_x, E1_y, E1_z, P_x, P_y, P_z, det);

		Vector3Sub(s_psx, s_psy, s_psz, s_stlp1x, s_stlp1y, s_stlp1z, T_x, T_y, T_z);
		Vector3Dot(T_x, T_y, T_z, P_x, P_y, P_z, u);
		u = u / det;

		Vector3Cross(T_x, T_y, T_z, E1_x, E1_y, E1_z, Q_x, Q_y, Q_z);
		Vector3Dot(s_dsx, s_dsy, s_dsz, Q_x, Q_y, Q_z, v);
		v = v / det;

		__syncthreads();

		if (u < 0.0 || u>1 || v < 0 || u + v>1) {
			//没有交点
		}
		else {
			

			//有交点
			Vector3Dot(E2_x, E2_y, E2_z, Q_x, Q_y, Q_z, s_prot);
			s_prot = s_prot / det;
			Vector3Add(s_psx, s_psy, s_psz, s_dsx*s_prot, s_dsy*s_prot, s_dsz*s_prot, inter_x, inter_y, inter_z);
			s_intersected = true;
			s_STLIndex = i;
			//将结果返回到全局内存
			f_STLIndex[_sourceIndex] = s_STLIndex;
			f_intersecb[_sourceIndex] = s_intersected;
			f_prot[_sourceIndex] = s_prot;
			f_inter_x[_sourceIndex] = inter_x;
			f_inter_y[_sourceIndex] = inter_y;
			f_inter_z[_sourceIndex] = inter_z;
			//atomic_store(&s_intersected,&f_intersecb[_sourceIndex]);
			//atomic_store(&s_STLIndex, &f_STLIndex[_sourceIndex]);
			
		}//else

	}
}

//只在这个文件中
__global__ void
FindInterSection_register_struc(const float _psx, const float _psy, const float _psz, const float _dsx, const float _dsy, const float _dsz,
	const int _sourceIndex,
	const float* f_stlp1x, const float* f_stlp1y, const float* f_stlp1z,
	const float* f_stlp2x, const float* f_stlp2y, const float* f_stlp2z,
	const float* f_stlp3x, const float* f_stlp3y, const float* f_stlp3z,
	bool* f_intersecb, float* f_prot, int* f_STLIndex,
	float* f_inter_x, float* f_inter_y, float* f_inter_z,
	const int numElements)
{
	int i = blockDim.x * blockIdx.x + threadIdx.x;
	//int shift = blockDim.x * blockIdx.x;
	int ii = threadIdx.x;

	if (i < numElements)
	{

		//C[i] = A[i] + B[i];
		register cuVector3 s_stlp1; 
		register cuVector3 s_stlp2;	
		register cuVector3 s_stlp3;
		register cuVector3 s_p; 
		register cuVector3 s_d;
		register cuVector3 E1;
		register cuVector3 E2;
		register cuVector3 P;
		register cuVector3 Q;
		register cuVector3 T;
		register float u = 0;	
		register float v = 0;	
		register float det = 0;
		register float s_prot = 0;
		register bool s_intersected = false;
		register int s_STLIndex = 0;
		register cuVector3 inter;

		SetcuVector3S(f_stlp1x[i], f_stlp1y[i], f_stlp1z[i],s_stlp1);
		SetcuVector3S(f_stlp2x[i], f_stlp2y[i], f_stlp2z[i],s_stlp2);
		SetcuVector3S(f_stlp3x[i], f_stlp3y[i], f_stlp3z[i],s_stlp3);

		//cuVector3 s_stlp1 = SetcuVector3(f_stlp1x[i], f_stlp1y[i], f_stlp1z[i]);
		//cuVector3 s_stlp2 = SetcuVector3(f_stlp2x[i], f_stlp2y[i], f_stlp2z[i]);
		//cuVector3 s_stlp3 = SetcuVector3(f_stlp3x[i], f_stlp3y[i], f_stlp3z[i]);

		//float s_stlp1x = f_stlp1x[i];		float s_stlp2x = f_stlp2x[i];		float s_stlp3x = f_stlp3x[i];
		//float s_stlp1y = f_stlp1y[i];		float s_stlp2y = f_stlp2y[i];		float s_stlp3y = f_stlp3y[i];
		//float s_stlp1z = f_stlp1z[i];		float s_stlp2z = f_stlp2z[i];		float s_stlp3z = f_stlp3z[i];

		 SetcuVector3S(_psx, _psy, _psz, s_p);
		SetcuVector3S(_dsx, _dsy, _dsz, s_d);

		//cuVector3 s_p = SetcuVector3(_psx, _psy, _psz);
		//cuVector3 s_d = SetcuVector3(_dsx, _dsy, _dsz);

		//float s_psx = _psx;	float s_psy = _psy;	float s_psz = _psz;
		//float s_dsx = _dsx;	float s_dsy = _dsy;	float s_dsz = _dsz;

		//float E1_x = 0;	float E1_y = 0;	float E1_z = 0;
		//float E2_x = 0; float E2_y = 0; float E2_z = 0;
		//float P_x = 0;	float P_y = 0;	float P_z = 0;
		//float Q_x = 0;	float Q_y = 0;	float Q_z = 0;
		//float T_x = 0;	float T_y = 0;	float T_z = 0;


		//float inter_x = 0;
		//float inter_y = 0;
		//float inter_z = 0;
		cuVector3SubS(s_stlp2, s_stlp1,E1);
		cuVector3SubS(s_stlp3, s_stlp1,E2);		
		//E1 = cuVector3Sub(s_stlp2, s_stlp1);
		//E2 = cuVector3Sub(s_stlp3, s_stlp1);
		//Vector3Sub(s_stlp2x, s_stlp2y, s_stlp2z, s_stlp1x, s_stlp1y, s_stlp1z, E1_x, E1_y, E1_z);
		//Vector3Sub(s_stlp3x, s_stlp3y, s_stlp3z, s_stlp1x, s_stlp1y, s_stlp1z, E2_x, E2_y, E2_z);
		
		cuVector3CrossS(s_d, E2,P);
		cuVector3DotS(E1, P,det);		
		//P = cuVector3Cross(s_d, E2);
		//det = cuVector3Dot(E1, P);
		//Vector3Cross(s_dsx, s_dsy, s_dsz, E2_x, E2_y, E2_z, P_x, P_y, P_z);
		//Vector3Dot(E1_x, E1_y, E1_z, P_x, P_y, P_z, det);

		cuVector3SubS(s_p, s_stlp1,T);
		cuVector3DotS(T, P,u);
		//T = cuVector3Sub(s_p, s_stlp1);
		//u = cuVector3Dot(T, P);
		//Vector3Sub(s_psx, s_psy, s_psz, s_stlp1x, s_stlp1y, s_stlp1z, T_x, T_y, T_z);
		//Vector3Dot(T_x, T_y, T_z, P_x, P_y, P_z, u);
		u = u / det;
		cuVector3CrossS(T, E1, Q);
		cuVector3DotS(s_d, Q, v);
		//Q = cuVector3Cross(T, E1);
		//v = cuVector3Dot(s_d, Q);
		//Vector3Cross(T_x, T_y, T_z, E1_x, E1_y, E1_z, Q_x, Q_y, Q_z);
		//Vector3Dot(s_dsx, s_dsy, s_dsz, Q_x, Q_y, Q_z, v);
		v = v / det;

		__syncthreads();

		if (u < 0.0 || u>1 || v < 0 || u + v>1) {
			//没有交点
		}
		else {


			//有交点
			cuVector3DotS(E2, Q, s_prot);
			//s_prot = cuVector3Dot(E2, Q);
			//Vector3Dot(E2_x, E2_y, E2_z, Q_x, Q_y, Q_z, s_prot);
			s_prot = s_prot / det;

			cuVector3MulRS(s_d, s_prot,E1);
			cuVector3AddS(s_p, E1,inter);
			//E1 = cuVector3MulR(s_d, s_prot);
			//inter = cuVector3Add(s_p,E1);
			//Vector3Add(s_psx, s_psy, s_psz, s_dsx*s_prot, s_dsy*s_prot, s_dsz*s_prot, inter_x, inter_y, inter_z);
			s_intersected = true;
			s_STLIndex = i;
			//将结果返回到全局内存
			f_STLIndex[_sourceIndex] = s_STLIndex;
			f_intersecb[_sourceIndex] = s_intersected;
			f_prot[_sourceIndex] = s_prot;
			f_inter_x[_sourceIndex] = inter.x;
			f_inter_y[_sourceIndex] = inter.y;
			f_inter_z[_sourceIndex] = inter.z;
			//atomic_store(&s_intersected,&f_intersecb[_sourceIndex]);
			//atomic_store(&s_STLIndex, &f_STLIndex[_sourceIndex]);

		}//else

	}
}



__global__  void
SetResultsZero(const int f_NumSource, const int f_NumSTL, bool* f_intersecb, float* f_prot, int* f_STLIndex, float* f_inter_x, float* f_inter_y, float* f_inter_z) 
{
	int i = blockDim.x * blockIdx.x + threadIdx.x;
	if (i < f_NumSource) {
		f_intersecb[i] = false;
		f_prot[i] = 0;
		f_STLIndex[i] = f_NumSTL + 1;
		f_inter_x[i] = 0.0;
		f_inter_y[i] = 0.0;
		f_inter_z[i] = 0.0;
	}
}

__global__ void
SetVector3GPU(const float* x, const float* y, const float* z,const int Num, cuVector3* vec) {
	int i = blockDim.x * blockIdx.x + threadIdx.x;
	if (i < Num) {
		SetcuVector3S(x[i], y[i], z[i], vec[i]);
	}
}


void DeviceInf(void) {
	FILE* cudalog;
	cudalog = fopen("./cudalog.txt", "w");
	fprintf(cudalog," CUDA Device Query (Runtime API) version (CUDART static linking)\n\n");
	//查询有几个CUDA Device
	int deviceCount = 0;
	cudaError_t error_id = cudaGetDeviceCount(&deviceCount);
	if (error_id != cudaSuccess)
	{
		fprintf(cudalog,"cudaGetDeviceCount returned %d\n-> %s\n", (int)error_id, cudaGetErrorString(error_id));
		fprintf(cudalog,"Result = FAIL\n");
		exit(EXIT_FAILURE);
	}
	//显示有几个CUDA Device
	// This function call returns 0 if there are no CUDA capable devices.
	if (deviceCount == 0)
	{
		fprintf(cudalog,"There are no available device(s) that support CUDA\n");
	}
	else
	{
		fprintf(cudalog,"Detected %d CUDA Capable device(s)\n", deviceCount);
	}
	//依次查询每个Device
	int dev, driverVersion = 0, runtimeVersion = 0;
	for (dev = 0; dev < deviceCount; ++dev)
	{
		cudaSetDevice(dev);
		cudaDeviceProp deviceProp;
		cudaGetDeviceProperties(&deviceProp, dev);

		printf("\nDevice %d: \"%s\"\n", dev, deviceProp.name);

		// Console log
		cudaDriverGetVersion(&driverVersion);
		cudaRuntimeGetVersion(&runtimeVersion);
		fprintf(cudalog,"  CUDA Driver Version / Runtime Version          %d.%d / %d.%d\n", driverVersion / 1000, (driverVersion % 100) / 10, runtimeVersion / 1000, (runtimeVersion % 100) / 10);
		fprintf(cudalog,"  CUDA Capability Major/Minor version number:    %d.%d\n", deviceProp.major, deviceProp.minor);

		char msg[256];
		SPRINTF(msg, "  Total amount of global memory:                 %.0f MBytes (%llu bytes)\n",
			(float)deviceProp.totalGlobalMem / 1048576.0f, (unsigned long long) deviceProp.totalGlobalMem);
		fprintf(cudalog,"%s", msg);

		fprintf(cudalog,"  (%2d) Multiprocessors, (%3d) CUDA Cores/MP:     %d CUDA Cores\n",
			deviceProp.multiProcessorCount,
			_ConvertSMVer2Cores(deviceProp.major, deviceProp.minor),
			_ConvertSMVer2Cores(deviceProp.major, deviceProp.minor) * deviceProp.multiProcessorCount);
		fprintf(cudalog,"  GPU Max Clock rate:                            %.0f MHz (%0.2f GHz)\n", deviceProp.clockRate * 1e-3f, deviceProp.clockRate * 1e-6f);


#if CUDART_VERSION >= 5000
		// This is supported in CUDA 5.0 (runtime API device properties)
		fprintf(cudalog,"  Memory Clock rate:                             %.0f Mhz\n", deviceProp.memoryClockRate * 1e-3f);
		fprintf(cudalog,"  Memory Bus Width:                              %d-bit\n", deviceProp.memoryBusWidth);

		if (deviceProp.l2CacheSize)
		{
			fprintf(cudalog,"  L2 Cache Size:                                 %d bytes\n", deviceProp.l2CacheSize);
		}

#else
		// This only available in CUDA 4.0-4.2 (but these were only exposed in the CUDA Driver API)
		int memoryClock;
		getCudaAttribute<int>(&memoryClock, CU_DEVICE_ATTRIBUTE_MEMORY_CLOCK_RATE, dev);
		fprintf(cudalog,"  Memory Clock rate:                             %.0f Mhz\n", memoryClock * 1e-3f);
		int memBusWidth;
		getCudaAttribute<int>(&memBusWidth, CU_DEVICE_ATTRIBUTE_GLOBAL_MEMORY_BUS_WIDTH, dev);
		fprintf(cudalog,"  Memory Bus Width:                              %d-bit\n", memBusWidth);
		int L2CacheSize;
		getCudaAttribute<int>(&L2CacheSize, CU_DEVICE_ATTRIBUTE_L2_CACHE_SIZE, dev);

		if (L2CacheSize)
		{
			fprintf(cudalog,"  L2 Cache Size:                                 %d bytes\n", L2CacheSize);
		}

#endif

		fprintf(cudalog,"  Maximum Texture Dimension Size (x,y,z)         1D=(%d), 2D=(%d, %d), 3D=(%d, %d, %d)\n",
			deviceProp.maxTexture1D, deviceProp.maxTexture2D[0], deviceProp.maxTexture2D[1],
			deviceProp.maxTexture3D[0], deviceProp.maxTexture3D[1], deviceProp.maxTexture3D[2]);
		fprintf(cudalog,"  Maximum Layered 1D Texture Size, (num) layers  1D=(%d), %d layers\n",
			deviceProp.maxTexture1DLayered[0], deviceProp.maxTexture1DLayered[1]);
		fprintf(cudalog,"  Maximum Layered 2D Texture Size, (num) layers  2D=(%d, %d), %d layers\n",
			deviceProp.maxTexture2DLayered[0], deviceProp.maxTexture2DLayered[1], deviceProp.maxTexture2DLayered[2]);


		fprintf(cudalog,"  Total amount of constant memory:               %lu bytes\n", deviceProp.totalConstMem);
		fprintf(cudalog, "  Total amount of shared memory per block:       %lu bytes\n", deviceProp.sharedMemPerBlock);
		fprintf(cudalog, "  Total number of registers available per block: %d\n", deviceProp.regsPerBlock);
		fprintf(cudalog, "  Warp size:                                     %d\n", deviceProp.warpSize);
		fprintf(cudalog, "  Maximum number of threads per multiprocessor:  %d\n", deviceProp.maxThreadsPerMultiProcessor);
		fprintf(cudalog, "  Maximum number of threads per block:           %d\n", deviceProp.maxThreadsPerBlock);
		fprintf(cudalog, "  Max dimension size of a thread block (x,y,z): (%d, %d, %d)\n",
			deviceProp.maxThreadsDim[0],
			deviceProp.maxThreadsDim[1],
			deviceProp.maxThreadsDim[2]);
		fprintf(cudalog, "  Max dimension size of a grid size    (x,y,z): (%d, %d, %d)\n",
			deviceProp.maxGridSize[0],
			deviceProp.maxGridSize[1],
			deviceProp.maxGridSize[2]);
		fprintf(cudalog, "  Maximum memory pitch:                          %lu bytes\n", deviceProp.memPitch);
		fprintf(cudalog, "  Texture alignment:                             %lu bytes\n", deviceProp.textureAlignment);
		fprintf(cudalog, "  Concurrent copy and kernel execution:          %s with %d copy engine(s)\n", (deviceProp.deviceOverlap ? "Yes" : "No"), deviceProp.asyncEngineCount);
		fprintf(cudalog, "  Run time limit on kernels:                     %s\n", deviceProp.kernelExecTimeoutEnabled ? "Yes" : "No");
		fprintf(cudalog, "  Integrated GPU sharing Host Memory:            %s\n", deviceProp.integrated ? "Yes" : "No");
		fprintf(cudalog, "  Support host page-locked memory mapping:       %s\n", deviceProp.canMapHostMemory ? "Yes" : "No");
		fprintf(cudalog, "  Alignment requirement for Surfaces:            %s\n", deviceProp.surfaceAlignment ? "Yes" : "No");
		fprintf(cudalog, "  Device has ECC support:                        %s\n", deviceProp.ECCEnabled ? "Enabled" : "Disabled");
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
		fprintf(cudalog,"  CUDA Device Driver Mode (TCC or WDDM):         %s\n", deviceProp.tccDriver ? "TCC (Tesla Compute Cluster Driver)" : "WDDM (Windows Display Driver Model)");
#endif
		fprintf(cudalog,"  Device supports Unified Addressing (UVA):      %s\n", deviceProp.unifiedAddressing ? "Yes" : "No");
		fprintf(cudalog,"  Device PCI Domain ID / Bus ID / location ID:   %d / %d / %d\n", deviceProp.pciDomainID, deviceProp.pciBusID, deviceProp.pciDeviceID);

		const char *sComputeMode[] =
		{
			"Default (multiple host threads can use ::cudaSetDevice() with device simultaneously)",
			"Exclusive (only one host thread in one process is able to use ::cudaSetDevice() with this device)",
			"Prohibited (no host thread can use ::cudaSetDevice() with this device)",
			"Exclusive Process (many threads in one process is able to use ::cudaSetDevice() with this device)",
			"Unknown",
			NULL
		};
		fprintf(cudalog,"  Compute Mode:\n");
		fprintf(cudalog,"     < %s >\n", sComputeMode[deviceProp.computeMode]);
	}

	// If there are 2 or more GPUs, query to determine whether RDMA is supported
	if (deviceCount >= 2)
	{
		cudaDeviceProp prop[64];
		int gpuid[64]; // we want to find the first two GPUs that can support P2P
		int gpu_p2p_count = 0;

		for (int i = 0; i < deviceCount; i++)
		{
			checkCudaErrors(cudaGetDeviceProperties(&prop[i], i));

			// Only boards based on Fermi or later can support P2P
			if ((prop[i].major >= 2)
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
				// on Windows (64-bit), the Tesla Compute Cluster driver for windows must be enabled to support this
				&& prop[i].tccDriver
#endif
				)
			{
				// This is an array of P2P capable GPUs
				gpuid[gpu_p2p_count++] = i;
			}
		}

		// Show all the combinations of support P2P GPUs
		int can_access_peer;

		if (gpu_p2p_count >= 2)
		{
			for (int i = 0; i < gpu_p2p_count; i++)
			{
				for (int j = 0; j < gpu_p2p_count; j++)
				{
					if (gpuid[i] == gpuid[j])
					{
						continue;
					}
					checkCudaErrors(cudaDeviceCanAccessPeer(&can_access_peer, gpuid[i], gpuid[j]));
					fprintf(cudalog,"> Peer access from %s (GPU%d) -> %s (GPU%d) : %s\n", prop[gpuid[i]].name, gpuid[i],
						prop[gpuid[j]].name, gpuid[j],
						can_access_peer ? "Yes" : "No");
				}
			}
		}
	}

	// csv masterlog info
	// *****************************
	// exe and CUDA driver name
	fprintf(cudalog,"\n");
	std::string sProfileString = "deviceQuery, CUDA Driver = CUDART";
	char cTemp[16];

	// driver version
	sProfileString += ", CUDA Driver Version = ";
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	sprintf_s(cTemp, 10, "%d.%d", driverVersion / 1000, (driverVersion % 100) / 10);
#else
	sprintf(cTemp, "%d.%d", driverVersion / 1000, (driverVersion % 100) / 10);
#endif
	sProfileString += cTemp;

	// Runtime version
	sProfileString += ", CUDA Runtime Version = ";
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	sprintf_s(cTemp, 10, "%d.%d", runtimeVersion / 1000, (runtimeVersion % 100) / 10);
#else
	sprintf(cTemp, "%d.%d", runtimeVersion / 1000, (runtimeVersion % 100) / 10);
#endif
	sProfileString += cTemp;

	// Device count
	sProfileString += ", NumDevs = ";
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	sprintf_s(cTemp, 10, "%d", deviceCount);
#else
	sprintf(cTemp, "%d", deviceCount);
#endif
	sProfileString += cTemp;

	// Print Out all device Names
	for (dev = 0; dev < deviceCount; ++dev)
	{
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
		sprintf_s(cTemp, 13, ", Device%d = ", dev);
#else
		sprintf(cTemp, ", Device%d = ", dev);
#endif
		cudaDeviceProp deviceProp;
		cudaGetDeviceProperties(&deviceProp, dev);
		sProfileString += cTemp;
		sProfileString += deviceProp.name;
	}

	sProfileString += "\n";
	fprintf(cudalog, "%s", sProfileString.c_str());

	fprintf(cudalog, "Result = PASS\n");


	fclose(cudalog);


}

void RunReflectionLine(int _NumPoints, float* _psourcex, float* _psourcey, float*  _psourcez,
											  float* _pdirx, float*  _pdiry, float*  _pdirz,
											  bool* &_intersected, float* &_prot,int* &_STLIndex,
												float* &_inter_x, float* &_inter_y, float* &_inter_z,
												int _NumSTL, float* _stlp1x, float* _stlp1y, float* _stlp1z,
												float* _stlp2x, float* _stlp2y, float* _stlp2z,
												float* _stlp3x, float* _stlp3y, float* _stlp3z) {
	cudaDeviceReset;
	cudaSetDevice(0);
	FILE* cudalog;
	cudalog = fopen("./cudalog_calculation.txt", "w");
	fprintf(cudalog, "This is log file for Cuda Calculation \n");
	fclose(cudalog); cudalog = fopen("./cudalog_calculation.txt", "a");
	cudaError_t err = cudaSuccess;
	int NumSTL = _NumSTL;

	//源的尺寸
	size_t sizeSource = _NumPoints * sizeof(float);
	size_t sizeSourceBool = _NumPoints * sizeof(bool);
	size_t sizeSourceInt = _NumPoints * sizeof(int);
	//网格的尺寸
	size_t sizeSTL = _NumSTL * sizeof(float);
	//申请GPU内存 - 输入量
	//光线源
	float* d_psourcex = NULL;		err = cudaMalloc((void **)&d_psourcex, sizeSource);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_psourcex!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}
	err = cudaMemcpy(d_psourcex, _psourcex, sizeSource, cudaMemcpyHostToDevice);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to copy memory from host d_psourcex!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}

	float* d_psourcey = NULL;		err = cudaMalloc((void **)&d_psourcey, sizeSource);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_psourcey!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}
	err = cudaMemcpy(d_psourcey, _psourcey, sizeSource, cudaMemcpyHostToDevice);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to copy memory from host d_psourcey!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}

	float* d_psourcez = NULL;		err = cudaMalloc((void **)&d_psourcez, sizeSource);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_psourcez!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}
	err = cudaMemcpy(d_psourcez, _psourcez, sizeSource, cudaMemcpyHostToDevice);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to copy memory from host d_psourcez!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}

	//源点坐标
	cuVector3* d_psource = NULL;		err = cudaMalloc((void **)&d_psource, _NumPoints * sizeof(cuVector3));
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_psource!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}

	float* d_pdirx = NULL;			err = cudaMalloc((void **)&d_pdirx, sizeSource);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_pdirx!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}
	err = cudaMemcpy(d_pdirx, _pdirx, sizeSource, cudaMemcpyHostToDevice);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to copy memory from host d_pdirx!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}

	float* d_pdiry = NULL;			err = cudaMalloc((void **)&d_pdiry, sizeSource);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_pdiry!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}
	err = cudaMemcpy(d_pdiry, _pdiry, sizeSource, cudaMemcpyHostToDevice);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to copy memory from host d_pdiry!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}

	float* d_pdirz = NULL;			err = cudaMalloc((void **)&d_pdirz, sizeSource);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_pdirz!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}
	err = cudaMemcpy(d_pdirz, _pdirz, sizeSource, cudaMemcpyHostToDevice);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to copy memory from host d_pdirz!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}
	//源点方向
	cuVector3* d_pdir = NULL;		err = cudaMalloc((void **)&d_pdir, _NumPoints * sizeof(cuVector3));
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_pdir!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}
	
	//镜面网格 3个点的坐标
	//点1
	float* d_stlp1x = NULL;			err = cudaMalloc((void **)&d_stlp1x, sizeSTL);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_stlp1x!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}
	err = cudaMemcpy(d_stlp1x, _stlp1x, sizeSTL, cudaMemcpyHostToDevice);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to copy memory from host d_stlp1x!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}

	float* d_stlp1y = NULL;			err = cudaMalloc((void **)&d_stlp1y, sizeSTL);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_stlp1y!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}
	err = cudaMemcpy(d_stlp1y, _stlp1y, sizeSTL, cudaMemcpyHostToDevice);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to copy memory from host d_stlp1y!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}

	float* d_stlp1z = NULL;			err = cudaMalloc((void **)&d_stlp1z, sizeSTL);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_stlp1z!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}
	err = cudaMemcpy(d_stlp1z, _stlp1z, sizeSTL, cudaMemcpyHostToDevice);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to copy memory from host d_stlp1z!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}

	//点2
	float* d_stlp2x = NULL;			err = cudaMalloc((void **)&d_stlp2x, sizeSTL);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_stlp2x!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}
	err = cudaMemcpy(d_stlp2x, _stlp2x, sizeSTL, cudaMemcpyHostToDevice);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to copy memory from host d_stlp2x!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}

	float* d_stlp2y = NULL;			err = cudaMalloc((void **)&d_stlp2y, sizeSTL);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_stlp2y!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}
	err = cudaMemcpy(d_stlp2y, _stlp2y, sizeSTL, cudaMemcpyHostToDevice);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to copy memory from host d_stlp2y!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}

	float* d_stlp2z = NULL;			err = cudaMalloc((void **)&d_stlp2z, sizeSTL);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_stlp2z!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}
	err = cudaMemcpy(d_stlp2z, _stlp2z, sizeSTL, cudaMemcpyHostToDevice);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to copy memory from host d_stlp2z!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}
	//点3
	float* d_stlp3x = NULL;			err = cudaMalloc((void **)&d_stlp3x, sizeSTL);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_stlp3x!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}
	err = cudaMemcpy(d_stlp3x, _stlp3x, sizeSTL, cudaMemcpyHostToDevice);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to copy memory from host d_stlp3x!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}

	float* d_stlp3y = NULL;			err = cudaMalloc((void **)&d_stlp3y, sizeSTL);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_stlp3y!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}
	err = cudaMemcpy(d_stlp3y, _stlp3y, sizeSTL, cudaMemcpyHostToDevice);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to copy memory from host d_stlp3y!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}

	float* d_stlp3z = NULL;			err = cudaMalloc((void **)&d_stlp3z, sizeSTL);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_stlp3z!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}
	err = cudaMemcpy(d_stlp3z, _stlp3z, sizeSTL, cudaMemcpyHostToDevice);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to copy memory from host d_stlp3z!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}


	//点1
	cuVector3* d_stlp1 = NULL;		err = cudaMalloc((void **)&d_stlp1, _NumSTL * sizeof(cuVector3));
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_stlp1!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}
	//点2
	cuVector3* d_stlp2 = NULL;		err = cudaMalloc((void **)&d_stlp2, _NumSTL * sizeof(cuVector3));
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_stlp2!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}
	//点3
	cuVector3* d_stlp3 = NULL;		err = cudaMalloc((void **)&d_stlp3, _NumSTL * sizeof(cuVector3));
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_stlp3!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}


	//申请GPU内存 - 输出量
	bool* d_intersected = NULL;		err = cudaMalloc((void **)&d_intersected, sizeSourceBool);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_intersected!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}
	float* d_prot = NULL;			err = cudaMalloc((void **)&d_prot, sizeSource);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_prot!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}
	float* d_inter_x = NULL;		err = cudaMalloc((void **)&d_inter_x, sizeSource);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_inter_x!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}
	float* d_inter_y = NULL;		err = cudaMalloc((void **)&d_inter_y, sizeSource);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_inter_y!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}
	float* d_inter_z = NULL;		err = cudaMalloc((void **)&d_inter_z, sizeSource);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_inter_z!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}

	//交点
	cuVector3* d_inter = NULL;		err = cudaMalloc((void **)&d_inter, _NumPoints * sizeof(cuVector3));
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_inter!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}


	//交点的数组坐标
	int* d_STLIndex = NULL;			err = cudaMalloc((void **)&d_STLIndex, sizeSourceInt);
	if (err != cudaSuccess)	{
		fprintf(cudalog, "Failed to allocate device d_STLIndex!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}
	/*
	//输出交点信息待查
	FILE* InputCheck;
	InputCheck = fopen("./SourceRay.txt", "w");
	for (int si = 0; si < _NumPoints; si++) {
		//if (_intersected[si]) fprintf(InputCheck, "ture ");
		//else fprintf(InputCheck, "false ");
		fprintf(InputCheck, "%f %f %f %f %f %f \n", _psourcex[si], _psourcey[si], _psourcez[si], _pdirx[si], _pdiry[si], _pdirz[si]);
	}
	fclose(InputCheck);
	//输出交点信息待查
	FILE* MeshCheck;
	MeshCheck = fopen("./Mesh.txt", "w");
	for (int si = 0; si < _NumSTL; si++) {
		//if (_intersected[si]) fprintf(InputCheck, "ture ");
		//else fprintf(InputCheck, "false ");
		fprintf(MeshCheck, "%f %f %f %f %f %f %f %f %f \n", _stlp1x[si], _stlp1y[si], _stlp1z[si], _stlp2x[si], _stlp2y[si], _stlp2z[si], _stlp3x[si], _stlp3y[si], _stlp3z[si]);
	}
	fclose(MeshCheck);
	*/

	//用GPU初始化交点结果
	int blocksPerGrid = (_NumPoints + threadsPerBlock - 1) / threadsPerBlock;
	fprintf(cudalog, "First, CUDA kernel will launch %d blocks of %d threads for Setting intersection results to Zero on GPU.\n", blocksPerGrid, threadsPerBlock);
	fclose(cudalog); cudalog = fopen("./cudalog_calculation.txt", "a");
	SetResultsZero <<< blocksPerGrid, threadsPerBlock >>>
		(_NumPoints,_NumSTL,d_intersected,d_prot,d_STLIndex,d_inter_x,d_inter_y,d_inter_z);

	//只用GPU遍历找交点
	//计算所需Block 和 Thread数
	blocksPerGrid = (_NumSTL + threadsPerBlock - 1) / threadsPerBlock;
	fprintf(cudalog,"Second, CUDA kernel will launch %d blocks of %d threads for finding the intersection.\n", blocksPerGrid, threadsPerBlock);
	fclose(cudalog); cudalog = fopen("./cudalog_calculation.txt", "a");
	for (int si = 0; si < _NumPoints; si++) {
		float sx = _psourcex[si];
		float sy = _psourcey[si];
		float sz = _psourcez[si];
		float dx = _pdirx[si];
		float dy = _pdiry[si];
		float dz = _pdirz[si];
		FindInterSection_register<<< blocksPerGrid, threadsPerBlock >>>
		(sx, sy, sz, dx, dy, dz,
			si,
			d_stlp1x, d_stlp1y, d_stlp1z,
			d_stlp2x, d_stlp2y, d_stlp2z,
			d_stlp3x, d_stlp3y, d_stlp3z,
			d_intersected, d_prot, d_STLIndex,
			d_inter_x, d_inter_y, d_inter_z,
			NumSTL);
		//完成交点搜寻
		if (si % 100 == 0) {
			fprintf(cudalog, "CUDA kernel finished %d Source Point of %d SourcePoints\n", si, _NumPoints);
			fclose(cudalog); cudalog = fopen("./cudalog_calculation.txt", "a");
		}
	}

	//将CUDA计算结果传回
	err = cudaMemcpy(_intersected,d_intersected, sizeSourceBool, cudaMemcpyDeviceToHost);
	err = cudaMemcpy(_STLIndex, d_STLIndex, sizeSourceInt, cudaMemcpyDeviceToHost);
	err = cudaMemcpy(_prot, d_prot, sizeSource, cudaMemcpyDeviceToHost);
	err = cudaMemcpy(_inter_x, d_inter_x, sizeSource, cudaMemcpyDeviceToHost);
	err = cudaMemcpy(_inter_y, d_inter_y, sizeSource, cudaMemcpyDeviceToHost);
	err = cudaMemcpy(_inter_z, d_inter_z, sizeSource, cudaMemcpyDeviceToHost);
	/*
	//输出交点信息待查
	FILE* ResultCheck;
	ResultCheck = fopen("./Intersection Results.txt", "w");
	for (int si = 0; si < _NumPoints; si++) {
		if (_intersected[si]) fprintf(ResultCheck, "ture ");
		else fprintf(ResultCheck, "false ");
		fprintf(ResultCheck, "%f %f %f %f \n", _prot[si], _inter_x[si], _inter_y[si], _inter_z[si]);
	}
	fclose(ResultCheck);
	*/
	fprintf(cudalog, "CUDA kernel finished computation，Next to free CUDA Memories\n", _NumPoints);
	fclose(cudalog); cudalog = fopen("./cudalog_calculation.txt", "a");
	err = cudaFree(d_psourcex);	err = cudaFree(d_psourcey);	err = cudaFree(d_psourcez);
	err = cudaFree(d_pdirx);	err = cudaFree(d_pdiry);	err = cudaFree(d_pdirz);
	err = cudaFree(d_stlp1x);	err = cudaFree(d_stlp1y);	err = cudaFree(d_stlp1z);
	err = cudaFree(d_stlp2x);	err = cudaFree(d_stlp2y);	err = cudaFree(d_stlp2z);
	err = cudaFree(d_stlp3x);	err = cudaFree(d_stlp3y);	err = cudaFree(d_stlp3z);
	err = cudaFree(d_intersected);
	err = cudaFree(d_STLIndex);
	err = cudaFree(d_prot);
	err = cudaFree(d_inter_x);
	err = cudaFree(d_inter_y);
	err = cudaFree(d_inter_z);

	err = cudaFree(d_psource);	err = cudaFree(d_pdir);		err = cudaFree(d_inter);
	err = cudaFree(d_stlp1);	err = cudaFree(d_stlp2);	err = cudaFree(d_stlp3);

	fprintf(cudalog, "CUDA Memory Cleaned\n", _NumPoints);
	fclose(cudalog); cudalog = fopen("./cudalog_calculation.txt", "a");
	fclose(cudalog);
}

void RunReflectionLineS(int _NumPoints, float* _psourcex, float* _psourcey, float*  _psourcez,
	float* _pdirx, float*  _pdiry, float*  _pdirz,
	bool* &_intersected, float* &_prot, int* &_STLIndex,
	float* &_inter_x, float* &_inter_y, float* &_inter_z,
	int _NumSTL, float* _stlp1x, float* _stlp1y, float* _stlp1z,
	float* _stlp2x, float* _stlp2y, float* _stlp2z,
	float* _stlp3x, float* _stlp3y, float* _stlp3z) {
	cudaDeviceReset;
	cudaSetDevice(0);//
	FILE* cudalog;
	cudalog = fopen("./cudalog_calculation.txt", "w");
	fprintf(cudalog, "This is log file for Cuda Calculation \n");
	fclose(cudalog); cudalog = fopen("./cudalog_calculation.txt", "a");
	cudaError_t err = cudaSuccess;
	int NumSTL = _NumSTL;
	
	//申请内存-Host
	//cuVector3* d_psource = NULL;	
	

	//源的尺寸
	size_t sizeSource = _NumPoints * sizeof(float);
	size_t sizeSourceBool = _NumPoints * sizeof(bool);
	size_t sizeSourceInt = _NumPoints * sizeof(int);
	//网格的尺寸
	size_t sizeSTL = _NumSTL * sizeof(float);
	//申请GPU内存 - 输入量
	//光线源
	float* d_psourcex = NULL;		err = cudaMalloc((void **)&d_psourcex, sizeSource);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_psourcex!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}
	err = cudaMemcpy(d_psourcex, _psourcex, sizeSource, cudaMemcpyHostToDevice);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to copy memory from host d_psourcex!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}

	float* d_psourcey = NULL;		err = cudaMalloc((void **)&d_psourcey, sizeSource);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_psourcey!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}
	err = cudaMemcpy(d_psourcey, _psourcey, sizeSource, cudaMemcpyHostToDevice);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to copy memory from host d_psourcey!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}

	float* d_psourcez = NULL;		err = cudaMalloc((void **)&d_psourcez, sizeSource);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_psourcez!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}
	err = cudaMemcpy(d_psourcez, _psourcez, sizeSource, cudaMemcpyHostToDevice);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to copy memory from host d_psourcez!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}

	//源点坐标
	cuVector3* d_psource = NULL;		err = cudaMalloc((void **)&d_psource, _NumPoints * sizeof(cuVector3));
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_psource!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}

	float* d_pdirx = NULL;			err = cudaMalloc((void **)&d_pdirx, sizeSource);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_pdirx!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}
	err = cudaMemcpy(d_pdirx, _pdirx, sizeSource, cudaMemcpyHostToDevice);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to copy memory from host d_pdirx!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}

	float* d_pdiry = NULL;			err = cudaMalloc((void **)&d_pdiry, sizeSource);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_pdiry!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}
	err = cudaMemcpy(d_pdiry, _pdiry, sizeSource, cudaMemcpyHostToDevice);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to copy memory from host d_pdiry!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}

	float* d_pdirz = NULL;			err = cudaMalloc((void **)&d_pdirz, sizeSource);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_pdirz!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}
	err = cudaMemcpy(d_pdirz, _pdirz, sizeSource, cudaMemcpyHostToDevice);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to copy memory from host d_pdirz!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}
	//源点方向
	cuVector3* d_pdir = NULL;		err = cudaMalloc((void **)&d_pdir, _NumPoints * sizeof(cuVector3));
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_pdir!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}

	//镜面网格 3个点的坐标
	//点1
	float* d_stlp1x = NULL;			err = cudaMalloc((void **)&d_stlp1x, sizeSTL);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_stlp1x!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}
	err = cudaMemcpy(d_stlp1x, _stlp1x, sizeSTL, cudaMemcpyHostToDevice);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to copy memory from host d_stlp1x!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}

	float* d_stlp1y = NULL;			err = cudaMalloc((void **)&d_stlp1y, sizeSTL);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_stlp1y!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}
	err = cudaMemcpy(d_stlp1y, _stlp1y, sizeSTL, cudaMemcpyHostToDevice);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to copy memory from host d_stlp1y!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}

	float* d_stlp1z = NULL;			err = cudaMalloc((void **)&d_stlp1z, sizeSTL);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_stlp1z!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}
	err = cudaMemcpy(d_stlp1z, _stlp1z, sizeSTL, cudaMemcpyHostToDevice);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to copy memory from host d_stlp1z!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}

	//点2
	float* d_stlp2x = NULL;			err = cudaMalloc((void **)&d_stlp2x, sizeSTL);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_stlp2x!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}
	err = cudaMemcpy(d_stlp2x, _stlp2x, sizeSTL, cudaMemcpyHostToDevice);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to copy memory from host d_stlp2x!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}

	float* d_stlp2y = NULL;			err = cudaMalloc((void **)&d_stlp2y, sizeSTL);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_stlp2y!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}
	err = cudaMemcpy(d_stlp2y, _stlp2y, sizeSTL, cudaMemcpyHostToDevice);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to copy memory from host d_stlp2y!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}

	float* d_stlp2z = NULL;			err = cudaMalloc((void **)&d_stlp2z, sizeSTL);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_stlp2z!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}
	err = cudaMemcpy(d_stlp2z, _stlp2z, sizeSTL, cudaMemcpyHostToDevice);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to copy memory from host d_stlp2z!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}
	//点3
	float* d_stlp3x = NULL;			err = cudaMalloc((void **)&d_stlp3x, sizeSTL);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_stlp3x!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}
	err = cudaMemcpy(d_stlp3x, _stlp3x, sizeSTL, cudaMemcpyHostToDevice);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to copy memory from host d_stlp3x!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}

	float* d_stlp3y = NULL;			err = cudaMalloc((void **)&d_stlp3y, sizeSTL);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_stlp3y!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}
	err = cudaMemcpy(d_stlp3y, _stlp3y, sizeSTL, cudaMemcpyHostToDevice);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to copy memory from host d_stlp3y!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}

	float* d_stlp3z = NULL;			err = cudaMalloc((void **)&d_stlp3z, sizeSTL);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_stlp3z!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}
	err = cudaMemcpy(d_stlp3z, _stlp3z, sizeSTL, cudaMemcpyHostToDevice);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to copy memory from host d_stlp3z!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}


	//点1
	cuVector3* d_stlp1 = NULL;		err = cudaMalloc((void **)&d_stlp1, _NumSTL * sizeof(cuVector3));
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_stlp1!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}
	//点2
	cuVector3* d_stlp2 = NULL;		err = cudaMalloc((void **)&d_stlp2, _NumSTL * sizeof(cuVector3));
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_stlp2!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}
	//点3
	cuVector3* d_stlp3 = NULL;		err = cudaMalloc((void **)&d_stlp3, _NumSTL * sizeof(cuVector3));
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_stlp3!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}


	//申请GPU内存 - 输出量
	bool* d_intersected = NULL;		err = cudaMalloc((void **)&d_intersected, sizeSourceBool);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_intersected!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}
	float* d_prot = NULL;			err = cudaMalloc((void **)&d_prot, sizeSource);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_prot!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}
	float* d_inter_x = NULL;		err = cudaMalloc((void **)&d_inter_x, sizeSource);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_inter_x!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}
	float* d_inter_y = NULL;		err = cudaMalloc((void **)&d_inter_y, sizeSource);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_inter_y!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}
	float* d_inter_z = NULL;		err = cudaMalloc((void **)&d_inter_z, sizeSource);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_inter_z!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}

	//交点
	cuVector3* d_inter = NULL;		err = cudaMalloc((void **)&d_inter, _NumPoints * sizeof(cuVector3));
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_inter!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}


	//交点的数组坐标
	int* d_STLIndex = NULL;			err = cudaMalloc((void **)&d_STLIndex, sizeSourceInt);
	if (err != cudaSuccess) {
		fprintf(cudalog, "Failed to allocate device d_STLIndex!\n", cudaGetErrorString(err));
		fclose(cudalog);
		exit(EXIT_FAILURE);
	}
	/*
	//输出交点信息待查
	FILE* InputCheck;
	InputCheck = fopen("./SourceRay.txt", "w");
	for (int si = 0; si < _NumPoints; si++) {
	//if (_intersected[si]) fprintf(InputCheck, "ture ");
	//else fprintf(InputCheck, "false ");
	fprintf(InputCheck, "%f %f %f %f %f %f \n", _psourcex[si], _psourcey[si], _psourcez[si], _pdirx[si], _pdiry[si], _pdirz[si]);
	}
	fclose(InputCheck);
	//输出交点信息待查
	FILE* MeshCheck;
	MeshCheck = fopen("./Mesh.txt", "w");
	for (int si = 0; si < _NumSTL; si++) {
	//if (_intersected[si]) fprintf(InputCheck, "ture ");
	//else fprintf(InputCheck, "false ");
	fprintf(MeshCheck, "%f %f %f %f %f %f %f %f %f \n", _stlp1x[si], _stlp1y[si], _stlp1z[si], _stlp2x[si], _stlp2y[si], _stlp2z[si], _stlp3x[si], _stlp3y[si], _stlp3z[si]);
	}
	fclose(MeshCheck);
	*/

	//用GPU初始化交点结果
	int blocksPerGrid = (_NumPoints + threadsPerBlock - 1) / threadsPerBlock;
	fprintf(cudalog, "First, CUDA kernel will launch %d blocks of %d threads for Setting intersection results to Zero on GPU.\n", blocksPerGrid, threadsPerBlock);
	fclose(cudalog); cudalog = fopen("./cudalog_calculation.txt", "a");
	SetResultsZero << < blocksPerGrid, threadsPerBlock >> >
		(_NumPoints, _NumSTL, d_intersected, d_prot, d_STLIndex, d_inter_x, d_inter_y, d_inter_z);

	//只用GPU遍历找交点
	//计算所需Block 和 Thread数
	blocksPerGrid = (_NumSTL + threadsPerBlock - 1) / threadsPerBlock;
	fprintf(cudalog, "Second, CUDA kernel will launch %d blocks of %d threads for finding the intersection.\n", blocksPerGrid, threadsPerBlock);
	fclose(cudalog); cudalog = fopen("./cudalog_calculation.txt", "a");
	for (int si = 0; si < _NumPoints; si++) {
		float sx = _psourcex[si];
		float sy = _psourcey[si];
		float sz = _psourcez[si];
		float dx = _pdirx[si];
		float dy = _pdiry[si];
		float dz = _pdirz[si];
		FindInterSection_register_struc << < blocksPerGrid, threadsPerBlock >> >
			(sx, sy, sz, dx, dy, dz,
				si,
				d_stlp1x, d_stlp1y, d_stlp1z,
				d_stlp2x, d_stlp2y, d_stlp2z,
				d_stlp3x, d_stlp3y, d_stlp3z,
				d_intersected, d_prot, d_STLIndex,
				d_inter_x, d_inter_y, d_inter_z,
				NumSTL);
		//完成交点搜寻
		if (si % 100 == 0) {
			fprintf(cudalog, "CUDA kernel finished %d Source Point of %d SourcePoints\n", si, _NumPoints);
			fclose(cudalog); cudalog = fopen("./cudalog_calculation.txt", "a");
		}
	}

	//将CUDA计算结果传回
	err = cudaMemcpy(_intersected, d_intersected, sizeSourceBool, cudaMemcpyDeviceToHost);
	err = cudaMemcpy(_STLIndex, d_STLIndex, sizeSourceInt, cudaMemcpyDeviceToHost);
	err = cudaMemcpy(_prot, d_prot, sizeSource, cudaMemcpyDeviceToHost);
	err = cudaMemcpy(_inter_x, d_inter_x, sizeSource, cudaMemcpyDeviceToHost);
	err = cudaMemcpy(_inter_y, d_inter_y, sizeSource, cudaMemcpyDeviceToHost);
	err = cudaMemcpy(_inter_z, d_inter_z, sizeSource, cudaMemcpyDeviceToHost);
	/*
	//输出交点信息待查
	FILE* ResultCheck;
	ResultCheck = fopen("./Intersection Results.txt", "w");
	for (int si = 0; si < _NumPoints; si++) {
	if (_intersected[si]) fprintf(ResultCheck, "ture ");
	else fprintf(ResultCheck, "false ");
	fprintf(ResultCheck, "%f %f %f %f \n", _prot[si], _inter_x[si], _inter_y[si], _inter_z[si]);
	}
	fclose(ResultCheck);
	*/
	fprintf(cudalog, "CUDA kernel finished computation，Next to free CUDA Memories\n", _NumPoints);
	fclose(cudalog); cudalog = fopen("./cudalog_calculation.txt", "a");
	err = cudaFree(d_psourcex);	err = cudaFree(d_psourcey);	err = cudaFree(d_psourcez);
	err = cudaFree(d_pdirx);	err = cudaFree(d_pdiry);	err = cudaFree(d_pdirz);
	err = cudaFree(d_stlp1x);	err = cudaFree(d_stlp1y);	err = cudaFree(d_stlp1z);
	err = cudaFree(d_stlp2x);	err = cudaFree(d_stlp2y);	err = cudaFree(d_stlp2z);
	err = cudaFree(d_stlp3x);	err = cudaFree(d_stlp3y);	err = cudaFree(d_stlp3z);
	err = cudaFree(d_intersected);
	err = cudaFree(d_STLIndex);
	err = cudaFree(d_prot);
	err = cudaFree(d_inter_x);
	err = cudaFree(d_inter_y);
	err = cudaFree(d_inter_z);

	err = cudaFree(d_psource);	err = cudaFree(d_pdir);		err = cudaFree(d_inter);
	err = cudaFree(d_stlp1);	err = cudaFree(d_stlp2);	err = cudaFree(d_stlp3);

	fprintf(cudalog, "CUDA Memory Cleaned\n", _NumPoints);
	fclose(cudalog); cudalog = fopen("./cudalog_calculation.txt", "a");
	fclose(cudalog);
}

