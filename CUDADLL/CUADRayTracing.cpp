#include "CUDARayTracing.h"
#include "CUDADLL.h"
#include <vtkPoints.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>


CUDARayTracing::CUDARayTracing()
	:numPoints(0),
	psourcex(nullptr),
	pdirx(nullptr),
	pdiry(nullptr),
	pdirz(nullptr),
	intersected(nullptr),
	prot(nullptr),
	STLIndex(nullptr),
	inter_x(nullptr),
	inter_y(nullptr),
	inter_z(nullptr),
	numSTL(0),
	stlp1x(nullptr),
	stlp1y(nullptr),
	stlp1z(nullptr),
	stlp2x(nullptr),
	stlp2y(nullptr),
	stlp2z(nullptr),
	stlp3x(nullptr),
	stlp3y(nullptr),
	stlp3z(nullptr)
{

}

CUDARayTracing::~CUDARayTracing()
{
	if (psourcex)
	{
		delete[] psourcex;
		psourcex = nullptr;
	}
	if (pdirx)
	{
		delete[] pdirx;
		pdirx = nullptr;
	}
	if (pdiry)
	{
		delete[] pdiry;
		pdiry = nullptr;
	}
	if (pdirz)
	{
		delete[] pdirz;
		pdirz = nullptr;
	}
	if (intersected)
	{
		delete[] intersected;
		intersected = nullptr;
	}
	if (prot)
	{
		delete[] prot;
		prot = nullptr;
	}
	if (STLIndex)
	{
		delete[] STLIndex;
		STLIndex = nullptr;
	}

	if (inter_x)
	{
		delete[] inter_x;
		inter_x = nullptr;
	}
	if (inter_y)
	{
		delete[] inter_y;
		inter_y = nullptr;
	}
	if (inter_z)
	{
		delete[] inter_z;
		inter_z = nullptr;
	}

	if (stlp1x)
	{
		delete[] stlp1x;
		stlp1x = nullptr;
	}
	if (stlp1y)
	{
		delete[] stlp1y;
		stlp1y = nullptr;
	}
	if (stlp1z)
	{
		delete[] stlp1z;
		stlp1z = nullptr;
	}

	if (stlp2x)
	{
		delete[] stlp2x;
		stlp2x = nullptr;
	}
	if (stlp2y)
	{
		delete[] stlp2y;
		stlp2y = nullptr;
	}
	if (stlp2z)
	{
		delete[] stlp2z;
		stlp2z = nullptr;
	}
}

int CUDARayTracing::getCUDAInfo()
{
	//to do
	DeviceInf();
	return 0;
}

int CUDARayTracing::run()
{
	RunReflectionLine(numPoints, psourcex, psourcey, psourcez,
		pdirx, pdiry, pdirz,
		intersected, prot, STLIndex,
		inter_x, inter_y, inter_z,
		numSTL, stlp1x, stlp1y, stlp1z,
		stlp2x, stlp2y, stlp2z,
		stlp3x, stlp3y, stlp3z);

	return 0;
}

void CUDARayTracing::setSTL(void* _polyData)
{
	vtkPolyData* polyData = (vtkPolyData*)(_polyData);
	numSTL = polyData->GetNumberOfCells();
	stlp1x = (float*)malloc(numSTL * sizeof(float));
	stlp1y = (float*)malloc(numSTL * sizeof(float));
	stlp1z = (float*)malloc(numSTL * sizeof(float));
	stlp2x = (float*)malloc(numSTL * sizeof(float));
	stlp2y = (float*)malloc(numSTL * sizeof(float));
	stlp2z = (float*)malloc(numSTL * sizeof(float));
	stlp3x = (float*)malloc(numSTL * sizeof(float));
	stlp3y = (float*)malloc(numSTL * sizeof(float));
	stlp3z = (float*)malloc(numSTL * sizeof(float));

	vtkIdList * p;
	double * point;
	//��ȡ�����λ��
	for (int i = 0; i < numSTL; i++) {
		p = polyData->GetCell(i)->GetPointIds();
		//��1
		point = polyData->GetPoint(p->GetId(0));
		stlp1x[i] = point[0]; stlp1y[i] = point[1]; stlp1z[i] = point[2];
		//��2
		point = polyData->GetPoint(p->GetId(1));
		stlp2x[i] = point[0]; stlp2y[i] = point[1]; stlp2z[i] = point[2];
		//��3
		point = polyData->GetPoint(p->GetId(2));
		stlp3x[i] = point[0]; stlp3y[i] = point[1]; stlp3z[i] = point[2];
	}
}

void CUDARayTracing::setRays(Vector3 ** Plane, Vector3 ** n_Plane, int n, int m)
{
	numPoints = n*m;
	intersected = (bool*)malloc(numPoints * sizeof(bool));
	STLIndex = (int*)malloc(numPoints * sizeof(int));
	prot = (float*)malloc(numPoints * sizeof(float));
	inter_x = (float*)malloc(numPoints * sizeof(float));
	inter_y = (float*)malloc(numPoints * sizeof(float));
	inter_z = (float*)malloc(numPoints * sizeof(float));
	//���䳡��������
	psourcex = (float*)malloc(numPoints * sizeof(float));
	psourcey = (float*)malloc(numPoints * sizeof(float));
	psourcez = (float*)malloc(numPoints * sizeof(float));
	pdirx = (float*)malloc(numPoints * sizeof(float));
	pdiry = (float*)malloc(numPoints * sizeof(float));
	pdirz = (float*)malloc(numPoints * sizeof(float));
	int index;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			index = j + i*m;
			psourcex[index] = Plane[i][j].x;
			psourcey[index] = Plane[i][j].y;
			psourcez[index] = Plane[i][j].z;

			pdirx[index] = n_Plane[i][j].x;
			pdiry[index] = n_Plane[i][j].y;
			pdirz[index] = n_Plane[i][j].z;
		}
	}
}
