#include <iostream>
#include <thread>

const size_t ROW{ 25 };
const size_t COL{ 25 };
const int NTHREAD{ 4 };

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

void sumThread(int** matrix, size_t row, int& result) {
	result = sumEvenFirstDigit(matrix, row, COL);
	std::cout << "ID from C++ API:" << std::this_thread::get_id() << ": " << result << "\n";
}

int summParallel(int** matrix) {
	std::thread thr[NTHREAD - 1]{};
	int sum_threads[NTHREAD - 1]{};
	int chunk{ ROW / NTHREAD };

	for (int i = 0; i < NTHREAD - 1; ++i)
		thr[i] = std::thread(sumThread, matrix + chunk * i, chunk, std::ref(sum_threads[i]));
	
	int global_sum{};
	sumThread(matrix + chunk * (NTHREAD - 1), chunk + ROW % NTHREAD, global_sum);

	for (int i = 0; i < NTHREAD - 1; ++i) {
		thr[i].join();
		global_sum += sum_threads[i];
	}

	return global_sum;
}



int main()
{
	std::cout << "mainID: " << std::this_thread::get_id() << "\n";
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