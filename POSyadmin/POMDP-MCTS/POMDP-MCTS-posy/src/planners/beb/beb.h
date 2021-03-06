#pragma once



class SIMULATOR;
class SamplerFactory;

typedef unsigned int uint;

class BEB
{
public:

    struct PARAMS
    {
        PARAMS();

        int Verbose;
				uint b;
				double epsilon;
    };

    BEB(const SIMULATOR& simulator, const PARAMS& params,
				SamplerFactory& sampFact);
    ~BEB();

    uint SelectAction(uint state);
    bool Update(uint state, uint action, uint observation, double reward);

   
	private:
		double* counts;
		double* countsSum;
    const SIMULATOR& Simulator;
	
		PARAMS Params;
	
		//For mean model
		double* P;
		//Modified reward function
		double* R;
	
		double* V;
		uint* RLPI;

		//Cached values
		uint S,A,SA,SAS;

		
		SamplerFactory& SampFact;
};

