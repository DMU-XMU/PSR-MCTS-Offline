#include "FDMTransitionSampler.h"
#include "envs/basicMDP.h"
#include "envs/basicPOMDP.h"

//FDMTransitionSampler functions
//-----------------------------------------------------


FDMTransitionSampler::FDMTransitionSampler(double* _P, uint _S) : P(_P), S(_S){

}

FDMTransitionSampler::~FDMTransitionSampler()
{
	delete[] P;
}
uint FDMTransitionSampler::getNextStateSample(){
	uint sp;
	utils::rng.multinom(P,S,sp);
	return sp;
}

//FDMTransitionParamSampler functions
//-----------------------------------------------------


FDMTransitionParamSampler::FDMTransitionParamSampler(const uint* _counts,
		double _alpha, uint _S) :
	counts(_counts),alpha(_alpha), S(_S){

}

FDMTransitionParamSampler::~FDMTransitionParamSampler()
{
}

void FDMTransitionParamSampler::getNextTParamSample(double* P){
	utils::sampleDirichlet(P,counts,S,alpha);
}

//FDMMDPSampler functions
//-----------------------------------------------------

FDMMDPSampler::FDMMDPSampler(const uint* _counts, double _alpha,
				uint _S, uint _A, double* _R, bool _rsas, double _gamma):
counts(_counts),alpha(_alpha), S(_S), A(_A), R(_R), rsas(_rsas),gamma(_gamma){
	SA = S*A;
	T = new double[SA*S];
	simulator = 0;

}

FDMTigerPOMDPSampler::FDMTigerPOMDPSampler(const uint* _counts1, double _alpha,
				uint _S, uint _A, uint _O,double* _R, bool _rsas, double _gamma):
counts1(_counts1),alpha(_alpha), S(_S), A(_A),O(_O), R(_R), rsas(_rsas),gamma(_gamma){
	SA = S*A;
	T = new double[SA*S];
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

	OT = new double[SA*O];
	simulator = 0;

}

FDMMDPSampler::~FDMMDPSampler(){
	delete[] T;
	delete[] O;
	if(simulator)
		delete simulator;
}
FDMTigerPOMDPSampler::~FDMTigerPOMDPSampler(){
	delete[] T;
	delete[] OT;
	if(simulator)
		delete simulator;
}

SIMULATOR* FDMMDPSampler::updateMDPSample(){

	if(simulator)
		delete simulator;
	uint sSA;
	//Sample transitions from the counts
	for(uint s=0;s<S;++s){
		sSA = s*SA;
		for(uint a=0;a<A;++a){
			utils::sampleDirichlet(T+sSA+a*S,counts+(sSA+a*S),S,alpha);
		}
	}
	simulator = new BasicMDP(S,A,gamma,R,rsas,T);
	return (SIMULATOR*) simulator;
}
SIMULATOR* FDMTigerPOMDPSampler::updateTigerMDPSample(){

	if(simulator)
		delete simulator;
	uint sSA;
	//Sample transitions from the counts
	for(uint s=0;s<S;++s){
		sSA = s*SA;
		for(uint a=0;a<A;++a){
			//std::cout<<"SDbegin"<<std::endl;
			utils::sampleDirichlet(OT+sSA+a*S,counts1+(sSA+a*S),S,alpha);
			/*std::cout<<"************"<<std::endl;
			for(int i=0;i<12;i++){
				std::cout<<counts1[i]<<std::endl;
			}
			std::cout<<"************"<<std::endl;*/
			//std::cout<<"************"<<std::endl;
			//for(int i=0;i<12;i++){
				//std::cout<<T[i]<<std::endl;
			//}
			//std::cout<<"************"<<std::endl;
			//std::cout<<"SDend"<<std::endl;
		}
	}
	//std::cout<<"BasicPOMDPbegin"<<std::endl;
	simulator = new BasicPOMDP(S,A,O,gamma,R,rsas,T,OT);
	//std::cout<<"BasicPOMDPend"<<std::endl;
	return (SIMULATOR*) simulator;
}
