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


#ifdef _MSC_VER
#pragma warning(disable:4305)
#pragma warning(disable:4244)
#define _CRT_SECURE_NO_WARNINGS
#endif


#include <iostream>
#include <algorithm>
#include <stdarg.h>
#include <gl/glut.h>
#include "timer.h"


#include "particle_system.h"
#include "grid.h"
#include "cube.h"
#include "dodecahedron.h"
#include "pig.h"


// used to define the current deformable body
enum ShapeType {CUBE, DODECAHEDRON, PIG, CLOTH, SHELL, NOSHAPE };
ShapeType shape = NOSHAPE;
double step_volume = 0.0;


// used to pick a particle and to drag it
Particle * PickPart(int x, int y,int w,int h, float & dist);
Particle * pickedPart = NULL;
float ds;	// distance of the picked primitive from the mouse cursor
bool pickOn = false, isDragging = false;
Vector3 mouse2D, mouse3D;

// size of the window
int window_size_x = 0, window_size_y = 0;

// used for the trackball implementation
const double m_ROTSCALE = 90.0;
const double m_ZOOMSCALE = 0.008;
float fit_factor = 1.f;
Vector3 trackBallMapping(int x, int y);    // Utility routine to convert mouse locations to a virtual hemisphere
Vector3 lastPoint;                           // Keep track of the last mouse location
enum MovementType { ROTATE, ZOOM, NONE };  // Keep track of the current mode of interaction (which mouse button)
MovementType Movement;                     //    Left-mouse => ROTATE, Right-mouse => ZOOM

static GLdouble objectXform[4][4] = {
	{1.0, 0.0, 0.0, 0.0},
	{0.0, 1.0, 0.0, 0.0},
	{0.0, 0.0, 1.0, 0.0},
	{0.0, 0.0, 0.0, 1.0}
};


GLint FPS = 60;		

ParticleSystem ps;	// this is the deformable body
bool bFreeze = false; // if true, freese the simulation

// declaration of functions to build the deformable body
void BuildCloth();
void BuildShell();
void BuildCube();
void BuildDodecahedron();
void BuildPig();

// implementation of printf with GLUT
void glPrint(float* c, float x, float y, float z, const char *fmt, ...);


// handles key down event
void keyboardDown(unsigned char key, int x, int y) 
{
	switch(key) 
	{
	case '1':
		BuildCube();
		break;

	case '2':
		BuildDodecahedron();
		break;

	case '3':
		BuildPig();
		break;

	case '4':
		BuildCloth();
		break;

	case '5':
		BuildShell();
		break;

	case 32: // space
		bFreeze = !bFreeze;
		break;

	case 'Q':
	case 'q':
	case  27:   // ESC
		exit(0);
	}

	glutPostRedisplay();
}

// by pressing left and right cursors, the closed mesh inflates
void specialDown(int key, int x, int y)
{
	switch(key) 
	{
	case GLUT_KEY_LEFT:
		if (ps.volume_constraint != NULL)
			ps.volume_constraint->rest_value -= step_volume;
		break;

	case GLUT_KEY_RIGHT:
		if (ps.volume_constraint != NULL)
			ps.volume_constraint->rest_value += step_volume;
		break;
	}

	glutPostRedisplay();
}


void reshape(int width, int height) 
{
	window_size_x = width;
	window_size_y = height;

	// Determine the new aspect ratio
	GLdouble gldAspect = (GLdouble) width/ (GLdouble) height;

	// Reset the projection matrix with the new aspect ratio.
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(40.0, gldAspect, 0.01, 60.0);
	glTranslatef( 0.0, 0.0, -2.0 );

	// Set the viewport to take up the entire window.
	glViewport(0, 0, width, height);
}

