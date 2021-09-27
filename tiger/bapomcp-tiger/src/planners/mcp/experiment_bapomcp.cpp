#include "experiment_bapomcp.h"
#include "boost/timer.hpp"

using namespace std;

EXPERIMENTBAPOMCP::PARAMS::PARAMS()
:   NumRuns(100),
    NumSteps(300),
    SimSteps(1000),
    TimeOut(15000),
    Accuracy(0.01),
    UndiscountedHorizon(1000),
    AutoExploration(true)
{
}

EXPERIMENTBAPOMCP::EXPERIMENTBAPOMCP(const SIMULATOR& real,
    const SIMULATOR& simulator, const string& outputFile,
    EXPERIMENTBAPOMCP::PARAMS& expParams, BAPOMCP::PARAMS& searchParams, SamplerFactory& _samplerFact)
:   Real(real),
    Simulator(simulator),
    ExpParams(expParams),
    SearchParams(searchParams),
		samplerFact(_samplerFact),
    OutputFile(outputFile.c_str())
{
    if (ExpParams.AutoExploration)
    {
      SearchParams.ExplorationConstant = simulator.GetRewardRange();
    }

    BAPOMCP::InitFastUCB(SearchParams.ExplorationConstant);
    std::cout<<"initfastucbend"<<std::endl;
}
EXPERIMENTBAPOMCP::~EXPERIMENTBAPOMCP()
{
}

