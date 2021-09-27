//#include "bamcp.h"
#include "bapomcp.h"
#include <math.h>
#include "utils/utils.h"
#include <algorithm>

#include <iomanip>

#include "node.h"

//#include "samplers/sampler.h"
#include "samplers/samplerFactory.h"

#include "envs/banditSim.h"
#include "planners/MDPutils.h"

#include "envs/basicMDP.h"
#include "envs/basicPOMDP.h"

using namespace std;
using namespace UTILS;

//-----------------------------------------------------------------------------

BAPOMCP::PARAMS::PARAMS()
:   Verbose(0),
    MaxDepth(100),
    NumSimulations(1000),
    NumStartStates(1000),
    ExpandCount(1),
    ExplorationConstant(110*20),
		ReuseTree(false),
		BANDIT(false),   //Deal with bandits a bit differently in the code
		RB(-1),
		eps(1)
{
}

BAPOMCP::BAPOMCP(const SIMULATOR& simulator, const PARAMS& params,
		SamplerFactory& sampFact)
:   Params(params),
		Simulator(simulator),
    TreeDepth(0),
		SampFact(sampFact)
{
    VNODE::NumChildren = Simulator.GetNumActions();
    QNODE::NumChildren = Simulator.GetNumObservations();

    Root = ExpandNode();


		A = Simulator.GetNumActions();
		S=8;
    		O = Simulator.GetNumObservations();
		SA = S*A;
		SAS = S*A*S;
   	        SAO = S*A*O;

		RLPI = new uint[S];
		V = new double[S];

		Q = new double[SA];
		std::fill(Q,Q+SA,0);
		GreedyA = new std::vector<uint>*[S];
		std::vector<uint>	 tmp(A);
		for(uint a = 0;a<A;++a){
			tmp[a] = a;
		}
		for(uint s = 0;s<S;++s){
			GreedyA[s] = new std::vector<uint>(tmp);
		}
   		 QlearningRate = 0.2;

		//Initialize transition counts for posterior estimation
		if(!Params.BANDIT){
			counts = new uint[SAS];
			for(uint c=0;c<SAS;++c)
			{
				counts[c] = 0;
			}
      			counts1 = new uint[S*A*S];
			for (uint c = 0; c < S*A*S; ++c)
			{
        		counts1[c]=Simulator.getcountsOini(c);
      			//std::cout<<counts1[c]<<std::endl;
			}
 

		}
		else{
			assert(S == 2);
			counts = new uint[SA];
			for(uint c=0;c<SA;++c){
				counts[c] = 0;
			}
		}

		countsSum = new double[SA];
		std::fill(countsSum,countsSum+SA,SampFact.getAlphaMean()*S);
		for (int i = 0; i < 1000; i++)
        	Root->Beliefs().AddSample(Simulator.CreatesimStartState(counts1));
		step = 0;


//		meand.open("BMCP_meandepth",std::ios_base::app);
//		maxd.open("BMCP_maxdepth",std::ios_base::app);
}

BAPOMCP::~BAPOMCP()
{
	//std::cout<<"~BAPOMCP"<<std::endl;
	delete[] counts;
  	delete[] counts1;
	delete[] countsSum;

	delete[] RLPI;
	delete[] V;

	delete[] Q;
	for(uint s=0;s<S;++s)
		delete GreedyA[s];
	delete[] GreedyA;

	VNODE::Free(Root, Simulator);
  	VNODE::FreeAll();
}

