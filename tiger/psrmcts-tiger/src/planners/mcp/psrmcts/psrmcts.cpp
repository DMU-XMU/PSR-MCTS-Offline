#include "psrmcts.h"
#include <math.h>
#include <algorithm>
#include <iomanip>
#include "node.h"



using namespace std;
using namespace UTILS;

//-----------------------------------------------------------------------------

PSRMCTS::PARAMS::PARAMS()
:   Verbose(0),
    MaxDepth(100),
    NumSimulations(1000),
    ExpandCount(1),
    ExplorationConstant(1),
		ReuseTree(false)
{
}

PSRMCTS::PSRMCTS(const SIMULATOR& simulator, const PARAMS& params,
		SPECTRAL& spectral)
:   Params(params),
		Simulator(simulator),
    TreeDepth(0),
	Spectral(spectral)
{
    VNODE::NumChildren = Simulator.GetNumActions();
    QNODE::NumChildren = Simulator.GetNumObservations();

    Root = ExpandNode();
	

    A = Simulator.GetNumActions();
		
    step = 0;
    continueData=new std::vector<Symbol*>;


}

void PSRMCTS::SVD(CMatrix Pth)//SVD
{


	CMatrix Utemp;
	CMatrix Vtemp;
	Pth.SplitUV(Utemp,Vtemp);
	
	int rank=Pth.RankGauss();
	std::cout<<"Rank="<<rank<<std::endl;
	
	ofstream Utxt("U.txt");
	U=new Array2D<double>(Pth.GetNumRows(),rank);
	for(int i=0;i<Utemp.GetNumRows();i++)
	{
		for(int j=0;j<rank;j++)
		{
			(*U)[i][j]=Utemp.GetElement(i,j);
			
			Utxt<<(*U)[i][j]<<" ";
		}
		
		Utxt<<std::endl;
	}


}


