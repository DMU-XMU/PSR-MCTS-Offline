#pragma once




#include "history.h"
#include "envs/simulator.h"
#include "planners/mcp/statistic.h"
#include "samplers/sampler.h"

#include <fstream>

class VNODE;
class QNODE;
class SamplerFactory;

class BAPOMCP
{
public:

    struct PARAMS
    {
        PARAMS();

        int Verbose;
        int MaxDepth;
        int NumSimulations;
        int NumStartStates;
        int ExpandCount;
        double ExplorationConstant;
        bool ReuseTree;
				bool BANDIT;
				int RB;
				double eps;
    };

    BAPOMCP(const SIMULATOR& simulator, const PARAMS& params,SamplerFactory& sampFact);
    ~BAPOMCP();

    //int SelectAction(uint current_state);
    //int SelectAction(double* Belief);
    int SelectAction();
    bool Update(uint state, uint action, uint observation, double reward);
    bool Update(uint& statenew, uint state, uint action, uint observation, double reward);
    void Reset(const uint* countsrefine);

    //void UCTSearch(uint current_state);
    //void UCTSearch(double* Belief);
    void UCTSearch();
    void RolloutSearch();


    void AddSample(VNODE* node, const STATE& state);
    bool AddTransforms(VNODE* root, BELIEF_STATE& beliefs);
    STATE* CreateTransform() const;
    void GetNewBelief(BELIEF_STATE& beliefs);

    //double Rollout(const SIMULATOR* mdp, STATE& state);
    double Rollout(/*SIMULATOR* mdp,*/ STATE& state/*,Sampler* MDPSampler*/);

    const HISTORY& GetHistory() const { return History; }
    const SIMULATOR::STATUS& GetStatus() const { return Status; }
    void ClearStatistics();
    void DisplayStatistics(std::ostream& ostr) const;
    void DisplayValue(int depth, std::ostream& ostr) const;
    void DisplayPolicy(int depth, std::ostream& ostr) const;

    static void InitFastUCB(double exploration);
    uint* counts1;
private:
		uint* counts;

		double* countsSum;
		
		//Cached values
		uint O,S,A,SA,SAS,SAO;

    PARAMS Params;
    const SIMULATOR& Simulator;
    VNODE* Root;
    HISTORY History;
    SIMULATOR::STATUS Status;
    int TreeDepth, PeakTreeDepth;

		SamplerFactory& SampFact;

		uint* RLPI;
		double* V;

		double* Q;
    double QlearningRate;
		std::vector<uint>** GreedyA;

    STATISTIC StatTreeDepth;
    STATISTIC StatRolloutDepth;
    STATISTIC StatTotalReward;

    int GreedyUCB(VNODE* vnode, bool ucb) const;
    int SelectRandom() const;
    //double SimulateV(const SIMULATOR* senv, STATE& state, VNODE* vnode);
    //double SimulateQ(const SIMULATOR* senv, STATE& state, QNODE& qnode, uint action);
    double SimulateV(/*SIMULATOR* senv,*/ STATE& state, VNODE* vnode/*,Sampler* MDPSampler*/);
    double SimulateQ(/*SIMULATOR* senv,*/ STATE& state, QNODE& qnode, uint action/*,Sampler* MDPSampler*/);

    VNODE* ExpandNode();

    // Fast lookup table for UCB
    //static const int UCB_N = 10000, UCB_n = 100;
		static const int UCB_N = 10000, UCB_n = 5000;
    static double UCB[UCB_N][UCB_n];
    static bool InitialisedFastUCB;

    double FastUCB(int N, int n, double logN) const;

		uint step;

		std::ofstream meand;
		std::ofstream maxd;

};
