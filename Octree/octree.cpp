
#include "octree.h"
#include "utility.h"



// CONSTRUCTOR ///////////////////////////////////////////////////////////

c_Octree::c_Octree(AABB3 _aabb, int _level = 0)
{
	for (int i = 0; i < 8; i++)
		m_pChildren[i] = NULL;

	m_AABB = _aabb;
	m_iLevel = _level;
	m_bContainsVertex = false;
	m_vVertices.clear();

} // end c_Octree::c_Octree()



// MEMBER FUNCTIONS //////////////////////////////////////////////////////

bool c_Octree::Insert(c_Octree *_root, Vector3t<double> _v)
{
	if (!_root->GetAABB().ContainsPoint(_v) || _root->GetLevel() > OCTREE_MAX_DEPTH)
		return (false);

	_root->AddVector(_v);

} // end c_Octree::Insert()

//////////////////////////////////////////////////////////////////////////
void c_Octree::Split(c_Octree *_root)
{
	AABB3 aabb = _root->GetAABB();
	Vector3t<double> half = aabb.HalfAABB();

	_root->SetChild(0, new c_Octree(AABB3(aabb.v0.x, half.x, aabb.v0.y, half.y, aabb.v0.z, half.z), _root->GetLevel() + 1));
	_root->SetChild(1, new c_Octree(AABB3(half.x, aabb.v1.x, aabb.v0.y, half.y, aabb.v0.z, half.z), _root->GetLevel() + 1));
	_root->SetChild(2, new c_Octree(AABB3(aabb.v0.x, half.x, half.y, aabb.v1.y, aabb.v0.z, half.z), _root->GetLevel() + 1));
	_root->SetChild(3, new c_Octree(AABB3(half.x, aabb.v1.x, half.y, aabb.v1.y, aabb.v0.z, half.z), _root->GetLevel() + 1));
	_root->SetChild(4, new c_Octree(AABB3(aabb.v0.x, half.x, aabb.v0.y, half.y, half.z, aabb.v1.z), _root->GetLevel() + 1));
	_root->SetChild(5, new c_Octree(AABB3(half.x, aabb.v1.x, aabb.v0.y, half.y, half.z, aabb.v1.z), _root->GetLevel() + 1));
	_root->SetChild(6, new c_Octree(AABB3(aabb.v0.x, half.x, half.y, aabb.v1.y, half.z, aabb.v1.z), _root->GetLevel() + 1));
	_root->SetChild(7, new c_Octree(AABB3(half.x, aabb.v1.x, half.y, aabb.v1.y, half.z, aabb.v1.z), _root->GetLevel() + 1));

} // end c_Octree::Split()

//////////////////////////////////////////////////////////////////////////
void c_Octree::Clear(c_Octree *_root)
{
	if (_root == NULL)
		return;

	for (int i = 0; i < 8; i++)
		_root->Clear(GetChild(i));

	delete _root;
	_root = NULL;

} // end c_Octree::Clear()

//////////////////////////////////////////////////////////////////////////
int c_Octree::TreeDepth(c_Octree *_root)
{
	if (_root == NULL)
		return (0);

	int d = 0;
	for (int i = 0; i < 8; i++)
		d = max(_root->TreeDepth(_root->GetChild(i)), d);

	return (d + 1);

} // end c_Octree::TreeDepth()

//////////////////////////////////////////////////////////////////////////
void c_Octree::LinesAABB(c_Octree *_root, std::vector<Line3> *_vlines)
{

} // end c_Octree::LinesAABB()

//////////////////////////////////////////////////////////////////////////
void c_Octree::Print(c_Octree *_root)
{

} // end c_Octree::Print()


