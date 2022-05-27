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


#ifndef _PARTICLE__
#define _PARTICLE__

#include <set>

#include "vector3.h"

using namespace std;

// a particle with all the data useful during the physical computation
// by default, all the particles have unitary mass
class Particle
{
public:

	Vector3 pos;		// current position
	Vector3 restPos;	// initial position
	Vector3 oldPos;		// position at the previous time step

	Vector3 acc;		// accelleration; mass = 1 so acc == applied force 

	int index;			// index of the particle in the parentparticle system
	bool isFixed;		// if true, the particle can not move

	Vector3 grad;		// this member is useful in the volume constraint computation

	set<int> neighs;	// set of vertices which are directly connected to this particle
	set<int> faces;		// set of faces to whcih this particle belongs

	Vector3 normal;		// normal of the vertex, used only during rendering

	Particle()
	{
		Init();
	}
	
	Particle(double x, double y, double z)
	{
		oldPos = restPos = pos = Vector3(x, y, z);
		Init();
	}

	Particle(Vector3 _p)
	{
		oldPos = restPos = pos = _p;
		Init();
	}

	// clear state
	void Init()
	{
		neighs.clear();
		faces.clear();
		acc = Vector3(0, 0, 0);
		grad = Vector3(0, 0, 0);
		normal = Vector3(0, 0, 0);
		isFixed = false;
	}
};

# endif // _PARTICLE__