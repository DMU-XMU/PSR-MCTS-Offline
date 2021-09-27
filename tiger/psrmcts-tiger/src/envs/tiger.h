#pragma once

#include "simulator.h"




class Tiger : public SIMULATOR
{
public:

    Tiger(double discount);
		~Tiger();

    
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
    

    uint S,A,O;
protected:

		

};
