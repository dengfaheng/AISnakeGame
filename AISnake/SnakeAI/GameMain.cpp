#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <ctime>
#include <conio.h>
#include <Windows.h>

#include "FindPathBFS.h"

using namespace std;

//⦾
int dir[4][2] = { { 0,1 },{ 0,-1 },{ 1,0 },{ -1,0 } };

#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4
#define HEAD 0

int speed = 50;

void gotoxy(int x, int y)
{
	COORD c;
	c.X = x; c.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void setColor(unsigned short ForeColor = 7, unsigned short BackGroundColor = 0)
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);//获取当前窗口句柄
	SetConsoleTextAttribute(handle, ForeColor + BackGroundColor * 0x10);//设置颜色
}

class GameSetting
{
public:
	static const int window_height = 40;
	static const int window_width = 80;
public:
	static void GameInit()
	{
		char buffer[32];
		sprintf_s(buffer, "mode con cols=%d lines=%d",window_width, window_height);
		system(buffer);

		//隐藏光标
		HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_CURSOR_INFO CursorInfo;
		GetConsoleCursorInfo(handle, &CursorInfo);//获取控制台光标信息
		CursorInfo.bVisible = false; //隐藏控制台光标
		SetConsoleCursorInfo(handle, &CursorInfo);//设置控制台光标状态

		srand((unsigned int)time(0));
	}
};

class PrintInfo
{
public:


public:
	static void DrawMap()
	{
		int i, j;
		for (i = 0; i < GameSetting::window_width; i++)
			cout << "#";
		cout << endl;
		for (i = 0; i < GameSetting::window_height-2; i++)
		{
			for (j = 0; j < GameSetting::window_width; j++)
			{
				if (i == 13 && j >= GameSetting::window_width - 29)
				{
					cout << "#";
					continue;
				}

				if (j == 0 || j == GameSetting::window_width - 29 || j == GameSetting::window_width-1)
				{
					cout << "#";
				}
				else
					cout << " ";
				
			}
			cout << endl;
		}
		for (i = 0; i < GameSetting::window_width; i++)
			cout << "#";
		
	}
	static void GameOver()
	{
		gotoxy(GameSetting::window_width / 2 - 15, GameSetting::window_height / 2);
		cout << "游戏结束" << endl;;
	}

	static void DrawScore(int score)
	{
		gotoxy(GameSetting::window_width - 22, 6);
		cout << "当前玩家分数: " << score << endl;
		gotoxy(GameSetting::window_width - 22, 4);
		cout << "当前游戏速度: " << 11 - speed / 25 << endl;
	}

	static void DrawGameInfo(bool model)
	{
		gotoxy(GameSetting::window_width - 22, 8);
		if (model)
		{
			cout << "当前游戏模式: " << "人机" << endl;
		}
		else
		{
			cout << "当前游戏模式: " << "AI" << endl;
		}
		gotoxy(GameSetting::window_width - 22, 10);
		cout << "历史最优分数: " << 100 << endl;

		gotoxy(GameSetting::window_width - 22, 18);
		cout << "游戏操作说明：" << endl;
		gotoxy(GameSetting::window_width - 22, 20);
		cout << "W: 上    S: 下" << endl;
		gotoxy(GameSetting::window_width - 22, 22);
		cout << "A: 左    D: 右" << endl;

		gotoxy(GameSetting::window_width - 22, 24);
		cout << "调节游戏速度：" << endl;
		gotoxy(GameSetting::window_width - 22, 26);
		cout << "小键盘 + : 加快" << endl;
		gotoxy(GameSetting::window_width - 22, 28);
		cout << "小键盘 - : 减慢" << endl;

		gotoxy(GameSetting::window_width - 22, 32);
		cout << "作者: infinitor" << endl;
		gotoxy(GameSetting::window_width - 22, 34);
		cout << "版本: 1.0" << endl;
	}

};

class Food
{
private:
	COORDINATE m_coordinate;

public:
	//坐标范围：
	//x: 1 to GameSetting::window_width - 30 闭区间
	//y: 1 to GameSetting::window_height - 2 闭区间
	void RandomXY()
	{
		m_coordinate.x = rand() % (GameSetting::window_width - 30) + 1;
		m_coordinate.y = rand() % (GameSetting::window_height - 2) + 1;
	}

