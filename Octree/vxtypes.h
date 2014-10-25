#ifndef __VXTYPES_H
#define __VXTYPES_H


#pragma warning(disable : 4005)


// INCLUSIONS ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


// DEFINITIONS ///////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#define CHUNK_SIZE				16



// MACROS / INLINES //////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

// Chunk coordinate for-loop .............................................
#define FOR_XYZ(_START_X, _END_X, _START_Y, _END_Y, _START_Z, _END_Z)\
	int x, y, z;\
	for(x=(_START_X);x<(_END_X);x++){\
		for(y=(_START_Y);y<(_END_Y);y++){\
			for(z=(_START_Z);z<(_END_Z);z++){\

#define ENDFOR }}}

// Chunk voxel indexing ..................................................
#define CHUNK_INDEX(_X, _Y, _Z)	_X+_Y*CHUNK_SIZE+_Z*CHUNK_SIZE_PWR2

// Clamp integer on CHUNK_SIZE and 0 .....................................
inline int clamp_chunk(int n)
{
	n = (n > CHUNK_SIZE-1) ? CHUNK_SIZE-1 : n;
	return ((n < 0) ? 0 : n);

} // end inline clamp


// DATA TYPES ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
struct Index3ui
{
	unsigned int v0, v1, v2;
	
	Index3ui(unsigned int _v0, unsigned int _v1, unsigned int _v2) : v0(_v0), v1(_v1), v2(_v2) {};
	Index3ui() {};
};

struct Index3i
{
	int v0, v1, v2;
	
	Index3i(int _v0, int _v1, int _v2) : v0(_v0), v1(_v1), v2(_v2) {};
	Index3i() {};
};

struct rgb3uc
{
	unsigned char r, g, b;

	rgb3uc(unsigned char _r, unsigned char _g, unsigned char _b) : r(_r), g(_g), b(_b) {};
	rgb3uc() {};
};

struct rgb3f
{
	float r, g, b;

	rgb3f(float _r, float _g, float _b) : r(_r), g(_g), b(_b) {};
	rgb3f() {};
};

struct rgba4f
{
	float r, g, b, a;

	rgba4f(float _r, float _g, float _b, float _a) : r(_r), g(_g), b(_b), a(_a) {};
	rgba4f() {};
};


// CLASSES ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


#endif // __VXTYPES_H
