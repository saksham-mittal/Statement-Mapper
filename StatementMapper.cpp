#include "llvm/Transforms/StatementMapper/StatementMapper.h"
    
void StatementMapper::SMapper::getAnalysisUsage(AnalysisUsage &AU) const {
    AU.addRequired<LoopInfoWrapperPass>();
    AU.addPreserved<LoopInfoWrapperPass>();
    // AU.setPreservesAll();
}

void StatementMapper::SMapper::callUseDef(Instruction *Inst, std::map<int, std::vector<Instruction*>> &paramMap, std::map<int, std::vector<Instruction*>> &paramAllocaMap, int l) {            
    std::string cmp = "<Invalid operator> ";
    for(Use &U : Inst->operands()) {
        Value *v = U.get();
        Instruction* paramI = (Instruction *)v;
        callUseDef(paramI, paramMap, paramAllocaMap, l);
    }
    if(isa<AllocaInst>(Inst)) {
        if(std::find(paramAllocaMap[l].begin(), paramAllocaMap[l].end(), Inst) != paramAllocaMap[l].end()) {
            // If such an instruction is already present in the vector,
            // Remove that old one, and reinsert the new one at this position
            paramAllocaMap[l].erase(std::remove(paramAllocaMap[l].begin(), paramAllocaMap[l].end(), Inst), paramAllocaMap[l].end());
            if(strcmp(Inst->getOpcodeName(), cmp.c_str()))                    
                paramAllocaMap[l].push_back(Inst);
        } else {
            // Means this allocation instruction is pushed for the first time
            if(strcmp(Inst->getOpcodeName(), cmp.c_str()))
                paramAllocaMap[l].push_back(Inst);
        }
    } else {
        if(strcmp(Inst->getOpcodeName(), cmp.c_str()))
            paramMap[l].push_back(Inst);
    }
}

bool StatementMapper::SMapper::runOnFunction(Function &F) {
    std::map<int, std::vector<Instruction*>> InstMap;
    std::map<int, std::vector<Instruction*>> AllocaInstMap;

    // LoopInfo &LI = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();

    // for (Loop *L : LI){
    //     dbgs() << L->getCanonicalInductionVariable() << "\n";
    // }
    int count;
    
    for (BasicBlock &BB : F) {
        if(!BB.getName().startswith("for.inc")) {
            for (Instruction &I : BB) {
                if(DILocation *Loc = I.getDebugLoc()) {
                    if(isa<StoreInst>(I) || isa<ReturnInst>(I)) {
                        count = (int)Loc->getLine();
                        Instruction *paramI = &I;
                        callUseDef(paramI, InstMap, AllocaInstMap, count);
                    }
                }
            }
        }
    }

    for(auto elem : InstMap) {
        for(int i=(int)AllocaInstMap[elem.first].size() - 1; i>=0; i--) {
            InstMap[elem.first].insert(InstMap[elem.first].begin(), AllocaInstMap[elem.first][i]);
        }
    }

    bool found;
    for(auto elem : InstMap) {
        found = false;
        for(int i=0; i<(int)elem.second.size(); i++) {
            // Check if any instruction is a BinaryOperator instruction
            if(isa<BinaryOperator>(elem.second[i])) {
                found = true;
            }
        }
        if(found) {
            dbgs() << "Statement=" << elem.first << "{" << "\n";
            for(int i=0; i<(int)elem.second.size(); i++) {
                dbgs() << *elem.second[i] << "\n";
            }
            dbgs() << "}\n";
        }
    }

    return false;
}

char StatementMapper::SMapper::ID = 'a';
static RegisterPass<StatementMapper::SMapper> X("stmt-mapper", "Maps the arithmetic operations instructions to LLVM code.");
