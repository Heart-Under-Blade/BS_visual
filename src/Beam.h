#pragma once

#include "Particle.h"
#include "global.h"
#include "math/compl.hpp"
#include "JonesMatrix.h"
#include "float.h"

#ifdef _TRACK_ALLOW
//#include <vector>
#endif

class Beam : public Polygon
{
public:
	Beam();
	Beam(const Beam &other);
	Beam(const Polygon &other);
	Beam(Beam &&other);

	void RotateSpherical(const Point3f &dir, const Point3f &polarBasis);
	void RotatePlane(const Point3f& newBasis); ///< rotate Jones matrix in case of beam splitting

	void AddVertex(const Point3f &vertex);
	void SetPolygon(const Polygon &other);

	Beam & operator = (const Beam &other);
	Beam & operator =(const Polygon &other);
	Beam & operator = (Beam &&other);

	void SetTracingParams(int facetID, int level, Location location);

	void SetJonesMatrix(const Beam &other, const complex &coef1, const complex &coef2);

	// REF: перенести в PhisBeam
	complex DiffractionIncline(const Point3d& pt, double wavelength) const; ///< calculate diffraction at the point /b pt
	//--------------------------

	friend std::ostream & operator << (std::ostream &os, const Beam &beam);

	void GetSpherical(double &fi, double &teta) const;
	// REF: рассмотреть схему, где у пучка будет много полигонов

public:
	Point3f direction;				///< direction of beam
	JonesMatrix J;					///< Jones matrix of beam
	Point3f e;						///< basis of polarization plane

	int lastFacetID;				///< last reflected facet
	int level;						///< number of preview reflections
	Location location;				///< beam state towards the particle (inside or outside)

	// REF: перенести в PhisBeam
	double opticalPath;				///< optical path of beam
	double D;						///< current position of phase front from Ax+By+Cz+D=0

	long long int id = 0;
#ifdef _TRACK_ALLOW
//	std::vector<int> track;
#endif

private:
	void RotateJMatrix(const Point3f &newBasis);
	void Copy(const Beam &other);
};
