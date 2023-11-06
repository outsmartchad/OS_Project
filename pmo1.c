////////////////////////////////Libraries///////////////////////////////////////
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
//#include <sys/sysinfo.h> this is for window
//#include <sys/sysctl.h> // for mac 
#include <unistd.h>
// source c file OS Project
// 睇comment 理解咗啲code 先 
////////////////////////////////////////////////////////////////////////////////
////////////////////////////GLOBAL VARIABLES/STRUCTS////////////////////////////

#define MAX_SIZE 1000
#define MSIZE(m) (sizeof(m) / sizeof(m[0])) //Macro for finding matrix row:MSIZE(matrix); and col:MSIZE(matrix[0]);
#define numOfThreads 4
// 所有thread 都會access 到呢個 array of matrix
/***** Shared data part *****/
int **arr_Matrices[MAX_SIZE];
// defining data structure of stack 
typedef struct {
    char* operation_stack; // our stack to store the +, *, -
    int** matrix_stack[MAX_SIZE]; // our stack to store our matrices
    int operation_stack_top; // the current top element index of our operation stack
    int matrix_stack_top; // the current top element index of our matrix stack
    int** size_stack[MAX_SIZE]; // stack to store row and column value "[2][2]"
    int size_stack_top; // top element of size stack
} Stacks; // this structure defined as Stacks

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

int get_size_stack_size(Stacks* stack){
    assert(stack != NULL);
    return stack->size_stack_top+1;
}

void init(Stacks* stack){
    assert(stack != NULL);
    stack->matrix_stack_top = -1;
    stack->operation_stack_top = -1;
    stack->size_stack_top = -1;
    stack->operation_stack = (char*)calloc(MAX_SIZE, sizeof(char));
    for(int i=0; i<MAX_SIZE; i++){
        stack->matrix_stack[i] = NULL;
        stack->size_stack[i] = NULL;
    }
}

void push_Op(Stacks* stack, char operator){
    assert(stack != NULL);
    stack->operation_stack_top++;
    int new_Idx = stack->operation_stack_top;
    stack->operation_stack[new_Idx] = operator;
}

void push_Mat(Stacks* stack, int **matrix){
    assert(stack != NULL && matrix!=NULL);
    stack->matrix_stack_top++;
    int new_Idx = stack->matrix_stack_top;
    stack->matrix_stack[new_Idx] = matrix;
}

void push_Si(Stacks* stack, int **size){
    assert(stack != NULL && size!=NULL);
    stack->size_stack_top++;
    int new_Idx = stack->size_stack_top;
    stack->size_stack[new_Idx] = size;
}

char pop_Op(Stacks* stack){
    assert(stack!=NULL);
    int top_idx = stack->operation_stack_top;
    char op = stack->operation_stack[top_idx];
    stack->operation_stack_top -= 1;
    stack->operation_stack[top_idx] = ' ';
    return stack->operation_stack[top_idx];
}

int** pop_Si(Stacks* stack){
    assert(stack != NULL);
    int top_idx = stack->size_stack_top;
    int** size = stack->size_stack[top_idx];
    stack->size_stack_top -= 1;
    stack->size_stack[top_idx] = NULL;
    return size;
}

int** pop_Mat(Stacks* stack){
    assert(stack != NULL);
    int top_idx = stack->matrix_stack_top;
    int** matrix = stack->matrix_stack[top_idx];
    stack->matrix_stack_top -= 1;
    stack->matrix_stack[top_idx] = NULL; // Set the popped matrix pointer to NULL
    return matrix;
}

int is_matStack_Empty(Stacks* s){
    assert(s != NULL);
    return s->matrix_stack_top==-1;
}


////////////////////////////////FUNCTIONS///////////////////////////////////////
void release_memory(int **matrix){
    // this method is for releasing the memory of integer array
    assert(matrix!=NULL);
    free(matrix);
}

