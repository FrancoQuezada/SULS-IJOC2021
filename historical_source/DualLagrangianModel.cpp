#ifndef IndINCLUDE
#define IndINCLUDE
#include "Header.h"
#include <stdio.h>
#include <stdlib.h>
#endif

char RESULTS_1 [500] = "/home/fquezada/SDDIP-ULS/SubgradientValues.txt";

/////////////////////////////////////////////////////
///       CUT & BRANCH WITH (k,U) INEQUALITIES	 //// 
///	  SKIP LEAF + MAX U + MOST VIOLATED  LEAF    ////
/////////////////////////////////////////////////////

double  SubGradient (IloEnv env, int n, int SubIte, int ExpSize){
        //ofstream fileRESULTS;
        //fileRESULTS.open(RESULTS_1, ios::app);
        IloNum AuxDualValue=0;
        double step;
        double AuxStepSum=0;

        //We solve the Dual Lagrangian relaxation using the subgradient method
        IloNum Iteration=1;
        IloNum AuxOptimalDual=0;
	
        //for(int k=0; k<=2*I+1; k++){
            for(int h=0; h<ExpSize; h++){
                AuxOptimalDual+= B_AuxLambda[h]*AuxBS[StageModel[n]-1][Leaf_Label[SampleLeaf[StageModel[n]-1]]][h];
            }
        //}
	
        DualOptimalLagrangian[n]+=AuxOptimalDual;
        IloNum Zero=0;
	
        while(DualOptimalLagrangian[n]<IntegerCutValue[n]){
                AuxStepSum=0;
	
//                for(int k=0; k<=2*I+1; k++){
                    for(int h=0; h<ExpSize; h++){
                        AuxStepSum+=pow(AuxBS[StageModel[n]-1][Leaf_Label[SampleLeaf[StageModel[n]-1]]][h]-AuxBZ[n][h],2);
                    }
//                }
                if(AuxStepSum<=0.00001){break; }
                step=2*(IntegerCutValue[n]-DualOptimalLagrangian[n])/AuxStepSum;
		
//                for(int k=0; k<=2*I+1; k++){
                    for(int h=0; h<ExpSize; h++){
                        B_AuxLambda[h]=B_AuxLambda[h]+step*(AuxBS[StageModel[n]-1][Leaf_Label[SampleLeaf[StageModel[n]-1]]][h]-AuxBZ[n][h]);
                    }
//                }
		
                AuxOptimalDual=0;
		
//                for(int k=0; k<=2*I+1; k++){
                    for(int h=0; h<ExpSize; h++){
                        AuxOptimalDual+= B_AuxLambda[h]*AuxBS[StageModel[n]-1][Leaf_Label[SampleLeaf[StageModel[n]-1]]][h];
                    }
//                }

//                for(int k=0; k<=2*I+1; k++){
                    for(int h=0; h<ExpSize; h++){
                        B_AuxLambda[h]=-B_AuxLambda[h];
                    }
//                }
		
                AuxDualValue = SolveLagrangianModel_Sub (env, n, ExpSize);
		
//                for(int k=0; k<=2*I+1; k++){
                    for(int h=0; h<ExpSize; h++){
                        B_AuxLambda[h]=-B_AuxLambda[h];
                    }
//                }
		
                if(AuxDualValue+AuxOptimalDual>=DualOptimalLagrangian[n]&&AuxDualValue+AuxOptimalDual<=IntegerCutValue[n]){
                    DualOptimalLagrangian[n]=AuxDualValue+AuxOptimalDual;
//                        for(int k=0; k<=2*I+1; k++){
                        for(int h=0; h<ExpSize; h++){
                            B_Lambda[n][h]=B_AuxLambda[h];
                        }
                    }
//                }
		
                // fileRESULTS << IntegerCutValue[n] << "\t" <<DualOptimalLagrangian[n] << Iteration << endl;
                if(Iteration>=SubIte){break;}
                // CurrentSolution[Iteration+1]=DualOptimalLagrangian[n];
                Iteration++;
                // check=max(Zero,Iteration-5);
        }
	
        // cout << Iteration << endl;
        return Iteration-1;
        //fileRESULTS.close();
}

double  SolveLagrangianModel_Sub (IloEnv env, int n, int ExpSize){

        IloCplex cplex(LagrangianModels[n]);
//        for(int k=0; k<=2*I+1; k++){
            Objective[n].setLinearCoefs(BZ[n], B_AuxLambda);
//        }

        //To start solving the problem from a initial feasible value
        // IloNumVarArray startVar(env);
        // IloNumArray startVal(env);
        // for (int k = 0; k < ExpSize; k++) {
            // startVar.add(Z[n][k]);
            // startVal.add(AuxZ[n][k]);
        // }

            // for (int k = 0; k < ExpSize; k++) {
            // startVar.add(S[n][k]);
            // startVal.add(AuxiliarS[n][k]);
        // }

            // startVar.add(X[n]);
            // startVal.add(AuxX[n]);
            // startVar.add(Y[n]);
            // startVal.add(AuxY[n]);

        // cplex.addMIPStart(startVar, startVal, IloCplex::MIPStartAuto,  "secondMIPStart");
        // startVal.end();
        // startVar.end();
	
        // SOLVE THE LINEAR RELAXATION
//        cplex.setParam(IloCplex::EpGap, 0.0);
        cplex.setParam(IloCplex::TiLim, 30);
        cplex.setParam(IloCplex::Threads, 2);
        cplex.setOut(env.getNullStream());
        cplex.solve();
        IloNum LP_Val;
        LP_Val = cplex.getObjValue();
        // cout << "Lagrangian Opt cost = " << LP_Val << endl ;

        cplex.getValues(AuxBZ[n], BZ[n]);
        cplex.end();

        return LP_Val;
}
