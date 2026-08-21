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

void  InitialLagrangianModel (IloEnv env, int m){

    //OBJECTIVE FUNCTION
    IloExpr CostFunction (env) ;
        for(int n=0; n<=N_SubTree; n++){
                CostFunction +=Prob[NodeModel[0][n]]*(C[NodeModel[m][n]][0]*Y[n] + H[NodeModel[m][n]][0]*S[n] + g[NodeModel[m][n]]*X[n]);
        }

        if(StageModel[m]!=Nbstage/BkStg){
            for (int j=1;j<=NbLeaf;j++){
                CostFunction+= PSI[j];
            }
        }

    CostFunction-=Z[m];

    Objective[m] = IloMinimize(env);
    Objective[m].setExpr(CostFunction);
    LagrangianModels[m].add(Objective[m]);

        // CONSTRAINTS DEFINITONS
        // link between production and setup variables
    for(int n=0; n<=N_SubTree; n++){
            LagrangianModels[m].add(X[n] <= SumDemand_max[NodeModel[m][n]] * Y[n]);
    }
    //Inventory balance
    // - node 0
    for(int n=0; n<=N_SubTree; n++){
        if(NodeModel[m][n]==0){
                LagrangianModels[m].add(S[n] == X[n]  - D[NodeModel[m][n]] );
        }
        else{
            if(n==0){
                LagrangianModels[m].add(S[n] == Z[m] + X[n]  - D[NodeModel[m][n]] );
            }
            else{
                LagrangianModels[m].add(S[n] == S[Parent_SubModel[n]] + X[n]  - D[NodeModel[m][n]] );
            }
        }
    }

    CostFunction.end();
}

void Update_LagrangianModel(IloEnv env, int m, int ExpSize){

    //Binarization of leaf nodes
    if(StageModel[m]!=Nbstage/BkStg){
        for(int j=1; j<=NbLeaf; j++){
            IloExpr BinaryS  (env);
            for(int k=0; k<ExpSize; k++){
                BinaryS+=ExpCoe[k]*BS[j][k];
            }
            LagrangianModels[m].add(S[SetLeaf_SubModel[j]]==BinaryS);

            BinaryS.end();
        }
    }

    //Binarization of root nodes at each subtree
    if(StageModel[m]!=1) {
        IloExpr BinaryZ  (env);
        for(int k=0; k<ExpSize; k++){
            BinaryZ+=ExpCoe[k]*BZ[m][k];
        }

        LagrangianModels[m].add(Z[m]==BinaryZ);
        BinaryZ.end();


        //OBJECTIVE FUNCTION
        IloExpr CostFunction (env) ;
            for(int n=0; n<=N_SubTree; n++){
                    CostFunction +=Prob[NodeModel[0][n]]*(C[NodeModel[m][n]][0]*Y[n] + H[NodeModel[m][n]][0]*S[n] + g[NodeModel[m][n]]*X[n]);
            }

            for (int j=1;j<=NbLeaf;j++) {
                CostFunction+= PSI[j];
            }


        for(int k=0; k<ExpSize; k++){
            CostFunction -= BZ[m][k];
        }

        LagrangianModels[m].remove(Objective[m]);
        Objective[m] = IloMinimize(env);
        Objective[m].setExpr(CostFunction);
        LagrangianModels[m].add(Objective[m]);

        CostFunction.end();

    }
}

double  SolveLagrangianModel (IloEnv env, int n){

	IloCplex cplex(LagrangianModels[n]);

        Objective[n].setLinearCoef(Z[n], AuxLambda[n]);

//      To start solving the problem from a initial feasible value
//        if(Fix_Sol==0){
             IloNumVarArray startVar(env);
             IloNumArray startVal(env);
             for (int i = 0; i <= N_SubTree; i++) {
                 startVar.add(Y[i]);
                 startVal.add(Sol_Lagrangian_Y[n][i]);
             }

             cplex.addMIPStart(startVar, startVal, IloCplex::MIPStartAuto,  "LagrangianMIPStart");
             startVal.end();
             startVar.end();
//        }

        // SOLVE THE LINEAR RELAXATION
//        cplex.setParam(IloCplex::EpGap, 0.0);
        cplex.setParam(IloCplex::Threads, 2);
        cplex.setParam(IloCplex::TiLim, 120);		//maximum computation time
        cplex.setOut(env.getNullStream());
	cplex.solve();
	IloNum LP_Val;
	LP_Val = cplex.getObjValue();
	// cout << "Lagrangian Opt cost = " << LP_Val << endl ;

        cplex.getValues(Sol_Lagrangian_Y[n], Y);

//        AuxZ[n] = cplex.getValue(Z[n]);

	cplex.end();
	
//         cout << "ok Lagrangian Model" << endl;
	return LP_Val;
}

double  B_SolveLagrangianModel (IloEnv env, int n){

	IloCplex cplex(LagrangianModels[n]);
	// if(n==1){cplex.exportModel("LagrangianModel_1.lp");}
        Objective[n].setLinearCoefs(BZ[n], B_AuxLambda);
//        IloRangeArray CopyConstraints(env);
//        if(n>0){
//            CopyConstraints.add(Z[n]>=AuxS[SamplePath[StageModel[n]-1]]);
//        }
//        LagrangianModels[n].add(CopyConstraints);
	// SOLVE THE LINEAR RELAXATION
//        cplex.setParam(IloCplex::EpGap, 0.0);
        cplex.setParam(IloCplex::TiLim, 120);
        cplex.setParam(IloCplex::Threads, 2);
        cplex.setOut(env.getNullStream());

	cplex.solve();
	IloNum LP_Val;
	LP_Val = cplex.getObjValue();
//         cout << "Lagrangian Opt cost = " << LP_Val << endl ;
        
        cplex.getValues(AuxBZ[n], BZ[n]);

//        LagrangianModels[n].remove(CopyConstraints);
//        CopyConstraints.end();

	// cplex.clearModel();
	cplex.end();
	
	// cout << "ok Lagrangian Model" << endl;
	return LP_Val;
}
