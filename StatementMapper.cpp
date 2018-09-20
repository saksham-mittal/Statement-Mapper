#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/Debug.h"
#include "llvm/IR/DebugLoc.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Support/Casting.h"
#include <bits/stdc++.h>

using namespace llvm;
/*
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
            std::vector<int> lineNos;
            std::map<int, std::vector<Instruction*>> InstMap;
            std::map<StringRef, Instruction*> AllocaInstMap;

            std::map<int, std::vector<Instruction*>> LineAllocaInstMap;

            // LoopInfo &LI = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();

            // for (Loop *L : LI){
            //     dbgs() << L->getCanonicalInductionVariable() << "\n";
            // }

            
            for (BasicBlock &BB : F) {
                for (Instruction &I : BB) {
                    if (DILocation *Loc = I.getDebugLoc()) {
                        if (isa<BinaryOperator>(I)) { 
                            // Here I is an LLVM instruction
                            lineNos.push_back(Loc->getLine());
                        }
                    }
                    if(isa<AllocaInst>(I)) {
                        Instruction *paramI = &I;
                        AllocaInstMap[I.getName()] = paramI;
                    }
                }
            }

            for (BasicBlock &BB : F) {
                for (Instruction &I : BB) {
                    if (DILocation *Loc = I.getDebugLoc()) {
                        for(int i=0; i<(int)lineNos.size(); i++) {
                            if((int)Loc->getLine() == lineNos[i]) {
                                Instruction *paramI = &I;
                                InstMap[Loc->getLine()].push_back(paramI);
                            }
                        }
                    }
                }
            }

            for(auto elem : InstMap) {
                for(int i=0; i<(int)elem.second.size(); i++) {
                    
                    // Iterate over all the variables of the instruction
                    // and add the alloca for them
                    

                    // n is the number of variables in the instruction
                    int n = elem.second[i]->getNumOperands();
                    for(int j=0; j<n; j++) {
                        if(AllocaInstMap.find(elem.second[i]->getOperand(j)->getName()) != AllocaInstMap.end()) {
                            Instruction *paramI = AllocaInstMap[elem.second[i]->getOperand(j)->getName()];
                            if(std::find(LineAllocaInstMap[elem.first].begin(), LineAllocaInstMap[elem.first].end(), paramI) == LineAllocaInstMap[elem.first].end()) {
                                // Means 'paramI' is not present in the vector
                                LineAllocaInstMap[elem.first].push_back(paramI);
                            }
                        }
                    }
                }
            }

            for(auto elem : LineAllocaInstMap) {
                for(int i=(int)elem.second.size() - 1; i>=0; i--) {
                    InstMap[elem.first].insert(InstMap[elem.first].begin(), elem.second[i]);
                }
            }

            for(auto elem : InstMap) {
                dbgs() << "Statement=" << elem.first << "{" << "\n";
                for(int i=0; i<(int)elem.second.size(); i++) {
                    dbgs() << *elem.second[i] << "\n";
                }
                dbgs() << "}\n";
            }

            return false;
        }
    };
}
*/

namespace{
    struct SMapper : FunctionPass {
        static char ID;
        SMapper() : FunctionPass(ID) {}
        
        void getAnalysisUsage(AnalysisUsage &AU) const {
            AU.addRequired<LoopInfoWrapperPass>();
            AU.addPreserved<LoopInfoWrapperPass>();
            // AU.setPreservesAll();
        }

        void callUseDef(Instruction *Inst, std::map<int, std::vector<Instruction*>> &paramMap, std::map<int, std::vector<Instruction*>> &paramAllocaMap, int l) {            
            for(Use &U : Inst->operands()) {
                Value *v = U.get();
                Instruction* paramI = (Instruction *)v;
                callUseDef(paramI, paramMap, paramAllocaMap, l);
            }
            if(isa<AllocaInst>(Inst)) {
                paramAllocaMap[l].push_back(Inst);
            } else {
                paramMap[l].push_back(Inst);
            }
        }

        bool runOnFunction(Function &F) {
            std::map<int, std::vector<Instruction*>> InstMap;
            std::map<int, std::vector<Instruction*>> AllocaInstMap;

            // LoopInfo &LI = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();

            // for (Loop *L : LI){
            //     dbgs() << L->getCanonicalInductionVariable() << "\n";
            // }
            int count;
            
            for (BasicBlock &BB : F) {
                for (Instruction &I : BB) {
                    if(DILocation *Loc = I.getDebugLoc()) {
                        if(isa<StoreInst>(I)) {   
                            count = (int)Loc->getLine();
                            Instruction *paramI = &I;
                            callUseDef(paramI, InstMap, AllocaInstMap, count);
                        }
                    }
                }
            }

            for(auto elem : InstMap) {
                for(int i=(int)AllocaInstMap[elem.first].size() - 1; i>=0; i--) {
                    InstMap[elem.first].insert(InstMap[elem.first].begin(), AllocaInstMap[elem.first][i]);
                }
            }

            for(auto elem : InstMap) {
                dbgs() << "Statement=" << elem.first << "{" << "\n";
                for(int i=0; i<(int)elem.second.size(); i++) {
                    dbgs() << *elem.second[i] << "\n";
                }
                dbgs() << "}\n";
            }

            return false;
        }
    };
}

char SMapper::ID = 'a';
static RegisterPass<SMapper> X("stmt-mapper", "Maps the arithmetic operations instructions to LLVM code.");
