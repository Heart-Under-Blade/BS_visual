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
	Angle() {alpha = beta = gamma = 0;}
	double alpha;
	double beta;
	double gamma;
};

struct NumberedFacet
{
	QPolygonF pol;
	int num;
};

typedef QMap<QString, BeamInfo> BeamData;
typedef QMap<QString, BeamData> TrackMap;

class ParticleProxy
{
public:
	ParticleProxy();
	~ParticleProxy();

	void SetParticle(const QString &type, double refrIndex, double height,
					 double diameter, double additional = 1.0);
	void Trace(const Angle &angle, int reflNum);
	void Clear();

	QStringList GetParticleTypes() const;
	QString GetAdditionalParticleParam(const QString &type) const;

	QString GetBeamDataString();
	QString GetBeamDataString(const QString &searchLine);
	BeamInfo &GetBeamByKeys(const QString &trackKey, const QString &beamKey);
	void GetBeamByNumber(int number, BeamInfo &binfo);

	const TrackMap &GetTrackMap() const;

	Particle *GetParticle() const;
	void GetFacets(QVector<NumberedFacet> &facets);

	void SetTracing(const Point3f &incidentDir, int reflNum,
					const Point3f &polarizationBasis);

	QString RecoverTrack(long long id, int level);

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

	TrackMap beamData;

	Particle *particle;
	Tracing *tracing;
	PType ptype;

private:
	QPolygonF Union(QVector<QPolygonF> polygons, double epsilon);
};
