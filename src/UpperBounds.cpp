#ifndef IndINCLUDE
#define IndINCLUDE
#include "Header.h"
#endif

void cleannig (IloEnv env){
    Objective.end(); NodalModels.end(); LinearModels.end();
    LagrangianModels.end(); Duals.end(); Lambda.end(); B_AuxLambda.end();
    AuxLambda.end(); IntegerCutValue.end(); OptimalLinear.end();
    OptimalLagrangian.end(); DualOptimalLagrangian.end(); AuxPSI.end();
    AuxX.end(); AuxY.end(); AuxS.end(); AuxLS.end();
    AuxZ.end(); X.end(); Y.end();
    Z.end(); S.end(); BS.end();
    BZ.end(); LY.end(); LBS.end();
}

double Optimal_UpperBound(IloEnv env, int PHASE_I, int PHASE_II, int ExpSize, int NbScen, int TimeLimit_SND){
    double Time_SND;
    struct timespec start_SND,end_SND;
    clock_gettime(CLOCK_MONOTONIC,&start_SND);

    IloNum Aux_UpperBound=0;
    if(PHASE_I==1&&PHASE_II==0){
        IloNum Value_0=0;
        Value_0 = SolveNodalModel (env,  0, 1, 0, 0);
        for (int j=1;j<=NbLeaf;j++) {
           Value_0-= AuxPSI[j];
       }
        IloNum Check_NbScen=0;
        SampleLeaf = IloIntArray  (env, Nbstage/BkStg);
        for(int m=1; m<=M; m++){
            for(int j=1; j<=NbLeaf; j++){
                SampleLeaf[1]=SetLeaf_SubModel[j];
                IloNum Aux_Value = SolveNodalModel (env, m, 2, 0, 0);
                Value_0 += (Prob[NodeModel[m][0]])*Aux_Value;
                Check_NbScen+=pow(c, BkStg-1);
                if(Check_NbScen>=NbScen){break;}

                clock_gettime(CLOCK_MONOTONIC,&end_SND);
                Time_SND= BILLION*(end_SND.tv_sec - start_SND.tv_sec)+end_SND.tv_nsec -start_SND.tv_nsec;
                if(Time_SND/pow((double)10,9)>=TimeLimit_SND){break;}
            }

            if(Check_NbScen>=NbScen){break;}

            clock_gettime(CLOCK_MONOTONIC,&end_SND);
            Time_SND= BILLION*(end_SND.tv_sec - start_SND.tv_sec)+end_SND.tv_nsec -start_SND.tv_nsec;
            if(Time_SND/pow((double)10,9)>=TimeLimit_SND){break;}
        }

        SampleLeaf.end();
        Aux_UpperBound = Value_0;
    }
    else if(PHASE_II==1){
        IloNum Value_0=0;
        Value_0 = B_SolveNodalModel (env,  0, ExpSize, 0, 0);
        for (int j=1;j<=NbLeaf;j++) {
           Value_0-= AuxPSI[j];
       }
        IloNum Check_NbScen=0;
        SampleLeaf = IloIntArray  (env, Nbstage/BkStg);
        for(int m=1; m<=M; m++){
            for(int j=1; j<=NbLeaf; j++){
                SampleLeaf[1]=SetLeaf_SubModel[j];
                IloNum Aux_Value = B_SolveNodalModel (env, m, ExpSize, 0, 0);
                Value_0 += (Prob[NodeModel[m][0]])*Aux_Value;
                Check_NbScen+=pow(c, BkStg-1);
                if(Check_NbScen>=NbScen){break;}

                clock_gettime(CLOCK_MONOTONIC,&end_SND);
                Time_SND= BILLION*(end_SND.tv_sec - start_SND.tv_sec)+end_SND.tv_nsec -start_SND.tv_nsec;
                if(Time_SND/pow((double)10,9)>=TimeLimit_SND){break;}
            }

            if(Check_NbScen>=NbScen){break;}

            clock_gettime(CLOCK_MONOTONIC,&end_SND);
            Time_SND= BILLION*(end_SND.tv_sec - start_SND.tv_sec)+end_SND.tv_nsec -start_SND.tv_nsec;
            if(Time_SND/pow((double)10,9)>=TimeLimit_SND){break;}
        }

        SampleLeaf.end();
        Aux_UpperBound = Value_0;
    }


    return Aux_UpperBound;
}


