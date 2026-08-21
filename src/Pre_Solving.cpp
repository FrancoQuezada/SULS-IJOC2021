#ifndef IndINCLUDE
#define IndINCLUDE
#include "Header.h"
#endif
void variables_definition(IloEnv env, int ExpSize){

    //Array of Binary expasion's coefficients
    ExpCoe = IloNumArray  (env, ExpSize);
    for(int i=0; i<ExpSize; i++){
            ExpCoe[i]=pow(2, i);
    }

    // DECISION VARIABLES, we create general decision variables to use in each indivual model.
    // - production quantity
    X = IloNumVarArray  (env, N_SubTree+1, 0, IloInfinity,ILOFLOAT);
    // - setup variables
    Y = IloBoolVarArray  (env, N_SubTree+1);
    // - natural inventory of returned products
    S = IloNumVarArray  (env, N_SubTree+1, 0, IloInfinity,ILOFLOAT);

    //Copy inventory varibales
    Z = IloNumVarArray  (env, M+1, 0, IloInfinity,ILOFLOAT);

    //Binary variables for binarization of inventory variables
    BS = IloBoolVarArray2  (env, NbLeaf+1);
    for (int n=0; n<=NbLeaf; n++){
        BS[n]=IloBoolVarArray (env, ExpSize);
    }

    //Copy variables for binarization of inventory variables
    BZ = IloNumVarArray2  (env, M+1);
    for (int n=0; n<=M; n++){
        BZ[n] = IloNumVarArray (env, ExpSize, 0, 1, ILOFLOAT);
    }

    //LINEAR VARIABLES to solve nodal linear relaxations
    // linear setup variables
    LY = IloNumVarArray  (env, N_SubTree+1, 0, 1, ILOFLOAT);

    //Linear variables for binarization of inventory variables
    LBS = IloNumVarArray2  (env, NbLeaf+1);
    for (int n=0; n<=NbLeaf; n++){
        LBS[n]=IloNumVarArray (env, ExpSize, 0, 1, ILOFLOAT);
    }

    if(Extended==1){
        //Variables for Extended formulation
        W = IloNumVarArray3 (env, Ext_Tree+1);
        for (int i=0; i<=Ext_Tree; i++){
            W[i] = IloNumVarArray2 (env, Ext_Tree+1);
            for (int j=0; j<=Ext_Tree; j++){
                W[i][j] = IloNumVarArray (env, Ext_Tree+1, 0, 1, ILOFLOAT);
            }
        }

        U = IloNumVarArray2 (env, Ext_Tree+1);
        for (int j=0; j<=Ext_Tree; j++){
            U[j] = IloNumVarArray (env, Ext_Tree+1, 0, 1, ILOFLOAT);
        }

    }

    //General varible to approximate the expected cost-to-go function
    PSI = IloNumVarArray (env, NbLeaf+1, 0, IloInfinity, ILOFLOAT);

    //Auxiliars variables to save the values of variables at each model
    AuxX = IloNumArray  (env, N_SubTree+1);
    AuxY = IloNumArray  (env, N_SubTree+1);
    AuxLS = IloNumArray (env, N_SubTree+1);

    AuxS = IloNumArray2  (env, Nbstage/BkStg+1);
    AuxZ = IloNumArray (env, M+1);

    AuxBS = IloNumArray3  (env, Nbstage/BkStg+1);
    for(int s=0; s<=Nbstage/BkStg; s++){
        AuxS[s] = IloNumArray (env, N_SubTree+1);
        AuxBS[s] = IloNumArray2  (env, NbLeaf+1);
        for (int n=0; n<=NbLeaf; n++){
            AuxBS[s][n]=IloNumArray (env, ExpSize);
        }
    }

    Sol_Lagrangian_Y = IloNumArray2  (env, M+1);
    AuxBZ = IloNumArray2  (env, M+1);
    for (int n=0; n<=M; n++){
        AuxBZ[n]=IloNumArray (env, ExpSize);
        Sol_Lagrangian_Y[n] = IloNumArray (env, N_SubTree+1);
    }

    AuxPSI = IloNumArray  (env, NbLeaf+1);

    //Array to save nodal value into forward step
    IntegerCutValue = IloNumArray  (env, M+1);

    //Array to save Lagrangian relaxation objective function values
    OptimalLinear = IloNumArray  (env, M+1);
    OptimalLagrangian = IloNumArray  (env, M+1);
    DualOptimalLagrangian = IloNumArray (env, M+1);

    //Array of Duals values obtained by the linear relaxation of the  updated nodal problem
    //PHASE I
    Duals  = IloNumArray (env, M+1);
    Lambda  = IloNumArray (env, M+1);
    AuxLambda  = IloNumArray (env, M+1);

    //PHASE II
    //Array of Duals values obtained by the linear relaxation of the  updated nodal problem
    B_Duals  = IloNumArray2 (env, M+1);
    for(int n=0; n<=M; n++){
        B_Duals[n] = IloNumArray (env, ExpSize);
    }
    B_Lambda = IloNumArray2  (env, M+1);
    for(int n=0; n<=M; n++){
        B_Lambda[n] = IloNumArray (env, ExpSize);
    }

    B_AuxLambda = IloNumArray (env, ExpSize);

    //Array of nodal models for each subproblem
    NodalModels = IloModelArray (env, M+1);
    LinearModels = IloModelArray (env, M+1);
    LagrangianModels = IloModelArray (env, M+1);
    for(int n=0; n<=M; n++){
        NodalModels[n]=IloModel (env);
        LinearModels[n]=IloModel (env);
        LagrangianModels[n]=IloModel (env);
    }

    Objective = IloObjectiveArray (env, M+1);
    for(int n=0; n<=M; n++){Objective[n]=IloObjective (env);}

}

