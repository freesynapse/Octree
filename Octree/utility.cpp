

// INCLUSIONS ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include "utility.h"
#include "vxtypes.h"
#include "math3d.h"



// GLOBALS ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

FILE					*fpLog;
DWORD					dwTickCount;

__int64					sg_lFrame = 0;
double					sg_dFPS = 0.0;
double					sg_dTime = 0.0;
process_info			sg_processInfo;

double					PCFreq = 0.0;
__int64					i64CounterStart = 0;
__int64					i64Time = 0;

// process handling variables
static ULARGE_INTEGER	lastCPU;
static ULARGE_INTEGER	lastSysCPU;
static ULARGE_INTEGER	lastUserCPU;

static int				nProcessors;

static HANDLE			processHandle;



// ERROR UTILITY FUNCTIONS ///////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

int OpenLogFile(char *filename)
{
// Open error file for logging.

	// test if valid filename, return on invalid file
	if ((fpLog = fopen(filename, "w")) == NULL)
		return (0);

	
	time_t		t;
	struct tm	*cur_time;

	t = time(&t);
	cur_time = localtime(&t);

	Logw("%s: ", APPLICATION_NAME);
	Logw("%d-%.2d-%.2d %.2d:%.2d:%.2d\n\n",	(cur_time->tm_year+1900),
											(cur_time->tm_mon+1),
											(cur_time->tm_mday),
											(cur_time->tm_hour),
											(cur_time->tm_min),
											(cur_time->tm_sec));

	// file ready for logging
	return (1);

} // end Open_Error_File

//////////////////////////////////////////////////////////////////////////
int CloseLogFile(void)
{
// Close error file

	
	time_t		t;
	struct tm	*cur_time;

	t = time(&t);
	cur_time = localtime(&t);

	Logw("\n\nAPPLICATION TERMINATED : ");
	Logw("%d-%.2d-%.2d %.2d:%.2d:%.2d\n\n",	(cur_time->tm_year+1900),
											 (cur_time->tm_mon+1),
											 (cur_time->tm_mday),
											 (cur_time->tm_hour),
											 (cur_time->tm_min),
											 (cur_time->tm_sec));

	// return succes on open file
	if (fpLog)
	{
		fclose(fpLog);
		return (1);
	}
	else
		return (0);

} // end Close_Error_File

//////////////////////////////////////////////////////////////////////////
int	Logw(char *str, ...)
{
// Write error msg with arguments to error file.

	char buffer[1024];		// tmp buffer
	va_list arglist;		// arguments

	if (!str || !fpLog)
		return (0);

	// put str and arglist on stack
	va_start(arglist, str);
	vsprintf(buffer, str, arglist);
	va_end(arglist);

	// write to file
	fprintf(fpLog, buffer);

	// flush in case something F*CKS up
	fflush(fpLog);

	// return success
	return (1);

} // end Errorw



// FRAME RATE UTILITY FUNCTIONS //////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

DWORD GetClock(void)
{
// Return no. of ticks

	return (GetTickCount());

} // end Get_Clock

//////////////////////////////////////////////////////////////////////////
DWORD StartClock(void) 
{
// Starts the clock, i.e. stores the current tick count

	return (dwTickCount = GetClock());

} //end Start_Clock

//////////////////////////////////////////////////////////////////////////
DWORD WaitClock(DWORD count)
{
// Wait for specified no. of clicks since call to Start_Clock();

	while ((GetClock() - dwTickCount) < count);
	return (GetClock());

} // end Wait_Clock



// MORE PRECISE COUNTER //////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

__int64 StartCounter(void)
{
	LARGE_INTEGER li;
	QueryPerformanceFrequency(&li);

	// value in msecs
	PCFreq = double(li.QuadPart) / 1000.0;

	QueryPerformanceCounter(&li);
	return (i64CounterStart = li.QuadPart);

} // end StartCounter()

