#ifndef IndINCLUDE
#define IndINCLUDE
#include "Header.h"
#endif
using namespace std;
int randu(int a, int b){
        int val = rand()%(b-a+1) + a;
        return val;
}

int main(int argc, char * argv[]){
        cout.precision(12);

        // cout << "//////////////////////////////////////////////////////////////////////////" << endl;
        // cout << "/////                      STARTING NEW INSTANCE                     /////" << endl;
        // cout << "//////////////////////////////////////////////////////////////////////////" << endl;

        // name of the file from which the input data should be read
        strcat(INPUT,"Scenario_Tree_");
        strcat(INPUT,argv[1]);
        strcat(INPUT,".txt");
        cout << INPUT << endl;

        if(stoi(argv[2],0)==1){
            strcat(RESULTS,"CPX_");
        }
        if(stoi(argv[3],0)==1){
            strcat(RESULTS,"Ph-I_");
        }
        if(stoi(argv[4],0)==1){
            strcat(RESULTS,"Ph-II_");
        }
        if(stoi(argv[5],0)==1){
            strcat(RESULTS,"BD_");
        }
        if(stoi(argv[6],0)<=10&&stoi(argv[6],0)>0){
            strcat(RESULTS,"Int=");
            strcat(RESULTS, argv[6]);
            strcat(RESULTS,"_");
        }
        if(stoi(argv[7],0)){
            strcat(RESULTS,"SB_");
        }
        if(stoi(argv[8],0)<=10&&stoi(argv[8],0)>0){
            strcat(RESULTS,"L=");
            strcat(RESULTS, argv[8]);
            strcat(RESULTS,"_");
        }
        if(stoi(argv[9],0)>0){
            strcat(RESULTS,"PathInequalities");
            strcat(RESULTS,"_");
        }
        if(stoi(argv[11],0)>0){
            strcat(RESULTS,"TreeInequalities");
            strcat(RESULTS,"_");
        }
        if(stoi(argv[12],0)>=0){
            strcat(RESULTS,"Time=");
            strcat(RESULTS, argv[12]);
               strcat(RESULTS,"_");
        }
        if(stoi(argv[13],0)==1){
            strcat(RESULTS,"Extended");
        }
        else{
            strcat(RESULTS,"Natural");
        }
        strcat(RESULTS,".txt");

        // PREPROCESSING STEP FOR THE INSTANCE
        IloEnv env;

        // read data on an instance from the INPUT file
        read_data(env);

        // - find the stage to which each node belongs
        find_stage(env);

        pre_compute(env);

        pre_compute_SubModels(env);

        //  - find the leaves in L(n) for each node
        // CAREFUL : allocate memory for array Leaf only once in the main function (and not each time we call find_leaf)
        Leaf = IloIntArray2(env, N_SubTree+1);
        for (int n=0; n<=N_SubTree; n++){
                Leaf[n] = IloIntArray(env, Num_Leaf_SubTree[n]+ 1);
        }

        find_leaf (env);

        //  - find the immediate children of each node
        find_children(env);

        find_nodes(env);

        //-compute Big-M coefficients
        BigM(env);

        Pre_TreeInequalities(env);

        //Extended Formularion 1 or 0;

        Extended = stoi(argv[13],0);
        if(Extended==1){
            ExtendedDemand (env);
        }
        //Setting algorithm's paramters
        int NbIte=1000;                 //Number of interations     
        int ExpSize=10;   		//Number of binary variables into binary expasion
        int SubIte=30;			//Number of iterations into Subgradient method
        int NbScen=1000;
        int LimUpper=0;
        int CPLEX=stoi(argv[2],0);
        int PHASE_I =stoi(argv[3],0);
        int PHASE_II=stoi(argv[4],0);
        int BendersGeneration =stoi(argv[5],0);
        int IntegerGeneration =stoi(argv[6],0);
        int StrengthenedGeneration=stoi(argv[7],0);
        int LagGeneration=stoi(argv[8],0);
        int cutting =stoi(argv[9],0);
        int MultiCuts=stoi(argv[10],0);
        int TreeInequalities = stoi(argv[11],0);
        int TimeLimit_SND=stoi(argv[12],0);          //Total time limit of SND method
        int LimStop=20; //stoi(argv[14],0); 		//Limite of iteration with the same lower bound
        int Print  = stoi(argv[14],0);
        variables_definition(env, ExpSize);

        IloNum AuxLowerBound=0;

        //Array to save the upper and lower bound at each iteration of the SND algorithm
        LowerBound = IloNumArray  (env, NbIte);
        UpperBound = IloNumArray  (env, NbIte);
        TimeIte = IloNumArray  (env, NbIte);

        LowerBound[0]=0;
        UpperBound[0]=0;
        TimeIte[0]=0;

        IloNum AuxGlobalValue=0;

        double Time_SND;
        struct timespec start_SND,end_SND, Aux_start_SND;
        clock_gettime(CLOCK_MONOTONIC,&start_SND);
        if(PHASE_I==1||PHASE_II==1){
            for(int n=0; n<=M; n++){
                InitialNodalModel (env,  n);
                InitialLinearModel (env,  n);
                InitialLagrangianModel (env,  n);
            }
        }
//        cout << "ok models" << endl;

        //SND ALGORITHM PHASE I
        int it=1;
        int ite_check=0;
        int LimStop2 = LimStop;
        int Start_Cutting=0;
        int Start_Tree=0;
        IloNum UpperAverage=0;
        IloNum UpperDeviation=0;
        double Uppercheck=0;
        IloIntArray NbSubIte(env, NbIte);

        if(PHASE_I==1||PHASE_II==1){
//            LowerBound[0] = SolveNodalModel (env,  0, 0, 0, 0);
//            cout << "Intial lower bound " << LowerBound[0] << endl;
        }

//        cout << NodeModel << endl;
        int count_zero=0;

    if(PHASE_I==1){
        do{
            do{
                //Time iteration
                double Time_Ite;
                struct timespec start_Ite;
                clock_gettime(CLOCK_MONOTONIC,&start_Ite);

                //FORWARD STEP
                //Solve the nodal problem from the node 0 to N and compute the Upper bound of the optimal solution
                SampleModel = IloIntArray  (env, Nbstage/BkStg+1);  //Size for each stage after subtree decomposition, and +1 due to stage starts from 1
                SampleLeaf = IloIntArray  (env, Nbstage/BkStg+1);
                SampleModel[0]=-1;
                SampleLeaf[0]=-1;

                //Sampling step
                int n=0;
                int stg=1;
                SampleModel[stg]= n;
                SampleLeaf[stg]= SetLeaf_SubModel[randu(1, NbLeaf)];

                while(stg<=Nbstage/BkStg){
                    if(n>0||count_zero==0){
                        count_zero=1;
                        clock_gettime(CLOCK_MONOTONIC,&Aux_start_SND);
                        IntegerCutValue[n] =  SolveNodalModel (env,  n, stg, 0, 1);
                        clock_gettime(CLOCK_MONOTONIC,&end_SND);
                        Time_SND= BILLION*(end_SND.tv_sec - Aux_start_SND.tv_sec)+end_SND.tv_nsec -Aux_start_SND.tv_nsec;
//                        cout << "Nodal" << " " <<Time_SND/pow((double)10,9) << endl;
                    }
                    n=ChildrenModel[n][randu(1,c)];
                    stg++;
                    if(stg>=Nbstage/BkStg){break;}
                    SampleModel[stg]=n;
                    SampleLeaf[stg]= SetLeaf_SubModel[randu(1, NbLeaf)];
                }
    //             cout << "************************************************ END Forward Step *****************************************" << endl;

                n=M;

                for(int s=Nbstage/BkStg; s>1; s--){
                    while(StageModel[n]==s&&n>0){
                        //We solve the linear realxation and we collect the duals values of the copy constraints
                        clock_gettime(CLOCK_MONOTONIC,&Aux_start_SND);
                        OptimalLinear[n] = SolveLinearModel (env, n, s, Start_Cutting, Start_Tree) ;
    //                    cout << AuxS[StageModel[n]-1][SampleLeaf[s-1]] << endl;
    //                    cout<<Duals[n] << endl;
    //                    cout << OptimalLinear[n] << endl;
                        AuxLambda[n]=-Duals[n];
                        clock_gettime(CLOCK_MONOTONIC,&end_SND);
                        Time_SND= BILLION*(end_SND.tv_sec - Aux_start_SND.tv_sec)+end_SND.tv_nsec -Aux_start_SND.tv_nsec;
//                        cout << "Linear" << " " <<Time_SND/pow((double)10,9) << endl;
    //                    IntegerCutValue[n] =  SolveNodalModel (env,  n, s, 0, 0);

                        // We solve the Semi-Lagrangian relaxation using the duals values obtaind by the linear relaxation
                        clock_gettime(CLOCK_MONOTONIC,&Aux_start_SND);
                        if(StrengthenedGeneration==1) {
                            OptimalLagrangian[n] = SolveLagrangianModel (env, n);
                        }
                        clock_gettime(CLOCK_MONOTONIC,&end_SND);
                        Time_SND= BILLION*(end_SND.tv_sec - Aux_start_SND.tv_sec)+end_SND.tv_nsec -Aux_start_SND.tv_nsec;
//                        cout << "Lagrangian" << " " <<Time_SND/pow((double)10,9) << endl;
    //                    cout << OptimalLagrangian[n] << endl;
                        n--;

                        clock_gettime(CLOCK_MONOTONIC,&end_SND);
                        Time_SND= BILLION*(end_SND.tv_sec - start_SND.tv_sec)+end_SND.tv_nsec -start_SND.tv_nsec;
                        if(Time_SND/pow((double)10,9)>=TimeLimit_SND){break;}

                    }

                    //We generate and save the Strengthened Benders Cuts for the stage s-1
                    int i=n;
                    while(i>=0&&StageModel[i]==s-1){
                        // Generate and Save StrengthenedBendersCuts
                        if(StrengthenedGeneration==1) {
                            StrengthenedBendersCutsGeneration(env, i, MultiCuts);

                        }

                        if(BendersGeneration==1) {
                            BendersCutsGeneration(env, i, MultiCuts);

                        }

                        i--;
                    }

                    clock_gettime(CLOCK_MONOTONIC,&end_SND);
                    Time_SND= BILLION*(end_SND.tv_sec - start_SND.tv_sec)+end_SND.tv_nsec -start_SND.tv_nsec;
                    if(Time_SND/pow((double)10,9)>=TimeLimit_SND){break;}

                }//next stage
    //             cout << "************************************************ END Backward Step *****************************************" << endl;
                clock_gettime(CLOCK_MONOTONIC,&Aux_start_SND);
                IloNum Fix_Solution = SolveNodalModel (env, 0, 0, 1, 1);

                if(Fix_Solution-0.001>LowerBound[it-1]){
                    SolveLinearModel (env, 0, 1, 1, 0) ;
                    LowerBound[it] = SolveNodalModel (env,  0, 1, 0, 1);
                }
                else{
                    LowerBound[it]=LowerBound[it-1];
                }

                clock_gettime(CLOCK_MONOTONIC,&end_SND);
                Time_SND= BILLION*(end_SND.tv_sec - Aux_start_SND.tv_sec)+end_SND.tv_nsec -Aux_start_SND.tv_nsec;
//                cout << "Lower" << " " <<Time_SND/pow((double)10,9) << endl;

                if(LowerBound[it]>AuxLowerBound){AuxLowerBound=LowerBound[it];}
    //            Start_Cutting=1;

                ite_check=max(0,it-LimStop);

                clock_gettime(CLOCK_MONOTONIC,&end_SND);
                Time_SND= BILLION*(end_SND.tv_sec - start_SND.tv_sec)+end_SND.tv_nsec -start_SND.tv_nsec;
                Time_Ite= BILLION*(end_SND.tv_sec - start_Ite.tv_sec)+end_SND.tv_nsec -start_Ite.tv_nsec;

                Uppercheck=abs(-LowerBound[it]+UpperAverage/min(LimUpper, it))/LowerBound[it];

//                cout <<it << "\t" /*<< Uppercheck*/<< Fix_Solution << "\t"   << LowerBound[it]  << "\t"<< NbSubIte[it] << "\t" << Time_Ite/pow((double)10,9)  << "\t" << Time_SND/pow((double)10,9) << "\t" << NbInequalities /*<< "\t"<< SampleLeaf << "\t" << SampleModel*/ <<endl;

                //Time iteration
                TimeIte[it]=Time_Ite/pow((double)10,9);

                it++;
                if(Time_SND/pow((double)10,9)>=TimeLimit_SND){break;}
                if(it>=NbIte){break;}

                SampleModel.end();
                SampleLeaf.end();
//                cout << it <<" "  << Start_Tree << " " << Start_Cutting << endl;

            }
            while(LowerBound[ite_check]+0.1<LowerBound[it-1]||it<LimStop);

            if(cutting==1){
                if(Start_Cutting==0){LimStop2=it+LimStop;}
                if(TreeInequalities==1&&Start_Cutting==1&&Start_Tree==0&&it>=LimStop2){LimStop2=it+LimStop;Start_Tree=1;}
                Start_Cutting=1;
            }

            clock_gettime(CLOCK_MONOTONIC,&end_SND);
            Time_SND= BILLION*(end_SND.tv_sec - start_SND.tv_sec)+end_SND.tv_nsec -start_SND.tv_nsec;
            if(Time_SND/pow((double)10,9)>=TimeLimit_SND){break;}
        }
        while(LowerBound[ite_check]+0.1<LowerBound[it-1]||it<LimStop2);
    }

    cout << "End Phase I" << endl;


    //SND ALGORITHM PHASE II
    LimStop=10; 		//Limite of iteration with the same lower bound
    UpperDeviation=0;

    int Aux_it=it;
    count_zero=0;
//    StrengthenedGeneration=0;

    if(PHASE_II==1){
        for(int n=0; n<=M; n++){
            Update_NodalModel (env,  n, ExpSize);
            Update_LinearModel (env,  n, ExpSize);
            Update_LagrangianModel (env,  n, ExpSize);
        }

        do{
            //Time iteration
            double Time_Ite;
            struct timespec start_Ite;
            clock_gettime(CLOCK_MONOTONIC,&start_Ite);

            //FORWARD STEP
            SampleModel = IloIntArray  (env, Nbstage/BkStg+1);  //Size for each stage after subtree decomposition, and +1 due to stage starts from 1
            SampleLeaf = IloIntArray  (env, Nbstage/BkStg+1);
            SampleModel[0]=-1;
            SampleLeaf[0]=-1;

            int n=0;
            int stg=1;
            SampleModel[stg]= n;
            SampleLeaf[stg]= SetLeaf_SubModel[randu(1, NbLeaf)];

            while(stg<=Nbstage/BkStg){
                if(n>0||count_zero==0){
                    count_zero=1;
                    IntegerCutValue[n] =  B_SolveNodalModel (env,  n, ExpSize, 0, 1);
                }

                n=ChildrenModel[n][randu(1,c)];
                stg++;
                if(stg>=Nbstage/BkStg){break;}
                SampleModel[stg]=n;
                SampleLeaf[stg]= SetLeaf_SubModel[randu(1, NbLeaf)];
            }

//            cout << "************************************************ END Forward Step *****************************************" << endl;
            ite_check=max(0,it-LimUpper);
            UpperAverage+=UpperBound[it]-UpperBound[ite_check];

            n=M;
            for(int s=Nbstage/BkStg; s>=1; s--){
                while(StageModel[n]==s&&n>0){

                    clock_gettime(CLOCK_MONOTONIC,&Aux_start_SND);
                    //We solve the linear realxation and we collect the duals values of the copy constraints
                    OptimalLinear[n] = B_SolveLinearModel (env,  n, ExpSize, s, cutting, TreeInequalities, 0) ;
                    for(int h=0; h<ExpSize; h++){
                        B_AuxLambda[h]=-B_Duals[n][h];
                    }
                    clock_gettime(CLOCK_MONOTONIC,&end_SND);
                    Time_SND= BILLION*(end_SND.tv_sec - Aux_start_SND.tv_sec)+end_SND.tv_nsec -Aux_start_SND.tv_nsec;
                    if(Print==1){ cout << "Linear" << " " <<Time_SND/pow((double)10,9) << endl;}

                    // We solve the Semi-Lagrangian relaxation using the duals values obtaind by the linear relaxation
                    clock_gettime(CLOCK_MONOTONIC,&Aux_start_SND);
                    if(StrengthenedGeneration==1){
                        OptimalLagrangian[n] = B_SolveLagrangianModel (env, n);
                    }
                    clock_gettime(CLOCK_MONOTONIC,&end_SND);
                    Time_SND= BILLION*(end_SND.tv_sec - Aux_start_SND.tv_sec)+end_SND.tv_nsec -Aux_start_SND.tv_nsec;
                    if(Print==1){cout << "Strengthened" << " " <<Time_SND/pow((double)10,9) << endl;}

                    clock_gettime(CLOCK_MONOTONIC,&Aux_start_SND);
                    if(it%IntegerGeneration==0||it%LagGeneration==0){
                        IntegerCutValue[n] =  B_SolveNodalModel (env, n, ExpSize, 0, 0);
                    }
                    clock_gettime(CLOCK_MONOTONIC,&end_SND);
                    Time_SND= BILLION*(end_SND.tv_sec - Aux_start_SND.tv_sec)+end_SND.tv_nsec -Aux_start_SND.tv_nsec;
                    if(Print==1){cout << "integer" << " " <<Time_SND/pow((double)10,9) << endl;}


                    clock_gettime(CLOCK_MONOTONIC,&Aux_start_SND);
                    if(it%LagGeneration==0){
                    //****************************************** SUBGRADIENT METHOD*******************************************************
                        //Getting optimal solution of updated nodal problem/*
                            for(int h=0; h<ExpSize; h++){
                                B_AuxLambda[h]=B_Duals[n][h];
                                B_Lambda[n][h]=B_Duals[n][h];
                            }

                        DualOptimalLagrangian[n] = OptimalLagrangian[n];
                        NbSubIte[it] += SubGradient (env, n, SubIte, ExpSize);

                    }
                    n--;
                    clock_gettime(CLOCK_MONOTONIC,&end_SND);
                    Time_SND= BILLION*(end_SND.tv_sec - Aux_start_SND.tv_sec)+end_SND.tv_nsec -Aux_start_SND.tv_nsec;
                    if(Print==1){cout << "Lagrangian" << " " <<Time_SND/pow((double)10,9) << endl;}

                    clock_gettime(CLOCK_MONOTONIC,&end_SND);
                    Time_SND= BILLION*(end_SND.tv_sec - start_SND.tv_sec)+end_SND.tv_nsec -start_SND.tv_nsec;
                    if(Time_SND/pow((double)10,9)>=TimeLimit_SND){break;}
                    if(Print==1){cout << endl;}

                }

                //We generate and save the Strengthened Benders Cuts for the stage s-1
                int i=n;
                while(i>=0&&StageModel[i]==s-1){
                        // Generate and Save Strengthened Benders Cuts
                        if(StrengthenedGeneration==1){
                            B_StrengthenedBendersCutsGeneration(env, i, ExpSize, MultiCuts);
                            B_LStrengthenedBendersCutsGeneration(env, i, ExpSize, MultiCuts);
                        }
                        // Generate and Save Lagrangian Cuts
                        if(it%LagGeneration==0){
                            LagrangianCutsGeneration (env, i, ExpSize, MultiCuts);
                            LLagrangianCutsGeneration (env, i, ExpSize, MultiCuts);
                        }

                        // Generate and Save Integrality Cuts
                        if(it%IntegerGeneration==0){
                            IntegerOptimalityCutsGeneration(env, i, ExpSize, MultiCuts);
                            LIntegerOptimalityCutsGeneration(env, i, ExpSize, MultiCuts);
                        }

                        i--;
                }
//                cout << "Cutgeneration" << endl;

                clock_gettime(CLOCK_MONOTONIC,&end_SND);
                Time_SND= BILLION*(end_SND.tv_sec - start_SND.tv_sec)+end_SND.tv_nsec -start_SND.tv_nsec;
                if(Time_SND/pow((double)10,9)>=TimeLimit_SND){break;}

            }//next stage
//              cout << "************************************************ END Backward Step *****************************************" << endl;
//            OptimalLinear[0] = B_SolveLinearModel(env, 0, ExpSize, 1, 1, 1);
//            SampleLeaf[stg]= SetLeaf_SubModel[randu(1, NbLeaf)];
            clock_gettime(CLOCK_MONOTONIC,&Aux_start_SND);
            IloNum Fix_Solution = B_SolveNodalModel(env, 0, ExpSize, 1, 1);
            if(Fix_Solution-0.01>LowerBound[it-1]){
                LowerBound[it] = B_SolveNodalModel (env,  0, ExpSize, 0, 1);
            }
            else{
                LowerBound[it]=LowerBound[it-1];
            }
            clock_gettime(CLOCK_MONOTONIC,&end_SND);
            Time_SND= BILLION*(end_SND.tv_sec - Aux_start_SND.tv_sec)+end_SND.tv_nsec -Aux_start_SND.tv_nsec;
           if(Print==1){ cout << "Lower" << " " <<Time_SND/pow((double)10,9) << endl;}
            if(LowerBound[it]>AuxLowerBound){AuxLowerBound=LowerBound[it];}
            // if(AuxLowerBound>=AuxUpperBound){break;}

            ite_check=max(0,it-LimStop);
            clock_gettime(CLOCK_MONOTONIC,&end_SND);
            Time_SND= BILLION*(end_SND.tv_sec - start_SND.tv_sec)+end_SND.tv_nsec -start_SND.tv_nsec;
            Time_Ite= BILLION*(end_SND.tv_sec - start_Ite.tv_sec)+end_SND.tv_nsec -start_Ite.tv_nsec;

            Uppercheck=abs(-LowerBound[it]+UpperAverage/min(it,LimUpper))/LowerBound[it];

//            cout <<  it << "\t"  /*<< Uppercheck*/ <<Fix_Solution << "\t" << LowerBound[it] << "\t" << NbSubIte[it] << "\t" << Time_Ite/pow((double)10,9) <<"\t" << Time_SND/pow((double)10,9) << "\t" << NbInequalities << /*" " << SampleLeaf[1] << "\t"<< SampleLeaf[2] <<*/ endl;

            //Time iteration
            TimeIte[it]=Time_Ite/pow((double)10,9);

            it++;
            if(Time_SND/pow((double)10,9)>=TimeLimit_SND){break;}
            if(it>=NbIte){break;}

            SampleModel.end();
            SampleLeaf.end();
        }
        while(LowerBound[ite_check]+0.01<LowerBound[it-1]||it<Aux_it+LimStop);
    }

    double Aux_TimeSND=Time_SND;
    TimeLimit_SND = 900;
    cout << "End SDDIP " << endl;
    IloNumArray SampleUpperBound (env, NbScen+1);
    IloNum Aux_UpperBound = 0;
    int Optimal_Upper=0;
    IloNumArray Aux_NbScen (env, 1);

    if(Nbstage/BkStg==2&&pow(c, Nbstage-1)<=NbScen){
        Aux_UpperBound = Optimal_UpperBound(env, PHASE_I, PHASE_II, ExpSize, NbScen, TimeLimit_SND);
//        cout << "Opt Upper = " << Aux_UpperBound << " "  << endl;
    }
    else{
//        if(PHASE_I==1&&PHASE_II==0){
//            Aux_UpperBound = UpperBound_PhaseI (env, NbScen, SampleUpperBound, TimeLimit_SND, Aux_NbScen);
        if(Nbstage/BkStg>2){
            Aux_UpperBound = UpperBound_ScenarioBased_III (env, NbScen, SampleUpperBound, TimeLimit_SND, Aux_NbScen);
            cout << "Upper Bound Phase I = " << Aux_UpperBound << " " << Aux_NbScen[0] << endl;
        }
        else{
            Aux_UpperBound = UpperBound_ScenarioBased_II (env, NbScen, SampleUpperBound, TimeLimit_SND, Aux_NbScen);
            cout << "Upper Bound Phase I = " << Aux_UpperBound << " " << Aux_NbScen[0] << endl;
        }
//        }
//        else if(PHASE_II==1){
//            Aux_UpperBound = UpperBound_ScenarioBased_II (env, NbScen, SampleUpperBound, TimeLimit_SND, Aux_NbScen);
//           Aux_UpperBound = UpperBound_PhaseII (env, ExpSize, NbScen, SampleUpperBound, TimeLimit_SND, Aux_NbScen);
//           cout << "Upper Bound Phase II = " << Aux_UpperBound << " " << Aux_NbScen[0] << endl;
//        }

        int ite=0;
        IloNum Aux_Deviation = 0;

        while(ite<Aux_NbScen[0]){
            Aux_Deviation+=pow(SampleUpperBound[ite] - Aux_UpperBound, 2);
            ite++;
        }
        Aux_Deviation = Aux_Deviation/(Aux_NbScen[0]-1);
    //    cout << "Varianza " << Aux_Deviation << endl;
        Aux_Deviation=pow(Aux_Deviation,0.5);
    //    cout << "deviation " << Aux_Deviation << endl;
    //    cout << "error " << 1.96*Aux_Deviation/pow(NbScen, 0.5)<< endl;
        Aux_UpperBound = Aux_UpperBound + 1.96*Aux_Deviation/pow(Aux_NbScen[0], 0.5);
    //    cout << "statistical average " <<Aux_UpperBound << endl;
    }
    clock_gettime(CLOCK_MONOTONIC,&end_SND);
    Time_SND= BILLION*(end_SND.tv_sec - start_SND.tv_sec)+end_SND.tv_nsec -start_SND.tv_nsec;

    //Delete Arrays
    cleannig(env);

    double Time_GlobalModel;
    struct timespec start_GM,end_GM;
    clock_gettime(CLOCK_MONOTONIC,&start_GM);
    // Function to compute the gobal optimal value (MSIP)
    IloNumArray LowerBoundCplex (env, 3);
    // IloNumVarArray GlobalS (env, N+1, 0, IloInfinity, ILOFLOAT);
//  IloNum AuxGlobalBinaryValue;
    if(CPLEX==1){
        AuxGlobalValue = GlobalModel (env, LowerBoundCplex);
        cout <<  "Cplex " << endl;
    }
    // AuxGlobalBinaryValue = GlobalBinaryModel (env, ExpSize, ExpCoe, AuxX, X,  AuxY, Y, AuxS, S, L, LowerBoundCplex);
    clock_gettime(CLOCK_MONOTONIC,&end_GM);
    Time_GlobalModel= BILLION*(end_GM.tv_sec - start_GM.tv_sec)+end_GM.tv_nsec -start_GM.tv_nsec;

  cout << "Upper Bound = " << Aux_UpperBound << endl;
  cout << "Optimal Value = " << AuxGlobalValue << endl;
  cout << "Lower Bound = " << AuxLowerBound << endl;

//  if(Aux_UpperBound<AuxLowerBound){Aux_UpperBound=AuxLowerBound;}
//    if(AuxLowerBound<=AuxGlobalValue){cout << "Good job! :)" << endl;}
  // RESULTS  RECORDING
  ofstream fileRESULTS;
  fileRESULTS.open(RESULTS, ios::app);
    if(CPLEX==1){fileRESULTS << argv[0]  << "\t" << "CPLEX" << "\t" <<argv[1]  << "\t" << p <<"\t" << Nbstage  <<"\t" << c  << "\t" << BkStg << "\t" << N+1 << "\t" << (int) p*((1-pow(c,Nbstage))/(1-c))-1 << "\t" << pow(c, Nbstage-1) << "\t" << AuxGlobalValue <<   "\t"<< LowerBoundCplex[1] <<"\t" << LowerBoundCplex[0] << "\t" << Time_GlobalModel/pow((double)10,9) << "\t" << LowerBoundCplex[2] << endl;}
    if(PHASE_I==1&&PHASE_II==0){
        fileRESULTS << argv[0]  << "\t";
        if(stoi(argv[13],0)==1){
            fileRESULTS << "Extended \t";
        }
        else{
            fileRESULTS << "Natural \t";
        }
        fileRESULTS << "Phase_I" << "\t";
        if(stoi(argv[11],0)>0){
            fileRESULTS << "Tree \t";
        }
        else if(stoi(argv[9],0)>0){
            fileRESULTS << "Path \t";
        }
        else{
            fileRESULTS << "NoCut \t";
        }
        fileRESULTS  << argv[1] << "\t" << p  << "\t" << Nbstage  <<"\t" << c  << "\t" << BkStg << "\t" <<  N+1 << "\t" << (int) p*((1-pow(c,Nbstage))/(1-c))-1<< "\t" << pow(c, Nbstage-1) << "\t" << Aux_UpperBound << "\t" << AuxLowerBound  << "\t" << 100*abs(Aux_UpperBound-AuxLowerBound)/AuxLowerBound << "\t" <<Aux_TimeSND/pow((double)10,9) <<"\t"<<Time_SND/pow((double)10,9)<< "\t" << it  << "\t" << NbInequalities << "\t" << Aux_NbScen[0]  << endl;}
    if(PHASE_I==1&&PHASE_II==1){
        fileRESULTS << argv[0]  << "\t";
        if(stoi(argv[13],0)==1){
            fileRESULTS << "Extended \t";
        }
        else{
            fileRESULTS << "Natural \t";
        }
        fileRESULTS << "Phase_I-II" << "\t";
        if(stoi(argv[11],0)>0){
            fileRESULTS << "Tree \t";
        }
        else if(stoi(argv[9],0)>0){
            fileRESULTS << "Path \t";
        }
        else{
            fileRESULTS << "NoCut \t";
        }
        fileRESULTS << argv[1]  << "\t" << p << "\t" << Nbstage  <<"\t" << c  << "\t" << BkStg << "\t" <<  N+1 << "\t" << (int) p*((1-pow(c,Nbstage))/(1-c))-1<< "\t" << pow(c, Nbstage-1) << "\t" << Aux_UpperBound << "\t" << AuxLowerBound  << "\t" << 100*abs(Aux_UpperBound-AuxLowerBound)/AuxLowerBound << "\t" <<Aux_TimeSND/pow((double)10,9) <<"\t"<<Time_SND/pow((double)10,9)<< "\t" << it  << "\t" << NbInequalities << "\t" << Aux_NbScen[0]  <<  endl;}
    if(PHASE_I==0&&PHASE_II==1){fileRESULTS << argv[0]  << "\t" << "Phase_II" << "\t" << argv[1]  << "\t" << p << "\t" << Nbstage  <<"\t" << c  << "\t" << BkStg << "\t" <<  N+1 << "\t" << (int) p*((1-pow(c,Nbstage))/(1-c))-1<< "\t" << pow(c, Nbstage-1) << "\t" << Aux_UpperBound << "\t" << AuxLowerBound  << "\t" << 100*abs(Aux_UpperBound-AuxLowerBound)/AuxLowerBound << "\t" <<Aux_TimeSND/pow((double)10,9) <<"\t"<<Time_SND/pow((double)10,9)<< "\t" << it  << "\t" << NbInequalities << "\t" << Aux_NbScen[0]  <<  endl;}
  fileRESULTS.close();

  D.end(); Prob.end(); g.end(); SumDemand_max.end(); ExpCoe.end(); LowerBound.end();
  UpperBound.end(); TimeIte.end(); NbSubIte.end();
//      << AuxGlobalValue <<   "\t"  <<"\t" << LowerBoundCplex[0] << "\t" << Time_GlobalModel/pow((double)10,9)
}

