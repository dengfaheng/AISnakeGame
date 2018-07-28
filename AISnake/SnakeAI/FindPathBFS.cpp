#include "FindPathBFS.h"


int direction[4][2] = { { 0,1 },{ 0,-1 },{ 1,0 },{ -1,0 } };


FindPathBFS::FindPathBFS()
{
	m_size.x = 52;
	m_size.y = 40;
	m_chess = new bool *[m_size.x];
	m_visit = new bool *[m_size.x];
	m_parent = new COORDINATE *[m_size.x];

	for (int i = 0; i < m_size.x; i++)
	{
		m_chess[i] = new bool[m_size.y];
		m_visit[i] = new bool[m_size.y];
		m_parent[i] = new COORDINATE[m_size.y];
	}
}


FindPathBFS::~FindPathBFS()
{

	for (int i = 0; i < m_size.x; i++)
	{
		delete[] m_chess[i];
		delete[] m_visit[i];
		delete[] m_parent[i];
	}

	delete[] m_chess;
	delete[] m_visit;
	delete[] m_parent;

}

void FindPathBFS::InitMap(bool **chess)
{
	for (int i = 0; i < m_size.x; i++)
	{
		for (int j = 0; j < m_size.y; j++)
		{
			m_chess[i][j] = *((bool*)chess + m_size.y*i + j);
			m_visit[i][j] = false;
			m_parent[i][j].x = m_parent[i][j].y = -1;
		}
	}
	//清空队列
	while (!m_paths_queue.empty())
		m_paths_queue.pop();
}

void FindPathBFS::FindPath(COORDINATE begin_point, COORDINATE end_point)
{
	queue<COORDINATE> tempque;
	m_visit[begin_point.x][begin_point.y] = true;
	tempque.push(begin_point);

	COORDINATE parent_point, next_point;
	int quesize;

	while (!tempque.empty())
	{
		quesize = tempque.size();
		while (quesize--)
		{
			parent_point = tempque.front();
			tempque.pop();
			if (parent_point.x == end_point.x && parent_point.y == end_point.y)
			{
				return; //到达目的地
			}
			//往当前方块的上下左右四个方块依次尝试
			for (int i = 0; i < 4; i++)
			{
				next_point.x = parent_point.x + direction[i][0];
				next_point.y = parent_point.y + direction[i][1];
				//超出边界
				if (next_point.x < 0 || 
					(next_point.x > (m_size.x - 1)) ||
					next_point.y < 0 ||
					(next_point.y > (m_size.y - 1)) ||
					m_chess[next_point.x][next_point.y])  //路径不通不可走
				{
					continue;
				}
				//当前节点未访问并且能走通
				if (!m_visit[next_point.x][next_point.y])
				{
					m_visit[next_point.x][next_point.y] = true;
					tempque.push(next_point);
					
					m_parent[next_point.x][next_point.y].x = parent_point.x;
					m_parent[next_point.x][next_point.y].y = parent_point.y;
				}
			}

		}
	}
}

void ::FindPathBFS::GetQueue(COORDINATE end_point)
{
	if(end_point.x != -1 && end_point.y != -1)
	{
		GetQueue(m_parent[end_point.x][end_point.y]);
		m_paths_queue.push(end_point);
	}
}

void FindPathBFS::GetPath(COORDINATE begin_point, COORDINATE end_point)
{
	FindPath(begin_point, end_point);
	GetQueue(end_point);
	
	m_paths_queue.pop(); //弹出无用的起始点
}
