#include <iostream>
#include <Windows.h>
#include <omp.h>
#include <string>

const int ROW{ 77 };
const int COL{ 77 };
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

int sum_parallel(int** matrix) {
    omp_set_num_threads(NTHREAD);
    int global_sum{};
    int CHUNK{ 10 };

#pragma omp parallel shared(matrix)
    {
        int local_sum{};

#pragma omp for schedule(dynamic, CHUNK)
        for (int i = 0; i < ROW; ++i) {
		    for (int j = 0; j < COL; ++j) {
			int value = abs(matrix[i][j]);

			while (value >= 10)
				value /= 10;

			if (value % 2 == 0)
				local_sum += matrix[i][j];

		    }
	    }

#pragma omp critical
        std::cout << "thread: " << omp_get_thread_num() << ": " << local_sum << "\n";
#pragma omp atomic
        global_sum += local_sum;
    }


    return global_sum;
}

int main()
{
    int** matrix = new int* [ROW];

    for (int i = 0; i < ROW; ++i) {
		matrix[i] = new int[COL];
	}

	fillMatrix(matrix, ROW, COL);

	std::cout << "sum nonparallel: " << sumEvenFirstDigit(matrix, ROW, COL) << "\n";
	std::cout << "sum parallel: " +  std::to_string(sum_parallel(matrix)) + "\n";

	for (int i = 0; i < ROW; ++i) {
		delete[] matrix[i];
	}

	delete[] matrix;

    return 0;
}