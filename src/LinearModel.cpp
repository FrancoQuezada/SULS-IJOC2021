#ifndef IndINCLUDE
#define IndINCLUDE
#include "Header.h"
#endif

/////////////////////////////////////////////////////
///       CUT & BRANCH WITH (k,U) INEQUALITIES	 //// 
///	  SKIP LEAF + MAX U + MOST VIOLATED  LEAF    ////
/////////////////////////////////////////////////////

void  InitialLinearModel (IloEnv env, int m){

    //OBJECTIVE FUNCTION
    IloExpr CostFunction (env) ;
        for(int n=0; n<=N_SubTree; n++){
                CostFunction +=Prob[NodeModel[0][n]]*(C[NodeModel[m][n]][0]*LY[n] + H[NodeModel[m][n]][0]*S[n] + g[NodeModel[m][n]]*X[n]);
        }

        if(StageModel[m]!=Nbstage/BkStg){
            for (int j=1;j<=NbLeaf;j++) {
                CostFunction+= PSI[j];
            }
        }

    LinearModels[m].add(IloMinimize(env, CostFunction ));

    // CONSTRAINTS DEFINITONS
    // link between production and setup variables
    for(int n=0; n<=N_SubTree; n++){
            LinearModels[m].add(X[n] <= SumDemand_max[NodeModel[m][n]] * LY[n]);
    }
//    cout << "Model " << m << endl;
    //Inventory balance
    // - node 0
    for(int n=0; n<=N_SubTree; n++){
        if(NodeModel[m][n]==0){
                LinearModels[m].add(S[n] == X[n]  - D[NodeModel[m][n]] );
        }
        else{
            if(n==0){
                LinearModels[m].add(S[n] == Z[m] + X[n]  - D[NodeModel[m][n]] );
            }
            else{
                LinearModels[m].add(S[n] == S[Parent_SubModel[n]] + X[n]  - D[NodeModel[m][n]] );
//                cout << NodeModel[m][n] << endl;
            }
        }
    }

    CostFunction.end();

    if(Extended==1){
        //We assume the folowwing notation with respect to the notation in Guan 2014. The Dummy node 0 --> Ext_Tree et node 1 --> 0.
        //Extended Formulation Constraints
        IloExpr Aux_Sum (env);
        for(int k=0; k<=N_SubTree; k++){
            if(Ext_D[m][k]>0){
                Aux_Sum += W[0][Ext_Tree][k];
            }
        }
        LinearModels[m].add(Aux_Sum == 1);                                    //Constraint (12)
        Aux_Sum.end();
//        cout << "Constraint 12" << endl;


        for(int h=1; h<=c; h++){
            int i = Children[p-1][h];
            if(i!=-1){
                for(int j=0; j<=N_SubTree; j++){
                    IloNumArray Testing (env, N_SubTree+1);
                    IloExpr Aux_Sum (env);
                    for(int l=1;l<=Num_Leaf_SubTree[i]; l++){
                        int k=Leaf[i][l];
                        while(k>=i&&Testing[k]!=1){
                            Testing[k]=1;
                            if(Ext_D[m][k]>Ext_D[m][j]&&Ext_D[m][j]>=Ext_D[m][Parent_SubModel[i]]){
                                Aux_Sum+=W[i][j][k];
                            }
                            k=Parent_SubModel[k];
                        }
                    }
                    if(Ext_D[m][j]>=Ext_D[m][i]){
                        Aux_Sum+=U[i][j];
                    }
                    LinearModels[m].add(Aux_Sum-W[0][Ext_Tree][j]==0);                    //Constraints (13)
                    Aux_Sum.end();
                    Testing.end();
                }
            }
        }
//        cout << "Constraint 13" << endl;
        if(BkStg>=3){

            for(int i=0; i<=N_SubTree; i++){
                if(Stage_SubTree[i]>=3){

                    int h = Parent_SubModel[i]; //h --> i-
                    IloNumArray Checking (env, N_SubTree+1);
                    for(int l=1;l<=Num_Leaf_SubTree[h]; l++){
                        int j=Leaf[h][l];
                        while(j>=h&&Checking[j]!=1){
                            Checking[j]=1;


                            IloNumArray Testing (env, N_SubTree+1);
                            IloExpr Aux_Sum (env);
                            for(int m=1;m<=Num_Leaf_SubTree[i]; m++){
                                int k=Leaf[i][m];
                                while(k>=i&&Testing[k]!=1){
                                    Testing[k]=1;
                                    if(Ext_D[m][k]>Ext_D[m][j]&&Ext_D[m][j]>=Ext_D[m][Parent_SubModel[i]]){
                                        Aux_Sum+=W[i][j][k];
                                    }
                                    k=Parent_SubModel[k];
                                }
                            }

                            for(int v=0; v<=N_SubTree; v++){
                                if(Ext_D[m][j]>Ext_D[m][v]&&Ext_D[m][v]>=Ext_D[m][Parent_SubModel[Parent_SubModel[i]]]){
                                    Aux_Sum-=W[Parent_SubModel[i]][v][j];
                                }
                            }

                            if(Ext_D[m][j]>=Ext_D[m][i]){
                                Aux_Sum+=U[i][j];
                            }

                            if(Ext_D[m][j]>=Ext_D[m][Parent_SubModel[i]]){
                                Aux_Sum-=U[Parent_SubModel[i]][j];
                            }

                            LinearModels[m].add(Aux_Sum==0);                                      //Constraints (14)
                            Aux_Sum.end();
                            Testing.end();

                            j=Parent_SubModel[j];
                        }
                    }
                    Checking.end();

                }

            }

//            cout << "Constraint 14" << endl;
            for(int i=0; i<=N_SubTree; i++){
                if(Stage_SubTree[i]>=3){

                    int h = Parent_SubModel[i]; //h --> i-
                    IloNumArray Checking (env, N_Tree+1);
                    for(int l=1;l<=Num_Leaf_SubTree[0]; l++){
                        int j=Leaf[0][l];

                        if(j<Leaf[h][1]||j>Leaf[h][Num_Leaf_SubTree[h]]){
                        while(j>=0&&Checking[j]!=1){
                                Checking[j]=1;


                                IloNumArray Testing (env, N_Tree+1);
                                IloExpr Aux_Sum (env);
                                for(int m=1;m<=Num_Leaf_SubTree[i]; m++){
                                    int k=Leaf[i][m];
                                    while(k>=i&&Testing[k]!=1){
                                        Testing[k]=1;
                                        if(Ext_D[m][k]>Ext_D[m][j]&&Ext_D[m][j]>=Ext_D[m][Parent_SubModel[i]]){
                                            Aux_Sum+=W[i][j][k];
                                        }
                                        k=Parent_SubModel[k];
                                    }
                                }

                                if(Ext_D[m][j]>=Ext_D[m][i]){
                                    Aux_Sum+=U[i][j];
                                }
                                if(Ext_D[m][j]>=Ext_D[m][Parent_SubModel[i]]){
                                    Aux_Sum-=U[Parent_SubModel[i]][j];
                                }
                                LinearModels[m].add(Aux_Sum==0);    //Constraints (15)
                                Aux_Sum.end();
                                Testing.end();

                                j=Parent_SubModel[j];
                            }
                        }
                    }
                    Checking.end();

                }

            }
//            cout << "Constraint 15" << endl;

        }


        //Linking constraints
        //For root node 1-->0


        IloExpr Aux_SumX (env);
        IloExpr Aux_SumY (env);
        for(int k=0; k<=N_SubTree; k++){
            if(Ext_D[m][k]>0){
                Aux_SumX+=W[0][Ext_Tree][k]*(Ext_D[m][k]);
                Aux_SumY+=W[0][Ext_Tree][k];
            }
        }
        LinearModels[m].add(X[0]+Z[m]-Aux_SumX>=0);
//        LinearModels[m].add(LY[0]-Aux_SumY>=0);
        Aux_SumX.end();
        Aux_SumY.end();

        //For any other node
        for(int i=1;i<=N_SubTree; i++){

            IloExpr Aux_Sum (env);
            IloExpr Aux_SumY (env);
            for(int j=0; j<=N_SubTree; j++){
                IloNumArray Testing (env, N_SubTree+1);
                for(int v=1;v<=Num_Leaf_SubTree[i]; v++){
                    int k=Leaf[i][v];
                    while(k>=i&&Testing[k]!=1){
                        Testing[k]=1;
                        if(Ext_D[m][k]>Ext_D[m][j]&&Ext_D[m][j]>=Ext_D[m][Parent_SubModel[i]]){
                            Aux_Sum+=W[i][j][k]*(Ext_D[m][k]-Ext_D[m][j]);
                            Aux_SumY+=W[i][j][k];
                        }
                        k=Parent_SubModel[k];
                    }
                }
                Testing.end();

            }
            LinearModels[m].add(X[i]-Aux_Sum>=0);
            LinearModels[m].add(LY[i]-Aux_SumY==0);
            Aux_Sum.end();
            Aux_SumY.end();

        }
//        cout << "Constraint others" << endl;
    }
}

