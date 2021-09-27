#include "experiment_spectral.h"
#include "boost/timer.hpp"

using namespace std;

EXPERIMENTSPECTRAL::PARAMS::PARAMS()
:   NumRuns(100),
    NumSteps(300),
    SimSteps(1000),
    TimeOut(15000),
    Accuracy(0.01),
    UndiscountedHorizon(1000),
    AutoExploration(true)
{
}

EXPERIMENTSPECTRAL::EXPERIMENTSPECTRAL(const SIMULATOR& real,
    const SIMULATOR& simulator, 
    EXPERIMENTSPECTRAL::PARAMS& expParams, PSRMCTS::PARAMS& searchParams)
:   Real(real),
    Simulator(simulator),
    ExpParams(expParams),
    SearchParams(searchParams)
{
    if (ExpParams.AutoExploration)
    {
      SearchParams.ExplorationConstant = simulator.GetRewardRange();
    }

    PSRMCTS::InitFastUCB(SearchParams.ExplorationConstant);
    
}
EXPERIMENTSPECTRAL::~EXPERIMENTSPECTRAL()
{
}

void EXPERIMENTSPECTRAL::Run(std::vector<double>& Rhist)
{
      SPECTRAL* Spectral=0;
      Spectral=new SPECTRAL(Simulator);
     
      int episode;
      PSRMCTS mcts(Simulator, SearchParams, *Spectral);
      
      
     
      std::vector<float>* belief=new std::vector<float>;
      for(int i=0;i<2;i++)//initial belief state
      {
		belief->push_back(0.5);
      }
      
      

      symList2IntMap* Histcout=Spectral->createPsrdata2(200);//training data
      symSet *AOset=Spectral->getAOset();
      symSet *Testset=Spectral->getAOset();

      symSet *AOQset=mcts.getAOQset(AOset,Testset);
      
      symSet *AOQQset=mcts.getAOQset(AOset,AOQset);
   
      int aosize=AOset->size();
      int tsize=AOQset->size();
      int hsize=Histcout->size();
	
      
      mcts.getPH(Histcout);
      mcts.PTH=mcts.getProb(AOQset,belief,50,Histcout,1);
      mcts.PAOH=mcts.getProb(AOset,belief,50,Histcout,2);
      mcts.PAOQH2D=mcts.getProb(AOQQset,belief,50,Histcout,3);
      mcts.PAOQH3D=Spectral->Array2TO3(mcts.PAOQH2D,aosize,tsize,hsize);	
      
      mcts.getSpectral();//build PSR model
Results.Clear();      
for(episode = 0; episode<ExpParams.NumRuns; episode++)
{
    std::cout<<"------"<<episode<<"------"<<std::endl;
    


    double undiscountedReturn = 0.0;
    double discountedReturn = 0.0;
    double discount = 1.0;
    bool terminal = false;
    int t;
    
    mcts.ConstructSameTree();
    
    uint state = Simulator.CreateStartState();
    for(int b1i=0;b1i<mcts.b1->GetNumRows();b1i++)
	{
		
		mcts.btupdate->SetElement(b1i,0,mcts.b1->GetElement(b1i,0));

	}

    for (t = 0; t < ExpParams.NumSteps; t++)
    {
		
	boost::timer timer;     
	uint observation;
        uint statenew;
        double reward;
        uint action = mcts.SelectActionPSR();
        terminal = Real.StepPOMDP(state, action, observation, statenew, reward);//perform one step
	
	Results.Reward.Add(reward);
        undiscountedReturn += reward;
        discountedReturn += reward * discount;
        discount *= Real.GetDiscount();
	
        if (terminal)
        {
	    Results.Time.Add(timer.elapsed());
            break;
        }
	
	mcts.UpdatePSR(action,observation);//Update 
	
        if (timer.elapsed() > ExpParams.TimeOut)
        {
            cout << "Timed out after " << t << " steps in "
                << Results.Time.GetTotal() << "seconds" << endl;
            break;
        }
	state=statenew;
	Results.Time.Add(timer.elapsed());
    }
    mcts.FreeusedRoot();

    
    Results.UndiscountedReturn.Add(undiscountedReturn);
    Results.DiscountedReturn.Add(discountedReturn);


	Rhist.push_back(discountedReturn);
	cout << "(" << discountedReturn << ","
			<< Results.DiscountedReturn.GetMean() << ":"
			<< undiscountedReturn << "," << Results.UndiscountedReturn.GetMean()
			<< ") " << flush;
	std::cout<<std::endl;
    
}

	cout <<"mean time-"<<"(" << Results.Time.GetMean() << ")";
}

//----------------------------------------------------------------------------
