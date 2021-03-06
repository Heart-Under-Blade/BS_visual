#include "TracingConvex.h"

TracingConvex::TracingConvex(Particle *particle, const Point3f &incidentBeamDir, bool isOpticalPath,
							 const Point3f &polarizationBasis, int interReflectionNumber)
	: Tracing(particle, incidentBeamDir, isOpticalPath, polarizationBasis, interReflectionNumber)
{
}

void TracingConvex::SplitBeamByParticle(double beta, double gamma, std::vector<Beam> &outBeams,
										double alpha)
{
	m_particle->Rotate(beta, gamma, alpha);

	m_incommingEnergy = 0;
	m_treeSize = 0;

	/// first extermal beam
	for (int facetID = 0; facetID < m_particle->facetNum; ++facetID)
	{
		const Point3f &extNormal = m_facets[facetID].ex_normal;
		double cosIN = DotProduct(m_incidentDir, extNormal);

		if (cosIN >= EPS_M_COS_90) /// beam is not incident to this facet
		{
			continue;
		}

		Beam inBeam, outBeam;
		TraceFirstBeam(facetID, inBeam, outBeam);

		outBeam.lastFacetID = facetID;
		outBeam.level = 0;
		SetBeamID(outBeam);
		outBeams.push_back(outBeam);
		PushBeamToTree(inBeam, facetID, 0);

#ifdef _CHECK_ENERGY_BALANCE
		CalcFacetEnergy(facetID, outBeam);
#endif
	}

	TraceInternalBeams(outBeams);
}

void TracingConvex::SplitBeamByParticle(double, double, const std::vector<std::vector<int>> &/*tracks*/, std::vector<Beam> &)
{
}

void TracingConvex::TraceInternalBeams(std::vector<Beam> &outBeams)
{
	while (m_treeSize != 0)
	{
		Beam beam = m_beamTree[--m_treeSize];

		if (IsTerminalBeam(beam))
		{
			continue;
		}

		for (int facetID = 0; facetID < m_particle->facetNum; ++facetID)
		{
			if (facetID == beam.lastFacetID)
			{
				continue;
			}

			Beam inBeam;

			try
			{
				TraceSecondaryBeams(beam, facetID, inBeam, outBeams);
			}
			catch (const std::exception &)
			{
				continue;
			}

			inBeam.id = beam.id;
			PushBeamToTree(inBeam, facetID, beam.level+1);
		}
	}
}