int** allocate_memory(int r, int c){
    //assert(r!=0||c!=0);
    // this method allocate memory to integer array
    int **matrix = (int **)calloc(r, sizeof(int*)); // creating a 2d arr with #n of 1-d array inside it
    for(int i=0; i<r; i++) matrix[i] = (int*)calloc(c, sizeof(int)); // for every row of array, we have #m integers inside it
    return matrix;
}

int** matrix_Multiplication(int **A, int **B);


int** matrix_Addition(int** A, int** B, int** Asize, int** Bsize){
    assert(A!=NULL && B!=NULL);
    int row = Asize[0][0]; 
    int col = Asize[0][1];
    int **C = allocate_memory(row, col);
    for (int i=0;i<row;i++){
        for(int j=0;j<col;j++){
            C[i][j] = A[i][j] + B[i][j];
        }
    }
    return C;
}

int** matrix_Subtraction(int **A, int **B);

void matrix_Print(int** M, int** size){
    assert(M!=NULL);
    int row = size[0][0]; 
    int col = size[0][1];
    for(int i=0;i<row;i++){
        for(int j=0;j<col;j++){
            printf("%d ", M[i][j]);
        }
        printf("\n");
    }
}
////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////Threads FUNCTION///////////////////////////////

void *threadFunction1( void *arg )
{
    int passValue = *(int *)(arg);
    printf("Thread %d is running!",passValue);
    return NULL;
}




////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////MAIN FUNCTION//////////////////////////////////

int main() {
    // pthread variable
    pthread_t * threads;

    /***** Below is the input part*****/
    // first input -> A-B / A+B*C+D / A*B+C
    char expression[] = {};
    printf("%s", "Enter expression: "); 
    scanf("%s", expression);
    printf("\n");
    int noOfMatrix = strlen(expression) / 2 + 1; // cal how many matrix in expression
    Stacks *s = (Stacks*)malloc(sizeof(Stacks));
    // initialize our stack object
    init(s);
    for(int i=0; i<noOfMatrix; i++){ // A loop to input all the matrices
        // second input -> size of a nxm matrix
        int n, m; // n = row, m = col
        printf("Please input the (r c): \n");
        scanf("%d %d", &n , &m);
        /* initialization of our matrix */
        int **mat = allocate_memory(n, m);
        // third input -> values of our matrix
        printf("Please input the value for matrix: \n");
        for(int x=0; x<n; x++)
            for(int y=0; y<m; y++)
                scanf("%d", &mat[x][y]);
        arr_Matrices[i] = allocate_memory(n, m);
        arr_Matrices[i] = mat;

        // adding stack and char to our stacks
        int** size = allocate_memory(MAX_SIZE,2);
        size[i][0] = n;
        size[i][1] = m;
        push_Si(s, size);
        push_Mat(s, arr_Matrices[i]); 
    }
    
    /*test
    int** A = pop_Mat(s);
    int** Asize = pop_Si(s);
    int** B = pop_Mat(s);
    int** Bsize = pop_Si(s);
    int** C = matrix_Addition(A,B,Asize,Bsize);
    int** Csize = Asize;
    push_Mat(s, C);
    push_Mat(s, Csize);
    matrix_Print(pop_Mat(s), pop_Si(s));
    */
    printf("Our stack: \n");
    for(int i=s->matrix_stack_top; i>=0; i--){
        matrix_Print(s->matrix_stack[i], s->size_stack[i]); // PROBLEM: printing only the first matrix of A+B 
    }
    // popping all the matrix
    while(!is_matStack_Empty(s)){
        pop_Mat(s);
    }
    /*
    for(int i = 0;i< numOfThreads;i++){
        int *passValue;
        passValue = (int *) malloc( sizeof(int) );
        *passValue = i;
        //pthread_create( &threads[i], NULL, threadFunction1, (void *)passValue );
    }
    for (int i = 0; i < numOfThreads; i++ ) {
        pthread_join( threads[i], NULL );
    }
    */
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