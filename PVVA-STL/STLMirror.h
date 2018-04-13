/*
*	created by liyun 2018/3/13
*   function 导入STL曲面
*   version 1.0
*/

#ifndef STLMirror_H  
#define STLMirror_H

#include <vector>
#include <string>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkActor.h>

class STLMirror
{
public:
	STLMirror(); // 给继承用

	 ~STLMirror();

	void setNameFile(const std::string & file);

	void readData();

	void calActor();
	vtkSmartPointer<vtkActor> getActor() const;


private:
	std::string fileName;
	vtkSmartPointer<vtkPolyData> polyDataSTL;
	vtkSmartPointer<vtkActor> actor;


};



#endif // STLMirror_H
