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
		
		S = Simulator.GetNumStates();
		SA = S*A;
		SAS = S*A*S;


		step = 0;
		continueData=new std::vector<Symbol*>;

}


//SVD
void PSRMCTS::SVD(CMatrix Pth)
{

	
	

	CMatrix Utemp;
	CMatrix Vtemp;
	Pth.SplitUV(Utemp,Vtemp);
	
	int rank=Pth.RankGauss();
	std::cout<<"real rank"<<rank<<std::endl;
	
	rank=50;
	


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

	//P(H）
	CMatrix PHM(PH->dim1(),1);
	for(int i=0;i<PH->dim1();i++)
	{
		for(int j=0;j<1;j++)
		{
			double k=(*PH)[i];
			PHM.SetElement(i,j,k);
		}
	}
	
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
    int O=Simulator.GetNumObservations();
    std::vector<double> probvector;
    double prob=(float) random() * 1.0 / (float) RAND_MAX;
    double probtemp=0.0;
    double probsum=0.0;
    double probc=0.0;
    uint result=0;
    for(int o=0;o<O;o++)
	{
		int ao=action*O+o;
		CMatrix Baotemp(Bao->dim2(),Bao->dim3());
		for(int Baoi=0;Baoi<Bao->dim2();Baoi++)
		{
			for(int Baoj=0;Baoj<Bao->dim3();Baoj++)
			{
				double k=(*Bao)[ao][Baoi][Baoj];
				Baotemp.SetElement(Baoi,Baoj,k);
				
			}

		}
		probtemp=((*binfiT)*(Baotemp)*(*bt)).GetElement(0,0);
		if(probtemp<0)
		{probtemp=0;}
		probvector.push_back(probtemp);		
		probsum+=probtemp;			
		

	}
	for(uint i=0;i<probvector.size();i++)
	{
		probc=probc+probvector[i]/probsum;
		if(probc>prob)
		{result=i;break;}
		
	}

    return result;
	


}

