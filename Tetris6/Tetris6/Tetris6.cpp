#include <conio.h>
#include <iostream>
#include <windows.h>
#include <thread>
#include "Header.h"

using namespace std;

HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);;

constexpr int NMain = 23;//оно само сделало такое

int tetris();
bool collapse(unsigned long long, unsigned long long);
unsigned short keyPad(unsigned short&, int, int&, unsigned long long&, unsigned int&, bool&);
unsigned int delLine(unsigned short*);
unsigned long long tetraminoFunc(int, int);
void addTeram(unsigned short*, unsigned short*, unsigned short, unsigned short, int);
void printBuffer(CHAR_INFO*, SHORT, SHORT, SHORT, SHORT);
int menu(int, int, int);
void setGameScreen();

int main()
{
	SetConsoleTitle(L"Tetris");
	SMALL_RECT a = { 0, 0, 39, 20 };//устанавливаю размер окна вывода	
	SetConsoleWindowInfo(hStdOut, TRUE, &a);

	// с эти не разобрался, вставил как есть со своими значениями 
	CONSOLE_FONT_INFOEX cfon;
	ZeroMemory(&cfon, sizeof(CONSOLE_FONT_INFOEX));
	cfon.cbSize = sizeof(CONSOLE_FONT_INFOEX);
	cfon.nFont = 6;
	cfon.dwFontSize.X = 18;
	cfon.dwFontSize.Y = 18;
	cfon.FontFamily = FF_DONTCARE;
	cfon.FontWeight = 400;
	lstrcpyW(cfon.FaceName, L"Consolas");
	SetCurrentConsoleFontEx(hStdOut, false, &cfon);

	bool noExit = true;
	do
	{
		this_thread::sleep_for(chrono::milliseconds(200));

		switch (menu(0, 1, 2))//основное меню
		{
		case 0:
			highScore(tetris());
			break;
		case 1:
			showScore();
			this_thread::sleep_for(chrono::milliseconds(100));
			break;
		case 2:
			noExit = false;
			break;
		case 3:
			noExit = false;
			break;
		default:
			break;
		}
	} while (noExit);
	return 0;
}

int tetris()//функция игры
{
	union GAME
	{
		unsigned long long big;
		unsigned short part[4];
	};
	int score = 0;
	int level = 0;
	int randTetr = 0, randNext = rand() % 7, tetrAn = 0;// фигура
	unsigned short yMain = 0;
	unsigned long long tetrTmp = 0;
	unsigned int sleepTime = 105;
	unsigned short plGr[NMain] = { 0 };
	bool exitToMain = false;

	for (int i = 0; i < NMain - 1; i++)//формирование двоичного стакана
		plGr[i] = 4098;
	plGr[22] = 4095;//формирование днища	

	setGameScreen();// печать основного экрана

	GAME tetram; // основная фигура
	GAME tetramNext;//следующая
	GAME gameY; // поле проверки

	do//основной цикл игры
	{
		randTetr = randNext;//текущая фигура
		randNext = rand() % 7; //следующая фигура
		tetrAn = 0;//начальное положение фигуры -угол поворота
		tetram.big = tetraminoFunc(randTetr, tetrAn);//вызов фигуры
		tetramNext.big = tetraminoFunc(randNext, tetrAn);
		level = score / 1000 + 1;// размер уровня 1000 очков +1 - начинаем с первого

		sleepTime = 101 - (level * 10) * (level <= 10) - 100 * (level > 10);//скорость игры время задержки подобранное 

		scoreLevel(tetramNext.part, score, level);//печать элементов////

		for (int j = 0; j < 4; j++)//запись стакана в слой проверки
			gameY.part[j] = plGr[j + 0];

		unsigned short x = 32, xTmp = 32;// начальное положене фигуры

		for (yMain = 0; yMain < NMain; yMain++)//цикл опускания фигуры
		{
			for (size_t i = 0; i < 6; i++) //колличество нажатий на кнопки за одну иттерацию
			{
				xTmp = x;//временная х на случай наползания слева-справа
				tetrTmp = tetram.big;// временная тертамина на лучай неудачного поворота

				if (collapse(gameY.big, tetram.big * keyPad(x, randTetr, tetrAn, tetram.big, sleepTime, exitToMain))) {//проверки на столкновения плюс клафиши
					x = xTmp;
					tetram.big = tetrTmp;
				}
				this_thread::sleep_for(chrono::milliseconds(sleepTime));

				if (exitToMain) return score; // окончание и выход из игры в меню

				unsigned short figLay2[NMain + 4] = { 0 };//запись фигуры на второй слой
				for (size_t i = 0; i < 4; i++)
					figLay2[i + yMain] = tetram.part[i] * x;

				addToBuffer(plGr, figLay2, 2, 0, 1, 2, 20, 20);//печать
			}
			for (size_t i = 0; i < 4; i++)//проверка остановки фигуры, изменение слоя для проверки следующего ряда //gameY.big = (gameY.big << 16) + plGr[y + 4]; //- не работает хз почему
				gameY.part[i] = plGr[yMain + i + 1];

			if (collapse(gameY.big, tetram.big * x)) // выход если наложение
				break;
		}
		addTeram(plGr, tetram.part, x, yMain, randTetr);//запись фигуры в стакан

		score += delLine(plGr);//удаление линии если необходимо плюс запись очков

	} while (yMain);
	return score;
}

