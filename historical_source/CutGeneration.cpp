#ifndef IndINCLUDE
#define IndINCLUDE
#include "Header.h"
#include <stdio.h>
#include <stdlib.h>
#endif

/////////////////////////////////////////////////////
///       CUT & BRANCH WITH (k,U) INEQUALITIES	 //// 
///	  SKIP LEAF + MAX U + MOST VIOLATED  LEAF    ////
/////////////////////////////////////////////////////

void IntegerOptimalityCutsGeneration(IloEnv env, int n, int ExpSize, int MultiCuts){

    if(MultiCuts==0){
        IloExpr IntegerCut (env);
        IloExpr AuxIntegerCut (env);
        IloExpr AuxSumCut (env);
        for (int j=ChildrenModel[n][1]; j<=ChildrenModel[n][c]; j++){
            AuxIntegerCut += (Prob[NodeModel[j][0]]/Prob[NodeModel[n][0]])*IntegerCutValue[j];
        }
        for(int h=0; h<ExpSize; h++){
            AuxSumCut+= (AuxBS[StageModel[n]][Leaf_Label[SampleLeaf[StageModel[n]]]][h]-1)*BS[Leaf_Label[SampleLeaf[StageModel[n]]]][h]+(BS[Leaf_Label[SampleLeaf[StageModel[n]]]][h]-1)*AuxBS[StageModel[n]][Leaf_Label[SampleLeaf[StageModel[n]]]][h];
        }
        IntegerCut = AuxIntegerCut*AuxSumCut+AuxIntegerCut;

        NodalModels[n].add(PSI[Leaf_Label[SampleLeaf[StageModel[n]]]]-IntegerCut>=0);
        LagrangianModels[n].add(PSI[Leaf_Label[SampleLeaf[StageModel[n]]]]-IntegerCut>=0);

        IntegerCut.end();
        AuxIntegerCut.end();
        AuxSumCut.end();
    }
    else{
        for(int g=1; g<=NbLeaf; g++){
            IloExpr IntegerCut (env);
            IloExpr AuxIntegerCut (env);
            IloExpr AuxSumCut (env);

            for (int j=ChildrenModel[n][1]; j<=ChildrenModel[n][c]; j++){
                AuxIntegerCut += (Prob[NodeModel[j][0]]/Prob[NodeModel[n][0]])*IntegerCutValue[j];
            }

            for(int h=0; h<ExpSize; h++){
                AuxSumCut+= (AuxBS[StageModel[n]][Leaf_Label[SampleLeaf[StageModel[n]]]][h]-1)*BS[g][h]+(BS[g][h]-1)*AuxBS[StageModel[n]][Leaf_Label[SampleLeaf[StageModel[n]]]][h];
            }

            IntegerCut = AuxIntegerCut*AuxSumCut+AuxIntegerCut;

            NodalModels[n].add(PSI[g]-IntegerCut>=0);
            LagrangianModels[n].add(PSI[g]-IntegerCut>=0);

            IntegerCut.end();
            AuxIntegerCut.end();
            AuxSumCut.end();
        }
    }
}

void LIntegerOptimalityCutsGeneration(IloEnv env, int n, int ExpSize, int MultiCuts){

    if(MultiCuts==0){
        IloExpr IntegerCut (env);
        IloExpr AuxIntegerCut (env);
        IloExpr AuxSumCut (env);
        for (int j=ChildrenModel[n][1]; j<=ChildrenModel[n][c]; j++){
            AuxIntegerCut += (Prob[NodeModel[j][0]]/Prob[NodeModel[n][0]])*IntegerCutValue[j];
        }
        for(int h=0; h<ExpSize; h++){
            AuxSumCut+= (AuxBS[StageModel[n]][Leaf_Label[SampleLeaf[StageModel[n]]]][h]-1)*LBS[Leaf_Label[SampleLeaf[StageModel[n]]]][h]+(LBS[Leaf_Label[SampleLeaf[StageModel[n]]]][h]-1)*AuxBS[StageModel[n]][Leaf_Label[SampleLeaf[StageModel[n]]]][h];
        }
        IntegerCut = AuxIntegerCut*AuxSumCut+AuxIntegerCut;
        LinearModels[n].add(PSI[Leaf_Label[SampleLeaf[StageModel[n]]]]-IntegerCut>=0);
        IntegerCut.end();
        AuxIntegerCut.end();
        AuxSumCut.end();
    }
    else{
        for(int g=1; g<=NbLeaf; g++){
            IloExpr IntegerCut (env);
            IloExpr AuxIntegerCut (env);
            IloExpr AuxSumCut (env);

            for (int j=ChildrenModel[n][1]; j<=ChildrenModel[n][c]; j++){
                AuxIntegerCut += (Prob[NodeModel[j][0]]/Prob[NodeModel[n][0]])*IntegerCutValue[j];
            }

            for(int h=0; h<ExpSize; h++){
                AuxSumCut+= (AuxBS[StageModel[n]][Leaf_Label[SampleLeaf[StageModel[n]]]][h]-1)*LBS[g][h] + (LBS[g][h]-1)*AuxBS[StageModel[n]][Leaf_Label[SampleLeaf[StageModel[n]]]][h];
            }

            IntegerCut = AuxIntegerCut*AuxSumCut+AuxIntegerCut;
            LinearModels[n].add(PSI[g]-IntegerCut>=0);
            IntegerCut.end();
            AuxIntegerCut.end();
            AuxSumCut.end();
        }
    }
}

