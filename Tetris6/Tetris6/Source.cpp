#include "Header.h"

extern HANDLE hStdOut;

void printBuffer(CHAR_INFO* Buffer, SHORT xP0, SHORT yP0, SHORT xP2, SHORT yP2) // печать буфера
{
	SMALL_RECT writeArea = { xP0, yP0, xP0 + xP2, yP0 + yP2 };// прямоугольник для рисования поля
	WriteConsoleOutput(hStdOut, Buffer, { xP2,yP2 }, { 0,0 }, &writeArea);// переносим блок данных в буфер консоли
	return;
}
int menu(int m1, int m2, int m3)//меню
{
	for (size_t i = 0; i < 840; i++)//запись в буфер старого значения
	{
	}

	char menuOpt[6][18] = { "PLAY", "HIGH SCORES", "EXIT GAME", "RETURN",  "STOP GAME", "EXIT GAME", };

	menuOnTop();

	unsigned char countMenu = 0;//char чтобы отсечь значения выше 255
	
	do
	{		
		SHORT enterKey = GetAsyncKeyState(VK_RETURN);
		SetConsoleTextAttribute(hStdOut, 15);//15 белый на чёрном фоне
		this_thread::sleep_for(chrono::milliseconds(50));
		countMenu -= GetAsyncKeyState(VK_UP);
		countMenu += GetAsyncKeyState(VK_DOWN);
		countMenu %= 3;
		if (enterKey == - 32767) {
			enterKey = 0;
			this_thread::sleep_for(chrono::milliseconds(50));
			return countMenu;
		}
		if (GetAsyncKeyState(VK_ESCAPE)) return 3;


		SetConsoleTextAttribute(hStdOut, 15 * (countMenu != 0) + 224 * (countMenu == 0));
		SetConsoleCursorPosition(hStdOut, { 15, 7 });
		cout << menuOpt[m1];

		SetConsoleTextAttribute(hStdOut, 15 * (countMenu != 1) + 224 * (countMenu == 1));//15 белый на чёрном фоне
		SetConsoleCursorPosition(hStdOut, { 15,9 });
		cout << menuOpt[m2];

		SetConsoleTextAttribute(hStdOut, 15 * (countMenu != 2) + 224 * (countMenu == 2));//15 белый на чёрном фоне
		SetConsoleCursorPosition(hStdOut, { 15,11 });
		cout << menuOpt[m3];

	} while (true);
	this_thread::sleep_for(chrono::milliseconds(50));

}

void addToBuffer(unsigned short* layerMain, unsigned short* layer2, SHORT xA0, SHORT yA0, SHORT x1, SHORT y1, SHORT x2, SHORT y2)//буфер печати поля и фигур
//x0, y0 - первая точка, x1 - делитель z, y1 - взаимное отклонение буфера расчёта и буфера отображения x2, y2 - размер
{
	CHAR_INFO* mainBuffer = new CHAR_INFO[x2 * y2]; // Создаем символьный буфер для печати 

	int c = 0;
	for (int mY = 0; mY < y2; mY++)//y0=0, y2=20
	{
		for (unsigned int z = 2048 / x1; z >= 4; z /= 2, c += 2)
			if ((layerMain[mY + y1] & z) != 0 || (layer2[mY + y1] & z) != 0) {
				mainBuffer[c].Char.UnicodeChar = 7623; mainBuffer[c + 1].Char.UnicodeChar = 93;//заполнение фигурки
				mainBuffer[c].Attributes = 47; mainBuffer[c + 1].Attributes = 32;// цвет фигурок тут
			}
			else {
				mainBuffer[c].Char.UnicodeChar = mainBuffer[c + 1].Char.UnicodeChar = 183 * (c % 4 == 0);//заполнение фона, 183 * (i % 4 == 0)вертикальная линия
				mainBuffer[c].Attributes = mainBuffer[c + 1].Attributes = 1 * (c % 4 == 0); // цвет фона, 1 * (i % 4 == 0) синего цвета
			}
	}
	printBuffer(mainBuffer, xA0, yA0, x2, y2);
	delete[]mainBuffer;
	return;
}
void scoreLevel(unsigned short* tetrNext, int sco, int lev)// вывод правой части
{
	for (size_t i = 0; i < 4; i++)// вывод в окошко следующей фигуры
		tetrNext[i] *= 8;
	SetConsoleTextAttribute(hStdOut, 15);//15 белый на чёрном фоне
	addToBuffer(tetrNext, tetrNext, 28, 1, 64, 0, 8, 4);
	SetConsoleCursorPosition(hStdOut, { 28,11 });//запись счёта
	cout << "Score:";
	SetConsoleCursorPosition(hStdOut, { 28,13 });
	cout << sco;
	SetConsoleCursorPosition(hStdOut, { 28,15 });//запись уровня
	cout << "Level:";
	SetConsoleCursorPosition(hStdOut, { 28,17 });
	cout << lev;
}
void setGameScreen()// игровое поле
{
	int c = 0;
	CHAR_INFO scoreBuffer[840];
	for (int yi = 0; yi < 21; yi++)
	{
		for (int xj = 0; xj < 40; xj++, c++)
			if (yi == 0 || yi == 20 || yi == 7 || xj == 0 || xj == 1 || xj == 22 || xj == 23 || xj == 38 || xj == 39) {
				scoreBuffer[c].Char.UnicodeChar = 9617;
				scoreBuffer[c].Attributes = 127;
			}
			else {
				scoreBuffer[c].Char.UnicodeChar = ' ';
				scoreBuffer[c].Attributes = 0;
			}
	}
	printBuffer(scoreBuffer, 0, 0, 40, 21);
}
void menuOnTop() {// окошко меню
	CHAR_INFO scoreBuffer[200];
	int c = 0;
	for (int yi = 0; yi < 10; yi++)
	{
		for (int xj = 0; xj < 20; xj++, c++) {
			if (yi == 0 || yi == 9) {
				scoreBuffer[c].Char.UnicodeChar = 9472;
				scoreBuffer[c].Attributes = 15;
			}
			else if (xj == 0 || xj == 19) {
				scoreBuffer[c].Char.UnicodeChar = 9474;
				scoreBuffer[c].Attributes = 15;
			}
			else {
				scoreBuffer[c].Char.UnicodeChar = ' ';
				scoreBuffer[c].Attributes = 0;
			}
		}
	}
	scoreBuffer[0].Char.UnicodeChar = 9484;
	scoreBuffer[19].Char.UnicodeChar = 9488;
	scoreBuffer[180].Char.UnicodeChar = 9492;
	scoreBuffer[199].Char.UnicodeChar = 9496;
	printBuffer(scoreBuffer, 10, 5, 20, 10);
}


