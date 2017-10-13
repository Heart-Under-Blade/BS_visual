#include "Particle.h"
#include <fstream>

Particle::Particle() {}

void Particle::SetFromFile(const char *filename)
{
	std::ifstream pfile(filename, std::ios::in);
	//pfile >>
}

void Particle::Init(int facetCount, const complex &refrIndex, double size)
{
	facetNum = facetCount;
	m_refractiveIndex = refrIndex;
	m_mainSize = size;
}

void Particle::RotateCenters()
{
	for (int i = 0; i < facetNum; ++i)
	{
		RotatePoint(defaultFacets[i].center, facets[i].center);
	}
}

void Particle::Rotate(double beta, double gamma, double alpha)
{
	SetRotateMatrix(beta, gamma, alpha);

	// REF: слить всё в один цикл
	for (int i = 0; i < facetNum; ++i)
	{
		for (int j = 0; j < facets[i].size; ++j)
		{
			RotatePoint(defaultFacets[i].arr[j], facets[i].arr[j]);
		}
	}

	RotateNormals();
	RotateCenters();
}

void Particle::RotateGlobal(double phi, double theta, double psy)
{
	SetRotateMatrixGlobal(phi, theta, psy);

	// REF: слить всё в один цикл
	for (int i = 0; i < facetNum; ++i)
	{
		for (int j = 0; j < facets[i].size; ++j)
		{
			Point3f point;
			RotatePoint(facets[i].arr[j], point);
			facets[i].arr[j] = point;
		}
	}

	for (int i = 0; i < facetNum; ++i)
	{
		Point3f point;
		RotatePoint(facets[i].in_normal, point);
		facets[i].in_normal = point;
	}

	SetDParams();

	for (int i = 0; i < facetNum; ++i)
	{
		facets[i].ex_normal = -facets[i].in_normal;
		facets[i].ex_normal.d_param = -facets[i].in_normal.d_param;
	}

	for (int i = 0; i < facetNum; ++i)
	{
		Point3f point;
		RotatePoint(defaultFacets[i].center, point);
		facets[i].center = point;
	}
}

void Particle::Fix()
{
	for (int i = 0; i < facetNum; ++i)
	{
		for (int j = 0; j < facets[i].size; ++j)
		{
			defaultFacets[i].arr[j] = facets[i].arr[j];
		}
	}
}

void Particle::Concate(const std::vector<Particle> &parts)
{
	int i = 0;
	facetNum = 0;

	for (const Particle &part : parts)
	{
		facetNum += part.facetNum;

		for (int j = 0; j < part.facetNum; ++j)
		{
			defaultFacets[i++] = part.facets[j];
		}
	}

	isAggregate = true;
}

const double &Particle::GetMainSize() const
{
	return m_mainSize;
}

const complex &Particle::GetRefractionIndex() const
{
	return m_refractiveIndex;
}

const Symmetry &Particle::GetSymmetry() const
{
	return m_symmetry;
}

void Particle::RotatePoints(double beta, double gamma, double alpha,
							const std::vector<Point3f> &points,
							std::vector<Point3f> &result)
{
	SetRotateMatrix(beta, gamma, alpha);

	for (const Point3f &point : points)
	{
		Point3f resPoint;
		RotatePoint(point, resPoint);
		result.push_back(resPoint);
	}
}

void Particle::Move(float dx, float dy, float dz)
{
	for (int i = 0; i < facetNum; ++i)
	{
		for (int j = 0; j < defaultFacets[i].size; ++j)
		{
			facets[i].arr[j] = defaultFacets[i].arr[j] + Point3f(dx, dy, dz);
		}
	}
}

void Particle::Output()
{
	std::ofstream M("particle.dat", std::ios::out);

	for (int i = 0; i < facetNum; ++i)
	{
		for (int j = 0; j < facets[i].size; ++j)
		{
			Point3f p = facets[i].arr[j];
			M << p._point[0] << ' '
							<< p._point[1] << ' '
							<< p._point[2] << ' '
							<< i ;
			M << std::endl;
		}

		M << std::endl << std::endl;;
	}

	M.close();
}