double UpperBound_PhaseI (IloEnv env, int NbScen, IloNumArray SampleUpperBound, int TimeLimit_SND, IloNumArray Aux_NbScen){

    double Time_SND;
    struct timespec start_SND,end_SND;
    clock_gettime(CLOCK_MONOTONIC,&start_SND);

    int ite=0;
    double Aux_UpperBound=0;
    IloNum Value_0;
    IloNumArray2 ValuesModel (env, M+1);
    for(int n=0; n<=M; n++){
        ValuesModel[n] = IloNumArray (env, N_SubTree+1);
    }

    while(ite<NbScen){

        SampleModel = IloIntArray  (env, Nbstage/BkStg+1);  //Size for each stage after subtree decomposition, and +1 due to stage starts from 1
        SampleLeaf = IloIntArray  (env, Nbstage/BkStg+1);
        SampleModel[0]=-1;
        SampleLeaf[0]=-1;

        int n=0;
        int stg=1;
        SampleModel[stg]= n;
        SampleLeaf[stg]= SetLeaf_SubModel[randu(1, NbLeaf)];
        if(ite==0){
           Value_0 = SolveNodalModel (env,  0, 1, 0, 1);
           for (int j=1;j<=NbLeaf;j++) {
               Value_0-= AuxPSI[j];
           }
        }
        n=ChildrenModel[n][randu(1,c)];
        stg++;
        SampleModel[stg]= n;
        SampleLeaf[stg]= SetLeaf_SubModel[randu(1, NbLeaf)];

        IloNum Aux_SampleUB=0;
        while(stg<=Nbstage/BkStg){
            if(StageModel[n]<=2){
                if(ValuesModel[n][SampleLeaf[StageModel[n]-1]]<=0.001){
                    IloNum Aux_ValueModel = 0;
                    Aux_ValueModel +=  SolveNodalModel (env,  n, StageModel[n], 0, 1);
                    if(StageModel[n]!=Nbstage/BkStg){
                        for (int j=1;j<=NbLeaf;j++) {
                            Aux_ValueModel-= AuxPSI[j];
                        }
                    }
                    ValuesModel[n][SampleLeaf[StageModel[n]-1]]=Aux_ValueModel;
                    Aux_SampleUB+=ValuesModel[n][SampleLeaf[StageModel[n]-1]];
                }
                else{
                    Aux_SampleUB+=ValuesModel[n][SampleLeaf[StageModel[n]-1]];
                }
            }
            else{
                Aux_SampleUB +=  SolveNodalModel (env,  n, StageModel[n], 0, 1);
                if(StageModel[n]!=Nbstage/BkStg){
                    for (int j=1;j<=NbLeaf;j++) {
                        Aux_SampleUB -= AuxPSI[j];
                    }
                }
            }

            n=ChildrenModel[n][randu(1,c)];
            stg++;
            if(stg>Nbstage/BkStg){break;}
            SampleModel[stg]=n;
            SampleLeaf[stg]= SetLeaf_SubModel[randu(1, NbLeaf)];

            clock_gettime(CLOCK_MONOTONIC,&end_SND);
            Time_SND= BILLION*(end_SND.tv_sec - start_SND.tv_sec)+end_SND.tv_nsec -start_SND.tv_nsec;
            if(Time_SND/pow((double)10,9)>=TimeLimit_SND){break;}
        }

        Aux_UpperBound+=Aux_SampleUB + Value_0;
        SampleUpperBound[ite] = Aux_SampleUB + Value_0;
        ite++;
        Aux_NbScen[0] = ite;
        SampleModel.end();
        SampleLeaf.end();
        clock_gettime(CLOCK_MONOTONIC,&end_SND);
        Time_SND= BILLION*(end_SND.tv_sec - start_SND.tv_sec)+end_SND.tv_nsec -start_SND.tv_nsec;
        if(Time_SND/pow((double)10,9)>=TimeLimit_SND){break;}

//        cout << ite << " " << Time_SND/pow((double)10,9) << endl;
    }
    ValuesModel.end();

    return Aux_UpperBound=Aux_UpperBound/Aux_NbScen[0];

}


