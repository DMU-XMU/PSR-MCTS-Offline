#include "sysadmin.h"
#include "../utils/utils.h"
//#include "../utils/utils2.h"
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


		Discount = discount;
		rsas = false;

    		RewardRange = (0-((-10)*L-20))*20;

		Tc = new double[2];
		Tc[1] = 1;
		Tc[0] = 0;
		state1=new int[L];
		state2=new int[L];

		//Standard description
		T1=new double[S*A*S];
		Tnoisy=new double[S*A*S];
		O1=new double[S*A*O];
		//Osim=new double[S*A*O];
		//T
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
		//O
		/*std::ofstream Otxt("O.txt");
		for(uint s=0;s<S;s++)
		{
			GetStatebin(s,L,state1);
			for(uint a=0;a<A;a++)
			{
				for(uint o=0;o<O;o++)
				{
					if((a==0||a>L)&&o==0)
					{
						O1[s*A*O+a*O+o]=1;
					}else if((a==0||a>L)&&o!=0)
					{
						O1[s*A*O+a*O+o]=0;
					}

					else if(a>0&&a<(L+1))
					{
						if(state1[a-1]==0&&o==1)
						{O1[s*A*O+a*O+o]=1;}
						else if(state1[a-1]==1&&o==2)
						{O1[s*A*O+a*O+o]=1;}
						else
						{O1[s*A*O+a*O+o]=0;}

					}
					Otxt<<O1[s*A*O+a*O+o]<<" ";
				}
			}
			Otxt<<std::endl;
		}*/
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
		//R
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
		//Tnoisy
		std::ofstream TNtxt("TN.txt");
		double trand;
		for(uint s1=0;s1<S;s1++)
		{
			for(uint a=0;a<A;a++)
			{
				for(uint s2=0;s2<S;s2++)
				{
					trand=utils::rng.rand_closed01() <= 0.5?0:1;
					if(trand==0)
					{
						Tnoisy[s1*S*A+a*S+s2]=T1[s1*S*A+a*S+s2]+0.15;
						//Tnoisy[s1*S*A+a*S+s2]=T1[s1*S*A+a*S+s2]+0.3;
					}else if(trand==1)
					{
						Tnoisy[s1*S*A+a*S+s2]=T1[s1*S*A+a*S+s2]-0.15;
						//Tnoisy[s1*S*A+a*S+s2]=T1[s1*S*A+a*S+s2]-0.3;
					}
					if(Tnoisy[s1*S*A+a*S+s2]<0)
					{
						Tnoisy[s1*S*A+a*S+s2]=0.001;
			
					}
				TNtxt<<Tnoisy[s1*S*A+a*S+s2]<<" ";	
				}
				
			}
			TNtxt<<std::endl;
		}
		std::ofstream TNnortxt("TNnor.txt");
		for(uint s1=0;s1<S;s1++)
		{
			for(uint a=0;a<A;a++)
			{
				double Tnor=0;
				for(uint s2=0;s2<S;s2++)
				{
						Tnor=Tnor+Tnoisy[s1*S*A+a*S+s2];
				}
				for(uint s2=0;s2<S;s2++)
				{
					Tnoisy[s1*S*A+a*S+s2]=Tnoisy[s1*S*A+a*S+s2]/Tnor;
				
				TNnortxt<<Tnoisy[s1*S*A+a*S+s2]<<" ";	
				}
				
			}
			TNnortxt<<std::endl;
		}
		std::ofstream countsOiniTXT("countsOini.txt");
		countsOini.reserve(S*A*S);
		for (uint c = 0; c < S*A*S; ++c) {
			countsOini.push_back(0);
   		 }
		for(uint s1=0;s1<S;s1++)
		{
			for(uint a=0;a<A;a++)
			{
				for(uint s2=0;s2<S;s2++)
				{
					//countsOini[s1*S*A+a*S+s2]=Tnoisy[s1*S*A+a*S+s2]*20;
					//countsOini[s1*S*A+a*S+s2]=3;//
					countsOini[s1*S*A+a*S+s2]=int(utils::rng.rand_closed01()*10);
					countsOiniTXT<<countsOini[s1*S*A+a*S+s2]<<" ";

				}
			}
			countsOiniTXT<<std::endl;

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

	delete[] Tc;
}



