#ifndef __ENVIRONMENT_H_
#define __ENVIRONMENT_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "symbol.h"
#include "cassandra_grammar.h"

#include <set>
#include <map>
#include <vector>
using namespace std;

typedef set<symList *, symbolComparator> symSet; // a set of symbol strings
typedef symSet::iterator symIter; // an iterator over a set of symbol strings
typedef vector<Symbol*>::iterator vecsymIter;//vector<Symbol*>跌代器
typedef vector<symList*> vec2;//ty
typedef map<symList *,vec2*, symbolComparator> symList2Vec2Map;//ty
typedef map<symList*,vector<float>*,symbolComparator> symList2VfMap;
typedef map<symList*, double, symbolComparator> symList_DblMap;  
typedef map<symList*, symList_DblMap*,symbolComparator> symList2Prob;
typedef map<symList*,vector<symList2Prob*>*,symbolComparator>symList2Group;
typedef map<symList*, Symbol*,symbolComparator> symList2SymbolMap;
class Environment 
			{	
 			 public:
  		 Symbol *getAction(int i);//判断输入的整数i是否小于该环境下对应的动作个数，若小于，则返回对应下标为i的动作，反之则返回NULL
  		 Symbol *getObservation(int i);	//判断输入的整数i是否小于该环境下对应的动作个数，若小于，则返回对应下标为i的动作，反之则返回NULL
  		 symList *getActions();		// 获得环境下所有动作的连续序列（例如环境中有4个动作a1、a2、a3、a4，则得到a1-a2-a3-a4）
  		 symList *getObservations();	// 获得环境下所有观测值的连续序列（例如环境中有4个观测值O1、O2、O3、O4，则得到O1-O2-O3-O4）
  		 int nrActions();//得到对应环境下的动作个数
  		 int nrObservs();//得到对应环境下的观测值个数  
  		 Symbol **setLandmark(Symbol **Obs);//设定landmark
  		 Symbol *sendAction(Symbol* act);   //智能体在当前状态下采取给定的动作最终产生的观测值
  		 vector<Symbol*> *getData(int hist_Len);//获得长度为Len的连续的ao序列(TY)
  		 vector<Symbol*> *getData1(vector<float>*belief, int hist_Len);
			 symList* getData2(int Len,symList*actList);//获得长度为Len的连续的ao序列(TY)
			 vector<Symbol*>* getData3(symList *startPosLm,vector<float>*belief,int count);//获得一组数量为count,指定从某状态开始并且从某状态结束的连续的ao序列，并将这组数据首尾相接
			  vector<symList*>* getData5(vector<float>*belief, int count, int length);//产生固定长度的几串训练数据
  		 symList2VfMap *getLMbelief(symSet* landmarkSet);//得到每一个landmark对应的信念状态
			 symSet*findLandmarkSet();//得到landmark的集合（已经设定过landmark）
  		 symList *getDataList(vector<Symbol*>* var);//把vector<Symbol*>型的动作-观测序列数据转换成symList型
  		 vector<Symbol*> *getVecdata(symList* var);//把symList型的动作-观测序列数据转换成vector<Symbol*>型
  		 symList2Vec2Map *divideData(vector<Symbol*>* testData);//对完整数据Data利用landmark划分为不同的状态空间，把所有以相同landmark开头的数据存在一起（TY）
  		 vector<Symbol*> *getconData();//读取文件PsrDataId.txt的用来进行PSR建模的训练数据存放到vector<Symbol*>*conData中
  		 vector<float> *getBeliefState(vector<float>*belief ,vector<Symbol*> *var); // 计算智能体在当前信念状态belief下经过经历var后，得到观测值后的信念状态(TY)
  		 float getPomdpProb(vector<float>*belief, symList* var);//当前belief state下，观测序列var出现的概率
  		 symSet * getInterestTest(vector<int>*interestTest_id);//得到感兴趣的检验,这里我们设定为对某些观测值感兴趣
		//	 symSet * getInterestTest(symSet *AOset,int testnum);//获得指定个数的随机感兴趣的检验
			 void createPsrdata(symList *startPosLm,vector<float>*belief,int Length_Data);//利用getData3()生成一串训练数据，并且存在名为Psrdataid.txt文件中
       void createPsrHanddata(int Length_Data);//利用getData3()生成一串训练数据，并且存在名为Psrdataid.txt文件中
  		 void createTestdata(int Length_Data);//利用getData()生成一串训练数据，并且存在名为Testdataid.txt文件中
 			 void resert(vector<float>*belief);//将智能体的初始状态设定为当前需要的信念状态
			 symList2SymbolMap *getNewAct(symSet *AOset);//获取所有新动作及其对应内容
			 Symbol *getNewObservation(int i);//判断输入的整数i是否小于该环境下对应的新观测值个数，若小于，则返回对应下标为i的新观测值，反之则返回值为空
			 int getStatesize();//得到模型的状态个数
  		 Environment ( ); //构造函数，读取POMDP文件，存储POMDP文件的内容，设定landmark和初始状态      
  		 ~Environment( );//析构函数，释放内存
  		 Symbol **S ,**A, **O, **N, **B;	// MUST be set by descendant
  		 int S_sz ,A_sz, O_sz, N_sz, B_sz;
  		 vector<Symbol*> *testData;
  		 Symbol   *currState, *prevState;
  		 symList  *dataList;
  		 FILE *cass;	// the POMDP spec file in the grammar of Tony Cassandra
  		 Cassandra_return *ret;			// ( http://www.cs.brown.edu/research/ai/pomdp/ )
  		 float	*** Tr;		// Tr[a][s1][s2] is the probability of getting from s1 to s2 under a
  		 float	*** Ob;		// Ob[a][s][o] is the prob of observing o when performing a and ending up in s 
  	char * arr_states; // arr_states[i] is a NULL-terminated string representing mnemonic of state i
	char * arr_act; // arr_act[i] is a NULL-terminated string representing mnemonic of action i
	char * arr_obs; // arr_obs[i] is a NULL-terminated string representing mnemonic of observation i

	char * arr_newact;
	char * arr_newobs;
};
#endif