//Find the stage of all nodes n
void find_stage(IloEnv env) {
    Stage = IloIntArray(env, N+1);
    int n = 0;
    for(int i=0; i<p;i++){
        Stage[n] = 1;
        n++;
    }
    for (int s= 2; s<=Nbstage; s++){
        int count = 0;
        while (count < p*c) {
                Stage[n] = s;
                n++;
                count++;
        }
    }

    Stage_SubTree = IloIntArray(env, N_SubTree+1);
    n = 0;
    for (int s= 1; s<=BkStg; s++){
            int count = 0;
            while (count < p* pow ((double)c, s-1)) {
                    Stage_SubTree[n] = s;
                    n++;
                    count++;
            }
    }
//    cout << "Stage_SubTree " << Stage_SubTree << endl;

    StageModel = IloIntArray(env, M+1);
    StageModel[0]=1;
    int s=2;
    int m=1;
    for (int s= 2; s<=Nbstage/BkStg ; s++){
        int auxs=0;
        while(auxs<c){
            StageModel[m]=s;
            m++;
            auxs++;
        }
    }
//    cout << "StageModel  " <<StageModel << endl;

}

void pre_compute_SubModels (IloEnv env) {

    Parent_SubModel = IloIntArray (env, N_SubTree +1);
    Parent_SubModel[0] = -1;
    SetLeaf_SubModel = IloIntArray(env, pow(c, BkStg-1)+1);		//Set of leaf nodes
    Leaf_Label = IloIntArray(env, N_SubTree+1);                 //Number of leaves in the leaf set of node n
    Num_Leaf_SubTree = IloIntArray(env, N_SubTree+1);		//Number of leaves in the leaf set of node n in each subtree
    SubTree_Test_Leaf = IloIntArray(env, N_SubTree+1);
    Num_Leaf_SubTree[0] =pow(c, BkStg-1);

    int aux=1;
    for (int n=1; n<=N_SubTree  ; n++){
            // parent node
            if (c==1){
                    Parent_SubModel[n] = n-1;
            }
            else {
                    if (n%p != 0) {
                            Parent_SubModel[n] = n-1;
                    }
                    else {
                                    int z = floor (((double)n/p)/c);
                                    int r = (n/p)%c;
                                    Parent_SubModel[n] = n - (z*(c-1)*p + 1) - p* max (0, r-1);
                            }
            }
            //cout << Parent[n] << endl;
            // remaining horizon length at node n
            int temp = n;
            int count = 0;
            while (temp % p != p-1){
                    count++;
                    temp++;
            }

            //check whether node n is a leaf node or not
            SubTree_Test_Leaf[n]=0;
            if( n+(p*(c-1)*floor((double)n/p))+p > N_SubTree && n%p == p-1) {
                    SubTree_Test_Leaf[n]=1;
                    SetLeaf_SubModel[aux]=n;
                    Leaf_Label[n]=aux;
                    aux++;
            }

            //number of leaf nodes of a node
            Num_Leaf_SubTree[n]= (int) pow (c, BkStg - Stage_SubTree[n] );
    }
//    cout << "/////////////////////" << endl;
//    cout << Parent_SubModel << endl;
//    cout << "/////////////////////" << endl;
//    cout << SetLeaf_SubModel << endl;
//    cout << "/////////////////////" << endl;
//    cout << Leaf_Label << endl;
//    cout << "/////////////////////" << endl;
//    cout << Num_Leaf_SubTree << endl;
//    cout << "/////////////////////" << endl;
//    cout << "Subtree Test Leaf "<<SubTree_Test_Leaf << endl;
//    cout << "/////////////////////" << endl;
}

