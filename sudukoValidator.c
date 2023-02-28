#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define thread_num 28

// structure used to pass data to threads
typedef struct {
	int row;
	int column;		
} parameters;


int sudoku[9][9] = {
	{5, 3, 4, 6, 7, 8, 9, 1, 2},
	{6, 7, 2, 1, 9, 5, 3, 4, 8},
	{1, 9, 8, 3, 4, 2, 5, 6, 7},
	{8, 5, 9, 7, 6, 1, 4, 2, 3},
	{4, 2, 6, 8, 5, 3, 7, 9, 1},
	{7, 1, 3, 9, 2, 4, 8, 5, 6},
	{9, 6, 1, 5, 3, 7, 2, 8, 4},
	{2, 8, 7, 4, 1, 9, 6, 3, 5},
	{3, 4, 5, 2, 8, 6, 1, 7, 9}
};

int valid[thread_num] = {0};

void *printPuzzle(){
	for (int i = 0; i < 9; i++){
		printf("\n\n");
		for (int j = 0; j < 9; j++){	
			printf(" %d  ", sudoku[i][j]);
		}
	}
	pthread_exit(NULL);
}

void *checkRow(void *param){

	parameters *params = (parameters*) param;
	int row = params->row;
	int col = params->column;		
	if (col != 0 || row > 8){
		fprintf(stderr, "Invalid row or column for row subsection.\nRow:%d, Col:%d\n", row, col);
		pthread_exit(NULL);
	}

	int validityArray[9] = {0};

	for (int i = 0; i < 9; i++){

		int num = sudoku[row][i];
		if (num < 1 || num > 9 || validityArray[num - 1] == 1) {
			pthread_exit(NULL);
		} else {
			validityArray[num - 1] = 1;		
		}
	}

	valid[9 + row] = 1;
	pthread_exit(NULL);
}

void *checkColumn(void* param){

	parameters *params = (parameters*) param;
	int row = params->row;
	int col = params->column;		
	if (row != 0 || col > 8){
		fprintf(stderr, "Invalid row or column for col subsection.\nRow:%d, Col:%d\n", row, col);
		pthread_exit(NULL);
	}

	int validityArray[9] = {0};
	for (int i = 0; i < 9; i++){
		int num = sudoku[i][col];
		if (num < 1 || num > 9 || validityArray[num - 1] == 1) {
			pthread_exit(NULL);
		} else {
			validityArray[num - 1] = 1;		
		}
	}

	valid[18 + col] = 1;
	pthread_exit(NULL);
}

void *checkSubGrid(void* param){

	parameters *params = (parameters*) param;
	int row = params->row;
	int col = params->column;		
	if (row > 6 || row % 3 != 0 || col > 6 || col % 3 != 0){
		fprintf(stderr, "Invalid row or column for subsection.\nRow:%d, Col:%d\n", row, col);
		pthread_exit(NULL);
	}
	int validityArray[9] = {0};

	for (int i = row; i < row + 3; i++){
		for (int j = col; j < col + 3; j++){
			int num = sudoku[i][j];
			if (num < 1 || num > 9 || validityArray[num - 1] == 1){
				pthread_exit(NULL);
			} else {
				validityArray[num - 1] = 1;		
			}
		}
	}

	valid[row + col/3] = 1; 
	pthread_exit(NULL);
}

int main(){	
	pthread_t threads[thread_num];
	int threadIndex = 0;

	printf("\n");
	printf(R"(ⲯ﹍︿﹍ 𝚂𝚞𝚍𝚘𝚔𝚞 𝚅𝚊𝚕𝚒𝚍𝚊𝚝𝚘𝚛 ﹍ⲯ﹍︿﹍☼)");

	pthread_create(&threads[threadIndex++], NULL, printPuzzle, NULL);

	for (int i = 0; i < 9; i++){
		for (int j = 0; j < 9; j++){	
			parameters *params = (parameters *) malloc(sizeof(parameters));					
			params->row = i;		
			params->column = j;

			if (j == 0){
				pthread_create(&threads[threadIndex++], NULL, checkRow, params);
			}

			if (i == 0){
				pthread_create(&threads[threadIndex++], NULL, checkColumn, params);
			}
			
			if (i%3 == 0 && j%3 == 0){
				pthread_create(&threads[threadIndex++], NULL, checkSubGrid, params);
			}
		}
	}

	for (int i = 0; i < thread_num; i++){
		pthread_join(threads[i], NULL);			
	}

	for (int i = 0; i < thread_num; i++){
		if (valid[i] == 0){
			printf("\n\nThe sudoku solution is invalid.\n");
			return EXIT_SUCCESS;
		}
	}
	printf("\n\nThe sudoku solution is valid.\n");
	return EXIT_SUCCESS;
}