double UpperBound_PhaseII (IloEnv env, int ExpSize, int NbScen, IloNumArray SampleUpperBound, int TimeLimit_SND, IloNumArray Aux_NbScen){
    double Time_SND;
    struct timespec start_SND,end_SND;
    clock_gettime(CLOCK_MONOTONIC,&start_SND);

    int ite=0;
    IloNum Value_0;
    IloNum Aux_UpperBound=0;
    IloNumArray2 ValuesModel (env, M+1);
    for(int n=0; n<=M; n++){
        ValuesModel[n] = IloNumArray (env, N+1);
    }
    while(ite<NbScen){

        SampleModel = IloIntArray  (env, Nbstage/BkStg+1);  //Size for each stage after subtree decomposition, and +1 due to stage starts from 1
        SampleLeaf = IloIntArray  (env, Nbstage/BkStg+1);
        SampleModel[0]=-1;
        SampleLeaf[0]=-1;

        int n=0;
        int stg=1;
        SampleModel[stg]= n;
        SampleLeaf[stg]= SetLeaf_SubModel[randu(1, NbLeaf)];
        if(ite==0){
           Value_0 = B_SolveNodalModel (env,  0, ExpSize, 0, 1);
           for (int j=1;j<=NbLeaf;j++) {
               Value_0-= AuxPSI[j];
           }
        }
        n=ChildrenModel[n][randu(1,c)];
        stg++;
        SampleModel[stg]= n;
        SampleLeaf[stg]= SetLeaf_SubModel[randu(1, NbLeaf)];

        IloNum Aux_SampleUB=0;
        while(stg<=Nbstage/BkStg){
            if(stg<=2){
                if(ValuesModel[n][SampleLeaf[StageModel[n]-1]]<=0.001){
                    IloNum Aux_ValueModel = 0;
                    Aux_ValueModel +=  B_SolveNodalModel (env,  n, ExpSize, 0, 1);
                    if(StageModel[n]!=Nbstage/BkStg){
                        for (int j=1;j<=NbLeaf;j++) {
                            Aux_ValueModel-= AuxPSI[j];
                        }
                    }
                    ValuesModel[n][SampleLeaf[StageModel[n]-1]]=Aux_ValueModel;
                    Aux_SampleUB+=ValuesModel[n][SampleLeaf[StageModel[n]-1]];
                }
                else{
                    Aux_SampleUB+=ValuesModel[n][SampleLeaf[StageModel[n]-1]];
                }
            }
            else{
                Aux_SampleUB +=  B_SolveNodalModel (env,  n, ExpSize, 0, 1);
                if(StageModel[n]!=Nbstage/BkStg){
                    for (int j=1;j<=NbLeaf;j++) {
                        Aux_SampleUB-= AuxPSI[j];
                    }
                }
            }

            n=ChildrenModel[n][randu(1,c)];
            stg++;
            if(stg>Nbstage/BkStg){break;}
            SampleModel[stg]=n;
            SampleLeaf[stg]= SetLeaf_SubModel[randu(1, NbLeaf)];

            clock_gettime(CLOCK_MONOTONIC,&end_SND);
            Time_SND= BILLION*(end_SND.tv_sec - start_SND.tv_sec)+end_SND.tv_nsec -start_SND.tv_nsec;
            if(Time_SND/pow((double)10,9)>=TimeLimit_SND){break;}
        }

        Aux_UpperBound+=Aux_SampleUB + Value_0;
        SampleUpperBound[ite] = Aux_SampleUB + Value_0;
        ite++;
        Aux_NbScen[0] = ite;
        SampleModel.end();
        SampleLeaf.end();


        clock_gettime(CLOCK_MONOTONIC,&end_SND);
        Time_SND= BILLION*(end_SND.tv_sec - start_SND.tv_sec)+end_SND.tv_nsec -start_SND.tv_nsec;
        if(Time_SND/pow((double)10,9)>=TimeLimit_SND){break;}
//        cout << ite << " " << Time_SND/pow((double)10,9) << endl;
    }
    ValuesModel.end();

    return Aux_UpperBound=Aux_UpperBound/Aux_NbScen[0];

}


