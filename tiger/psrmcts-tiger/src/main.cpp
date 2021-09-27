#include <iostream>
#include "envs/bandit.h"
#include "envs/tiger.h"
#include "planners/mcp/experiment_spectral.h"
#include "planners/mcp/psrmcts/spectral.h"


#include "utils/rng.h"
#include "utils/hr_time.h"
#include "utils/utils.h"
#include "utils/anyoption.h"

#include <boost/foreach.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/lexical_cast.hpp>


using namespace boost::numeric::ublas;
using namespace boost;

#include <algorithm>



//Number of steps to execute
size_t T = 100;
//Number of trials
size_t N = 1;

//Number of states
size_t S = 5;
//Number of actions
size_t A = 4;

//Discount factor
double gammadisc = 0.95;

//Number of Simulations
size_t K = 10;

//Default exploration constant
double MCTSEC = 110*20;

//Seed
ulong seed = 0;


bool PSRMCTS = false;

uint ENV = 0;
// 11 - Tiger


//Read command-line arguments using the anyoption library
//(refer to anyoption.h)
void readCLArgs(int argc, char* argv[])
{
  AnyOption *opt = new AnyOption();
  opt->setVerbose();
  opt->autoUsagePrint(true);

  opt->addUsage( "See README file for examples.");
  opt->addUsage( "" );
  opt->addUsage( " -h  --help  		Prints this help " );
  opt->addUsage( " --T     Number of exploration steps" );
  opt->addUsage( " --N     Number of trials" );
	opt->addUsage( " --S     Number of states");
  opt->addUsage( " --A     Number of ations");
  opt->addUsage( " --gamma Dicount factor");
	opt->addUsage( " --seed  Seed for RNG");
	opt->addUsage( "" );


	opt->setFlag("help", 'h'); 
	opt->setOption("N");
	opt->setOption("T"); 
	opt->setOption("S");
	opt->setOption("A"); 
	opt->setOption("K");
	opt->setOption("gamma");
	opt->setOption("seed");
	opt->setOption("ENV");
	opt->setOption("MCTSEC");
	opt->setFlag("PSRMCTS");

  opt->processCommandArgs(argc, argv);

  if( opt->getFlag( "help" ) || opt->getFlag( 'h' ) ){
    opt->printUsage();
    exit(0);
  }
  if( opt->getValue("N") != NULL)
		N = utils::StringToInt(opt->getValue("N"));
  if( opt->getValue("S") != NULL)
		S = utils::StringToInt(opt->getValue("S"));
	if( opt->getValue("A") != NULL)
		A = utils::StringToInt(opt->getValue("A"));
	if( opt->getValue("gamma") != NULL)
		gammadisc = utils::StringToFloat(opt->getValue("gamma"));
	
	if( opt->getValue("T") != NULL)
		T = utils::StringToInt(opt->getValue("T"));
	if( opt->getValue("K") != NULL)
		K = utils::StringToInt(opt->getValue("K"));
	
	if( opt->getValue("seed") != NULL)
		seed = (ulong) utils::StringToInt(opt->getValue("seed"));
	
	if(opt->getFlag("PSRMCTS"))
		PSRMCTS = true;
	
	if(opt->getValue("ENV"))
		ENV = utils::StringToInt(opt->getValue("ENV"));

	if(opt->getValue("MCTSEC"))
		MCTSEC = utils::StringToFloat(opt->getValue("MCTSEC"));
	delete opt;
}

void printShortParams(){
	std::cout << "gam=" << gammadisc << " steps " << T << " N " << N << std::endl;
}
void printParams(){
	std::cout << "Exploration -- Parameters:" << std::endl;
	std::cout << "Number of steps:          " << T << std::endl;
	std::cout << "Number of trials : " << N << std::endl;
	std::cout << "Number of states:         " << S << std::endl;
	std::cout << "Number of actions:        " << A << std::endl;
	std::cout << "Discount factor:          " << gammadisc << std::endl;
	std::cout << "K:                        " << K << std::endl;
	std::cout << "------------------------------" << std::endl;
	std::cout << ".............................." << std::endl;
}


/*
 *  Main, sets up the experiment and dumps results.
 *
 */
int main(int argc, char* argv[]){
	//Read arguments
	readCLArgs(argc,argv);

	if(seed != 0)
		utils::setSeed(seed);

	//printParams();
	printShortParams();

	//-------------------------------------------------------------
	
	/////////////////////////////////////////
	SIMULATOR* real = 0;


	//Select environment
	
	if(ENV == 11){
		real = new Tiger(gammadisc);
	}
	//------------------------
	
	std::vector<std::vector<double> > Rhist(1);
	

	

	//Run selected algorithm(s)

		CStopWatch timer;




  	if(PSRMCTS){
		std::cout << std::endl << "**************" << std::endl;
		std::cout << "--PSRMCTS-- K " << K << " EC " << MCTSEC << std::endl;

		
		Rhist = std::vector<std::vector<double> >(1);;

		PSRMCTS::PARAMS searchParamsPSRMCTS;
		EXPERIMENTSPECTRAL::PARAMS expParamsPSRMCTS;
		expParamsPSRMCTS.NumRuns=N;
		searchParamsPSRMCTS.MaxDepth = real->GetHorizon(expParamsPSRMCTS.Accuracy, expParamsPSRMCTS.UndiscountedHorizon);
		expParamsPSRMCTS.NumSteps = T;
		searchParamsPSRMCTS.NumSimulations = K;
		expParamsPSRMCTS.AutoExploration = true;
		expParamsPSRMCTS.TimeOut = 32000;
		searchParamsPSRMCTS.ExplorationConstant = MCTSEC;
		EXPERIMENTSPECTRAL experiment8(*real, *real,
				expParamsPSRMCTS,
				searchParamsPSRMCTS);

		timer.startTimer();
		double *RESU =new double[N];
		for(uint i=0;i<1;++i){
			
			Rhist[i].reserve(N);
			experiment8.Run(Rhist[i]);
			for(uint a=0;a<N;a++){			
			RESU[a]=Rhist[i][a];
			
			}
			Rhist[i].clear();
			
		}
		std::ofstream result("result.txt");
		double mean=0;
		for(uint k=0;k<N;k++){
		
		result<<RESU[k]<<std::endl;
		mean+=RESU[k];
		
}		std::cout<<mean/N<<std::endl;
		result << "mean results: " <<mean/N << std::endl;
		timer.stopTimer();
		
	}


}

