#include <iostream>
#include <chrono>
#include "ThreadSafeQueue.h"
#include <string>


const int ROW{ 77 };
const int COL{ 77 };
//const int NTHREAD{ 4 };
const int NPRODUCERS{ 2 };
//const int NCONSUMERS{ NTHREAD - NPRODUCERS };
const int NCONSUMERS{ 4 };
const int CHUNK{ 10 };

ThreadSafeQueue TSQ(3);
CRITICAL_SECTION cs_task;
CRITICAL_SECTION cs_console;
CRITICAL_SECTION cs_row;
int nextRow{};

int takeNextStart() {
	LockGuardWinAPI locker_row(cs_row);
	int start = nextRow;
	nextRow += CHUNK;
	return start;
}

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

void prodecerThread(int** matrix, int id) {
	int start = takeNextStart();

	while (start < ROW) {
		int rows = ROW - start < CHUNK ? ROW - start : CHUNK;

		TSQ.push(Pair{ matrix + start, rows });

		LockGuardWinAPI locker_console(cs_console);
		std::cout << "Producer " << id << ": block " << " (rows " << start << " - " << start + rows - 1 << ")\n";

		start = takeNextStart();
	}
}

void consumerThread(long volatile& global_sum, int id) {
	Pair pair{};
	int local_sum{};

	while (TSQ.try_pop(pair)) {
		local_sum += sumEvenFirstDigit(pair.matrix, pair.row, COL);
		std::this_thread::sleep_for(std::chrono::milliseconds(10));

		LockGuardWinAPI locker_console(cs_console);
		std::cout << "Consumer " << id << " (thread " << std::this_thread::get_id()<< "): local = " << local_sum << "\n";
	}

	LockGuardWinAPI locker_task(cs_task);
	global_sum += local_sum;
}

int sum_parallel(int** matrix) {
	long volatile global_sum{};

	//nextChunk = 0;
	std::thread producers[NPRODUCERS];
	std::thread consumers[NCONSUMERS];

	for (int i = 0; i < NPRODUCERS; ++i) {
		producers[i] = std::thread(prodecerThread, matrix, i);
	}

	for (int i = 0; i < NCONSUMERS; ++i) {
		consumers[i] = std::thread(consumerThread, std::ref(global_sum), i);
	}

	for (int i = 0; i < NPRODUCERS; ++i)
		producers[i].join();
	TSQ.close();

	for (int i = 0; i < NCONSUMERS; ++i)
		consumers[i].join();

	return global_sum;
}


int main()
{
	InitializeCriticalSection(&cs_task);
	InitializeCriticalSection(&cs_console);
	InitializeCriticalSection(&cs_row);

	//std::cout << "mainID: " << std::this_thread::get_id() << "\n";
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
	delete[] matrix;

	DeleteCriticalSection(&cs_task);
	DeleteCriticalSection(&cs_console);
	DeleteCriticalSection(&cs_row);

	return 0;
}