//build PSR model
void PSRMCTS::getSpectral()
{	
	
	CMatrix onekM(PTH->dim2(),1);
	for(int i=0;i<PTH->dim2();i++)
	{
		onekM.SetElement(i,0,1);;
	}

	//P(H)
	CMatrix PHM(PH->dim1(),1);
	for(int i=0;i<PH->dim1();i++)
	{
		for(int j=0;j<1;j++)
		{
			double k=(*PH)[i];
			PHM.SetElement(i,j,k);
		}
	}
	//P(T|H)*P(H)
	CMatrix PTHM(PTH->dim1(),PTH->dim2());
	for(int i=0;i<PTH->dim1();i++)
	{
		for(int j=0;j<PTH->dim2();j++)
		{
			double k=(*PTH)[i][j]*(*PH)[j];
			PTHM.SetElement(i,j,k);
		}
	}
	
	SVD(PTHM);	
	
	
	CMatrix UM(U->dim1(),U->dim2());
	for(int i=0;i<U->dim1();i++)
	{
		for(int j=0;j<U->dim2();j++)
		{
			double k=(*U)[i][j];
			UM.SetElement(i,j,k);
		}
	}
	
	CMatrix PAOHM(PAOH->dim1(),PAOH->dim2());
	for(int i=0;i<PAOH->dim1();i++)
	{
		for(int j=0;j<PAOH->dim2();j++)
		{
			double k=(*PAOH)[i][j]*(*PH)[j];
			PAOHM.SetElement(i,j,k);
		}
	}
	


	CMatrix UMT;
	UMT=UM.Transpose(); 
	

	CMatrix inv,Utemp,Vtemp;
	(UMT*PTHM).GInvertUV(inv,Utemp,Vtemp);
	ofstream invtxt("inv.txt");
	for(int i=0;i<inv.GetNumRows();i++)
	{
		for(int j=0;j<inv.GetNumColumns();j++)
		{
			invtxt<<inv.GetElement(i,j)<<" ";


		}
		invtxt<<std::endl;
	}


	
	//b1	
	
	b1=new CMatrix();
	*b1=UMT*PTHM*onekM;
	btupdate=new CMatrix(b1->GetNumRows(),b1->GetNumColumns());
	for(int b1i=0;b1i<b1->GetNumRows();b1i++)
	{
		
		btupdate->SetElement(b1i,0,b1->GetElement(b1i,0));

	}

	btused=new CMatrix(b1->GetNumRows(),b1->GetNumColumns());
	b1used=new Array1D<double>(b1->GetNumRows());

	ofstream b1txt("b1.txt");
	for(int b1i=0;b1i<b1->GetNumRows();b1i++)
	{
		
		(*b1used)[b1i]=b1->GetElement(b1i,0);
		b1txt<<(*b1used)[b1i]<<" ";

	}


	//binfiT
	
	binfiT=new CMatrix();
	*binfiT=(PHM.Transpose())*inv;
	
	

	binfiTused=new Array2D<double>(binfiT->GetNumRows(),binfiT->GetNumColumns());
	ofstream binfiTtxt("binfiT.txt");
	for(int binfii=0;binfii<binfiT->GetNumRows();binfii++)
	{
		for(int binfij=0;binfij<binfiT->GetNumColumns();binfij++)
		{
			(*binfiTused)[binfii][binfij]=binfiT->GetElement(binfii,binfij);
			binfiTtxt<<(*binfiTused)[binfii][binfij]<<" ";
		}
		binfiTtxt<<std::endl;
	}

	PAOQH3Djoint=new Array3D<double>(PAOQH3D->dim1(),PAOQH3D->dim2(),PAOQH3D->dim3());	
	

	//Bao
	
	for(int ao=0;ao<PAOQH3D->dim1();ao++)
	{
		CMatrix PaoTHM(PTH->dim1(),PTH->dim2());
		for(int i=0;i<PTH->dim1();i++)
		{
			for(int j=0;j<PTH->dim2();j++)
			{
				double k=(*PAOQH3D)[ao][i][j]*(*PAOH)[ao][j]*(*PH)[j];
				PaoTHM.SetElement(i,j,k);
				(*PAOQH3Djoint)[ao][i][j]=k;
			}
		}
				


	}
	
	Bao=new Array3D<double>(PAOQH3D->dim1(),UMT.GetNumRows(),inv.GetNumColumns());
	for(int ao=0;ao<PAOQH3Djoint->dim1();ao++)
	{
		CMatrix PaoTHMtemp(UMT.GetNumColumns(),inv.GetNumRows());
		for(int i=0;i<UMT.GetNumColumns();i++)
		{
			for(int j=0;j<UMT.GetNumRows();j++)
			{
				double k=(*PAOQH3Djoint)[ao][i][j];
				PaoTHMtemp.SetElement(i,j,k);
			}
		}


		CMatrix Baotemp=UMT*PaoTHMtemp*inv;


	
		

		for(int k=0;k<Baotemp.GetNumRows();k++)
		{
			for(int l=0;l<Baotemp.GetNumColumns();l++)
			{
				(*Bao)[ao][k][l]=Baotemp.GetElement(k,l);
			}
		}
	
	}
	

}

//get observation by PSR model
uint PSRMCTS::getPSRresult(uint action,CMatrix* bt)
{
    double prob=(float) random() * 1.0 / (float) RAND_MAX;
    double probtemp=0.0;
    uint result=0;
    for(int o=0;o<4;o++)
	{
		int ao=action*4+o;
		CMatrix Baotemp(Bao->dim2(),Bao->dim3());
		for(int Baoi=0;Baoi<Bao->dim2();Baoi++)
		{
			for(int Baoj=0;Baoj<Bao->dim3();Baoj++)
			{
				double k=(*Bao)[ao][Baoi][Baoj];
				Baotemp.SetElement(Baoi,Baoj,k);
				
			}
			

		}
		
		
		probtemp+=((*binfiT)*(Baotemp)*(*bt)).GetElement(0,0);
		
		if(probtemp>prob)
		{result=o;break;}
		
		
			
		

	}
    return result;
	
	

}
//update the belief of PSR
void PSRMCTS::UpdateBt(int ao,CMatrix*& bt)
{
	CMatrix Baotemp1(Bao->dim2(),Bao->dim3());
	for(int Baoi=0;Baoi<Bao->dim2();Baoi++)
	{
		for(int Baoj=0;Baoj<Bao->dim3();Baoj++)
		{
			double k=(*Bao)[ao][Baoi][Baoj];
			Baotemp1.SetElement(Baoi,Baoj,k);
		}
	}	

	
	(*bt)=(Baotemp1)*(*bt)*(1/((*binfiT)*(Baotemp1)*(*bt)).GetElement(0,0));
	

}

//copy offline tree
void PSRMCTS::ConstructSameTree()
{
	
	CopyTree(Root,usedRoot);

}

