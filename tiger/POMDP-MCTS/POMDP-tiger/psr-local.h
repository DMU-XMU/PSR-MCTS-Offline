#ifndef __PSR_LOCAL_H_
#define __PSR_LOCAL_H_

#include "psr.h"
#include "symbol.h"
#include "environment.h"
#include "sampler.h"

#ifdef CONDDELAY
#include <time.h>
#endif

#include <math.h>
#include <algorithm>

#include "tnt/tnt_array1d.h"
#include "tnt/tnt_array2d.h"
#include "tnt/tnt_array3d.h"
#include "jama/jama_lu.h"
#include "jama/jama_svd.h"

using namespace Sampler;
using namespace TNT;
using namespace JAMA;

typedef map<symList*, double, symbolComparator> symList_DblMap;  
typedef map<symList*, symList_DblMap*,symbolComparator> symList2Prob;
typedef map<symList*,vector<symList2Prob*>*,symbolComparator>symList2Group;
typedef vector<double> vec_dou;
typedef map<symList*, vec_dou*, symbolComparator> symListDblMap;  
typedef map<symList*,symListDblMap*,symbolComparator>symList_symListDblMap;
typedef map<symList*,symList*,symbolComparator>symList_symListMap;
typedef map<symList*,symList_symListMap*,symbolComparator>symList_sym2symMap;


typedef struct {
    						Array3D<double>	*TMatrix;		
   						  Array3D<double>	*OMatrix;	
  						 } PomdpData;
