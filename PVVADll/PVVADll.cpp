#include "PVVADll.h"
#include "PVVA.h"
#include "Field.h"
#include "STLMirror.h"

PVVADll::PVVADll()
{
	this->returnFloat = NULL;
	this->user = NULL;
}

PVVADll::~PVVADll()
{
}

void PVVADll::setInputFile(const std::string & file)
{
	inputFieldFile = file;
	
}

void PVVADll::setModelFile(const std::string & file)
{
	stlMirrorFile = file;
	
}

void PVVADll::calculate(double fre, double dis)
{
	Field inputField;
	inputField.setFileAddress(inputFieldFile);
	inputField.readData();

	STLMirror stlMirror;
	stlMirror.setNameFile(stlMirrorFile);
	stlMirror.readData();

	PVVA pvva;
	// 设置单位
	pvva.SetReturnFloat(returnFloat, user);
	pvva.setUnit(1);
	// 设置频率
	pvva.setFre(fre);
	// 读入源并分配内存
	pvva.setSource(&inputField);

	pvva.setMirror(&stlMirror);
	pvva.CalZ0Theta();
	pvva.ReflectCUDA();
	// pvva.Reflect();
	pvva.InterVal();
	
	pvva.Result(dis);

	//pvva.getField(&inputField);
}

void PVVADll::SetReturnFloat(void(*returnFloat)(float, void *), void * _user)
{
	this->returnFloat = returnFloat;
	this->user = _user;
}