//construct offline tree
void PSRMCTS::ConstructTree()
{
    ClearStatistics();
    int historyDepth = History.Size();
    for (int n = 0; n < 0; n++)
    {
	
        TreeDepth = 0;
        PeakTreeDepth = 0;
		uint state=Simulator.CreateStartState();
				double totalReward = SimulateV(state, Root);
		//std::cout<<state<<std::endl;
        StatTotalReward.Add(totalReward);
        StatTreeDepth.Add(PeakTreeDepth);

        if (Params.Verbose >= 2)
            cout << "Total reward = " << totalReward << endl;
        if (Params.Verbose >= 4)
            DisplayValue(4, cout);

        History.Truncate(historyDepth);

				
	}
  	
  ofstream PsrDataId  ("Psrdataid.txt");
  ofstream PsrData  ("Psrdata.txt");
  std::vector<Symbol*>::iterator vi=continueData->begin();
  for(vi=continueData->begin();vi!=continueData->end();vi++)
  {
    PsrData<<**vi<<endl;
  }

  std::vector<int> symbolId;
  for(uint pos=0;pos<continueData->size();)
  {
    symList *act = Spectral.getMemory(continueData, pos, 1);pos++;
    int aId=act->s->id;
    symbolId.push_back(aId);
    PsrDataId<<aId<<endl;
    delete act;

    symList *obs=Spectral.getMemory(continueData,pos,1);pos++;
    int oId=obs->s->id;
    symbolId.push_back(oId);
    PsrDataId<<oId<<endl;
    delete obs;
  }	
    DisplayStatistics(cout);

}


void PSRMCTS::CopyTree(VNODE* vnode,VNODE*& vnodenew)
{	
	double valueToCopy;
	int countToCopy;
	valueToCopy=vnode->Value.GetTotal();
	countToCopy=vnode->Value.GetCount();
	vnodenew=ExpandNode();
	vnodenew->Value.CopySet(countToCopy,valueToCopy);
	for(uint action=0;action<A;action++)
	{
		
		QNODE& qnode = vnode->Child(action);
        	valueToCopy = qnode.Value.GetTotal();
        	countToCopy = qnode.Value.GetCount();		
		QNODE& qnodeToUsed = vnodenew->Child(action);
		qnodeToUsed.Value.CopySet(countToCopy,valueToCopy);
		for(uint observation=0;observation<uint(Simulator.GetNumObservations());observation++)
			{
				VNODE*& vnodechildren = qnode.Child(observation);
				if(vnodechildren)
				{
					VNODE*& vnodeToUsed=qnodeToUsed.Child(observation);
					CopyTree(vnodechildren,vnodeToUsed);
				}			
			}		
	}
	
	
}





void PSRMCTS::DisplayTree(VNODE* vnode)
{

	double valueToCopy;
	int countToCopy;
	valueToCopy=vnode->Value.GetTotal();
	countToCopy=vnode->Value.GetCount();
	std::cout<<"vnode"<<valueToCopy<<"-"<<countToCopy<<std::endl;

	for(uint action=0;action<A;action++)
	{

		QNODE& qnode = vnode->Child(action);
        valueToCopy = qnode.Value.GetTotal();
        countToCopy = qnode.Value.GetCount();		
		std::cout<<"qnode"<<valueToCopy<<"-"<<countToCopy<<std::endl;

		for(uint observation=0;observation<uint(Simulator.GetNumObservations());observation++)
			{
				VNODE*& vnodechildren = qnode.Child(observation);
				if(vnodechildren)
				{
					DisplayTree(vnodechildren);
				}

			
			}
		
		
	}
	
}







symSet *PSRMCTS::getAOQset(symSet* AOset,symSet* Testset)
{
	symSet *AOQset = new symSet();
	AOQset=Spectral.getAOQset(AOset,Testset);
	return AOQset;

}

//P(H)
Array1D<double>* PSRMCTS::getPH(symList2IntMap *Histcout)
{
	PH=new Array1D<double>(Histcout->size());
	PH=Spectral.getPH(Histcout);
	ofstream phtxt("PH.txt");
	for(int i=0;i<PH->dim();i++)
	{phtxt<<(*PH)[i]<<endl;}
	return PH;
}
//P(T|H)
Array2D<double>*PSRMCTS::getProb( symSet* interestTests,std::vector<float>*belief,int action_N,symList2IntMap *Histcout,int flag)
{

	int h_len=Histcout->size();
	int size_m=h_len;
  	int size_n=interestTests->size();
  	Array2D<double>*ctProb  = new Array2D<double>(size_n, size_m);
  	*ctProb=0.0;
	ctProb=Spectral.getProb(interestTests,belief,action_N,Histcout,flag);
	return ctProb;

}

