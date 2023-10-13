#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#define MAX_SIZE 1000

// defining data structure of stack 
typedef struct {
    char operation_stack[MAX_SIZE]; // our stack to store the +, *, -
    int **matrix_stack[MAX_SIZE]; // our stack to store our matrices
    int operation_stack_top; // the current top element index of our operation stack
    int matrix_stack_top; // the current top element index of our matrix stack
} Stacks; // this structure defined as Stacks

/************* basic function of our stacks ***************/
int get_operation_stack_size(Stacks* stack){
    return stack->operation_stack_top+1;
}

int get_matrix_stack_size(Stacks* stack){
    return stack->matrix_stack_top+1;
}
void init(Stacks* stack){
    stack->matrix_stack_top = -1;
    stack->operation_stack_top = -1;
}

void push_Op(Stacks* stack, char operator){
    int new_Idx = stack->operation_stack_top+=1;
    stack->operation_stack[new_Idx] = operator;
}

void push_Mat(Stacks* stack, int **matrix){
    int new_Idx = stack->matrix_stack_top+=1;
    stack->matrix_stack[new_Idx] = matrix;
}
char peek_Op(Stacks* stack){
    int top_idx = stack->operation_stack_top;
    return stack->operation_stack[top_idx];
}

int** peek_Mat(Stacks* stack){
    int top_idx = stack->matrix_stack_top;
    return stack->matrix_stack[top_idx];
}
/***********************************************************/
/***** 
source c file OS Project
睇comment 理解咗啲code 先 

*****/
// 所有thread 都會access 到呢個 array of matrix
int **arr_Matrices[MAX_SIZE];
void matrix_Multiplication();

void matrix_Addition();

void matrix_Subtraction();

int NoOfMatrix(char expr[]){
    int res = 0, len = strlen(expr);
    for(int i=0; i<len; i++){
        if((int)expr[i]>=65&&(int)expr[i]<=90) res++;
    }
    return res;
}
int main() {

    /***** Below is the input part*****/
    // first input -> A-B / A+B*C+D / A*B+C
    char expression[]={}; 
    scanf("%s", expression); 
    int noOfMatrix = NoOfMatrix(expression); // cal how many matrix in expression
    printf("%d", noOfMatrix);
    for(int i=0; i<noOfMatrix; i++){ // A loop to input all the matrices
        // second input -> size of a nxm matrix
        int n, m; // n = row, m = col
        printf("Input the size for matrix, separated by space: ");
        scanf("%d %d", &n , &m);
        /* initialization of our matrix */
        int **matrix = (int **)malloc(n* sizeof(int*)); // creating a 2d arr with #n of 1-d array inside it
        for(int i=0; i<n; i++) matrix[i] = (int*)malloc(m* sizeof(int)); // for every row of array, we have m integers inside it
        printf("\n");
        // third input -> values of our matrix
        printf("Please input the value for matrix: \n");
        for(int i=0; i<n; i++)
            for(int j=0; j<m; j++)
                scanf("%d", &matrix[i][j]);
        arr_Matrices[i] = matrix;
    }
    /****** Below is the part of multi-threading programming******/
    // it would be more than one thread
    pthread_t thread1;
    pthread_attr_t attr; // set of thread attributes
    /* creating the thread */ 
     /* first argument: a pointer to the thread, 
        second argument: properties of the thread
        third argument: function name
        fourth argument: parameter of the function*/
     // pthread_create(&thread1, NULL, /* our matrix function */, our matrix);
    /* wait for the thread to exit */
     // pthread_join(thread1, NULL);
    return 0;
}
