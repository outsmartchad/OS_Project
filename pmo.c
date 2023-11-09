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
//#include <sys/sysctl.h> // for mac 
#include <unistd.h>
// source c file OS Project
// 睇comment 理解咗啲code 先 
////////////////////////////////////////////////////////////////////////////////
////////////////////////////GLOBAL VARIABLES/STRUCTS////////////////////////////
#define MAX_SIZE 1000
#define EXP_MAX_SIZE 20 //10 matrixs + 9 operands
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
int get_operation_stack_size(Stacks* stack){ // not used can delete?
    assert(stack != NULL);
    return stack->operation_stack_top+1;
}

int get_matrix_stack_size(Stacks* stack){ // not used can delete?
    assert(stack != NULL);
    return stack->matrix_stack_top+1;
}
void init(Stacks* stack){ // initilize stack
    assert(stack != NULL);
    stack->matrix_stack_top = -1;
    stack->operation_stack_top = -1;
    stack->operation_stack = (char*)calloc(EXP_MAX_SIZE, sizeof(char));
    for(int i=0; i<MAX_SIZE; i++){
        stack->matrix_stack[i] = (Matrix*)malloc(sizeof(Matrix));
    }
}

void push_Op(Stacks* stack, char operator){ // push operand into stack
    assert(stack != NULL);
    stack->operation_stack_top++;
    int new_Idx = stack->operation_stack_top;
    stack->operation_stack[new_Idx] = operator;
}

void push_Mat(Stacks* stack, Matrix* matrix){// push matrix into stack
    assert(stack != NULL && matrix!=NULL);
    stack->matrix_stack_top++;
    int new_Idx = stack->matrix_stack_top;
    stack->matrix_stack[new_Idx] = matrix;
}
char pop_Op(Stacks* stack){ // pop operand from stack
    assert(stack!=NULL);
    int top_idx = stack->operation_stack_top;
    char op = stack->operation_stack[top_idx];
    stack->operation_stack_top -= 1;
    return op;
}

Matrix* pop_Mat(Stacks* stack){ // pop matrix from stack
    assert(stack != NULL);
    int top_idx = stack->matrix_stack_top;
    Matrix* matrix = stack->matrix_stack[top_idx];
    stack->matrix_stack_top -= 1;
    stack->matrix_stack[top_idx] = NULL; // Set the popped matrix pointer to NULL
    return matrix;
}

int is_matStack_Empty(Stacks* s){ // check is matrix stack empty
    assert(s != NULL);
    return s->matrix_stack_top==-1;
}

////////////////////////////////FUNCTIONS///////////////////////////////////////
Matrix* matrix_Multiplication(Matrix *A, Matrix *B){ // A*B
    assert(A!=NULL && B!=NULL);
    assert(A->data!=NULL&&B->data!=NULL);
    assert(A->col>0&&B->col>0);
    assert(A->row>0&&B->row>0);
    int row = A->row;
    int col = B->col;
    int colrow = B->row;
    Matrix* C = mat_allocate_memory(row, col);
    for(int i=0;i<row;i++){
        for(int j=0;j<col;j++){
            C->data[i][j] = 0;
            for (int k=0;k<colrow;k++){
                C->data[i][j] += A->data[i][k] * B->data[k][j];
            }
        }
    }
    release_memory(A);
    release_memory(B);
    return C;
}


Matrix* matrix_Addition(Matrix *A, Matrix *B){ // A+B
    assert(A!=NULL && B!=NULL);
    assert(A->data!=NULL&&B->data!=NULL);
    assert(A->col>0&&B->col>0);
    assert(A->row>0&&B->row>0);
    int row = A->row;
    int col = B->col;
    Matrix* C = mat_allocate_memory(row, col);
    for(int i=0;i<row;i++){
        for(int j=0;j<col;j++){
            C->data[i][j] = A->data[i][j] + B->data[i][j];
        }
    }
    release_memory(A);
    release_memory(B);
    return C;
}

Matrix * matrix_Subtraction(Matrix *A, Matrix *B){ // A-B
    assert(A!=NULL && B!=NULL);
    assert(A->data!=NULL&&B->data!=NULL);
    assert(A->col>0&&B->col>0);
    assert(A->row>0&&B->row>0);
    int row = A->row;
    int col = B->col;
    Matrix* C = mat_allocate_memory(row, col);
    for(int i=0;i<row;i++){
        for(int j=0;j<col;j++){
            C->data[i][j] = A->data[i][j] - B->data[i][j];
        }
    }
    release_memory(A);
    release_memory(B);
    return C;
}

void matrix_Print(Matrix* M){ // print matrix content
    assert(M!=NULL);
    int row = M->row;
    int col = M->col;
    printf("%d %d\n", row, col);
    for(int i=0;i<row;i++){
        for(int j=0;j<col;j++){
            printf("%d ", M->data[i][j]);
        }
        printf("\n");
    }
}

