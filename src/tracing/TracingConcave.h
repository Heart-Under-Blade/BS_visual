#pragma once

#include "Tracing.h"

/** NOTE: пучки выходят со случайно ориентированным порядком вершин */
class TracingConcave : public Tracing
{
public:
	TracingConcave(Particle *particle, const Point3f &startBeamDir,
				   bool isOpticalPath, const Point3f &polarizationBasis,
				   int interReflectionNumber);

	void SplitBeamByParticle(double beta, double gamma, std::vector<Beam> &scaterredBeams,
							 double alpha = 0.0) override;
	void SplitBeamByParticle(double beta, double gamma, const std::vector<std::vector<int>> &tracks,
							 std::vector<Beam> &scaterredBeams) override;

private:
	void CutBeamByFacet(int facetID, Beam &beam, bool &isDivided,
						Polygon *resultBeams, int &resultSize);

	void CutFacetByShadows(int facetID, const IntArray &shadowFacetIDs, int prevFacetNum,
						   PolygonArray &resFacets);

	void CatchExternalBeam(const Beam &beam, std::vector<Beam> &scatteredBeams);

	void FindVisibleFacets(const Beam &beam, IntArray &facetIds);

	void SelectVisibleFacets(const Beam &beam, IntArray &facetIDs);

	void SetOpticalBeamParams(int facetID, const Beam &incidentBeam,
							  Beam &inBeam, Beam &outBeam, bool &hasOutBeam);

	void IntersectWithFacet(const IntArray &facetIDs, int prevFacetNum,
							PolygonArray &resFacets);

	void TraceFirstBeam();

	bool isExternalNonEmptyBeam(Beam &incidentBeam);

	int FindFacetID(int facetID, const IntArray &arr);

	void TraceFirstBeamFixedFacet(int facetID, bool &isIncident);

#ifdef _TRACK_ALLOW
//	void AddToTrack(Beam &beam, int facetId);
#endif

	void PushBeamsToTree(const Beam &beam, int facetID, bool hasOutBeam,
						 Beam &inBeam, Beam &outBeam);
	void PushBeamsToTree(int facetID, const PolygonArray &polygons,
						 Beam &inBeam, Beam &outBeam);

	bool IsVisibleFacet(int facetID, const Beam &beam);

	void TraceByFacet(const IntArray &facetIDs, int facetIndex);

	void TraceSecondaryBeamByFacet(Beam &beam, int facetID, bool &isDivided);

	void PushBeamsToBuffer(int facetID, const Beam &beam, bool hasOutBeam, Beam &inBeam, Beam &outBeam, std::vector<Beam> &passed);

protected:
	void TraceSecondaryBeams(std::vector<Beam> &scaterredBeams);

	// Tracing interface
public:
	void GetVisiblePart(double b, double g, double a,
						std::vector<Beam> &beams) override;
};

