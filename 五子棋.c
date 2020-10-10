#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#define WIDTH	26
#define HIGHT	26

HANDLE handle_in;				//������
HANDLE handle_out;				//������
DWORD mode;						//����̨����ģʽ
INPUT_RECORD mouseRec;			//�¼�����
DWORD res;						//Ҫ��ȡ���¼�
COORD crPos;					//�������λ��

typedef struct item {
	int x;
	int y;
} Position;

typedef struct play {
	Position position;			//�������λ��
	int color;					//���������ɫ
} Player;

Player player[2];				//�������
bool setFlag;					//���ñ��
bool gameState;					//��Ϸ״̬
bool bout;						//�غϽ������
int playing;					//��ǰ�غ����
int Canvas[HIGHT][WIDTH] = {0};	//������ά����

char keydown();					//��ȡ����
void loading();					//���ػ���
void startMenu();				//��ʼҳ��˵�
void setMenu();					//���ò˵�
void initialized();				//��ʼ������
void show();					//��ʾ����
void updateWithoutInput();		//���û������޹صĸ���
void updateWithInput();			//���û������йصĸ���

bool detectionGameState(Player p);		//�����Ϸ״̬
void readRecordFile();			//��ȡ��Ϸ�����ļ��浵
void writeRecordFile();			//�洢��Ϸ�����ļ��浵

HANDLE getInputHandle();		//��ȡ����̨������
HANDLE getOutputHandle();		//��ȡ����̨������
void gotoxy(int x, int y);		//���ù��λ��
void setcolor(int color);		//�����ı���ɫ
void hideCursor();				//���ع��



int main(void)
{
	handle_in = GetStdHandle(STD_INPUT_HANDLE);			//��ȡ����̨������
	handle_out = GetStdHandle(STD_OUTPUT_HANDLE);		//��ȡ����̨������
	
	//����ʹ��system()����֮ǰ�Ŀ���̨����ģʽ����Ϊsystem()��ReadConsoleInput()������ͻ������Ϊʲô�����
	GetConsoleMode(handle_in, &mode);					
	hideCursor();										//���ع��
	
	startMenu();												//��ʼ�˵�
	SetConsoleMode(handle_in, mode);					//�ص�ʹ��system()����֮ǰ�Ŀ���̨����ģʽ
	while(gameState)
	{
		show();											//��ʾ����
		updateWithoutInput();							//���û������޹صĸ���
		updateWithInput();								//���û������йصĸ���
		if (setFlag)
			setMenu();
		Sleep(50);
	}
	
	return 0;
}

void startMenu()						//��ʼҳ��˵�
{
	system("title ������");
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
	printf("\t\t  *         ��ʼ��Ϸ         *\n");
	printf("\t\t  *         ��ȡ��Ϸ         *\n");
	printf("\t\t  *         �˳���Ϸ         *\n");
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

void loading()							//���ػ���
{	
	system("cls");
	int load[15] = {0};
	
	for (int m = 0; m < 15; m++)
	{
		load[m] = 1;
		gotoxy(15, 15);
		printf("������...");
		gotoxy(15, 16);
		for (int n = 0; n < 15; n++)
		{
			if (load[n] == 0)
				printf("��");
			else if (load[n] == 1)
				printf("��");
		}
		Sleep(80);
	}
	gotoxy(50, 30);
	printf("�����������...");
	getch();
	system("cls");
	system("color E0");
}

void initialized()						//��ʼ������
{
	//��ʼ������
	for (int i = 0; i < HIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			Canvas[i][j] = 0;
		}
	}
		//��ʼ�������Ľ�
	Canvas[0][0] = 1;
	Canvas[0][WIDTH-1] = 3;
	Canvas[HIGHT-1][0] = 7;
	Canvas[HIGHT-1][WIDTH-1] = 9;

		//��ʼ�������ıߣ����Ľ�֮�⣩
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
	//��ʼ�������Ϣ
	//���1P������
    player[0].position.x = 0;
	player[0].position.y = -1;
	player[0].color = 5;
	//���2P������
	player[1].position.x = 0;
	player[1].position.y = -1;
	player[1].color = -5;
}

void show()								//��ʾ����
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
					case  5:	printf("��");	break;
					case -5:	printf("��");	break;
					case  1:	printf("��");	break;
					case  2:	printf("��");	break;
					case  3:	printf("��");	break;
					case  4:	printf("��");	break;
					case  0:	printf("��");	break;
					case  6:	printf("��");	break;
					case  7:	printf("��");	break;
					case  8:	printf("��");	break;
					case  9:	printf("��");	break;
				}
				
			}
			printf("%d\n", i);
		}
		for (int k = 0; k < WIDTH; k++)
			printf(" %c", k + 'A');
	}
	bout = false;
	gotoxy(0, 29);
	printf("\n ��ǰ�غϣ�P%d", playing + 1);
	if (playing % 2)
		printf("��");
	else
		printf("��");
	
}

void updateWithoutInput()				//���û������޹صĸ���
{
	if(detectionGameState(player[(playing+1)%2]))
	{
		gotoxy(WIDTH/2,HIGHT/2);
		printf("��ϲ���P%dӮ����Ϸʤ����", (playing+1)%2+1);
		gotoxy(WIDTH/2,HIGHT/2+1);
		printf("�Ƿ����¿�ʼ��Ϸ��y/n����");
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
				printf("��������˳�");
				gameState = false;
				break;
		}
	}
}