void BAPOMCP::Reset(const uint* countsrefine)
{   
	
	BELIEF_STATE beliefs;
	//std::cout<<"GetNewBelief"<<std::endl;	
	GetNewBelief(beliefs);  

	History.Clear();
    delete[] counts;
    delete[] counts1;
    //Root->Beliefs().Free(Simulator);
    VNODE::Free(Root, Simulator);
	//std::cout<<"FreeRoot"<<std::endl;
    VNODE::FreeAll();
    //std::cout<<"RootExbg"<<std::endl;
    VNODE* newRoot = ExpandNode();
    Root = newRoot;
    //Root = ExpandNode();
    //std::cout<<"RootEx"<<std::endl;

			//Initialize transition counts for posterior estimation
		if(!Params.BANDIT){
			counts = new uint[SAS];
			for(uint c=0;c<SAS;++c){
				counts[c] = 0;
			}
      counts1 = new uint[SAO];
      for (uint c = 0; c < SAO; ++c) {
        counts1[c]=10000;
      }
     /* counts1[0]=5;
      counts1[1]=3;
      counts1[SA]=3;
      counts1[SA+1]=5;*/

	  counts1[0]=countsrefine[0];
      counts1[1]=countsrefine[1];
      counts1[SA]=countsrefine[SA];
      counts1[SA+1]=countsrefine[SA+1];

		}
		else{
			assert(S == 2);
			counts = new uint[SA];
			for(uint c=0;c<SA;++c){
				counts[c] = 0;
			}
		}


   	 /*for (int i = 0; i < 1000; i++)
        Root->Beliefs().AddSample(Simulator.CreatesimStartState(counts1));*/
	 Root->Beliefs()=beliefs;
     //std::cout<<"AddSample"<<std::endl;



		step = 0;
}

bool BAPOMCP::Update(uint& ssnew , uint ss, uint aa, uint observation, double reward)
{
    //std::cout<<"startUpdate"<<std::endl;
    History.Add(aa, observation);
    bool particle;
   
    BELIEF_STATE beliefs;
    //std::cout<<"startAddTranforms"<<std::endl;
    particle=AddTransforms(Root, beliefs);
    //std::cout<<"AddTranformsend"<<std::endl;





     if(!particle)
	{ 	std::cout<<"outofparticle"<<std::endl;
		beliefs.Free(Simulator);
		return particle;}	
    //Reuse previous subtree for next search
    //Get to subtree
    QNODE& qnode = Root->Child(aa);
    VNODE* vnode = qnode.Child(observation);
    if(vnode && Params.ReuseTree){
      //Delete other subtrees but this one
      vnode->preventfree = true;
      VNODE::Free(Root,Simulator);
      vnode->preventfree = false;
      Root = vnode;
    }else{
      // Delete old tree and create new root
      VNODE::Free(Root,Simulator);
      VNODE* newRoot = ExpandNode();
      Root = newRoot;
    }
    Root->Beliefs() = beliefs;
    //std::cout<<"Updateend"<<std::endl;

    return particle;
}





int BAPOMCP::SelectAction()
{
  UCTSearch();
  //std::cout<<"UCTend"<<std::endl;
  step++;
  return GreedyUCB(Root,false);;
}

void BAPOMCP::UCTSearch()//BAPOMCP
{
    //std::cout<<"UCTSearch"<<std::endl;
    //uint state;
    ClearStatistics();
    int historyDepth = History.Size();
		//double* parm = 0;
		//double* p_samp = 0;
		
		Sampler* MDPSampler = 0;
		//Sampler* MDPSamplerbak=MDPSampler;
		if(Params.BANDIT){
			//parm = new double[2];
			//p_samp = new double[Simulator.GetNumActions()];
		}
		else{
			//std::cout<<"getTigerPOMDPSampler"<<std::endl;
			MDPSampler = SampFact.getTigerPOMDPSampler(counts1,S,A,O,
					Simulator.R,Simulator.rsas,Simulator.GetDiscount());
			
			//std::cout<<"getTigerPOMDPSamplerendl"<<std::endl;
		}


    for (int n = 0; n < Params.NumSimulations; n++)
    {
	//std::cout<<"Numsim"<<n<<std::endl;

    	
    	STATE* state = Root->Beliefs().CreateSampleWithProb(Simulator);

				
	Status.Phase = SIMULATOR::STATUS::TREE;

        if (Params.Verbose >= 2)
        {
            cout << "Starting simulation" << endl;
        }

        TreeDepth = 0;
        PeakTreeDepth = 0;

	double totalReward = SimulateV(/*mdp,*/ *state, Root/*, MDPSampler*/);
        StatTotalReward.Add(totalReward);
        StatTreeDepth.Add(PeakTreeDepth);

        if (Params.Verbose >= 2)
            cout << "Total reward = " << totalReward << endl;
        if (Params.Verbose >= 4)
            DisplayValue(4, cout);

        History.Truncate(historyDepth);

				
        Simulator.FreeState(state);

	}
	delete MDPSampler;
		
    DisplayStatistics(cout);
}

