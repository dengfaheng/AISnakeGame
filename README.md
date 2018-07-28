# AISnake
C++写的一个AI贪吃蛇
```C++
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
```
