/*
*	created by liyun 2017/11/27
*   function �������׷�� ���뾵�ӵ�ָ�� ���������ߺͽ����Լ��Ƿ��ཻ
*   version 1.0
*/

#ifndef RAYTRACING_H  
#define RAYTRACING_H

#include <vtkSmartPointer.h>
#include <vtkPolyData.h>

#include "Vector3.h"
#include "STLMirror.h"

#include "Matrix4D.h"
#include "Vector3D.h"
#include <vector>

namespace calculation
{
	class RayTracing
	{
	public:
		RayTracing(STLMirror* _mirror);

		~RayTracing();

		void setMirror(STLMirror* mirror);

		// ����ֱ����mirror�ཻ������������뷨��
		void calcNormalOfLine_Mirror(const Vector3& startPiont,
			const Vector3& direction,
			Vector3 &normal, Vector3 &intersection,
			bool &isIntersect, double &t);

		// ���������ߺͽ����Լ��Ƿ��ཻ
		void calcReflect(const Vector3& startPiont,
			const Vector3& direction,
			Vector3 &reflex, Vector3 &intersection,
			bool &isIntersect);


		// ����������ߺͷ����������
		static Vector3 reflectLight(const Vector3& a, const Vector3& n);
		void SetupNodes(void);

	private:

		// ����ģ���ʷ����ݼ��㷴��
		void calcReflectByPolyDataBatch(const vector<vector<Vector3>>& startPiont,
			const vector<vector<Vector3>>& direction,
			vector<vector<Vector3>> &reflex, vector<vector<Vector3>> &intersection,
			vector<vector<bool>> &isIntersect);

		void calcReflectByPolyData(const Vector3& startPiont,
			const Vector3& direction, Vector3 &reflex,
			Vector3 &intersection,
			bool &isIntersect);

		void calcNormalOfLine_MirrorByPolyData(const Vector3& startPiont,
			const Vector3& direction,
			Vector3 &normal, Vector3 &intersection,
			bool &isIntersect, double &t);

		void calcNormalOfLine_MirrorByPolyData_omp(const Vector3& startPiont,
			const Vector3& direction,
			Vector3 &normal, Vector3 &intersection,
			bool &isIntersect, double &t);


		// ��������ֱ���ཻ�ж�
		bool isIntersect(const Vector3 &orig, const Vector3 &dir,
			const Vector3 &v0, const Vector3 &v1, const Vector3 &v2,
			Vector3 &intersection, double &t);


		STLMirror* mirror;
		vector<Vector3> NodesXYZ1;
		vector<Vector3> NodesXYZ2;
		vector<Vector3> NodesXYZ3;
	};

}

#endif // RAYTRACING_H

