
#include "octree.h"
#include "utility.h"



// CONSTRUCTOR ///////////////////////////////////////////////////////////

c_Octree::c_Octree(AABB3 _aabb, int _level)
{
	memset(m_pChildren, 0, sizeof(m_pChildren));

	m_AABB = _aabb;
	m_iLevel = _level;
	m_bContainsVertex = false;
	m_vVertices.clear();

} // end c_Octree::c_Octree()



// MEMBER FUNCTIONS //////////////////////////////////////////////////////

bool c_Octree::Insert(c_Octree *_root, Vector3t<double> _v)
{
	if (/*!_root->GetAABB().ContainsPoint(_v) || */_root->GetLevel() > OCTREE_MAX_DEPTH)
		return (false);

	Logw("insert at level %d\n", _root->GetLevel());

	if (_root->ContainsVertex() && _root->GetChild(0) != NULL)
	{
		// this node have already been split and hence the vertex
		// should be inserted into the correct child node
		int index = _root->GetChildIndex(_root, _v);
		_root->Insert(_root->GetChild(index), _v);

		Logw("\ttried to insert into node at level %d, but already contained vertices below.\tInserted into child %d\n", _root->GetLevel(), index);

		_root->SetContainsVertex(true);

		return (true);

	}
	else if (_root->GetVertices().size() >= OCTREE_MAX_NODE_VERTICES &&
			 _root->GetLevel() != OCTREE_MAX_DEPTH)
	{
		_root->Split(_root);

		Logw("\tperformed a split due to % verts in this node (level %d). Inserting vertices into children\n", _root->GetLevel());

		// add the incoming vertex
		_root->AddVector(_v);

		std::vector<Vector3t<double> > vertices = _root->GetVertices();

		// step through all vertices already in this node
		// and insert into children instead
		for (size_t i = 0; i < vertices.size(); i++)
		{
			int index = _root->GetChildIndex(_root, vertices[i]);
			_root->Insert(_root->GetChild(index), vertices[i]);
		}
		
		Logw("\t(%d verts in here that were inserted)\n", vertices.size());
		_root->ClearVertices();

		// this node (or its children) contains vertices
		_root->SetContainsVertex(true);

		return (true);
	}
	else
	{
		Logw("this is the end, at level %d. %d verts at this level\n", _root->GetLevel(), _root->GetVertices().size());
		// ended up here if
		//	1. we reached max depth
		//	2. there is still room for more vertices in this node
		_root->AddVector(_v);

		// this node (or its children) contains vertices
		_root->SetContainsVertex(true);

		return (true);
	}

	return (false);

} // end c_Octree::Insert()

