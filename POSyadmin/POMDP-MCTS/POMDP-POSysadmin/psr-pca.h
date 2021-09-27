#ifndef __PSR_PCA_H_
#define __PSR_PCA_H_

#include "psr.h"
#include "symbol.h"
#include "environment.h"
#include "sampler.h"

#ifdef CONDDELAY
#include <time.h>
#endif

#include <math.h>
#include <algorithm>

// the linear algebra is handled using the TNT and JAMA packages maintained by Roldan Pozo from the
// Mathematical and Computational Science Division of the National Institute of Standards and Technology
//	http://math.nist.gov/tnt/
#include "tnt/tnt_array1d.h"
#include "tnt/tnt_array2d.h"
#include "tnt/tnt_array3d.h"
#include "jama/jama_lu.h"
#include "jama/jama_svd.h"

using namespace Sampler;
using namespace TNT;
using namespace JAMA;


typedef map<symList*, tEstimateData*,symbolComparator> symList2EstMap;//landmark和相应的子状态空间组成的map数据

typedef struct {
  symListDblMat	*ctProb;//存储概率矩阵p(ao|B)
  symListDblMat *QaoQctProb;//存储概率矩阵p(Q|B),p(aoQ|B)
} ProbMat;

typedef map<symList*, symListDblMat*,symbolComparator> symList2DblMatMap;





  typedef struct {
    // these Array2D's contain the actual data:
    Array2D<double>	*probs;		
    //symListDblMat	*ctProb;	// -> probs
  } tEstimateT;

typedef map<symList*, tEstimateT*,symbolComparator> symList2TMap;//LM-T0

typedef map<symList*, symList2TMap*,symbolComparator> symList2symListTMap;//LM-ao-Tao



  typedef struct {
    // these Array2D's contain the actual data:
    Array2D<double>	*probs;		
    symListDblMat	*ctProb;	// -> probs
  } tEstimateProb;//用于存储p(Q|Bao)



typedef struct {
   symList2VecMap *maoMatrix;
   vector<double> *firstprob;
   symList2TMap *MaoMatrix;
} pca_LM_modelPara;





class psr_pca : psr {
  // This class implements a PSR that uses the learning algorithm described in
  // James, M. & Singh, S. , "Learning & Discovery of PSRs in Dynamical Systems with Reset", 2004


  public:

  void learn();

  psr_pca(Environment *e);
  ~psr_pca();

  ProbMAP *getProb(symList2Vec2Map *divideData,symList2symListSet *landmarkHist,int step);//给定经历的集合landmarkHist和检验的集合Q，求概率矩阵p(ao|Q),p(Q|B),p(aoQ|B)
  ProbMAP *fillProb(symList2VfMap *landmarkbe,ProbMAP *Probmatrix);//补充p(ao|Q),p(Q|B),p(aoQ|B)的缺值

  symList2DblMatMap *getCaoMap(ProbMAP *fillProbmatrix);//每个子状态空间对应的Cao集合
  symListDblMat *getCao(tEstimateData *matData);//给定p(ao|Q)及其相关计数概率矩阵,得到单纯的p(ao|Q)
  symSet *getQtest(ProbMAP *fillProbmatrix, int step);//概率矩阵p(Q|B)中检验的集合Qtest

  symList2TMap *getT0Map(ProbMAP *fillProbmatrix, symSet *Qtest);//给定p(ao|Q),p(Q|B),p(aoQ|B)及其相关计数概率矩阵,得到单纯的p(Q|B),即T0

  symSet *getAOset();//得到所有动作-观测ao对的集合AO


  tEstimateProb *getTao(tEstimateData *matData, symSet *Qtest, symList* ao, symList2TMap *T0Map);//给定p(ao|Q),p(Q|B),p(aoQ|B)及其相关计数概率矩阵,得到单纯的p(Q|Bao),即Tao

  symList2symListTMap *getTMap(ProbMAP *fillProbmatrix,symSet *Qtest, symSet *AOset, symList2TMap *T0Map);//每一子状态空间的概率矩阵p(Q|BAO),即每一子状态空间所有的Tao的集合

  symList2TMap *getLmTmatrixMap(symList2symListTMap *LmTaoMap, symList2TMap *T0Map, int size_ao);//给定Tao,T0,得到T0和所有的Tao叠加起来形成概率矩阵T 
  

  symList2IntMap *getNumerState(symSet* landmarkSet);//每一子状态空间的状态的个数
  symList2TMap *decompSVD(symList2TMap *LmTmatrixMap, symList2IntMap *NumerState);//给定概率矩阵T,得到每一子状态空间的R矩阵

  symList2TMap *getX0(symList2TMap *LmRMap, symList2TMap *T0Map);//给定每一子状态空间的R矩阵和T0,得到每一子状态空间的X0矩阵,X0=T0*R
  symList2symListTMap *getXao(symList2TMap *LmRMap, symList2symListTMap *LmTaoMap);//给定每一子状态空间的R矩阵和Tao,得到每一子状态空间的中每一个ao对应的Xao矩阵,Xao=Tao*R

  symList2VecMap *getmao_Mao(symSet* landmarkSet);//Lm_mao_MaoMap--andmark对应的所有mao或Mao的值
  symListVecMat *getLm_maoMap(symList2TMap *LmRMap, symList2VecMap *Lm_mao, symSet *AOset);//每一子状态空间中每一个ao对应的mao

  symList2symListTMap *getLm_MaoMap(symList2TMap *LmRMap, symList2VecMap *Lm_Mao, symSet *AOset);//每一子状态空间中每一个ao对应的Mao

  symList2VecMap *getLM_firstprob(   symList2TMap *X0Map );//每个landmark对应子状态空间模型的学习参数firstprob 
  pca_LM_modelPara *getLM_modelPara(symListVecMat *Lm_maoMap,   symList2symListTMap *Lm_MaoMap, symList2VecMap *LM_firstprobMap, symList *Landmark);//该Landmark对应的子空间模型(包含maoMatrix,firstprob,MaoMatrix)

double getError_oneStep(symSet* landmarkSet, vector<Symbol*> *testData, vector<float> *startLMbelief, int2SymListMap *startLMPos, pca_LM_modelPara *startLM_modelPara,  symList2symListTMap *Lm_MaoMap, symListVecMat *Lm_maoMap, symList2VecMap *LM_firstprobMap);

double getError_fourStep(symSet* landmarkSet, vector<Symbol*> *testData, vector<float> *startLMbelief, int2SymListMap *startLMPos, pca_LM_modelPara *startLM_modelPara,  symList2symListTMap *Lm_MaoMap, symListVecMat *Lm_maoMap, symList2VecMap *LM_firstprobMap);






  private:
  symIter  iCurr, iCurr1, iCurrH;
  symList2DblMatMap::iterator s2DM;
  
  symList2symListTMap::iterator sLsT, sLsT1;
  symList2TMap::iterator sT, sT1, sT2, sT3, sT4;

  SVD<double> *singular;

  symListVecMat::iterator Lm_m;

tEstimateT *newAO_Mao;
    Array2D<double>	*probs;	
    symList *newAO;


};

#endif
