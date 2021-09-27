#include "tiger.h"
#include "utils.h"
#include "utils2.h"
#include <algorithm>

using namespace UTILS;

//Tiger
Tiger::Tiger(double discount)
{
		A = 3;
		S = 2;
		O = 4;
		NumObservations = 4;
		NumActions = 3;
		Discount = discount;
		RewardRange = 110*20;
		
		T1=new double[S*A*S];
		O1=new double[S*A*O];
		
		T1[0]=1.0;
		T1[1]=0.0;
		T1[2]=1.0;
		T1[3]=0.0;
		T1[4]=1.0;
		T1[5]=0.0;
		T1[6]=0.0;
		T1[7]=1.0;
		T1[8]=0.0;
		T1[9]=1.0;
		T1[10]=0.0;
		T1[11]=1.0;

		O1[0]=0.85;
		O1[1]=0.15;
		O1[2]=0.0;
		O1[3]=0.0;
		O1[4]=0.0;
		O1[5]=0.0;
		O1[6]=1.0;
		O1[7]=0.0;
		O1[8]=0.0;
		O1[9]=0.0;
		O1[10]=0.0;
		O1[11]=1.0;
		O1[12]=0.15;
		O1[13]=0.85;
		O1[14]=0.0;
		O1[15]=0.0;
		O1[16]=0.0;
		O1[17]=0.0;
		O1[18]=0.0;
		O1[19]=1.0;
		O1[20]=0.0;
		O1[21]=0.0;
		O1[22]=1.0;
		O1[23]=0.0;

		
}

Tiger::~Tiger(){
	
	if(T1!=0)
		delete[] T1;
	if(O1!=0)
		delete[] O1;

}

double Tiger::getT(int i) const
{	
	return T1[i];
}


double Tiger::getO(int i) const
{	
	return O1[i];
		
}


uint Tiger::CreateStartState() const
{
		return utils::rng.rand_closed01() <= 0.5?0:1;
		
}


bool Tiger::StepPOMDP(uint state, uint action,
    uint& observation, uint& statenew, double& reward) const
{
	utils::rng.multinom(T1+state*S*A+action*S,S,statenew);
	utils::rng.multinom(O1+statenew*4*A+action*4,4,observation);
	
	if(observation==0||observation==1)
	{
		reward=-1.0;
	}else if(observation==2)
	{
		reward=-100.0;
	}else
	{
		reward=10.0;
	}

	
	
	if(action==1||action==2)
	{return true;}
	return false;

}


double Tiger::GetRewardbyO(uint observation) const
{
	double reward;
	if(observation==2)
	{reward=-100.0;}
	else if(observation==3)
	{
	  reward=10.0;
	}
	else
	{

	  reward=-1.0;
	}
        if(observation!=0&&observation!=1&&observation!=2&&observation!=3)
	{std::cout<<"observation wrong"<<std::endl;}
	return reward;

}


 bool Tiger::CheckTerminalbyO(uint observation) const
{
	bool terminal;
	if(observation==2)
	{terminal=true;}
	else if(observation==3)
	{
	  terminal=true;
	}
	else
	{

	  terminal=false;
	}
        if(observation!=0&&observation!=1&&observation!=2&&observation!=3)
	{std::cout<<"observation wrong"<<std::endl;}
	return terminal;



}


void Tiger::DisplayState(const uint state, std::ostream& ostr) const
{}

void Tiger::DisplayObservation(const uint state, uint observation, std::ostream& ostr) const
{}

void Tiger::DisplayAction(int action, std::ostream& ostr) const{
        ostr << action << std::endl;
}
