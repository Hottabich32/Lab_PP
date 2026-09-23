#include <iostream>
#include <Windows.h>
#include <process.h>

const size_t ROW{ 25 };
const size_t COL{ 25 };
const int NTHREAD{ 4 };

DWORD mainID{ GetCurrentThreadId() };

struct INFORM {
	int** matrix;
	size_t row;
	size_t col;
	int sum;
};


void fillMatrix(int** pMatr, size_t pRow, size_t pCol) {
	int value = 1;

	for (int i = 0; i < pRow; ++i) {
		for (int j = 0; j < pCol; ++j) {
			pMatr[i][j] = value;
			value++;
		}
	}
}

int sumEvenFirstDigit(int** pMatr, size_t pRow, size_t pCol) {
	int sum = 0;

	for (int i = 0; i < pRow; ++i) {
		for (int j = 0; j < pCol; ++j) {
			int value = abs(pMatr[i][j]);

			while (value > 10)
				value /= 10;

			if (value % 2 == 0)
				sum += pMatr[i][j];

		}
	}

	return sum;
}

unsigned __stdcall sumWinAPI(void* param) {
	INFORM* inform = (INFORM*)param;
	inform->sum = sumEvenFirstDigit(inform->matrix, inform->row, inform->col);
	DWORD currentID{ GetCurrentThreadId() };
	std::cout << "ID from WinAP:" << currentID << ": " << inform->sum << "\n";
	if(currentID != mainID)
		_endthreadex(0);
	return 0;
}

int summParallel(int** matrix) {
	HANDLE thr[NTHREAD - 1]{};
	INFORM informs[NTHREAD]{};
	int chunk{ ROW / NTHREAD };

	for (int i = 0; i < NTHREAD; ++i) {
		informs[i].matrix = matrix + chunk * i;
		informs[i].row = chunk;
		informs[i].col = COL;
		informs[i].sum = 0;

		if (i < NTHREAD - 1)
			thr[i] = (HANDLE)_beginthreadex(nullptr, 0, &sumWinAPI, &informs[i], 0, nullptr);
		else
			informs[i].row = chunk + ROW % NTHREAD;
	}

	sumWinAPI(informs + NTHREAD - 1);

	int global_sum = informs[NTHREAD - 1].sum;
	for (int i = 0; i < NTHREAD - 1; ++i) {
		WaitForSingleObject(thr[i], INFINITE);
		CloseHandle(thr[i]);
		global_sum += informs[i].sum;
	}

	return global_sum;
}



int main()
{
	std::cout << "mainID: " << mainID << "\n";
	int** matrix = new int* [ROW];

	for (int i = 0; i < ROW; ++i) {
		matrix[i] = new int[COL];
	}

	fillMatrix(matrix, ROW, COL);

	std::cout << "sum nonparallel: " << sumEvenFirstDigit(matrix, ROW, COL) << "\n";
	std::cout << "sum parallel: " << summParallel(matrix) << "\n";

	for (int i = 0; i < ROW; ++i) {
		delete[] matrix[i];
	}

	delete[] matrix;

	return 0;
}