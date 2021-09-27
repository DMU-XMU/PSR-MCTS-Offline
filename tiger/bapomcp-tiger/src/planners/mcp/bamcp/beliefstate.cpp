#include "beliefstate.h"
#include "envs/simulator.h"
#include "utils/utils2.h"

using namespace UTILS;

BELIEF_STATE::BELIEF_STATE()
{
    Samples.clear();
}

void BELIEF_STATE::Free(const SIMULATOR& simulator)
{
    for (std::vector<STATE*>::iterator i_state = Samples.begin();
            i_state != Samples.end(); ++i_state)
    {
        simulator.FreeState(*i_state);
    }
    Samples.clear();
}

STATE* BELIEF_STATE::CreateSample(const SIMULATOR& simulator) const
{
    int index = Random(Samples.size());
    return simulator.Copy(*Samples[index]);
}

STATE* BELIEF_STATE::CreateSamplebyindex(const SIMULATOR& simulator,int i) const
{
    return simulator.Copy(*Samples[i]);
}

STATE* BELIEF_STATE::CreateSampleWithProb(const SIMULATOR& simulator) const
{
    double Prob=RandomDouble(0.0,1.0);
    //std::cout<<"Prob="<<Prob<<std::endl;
    double stateprob=0;
    for(int i=0;i<1000;i++){
	stateprob+=simulator.GetStateProb(Samples[i]);
	//std::cout<<"prob"<<stateprob<<std::cout;
	if (stateprob>=Prob) {
        	return simulator.Copy(*Samples[i]);
        }
    //int index = Random(Samples.size());
    //return simulator.Copy(*Samples[index]);
    }

    return simulator.Copy(*Samples[999]);

}

void BELIEF_STATE::AddSample(STATE* state)
{
    Samples.push_back(state);
}

void BELIEF_STATE::Copy(const BELIEF_STATE& beliefs, const SIMULATOR& simulator)
{
    for (std::vector<STATE*>::const_iterator i_state = beliefs.Samples.begin();
        i_state != beliefs.Samples.end(); ++i_state)
    {
        AddSample(simulator.Copy(**i_state));
    }
}

void BELIEF_STATE::Move(BELIEF_STATE& beliefs)
{
    for (std::vector<STATE*>::const_iterator i_state = beliefs.Samples.begin();
        i_state != beliefs.Samples.end(); ++i_state)
    {
        AddSample(*i_state);
    }
    beliefs.Samples.clear();
}
