#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "utils/utils2.h"
#include <iostream>
#include <math.h>
#include "planners/mcp/bamcp/beliefstate.h"
#include "planners/mcp/bamcp/history.h"

typedef unsigned int uint;

class STATE : public MEMORY_OBJECT
{
	/*public:

    uint position;
    uint* countsT;
    uint* countsO;*/
};

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
    virtual STATE* CreatesimStartState(const uint* countsrefine) const {return 0;};
    virtual STATE* Copy(const STATE& state) const{return 0;};
    virtual void FreeState(STATE* state) const {};
    virtual double GetStateProb(STATE* state) const{return 0;};

    // Update state according to action, and get observation and reward.
    // Return value of true indicates termination of episode (if episodic)
    virtual bool Step(uint state, uint action,
        uint& observation, double& reward) const = 0;
    //For pomdp-STEP
    virtual bool StepPOMDP(uint state, uint action,
        uint& observation, uint& statenew, double& reward) const {return false;};
    virtual void Updatemdp(const uint* counts) const{};
    virtual bool LocalMove(STATE& state,const HISTORY& history, int stpObs)const{return false;};
    virtual bool StepPOMDPsim(STATE& state, uint action, uint& observation, double& immediateReward)const{return false;};
    virtual void Statenormalized(STATE& state,double probsum)const{};
    virtual bool StateUpdateWithMC(STATE& state, uint action, uint observation)const{return false;};
    virtual void ResetPosition(STATE& state)const{};

    virtual int getcountsOini(uint index)const{ std::cout<<"sim-getcountsOini"<<std::endl;return 0;};


    


    // Sanity check
    virtual void Validate(const uint& state) const;

    int SelectRandom(const uint& state,
        const STATUS& status) const;
    int SelectRandom(const STATE& state,
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
    //double* Osim;

		//Reward R(s,a,s') ? (otherwise reward function is R(s,a))
		bool rsas;
    std::vector<int> countsOini;

protected:

    int NumActions, NumObservations, NumStates;
    double Discount, RewardRange;
};

#endif // SIMULATOR_H