//get observation by PSR model
uint PSRMCTS::getPSRresultAOless(uint action,CMatrix* bt)
{
    
    int startposition=(*actionIndex)[action];
    int Onum=(*AOnum)[action];
    std::vector<double> probvector;
    double prob=(float) random() * 1.0 / (float) RAND_MAX;
    double probtemp=0.0;
    double probsum=0.0;
    double probc=0.0;
    uint result=0;
    for(int o=0;o<Onum;o++)
	{
		int ao=startposition+o;
		CMatrix Baotemp(Bao->dim2(),Bao->dim3());
		for(int Baoi=0;Baoi<Bao->dim2();Baoi++)
		{
			for(int Baoj=0;Baoj<Bao->dim3();Baoj++)
			{
				double k=(*Bao)[ao][Baoi][Baoj];
				Baotemp.SetElement(Baoi,Baoj,k);
				
			}


		}
		probtemp=((*binfiT)*(Baotemp)*(*bt)).GetElement(0,0);
		if(probtemp<0)
		{probtemp=0;}
		probvector.push_back(probtemp);

		
		probsum+=probtemp;
	}
	for(uint i=0;i<probvector.size();i++)
	{
		probc=probc+probvector[i]/probsum;
		if(probc>prob)
		{result=i;break;}

	}

	result=startposition+result;
	result=(*index)[result];
	result=getObyAO(result);
	
	
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



void PSRMCTS::ConstructSameTree()
{

	CopyTree(Root,usedRoot);

}


void PSRMCTS::ConstructSameTree1()
{

	//CopyTree(Root,usedRoot);
	usedRoot=ExpandNode();

}


void PSRMCTS::ConstructTree()
{
    ClearStatistics();
    int historyDepth = History.Size();
    for (int n = 0; n < 100; n++)
    {
       	
        TreeDepth = 0;
        PeakTreeDepth = 0;
	uint state=Simulator.CreateStartState();
	double totalReward = SimulateV(state, Root);
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






symList2IntMap * PSRMCTS::getHistcout()
{
   divideData=Spectral.divideData(continueData);
   string TXTNAME="historyData.txt";
   return Spectral.getHistcout(divideData,1,TXTNAME);
}




symSet *PSRMCTS::getAOQset(symSet* AOset,symSet* Testset)
{
	symSet *AOQset = new symSet();
	AOQset=Spectral.getAOQset(AOset,Testset);
	return AOQset;

}


Array1D<double>* PSRMCTS::getPH(symList2IntMap *Histcout)
{
	PH=new Array1D<double>(Histcout->size());
	PH=Spectral.getPH(Histcout);
	ofstream phtxt("PH.txt");
	for(int i=0;i<PH->dim();i++)
	{phtxt<<(*PH)[i]<<endl;}
	return PH;
}

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




int PSRMCTS::SelectActionPSR()
{
  UCTSearch();
  step++; 
  return GreedyUCB(usedRoot,false);

}



//PSR-MCTS
void PSRMCTS::UCTSearch()
{

    
    ClearStatistics();
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

    
}



void PSRMCTS::UpdatePSR(uint action,uint observation,bool& find)
{
    History.Add(action, observation);
    int O=Simulator.GetNumObservations();
    int ao=action*O+observation;
    ao=getAOindex(ao,find);
    UpdateBt(ao,btupdate);
    	

    

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
    //uint action = GreedyUCB(vnode, true);
    uint action = Simulator.SelectRandom(state, Status);

    PeakTreeDepth = TreeDepth;
    //if (TreeDepth >= Params.MaxDepth) // search horizon reached
    if (TreeDepth >= 20) // search horizon reached
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

    observation=getPSRresultAOless(action,btused);
    terminal=Simulator.CheckTerminalbyO(observation);
    immediateReward=Simulator.GetRewardbyAO(observation,action);
    int O=Simulator.GetNumObservations();
    int ao=action*O+observation;
    bool find;
    ao=getAOindex(ao,find);
    UpdateBt(ao,btused);

	
    History.Add(action, observation);


    VNODE*& vnode = qnode.Child(observation);
    if (!vnode && !terminal && qnode.Value.GetCount() >= Params.ExpandCount)
        vnode = ExpandNode(); 

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
	
	observation=getPSRresultAOless(action,btused);
	terminal=Simulator.CheckTerminalbyO(observation);
    	reward=Simulator.GetRewardbyAO(observation,action);
	int O=Simulator.GetNumObservations();
    	int ao=action*O+observation;
	bool find;
	ao=getAOindex(ao,find);
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
    for (numSteps = 0; numSteps + TreeDepth < 20/*Params.MaxDepth*/ && !terminal; ++numSteps)
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
    ostr << "BAMCP Values:" << endl;
    Root->DisplayValue(history, depth, ostr);
}

void PSRMCTS::DisplayPolicy(int depth, ostream& ostr) const
{
    HISTORY history;
    ostr << "BAMCP Policy:" << endl;
    Root->DisplayPolicy(history, depth, ostr);
}

void PSRMCTS::getsysAOsetpossibleidInAOset(symSet* AOsetpos,symSet* AOset)
{
	
	ofstream indextxt("index.txt");
	index=new Array1D<int>(AOsetpos->size());
	symIter s=AOset->begin();
	symIter s1=AOsetpos->begin();
	for(uint i=0;i<AOsetpos->size();i++,s1++)
	{
		for(uint j=0;j<AOset->size();j++,s++)
		{
			
			if(**s1==**s)
			{
				(*index)[i]=j;
				indextxt<<(*index)[i]<<std::endl;
				
				break;
			}
			
		}
		s=AOset->begin();
	}

}


void PSRMCTS::getAOnumByA()
{
	AOnum=new Array1D<int>(A);
	ofstream AOnumByA("getAOnumByA.txt");	
	uint O=Simulator.GetNumObservations();
	for(uint i=0;i<A;i++)
	{	
		(*AOnum)[i]=0;
		for(int j=0;j<index->dim1();j++)
		{
			if(((*index)[j]/O)==i)
			{
				(*AOnum)[i]=(*AOnum)[i]+1;
			}			
		}
		AOnumByA<<(*AOnum)[i]<<std::endl;
	}	


}

void PSRMCTS::getActBeginIndex()
{
	ofstream getActBeginIndex("getActBeginIndex.txt");
	int O=Simulator.GetNumObservations();
	actionIndex=new Array1D<int>(A);
	for(int i=0;i<actionIndex->dim1();i++)
	{
		for(int j=0;j<index->dim1();j++)
		{
			if((*index)[j]/O==i)
			{
				(*actionIndex)[i]=j;
				break;
			}

		}
		getActBeginIndex<<(*actionIndex)[i]<<std::endl;

	}

}

int PSRMCTS::getAOindex(int ao,bool& find)
{
	int k;
	bool flag=false;
	for(int i=0;i<index->dim1();i++)
	{
		if((*index)[i]==ao)
		{
			k=i;
			flag=true;
			find=true;
			break;
		}
			
	}
	
	if(!flag)
	{
		find=false;
		//std::cout<<"not found the ao"<<std::endl;
		int O=Simulator.GetNumObservations();		
		int a=ao/O;
		k=(*actionIndex)[a];

	}
	return k; 
	
}

int PSRMCTS::getObyAO(int ao)
{
	int O=Simulator.GetNumObservations();
	return ao%O;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
