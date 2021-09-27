#pragma once

#include "simulator.h"
#include <string>

class BasicPOMDP : public SIMULATOR
{
public:

    BasicPOMDP(uint _S, uint _A, double discount, double* _reward,
		bool _rsas,
		const uint* counts);
		//BasicPOMDP(uint _S, uint _A, double discount, double* _R,
		//bool _rsas, double* _T);
    		BasicPOMDP(uint _S, uint _A, uint _O, double discount, double* _R,
		bool _rsas, double* _T, double* _OT);
		~BasicPOMDP();

		BasicPOMDP(uint _S, uint _A, std::string filename, double discount);

		void saveMDP(std::string filename);

    virtual void Validate(const uint state) const;
    virtual uint CreateStartState() const;
    virtual bool Step(uint state, uint action,
        uint& observation, double& reward) const;
    bool StepPOMDP(uint state, uint action,
        uint& observation, uint& statenew, double& reward) const;
    void Updatemdp(const uint* counts) const;

    void GeneratePreferred(const uint state,
        std::vector<int>& legal, const STATUS& status) const;

    virtual void DisplayState(const uint state, std::ostream& ostr) const;
    virtual void DisplayObservation(const uint state, uint observation, std::ostream& ostr) const;
    virtual void DisplayAction(int action, std::ostream& ostr) const;

protected:
		uint S,A,O;


		//Cached value
		uint SA;

private:
		bool Tresp;

};