void Particle::SetDefaultNormals()
{
	for (int i = 0; i < facetNum; ++i)
	{
		defaultFacets[i].SetNormal();
	}
}

void Particle::Reset()
{
	for (int i = 0; i < facetNum; ++i)
	{
		facets[i] = defaultFacets[i];
	}
}

void Particle::SetDefaultCenters()
{
	for (int i = 0; i < facetNum; ++i)
	{
		defaultFacets[i].SetCenter();
	}
}

void Particle::SetRotateMatrix(double beta, double gamma, double alpha)
{
	double cosA, cosB, cosG,
			sinA, sinB, sinG;

	sincos(alpha, &sinA, &cosA);
	sincos(beta,  &sinB, &cosB);
	sincos(gamma, &sinG, &cosG);

	double cosAcosB = cosA*cosB;
	double sinAcosG = sinA*cosG;
	double sinAsinG = sinA*sinG;

	m_rotMatrix[0][0] = cosAcosB*cosG - sinAsinG;
	m_rotMatrix[1][0] = sinAcosG*cosB + cosA*sinG;
	m_rotMatrix[2][0] = -sinB*cosG;

	m_rotMatrix[0][1] = -(cosAcosB*sinG + sinAcosG);
	m_rotMatrix[1][1] = cosA*cosG - sinAsinG*cosB;
	m_rotMatrix[2][1] = sinB*sinG;

	m_rotMatrix[0][2] = cosA*sinB;
	m_rotMatrix[1][2] = sinA*sinB;
	m_rotMatrix[2][2] = cosB;
}

void Particle::SetRotateMatrixGlobal(double phi, double theta, double psy)
{
	double cosF, cosT, cosP,
			sinF, sinT, sinP;

	sincos(phi, &sinF, &cosF);
	sincos(theta, &sinT, &cosT);
	sincos(psy, &sinP, &cosP);

	double sinFsinT = sinF*sinT;
	double cosFsinT = cosF*sinT;

	m_rotMatrix[0][0] = cosT*cosP;
	m_rotMatrix[1][0] = sinFsinT*cosP + cosF*sinP;
	m_rotMatrix[2][0] = -cosFsinT*cosP + sinF*sinP;

	m_rotMatrix[0][1] = -cosT*sinP;
	m_rotMatrix[1][1] = -sinFsinT*sinP + cosF*cosP;
	m_rotMatrix[2][1] = cosFsinT*sinP + sinF*cosP;

	m_rotMatrix[0][2] = sinT;
	m_rotMatrix[1][2] = -sinF*cosT;
	m_rotMatrix[2][2] = cosF*cosT;
}

void Particle::RotateNormals()
{
	for (int i = 0; i < facetNum; ++i)
	{
		RotatePoint(defaultFacets[i].in_normal, facets[i].in_normal);
	}

	SetDParams();

	for (int i = 0; i < facetNum; ++i)
	{
		facets[i].ex_normal = -facets[i].in_normal;
		facets[i].ex_normal.d_param = -facets[i].in_normal.d_param;
	}
}

void Particle::SetDParams()
{
	for (int i = 0; i < facetNum; ++i)
	{
		double d = DotProduct(facets[i].arr[0], facets[i].in_normal);
		facets[i].in_normal.d_param = -d;
	}
}

void Particle::RotatePoint(const Point3f &point, Point3f &result)
{
	result.c_x = point.c_x*m_rotMatrix[0][0] + point.c_y*m_rotMatrix[0][1] + point.c_z*m_rotMatrix[0][2];
	result.c_y = point.c_x*m_rotMatrix[1][0] + point.c_y*m_rotMatrix[1][1] + point.c_z*m_rotMatrix[1][2];
	result.c_z = point.c_x*m_rotMatrix[2][0] + point.c_y*m_rotMatrix[2][1] + point.c_z*m_rotMatrix[2][2];
}

void Particle::SetSymmetry(double beta, double gamma, double alpha)
{
	m_symmetry.beta = beta;
	m_symmetry.gamma = gamma;
	m_symmetry.alpha = alpha;
}