// record, for each node n, 
//    ** its node parent, 
//    ** the length of the path between one of its immediate successors and a leaf node
//    ** whether it is a leaf node or not
//    ** the number of leaf nodes in the set L(n)
void pre_compute (IloEnv env) {
    Parent = IloIntArray2 (env, N+1);
    for(int n=0; n<=N; n++){
        Parent[n] = IloIntArray (env, c+1);
    }


    Test_Leaf = IloIntArray(env, N+1);		//Binary to see if n is a leaf-node
    Num_Leaf = IloIntArray(env, N+1);		//Number of leaves in the leaf set of node n

//    SubTree_Test_Root = IloIntArray(env, N+1);  //Usefull for

    //For node zero (for notation a_0=0)
    for(int n=0; n<=N; n++ ){
        for(int j=0; j<=c; j++){
            Parent[n][j]=-1;
        }
    }
    Test_Leaf[0]=0;
    Num_Leaf[0]= c;
//    SetLeaf = IloIntArray(env, Num_Leaf[0]);		//Set of leaf nodes

    // for nodes 1 to N
    for (int n=1; n<=N; n++){
        if (c==1){
            Parent[n][1] = n-1;
        }
        else {
            if(Stage[n]==1){
                for (int j=1;j<=c;j++) {
                    Parent[n][j] = n-1;
                }
            }
            else if(Stage[n]==2){
                if (n%p != 0) {
                    for (int j=1;j<=c;j++) {
                        Parent[n][j] = n-1;
                    }
                }
                else {
                    int z = floor (((double)n/p)/c);
                    int r = (n/p)%c;
                    for (int j=1;j<=c;j++) {
                        Parent[n][j] = n - (z*(c-1)*p + 1) - p* max (0, r-1);

                    }
                }
            }
            else {
                if (n%p != 0) {
                    for (int j=1;j<=c;j++) {
                        Parent[n][j] = n-1;
                    }
                }
                else {
                    int auxn=(p-1)+(Stage[n]-2)*c*p;
                    for(int j=c; j>0;j--){
                        Parent[n][j] = auxn;
                        auxn-=p;
                    }
                }
            }

            //check whether node n is a leaf node or not
            Test_Leaf[n]=0;
            int aux=0;
            if(Stage[n]==Nbstage&&n%p==p-1) {
                    Test_Leaf[n] = 1;
//                    SetLeaf[aux]=n;
                    aux++;
            }

//            if(Stage[n]%BkStg==0&&n%p==p-1){
//                SubTree_Test_Leaf[n] = 1;
//            }

//            SubTree_Test_Root[0] = 1;
//            if(Stage[n]%BkStg==1&&n%p==0){
//                SubTree_Test_Root[n] = 1;
//            }

            //number of leaf nodes of a node
            Num_Leaf[n]= c;
        }

    }
//    cout << "****************" << endl;
//    cout << Num_Leaf << endl;
//    cout << "////////////////" << endl;
//    cout << "ParentNode "<< Parent << endl;


    ParentModel = IloIntArray2 (env, M+1);
    for(int n=0; n<=M; n++){
            ParentModel[n] = IloIntArray (env, c+1);
    }

    for(int j=0; j<=c; j++){
        ParentModel[0][j]=-1;
    }

    for(int m=1; m<=M; m++){
        // parent Model
        if (StageModel[m]==2){
            for(int j=0; j<=c; j++){
                ParentModel[m][j]=-1;
            }
            ParentModel[m][1] = 0;
        }
        else {
            ParentModel[m][0]=-1;
            for(int j=1; j<=c; j++){
                ParentModel[m][j]= (StageModel[m]-3)*c+j;
            }
        }
    }

//cout << "ParentModel " << ParentModel << endl;

}