uint SYS::CreateStartState() const
{
		return utils::rng.rand_closed01() <= 0.5?0:1;
		//return 0; //(rand() % S);
}

STATE* SYS::CreatesimStartState(const uint* countsOrefine) const
{
    SYS_STATE* sysstate = MemoryPool.Allocate();
    sysstate->prob=1.0/1000.0;
    //sysstate->prob=1.0/200.0;
    sysstate->countsO.clear();
    sysstate->countsO.reserve(S*A*S);
    //sysstate->position = utils::rng.rand_closed01() <= 0.5?0:1;
    sysstate->position =7;
    for (uint c = 0; c < S*A*S; ++c) {
	sysstate->countsO.push_back(0);
    }
    for (uint c = 0; c < S*A*S; ++c) {
	sysstate->countsO.at(c)=countsOrefine[c];
    }
    

    return sysstate;
}



STATE* SYS::Copy(const STATE& state) const
{
    const SYS_STATE& sysstate = safe_cast<const SYS_STATE&>(state);
    SYS_STATE* newstate = MemoryPool.Allocate();
    *newstate = sysstate;
    return newstate;
}

void SYS::FreeState(STATE* state) const
{
    SYS_STATE* sysstate = safe_cast<SYS_STATE*>(state);
    //delete[] state->countsT;
    //delete[] state->countsO;
    sysstate->countsO.clear();
    MemoryPool.Free(sysstate);
}

double SYS::GetStateProb(STATE* state) const
{
	double prob=0;
	SYS_STATE* sysstate = safe_cast<SYS_STATE*>(state);
	prob=sysstate->prob;
	//std::cout<<"prob="<<prob<<std::endl;
	return prob;
}

void SYS::Validate(const uint state) const{

}
bool SYS::Step(uint state, uint action,
        uint& observation, double& reward) const
{
	return false;
}

bool SYS::StepPOMDP(uint state, uint action,
    uint& observation, uint& statenew, double& reward) const
{
	utils::rng.multinom(T1+state*S*A+action*S,S,statenew);
	utils::rng.multinom(O1+statenew*O*A+action*O,O,observation);
	reward = R[statenew*A+action];
	
	return false;

}

bool SYS::StepPOMDPsim(STATE& state, uint action, uint& observation, double& immediateReward)const
{
	SYS_STATE& sysstate = safe_cast<SYS_STATE&>(state);
	//std::cout<<"begin"<<std::endl;
	immediateReward = 0;
	uint sSA;
	uint statenew;
	double* countsim;
	countsim=new double[S*A*S];
	for(uint i=0;i<S*A*S;i++)
	{
		//std::cout<<"countsim"<<std::endl;

		if(i<sysstate.countsO.size())
		{		
			countsim[i]=sysstate.countsO.at(uint(i));
		}
		else
		{
		 std::cout<<"simi>size()"<<std::endl;		
		}

	}
	double* Osim;
	/*Osim=new double[S*A*S];
	for(uint s=0;s<S;++s){
		sSA = s*S*A;
		for(uint a=0;a<A;++a){
			utils::sampleDirichlet(Osim+sSA+a*S,countsim+(sSA+a*S),S,1/(double)S);
			}
		}

	utils::rng.multinom(Osim+sysstate.position*S*A+action*S,S,statenew);*/
	Osim=new double[S];
	sSA = (sysstate.position)*S*A;
	utils::sampleDirichlet(Osim,countsim+(sSA+action*S),S,1/(double)S);
		

	utils::rng.multinom(Osim,S,statenew);
	
	utils::rng.multinom(O1+statenew*O*A+action*O,O,observation);
	immediateReward = R[statenew*A+action];
	
	  uint index=(sysstate.position)*S*A+S*action+statenew;

	 
	  if(index<sysstate.countsO.size())
	  {
		sysstate.countsO.at(uint(index))=countsim[index]+1;
	  }
	  else
	 {
		std::cout<<"simindex>size()"<<std::endl;}

	sysstate.position=statenew;
	 //std::cout<<"after"<<sysstate.countsO[statenew*S*A+S*action+observation]<<std::endl;
	 
	//std::cout<<sysstate.countsO[statenew*S*A+S*action+observation]<<std::endl;
	delete[] countsim;
	delete[] Osim;


	return false;
}