bool collapse(unsigned long long  aVal, unsigned long long  bVal)//проверка на столкновения
{
	return !(aVal & bVal) == 0;
}

unsigned short keyPad(unsigned short& xMove, int rndTetr, int& an, unsigned long long& tetrambig, unsigned int& sleep, bool& extToMain)// управление
{
	xMove <<= 1 * (GetAsyncKeyState(VK_LEFT) == -32767); //надо подбирать, у меня лучше работает если ==1
	xMove >>= 1 * (GetAsyncKeyState(VK_RIGHT) == -32767);
	if (GetAsyncKeyState(VK_UP) == -32767) {//поворот фигуры
		an = (an + 1) % 4;
		tetrambig = tetraminoFunc(rndTetr, an);
	}
	if (GetAsyncKeyState(VK_DOWN) == -32767) {//сброс фигуры = устранение задержки до следующей фигуры
		sleep = 1;
	}
	if (GetAsyncKeyState(VK_ESCAPE)) {// меню в игре
		this_thread::sleep_for(chrono::milliseconds(50));
		switch (menu(3, 4, 5))
		{
		case 0://возврат в игру
			break;
		case 1://в гавное меню
			extToMain = true;
			break;
		case 2://выход ваще
			exit(0);
		case 3://возврат в игру
			 
			break;
		}
		setGameScreen();
	}
	return xMove;
}

void addTeram(unsigned short* plGr1, unsigned short* tetram1, unsigned short xAdd, unsigned short yAdd, int numTetr)//запись фиугры в стакан
{
	for (int i = 0; i < 4; i++)
	{
		plGr1[yAdd + i] = plGr1[yAdd + i] + (tetram1[i] * xAdd);
	}
}

unsigned int delLine(unsigned short* plGrDel)//очистка линии
{
	bool delLine = false;
	int scoreTmp = 0;
	do
	{
		scoreTmp = scoreTmp + scoreTmp * delLine + 100 * delLine;
		delLine = false;
		for (int i = NMain - 2; i > 3; i--)
		{
			delLine |= (plGrDel[i] == 8190);//или
			plGrDel[i] = plGrDel[i - delLine];
		}
	} while (delLine);
	return scoreTmp;// счёт
}

unsigned long long tetraminoFunc(int a1, int a2)//фигурки
{
	unsigned long long tetramino[7][4] = { 1688875630067712, 1688875630067712, 1688875630067712, 1688875630067712,
											   562958543486978, 64424509440, 562958543486978, 64424509440,
											   562962838388736, 844450699935744, 562962838388736, 844450699935744,
											   281487861743616, 1688862745165824, 281487861743616, 1688862745165824,
											   1125929971613696, 281479271874560, 1970329131941888, 1688867040395264,
											   281505041481728, 844429225164800, 1970342016843776, 1125917087105024,
											   562980018192384, 562975723356160, 30064902144, 562962838454272 };
	return tetramino[a1][a2];
}