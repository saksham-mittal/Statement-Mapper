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
%31 = getelementptr inbounds [100 x i32 ], [100 x i32 ]* %2 , i64 0, i64 %30
%32 = load i32 , i32 * %31 , align 4
%33 = load i32 , i32 * %6 , align 4
%34 = sext i32 %33 to i64
%35 = getelementptr inbounds [100 x i32 ], [100 x i32 ]* %3 , i64 0, i64 %34
%36 = load i32 , i32 * %35 , align 4
%37 = add nsw i32 %32 , %36
%38 = load i32 , i32 * %6 , align 4
%39 = sext i32 %38 to i64
%40 = getelementptr inbounds [100 x i32 ], [100 x i32 ]* %4 , i64 0, i64 %39
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

## Design of code

The basic idea used in the assignment, is to find the end of the arithmetic operation statement in the *.ll* file, and then traverse back till we have traversed all the required instructions. The end instruction is **store** instruction because we will need to store the arithmetic instruction result in some variable.

We find the line number of the corresponding instruction using meta data information present in the *.ll* file, which is compiled using *-g* flag. Relevant information can be found at : [**Link**](https://llvm.org/docs/SourceLevelDebugging.html#object-lifetimes-and-scoping)

The next challenge was to traverse back to the operand's instruction of the given store instruction. For this, we used def-use and use-def chains in recursion. Relevant information can be found at : [**Link**](http://llvm.org/docs/ProgrammersManual.html#iterating-over-def-use-use-def-chains)

The instructions extracted from the use-def chains were iterated in a post-order traversal, to get the desired ouput format.

Then, next problem was that some extra instructions were printed, like `i32 0`, `i64 1`. To prevent such instructions from printing, we used a simple check condition that the `getOpcodeName()` should not return `"<Invalid operator> "`.

The next problem faced was **not** mapping the instructions for induction variables. For this we assumed that induction variables only comes in loops, so while traversing the basic block, the name of the basic block should not start with `"for.inc"`. This will prevent the induction variables from being mapped to llvm code.

The next problem faced was to group the `alloca` intructions on the top, with induction variables allocation after the other variables allocation. This was solved using a `std::map` for the `alloca` instructions, which were detected using `isa<AllocaInst>`.

The code also handles `fadd`, `fmul`... and other arithmetic instructions, which was handled by checking `isa<BinaryOperator>` and ending with store instruction.

The other llvm instruction functions used in the code can be found at [**Link**](http://llvm.org/doxygen/classllvm_1_1Instruction.html)