// handles when a mouse button is pressed / released
void mouseClick(int button, int state, int x, int y) 
{
	mouse2D = Vector3(x, window_size_y - y, 0);

	if (state == GLUT_UP)
	{
		// stop dragging
		if (button == GLUT_RIGHT_BUTTON)
			isDragging = false;

		// Turn-off rotations and zoom.
		Movement = NONE;
		glutPostRedisplay();
		return;
	}

	switch (button)
	{
	case (GLUT_LEFT_BUTTON):

		// Turn on user interactive rotations.
		// As the user moves the mouse, the scene will rotate.
		Movement = ROTATE;

		isDragging = false;	// turn off picking anyway

		// Map the mouse position to a logical sphere location.
		// Keep it in the class variable lastPoint.
		lastPoint = trackBallMapping( x, y );

		// Make sure we are modifying the MODELVIEW matrix.
		glMatrixMode( GL_MODELVIEW );
		break;

	case (GLUT_MIDDLE_BUTTON):

		isDragging = false;

		// Turn on user interactive zooming.
		// As the user moves the mouse, the scene will zoom in or out
		//   depending on the x-direction of travel.
		Movement = ZOOM;

		// Set the last point, so future mouse movements can determine
		//   the distance moved.
		lastPoint.x = (double) x;
		lastPoint.y = (double) y;

		// Make sure we are modifying the PROJECTION matrix.
		glMatrixMode( GL_PROJECTION );

		break;

	case (GLUT_RIGHT_BUTTON):
		// enable picking of a particle
		Movement = NONE;
		pickOn = true;		
		break;
	}

	glutPostRedisplay();
}


// handle any necessary mouse movements through the trackball
void mouseMotion(int x, int y) 
{
	Vector3 direction;
	double pixel_diff;
	double rot_angle, zoom_factor;
	Vector3 curPoint;

	switch (Movement) 
	{
	case ROTATE :  // Left-mouse button is being held down
		{
			curPoint = trackBallMapping( x, y );  // Map the mouse position to a logical sphere location.
			direction = curPoint - lastPoint;
			double velocity = direction.Length();
			if( velocity > 0.0001 )
			{
				// Rotate about the axis that is perpendicular to the great circle connecting the mouse movements.
				Vector3 rotAxis;
				rotAxis = lastPoint ^ curPoint ;
				rot_angle = velocity * m_ROTSCALE;

				// We need to apply the rotation as the last transformation.
				//   1. Get the current matrix and save it.
				//   2. Set the matrix to the identity matrix (clear it).
				//   3. Apply the trackball rotation.
				//   4. Pre-multiply it by the saved matrix.
				glGetFloatv( GL_MODELVIEW_MATRIX, (GLfloat *) objectXform );
				glLoadIdentity();
				glRotatef( rot_angle, rotAxis.x, rotAxis.y, rotAxis.z );
				glMultMatrixf( (GLfloat *) objectXform );

				//  If we want to see it, we need to force the system to redraw the scene.
				glutPostRedisplay();
			}
			break;
		}
	case ZOOM :  // Right-mouse button is being held down
		//
		// Zoom into or away from the scene based upon how far the mouse moved in the x-direction.
		//   This implementation does this by scaling the eye-space.
		//   This should be the first operation performed by the GL_PROJECTION matrix.
		//   1. Calculate the signed distance
		//       a. movement to the left is negative (zoom out).
		//       b. movement to the right is positive (zoom in).
		//   2. Calculate a scale factor for the scene s = 1 + a*dx
		//   3. Call glScalef to have the scale be the first transformation.
		// 
		pixel_diff = y - lastPoint.y; 
		zoom_factor = 1.0 + pixel_diff * m_ZOOMSCALE;
		glScalef( zoom_factor, zoom_factor, zoom_factor );

		// Set the current point, so the lastPoint will be saved properly below.
		curPoint.x = (double) x;  curPoint.y = (double) y;  (double) curPoint.z = 0;

		//  If we want to see it, we need to force the system to redraw the scene.
		glutPostRedisplay();
		break;
	}

	// Save the location of the current point for the next movement. 
	lastPoint = curPoint;	// in spherical coordinates
	mouse2D = Vector3(x, window_size_y - y, 0);	// in window coordinates
}

