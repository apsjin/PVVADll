#include "PVVAProgressDialog.h"
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>

PVVAProgressDialog::PVVAProgressDialog(QWidget *parent)
	: QDialog(parent)
{
	setWindowTitle(tr("PVVA Calculation"));
	txtLabel = new QLabel();
	slaverBar = new QProgressBar();
	slaverBar->setRange(0, 100);
	slaverBar->setMinimumWidth(500);
	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->addWidget(txtLabel);
	layout->addWidget(slaverBar);

	closeBtn = new QPushButton(tr("Close"));
	connect(closeBtn, SIGNAL(clicked()), this, SLOT(on_close()));
	stopBtn = new QPushButton(tr("Stop"));
	connect(stopBtn, SIGNAL(clicked()), this, SLOT(on_stop()));

	QHBoxLayout *hlayout = new QHBoxLayout;
	hlayout->addSpacing(300);
	hlayout->addWidget(closeBtn);
	//hlayout->addWidget(stopBtn);

	layout->addLayout(hlayout);

}

PVVAProgressDialog::~PVVAProgressDialog()
{
}

void PVVAProgressDialog::setSlaverValue(int val)
{
	if (0 == val)
		txtLabel->setText(tr("Initializing..."));
	else if (val >= 10 && val < 80)
		txtLabel->setText(tr("calculate reflection..."));
	else if (val >= 80 && val < 90)
		txtLabel->setText(tr("calculate propagation..."));
	else
	{
		txtLabel->setText(tr("All computed, Loading Fields..."));
	}
	slaverBar->setValue(val);
}

void PVVAProgressDialog::on_close()
{
	hide();
}

void PVVAProgressDialog::on_stop()
{
	switch (QMessageBox::question(this, tr("Question"),
		tr("Whether to terminate the calculation immediately?"),
		QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes))
	{
	case QMessageBox::Yes: // 加载源
		close();
		emit sendStop();
		break;
	case QMessageBox::No: //  不加载源
		return;
	default:
		break;
	}
}

void PVVAProgressDialog::closeEvent(QCloseEvent * event)
{
	hide();
}