double UpperBound_ScenarioBased (IloEnv env, int NbScen, IloNumArray SampleUpperBound, int TimeLimit_SND, IloNumArray Aux_NbScen){

    double Time_SND;
    struct timespec start_SND,end_SND;
    clock_gettime(CLOCK_MONOTONIC,&start_SND);

    int ite=0;
    double Aux_UpperBound=0;
    IloNumArray2 ValuesModel (env, M+1);
    for(int n=0; n<=M; n++){
        ValuesModel[n] = IloNumArray (env, N_SubTree+1);
    }

    IloNumArray2 UpperX (env, M+1);
    IloNumArray2 UpperY (env, M+1);
    IloNumArray2 UpperS (env, M+1);
    for(int m=0;m<=M; m++){
       UpperX[m] = IloNumArray (env, N_SubTree+1);
       UpperY[m] = IloNumArray (env, N_SubTree+1);
       UpperS[m] = IloNumArray (env, N_SubTree+1);
    }
    while(ite<NbScen){

        SampleModel = IloIntArray  (env, Nbstage/BkStg+1);  //Size for each stage after subtree decomposition, and +1 due to stage starts from 1
        SampleLeaf = IloIntArray  (env, Nbstage/BkStg+1);
        SampleModel[0]=-1;
        SampleLeaf[0]=-1;

        int m=0;
        int stg=1;
        SampleModel[stg]= m;
        SampleLeaf[stg]= SetLeaf_SubModel[randu(1, NbLeaf)];
        if(ite==0){
           SolveNodalModel_Upper (env,  m, UpperX, UpperY, UpperS);
        }

        int n = SampleLeaf[1];
        IloNum Value_0=0;
        while(n>=0){
            Value_0+=(C[NodeModel[m][n]][0]*UpperY[m][n] + H[NodeModel[m][n]][0]*UpperS[m][n] + g[NodeModel[m][n]]*UpperX[m][n]);
//            cout << n <<  "\t";
            n=Parent_SubModel[n];
        }
//        cout << "\t" << Value_0 <<"\t"<<endl;

        m=ChildrenModel[m][randu(1,c)];
        stg++;
        SampleModel[stg]= m;
        SampleLeaf[stg]= SetLeaf_SubModel[randu(1, NbLeaf)];

        SolveNodalModel_Upper (env,  m, UpperX, UpperY, UpperS);
        int aux_ite=0;
        for(int l=1; l<=NbLeaf; l++){
            IloNum Aux_SampleUB=0;
            n=SetLeaf_SubModel[l];
            while(n>=0){
                Aux_SampleUB+=(C[NodeModel[m][n]][0]*UpperY[m][n] + H[NodeModel[m][n]][0]*UpperS[m][n] + g[NodeModel[m][n]]*UpperX[m][n]);
//               cout << n <<  "\t";
                n=Parent_SubModel[n];
            }

//            cout <<"\t" << Aux_SampleUB<< "\t"<<endl;

            Aux_UpperBound+=Aux_SampleUB + Value_0;
            SampleUpperBound[ite] = Aux_SampleUB + Value_0;
            clock_gettime(CLOCK_MONOTONIC,&end_SND);
            Time_SND= BILLION*(end_SND.tv_sec - start_SND.tv_sec)+end_SND.tv_nsec -start_SND.tv_nsec;
            if(Time_SND/pow((double)10,9)>=TimeLimit_SND){break;}
//            cout << ite << " " <<SampleUpperBound[ite]<< " " << Time_SND/pow((double)10,9) << endl;
            ite++;
            aux_ite++;
            if(aux_ite>=10);
            if(ite>=NbScen){break;}
        }

        Aux_NbScen[0] = ite;
        SampleModel.end();
        SampleLeaf.end();

        clock_gettime(CLOCK_MONOTONIC,&end_SND);
        Time_SND= BILLION*(end_SND.tv_sec - start_SND.tv_sec)+end_SND.tv_nsec -start_SND.tv_nsec;
        if(Time_SND/pow((double)10,9)>=TimeLimit_SND){break;}
//        cout << ite << " " <<SampleUpperBound[ite] << " "<< Time_SND/pow((double)10,9) << endl;
//        cout << Aux_NbScen << endl;
    }
    UpperX.end();
    UpperY.end();
    UpperS.end();
    ValuesModel.end();

    return Aux_UpperBound=Aux_UpperBound/Aux_NbScen[0];

}


