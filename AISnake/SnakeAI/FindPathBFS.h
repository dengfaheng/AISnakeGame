#pragma once
#include <queue>
using std::queue;

typedef struct
{
	int x;
	int y;
}COORDINATE;

class FindPathBFS
{
private:
	bool **m_chess;//用矩阵表示的图.m_chess是一个二维数组,其中false表示通路,true表示不通
	bool **m_visit;//节点是否被访问过
	COORDINATE **m_parent;//每个访问过的节点的父节点.
	COORDINATE m_size;//图的大小.
	void FindPath(COORDINATE begin_point, COORDINATE end_point);
	void GetQueue(COORDINATE end_point);

public:
	FindPathBFS();
	
	~FindPathBFS();

	void InitMap(bool **chess);
	void GetPath(COORDINATE begin_point, COORDINATE end_point);

	queue<COORDINATE> m_paths_queue;

};