//if(Leaf_Selection>0&&it>=3&&LowerBound[it-1]+0.1>=LowerBound[it]){
//    int s = randu(1,Nbstage/BkStg-1);
//    Count_Stage[s]+=1;
//    Tabu_leaf[s][Leaf_Label[SampleLeaf[s]]]=1;

//    if(Count_Stage[s]>=NbLeaf){
//        Count_Stage[s]=0;

//        for(int j=1; j<=NbLeaf; j++){
//            Tabu_leaf[s][j]=0;
//        }
//    }
//}


//if(Leaf_Selection==1){
//    IloNum Min_Leaf=IloInfinity;
//    for (int j=1;j<=NbLeaf;j++){
//        if(Tabu_leaf[stg][j]==0){
//            if(Min_Leaf>AuxPSI[j]){
//                Min_Leaf=AuxPSI[j];
//                Aux_Leaf=SetLeaf_SubModel[j];
//            }
//        }
//    }
//}
//else if(Leaf_Selection==2){
//    IloNum Min_Leaf=IloInfinity;
////                    int Aux_Leaf=NodeModel[n][(p-1)+p*randu(1,c)];
//    for (int j=1;j<=NbLeaf;j++){
//        if(Tabu_leaf[stg][j]==0){
//            if(Min_Leaf>AuxS[SetLeaf_SubModel[j]]){
//                Min_Leaf=AuxS[SetLeaf_SubModel[j]];
//                Aux_Leaf=SetLeaf_SubModel[j];
//            }
//        }
//    }
////                    SampleLeaf[stg]= Aux_Leaf;
//}

//if(Leaf_Selection==1){
//    IloNum Min_Leaf=IloInfinity;
//    for (int j=1;j<=NbLeaf;j++){
//        if(Tabu_leaf[stg][j]==0){
//            if(Min_Leaf>AuxPSI[j]){
//                Min_Leaf=AuxPSI[j];
//                Aux_Leaf=SetLeaf_SubModel[j];
//            }
//        }
//    }
//}
//else if(Leaf_Selection==2){
//    IloNum Min_Leaf=IloInfinity;
////                    int Aux_Leaf=NodeModel[n][(p-1)+p*randu(1,c)];
//    for (int j=1;j<=NbLeaf;j++){
//        if(Tabu_leaf[stg][j]==0){
//            if(Min_Leaf>AuxS[SetLeaf_SubModel[j]]){
//                Min_Leaf=AuxS[SetLeaf_SubModel[j]];
//                Aux_Leaf=SetLeaf_SubModel[j];
//            }
//        }
//    }
////                    SampleLeaf[stg]= Aux_Leaf;
//}



