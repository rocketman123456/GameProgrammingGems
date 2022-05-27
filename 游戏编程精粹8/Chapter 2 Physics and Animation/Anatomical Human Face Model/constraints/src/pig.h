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


#ifndef __pig__
#define __pig__

const int nb_pig_vertices = 3522;	// number of vertices
const int nb_pig_faces = 7040;		// number of triangular faces
const int nb_pig_edges = 10560;		// number of edges

// triangular face connectivity; 
// pig_face_indices[i][j]: vertex j belongs to the face i
extern short	pig_face_indices[7040][3];

// position of each vertex 
extern double	pig_vertex_positions[3522][3];

// given a vertex, which vertices are connected to it?
// pig_vertex_vertex_indices[i][j]: vertex j is directly connected to vertex i
extern short	pig_vertex_vertex_indices[3522][22];

// which are the vertices connceted by an edge?

/*							   * pig_edge_indices[i][2]		*/
/*							  / \							*/
/*							 /   \							*/
/*	 pig_edge_indices[i][0] *-----* pig_edge_indices[i][1]	*/
/*							 \   /							*/
/*							  \ /							*/
/*							   * pig_edge_indices[i][3]		*/
extern short	pig_edge_indices[10560][4];

#endif