bool SYS::StateUpdateWithMC(STATE& state, uint action, uint observation)const
{
	SYS_STATE& sysstate = safe_cast<SYS_STATE&>(state);
	//std::cout<<"begin"<<std::endl;
	//immediateReward = 0;
	uint sSA;
	uint statenew;
	uint observationreal;
	double* countsim;
	countsim=new double[S*A*S];
	for(uint i=0;i<S*A*S;i++)
	{

		if(i<sysstate.countsO.size())
		{		
			countsim[i]=sysstate.countsO.at(uint(i));
		}
		else
		{
		 std::cout<<"MCi>size()"<<std::endl;		
		}

	}
	double* Osim;
	/*Osim=new double[S*A*S];
	for(uint s=0;s<S;++s){
		sSA = s*S*A;
		for(uint a=0;a<A;++a){
			utils::sampleDirichlet(Osim+sSA+a*S,countsim+(sSA+a*S),S,1/(double)S);
			}
		}

	//std::cout<<"sample"<<std::endl;
	utils::rng.multinom(Osim+sysstate.position*S*A+action*S,S,statenew);*/
	Osim=new double[S];
	sSA = (sysstate.position)*S*A;
	utils::sampleDirichlet(Osim,countsim+(sSA+action*S),S,1/(double)S);
	utils::rng.multinom(Osim,S,statenew);
	utils::rng.multinom(O1+statenew*O*A+action*O,O,observationreal);
	
	
	//sysstate.prob=sysstate.prob*Osim[sysstate.position*S*A+S*action+statenew]*O1[statenew*O*A+O*action+observation]+1.0/1000.0;
	sysstate.prob=sysstate.prob*Osim[statenew]*O1[statenew*O*A+O*action+observation]+1.0/1000.0;
	
	
	//immediateReward = R[sysstate.position*A+action];
	
	
		uint index=(sysstate.position)*S*A+S*action+statenew;

		if(index<sysstate.countsO.size())
		{sysstate.countsO.at(uint(index))=countsim[index]+1;}
		else
		{std::cout<<"MCindex>size()"<<std::endl;}
	sysstate.position=statenew;
	
	delete[] countsim;
	delete[] Osim;
	//std::cout<<"end"<<std::endl;
	if(observationreal!=observation)
		return false;
	return true;
}
void SYS::Statenormalized(STATE& state,double probsum)const
{
	SYS_STATE& sysstate = safe_cast<SYS_STATE&>(state);
	sysstate.prob=sysstate.prob/probsum;
}

void SYS::ResetPosition(STATE& state)const
{
	SYS_STATE& sysstate = safe_cast<SYS_STATE&>(state);
	//for(int i=0;i<12;i++)
	   //std::cout<<sysstate.countsO[i]<<std::endl;
	//std::cout<<"***********************************"<<std::endl;
	//sysstate.position=utils::rng.rand_closed01() <= 0.5?0:1;
	sysstate.position=7;
}

bool SYS::LocalMove(STATE& state,const HISTORY& history, uint stpObs)const
{
	/*uint statenew;
	SYS_STATE& sysstate = safe_cast<SYS_STATE&>(state);
	 uint sSA;
		for(uint s=0;s<S;++s){
		sSA = s*S*A;
		for(uint a=0;a<A;++a){
			utils::sampleDirichlet(Osim+sSA+a*S,sysstate.countsO+(sSA+a*S),S,1/(double)S);
			}
		}
	utils::rng.multinom(T+sysstate.position*S*A+history.Back().Action*S,S,statenew);
	utils::rng.multinom(OT+statenew*S*A+history.Back().Action*S,S,stpObs);
	std::cout<<stpObs<<std::endl;
	if(stpObs!=(uint)history.Back().Observation)
	{return false;}

        sysstate.position=statenew;
	sysstate.countsO[statenew*S*A+S*history.Back().Action+stpObs] += 1;
	return true;*/
	return true;

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
int SYS::getcountsOini(uint index)const
{
	return countsOini[index];


}

void SYS::DisplayState(const uint state, std::ostream& ostr) const
{}

void SYS::DisplayObservation(const uint state, uint observation, std::ostream& ostr) const
{}

void SYS::DisplayAction(int action, std::ostream& ostr) const{
        ostr << action << std::endl;
}
