#include "sysadmin.h"
#include "../utils/utils.h"
#include <algorithm>


using namespace UTILS;

SYS::SYS(uint _L, double discount)
:   L(_L)
{
		A = 2*L+1;
		S = pow(2,L);
		O = (L+1)*3;
		NumObservations = O;
		NumActions = A;
		NumStates = S;


		Discount = discount;
		rsas = false;

    		RewardRange = (0-((-10)*L-20))*20;


		state1=new int[L];
		state2=new int[L];

		
		T1=new double[S*A*S];
		O1=new double[S*A*O];
		
		
		std::ofstream Ttxt("T.txt");
		for(uint s1=0;s1<S;s1++)
		{
			GetStatebin(s1,L,state1);
			for(uint a=0;a<A;a++)
			{
				for(uint s2=0;s2<S;s2++)
				{

					T1[s1*S*A+a*S+s2]=1;
					GetStatebin(s2,L,state2);
					for(uint i=0;i<L;i++)
					{
						if(a<L+1)
						{ 
							if(state1[i]==0&&state2[i]==1)
							{
								T1[s1*S*A+a*S+s2]=0;
								break;
							}else if(state1[i]==1&&state2[i]==0)
							{
								T1[s1*S*A+a*S+s2]=T1[s1*S*A+a*S+s2]*0.1;

							}else if(state1[i]==1&&state2[i]==1)
							{

								T1[s1*S*A+a*S+s2]=T1[s1*S*A+a*S+s2]*0.9;
							}						

						}
						else
						{
							if(state1[i]==0&&state2[i]==1&&(a-L)!=(i+1))
							{
								T1[s1*S*A+a*S+s2]=0;
								break;
							}else if(state2[i]==0&&(a-L)==(i+1))
							{
								T1[s1*S*A+a*S+s2]=0;
								break;
							}else if(state1[i]==1&&state2[i]==0)
							{
								T1[s1*S*A+a*S+s2]=T1[s1*S*A+a*S+s2]*0.1;

							}else if(state1[i]==1&&state2[i]==1&&(a-L)!=(i+1))
							{
								T1[s1*S*A+a*S+s2]=T1[s1*S*A+a*S+s2]*0.9;
							}	

						}

						
					}
					Ttxt<<T1[s1*S*A+a*S+s2]<<" ";
				}

			}
			Ttxt<<std::endl;
		}
		
		
		
		std::ofstream Otxt("O.txt");
		for(uint s=0;s<S;s++)
		{
			uint downsum=0;
			GetStatebin(s,L,state1);
			for(uint statei=0;statei<L;statei++)
			{
				downsum=downsum+state1[statei];
			}
			downsum=L-downsum;
			for(uint a=0;a<A;a++)
			{	
				
				   if(a==0||a>L)
				   {
					for(uint o=0;o<O;o++)
					{
						if(o==downsum)
						{						
							O1[s*A*O+a*O+o]=1;

						}else
						{
							O1[s*A*O+a*O+o]=0;
						}
					Otxt<<O1[s*A*O+a*O+o]<<" ";						
					}
				    }else
				    {
					for(uint o=0;o<O;o++)
					{
						if((o-(L+1))==downsum&&state1[a-1]==0)
						{
							O1[s*A*O+a*O+o]=1;
						}else if((o-2*(L+1))==downsum&&state1[a-1]==1)
						{
							O1[s*A*O+a*O+o]=1;
						}else
						{

							O1[s*A*O+a*O+o]=0;

						}
					Otxt<<O1[s*A*O+a*O+o]<<" ";						
					}




				    }
					
				
			}
			Otxt<<std::endl;
		}



		std::ofstream Rtxt("R.txt");
		R = new double[S*A];
		
		for(uint s=0;s<S;s++)
		{
			
			
			GetStatebin(s,L,state1);
			for(uint a=0;a<A;a++)
			{
				R[s*A+a]=0;
				if(a>0&&a<(L+1))
				{
					R[s*A+a]=R[s*A+a]-1;

				}else if(a>L)
				{
					R[s*A+a]=R[s*A+a]-20;
				}


				for(uint i=0;i<L;i++)
				{
					if(state1[i]==0)
					{
						R[s*A+a]=R[s*A+a]-10;
					}

				}


				Rtxt<<R[s*A+a]<<" ";
			}			
			Rtxt<<std::endl;
			

		}
}

SYS::~SYS(){
	if(R != 0)
		delete[] R;
	if(T != 0)
		delete[] T;
	if(T1!=0)
		delete[] T1;
	if(O1!=0)
		delete[] O1;


}

double SYS::getT(int i) const
{	
	return T1[i];
		
}


double SYS::getO(int i) const
{	
	return O1[i];
		
}


uint SYS::CreateStartState() const
{
		
		return S-1;
}

void SYS::Validate(const uint state) const{

}

bool SYS::StepPOMDP(uint state, uint action,
    uint& observation, uint& statenew, double& reward) const
{
	utils::rng.multinom(T1+state*S*A+action*S,S,statenew);
	utils::rng.multinom(O1+statenew*O*A+action*O,O,observation);
	reward=0.0;
	int downc=observation%(L+1);
	reward=reward-downc*10;
	
	if(action>0&&action<L+1)
	{
		reward=reward-1;
	}else if(action>L)
	{
		reward=reward-20;
	}

	
	

	return false;

}


double SYS::GetRewardbyO(uint observation) const
{
	double reward=0.0;
	int downc=observation%(L+1);
	reward=reward-downc*10;
	return reward;

}
double SYS::GetRewardbyAO(uint observation,uint action) const
{
	double reward=0.0;
	int downc=observation%(L+1);
	reward=reward-downc*10;
	if(action>0&&action<(L+1))
	{
		reward=reward-1.0;
	}else if(action>L)
	{
		reward=reward-20.0;
	}
	return reward;

}
 bool SYS::CheckTerminalbyO(uint observation) const
{
	bool terminal;
	

	terminal=false;

	return terminal;



}
void SYS::GetStatebin(int stateten,int len,int* statebin)
{
	int i=len-1;
	while(stateten)
	{
		statebin[i]=stateten%2;
		i--;
		stateten/=2;

	}
	if(i>=0);
	for(int j=0;j<=i;j++)
	{
		statebin[j]=0;
	}


}


void SYS::DisplayState(const uint state, std::ostream& ostr) const
{}

void SYS::DisplayObservation(const uint state, uint observation, std::ostream& ostr) const
{}

void SYS::DisplayAction(int action, std::ostream& ostr) const{
        ostr << action << std::endl;
}
