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


#ifndef __CONSTRAINT__
#define __CONSTRAINT__

#include <vector>
#include <assert.h>
#include "particle.h"

using namespace std;


enum ConstraintType { STRETCH, BENDING, AREA, VOLUME, CONSTR_NONE };

// represents a spatial constraint
// see "Muller, Heidelberger, Hennix, Ratcliff - Position Based Dynamics"
// available for free here http://www.matthiasmueller.info/publications/publications.htm
class Constraint
{
public:

	ConstraintType type;
	vector<Particle *> ref_parts;	// reference to the particles affected by this constraint
	vector<short*> ref_faces;		// reference to the faces affected by this constraint
	
	double stiffness;				// stiffness value (see paper), 0 <= stiffness <= 1
	double rest_value;				// value which satisfies the constraint
	
	Constraint()
	{
		ref_parts.clear();
		type = CONSTR_NONE;
		rest_value = 0.f;
		stiffness = 1.f;
	}
	
	Constraint(ConstraintType _type, double _rest_value, double _stiffness = 1.f)
	{
		rest_value	= _rest_value;
		stiffness	= _stiffness;
		type		= _type;
	}

	void SetRestVolume(double _volume = -1)
	{
		if (type != VOLUME)
		{
			printf("Constraint::SetRestVolume: ehy, I am not a volume constraint!\n");
			return;
		}

		if (_volume < 0)
			rest_value = ComputeVolume();
		else
			rest_value = _volume;
	}

	
	void Solve()
	{
		switch (type)
		{
		case STRETCH:
			SolveStretch();
			break;
		case BENDING:
			SolveBending();
			break;
		case AREA:
			SolveTriArea();
			break;
		case VOLUME:
			SolveVolume();
			break;
		}
	}

	// distance between p0 and p1 must be rest_value
	void SolveStretch()
	{
		assert(ref_parts.size() == (size_t)2);

		Particle & p0 = *ref_parts[0];
		Particle & p1 = *ref_parts[1];
		
		Vector3 delta = p1.pos - p0.pos;
		double deltaLength = delta.Length();
		
		double diff = (deltaLength - rest_value) / deltaLength;
		
		//error = fabs(diff);
		
		if (!p0.isFixed)
			p0.pos += delta * 0.5 * diff * stiffness;
		if (!p1.isFixed)
			p1.pos -= delta * 0.5 * diff * stiffness;
	}

	// angle between two triangular faces (p0,p1,p2) and (p0,p3,p1) must be rest_value
	void SolveBending()
	{
		assert(ref_parts.size() == (size_t)4);

		Particle & p0 = *ref_parts[0];   /*        * p2		*/
		Particle & p1 = *ref_parts[1];	 /*       / \		*/
		Particle & p2 = *ref_parts[2];	 /*      /   \		*/
		Particle & p3 = *ref_parts[3];	 /*	 p0 *-----* p1	*/
										 /*      \   /		*/
										 /*       \ /		*/
										 /*        * p3		*/

		// translate everything in p0
		Vector3 _p0 = p0.pos;
		Vector3 _p1 = p1.pos - _p0;
		Vector3 _p2 = p2.pos - _p0;
		Vector3 _p3 = p3.pos - _p0;

		Vector3 n0 = (_p1 ^ _p2).Normalize();
		Vector3 n1 = (_p1 ^ _p3).Normalize();;

		double d = n0 * n1;

		Vector3 q[4];
		q[3] = ((_p1 ^ n0) + (n1 ^ _p1) * d) / (_p1 ^ _p3).Length();
		q[2] = ((_p1 ^ n1) + (n0 ^ _p1) * d) / (_p1 ^ _p2).Length();
		q[1] = ((((_p2 ^ n1) + (n0 ^ _p2) * d) / (_p1 ^ _p2).Length()) +
				(((_p3 ^ n0) + (n1 ^ _p3) * d) / (_p1 ^ _p3).Length())) * -1;
		q[0] = (q[1] + q[2] + q[3]) * -1;

		double sum_q = 0;
		for (int i = 0; i < 4; i++)
			sum_q += q[i].SquaredLength();

		if (sum_q < 0.00001) return;		// avoid overflow and division by zero

		// so far, all the masses are considerer like 1
		double s = -stiffness * (sqrt(1 - d * d) * (acosf(d) - rest_value)) / sum_q;

		for (int i = 0; i < 4; i++)
			if (!ref_parts[i]->isFixed)
				ref_parts[i]->pos += q[i] * s;
	}