double UpperBound_ScenarioBased_II (IloEnv env, int NbScen, IloNumArray SampleUpperBound, int TimeLimit_SND, IloNumArray Aux_NbScen){

    double Time_SND;
    struct timespec start_SND,end_SND;
    clock_gettime(CLOCK_MONOTONIC,&start_SND);

    int ite=0;
    double Aux_UpperBound=0;
    IloNumArray2 ValuesModel (env, M+1);
    for(int n=0; n<=M; n++){
        ValuesModel[n] = IloNumArray (env, N_SubTree+1);
    }

    IloNumArray2 UpperX (env, M+1);
    IloNumArray2 UpperY (env, M+1);
    IloNumArray2 UpperS (env, M+1);
    for(int m=0;m<=M; m++){
       UpperX[m] = IloNumArray (env, N_SubTree+1);
       UpperY[m] = IloNumArray (env, N_SubTree+1);
       UpperS[m] = IloNumArray (env, N_SubTree+1);
    }
    while(ite<NbScen){

        SampleModel = IloIntArray  (env, Nbstage/BkStg+1);  //Size for each stage after subtree decomposition, and +1 due to stage starts from 1
        SampleLeaf = IloIntArray  (env, Nbstage/BkStg+1);
        SampleModel[0]=-1;
        SampleLeaf[0]=-1;

        int m=0;
        int stg=1;
        SampleModel[stg]= m;
        SampleLeaf[stg]= SetLeaf_SubModel[randu(1, NbLeaf)];
        if(ite==0){
           SolveNodalModel_Upper (env,  m, UpperX, UpperY, UpperS);
        }

        IloNum Value_0=0;
        int n = SampleLeaf[stg];
        while(n>=0){
            Value_0+=(C[NodeModel[m][n]][0]*UpperY[m][n] + H[NodeModel[m][n]][0]*UpperS[m][n] + g[NodeModel[m][n]]*UpperX[m][n]);
            n=Parent_SubModel[n];
        }


        while(stg<Nbstage/BkStg-1){
            stg++;
            m=ChildrenModel[m][randu(1,c)];
            SampleModel[stg]= m;
            SampleLeaf[stg]= SetLeaf_SubModel[randu(1, NbLeaf)];

            SolveNodalModel_Upper (env,  m, UpperX, UpperY, UpperS);

            n = SampleLeaf[stg];
            while(n>=0){
                Value_0+=(C[NodeModel[m][n]][0]*UpperY[m][n] + H[NodeModel[m][n]][0]*UpperS[m][n] + g[NodeModel[m][n]]*UpperX[m][n]);
                n=Parent_SubModel[n];
            }

        }
        stg++;
        if(stg==Nbstage/BkStg){
            m=ChildrenModel[m][randu(1,c)];
            SampleModel[stg]= m;
            SampleLeaf[stg]= SetLeaf_SubModel[randu(1, NbLeaf)];

            SolveNodalModel_Upper (env,  m, UpperX, UpperY, UpperS);

            int aux_ite=0;
            for(int l=1; l<=NbLeaf; l++){
                IloNum Aux_SampleUB=0;
                n=SetLeaf_SubModel[randu(1, NbLeaf)];
                while(n>=0){
                    Aux_SampleUB+=(C[NodeModel[m][n]][0]*UpperY[m][n] + H[NodeModel[m][n]][0]*UpperS[m][n] + g[NodeModel[m][n]]*UpperX[m][n]);
                    n=Parent_SubModel[n];
                }

                Aux_UpperBound+=Aux_SampleUB + Value_0;
                SampleUpperBound[ite] = Aux_SampleUB + Value_0;
                clock_gettime(CLOCK_MONOTONIC,&end_SND);
                Time_SND= BILLION*(end_SND.tv_sec - start_SND.tv_sec)+end_SND.tv_nsec -start_SND.tv_nsec;
                if(Time_SND/pow((double)10,9)>=TimeLimit_SND){break;}
//                cout << ite << " " <<SampleUpperBound[ite]<< " " << Time_SND/pow((double)10,9) << endl;
                ite++;
                aux_ite++;
                if(aux_ite>=100){break;}
                if(ite>=NbScen){break;}
            }

        }


        Aux_NbScen[0] = ite;
        SampleModel.end();
        SampleLeaf.end();

        clock_gettime(CLOCK_MONOTONIC,&end_SND);
        Time_SND= BILLION*(end_SND.tv_sec - start_SND.tv_sec)+end_SND.tv_nsec -start_SND.tv_nsec;
        if(Time_SND/pow((double)10,9)>=TimeLimit_SND){break;}
//        cout << ite << " " <<SampleUpperBound[ite] << " "<< Time_SND/pow((double)10,9) << endl;
//        cout << Aux_NbScen << endl;
    }
    UpperX.end();
    UpperY.end();
    UpperS.end();
    ValuesModel.end();

    return Aux_UpperBound=Aux_UpperBound/Aux_NbScen[0];

}

