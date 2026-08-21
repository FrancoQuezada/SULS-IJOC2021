
#ifndef IndINCLUDE
#define IndINCLUDE
#include "Header.h"
#endif

//Read data from the file INPUT
void read_data(IloEnv env){

	ifstream readfile (INPUT) ;
	if(!readfile) {
		cout << "Error in reading data file" << endl ; 
	}
	else {
		//Demand Vector D
		D = IloNumArray(env, N+1);
		readfile >> D; 
//cout << D << endl;
		//Returns Supply vector R
//                R = IloNumArray(env, N+1);
//                readfile >> R;
//cout << R << endl;
		//Bill of material alpha
//                a = IloNumArray(env, I+2);
//                readfile >> a;
//cout << a << endl;
		//Set-up cost vector S
		C = IloNumArray2(env, N+1);
		for (int n=0; n<=N; n++){
			C[n]=IloNumArray(env,I+2);
		}
		readfile >> C; 
//cout << C << endl;
		//Inventory holding costs
		H = IloNumArray2(env, N+1);
		for(int n=0; n<=N; n++){
			H[n]=IloNumArray(env,2*I+2);
		}
		readfile >> H; 
//cout << H << endl;
		// Probability of node n
		Prob = IloNumArray(env, N+1);
		readfile >> Prob; 
//cout << Prob << endl;
                // Unit cost for discarding
//                ga = IloNumArray2(env, N+1);
//                for(int n=0; n<=N; n++){
//                        ga[n]=IloNumArray(env, I+1);
//                }
//                readfile >> ga;
//                cout << "gamma=" << ga << endl;
//
//		//proportion of recoverable parts
//                pi = IloNumArray2(env, N+1);
//                for (int n=0; n<=N; n++){
//                        pi[n] = IloNumArray(env,I+1);
//                }
//                readfile >> pi;
//                cout << "pi=" << pi << endl ;

		g = IloNumArray (env, N+1);
		readfile >> g;

		// //Print the input data
//                 cout << "D=" << D << endl ;
//                 cout << endl;
//                 cout << "R= " << R << endl ;
//                 cout << endl;
//                 cout << "alpha= " << a << endl ;
//                 cout << endl;
//                 cout << "C=" << C << endl ;
//                 cout << endl;
//                 cout << "H=" << H << endl ;
//                 cout << endl;
//                 cout << "P_n=" << Prob << endl ;
//                 cout << endl;
//                 cout << "g= " << g << endl ;
//                 cout << endl;
//                 cout << "pi= " << pi << endl ;
//                 cout << endl;
//                 cout << "ga= " << ga << endl ;
		
		readfile.close();
	}
}