void EXPERIMENTBAPOMCP::Run(std::vector<double>& Rhist)
{
     
      uint* counts1copy=new uint[12];
      int episode;
      BAPOMCP mcts(Simulator, SearchParams, samplerFact);
      bool particle;
      for(episode = 0; episode<400; episode++)
{
    //BAPOMCP mcts(Simulator, SearchParams, samplerFact);
    particle=true;  
    Results.Clear();
    boost::timer timer;
    //std::cout<<"episode="<<episode<<std::endl;
    if(episode!=0)
    	mcts.Reset(counts1copy);
    //std::cout<<"mcts="<<episode<<std::endl;
    //BAPOMCP mcts(Simulator, SearchParams, samplerFact);
    if(episode!=0){
      	for(int i=0;i<12;i++)
      	{mcts.counts1[i]=counts1copy[i];
      		 //std::cout<<counts1copy[i]<<std::endl;
      	}
	//std::cout<<"**********************"<<std::endl;
	   }

    //std::cout<<"MCTSEND"<<std::endl;


    double undiscountedReturn = 0.0;
    double discountedReturn = 0.0;
    double discount = 1.0;
    bool terminal = false;
    int t;


    uint state = utils::rng.rand_closed01() <= 0.5?0:1;//initial state
    //uint simstate; //= utils::rng.rand_closed01() <= 0.5?0:1;
    //double* Belief=new double[2];
    //Belief[0]=0.50;
    //Belief[1]=0.50;
    //std::cout<<"Beliefend"<<std::endl;

    if (SearchParams.Verbose >= 1)
        Real.DisplayState(state, cout);


    for (t = 0; t < ExpParams.NumSteps; t++)
    {
		
	      
	uint observation;
        uint statenew;
        uint simstatenew;
        double reward;
        //uint action = mcts.SelectAction(simstate);
	      //for(int i=0;i<2;i++){std::cout<<Belief[i]<<cout<<endl;}
	      uint action = mcts.SelectAction();//BAPOMCP
		//std::cout << "selectaction" << std::endl;
	      if(action==0)
	      {
		       //std::cout<<action<<std::endl;
	      }
        terminal = Real.StepPOMDP(state, action, observation, statenew, reward);//step
        if(action==0)
          mcts.counts1[statenew*6+2*action+observation]+=1;
				//Rhist.push_back(reward);

				//std::cout << state << " " << action << " " << observation << " " << reward << std::endl;

				Results.Reward.Add(reward);
        undiscountedReturn += reward;
        discountedReturn += reward * discount;
        discount *= Real.GetDiscount();

        if (SearchParams.Verbose >= 1)
        {
            Real.DisplayAction(action, cout);
            Real.DisplayState(state, cout);
            Real.DisplayObservation(state, observation, cout);
            Real.DisplayReward(reward, cout);
        }
	      //mcts.Update(Belief, simstatenew, state, action, observation, reward);
        if (terminal)
        {
            //cout << "Terminated" << endl;
            break;
        }
       


        particle=mcts.Update(simstatenew, state, action, observation, reward);//update belief
	      //if(action!=0){Belief[0]=0.5;Belief[1]=0.5;}
	      //std::cout<<"updatebelief"<<std::endl;

				//For MDP:
        // simstate = simstatenew;
				state = statenew;
	if(!particle){episode--;break;}

        if (timer.elapsed() > ExpParams.TimeOut)
        {
            cout << "Timed out after " << t << " steps in "
                << Results.Time.GetTotal() << "seconds" << endl;
            break;
        }
    }
    
    if(particle)
    {
    for(int i=0;i<12;i++){counts1copy[i]=mcts.counts1[i];}
    //delete mcts;
    Results.Time.Add(timer.elapsed());
    Results.UndiscountedReturn.Add(undiscountedReturn);
    Results.DiscountedReturn.Add(discountedReturn);
    //cout << "(" << discountedReturn << ","
    //			<< Results.DiscountedReturn.GetMean() << ":"
    //			<< undiscountedReturn << "," << Results.UndiscountedReturn.GetMean()
    //			<< ") " << flush;


	Rhist.push_back(discountedReturn);
	/*cout << "(" << discountedReturn << ","
			<< Results.DiscountedReturn.GetMean() << ":"
			<< undiscountedReturn << "," << Results.UndiscountedReturn.GetMean()
			<< ") " << flush;*/
    }
}
	delete[] counts1copy;

}
/*
//FIXME Code repetition...
//Same code as in Run() but with Bandit specific argument
void EXPERIMENTBAPOMCP::RunBandit(std::vector<uint>& Rhist, std::vector<uint>& optArm, uint bestArm)
{
    boost::timer timer;

    BAPOMCP mcts(Simulator, SearchParams, samplerFact);


    double undiscountedReturn = 0.0;
    double discountedReturn = 0.0;
    double discount = 1.0;
    bool terminal = false;
    int t;

    uint state = Real.CreateStartState();
    if (SearchParams.Verbose >= 1)
        Real.DisplayState(state, cout);

    for (t = 0; t < ExpParams.NumSteps; t++)
    {
        uint observation;
        double reward;
        uint action = mcts.SelectAction(state);
        terminal = Real.Step(state, action, observation, reward);


				Rhist.push_back(reward);
				if(action==bestArm)
					optArm.push_back(1);
				else
					optArm.push_back(0);

        Results.Reward.Add(reward);
        undiscountedReturn += reward;
        discountedReturn += reward * discount;
        discount *= Real.GetDiscount();

        if (SearchParams.Verbose >= 1)
        {
            Real.DisplayAction(action, cout);
            Real.DisplayState(state, cout);
            Real.DisplayObservation(state, observation, cout);
            Real.DisplayReward(reward, cout);
        }

        if (terminal)
        {
            cout << "Terminated" << endl;
            break;
        }
				//state --> 0
        mcts.Update(state, action, observation, reward);


        if (timer.elapsed() > ExpParams.TimeOut)
        {
            cout << "Timed out after " << t << " steps in "
                << Results.Time.GetTotal() << "seconds" << endl;
            break;
        }
    }

    Results.Time.Add(timer.elapsed());
    Results.UndiscountedReturn.Add(undiscountedReturn);
    Results.DiscountedReturn.Add(discountedReturn);
    cout << "Discounted return = " << discountedReturn
        << ", average = " << Results.DiscountedReturn.GetMean() << endl;
    cout << "Undiscounted return = " << undiscountedReturn
        << ", average = " << Results.UndiscountedReturn.GetMean() << endl;
}
*/
//----------------------------------------------------------------------------