void StrengthenedBendersCutsGeneration(IloEnv env, int i, int MultiCuts){

    if(MultiCuts==0){
        IloExpr Cut (env);
        for (int j=ChildrenModel[i][1]; j<=ChildrenModel[i][c]; j++){
            Cut += (Prob[NodeModel[j][0]]/Prob[NodeModel[i][0]])*(OptimalLagrangian[j]);
            Cut += (Prob[NodeModel[j][0]]/Prob[NodeModel[i][0]])*(Duals[j]*S[SampleLeaf[StageModel[i]]]);
        }
        NodalModels[i].add(PSI[Leaf_Label[SampleLeaf[StageModel[i]]]]-Cut>=0);
        LinearModels[i].add(PSI[Leaf_Label[SampleLeaf[StageModel[i]]]]-Cut>=0);
        LagrangianModels[i].add(PSI[Leaf_Label[SampleLeaf[StageModel[i]]]]-Cut>=0);
        Cut.end();

    }
    else{
        for(int g=1; g<=NbLeaf; g++){
            IloExpr Cut (env);
            for (int j=ChildrenModel[i][1]; j<=ChildrenModel[i][c]; j++){
                Cut += (Prob[NodeModel[j][0]]/Prob[NodeModel[i][0]])*(OptimalLagrangian[j]);
                Cut += (Prob[NodeModel[j][0]]/Prob[NodeModel[i][0]])*(Duals[j]*S[SetLeaf_SubModel[g]]);
            }
            NodalModels[i].add(PSI[g]-Cut>=0);
            LinearModels[i].add(PSI[g]-Cut>=0);
            LagrangianModels[i].add(PSI[g]-Cut>=0);
            Cut.end();
        }
    }

}

void BendersCutsGeneration(IloEnv env, int i, int MultiCuts){
    if(MultiCuts==0){
        IloExpr Cut (env);
        for (int j=ChildrenModel[i][1]; j<=ChildrenModel[i][c]; j++){
            Cut += (Prob[NodeModel[j][0]]/Prob[NodeModel[i][0]])*(OptimalLinear[j]);
            Cut += (Prob[NodeModel[j][0]]/Prob[NodeModel[i][0]])*(Duals[j]*(S[SampleLeaf[StageModel[i]]]-AuxS[StageModel[i]][SampleLeaf[StageModel[i]]]));

        }
        NodalModels[i].add(PSI[SampleLeaf[StageModel[i]]]-Cut>=0);
        LinearModels[i].add(PSI[SampleLeaf[StageModel[i]]]-Cut>=0);
        LagrangianModels[i].add(PSI[SampleLeaf[StageModel[i]]]-Cut>=0);
        Cut.end();
    }
    else{
        for(int g=1; g<=NbLeaf; g++){
            IloExpr Cut (env);
            for (int j=ChildrenModel[i][1]; j<=ChildrenModel[i][c]; j++){
                Cut += (Prob[NodeModel[j][0]]/Prob[NodeModel[i][0]])*(OptimalLinear[j]);
                Cut += (Prob[NodeModel[j][0]]/Prob[NodeModel[i][0]])*(Duals[j]*(S[SetLeaf_SubModel[g]]-AuxS[StageModel[i]][SampleLeaf[StageModel[i]]]));

            }
            NodalModels[i].add(PSI[g]-Cut>=0);
            LinearModels[i].add(PSI[g]-Cut>=0);
            LagrangianModels[i].add(PSI[g]-Cut>=0);
            Cut.end();
        }
    }
}