double UpperBound_ScenarioBased_III (IloEnv env, int NbScen, IloNumArray SampleUpperBound, int TimeLimit_SND, IloNumArray Aux_NbScen){

    double Time_SND;
    struct timespec start_SND,end_SND;
    clock_gettime(CLOCK_MONOTONIC,&start_SND);

    int ite=0;
    double Aux_UpperBound=0;
    IloNumArray2 ValuesModel (env, M+1);
    for(int n=0; n<=M; n++){
        ValuesModel[n] = IloNumArray (env, N_SubTree+1);
    }

    IloNumArray2 UpperX (env, M+1);
    IloNumArray2 UpperY (env, M+1);
    IloNumArray2 UpperS (env, M+1);
    for(int m=0;m<=M; m++){
       UpperX[m] = IloNumArray (env, N_SubTree+1);
       UpperY[m] = IloNumArray (env, N_SubTree+1);
       UpperS[m] = IloNumArray (env, N_SubTree+1);
    }
    while(ite<NbScen){

        SampleModel = IloIntArray  (env, Nbstage/BkStg+1);  //Size for each stage after subtree decomposition, and +1 due to stage starts from 1
        SampleLeaf = IloIntArray  (env, Nbstage/BkStg+1);
        SampleModel[0]=-1;
        SampleLeaf[0]=-1;

        int m=0;
        int n=0;
        int stg=1;
        SampleModel[stg]= m;
        SampleLeaf[stg]= SetLeaf_SubModel[randu(1, NbLeaf)];

        if(ite==0){
           SolveNodalModel_Upper (env,  m, UpperX, UpperY, UpperS);
        }


        IloNum Value_0=0;
        n = SampleLeaf[stg];
//        n = N_SubTree;
        while(n>=0){
            Value_0+=/*Prob[NodeModel[0][n]]**/(C[NodeModel[m][n]][0]*UpperY[m][n] + H[NodeModel[m][n]][0]*UpperS[m][n] + g[NodeModel[m][n]]*UpperX[m][n]);
            n=Parent_SubModel[n];
//            n--;
        }


        while(stg<Nbstage/BkStg-2){
            stg++;
            m=ChildrenModel[m][randu(1,c)];
            SampleModel[stg]= m;
            SampleLeaf[stg]= SetLeaf_SubModel[randu(1, NbLeaf)];

            SolveNodalModel_Upper (env,  m, UpperX, UpperY, UpperS);

            n = SampleLeaf[stg];
//            n=N_SubTree;
            while(n>=0){
                Value_0+=/*Prob[NodeModel[0][n]]**/(C[NodeModel[m][n]][0]*UpperY[m][n] + H[NodeModel[m][n]][0]*UpperS[m][n] + g[NodeModel[m][n]]*UpperX[m][n]);
                n=Parent_SubModel[n];
//                n--;
            }

        }

        stg++;
        if(stg==Nbstage/BkStg-1){
            m=ChildrenModel[m][randu(1,c)];
            SampleModel[stg]= m;
            SampleLeaf[stg]= SetLeaf_SubModel[randu(1, NbLeaf)];

            SolveNodalModel_Upper (env,  m, UpperX, UpperY, UpperS);

            stg++;
            int aux_ite=0;
            for(int h=1; h<=NbLeaf; h++){
                m = SampleModel[stg-1];
                IloNum Value_Aux=0;
                n = SetLeaf_SubModel[h];
//                n= N_SubTree;
                while(n>=0){
                    Value_Aux+=/*Prob[NodeModel[0][n]]**/(C[NodeModel[m][n]][0]*UpperY[m][n] + H[NodeModel[m][n]][0]*UpperS[m][n] + g[NodeModel[m][n]]*UpperX[m][n]);
                    n=Parent_SubModel[n];
//                    n--;
                }
                m=M;
                while(StageModel[m]==Nbstage/BkStg){
                    SampleModel[stg]= m;
                    SolveNodalModel_Upper (env,  m, UpperX, UpperY, UpperS);

                    for(int l=1; l<=NbLeaf; l++){
                        IloNum Aux_SampleUB=0;
                        n=SetLeaf_SubModel[l];
                        while(n>=0){
                            Aux_SampleUB+=(C[NodeModel[m][n]][0]*UpperY[m][n] + H[NodeModel[m][n]][0]*UpperS[m][n] + g[NodeModel[m][n]]*UpperX[m][n]);
                            n=Parent_SubModel[n];
                        }

                        Aux_UpperBound+=Aux_SampleUB + Value_Aux + Value_0;
                        SampleUpperBound[ite] = Aux_SampleUB + Value_Aux + Value_0;
                        clock_gettime(CLOCK_MONOTONIC,&end_SND);
                        Time_SND= BILLION*(end_SND.tv_sec - start_SND.tv_sec)+end_SND.tv_nsec -start_SND.tv_nsec;
                        if(Time_SND/pow((double)10,9)>=TimeLimit_SND){break;}
//                            cout << ite << " " <<SampleUpperBound[ite]<< " " << Time_SND/pow((double)10,9) << endl;
                        ite++;
                        aux_ite++;
                        if(aux_ite>=20){break;}
                        if(ite>=NbScen){break;}
                   }
                   m--;

                   if(aux_ite>=20){break;}
                   if(ite>=NbScen){break;}
                   clock_gettime(CLOCK_MONOTONIC,&end_SND);
                   Time_SND= BILLION*(end_SND.tv_sec - start_SND.tv_sec)+end_SND.tv_nsec -start_SND.tv_nsec;
                   if(Time_SND/pow((double)10,9)>=TimeLimit_SND){break;}
                }
                if(aux_ite>=20){break;}
                if(ite>=NbScen){break;}
                clock_gettime(CLOCK_MONOTONIC,&end_SND);
                Time_SND= BILLION*(end_SND.tv_sec - start_SND.tv_sec)+end_SND.tv_nsec -start_SND.tv_nsec;
                if(Time_SND/pow((double)10,9)>=TimeLimit_SND){break;}
            }

        }

        Aux_NbScen[0] = ite;
        SampleModel.end();
        SampleLeaf.end();

        clock_gettime(CLOCK_MONOTONIC,&end_SND);
        Time_SND= BILLION*(end_SND.tv_sec - start_SND.tv_sec)+end_SND.tv_nsec -start_SND.tv_nsec;
        if(Time_SND/pow((double)10,9)>=TimeLimit_SND){break;}
//        cout << ite << " " <<SampleUpperBound[ite] << " "<< Time_SND/pow((double)10,9) << endl;
//        cout << Aux_NbScen << endl;
    }
    UpperX.end();
    UpperY.end();
    UpperS.end();
    ValuesModel.end();

    return Aux_UpperBound=Aux_UpperBound/Aux_NbScen[0];

}
