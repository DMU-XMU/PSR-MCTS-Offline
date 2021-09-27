#include "tiger.h"
#include "../utils/utils.h"
//#include "../utils/utils2.h"
#include <algorithm>


using namespace UTILS;

Tiger::Tiger(uint _L, double discount)
:   L(_L)
{
		A = 3;
		S = 2;
		O = 2;
		NumObservations = 2;
		NumActions = 3;


		Discount = discount;
		rsas = false;

    RewardRange = 110*20;

		Tc = new double[2];
		Tc[1] = 1;
		Tc[0] = 0;

		//Standard description
		//T1 = new double[S*A*S](1,0,0.5,0.5,0.5,0.5,0,1,0.5,0.5,0.5,0.5);
		//O1 = new double[S*A*O](0.85,0.15,0.5,0.5,0.5,0.5,0.15,0.85,0.5,0.5,0.5,0.5);
		T1=new double[S*A*S];
		O1=new double[S*A*O];
		//Osim=new double[S*A*O];
		T1[0]=1.0;
		T1[1]=0.0;
		T1[2]=0.5;
		T1[3]=0.5;
		T1[4]=0.5;
		T1[5]=0.5;
		T1[6]=0.0;
		T1[7]=1.0;
		T1[8]=0.5;
		T1[9]=0.5;
		T1[10]=0.5;
		T1[11]=0.5;

		O1[0]=0.85;
		O1[1]=0.15;
		O1[2]=0.5;
		O1[3]=0.5;
		O1[4]=0.5;
		O1[5]=0.5;
		O1[6]=0.15;
		O1[7]=0.85;
		O1[8]=0.5;
		O1[9]=0.5;
		O1[10]=0.5;
		O1[11]=0.5;



		R = new double[S*A];
		//std::fill(R,R+S*A,0);
		//R[(S-1)*A] = 1;
		//R[(S-1)*A+1] = 1;
		//R[(S-1)*A+2] = 1;
		//R[(S-1)*A+3] = 1;
		R[0]= -1.0;
		R[1]=-100.0;
		R[2]=10.0;
		R[3]= -1.0;
		R[4]=10.0;
		R[5]=-100.0;
}

Tiger::~Tiger(){
	if(R != 0)
		delete[] R;
	if(T != 0)
		delete[] T;
	if(T1!=0)
		delete[] T1;
	if(O1!=0)
		delete[] O1;

	delete[] Tc;
}

uint Tiger::CreateStartState() const
{
		return utils::rng.rand_closed01() <= 0.5?0:1;
		//return 0; //(rand() % S);
}

STATE* Tiger::CreatesimStartState(const uint* countsOrefine) const
{
    TIGER_STATE* tigerstate = MemoryPool.Allocate();
    tigerstate->prob=1.0/1000.0;
    //tigerstate->prob=1.0/200.0;
    tigerstate->countsO.clear();
    tigerstate->countsO.reserve(12);
    tigerstate->position = utils::rng.rand_closed01() <= 0.5?0:1;
	//std::cout<<"tigerstate="<<tigerstate->position<<std::endl;
		//tigerstate->countsO = new uint[S*A*O];
		for (uint c = 0; c < S*A*O; ++c) {
			tigerstate->countsO.push_back(10000);
		}
		/*for(uint c = 0; c < S*A*O; ++c){
			tigerstate->countsO[c]=10000;
		}*/
		/*tigerstate->countsO[0]=5;
		tigerstate->countsO[1]=3;
		tigerstate->countsO[2*3]=3;
		tigerstate->countsO[2*3+1]=5;*/
		tigerstate->countsO.at(0)=countsOrefine[0];
		tigerstate->countsO.at(1)=countsOrefine[1];
		tigerstate->countsO.at(6)=countsOrefine[2*3];
		tigerstate->countsO.at(7)=countsOrefine[2*3+1];
    return tigerstate;
}



STATE* Tiger::Copy(const STATE& state) const
{
    const TIGER_STATE& tigerstate = safe_cast<const TIGER_STATE&>(state);
    TIGER_STATE* newstate = MemoryPool.Allocate();
    *newstate = tigerstate;
    return newstate;
}

void Tiger::FreeState(STATE* state) const
{
    TIGER_STATE* tigerstate = safe_cast<TIGER_STATE*>(state);
    //delete[] state->countsT;
    //delete[] state->countsO;
    tigerstate->countsO.clear();
    MemoryPool.Free(tigerstate);
}

