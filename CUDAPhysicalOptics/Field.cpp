#include "Field.h"
#include "Constant_Val.h"

Field::Field()
{
	init();
}

Field::~Field()
{
	freeMemory();

}

void Field::init()
{
	N_width = 0; M_depth = 0;

	ds = 0;
	isSource = false;

	data.resize(2);
}

void Field::freeMemory()
{

}

void Field::allocateMemory()
{
	Ex.resize(N_width);
	Ey.resize(N_width);
	for (int i = 0; i < N_width; ++i)
	{
		Ex[i].resize(M_depth);
		Ey[i].resize(M_depth);
	}

	if (!isSource) // ����Դ�ŷ��������ĳ�����
	{
		Ez.resize(N_width);
		Hx.resize(N_width);
		Hy.resize(N_width);
		Hz.resize(N_width);
		for (int i = 0; i < N_width; ++i)
		{
			Ez[i].resize(M_depth);
			Hx[i].resize(M_depth);
			Hy[i].resize(M_depth);
			Hz[i].resize(M_depth);
		}
	}
}

void Field::setNM(int N, int M)
{
	N_width = N;
	M_depth = M;
}

void Field::getNM(int & N, int & M) const
{
	N = N_width;
	M = M_depth;
}

void Field::setPlane(const GraphTrans & _graphTransPara, double _ds)
{
	graphTrans = _graphTransPara;
	ds = _ds;
	data[0] = N_width * _ds;
	data[1] = M_depth * _ds;

}

void Field::setField(complex<double>** _Ex, complex<double>** _Ey,
	complex<double>** _Ez, complex<double>** _Hx, complex<double>** _Hy,
	complex<double>** _Hz)
{
	allocateMemory();
	for (int i = 0; i < N_width; i++)
		for (int j = 0; j < M_depth; j++)
		{
			Ex[i][j] = _Ex[i][j];
			Ey[i][j] = _Ey[i][j];
			Ez[i][j] = _Ez[i][j];
			Hx[i][j] = _Hx[i][j];
			Hy[i][j] = _Hy[i][j];
			Hz[i][j] = _Hz[i][j];
		}
}

const vector<vector<complex<double>>>& Field::getEx() const
{
	return Ex;
}

const vector<vector<complex<double>>>& Field::getEy() const
{
	return Ey;
}

void Field::setDs(double _Ds)
{
	ds = _Ds;
}

double Field::getDs() const
{
	return ds;
}

void Field::getSourcePara(GraphTrans & _graphTransPara,
	int & _N_width, int & _M_depth, double & _ds) const
{
	_graphTransPara = graphTrans;

	_N_width = N_width;
	_M_depth = M_depth;
	_ds = ds;
}

void Field::save(const std::string & fileName) const
{
	if (isSource)
	{
		ofstream outfile(fileName + "_ExEy.txt");
		outfile << "Ex   Ey   Ez" << endl;
		for (int i = 0; i < N_width; i++)
		{
			for (int j = 0; j < M_depth; j++)
			{
				complex<double> temp = Ex[i][j];
				double tempD = pow((temp.real() * temp.real() +
					temp.imag() * temp.imag()), 0.5);
				outfile << tempD << " ";
				if (temp.real() != 0)
					tempD = atan(temp.imag() / temp.real());
				else
					tempD = 0;
				outfile << tempD << " ";

				temp = Ey[i][j];
				tempD = pow((temp.real() * temp.real() +
					temp.imag() * temp.imag()), 0.5);
				outfile << tempD << " ";
				if (temp.real() != 0)
					tempD = atan(temp.imag() / temp.real());
				else
					tempD = 0;
				outfile << tempD << " ";
				outfile << "0 0" << "\n";
			}
		}
		outfile.close();

	}
	else 
	{
		ofstream outfile(fileName + "_ExEyEz.txt");
		outfile << "Ex   Ey   Ez" << endl;
		for (int i = 0; i < N_width; i++)
		{
			for (int j = 0; j < M_depth; j++)
			{
				//Ex
				complex<double> temp = Ex[i][j];
				double tempD = pow((temp.real() * temp.real() +
					temp.imag() * temp.imag()), 0.5);
				outfile << tempD << " ";
				if (temp.real() != 0)
					tempD = atan(temp.imag() / temp.real());
				else
					tempD = 0;
				outfile << tempD << " ";

				//Ey
				temp = Ey[i][j];
				tempD = pow((temp.real() * temp.real() +
					temp.imag() * temp.imag()), 0.5);
				outfile << tempD << " ";
				if (temp.real() != 0)
					tempD = atan(temp.imag() / temp.real());
				else
					tempD = 0;
				outfile << tempD << " ";

				//Ez
				temp = Ez[i][j];
				tempD = pow((temp.real() * temp.real() +
					temp.imag() * temp.imag()), 0.5);
				outfile << tempD << " ";
				if (temp.real() != 0)
					tempD = atan(temp.imag() / temp.real());
				else
					tempD = 0;
				outfile << tempD << " ";

				outfile << "\n";
			}
		}
		outfile.close();

		ofstream outfile1(fileName + "_HxHyHz.txt");
		outfile1 << "Hx   Hy   Hz" << endl;
		for (int i = 0; i < N_width; i++)
		{
			for (int j = 0; j < M_depth; j++)
			{
				//Hx
				complex<double> temp = Hx[i][j];
				double tempD = pow((temp.real() * temp.real() +
					temp.imag() * temp.imag()), 0.5);
				outfile1 << tempD << " ";
				if (temp.real() != 0)
					tempD = atan(temp.imag() / temp.real());
				else
					tempD = 0;
				outfile1 << tempD << " ";

				//Hy
				temp = Hy[i][j];
				tempD = pow((temp.real() * temp.real() +
					temp.imag() * temp.imag()), 0.5);
				outfile1 << tempD << " ";
				if (temp.real() != 0)
					tempD = atan(temp.imag() / temp.real());
				else
					tempD = 0;
				outfile1 << tempD << " ";

				//Hz
				temp = Hz[i][j];
				tempD = pow((temp.real() * temp.real() +
					temp.imag() * temp.imag()), 0.5);
				outfile1 << tempD << " ";
				if (temp.real() != 0)
					tempD = atan(temp.imag() / temp.real());
				else
					tempD = 0;
				outfile1 << tempD << " ";

				outfile << "\n";
			}
		}
		outfile.close();
	}
}

void Field::readData()
{
	ifstream file(fileAddress);
	string temp;
	getline(file, temp);
	istringstream tempLine(temp);

	double tx, ty, tz, rx, ry, rz, rth;
	tempLine >> tx >> ty >> tz >> rx >> ry >> rz >> rth;
	graphTrans.setGraphTransPar(tx, ty, tz, rx, ry, rz, rth);

	tempLine >> N_width >> M_depth >> ds;

	isSource = true;
	allocateMemory();
		
	for (int i = 0; i < N_width; i++)
	{
		for (int j = 0; j < M_depth; j++)
		{
			getline(file, temp);
			istringstream tempLine(temp);

			double a1, p1, a2, p2;
			tempLine >> a1 >> p1 >> a2 >> p2;

			Ex[i][j] = complex<double>(a1*cos(p1 / 180 * Pi),
				a1*sin(p1 / 180 * Pi));
			Ey[i][j] = complex<double>(a2*cos(p2 / 180 * Pi),
				a2*sin(p2 / 180 * Pi));
		}
	}
	file.close();
}

void Field::setFileAddress(const string & file)
{
	fileAddress = file;
}



