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

void  InitialNodalModel (IloEnv env, int m){
//        cout << "Model " << m << endl;
        //OBJECTIVE FUNCTION
        IloExpr CostFunction (env) ;
            for(int n=0; n<=N_SubTree; n++){
                    CostFunction +=Prob[NodeModel[0][n]]*(C[NodeModel[m][n]][0]*Y[n] + H[NodeModel[m][n]][0]*S[n] + g[NodeModel[m][n]]*X[n]);
            }

            if(StageModel[m]!=Nbstage/BkStg){
                for (int j=1;j<=NbLeaf;j++) {
                    CostFunction+= PSI[j];
                }
            }


        NodalModels[m].add(IloMinimize(env, CostFunction ));

	// CONSTRAINTS DEFINITONS
	// link between production and setup variables 
        for(int n=0; n<=N_SubTree; n++){
                NodalModels[m].add(X[n] <= SumDemand_max[NodeModel[m][n]] * Y[n]);
        }
	//Inventory balance 

        // - node 0
        for(int n=0; n<=N_SubTree; n++){
                if(NodeModel[m][n]==0){
                        NodalModels[m].add(S[n] == X[n]  - D[NodeModel[m][n]] );
                }
                else{
                    if(n==0){
                        NodalModels[m].add(S[n] == Z[m] + X[n]  - D[NodeModel[m][n]] );
                    }
                    else{
                        NodalModels[m].add(S[n] == S[Parent_SubModel[n]] + X[n]  - D[NodeModel[m][n]] );
                    }
                }
        }

        if(StageModel[m]!=Nbstage/BkStg){
            for (int j=0;j<=NbLeaf;j++) {
                NodalModels[m].add(PSI[j]>=0);
            }
        }

	CostFunction.end(); 	
}

void Update_NodalModel(IloEnv env, int m, int ExpSize){

    //Binarization of leaf nodes
    if(StageModel[m]!=Nbstage/BkStg){
        for(int j=1; j<=NbLeaf; j++){

            IloExpr BinaryS  (env);
            for(int k=0; k<ExpSize; k++){
                BinaryS+=ExpCoe[k]*BS[j][k];
            }
            NodalModels[m].add(S[SetLeaf_SubModel[j]]==BinaryS);

            BinaryS.end();
        }
    }

    //Binarization of root nodes at each subtree
    if(StageModel[m]!=1) {
        IloExpr BinaryZ  (env);

        for(int k=0; k<ExpSize; k++){
            BinaryZ+=ExpCoe[k]*BZ[m][k];
        }
        NodalModels[m].add(Z[m]==BinaryZ);

        BinaryZ.end();

    }
}

double  SolveNodalModel (IloEnv env, int n, int stg, int Fix_Sol, int Save_Sol){

	IloCplex cplex(NodalModels[n]);
//        if(n==0){cplex.exportModel("NodalModel_0.lp");}
	IloRangeArray CopyConstraints(env);
	//COPY CONSTRAINTS
	if(n>0){
                CopyConstraints.add(Z[n]==AuxS[StageModel[n]-1][SampleLeaf[stg-1]]);
	}

        if(Fix_Sol==1&&n==0){
            for(int i=0; i<=N_SubTree; i++){
//                CopyConstraints.add(X[i]==AuxX[i]);
                 if(AuxY[i]>=0.99){
                    CopyConstraints.add(Y[i]==1);
                }
                else{
                    CopyConstraints.add(Y[i]==0);
                }
            }
        }

	NodalModels[n].add(CopyConstraints);
//         if(n==2){cplex.exportModel("NodalModel_1.lp");}
	// SOLVE THE LINEAR RELAXATION
        if(n==0){
            cplex.setParam(IloCplex::EpGap, 0.01);
        }
        else{
            cplex.setParam(IloCplex::EpGap, 0.005);
        }
        cplex.setParam(IloCplex::TiLim, 30);		//maximum computation time
        cplex.setParam(IloCplex::Threads, 2);
        cplex.setOut(env.getNullStream());
	cplex.solve();
	IloNum LP_Val;
	LP_Val = cplex.getObjValue();

        if(stg>0){                                                      //Condition to estimate initial lower bound
            if(StageModel[n]!=Nbstage/BkStg){
                for (int j=1;j<=NbLeaf;j++) {
                   AuxS[StageModel[n]][SetLeaf_SubModel[j]]=cplex.getValue(S[SetLeaf_SubModel[j]]);
//                   cout << AuxS[StageModel[n]][SetLeaf_SubModel[j]] << endl;
                }


                for (int j=1;j<=NbLeaf;j++) {
                   AuxPSI[j]=cplex.getValue(PSI[j]);
                }
            }
        }
        else{
            for (int j=1;j<=NbLeaf;j++) {
               AuxPSI[j]=cplex.getValue(PSI[j]);
            }
        }

        if(Save_Sol==1&&n==0){
            cplex.getValues(AuxX, X);
            cplex.getValues(AuxY, Y);

        }

        NodalModels[n].remove(CopyConstraints);
	CopyConstraints.end();
        cplex.end();
	
	// cout << "ok Nodal Model" << endl;
	return LP_Val;
}

