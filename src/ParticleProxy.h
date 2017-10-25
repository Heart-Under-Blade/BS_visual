#pragma once

#include <QMap>
#include <QVector>
#include <QPolygonF>
#include <QString>
#include <QStringList>
#include "Beam.h"

class Particle;
class Tracing;

struct BeamInfo
{
	BeamInfo() {}

	Beam beam;
	double thetaDeg;
	double phiDeg;
	double area;
	QString track;
	QVector<double> M;
	int number;
};

struct Angle
{
	Angle() : alpha(0.f), beta(0.f), gamma(0.f) {}

	Angle(double a, double b, double g)
		: alpha(a), beta(b), gamma(g) {}

	double alpha;
	double beta;
	double gamma;
};

struct GlobalAngle
{
	GlobalAngle() : phi(0.f), theta(0.f), psy(0.f) {}

	GlobalAngle(double a, double b, double g)
		: phi(a), theta(b), psy(g) {}

	double phi;
	double theta;
	double psy;
};

struct NumberedFacet
{
	int num;
	QPolygonF pol;
};

struct Axes
{
	Axes()
	{
		x = Point3f(1, 0, 0);
		y = Point3f(0, 1, 0);
		z = Point3f(0, 0, 1);
	}

	Axes(const Point3f &px, const Point3f &py, const Point3f &pz)
	{
		x = px;
		y = py;
		z = pz;
	}

	Point3f x;
	Point3f y;
	Point3f z;
};

typedef QMap<QString, BeamInfo> BeamData;
typedef QMap<QString, BeamData> BeamMap;

class ParticleProxy
{
public:
	ParticleProxy();
	~ParticleProxy();

	void SetParticle(const QString &type, double refrIndex, double height,
					 double diameter, double additional = 1.0);
	void Trace(const Angle &angle, const Angle &viewAngle, int reflNum);
	void Clear();

	void RotateParticle(const Angle &rotAngle, const Angle &viewAngle);
	QVector<QPointF> RotateLocalAxes(const Angle &angle, const Angle &viewAngle);
	QVector<QPointF> RotateGlobalAxes(const Angle &viewAngle);

	QStringList GetParticleTypes() const;
	QString GetAdditionalParticleParam(const QString &type) const;

	void GetTrack(int beamNumber, const Angle &viewAngle,
				  QVector<NumberedFacet> &track);
	QString GetBeamDataString();
	QString GetBeamDataString(const QString &searchLine);
	BeamInfo &GetBeamByKeys(const QString &trackKey, const QString &beamKey);
	void GetBeamByNumber(int number, BeamInfo &binfo);

	const BeamMap &GetBeamMap() const;

	Particle *GetParticle() const;
	void GetVisibleFacets(QVector<NumberedFacet> &visfacets, QVector<NumberedFacet> &invisFacets);

	void SetTracing(const Point3f &incidentDir, int reflNum,
					const Point3f &polarizationBasis);

	QString RecoverTrack(long long id, int level);
	void TranslateCoordinates(QPolygonF &pol);

private:

	enum class PType : int
	{
		Column,
		HollowColumn,
		Bullet,
		BulletRosette,
		Aggregate
	};

	QMap<PType, QString> particleTypes;

	BeamMap beamData;

	Particle *particle;
	Tracing *tracing;
	PType ptype;
	Axes localAxes;
	Axes globalAxes;

private:
	QPolygonF Union(QVector<QPolygonF> polygons, double epsilon);
	void RotateStates(const Angle &angle, Beam &beam);
	void SetBeamInfo(int beamNumber, Beam &beam, BeamInfo &info);
};