// draw the coordinate axes
void DrawAxes(double length)
{
	glDisable(GL_LIGHTING);
	glPushMatrix();
	glScalef(length, length, length);

	glLineWidth(2.f);
	glBegin(GL_LINES);

	// x red
	glColor3f(1.f, 0.f, 0.f);
	glVertex3f(0.f, 0.f, 0.f);
	glVertex3f(1.f, 0.f, 0.f);

	// y green
	glColor3f(0.f, 1.f, 0.f);
	glVertex3f(0.f, 0.f, 0.f);
	glVertex3f(0.f, 1.f, 0.f);

	// z blue
	glColor3f(0.f, 0.f, 1.f);
	glVertex3f(0.f, 0.f, 0.f);
	glVertex3f(0.f, 0.f, 1.f);

	glEnd();
	glLineWidth(1.f);

	glPopMatrix();
}

// draw information on the screen
void DrawInfo()
{
	// -- start Ortographic Mode
	glMatrixMode(GL_PROJECTION);					//Select the projection matrix
	glPushMatrix();									//Store the projection matrix
	glLoadIdentity();								//Reset the projection matrix
	glOrtho(0, window_size_x, window_size_y, 0, -1, 1);		//Set up an ortho screen

	glMatrixMode(GL_MODELVIEW);						//Select the modelview matrix
	glPushMatrix();									//Store the projection matrix

	glLoadIdentity();								//Reset the projection matrix

	float c[3] = {0.1, 0.1, 0.1};
	float y = 25;

	// Display results

	static double start = GetRealTimeInMS();
	int now = GetRealTimeInMS();

	glPrint(c, 10, y, 0, "Select a shape by pressing a numerical key:"); y += 20;
	glPrint(c, 10, y, 0, "[ 1 ] Cube   [ 2 ] Dodecahedron   [ 3 ] Pig   [ 4 ] Cloth   [ 5 ] Shell"); y += 20;
	glPrint(c, 10, y, 0, "iterations: %d   dt: %.3f   fps: %.2f", ps.GetNbIterations(), ps.GetTimeStep(), 1000.0 / (now - start)); 
	start = now;

	y = 450;
	glPrint(c, 10, y, 0, "[ Left / Right ] : Deflate / Inflate"); y += 50; 
	glPrint(c, 10, y, 0, "[ Left click + drag ] : Rotate          [ Central click + drag ] : Zoom"); y += 25;
	glPrint(c, 10, y, 0, "[ Right click + drag ] : Grab and pull a particle"); y += 25;
	glPrint(c, 10, y, 0, "[ Space ] : Freeze / Continue the simulation "); y += 25;
	glPrint(c, 10, y, 0, "Coded by Marco Fratarcangeli for Game Programming Gems 8"); y += 25;

	glPopMatrix();									//Restore the old projection matrix
	glMatrixMode(GL_PROJECTION);					//Select the projection matrix
	glPopMatrix();									//Restore the old projection matrix
	glMatrixMode(GL_MODELVIEW);
	// -- end Ortographic mode
}

