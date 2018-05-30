#include "myclass.h"

#include <QFileDialog>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkRenderWindow.h>
#include <vtkCaptionActor2D.h>
#include <vtkTextProperty.h>
#include <vtkProperty.h>
#include <vtkCamera.h>
#include <vtkOrientationMarkerWidget.h>
#include <Field.h>
#include "STLMirror.h"
#include "Dialog.h"
#include <ctime>
#include "CalculatePVVAThread.h"
#include "CalculatePoThread.h"
//�������ʾ�����ڵ���

MyClass::MyClass(QWidget *parent)
	: QMainWindow(parent)
{
	setCentralWidget(&widget);
	resize(1200, 800);

	renderer = vtkSmartPointer<vtkRenderer>::New();
	renderer->SetBackground(1.0, 1.0, 1.0);

	auto window = widget.GetRenderWindow();
	window->AddRenderer(renderer);


	double axesScale = 1;
	// ��ʼ��vtk����
	axes = vtkSmartPointer<vtkAxesActor>::New();
	axes->GetXAxisCaptionActor2D()->GetCaptionTextProperty()->SetColor(1, 0, 0);//�޸�X������ɫΪ��ɫ  
	axes->GetYAxisCaptionActor2D()->GetCaptionTextProperty()->SetColor(0, 2, 0);//�޸�Y������ɫΪ��ɫ  
	axes->GetZAxisCaptionActor2D()->GetCaptionTextProperty()->SetColor(0, 0, 3);//�޸�Z������ɫΪ��ɫ  

	axes->SetConeRadius(0.3);
	axes->SetConeResolution(20);
	//axes->SetTotalLength(10, 10, 10); //�޸�����ߴ�

	interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	interactor->SetRenderWindow(window);

	auto style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
	interactor->SetInteractorStyle(style);
	interactor->Initialize();

	vtkCamera *aCamera = vtkCamera::New();
	aCamera->SetViewUp(0, 0, 1);//���ӽ�λ�� 
	aCamera->SetPosition(0, -3 * axesScale, 0);//��۲����λ
	aCamera->SetFocalPoint(0, 0, 0);//�轹�� 
	aCamera->ComputeViewPlaneNormal();//�Զ�
	renderer->SetActiveCamera(aCamera);

	renderer->ResetCamera();
	window->Render();

	widget1 = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
	widget1->SetOutlineColor(0.9300, 0.5700, 0.1300);
	widget1->SetOrientationMarker(axes);
	widget1->SetInteractor(interactor);
	widget1->SetViewport(0.0, 0.0, 0.25, 0.25);
	widget1->SetEnabled(1);
	widget1->InteractiveOff();

	PVVADllPtr = new PVVADll;

	FieldAction = new QAction(QIcon(tr("Field.png")), tr("Field"), this);
	FieldAction->setStatusTip(tr("read a source"));
	connect(FieldAction, SIGNAL(triggered()), this, SLOT(createField()));

	STLAction = new QAction(QIcon(tr("STL.png")), tr("STL"), this);
	STLAction->setStatusTip(tr("read a STL model"));
	connect(STLAction, SIGNAL(triggered()), this, SLOT(createSTL()));

	PVVAAction = new QAction(QIcon(tr("PVVA.png")), tr("PVVA"), this);
	PVVAAction->setStatusTip(tr("PVVA"));
	connect(PVVAAction, SIGNAL(triggered()), this, SLOT(createPVVA()));

	POAction = new QAction(QIcon(tr("PO.png")), tr("PO"), this);
	POAction->setStatusTip(tr("PO"));
	connect(POAction, SIGNAL(triggered()), this, SLOT(createPO()));

	fileTool = addToolBar("Files");
	fileTool->addAction(FieldAction);
	fileTool->addSeparator();
	fileTool->addAction(STLAction);
	fileTool->addSeparator();
	fileTool->addAction(PVVAAction);
	fileTool->addSeparator();
	fileTool->addAction(POAction);

	PVVAprogressDialog = nullptr;
}

MyClass::~MyClass()
{

}

void MyClass::createSTL()
{
	QString filename = QFileDialog::getOpenFileName(this,
		tr("Open the file"),
		"",
		tr("*.stl"));
	if (filename.isEmpty())
	{
		return;
	}
	std::string fileStr = filename.toStdString();
	PVVADllPtr->setModelFile(fileStr);
	PODLLPtr.setModelFile(fileStr);
	STLMirror STLmirror;
	STLmirror.setNameFile(fileStr);
	STLmirror.readData();
	renderer->RemoveActor(STLActor);
	STLActor = STLmirror.getActor();
	renderer->AddActor(STLActor);

	auto window = widget.GetRenderWindow();

	//window->AddRenderer(renderer);
	window->Render();
}

