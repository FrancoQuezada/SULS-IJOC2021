# SULS-IJOC2021

Author:Franco Quezada
date:22/07/2021

This repository has as main objective to provide the relevant data, source code and results of the extSDDiP algorithm  published in INFORMS Journal on Computing.

Results are provided in three files.

1) Results in Tables 1-4  and 6-9 of the manuscript can be found in "Performance of each method at solving instances from Set 1, 2 and 3 of the SULS problem (extSDDiP-I and extSDDiP-I-II).ods"
2) Results in Tables 10-13 of the manuscript can be found in "Performance of each method at solving instances from Set 1, 2 et 3  of the SULS problem (ExtSDDiP-II).ods"
3) Results in Table 14 of the manuscript can be found in "Performance of each method at solving instances with Sigma=20  of the SULS problem (ExtSDDiP-I-II).ods"

Each file contains self-explanatory header columns.

Data (instances of the SULS) is provided in a zip file which contains 3 folders.

1) Folder Instances_s4_s6 contains instances with 4 and 6 stages.
2) Folder Instances_s8_s12 contains instances with 8 and 12 stages.
3) Folder Instances_s20 contains instances with 20 stages.

The code of the extSDDiP algorithm is provided in a zip file. 

1) Main.cpp includes main functions for extSDDiP algorithm. 
2) CutGeneration.cpp includes functions to generate Benders', Strengthened Benders', Integer Optimality and Lagrangian cuts.
3) GlobalModel.cpp includes functions to generate an extensive formulation for the SULS problem.
4) NodalModel.cpp includes functions to generate all mixed-integer nodal subproblems.
5) LinearModel.cpp includes functions to generate all linear relaxation of nodal subproblems.
6) LagrangianModel.cpp includes functions to generate all Lagrangian relaxation of nodal subproblems.
7) DualLagrangianModel.cpp includes functions to solve to optimality through a sub-gradient algorithm the Lagrangian relaxation of nodal subproblems.
10) TreeInequalities.cpp includes functions to generate Tree Inequalities for each nodal subproblem based on the separation algorithm proposed in Guan et. al. 2009. Operations Research.
11) UpperBounds.cpp includes functions to calculate a statistical upper bound.