double  B_SolveNodalModel (IloEnv env, int n, int ExpSize, int Fix_Sol, int Save_Sol ){

	IloCplex cplex(NodalModels[n]);
	
	IloRangeArray CopyConstraints(env);
	//COPY CONSTRAINTS

	if(n>0){
            for(int k=0; k<ExpSize; k++){
                CopyConstraints.add(BZ[n][k]==AuxBS[StageModel[n]-1][Leaf_Label[SampleLeaf[StageModel[n]-1]]][k]);
           }
	}

        if(Fix_Sol==1&&n==0){
//            cout << AuxY << endl;
            for(int i=0; i<=N_SubTree; i++){
//                CopyConstraints.add(X[i]==AuxX[i]);
                if(AuxY[i]>=0.99){
                    CopyConstraints.add(Y[i]>=1);
                }
                else{
                    CopyConstraints.add(Y[i]<=0);
                }
            }
        }
//        To start solving the problem from a initial feasible value
        if(n==0&&Save_Sol==1&&Fix_Sol==0){
             IloNumVarArray startVar(env);
             IloNumArray startVal(env);
             for (int i = 0; i <= N_SubTree; i++) {
                 startVar.add(Y[i]);
                 startVal.add(AuxY[i]);
             }

             cplex.addMIPStart(startVar, startVal, IloCplex::MIPStartAuto,  "secondMIPStart");
             startVal.end();
             startVar.end();
        }
	
	NodalModels[n].add(CopyConstraints);
//        if(n==30){cplex.exportModel("NodalModel.lp");}
	// SOLVE THE LINEAR RELAXATION
        if(n==0){
//            cplex.setParam(IloCplex::EpGap, 0.01);
        }
        else{
//            cplex.setParam(IloCplex::EpGap, 0.005);   
        }
        cplex.setParam(IloCplex::TiLim, 30);
        cplex.setParam(IloCplex::Threads, 2);
//        if(n!=0||Fix_Sol==1){
            cplex.setOut(env.getNullStream());
//        }
	cplex.solve();
	IloNum LP_Val;
	LP_Val = cplex.getObjValue();
//         cout << "Nodal Opt cost = " << LP_Val << endl ;
        if(Save_Sol==1){
            if(StageModel[n]!=Nbstage/BkStg){
                for (int j=1;j<=NbLeaf;j++) {
                    cplex.getValues(AuxBS[StageModel[n]][j], BS[j]);

                    AuxPSI[j]=cplex.getValue(PSI[j]);
                }

            }
        }

        if(Save_Sol==1&&n==0){
            cplex.getValues(AuxX, X);
            cplex.getValues(AuxY, Y);
//            cout << AuxY << endl;
        }

	NodalModels[n].remove(CopyConstraints);
	CopyConstraints.end();
	// cplex.clearModel();
	cplex.end();
	
//         cout << "ok Nodal Model" << endl;
	return LP_Val;
}


double  SolveNodalModel_Upper (IloEnv env, int n, IloNumArray2 UpperX, IloNumArray2 UpperY, IloNumArray2 UpperS){

        IloCplex cplex(NodalModels[n]);
        IloRangeArray CopyConstraints(env);
        //COPY CONSTRAINTS
        if(n>0){
                CopyConstraints.add(Z[n]==AuxS[StageModel[n]-1][SampleLeaf[StageModel[n]-1]]);
        }
        NodalModels[n].add(CopyConstraints);
        // SOLVE THE LINEAR RELAXATION
        if(n==0){
//            cplex.setParam(IloCplex::EpGap, 0.005);
            cplex.setParam(IloCplex::TiLim, 240);		//maximum computation time
        }
        else{
//            cplex.setParam(IloCplex::EpGap, 0.005);
            cplex.setParam(IloCplex::TiLim, 120);		//maximum computation time
        }
        cplex.setParam(IloCplex::Threads, 2);
        cplex.setOut(env.getNullStream());
        cplex.solve();
        IloNum LP_Val;
        LP_Val = cplex.getObjValue();

        if(StageModel[n]>0){                                                      //Condition to estimate initial lower bound
            if(StageModel[n]!=Nbstage/BkStg){
                for (int j=1;j<=NbLeaf;j++) {
                   AuxS[StageModel[n]][SetLeaf_SubModel[j]]=cplex.getValue(S[SetLeaf_SubModel[j]]);
//                   cout << AuxS[StageModel[n]][SetLeaf_SubModel[j]] << endl;
                }


                for (int j=1;j<=NbLeaf;j++) {
                   AuxPSI[j]=cplex.getValue(PSI[j]);
                }
            }
        }
        else{
            for (int j=1;j<=NbLeaf;j++) {
               AuxPSI[j]=cplex.getValue(PSI[j]);
            }
        }


        cplex.getValues(UpperX[n], X);
        cplex.getValues(UpperY[n], Y);
        cplex.getValues(UpperS[n], S);

        NodalModels[n].remove(CopyConstraints);
        CopyConstraints.end();
        cplex.end();

        return LP_Val;
}