	Food()
	{
		RandomXY();
	}

	void DrawFood()
	{
		setColor(12, 0);
		gotoxy(m_coordinate.x, m_coordinate.y);
		cout << "@";
		setColor(7, 0);
	}

	COORDINATE GetFoodCoordinate()
	{
		return m_coordinate;
	}

};


//地图范围width:2 to width-2  height: 2 to height-2
class Snake
{
private:
	bool m_model; //true人机  false AI
	vector<COORDINATE> m_coordinate;
	int m_direction;
	bool m_is_alive;
	bool m_chess[GameSetting::window_width - 29 + 1][GameSetting::window_height]; //AI功能用
	FindPathBFS m_AISnake;
	COORDINATE map_size;

public:
	Snake(bool model = true) : m_model(model) //默认人机模式
	{
		map_size.x = GameSetting::window_width - 29 + 1;
		map_size.y = GameSetting::window_height;

		m_direction = 1;
		m_is_alive = true;
		COORDINATE snake_head;
		snake_head.x = GameSetting::window_width / 2 - 15;
		snake_head.y = GameSetting::window_height / 2;

		this->m_coordinate.push_back(snake_head);
		snake_head.y++;
		this->m_coordinate.push_back(snake_head);
		snake_head.y++;
		this->m_coordinate.push_back(snake_head); //初始蛇身长度三节

		//围墙是障碍
		for (int i = 0; i < GameSetting::window_width - 29 + 1; i++)
		{
			m_chess[i][0] = true;
			m_chess[i][GameSetting::window_height - 1] = true;
		}

		for (int j = 0; j < GameSetting::window_height - 1; j++)
		{
			m_chess[0][j] = true;
			m_chess[GameSetting::window_width - 29][j] = true;
		}

	}

	bool GetModel()
	{
		return m_model;
	}

	void listen_key_borad()
	{
		char ch;

		if (_kbhit())					//kbhit 非阻塞函数 
		{
			ch = _getch();	//使用 getch 函数获取键盘输入 
			switch (ch)
			{
			case 'w':
			case 'W':
				if (this->m_direction == DOWN)
					break;
				this->m_direction = UP;
				break;
			case 's':
			case 'S':
				if (this->m_direction == UP)
					break;
				this->m_direction = DOWN;
				break;
			case 'a':
			case 'A':
				if (this->m_direction == RIGHT)
					break;
				this->m_direction = LEFT;
				break;
			case 'd':
			case 'D':
				if (this->m_direction == LEFT)
					break;
				this->m_direction = RIGHT;
				break;
			case '+':
				if (speed > 25)
				{
					speed -= 25;
				}
				break;
			case '-':
				if (speed < 250)
				{
					speed += 25;
				}
				break;
			}
		}
	}

	void AI_speed()
	{
		char ch;

		if (_kbhit())					//kbhit 非阻塞函数 
		{
			ch = _getch();	//使用 getch 函数获取键盘输入 
			switch (ch)
			{
			case '+':
				if (speed > 25)
				{
					speed -= 25;
				}
				break;
			case '-':
				if (speed < 250)
				{
					speed += 25;
				}
				break;
			}
		}
	}

	void AI_find_path(Food &f)
	{
		static int not_found = 1;
		COORDINATE fpoint = f.GetFoodCoordinate();
		
		for (unsigned int i = 0; i < m_coordinate.size(); i++)
		{
			m_chess[m_coordinate[i].x][m_coordinate[i].y] = true; //蛇的身体也是障碍
		}
		COORDINATE begin_point, end_point;
		
		begin_point = m_coordinate[HEAD];
		end_point = fpoint;

		m_AISnake.InitMap((bool**)m_chess);
		m_AISnake.GetPath(begin_point, end_point);

		if (m_AISnake.m_paths_queue.empty())
		{
			gotoxy(GameSetting::window_width - 22, 38);
			cout << "not_found = " << not_found++ << endl;
		}

	}
	//检测是否碰到自己
	bool self_collision(COORDINATE head)
	{
		for (unsigned int i = 1; i < m_coordinate.size(); i++)
		{
			if (head.x == m_coordinate[i].x && head.y == m_coordinate[i].y)
			{
				return true;
			}
		}
		return false;
	}