// draw the particle system
// note: I could put this inside ParticleSystem class,
// I preferred to put it here to make ParticleSystem independent from the render method
void DrawParticleSystem(double vertex_positions[][3], int nb_faces, short face_indices[][3])
{
	if (ps._particles.empty())
		return;

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// ------------  compute normals for correct lighting
	vector<Vector3> face_normals;
	// compute the normal for each face
	for (int i = 0; i < nb_faces; i++)
	{
		Particle & p0 = *(ps.GetParticleAt(face_indices[i][0]));
		Particle & p1 = *(ps.GetParticleAt(face_indices[i][1]));
		Particle & p2 = *(ps.GetParticleAt(face_indices[i][2]));

		Vector3 normal = (p1.pos - p0.pos) ^ (p2.pos - p0.pos);
		normal.Normalize();
		face_normals.push_back(normal);
	}

	// assign the normal to the vertices
	for (int i = 0; i < (int)ps._particles.size(); i++)
	{
		Particle & p = *(ps.GetParticleAt(i));
		p.normal = Vector3(0, 0, 0);

		set<int>::iterator it = p.faces.begin();
		for (; it != p.faces.end(); it++)
			p.normal += face_normals[*it];
		p.normal /= (int) p.faces.size();
	}
	// ------------  end compute normals 

	// draw fixed points as small spheres
	glPolygonMode(GL_FRONT_AND_BACK ,GL_FILL);
	for (int i = 0; i < (int)ps._particles.size(); i++)
	{
		if (ps.GetParticleAt(i)->isFixed)
		{
			Vector3 & pos = ps.GetParticleAt(i)->pos;
			glPushMatrix();
			glTranslatef(pos.x, pos.y, pos.z);
			glColor3f(1.f, 0.f, 0.f);
			gluSphere(gluNewQuadric(), .01, 10, 10);
			glPopMatrix();
		}
	}


	// draw the mesh
	glEnableClientState (GL_VERTEX_ARRAY);
	glEnableClientState (GL_NORMAL_ARRAY);
	glVertexPointer(3, GL_DOUBLE, sizeof(Particle), &(ps.GetParticleAt(0)->pos.x));
	glNormalPointer(GL_DOUBLE, sizeof(Particle), &(ps.GetParticleAt(0)->normal.x));

	// draw filled triangles
	glColor3f(.9f, .9f, .95f);
	glPolygonMode(GL_FRONT_AND_BACK ,GL_FILL);
	glDrawElements(GL_TRIANGLES , (GLsizei)nb_faces * 3, GL_UNSIGNED_SHORT, face_indices);

	// draw wireframe
	glColor3f(0.f, 0.f, 0.2f);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glDisable(GL_LIGHTING);
	glPolygonOffset(-1.0f, -1.0f);
	glPolygonMode(GL_FRONT_AND_BACK ,GL_LINE);
	glDrawElements(GL_TRIANGLES , (GLsizei)nb_faces * 3, GL_UNSIGNED_SHORT, face_indices);
	glDisable(GL_POLYGON_OFFSET_LINE);

	glDisableClientState (GL_NORMAL_ARRAY);
	glDisableClientState (GL_VERTEX_ARRAY);
}

// draw the scene
void draw() 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);

	DrawAxes(0.5);

	glPushMatrix();
	glScalef(fit_factor, fit_factor, fit_factor);

	// draw the particle system
	switch (shape)
	{
	case CLOTH:
	case SHELL:
		DrawParticleSystem(grid_vertex_positions, nb_grid_faces, grid_face_indices);
		break;

	case CUBE:
		DrawParticleSystem(cube_vertex_positions, nb_cube_faces, cube_face_indices);
		break;

	case DODECAHEDRON:
		DrawParticleSystem(dodecahedron_vertex_positions, nb_dodecahedron_faces, dodecahedron_face_indices);
		break;

	case PIG:
		DrawParticleSystem(pig_vertex_positions, nb_pig_faces, pig_face_indices);
		break;
	}

	// if picking enabled (right mouse button pressed), handle the picking
	if(pickOn)
	{
		pickedPart = PickPart(mouse2D.x, mouse2D.y, 7, 7, ds);
		if (pickedPart) 
			isDragging = true;

		pickOn = false;
	}

	// while dragging a particle, draw it together with the current mouse cursor in 3D coordinates
	if (isDragging)
	{
		// find mouse position in 3D
		GLdouble p[3];
		GLdouble mm[16], prm[16];
		int viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		glGetDoublev(GL_MODELVIEW_MATRIX, mm);
		glGetDoublev(GL_PROJECTION_MATRIX, prm);

		gluUnProject(mouse2D.x, mouse2D.y, ds, mm, prm, viewport, &p[0], &p[1], &p[2]);

		mouse3D = Vector3(p[0], p[1], p[2]);	// mouse position in 3D

		// draw picked particle and mouse position
		glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
		glPolygonMode(GL_FRONT_AND_BACK ,GL_FILL);
		glEnable(GL_LIGHTING);

		glPushMatrix();
		glColor3f(0.f, 1.f, 0.f);
		glTranslatef(pickedPart->pos.x, pickedPart->pos.y, pickedPart->pos.z);
		gluSphere(gluNewQuadric(), .05, 10, 10);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(mouse3D.x, mouse3D.y, mouse3D.z);
		glColor3f(1.f, 0.f, 0.f);
		gluSphere(gluNewQuadric(), .05, 10, 10);
		glPopMatrix();

		glDisable(GL_LIGHTING);
		glColor3f(0.f, 0.f, 1.f);
		glBegin(GL_LINES);
		glVertex3f(pickedPart->pos.x, pickedPart->pos.y, pickedPart->pos.z);
		glVertex3f(mouse3D.x, mouse3D.y, mouse3D.z );
		glEnd();
		glPopAttrib();
	}

	glPopMatrix();

	DrawInfo();

	glutSwapBuffers();
}


