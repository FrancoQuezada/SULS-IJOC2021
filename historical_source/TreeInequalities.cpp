#ifndef IndINCLUDE
#define IndINCLUDE
#include "Header.h"
#endif

/////////////////////////////////////////////////////
///       CUT & BRANCH WITH (k,U) INEQUALITIES	 //// 
///	  SKIP LEAF + MAX U + MOST VIOLATED  LEAF    ////
/////////////////////////////////////////////////////

//void dijkstra(int** G,int n,int startnode);

void Pre_TreeInequalities(IloEnv env){
//   int G[N_SubTree][N_SubTree];
//   int n=5;
//   int u=0;
   SumD = IloNumArray2 (env, M+1);
   for(int m=0; m<=M; m++){
    SumD[m] = IloNumArray (env, N_SubTree+2);
   }

   for(int m=0; m<=M; m++){
       for(int i=0;i<=N_SubTree+1;i++){
           if(i==0){
               SumD[m][i]=0;
           }
           else {
               IloNum AuxD=0;
               IloNum Aux_n=i-1;
               while(Aux_n>=0){
                   AuxD+=D[NodeModel[m][Aux_n]];
                   Aux_n=Parent_SubModel[Aux_n];
               }
               SumD[m][i] = AuxD;

           }
       }
   }


   G = IloNumArray3 (env, M+1);
   for(int m=0; m<=M; m++){
    G[m] = IloNumArray2 (env, N_SubTree+2);
       for(int i=0;i<=N_SubTree+1;i++){
        G[m][i] = IloNumArray (env, N_SubTree+2);
       }
   }

   for(int m=0; m<=M; m++){
       for(int i=0;i<=N_SubTree+1;i++){
          for(int j=0;j<=N_SubTree+1;j++){
              if(SumD[m][i]<SumD[m][j]){
                  G[m][i][j] = SumD[m][j]-SumD[m][i];
              }
              else{
                  G[m][i][j] = 0;
              }
          }
       }
    }

}

void dijkstra(IloEnv env,  IloCplex cplex,  int m, int n, int startnode) {
   int distance[n+1],pred[n+1];
   IloNumArray2 cost (env, n+1);
   for(int i=0; i<=n; i++){
       cost[i] = IloNumArray (env, n+1);
   }
   int visited[n+1],count,mindistance,nextnode,i,j;

   for(i=0;i<=n;i++){
      for(j=0;j<=n;j++){
           if(G[m][i][j]==0){
              cost[i][j]=IloInfinity;
           }
           else{
               IloNum Aux_j=j-1;
               IloNum SumY = 0;
               while(Aux_j>=0){
                   SumY+=cplex.getValue(LY[Aux_j]);
                   Aux_j=Parent_SubModel[Aux_j];
               }
              cost[i][j]=G[m][i][j]*SumY;
           }
      }
   }
//    cout << SumD[m] << endl;
   for(i=0;i<=n;i++) {
      distance[i]=cost[startnode][i];
      pred[i]=startnode;
      visited[i]=0;
   }

   distance[startnode]=0;
   visited[startnode]=1;
   count=1;
   while(count<=n-1) {
      mindistance=IloInfinity;
      for(i=0;i<=n;i++){
         if(distance[i]<mindistance&&!visited[i]) {
             mindistance=distance[i];
             nextnode=i;
         }
      }
      visited[nextnode]=1;
      for(i=0;i<=n;i++){
         if(!visited[i]){
              if(mindistance+cost[nextnode][i]<distance[i]) {
                 distance[i]=mindistance+cost[nextnode][i];
                 pred[i]=nextnode;
              }
         }
      }
      count++;
   }
//   cout << G[m] << endl;
   IloNum Min_Dist = IloInfinity;
//   int Min_node= -1;

   //Print Paths
   for(i=0;i<=n;i++){
       if(i!=startnode) {
//          cout<<"\n Distance of node "<<i<<" = "<<distance[i]-SumD[m][i];
//          cout<<"\n Path = "<<i;
          j=i;
          int count=0;
          do {
             j=pred[j];
//             cout<<" <- "<<j;
             count ++;
//             if(distance[i]-SumD[m][i]<Min_Dist){Min_Dist=distance[i]-SumD[m][i]; Min_node= i;}
          }while(j!=startnode);
       }
   }
//   cout << endl << "Min Dis = " << cplex.getValue(Z[m]) + Min_Dist - SumD[m][Min_node] << endl;
//   cout << "Min node = " << Min_node << endl;

   //Look for Tree inequalities at each node in V
   //Rebuild Tree Inequalities
   IloNumArray CheckingTree (env, n);
   IloRangeArray Constraint (env);
//   IloRangeArray Nodal_Tree (env);
      for(int k=n;k>=1;k--){
       for(i=0;i<n;i++){
           CheckingTree[i]=0;
        }

       i=k;

       IloNum Violation = 0;
       IloExpr LHS (env);
//       IloExpr Nodal_LHS (env);
          while(i>=1) {
//              cout << "i = " << i << endl;
            if(i!=startnode) {
                IloNum Aux_i = i-1;
                while(Aux_i>=0){
//                    cout << "Aux_i = " << Aux_i << endl;
                    if(CheckingTree[Aux_i]==0){
                        CheckingTree[Aux_i]=1;
                        IloNum Delta=0;
                        IloNum Max_D=0;
                        //Look for the RHS of each node j in the shortest path connect to node Aux_i
                        j=k;
                        while(j>=1){
//                            cout << "j = " << j << endl;
                            IloNum Aux_j=j-1;
                            IloNum Aux_D=0;
                            while(Aux_j>=0){
                                Aux_D+=D[NodeModel[m][Aux_j]];
//                                cout << "Aux_j = " << Aux_j << endl;
                                //it's mean that node j is connected to node Aux_i
                                if(Aux_j==Aux_i){
                                    Delta+=SumD[m][j]-SumD[m][pred[j]];
                                    if(Aux_D>Max_D){Max_D=Aux_D;}
                                     break;
                                }
                                Aux_j=Parent_SubModel[Aux_j];
                            }
                            j=pred[j];
                        }

                        if(cplex.getValue(X[Aux_i])<min(Max_D,Delta)*cplex.getValue(LY[Aux_i])){
                            Violation+=cplex.getValue(X[Aux_i]);
                            LHS+=X[Aux_i];
//                            Nodal_LHS+=X[Aux_i];
                        }
                        else {
                            Violation+=min(Max_D,Delta)*cplex.getValue(LY[Aux_i]);
                            LHS+=min(Max_D,Delta)*LY[Aux_i];
//                            Nodal_LHS+=min(Max_D,Delta)*Y[Aux_i];
                        }
                    }
                    else {
                        break;
                    }
                    Aux_i= Parent_SubModel[Aux_i];
                }
            }
            i=pred[i];
          }

          Violation+=cplex.getValue(Z[m]);
          LHS+=Z[m];
//          Nodal_LHS+=Z[m];

          Violation-=SumD[m][k];
          LHS-=SumD[m][k];
//          Nodal_LHS-=SumD[m][k];



          if(Violation<=-0.001){
//          cout << "Violation = " <<Violation << endl;
              Constraint.add(LHS>=0);
//              Nodal_Tree.add(Nodal_LHS>=0);
                NbInequalities+=1;
          }

          LHS.end();
//          Nodal_LHS.end();
      }

      LinearModels[m].add(Constraint);
//      NodalModels[m].add(Nodal_Tree);
//      LagrangianModels[m].add(Nodal_Tree);
      Constraint.end();
//      Nodal_Tree.end();
      CheckingTree.end();


   cost.end();


}