void B_StrengthenedBendersCutsGeneration(IloEnv env, int i, int ExpSize, int MultiCuts){
    if(MultiCuts==0){

        IloExpr Cut (env);
        for (int j=ChildrenModel[i][1]; j<=ChildrenModel[i][c]; j++){
            Cut += (Prob[NodeModel[j][0]]/Prob[NodeModel[i][0]])*(OptimalLagrangian[j]);
                for(int h=0; h<ExpSize; h++){
                    Cut+= (Prob[NodeModel[j][0]]/Prob[NodeModel[i][0]])*(B_Duals[j][h]*BS[Leaf_Label[SampleLeaf[StageModel[i]]]][h]);
                }
        }
        NodalModels[i].add(PSI[Leaf_Label[SampleLeaf[StageModel[i]]]]-Cut>=0);
        LagrangianModels[i].add(PSI[Leaf_Label[SampleLeaf[StageModel[i]]]]-Cut>=0);
        Cut.end();
    }
    else{
        for(int g=1; g<=NbLeaf; g++){
            IloExpr Cut (env);
            for (int j=ChildrenModel[i][1]; j<=ChildrenModel[i][c]; j++){
                Cut += (Prob[NodeModel[j][0]]/Prob[NodeModel[i][0]])*(OptimalLagrangian[j]);
                    for(int h=0; h<ExpSize; h++){
                        Cut+= (Prob[NodeModel[j][0]]/Prob[NodeModel[i][0]])*(B_Duals[j][h]*BS[g][h]);
                    }
            }
            NodalModels[i].add(PSI[g]-Cut>=0);
            LagrangianModels[i].add(PSI[g]-Cut>=0);
            Cut.end();
        }

    }

}

void B_LStrengthenedBendersCutsGeneration(IloEnv env, int i, int ExpSize, int MultiCuts){
    if(MultiCuts==0){

        IloExpr Cut (env);
        for (int j=ChildrenModel[i][1]; j<=ChildrenModel[i][c]; j++){
            Cut += (Prob[NodeModel[j][0]]/Prob[NodeModel[i][0]])*(OptimalLagrangian[j]);
                for(int h=0; h<ExpSize; h++){
                    Cut+= (Prob[NodeModel[j][0]]/Prob[NodeModel[i][0]])*(B_Duals[j][h]*LBS[Leaf_Label[SampleLeaf[StageModel[i]]]][h]);
                }
        }
        LinearModels[i].add(PSI[Leaf_Label[SampleLeaf[StageModel[i]]]]-Cut>=0);
        Cut.end();
    }
    else{
         for(int g=1; g<=NbLeaf; g++){
             IloExpr Cut (env);
             for (int j=ChildrenModel[i][1]; j<=ChildrenModel[i][c]; j++){
                 Cut += (Prob[NodeModel[j][0]]/Prob[NodeModel[i][0]])*(OptimalLagrangian[j]);
                     for(int h=0; h<ExpSize; h++){
                         Cut+= (Prob[NodeModel[j][0]]/Prob[NodeModel[i][0]])*(B_Duals[j][h]*LBS[g][h]);
                     }
             }
             LinearModels[i].add(PSI[g]-Cut>=0);
             Cut.end();
         }

    }
}

void LagrangianCutsGeneration(IloEnv env, int i, int ExpSize, int MultiCuts){
    if(MultiCuts==0){

        IloExpr LangrangianCut (env);
        for (int j=ChildrenModel[i][1]; j<=ChildrenModel[i][c]; j++){
            IloNum AuxOptimalDual=0;
            for(int h=0; h<ExpSize; h++){
                    AuxOptimalDual+= B_Lambda[j][h]*AuxBS[StageModel[i]][Leaf_Label[SampleLeaf[StageModel[i]]]][h];
            }
            LangrangianCut += (Prob[NodeModel[j][0]]/Prob[NodeModel[i][0]])*(DualOptimalLagrangian[j]-AuxOptimalDual);
            for(int h=0; h<ExpSize; h++){
                    LangrangianCut+= (Prob[NodeModel[j][0]]/Prob[NodeModel[i][0]])*(B_Lambda[j][h]*BS[Leaf_Label[SampleLeaf[StageModel[i]]]][h]);
            }
        }
        NodalModels[i].add(PSI[Leaf_Label[SampleLeaf[StageModel[i]]]]-LangrangianCut>=0);
        LagrangianModels[i].add(PSI[Leaf_Label[SampleLeaf[StageModel[i]]]]-LangrangianCut>=0);
        LangrangianCut.end();
    }
    else{

         for(int g=1; g<=NbLeaf; g++){
             IloExpr LangrangianCut (env);
             for (int j=ChildrenModel[i][1]; j<=ChildrenModel[i][c]; j++){
                 IloNum AuxOptimalDual=0;
                 for(int h=0; h<ExpSize; h++){
                         AuxOptimalDual+= B_Lambda[j][h]*AuxBS[StageModel[i]][Leaf_Label[SampleLeaf[StageModel[i]]]][h];
                 }
                 LangrangianCut += (Prob[NodeModel[j][0]]/Prob[NodeModel[i][0]])*(DualOptimalLagrangian[j]-AuxOptimalDual);
                 for(int h=0; h<ExpSize; h++){
                         LangrangianCut+= (Prob[NodeModel[j][0]]/Prob[NodeModel[i][0]])*(B_Lambda[j][h]*BS[g][h]);
                 }
             }
             NodalModels[i].add(PSI[g]-LangrangianCut>=0);
             LagrangianModels[i].add(PSI[g]-LangrangianCut>=0);
             LangrangianCut.end();
         }
    }

}

