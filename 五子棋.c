#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#define WIDTH	26
#define HIGHT	26

HANDLE handle_in;				//输入句柄
HANDLE handle_out;				//输出句柄
DWORD mode;						//控制台输入模式
INPUT_RECORD mouseRec;			//事件变量
DWORD res;						//要读取的事件
COORD crPos;					//光标所在位置

typedef struct item {
	int x;
	int y;
} Position;

typedef struct play {
	Position position;			//玩家落子位置
	int color;					//玩家棋子颜色
} Player;

Player player[2];				//玩家数组
bool setFlag;					//设置标记
bool gameState;					//游戏状态
bool bout;						//回合结束标记
int playing;					//当前回合玩家
int Canvas[HIGHT][WIDTH] = {0};	//画布二维数组

char keydown();					//获取按键
void loading();					//加载画面
void startMenu();				//开始页面菜单
void setMenu();					//设置菜单
void initialized();				//初始化数据
void show();					//显示画面
void updateWithoutInput();		//与用户输入无关的更新
void updateWithInput();			//与用户输入有关的更新

bool detectionGameState(Player p);		//检测游戏状态
void readRecordFile();			//读取游戏数据文件存档
void writeRecordFile();			//存储游戏数据文件存档

HANDLE getInputHandle();		//获取控制台输入句柄
HANDLE getOutputHandle();		//获取控制台输出句柄
void gotoxy(int x, int y);		//设置光标位置
void setcolor(int color);		//设置文本颜色
void hideCursor();				//隐藏光标



int main(void)
{
	handle_in = GetStdHandle(STD_INPUT_HANDLE);			//获取控制台输入句柄
	handle_out = GetStdHandle(STD_OUTPUT_HANDLE);		//获取控制台输出句柄
	
	//保存使用system()函数之前的控制台输入模式，因为system()和ReadConsoleInput()函数冲突，具体为什么不清楚
	GetConsoleMode(handle_in, &mode);					
	hideCursor();										//隐藏光标
	
	startMenu();												//开始菜单
	SetConsoleMode(handle_in, mode);					//回到使用system()函数之前的控制台输入模式
	while(gameState)
	{
		show();											//显示画面
		updateWithoutInput();							//与用户输入无关的更新
		updateWithInput();								//与用户输入有关的更新
		if (setFlag)
			setMenu();
		Sleep(50);
	}
	
	return 0;
}

void startMenu()						//开始页面菜单
{
	system("title 五子棋");
	system("mode con cols=66 lines=33");
	printf("\n\n"
		
		"\t       _==/          i     i          \\==_		\n"
		"\t     /XX/            |\\___/|            \\XX\\	\n"
		"\t   /XXXX\\            |XXXXX|            /XXXX\\	\n"
		"\t  |XXXXXX\\_         _XXXXXXX_         _/XXXXXX|	\n"
		"\t XXXXXXXXXXXxxxxxxxXXXXXXXXXXXxxxxxxxXXXXXXXXXXX	\n"
		"\t|XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX|	\n"
		"\tXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX	\n"
		"\t|XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX|	\n"
		"\t XXXXXX/^^^^\"\\XXXXXXXXXXXXXXXXXXXXX/^^^^^\\XXXXXX\n"
		"\t  |XXX|       \\XXX/^^\\XXXXX/^^\\XXX/       |XXX|	\n"
		"\t    \\XX\\       \\X/    \\XXX/    \\X/       /XX/	\n"
		"\t       \"\\       \"      \\X/      \"      /\"	\n"
	);
	printf("\t\t  ****************************\n");
	printf("\t\t  *         开始游戏         *\n");
	printf("\t\t  *         读取游戏         *\n");
	printf("\t\t  *         退出游戏         *\n");
	printf("\t\t  ****************************\n");
	
	int choise = 15;
	gotoxy(24,choise);
	printf("==>");
	char ch;
	while ( (ch = keydown()) != '\r')
	{
		gotoxy(24,choise);
		printf("   ");
		
		if (ch == 'w')
			choise--;
		else if (ch == 's')
			choise++;
		
		if (choise == 14)
			choise = 17;
		else if (choise == 18)
			choise = 15;
		
		gotoxy(24,choise);
		printf("==>");
		Sleep(100);
	}
	
	switch (choise)
	{
		case 15:
			loading();
			initialized();
			break;
		case 16:
			loading();
			readRecordFile();
			break;
		case 17:
			system("cls");
			exit(0);
			break;
	}

}

void loading()							//加载画面
{	
	system("cls");
	int load[15] = {0};
	
	for (int m = 0; m < 15; m++)
	{
		load[m] = 1;
		gotoxy(15, 15);
		printf("加载中...");
		gotoxy(15, 16);
		for (int n = 0; n < 15; n++)
		{
			if (load[n] == 0)
				printf("□");
			else if (load[n] == 1)
				printf("■");
		}
		Sleep(80);
	}
	gotoxy(50, 30);
	printf("按任意键继续...");
	getch();
	system("cls");
	system("color E0");
}

