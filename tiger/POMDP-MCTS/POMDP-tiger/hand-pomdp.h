#ifndef __HAND_POMDP_H_
#define __HAND_POMDP_H_

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
  						 } HandPomdpData;
typedef map<symList*,Alphadata*,symbolComparator> symAlpha;
class hand_pomdp : psr // This class implements a PSR that uses the learning algorithm described in
                      // James, M. & Singh, S. , "Learning & Discovery of PSRs in Dynamical Systems with Reset", 2004
      {
       public:
       void learn();
       hand_pomdp(Environment *e);
       ~hand_pomdp();
			 HandPomdpData *getRandomPomdp(int Asize,int Ssize,int Osize);//获取初始随机的POMDP模型
			 Array2D<double>*getAlpha(HandPomdpData *RandomPOMDP,symList *NewAbstractData, int size_NewS,Array1D<int>*action,Array1D<int>*observation,vector<float> *belief);//计算每一个landmark对应的所有抽象数据数据的alpha值矩阵
       Array2D<double>*getBeta(HandPomdpData *RandomPOMDP,symList*NewAbstractData,int size_NewAct,int size_NewS,Array1D<double>*scale,Array1D<int>*action,Array1D<int>*observation);//计算每一个landmark对应的所有抽象数据数据的Beta值矩阵
			 Array1D<double>*getScale(HandPomdpData *RandomPOMDP,symList*NewAbstractData, int size_NewS,Array1D<int>*action,Array1D<int>*observation);//计算sacle【t】的值
			 HandPomdpData*youhua(HandPomdpData *RandomPOMDP ,int size_NewAct,int size_S,int size_NewOb);//对POMDP模型进行优化
			 void *outputPOMDP(HandPomdpData *RandomPOMDP,int size_NewAct,int size_S,int size_NewOb);//输出POMDP模型到txt文件上  
       double getError(HandPomdpData *RandomPOMDP,symSet *interestTests,int size_newS,vector<float> *belief);//计算误差
			 float getabstractBeliefprob(HandPomdpData *RandomPOMDP,vector<float>*belief ,symList*newA,symList*obs,int size_NewS);//计算Abstractbelief_start下，NEWANEWO发生的概率
			 vector<float>* getAbstractBeliefState(HandPomdpData *RandomPOMDP,vector<float>*belief ,symList*newA,symList*obs,int size_NewS);//计算抽象数据的信念状态
       Array1D<int>*getObservation(symList*NewAbstractData);//获取抽象数据的所有观测
       Array1D<int>*getAction(symList*NewAbstractData);//获取抽象数据的所有动作
			 Array3D<double>*getSigma(HandPomdpData *RandomPOMDP,symList*NewAbstractData,int size_NewAct,int size_NewOb,Array2D<double>*Beta,Array2D<double>*Alpha,Array1D<int>*action,Array1D<int>*observation);//求参数西各马，表达智能体在t时刻在状态i,t+1时刻在状态j的概率
       Array2D<double>*getGama(symList*NewAbstractData,int size_NewAct,int size_NewOb,Array2D<double>*Beta,Array2D<double>*Alpha,Array1D<double>*scale);//求参数gama，表达智能体在t时刻在状态i的概率
       HandPomdpData *upDataTMatrix(HandPomdpData *RandomPOMDP,symList*NewAbstractData,int size_NewAct,int size_NewOb,Array3D<double>*sigma,Array2D<double>*gama,Array1D<int>*action);//更新状态转移矩阵
       HandPomdpData *upDataOMatrix(HandPomdpData *RandomPOMDP,symList*NewAbstractData,int size_NewAct,int size_S,int size_NewOb,Array2D<double>*gama,Array1D<int>*observation);//更新观测值转移矩阵
       HandPomdpData *EM(HandPomdpData *RandomPOMDP,symList *NewAbstractData,int size_NewAct, int size_newS,int size_NewOb,vector<float> *belief);//利用EM算法更新POMDP模型       
 			 symSet *getAOset();//得到所有动作-观测ao对的集合AO
  		 symList* getA(symList* ao,symList2SymbolMap *newAct);//已知原始ao得到抽象后的动作
  		 symList* getO(symList*history,symList_sym2symMap *S_h_newB,symList*landmark);//得到h下的抽象观测
       symList2Group::iterator shp;
       symList2Prob::iterator sp;
       symList_DblMap ::iterator sd;
       vector<symList2Prob*>::iterator vs;
  		 vector<float>::iterator vf;
  		 SVD<double> *singular;
};
#endif

