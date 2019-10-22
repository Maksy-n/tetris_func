#pragma once
#include <conio.h>
#include <iostream>
#include <windows.h>
#include <thread>
#include <Conio.h>
using namespace std;



void printBuffer(CHAR_INFO*, SHORT, SHORT, SHORT, SHORT);
int menu(int, int, int);
void setGameScreen();
void addToBuffer(unsigned short*, unsigned short*, SHORT, SHORT, SHORT, SHORT, SHORT, SHORT);
void scoreLevel(unsigned short*, int, int);
void menuOnTop();

void highScore(int);
void scoreFileOpn();
void showScore();
void scoreFileSv();

