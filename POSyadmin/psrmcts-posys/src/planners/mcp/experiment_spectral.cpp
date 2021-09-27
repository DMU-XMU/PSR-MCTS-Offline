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
      
      mcts.ConstructTree();//construct offline tree
     
      
      symList2IntMap* Histcout1=mcts.getHistcout();
     
      std::vector<float>* belief=new std::vector<float>;

      int S=Simulator.GetNumStates();
      for(int i=0;i<S;i++)
      {
	if(i!=(S-1))
	{
	 belief->push_back(0.0);
	}
	else
	{
	belief->push_back(1.0);
	}
      }

      symList2IntMap* Histcout=Spectral->createPsrdata2(300);//training data

      //symSet *AOsetpos=Spectral->getAOsethappendbyHistcout(Histcout1);
	/*****/      
	symSet *AOsetpos=Spectral->getAOset();
	/*****/
      
      symSet *AOset=Spectral->getAOset();
      symSet *AOset1=Spectral->getAOset();

      symSet *Testset=Spectral->getAOset();


      mcts.getsysAOsetpossibleidInAOset(AOsetpos,AOset1);
      mcts.getAOnumByA();
      mcts.getActBeginIndex();


      symSet *AOQset=mcts.getAOQset(AOset,Testset);
      symSet *AOQQset=mcts.getAOQset(AOsetpos,AOQset);
   
      int aosize=AOsetpos->size();
      int tsize=AOQset->size();
      int hsize=Histcout->size();




	
      mcts.getPH(Histcout);
      mcts.PTH=mcts.getProb(AOQset,belief,50,Histcout,1);
      mcts.PAOH=mcts.getProb(AOsetpos,belief,50,Histcout,2);
      mcts.PAOQH2D=mcts.getProb(AOQQset,belief,50,Histcout,3);
      mcts.PAOQH3D=Spectral->Array2TO3(mcts.PAOQH2D,aosize,tsize,hsize);	
      

      boost::timer timer;
      mcts.getSpectral();//build PSR model
      cout << timer.elapsed() << endl;
      
      

      
      
