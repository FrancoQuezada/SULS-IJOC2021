//For using some specific functions in C++
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <cmath>
#include <math.h>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <iomanip>
#include <sstream>

//For using standard libraries
#include <ilcplex/ilocplex.h>
using namespace std;

ILOSTLBEGIN

#define BILLION 1000000000L

//BASIC PARAMETERS of the model
//General Tree
const int Nbstage=20; 		// Number of Stages
const int c=3; 		// Number of children of the nodes at last periods of each stage
const int p=1;			// Length of each stage	 i.e noted as b in the report
const int I=1;			// Number of items
const int N_Tree = (int) p*((1-pow(c,Nbstage))/(1-c));

//SubTree
const int BkStg = 10;                                                //Number of stages in a subtree (or subproblem)
const int NbLeaf = pow(c,BkStg-1);
const int N_SubTree = (int) p*((1-pow(c,BkStg))/(1-c))-1;           //Number of nodes in a subtree (or subproblem)
const int M = (Nbstage/BkStg-1)*c;                                  //M+1 is the number of submodels starting from m=0
const int Ext_Tree = N_SubTree+1;

//SDDP Tree
const int N = (p-1) + (int) p*c*(Nbstage-1);		// N+1 nodes in the tree labelled from 0 to N

//const int l = 10000;		    //fixed lost-sale unit cost

//Definition of arrays of decision variables
typedef IloArray <IloNumVarArray> IloNumVarArray2; 
typedef IloArray <IloNumVarArray2> IloNumVarArray3; 
typedef IloArray <IloBoolVarArray> IloBoolVarArray2; 
typedef IloArray <IloBoolVarArray2> IloBoolVarArray3; 
typedef IloArray <IloExprArray> IloExprArray2;
typedef IloArray <IloExprArray2> IloExprArray3;
typedef IloArray <IloModel> IloModelArray;
typedef IloArray <IloObjective> IloObjectiveArray ; 
typedef IloArray <IloNumArray2> IloNumArray3;
// typedef IloArray <IloIntArray> IloIntArray2;


//Definition for the character stand for abbrevation on input + result files
extern char  INPUT [500] ; 
extern char  RESULTS [500] ;
extern char  RESULTS_3 [500] ;
extern char  RESULTS_4 [500] ;
extern char  RESULTS_5 [500] ;

// Global variables
// - problem parameters 
extern IloNumArray  D, Prob, g, SumDemand_max, ExpCoe, AuxPSI, LowerBound, UpperBound, TimeIte, IntegerCutValue, OptimalLinear, OptimalLagrangian, DualOptimalLagrangian, AuxLambda, AuxX, AuxY, AuxLS, AuxZ, Duals, Lambda, B_AuxLambda;
extern IloNumArray2 C, H, ga, pi, AuxBZ, B_Duals, B_Lambda, AuxS, SumD, Sol_Lagrangian_Y, Ext_D;

extern IloNumArray3 AuxBS, G;

//- description of the scenario tree
extern IloIntArray  Test_Leaf, Num_Leaf, Stage, SampleLeaf, SampleModel, StageModel, SubTree_Test_Leaf, Parent_SubModel, SetLeaf_SubModel, Leaf_Label, Num_Leaf_SubTree, Stage_SubTree ;
extern IloIntArray2 Leaf, Children, Parent, ChildrenModel, NodeModel, ParentModel;

extern IloNumVarArray PSI, X, S, Z, LY;
extern IloNumVarArray2 LBS, BZ, U;
extern IloNumVarArray3 W;
extern IloBoolVarArray Y;
extern IloBoolVarArray2 BS;

extern IloModelArray NodalModels, LinearModels, LagrangianModels;
extern IloObjectiveArray Objective;

extern int NbInequalities, Extended;

// Read/generate instance data + echelon stock computation
void read_data(IloEnv env) ;
void instance_generation(IloEnv env) ;
void echelon_cost(IloEnv env) ;
void variables_definition(IloEnv env, int ExpSize);

// Pre-solving steps: scenario tree description 
void find_stage_Tree(IloEnv env);
void pre_compute_Tree (IloEnv env);
void find_children_Tree (IloEnv env);
int first_bud_Tree (int n);
int first_leaf_Tree (int n);
void find_leaf_Tree (IloEnv env);

//Pre_solving steps: scenario SDDP tree description
void find_stage(IloEnv env);
void pre_compute (IloEnv env);
void find_children (IloEnv env);
int first_bud (int n);
int first_leaf (int n);
void find_leaf (IloEnv env);
void BigM(IloEnv env);

//Pre_solving SubModels
void pre_compute_SubModels (IloEnv env);

// Define initial formulation for each nodal subproblem
void InitialNodalModel (IloEnv env, int n);
void InitialLinearModel (IloEnv env, int n);
void InitialLagrangianModel (IloEnv env, int n);