void release_memory(Matrix *matrix){ // deallocate matrix
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
/////////////////////////////////Threads FUNCTION///////////////////////////////

struct arg_struct
{
    Matrix *A;
    Matrix *B;
} *args;

void* printRunning(void* data) {
    struct arg_struct *args = data;
  //int *str = (int*) data; 
    printf("Running in Thread %d\n", 1); 
    printf("\n Matrix:\n");
    matrix_Print(args->A);
//   free(str);
  
  pthread_exit(NULL); 
}

void* threadAddition(void *data) {
    struct arg_struct *args = data;
    Matrix *ans = malloc(sizeof(Matrix) * 1);
    ans = matrix_Addition(args->A,args->B);
    pthread_exit((void*)ans); 
}

void* threadSubtraction(void *data) {
    struct arg_struct *args = data;
    Matrix *ans = malloc(sizeof(Matrix) * 1);
    ans = matrix_Subtraction(args->A,args->B);
    pthread_exit((void*)ans); 
}
void* threadMultiplication(void *data) {
    struct arg_struct *args = data;
    Matrix *ans = malloc(sizeof(Matrix) * 1);
    ans = matrix_Multiplication(args->A,args->B);
    pthread_exit((void*)ans); 
}





////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////MAIN FUNCTION//////////////////////////////////

int main() {
    // pthread variable
    pthread_t * threads;
    threads = (pthread_t*)malloc(sizeof(pthread_t)*numOfThreads); // allocate memony for thread
    args = malloc(sizeof(struct arg_struct) * 1);
    void* ans; //for thread return result of matrix

    
    /***** Below is the input part*****/
    // first input -> A-B / A+B*C+D / A*B+C
    char expression[EXP_MAX_SIZE] = {};
    scanf("%s", expression);
    int explen = strlen(expression);
    int noOfMatrix = explen / 2 + 1; // cal how many matrix in expression
    
    // init the stack object
    Stacks *s = (Stacks*)malloc(sizeof(Stacks));
    // initialize our stack object
    init(s);
    
    for(int i=0; i<noOfMatrix; i++){ // A loop to input all the matrices
        // second input -> size of a nxm matrix
        int n, m; // n = row, m = col
        //printf("Please input the (r c): ");
        printf("\n");
        scanf("%d %d", &n , &m);
        /* initialization of our matrix */
        Matrix *mat = mat_allocate_memory(n, m);
        printf("\n");
        // third input -> values of our matrix
        //printf("Please input the value for matrix: \n");
        for(int x=0; x<n; x++)
            for(int y=0; y<m; y++)
            {
                scanf("%d", &mat->data[x][y]);
                mat->row = n;
                mat->col = m;
            }
        arr_Matrices[i] = mat_allocate_memory(n, m);
        arr_Matrices[i] = mat;
    }
    printf("\n");
    
    // adding stack and char to our stacks and multiply
    for (int i=0;i<explen;i++){
        if (i%2 == 0){
            if (arr_Matrices[i/2] != NULL){
                push_Mat(s, arr_Matrices[i/2]);
            }
        }else{
            char op[1];
            strncpy(op, expression+i,1);
            if (op[0] == '*'){
                args->A = pop_Mat(s);
                args->B = arr_Matrices[i/2+1];
                pthread_create( &threads[0], NULL, threadMultiplication , args);
                pthread_join( threads[0], &ans );
                push_Mat(s,ans);
                arr_Matrices[i/2+1]= NULL;
            }else{
                push_Op(s, op[0]);
            }
        }
    }

    // last calculation
    while (s->matrix_stack_top != 0){
        Matrix* B = pop_Mat(s);
        Matrix* A = pop_Mat(s);

        args->A = A; //for thread
        args->B = B; //for thread

        char now = pop_Op(s);
        int optop = s->operation_stack_top;
        char front;
        if (optop == -1){
            front = ' ';
        }else{
            front = s ->operation_stack[optop];
        }
        if (now == '+'){
            if (front== '-'){
                pthread_create( &threads[0], NULL, threadSubtraction , args);
                pthread_join( threads[0], &ans );
                push_Mat(s,ans); //A-B
            }else{
                pthread_create( &threads[0], NULL, threadAddition , args);
                pthread_join( threads[0], &ans );
                push_Mat(s,ans); //A+B
            }
        }else{ // pop_Op(s) == '-'
            if (front== '-'){
                pthread_create( &threads[0], NULL, threadAddition , args);
                pthread_join( threads[0], &ans );
                push_Mat(s,ans); //A+B
            }else{
                pthread_create( &threads[0], NULL, threadSubtraction , args);
                pthread_join( threads[0], &ans );
                push_Mat(s,ans); //A-B
            }
        }



    }
    
    //printf("Our stack: \n");
    for(int i=s->matrix_stack_top; i>=0; i--){
        matrix_Print(s->matrix_stack[i]);
    }
    // popping all the matrix
    while(!is_matStack_Empty(s)){
        pop_Mat(s);
    }


    // for(int i = 0;i< numOfThreads;i++){

    //     pthread_create( &threads[i], NULL, printRunning, args);
    //     printf("thread created!(main)\n");
    // }

  

     
    // for (int i = 0; i < numOfThreads;i++ ) {
    //     pthread_join( threads[i], NULL );
    // }


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
