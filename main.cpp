#include <iostream>
#include <stdio.h>
#include <thread>
#include <time.h>
#include <string.h>
#include <Windows.h>
#include <vector>
std::wstring Figure[7];
const int screenWidth = 120;
const int screenHeight = 30;
int boardHeight = 18;
int boardWidth = 12;
bool Key[5];

unsigned char* pBoard = nullptr;
bool gameOver = true;

int Rotate(int px, int py, int rot) {
	switch (rot % 4) {
	case 0: return py * 4 + px;
	case 1: return 12 + py - (px * 4);
	case 2: return 15 - (py * 4) - px;
	case 3:return 3 - py + (px * 4);
	}
	return 0;
}


bool isFit(int Figure, int rot, int x, int y) {
	for (int px = 0; px < 4; px++) {
		for (int py = 0; py < 4; py++) {
			int pi = Rotate(px, py, rot);
			int fi = (y + py) * boardWidth + (x + px);

			if (x + px >= 0 && x + px < boardWidth) {
				if (y + py >= 0 && y + py < boardHeight) {
					if (::Figure[Figure][pi] == L'X' && pBoard[fi] != 0) {
						return false;
					}
				}
			}
		}
	}
	return true;
}









int main() {
	srand(time(NULL));
	Figure[0].append(L"..X.");
	Figure[0].append(L"..X.");
	Figure[0].append(L"..X.");
	Figure[0].append(L"..X.");

	Figure[1].append(L"..X.");
	Figure[1].append(L".XX.");
	Figure[1].append(L"..X.");
	Figure[1].append(L"....");

	Figure[2].append(L"....");
	Figure[2].append(L".XX.");
	Figure[2].append(L"..X.");
	Figure[2].append(L"..X.");

	Figure[3].append(L"....");
	Figure[3].append(L".XX.");
	Figure[3].append(L".X..");
	Figure[3].append(L".X..");

	Figure[4].append(L"....");
	Figure[4].append(L".XX.");
	Figure[4].append(L".XX.");
	Figure[4].append(L"....");

	Figure[5].append(L"..X.");
	Figure[5].append(L".XX.");
	Figure[5].append(L".X..");
	Figure[5].append(L"....");

	Figure[6].append(L".X..");
	Figure[6].append(L".XX.");
	Figure[6].append(L"..X.");
	Figure[6].append(L"....");

	int currFigure = rand() % 7;
	int currRot = 0;
	int FigureCount = 0;
	bool rotHold = true;
	int currX = boardWidth / 2;
	int currY = 0;
	int score = 0;
	int Speed = 20;
	int countSpeed = 0;
	bool fall = false;
	std::vector<int> lines;












	pBoard = new unsigned char[boardWidth * boardHeight];
	for (int x = 0; x < boardWidth; x++)
		for (int y = 0; y < boardHeight; y++)
			pBoard[y * boardWidth + x] = (x == 0 || x == boardWidth - 1 || y == boardHeight - 1) ? 9 : 0;


	wchar_t* screen = new wchar_t[screenWidth * screenHeight];
	for (int i = 0; i < screenWidth * screenHeight; i++) screen[i] = L' ';
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0,
		NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	while (gameOver) {
		Sleep(50);
		countSpeed++;
		fall = (countSpeed == Speed);
		for (int k = 0; k < 5; k++)
			Key[k] = (0x8000 & GetAsyncKeyState((unsigned char)("D\S\A\R\Q"[k]))) != 0;

		currX += (Key[0] && isFit(currFigure, currRot, currX + 1, currY)) ? 1 : 0;
		if (Key[1] && isFit(currFigure, currRot, currX, currY + 1)) {
			currY = currY + 1;
		}
		currX -= (Key[2] && isFit(currFigure, currRot, currX - 1, currY)) ? 1 : 0;
		if (Key[3]) {
			currRot += (rotHold && isFit(currFigure, currRot + 1, currX, currY));
			rotHold = false;
		}
		else {
			rotHold = true;
		}
		if (Key[4]) gameOver = false;

		if (fall) {
			countSpeed = 0;
			FigureCount++;
			if (FigureCount % 50 == 0) {
				if (Speed > 10)Speed--;
			}
			if (isFit(currFigure, currRot, currX, currY + 1))currY++;
			else {
				for (int px = 0; px < 4; px++)
					for (int py = 0; py < 4; py++)
						if (Figure[currFigure][Rotate(px, py, currRot)] != L'.')
							pBoard[(currY + py) * boardWidth + (currX + px)] = currFigure + 1;

				for (int py = 0; py < 4; py++) {
					if (currY + py < boardHeight - 1) {
						bool line = true;
						for (int px = 1; px < boardWidth - 1; px++) {
							line &= (pBoard[(currY + py) * boardWidth + px] != 0);
						}
						if (line) {
							for (int px = 1; px < boardWidth - 1; px++) {
								pBoard[(currY + py) * boardWidth + px] = 8;
							}
							lines.push_back(currY + py);
						}
					}
				}
				score += 25;
				if (!lines.empty())score += (1 << lines.size()) * 100;
				currX = boardWidth / 2;
				currY = 0;
				currRot = 0;
				gameOver = !isFit(currFigure, currRot, currX, currY);
				currFigure = rand() % 7;
			}

		}

		if (!lines.empty()) {
			WriteConsoleOutputCharacter(hConsole, screen, screenWidth * screenHeight, { 0,0 }, &dwBytesWritten);
			Sleep(400);
			for (auto& v : lines) {
				for (int px = 1; px < boardWidth - 1; px++) {
					for (int py = v; py > 0; py--) {
						pBoard[py + boardWidth + px] = pBoard[(py - 1) * boardWidth + px];
					}
					pBoard[px] = 0;
				}
			}
			lines.clear();
		}

		for (int x = 0; x < boardWidth; x++)
			for (int y = 0; y < boardHeight; y++)
				screen[(y + 2) * screenWidth + (x + 2)] = L" 1234567-#"[pBoard[y * boardWidth + x]];



		for (int px = 0; px < 4; px++) {
			for (int py = 0; py < 4; py++) {
				if (Figure[currFigure][Rotate(px, py, currRot)] != L'.')
					screen[(currY + py + 2) * screenWidth + (currX + px + 2)] = rand() % 65 + 2;
			}
		}

		WriteConsoleOutputCharacter(hConsole, screen, screenHeight * screenWidth,
			{ 0,0 }, &dwBytesWritten);
	}









	return 0;
}