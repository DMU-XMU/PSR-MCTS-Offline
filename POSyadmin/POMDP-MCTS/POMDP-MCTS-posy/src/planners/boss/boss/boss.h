#pragma once



class SIMULATOR;
class SamplerFactory;

typedef unsigned int uint;

class BOSS
{
public:

    struct PARAMS
    {
        PARAMS();

        int Verbose;
				uint K;
				uint B;
				double epsilon;
    };

    BOSS(const SIMULATOR& simulator, const PARAMS& params,
				SamplerFactory& sampFact);
    ~BOSS();

    uint SelectAction(uint state);
    bool Update(uint state, uint action, uint observation, double reward);

		void createMergedModel();
   
	private:
		uint* counts;
    const SIMULATOR& Simulator;
	
		//BOSS-specific
		bool do_sample;
		uint* qcounts;
		PARAMS Params;
		
		//Merged model
		uint Am;	
		double* Pm;
		double* Rm;
		uint* RLPI;
		double* V;
		

		//Cached values
		uint S,A,SA,SAS,SAm;

		SamplerFactory& SampFact;
};