//Make the binary expansion for each subproblem
void Update_NodalModel (IloEnv env, int n, int ExpSize);
void Update_LinearModel (IloEnv env, int n, int ExpSize);
void Update_LagrangianModel (IloEnv env, int n, int ExpSize);

//Solve each subproblem
double  SolveNodalModel (IloEnv env, int n, int stg, int Fix_Sol, int Save_Sol);
double  SolveLinearModel (IloEnv env, int n, int stg, int cutting, int it);
double  SolveLagrangianModel (IloEnv env, int n);

//Solve each subproblem with Binary variables
double  B_SolveNodalModel (IloEnv env, int n, int ExpSize, int Fix_Sol, int Save_Sol);
double  B_SolveLinearModel (IloEnv env, int n, int ExpSize, int stg, int cutting, int it, int Fix_Sol);
double  B_SolveLagrangianModel (IloEnv env, int n);

//Formulation using extensive formulation
double  GlobalModel (IloEnv env, IloNumArray LowerBoundCplex);
double  Global_LinearModel (IloEnv env, IloNumArray LowerBoundCplex);
double  GlobalBinaryModel (IloEnv env, int ExpSize);

//Functions to generate Integer Optimality Cuts
void IntegerOptimalityCutsGeneration(IloEnv env, int n, int ExpSize, int MultiCuts);
void LIntegerOptimalityCutsGeneration(IloEnv env, int n, int ExpSize, int MultiCuts);

//Function to Generate Strengthend Benders Cuts
void StrengthenedBendersCutsGeneration(IloEnv env, int i, int MultiCuts);
void B_StrengthenedBendersCutsGeneration(IloEnv env, int i, int ExpSize, int MultiCuts);

//Function to Generate Benders Cuts
void BendersCutsGeneration(IloEnv env, int i, int MultiCuts);
void B_LStrengthenedBendersCutsGeneration(IloEnv env, int i, int ExpSize, int MultiCuts);

//Function to Generate Lagrangian Cuts
void LagrangianCutsGeneration (IloEnv env, int i, int ExpSize, int MultiCuts);
void LLagrangianCutsGeneration (IloEnv env, int i, int ExpSize, int MultiCuts);

double  SubGradient (IloEnv env, int n, int SubIte, int ExpSize);
double  SolveLagrangianModel_Sub (IloEnv env, int n, int ExpSize);

//Function to add Cuts to each subproblem
void UpdateModels(IloEnv env, int n);
void CuttingPlaneGeneration(IloEnv env, IloCplex cplex, int m);

//Functions to generate Tree inequalities
void dijkstra(IloEnv env, IloCplex cplex, int m, int n, int startnode);
void Pre_TreeInequalities(IloEnv env);

//Function to generate Demand for the exteneded formulation
void ExtendedDemand (IloEnv env);

// Heuristic separation of tree inequalities
// - round to the nearest value with nbdigits decimals (manage numerical inaccuracies around 0)
IloNum round_num(IloNum val, int nbdigits) ; 
// - sort the value of the RHS in increasing order 
void quickSort(IloNumArray RHS, int left, int right);
// - in case RHS contains several times the same value, order the corresponding leaves in the increasing order of their index
void quickSort_2(IloIntArray RHS_Leaf, int left, int right);
// - sort the RHS value while taking into account multiple occurences of the same value in RHS
void SortRHS (IloIntArray RHS, IloIntArray RHS_Leaf);

void find_nodes(IloEnv env);

int randu(int a, int b);

//Caculate Upper Bounds
double Optimal_UpperBound(IloEnv env, int PHASE_I, int PHASE_II, int ExpSize, int NbScen, int TimeLimit_SND);
double UpperBound_PhaseI (IloEnv env, int NbScen, IloNumArray SampleUpperBound, int TimeLimit_SND, IloNumArray Aux_NbScen);
double UpperBound_PhaseII (IloEnv env, int ExpSize,int NbScen, IloNumArray SampleUpperBound, int TimeLimit_SND, IloNumArray Aux_NbScen);

double UpperBound_ScenarioBased (IloEnv env, int NbScen, IloNumArray SampleUpperBound, int TimeLimit_SND, IloNumArray Aux_NbScen);
double UpperBound_ScenarioBased_II (IloEnv env, int NbScen, IloNumArray SampleUpperBound, int TimeLimit_SND, IloNumArray Aux_NbScen);
double UpperBound_ScenarioBased_III (IloEnv env, int NbScen, IloNumArray SampleUpperBound, int TimeLimit_SND, IloNumArray Aux_NbScen);
double  SolveNodalModel_Upper (IloEnv env, int n, IloNumArray2 UpperX, IloNumArray2 UpperY, IloNumArray2 UpperS);

void cleannig (IloEnv env);
