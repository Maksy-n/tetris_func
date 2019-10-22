#include "Header.h"
extern HANDLE hStdOut;

struct HighSc
{
	char name[4];
	int hScore;
};
HighSc tabScore[5]; //таблица результатов

void highScore(int scrH) //новый высший результат
{
	scoreFileOpn();
	this_thread::sleep_for(chrono::milliseconds(500));
	for (SHORT i = 0; i < 5; i++)
	{
		
		if (tabScore[i].hScore < scrH)//перезапись результата
		{
			for (SHORT j = 4; j > i; j--)
			{
				strcpy_s(tabScore[j].name, 4, tabScore[j - 1].name);//2->3
				tabScore[j].hScore = tabScore[j - 1].hScore;
			}

			menuOnTop();

			SetConsoleCursorPosition(hStdOut, { 13, 6 });
			cout << "NEW HIGH SCORE";
			SetConsoleCursorPosition(hStdOut, { 13, 7 });
			cout << "ENTER YOUR NAME";
			this_thread::sleep_for(chrono::milliseconds(100));
			cin.ignore();
			SetConsoleCursorPosition(hStdOut, { 18, 9 });

			cin.getline(tabScore[i].name, 4);
			tabScore[i].hScore = scrH;
			scoreFileSv();
			this_thread::sleep_for(chrono::milliseconds(100));
			break;
		}
	}
	return;
}

void scoreFileOpn()// чтение файла результата
{
	FILE* pF1;
	
	fopen_s(&pF1, "score.txt", "r+b");
	if (pF1 == nullptr) { // если файла нет создаю новый
		fopen_s(&pF1, "score.txt", "wb");
		
	}

	fread(tabScore, sizeof(HighSc), 5, pF1);
	fclose(pF1);
	return;
}

void scoreFileSv()// сохранение файла результата
{
	FILE* pF1;

	fopen_s(&pF1, "score.txt", "r+b");

	if (pF1 == nullptr) {
		SetConsoleCursorPosition(hStdOut, { 13, 7 });
		cout << "Error\n" << endl;
	}

	fwrite(tabScore, sizeof(HighSc), 5, pF1);
	fclose(pF1);
	return;
}

void showScore() // отображение счёта
{
	scoreFileOpn();
	menuOnTop();
	for (SHORT i = 0; i < 8; i++) //забиваю меню пробелами
	{
		SetConsoleCursorPosition(hStdOut, { 11, 6 + i });
		cout << "                  ";
	}

	SetConsoleCursorPosition(hStdOut, { 15, 6 });//печать резкльтатов
	cout << "HIGH SCORES";
	SetConsoleCursorPosition(hStdOut, { 13, 7 });
	cout << "______________";
	for (SHORT i = 0; i < 5; i++)
	{
		SetConsoleCursorPosition(hStdOut, { 14, 8 + i });
		cout << tabScore[i].name << ".....";
		cout << tabScore[i].hScore;
	}
	system("pause>>0");
	this_thread::sleep_for(chrono::milliseconds(500));
	return;
}

