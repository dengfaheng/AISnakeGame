#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <ctime>
#include <conio.h>
#include <Windows.h>

#include "FindPathBFS.h"
using namespace std;

int dir[4][2] = { { 0,1 },{ 0,-1 },{ 1,0 },{ -1,0 } };

#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4
#define HEAD 0

int speed = 0;
//将光标移动到x,y位置
void gotoxy(int x, int y)
{
	COORD c;
	c.X = x; c.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}
//设置颜色 用到两个Windows API  不做详细介绍
void setColor(unsigned short ForeColor = 7, unsigned short BackGroundColor = 0)
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);//获取当前窗口句柄
	SetConsoleTextAttribute(handle, ForeColor + BackGroundColor * 0x10);//设置颜色
}
//游戏设置相关模块，把函数都放到一个类里面了。函数定义为static静态成员，不生成实体也可以直接调用
class GameSetting
{
public:
	//游戏窗口的长宽
	static const int window_height = 40;
	static const int window_width = 80;
public:
	static void GameInit()
	{
		//设置游戏窗口大小
		char buffer[32];
		sprintf_s(buffer, "mode con cols=%d lines=%d",window_width, window_height);
		system(buffer);

		//隐藏光标
		HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_CURSOR_INFO CursorInfo;
		GetConsoleCursorInfo(handle, &CursorInfo);//获取控制台光标信息
		CursorInfo.bVisible = false; //隐藏控制台光标
		SetConsoleCursorInfo(handle, &CursorInfo);//设置控制台光标状态
		//初始化随机数种子
		srand((unsigned int)time(0));
	}
};
//打印信息类，打印相关信息：欢迎，分数，说明，结束等等
class PrintInfo
{
public:
	//选择模式：手动? AI?
	static void DrawChoiceInfo()
	{
		gotoxy(GameSetting::window_width / 2 - 10, GameSetting::window_height / 2 - 5);
		cout << "请选择游戏模式：" << endl;
		gotoxy(GameSetting::window_width / 2 - 10, GameSetting::window_height / 2 - 3);
		cout << "1. 手动操作模式" << endl;
		gotoxy(GameSetting::window_width / 2 - 10, GameSetting::window_height / 2 - 1);
		cout << "2. AI智能模式" << endl;
		gotoxy(GameSetting::window_width / 2 - 10, GameSetting::window_height / 2 + 1);
		cout << "请输入您的选择-> ";
	}
	//画地图边界
	static void DrawMap()
	{
		system("cls");
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
	//游戏结束
	static void GameOver(int score)
	{
		setColor(12, 0);
		gotoxy(GameSetting::window_width / 2 - 20, GameSetting::window_height / 2 - 5);
		cout << "游戏结束，您输了！" << endl;;
		gotoxy(GameSetting::window_width / 2 - 20, GameSetting::window_height / 2 - 3);
		cout << "您的得分为：" << score << endl;
	}
	//画分数
	static void DrawScore(int score)
	{
		gotoxy(GameSetting::window_width - 22+14, 6);
		cout << "  ";
		gotoxy(GameSetting::window_width - 22+14, 4);
		cout << "  ";

		gotoxy(GameSetting::window_width - 22, 6);
		cout << "当前玩家分数: " << score << endl;
		gotoxy(GameSetting::window_width - 22, 4);
		cout << "当前游戏速度: " << 10 - speed / 25 << endl;

	}
	//画游戏操作说明等
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
//食物类，定义食物的生成等相关操作
class Food
{
private:
	//食物坐标
	COORDINATE m_coordinate;
public:
	//坐标范围：
	//x: 1 to GameSetting::window_width - 30 闭区间
	//y: 1 to GameSetting::window_height - 2 闭区间
	void RandomXY(vector<COORDINATE> & coord)
	{
		m_coordinate.x = rand() % (GameSetting::window_width - 30) + 1;
		m_coordinate.y = rand() % (GameSetting::window_height - 2) + 1;
		unsigned int i;
		//原则上不允许食物出现在蛇的位置上，如果有，重新生成
		for (i = 0; i < coord.size(); i++)
		{
			//食物出现在蛇身的位置上。重新生成
			if (coord[i].x == m_coordinate.x && coord[i].y == m_coordinate.y)
			{
				m_coordinate.x = rand() % (GameSetting::window_width - 30) + 1;
				m_coordinate.y = rand() % (GameSetting::window_height - 2) + 1;
				i = 0;
			}
		}
	}
	//默认构造函数
	Food() {}
	//构造函数，传入参数为蛇身坐标
	Food(vector<COORDINATE> & coord)
	{
		RandomXY(coord);
	}
	//画出食物的位置
	void DrawFood()
	{
		setColor(12, 0);
		gotoxy(m_coordinate.x, m_coordinate.y);
		cout << "@";
		setColor(7, 0);
	}
	//接口，获取食物位置
	COORDINATE GetFoodCoordinate()
	{
		return m_coordinate;
	}

};
//贪吃蛇类，定义贪吃蛇的移动，打印，吃食物等等
//地图范围width:2 to width-2  height: 2 to height-2
class Snake
{
private:
	bool m_model; //true人机  false AI
	int m_direction;
	bool m_is_alive;
private: //AI功能相关
	bool m_chess[GameSetting::window_width - 29 + 1][GameSetting::window_height]; //AI功能用
	FindPathBFS m_AISnake;
	COORDINATE map_size;
public://蛇身坐标
	vector<COORDINATE> m_coordinate;

public://默认构造函数
	Snake(bool model = false) : m_model(model) //默认人机模式
	{
		map_size.x = GameSetting::window_width - 29 + 1;
		map_size.y = GameSetting::window_height;
		//移动方向向上
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
	//设置游戏模式
	void set_model(bool m) { m_model = m; }
	//监听键盘
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
				if (speed >= 25)
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
	//AI功能
	void AI_speed()
	{
		char ch;

		if (_kbhit())					//kbhit 非阻塞函数 
		{
			ch = _getch();	//使用 getch 函数获取键盘输入 
			switch (ch)
			{
			case '+':
				if (speed >= 25)
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
	//AI功能
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
	//AI功能
	void AI_move_snake()
	{
		static int cot = 0;
		AI_speed();
		COORDINATE head, temp;
		if (!m_AISnake.m_paths_queue.empty())
		{
			head = m_AISnake.m_paths_queue.front();
			m_AISnake.m_paths_queue.pop();
		}
		else
		{
			//                          下        上     左       右
			//int direction[4][2] = { { 0,1 },{ 0,-1 },{ 1,0 },{ -1,0 } };
			//随机走一步，但是不能碰墙或者碰到自己
			for (int i = 0; i < 4; i++)
			{
				int break_num = rand() % 4;
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
				if(break_num == i)
					break;
			}
		}

		m_coordinate.insert(m_coordinate.begin(), head);
	}
	//移动贪吃蛇
	void move_snake()
	{
		//监听键盘
		listen_key_borad();
		//蛇头
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
		//插入移动后新的蛇头
		m_coordinate.insert(m_coordinate.begin(), head);
	}
	//判断是否吃到食物
	bool is_eat_food(Food & f)
	{
		//获取食物坐标
		COORDINATE food_coordinate = f.GetFoodCoordinate();
		//吃到食物，食物重新生成，不删除蛇尾
		if (m_coordinate[HEAD].x == food_coordinate.x && m_coordinate[HEAD].y == food_coordinate.y)
		{
			f.RandomXY(m_coordinate);
			return true;
		}
		else
		{
			//没有吃到食物，删除蛇尾
			m_coordinate.erase(m_coordinate.end() - 1);
			return false;
		}
	}
	//判断贪吃蛇死了没
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
	//画出贪吃蛇
	void draw_snake()
	{
		//设置颜色为浅绿色
		setColor(10, 0);
		for (unsigned int i = 0; i < this->m_coordinate.size(); i++)
		{
			gotoxy(m_coordinate[i].x, m_coordinate[i].y);
			cout << "*";
		}
		//恢复原来的颜色
		setColor(7, 0);
	}
	//清除屏幕上的贪吃蛇
	void ClearSnake()
	{
		for (unsigned int i = 0; i < m_coordinate.size(); i++)
		{
			m_chess[m_coordinate[i].x][m_coordinate[i].y] = false;
		}
		gotoxy(m_coordinate[this->m_coordinate.size()-1].x, m_coordinate[this->m_coordinate.size() - 1].y);
		cout << " ";

	}
	//获取贪吃蛇的长度
	int GetSnakeSize()
	{
		return m_coordinate.size();
	}
	//获取当前游戏模式
	bool GetModel()
	{
		return m_model;
	}
};

//主函数，组合各种类和资源，进行游戏。
int main()
{
	GameSetting setting;
	PrintInfo print_info;
	Snake  snake;
	//初始化游戏
	setting.GameInit();
	//游戏模式选择
	print_info.DrawChoiceInfo();

	char ch = _getch();
	switch (ch)
	{
	case '1':
		snake.set_model(true);
		speed = 50;
		break;
	case '2':
		snake.set_model(false);
		break;
	default:
		gotoxy(GameSetting::window_width / 2 - 10, GameSetting::window_height / 2 + 3);
		cout << "输入错误，Bye！" << endl;
		cin.get();
		cin.get();
		return 0;
	}
	gotoxy(GameSetting::window_width / 2 - 10, GameSetting::window_height / 2 + 3);
	system("pause");
	//画地图
	print_info.DrawMap();
	print_info.DrawGameInfo(snake.GetModel());
	//生成食物
	Food food(snake.m_coordinate);
	//游戏死循环
	while (true)
	{
		//打印成绩
		print_info.DrawScore(snake.GetSnakeSize());
		//画出食物
		food.DrawFood();
		//清理蛇尾，每次画蛇前必做
		snake.ClearSnake();
		//判断是否吃到食物
		snake.is_eat_food(food);
		//根据用户模式选择不同的调度方式
		if (snake.GetModel() == true)
		{
			snake.move_snake();
		}
		else
		{
			snake.AI_find_path(food);
			snake.AI_move_snake();
		}
		//画蛇
		snake.draw_snake();
		//判断蛇是否还活着
		if (!snake.snake_is_alive())
		{
			print_info.GameOver(snake.GetSnakeSize());
			break;
		}
		//控制游戏速度
		Sleep(speed);
	}

	cin.get();
	cin.get();

	return 0;
}