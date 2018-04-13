#ifndef calculationDialog_H
#define calculationDialog_H

#include <QtWidgets/QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>


class calculationDialog : public QDialog
{
	Q_OBJECT

public:
	calculationDialog(QWidget *parent = 0);
	~calculationDialog();

	void getData(double & fre, double & dis);

	public slots:

	void on_OK();
	void on_cancel();

private:

	QLabel * lengthLabel;
	QLineEdit * lengthLineEdit;
	QLabel * gaussLabel;
	QLineEdit * gaussLineEdit;
	QPushButton *cancelBtn;
	QPushButton *OKBtn;

};



#endif // calculationDialog_H