void initialized()						//初始化数据
{
	//初始化棋盘
	for (int i = 0; i < HIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			Canvas[i][j] = 0;
		}
	}
		//初始化棋盘四角
	Canvas[0][0] = 1;
	Canvas[0][WIDTH-1] = 3;
	Canvas[HIGHT-1][0] = 7;
	Canvas[HIGHT-1][WIDTH-1] = 9;

		//初始化棋盘四边（除四角之外）
	for (int m = 1; m < HIGHT - 1; m++)
	{
			Canvas[m][0] = 4;
			Canvas[m][WIDTH-1] = 6;
	}
	for (int n = 1; n < WIDTH - 1; n++)
	{
			Canvas[0][n] = 2;
			Canvas[HIGHT-1][n] = 8;
	}
	
	gameState = true;
	setFlag = false;
	bout = true;
	playing = 0;
	//初始化玩家信息
	//玩家1P：白棋
    player[0].position.x = 0;
	player[0].position.y = -1;
	player[0].color = 5;
	//玩家2P：黑棋
	player[1].position.x = 0;
	player[1].position.y = -1;
	player[1].color = -5;
}

void show()								//显示画面
{
	if (bout)
	{
		gotoxy(0,0);
		for (int i = 0; i < HIGHT; i++)
		{
			for (int j = 0; j < WIDTH; j++)
			{
				switch(Canvas[i][j])
				{	
					case  5:	printf("○");	break;
					case -5:	printf("●");	break;
					case  1:	printf("┌");	break;
					case  2:	printf("┬");	break;
					case  3:	printf("┐");	break;
					case  4:	printf("├");	break;
					case  0:	printf("┼");	break;
					case  6:	printf("┤");	break;
					case  7:	printf("└");	break;
					case  8:	printf("┴");	break;
					case  9:	printf("┘");	break;
				}
				
			}
			printf("%d\n", i);
		}
		for (int k = 0; k < WIDTH; k++)
			printf(" %c", k + 'A');
	}
	bout = false;
	gotoxy(0, 29);
	printf("\n 当前回合：P%d", playing + 1);
	if (playing % 2)
		printf("●");
	else
		printf("○");
	
}

void updateWithoutInput()				//与用户输入无关的更新
{
	if(detectionGameState(player[(playing+1)%2]))
	{
		gotoxy(WIDTH/2,HIGHT/2);
		printf("恭喜玩家P%d赢得游戏胜利！", (playing+1)%2+1);
		gotoxy(WIDTH/2,HIGHT/2+1);
		printf("是否重新开始游戏（y/n）：");
		switch(getch())
		{
			case 'y':
			case 'Y':
				initialized();
				break;
			case 'n':
			case 'N':
				gameState = false;
				break;
			default:
				printf("输入错误，退出");
				gameState = false;
				break;
		}
	}
}

