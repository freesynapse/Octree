#ifndef __OCTREE_H
#define __OCTREE_H


// INCLUSIONS ////////////////////////////////////////////////////////////

#include <vector>
#include "math3d.h"



// DEFINITIONS / MACROS //////////////////////////////////////////////////

#define OCTREE_MAX_DEPTH				7
#define OCTREE_MAX_NODE_VERTICES		4



// STRUCTURES / TYPE DEFINITIONS /////////////////////////////////////////

// Simple line in three dimensions
struct Line3
{
	Vector3t<double> v0, v1;
	Line3() {}
	Line3(Vector3t<double> _v0, Vector3t<double> _v1) :
		v0(_v0), v1(_v1) {}

};

// Axis-aligned bounding box in three dimensions
struct AABB3
{
	Vector3t<double> v0;
	Vector3t<double> v1;

	AABB3() {}
	AABB3(Vector3t<double> _v0, Vector3t<double> _v1) :
		v0(_v0), v1(_v1) {}
	AABB3(double _x0, double _x1, double _y0, double _y1, double _z0, double _z1)
	{
		v0 = Vector3t<double>(_x0, _y0, _z0);
		v1 = Vector3t<double>(_x1, _y1, _z1);
	}

	bool ContainsPoint(Vector3t<double> _v)
	{
		return (_v.x >= v0.x && _v.x <= v1.x &&
				_v.y >= v0.y && _v.y <= v1.y &&
				_v.z >= v0.z && _v.z <= v1.z);
	}

	Vector3t<double> HalfAABB()
	{
		double h[3];
		h[0] = v0.x + ((v1.x - v0.x) / 2.0);
		h[1] = v0.y + ((v1.y - v0.y) / 2.0);
		h[2] = v0.z + ((v1.z - v0.z) / 2.0);

		return (Vector3t<double>(h[0], h[1], h[2]));
	}

};

// CLASSES ///////////////////////////////////////////////////////////////

class c_Octree
{
public:
	// Constructor
	c_Octree(AABB3 _aabb, int _level = 0);

	// Accessors
	c_Octree *GetChild(int _i)						{	return (m_pChildren[_i]);	}
	void SetChild(int _i, c_Octree *_tree)			{	m_pChildren[_i] = _tree;	}
	AABB3 GetAABB()									{	return (m_AABB);			}
	int GetLevel()									{	return (m_iLevel);			}
	void AddVector(Vector3t<double> _v)				{	m_vVertices.push_back(_v);	}
	std::vector<Vector3t<double> > GetVertices()	{	return (m_vVertices);		}
	void ClearVertices()							{	m_vVertices.clear();		}
	bool ContainsVertex()							{	return (m_bContainsVertex); }
	void SetContainsVertex(bool _b)					{	m_bContainsVertex = _b;		}

	// Member functions
	bool Insert(c_Octree *_root, Vector3t<double> _v);
	void Split(c_Octree *_root);
	int GetChildIndex(c_Octree *_root, Vector3t<double> _v);
	void DestroyTree(c_Octree *_root);
	int TreeDepth(c_Octree *_root);

	void LinesAABB(c_Octree *_root, std::vector<Line3> *_vlines);
	void Print(c_Octree *_root);

private:
	c_Octree *m_pChildren[8];
	AABB3 m_AABB;
	int m_iLevel;
	bool m_bContainsVertex;

	std::vector<Vector3t<double> > m_vVertices;

};



#endif // __OCTREE_H