	void AI_move_snake()
	{
		static int cot = 0;
		AI_speed();
		COORDINATE head, temp;
		if (!m_AISnake.m_paths_queue.empty())
		{
			head = m_AISnake.m_paths_queue.front();
			m_AISnake.m_paths_queue.pop();

			gotoxy(GameSetting::window_width - 22, 36);
			cout << "cot = "<< cot++ << endl;
			
		}
		else
		{
			//                          下        上     左       右
			//int direction[4][2] = { { 0,1 },{ 0,-1 },{ 1,0 },{ -1,0 } };
			//随机走一步，但是不能碰墙或者碰到自己
			while(true)
			{
				int i = rand() % 4;
				temp = m_coordinate[HEAD];
				temp.x = temp.x + dir[i][0];
				temp.y = temp.y + dir[i][1];

				if (temp.x <= 0 ||
					(temp.x >= (map_size.x - 1)) ||
					temp.y <= 0 ||
					(temp.y >= (map_size.y - 1)) ||
					self_collision(temp)
					)  //路径不通不可走
				{
					continue;
				}
				head = temp;
				break;
			}
		}

		m_coordinate.insert(m_coordinate.begin(), head);
	}

	void move_snake()
	{

		listen_key_borad();

		COORDINATE head = m_coordinate[0];

		//direction方向:1 上  2 下  3 左  4 右
		switch (this->m_direction)
		{
		case UP:
			head.y--;
			break;
		case DOWN:
			head.y++;
			break;
		case LEFT:
			head.x--;
			break;
		case RIGHT:
			head.x++;
			break;
		}

		m_coordinate.insert(m_coordinate.begin(), head);
	}

	bool is_eat_food(Food & f)
	{
		//TODO
		COORDINATE food_coordinate = f.GetFoodCoordinate();
		if (m_coordinate[HEAD].x == food_coordinate.x && m_coordinate[HEAD].y == food_coordinate.y)
		{
			f.RandomXY();
			return true;
		}
		else
		{
			m_coordinate.erase(m_coordinate.end() - 1);
			return false;
		}
	}

	bool snake_is_alive()
	{
		if (m_coordinate[HEAD].x <= 0 ||
			m_coordinate[HEAD].x >= GameSetting::window_width - 29 ||
			m_coordinate[HEAD].y <= 0 ||
			m_coordinate[HEAD].y >= GameSetting::window_height - 1)
		{
			//超出边界
			m_is_alive = false;
			return m_is_alive;
		}
		//和自己碰到一起
		for (unsigned int i = 1; i < m_coordinate.size(); i++)
		{
			if (m_coordinate[i].x == m_coordinate[HEAD].x && m_coordinate[i].y == m_coordinate[HEAD].y)
			{
				m_is_alive = false;
				return m_is_alive;
			}
		}
		m_is_alive = true;

		return m_is_alive;
	}

	void draw_snake()
	{
		setColor(10, 0);
		for (unsigned int i = 0; i < this->m_coordinate.size(); i++)
		{
			gotoxy(m_coordinate[i].x, m_coordinate[i].y);
			cout << "*";
		}
		setColor(7, 0);
	}

	void ClearSnake()
	{
		for (unsigned int i = 0; i < m_coordinate.size(); i++)
		{
			m_chess[m_coordinate[i].x][m_coordinate[i].y] = false;
		}
		gotoxy(m_coordinate[this->m_coordinate.size()-1].x, m_coordinate[this->m_coordinate.size() - 1].y);
		cout << " ";

	}

	int GetSnakeSize()
	{
		return m_coordinate.size();
	}


};

int main()
{
	GameSetting setting;
	PrintInfo print_info;
	Snake  snake;

	setting.GameInit();
	print_info.DrawMap();
	print_info.DrawGameInfo(snake.GetModel());

	Food food;
	snake.AI_find_path(food);

	while (true)
	{
		//print_info.ClearScreen();
		print_info.DrawScore(snake.GetSnakeSize());

		food.DrawFood();

		snake.ClearSnake();
		
		//snake.move_snake();
		

		snake.is_eat_food(food);
		
		snake.AI_find_path(food);
		snake.AI_move_snake();
		

		snake.draw_snake();
		

		if (!snake.snake_is_alive())
		{
			print_info.GameOver();
			break;
		}

		
		Sleep(speed);
	}

	

	std::cin.get();
	return 0;
}