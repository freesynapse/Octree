#ifndef __UTILITY_H
#define __UTILITY_H

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifndef _CRT_NON_CONFORMING_SWPRINTFS
#define _CRT_NON_CONFORMING_SWPRINTFS
#endif

#pragma warning(disable : 4005)


// INCLUSIONS ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

// Header inclusions .....................................................
#include <windows.h>		// include important windows stuff
#include <windowsx.h> 
#include <mmsystem.h>
#include <Psapi.h>
#include <Pdh.h>

#include <conio.h>			// include important C/C++ stuff
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <math.h>
#include <io.h>
#include <fcntl.h>
#include <sys/timeb.h>
#include <time.h>
#include <tchar.h>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <iterator>
#include <algorithm>
#include <sstream>
#include <limits>
#include <cstdint>

#include <gl/glew.h>
#include <gl/freeglut.h>


#include "math3d.h"


// Library inclusions ....................................................
#pragma comment (lib, "freeglut.lib")
#pragma comment (lib, "glew32.lib")
#pragma comment (lib, "freetype.lib")

#pragma comment (lib, "psapi.lib")
#pragma comment (lib, "pdh.lib")

// Warning suppression ...................................................
#pragma warning(disable:4996)		// sprintf / vsprintf


// DEFINITIONS ///////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#define RETURN_SUCCESS						0x0
#define RETURN_FAILURE						0x1

//#define EXIT_SUCCESS						0x0
//#define EXIT_FAILURE						0x1
#define EXIT_WARNING						0x2

#define APPLICATION_NAME					"OctreeSpheres"


// TYPE DEFINITIONS /STRUCTS /////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

struct process_info
{
	DWORD	process_id;
	float	process_physical;	// mb
	float	process_virtual;	// mb
	double	process_cpu;		// percent
};


// CONSTANTS /////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


// MACROS / INLINE FUNCTIONS /////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#define BUFFER_OFFSET(offset)				((void *)(offset))


// CLASSES ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


// FUNCTION PROTOTYPES ///////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

// Error utility .........................................................
int					OpenLogFile				(char *filename);
int					CloseLogFile			(void);
int					Logw					(char *str, ...);

// Frame rate utility ....................................................
DWORD				StartClock				(void);
DWORD				GetClock				(void);
DWORD				WaitClock				(DWORD count);

// More precise counters .................................................
__int64				StartCounter			(void);
__int64				GetCounter				(void);
double				WaitCounter				(double _time);

// WinAPI necessities ....................................................
TCHAR				*CStrToTCHAR			(char *_str);
Vector2t<int>		WindowsDimensions		();
__int64				FileSize				(const char *_filename);
process_info		ProcessInfo				();
void				InitProcessCPU			();
double				ProcessCPU				();

// Debugging functions ...................................................
void				PrintGLMatrix			(int _matrix, const char *_str);

// Load Blender OBJ files ................................................
void				LoadBlenderObject		(std::string _filename,
											 std::vector<Vector3t<float> > &_vertices,
											 std::vector<Vector3t<float> > &_normals,
											 int &_n_vertices,
											 int &_n_normals);



// GLOBAL VARIABLES //////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

extern FILE				*fpLog;				// pointer to log file
extern DWORD			dwTickCount;		// FPS counter global

extern __int64			sg_lFrame;
extern double			sg_dFPS;
extern double			sg_dTime;
extern process_info		sg_processInfo;

extern double			PCFreq;
extern __int64			i64CounterStart;
extern __int64			i64Time;



#endif // __UTILITY_H