	// area of the triangular face (p0,p1,p2) must be rest_value
	void SolveTriArea()
	{
		assert(ref_parts.size() == (size_t)3);

		Particle * p[3];

		p[0] = ref_parts[0];
		p[1] = ref_parts[1];
		p[2] = ref_parts[2];

		Vector3 grad[3];

		for (int i = 0; i < 3; i++)
		{
			Vector3 e2 = (p[(i + 1) % 3]->pos - p[i]->pos);
			Vector3 e1 = (p[(i + 2) % 3]->pos - p[i]->pos);
			Vector3 e0 = (p[(i + 2) % 3]->pos - p[(i + 1) % 3]->pos);

			Vector3 n = e1 ^ e2;
			Vector3 h = e0 ^ n;

			grad[i] = h.Normalize() * e0.Length() * .5;
		}

		double area = ((p[1]->pos - p[0]->pos) ^ (p[2]->pos - p[0]->pos)).Length() / 2.0;

		double c = area - rest_value;

		double s = c / (grad[0] * grad[0] + grad[1] * grad[1] + grad[2] * grad[2]);

		for (int i = 0; i < 3; i++)
			if (!p[i]->isFixed)
				p[i]->pos += grad[i] * -s; 
	}

	// it constraints the particles to move in order to fit the desired volume rest_value
	void SolveVolume()
	{
		double C = rest_value - ComputeVolume();

		double sum_squared_grad_p = 0;	// denominator of the scaling factor
		double s = 0;										// scaling factor

		// for each particle, compute the corresponding gradient
		vector<Particle *>::iterator pit = ref_parts.begin();
		for (; pit != ref_parts.end(); pit++)
		{
			Particle & pa = **pit;
			pa.grad = Vector3(0, 0, 0);

			set<int>::iterator fit = pa.faces.begin();
			for (; fit != pa.faces.end(); fit++)
			{
				int j = 0;
				for (; j < 3; j++)
				{
					int iPart = (ref_faces[*fit])[j];
					if (iPart == pa.index)
						break;
				}

				assert(j != 3);
				Vector3 vb =  ref_parts[(ref_faces[*fit])[(j + 1) % 3]]->pos;
				Vector3 vc =  ref_parts[(ref_faces[*fit])[(j + 2) % 3]]->pos;

				pa.grad += vb ^ vc;
			}
		}

		for (pit = ref_parts.begin(); pit != ref_parts.end(); pit++)
			sum_squared_grad_p += (**pit).grad.SquaredLength();

		if (sum_squared_grad_p > 0)
			s = C / sum_squared_grad_p;
		else
			s = 0;

		// move the particles in order to fill the target volume
		for (pit = ref_parts.begin(); pit != ref_parts.end(); pit++)
		{
			Particle & p = **pit;
			if (!p.isFixed)
				p.pos += p.grad * s * stiffness;
		}

	}

	protected:

		// (quite) fast volume computation 
		// the volume is embedded by the triangulat faces stored in ref_faces
		double ComputeVolume()
		{
			double V = 0;

			vector<short*>::iterator it = ref_faces.begin();
			for(; it != ref_faces.end(); it++)
			{
				Vector3 v0 = ref_parts[(*it)[0]]->pos;
				Vector3 v1 = ref_parts[(*it)[1]]->pos;
				Vector3 v2 = ref_parts[(*it)[2]]->pos;

				V += (v0 ^ v1) * v2;
			}

			return V / 6.0;
		}
};

#endif // __CONSTRAINT__