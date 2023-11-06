# OS_Project

# 未做嘅嘢
1. matrix addition, substraction, and multiplication
2. multi-threading to split the matrix into parts and calculate it
    - split the expression("A*B+C"/"A+B*C+D") for parallel calculation.
       - use two stacks as the project instructions?
       - how to deal with different size of matrix between them
3. adding mutex lock to each different matrix
   
# 有可能要做嘅嘢
1. adding usual wrappers to the functions from the library in case they are failing without being shown in the debug window.
    a. ``` void Pthread_join(pthread_t thread, void **value_ptr) {
    int rc = pthread_join(thread, value_ptr);
    assert(rc == 0);
}```
2. we could define a matrix structure that includes a mutex lock and its value
3. The global variable or some header function should be put in a new header file
4. taking a different approach of concurrency of matrix addition/multiplication -> different version of grabbing a lock of that matrix
   
# Below is a brief description of each test case:
• Test case 1-3: the small matrix operation including one matrix addition, or subtraction, or multiplication. You can use this test case to debug your codes.

• Test case 4-5: multiple matrix operations. There are more than 3 matrixes in the expression. There is only one type of operator in the expression (addition and subtraction are regareded as the same operator). The matrix size in those cases are smaller than 500 * 500, thus the time requirement is only 1 second.

• Test case 6-7: combination of matrix operations. There are more than 3 matrixes and multiple types of operators in the expression. The matrix size in those cases are also smaller than 500 * 500, thus the time requirement is only 1 second.

• Test case 8-10: complex matrix operations. There are more than 5 matrixes and multiple types of operators in the expression. The matrix size in those cases are smaller than 1000 * 1000, thus the time requirement is loose and set to 10 seconds.


# Each test consists of 5 files with different filename extension:

• n.json (in tests/config/ directory): The configuration of test case n.

    ▪ binary: Indicates the data type of input and output is binary.
    
    ▪ timeout: Time limitation (seconds).
    
    ▪ seed: The seed used to generate the content of input files.
    
    ▪ params: Including the maximum matrix size and the expression.
    
    ▪ description: A short text description of the test.
    
• n.rc (in tests/stdout/ directory): The return code the program should return (usually 0 or 1).

• n.out (in tests/stdout/ directory): The standard output expected from the test.

• n/input.txt (in tests/stdin/ directory): The test data.