double BAPOMCP::SimulateV(/*const SIMULATOR* mdp,*/ STATE& state, VNODE* vnode/*, Sampler* MDPSampler*/)
{
    //std::cout<<"SimulateV"<<endl;
    uint action = GreedyUCB(vnode, true);

    PeakTreeDepth = TreeDepth;
    if (TreeDepth >= Params.MaxDepth) // search horizon reached
        return 0;

    QNODE& qnode = vnode->Child(action);
    double totalReward = SimulateQ(state, qnode, action/*, MDPSampler*/);
    vnode->Value.Add(totalReward);
    return totalReward;
}

double BAPOMCP::SimulateQ(/*const SIMULATOR* mdp,*/ STATE& state, QNODE& qnode, uint action/*,Sampler* MDPSampler*/)
{
    //std::cout<<"SimulateQ"<<endl;
    uint observation;
    //uint statenew;
    double immediateReward, delayedReward = 0;

    
    
    
    //std::cout<<"startstep"<<std::endl;
    bool terminal = Simulator.StepPOMDPsim(state, action, observation, immediateReward);
    //std::cout<<"endstep"<<std::endl;
 

    History.Add(action, observation);


    VNODE*& vnode = qnode.Child(observation);
    if (!vnode && !terminal && qnode.Value.GetCount() >= Params.ExpandCount)
        vnode = ExpandNode(); //&state);

    if (!terminal)
    {
        TreeDepth++;
        if (vnode)
            delayedReward = SimulateV(state, vnode/*, MDPSampler*/);
        else{
					

	delayedReward = Rollout(/*mdp,*/ state/*, MDPSampler*/);
							

				}
        TreeDepth--;
    }

    double totalReward = immediateReward + Simulator.GetDiscount() * delayedReward;
    qnode.Value.Add(totalReward);
    return totalReward;
}

VNODE* BAPOMCP::ExpandNode()
{
    VNODE* vnode = VNODE::Create();
    vnode->Value.Set(0, 0);
		vnode->SetChildren(0,0);


    if (Params.Verbose >= 2)
    {
        cout << "Expanding node: ";
        History.Display(cout);
        cout << endl;
    }

    return vnode;
}


int BAPOMCP::GreedyUCB(VNODE* vnode, bool ucb) const//choose an aciton by UCB
{
    static std::vector<int> besta;
    besta.clear();
    double bestq = -Infinity;
    int N = vnode->Value.GetCount();
    double logN = log(N + 1);

    for (int action = 0; action < Simulator.GetNumActions(); action++)
    {
        double q;
        int n;

        QNODE& qnode = vnode->Child(action);
        q = qnode.Value.GetValue();
        n = qnode.Value.GetCount();
	/*if(!ucb)
		std::cout<<action<<"--"<<q<<std::endl;*/

				if (ucb)
            q += FastUCB(N, n, logN);

        if (q >= bestq)
        {
            if (q > bestq)
                besta.clear();
            bestq = q;
            besta.push_back(action);
        }
    }

    assert(!besta.empty());
    return besta[Random(besta.size())];
}

