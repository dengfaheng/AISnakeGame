# C++编写的一个AI贪吃蛇

## 总体概况
- 开发环境：VIsual Studio 2017
- 开发语言：C++ 和 少许Windows API
- 运行环境：Windows 10

- 效果如下：

![](http://upload-images.jianshu.io/upload_images/10386940-1e76db60f4bf831a.jpg?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
![](http://upload-images.jianshu.io/upload_images/10386940-f65452c70ec0af2b.jpg?imageMogr2/auto-orient/strip)

## 已知问题

目前这货还不能算成品。基本是蛇每移动一次，就进行一次BFS找路。但是不能保证每一次都能找到一条路，找不到路的时候怎么办呢？我们让蛇随机走几步，走几步以后就有可能找到路了。但是这样做带来的后果就是：可能随机走着走着就走进死胡同了，结果只能GG。

改进的话：我想到一个点就是，BFS找不到路的时候，不随机走。而是有规律跟着尾巴走，比如做S型运动等。想法是这样，代码待写。


## 下载使用

大体情况就是这样，代码是VS2017编译的。大家可以下载后，VS2017的直接打开，其他编译器的，新建一个工程，把.h和.cpp文件拖进去编译即可。

具体的代码详解，会在后续发表相关文章的。也会更新在这里。

## 完善
最后就是想不断完善这个小程序，毕竟麻雀虽小五脏俱全。想把各种算法都实现一遍。

盲目式搜索：
- BFS
- DFS

启发式搜索：
- AStar
- 有序搜索

慢慢完成吧。


# C++编写贪吃蛇小游戏快速入门
刚学完C++。一时兴起，就花几天时间手动做了个贪吃蛇，后来觉得不过瘾，于是又加入了AI功能。希望大家Enjoy It.

# 效果图示

AI模式演示

![](http://upload-images.jianshu.io/upload_images/10386940-1e76db60f4bf831a.jpg?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
![](http://upload-images.jianshu.io/upload_images/10386940-f65452c70ec0af2b.jpg?imageMogr2/auto-orient/strip)

# 整体规划+原理

![](http://upload-images.jianshu.io/upload_images/10386940-f8ed50c8f39668c9.jpg?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

大体上可以分为图上所示的几个类。不过……怎么看都有点强行面向对象的味道在里面。。[哭笑][哭笑][哭笑]。不管了……代码写得可能有点凌乱，下面我会为大家一一讲解。

整个程序设计的原理就是：主函数死循环，不断刷新打印贪吃蛇和食物。这样每循环一次，就类似电影里面的一帧，最终显示的效果就是蛇会动起来。

# 01 初始化工作-游戏设置
游戏设置和相关初始化放在了一个类里面，并进行了静态声明。主要设置了游戏窗口的长和款。并在GameInit()函数里面设置了窗口大小，隐藏光标，初始化随机数种子等。代码如下：
```C++
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
```
用到了几个相关的Windows API，本文不做过多介绍，大家百度即可。

# 02 打印信息类
该类主要是用来打印一些游戏相关信息的。该类大体如下：
![image](http://upload-images.jianshu.io/upload_images/10386940-31ef77c3d6f2da28.jpg?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

下面挑几个重点的来讲：
## 2.1 画地图边界
这个函数主要是根据上面所给的游戏窗口长宽来打印地图边界的。其中还划分了几个区域，主要用来放不同的信息的。
```C++
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
```
划分区域如下图，#就是边框了：
![image](http://upload-images.jianshu.io/upload_images/10386940-4992ee3e118c196f.jpg?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

## 2.2 画出分数和模式

该函数主要是在右上角画出成绩和游戏模式的，在绘制之前会进行刷新处理。先清除，再重新打印。用到了一个gotoxy()函数。这个函数主要是移动光标到(x, y)坐标处的。关于(x, y)的位置，根据实际情况调整即可。

```C++
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
```

# 03 食物类
食物类定义了食物的坐标，随机生成规则，和画出食物等一系列操作。其中食物坐标我们用了一个结构体：
```C++
typedef struct
{
	int x;
	int y;
}COORDINATE;
```
该结构体两个成员，分别保存坐标的(x, y)。蛇身的坐标也会用到这个结构体。
有关食物类的大体如下：
![image](http://upload-images.jianshu.io/upload_images/10386940-79e8cd6597585790.jpg?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

下面我们还是挑几个重点来讲。
## 3.1 随机生成食物
随机生成食物，**原则上不允许食物出现在蛇身的位置上**，如果有。我们重新生成。注意地图的范围，就是区域左边一块。实际情况根据自身的地图范围来调整食物坐标的范围，注意不要越界。用rand()函数获得随机坐标。代码如下：
```C++
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
```
然后，在构造函数里面传入蛇身的坐标。即可生成食物。

## 3.2 画出食物
画出食物比较简单了，gotoxy到随机生成的坐标之后，cout就行。我们在这还设置了一个食物颜色为红色。代码如下：
```C++
void DrawFood()
{
    setColor(12, 0);
    gotoxy(m_coordinate.x, m_coordinate.y);
    cout << "@";
    setColor(7, 0);
}
```
# 04 贪吃蛇类
定义贪吃蛇的移动，打印，吃食物等等。这节课我们暂时不讨论AI功能，先把手动操作的贪吃蛇做了跑起来，下节课再做AI功能的介绍。该类大体如下：
![image](http://upload-images.jianshu.io/upload_images/10386940-1624cf64843452f7.jpg?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
## 4.1 成员变量
成员变量m_direction记录每次移动的方向。m_is_alive记录贪吃蛇是否还活着。m_coordinate则是贪吃蛇身体坐标的记录。贪吃蛇是一节一节的，整条蛇必然是由许多节组成的。因此用了一个vector来存储蛇身，每节类型是COORDINATE结构体的。

## 4.2 默认构造函数
默认构造函数Snake()里面主要是做了初始贪吃蛇的生成，以及移动方向的定义等。初始的蛇为3节。在中间位置，向上移动。代码如下：
```C++
        //移动方向向上
    m_direction = 1;
    m_is_alive = true;
    COORDINATE snake_head;
    //蛇头生成位置
    snake_head.x = GameSetting::window_width / 2 - 15;
    snake_head.y = GameSetting::window_height / 2;

    this->m_coordinate.push_back(snake_head);
    snake_head.y++;
    this->m_coordinate.push_back(snake_head);
    snake_head.y++;
    this->m_coordinate.push_back(snake_head); //初始蛇身长度三节
```
## 4.3 监听键盘
监听键盘用了C里面的一个库函数。_kbhit()非阻塞函数，可以不断监听键盘的情况从而不产生阻塞。有键盘按下的时候，就获取按下的键盘是哪个。然后做出相应的变化，其实是方向的调整。需要注意的是，当我们的蛇往上走的时候，按下方向的键，我们是不做处理的。其它方向一样。还有一个调整游戏速度的，speed是休眠时间，speed越小，速度越快。反之速度越慢。
```C++
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
```
## 4.4 移动贪吃蛇
移动贪吃蛇，我们用了一个方向变量，在监听键盘的时候获取移动的方向，然后在根据方向移动贪吃蛇的蛇头。这里的移动我们是这样处理的，首先，贪吃蛇每移动一次，需要改变的只有蛇头和蛇尾两节。我们只需要把新的蛇头插进去，最后再画出来就可以了。至于蛇尾，如果我们不删除蛇尾的话，蛇会不断变长的。因此我们的做法是：吃到食物的时候插入蛇头而不删除蛇尾，没有吃到食物的时候插入蛇头同时删除蛇尾。这样就完美搞定了。
```C++
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
    //插入移动后新的蛇头。是否删除蛇尾，在后续吃到食物判断那里做
    m_coordinate.insert(m_coordinate.begin(), head);
}
```
## 4.5 是否吃到食物
判断是否吃到食物，就是看看蛇头的坐标等不等于食物的坐标。如果等于，就重新生成食物，不删除蛇尾，蛇变长一节。不等于，就删除蛇尾,蛇长不变。
```C++
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
```
## 4.6判断蛇是否还存活
判断蛇是否GG，主要是看是否超出边界，是否碰到自己身体其他部分。
```C++
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
```
## 4.7 画出贪吃蛇
画出贪吃蛇比较简单，gotoxy到身体的每一节，然后cout就行。在此之前设置了颜色为浅绿色。
```C++
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
```
## 4.8 清除屏幕上的贪吃蛇
我们是死循环不断刷新打印贪吃蛇的，因此每移动一次，必然会在屏幕上留下上一次贪吃蛇的痕迹。因此我们每次在画蛇之前，不是添足，而是清理一下上次遗留的蛇身。我们知道，蛇每次移动，变的只有蛇头和蛇尾，因此该函数我们只需要清理蛇尾就行。gotoxy到蛇尾的坐标，cout<<" ";就行。
```C++
gotoxy(m_coordinate[this->m_coordinate.size()-1].x, m_coordinate[this->m_coordinate.size() - 1].y);
cout << " ";
```

# 05 主函数，组装我们的游戏
我们的游戏在主函数里面进行组装。然后开始运行。
首先我们做游戏相关的初始化和模式选择。
```C++
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
```
然后就是画地图边框，打印游戏相关信息和说明。生成食物了。
```C++
//画地图
print_info.DrawMap();
print_info.DrawGameInfo(snake.GetModel());
//生成食物
Food food(snake.m_coordinate);
```
最后就是游戏死循环，在死循环里面，我们需要不断移动蛇，画蛇，判断蛇的状态，判断食物的状态，是否吃到食物等等。具体代码：
```C++
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
```
最终，我们的代码就可以Run起来了。具体效果放在开头了。界面算不上好看，但是整个程序向大家展示了最基本最核心的功能和代码，大家可以在这个基础上开发自己喜欢的各种美丽的界面哦。

# 06 AI部分和完善
代码是画了几天间间断断写出来的，水平不算很高，代码也写得乱七八糟的。不过代码会在后期不断优化，尽量做到精简优美。至于AI功能，等下一篇博文写吧。

# 代码获取
欲获取代码，请关注我们的微信公众号【程序猿声】，在后台回复：**aisnake**。即可下载。

![微信公众号](http://upload-images.jianshu.io/upload_images/10386940-546ac15b9d7add56.jpg?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

推荐文章：[10分钟教你用Python做个打飞机小游戏超详细教程](https://www.cnblogs.com/infroad/p/9260954.html)

推荐文章：[10分钟教你用python下载和拼接微信好友头像图片](https://www.cnblogs.com/infroad/p/9269158.html)

推荐文章：[10分钟教你用python一行代码搞点大新闻](https://www.cnblogs.com/infroad/p/9275903.html)

推荐文章：[10分钟教你用python打造贪吃蛇超详细教程
](https://www.cnblogs.com/infroad/p/9241267.html)