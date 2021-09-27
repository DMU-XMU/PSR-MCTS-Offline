#pragma once

#include "simulator.h"


class SYS : public SIMULATOR
{
public:

    SYS(uint _S,double discount);
		~SYS();

    virtual void Validate(const uint state) const;
    virtual uint CreateStartState() const;
    virtual bool StepPOMDP(uint state, uint action,
        uint& observation, uint& statenew, double& reward) const;
    

    virtual void DisplayState(const uint state, std::ostream& ostr) const;
    virtual void DisplayObservation(const uint state, uint observation, std::ostream& ostr) const;
    virtual void DisplayAction(int action, std::ostream& ostr) const;
    
    double* T1;
    virtual double getT(int i) const;
    double* O1;
    virtual double getO(int i) const;
    
    virtual double GetRewardbyO(uint observation) const;
    virtual bool CheckTerminalbyO(uint observation) const;
    virtual double GetRewardbyAO(uint observation,uint action) const;
    void GetStatebin(int stateten,int len,int* statebin);
    int* state1;
    int* state2;

    uint S,A,O;
protected:
		
		uint L;


};