PSRMCTS::~PSRMCTS()
{
	

	VNODE::Free(Root, Simulator);
  	VNODE::FreeAll();
}



//PSR-MCTS
int PSRMCTS::SelectActionPSR()
{
  UCTSearch();
  step++; 
  return GreedyUCB(usedRoot,false);

}

void PSRMCTS::UCTSearch()
{

    
    ClearStatistics();
    //std::cout<<"begin"<<std::endl;
    int historyDepth = History.Size();
    
    
    for (int n = 0; n < Params.NumSimulations; n++)
    {
	for(int i=0;i<btupdate->GetNumRows();i++)
	{
		for(int j=0;j<btupdate->GetNumColumns();j++)
		{
			btused->SetElement(i,j,btupdate->GetElement(i,j));
		}
	}
	
	

	
	Status.Phase = SIMULATOR::STATUS::TREE;
	TreeDepth = 0;
        PeakTreeDepth = 0;
	double totalReward = SimulateVPSR(usedRoot);
        StatTotalReward.Add(totalReward);
        StatTreeDepth.Add(PeakTreeDepth);
        History.Truncate(historyDepth);



     }
     //std::cout<<"bt"<<std::endl;	
     
    
}


//update the tree
void PSRMCTS::UpdatePSR(uint action,uint observation)
{
    History.Add(action, observation);
    UpdateBt(action*4+observation,btupdate);

    //Reuse previous subtree for next search
    //Get to subtree
    QNODE& qnode = usedRoot->Child(action);
    VNODE* vnode = qnode.Child(observation);
    if(vnode){
        
      //Delete other subtrees but this one
      vnode->preventfree = true;
      VNODE::Free(usedRoot,Simulator);
      vnode->preventfree = false;
      usedRoot = vnode;
    }else{
      
      // Delete old tree and create new root
      VNODE::Free(usedRoot,Simulator);
      VNODE* newRoot = ExpandNode();
      usedRoot = newRoot;
    }

}

void PSRMCTS::FreeusedRoot()
{

	VNODE::Free(usedRoot, Simulator);

}


double PSRMCTS::SimulateV(uint state, VNODE* vnode)
{
    
    uint action = Simulator.SelectRandom(state, Status);

    PeakTreeDepth = TreeDepth;
    if (TreeDepth >= Params.MaxDepth) // search horizon reached
        return 0;

    QNODE& qnode = vnode->Child(action);
    double totalReward = SimulateQ(state, qnode, action);
    vnode->Value.Add(totalReward);
    return totalReward;
}



double PSRMCTS::SimulateVPSR(VNODE* vnode)
{
    uint action = GreedyUCB(vnode, true);

    PeakTreeDepth = TreeDepth;
    if (TreeDepth >= Params.MaxDepth) // search horizon reached
        return 0;

    QNODE& qnode = vnode->Child(action);
    double totalReward = SimulateQPSR(qnode, action);
    vnode->Value.Add(totalReward);
    return totalReward;
}


double PSRMCTS::SimulateQPSR(QNODE& qnode, uint action)
{
    uint observation;
    double immediateReward, delayedReward = 0;
    bool terminal;
    observation=getPSRresult(action,btused);
    terminal=Simulator.CheckTerminalbyO(observation);
    immediateReward=Simulator.GetRewardbyO(observation);
    int ao=action*4+observation;
    UpdateBt(ao,btused);

	
    History.Add(action, observation);


    VNODE*& vnode = qnode.Child(observation);
    if (!vnode && !terminal && qnode.Value.GetCount() >= Params.ExpandCount)
        vnode = ExpandNode(); //&state);

    if (!terminal)
    {
        TreeDepth++;
        if (vnode)
            delayedReward = SimulateVPSR(vnode);
        else{
						
	    delayedReward = RolloutPSR();

	    }
        TreeDepth--;
    }

    double totalReward = immediateReward + Simulator.GetDiscount() * delayedReward;
    qnode.Value.Add(totalReward);
    return totalReward;
}


