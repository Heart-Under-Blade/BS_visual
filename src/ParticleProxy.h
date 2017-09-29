#pragma once

#include <QMap>
#include <QVector>
#include <QString>
#include <QStringList>
#include "Beam.h"

class Particle;
class Tracing;

struct BeamInfo
{
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
	double alpha;
	double beta;
	double gamma;
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
	BeamInfo &GetBeamByNumber(int number);

	const TrackMap &GetTrackMap() const;

private:

	enum class PType : int
	{
		Column,
		HollowColumn,
		Bullet,
		BulletRosette,
		Aggregate
	};

private:
	QMap<PType, QString> particleTypes;

	TrackMap beamData;

	Particle *particle;
	Tracing *tracing;
	PType ptype;
};