//////////////////////////////////////////////////////////////////////////
__int64 GetCounter(void)
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	return (li.QuadPart);

} // end GetCounter()

//////////////////////////////////////////////////////////////////////////
double WaitCounter(double _time)
{
	while ((double)((GetCounter() - i64CounterStart)/PCFreq) < _time);
	return ((double)GetCounter());

} // end WaitCounter()



// WINAPI UTILITY FUNCTIONS //////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

TCHAR *CStrToTCHAR(char *_str)
{
	TCHAR	*tStr;
	size_t	wn;

	wn = mbstowcs(NULL, _str, NULL);
	tStr = new TCHAR[wn+1];
	wn = mbstowcs(tStr, _str, wn+1);

	return (tStr);

} // end CharToTCHAR()

//////////////////////////////////////////////////////////////////////////
Vector2t<int> WindowsDimensions()
{
	RECT rDesktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &rDesktop);

	return (Vector2t<int>(rDesktop.right, rDesktop.bottom));

} // end WindowsDimensions()

//////////////////////////////////////////////////////////////////////////
__int64	FileSize(const char *_filename)
{
	__stat64 buf;
	
	size_t len = strlen(_filename) + 1;
	wchar_t wcstr[128];
	size_t n;
	mbstowcs_s(&n, wcstr, len, _filename, _TRUNCATE);

	if (_wstat64(wcstr, &buf) != 0)
	{
		Logw("ERROR: FileSize: _wstat64(%s) = %d\n", _filename, errno);
		return (RETURN_FAILURE);
	}

	return (buf.st_size);


} // end FileSize()

//////////////////////////////////////////////////////////////////////////
process_info ProcessInfo()
{
	// virtual memory used by process
	PROCESS_MEMORY_COUNTERS_EX pmc;
	pmc.cb = sizeof(pmc);
	GetProcessMemoryInfo(GetCurrentProcess(), 
						 (PROCESS_MEMORY_COUNTERS *)&pmc, 
						 pmc.cb);

	process_info processInfo;
	processInfo.process_id = GetCurrentProcessId();
	processInfo.process_physical = (float)pmc.WorkingSetSize / 1048576.0f;	// conversion to Mb
	processInfo.process_virtual = (float)pmc.PrivateUsage / 1048576.0f;
	processInfo.process_cpu = ProcessCPU();

	return (processInfo);

} // end MemoryUsageByProcess()

//////////////////////////////////////////////////////////////////////////
void InitProcessCPU()
{
	SYSTEM_INFO sysInfo;
	FILETIME ftime, fsys, fuser;

	GetSystemInfo(&sysInfo);
	nProcessors = sysInfo.dwNumberOfProcessors;

	GetSystemTimeAsFileTime(&ftime);
	memcpy(&lastCPU, &ftime, sizeof(FILETIME));

	processHandle = GetCurrentProcess();
	GetProcessTimes(processHandle, &ftime, &ftime, &fsys, &fuser);
	memcpy(&lastSysCPU, &fsys, sizeof(FILETIME));
	memcpy(&lastUserCPU, &fuser, sizeof(FILETIME));

} // end InitProcessCPU()

//////////////////////////////////////////////////////////////////////////
double ProcessCPU()
{
	FILETIME ftime, fsys, fuser;
	ULARGE_INTEGER now, sys, user;
	double percent;

	GetSystemTimeAsFileTime(&ftime);
	memcpy(&now, &ftime, sizeof(FILETIME));

	GetProcessTimes(processHandle, &ftime, &ftime, &fsys, &fuser);
	memcpy(&sys, &fsys, sizeof(FILETIME));
	memcpy(&user, &fuser, sizeof(FILETIME));
	
	percent  = (double)((sys.QuadPart - lastSysCPU.QuadPart) + (user.QuadPart - lastUserCPU.QuadPart));
	percent /= (now.QuadPart - lastCPU.QuadPart);
	percent /= nProcessors;

	lastCPU		= now;
	lastSysCPU	= sys;
	lastUserCPU = user;

	return (percent * 100.0);
	

} // end ProcessCPU



