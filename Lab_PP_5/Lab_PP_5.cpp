#include <iostream>
#include <chrono>
#include "ThreadSafeQueue.h"
#include <string>


const size_t ROW{ 77 };
const size_t COL{ 77 };
const int NTHREAD{ 4 };

ThreadSafeQueue TSQ;
CRITICAL_SECTION cs_task;
CRITICAL_SECTION cs_console;


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

			while (value >= 10)
				value /= 10;

			if (value % 2 == 0)
				sum += pMatr[i][j];

		}
	}

	return sum;
}

void sumThread(long volatile& global_sum) {
	Pair pair{};
	int local_sum{};

	while (TSQ.try_pop(pair)) {
		//local_sum += sumEvenFirstDigit(pair.matrix, pair.row, COL);
		for (int i = 0; i < pair.row; ++i) {
			for (int j = 0; j < COL; ++j) {
				int value = abs(pair.matrix[i][j]);

				while (value >= 10)
					value /= 10;

				if (value % 2 == 0)
					local_sum += pair.matrix[i][j];
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
		}

		{
			LockGuardWinAPI locker_console(cs_console);
			std::cout << "ID: " << std::this_thread::get_id() << ": " << local_sum << "\n";
		}
	}

	{
		LockGuardWinAPI locker_task(cs_task);
		global_sum += local_sum;
	}
}

int sum_parallel(int** matrix) {
	int chunk{ 10 }, row{};
	int** beg{ matrix }, ** end{ matrix + ROW };
	while (beg < end) {
		row = beg + chunk < end ? chunk : ROW % chunk;
		TSQ.push(Pair{ beg, row });
		beg += chunk;
	}

	TSQ.finish();

	std::thread thr[NTHREAD - 1];
	long volatile global_sum{};
	for (int i = 0; i < NTHREAD - 1; ++i)
		thr[i] = std::thread(sumThread, std::ref(global_sum));
	sumThread(global_sum);

	for (int i = 0; i < NTHREAD - 1; ++i)
		thr[i].join();

	return global_sum;
}


int main()
{
	InitializeCriticalSection(&cs_task);
	InitializeCriticalSection(&cs_console);

	std::cout << "mainID: " << std::this_thread::get_id() << "\n";
	int** matrix = new int* [ROW];

	for (int i = 0; i < ROW; ++i) {
		matrix[i] = new int[COL];
	}

	fillMatrix(matrix, ROW, COL);

	std::cout << "sum nonparallel: " << sumEvenFirstDigit(matrix, ROW, COL) << "\n";
	std::cout << "sum parallel: " + std::to_string(sum_parallel(matrix)) + "\n";

	for (int i = 0; i < ROW; ++i) {
		delete[] matrix[i];
	}

	DeleteCriticalSection(&cs_task);
	DeleteCriticalSection(&cs_console);
	delete[] matrix;

	return 0;
}