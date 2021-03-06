#include "experiment.h"
#include "boost/timer.hpp"

using namespace std;

EXPERIMENT_BOSS::PARAMS::PARAMS()
:   NumRuns(100),
    NumSteps(300),
    SimSteps(1000),
    TimeOut(3600),
    Accuracy(0.01),
    UndiscountedHorizon(1000),
    AutoExploration(true)
{
}

EXPERIMENT_BOSS::EXPERIMENT_BOSS(const SIMULATOR& real,
    const SIMULATOR& simulator, const string& outputFile,
    EXPERIMENT_BOSS::PARAMS& expParams, BOSS::PARAMS& searchParams,
		SamplerFactory& _samplerFact)
:   Real(real),
    Simulator(simulator),
    ExpParams(expParams),
    SearchParams(searchParams),
		samplerFact(_samplerFact),
    OutputFile(outputFile.c_str())
{

}

void EXPERIMENT_BOSS::Run(std::vector<double>& Rhist)
{
    boost::timer timer;

    BOSS boss(Simulator, SearchParams, samplerFact);
	

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
        uint action = boss.SelectAction(state);
        terminal = Real.Step(state, action, observation, reward);
				Rhist.push_back(reward);
        
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
        boss.Update(state, action, observation, reward);
					
				//For MDP:
				state = observation;


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
    cout << "(" << discountedReturn << "," 
			<< Results.DiscountedReturn.GetMean() << ":"
			<< undiscountedReturn << "," << Results.UndiscountedReturn.GetMean()
			<< ") ";
}

//FIXME Code repetition... 
//Same code as in Run() but with Bandit specific argument
void EXPERIMENT_BOSS::RunBandit(std::vector<uint>& Rhist, std::vector<uint>& optArm, uint bestArm)
{
    boost::timer timer;

    BOSS boss(Simulator, SearchParams,samplerFact);

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
        uint action = boss.SelectAction(state);
				//TODO state --> 0
        terminal = Real.Step(state, action, observation, reward);
				
				std::cout << action << " ";
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
        boss.Update(state, action, observation, reward);
				

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
