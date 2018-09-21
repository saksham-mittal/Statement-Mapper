#pragma once

#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/Debug.h"
#include "llvm/IR/DebugLoc.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Support/Casting.h"
#include <bits/stdc++.h>

using namespace llvm;

namespace StatementMapper {
    class SMapper : public FunctionPass {
        public:
        static char ID;
        SMapper() : FunctionPass(ID) {}
        
        void getAnalysisUsage(AnalysisUsage &AU) const;

        void callUseDef(Instruction *Inst, std::map<int, std::vector<Instruction*>> &paramMap, std::map<int, std::vector<Instruction*>> &paramAllocaMap, int l);

        bool runOnFunction(Function &F);
    };
}