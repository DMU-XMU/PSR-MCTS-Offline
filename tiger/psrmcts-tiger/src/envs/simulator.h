#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "utils/utils2.h"
#include <iostream>
#include <math.h>

#include "planners/mcp/psrmcts/history.h"

typedef unsigned int uint;


class SIMULATOR
{
public:

    struct STATUS
    {
        STATUS();

        enum
        {
            TREE,
            ROLLOUT,
            NUM_PHASES
        };

        int Phase;
    };

    SIMULATOR();
    SIMULATOR(int numActions, int numObservations, double discount = 1.0);
    virtual ~SIMULATOR();

    // Create start start state (can be stochastic)
    virtual uint CreateStartState() const = 0;
  

   
    //For pomdp-STEP
    virtual bool StepPOMDP(uint state, uint action,
        uint& observation, uint& statenew, double& reward) const {return false;};
   
    virtual double GetRewardbyO(uint observation) const {std::cout<<"simGetRewardbyO"<<std::endl;return 0;};
    virtual bool CheckTerminalbyO(uint observation) const {std::cout<<"simCheckTerminalbyO"<<std::endl;return 0;};

    



    int SelectRandom() const;
    int SelectRandom(const STATUS& status) const;

    int SelectRandom(const uint& state,
        const STATUS& status) const;
   
    // Textual display
    virtual void DisplayState(const uint& state, std::ostream& ostr) const;
    virtual void DisplayAction(int action, std::ostream& ostr) const;
    virtual void DisplayObservation(const uint& state, uint observation, std::ostream& ostr) const;
    virtual void DisplayReward(double reward, std::ostream& ostr) const;

    // Accessors
    int GetNumActions() const { return NumActions; }
    int GetNumObservations() const { return NumObservations; }
    bool IsEpisodic() const { return false; }
    double GetDiscount() const { return Discount; }
    double GetRewardRange() const { return RewardRange; }
    double GetHorizon(double accuracy, int undiscountedHorizon = 100) const;

		double* T;
		double* R;
    double* OT;
    

		//Reward R(s,a,s') ? (otherwise reward function is R(s,a))
		bool rsas;
    uint S,A,O;
        double* T1;
	virtual double getT(int i) const{return 0;};
    double* O1;
	virtual double getO(int i) const{return 0;};
protected:

    int NumActions, NumObservations, NumStates;
    double Discount, RewardRange;
};

#endif // SIMULATOR_H
