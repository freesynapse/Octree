#ifndef __MAINGLOBALS_H
#define __MAINGLOBALS_H


// Inclusions ............................................................
#include "utility.h"
#include "shader.h"
#include "math3d.h"
#include "fonts.h"
#include "camera.h"
#include "octree.h"
#include "object.h"

#include <random>


// Definitions ...........................................................
#define SCREEN_WIDTH				800
#define SCREEN_HEIGHT				600
#define APP_FULLSCREEN				1


// Function prototypes ...................................................
// .GLUT callbacks
//

// ..Render
void Update();
void Render();
void Release();

// ..User input
void KeyDown(unsigned char key, int x, int y);
void KeyUp(unsigned char key, int x, int y);
void KeySpecialDown(int key, int x, int y);
void KeySpecialUp(int key, int x, int y);
void KeyboardAsync();

void MouseButton(int button, int state, int x, int y);
void MouseMove(int x, int y);
void MouseMovePassive(int x, int y);

// .the rest
void SetupGeometry();


// Globals ...............................................................

// .shaders
GLuint main_program;
c_ShaderObject *pShader;
c_GLSLProgram *pProgramLines;
c_GLSLProgram *pProgramGeometry;
c_OGLObject *pSphere;

// .font
c_Font *pFont;
double dFPS;

// .geometry
GLuint vaoNodes,
	   vboNodePositions,
	   vaoLines,
	   vboLines;
int nVertices = 0;
int nLines = 0;

// octree
c_Octree *pTree;

Matrix4f mModelMatrix;
Matrix4f mViewMatrix;
Matrix4f mProjectionMatrix;
glm::fquat qRotation;
Vector3t<float> vRotationAxis = Vector3t<float>(0.0f, 0.0f, 1.0f);
float fTheta = 0.0f;

int nTreeLevels;
int iDisplayLevel = 0;
Vector3t<float> rgbNodeColor = Vector3t<float>(1.0f, 0.0f, 0.0f);


// .camera/input
c_Camera *pCamera = NULL;
std::map<unsigned char, bool> bKeystate;
std::map<int, bool> bSpecialKeystate;

int iMouseDeltaX, iMouseDeltaY;
int iHalfScreenX = SCREEN_WIDTH / 2,
	iHalfScreenY = SCREEN_HEIGHT / 2;

bool bWireframeState = false;
bool bCullState = true;
int frame;

Vector2t<int> vWindowDimensions;





#endif // __MAINGLOBALS_H
