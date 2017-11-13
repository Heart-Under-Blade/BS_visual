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
	TraceFirstBeam(outBeams);
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
			inBeam.states = beam.states;

			BeamState state;
			state.fromPolygon(inBeam);
			state.loc = Location::Out;
			state.facetID = facetID;
			inBeam.states.push_back(state);

			PushBeamToTree(inBeam, facetID, beam.level+1);
		}
	}
}


void TracingConvex::GetVisiblePart(double b, double g, double a,
								   std::vector<Beam> &beams)
{
	m_particle->Rotate(b, g, a);
	std::vector<Beam> outBeams;
	TraceFirstBeam(outBeams);

	for (int i = 0; i < m_treeSize; ++i)
	{
		beams.push_back(outBeams[i]);
	}
}
