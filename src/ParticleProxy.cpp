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

#include <iostream>
using namespace std;

#ifdef _DEBUG
using namespace std;
ofstream debfile("deb.dat", ios::out);
#endif

ParticleProxy::ParticleProxy()
{
	particle = nullptr;
	tracing = nullptr;

	particleTypes.insert(PType::Column, "Column");
	particleTypes.insert(PType::HollowColumn, "Hollow column");
	particleTypes.insert(PType::Bullet, "Bullet");
	particleTypes.insert(PType::BulletRosette, "Bullet-rosette");
	particleTypes.insert(PType::Aggregate, "Aggregate");
}

ParticleProxy::~ParticleProxy()
{
	if (particle != nullptr)
	{
		delete particle;
	}

	if (tracing != nullptr)
	{
		delete tracing;
	}
}

QString ParticleProxy::RecoverTrack(long long id, int level)
{
	QString track;
	int coef = particle->facetNum + 1;
	std::vector<int> tmp_track;

	int tmpId = id/coef;
	for (int i = 0; i <= level; ++i)
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
			track += "-" + QString::number(tmp_track.at(i));
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

	particle->Rotate(0, DegToRad(90), 0);
}

void RotateMuller(const Point3f &dir, matrix &M)
{
	const float &z = dir.c_z;

	if (z >= 1-DBL_EPSILON && z <= DBL_EPSILON-1)
	{
		const float &y = dir.c_y;

		if (y*y > DBL_EPSILON)
		{	// rotate the Mueller matrix of the beam to appropriate coordinate system

			const float &x = dir.c_x;
			const float &y = dir.c_y;

			double tmp = y*y;

			tmp = acos(x/sqrt(x*x+tmp));

			if (y < 0)
			{
				tmp = M_2PI-tmp;
			}

			tmp *= -2.0;
			RightRotateMueller(M, cos(tmp), sin(tmp));
		}
	}
}

void ParticleProxy::SetTracing(const Point3f &incidentDir, int reflNum,
							   const Point3f &polarizationBasis)
{
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
}


void ParticleProxy::RotateStates(const Angle &angle, Beam &beam)
{
	for (BeamState &state : beam.states)
	{
		vector<Point3f> statePol, resPol;

		for (int i = 0; i < state.size; ++i)
		{
			statePol.push_back(state.arr[i]);
		}

		particle->RotatePointsGlobal(angle.beta, angle.gamma, angle.alpha,
									 statePol, resPol);

		state.Clear();

		for (Point3f &p : resPol)
		{
			state.Add(p);
		}
	}
}

void ParticleProxy::SetBeamInfo(int beamNumber, Beam &beam, BeamInfo &info)
{
	info.beam = beam;
	info.area = tracing->BeamCrossSection(beam)/**sinBeta*/;

	double phi, theta;
	beam.GetSpherical(phi, theta);
	info.thetaDeg = 180 - RadToDeg(theta);
	info.phiDeg = RadToDeg(phi);

	info.track = RecoverTrack(beam.id, beam.level)+":";
	info.number = beamNumber;

	matrix M = Mueller(beam.J);
	RotateMuller(beam.direction, M);
	info.M   << M[0][0] << M[0][1] << M[0][2] << M[0][3]
			 << M[1][0] << M[1][1] << M[1][2] << M[1][3]
			 << M[2][0] << M[2][1] << M[2][2] << M[2][3]
			 << M[3][0] << M[3][1] << M[3][2] << M[3][3];
}

void ParticleProxy::Trace(const Angle &angle, const Angle &viewAngle, int reflNum)
{
	beamData.clear();

	Point3f incidentDir(0, 0, -1);
	Point3f polarizationBasis(0, 1, 0);

	SetTracing(incidentDir, reflNum, polarizationBasis);

	vector<Beam> outBeams;
	tracing->SplitBeamByParticle(angle.beta, angle.gamma, outBeams, angle.alpha);

	int count = 0;

	for (Beam &beam : outBeams)
	{
		beam.RotateSpherical(-incidentDir, polarizationBasis);

//		RotateStates(viewAngle, beam);

		BeamInfo info;
		SetBeamInfo(++count, beam, info);

		QString dir = QString("%1, %2").arg(info.phiDeg).arg(info.thetaDeg);

		if (beamData.contains(dir))
		{
			auto it = beamData.find(dir);
			(*it).insertMulti(info.track, info);
		}
		else
		{
			BeamData data;
			data.insert(info.track, info);
			beamData.insert(dir, data);
		}
	}
}

