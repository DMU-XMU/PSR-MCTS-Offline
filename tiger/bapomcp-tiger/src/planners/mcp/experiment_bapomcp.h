#pragma once

//#include "bamcp/bamcp.h"
#include "bamcp/bapomcp.h"
#include "envs/simulator.h"
#include "samplers/samplerFactory.h"

#include "statistic.h"
#include <fstream>
#include "utils/utils.h"
#include "experiment.h"
//----------------------------------------------------------------------------
/*
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
}*/

//----------------------------------------------------------------------------

class EXPERIMENTBAPOMCP
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

    EXPERIMENTBAPOMCP(const SIMULATOR& real, const SIMULATOR& simulator,
        const std::string& outputFile,
        EXPERIMENTBAPOMCP::PARAMS& expParams, BAPOMCP::PARAMS& searchParams,
				 SamplerFactory& samplerFactory);
    ~EXPERIMENTBAPOMCP();
    void Run(std::vector<double>& Rhist);
		void RunBandit(std::vector<uint>& Rhist, std::vector<uint>& optArm, uint bestArm);

private:

    const SIMULATOR& Real;
    const SIMULATOR& Simulator;
    EXPERIMENTBAPOMCP::PARAMS& ExpParams;
    BAPOMCP::PARAMS& SearchParams;
    RESULTS Results;
		SamplerFactory& samplerFact;
    std::ofstream OutputFile;
};

//----------------------------------------------------------------------------