void Update_LinearModel(IloEnv env, int m, int ExpSize){

    //Binarization of leaf nodes
    if(StageModel[m]!=Nbstage/BkStg){
        for(int j=1; j<=NbLeaf; j++){
            IloExpr BinaryS  (env);
            for(int k=0; k<ExpSize; k++){
                BinaryS+=ExpCoe[k]*LBS[j][k];
            }
            LinearModels[m].add(S[SetLeaf_SubModel[j]]==BinaryS);

            BinaryS.end();
        }
    }

    //Binarization of root nodes at each subtree
    if(StageModel[m]!=1) {
        IloExpr BinaryZ  (env);
        for(int k=0; k<ExpSize; k++){
            BinaryZ+=ExpCoe[k]*BZ[m][k];
        }
        LinearModels[m].add(Z[m]==BinaryZ);
        BinaryZ.end();
    }
}

double  SolveLinearModel (IloEnv env, int n, int stg, int cutting, int it){

	IloCplex cplex(LinearModels[n]);
	
	//COPY CONSTRAINTS
	IloRangeArray CopyConstraints(env);
	
	if(n>0){	
                CopyConstraints.add(Z[n]==AuxS[StageModel[n]-1][SampleLeaf[stg-1]]);
//                cout << AuxS[SampleLeaf[stg-1]] << endl;
	}

	LinearModels[n].add(CopyConstraints);
//         if(n==0){cplex.exportModel("LinearModel_1.lp");}
	// SOLVE THE LINEAR RELAXATION
        cplex.setParam(IloCplex::EpGap, 0.0);
        cplex.setParam(IloCplex::TiLim, 30);		//maximum computation time
        cplex.setParam(IloCplex::Threads, 2);
        cplex.setOut(env.getNullStream());
	cplex.solve();
	IloNum LP_Val;
	LP_Val = cplex.getObjValue();
//        cout << "Linear Opt cost" << n << " = " << LP_Val << endl ;


        if(n>0){
            Duals[n] = cplex.getDual(CopyConstraints[0]);
        }

        if(cutting==1){
           CuttingPlaneGeneration(env, cplex, n);
           cplex.solve();

           if(it==1){
               dijkstra(env, cplex, n, N_SubTree+1, 0);
               cplex.solve();
           }
           LP_Val = cplex.getObjValue();
           if(n>0){
                Duals[n] = cplex.getDual(CopyConstraints[0]);
           }
           //cout << Duals[n] << endl;
       }

 
	//Remove Copy Constraints from the model
        LinearModels[n].remove(CopyConstraints);
	CopyConstraints.end();
	// cplex.clearModel();
	cplex.end();
	
	// cout << "ok Linear Model " << endl;
	return LP_Val;
}