int first_bud (int n){
        int f_b = 0;

        if (Stage_SubTree[n] < BkStg){

                int temp = n;
                while (temp % p != p - 1 ) {
                        temp = temp + 1 ;
                }
                if (temp % p == p-1) {
                        f_b = temp + (p* (c-1) * floor ((double)temp / (double)p)) + p;
                }

        }
        if (Stage_SubTree[n] == BkStg){
                f_b = n;
                while (f_b % p != p - 1 ) {
                        f_b = f_b + 1 ;
                }
        }
        return f_b;
}

//First leaf-node in the leaf set of n
int first_leaf (int n){
        int f_l = 0;

        int temp = first_bud (n);

        while (SubTree_Test_Leaf [temp] == 0){
                temp = first_bud (temp);
        }
        f_l = temp;

        return f_l;
}

//Find every leaf-node in the leaf set
void find_leaf (IloEnv env){
        for (int n=0; n<=N_SubTree; n++){
            Leaf[n][1] = first_leaf (n);

                for (int i = 2; i <= Num_Leaf_SubTree [n]; i++){
			Leaf[n][i] = Leaf[n][1] + (i-1)*p;

		}
	}
//        cout << "Leaf"<< endl;
//        cout << Leaf << endl;
	
}

//Find immediate children of n
void find_children (IloEnv env){
    Children = IloIntArray2(env, N+1);
    for (int n=0; n<=N; n++){
        Children[n] = IloIntArray(env, c+1);
        // if n is not a leaf node
        if (Test_Leaf[n] == 0){
                 if (n%p == p-1){
                    Children[n][0]=-1;
                    Children[n][1] = (p)+(Stage[n]-1)*c*p;
                    for (int t = 2; t<= c; t++){
                            Children[n][t]= Children[n][1] + (t-1)*p;
                    }
                 }
                 else {

                     for (int t = 2; t<= c; t++){
                             Children[n][t]=-1;
                     }
                     Children[n][1]=n+1;

                 }
        }
        // if n is a leaf node
        else {
            for (int t = 0; t<= c; t++){
                Children[n][t]=-1;
            }
        }
    }


    ChildrenModel = IloIntArray2 (env, M+1);
    for (int n=0; n<=M; n++){
        ChildrenModel[n] = IloIntArray(env, c+1);
        // if n is not a leaf node
        if (StageModel[n]*BkStg == Nbstage){
            for (int t = 0; t<= c; t++){
                ChildrenModel[n][t]=-1;
            }

        }
        // if n is a leaf node
        else {
            ChildrenModel[n][0]=-1;
            ChildrenModel[n][1] = (StageModel[n]-1)*c+1;
            for (int t = 2; t<= c; t++){
                    ChildrenModel[n][t]= ChildrenModel[n][1] + (t-1);
            }
        }
    }

//    cout << "ChildrenModel " << ChildrenModel << endl;
}

void BigM(IloEnv env){

	// link between production and setup variables
	SumDemand_max = IloNumArray (env, N+1);

        for (int n=N; n>=0; n--){

            if(Test_Leaf[n]==1){
                SumDemand_max[n] = D[n];
            }
            else if(n%p==p-1){
                IloNum AuxMax = 0;
                for(int j=1; j<=c; j++){
                    if(SumDemand_max[Children[n][j]]>AuxMax){
                        AuxMax=SumDemand_max[Children[n][j]];
                    }
                }
                SumDemand_max[n]=D[n] + AuxMax;
            }
            else{

                SumDemand_max[n] = D[n]+SumDemand_max[Children[n][1]];
            }
//            SumDemand_max[n]=10000;
        }

//        cout << SumDemand_max << endl << endl;
}