double BAPOMCP::Rollout(/*const SIMULATOR* mdp,*/ STATE& state/*, Sampler* MDPSampler*/)
{
    //std::cout<<"Rollout"<<endl;
    Status.Phase = SIMULATOR::STATUS::ROLLOUT;
    if (Params.Verbose >= 3)
        cout << "Starting rollout" << endl;

    double totalReward = 0.0;
    double discount = 1.0;
    bool terminal = false;
    int numSteps;
		int action;

  //Sampler* MDPSampler = 0;
  //MDPSampler = SampFact.getTigerPOMDPSampler(state.countsO,S,A,O,
     //Simulator.R,Simulator.rsas,Simulator.GetDiscount());
  //SIMULATOR* mdp;
    for (numSteps = 0; numSteps + TreeDepth < Params.MaxDepth && !terminal; ++numSteps)
    {
	//std::cout<<"numSteps"<<numSteps<<std::endl;
        uint observation;
        //uint statenew;
        double reward;
	//SIMULATOR* mdp=new BasicPOMDP(S,A,Simulator.GetDiscount(),Simulator.R,Simulator.rsas,state.countsO);
  	
	//action = mdp->SelectRandom(state.position, Status);
				
	//std::cout<<"stepPOMDPbegin"<<std::endl;			//}
        //terminal = mdp->StepPOMDP(state.position, action, observation, statenew, reward);

	action=Simulator.SelectRandom(state,Status);
	terminal = Simulator.StepPOMDPsim(state, action, observation, reward);

	//std::cout<<"stepPOMDPend"<<std::endl;
	//delete mdp;
	/*if(action==0){
	state.countsO[statenew*SA+S*action+observation] += 1;
	//mdp->Updatemdp(state.countsO);
	


  
  //std::cout<<"mdpupdate"<<std::endl;
  

}*/



        History.Add(action, observation);

       /* if (Params.Verbose >= 4)
        {
            mdp->DisplayAction(action, cout);
            mdp->DisplayObservation(state.position, observation, cout);
            mdp->DisplayReward(reward, cout);
            mdp->DisplayState(state.position, cout);
        }

	*/
			//state = statenew;
        //state.position=statenew;
        totalReward += reward * discount;
        discount *= Simulator.GetDiscount();
    }
//delete MDPSampler;
//delete mdp;
    StatRolloutDepth.Add(numSteps);
    if (Params.Verbose >= 3)
        cout << "Ending rollout after " << numSteps
            << " steps, with total reward " << totalReward << endl;
    return totalReward;
}

double BAPOMCP::UCB[UCB_N][UCB_n];
bool BAPOMCP::InitialisedFastUCB = true;

void BAPOMCP::InitFastUCB(double exploration)
{
    cout << "Initialising fast UCB table with exp. const. "
			<< exploration << "...  ";
    for (int N = 0; N < UCB_N; ++N)
        for (int n = 0; n < UCB_n; ++n)
            if (n == 0)
                UCB[N][n] = Infinity;
            else
                UCB[N][n] = exploration * sqrt(log(N + 1) / n);
    cout << "done" << endl;
    InitialisedFastUCB = true;
}


inline double BAPOMCP::FastUCB(int N, int n, double logN) const
{
    if (InitialisedFastUCB && N < UCB_N && n < UCB_n)
        return UCB[N][n];

    if (n == 0)
        return Infinity;
    else
        return Params.ExplorationConstant * sqrt(logN / n);
}

void BAPOMCP::ClearStatistics()
{
    StatTreeDepth.Clear();
    StatRolloutDepth.Clear();
    StatTotalReward.Clear();
}

void BAPOMCP::DisplayStatistics(ostream& ostr) const
{
    if (Params.Verbose >= 1)
    {
        StatTreeDepth.Print("Tree depth", ostr);
        StatRolloutDepth.Print("Rollout depth", ostr);
        StatTotalReward.Print("Total reward", ostr);
    }

    if (Params.Verbose >= 2)
    {
        ostr << "Policy after " << Params.NumSimulations << " simulations" << endl;
        DisplayPolicy(6, ostr);
        ostr << "Values after " << Params.NumSimulations << " simulations" << endl;
        DisplayValue(6, ostr);
    }
}

