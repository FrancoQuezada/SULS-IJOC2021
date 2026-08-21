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

double  GlobalModel (IloEnv env, IloNumArray LowerBoundCplex){
        int Nodes  = (int) p*((1-pow(c,Nbstage))/(1-c))-1;
	
        IloNumVarArray X (env, Nodes+1, 0, IloInfinity, ILOFLOAT);

        // - setup variables
        IloBoolVarArray Y (env, Nodes+1);

        IloNumVarArray S (env, Nodes+1, 0, IloInfinity, ILOFLOAT);

        IloModel model(env);
        IloCplex cplex(model);

        //OBJECTIVE FUNCTION
        IloExpr CostFunction (env) ;
        for(int n=0; n<p; n++){
                CostFunction +=(C[n][0]*Y[n] + H[n][0]*S[n] + g[n]*X[n]);
        }

        int n=p;
        int s=2;
        int auxnode=0;
        while(n<=Nodes){
            //Nodes in stage s
            int auxs=0;
            while(auxs<p*pow(c, s-1)){
                auxnode=p+c*p*(s-2);
                int auxc=0;
                while(auxc<c*p){
                    CostFunction +=Prob[auxnode]*(C[auxnode][0]*Y[n] + H[auxnode][0]*S[n] + g[auxnode]*X[n]);
                    n++;
                    auxnode++;
                    auxc++;
                    auxs++;
                }
            }
            s++;
        }

	
        model.add(IloMinimize(env, CostFunction ));
//        cout << SumReturn_n << endl << SumDemand_max << endl<< MinPi_n << endl;
        // CONSTRAINTS DEFINITONS
        // link between production and setup variables
        for(int n=0; n<p; n++){
            model.add(X[n] <= SumDemand_max[n] * Y[n]);
        }

        //Inventory balance
        // - node 0
        model.add(S[0] == X[0] - D[0] );
        
        for(int n=1; n<p; n++){
                model.add(S[n] == S[n-1] + X[n]  - D[n] ); 
        }

        n=p;
        s=2;
        while(n<=Nodes){
            //Nodes in stage s
            int auxs=0;
            while(auxs<p*pow(c, s-1)){
                    int auxnode=p+c*p*(s-2);
                    int auxc=0;
                    while(auxc<c*p){
                        model.add(X[n] <= SumDemand_max[auxnode] * Y[n]);

                        int z = floor (((double)n/p)/c);
                        int r = (n/p)%c;

                        if(n%p==0){
                                model.add(S[n] == S[n - (z*(c-1)*p + 1) - p* max (0, r-1)] + X[n]  - D[auxnode] );             
                        }    
                        else{
                                model.add(S[n] == S[n-1] + X[n]  - D[auxnode] ); 
                        }                        

                        n++;
                        auxnode++;
                        auxc++;
                        auxs++;
                    }
            }
            s++;
        }

        // SOLVE THE LINEAR RELAXATION
        cplex.setParam(IloCplex::EpGap, 0.0);
        cplex.setParam(IloCplex::TiLim, 900);		//maximum computation time
        cplex.setParam(IloCplex::Threads, 2);
        cplex.setOut(env.getNullStream());
//        cplex.exportModel("GlobalModel.lp");
        cplex.solve();
        IloNum LP_Val;
        LP_Val = cplex.getObjValue();
        LowerBoundCplex[0] = 100*(cplex.getObjValue()-cplex.getBestObjValue())/cplex.getBestObjValue();
        LowerBoundCplex[1] = cplex.getBestObjValue();
        LowerBoundCplex[2] = cplex.getNnodes();
//        cout << "Opt cost MILP model= " << LP_Val << endl ;
        //cout << LowerBoundCplex << endl;

        //cplex.getValues(AuxS, S);
        //cout << AuxS << endl;

	
        CostFunction.end();
        cplex.end();
        model.end();
        // Env.end();
        return LP_Val;
}


double  Global_LinearModel (IloEnv env, IloNumArray LowerBoundCplex){
        int Nodes  = (int) p*((1-pow(c,Nbstage))/(1-c))-1;

        IloNumVarArray X (env, Nodes+1, 0, IloInfinity, ILOFLOAT);

        // - setup variables
        IloNumVarArray Y (env, Nodes+1, 0, 1, ILOFLOAT);

        IloNumVarArray S (env, Nodes+1, 0, IloInfinity, ILOFLOAT);

        IloModel model(env);
        IloCplex cplex(model);

        //OBJECTIVE FUNCTION
        IloExpr CostFunction (env) ;
        for(int n=0; n<p; n++){
                CostFunction +=(C[n][0]*Y[n] + H[n][0]*S[n] + g[n]*X[n]);
        }

        int n=p;
        int s=2;
        int auxnode=0;
        while(n<=Nodes){
            //Nodes in stage s
            int auxs=0;
            while(auxs<p*pow(c, s-1)){
                auxnode=p+c*p*(s-2);
                int auxc=0;
                while(auxc<c*p){
                    CostFunction +=Prob[auxnode]*(C[auxnode][0]*Y[n] + H[auxnode][0]*S[n] + g[auxnode]*X[n]);
                    n++;
                    auxnode++;
                    auxc++;
                    auxs++;
                }
            }
            s++;
        }


        model.add(IloMinimize(env, CostFunction ));
//        cout << SumReturn_n << endl << SumDemand_max << endl<< MinPi_n << endl;
        // CONSTRAINTS DEFINITONS
        // link between production and setup variables
        for(int n=0; n<p; n++){
            model.add(X[n] <= SumDemand_max[n] * Y[n]);
        }

        //Inventory balance
        // - node 0
        model.add(S[0] == X[0] - D[0] );

        for(int n=1; n<p; n++){
                model.add(S[n] == S[n-1] + X[n]  - D[n] );
        }

        n=p;
        s=2;
        while(n<=Nodes){
            //Nodes in stage s
            int auxs=0;
            while(auxs<p*pow(c, s-1)){
                    int auxnode=p+c*p*(s-2);
                    int auxc=0;
                    while(auxc<c*p){
                        model.add(X[n] <= SumDemand_max[auxnode] * Y[n]);

                        int z = floor (((double)n/p)/c);
                        int r = (n/p)%c;

                        if(n%p==0){
                                model.add(S[n] == S[n - (z*(c-1)*p + 1) - p* max (0, r-1)] + X[n]  - D[auxnode] );
                        }
                        else{
                                model.add(S[n] == S[n-1] + X[n]  - D[auxnode] );
                        }

                        n++;
                        auxnode++;
                        auxc++;
                        auxs++;
                    }
            }
            s++;
        }

        // SOLVE THE LINEAR RELAXATION
        cplex.setParam(IloCplex::EpGap, 0.0);
        cplex.setParam(IloCplex::TiLim, 900);		//maximum computation time
        cplex.setParam(IloCplex::Threads, 2);
        cplex.setOut(env.getNullStream());
//        cplex.exportModel("GlobalModel.lp");
        cplex.solve();
        IloNum LP_Val;
        LP_Val = cplex.getObjValue();
        LowerBoundCplex[0] = 100*(cplex.getObjValue()-cplex.getBestObjValue())/cplex.getBestObjValue();
        LowerBoundCplex[1] = cplex.getBestObjValue();
        LowerBoundCplex[2] = cplex.getNnodes();
//        cout << "Opt cost MILP model= " << LP_Val << endl ;
        //cout << LowerBoundCplex << endl;

        //cplex.getValues(AuxS, S);
        //cout << AuxS << endl;


        CostFunction.end();
        cplex.end();
        model.end();
        // Env.end();
        return LP_Val;
}