double Tiger::GetStateProb(STATE* state) const
{
	double prob=0;
	TIGER_STATE* tigerstate = safe_cast<TIGER_STATE*>(state);
	prob=tigerstate->prob;
	//std::cout<<"prob="<<prob<<std::endl;
	return prob;
}

void Tiger::Validate(const uint state) const{

}
bool Tiger::Step(uint state, uint action,
        uint& observation, double& reward) const
{
	return false;
}

bool Tiger::StepPOMDP(uint state, uint action,
    uint& observation, uint& statenew, double& reward) const
{
	//uint success;
	reward = R[state*A+action];
	//utils::rng.multinom(Tc,2,success);

	//Action mapping
	//       0
	//       ^
	//       |
	//  3 <-- --> 1
	//       |
	//       v
	//       2
	// if(!success){
	// 	if(utils::rng.rand_closed01() < 0.5)
	// 		action = (action + 1) % A;
	// 	else
	// 		action = (action - 1) % A;
	// }

	//Decompress state
	//uint x = state/L;
	//uint y = state-L*x;
	//observation = state;

	if(action == 0){
		if (utils::rng.rand_closed01()<=O1[state*6+action*2]) {
			observation=0;
		}
		else{
			observation=1;
		}
		statenew=state;

	}
	else if(action == 1){
		if (utils::rng.rand_closed01()<0.5) {
			observation=0;
		}
		else{
			observation=1;
		}
		if (utils::rng.rand_closed01()<0.5) {
			statenew=0;
		}
		else{
			statenew=1;
		}

		return true;
	}
	else if(action == 2){
		if (utils::rng.rand_closed01()<0.5) {
			observation=0;
		}
		else{
			observation=1;
		}
		if (utils::rng.rand_closed01()<0.5) {
			statenew=0;
		}
		else{
			statenew=1;
		}
		return true;
	}

	return false;
	//If at goal
	// if(x == L-1 && y == L-1)
	// 	observation = 0;

//	uint xp = observation/L;
	//uint yp = observation-L*xp;
	//std::cout << x << " " << y << "- " << action << " -> " << xp << " " << yp << " r:" << reward << std::endl;
	//Never terminates
	//return false;
}

bool Tiger::StepPOMDPsim(STATE& state, uint action, uint& observation, double& immediateReward)const
{
	TIGER_STATE& tigerstate = safe_cast<TIGER_STATE&>(state);
	//std::cout<<"begin"<<std::endl;
	immediateReward = 0;
	uint sSA;
	uint statenew;
	double* countsim;
	countsim=new double[12];
	for(uint i=0;i<12;i++)
	{
		//std::cout<<"countsim"<<std::endl;

		if(i<tigerstate.countsO.size())
		{		
			countsim[i]=tigerstate.countsO.at(uint(i));
		}
		else
		{
		 std::cout<<"simi>size()"<<std::endl;		
		}
		//std::cout<<countsim[i]<<std::endl;
		//std::cout<<"countsim"<<std::endl;

	}
	double* Osim;
	/*Osim=new double[S*A*O];
	for(uint s=0;s<S;++s){
		sSA = s*S*A;
		for(uint a=0;a<A;++a){
			utils::sampleDirichlet(Osim+sSA+a*S,countsim+(sSA+a*S),S,1/(double)S);
			}
		}*/
	Osim=new double[S];
	sSA = (tigerstate.position)*S*A;
	utils::sampleDirichlet(Osim,countsim+(sSA+action*S),S,1/(double)S);
	//std::cout<<"sample"<<std::endl;
	utils::rng.multinom(T1+tigerstate.position*S*A+action*S,S,statenew);
	//utils::rng.multinom(Osim+statenew*S*A+action*S,S,observation);
	utils::rng.multinom(Osim,S,observation);

	immediateReward = R[tigerstate.position*A+action];
	tigerstate.position=statenew;
	if(action==0)
	{
	  uint index=statenew*S*A+S*action+observation;
	 //std::cout<<"before"<<tigerstate.countsO[statenew*S*A+S*action+observation]<<std::endl;
	 //std::cout<<"num"<<index<<std::endl;
	 
	  if(index<tigerstate.countsO.size())
	  {
		tigerstate.countsO.at(uint(index))=countsim[index]+1;
	  }
	  else
	 {
		std::cout<<"simindex>size()"<<std::endl;}
	 //std::cout<<"after"<<tigerstate.countsO[statenew*S*A+S*action+observation]<<std::endl;
	 }
	//std::cout<<tigerstate.countsO[statenew*S*A+S*action+observation]<<std::endl;
	delete[] countsim;
	delete[] Osim;

	if(action==1||action==2)
	{return true;}

	return false;
}