double PSRMCTS::RolloutPSR()
{
    Status.Phase = SIMULATOR::STATUS::ROLLOUT;

    double totalReward = 0.0;
    double discount = 1.0;
    bool terminal = false;
    int numSteps;
    int action;
    for (numSteps = 0; numSteps + TreeDepth < Params.MaxDepth && !terminal; ++numSteps)
    {
	
        uint observation;
        double reward;				
	action = Simulator.SelectRandom(Status);
	observation=getPSRresult(action,btused);
	terminal=Simulator.CheckTerminalbyO(observation);
    	reward=Simulator.GetRewardbyO(observation);
    	int ao=action*4+observation;
    	UpdateBt(ao,btused);


	
        History.Add(action, observation);
	
        totalReward += reward * discount;
        discount *= Simulator.GetDiscount();
    }

    StatRolloutDepth.Add(numSteps);
    return totalReward;
}


double PSRMCTS::SimulateQ(uint state, QNODE& qnode, uint action)
{
    uint observation;
	uint statenew;
    double immediateReward, delayedReward = 0;
	bool terminal = Simulator.StepPOMDP(state, action, observation, statenew, immediateReward);
	actionToSend=Spectral.getAction(action);
	obsReceived=Spectral.getObservation(observation);
	continueData->push_back(actionToSend);
    continueData->push_back(obsReceived); 
	
    History.Add(action, observation);

    

		VNODE*& vnode = qnode.Child(observation);
    if (!vnode && !terminal && qnode.Value.GetCount() >= Params.ExpandCount)
        vnode = ExpandNode(); 

    if (!terminal)
    {
        TreeDepth++;
        if (vnode)
            delayedReward = SimulateV(statenew, vnode);
        else{
						
			delayedReward = Rollout(statenew);

				}
        TreeDepth--;
    }

    double totalReward = immediateReward + Simulator.GetDiscount() * delayedReward;
    qnode.Value.Add(totalReward);
    return totalReward;
}



VNODE* PSRMCTS::ExpandNode()
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




int PSRMCTS::GreedyUCB(VNODE* vnode, bool ucb) const
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

				if (ucb)
            q += FastUCB(N, n, logN);


	if(!ucb)
	{
	//std::cout<<action<<"--"<<q<<std::endl;
	}

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



double PSRMCTS::Rollout(uint state)
{
    Status.Phase = SIMULATOR::STATUS::ROLLOUT;
    if (Params.Verbose >= 3)
        cout << "Starting rollout" << endl;

    double totalReward = 0.0;
    double discount = 1.0;
    bool terminal = false;
    int numSteps;
		int action;
	uint statenew;
    for (numSteps = 0; numSteps + TreeDepth < Params.MaxDepth && !terminal; ++numSteps)
    {
        uint observation;
        double reward;
				
		action = Simulator.SelectRandom(state, Status);
		terminal = Simulator.StepPOMDP(state, action, observation, statenew, reward);	
		actionToSend=Spectral.getAction(action);
		obsReceived=Spectral.getObservation(observation);
		continueData->push_back(actionToSend);
    	continueData->push_back(obsReceived); 	
       
        History.Add(action, observation);

				state = statenew;
        totalReward += reward * discount;
        discount *= Simulator.GetDiscount();
    }

    StatRolloutDepth.Add(numSteps);
    if (Params.Verbose >= 3)
        cout << "Ending rollout after " << numSteps
            << " steps, with total reward " << totalReward << endl;
    return totalReward;
}

double PSRMCTS::UCB[UCB_N][UCB_n];
bool PSRMCTS::InitialisedFastUCB = true;

void PSRMCTS::InitFastUCB(double exploration)
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


inline double PSRMCTS::FastUCB(int N, int n, double logN) const
{
    if (InitialisedFastUCB && N < UCB_N && n < UCB_n)
        return UCB[N][n];

    if (n == 0)
        return Infinity;
    else
        return Params.ExplorationConstant * sqrt(logN / n);
}

void PSRMCTS::ClearStatistics()
{
    StatTreeDepth.Clear();
    StatRolloutDepth.Clear();
    StatTotalReward.Clear();
}

void PSRMCTS::DisplayStatistics(ostream& ostr) const
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

void PSRMCTS::DisplayValue(int depth, ostream& ostr) const
{
    HISTORY history;
    ostr << "PSRMCTS Values:" << endl;
    Root->DisplayValue(history, depth, ostr);
}

void PSRMCTS::DisplayPolicy(int depth, ostream& ostr) const
{
    HISTORY history;
    ostr << "PSRMCTS Policy:" << endl;
    Root->DisplayPolicy(history, depth, ostr);
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
