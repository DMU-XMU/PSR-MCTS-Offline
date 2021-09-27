#pragma once


#include "envs/simulator.h"

#include "symbol.h"
#include <map>
#include <set>
#include <vector>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "tnt/tnt_array1d.h"
#include "tnt/tnt_array2d.h"
#include "tnt/tnt_array3d.h"
#include "jama/jama_lu.h"
#include "jama/jama_svd.h"


typedef set<symList *, symbolComparator> symSet; 
typedef symSet::iterator symIter; 
typedef std::vector<Symbol*>::iterator vecsymIter;
typedef std::vector<symList*> vec2;
typedef map<symList *,vec2*, symbolComparator> symList2Vec2Map;

typedef map<symList*,std::vector<float>*,symbolComparator> symList2VfMap;


typedef map<symList*, double, symbolComparator> symList_DblMap;  
typedef map<symList*, symList_DblMap*,symbolComparator> symList2Prob;
typedef map<symList*,std::vector<symList2Prob*>*,symbolComparator>symList2Group;
typedef map<symList*,int*,symbolComparator> symList2IntMap;
typedef map<symList*, double*, symbolComparator> symList2DblMap; 

class SPECTRAL
{
public:
	symList *getActions();
	symList *getObservations() ;
	symSet *getAOset();
	int nrActions();
	int nrObservs();
	Symbol *getAction(int i);
	Symbol *getObservation(int i);
	symList* getMemory(std::vector<Symbol*>* data, int pos, int len);
	Array1D<double>* getPH(symList2IntMap *Histcout);
	symSet* getaugAct(symSet* baseTest);
	std::vector<Symbol*>* getVecdata(symList* var);
	void resert(std::vector<float>*belief);
	symList*  getData2(int Len,symList*actList);
	Array2D<double>*getProb( symSet* interestTests,std::vector<float>*belief,int action_N,symList2IntMap *Histcout,int flag);
	std::vector<float> *getBeliefState(std::vector<float>*belief ,std::vector<Symbol*> *var);
	symSet *getAOQset(symSet* AOset,symSet* Testset);
	void Relize_symSet(symSet*symSetata);




	symList2IntMap* createPsrdata2(int num_Data);
	Symbol* sendAction1(Symbol* act) ;
	symList2IntMap *getHistcout1(symList2Vec2Map *divideData,int CUTOFF,string txtname);



	
	Array3D<double>* Array2TO3(Array2D<double>* PAOTH, int AOsize, int Tsize, int Hsize);

	
    SPECTRAL(const SIMULATOR& simulator);
    ~SPECTRAL();
    Symbol **S,**A,**O;
	Symbol *currState,*prevState;
    int S_sz,A_sz,O_sz;
    std::vector<Symbol*> *testData;
    double ***Tr;
    double ***Ob;
	
    

    void Display();

	
    const SIMULATOR& Simulator;
    

};
