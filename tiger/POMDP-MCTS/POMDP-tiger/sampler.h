#ifndef __SAMPLER_H_
#define __SAMPLER_H_

//#include "psr.h"
#include "environment.h"

#include "stdlib.h"
#include "time.h"
#include "math.h"
#include <limits>
#include "tnt/tnt_array2d.h"
#include "tnt/tnt_array1d.h"

#include <float.h>

using namespace std;
using namespace TNT;

namespace Sampler {

  typedef map<symList*, double*, symbolComparator> symList2DblMap; 
  typedef map<symList*, symList2DblMap*, symbolComparator> symListDblMat; 

  typedef map<symList*, int *, symbolComparator> symList2IntMap;
  typedef map<symList*, symList2IntMap*, symbolComparator> symListMatrix;

  typedef map<symList*, symList*, symbolComparator> symList2ListMap; //ºá×ø±êÎª landmark, ×ÝÎª Æä¶ÔÓŠµÄŸ­Àú


  typedef struct {
    // these Array2D's contain the actual data:
    Array2D<double>	*probs;		
    Array2D<int>	*counts;	// [i,j] is # of times test t_j was successful after history h_i
    Array2D<int>	*actcounts;	// [i,j] is # of times test t_j was attempted after history h_i (its act's were exec'ed)
    Array1D<int>	*hcounts;	// [i]   is # of times history h_i occured
    // these point to the appropriate data in the Array#D's and are used for reference by symbol list:
    symListDblMat	*ctProb;	// -> probs
    symListMatrix	*ctMat;		// -> counts
    symListMatrix	*ctAct;		// -> actcounts
    symList2IntMap	*ctHists;	// -> hcounts
  } tEstimateData;

  typedef struct {
    // these Array2D's contain the actual data:
    Array2D<double>	*probs;		
    Array1D<int>	*hcounts;	// [i]   is # of times history h_i occured
    // these point to the appropriate data in the Array#D's and are used for reference by symbol list:
    symListDblMat	*ctProb;	// -> probs
    symList2IntMap	*ctHists;	// -> hcounts
  } POMDP_probData;








  symSet* getaugAct(symSet* baseTest,int testLen,Environment *env);
  symList* getTest(vector<Symbol*>* data, int pos, int len); //获取vector<Symbol*>数据data中位置为pos，长度为len的检验
  symList* getHistory(vector<Symbol*>* data, int pos, int len); //获取vector<Symbol*>数据data中位置为pos，长度为len的经历
  symList* getMemory(vector<Symbol*>* data, int pos, int len);//获取vector<Symbol*>数据data中位置为pos，长度为len的记忆
  symList* getTestL2(symList* data, int pos, int len);//获取symList数据data中位置为pos，长度为len的检验(data以a开头）
  symList* getTestL(symList* data, int pos, int len);//获取symList数据data中位置为pos，长度为len的检验（data以o开头）
  symList* getHistoryL(symList* data, int pos, int len);//获取symList数据data中位置为pos，长度为len的经历
  symList* getMemoryL(symList* data, int pos, int len); ////获取symList数据data中位置为pos，长度为len的记忆

  POMDP_probData* exactProb(Environment *env,symSet* baseTest,symSet* baseHist,vector<symList*>* elementVec,vector<float>*belief);//根据POMDP文件获取准确的经历-检验概率矩阵
  tEstimateData*  augEstimate(Environment *env,symSet* baseTest,symSet* baseHist,vector<symList*> *elementVec,int testLen);//根据发生次数获取估计的经历-检验概率矩阵



  template <class type> type infNorm(Array2D<type> *M) 
  {
    // a function that could prove useful for whoever needs the sampler
    // g++ requires the definition of template functions to be readily available (i.e. from header) otherwise it won't link这个地方求的是矩阵的行和范数，目的是为了得到奇异值的阏值
    type max = -1 * numeric_limits<type>::max(); // this might cause real trouble with unsigned types, but that's fine enough...
    type sum;
    type *row;
    int i, j;
    cout<<"M->dim1()="<<M->dim1()<<" M->dim2()="<<M->dim2()<<endl;
    if( (M->dim1())>=(M->dim2()) )                 //如果 m>=n,求行和范数
    for (i=0; i<M->dim1(); i++) 
      {
	sum = 0;
	row = (*M)[i];
	for (j=0; j<M->dim2(); j++) sum += row[j];
	if (sum>max) max = sum;
      }
    else
      for(j=0; j<M->dim2(); j++)                  //如果 m<n, 求列和范数
      {
	sum = 0;
	for(i=0; i<M->dim1(); i++)
	  sum += (*M)[i][j];
	if(sum>max) 
	  max = sum;
      }
    return max;
  }



}

#endif
