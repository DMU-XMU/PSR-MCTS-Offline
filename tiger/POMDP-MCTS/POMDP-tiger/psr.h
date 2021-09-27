#ifndef __PSR_H_
#define __PSR_H_

#include "symbol.h"
#include "environment.h"
#include "sampler.h"
// === container templates for symbols, in use by sampler and different PSRs
#include <set>
#include <map>
#include <vector>


using namespace std;
using namespace Sampler;


typedef set<int> IntSet;
typedef set<symList *, symbolComparator> symSet; // a set of symbol strings
typedef symSet::iterator symIter; // an iterator over a set of symbol strings
typedef vector<symList2ListMap*> symList2symListVec;//存储landmark,及这一landmark开头后一landmark结尾的数据 
typedef vector<double> vec;
typedef map<symList*,vec*, symbolComparator> symList2VecMap;
typedef map<symList*, symList2VecMap*, symbolComparator> symListVecMat;
typedef map<symList*, tEstimateData*,symbolComparator> symList2EstMap;//landmark和相应的子狀太空间组成的map数据
typedef map<symList*, symList2VecMap*,symbolComparator> symList2symListVecMatMapMap;//存储landmark及学习参数Mao
typedef map<symList*, symListVecMat*,symbolComparator> symList2symListVecMatMap;//存储landmark及学习参数Maoq
typedef map<symList*, symSet*, symbolComparator> symList2symListSet;//存儲每个landmark及其对应的满足条件的经历的集合
typedef map<symList*, vector<float>*,symbolComparator> symList2VfMap;//landmark和相应的信念状态
typedef map<int, symList*> int2SymListMap;
typedef map<symList *,vector<symSet*>*, symbolComparator> symList2VecSetMap;//ty
typedef struct{ Array2D<double>*Alpha;}Alphadata;
typedef struct {
    symList2symListVecMatMap *landmarkMaoqMap;//存储landmark及学习参数Maoq
    symList2symListVecMatMapMap *landmarkMaoMap;//存储landmark及学习参数Mao
    symList2VecMap *landmarkfirstProbMap;//存储landmark及每个子模型的初始状态;
    symListDblMat *lmqtestProbMap;//存储landmark及每个子模型的初始状态中每个检验及检验对应的概率;
}Whole_modelPara;


typedef struct {
  symList2EstMap *aoProb;//存储概率矩阵p(ao|B)及其相关的计数矩阵
  symList2EstMap *QaoQProb;//存储概率矩阵p(Q|B),p(aoQ|B)及其相关的计数矩阵
} ProbMAP;


class psr {
// the base class for a Predictive State Representation of a controlled dynamical system
// (more specifically, a base class for linear (s|e)-psr)
// descendants should be created for each kind of learning algorithm, implementing the learn(n) method

  public:
		
  virtual void learn()= 0;	   // let the PSR work with the environment to learn the parameters it needs
 // virtual   ProbMAP *getProb(symList2Vec2Map *divideData,symList2symListSet *landmarkHist,int step);//= 0;//计算经历-检验的估计概率矩阵。


//virtual Whole_modelPara *getWhole_modelPara(symList2VfMap *landmarkbe,ProbMAP *PSRmatrix)= 0;//得到各个子空间模型参数,并组合成整个空间模型参数;

 symList2symListSet *getHistorySet(symList2Vec2Map *divideData,int CUTOFF);//得到发生次数大于CUTOFF的经历的集合	
  symList2VecSetMap *getHistorySet2(symList2Vec2Map *divideData,int CUTOFF,symList2ListMap*landmarkhis,symList*startPosLm);//得到整体发生次数大于CUTOFF的经历的集合和发生次数少于CUTOFF的集合
  
  symList2symListSet *getMHistorySet(symList2VecSetMap*landmarkHist);//得到发生次数大于CUTOFF的经历的集合和发生次数少于CUTOFF的集合后，得到发生次数大于CUTOFF的经历的集合
  symList2symListSet *getLHistorySet(symList2VecSetMap*landmarkHist);//得到发生次数小于CUTOFF的经历的集合	
  vector<float> *getStartLMbelief(symList *startLandmark,symList2VfMap *landmarkbe);//在各landmark对应的信念状态集合中找到startLandmark对应的信念状态
  int2SymListMap *getStartLMPos(vector<Symbol*> *testData,symSet* landmarkSet);//找到testData的第一个landmark以及landmark在testData中的位置（TY）
symList2IntMap *getHistcout(symList2Vec2Map *divideData);//得到整体状态空间发生的经历和经历发生的次数
	
	symList2IntMap *getHistcout1(vector<symList*>  *divideData);//得到整体状态空间发生的经历和经历发生的次数
  tEstimateData *fillValue(symList* var1,symList2EstMap* var2);//补充缺值。
  void deleteStruct_pom(POMDP_probData* pomData);//释放POMDP_probData* pomData的内存空间。
  void deleteProbMAP(ProbMAP *Probmatrix);
  void deleteStruct_tEs(tEstimateData* matData);

  psr(Environment *e);
  ~psr();

  symList2VfMap ::iterator svf;
  symList2EstMap::iterator s2Es;
  symList2DblMap::iterator piT, piT1;
  symListMatrix::iterator  iH, tiH, iH1;
  symListDblMat::iterator  piH, piH1;
  symList2IntMap::iterator iT, iT1, tiT;

  symIter  iCurr, iCurr1, iCurrT;
  int i, j;
  double *rowDbl;
  int    *row;
  symList2DblMap *rowDblMap;
  symList2IntMap *rowMap; 

  symList2VecMap::iterator viT;


  protected:
  Environment *env;


};

#endif