void ParticleProxy::TranslateCoordinates(QPolygonF &pol)
{
	for (int i = 0; i < pol.size(); ++i)
	{
		pol[i].setY(-pol[i].y());
	}
}

void ParticleProxy::RotateParticle(const Angle &rotAngle, const Angle &viewAngle)
{
	particle->Rotate(rotAngle.beta, rotAngle.gamma, rotAngle.alpha);
	particle->RotateGlobal(viewAngle.beta, viewAngle.gamma, viewAngle.alpha);
}

QVector<QPointF> ParticleProxy::RotateAxes(const Angle &viewAngle)
{
	vector<Point3f> resAxes;
	particle->RotatePointsGlobal(viewAngle.beta, viewAngle.gamma, viewAngle.alpha,
								 vector<Point3f>{axes.x, axes.y, axes.z}, resAxes);

	double size = particle->GetMainSize();
	resAxes[0] = resAxes[0] * size;
	resAxes[1] = resAxes[1] * size;
	resAxes[2] = resAxes[2] * size;

	QPolygonF res;
	res.append(QPointF{resAxes.at(0).c_x, resAxes.at(0).c_y});
	res.append(QPointF{resAxes.at(1).c_x, resAxes.at(1).c_y});
	res.append(QPointF{resAxes.at(2).c_x, resAxes.at(2).c_y});

	TranslateCoordinates(res);
	return res;
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

void ParticleProxy::GetTrack(int beamNumber, const Angle &viewAngle,
							 QVector<NumberedFacet> &track)
{
	BeamInfo info;
	GetBeamByNumber(beamNumber, info);

	RotateStates(viewAngle, info.beam);

	for (BeamState &state : info.beam.states)
	{
		QPolygonF pol;

		for (int j = 0; j < state.size; ++j)
		{
			Point3f &p = state.arr[j];

//			vector<Point3f> resP;
//			resP.push_back(p);
//			particle->RotatePointsGlobal(viewAngle.beta, viewAngle.gamma, viewAngle.alpha,
//										 vector<Point3f>{p}, resP);

//			pol.append(QPointF(resP.at(0).c_x, resP.at(0).c_y));
			pol.append(QPointF(p.c_x, p.c_y));
		}

		TranslateCoordinates(pol);
		track.append(NumberedFacet{state.facetID, pol});
	}
}

QString ParticleProxy::GetBeamDataString()
{
	QString res;

	foreach (QString key, beamData.keys())
	{
		BeamData data = beamData.value(key);
		res += key+"\t"+QString::number(data.size())+"\n";

		foreach (QString dkey, data.uniqueKeys())
		{
			auto infos = data.values(dkey);

			foreach (BeamInfo info, infos)
			{
				res += " "+dkey+"\t"+QString::number(info.number)+"\n";
			}
		}
	}

	return res;
}

QString ParticleProxy::GetBeamDataString(const QString &searchLine)
{
	QString res;

	foreach (QString key, beamData.keys())
	{
		BeamData data = beamData.value(key);

		QStringList children;

		foreach (QString dkey, data.uniqueKeys())
		{
			if (dkey.startsWith(searchLine))
			{
				auto infos = data.values(dkey);

				foreach (BeamInfo info, infos)
				{
					children += " "+dkey+"\t"+QString::number(info.number)+"\n";
				}
			}
		}

		if (!children.isEmpty())
		{
			res += key+"\t"+QString::number(children.size())+"\n";
			res += QString(children.join(""));
		}
	}

	return res;
}

BeamInfo &ParticleProxy::GetBeamByKeys(const QString &trackKey, const QString &beamKey)
{
	return beamData[trackKey][beamKey];
}

void ParticleProxy::GetBeamByNumber(int number, BeamInfo &binfo)
{
	foreach (QString key, beamData.keys())
	{
		BeamData data = beamData.value(key);

		foreach (QString dkey, data.uniqueKeys())
		{
			auto infos = data.values(dkey);

			foreach (BeamInfo info, infos)
			{
				if (info.number == number)
				{
					binfo = info;
				}
			}
		}
	}
}

void ParticleProxy::GetVisibleFacets(QVector<NumberedFacet> &visfacets,
									 QVector<NumberedFacet> &invisFacets)
{
	Point3f incidentDir(0, 0, -1);
	Point3f polarizationBasis(0, 1, 0);

	Point3f point = incidentDir * particle->GetMainSize();
	incidentDir.d_param = DotProduct(point, incidentDir);

	SetTracing(incidentDir, 0, polarizationBasis);

	IntArray facetIDs;

//	for (int i = 0; i < particle->facetNum; ++i)
//	{
//		facetIDs.Add(i);
//	}

	tracing->SelectVisibleFacetsForWavefront(facetIDs);
//	tracing->SortFacets(incidentDir, facetIDs);
//	tracing->SortFacets_2(incidentDir, Location::Out, facetIDs);
//	tracing->SortFacets_3(incidentDir, Location::Out, facetIDs);

	for (int i = facetIDs.size-1; i >= 0; --i)
//	for (int i = 0; i < facetIDs.size; ++i)
	{
		int id = facetIDs.arr[i];
		Facet &facet = particle->facets[id];
		QPolygonF pol;

		for (int j = 0; j < facet.size; ++j)
		{
			Point3f &p = facet.arr[j];
			pol.append(QPointF(p.c_x, p.c_y));
		}

		TranslateCoordinates(pol);
		visfacets.append(NumberedFacet{id, pol});
	}


//	for (int i = 0; i < particle->facetNum; ++i)
//	{
//		if (!facetIDs.Consist(i))
//		{
//			Facet &facet = particle->facets[i];
//			QPolygonF pol;

//			for (int j = 0; j < facet.size; ++j)
//			{
//				Point3f &p = facet.arr[j];
//				pol.append(QPointF(p.c_x, p.c_y));
//			}

//			TranslateCoordinates(pol);
//			invisFacets.append(NumberedFacet{i, pol});
//		}
//	}
}

// BUG: не всегда правильно объединяет, пофиксить
QPolygonF ParticleProxy::Union(QVector<QPolygonF> polygons, double epsilon)
{
	QVector<QPair<int, int>> connectPoints;

	QPolygonF resPol = polygons[0];

	for (int i = 1; i < polygons.size(); ++i)
	{
		QPolygonF &pol = polygons[i];
		bool isFound = false;

		for (int j = 0; j < resPol.size(); ++j)
		{
			for (int k = 0; (k < polygons[i].size()) && !isFound; ++k)
			{
				QPointF dif = resPol[j] - pol[k];
				double length = sqrt(QPointF::dotProduct(dif, dif));

				if (length < epsilon)
				{
					connectPoints.append(QPair<int, int>(j, k));

					if (connectPoints.size() == 2)
					{
						isFound = true;
					}
				}
			}

			if (isFound) // union
			{
				bool isNear = (connectPoints[1].first - connectPoints[0].first == 1);
				int first = isNear ? 0 : 1;
				int last = isNear ? 1 : 0;

				auto pFirst = connectPoints[first];
				auto pLast = connectPoints[last];

				int resFirst = connectPoints[first].first;
				resPol.remove(resFirst);

				// determine order
				int dir = (pLast.second - pFirst.second == 1) ? -1 : 1;

				for (int l = pFirst.second; l != pLast.second; l += 1*dir)
				{
					if (l == pol.size())
					{
						l = -1;
						continue;
					}
					else if (l == -1)
					{
						l = pol.size();
						continue;
					}

					resPol.insert(resFirst++, pol[l]);
				}

				polygons.remove(i); // current polygon is already united with result polygon
				i = 0;
				connectPoints.clear();
				break;
			}
		}
	}

	// remove duplicates
	std::vector<int> remList;

	int i = 1;
	int j = resPol.size()-1;

	while (j != -1)
	{
		if (i == resPol.size())
		{
			i = 0;
			j = resPol.size()-2;
		}

		QPointF dif = resPol[i] - resPol[j];
		double length = sqrt(QPointF::dotProduct(dif, dif));

		if (length < epsilon)
		{
			remList.push_back(i);
		}

		j = i-1;
		++i;
	}

	std::sort(remList.begin(), remList.end());

	for (int k = remList.size()-1; k >= 0; --k)
	{
		resPol.remove(k);
	}

	return resPol;
}

const TrackMap &ParticleProxy::GetTrackMap() const
{
	return beamData;
}

Particle *ParticleProxy::GetParticle() const
{
	return particle;
}
