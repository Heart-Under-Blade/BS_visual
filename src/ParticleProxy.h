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
};

class ParticleProxy
{
public:
	ParticleProxy();
	~ParticleProxy();

	void SetParticle(const QString &type, double refrIndex, double height,
					 double diameter, double additional = 1.0);
	void Trace(double beta, double gamma, double alpha, int reflNum);
	void Clear();

	QStringList GetParticleTypes() const;
	QString GetAdditionalParticleParam(const QString &type) const;

	QString GetTracks();
	BeamInfo &GetBeamInfo(const QString &trackKey, const QString &beamKey);

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

	typedef QMap<QString, BeamInfo> BeamData;
	QMap<QString, BeamData> beamData;

	Particle *particle;
	Tracing *tracing;
	PType ptype;
};