void idle() { }


void BuildCloth()
{
	// cloth has strong stretch stiffness and low bending
	ps.stretch_stiffness = 0.8;
	ps.bending_stiffness = 0.05;
	ps.volume_stiffness = 0.0;
	ps.DefineSystem(nb_grid_vertices, 
		nb_grid_faces, 
		nb_grid_edges, 
		grid_vertex_positions, 
		grid_face_indices,
		grid_edge_indices);


	ps.GetParticleAt(0)->isFixed = true;
	ps.GetParticleAt(9)->isFixed = true;

	shape = CLOTH;
	fit_factor = .9f;
}

void BuildShell()
{
	// shell has both strong stretch stiffness and bending
	ps.stretch_stiffness = 0.8;
	ps.bending_stiffness = 0.8;
	ps.volume_stiffness = 0.0;
	ps.DefineSystem(nb_grid_vertices, 
		nb_grid_faces, 
		nb_grid_edges, 
		grid_vertex_positions, 
		grid_face_indices,
		grid_edge_indices);


	ps.GetParticleAt(0)->isFixed = true;
	ps.GetParticleAt(9)->isFixed = true;

	shape = SHELL;
	fit_factor = .9f;
}

void BuildCube()
{
	ps.stretch_stiffness = 0.8;
	ps.bending_stiffness = 0.5;
	ps.volume_stiffness = 1.0;
	ps.DefineSystem(nb_cube_vertices, 
		nb_cube_faces, 
		nb_cube_edges, 
		cube_vertex_positions, 
		cube_face_indices,
		cube_edge_indices);

	shape = CUBE;

	fit_factor = 0.35f;
	step_volume = ps.volume_constraint->rest_value / 10.0;
}

void BuildDodecahedron()
{
	ps.stretch_stiffness = 1.0;
	ps.bending_stiffness = 0.8;
	ps.volume_stiffness = 1.0;
	ps.DefineSystem(nb_dodecahedron_vertices, 
		nb_dodecahedron_faces, 
		nb_dodecahedron_edges, 
		dodecahedron_vertex_positions, 
		dodecahedron_face_indices,
		dodecahedron_edge_indices);

	shape = DODECAHEDRON;

	fit_factor = 0.1f;
	step_volume = ps.volume_constraint->rest_value / 10.0;

}

void BuildPig()
{
	ps.stretch_stiffness = 0.5;
	ps.bending_stiffness = 0.5;
	ps.volume_stiffness = 1.0;
	ps.DefineSystem(nb_pig_vertices, 
		nb_pig_faces, 
		nb_pig_edges, 
		pig_vertex_positions, 
		pig_face_indices,
		pig_edge_indices);

	shape = PIG;

	fit_factor = 0.25f;
	step_volume = ps.volume_constraint->rest_value / 10.0;
}


