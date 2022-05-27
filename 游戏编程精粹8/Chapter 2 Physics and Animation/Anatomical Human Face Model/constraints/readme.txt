Anatomical Human Face Model - Game Programming Gems 8
by Marco Fratarcangeli - 2010
  marco@fratarcangeli.net
  www.fratarcangeli.net


The purpose of the demo is to provide a learning implementation of some  constraints (stretch, bending, area and volume), and show how they work. There are several pre-built deformable bodies (a pig, a cube, a dodecahedron, a piece of cloth and a thin shell), and you can switch among them by pressing the numerical keys from 1 to 5. 

You can pick a vertex of the mesh and pull by right clicking on it and drag.
To comfortably pick a vertex, you may press the "space" key and freeze the simulation, pick the vertex, and then press space again to continue.
Left click rotates the scene and central click allows to zoom.

You can run the executable in the bin directory or build the demo from the source.
The source code is cross platform and relies only on the GLUT library. I didn't have the chance to test it on *nix but it should be fine because it is standard c++ and STL.

In the main directory, you will find a Microsoft Visual Studio 2008 solution file named phys_sim.sln. If you don't want to use Visual Studio, it is possible to import it in other IDEs like code::blocks or use it in other building systems like CMake.


----------------------------------------------------------------------------
In the following, there is a high level overview of the code structure of the demo and its classes.



vector3.h
implementation of a 3D vector based on double type, including standard operations like cross and dot product.


particle.h
class representing a particle. In this demo each particle corresponds to a mesh vertex. The class includes members like position, velocity, accelleration and everything else needed during the physical simulation.


constraint.h
class representing a spatial constraint. There are four constraint types implemented:
.stretch: keeps two particles at a given distance;
.bending: keeps two triangular faces sharing an edge at a given angle;
.area: conserves the area of a triangular face;
.volume: conserves the volume of a closed mesh.


particle_system.h
class which represent a deformable body. It stores particles and define constraints among them. It also advance the physical simulation and update the position of the particles.


phys_sim.cpp
application file. Using OpenGL and GLUT, it handles the window, the user input and the rendering of the particle system.


pig.h, cube.h, dodecahedron.h, grid.h
for each corresponding object, these files hold the definition of static arrays storing the initial position of the particles, the face connectivity and other topological information, like "given a vertex, which vertices are connected to it?" and so on. More in the source comments.
If you want to query such information for your own deformable objects, see open source libraries for mesh processing, like OpenMesh ( http://www.openmesh.org/ ) or VCGLib ( http://vcg.sourceforge.net/ ).