Results.Clear();   
for(episode = 0; episode<ExpParams.NumRuns; episode++)
{
    
    
    //std::cout<<"------"<<episode<<"------"<<std::endl;


    double undiscountedReturn = 0.0;
    double discountedReturn = 0.0;
    double discount = 1.0;
    bool terminal = false;
    int t;

    mcts.ConstructSameTree1();//copy offline tree

    uint state = Simulator.CreateStartState();
    for(int b1i=0;b1i<mcts.b1->GetNumRows();b1i++)//initial belief state of PSR model
	{
		
		mcts.btupdate->SetElement(b1i,0,mcts.b1->GetElement(b1i,0));

	}
    bool find;
    for (t = 0; t < ExpParams.NumSteps; t++)
    {
	boost::timer timer;
		
	      
	uint observation;
        uint statenew;
        double reward;
        uint action = mcts.SelectActionPSR();
        terminal = Real.StepPOMDP(state, action, observation, statenew, reward);
	
	Results.Reward.Add(reward);
        undiscountedReturn += reward;
        discountedReturn += reward * discount;
        discount *= Real.GetDiscount();
        if (terminal)
        {
            break;
        }

	mcts.UpdatePSR(action,observation,find);

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

	if(episode==ExpParams.NumRuns-1){
	cout << "(" << discountedReturn << ","
			<< Results.DiscountedReturn.GetMean() << ":"
			<< undiscountedReturn << "," << Results.UndiscountedReturn.GetMean()
			<< ") " << flush;
	std::cout<<std::endl;
	}
    
}




Results.Clear();   
for(episode = 0; episode<ExpParams.NumRuns; episode++)
{
    
    
    //std::cout<<"------"<<episode<<"------"<<std::endl;


    double undiscountedReturn = 0.0;
    double discountedReturn = 0.0;
    double  discount = 1.0;
    bool terminal = false;
    int t;

    mcts.ConstructSameTree();//copy offline tree

    uint state = Simulator.CreateStartState();
    for(int b1i=0;b1i<mcts.b1->GetNumRows();b1i++)//initial belief state of PSR model
	{
		
		mcts.btupdate->SetElement(b1i,0,mcts.b1->GetElement(b1i,0));

	}
    bool find;
    for (t = 0; t < ExpParams.NumSteps; t++)
    {
	boost::timer timer;
		
	      
	uint observation;
        uint statenew;
        double reward;
        uint action = mcts.SelectActionPSR();
        terminal = Real.StepPOMDP(state, action, observation, statenew, reward);
	
	Results.Reward.Add(reward);
        undiscountedReturn += reward;
        discountedReturn += reward * discount;
        discount *= Real.GetDiscount();
        if (terminal)
        {
            break;
        }

	mcts.UpdatePSR(action,observation,find);

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

	if(episode==ExpParams.NumRuns-1){
	cout << "(" << discountedReturn << ","
			<< Results.DiscountedReturn.GetMean() << ":"
			<< undiscountedReturn << "," << Results.UndiscountedReturn.GetMean()
			<< ") " << flush;
	std::cout<<std::endl;
	}
    
}










/*************************删减AO无离线树******************************/
      AOsetpos=Spectral->getAOsethappendbyHistcout(Histcout1);

      
      AOset=Spectral->getAOset();
      AOset1=Spectral->getAOset();

      Testset=Spectral->getAOset();


      mcts.getsysAOsetpossibleidInAOset(AOsetpos,AOset1);
      mcts.getAOnumByA();
      mcts.getActBeginIndex();


      AOQset=mcts.getAOQset(AOset,Testset);
      AOQQset=mcts.getAOQset(AOsetpos,AOQset);
   
      aosize=AOsetpos->size();
      tsize=AOQset->size();
      hsize=Histcout->size();




	
      mcts.getPH(Histcout);
      mcts.PTH=mcts.getProb(AOQset,belief,50,Histcout,1);
      mcts.PAOH=mcts.getProb(AOsetpos,belief,50,Histcout,2);
      mcts.PAOQH2D=mcts.getProb(AOQQset,belief,50,Histcout,3);
      mcts.PAOQH3D=Spectral->Array2TO3(mcts.PAOQH2D,aosize,tsize,hsize);	
      
      mcts.getSpectral();//build PSR model


Results.Clear();   
for(episode = 0; episode<ExpParams.NumRuns; episode++)
{
    
    
    //std::cout<<"------"<<episode<<"------"<<std::endl;


    double undiscountedReturn = 0.0;
    double discountedReturn = 0.0;
    double discount = 1.0;
    bool terminal = false;
    int t;

    mcts.ConstructSameTree1();//copy offline tree

    uint state = Simulator.CreateStartState();
    for(int b1i=0;b1i<mcts.b1->GetNumRows();b1i++)//initial belief state of PSR model
	{
		
		mcts.btupdate->SetElement(b1i,0,mcts.b1->GetElement(b1i,0));

	}
    bool find;
    for (t = 0; t < ExpParams.NumSteps; t++)
    {
	boost::timer timer;
		
	      
	uint observation;
        uint statenew;
        double reward;
        uint action = mcts.SelectActionPSR();
        terminal = Real.StepPOMDP(state, action, observation, statenew, reward);
	
	Results.Reward.Add(reward);
        undiscountedReturn += reward;
        discountedReturn += reward * discount;
        discount *= Real.GetDiscount();
        if (terminal)
        {
            break;
        }

	mcts.UpdatePSR(action,observation,find);

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

	if(episode==ExpParams.NumRuns-1){
	cout << "(" << discountedReturn << ","
			<< Results.DiscountedReturn.GetMean() << ":"
			<< undiscountedReturn << "," << Results.UndiscountedReturn.GetMean()
			<< ") " << flush;
	std::cout<<std::endl;
	}
    
}




cout <<"time-"<<"(" << Results.Time.GetMean() << ")";
}

//----------------------------------------------------------------------------