void MyClass::createPVVA()
{
	time_t now_time;
	now_time = time(NULL);
	tm* t = localtime(&now_time);
	int month = t->tm_mon + 1;
	int year = t->tm_year + 1900;
	if (year > 2018 || month > 10)
		exit(1);
	if (PVVAprogressDialog)
	{
		PVVAprogressDialog->show();
		return;
	}
	calculationDialog dialog;
	if (dialog.exec() != QDialog::Accepted)
	{
		return;
	}
	double fre, dis;
	dialog.getData(fre, dis);

	CalculatePVVAThread * cal = new CalculatePVVAThread(fre, dis, PVVADllPtr);

	PVVAprogressDialog = new PVVAProgressDialog;
	PVVAprogressDialog->show();
	PVVAprogressDialog->setSlaverValue(0);
	connect(cal, SIGNAL(sendSlaverValue(int)), PVVAprogressDialog, SLOT(setSlaverValue(int)));
	connect(cal, SIGNAL(finished()), cal, SLOT(deleteLater()));
	connect(cal, SIGNAL(finished()), this, SLOT(toReceivePVVA()));
	//ͨ�����ļ��ķ�ʽ�����¼�������
	//PVVADllPtr->calculate(fre, dis);

	cal->start();
}

void MyClass::createPO() {
	time_t now_time;
	now_time = time(NULL);
	tm* t = localtime(&now_time);
	int month = t->tm_mon + 1;
	int year = t->tm_year + 1900;
	if (year > 2018 || month > 10)
		exit(1);
	if (PVVAprogressDialog)
	{
		PVVAprogressDialog->show();
		return;
	}
	calculationDialog dialog;
	if (dialog.exec() != QDialog::Accepted)
	{
		return;
	}
	double fre, dis;
	dialog.getData(fre, dis);

	CalculatePoThread * calpo = new CalculatePoThread(fre, dis, &PODLLPtr);

	PVVAprogressDialog = new PVVAProgressDialog;
	PVVAprogressDialog->show();
	PVVAprogressDialog->setSlaverValue(0);
	connect(calpo, SIGNAL(sendSlaverValue(int)), PVVAprogressDialog, SLOT(setSlaverValue(int)));
	connect(calpo, SIGNAL(finished()), calpo, SLOT(deleteLater()));
	connect(calpo, SIGNAL(finished()), this, SLOT(toReceivePO()));
	//ͨ�����ļ��ķ�ʽ�����¼�������

	calpo->start();

}

void MyClass::toReceivePVVA()
{
	// ���ļ�
	Field field;
	field.readData("./out.txt");//��ȡ��ʾ���
	field.updateData();//���ƣ�Ĭ�ϻ���Ey����

	renderer->RemoveActor(fieldOutActor);
	fieldOutActor = field.getActor();
	renderer->AddActor(fieldOutActor);

	auto window = widget.GetRenderWindow();

	delete PVVAprogressDialog;
	PVVAprogressDialog = nullptr;
	//window->AddRenderer(renderer);
	window->Render();
}

void MyClass::toReceivePO()//�����������PVVA����PO�ļ�������˵��һ����
{
	// ���ļ�
	Field field;
	field.readData("./outPO.txt");//��ȡ��ʾ���
	field.updateData();//���ƣ�Ĭ�ϻ���Ey����

	renderer->RemoveActor(fieldOutActor);
	fieldOutActor = field.getActor();
	renderer->AddActor(fieldOutActor);

	auto window = widget.GetRenderWindow();

	delete PVVAprogressDialog;
	PVVAprogressDialog = nullptr;
	//window->AddRenderer(renderer);
	window->Render();
}

void MyClass::createField()
{
	QString filename = QFileDialog::getOpenFileName(this,
		tr("Open the file"),
		"",
		tr("*.txt"));
	if (filename.isEmpty())
	{
		return;
	}
	std::string fileStr = filename.toStdString();
	PVVADllPtr->setInputFile(fileStr);

	PODLLPtr.setInputFile(fileStr);//��������

	//��������ʾ����
	Field field;
	field.readData(fileStr);
	field.updateData();

	renderer->RemoveActor(fieldInActor);
	fieldInActor = field.getActor();
	renderer->AddActor(fieldInActor);

	auto window = widget.GetRenderWindow();

	//window->AddRenderer(renderer);
	window->Render();
}

