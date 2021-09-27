#pragma once



class SIMULATOR;
class SamplerFactory;

typedef unsigned int uint;

class SBOSS
{
public:

    struct PARAMS
    {
        PARAMS();

        int Verbose;
				uint K;
				double delta;
				double epsilon;
    };

    SBOSS(const SIMULATOR& simulator, const PARAMS& params,
				SamplerFactory& sampFact);
    ~SBOSS();

    uint SelectAction(uint state);
    bool Update(uint state, uint action, uint observation, double reward);
		
		double posteriorDeviation(const uint* counts1,
													uint sum1,
		                      const uint* counts2,
													uint sum2);
		void createMergedModel();
   
	private:
		uint* counts;
    const SIMULATOR& Simulator;
	
		//SBOSS-specific
		bool do_sample;
		uint* countsSum;
		uint* countsSumLastResample;
		uint* countsLastResample;	
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

