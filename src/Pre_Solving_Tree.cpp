#ifndef IndINCLUDE
#define IndINCLUDE
#include "Header.h"
#endif

//Find the stage of all nodes n
void find_stage_Tree(IloEnv env) {
        Stage_Tree = IloIntArray(env, N+1);
	int n = 0;
	for (int s= 1; s<=Nbstage ; s++){
		int count = 0;
		while (count < p* pow ((double)c, s-1)) {
                        Stage_Tree[n] = s;
			n++;
			count++;
		}
	}
}

// record, for each node n, 
//    ** its node parent, 
//    ** the length of the path between one of its immediate successors and a leaf node
//    ** whether it is a leaf node or not
//    ** the number of leaf nodes in the set L(n)
void pre_compute_Tree (IloEnv env) {
        Parent_Tree = IloIntArray(env, N+1);
        Horizon_Tree = IloIntArray(env, N+1);		//The length of Path from its successor to the leaf node
        Test_Leaf_Tree = IloIntArray(env, N+1);		//Binary to see if n is a leaf-node
        Num_Leaf_Tree = IloIntArray(env, N+1);		//Number of leaves in the leaf set of node n

	//For node zero (for notation a_0=0)
        Parent_Tree[0]=-1;
        Horizon_Tree[0]= p-1 + (Nbstage -1)*p;
        Test_Leaf_Tree[0]=0;
        Num_Leaf_Tree[0]= (int) pow(c, Nbstage-1) ;
        SetLeaf_Tree = IloIntArray(env, Num_Leaf_Tree[0]);		//Set of leaf nodes

	// for nodes 1 to N
        for (int n=1; n<=N_Tree; n++){
		// parent node
		if (c==1){
                        Parent_Tree[n] = n-1;
		}
		else {
			if (n%p != 0) {
                                Parent_Tree[n] = n-1;
			}
			else {
					int z = floor (((double)n/p)/c);
					int r = (n/p)%c;
                                        Parent_Tree[n] = n - (z*(c-1)*p + 1) - p* max (0, r-1);
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

                Horizon_Tree[n] = count + (Nbstage - Stage_Tree[n])*p;

		//check whether node n is a leaf node or not
                Test_Leaf_Tree[n]=0;
		int aux=0;
		if( n+(p*(c-1)*floor((double)n/p))+p > N && n%p == p-1) {
                        Test_Leaf_Tree[n] = 1;
                        SetLeaf_Tree[aux]=n;

			//cout << SetLeaf[aux] << endl;
			aux++;
		}

		//number of leaf nodes of a node
                Num_Leaf_Tree[n]= (int) pow (c, Nbstage - Stage_Tree[n] );
	}
}

//First_bud(n) is the first node (smallest label) in the last period of the stage Stage[n]+1 which is connected to n. If Stage[n]=NbStage then it is the first leaf-node

int first_bud_Tree (int n){
	int f_b = 0;
        if (Stage_Tree[n] < Nbstage){
		int temp = n;
		while (temp % p != p - 1 ) { 
			temp = temp + 1 ;
		}
		if (temp % p == p-1) {
			f_b = temp + (p* (c-1) * floor ((double)temp / (double)p)) + p;
		}
	}
        if (Stage_Tree[n] == Nbstage){
		f_b = n;
		while (f_b % p != p - 1 ) { 
			f_b = f_b + 1 ;
		}
	}
	return f_b;
}

//First leaf-node in the leaf set of n
int first_leaf_Tree (int n){
	int f_l = 0;
        int temp = first_bud_Tree(n);
	while (Test_Leaf [temp] == 0){
                temp = first_bud_Tree(temp);
	}
	f_l = temp;
	return f_l;
}

//Find every leaf-node in the leaf set
void find_leaf_Tree (IloEnv env){
	for (int n=0; n<=N; n++){
                Leaf_Tree[n][1] = first_leaf_Tree (n);
		//cout << "Leaf" << n << 1 << "=" << Leaf[n][1] << endl; 
                for (int i = 2; i <= Num_Leaf_Tree [n]; i++){
                        Leaf_Tree[n][i] = Leaf_Tree[n][1] + (i-1)*p;
			//cout << "Leaf" << n << i << "=" << Leaf[n][i] << endl; 
		}
	}
}

//Find immediate children of n
void find_children_Tree (IloEnv env){
        Children_Tree = IloIntArray2(env, N+1);
	for (int n=0; n<=N; n++){
                Children_Tree[n] = IloIntArray(env, c+1);
		// if n is not a leaf node
                if (Test_Leaf_Tree[n] == 0){
			if (n%p == p-1){
                                Children_Tree[n][1] = n+ p*(c-1)* floor((double)n/(double)p) +1;
				for (int t = 2; t<= c; t++){
                                        Children_Tree[n][t]= Children_Tree[n][1] + (t-1)*p;
				}
			}
			else {
                                Children_Tree[n][1]=n+1;
				for (int t = 2; t<= c; t++){
                                        Children_Tree[n][t]=0;
				}
			}
		}
		// if n is a leaf node
		else {                    
			for (int t = 0; t<= c; t++){
                                        Children_Tree[n][t]=0;
			}
		}
	}
}


