#pragma once

#include "simulator.h"


class TIGER_STATE : public STATE
{
    public:

    uint position;
    double prob;
    //uint* countsO;

    std::vector<double> countsO;
};

class Tiger : public SIMULATOR
{
public:

    Tiger(uint _S,double discount);
		~Tiger();

    virtual void Validate(const uint state) const;
    virtual uint CreateStartState() const;
    //virtual STATE* CreateStartState() const;
    virtual STATE* CreatesimStartState(const uint* countsOrefine) const;
    virtual STATE* Copy(const STATE& state) const;
    void FreeState(STATE* state) const;
    virtual bool Step(uint state, uint action,
        uint& observation, double& reward) const;
    virtual bool StepPOMDP(uint state, uint action,
        uint& observation, uint& statenew, double& reward) const;
    virtual bool LocalMove(STATE& state,const HISTORY& history, uint stpObs)const;

    virtual void DisplayState(const uint state, std::ostream& ostr) const;
    virtual void DisplayObservation(const uint state, uint observation, std::ostream& ostr) const;
    virtual void DisplayAction(int action, std::ostream& ostr) const;
    virtual bool StepPOMDPsim(STATE& state, uint action, uint& observation, double& immediateReward)const;
    virtual void Statenormalized(STATE& state,double probsum)const;
    virtual bool StateUpdateWithMC(STATE& state, uint action, uint observation)const;
    virtual void ResetPosition(STATE& state)const;
    virtual double GetStateProb(STATE* state) const;
    double* T1;
    double* O1;
    //double* Osim;


protected:
		uint S,A,O;
		uint L;
		double *Tc;
private:
    mutable MEMORY_POOL<TIGER_STATE> MemoryPool;

};