bool Tiger::StateUpdateWithMC(STATE& state, uint action, uint observation)const
{
	TIGER_STATE& tigerstate = safe_cast<TIGER_STATE&>(state);
	//std::cout<<"begin"<<std::endl;
	//immediateReward = 0;
	uint sSA;
	uint statenew;
	uint observationreal;
	double* countsim;
	countsim=new double[12];
	for(uint i=0;i<12;i++)
	{
		//std::cout<<"countsim"<<std::endl;
		//countsim[i]=tigerstate.countsO.at(i);
		//std::cout<<"countsim"<<std::endl;

		if(i<tigerstate.countsO.size())
		{		
			countsim[i]=tigerstate.countsO.at(uint(i));
		}
		else
		{
		 std::cout<<"MCi>size()"<<std::endl;		
		}

	}
	double* Osim;
	/*Osim=new double[S*A*O];
	for(uint s=0;s<S;++s){
		sSA = s*S*A;
		for(uint a=0;a<A;++a){
			utils::sampleDirichlet(Osim+sSA+a*S,countsim+(sSA+a*S),S,1/(double)S);
			}
		}*/
	Osim=new double[S];
	sSA = (tigerstate.position)*S*A;
	utils::sampleDirichlet(Osim,countsim+(sSA+action*S),S,1/(double)S);
		
	
	//std::cout<<"sample"<<std::endl;
	utils::rng.multinom(T1+tigerstate.position*S*A+action*S,S,statenew);
	//utils::rng.multinom(Osim+statenew*S*A+action*S,S,observationreal);
	utils::rng.multinom(Osim,S,observationreal);
	
	
	//tigerstate.prob=tigerstate.prob*T1[tigerstate.position*S*A+S*action+statenew]*Osim[statenew*S*A+O*action+observation]+1.0/1000.0;
	tigerstate.prob=tigerstate.prob*T1[tigerstate.position*S*A+S*action+statenew]*Osim[observation]+1.0/1000.0;
	
	//immediateReward = R[tigerstate.position*A+action];
	tigerstate.position=statenew;
	if(action==0)
	{
		uint index=statenew*S*A+S*action+observation;

		if(index<tigerstate.countsO.size())
		{tigerstate.countsO.at(uint(index))=countsim[index]+1;}
		else
		{std::cout<<"MCindex>size()"<<std::endl;}

	}
	delete[] countsim;
	delete[] Osim;
	//std::cout<<"end"<<std::endl;
	if(observationreal!=observation)
		return false;
	return true;
}
void Tiger::Statenormalized(STATE& state,double probsum)const
{
	TIGER_STATE& tigerstate = safe_cast<TIGER_STATE&>(state);
	tigerstate.prob=tigerstate.prob/probsum;
}

void Tiger::ResetPosition(STATE& state)const
{
	TIGER_STATE& tigerstate = safe_cast<TIGER_STATE&>(state);
	//for(int i=0;i<12;i++)
	   //std::cout<<tigerstate.countsO[i]<<std::endl;
	//std::cout<<"***********************************"<<std::endl;
	tigerstate.position=utils::rng.rand_closed01() <= 0.5?0:1;
}

bool Tiger::LocalMove(STATE& state,const HISTORY& history, uint stpObs)const
{
	/*uint statenew;
	TIGER_STATE& tigerstate = safe_cast<TIGER_STATE&>(state);
	 uint sSA;
		for(uint s=0;s<S;++s){
		sSA = s*S*A;
		for(uint a=0;a<A;++a){
			utils::sampleDirichlet(Osim+sSA+a*S,tigerstate.countsO+(sSA+a*S),S,1/(double)S);
			}
		}
	utils::rng.multinom(T+tigerstate.position*S*A+history.Back().Action*S,S,statenew);
	utils::rng.multinom(OT+statenew*S*A+history.Back().Action*S,S,stpObs);
	std::cout<<stpObs<<std::endl;
	if(stpObs!=(uint)history.Back().Observation)
	{return false;}

        tigerstate.position=statenew;
	tigerstate.countsO[statenew*S*A+S*history.Back().Action+stpObs] += 1;
	return true;*/
	return true;

}


void Tiger::DisplayState(const uint state, std::ostream& ostr) const
{}

void Tiger::DisplayObservation(const uint state, uint observation, std::ostream& ostr) const
{}

void Tiger::DisplayAction(int action, std::ostream& ostr) const{
        ostr << action << std::endl;
}