// Quick sort algorithm - to sort the RHS values
void quickSort(IloNumArray RHS, int left, int right){

        int i = left, j = right;
        int tmp;
        int Aux_tmp;
        int debut;
        int fin;

        double pivot = RHS[(left + right) / 2];

        /* partition */
        while (i <= j) {
                while (RHS[i] < pivot)
                        i++;
                while (RHS[j] > pivot)
                        j--;
                if (i <= j) {
                        tmp = RHS[i];
                        //Aux_tmp = RHS_Leaf[i];

                        RHS[i] = RHS[j];
                        //RHS_Leaf[i]=RHS_Leaf[j];
                        RHS[j] = tmp;
                        //RHS_Leaf[j] = Aux_tmp;

                        i++;
                        j--;
                }
        };
        /* recursion */
        if (left < j)
                quickSort(RHS, left, j);
        if (i < right)
                quickSort(RHS, i, right);
}

//void Pre_Upper (IloEnv env) {
//     int Nodes  = (int) p*((1-pow(c,Nbstage))/(1-c))-1;

//     UpperIndex = IloNumArray (env, Nodes+1);
//     UpperGlobal = IloNumArray2 (env, Nodes+1);
//     for(int n=0; n<=Nodes; n++){
//         UpperGlobal[n] = IloNumArray (env, 2);
//     }
//     for(int n=0; n<p; n++){
//         UpperIndex[n] = n;
//     }
//     int n=p;
//     int s=2;
//     while(n<=Nodes){
//         //Nodes in stage s
//         int auxs=0;
//         while(auxs<p*pow(c, s-1)){
//                 int auxnode=p+c*p*(s-2);
//                 int auxc=0;
//                 while(auxc<c*p){
//                     UpperIndex[n] = auxnode;

//                     n++;
//                     auxnode++;
//                     auxc++;
//                     auxs++;
//                 }
//         }
//         s++;
//     }

////     cout << "UpperIndex " <<UpperIndex << endl;

//}

void find_nodes (IloEnv env){
    NodeModel = IloIntArray2 (env, M+1);
    for(int m=0; m<=M; m++){
        NodeModel[m]=IloIntArray (env, N_SubTree+1);
    }


    for(int m=0; m<=M; m++){
        if(StageModel[m]==1){
            for(int n=0; n<p; n++){
                NodeModel[m][n]=n;
            }
//            cout << "m " << m << " " << N_SubTree <<  endl;
            int n=p;
            int s=2;
            int auxnode=0;
//            while(Stage[n]<=BkStg){
            while(n<=N_SubTree){
                int auxs=0;
                while(auxs<p*pow(c, s-1)){
                    auxnode=p+c*p*(s-2);
                    int auxc=0;
                    while(auxc<c*p){
//                        cout << n << " "<<auxnode << endl;
                        NodeModel[m][n]=auxnode;
                        n++;
                        auxnode++;
                        auxc++;
                        auxs++;
                    }
                }
                s++;
            }
        }
        else{

            if(StageModel[m]==2){
                for(int n=0; n<p; n++){
                    NodeModel[m][n]=(StageModel[m]-1)*(p*(BkStg)*c)-p*(c-1) +p*(m-1)+ (n);
                }
            }
            else{
                for(int n=0; n<p; n++){
                    NodeModel[m][n]=(StageModel[m]-1)*(p*(BkStg)*c)-p*(c-1) + p*(m%((StageModel[m]-2)*c+1))+ (n);
                }
            }

            int n=p;
            int s=2;
            int auxnode=0;
//            while(Stage[n]<=BkStg){
            while(n<=N_SubTree){
                int auxs=0;
                while(auxs<p*pow(c, s-1)){
                    auxnode=(StageModel[m]-1)*(p*(BkStg)*c)-p*(c-1)+ c*p*(s-1);
                    int auxc=0;
                    while(auxc<c*p){
                        NodeModel[m][n]= auxnode;
                        n++;
                        auxnode++;
                        auxc++;
                        auxs++;
                    }
                }
                s++;
            }

        }

    }

//      cout << "NodeModel " << NodeModel << endl;
}

void ExtendedDemand (IloEnv env){
    Ext_D = IloNumArray2 (env, M+1);
    for(int m=0; m<=M; m++){
        Ext_D[m] = IloNumArray (env, N_SubTree+1);
   }

    for(int m=0; m<=M; m++){
        for(int i=0; i<=N_SubTree; i++){
            int k=i;
            while(k>=0){
                 Ext_D[m][i]+=D[NodeModel[m][k]];
                 k=Parent_SubModel[k];
            }
        }
    }
//    cout << NodeModel << endl;
//    cout << "//////////" << endl;
//    cout << Ext_D << endl;
}
