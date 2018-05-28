#ifndef MYCLASS_H
#define MYCLASS_H

#include <QtWidgets/QMainWindow>
#include <QVTKWidget.h>
#include <QToolBar>
#include <QAction>
#include "../PVVADll/PVVADll.h"
#include "../CUDAPhysicalOptics/CUDAPhysicalOptics.h"//这个是对PO DLL的引用
#include "PVVAProgressDialog.h"

#include "vtkSmartPointer.h"
#include "vtkOrientationMarkerWidget.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkAxesActor.h"
#include "vtkImageActor.h"
#include "vtkActor.h"

class MyClass : public QMainWindow
{
	Q_OBJECT

public:
	MyClass(QWidget *parent = 0);
	~MyClass();

private slots:
	void createField();
	void createSTL();
	void createPVVA();
	void createPO();
	void toReceivePVVA();
	void toReceivePO();

private:
	PVVADll * PVVADllPtr;
	CUDAPhysicalOptics PODLLPtr;

	vtkSmartPointer<vtkOrientationMarkerWidget> widget1;
	QVTKWidget widget; // vtk 显示窗口
	vtkSmartPointer<vtkRenderWindowInteractor> interactor; // 交互
	vtkSmartPointer<vtkRenderer> renderer;
	vtkSmartPointer<vtkAxesActor> axes; // 坐标

	//----------- ToolBar ------------------- 
	QToolBar * fileTool;   //工具栏

	//----------- Action ----------------- 
	//文件菜单项
	QAction * FieldAction;
	QAction * STLAction;
	QAction * PVVAAction;
	QAction * POAction;

	vtkSmartPointer<vtkImageActor> fieldInActor;
	vtkSmartPointer<vtkImageActor> fieldOutActor;
	vtkSmartPointer<vtkActor> STLActor;
	PVVAProgressDialog *PVVAprogressDialog;



};

#endif // MYCLASS_H
