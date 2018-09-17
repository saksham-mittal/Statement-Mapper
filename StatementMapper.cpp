#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/Debug.h"
#include "llvm/IR/DebugLoc.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/Analysis/LoopInfo.h"
#include <bits/stdc++.h>

using namespace llvm;

namespace{
    struct SMapper : FunctionPass {
        static char ID;
        SMapper() : FunctionPass(ID) {}
        
        void getAnalysisUsage(AnalysisUsage &AU) const {
            AU.addRequired<LoopInfoWrapperPass>();
            AU.addPreserved<LoopInfoWrapperPass>();
            // AU.setPreservesAll();
        }

        bool runOnFunction(Function &F) {
            std::vector<int> lNo;
            const char* opName = "add";
            int localLineNo;

            LoopInfo &LI = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();

            for (Loop *L : LI){
                dbgs() << L->getCanonicalInductionVariable()->getName() << "\n";
            }

            
            for (BasicBlock &BB : F) {
                for (Instruction &I : BB) {
                    if (DILocation *Loc = I.getDebugLoc()) {
                        // dbgs() << I.getOpcodeName() << "\n";
                        if (!strcmp(I.getOpcodeName(), opName)) { // Here I is an LLVM instruction
                            // dbgs() << "Matched!" << "\n";
                            unsigned Line = Loc->getLine();
                            lNo.push_back(Line);
                        }
                    }
                }
            }

            while(!lNo.empty()) {
                localLineNo = lNo.back();
                lNo.pop_back();
                dbgs() << "Statement=" << localLineNo << "{" << "\n";
                for (BasicBlock &BB : F) {
                    for (Instruction &I : BB) {
                        // Here I is an LLVM instruction
                        if (DILocation *Loc = I.getDebugLoc()) {
                            unsigned Line = Loc->getLine();
                            if(Line == (unsigned)localLineNo) {
                                dbgs() << I << "\n";
                            }
                        }
                    }
                }

                dbgs() << "}\n";
            }

            return false;
        }
    };
}

char SMapper::ID = 'a';
static RegisterPass<SMapper> X("stmt-mapper", "Maps the arithmetic operations instructions to LLVM code.");
