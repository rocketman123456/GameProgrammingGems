// IawFrameWork.h App Wizard Version 2.0 Beta 1
// ----------------------------------------------------------------------
// 
// Copyright � 2001 Intel Corporation
// All Rights Reserved
// 
// Permission is granted to use, copy, distribute and prepare derivative works of this 
// software for any purpose and without fee, provided, that the above copyright notice
// and this statement appear in all copies.  Intel makes no representations about the
// suitability of this software for any purpose.  This software is provided "AS IS." 
//
// Intel specifically disclaims all warranties, express or implied, and all liability,
// including consequential and other indirect damages, for the use of this software, 
// including liability for infringement of any proprietary rights, and including the 
// warranties of merchantability and fitness for a particular purpose.  Intel does not
// assume any responsibility for any errors which may appear in this software nor any
// responsibility to update it.

// ----------------------------------------------------------------------
// Authors: Kim Pallister,Dean Macri - Intel Technology Diffusion Team
// ----------------------------------------------------------------------

#if !defined(IawFrameWork_h)
#define IawFrameWork_h

#define STRICT
#define D3D_OVERLOADS 1

#define PI            3.14159265358979323846f

typedef float FP_t;



#include "Utils\MissingDX7Stuff.h"
#include <d3dvec.inl>
#include <d3d8.h>
#include <d3d8types.h>
#include <dxfile.h>

#include <mmsystem.h>
#include <stdio.h>
#include <math.h>

#define SAFE_RELEASE( x ) do { if( (x) ) { x->Release(); x = NULL; } } while (0)
#define SAFE_DELETE( x ) do { if( (x) ) { delete x; x = NULL;} } while (0)
#define SAFE_DELETE_ARRAY( x ) do { if( (x) ) { delete [] x; x = NULL;} } while (0)

inline DWORD FLOATtoDWORD(FLOAT f) {return* ((DWORD *)&f);}
inline FLOAT DWORDtoFLOAT(DWORD d) {return* ((FLOAT *)&d);}

#include "IawD3dWrapper.h"

#include "FrameWork\IawMatrix.h"


#include "Utils\IawNoiseGenerator.h"
#include "Utils\IawExponentialNoise.h"

#include "DataIO\IawDataStream.h"
#include "DataIO\IawFileStream.h"
#include "DataIO\IawImageLoader.h"
#include "DataIO\IawBmpLoader.h"

#include "FrameWork\IawTexture.h"
#include "FrameWork\IawRenderTarget.h"

// Include this to draw normals on any terrain objects (for debugging only,
// poor performance)
//#define DRAWNORMALS

#include "Shaders\IawShader.h"
#include "Shaders\IawShaderMgr.h" //manages the shader efcee

#include "Objects\IawSuperVertex.h"
#include "Objects\IawObject.h"
#include "Objects\IawBackground.h"
#include "Objects\IawCone.h"
#include "Objects\IawCube.h"
#include "Objects\IawCylinder.h"
#include "Objects\IawPlane.h"
#include "Objects\IawSkyPlane.h"
#include "Objects\IawSphere.h"
#include "Objects\IawGnomon.h"
#include "Objects\IawTerrain.h"

// Managers are special objects that don't contain geometry, but manage other objects that do.
#include "Objects\IawTextMgr.h"
#include "Objects\IawTerrainMgr.h"

// The camera is a special object that has some additional properties
#include "Objects\IawCamera.h"



#include "RandTexture.h"
#include "StaticRandTexture.h"
#include "ProceduralCloudDemo.h"
#include "IawWindow3d.h"

#endif // !defined(IawFrameWork_h)
