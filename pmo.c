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


struct arg_struct
{                          
    Matrix *A;
    Matrix *B;

};

struct arg_struct thread_data_array[numOfThreads];

//for thread memony sturture
// struct arg_struct
// {                          
//     Matrix *A[4];
//     Matrix *B[4];

// } *args;


// struct arg_struct
// {                           // matrix A
//     Matrix *A1;              // ----------
//     Matrix *B1;              //| 1    2  |
//                             // | 3    4  |
//     Matrix *A2;              //-----------
//     Matrix *B2;

//     Matrix *A3;
//     Matrix *B3;

//     Matrix *A4;
//     Matrix *B4;

//     Matrix *A;
//     Matrix *B;

// } *args;
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

Matrix* copymatrix(Matrix *source,int part){
    int startRowIndex = 0;
    int startColIndex = 0;
    int endRowIndex = 0;
    int endColIndex = 0;
    int row;
    int col;
    Matrix* dest;

    if(part == 1){
        startRowIndex = 0;                              
        startColIndex = 0;
        endRowIndex = source->row /2;
        endColIndex = source->col /2;

        row = source->row/2;
        col = source->col/2;

        dest = mat_allocate_memory(row, col);
        for(int i=startRowIndex;i<endRowIndex;i++){
            for(int j=startColIndex;j<endColIndex;j++){
                dest->data[i][j] = source->data[i][j];
            }
        }
    }else if(part == 2){
        startRowIndex = 0;                         
        startColIndex = source->col /2;
        endRowIndex = source->row /2;
        endColIndex = source->row;     

        row = source->row/2;
        col = source->col/2 +1;   
        
        dest = mat_allocate_memory(row, col);
        for(int i=startRowIndex;i<endRowIndex;i++){
            for(int j=startColIndex;j<endColIndex;j++){
                dest->data[i][j-col+1] = source->data[i][j];
            }
        }    

    }else if(part == 3){                         
        startRowIndex = source->row /2; 
        startColIndex = 0;  
        endRowIndex = source->row;
        endColIndex = source->col /2;  

        row = source->row/2 +1;
        col = source->col/2;

        dest = mat_allocate_memory(row, col);
        for(int i=startRowIndex;i<endRowIndex;i++){
            for(int j=startColIndex;j<endColIndex;j++){
                dest->data[i-row+1][j] = source->data[i][j];
            }
        }
    }else{ 
        startRowIndex = source->row /2; 
        startColIndex = source->col /2;
        endRowIndex = source->row;
        endColIndex = source->col;  

        row = source->row/2 +1;
        col = source->col/2 +1;

        dest = mat_allocate_memory(row, col);
        for(int i=startRowIndex;i<endRowIndex;i++){
            for(int j=startColIndex;j<endColIndex;j++){
                dest->data[i-row+1][j-col+1] = source->data[i][j];
            }
        }    
    }


    return dest;
}

void divideMatrix(Matrix* matrixA,Matrix* matrixB){

    // for (int i = 0; i < numOfThreads; i++)
    // {
    //     args[i]->A = copymatrix(matrixA,i+1);
    //     args[i]->B = copymatrix(matrixB,i+1);
    // }
    
    // matrix_Print(args[0]->A);
    // matrix_Print(args[1]->A);
    // matrix_Print(args[2]->A);
    // matrix_Print(args[3]->A);
    // matrix_Print(args[0]->B);

//    args->A1 = copymatrix(matrixA,1);
//    matrix_Print(args->A1);
//    args->A2 = copymatrix(matrixA,2);
//    matrix_Print(args->A2);
//    args->A3 = copymatrix(matrixA,3);
//    matrix_Print(args->A3);
//    args->A4 = copymatrix(matrixA,4);
//    matrix_Print(args->A4);
//    args->B1 = copymatrix(matrixB,1);
//    matrix_Print(args->B1);
//    args->B2 = copymatrix(matrixB,2);
//    args->B3 = copymatrix(matrixB,3);
//    args->B4 = copymatrix(matrixB,4);
for(int i = 0;i<numOfThreads;i++){
    thread_data_array[i].A = copymatrix(matrixA,i+1);
    thread_data_array[i].B = copymatrix(matrixB,i+1);
}

//     args->A[0] = copymatrix(matrixA,1);
//    matrix_Print(args->A[0]);
//    args->A[1] = copymatrix(matrixA,2);
//    matrix_Print(args->A[1]);
//    args->A[2] = copymatrix(matrixA,3);
//    matrix_Print(args->A[2]);
//    args->A[3]= copymatrix(matrixA,4);
//    matrix_Print(args->A[3]);
//    args->B[0] = copymatrix(matrixB,1);
//    matrix_Print(args->B[0]);
//    args->B[1] = copymatrix(matrixB,2);
//    args->B[2] = copymatrix(matrixB,3);
//    args->B[3] = copymatrix(matrixB,4);


}

