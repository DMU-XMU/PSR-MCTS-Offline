#include "basicPOMDP.h"
#include "../utils/utils.h"

#include <iostream>
#include <fstream>

using namespace UTILS;

BasicPOMDP::BasicPOMDP(uint _S, uint _A, double discount, double* _reward,
		bool _rsas,
		const uint* counts)
:   S(_S), A(_A)
{
		NumObservations = _S;
		NumActions = _A;
		Discount = discount;
		R = _reward;
		rsas = _rsas;

    RewardRange = 1;

		SA = S*A;
		Tresp = true;
		T = new double[SA*S];
		//Sample transitions from the counts

		/*for(uint s=0;s<S;++s){
			for(uint a=0;a<A;++a){
				utils::sampleDirichlet(T+s*SA+a*S,counts+(s*SA+a*S),S,1/(double)S);
			}
		}*/
		T[0]=1.0;
		T[1]=0.0;
		T[2]=0.5;
		T[3]=0.5;
		T[4]=0.5;
		T[5]=0.5;
		T[6]=0.0;
		T[7]=1.0;
		T[8]=0.5;
		T[9]=0.5;
		T[10]=0.5;
		T[11]=0.5;

OT = new double[SA*2];

		uint sSA;
		for(uint s=0;s<S;++s){
		sSA = s*SA;
		for(uint a=0;a<A;++a){
			utils::sampleDirichlet(OT+sSA+a*S,counts+(sSA+a*S),S,1/(double)S);
		}
	}
}

BasicPOMDP::BasicPOMDP(uint _S, uint _A, uint _O,double discount, double* _R,
		bool _rsas, double* _T, double* _OT): S(_S), A(_A), O(_O){

	NumObservations = _S;
	NumActions = _A;
	NumStates=_O;
	Discount = discount;
	R = _R;
	rsas = _rsas;
	Tresp = false;
	T = _T;
	OT = _OT;

	SA = S*A;
	RewardRange = 1;
}

//Load from file
BasicPOMDP::BasicPOMDP(uint _S, uint _A, std::string filename, double discount){
	Discount = discount;
	Tresp = true;
	double rmin = 99999;
	double rmax = -99999;
	std::ifstream ifs;
	ifs.open(filename.c_str());
	assert(ifs.good());
	ifs >>	S;
	assert(S == _S);
	NumObservations = S;
	ifs >> A;
	assert(A == _A);
	NumActions = A;
	SA = S*A;
	T = new double[SA*S];
	for(uint s=0;s<S;++s){
		for(uint a=0;a<A;++a){
			for(uint sp = 0; sp < S; ++sp){
				ifs >> T[s*SA+a*S+sp];
			}
		}
	}
	ifs >> rsas;
	if(rsas){
		R = new double[SA*S];
		for(uint s=0;s<S;++s){
			for(uint a=0;a<A;++a){
				for(uint sp = 0; sp < S; ++sp){
					ifs >> R[s*SA+a*S+sp];
					if(R[s*SA+a*S+sp] > rmax)
						rmax =  R[s*SA+a*S+sp];
					if(R[s*SA+a*S+sp] < rmin)
						rmin = R[s*SA+a*S+sp];
				}
			}
		}
	}
	else{
		R = new double[SA];
		for(uint s=0;s<S;++s){
			for(uint a=0;a<A;++a){
				ifs >> R[s*A+a];
				if(R[s*A+a] > rmax)
					rmax = R[s*A+a];
				if(R[s*A+a] < rmin)
					rmin = R[s*A+a];
			}
		}
	}
	if(ifs.good())
		ifs.close();

	RewardRange = rmax-rmin;
}


BasicPOMDP::~BasicPOMDP(){
	if(Tresp)
		{delete[] T;
		delete[] OT;
		
		}
}

void BasicPOMDP::saveMDP(std::string filename){
	std::ofstream ofs;
	ofs.open(filename.c_str());
	ofs << S << " " << A << std::endl;
	for(uint s=0;s<S;++s){
		for(uint a=0;a<A;++a){
			for(uint sp = 0; sp < S; ++sp){
				ofs << T[s*SA+a*S+sp] << " ";
			}
			ofs << std::endl;
		}
	}
	ofs << rsas << std::endl;
	if(rsas){
		for(uint s=0;s<S;++s){
			for(uint a=0;a<A;++a){
				for(uint sp = 0; sp < S; ++sp){
					ofs << R[s*SA+a*S+sp] << " ";
				}
				ofs << std::endl;
			}
		}
	}
	else{
		for(uint s=0;s<S;++s){
			for(uint a=0;a<A;++a){
				ofs << R[s*A+a] << " ";
			}
			ofs << std::endl;
		}
	}
	ofs.close();
}

uint BasicPOMDP::CreateStartState() const
{
		return 0; //(rand() % S);
}


void BasicPOMDP::Validate(const uint state) const{

}

bool BasicPOMDP::Step(uint state, uint action,
    uint& observation, double& reward) const
{
	utils::rng.multinom(T+state*SA+action*S,S,observation);
	if(rsas)
		reward = R[state*SA+action*S+observation];
	else
		reward = R[state*A+action];

	//Never terminates
	//if (action==0) {
	//	return true;
	//}
	return false;
}


bool BasicPOMDP::StepPOMDP(uint state, uint action,
    uint& observation, uint& statenew, double& reward) const
{
	//std::cout<<"StepPOMDP"<<std::endl;
	utils::rng.multinom(T+state*SA+action*S,S,statenew);
	utils::rng.multinom(OT+statenew*SA+action*S,S,observation);
	if(rsas)
		reward = R[state*SA+action*S+observation];
	else
		reward = R[state*A+action];

	//Never terminates
	if(action==1||action==2)
	{return true;}

	return false;
}
void BasicPOMDP::Updatemdp(const uint* counts) const
{   uint sSA;
    for(uint s=0;s<S;++s){
		sSA = s*SA;
		for(uint a=0;a<A;++a){
			
			utils::sampleDirichlet(OT+sSA+a*S,counts+(sSA+a*S),S,1/(double)S);

		}
	}
}


void BasicPOMDP::GeneratePreferred(const uint state,
    std::vector<int>& actions, const STATUS& status) const
{
    for (uint a = 0; a < A; ++a)
			actions.push_back(a);
}


void BasicPOMDP::DisplayState(const uint state, std::ostream& ostr) const
{

}

void BasicPOMDP::DisplayObservation(const uint state, uint observation, std::ostream& ostr) const
{

}

void BasicPOMDP::DisplayAction(int action, std::ostream& ostr) const
{
        ostr << action << std::endl;
}
