#ifndef __PSR_SYSDYN_H_
#define __PSR_SYSDYN_H_

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



#define CONFIDENCE 0.995



typedef set<symList*, symbolComparator> symSet; // a set of symbol strings



/*
typedef struct {
  symList2EstMap *PSRS;//经历核-检验核对应的各矩阵
  symList2EstMap *aoPSRS;//经历核-检验核的一步扩展和所有一步檢验对应的各矩阵
} ProbMAP;
*/


typedef struct {
   symList2VecMap *maoMatrix;
   vector<double> *firstprob;
   symListVecMat *maoqMatrix;
} LM_modelPara;




typedef map<symList* ,double ,symbolComparator> symList2DoubleMap;//czz存放要移除的检验和剩余矩阵的条件数map





class psr_sysdyn : psr {
  // This class implements a PSR that uses the learning algorithm described in
  // James, M. & Singh, S. , "Learning & Discovery of PSRs in Dynamical Systems with Reset", 2004


  public:
  void learn();


  psr_sysdyn(Environment *e);
  ~psr_sysdyn();


  //void deleteStruct_tEs(tEstimateData* matData);//释放tEstimateData* matData的内存空间。
  void useMATLAB(symList2VfMap *landmarkbe,symList2Vec2Map *divideData,symList2symListSet *landmarkHist);//获取给定经历和检验下准确的概率矩阵，此矩阵用于MATLAB提取经历核和检验核
  //void deleteProbMAP(ProbMAP *PSRmatrix);//释放各个子空间模型中矩阵所占的内存
 

  ProbMAP *getProb(symList2Vec2Map *divideData,symList2symListSet *landmarkHist,int step);//已知经历核-检验核对应的行号和列号，求出经历核-检验核，并且计算经历核-检验核的估计概率矩阵。
  Whole_modelPara *getWhole_modelPara(symList2VfMap *landmarkbe,ProbMAP *PSRmatrix);//得到各个子空间模型参数,并组合成整个空间模型参数
  LM_modelPara *getLM_modelPara(Whole_modelPara *modelMatrix,symList *Landmark);//在整个空间模型whole_modelMatrix中找到指定Landmark对应的子空间模型参数LM_modelPara(包括Mao,Maoq,firstprob)
  set<int> *getHroworTcolumn();//把经历对应的行号存如set<int>--20130909czz



  Whole_modelPara *getOld_Whole_modelPara(symList2Vec2Map *divideData, symSet* landmarkSet, symList2symListSet *landmarkHist);
tEstimateData *removeColumn(tEstimateData* matData,int testNum,int k);
symListDblMat *removeRow(tEstimateData* matData,int k);
int getEstaRank(tEstimateData* matData);


  double getError_oneStep(int2SymListMap *startLMPos,LM_modelPara *startlandmark_para,vector<float> *startLMbelief,Whole_modelPara *whole_modelMatrix,vector<Symbol*> *testData,symSet* landmarkSet);//获取一步检验的PSR预测值与POMDP准确值的差值(TY)
  double getError_fourStep(int2SymListMap *startLMPos,LM_modelPara *startlandmark_para,vector<float> *startLMbelief,Whole_modelPara *whole_modelMatrix,vector<Symbol*> *testData,symSet* landmarkSet);//获取四步检验的PSR预测值与POMDP准确值的差值


  private:


  int m, n, size_m, size_n, size_act;
  double  numerator, denominator;

  Array1D<double> *tempCoreValue, *coreValue;


  symSet  *augTest, *nextAoSet, *augTest1;

  symList *del, *newAO;
  SVD<double> *singular;
 
  symListMatrix::iterator tiH1 ;
  symList2IntMap::iterator tempTit;
  symList2DblMap::iterator qP, tempPit;

  symListVecMat::iterator  viH, viH4;
  symIter  iCurr2;

  map<symList* ,symListVecMat*>::iterator lmMaoq;
  map<symList* ,symList2VecMap*>::iterator lmMao;
  map<symList*,vec*>::iterator lmfP;
  map<symList*, symList2DblMap*>::iterator lmqP;


  symListDblMat  *ctProbP,*ctProbP1;
  symListMatrix  *ctMatP, *ctActP,*ctMatP1,*ctActP1;
  Array2D<double> *probsP,  *probsP1;
  Array2D<int>    *countsP, *countsP1,*actcountsP,*actcountsP1;
  Array2D<int>    *augActcounts,*augActcounts1;

  Array1D<int> *hcountsP;
  symList2IntMap *ctHistP,*ctHistsP,*ctHistsP1,*rowMap1;                 //ctHistP需不需要，暂时先留着
  symList2DblMap *rowDblMap1;                        //以 symList 为 key 值，而以 double 数据为元素的 map

  bool zeroColumn,zeroRow;
  double epsilon, cutoffValue;
  double TH, THa, norm;





};

#endif