double  B_SolveLinearModel (IloEnv env, int n,  int ExpSize, int stg, int cutting, int it, int Fix_Sol){

	IloCplex cplex(LinearModels[n]);
	
	//COPY CONSTRAINTS
	IloRangeArray CopyConstraints(env);

	if(n>0){
	        for(int k=0; k<ExpSize; k++){
                    CopyConstraints.add(BZ[n][k]==AuxBS[StageModel[n]-1][Leaf_Label[SampleLeaf[stg-1]]][k]);
                }
	}

        if(Fix_Sol==1&&n==0){
//            cout << " Linear "<< AuxY << endl;
            for(int i=0; i<=N_SubTree; i++){
                if(AuxY[i]>=0.99){
                    CopyConstraints.add(LY[i]>=1);
                }
                else{
                    CopyConstraints.add(LY[i]<=0);
                }
            }
        }

	LinearModels[n].add(CopyConstraints);
//        if(n==0){cplex.exportModel("LinearModel_1.lp");}
	// if(n==1){cplex.exportModel("LinearModel_1.lp");}
	// SOLVE THE LINEAR RELAXATION
	cplex.setParam(IloCplex::EpGap, 0.0);
        cplex.setParam(IloCplex::TiLim, 30);		//maximum computation time
	cplex.setParam(IloCplex::Threads, 2);
	cplex.setOut(env.getNullStream());
	cplex.solve();

	IloNum LP_Val;
	LP_Val = cplex.getObjValue();
//         cout << "Linear Opt cost = " << LP_Val << endl ;

        //int Aux=0;
        //for(int k=0; k<ExpSize; k++){
        if(n>0){
            cplex.getDuals(B_Duals[n], CopyConstraints);
        }
            //Aux++;
	//}

        if(cutting==1){
           CuttingPlaneGeneration(env, cplex, n);
           cplex.solve();
           dijkstra(env, cplex, n, N_SubTree+1, 0);
           cplex.solve();
           LP_Val = cplex.getObjValue();

           if(it==1){
               dijkstra(env, cplex, n, N_SubTree+1, 0);
               cplex.solve();
           }
           if(n>0){
                cplex.getDuals(B_Duals[n], CopyConstraints);
           }

           //cout << Duals[n] << endl;
       }

	//Remove Copy Constraints from the model
        LinearModels[n].remove(CopyConstraints);
	CopyConstraints.end();
	// cplex.clearModel();
	cplex.end();
	
//         cout << "ok Linear Model " << endl;
	return LP_Val;
}



