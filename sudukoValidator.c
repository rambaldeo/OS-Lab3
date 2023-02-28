// Operating Systems Lab 3
// CRN: 74026 - Group 10
// Instructor: Ghadeer Abdelkader
// Group Members:
// Darren Constantine - 100782701
// Md Fardin Rahman - 100704517
// Ram Baldeo Baldeo - 100701013

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// number of threads is defined (9 for rows, 9 for columns, 9 for subgrids, 1 for printing puzzle)
#define thread_num 28

// structure used to pass data to threads
typedef struct {
	int row;
	int column;		
} parameters;

// sudoku solution under consideration
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

// keeps track of the number of threads that return a valid state
int valid[thread_num] = {0};

// prints the sudoku solution
void *printPuzzle(){
	for (int i = 0; i < 9; i++){
		if(i!=0 && i%3==0)
			printf("\n-----------------------------\n");
	
		else if(i==0)
			printf("\n\n");
		
		else
		printf("\n         |         |         \n");

		for (int j = 0; j < 9; j++){	
			if(j!=0 && j%3==0){
				printf("|");
			}
			printf(" %d ", sudoku[i][j]);
		}
	}
	printf("\n");	
	pthread_exit(NULL);
}

// checks a given row
void *checkRow(void *param){
	// struct is initialized from arguments
	parameters *params = (parameters*) param;
	// row and column data are retrieved
	int row = params->row;
	int col = params->column;		
	// checks if the column pointer is at 0 (left-most) and that row argument is valid for reading purposes
	if (col != 0 || row > 8){
		printf("Invalid column pointer index or row inspection argument.\nRow:%d, Col:%d\n", row, col);
		pthread_exit(NULL);
	}

	// array initialized which will store information on whether numbers 1-9 exist in subproblem
	int num_exists[9] = {0};

	// loops through the column indeces of the row and verifies whether the row adheres to the rules
	for (int i = 0; i < 9; i++){
		int num = sudoku[row][i];	
		// checks if entry was invalid
		if (num < 1 || num > 9){
			printf("\nInvalid entry. (Row:%d, Col:%d)\n", row, i);
			pthread_exit(NULL);
		}
		// checks if the number already exists within the row
		else if(num_exists[num - 1] == 1) {
			printf("\nRepeated value found in row. (Row:%d)\n", row);
			pthread_exit(NULL);
		}
		// sets the value of num_exists array at index num-1 to 1 -> needed for succeeding loop iterations along the row
		else {
			num_exists[num - 1] = 1;		
		}
	}

	// correctness of the row is validated
	valid[9 + row] = 1;
	pthread_exit(NULL);
}

// checks a given column
void *checkColumn(void* param){
	// struct is initialized from arguments
	parameters *params = (parameters*) param;
	// row and column data are retrieved
	int row = params->row;
	int col = params->column;		
	// checks if the row pointer is at 0 (top-most) and that column argument is valid for reading purposes
	if (row != 0 || col > 8){
		printf("\nInvalid row pointer index or column inspection argument.\nRow:%d, Col:%d\n", row, col);
		pthread_exit(NULL);
	}
	
	// array initialized which will store information on whether numbers 1-9 exist in subproblem
	int num_exists[9] = {0};
	// loops through the row indeces of the column and verifies whether the column adheres to the rules
	for (int i = 0; i < 9; i++){
		int num = sudoku[i][col];
		// checks if entry was invalid
		if (num < 1 || num > 9){
			printf("\nInvalid entry. (Row:%d, Col:%d)\n", i, col);
			pthread_exit(NULL);
		} 
		// checks if the number already exists within the column
		else if(num_exists[num - 1] == 1) {
			printf("\nRepeated value found in column. (Col:%d)\n", col);
			pthread_exit(NULL);
		} 
		// sets the value of num_exists array at index num-1 to 1 -> needed for succeeding loop iterations along the column
		else {
			num_exists[num - 1] = 1;		
		}
	}

	// correctness of the column is validated
	valid[18 + col] = 1;
	pthread_exit(NULL);
}