//////////////////////////////////////////////////////////////////////////
void c_Octree::Split(c_Octree *_root)
{
	AABB3 aabb = _root->GetAABB();
	Vector3t<double> half = aabb.HalfAABB();

	//Logw("SPLIT(): \tv0[ %.1f  %.1f  %.1f]   v1[ %.1f  %.1f  %.1f ]\n",
	//	aabb.v0.x, aabb.v0.y, aabb.v0.z, aabb.v1.x, aabb.v1.y, aabb.v1.z);

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
int c_Octree::GetChildIndex(c_Octree *_root, Vector3t<double> _v)
{
	AABB3 aabb = _root->GetAABB();
	Vector3t<double> half = aabb.HalfAABB();

	/*
	int index = 0;

	bool right = _v.x > half.x;
	bool top = _v.y > half.y;
	bool back = _v.z > half.z;

	index = right + (top << 1) + (back << 2);
	*/

	return ((_v.x > half.x) + ((_v.y > half.y) << 1) + ((_v.z > half.z) << 2));
	
} // end c_Octree::GetChildIndex()

//////////////////////////////////////////////////////////////////////////
void c_Octree::DestroyTree(c_Octree *_root)
{
	if (_root == NULL)
		return;

	for (int i = 0; i < 8; i++)
		_root->DestroyTree(_root->GetChild(i));

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
	if (_root == NULL)
		return;

	if ((_root->GetLevel() > 0 && _root->GetVertices().size() > 0) || _root->GetLevel() == 0)
	{
		AABB3 aabb = _root->GetAABB();

		// X lines
		_vlines->push_back(Line3(Vector3t<double>(aabb.v0.x, aabb.v0.y, aabb.v0.z), Vector3t<double>(aabb.v1.x, aabb.v0.y, aabb.v0.z)));
		_vlines->push_back(Line3(Vector3t<double>(aabb.v0.x, aabb.v1.y, aabb.v0.z), Vector3t<double>(aabb.v1.x, aabb.v1.y, aabb.v0.z)));
		_vlines->push_back(Line3(Vector3t<double>(aabb.v0.x, aabb.v0.y, aabb.v1.z), Vector3t<double>(aabb.v1.x, aabb.v0.y, aabb.v1.z)));
		_vlines->push_back(Line3(Vector3t<double>(aabb.v0.x, aabb.v1.y, aabb.v1.z), Vector3t<double>(aabb.v1.x, aabb.v1.y, aabb.v1.z)));

		// Y lines
		_vlines->push_back(Line3(Vector3t<double>(aabb.v0.x, aabb.v0.y, aabb.v0.z), Vector3t<double>(aabb.v0.x, aabb.v1.y, aabb.v0.z)));
		_vlines->push_back(Line3(Vector3t<double>(aabb.v1.x, aabb.v0.y, aabb.v0.z), Vector3t<double>(aabb.v1.x, aabb.v1.y, aabb.v0.z)));
		_vlines->push_back(Line3(Vector3t<double>(aabb.v0.x, aabb.v0.y, aabb.v1.z), Vector3t<double>(aabb.v0.x, aabb.v1.y, aabb.v1.z)));
		_vlines->push_back(Line3(Vector3t<double>(aabb.v1.x, aabb.v0.y, aabb.v1.z), Vector3t<double>(aabb.v1.x, aabb.v1.y, aabb.v1.z)));

		// Z lines
		_vlines->push_back(Line3(Vector3t<double>(aabb.v0.x, aabb.v0.y, aabb.v0.z), Vector3t<double>(aabb.v0.x, aabb.v0.y, aabb.v1.z)));
		_vlines->push_back(Line3(Vector3t<double>(aabb.v1.x, aabb.v0.y, aabb.v0.z), Vector3t<double>(aabb.v1.x, aabb.v0.y, aabb.v1.z)));
		_vlines->push_back(Line3(Vector3t<double>(aabb.v0.x, aabb.v1.y, aabb.v0.z), Vector3t<double>(aabb.v0.x, aabb.v1.y, aabb.v1.z)));
		_vlines->push_back(Line3(Vector3t<double>(aabb.v1.x, aabb.v1.y, aabb.v0.z), Vector3t<double>(aabb.v1.x, aabb.v1.y, aabb.v1.z)));

	}

	for (int i = 0; i < 8; i++)
		_root->LinesAABB(_root->GetChild(i), _vlines);


} // end c_Octree::LinesAABB()

//////////////////////////////////////////////////////////////////////////
void c_Octree::Print(c_Octree *_root)
{
	if (_root == NULL)
		return;

	for (int i = 0; i < 8; i++)
		_root->Print(_root->GetChild(i));
	
	AABB3 aabb = _root->GetAABB();
	Logw("level = %d\tv0[ %.1f  %.1f  %.1f]   v1[ %.1f  %.1f  %.1f ]\tnodes = %d (%s)\n",
		_root->GetLevel(), aabb.v0.x, aabb.v0.y, aabb.v0.z, aabb.v1.x, aabb.v1.y, aabb.v1.z, _root->GetVertices().size(), _root->ContainsVertex() ? "TRUE" : "FALSE");

} // end c_Octree::Print()