// DEBUGGING FUNCTIONS ///////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void PrintGLMatrix(int _matrix, const char *_str)
{
	double mat[16];
	if (_matrix == GL_MODELVIEW_MATRIX)
	{
		glGetDoublev(GL_MODELVIEW_MATRIX, mat);
		Logw("%s (GL_MODELVIEW_MATRIX)\n", _str);
	}
	else if (_matrix == GL_PROJECTION_MATRIX)
	{
		glGetDoublev(GL_PROJECTION_MATRIX, mat);
		Logw("%s (GL_PROJECTION_MATRIX)\n", _str);
	}
	else
	{
		Logw("PrintGLMatrix: error: matrix wasn't found.\n");
		return;
	}

	for (int i = 1; i < 16+1; i++)
	{
		Logw("%.1f  ", mat[i-1]);
		if (i % 4 == 0)
			Logw("\n");
	}
	Logw("\n");

} // end PrintGLMatrix()



// LOAD BLENDER OBJJ FILES ///////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void LoadBlenderObject(std::string _filename,
	std::vector<Vector3f> &_vertices,
	std::vector<Vector3f> &_normals,
	int &_n_vertices,
	int &_n_normals)
{
	std::ifstream infile;
	infile.open(_filename.c_str(), std::ios::in);

	Logw("LoadBlenderObject(): Loading .OBJ file '%s'.\n", _filename.c_str());

	if (!infile)
	{
		Logw("ERROR: LoadBlenderObject(): file '%s' not found. Exit.\n", _filename.c_str());
		exit(-1);
	}

	std::string line;
	std::vector<Index3ui> vElements;
	std::vector<Vector3f> vVertices;
	unsigned int nlines = 0;

	while (std::getline(infile, line))
	{
		if (line.substr(0, 2) == "v ")
		{
			// vertex found
			std::istringstream s(line.substr(2));
			Vector3f v;
			s >> v.x;
			s >> v.y;
			s >> v.z;
			vVertices.push_back(v);
		}
		else if (line.substr(0, 2) == "f ")
		{
			// element found
			std::istringstream s(line.substr(2));
			unsigned int a, b, c;
			s >> a;
			s >> b;
			s >> c;
			vElements.push_back(Index3ui(a, b, c));
		}
		else if (line[0] == '#')
		{
			// comment: do nothing
		}
		else
		{
			// blank line: do nothing
		}

		nlines++;
	}
	Logw("Read %d lines, reformatting data and computing normals:\n", nlines);

	// get rid of the element buffer and compute normals
	for (size_t i = 0; i < vElements.size(); i++)
	{
		Vector3f v0, v1, v2;
		v0.x = vVertices[vElements[i].v0 - 1].x;
		v0.y = vVertices[vElements[i].v0 - 1].y;
		v0.z = vVertices[vElements[i].v0 - 1].z;

		v1.x = vVertices[vElements[i].v1 - 1].x;
		v1.y = vVertices[vElements[i].v1 - 1].y;
		v1.z = vVertices[vElements[i].v1 - 1].z;

		v2.x = vVertices[vElements[i].v2 - 1].x;
		v2.y = vVertices[vElements[i].v2 - 1].y;
		v2.z = vVertices[vElements[i].v2 - 1].z;

		_vertices.push_back(v2);
		_vertices.push_back(v1);
		_vertices.push_back(v0);

		Vector3f n = Cross((v1 - v2), (v0 - v2));
		n.Normalize();
		_normals.push_back(n);
		_normals.push_back(n);
		_normals.push_back(n);

	}

	// number of vertices (i.e. Vector3f structs)
	_n_vertices = (int)_vertices.size();
	_n_normals = (int)_normals.size();

	Logw("\tnumber of vertices\t%d\n\tnumber of normals\t%d\n\n", _n_vertices, _n_normals);

} // end LoadBlenderObject()




