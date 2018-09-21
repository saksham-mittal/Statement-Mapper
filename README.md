# Statement-Mapper

Part of the course **Compiler-Enginnering (CS6383)**, we have to write a LLVM pass to map list of LLVM IR instructions to corresponding high level statements involving arithmetic operations.

For example, for the following Source code :
``` cpp
int main() {
    int A[100], B[100], C[100];
    // init A, B, C
    for (int i=0; i<100; i++)
        C[i] = A[i] + B[i];
    return 0;
}
```
The following LLVM IR instructions should be printed :
```
%2 = alloca [100 x i32 ], align 16
%3 = alloca [100 x i32 ], align 16
%4 = alloca [100 x i32 ], align 16
%6 = alloca i32 , align 4
%29 = load i32 , i32 * %6 , align 4
%30 = sext i32 %29 to i64
%31 = getelementptr inbounds [100 x i32 ], [100 x i32 ]* %2 , ←-
i64 0, i64 %30
%32 = load i32 , i32 * %31 , align 4
%33 = load i32 , i32 * %6 , align 4
%34 = sext i32 %33 to i64
%35 = getelementptr inbounds [100 x i32 ], [100 x i32 ]* %3 , ←-
i64 0, i64 %34
%36 = load i32 , i32 * %35 , align 4
%37 = add nsw i32 %32 , %36
%38 = load i32 , i32 * %6 , align 4
%39 = sext i32 %38 to i64
%40 = getelementptr inbounds [100 x i32 ], [100 x i32 ]* %4 , ←-
i64 0, i64 %39
store i32 %37 , i32 * %40 , align 4
```

To run the pass :
* Make a directory in the `$LLVM_SOURCE/lib/Transforms/` named **StatementMapper**. 
* Add the *StatementMapper.cpp* and *CmakeLists.txt* in the directory.
* In the `$LLVM_SOURCE/lib/Transforms/` directory, edit the *CMakeLists.txt* as :
```
add_subdirectory(StatementMapper)
...
```
* Now, change directory to `$LLVM_BUILD`.
* Run the cmake command : `cmake ../$LLVM_SOURCE/ -DCMAKE_BUILD_TYPE=Debug`
* Run the make command : `make -j3 bin/./opt StatementMapper.so` (This generates the *.so* file)
* Run the pass on LLVM IR code : `bin/./opt -load lib/StatementMapper.so -stmt-mapper test.ll`

The directory structure followed is :
```
./
|___llvm
|   |___lib
|   |   |___Transforms
|   |       |___StatementMapper/
|   |___include
|       |___llvm
|           |___Transforms
|               |___StatementMapper/
|___test/
|___README
``` 

**NOTE : Change the appropriate paths according to your own system.**