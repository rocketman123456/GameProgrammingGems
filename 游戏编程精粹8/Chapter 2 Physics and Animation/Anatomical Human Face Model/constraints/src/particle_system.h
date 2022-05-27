// Anatomical Human Face Model - Game Programming Gems 8
// by Marco Fratarcangeli - 2010
//   marco@fratarcangeli.net
//   http://www.fratarcangeli.net
//
// This software is provided 'as-is', without any express or
// implied warranty. In no event will the author be held
// liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute
// it freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but
//    is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any
//    source distribution.


#ifndef __PARTICLE_SYSTEM__
#define __PARTICLE_SYSTEM__

#include "particle.h"
#include "constraint.h"

#include <vector>
using namespace std;

#define NUM_ITERATIONS 4	// initial number of iterations in the Gauss-Siedel solver
#define TIME_STEP 0.01f	



class ParticleSystem
{
	
	double      m_fTimeStep;
	int			m_iNbIterations;

public:
	
	vector<Particle>   _particles;		// set of particles stored in the particle system
	vector<Constraint> _constraints;	// set of constraints influencing the particles

	Constraint * volume_constraint;

	// these three parameters are the stiffness of the various contraint types 
	// involved in the simulation
	// by changing their value (between 0 and 1), the dynamic behavior of the deformable body changes
	double stretch_stiffness;
	double bending_stiffness;
	double volume_stiffness;


	ParticleSystem()
	{
		_particles.reserve(100);
		_constraints.reserve(1000);

		_particles.clear();
		_constraints.clear();
		
		m_fTimeStep = TIME_STEP;
		m_iNbIterations = NUM_ITERATIONS;

		stretch_stiffness = 1.f;
		bending_stiffness = 1.f;
		volume_stiffness = 1.f;
	}
	

	// reset the state of the particel system
	void Reset()
	{
		_particles.clear();
		_constraints.clear();

		volume_constraint = NULL;
	}
	
	void SetTimeStep(double ts)		
	{
		if (ts > 0)
			m_fTimeStep = ts;
	}
	double GetTimeStep() {return m_fTimeStep;}
	
	void SetNbIterations(int ni)	
	{
		if (m_iNbIterations > 0)
			m_iNbIterations = ni;
	}
	int GetNbIterations() {return m_iNbIterations;}

	Particle* GetParticleAt(int i)
	{
		assert(i < (int)_particles.size());
		return &_particles[i];
	}
	
	Particle* AddParticle(Particle _p)
	{
		_particles.push_back(_p);
		return &(_particles.back());
	}
	
	Constraint * AddConstraint(Constraint _c)
	{
		_constraints.push_back(_c);
		return &(_constraints.back());
	}
	

	// advance the simulation of one time step
	void TimeStep()
	{
		Verlet();					// numerical integration with Verlet (the paper does it better)
		SatisfyConstraints();		// solve the constraints defined on this particle system
	}	

	// define a set of default constraint over the shape of the particle system
	// if the mesh is closed a volume constraint is set as well
	void DefineSystem(	int nb_vertices, 
						int nb_faces, 
						int nb_edges, 
						double vertex_positions[][3],
						short face_indices[][3],
						short edge_indices[][4])
	{
		Reset();

		for (int i = 0; i < nb_vertices; i++)
		{
			Particle p(	vertex_positions[i][0],	vertex_positions[i][1],	vertex_positions[i][2]);
			p.index = i;
			AddParticle(p);
		}

		for (int i = 0; i < nb_faces; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				Particle & p = *(GetParticleAt(face_indices[i][j]));
				p.faces.insert(i);
			}
		}

		for (int i = 0; i < nb_edges; i++)
		{
			Particle & p0 = *(GetParticleAt(edge_indices[i][0]));
			Particle & p1 = *(GetParticleAt(edge_indices[i][1]));

			// defien stretch constraints
			{
				Constraint c(STRETCH, (p0.pos - p1.pos).Length(), stretch_stiffness);
				c.ref_parts.push_back(&p0);
				c.ref_parts.push_back(&p1);
				AddConstraint(c);
			}

			if (edge_indices[i][3] == -1) // this is a border edge, so skip it
				continue;

			Particle & p2 = *(GetParticleAt(edge_indices[i][2]));
			Particle & p3 = *(GetParticleAt(edge_indices[i][3]));

			// translate everything in _p1
			Vector3 _pos0 = p0.pos;
			Vector3 _pos1 = p1.pos - _pos0;
			Vector3 _pos2 = p2.pos - _pos0;
			Vector3 _pos3 = p3.pos - _pos0;

			// oriented normals
			Vector3  & n0 = (_pos1 ^ _pos2).Normalize();
			Vector3  & n1 = (_pos1 ^ _pos3).Normalize();;

			// rest angle
			double d = n0 * n1;
			double rest_angle = acosf((double)d);

			// define the bending constraint
			{
				Constraint c(BENDING, rest_angle, bending_stiffness);
				c.ref_parts.push_back(&p0);
				c.ref_parts.push_back(&p1);
				c.ref_parts.push_back(&p2);
				c.ref_parts.push_back(&p3);
				AddConstraint(c);
			}
		}

		// add the volume constraint
		{
			Constraint c(VOLUME, -1, volume_stiffness);
			for (int i = 0; i < nb_vertices; i++)
				c.ref_parts.push_back(GetParticleAt(i));

			for (int i = 0; i < nb_faces; i++)
				c.ref_faces.push_back( &(face_indices[i][0]) );

			c.SetRestVolume();
			volume_constraint = AddConstraint(c);
		}

	}
	
private:
		
	// solve according to verlet integration
	void Verlet()
	{
		vector<Particle>::iterator pIt;
		for(pIt = _particles.begin(); pIt != _particles.end(); pIt++) 
			if(!pIt->isFixed)
			{
				Vector3& x = pIt->pos;
				Vector3  temp = x;
				Vector3& oldx = pIt->restPos;
				Vector3& a = pIt->acc;
				
				x += x - oldx + a * m_fTimeStep * m_fTimeStep ;
				
				oldx = temp;
			}
	}		
	

	// Gauss-Seidel solver
	// solve the constraint in iterative way
	void SatisfyConstraints() 
	{
		for(int j = 0; j < m_iNbIterations; j++) 
		{	
			vector<Constraint>::iterator cIt;
			for(cIt = _constraints.begin(); cIt != _constraints.end(); cIt++) 
			{
				cIt->Solve();
			}
		}
	}
};

#endif // __PARTICLE_SYSTEM__