void CuttingPlaneGeneration(IloEnv env, IloCplex cplex, int m){

    IloRangeArray CuttingPlane (env);
    IloRangeArray CuttingPlane_Nodal (env);

        //For each node into the subproblem, we look for a (l,S) inequality
        for(int n=N_SubTree; n>=0; n--){
//            cout << n << endl;
            IloNum AuxD=0;
            IloNum Aux_n=n;
            IloExpr LHS(env);
            IloExpr LHS_Nodal(env);
            IloNum viol=0;
            while(Aux_n>=0){
                AuxD+=D[NodeModel[m][Aux_n]];

                if(cplex.getValue(X[Aux_n])<=AuxD*cplex.getValue(LY[Aux_n])){
                    LHS += X[Aux_n];
                    LHS_Nodal += X[Aux_n];
                    viol+=cplex.getValue(X[Aux_n]);
                }
                else{
                    LHS +=  AuxD*LY[Aux_n];
                    LHS_Nodal += AuxD*Y[Aux_n];
                    viol+=  AuxD*cplex.getValue(LY[Aux_n]);
                }

                LHS -= D[NodeModel[m][Aux_n]];
                LHS_Nodal -= D[NodeModel[m][Aux_n]];
                viol -= D[NodeModel[m][Aux_n]];
                Aux_n=Parent_SubModel[Aux_n];
                if(Aux_n<0){break;}
            }

            if(m>0){
                LHS+=Z[m];
                LHS_Nodal+=Z[m];
                viol+=cplex.getValue(Z[m]);
            }

            if(viol<=-0.001){
                //Create inequality
                CuttingPlane.add(LHS>=0);
                CuttingPlane_Nodal.add(LHS_Nodal>=0);


            }

            LHS.end();
            LHS_Nodal.end();
//            if(viol<=-0.001){break;}
//            cout << endl;
        }
    if(m!=0){
        NbInequalities+=CuttingPlane.getSize();
        LinearModels[m].add(CuttingPlane);
        LagrangianModels[m].add(CuttingPlane_Nodal);
    }
    NodalModels[m].add(CuttingPlane_Nodal);
    CuttingPlane.end();
    CuttingPlane_Nodal.end();


}

