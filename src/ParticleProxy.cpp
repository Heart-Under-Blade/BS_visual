#include "ParticleProxy.h"

#include "Particle.h"
#include "Hexagonal.h"
#include "HexagonalAggregate.h"
#include "CertainAggregate.h"
#include "ConcaveHexagonal.h"
#include "Bullet.h"
#include "BulletRosette.h"
#include "TiltedHexagonal.h"

#include "global.h"
#include "TracingConvex.h"
#include "TracingConcave.h"
#include "Mueller.hpp"

using namespace std;

ParticleProxy::ParticleProxy()
{
	particleTypes.insert(PType::Column, "Column");
	particleTypes.insert(PType::HollowColumn, "Hollow column");
	particleTypes.insert(PType::Bullet, "Bullet");
	particleTypes.insert(PType::BulletRosette, "Bullet-rosette");
	particleTypes.insert(PType::Aggregate, "Aggregate");
}

ParticleProxy::~ParticleProxy()
{
	delete particle;
	delete tracing;
}

QString RecoverTrack(const Beam &beam, int facetNum)
{
	QString track;
	int coef = facetNum + 1;
	std::vector<int> tmp_track;

	int tmpId = beam.id/coef;
	for (int i = 0; i <= beam.level; ++i)
	{
		int tmp = tmpId%coef;
		tmpId -= tmp;
		tmpId /= coef;
		tmp -= 1;
		tmp_track.push_back(tmp);
	}

	if (!tmp_track.empty())
	{
		track = QString::number(tmp_track.at(tmp_track.size()-1));

		for (int i = tmp_track.size()-2; i >= 0; --i)
		{
			track += " - " + QString::number(tmp_track.at(i));
		}
	}

	return track.trimmed();
}

void ParticleProxy::SetParticle(const QString &type, double refrIndex,
								double height, double diameter,
								double additional)
{
	if (particle != nullptr)
	{
		delete particle;
	}

	if (type == particleTypes.value(PType::Column))
	{
		ptype = PType::Column;
		particle = new Hexagonal(refrIndex, diameter, height);
	}
	else if (type == particleTypes.value(PType::HollowColumn))
	{
		ptype = PType::HollowColumn;
		particle = new ConcaveHexagonal(refrIndex, diameter, height, additional);
	}
	else if (type == particleTypes.value(PType::Bullet))
	{
		ptype = PType::Bullet;
		additional = (diameter*sqrt(3)*tan(DegToRad(62)))/4;
		particle = new Bullet(refrIndex, diameter, height, additional);
	}
	else if (type == particleTypes.value(PType::BulletRosette))
	{
		ptype = PType::BulletRosette;
		additional = (diameter*sqrt(3)*tan(DegToRad(62)))/4;
		particle = new BulletRosette(refrIndex, diameter, height, additional);
	}
	else if (type == particleTypes.value(PType::Aggregate))
	{
		ptype = PType::Aggregate;
		particle = new CertainAggregate(refrIndex, additional);
	}
}

void RotateMuller(const Point3f &dir, matrix &M)
{
	const float &x = dir.cx;
	const float &y = dir.cy;

	double tmp = y*y;

	tmp = acos(x/sqrt(x*x+tmp));

	if (y < 0)
	{
		tmp = M_2PI-tmp;
	}

	tmp *= -2.0;
	RightRotateMueller(M, cos(tmp), sin(tmp));
}

void ParticleProxy::Trace(double beta, double gamma, double alpha, int reflNum)
{
	beamData.clear();

	Point3f incidentDir(0, 0, -1);
	Point3f polarizationBasis(0, 1, 0);
	bool isOpticalPath = true;

	if (tracing != nullptr)
	{
		delete tracing;
	}

	if (ptype == PType::HollowColumn ||
			ptype == PType::Aggregate ||
			ptype == PType::BulletRosette)
	{
		tracing = new TracingConcave(particle, incidentDir, isOpticalPath,
									 polarizationBasis, reflNum);
	}
	else
	{
		tracing = new TracingConvex(particle, incidentDir, isOpticalPath,
									polarizationBasis, reflNum);
	}

	vector<Beam> outBeams;
	double betaR = DegToRad(beta);
	double gammaR = DegToRad(gamma);
	tracing->SplitBeamByParticle(betaR, gammaR, outBeams, DegToRad(alpha));

	for (Beam &beam : outBeams)
	{
		BeamInfo info;
		info.beam = beam;

		beam.RotateSpherical(-incidentDir, polarizationBasis);

		double cross = tracing->BeamCrossSection(beam);
		info.area = cross/**sinBeta*/;
		matrix M = Mueller(beam.J);

		double phi, theta;
		beam.GetSpherical(phi, theta);

		const float &z = beam.direction.cz;

		if (z >= 1-DBL_EPSILON && z <= DBL_EPSILON-1)
		{
			const float &y = beam.direction.cy;

			if (y*y > DBL_EPSILON)
			{	// rotate the Mueller matrix of the beam to appropriate coordinate system
				RotateMuller(beam.direction, M);
			}
		}

		info.thetaDeg = RadToDeg(theta);
		info.phiDeg = RadToDeg(phi);
		info.track = RecoverTrack(beam, particle->facetNum);

		QString dir = QString("%1, %2").arg(info.phiDeg).arg(info.thetaDeg);

		info.M   << M[0][0] << M[0][1] << M[0][2] << M[0][3]
				 << M[1][0] << M[1][1] << M[1][2] << M[1][3]
				 << M[2][0] << M[2][1] << M[2][2] << M[2][3]
				 << M[3][0] << M[3][1] << M[3][2] << M[3][3];

		if (beamData.contains(dir))
		{
			auto it = beamData.find(dir);
			(*it).insert("("+QString::number((*it).size()+1)+"): "+info.track, info);
		}
		else
		{
			BeamData data;
			data.insert("("+QString::number(data.size()+1)+"): "+info.track, info);
			beamData.insert(dir, data);
		}
//		contr.scatMatrix.insert(0, thetaDeg, area*M);
	}
	//	int b =0;
}

QStringList ParticleProxy::GetParticleTypes() const
{
	return particleTypes.values();
}

QString ParticleProxy::GetAdditionalParticleParam(const QString &type) const
{
	QString param;

	if (type == particleTypes.value(PType::HollowColumn))
	{
		param = "Cavity angle";
	}
	else if (type == particleTypes.value(PType::Aggregate))
	{
		param = "Size index";
	}

	return param;
}

QString ParticleProxy::GetTracks()
{
	QString res;

	foreach (QString key, beamData.keys())
	{
		BeamData data = beamData.value(key);
		res += key+"\t"+QString::number(data.size())+"\n";

		foreach (QString dkey, data.keys())
		{
			BeamInfo info = data.value(dkey);
			res += " "+dkey+"\n";
		}
	}

	return res;
}

BeamInfo &ParticleProxy::GetBeamInfo(const QString &trackKey, const QString &beamKey)
{
	return beamData[trackKey][beamKey];
}