void LLagrangianCutsGeneration(IloEnv env, int i, int ExpSize, int MultiCuts){
    if(MultiCuts==0){

        IloExpr LangrangianCut (env);
        for (int j=ChildrenModel[i][1]; j<=ChildrenModel[i][c]; j++){
            IloNum AuxOptimalDual=0;
            for(int h=0; h<ExpSize; h++){
                    AuxOptimalDual+= B_Lambda[j][h]*AuxBS[StageModel[i]][Leaf_Label[SampleLeaf[StageModel[i]]]][h];
            }
            LangrangianCut += (Prob[NodeModel[j][0]]/Prob[NodeModel[i][0]])*(DualOptimalLagrangian[j]-AuxOptimalDual);
            for(int h=0; h<ExpSize; h++){
                    LangrangianCut+= (Prob[NodeModel[j][0]]/Prob[NodeModel[i][0]])*(B_Lambda[j][h]*LBS[Leaf_Label[SampleLeaf[StageModel[i]]]][h]);
            }
        }
        LinearModels[i].add(PSI[Leaf_Label[SampleLeaf[StageModel[i]]]]-LangrangianCut>=0);
        LangrangianCut.end();
    }
    else{
        for(int g=1; g<=NbLeaf; g++){
            IloExpr LangrangianCut (env);
            for (int j=ChildrenModel[i][1]; j<=ChildrenModel[i][c]; j++){
                IloNum AuxOptimalDual=0;
                for(int h=0; h<ExpSize; h++){
                        AuxOptimalDual+= B_Lambda[j][h]*AuxBS[StageModel[i]][Leaf_Label[SampleLeaf[StageModel[i]]]][h];
                }
                LangrangianCut += (Prob[NodeModel[j][0]]/Prob[NodeModel[i][0]])*(DualOptimalLagrangian[j]-AuxOptimalDual);
                for(int h=0; h<ExpSize; h++){
                        LangrangianCut+= (Prob[NodeModel[j][0]]/Prob[NodeModel[i][0]])*(B_Lambda[j][h]*LBS[g][h]);
                }
            }
            LinearModels[i].add(PSI[g]-LangrangianCut>=0);
            LangrangianCut.end();

        }
    }
}

//void UpdateModels(IloEnv env, int n){
//    if(StrengthenedBendersCuts.getSize()>0){
////      NodalModels[n].add(StrengthenedBendersCuts[M-n]);
////      LinearModels[n].add(LStrengthenedBendersCuts[M-n]);
////      LagrangianModels[n].add(StrengthenedBendersCuts[M-n]);
//        NodalModels[n].add(StrengthenedBendersCuts);
//        LinearModels[n].add(LStrengthenedBendersCuts);
//        LagrangianModels[n].add(StrengthenedBendersCuts);
//    }

//    if(IntegerOptimalityCuts.getSize()>0){
////        NodalModels[n].add(IntegerOptimalityCuts[M-n]);
////        LinearModels[n].add(LIntegerOptimalityCuts[M-n]);
////        LagrangianModels[n].add(IntegerOptimalityCuts[M-n]);
//        NodalModels[n].add(IntegerOptimalityCuts);
//        LinearModels[n].add(LIntegerOptimalityCuts);
//        LagrangianModels[n].add(IntegerOptimalityCuts);
//    }

//    if(LagrangianCuts.getSize()>0){
////        NodalModels[n].add(LagrangianCuts[M-n]);
////        LinearModels[n].add(LLagrangianCuts[M-n]);
////        LagrangianModels[n].add(LagrangianCuts[M-n]);
//        NodalModels[n].add(LagrangianCuts);
//        LinearModels[n].add(LLagrangianCuts);
//        LagrangianModels[n].add(LagrangianCuts);
//    }
//}