void updateWithInput()					//与用户输入有关的更新
{
	ReadConsoleInput(handle_in, &mouseRec, 1, &res);
	if (mouseRec.EventType == MOUSE_EVENT)
	{
		crPos = mouseRec.Event.MouseEvent.dwMousePosition;
		if (mouseRec.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
		{
			if (crPos.X < WIDTH*2 && crPos.Y < HIGHT)		//判断鼠标是否在棋盘内
			{
				if(abs(Canvas[crPos.Y][crPos.X/2]) != 5)	//判断鼠标位置能否落子
				{
					player[playing].position.x = crPos.X/2;
					player[playing].position.y = crPos.Y;
					Canvas[player[playing].position.y][player[playing].position.x] = player[playing].color;
					bout = true;		//回合结束
					playing++;
					if (playing == 2)
						playing = 0;
				}
				else
				{
					gotoxy(0,30);
					printf("当前位置已有棋子，不能落子，请移动！！！");
					Sleep(500);
					gotoxy(0,30);
					printf("                                        ");
				}
			}
		}
		else if (mouseRec.Event.MouseEvent.dwButtonState == RIGHTMOST_BUTTON_PRESSED)
			setFlag = true;
		gotoxy(0,29);
		printf("[Cursor Position] X: %2d Y: %2d", crPos.X, crPos.Y);
	}
}

void setMenu()							//鼠标右键呼出设置菜单
{
	gotoxy(WIDTH/2, HIGHT/2-3);
	printf("====================");
	gotoxy(WIDTH/2, HIGHT/2-2);
	printf("|     继续游戏     |");
	gotoxy(WIDTH/2, HIGHT/2-1);
	printf("|     保存游戏     |");
	gotoxy(WIDTH/2, HIGHT/2);
	printf("|     读取游戏     |");
	gotoxy(WIDTH/2, HIGHT/2+1);
	printf("|     退出游戏     |");
	gotoxy(WIDTH/2, HIGHT/2+2);
	printf("====================");
	
	int choise = HIGHT/2-2;
	gotoxy(WIDTH/2 +2, choise);
	printf("==>");
	char ch;
	while ( (ch = keydown()) != '\r')
	{
		gotoxy(WIDTH/2 +2,choise);
		printf("   ");
		
		if (ch == 'w')
			choise--;
		else if (ch == 's')
			choise++;
		
		if (choise == HIGHT/2-3)
			choise = HIGHT/2+1;
		else if (choise == HIGHT/2+2)
			choise = HIGHT/2-2;
		
		gotoxy(WIDTH/2 +2,choise);
		printf("==>");
		Sleep(100);
	}
	bout = true;
	switch (choise)
	{
		case HIGHT/2-2:					//继续游戏
			setFlag = false;
			break;
		case HIGHT/2-1:					//保存游戏
			writeRecordFile();
			break;
		case HIGHT/2:					//读取游戏
			readRecordFile();
			break;
		case HIGHT/2+1:					//退出游戏
			system("cls");
			exit(0);
			break;
	}
}

char keydown()							//获取按键
{
	char input;
	if (_kbhit())
	{
		input = _getch();
	}
	return tolower(input);
	
}

bool detectionGameState(Player p)		//检测游戏状态
{
	//当前有多少个棋子连成一线
	int count;
	Position temp;
	//检测当前落子后是否胜利
	//横向检测
	count = 1;
	temp = p.position;
	while (temp.x-1 >= 0)	//向左查找
	{
		temp.x--;
		if (Canvas[temp.y][temp.x] != p.color)
			break;
		count++;
		if(count == 5)
			return true;
	}
	temp = p.position;
	while (temp.x+1 < WIDTH)	//向右查找
	{
		temp.x++;
		if (Canvas[temp.y][temp.x] != p.color)
			break;
		count++;
		if(count == 5)
			return true;
	}
	
	//纵向检测
	count = 1;
	temp = p.position;
	while (temp.y-1 >= 0)	//向上查找
	{
		temp.y--;
		if (Canvas[temp.y][temp.x] != p.color)
			break;
		count++;
		if(count == 5)
			return true;
	}
	temp = p.position;
	while (temp.y+1 < HIGHT)	//向下查找
	{
		temp.y++;
		if (Canvas[temp.y][temp.x] != p.color)
			break;
		count++;
		if(count == 5)
			return true;
	}
	
	//45度斜向检测
	count = 1;
	temp = p.position;
	while (temp.y-1 >= 0 && temp.x+1 < WIDTH)	//斜向上查找
	{
		temp.y--;
		temp.x++;
		if (Canvas[temp.y][temp.x] != p.color)
			break;
		count++;
		if(count == 5)
			return true;
	}
	temp = p.position;
	while (temp.y+1 < HIGHT && temp.x-1 >= 0)	//斜向下查找
	{
		temp.y++;
		temp.x--;
		if (Canvas[temp.y][temp.x] != p.color)
			break;
		count++;
		if(count == 5)
			return true;
	}
	
	//135度斜向检测
	count = 1;
	temp = p.position;
	while (temp.y-1 >= 0 && temp.x-1 >= 0)	//斜向上查找
	{
		temp.y--;
		temp.x--;
		if (Canvas[temp.y][temp.x] != p.color)
			break;
		count++;
		if(count == 5)
			return true;
	}
	temp = p.position;
	while (temp.y+1 < HIGHT && temp.x+1 < WIDTH)	//斜向下查找
	{
		temp.y++;
		temp.x++;
		if (Canvas[temp.y][temp.x] != p.color)
			break;
		count++;
		if(count == 5)
			return true;
	}
	
	
	return false;
}

void readRecordFile()					//读取游戏数据文件存档
{
	FILE *fp;
	fp = fopen(".\\gameData.dat", "r");
	if (fp == NULL)
	{
		printf("没有存档！");
		_getch();
		exit(0);
	}
	for (int i = 0; i < HIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			fscanf(fp, "%d ", &Canvas[j][i]);
		}
	}	
	fscanf(fp, "%d %d %d %d %d %d %d", &playing, 
		&player[0].position.x, &player[0].position.y, &player[0].color,
		&player[1].position.x, &player[1].position.y, &player[1].color
		);
	fclose(fp);
	
	gameState = true;
	setFlag = false;
	bout = true;
}

void writeRecordFile()					//存储游戏数据文件存档
{
	FILE *fp;
	fp = fopen(".\\gameData.dat", "w");
	for (int i = 0; i < HIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			fprintf(fp, "%d ", Canvas[j][i]);
		}
	}
	fprintf(fp, "%d %d %d %d %d %d %d", playing, 
		player[0].position.x, player[0].position.y, player[0].color,
		player[1].position.x, player[1].position.y, player[1].color
		);
	fclose(fp);
}


/**************************************************
* 调用windows API
*/

HANDLE getOutputHandle()				//获取控制台输出句柄
{
	HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
	return handle_out;
}

HANDLE getInputHandle()					//获取控制台输入句柄
{
	HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
	return handle_in;
}

void gotoxy(int x, int y)				//设置光标位置
{
	COORD coord;
	coord.X = x;
	coord.Y = y;

	SetConsoleCursorPosition(getOutputHandle(), coord);
}

void setcolor(int color)				//设置文本颜色
{
	SetConsoleTextAttribute(getOutputHandle(), color);
}

void hideCursor()						//隐藏光标
{
	CONSOLE_CURSOR_INFO cursor_info = {1, 0};
	SetConsoleCursorInfo(getOutputHandle(), &cursor_info);
}

