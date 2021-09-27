#pragma once




#include "history.h"
#include "envs/simulator.h"
#include "spectral.h"
#include "planners/mcp/statistic.h"
#include <vector>
#include "Matrix.h"
#include <fstream>
using namespace TNT;
using namespace JAMA;
class VNODE;
class QNODE;


class PSRMCTS
{
public:

    struct PARAMS
    {
        PARAMS();

        int Verbose;
        int MaxDepth;
        int NumSimulations;
        int ExpandCount;
        double ExplorationConstant;
        bool ReuseTree;
    };

    PSRMCTS(const SIMULATOR& simulator, const PARAMS& params,SPECTRAL& spectral);
    ~PSRMCTS();

    int SelectAction(uint current_state);
    bool Update(uint state, uint action, uint observation, double reward);

    void UCTSearch();
    void ConstructTree();
    void CopyTree(VNODE* vnode,VNODE*& vnodenew);
    void DisplayTree(VNODE* vnode);
    


    

    symSet *getAOQset(symSet* AOset,symSet* Testset);
   

    Array1D<double>* getPH(symList2IntMap *Histcout);
    Array2D<double>* getProb( symSet* interestTests,std::vector<float>*belief,int action_N,symList2IntMap *Histcout,int flag);
    

    symList2Vec2Map *divideData;
    Array1D<double>* PH;
    Array2D<double>* PTH;//P(T|H)
    Array2D<double>* PAOH;//P(AO|H)
    Array2D<double>* PAOQH2D;//P(AOQ|H)
    Array3D<double>* PAOQH3D;//P(AOQ|H)
    Array3D<double>* PAOQH3Djoint;//P(AOQ|H)    

    
    //Array1D<double>* bt;
    CMatrix* b1;
    CMatrix* binfiT;
    
    Array1D<double>* b1used;
    Array2D<double>* binfiTused;
    Array3D<double>* Bao;

    Array2D<double>*U;
    Array2D<double>*UT;


    


    void getSpectral();

    void UpdateBt(int ao,CMatrix*& bt);

    double SimulateVPSR(VNODE* vnode);
    double SimulateQPSR(QNODE& qnode, uint action);
    double RolloutPSR();
    void UpdatePSR(uint action,uint observation);
    void FreeusedRoot();


    uint getPSRresult(uint action,CMatrix* bt);


    CMatrix* btupdate;
    CMatrix* btused;

    void ConstructSameTree();
    int SelectActionPSR();


    void RolloutSearch();
    void SVD(CMatrix Pth);

    double Rollout(uint state);

    const HISTORY& GetHistory() const { return History; }
    const SIMULATOR::STATUS& GetStatus() const { return Status; }
    void ClearStatistics();
    void DisplayStatistics(std::ostream& ostr) const;
    void DisplayValue(int depth, std::ostream& ostr) const;
    void DisplayPolicy(int depth, std::ostream& ostr) const;

    static void InitFastUCB(double exploration);

    VNODE* Root;
    VNODE* usedRoot;



    std::vector<Symbol*>* continueData;
    Symbol *actionToSend, *obsReceived;

private:
		
		
		//Cached values
		uint S,A,SA,SAS;

    PARAMS Params;
    const SIMULATOR& Simulator;
    
    
    HISTORY History;
    SIMULATOR::STATUS Status;
    int TreeDepth, PeakTreeDepth;

		
    SPECTRAL& Spectral;

		

    STATISTIC StatTreeDepth;
    STATISTIC StatRolloutDepth;
    STATISTIC StatTotalReward;

    int GreedyUCB(VNODE* vnode, bool ucb) const;
    int SelectRandom() const;
    double SimulateV(uint state, VNODE* vnode);
    double SimulateQ(uint state, QNODE& qnode, uint action);

    VNODE* ExpandNode();

    // Fast lookup table for UCB
    //static const int UCB_N = 10000, UCB_n = 100;
		static const int UCB_N = 10000, UCB_n = 5000;
    static double UCB[UCB_N][UCB_n];
    static bool InitialisedFastUCB;

    double FastUCB(int N, int n, double logN) const;

		uint step;

		

};
