////////////////////////////////Libraries///////////////////////////////////////
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
 // #include <sys/sysinfo.h> //this is for window
#include <sys/sysctl.h> // for mac 
#include <unistd.h>
// source c file OS Project
// 睇comment 理解咗啲code 先 
////////////////////////////////////////////////////////////////////////////////
////////////////////////////GLOBAL VARIABLES/STRUCTS////////////////////////////
#define MAX_SIZE 1000
//#define MSize(m) (sizeof(m) / sizeof(m[0])) //Macro for finding matrix row:MSIZE(matrix); and col:MSIZE(matrix[0]);
#define numOfThreads 4
// defining a matrix structure
typedef struct{
    int **data;
    int row;
    int col;
} Matrix;


// defining data structure of stack 
typedef struct {
    char* operation_stack; // our stack to store the +, *, -
    Matrix* matrix_stack[MAX_SIZE]; // our stack to store our matrices
    int operation_stack_top; // the current top element index of our operation stack
    int matrix_stack_top; // the current top element index of our matrix stack
} Stacks; // this structure defined as Stacks
// 所有thread 都會access 到呢個 array of matrix
/***** Shared data part *****/
Matrix* arr_Matrices[MAX_SIZE];

/* Prototype */
Matrix* mat_allocate_memory(int r, int c);
int **arr_Allocate_memory(int r, int c);
void release_memory(Matrix *matrix);
////////////////////////////////////////////////////////////////////////////////
/************* basic function of our stacks ***************/
int get_operation_stack_size(Stacks* stack){
    assert(stack != NULL);
    return stack->operation_stack_top+1;
}

int get_matrix_stack_size(Stacks* stack){
    assert(stack != NULL);
    return stack->matrix_stack_top+1;
}
void init(Stacks* stack){
    assert(stack != NULL);
    stack->matrix_stack_top = -1;
    stack->operation_stack_top = -1;
    stack->operation_stack = (char*)calloc(MAX_SIZE, sizeof(char));
    for(int i=0; i<MAX_SIZE; i++){
        stack->matrix_stack[i] = (Matrix*)malloc(sizeof(Matrix));
    }
}

void push_Op(Stacks* stack, char operator){
    assert(stack != NULL);
    stack->operation_stack_top++;
    int new_Idx = stack->operation_stack_top;
    stack->operation_stack[new_Idx] = operator;
}

void push_Mat(Stacks* stack, Matrix* matrix){
    assert(stack != NULL && matrix!=NULL);
    stack->matrix_stack_top++;
    int new_Idx = stack->matrix_stack_top;
    stack->matrix_stack[new_Idx] = matrix;
}
char pop_Op(Stacks* stack){
    assert(stack!=NULL);
    int top_idx = stack->operation_stack_top;
    char op = stack->operation_stack[top_idx];
    stack->operation_stack_top -= 1;
    stack->operation_stack[top_idx] = ' ';
    return stack->operation_stack[top_idx];
}

Matrix* pop_Mat(Stacks* stack){
    assert(stack != NULL);
    int top_idx = stack->matrix_stack_top;
    Matrix* matrix = stack->matrix_stack[top_idx];
    stack->matrix_stack_top -= 1;
    stack->matrix_stack[top_idx] = NULL; // Set the popped matrix pointer to NULL
    return matrix;
}

int is_matStack_Empty(Stacks* s){
    assert(s != NULL);
    return s->matrix_stack_top==-1;
}

////////////////////////////////FUNCTIONS///////////////////////////////////////
Matrix* matrix_Multiplication(Matrix *A, Matrix *B);


Matrix* matrix_Addition(Matrix *A, Matrix *B){
    assert(A!=NULL && B!=NULL);
    int row = A->row;
    int col = B->col;
    Matrix* C = mat_allocate_memory(row, col);
    for(int i=0;i<row;i++){
        for(int j=0;j<col;j++){
            C->data[i][j] = A->data[i][j] + B->data[i][j];
        }
    }
    return C;
}

Matrix * matrix_Subtraction(Matrix *A, Matrix *B);

void matrix_Print(int** M){
    assert(M!=NULL);
    int row = 2;
    int col = 2;
    for(int i=0;i<row;i++){
        for(int j=0;j<col;j++){
            printf("%d ", M[i][j]);
        }
        printf("\n");
    }
}

void release_memory(Matrix *matrix){
    // this method is for releasing the memory of integer array
    assert(matrix!=NULL);
    free(matrix);
}
int **arr_allocate_memory(int r, int c){
     //assert(r!=0||c!=0);
    // this method allocate memory to integer array
    int **matrix = (int **)calloc(r, sizeof(int*)); // creating a 2d arr with #n of 1-d array inside it
    for(int i=0; i<r; i++) matrix[i] = (int*)calloc(c, sizeof(int)); // for every row of array, we have #m integers inside it
    return matrix;
}
Matrix* mat_allocate_memory(int r, int c){
    assert(r!=0||c!=0);
    // this method allocate memory to integer array
    Matrix *matrix = (Matrix*)malloc(sizeof(Matrix)); // creating a 2d arr with #n of 1-d array inside it
    matrix->data = arr_allocate_memory(r, c);
    matrix->col = c;
    matrix->row = r;
    return matrix;
}

////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////MAIN FUNCTION//////////////////////////////////

int main() {
    
    /***** Below is the input part*****/
    // first input -> A-B / A+B*C+D / A*B+C
    char expression[] = {};
    scanf("%s", expression);
    int noOfMatrix = strlen(expression) / 2 + 1; // cal how many matrix in expression
    // init the stack object
    Stacks *s = (Stacks*)malloc(sizeof(Stacks));
    for(int i=0; i<noOfMatrix; i++){ // A loop to input all the matrices
        // second input -> size of a nxm matrix
        int n, m; // n = row, m = col
        printf("Please input the (r c): ");
        scanf("%d %d", &n , &m);
        /* initialization of our matrix */
        Matrix *mat = mat_allocate_memory(n, m);
        printf("\n");
        // third input -> values of our matrix
        printf("Please input the value for matrix: \n");
        for(int x=0; x<n; x++)
            for(int y=0; y<m; y++)
                scanf("%d", &mat->data[x][y]);
        arr_Matrices[i] = mat_allocate_memory(n, m);
        arr_Matrices[i] = mat;
    }
    // initialize our stack object
    init(s);
    // adding stack and char to our stacks
    for(int i=0; i<noOfMatrix; i++){
        push_Mat(s, arr_Matrices[i]);
    }
    printf("Our stack: \n");
    for(int i=s->matrix_stack_top; i>=0; i--){
        matrix_Print(s->matrix_stack[i]->data);
    }
    // popping all the matrix
    while(!is_matStack_Empty(s)){
        pop_Mat(s);
    }
    
    //test
    // push_Mat(s, matrix_Addition(pop_Mat(arr_Matrices),pop_Mat(arr_Matrices)));
    // matrix_Print(pop_Mat(arr_Matrices));
    /****** Below is the part of multi-threading programming******/
    // it would be more than one thread
    //pthread_t* threads; // We have four threads
    //pthread_attr_t attr; // set of thread attributes
     /* first argument: a pointer to the thread, 
        second argument: properties of the thread
        third argument: function name
        fourth argument: parameter of the function*/
     // pthread_create(&thread1, NULL, /* our matrix function */, our matrix);
    /* wait for the thread to exit */
     // pthread_join(thread1, NULL);
    free(s);
    return 0;
}
////////////////////////////////////////////////////////////////////////////////