typedef map<symList*,PomdpData*,symbolComparator>symPomdpMap;
typedef map<symList*,Alphadata*,symbolComparator> symAlpha;
class psr_local : psr // This class implements a PSR that uses the learning algorithm described in
                      // James, M. & Singh, S. , "Learning & Discovery of PSRs in Dynamical Systems with Reset", 2004
      {
       public:
       void learn();
     psr_local(Environment *e);
       ~psr_local();
			 symList2ListMap*getLandmarkhist(vector<Symbol*>* conData, symSet* landmarkSet,int startPos);//求取每个landmark所对应的经历
       symList2Vec2Map*getfinalhis(symList2Vec2Map*newdata,symList *startPosLm);//获得整体状态空间的经历
			 PomdpData *getRandomPomdp(int Asize,int Ssize,int Osize);//获取初始随机的POMDP模型
			 Array2D<double>*getAlpha(PomdpData *RandomPOMDP,symList*startPosLm,symList *NewAbstractData,int size_NewAct, int size_NewOb,Array1D<int>*action,Array1D<int>*observation,vector<float> *belief);//计算每一个landmark对应的所有抽象数据数据的alpha值矩阵

	Array2D<double>*getAlpha1(PomdpData *RandomPOMDP,symList *NewAbstractData,int size_NewAct, int size_NewOb,Array1D<int>*action,Array1D<int>*observation,vector<float> *belief);

       Array2D<double>*getBeta(PomdpData *RandomPOMDP,symList*startPosLm,symList*NewAbstractData,int size_NewAct,int size_NewOb,Array1D<double>*scale,Array1D<int>*action,Array1D<int>*observation);//计算每一个landmark对应的所有抽象数据数据的Beta值矩阵
       
	Array2D<double>* getBeta1(PomdpData *RandomPOMDP, symList* NewAbstractData,int size_NewAct,int size_NewOb, Array1D<double>* scale, Array1D<int>* action, Array1D<int>* observation);
       
       symList*getAbstract(vector<Symbol*>* conData,symSet* landmarkSet,symList2ListMap*landmarkhis,symList_sym2symMap *S_h_newB,symList2SymbolMap *newAct,symList *startPosLm,int startPos);//对一长串数据进行抽象
       symList*getAbstract1(symList* conData,symList_symListMap *S_h_newB,symList2SymbolMap *newAct);
       
			 Array1D<double>*getScale(PomdpData *RandomPOMDP,symList*NewAbstractData, int size_NewOb, symList*startPosLm,Array1D<int>*action,Array1D<int>*observation);//计算sacle【t】的值
			 
	Array1D<double>*getScale1(PomdpData *RandomPOMDP, symList* NewAbstractData, int size_NewOb, Array1D<int>*action, Array1D<int>* observation);
		
			 PomdpData*youhua(PomdpData *RandomPOMDP ,int size_NewAct,int size_S,int size_NewOb);//对POMDP模型进行优化
			 void *outputPOMDP(PomdpData *RandomPOMDP,int size_NewAct,int size_S,int size_NewOb,string pomdptxt);//输出POMDP模型到txt文件上  
       double getError(PomdpData *RandomPOMDP,symList_symListDblMap *newB_prob,symList2SymbolMap *newAct,symSet *interestTests,symList2VfMap *landmarkbe,int size_S,symSet* landmarkSet,symList*startPosLm);//计算误差
       
	double getError1(PomdpData *RandomPOMDP, symListDblMap *newB_prob, symList2SymbolMap *newAct, symSet *interestTests, int size_S);
       
			 float getabstractBeliefprob(PomdpData *RandomPOMDP,vector<float>*belief ,symList*var,int size_NewS);//计算Abstractbelief_start下，NEWANEWO发生的概率
			 vector<float>* getAbstractBeliefState(PomdpData *RandomPOMDP,vector<float>*belief ,symList*newA,symList*obs,int size_NewS);//计算抽象数据的信念状态
  		 int getSympos(vector<Symbol*>* conData,symList*startPosLm);//获取指定landmark在数据中所在的位置
       Array1D<int>*getObservation(symList*NewAbstractData);//获取抽象数据的所有观测
       Array1D<int>*getAction(symList*NewAbstractData);//获取抽象数据的所有动作
			 Array3D<double>*getSigma(PomdpData *RandomPOMDP,symList*NewAbstractData,int size_NewAct,int size_NewOb,Array2D<double>*Beta,Array2D<double>*Alpha,Array1D<int>*action,Array1D<int>*observation);//求参数西各马，表达智能体在t时刻在状态i,t+1时刻在状态j的概率
       Array2D<double>*getGama(symList*NewAbstractData,int size_NewAct,int size_NewOb,Array2D<double>*Beta,Array2D<double>*Alpha,Array1D<double>*scale);//求参数gama，表达智能体在t时刻在状态i的概率
       PomdpData *upDataTMatrix(PomdpData *RandomPOMDP,symList*NewAbstractData,int size_NewAct,int size_NewOb,Array3D<double>*sigma,Array2D<double>*gama,Array1D<int>*action);//更新状态转移矩阵
       PomdpData *upDataOMatrix(PomdpData *RandomPOMDP,symList*NewAbstractData,int size_NewAct,int size_S,int size_NewOb,Array2D<double>*gama,Array1D<int>*observation);//更新观测值转移矩阵
       PomdpData *EM(PomdpData *RandomPOMDP,symList*startPosLm,symList *NewAbstractData,int size_NewAct, int size_S,int size_NewOb,vector<float> *belief);//利用EM算法更新POMDP模型   
       
	PomdpData *EM1(PomdpData *RandomPOMDP,symList* NewAbstractData,int size_NewAct, int size_S,int size_NewOb,vector<float> *belief);
       
  		 tEstimateData *getProb(symSet *interestTests,vector<float>*belief,int action_N,symList2IntMap *Histcout);//估计待分组的p(t|h)矩阵
  		 symList2Group*find_H_Groups(tEstimateData * matData,symList*startPosLm);//对得到的p（t|h）概率矩阵利用线性无关的方法进行分组
  		 symList2Group* find_H_Groups2(tEstimateData * matData,symList*startPosLm);//对得到的p（t|h）概率矩阵利用似然比检验的方法进行分组
	
	vector<symList2Prob*> *find_H_Groups1(tEstimateData * matData);////对得到的p（t|h）概率矩阵利用线性无关的方法进行分组
	vector<symList2Prob*> *find_H_Groups21(tEstimateData * matData);
	
       int findtherank(Array2D<double>*probsP,Array2D<double>*countsP);//利用线性无关的方法判断有噪声的矩阵的2行是否相似
       int similarityTest(vector<int>*compareCounts);//计算两行的矩阵两行的相似度，相似返回1，不相似返回0
       int likelihood_ratioTest(vector<int>*TCompare);//利用似然比检验判断对2行来说某一个检验的相似度
       double f(int x);//对数似然比函数 			 
 			 symSet *getAOset();//得到所有动作-观测ao对的集合AO
  		 int getTrueNum_NewObs(symList2Group* H_Groups);//获取新观測真实个数(与POMDP文件中不一样) 
  		 int getTrueNum_NewObs1(vector<symList2Prob*>* H_Groups);
  		 symList_symListDblMap *getNewObs_prob(symList2Group* H_Groups,int size_NewOb);//获取所有新测值及其对应内容
	symListDblMap *getNewObs_prob1(vector<symList2Prob*>* H_Groups,int size_NewOb);
  		 symList_sym2symMap *getH_NewObs(symList2Group* H_Groups,int size_NewOb);//经历h及其对应的新观测值
	symList_symListMap *getH_NewObs1(vector<symList2Prob*>* H_Groups,int size_NewOb);  		 
  		 symList* getA(symList* ao,symList2SymbolMap *newAct);//已知原始ao得到抽象后的动作
  		 symList* getO(symList*history,symList_sym2symMap *S_h_newB,symList*landmark);//得到h下的抽象观测
	symList* getO1(symList*history,symList_symListMap *S_h_newB);
			 void getFinalresult(ofstream&Finalresult1,int j,double errsum,int size_NewOb,clock_t start,clock_t end);//将实验结果的数据存储起来
       symList2Group::iterator shp;
       symList2Prob::iterator sp;
       symList_DblMap ::iterator sd;
       vector<symList2Prob*>::iterator vs;
  		 vector<float>::iterator vf;
  		 SVD<double> *singular;
  		 
  		 vector<float> * getStartbelief();//从文件startbelief.txt读入初始
};
#endif