void BAPOMCP::DisplayValue(int depth, ostream& ostr) const
{
    HISTORY history;
    ostr << "BAPOMCP Values:" << endl;
    Root->DisplayValue(history, depth, ostr);
}

void BAPOMCP::DisplayPolicy(int depth, ostream& ostr) const
{
    HISTORY history;
    ostr << "BAPOMCP Policy:" << endl;
    Root->DisplayPolicy(history, depth, ostr);
}


bool BAPOMCP::AddTransforms(VNODE* root, BELIEF_STATE& beliefs)//update particle
{
    int attempts = 0, added = 0,maxattempts=0;
    double probsum=0;
   
    //SIMULATOR* mdp1;
    //std::cout<<"creatmdp"<<std::endl;
    //mdp1 = (SIMULATOR*) new BasicPOMDP(S,A,Simulator.GetDiscount(),Simulator.R,Simulator.rsas,counts1);

    // Local transformations of state that are consistent with history
    while (added < 1000&&maxattempts<1000*1000)
    {
	//std::cout<<"added"<<added<<std::endl;
        STATE* transform = CreateTransform();
        if (transform)
        {
            beliefs.AddSample(transform);
            added++;
	   		//std::cout<<"add"<<added<<std::endl;
        	probsum+=Simulator.GetStateProb(transform);
		}
	maxattempts++;
	
	
       
    }
    //std::cout<<probsum<<std::endl;
    while(attempts<1000&&added==1000)
    {
	STATE* state = beliefs.GetSampleByIndex(attempts);
	Simulator.Statenormalized(*state,probsum);
	attempts++;

    }

    if(added<1000)
	return false;
    return true;
    //std::cout<<"attemptend"<<std::endl;
    //delete mdp1;
    //delete MDPSampler;
    // if (Params.Verbose >= 1)
    // {
    //     cout << "Created " << added << " local transformations out of "
    //         << attempts << " attempts" << endl;
    // }
}

STATE* BAPOMCP::CreateTransform() const
{
    //uint observation;
    //double immediateReward;

    STATE* state = Root->Beliefs().CreateSampleWithProb(Simulator);
    //std::cout<<"startStateUpdateWithMC"<<std::endl;
    //if (Simulator.LocalMove(*state,History,stepObs))
    
    bool terminal = Simulator.StateUpdateWithMC(*state, History.Back().Action, History.Back().Observation);
	
    if(terminal)
	{
		 //std::cout<<"startStateUpdateWithMCsuc"<<std::endl;
		//std::cout<<"successsuccesssuccesssuccesssuccesssuccesssuccesssuccesssuccesssuccesssuccess"<<std::endl;
		return state;
	}
     
	//std::cout<<"fail"<<std::endl;	
	Simulator.FreeState(state);
	//std::cout<<"startStateUpdateWithMCsfail"<<std::endl;
	return 0;
	
   
    
    
}

void BAPOMCP::GetNewBelief(BELIEF_STATE& beliefs)
{
	int index;
	for(index=0;index<1000;index++)
		{
			//std::cout<<"getindex"<<std::endl;
			STATE* state = Root->Beliefs().CreateSamplebyindex(Simulator,index);
			//std::cout<<"resetposition"<<std::endl;
			Simulator.ResetPosition(*state);
			//std::cout<<"resetpositionend"<<std::endl;
			beliefs.AddSample(state);
		}
}


void BAPOMCP::AddSample(VNODE* node, const STATE& state)
{
    //STATE* sample = Simulator.Copy(state);
    //node->Beliefs().AddSample(sample);
    // if (Params.Verbose >= 2)
    // {
    //     cout << "Adding sample:" << endl;
    //     Simulator.DisplayState(*sample, cout);
    // }
}


//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
