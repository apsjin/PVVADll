#include "RayTracing.h"

calculation::RayTracing::RayTracing(STLMirror* _mirror)
	:mirror(_mirror)
{
}

calculation::RayTracing::~RayTracing()
{
}

void calculation::RayTracing::setMirror(STLMirror * mirror)
{
	this->mirror = mirror;
}

void calculation::RayTracing::calcNormalOfLine_Mirror(const Vector3 & startPiont,
	const Vector3 & direction, Vector3 & normal, Vector3 & intersection,
	bool & isIntersect, double & t)
{
	calcNormalOfLine_MirrorByPolyData(startPiont, direction, normal,
		intersection, isIntersect, t);	
}


void calculation::RayTracing::calcReflect(const Vector3 & startPiont, const Vector3 & direction, 
	Vector3 & reflex, Vector3 & intersection, bool & isIntersect)
{
	calcReflectByPolyData(startPiont, direction, reflex, intersection, isIntersect);
}


void calculation::RayTracing::calcReflectByPolyDataBatch(const vector<vector<Vector3>>& startPiont,
	const vector<vector<Vector3>>& direction,
	vector<vector<Vector3>> &reflex, vector<vector<Vector3>> &intersection,
	vector<vector<bool>> &isIntersect)
{
	for (int i = 0; i < startPiont.size(); i++)
		for (int j = 0; j < startPiont[i].size(); j++)
		{
			bool isTmep = false;
			calcReflectByPolyData(startPiont[i][j], direction[i][j], reflex[i][j],
				intersection[i][j], isTmep);
			isIntersect[i][j] = isTmep;
		}
}

void calculation::RayTracing::calcReflectByPolyData(const Vector3 & startPiont, 
	const Vector3 & direction, Vector3 & reflex, 
	Vector3 & intersection, bool & isIntersect)
{
	vtkSmartPointer<vtkPolyData> polyData = mirror->getPolyData();
	int EleNum = polyData->GetNumberOfCells();
	double t;
	for (int i = 0; i < EleNum; i++)  //求与反射面的交点
	{
		vtkIdList * p;
		p = polyData->GetCell(i)->GetPointIds();
		double * point;
		point = polyData->GetPoint(p->GetId(0));
		Vector3 NodesXYZ1(point[0], point[1], point[2]);
		point = polyData->GetPoint(p->GetId(1));
		Vector3 NodesXYZ2(point[0], point[1], point[2]);
		point = polyData->GetPoint(p->GetId(2));
		Vector3 NodesXYZ3(point[0], point[1], point[2]);
		if (this->isIntersect(startPiont, direction, NodesXYZ1,
			NodesXYZ2, NodesXYZ3, intersection, t))
		{
			if (t >= 0)
			{
				Vector3 tempa = NodesXYZ1 - NodesXYZ2;
				Vector3 tempb = NodesXYZ1 - NodesXYZ3;
				Vector3 n_light = tempa.Cross(tempb);  //法向量

				isIntersect = true;
				reflex = reflectLight(direction, n_light);
				return;
			}
		}
	}
	isIntersect = false;
}


void calculation::RayTracing::calcNormalOfLine_MirrorByPolyData(
	const Vector3 & startPiont,const Vector3 & direction, Vector3 & normal, 
	Vector3 & intersection, bool & isIntersect, double & t)
{
	vtkSmartPointer<vtkPolyData> polyData = mirror->getPolyData();
	int EleNum = polyData->GetNumberOfCells();
	for (int i = 0; i < EleNum; i++)  //求与反射面的交点
	{
		vtkIdList * p;
		p = polyData->GetCell(i)->GetPointIds();
		double * point;
		point = polyData->GetPoint(p->GetId(0));
		Vector3 NodesXYZ1(point[0], point[1], point[2]);
		point = polyData->GetPoint(p->GetId(1));
		Vector3 NodesXYZ2(point[0], point[1], point[2]);
		point = polyData->GetPoint(p->GetId(2));
		Vector3 NodesXYZ3(point[0], point[1], point[2]);
		if (this->isIntersect(startPiont, direction, NodesXYZ1,
			NodesXYZ2, NodesXYZ3, intersection, t))
		{			
			Vector3 tempa = NodesXYZ1 - NodesXYZ2;
			Vector3 tempb = NodesXYZ1 - NodesXYZ3;
			normal = tempa.Cross(tempb);  //法向量

			isIntersect = true;
			return;			
		}
	}
	isIntersect = false;
}

bool calculation::RayTracing::isIntersect(const Vector3 & orig, const Vector3 & dir,
	const Vector3 & v0, const Vector3 & v1, const Vector3 & v2, 
	Vector3 & intersection, double & t)
{
	double u, v;
	// E1
	Vector3 E1 = v1 - v0;

	// E2
	Vector3 E2 = v2 - v0;

	// P
	Vector3 P = dir.Cross(E2);

	// determinant
	double det = E1.Dot(P);

	Vector3 T;
	T = orig - v0;

	// If determinant is near zero, ray lies in plane of triangle
	//if (det < 0.00000001 && det > -0.00000001)
	//	return false;

	// Calculate u and make sure u <= 1
	u = T.Dot(P);
	double fInvDet = 1.0f / det;
	u *= fInvDet;

	if (u < 0.0 || u > 1)
		return false;

	// Q
	Vector3 Q = T.Cross(E1);

	// Calculate v and make sure u + v <= 1
	v = dir.Dot(Q);
	v *= fInvDet;
	if (v < 0.0 || u + v > 1)
		return false;

	// Calculate t, scale parameters, ray intersects triangle
	t = E2.Dot(Q);
	t *= fInvDet;

	intersection = orig + dir * t;

	return true;
}


Vector3 calculation::RayTracing::reflectLight(const Vector3 & a, const Vector3 & n)
{
	//先单位化
	double absa = pow(a.Dot(a), 0.5);
	double absn = pow(n.Dot(n), 0.5);
	Vector3 tempa = a * (1 / absa);
	Vector3 tempn = n * (1 / absn);
	double I = 2 * tempn.Dot(tempa);
	if (I < 0)
		I = -I;
	else
		tempa = Vector3(0.0, 0.0, 0.0) - tempa;

	return tempn * I + tempa;
}