// checks a given subgrid
void *checkSubGrid(void* param){
	// struct is initialized from arguments
	parameters *params = (parameters*) param;
	// row and column data are retrieved
	int row = params->row;
	int col = params->column;


	if (row > 6 || row % 3 != 0 || col > 6 || col % 3 != 0){
		printf("\nInvalid row or column argument.\nRow:%d, Col:%d\n", row, col);
		pthread_exit(NULL);
	}

	// array initialized which will store information on whether numbers 1-9 exist in subproblem
	int num_exists[9] = {0};

	// loops through the indeces of the subgrid and verifies whether it adheres to the rules
	for (int i = row; i < row + 3; i++){
		for (int j = col; j < col + 3; j++){
			int num = sudoku[i][j];
			// checks if entry was invalid
			if (num < 1 || num > 9 ){
				printf("\nInvalid entry. (Row:%d, Col:%d)\n", row, col);
				pthread_exit(NULL);
			} 
			// checks if the number already exists within the subgrid			
			else if (num_exists[num - 1] == 1){
				printf("\nRepeated value found in subgrid. (Row:%d, Col:%d)\n", row, col);
				pthread_exit(NULL);
			} 
			// sets the value of num_exists array at index num-1 to 1 -> needed for succeeding loop iterations through the subgrid
			else {
				num_exists[num - 1] = 1;		
			}
		}
	}

	// correctness of the subgrid is validated
	valid[row + col/3] = 1; 
	pthread_exit(NULL);
}

// main function
int main(){	
	// array of posix threads are initialized (size 28)
	pthread_t threads[thread_num];
	// counter iniatlized with 0
	int threadIndex = 0;

	printf("\n");
	printf(R"(ⲯ﹍︿﹍𝚂𝚞𝚍𝚘𝚔𝚞 𝚅𝚊𝚕𝚒𝚍𝚊𝚝𝚘𝚛﹍ⲯ﹍☼)");

	// thread created to print the puzzle
	pthread_create(&threads[threadIndex++], NULL, printPuzzle, NULL);
	// all other threads must wait for thread[0] to finish execution
	pthread_join(threads[0], NULL);	

	// loops through columns and rows
	for (int i = 0; i < 9; i++){
		for (int j = 0; j < 9; j++){	
			// memeory is allocated for struct of type 'parameters'
			parameters *params = (parameters *) malloc(sizeof(parameters));					
			// sets the row variable of parameter struct to i, and column to j
			params->row = i;		
			params->column = j;
			
			// if the column pointer is at the left-most of grid, pthread is created to check row i
			if (j == 0){
				pthread_create(&threads[threadIndex++], NULL, checkRow, params);
			}
			// if the row pointer is at the top-most of grid, pthread is created to check column j
			if (i == 0){
				pthread_create(&threads[threadIndex++], NULL, checkColumn, params);
			}
			// if the column and row pointer are divisible by 0 (occurs at top-left of a sub-grid), pthread is created to check subgrid(i,j)
			if (i%3 == 0 && j%3 == 0){
				pthread_create(&threads[threadIndex++], NULL, checkSubGrid, params);
			}
		}
	}
	// joins all the seperate threads
	for (int i = 0; i < thread_num; i++){
		pthread_join(threads[i], NULL);			
	}

	// checks if any of the threads were not validated by looping through 'valid array'
	// If so, the sudoku solution is invalid, and corresponding message is printed
	// (thread_num-1 because the first thread for puzzle printing did not need to be validated)
	for (int i = 0; i < thread_num-1; i++){
		if (valid[i] == 0){
			printf("\n\nThe sudoku solution is invalid.\n");
			return EXIT_SUCCESS;
		}
	}
	// displays corresponding message if sudoku solution is valid
	printf("\n\nThe sudoku solution is valid.\n");
	return EXIT_SUCCESS;
}