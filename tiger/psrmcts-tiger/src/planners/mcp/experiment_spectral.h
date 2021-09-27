#pragma once


#include "psrmcts/psrmcts.h"
#include "psrmcts/spectral.h"
#include "envs/simulator.h"


#include "statistic.h"
#include <fstream>

//----------------------------------------------------------------------------

struct RESULTS
{
    void Clear();

    STATISTIC Time;
    STATISTIC Reward;
    STATISTIC DiscountedReturn;
    STATISTIC UndiscountedReturn;
};

inline void RESULTS::Clear()
{
    Time.Clear();
    Reward.Clear();
    DiscountedReturn.Clear();
    UndiscountedReturn.Clear();
}

//----------------------------------------------------------------------------

class EXPERIMENTSPECTRAL
{
public:

    struct PARAMS
    {
        PARAMS();

        int NumRuns;
        int NumSteps;
        int SimSteps;
        double TimeOut;
        double Accuracy;
        int UndiscountedHorizon;
        bool AutoExploration;
    };

    EXPERIMENTSPECTRAL(const SIMULATOR& real, const SIMULATOR& simulator,
        /*const std::string& outputFile,*/
        EXPERIMENTSPECTRAL::PARAMS& expParams, PSRMCTS::PARAMS& searchParams);
    ~EXPERIMENTSPECTRAL();
    void Run(std::vector<double>& Rhist);
		void RunBandit(std::vector<uint>& Rhist, std::vector<uint>& optArm, uint bestArm);

private:

    const SIMULATOR& Real;
    const SIMULATOR& Simulator;
    EXPERIMENTSPECTRAL::PARAMS& ExpParams;
    PSRMCTS::PARAMS& SearchParams;
    RESULTS Results;
    //std::ofstream OutputFile;
};

//----------------------------------------------------------------------------