Matrix* combineMatrix(void *data){

    Matrix** matrix;
    matrix = (Matrix**) data;

    Matrix* result;
    int row = matrix[0]->row + matrix[2]->row;
    int col = matrix[0]->col + matrix[1]->col;
    result = mat_allocate_memory(row,col);

    //for part 1
    for(int i=0;i<matrix[0]->row;i++){
            for(int j=0;j<matrix[0]->col;j++){
                result->data[i][j] = matrix[0]->data[i][j];
            }
        }    
    //for part 2
    for(int i=0;i<matrix[1]->row;i++){
            for(int j=0;j<matrix[1]->col;j++){
                result->data[i][j+row/2] = matrix[1]->data[i][j];
            }
        } 
       //for part 3
    for(int i=0;i<matrix[2]->row;i++){
            for(int j=0;j<matrix[2]->col;j++){
                result->data[i+row/2][j] = matrix[2]->data[i][j];
            }
        } 
           //for part 4
    for(int i=0;i<matrix[3]->row;i++){
            for(int j=0;j<matrix[3]->col;j++){
                result->data[i+row/2][j+col/2] = matrix[3]->data[i][j];
            }
        } 

    return result;
}
////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////Threads FUNCTION///////////////////////////////


// void* printRunning(void* data) {
//     struct arg_struct *args = data;
//   //int *str = (int*) data; 
//     printf("Running in Thread %d\n", 1); 
//     printf("\n Matrix:\n");
//     matrix_Print(args->A);
// //   free(str);
//   pthread_exit(NULL); 
// }

void* threadAddition(void *data) {

    struct arg_struct *args;
    args = (struct arg_struct*) data;
    Matrix *ans = malloc(sizeof(Matrix) * 1);
    
    // struct arg_struct *args = (arg_struct*)data;
    // Matrix *ans = malloc(sizeof(Matrix) * 1);

    ans = matrix_Addition(args->A,args->B);
    pthread_exit((void*)ans);
}

void* threadSubtraction(void *data) {
struct arg_struct *args;
    args = (struct arg_struct*) data;
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
    //thread_data_array = malloc(sizeof(struct arg_struct) * 4);
    void* ans;
    void* tempans[4]; //for thread return result of matrix
    

     
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
                // args->A = pop_Mat(s);
                // args->B = arr_Matrices[i/2+1];
                pthread_create( &threads[0], NULL, threadMultiplication , (void*) &thread_data_array[0]);
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

        divideMatrix(A,B);

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
                for(int i=0;i<numOfThreads;i++){
                    pthread_create( &threads[i], NULL,threadSubtraction, (void*) &thread_data_array[i]);
                }
                for (int i = 0; i < numOfThreads; i++)
                {
                    pthread_join( threads[i], &tempans[i] );
                }
            }else{


                // pthread_create( &threads[0], NULL, threadAddition , args);
                // pthread_join( threads[0], &tempans[0] );

                for(int i=0;i<numOfThreads;i++){
                    pthread_create( &threads[i], NULL,threadAddition, (void*) &thread_data_array[i]);
                }
                for (int i = 0; i < numOfThreads; i++)
                {
                    pthread_join( threads[i], &tempans[i] );
                }
                

                
            }
        }else{ // pop_Op(s) == '-'
            if (front== '-'){
                for(int i=0;i<numOfThreads;i++){
                    pthread_create( &threads[i], NULL,threadAddition, (void*) &thread_data_array[i]);
                }
                for (int i = 0; i < numOfThreads; i++)
                {
                    pthread_join( threads[i], &tempans[i] );
                }
            }else{
                for(int i=0;i<numOfThreads;i++){
                    pthread_create( &threads[i], NULL,threadSubtraction, (void*) &thread_data_array[i]);
                }
                for (int i = 0; i < numOfThreads; i++)
                {
                    pthread_join( threads[i], &tempans[i] );
                }
            }
        }
        
        ans = combineMatrix((void*)tempans);
                        //合matrix
        matrix_Print(ans);
               
        push_Mat(s,ans);


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
