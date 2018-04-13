#include "Dialog.h"
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>


calculationDialog::calculationDialog(QWidget *parent)
	: QDialog(parent)
{
	setWindowTitle(tr("Phase Correction"));
	
	lengthLabel = new QLabel(tr("Frequency:"));
	lengthLineEdit = new QLineEdit(tr("1e10"));

	gaussLabel = new QLabel(tr("Propagation distance:"));
	gaussLineEdit = new QLineEdit(tr("0.5"));

	QGridLayout *gridLayout = new QGridLayout;
	gridLayout->addWidget(lengthLabel, 0, 0);
	gridLayout->addWidget(lengthLineEdit, 0, 1);
	gridLayout->addWidget(gaussLabel, 2, 0);
	gridLayout->addWidget(gaussLineEdit, 2, 1);

	OKBtn = new QPushButton(tr("OK"));
	connect(OKBtn, SIGNAL(clicked()), this, SLOT(on_OK()));
	cancelBtn = new QPushButton(tr("Cancel"));
	connect(cancelBtn, SIGNAL(clicked()), this, SLOT(on_cancel()));

	QHBoxLayout *hlayout = new QHBoxLayout;
	hlayout->addSpacing(100);
	hlayout->addWidget(OKBtn);
	hlayout->addWidget(cancelBtn);

	QVBoxLayout *layout = new QVBoxLayout(this);

	layout->addLayout(gridLayout);
	layout->addLayout(hlayout);

}

calculationDialog::~calculationDialog()
{
}

void calculationDialog::getData(double & fre, double & dis)
{
	fre = lengthLineEdit->text().toDouble();
	dis = gaussLineEdit->text().toDouble();
}

void calculationDialog::on_OK()
{
	accept();
}

void calculationDialog::on_cancel()
{
	close();
}