void initGL(int width, int height) 
{
	GLfloat light_diffuse_0[] = {0.3, 0.3, 0.35, 1.0};
	glLightfv (GL_LIGHT0, GL_DIFFUSE,	light_diffuse_0);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	reshape(width, height);

	glClearColor(0.6f, 0.6f, 0.8f, 1.0f);
	glClearDepth(1.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	ps.Reset();
	BuildCube();
}


void animation(int t)
{
	if ((shape == CLOTH) || (shape == SHELL))
		for (int i = 0; i < (int)ps._particles.size(); i++)
			ps.GetParticleAt(i)->acc = Vector3(0, -9.81, 0);

	if (!bFreeze)
	{
		if ((pickedPart != NULL) && isDragging)
			pickedPart->pos = mouse3D;

		ps.TimeStep();
	}

	glutPostRedisplay();

	glutTimerFunc((int) 1000/FPS, animation, 0);
}


int main(int argc, char** argv) 
{
	int width = 800;
	int eight = 600;

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(width, eight);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Position-based Dynamics - Constraints Test");

	glutKeyboardFunc(keyboardDown);
	glutSpecialFunc(specialDown);

	glutMouseFunc(mouseClick);
	glutMotionFunc(mouseMotion);
	glutReshapeFunc(reshape);
	glutDisplayFunc(draw);  
	glutIdleFunc(idle);
	glutTimerFunc((int) 1000/FPS, animation, 0);
	glutIgnoreKeyRepeat(false); // process keys held down

	initGL(width, eight);

	glutMainLoop();
	return 0;
}

//
// Utility routine to calculate the 3D position of a 
// projected unit vector onto the xy-plane. Given any
// point on the xy-plane, we can think of it as the projection
// from a sphere down onto the plane. The inverse is what we
// are after.
//
Vector3 trackBallMapping(int x, int y)
{
	Vector3 v;
	double d;

	v.x = (2.0 * x - window_size_x) / window_size_x;
	v.y = (window_size_y - 2.0 * y) / window_size_y;
	v.z = 0.0;
	d = v.Length();
	d = (d < 1.0) ? d : 1.0;  // If d is > 1, then clamp it at one.
	v.z = sqrtf( 1.001 - d * d );  // project the line segment up to the surface of the sphere.

	v.Normalize();  // We forced d to be less than one, not v, so need to normalize somewhere.


	return v;
}

// x and y are the current 2D coords of the mouse
// w and h are the width and the height of the pickable area (around the mouse cursor)
// dist is filled with the distance of the picked primitive from the mouse cursor
Particle * PickPart(int x, int y,int w,int h, float & dist)
{
	long hits;	
	int sz = (int)ps._particles.size() * 5;
	unsigned int *selectBuf = new unsigned int[sz];

	glSelectBuffer(sz, selectBuf);
	glRenderMode(GL_SELECT);
	glInitNames();

	/* Because LoadName() won't work with no names on the stack */
	glPushName(0);
	double mp[16];

	int viewport[4];
	glGetIntegerv(GL_VIEWPORT,viewport);
	glMatrixMode(GL_PROJECTION);
	glGetDoublev(GL_PROJECTION_MATRIX ,mp);
	glPushMatrix();
	glLoadIdentity();
	gluPickMatrix(x, y, w, h, viewport);
	glMultMatrixd(mp);

	glMatrixMode(GL_MODELVIEW);

	int cnt = 0; 
	glColor3f(1.f, 0.f, 0.f);
	vector<Particle>::iterator pi = ps._particles.begin();
	for(; pi != ps._particles.end(); ++pi)
	{
		glLoadName(cnt);

		glBegin(GL_POINTS);
		glVertex3f((*pi).pos.x, (*pi).pos.y, (*pi).pos.z);
		glEnd();
		cnt++;
	}

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	hits = glRenderMode(GL_RENDER);
	std::vector< std::pair<double,int> > H;
	for(int ii = 0; ii < hits; ii++){
		H.push_back( std::pair<double,int>(selectBuf[ii*4+1]/4294967295.0,selectBuf[ii*4+3]));
	}

	delete selectBuf;
	if(H.empty()) 
	{
		return NULL;
	}
	std::sort(H.begin(),H.end());

	dist = H[0].first;
	return (Particle*)&ps._particles[H[0].second];
}

// Custom GL "Print" Routine
// needs glut
void glPrint(float* c, float x, float y, float z, const char *fmt, ...)
{
	if (fmt == NULL)	// If There's No Text
		return;			// Do Nothing

	char text[256];		// Holds Our String
	va_list ap;			// Pointer To List Of Arguments

	va_start(ap, fmt);								// Parses The String For Variables
	vsprintf(text,/* 256 * sizeof(char),*/ fmt, ap);	// And Converts Symbols To Actual Numbers
	va_end(ap);										// Results Are Stored In Text

	size_t len = strlen(text);

	if (c != NULL)
		glColor3fv(c);

	glRasterPos3f(x, y, z);
	for(size_t i = 0; i < len; i++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, text[i]);

}


