#include "STLMirror.h"
#include <vtkSTLReader.h>
#include <vtkTransform.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>


STLMirror::STLMirror()
{
}

STLMirror::~STLMirror()
{
}


void STLMirror::setNameFile(const std::string & file)
{
	fileName.clear();
	fileName = file;
	readData();
}

void STLMirror::calActor()
{
	vtkSmartPointer<vtkPolyDataMapper>mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(polyDataSTL);
	mapper->ScalarVisibilityOff();

	auto property = vtkSmartPointer<vtkProperty>::New();
	property->SetOpacity(1);
	property->SetColor(180.0 / 255.0, 180.0 / 255.0, 180.0 / 255.0);

	actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);
	actor->SetProperty(property);
}

vtkSmartPointer<vtkActor> STLMirror::getActor() const
{
	return actor;
}


void STLMirror::readData()
{
	vtkSmartPointer<vtkSTLReader> reader =
		vtkSmartPointer<vtkSTLReader>::New();
	reader->SetFileName(fileName.c_str());
	reader->Update();
	polyDataSTL = reader->GetOutput();

	calActor();
}