void updateWithInput()					//���û������йصĸ���
{
	ReadConsoleInput(handle_in, &mouseRec, 1, &res);
	if (mouseRec.EventType == MOUSE_EVENT)
	{
		crPos = mouseRec.Event.MouseEvent.dwMousePosition;
		if (mouseRec.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
		{
			if (crPos.X < WIDTH*2 && crPos.Y < HIGHT)		//�ж�����Ƿ���������
			{
				if(abs(Canvas[crPos.Y][crPos.X/2]) != 5)	//�ж����λ���ܷ�����
				{
					player[playing].position.x = crPos.X/2;
					player[playing].position.y = crPos.Y;
					Canvas[player[playing].position.y][player[playing].position.x] = player[playing].color;
					bout = true;		//�غϽ���
					playing++;
					if (playing == 2)
						playing = 0;
				}
				else
				{
					gotoxy(0,30);
					printf("��ǰλ���������ӣ��������ӣ����ƶ�������");
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

void setMenu()							//����Ҽ��������ò˵�
{
	gotoxy(WIDTH/2, HIGHT/2-3);
	printf("====================");
	gotoxy(WIDTH/2, HIGHT/2-2);
	printf("|     ������Ϸ     |");
	gotoxy(WIDTH/2, HIGHT/2-1);
	printf("|     ������Ϸ     |");
	gotoxy(WIDTH/2, HIGHT/2);
	printf("|     ��ȡ��Ϸ     |");
	gotoxy(WIDTH/2, HIGHT/2+1);
	printf("|     �˳���Ϸ     |");
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
		case HIGHT/2-2:					//������Ϸ
			setFlag = false;
			break;
		case HIGHT/2-1:					//������Ϸ
			writeRecordFile();
			break;
		case HIGHT/2:					//��ȡ��Ϸ
			readRecordFile();
			break;
		case HIGHT/2+1:					//�˳���Ϸ
			system("cls");
			exit(0);
			break;
	}
}

char keydown()							//��ȡ����
{
	char input;
	if (_kbhit())
	{
		input = _getch();
	}
	return tolower(input);
	
}

bool detectionGameState(Player p)		//�����Ϸ״̬
{
	//��ǰ�ж��ٸ���������һ��
	int count;
	Position temp;
	//��⵱ǰ���Ӻ��Ƿ�ʤ��
	//������
	count = 1;
	temp = p.position;
	while (temp.x-1 >= 0)	//�������
	{
		temp.x--;
		if (Canvas[temp.y][temp.x] != p.color)
			break;
		count++;
		if(count == 5)
			return true;
	}
	temp = p.position;
	while (temp.x+1 < WIDTH)	//���Ҳ���
	{
		temp.x++;
		if (Canvas[temp.y][temp.x] != p.color)
			break;
		count++;
		if(count == 5)
			return true;
	}
	
	//������
	count = 1;
	temp = p.position;
	while (temp.y-1 >= 0)	//���ϲ���
	{
		temp.y--;
		if (Canvas[temp.y][temp.x] != p.color)
			break;
		count++;
		if(count == 5)
			return true;
	}
	temp = p.position;
	while (temp.y+1 < HIGHT)	//���²���
	{
		temp.y++;
		if (Canvas[temp.y][temp.x] != p.color)
			break;
		count++;
		if(count == 5)
			return true;
	}
	
	//45��б����
	count = 1;
	temp = p.position;
	while (temp.y-1 >= 0 && temp.x+1 < WIDTH)	//б���ϲ���
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
	while (temp.y+1 < HIGHT && temp.x-1 >= 0)	//б���²���
	{
		temp.y++;
		temp.x--;
		if (Canvas[temp.y][temp.x] != p.color)
			break;
		count++;
		if(count == 5)
			return true;
	}
	
	//135��б����
	count = 1;
	temp = p.position;
	while (temp.y-1 >= 0 && temp.x-1 >= 0)	//б���ϲ���
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
	while (temp.y+1 < HIGHT && temp.x+1 < WIDTH)	//б���²���
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

void readRecordFile()					//��ȡ��Ϸ�����ļ��浵
{
	FILE *fp;
	fp = fopen(".\\gameData.dat", "r");
	if (fp == NULL)
	{
		printf("û�д浵��");
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

void writeRecordFile()					//�洢��Ϸ�����ļ��浵
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
* ����windows API
*/

HANDLE getOutputHandle()				//��ȡ����̨������
{
	HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
	return handle_out;
}

HANDLE getInputHandle()					//��ȡ����̨������
{
	HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
	return handle_in;
}

void gotoxy(int x, int y)				//���ù��λ��
{
	COORD coord;
	coord.X = x;
	coord.Y = y;

	SetConsoleCursorPosition(getOutputHandle(), coord);
}

void setcolor(int color)				//�����ı���ɫ
{
	SetConsoleTextAttribute(getOutputHandle(), color);
}

void hideCursor()						//���ع��
{
	CONSOLE_CURSOR_INFO cursor_info = {1, 0};
	SetConsoleCursorInfo(getOutputHandle(), &cursor_info);
}

