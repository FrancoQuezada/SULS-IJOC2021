#ifndef IndINCLUDE
#define IndINCLUDE
#include "Header.h"
#endif

// address of the file containing the input data
//char INPUT [500] = "/home/quezada/DecompositionMethod/SubTree/InstancesGeneration/Small_Instances/";
//char INPUT [500] = "/home/oc/Documents/Franco/DecompositionMethods/SubTree/InstancesGeneration/Small_Instances/";
//char INPUT [500] = "/home/oc/Documents/Franco/DecompositionMethods/SubTree/InstancesGeneration/Extended_Test_Instances/";
//char INPUT [500] = "/home/oc/Documents/Franco/DecompositionMethods/SubTree/InstancesGeneration/Instances_2/";
//char INPUT [500] = "/home/oc/Documents/Franco/DecompositionMethods/SubTree/InstancesGeneration/Instances_ICAPS/";
char INPUT [500] = "/home/quezada/SDDP/Instances_RevisionTest/";
//char INPUT [500] = "/home/quezada/SDDP/Extended_Test_Instances/";
//InstanceGeneration/Instances/Q3/R3/Scenario_Tree_
// address of the files where the results are written


//char RESULTS [500] = "/home/oc/Documents/Franco/DecompositionMethods/SubTree/Results_SubTree_";
char RESULTS [500] = "/home/quezada/SDDP/SubTree/Results/Results_RevisionTest_";

char RESULTS_3 [500] = "/home/oc/Documents/Franco/DecompositionMethods/SubTree/Output_CPLEX_p3.txt";
char RESULTS_4 [500] = "/home/oc/Documents/Franco/DecompositionMethods/SubTree/ResultsTreeInequalities_";
char RESULTS_5 [500] = "/home/oc/Documents/Franco/DecompositionMethods/SubTree/SetupVariable_";

// Global variables
// - problem parameters
IloNumArray  D, Prob , g, SumDemand_max, ExpCoe, AuxPSI, LowerBound, UpperBound, TimeIte, IntegerCutValue, OptimalLinear, OptimalLagrangian, DualOptimalLagrangian, AuxLambda, AuxX, AuxY, AuxLS, AuxZ, Duals, Lambda, B_AuxLambda;
IloNumArray2 C, H, ga, pi, AuxBZ, B_Duals, B_Lambda, AuxS, SumD, Sol_Lagrangian_Y, Ext_D   ;

IloNumArray3 AuxBS, G;

//- description of the scenario tree
IloIntArray  Test_Leaf, Num_Leaf, Stage, SampleLeaf, SampleModel, StageModel, SubTree_Test_Leaf, Parent_SubModel, SetLeaf_SubModel, Leaf_Label, Num_Leaf_SubTree, Stage_SubTree ;
IloIntArray2 Leaf, Children, Parent, ChildrenModel, NodeModel, ParentModel;

IloNumVarArray PSI, X, S, Z, LY;
IloNumVarArray2 LBS, BZ, U;
IloNumVarArray3 W;
IloBoolVarArray Y;
IloBoolVarArray2 BS;

IloModelArray NodalModels, LinearModels, LagrangianModels;
IloObjectiveArray Objective;

int NbInequalities, Extended;



