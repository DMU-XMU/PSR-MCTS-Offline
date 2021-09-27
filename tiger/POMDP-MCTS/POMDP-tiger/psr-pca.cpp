#ifndef __PSR_PCA_CPP_
#define __PSR_PCA_CPP_


#include <fstream>
#include <iostream>
#include <time.h>
#include <iomanip>
#include "unistd.h"

#include "psr-pca.h"
#include "symbol.h"
using namespace std;

void psr_pca::learn()
{
  ofstream resultDataPCA("resultDataPCA.txt");//存储误差结果

  int data_YoN;
  cout<<"if create new data please input data_YoN=1 and donnot create new data please input data_YoN=2"<<endl;
  cout<<"data_YoN=";
  cin>>data_YoN;
  switch(data_YoN)
  {
    case 1:
    {
      int Length_Data;
      cout<<"please input the Length_Data=";
      cin>>Length_Data;
  //    env->createPsrdata(Length_Data);
      break;
    }
    case 2:
    {
      break;  
    }
  }


  //vector<Symbol*>* conData=env->getData(Length_Data);//获取整个状态空间的PSR模型训练数据
  cout<<"please input filename:/Psrdataid.txt"<<endl;      
  vector<Symbol*> *conData=env->getconData();//需获取整个状态空间的PSR模型时,这样获取数据,使得获取整个状态空间的PSR模型的数据和进行状态空间划分获取PSR模型的数据相同---20130822
  resultDataPCA<<"dataList--Len="<<conData->size()/2<<endl;

  symList2Vec2Map *divideData=env->divideData(conData);//分隔数据

  symSet* landmarkSet= new symSet();
  symList2Vec2Map::iterator vi=divideData->begin();
  for(vi=divideData->begin();vi!=divideData->end(); vi++)
  {
      landmarkSet->insert(new symList(vi->first));
  }//得到landmark的集合

  int CUTOFF;
  cout<<"please input the CUTOFF=";
  cin>>CUTOFF;cout<<"CUTOFF="<<CUTOFF<<endl;resultDataPCA<<"CUTOFF="<<CUTOFF<<endl;
  symList2symListSet *landmarkHist=getHistorySet(divideData,CUTOFF);//得到符合要求的经历的集合

  cout<<"please choose the step of Qtest:one or two"<<endl;
  cout<<"choose one step please input 1 in complex environmnet(such as hallway) and choose two step please input 2 in little environmnet(such as cheese-maze)"<<endl;
  int step;
  cin>>step;cout<<"step input="<<step<<endl;

  ProbMAP *Probmatrix=getProb(divideData,landmarkHist,step);//得到p(ao|Q),p(Q|B),p(aoQ|B)估计概率矩阵

  symList2VfMap *landmarkbe=env->getLMbelief(landmarkSet);//得到每个landmark对应的信念状态
 
  ProbMAP *fillProbmatrix=fillProb(landmarkbe,Probmatrix);//补充p(ao|Q),p(Q|B),p(aoQ|B)的缺值
  deleteProbMAP(Probmatrix);//释放内存空间
  delete Probmatrix;
  Probmatrix=NULL;

  symList2DblMatMap *CaoMap=getCaoMap(fillProbmatrix);//每个子状态空间对应的Cao集合  
  ofstream CaoData("CaoData.txt");//存储Cao
  for(s2DM=CaoMap->begin();s2DM!=CaoMap->end();s2DM++)
  {
     CaoData<<"lm="<<(*s2DM->first)<<endl;
     for(piH=s2DM->second->begin();piH!=s2DM->second->end();piH++)
     {
       for(piT=piH->second->begin();piT!=piH->second->end();piT++)
       {
         CaoData<<(*piT->second)<<" ";
       }
       CaoData<<";";
       CaoData<<endl;
     }
     CaoData<<endl;
  }

  symSet  *Qtest=getQtest(fillProbmatrix,step);//得到检验的集合Q

  symList2TMap *T0Map=getT0Map(fillProbmatrix,Qtest);//得到T0的集合
  ofstream LmT0matrix("LmT0matrix.txt");
  for(sT=T0Map->begin();sT != T0Map->end();sT++)
  {
    LmT0matrix<<"landmark="<<*(sT->first);
    LmT0matrix<<"row="<<sT->second->probs->dim1()<<" column="<<sT->second->probs->dim2()<<endl;
    for(int i=0;i<sT->second->probs->dim1();i++)
    {
      for(int j=0;j<sT->second->probs->dim2();j++)
      {
          LmT0matrix<<(*sT->second->probs)[i][j]<<" ";
      }
      LmT0matrix<<";";
      LmT0matrix<<endl;
    }
    LmT0matrix<<endl;
  }

  symSet *AOset=getAOset();//得到所有ao的集合

  symList2symListTMap *LmTaoMap=getTMap(fillProbmatrix, Qtest, AOset, T0Map);//得到Tao的集合
  ofstream LmTaomatrix("LmTaomatrix.txt");
  for(sLsT=LmTaoMap->begin();sLsT!=LmTaoMap->end();sLsT++)
  {
    LmTaomatrix<<"landmark="<<*(sLsT->first)<<endl;
    int row3=0;
    for(sT4=sLsT->second->begin();sT4 !=sLsT->second->end();sT4++)
    {
      for(int i=0;i<sT4->second->probs->dim1();i++)
      {row3++;
        for(int j=0;j<sT4->second->probs->dim2();j++)
        {
          LmTaomatrix<<(*sT4->second->probs)[i][j]<<" ";
        }
      LmTaomatrix<<";";
      LmTaomatrix<<endl;
      }
    }
    LmTaomatrix<<"row3="<<row3<<endl;
    LmTaomatrix<<endl;
  }

  int size_ao=AOset->size();

  symList2TMap *LmTmatrixMap=getLmTmatrixMap(LmTaoMap, T0Map, size_ao);//得到T
  ofstream LmTmatrix("LmTmatrix.txt");
  for(sT3=LmTmatrixMap->begin();sT3 != LmTmatrixMap->end();sT3++)
  {
    LmTmatrix<<"landmark="<<*(sT3->first);
    LmTmatrix<<"row="<<sT3->second->probs->dim1()<<" column="<<sT3->second->probs->dim2()<<endl;
    for(int i=0;i<sT3->second->probs->dim1();i++)
    {
      for(int j=0;j<sT3->second->probs->dim2();j++)
      {
          LmTmatrix<<(*sT3->second->probs)[i][j]<<" ";
      }
      LmTmatrix<<";";
      LmTmatrix<<endl;
    }
    LmTmatrix<<endl;
  }

  symList2IntMap *NumerState=getNumerState(landmarkSet);//每一子状态空间的状态的个数
  ofstream NumerStateData("NumerStateData.txt");
  for(iT=NumerState->begin(); iT!=NumerState->end(); iT++)
  {
    NumerStateData<<"landmark="<<*(iT->first)<<"  numberState="<<*(iT->second)<<endl;
  }

  symList2TMap *LmRMap=decompSVD(LmTmatrixMap, NumerState);//得到R
  ofstream LmRmatrix("LmRmatrix.txt");
  for(sT3=LmRMap->begin();sT3 != LmRMap->end();sT3++)
  {
    LmRmatrix<<"landmark="<<*(sT3->first);
    LmRmatrix<<"row="<<sT3->second->probs->dim1()<<" column="<<sT3->second->probs->dim2()<<endl;
    for(int i=0;i<sT3->second->probs->dim1();i++)
    {
      for(int j=0;j<sT3->second->probs->dim2();j++)
      {
          LmRmatrix<<(*sT3->second->probs)[i][j]<<" ";
      }
      LmRmatrix<<";";
      LmRmatrix<<endl;
    }
    LmRmatrix<<endl;
  }


  symList2TMap *X0Map=getX0(LmRMap, T0Map);//计算X0
  ofstream X0matrix("X0matrix.txt");
  for(sT3=X0Map->begin();sT3 != X0Map->end();sT3++)
  {
    X0matrix<<"landmark="<<*(sT3->first);
    X0matrix<<"row="<<sT3->second->probs->dim1()<<" column="<<sT3->second->probs->dim2()<<endl;
    for(int i=0;i<sT3->second->probs->dim1();i++)
    {
      for(int j=0;j<sT3->second->probs->dim2();j++)
      {
          X0matrix<<(*sT3->second->probs)[i][j]<<" ";
      }
      X0matrix<<";";
      X0matrix<<endl;
    }
    X0matrix<<endl;
  }

  symList2symListTMap *XaoMap=getXao(LmRMap, LmTaoMap);//计算Xao
  ofstream Xaomatrix("Xaomatrix.txt");
  for(sLsT1 =XaoMap->begin();sLsT1 !=XaoMap->end();sLsT1 ++)
  {
    Xaomatrix<<"landmark="<<*(sLsT1->first)<<endl;
    for(sT1=sLsT1->second->begin();sT1 != sLsT1->second->end();sT1++)
    {
      Xaomatrix<<"ao="<<*(sT1->first)<<endl;
      Xaomatrix<<"row="<<sT1->second->probs->dim1()<<" column="<<sT1->second->probs->dim2()<<endl;
      for(int i=0;i<sT1->second->probs->dim1();i++)
      {
        for(int j=0;j<sT1->second->probs->dim2();j++)
        {
          Xaomatrix<<(*sT1->second->probs)[i][j]<<" ";
        }
        Xaomatrix<<";";
        Xaomatrix<<endl;
      }
      Xaomatrix<<endl;
    }
    Xaomatrix<<endl;
  }



  symList2VecMap *Lm_mao=getmao_Mao(landmarkSet);//读取由matlab求的每一个landmark对应的所有mao的值,并存入一个vector中

  symListVecMat *Lm_maoMap=getLm_maoMap(LmRMap, Lm_mao, AOset);//每一子状态空间中每一个ao对应的mao
  ofstream Lm_maomatrix("Lm_maomatrix.txt");
  for(Lm_m =Lm_maoMap->begin();Lm_m !=Lm_maoMap->end();Lm_m ++)
  {
    Lm_maomatrix<<"landmark="<<*(Lm_m->first)<<endl;
    for(viT=Lm_m->second->begin();viT != Lm_m->second->end();viT ++)
    {
      Lm_maomatrix<<"ao="<<*(viT->first)<<"    ";
      for(int j=0;j<viT->second->size();j++)
      {
          Lm_maomatrix<<viT->second->at(j)<<" ";
      }

      Lm_maomatrix<<endl;
    }
    Lm_maomatrix<<endl;
  }

  symList2VecMap *Lm_Mao=getmao_Mao(landmarkSet);//读取由matlab求的每一个landmark对应的所有Mao的值,并存入一个vector中
  ofstream Lm_MaoData("Lm_MaoData.txt");
  for(viT=Lm_Mao->begin();viT!=Lm_Mao->end();viT++)
  {
    Lm_MaoData<<"landmark="<<*(viT->first)<<endl;
    Lm_MaoData<<"size="<<viT->second->size()<<endl;
    for(int i=0;i<viT->second->size();i++)
    {
      Lm_MaoData<<viT->second->at(i)<<"  ";
    }
    Lm_MaoData<<endl;
    Lm_MaoData<<endl;
  }



  symList2symListTMap *Lm_MaoMap=getLm_MaoMap(LmRMap, Lm_Mao, AOset);//每一子状态空间中每一个ao对应的Mao
  ofstream Lm_Maomatrix("Lm_Maomatrix.txt");
  for(sLsT =Lm_MaoMap->begin();sLsT !=Lm_MaoMap->end();sLsT ++)
  {
    Lm_Maomatrix<<"landmark="<<*(sLsT->first)<<endl;
    for(sT=sLsT->second->begin();sT != sLsT->second->end();sT ++)
    {
      Lm_Maomatrix<<"ao="<<*(sT->first)<<endl;
      for(int i=0;i<sT->second->probs->dim1();i++)
      { 
        for(int j=0;j<sT->second->probs->dim1();j++)        
        {
          Lm_Maomatrix<<(*sT->second->probs)[i][j]<<" ";
        }
        Lm_Maomatrix<<endl;
      }
      Lm_Maomatrix<<endl;
    }
    Lm_Maomatrix<<endl;
  }


  delete conData;
   
  for(vi=divideData->begin();vi!=divideData->end();vi++)
  {
     delete vi->first;//该处内存对应数据已经copy到landmarkSet中了 
     vector<symList*>::iterator v2=vi->second->begin();
     for(v2=vi->second->begin();v2!=vi->second->end();v2++)
     {
      delete *v2;//释放每一串数据的存储内存
     }
     delete vi->second;
  }
  delete divideData;


  int Test_Data_YoN;
  cout<<"if create new data please input Test_Data_YoN=1 and donnot create new data please input Test_Data_YoN=2"<<endl;
  cout<<"Test_Data_YoN=";
  cin>>Test_Data_YoN;
  switch(Test_Data_YoN)
  {
    case 1:
    {
      int Test_Data;
      cout<<"please input the Test_Data=";
      cin>>Test_Data;
      //env->createTestdata(Test_Data);
      break;
    }
    case 2:
    {
      break;  
    }
  }
  //vector<Symbol*> *testData=env->get1wTest();//得到固定的检验--20130821
  //vector<Symbol*> *testData=env->getData(Length_TestData);
  cout<<"please input filename:/Testdataid.txt"<<endl;    
  vector<Symbol*> *testData=env->getconData( );

  int2SymListMap *startLMPos=getStartLMPos(testData,landmarkSet);//找到testData的第一个landmark,以此landmark作为10000步检验数据初始状态

  int currPos=startLMPos->begin()->first;//testData的第一个landmark的位置
  symList *startLandmark=startLMPos->begin()->second;//testData的第一个landmark

  vector<float> *startLMbelief=getStartLMbelief(startLandmark,landmarkbe);//得到startLandmark对应的信念状态

  symList2VecMap *LM_firstprobMap=getLM_firstprob( X0Map );//每个landmark对应子状态空间模型的学习参数firstprob 

  pca_LM_modelPara *startLM_modelPara=getLM_modelPara(Lm_maoMap, Lm_MaoMap, LM_firstprobMap, startLandmark);//该Landmark对应的子空间模型(包含maoMatrix,firstprob,MaoMatrix)
  ofstream startLM_modelParamatrix("startLM_modelParamatrix.txt");
  startLM_modelParamatrix<<"landmark="<<*startLandmark<<endl;
  for(int i=0;i<startLM_modelPara->firstprob->size();i++)
  {
    startLM_modelParamatrix<<startLM_modelPara->firstprob->at(i)<<" ";
  }
  startLM_modelParamatrix<<endl;

  for(viT =startLM_modelPara->maoMatrix->begin();viT  !=startLM_modelPara->maoMatrix->end();viT  ++)
  {
    startLM_modelParamatrix<<"ao="<<*(viT->first)<<" ";
    for(int i=0;i<viT->second->size();i++)
    {
      startLM_modelParamatrix<<viT->second->at(i)<<" ";
    }
    startLM_modelParamatrix<<endl;
  }
  startLM_modelParamatrix<<endl;

  for(sT =startLM_modelPara->MaoMatrix->begin();sT !=startLM_modelPara->MaoMatrix->end();sT ++)
  {
    startLM_modelParamatrix<<"ao="<<*(sT->first)<<endl;
    for(int i=0;i<sT->second->probs->dim1();i++)
    {
      for(int j=0;j<sT->second->probs->dim2();j++)
      {
        startLM_modelParamatrix<<(*sT->second->probs)[i][j]<<" ";
      }
      startLM_modelParamatrix<<endl;
    }
    startLM_modelParamatrix<<endl;
  }  

  double oneError=getError_oneStep(landmarkSet, testData, startLMbelief, startLMPos, startLM_modelPara, Lm_MaoMap, Lm_maoMap, LM_firstprobMap);
  cout<<"oneError="<<oneError<<endl;
  resultDataPCA<<"oneError="<<oneError<<endl;

double fourError=getError_fourStep(landmarkSet, testData, startLMbelief, startLMPos, startLM_modelPara, Lm_MaoMap, Lm_maoMap, LM_firstprobMap);
  cout<<"fourError="<<fourError<<endl;
  resultDataPCA<<"fourError="<<fourError<<endl;




}






/*************************************************************************************************
  函数功能：获取一步检验的PSR预测值与POMDP准确值的差值
  参数：  
landmarkSet——landmark的集合 
testData——测试数据 
startLMbelief——测試数据中第一个landmark对应的信念状态   
startLMPos——testData的第一个landmark及其在testData中所处的位置
startLM_modelPara——测試数据testData中第一个landmark对应的子空间模型参数(Mao,mao,firstprob) 
Lm_MaoMap--每一子状态空间中每一个ao对应的Mao
Lm_maoMap--每一子状态空间中每一个ao对应的mao
LM_firstprobMap--每个landmark对应子状态空间模型的学习参数firstprob 
  返回值：
  errSum：一步检验的PSR预测值与POMDP准确值的差值
/*************************************************************************************************/
double psr_pca::getError_oneStep(symSet* landmarkSet, vector<Symbol*> *testData, vector<float> *startLMbelief, int2SymListMap *startLMPos, pca_LM_modelPara *startLM_modelPara,  symList2symListTMap *Lm_MaoMap, symListVecMat *Lm_maoMap, symList2VecMap *LM_firstprobMap)
{   
  vector<float> *belief_start=new vector<float>;
  vector<float>::iterator be;
  for(be=startLMbelief->begin();be!=startLMbelief->end();be++)
  {
    belief_start->push_back(*be);
  }
  vector<float> *belief_end=new vector<float>;
  int size_T=testData->size();
  int lmSize = (*landmarkSet->begin())->size();          //所有 landmark 已定为长度是一致的

  vector<double> *first=startLM_modelPara->firstprob;
  symList2VecMap *maoMatrix=startLM_modelPara->maoMatrix;
  symList2TMap   *MaoMatrix=startLM_modelPara->MaoMatrix;

  int k=startLM_modelPara->maoMatrix->begin()->second->size();cout<<"maoMatrix----k="<<k<<endl;

  Array1D<double> *tempCoreValue = new Array1D<double>(k, 0.0);     //用于存放 X(t)的值
  Array1D<double> *coreValue = new Array1D<double>(k, 0.0);         //用于存放 下一状态X(t+1)的值,(*coreValue)[hao]=X(t+1)=X(t)Mao/X(t)mao


  for(i=0; i<k; i++)
  {(*tempCoreValue)[i] = (*coreValue)[i] = (*first)[i];}
  for(i=0; i<k; i++)
  {cout<<"(*tempCoreValue)[i]="<<(*tempCoreValue)[i]<<endl;}
  ofstream one_errorData("one_errorData.txt"); 
  sT=MaoMatrix->begin();cout<<"MaoMatrix----k="<<MaoMatrix->begin()->second->probs->dim1()<<endl;

  int currPos=startLMPos->begin()->first;                   //testData的第一个landmark的位置

  symList *startLandmark=startLMPos->begin()->second;       //testData的第一个landmark  

  double pomPre, psrPre, errSum; 
  errSum=0.0;

  int num=0;
  for(int posT=currPos+1;posT<size_T;posT+=2)               //获取一步检验的PSR预测值和POMDP准確值的差值
  {
    num++;

    symSet *nextAoSet = new symSet();//获取采取动作a所有观测值的一步检验
    symList *obslist = env->getObservations();
    symList *action=getMemory(testData, posT, lmSize);//取动作a(symList型)
    Symbol* actionToSend =action->s;     //获取动作a(Symbol型)      
    symList *currobs = obslist;                            //此处的初始化切记要加上，要不然下一次循环的时候 currobs 为 NULL
    while ((currobs != NULL) && (currobs->s != NULL)) 
    { 
      nextAoSet->insert(new symList(actionToSend, new symList(currobs->s)));//得到ao
      currobs = currobs->n;
    } // obs
    delete action;

    double   sum3=0.0;
    for(iCurr=nextAoSet->begin(); iCurr!=nextAoSet->end(); iCurr++)
    {
      psrPre = 0.0;
      for(int m=0; m<k; m++)                                             //得到 psr 模型的预测值
      {
        psrPre += ((*coreValue)[m] * (maoMatrix->find(*iCurr)->second->at(m))); 
      }
      if(psrPre>1.0)psrPre=1.0;
      if(psrPre<0.0)psrPre=0.0;       
      sum3=sum3+psrPre;
    }
      
    for(iCurr=nextAoSet->begin(); iCurr!=nextAoSet->end(); iCurr++)   //下面开始获取当前 PSR 模型和POMDP精确模型预测之间的差值;针对的是所有的观测值,故此处要加一个 for 语句
    {
      pomPre = 0.0;
      pomPre=env->getPomdpProb(belief_start, *iCurr);                //得到当前 pomdp 模型的预测值即真值
      psrPre = 0.0;
      for(int m=0; m<k; m++)                              
      {
        psrPre += ((*coreValue)[m] * (maoMatrix->find(*iCurr)->second->at(m))); //得到 psr 模型的预测值
      }
      if(psrPre>1.0)psrPre=1.0;
      if(psrPre<0.0)psrPre=0.0;

      if(sum3!=0.0)
      {
        psrPre=psrPre/sum3;
      }
      if(sum3==0.0)
      {
        psrPre=1.0/env->nrObservs();
      }

      if(psrPre != pomPre)  //表示预测的不相同
      errSum += pow( (psrPre-pomPre), 2 );
      cout<<"test="<<**iCurr<<" psrPre="<<psrPre<<"    pomPre="<<pomPre<<"    errSum="<<errSum<<endl;
      one_errorData<<"test="<<**iCurr<<" psrPre="<<psrPre<<"    pomPre="<<pomPre<<"    errSum="<<errSum<<endl;
    }
    one_errorData<<endl;
    for(iCurr=nextAoSet->begin(); iCurr!=nextAoSet->end(); iCurr++)
    {
      delete *iCurr;
    }
    delete nextAoSet;
    cout<<endl;
      
    symList *newAO=getHistory(testData,posT+1,1);            //经历进行一步扩展h到hao,根据采取的动作值a和得到的观测值o更新POMDP状态和PSR状态
    vector<Symbol*>* testData1=env->getVecdata(newAO);       //testData1,经历经过一步ao,用于更新 POMDP
    symList *currMemory=getMemory(testData1, 1, lmSize);     //取观测值o(symList型)

    if(landmarkSet->find(currMemory) != landmarkSet->end())  //说明当前的memory为landmark,更新PSR状态(从X(t),经过一步newAO,到X(t+1)),(*coreValue)[newAO]=X(t+1)=X(t)Mao(newAO)/X(t)m(newAO))
    {
      symList *Landmark=new symList(currMemory);cout<<"Landmark="<<*Landmark<<endl;
        
      pca_LM_modelPara *Landmark_para=getLM_modelPara(Lm_maoMap, Lm_MaoMap, LM_firstprobMap, Landmark);   //Landmark对应的landmark的子空间的模型参数,重新给first,maoMatrix,MaoMatrix赋值 
      maoMatrix=Landmark_para->maoMatrix;
      MaoMatrix=Landmark_para->MaoMatrix;
      first=Landmark_para->firstprob;
      k=maoMatrix->begin()->second->size();cout<<"maoMatrix----k="<<k<<endl;
         //viH=MaoMatrix->begin();cout<<"maoqMatrix----k="<<maoqMatrix->begin()->second->size()<<endl;
      sT=MaoMatrix->begin();

      delete tempCoreValue;
      delete coreValue;
      tempCoreValue = new Array1D<double>(k, 0.0);      
      coreValue = new Array1D<double>(k, 0.0);
      for(int m1=0; m1<k; m1++) (*tempCoreValue)[m1] = (*coreValue)[m1] = (*first)[m1];

      delete Landmark;
    }
    delete currMemory;

    double denominator = 0;
    for(int n=0; n<k; n++)
    {
      denominator += ((*tempCoreValue)[n] * (maoMatrix->find(newAO)->second->at(n)));  //X(t)m(newAO)
    }
      
    delete coreValue;
    coreValue = new Array1D<double>(k, 0.0);
    if(denominator == 0)       //除数为0,此处要加点预防措施
    {
      for(int m=0; m<k; m++)
      (*coreValue)[m] = 0.0;
    }
    else
    {
      tEstimateT *newAO_Mao=new tEstimateT;
      newAO_Mao->probs=new Array2D<double>(k,k);//存储该landmark中newAO对应的Mao
      for(sT=MaoMatrix->begin();sT !=MaoMatrix->end();sT ++)
      {
        if(*(sT->first)==*newAO)
        {
          for(int i1=0;i1<sT->second->probs->dim1();i1 ++)
          {
            for(int j1=0;j1<sT->second->probs->dim2();j1 ++)
            {
              (*newAO_Mao->probs)[i1][j1]=(*sT->second->probs)[i1][j1];
            }
          }
          break;
        }
      }

      for(int j=0; j<k; j++)
      {
        double numerator=0;
	for(int n=0; n<k; n++)
        {
          numerator += (*tempCoreValue)[n] * (*newAO_Mao->probs)[j][n];//X(t)Mao(newAO)
        }
        (*coreValue)[j] = (double)(numerator/denominator);  //X(t+1)=X(t)Mao(newAO)/X(t)m(newAO)
         if( (*coreValue)[j]>1) (*coreValue)[j] = 1;            
	 if( (*coreValue)[j]<0)(*coreValue)[j] = 0;	      
      }
    }
    delete newAO;

    delete tempCoreValue;
    tempCoreValue = new Array1D<double>(k, 0.0);  
    for(int m3=0; m3<k; m3++)
    {(*tempCoreValue)[m3] = (*coreValue)[m3];}              //经历经过一步ao,更新(*tempCoreValue)[i]
     
    belief_end=env->getBeliefState(belief_start,testData1);//更新POMDP信念状态,此时的belief作为下面要获取的数据的初始beliefstate
    delete belief_start;
    belief_start=new vector<float>;
    for(be=belief_end->begin();be!=belief_end->end();be++)
    { belief_start->push_back(*be);}
    delete belief_end;

    delete testData1;
    if(num==10000)break;
  }
  errSum /= (env->nrObservs() *num);                       //其中 nrObservs 为观测值的数量,num为检验数据的长度(从第一个landmark开始算起)
  cout<<"一步检验误差率Result data is "<<errSum<<endl;
  delete coreValue;
  delete tempCoreValue;
  delete belief_start;

  return errSum;
}






/*************************************************************************************************
  函数功能：获取四步检验的PSR预测值与POMDP准确值的差值（CZZ）
  输入参数：  
landmarkSet——landmark的集合 
testData——测试数据 
startLMbelief——测試数据中第一个landmark对应的信念状态   
startLMPos——testData的第一个landmark及其在testData中所处的位置
startLM_modelPara——测試数据testData中第一个landmark对应的子空间模型参数(Mao,mao,firstprob) 
Lm_MaoMap--每一子状态空间中每一个ao对应的Mao
Lm_maoMap--每一子状态空间中每一个ao对应的mao
LM_firstprobMap--每个landmark对应子状态空间模型的学习参数firstprob 
  返回值：
  四步检验的PSR预测值与POMDP准确值的差值.
/*************************************************************************************************/
double psr_pca::getError_fourStep(symSet* landmarkSet, vector<Symbol*> *testData, vector<float> *startLMbelief, int2SymListMap *startLMPos, pca_LM_modelPara *startLM_modelPara,  symList2symListTMap *Lm_MaoMap, symListVecMat *Lm_maoMap, symList2VecMap *LM_firstprobMap)
{
  double pomPre, psrPre, errSum=0.0; 
  vector<float> *belief_start=new vector<float>;
  vector<float>::iterator be;
  for(be=startLMbelief->begin();be!=startLMbelief->end();be++)   //初始化到startLandmark对应的状态----29130821czz
  { belief_start->push_back(*be);}

  symList2VecMap *maoMatrix=startLM_modelPara->maoMatrix;       //得到检验数据中第一个landmark--startLandmark对应的子空间模型参数maoMatrix
  symList2TMap   *MaoMatrix=startLM_modelPara->MaoMatrix;     //得到检验数据中第一个landmark--startLandmark对应的子空间模型参数MaoMatrix
  vector<double> *first=startLM_modelPara->firstprob;           //得到检验数据中第一个landmark--startLandmark对应的子空间模型参数first
  int k=maoMatrix->begin()->second->size();cout<<"maoMatrix----k="<<k<<endl;
  Array1D<double> *tempCoreValue = new Array1D<double>(k, 0.0);  //用于存放 p(Q|h)的值
  Array1D<double> *coreValue = new Array1D<double>(k, 0.0);      //用于存放 p(Q|h)的值,(*coreValue)[hao]=P(Q|hao)=P(Q|h)Maoq(hao)/P(Q|h)m(hao)
  for(i=0; i<k; i++)
  {(*tempCoreValue)[i] = (*coreValue)[i] = (*first)[i];}
  sT=MaoMatrix->begin();cout<<"MaoMatrix----k="<<MaoMatrix->begin()->second->probs->dim1()<<endl;


  int currPos=startLMPos->begin()->first;             //testData的第一个landmark的位置
  symList *startLandmark=startLMPos->begin()->second; //testData的第一个landmark
  int size_T=testData->size();                                   //testData的长度
  ofstream four_errorData("four_errorData.txt"); 
  vector<float> *belief_end=new vector<float>;
  int num=0;  
  for(int posT=currPos+2;posT<size_T-8;posT+=2)                    //获取四步检验的PSR预测值和POMDP准確值的差值
  { 
    num++;//累加检验的步數
    symList *Test4ao=getHistory(testData,posT+6,4);cout<<"Test4ao="<<*Test4ao<<endl;//获取四步检验     
       
    pomPre = 0.0;
    pomPre=env->getPomdpProb(belief_start, Test4ao);          //POMDP精确值
    psrPre = 0.0;
              
    vector<Symbol*> *Test=env->getVecdata(Test4ao);//得到 psr 模型的预测值,判断t=(Test4ao)中有无landmark,如果有则p(t|h)=p(t1t2|h)=p(t1|h)*p(t2|ht2),psrPre += ((*coreValue)[m] * (maoMatrix->find(Test4ao)->second->at(m)));
    int len=Test->size();
    symList *Test1=NULL;symList *Test2=NULL;                //new symList();
    for(int pos=1;pos<len;pos+=2)
    {
      symList *mark=getMemory(Test, pos, 1);               //cout<<"mark="<<*mark<<endl;
      if(landmarkSet->find(mark) != landmarkSet->end())    //如果Test中的一个观测值为landmark,则以landmark为划分点把Test划分为两部分
      {  cout<<"pos="<<pos<<endl;
         Test1=getHistory(Test,pos,(pos+1)/2);             //取landmark前对应的一部分检验
         Test2=getTest(Test,pos,(len-pos-1)/2);            //取landmark后对应的一部分检验
         break;
      }
      delete mark;
    }
    Array1D<double> *tempTest1Value=new Array1D<double>(k, 0.0);   //存放p(Test|h)=p(Q|h)M(a1o1)M(a2o2)M(a3o3)m(a4o4)中p(Q|h)M(a1o1)M(a2o2)M(a3o3)的值
    Array1D<double> *tempValue1=new Array1D<double>(k, 0.0);       //(*coreValue)要保持不变,用于复制(*coreValue)
    for(int m1=0; m1<k; m1++)
    {    
      (*tempTest1Value)[m1] = (*tempValue1)[m1]=(*coreValue)[m1];//此时经历h对应的p(Q|h)
    }

    if(Test2==NULL)                       //说明Test2=new symList(),为空,即Test=Test4ao中不含landmark,Test没有被分隔成两部份
    {
      for(int pos1=1;pos1<len-2;pos1+=2)//依次取ao,ao,ao,进而得到M(ao),M(ao),M(ao)
      {
        symList* test=getHistory(Test,pos1,1);
        tEstimateT *test_Mao=new tEstimateT;
        test_Mao->probs=new Array2D<double>(k,k);//存储该landmark中newAO对应的Mao
        for(sT=MaoMatrix->begin();sT !=MaoMatrix->end();sT ++)
        {
          if(*(sT->first)==*test)
          {
            for(int i1=0;i1<sT->second->probs->dim1();i1 ++)
            {
              for(int j1=0;j1<sT->second->probs->dim2();j1 ++)
              {
                (*test_Mao->probs)[i1][j1]=(*sT->second->probs)[i1][j1];
              }
            }
            break;
          }
        }
        for(int j=0; j<k; j++)
        {
          double numerator=0;
	  for(int n=0; n<k; n++)
          {
            numerator += (*tempTest1Value)[n] * (*test_Mao->probs)[j][n];//X(t)Mao(newAO)
          }
          (*tempValue1)[j] = (double)(numerator);
          if( (*tempValue1)[j]>1.0) (*tempValue1)[j]=1.0;
          if( (*tempValue1)[j]<0.0) (*tempValue1)[j]=0.0;   
        }
        for(int n=0; n<k; n++)
        {
          (*tempTest1Value)[n]=(*tempValue1)[n];  //把(*tempValue1)[m]的值赋给(*tempTestValue)[m],用于上面循环的下一次(*tempTestValue)[m]
        }
        delete test;
      }//已经得到P(Q|h)M(ao)M(ao)M(ao)的值,存于(*tempTestValue)[m]中,此时(*tempTestValue)[m]=(*tempValue1)[m]
           
      symList *test1=getHistory(Test,len-1,1);      //得到最后一步ao
      for(int m=0; m<k; m++)                           //得到 psr 模型的预测值
      {
        psrPre += ((*tempValue1)[m] * (maoMatrix->find(test1)->second->at(m)));//p(Test|h)=p(Q|h)M(a1o1)M(a2o2)M(a3o3)m(a4o4)
      }
      if(psrPre>1.0)psrPre=1.0/env->nrObservs();
      if(psrPre<0.0)psrPre=0.0;
      delete test1;
    }
    else                                                //即Test=Test4ao中含landmark,Test被分隔成两部份Test1,Test2
    {  
      double psrPre1=0.0,psrPre2=0.0;
          
      if((Test1->size())>2)//计算psrPre1=p(Test1|h),分两种情况:(1)(Test1->size())>2,p(Test1|h)=p(Q|h)M(ao)...m(ao);(2)(Test1->size())==2,p(Test1|h)=p(Q|h)m(ao)p(Test1|h)=p(Q|h)M(ao)...m(ao)
      {
        for(int pos1=1;pos1<(Test1->size())-2;pos1+=2)//依次取ao...,进而得到M(ao)...
        {
          symList* test=getHistoryL(Test1,pos1,1);   //cout<<"test="<<*test<<endl;
          tEstimateT *test_Mao=new tEstimateT;
          test_Mao->probs=new Array2D<double>(k,k);//存储该landmark中newAO对应的Mao
          for(sT=MaoMatrix->begin();sT !=MaoMatrix->end();sT ++)
          {
            if(*(sT->first)==*test)
            {
              for(int i1=0;i1<sT->second->probs->dim1();i1 ++)
              {
                for(int j1=0;j1<sT->second->probs->dim2();j1 ++)
                {
                  (*test_Mao->probs)[i1][j1]=(*sT->second->probs)[i1][j1];
                }
              }
              break;
            }
          }
          for(int j=0; j<k; j++)
          {
            double numerator=0;
	    for(int n=0; n<k; n++)
            {
              numerator += (*tempTest1Value)[n] * (*test_Mao->probs)[j][n];//X(t)Mao(newAO)
            }
            (*tempValue1)[j] = (double)(numerator);
            if( (*tempValue1)[j]>1.0) (*tempValue1)[j]=1.0;
            if( (*tempValue1)[j]<0.0) (*tempValue1)[j]=0.0;   
          }

          delete test;  
          for(int n=0; n<k; n++)
          {
            (*tempTest1Value)[n]=(*tempValue1)[n];//把(*coreValue)[m]的值赋给(*tempTestValue)[m],用于上面循环的下一次(*tempTestValue)[m]
          }
        }//已经得到P(Q|h)M(ao)M(ao)M(ao)的值,存于(*tempTestValue)[m]中,此时(*tempTestValue)[m]=(*coreValue)[m]
           
        symList* test1=getHistory(Test,len-1,1);   //得到最后一步ao
        for(int m=0; m<k; m++)                              
        {
          psrPre1 += ((*tempValue1)[m] * (maoMatrix->find(test1)->second->at(m)));//p(Test|h)=p(Q|h)M(a1o1)M(a2o2)M(a3o3)m(a4o4)
        }
        if(psrPre1>1.0)psrPre1=1.0;
        if(psrPre1<0.0)psrPre1=0.0;
        delete test1;
      }
      else//(2)(Test1->size())==2,p(Test1|h)=p(Q|h)m(ao)
      {  
        for(int m=0; m<k; m++)                              
        { 
          psrPre1 += ((*tempValue1)[m] * (maoMatrix->find(Test1)->second->at(m)));//得到 psr 模型的预测值
        }//p(Test1|h)=p(Q|h)m(ao)
        if(psrPre1>1.0)psrPre1=1.0;
        if(psrPre1<0.0)psrPre1=0.0;
      }
          
      vector<int> v1;
      for(int pos2=1;pos2<(Test4ao->size())-2;pos2=pos2+2) //计算p(Test2|hTest1),还需判断Test2中有无landmark----pos2所在位置为Test2观测值,除去最后一个观測值
      { 
        symList* mark2=getMemoryL(Test4ao, pos2, 1);      // pos2对应观测值
        if(landmarkSet->find(mark2) != landmarkSet->end())//判断观测值是否为landmark
        v1.push_back(pos2);                               //将landmark所在位置依次存入容器中
        delete mark2;
      }
      v1.push_back((Test4ao->size())-1);//將最后一个观测值所在位置放入
      symList2symListVec *v2=new symList2symListVec;//存储landmark,及这一landmark开头后一landmark结尾的数据
      for(int i=0;i<v1.size()-1; i++)
      {
        int j=v1.at(i);int k=v1.at(++i);             
        symList* landmark=getMemoryL(Test4ao, j, 1);    //j对应的观测值是landmark
        symList* test=getTestL(Test4ao,j,(k-j)/2);      //取两个landmark之间的一部分检验或是最后一个landmark后的部分检验
        if(test->size()>1)                              //test不为空时存入下面map中
        {
          symList2ListMap* lmTest=new symList2ListMap;
          lmTest->insert(pair<symList*, symList*>(landmark,test));//把landmark及对应的test存入
          v2->push_back(lmTest);
        }
        i--;  
      } 
          
      double denom2=1.0;
      vector<symList2ListMap*>::iterator iter2=v2->begin();
      for(iter2=v2->begin();iter2!=v2->end(); iter2++)//如果Test2中有landmark,则再对其划分,每一组数据即对应landmark和相应的test,存储于v2中
      {
        map<symList*, symList*>::iterator lt=(*iter2)->begin();
        pca_LM_modelPara *Landmark4_para=getLM_modelPara(Lm_maoMap, Lm_MaoMap, LM_firstprobMap, lt->first);   //Landmark对应的landmark的子空间的模型参数,重新给first,maoMatrix,MaoMatrix赋值
        symList2VecMap   *mao4Matrix=Landmark4_para->maoMatrix;
        symList2TMap     *Mao4Matrix=Landmark4_para->MaoMatrix;
        vector<double>   *firstProb4=Landmark4_para->firstprob;
        sT4=Mao4Matrix->begin();
 
        int k4=mao4Matrix->begin()->second->size();cout<<"mao4Matrix----k4="<<k4<<endl;
        Array1D<double> *tempTest2Value=new Array1D<double>(k4, 0.0); 
        Array1D<double> *tempValue2=new Array1D<double>(k4, 0.0);
        for(int m1=0; m1<k4; m1++)
        {  
          (*tempTest2Value)[m1] = (*tempValue2)[m1]=(*firstProb4)[m1];               //此时经历h对应的p(Q|h)
        }
        if((lt->second)->size()>2)
        {
          for(int pos1=1;pos1<((lt->second)->size())-2;pos1+=2)                       //依次取ao...,进而得到M(ao)...
          {
            symList* test=getHistoryL((lt->second),pos1,1);
            tEstimateT *test_Mao=new tEstimateT;
            test_Mao->probs=new Array2D<double>(k4,k4);//存储该landmark中newAO对应的Mao
            for(sT4=Mao4Matrix->begin();sT4 !=Mao4Matrix->end();sT4 ++)
            {
              if(*(sT4->first)==*test)
              {
                for(int i1=0;i1<sT4->second->probs->dim1();i1 ++)
                {
                  for(int j1=0;j1<sT4->second->probs->dim2();j1 ++)
                  {
                    (*test_Mao->probs)[i1][j1]=(*sT4->second->probs)[i1][j1];
                  }
                }
                break;
              }
            }
            for(int j=0; j<k4; j++)
            {
              double numerator=0;
	      for(int n=0; n<k4; n++)
              {
                numerator += (*tempCoreValue)[n] * (*test_Mao->probs)[j][n];//X(t)Mao(newAO)
              }
              (*tempValue2)[j] = (double)(numerator);
              if( (*tempValue2)[j]>1.0)(*tempValue2)[j]=1.0;                      
	      if( (*tempValue2)[j]<0.0)(*tempValue2)[j]=0.0;
            }
            for(int n=0; n<k4; n++)
            {
              (*tempTest2Value)[n]=(*tempValue2)[n];//把(*tempValue2)[m]的值赋给(*tempTestValue)[m],用于上面循环的下一次(*tempTestValue)[m]
            }
            delete test;
          }//已经得到P(Q|空经历)M(ao)M(ao)M(ao)的值,存于(*tempTest2Value)[m]中,此时(*tempTest2Value)[m]=(*tempValue2)[m]
          symList* test1=getHistory(Test,len-1,1);//得到最后一步ao
          for(int m=0; m<k4; m++) //得到 psr 模型的预测值
          {psrPre2 += ((*tempTest2Value)[m] * (mao4Matrix->find(test1)->second->at(m)));}//p((lt->second)|空经历)=p(Q|空经历)M(ao)...m(ao)
          if(psrPre2>1.0)psrPre2=1.0;
          if(psrPre2<0.0)psrPre2=0.0;
          delete test1;
        }
        else
        { 
          for(int m=0; m<k4; m++)                              
          {psrPre2 += ((*firstProb4)[m] * (mao4Matrix->find(lt->second)->second->at(m)));}//p(Test2|hTest1)=p(Q|hTest1)m(ao)=p(Q|某个子模型空经历)m(ao)
          if(psrPre2>1.0)psrPre2=1.0;
          if(psrPre2<0.0)psrPre2=0.0;
        }

        denom2=denom2*psrPre2;//denom2=p(Test2|hTest1)=....p(Test3|hTest1Test2)*p(Test2|hTest1)

        delete tempValue2;
        delete tempTest2Value;
      }//上面求denom2=p(Test2|hTest1)=p(Q|hTest1)M(ao)..m(ao)

      for(iter2=v2->begin();iter2!=v2->end(); iter2++)//释放四步检验中的landmark存儲内存
      {
        map<symList*, symList*>::iterator lt=(*iter2)->begin();
        delete lt->first;
        delete lt->second;
        delete *iter2;
      }
      delete v2;

      psrPre=psrPre1*denom2;
    }
    delete Test1;
    delete Test2;
    delete Test;
    delete tempValue1;
    delete tempTest1Value;

    four_errorData<<"psrPre="<<psrPre<<"  pomPre="<<pomPre<<endl;

    if(psrPre != pomPre)  errSum += pow( (psrPre-pomPre), 2 );  //表示预测的不相同
    delete Test4ao;     
     
    newAO=getHistory(testData,posT,1);//经过一步经历,根据采取的动作值和得到的观测值更新POMDP状态和PSR状态
    vector<Symbol*> *testData1=env->getVecdata(newAO);//testData1,经历经过一步ao,用于更新 POMDP
    int lmSize=1;//只获取长度为1的记忆
    symList *currMemory = getMemory(testData1, 1, lmSize); cout<<"currMemory="<<*currMemory<<endl; //位置pos=1对应testData1的观测值      
      
    if(landmarkSet->find(currMemory) != landmarkSet->end())//说明当前的 memory 为 landmark,更新 POMDP 模型(从P(Q|AO)到P(Q|newAO),(*coreValue)[newAO]=P(Q|newAO)=P(Q|AO)Maoq(newAO)/P(Q|AO)m(newAO))20130623
    {
       symList *Landmark=new symList(currMemory);//得到检测数据testData中下一个landmark,可能和前一个landmark相同,也肯能不同,碰到landmark重新给(*first)[j],maoMatrix,maoqMatrix赋值
       pca_LM_modelPara *Landmark_para=getLM_modelPara(Lm_maoMap, Lm_MaoMap, LM_firstprobMap, Landmark);   //Landmark对应的landmark的子空间的模型参数,重新给first,maoMatrix,MaoMatrix赋值
       maoMatrix=Landmark_para->maoMatrix;
       MaoMatrix=Landmark_para->MaoMatrix;
       first=Landmark_para->firstprob;
       k=maoMatrix->begin()->second->size();cout<<"mao4Matrix----k="<<k<<endl;
       
       sT=MaoMatrix->begin();

       delete Landmark;
       delete tempCoreValue;
       delete coreValue;
       tempCoreValue = new Array1D<double>(k, 0.0);      
       coreValue = new Array1D<double>(k, 0.0);
       for(int m1=0; m1<k; m1++) (*tempCoreValue)[m1] = (*coreValue)[m1] = (*first)[m1];
     }
     delete currMemory;
    
     double denominator = 0.0;
     for(int n=0; n<k; n++)
     {denominator += ((*tempCoreValue)[n] * (maoMatrix->find(newAO)->second->at(n)));}//P(Q|h)m(newAO)
     cout<<"denominator="<<denominator<<endl;
     if(denominator == 0.0)   //如除数为0,则此处要加点预防措施
     {for(int m=0; m<k; m++)(*coreValue)[m] = 0.0;}
     else
     {
       tEstimateT *newAO_Mao=new tEstimateT;
       newAO_Mao->probs=new Array2D<double>(k,k);//存储该landmark中newAO对应的Mao
       for(sT=MaoMatrix->begin();sT !=MaoMatrix->end();sT ++)
       {
         if(*(sT->first)==*newAO)
         {
           for(int i1=0;i1<sT->second->probs->dim1();i1 ++)
           {
             for(int j1=0;j1<sT->second->probs->dim2();j1 ++)
             {
               (*newAO_Mao->probs)[i1][j1]=(*sT->second->probs)[i1][j1];
             }
           }
           break;
         }
       }
       for(int j=0; j<k; j++)
       {
         double numerator=0;
	 for(int n=0; n<k; n++)
         {
           numerator += (*tempCoreValue)[n] * (*newAO_Mao->probs)[j][n];//X(t)Mao(newAO)
         }
         (*coreValue)[j] = (double)(numerator/denominator);  //X(t+1)=X(t)Mao(newAO)/X(t)m(newAO)
         if( (*coreValue)[j]>1) (*coreValue)[j] = 1;            
	 if( (*coreValue)[j]<0)(*coreValue)[j] = 0;	      
       }
    }
    delete newAO;
    for(int m3=0; m3<k; m3++)
    {
        (*tempCoreValue)[m3] = (*coreValue)[m3];
        cout<<"(*tempCoreValue)[m3]3333="<<(*tempCoreValue)[m3]<<endl;
    }        
    belief_end=env->getBeliefState(belief_start,testData1);//更新POMDP信念状态,此时的belief作为下面要获取的数据的初始beliefstate
    delete belief_start;
    belief_start=new vector<float>;
    for(be=belief_end->begin();be!=belief_end->end();be++)
    { belief_start->push_back(*be);}
    delete belief_end;
    delete testData1;
    if(num==10000)break;
  }
  errSum /= num;  //num为实際上测试数据的长度
  delete coreValue;
  delete tempCoreValue;
  delete belief_start;
  return errSum;
}












/*************************************************************************************************
 函数功能：在整个空间模型whole_modelMatrix中找到指定Landmark对应的子空间模型参数LM_modelPara(包括mao,Mao,firstprob)
 输入参数:
 Lm_maoMap--每一子状态空间中每一个ao对应的mao
 Lm_MaoMap--每一子状态空间中每一个ao对应的Mao
 LM_firstprobMap--每个landmark对应子状态空间模型的学习参数firstprob 
 Landmark--指定的landmark
 输出:
 toret--该Landmark对应的子空间模型(包含maoMatrix,firstprob,MaoMatrix)
/*************************************************************************************************/
pca_LM_modelPara *psr_pca::getLM_modelPara(symListVecMat *Lm_maoMap,   symList2symListTMap *Lm_MaoMap, symList2VecMap *LM_firstprobMap, symList *Landmark)
{
  pca_LM_modelPara *toret=new pca_LM_modelPara;  //存储Landmark对应子状态空间模型的各个学习参数   
  toret->maoMatrix=NULL;         //Landmark对应状态的初始学习模型参数mao
  for(Lm_m =Lm_maoMap->begin();Lm_m !=Lm_maoMap->end();Lm_m ++)
  {
    if(*Landmark==*(Lm_m->first))
    {
      cout<<"startLandmark="<<*(Lm_m->first)<<endl;
      toret->maoMatrix=(Lm_m->second);
      break;
    }
  }
  int k=toret->maoMatrix->begin()->second->size();cout<<"maoMatrix----k="<<k<<endl;

  toret->firstprob=NULL;       //Landmark对应状态的初始学习模型参数core-test,存放初始状态的 core-test 的值,
  for(viT=LM_firstprobMap->begin();viT!=LM_firstprobMap->end();viT++)
  {
    cout<<"landmark="<<*(viT->first)<<endl;
    if(*Landmark==*(viT->first))
    {
      toret->firstprob=(viT->second);
      break; 
    }
  }
  
   toret->MaoMatrix=NULL;    //Landmark对应状态的初始学习模型参数MaoMatrix
  for(sLsT =Lm_MaoMap->begin();sLsT !=Lm_MaoMap->end();sLsT ++)
  {
    cout<<"landmarkq="<<*(sLsT->first)<<endl;
    if(*Landmark==*(sLsT->first))
    {
       toret->MaoMatrix=(sLsT->second);
       break;
    }
  }
  return toret;
}



/*************************************************************************************************
 函数功能：每个landmark对应子状态空间模型的学习参数firstprob 
 输入参数:
 X0Map--每一子状态空间的X0矩阵
 输出:
 LM_firstprob--每个landmark对应子状态空间模型的学习参数firstprob 
/*************************************************************************************************/
symList2VecMap *psr_pca::getLM_firstprob(   symList2TMap *X0Map )
{
  symList2VecMap *LM_firstprob=new symList2VecMap;  //存储每个landmark对应子状态空间模型的学习参数firstprob   
  for(sT=X0Map->begin();sT != X0Map->end();sT++)
  {
    vec *firstprob=new vec;
    for(int j=0;j<sT->second->probs->dim2();j++)
    {
      firstprob->push_back( (*sT->second->probs)[0][j] );//取每一个X0中第一行作为firstprob 
    }
    
    LM_firstprob->insert(pair<symList*,vec*>(new symList(sT->first),firstprob ));
  }
  return LM_firstprob;
}





/*********************************************************************************************
函数功能：每一子状态空间中每一个ao对应的Mao
输入参数：
LmRMap--每一子状态空间的R矩阵
Lm_Mao--每一子状态空间的所有Mao的值
AOset--所有ao对
输出：
Lm_MaoMap--每一子状态空间中每一个ao对应的Mao
**********************************************************************************************/
symList2symListTMap *psr_pca::getLm_MaoMap(symList2TMap *LmRMap, symList2VecMap *Lm_Mao, symSet *AOset)
{  
  symList2symListTMap *Lm_MaoMap=new symList2symListTMap; //为Lm_MaoMap矩阵分配内存,LM-ao-Mao
  for(sT=LmRMap->begin(),viT =Lm_Mao->begin();sT != LmRMap->end(),viT !=Lm_Mao->end();sT++, viT++)
  {
    symList2TMap *MaoMap=new symList2TMap;//ao-Mao
    int pos=0;
    int k=sT->second->probs->dim2();     
    for(iCurr1=AOset->begin();iCurr1 !=AOset->end();iCurr1 ++)
    {
      tEstimateT *Mao=new tEstimateT;//为每一个Mao分配内存
      Mao->probs=new Array2D<double>(k, k);  *(Mao->probs) = 0.0;   
      for(int i=0;i<k;i++)
      {
        for(int j=0;j<k;j++)//每次从Lm_Mao取k X k(k=sT->second->probs->dim2())个值给Mao
        {
          double prob=viT->second->at(pos);
          (*Mao->probs)[i][j]=prob;
          pos++;
        }
      }
      MaoMap->insert(pair< symList*, tEstimateT* >(new symList(*iCurr1),Mao));//每一个ao对应一个Mao
    }

    Lm_MaoMap->insert(pair< symList*, symList2TMap* >(new symList(sT->first), MaoMap));//每一个landmark对应一个MaoMap
  }

  return Lm_MaoMap;
}







/*********************************************************************************************
函数功能：每一子状态空间中每一个ao对应的mao
输入参数：
LmRMap--每一子状态空间的R矩阵
Lm_mao--每一子状态空间的所有mao的值
AOset--所有ao对
输出：
Lm_maoMap--每一子状态空间中每一个ao对应的mao
**********************************************************************************************/
symListVecMat *psr_pca::getLm_maoMap(symList2TMap *LmRMap, symList2VecMap *Lm_mao, symSet *AOset)
{
  symListVecMat *Lm_maoMap=new symListVecMat; //为Lm_maoMap矩阵分配内存,LM-ao-mao
  for(sT=LmRMap->begin(),viT =Lm_mao->begin();sT != LmRMap->end(),viT !=Lm_mao->end();sT++, viT++)
  {
    symList2VecMap *maoMap=new symList2VecMap;//ao-mao
    int pos=0;
    for(iCurr1=AOset->begin();iCurr1 !=AOset->end();iCurr1 ++)
    {
      vec *mao=new vec;
      for(int j=0;j<sT->second->probs->dim2();j++)//每次从Lm_mao取k(k=sT->second->probs->dim2())个值给mao
      {
        double m=viT->second->at(pos);
        mao->push_back(m);
        pos++;
      }
      maoMap->insert(pair<symList*,vec* >(new symList(*iCurr1),mao));//每一个ao对应一个mao
    }

    Lm_maoMap->insert(pair<symList*, symList2VecMap*>(new symList(sT->first), maoMap));//每一个landmark对应一个maoMap
  }

  return Lm_maoMap;
}





/**********************************
函数功能:读取由matlab求的每一个landmark对应的所有mao或Mao的值,并存入一个vector中
输入：landmarkSet--每一子状态空间的landmark的集合
输出：Lm_mao_MaoMap--landmark对应的所有mao或Mao的值
********************************/
symList2VecMap *psr_pca::getmao_Mao(symSet* landmarkSet)
{
  symList2VecMap *Lm_mao_MaoMap=new symList2VecMap;
  for(iCurr=landmarkSet->begin();iCurr!=landmarkSet->end();iCurr++)
  {
    char name[80];
    cout<<"please cout landmark:/O?.txt or M?.txt"<<endl;
    cout<<"landmark="<<**iCurr<<endl;
    cout<<"please cout landmark of O or M:";
    cin >>name;                    //输入存储经历核对应的行号的txt文件或存储检验核对应的列号的txt文件
    cout<<name<<endl;
    char buffer[160];
    getcwd(buffer, 160);           
    char *workdir=strcat(buffer,name);  //读取文件绝对路徑
    cout<<workdir<<endl;  

    ifstream in(workdir);

    vec *mao_Mao =new vec;
    for(double t;in>>t;)
    {
      mao_Mao->push_back(t);            //将txt中的整数依次插入mao_Mao中
    }
    cout<<"mao_Mao->size="<<mao_Mao->size()<<endl;
    Lm_mao_MaoMap->insert(pair<symList*,vec*>(new symList(*iCurr),mao_Mao) );
  }
  return Lm_mao_MaoMap;
}






/*********************************************************************************************
函数功能：给定每一子状态空间的R矩阵和Tao,得到每一子状态空间的中每一个ao对应的Xao矩阵,Xao=Tao*R
输入参数：
LmRMap--每一子状态空间的R矩阵
LmTaoMap--每一子状态空间的Tao矩阵
输出：
XaoMap--每一子状态空间中每一个ao对应的Xao矩阵
**********************************************************************************************/
symList2symListTMap *psr_pca::getXao(symList2TMap *LmRMap, symList2symListTMap *LmTaoMap)
{
  symList2symListTMap *LMXaoMap=new symList2symListTMap; //为LMXaoMap矩阵分配内存,LM-ao-Xao
  for(sT=LmRMap->begin(),sLsT =LmTaoMap->begin();sT != LmRMap->end(),sLsT !=LmTaoMap->end();sT++,sLsT++)
  {
    symList2TMap *XaoMap=new symList2TMap;
    for(sT2=sLsT->second->begin();sT2 !=sLsT->second->end();sT2 ++)
    {
      tEstimateT *Xao=new tEstimateT;//为Xao矩阵分配内存

      int size_m=sT2->second->probs->dim1();
      int size_n=sT->second->probs->dim2();//cout<<"size_n="<<size_n<<endl;

      Xao->probs=new Array2D<double>(size_m, size_n);  *(Xao->probs) = 0.0;   
      for(int i=0;i<Xao->probs->dim1();i++)//Tao的行数
      {
        for(int j=0;j<sT->second->probs->dim2();j++)//R的列数
        {
          for(int k=0;k<sT2->second->probs->dim2();k++)//Tao的列数
          {
            (*Xao->probs)[i][j]+=(*sT2->second->probs)[i][k]*(*sT->second->probs)[k][j];    //计算Xao=Tao*R
          }
        }        
      }
      XaoMap->insert(pair<symList*, tEstimateT*>(new symList(sT2->first), Xao));//每一个ao对应一个Xao
    }

    LMXaoMap->insert(pair<symList*, symList2TMap*>(new symList(sT->first), XaoMap));//每一个landmark对应一个XaoMap
  }

  return LMXaoMap;
}







/*********************************************************************************************
函数功能：给定每一子状态空间的R矩阵和T0,得到每一子状态空间的X0矩阵,X0=T0*R
输入参数：
LmRMap--每一子状态空间的R矩阵
T0Map--每一子状态空间的T0矩阵
输出：
X0Map--每一子状态空间的X0矩阵
**********************************************************************************************/
symList2TMap *psr_pca::getX0(symList2TMap *LmRMap, symList2TMap *T0Map)
{
  symList2TMap *X0Map=new symList2TMap; 
  for(sT=LmRMap->begin(),sT1 =T0Map->begin();sT != LmRMap->end(),sT1 !=T0Map->end();sT++,sT1++)
  {
    tEstimateT *X0=new tEstimateT;//为X0矩阵分配内存

    int size_m=sT1->second->probs->dim1();
    int size_n=sT->second->probs->dim2();

    X0->probs=new Array2D<double>(size_m, size_n);  *(X0->probs) = 0.0;   
    for(int i=0;i<X0->probs->dim1();i++)//T0的行数
    {
      for(int j=0;j<sT->second->probs->dim2();j++)//R的列数
      {
        for(int k=0;k<sT1->second->probs->dim2();k++)//T0的列数
        {
          (*X0->probs)[i][j]+=(*sT1->second->probs)[i][k]*(*sT->second->probs)[k][j];    //计算X0=T0*R
        }
      }        
    }
    X0Map->insert(pair<symList*, tEstimateT*>(new symList(sT->first),X0));//每一个landmark对应一个X0
  }

  return X0Map;
}



/*********************************************************************************************
函数功能：每一子状态空间的状态的个数
输入参数：
landmarkSet--每一子状态空间的landmark的集合
输出：
NumerState--每一子状态空间的状态的个数
**********************************************************************************************/
symList2IntMap *psr_pca::getNumerState(symSet* landmarkSet)
{
  symList2IntMap *NumerState=new symList2IntMap;
  cout<<"please input filename:/environment-Num.txt"<<endl;                          //输入environment-Number of state文件名
  cout<<"please input filename:"; 
  char name[80];
  cin >>name;                       
  cout<<name<<endl;

  char buffer[160];
  getcwd(buffer, 160);          
  printf("The current directory is: %s\n", buffer);
  char *workdir=strcat(buffer,name);  
  cout<<workdir<<endl;  

  ifstream in(workdir);

  vector<int> numberState;
  for(int id;in>>id;)
  {
    numberState.push_back(id);
  }

  int pos=0;
  for(iCurr=landmarkSet->begin();iCurr!=landmarkSet->end();iCurr++)
  {
    int num=numberState.at(pos);
    NumerState->insert(pair<symList*, int*>(new symList(*iCurr),new int(num)));
    pos++;
  }
  return NumerState;
}


/*********************************************************************************************
函数功能：给定概率矩阵T,得到每一子状态空间的R矩阵
输入参数：
LmTmatrixMap--每一子状态空间的T0和所有的Tao叠加起来形成概率矩阵T
NumerState--每一子状态空间的状态个数
输出：
LmRMap--每一子状态空间的R矩阵
**********************************************************************************************/
symList2TMap *psr_pca::decompSVD(symList2TMap *LmTmatrixMap, symList2IntMap *NumerState)
{
  symList2TMap *LmRMap=new symList2TMap; 
  ofstream Vmatrix("Vmatrix.txt");
  int randORfix;
  cout<<"if choose random K please input the randORfix=1 and if choose fixed K please input the randORfix=2"<<endl;
  cout<<"please input the randORfix=";
  cin>>randORfix;
  Vmatrix<<"randORfix="<<randORfix<<endl;

  double effience;
  cout<<"please input the effience=";//累积贡献率
  cin>>effience;
  Vmatrix<<"effience="<<effience<<endl;

  for(sT=LmTmatrixMap->begin();sT != LmTmatrixMap->end();sT++)
  {
    Vmatrix<<"landmark="<<*(sT->first)<<endl;
    Vmatrix<<"T:"<<" row="<<sT->second->probs->dim1()<<" column="<<sT->second->probs->dim2()<<endl;

    singular = new SVD<double>(*sT->second->probs);          //利用svd分解矩阵T--[U,S,V]=svd(T),则有T=U*S*V'
    Array2D<double> *U=new Array2D<double>();
    singular->getU(*U);//得到U矩阵
    Vmatrix<<"U:"<<" row="<<U->dim1()<<" column="<<U->dim2()<<endl;

    Array2D<double> *V=new Array2D<double>();
    singular->getV(*V);//得到V矩阵 


    Vmatrix<<"V:"<<" row="<<V->dim1()<<" column="<<V->dim2()<<endl;
    /*for(int i=0;i<V->dim1();i++)
    {
      for(int j=0;j<V->dim2();j++)
      {
        Vmatrix<<(*V)[i][j]<<" ";
      }
      Vmatrix<<endl;
    } 
    Vmatrix<<endl;*/

    Array2D<double> *S=new Array2D<double>();
    singular->getS(*S);//得到S矩阵
    Vmatrix<<"S:"<<" row="<<S->dim1()<<" column="<<S->dim2()<<endl;
    for(int i=0;i<S->dim1();i++)
    {
      for(int j=0;j<S->dim2();j++)
      {
        Vmatrix<<(*S)[i][j]<<" ";
      }
      Vmatrix<<endl;
    } 
    Vmatrix<<endl;

    int k;
    switch(randORfix)
    {
      case 1:
      {
        for(iT=NumerState->begin(); iT!=NumerState->end(); iT++)
        {
          if(*(iT->first)== *(sT->first))
          {
            k=*(iT->second);
            break;
          }          
        }
        break;
      }
      case 2:
      {
        Array1D<double> *singvals = new Array1D<double>();
	singular->getSingularValues(*singvals);
        double total=0.0;
        for(int i=0;i<singvals->dim();i++)
        {
          total+=(*singvals)[i];
        }
        cout<<"total="<<total<<endl;
        double part=0.0;
        int k1=0;
        for(int i=0;i<singvals->dim();i++)
        {         
          k1 ++;      
          part+=(*singvals)[i];
          double rate=part/total ;
          if(rate>effience || rate==effience)        
          {
            break;
          }
        }

        //k=k1-1;//k=k1;??????????????????
        k=k1;
        break;  
      }
    }


    //int k=singular->rank(cutoffValue);
    cout<<"rank="<<k<<endl;

    tEstimateT *R=new tEstimateT;//为R矩阵分配内存,R是V的前K列

    //int size_m=sT->second->probs->dim1();
    int size_m=V->dim1();
    R->probs=new Array2D<double>(size_m, k);  *(R->probs) = 0.0; 
    Vmatrix<<"R->probs->dim1()="<<R->probs->dim1()<<" R->probs->dim2()="<<R->probs->dim2()<<endl; 
    /*if(size_m > U->dim1())
    {
      Vmatrix<<"size_m="<<size_m<<" U->dim1()="<<U->dim1()<<endl; 
      for(int i=0;i<size_m;i++)
      {
        for(int j=0;j<k;j++)
        {
          Vmatrix<<"i="<<i<<" j="<<j<<" (*V)[i][j]="<<(*V)[i][j]<<endl;
          (*R->probs)[i][j]=(*V)[i][j];//把V的前K列内容复制给R??????????????????????????????????20140321有问題
        }
      }
    }
    else
    {*/
      for(int i=0;i<V->dim1();i++)
      {
        for(int j=0;j<k;j++)
        {
          Vmatrix<<"i="<<i<<" j="<<j<<" (*V)[i][j]="<<(*V)[i][j]<<endl;
          (*R->probs)[i][j]=(*V)[i][j];//把V的前K列内容复制给R??????????????????????????????????20140321有问題
        }
      } 
    //}
    Vmatrix<<"R:"<<" row="<<R->probs->dim1()<<" column="<<R->probs->dim2()<<endl;

    LmRMap->insert(pair<symList*, tEstimateT*>(new symList(sT->first),R));//每一个landmark对应一个T
  }

  return LmRMap;
}




/*********************************************************************************************
函数功能：给定Tao,T0,得到T0和所有的Tao叠加起来形成概率矩阵T 
输入参数：
LmTaoMap——每一子状态空间的所有的Tao
T0Map--每一子状态空间的T0
size_ao--ao组合对个数
输出：
每一子状态空间的T0和所有的Tao叠加起来形成概率矩阵T 
**********************************************************************************************/
symList2TMap *psr_pca::getLmTmatrixMap(symList2symListTMap *LmTaoMap, symList2TMap *T0Map, int size_ao)
{
  symList2TMap *LmTmatrixMap=new symList2TMap;    
    
  for(sT=T0Map->begin();sT!=T0Map->end();sT++)
  {
    tEstimateT *T=new tEstimateT;//为T矩阵分配内存
    int size_mT0=sT->second->probs->dim1();
    int size_n=sT->second->probs->dim2();  //T的列数
    int size_mT=size_mT0+(size_mT0)*(size_ao);//T的行数
    T->probs=new Array2D<double>(size_mT, size_n);  *(T->probs) = 0.0;   
    for(int i=0;i<sT->second->probs->dim1();i++)
    {
      for(int j=0;j<size_n;j++)
      {
        (*T->probs)[i][j]=(*sT->second->probs)[i][j];//先把T0内容复制过来
      }
    } 

    LmTmatrixMap->insert(pair<symList*, tEstimateT*>(new symList(sT->first),T));//每一个landmark对应一个T
  }

  for(sLsT1=LmTaoMap->begin(),sT1=LmTmatrixMap->begin();sLsT1 !=LmTaoMap->end(),sT1 != LmTmatrixMap->end();sLsT1 ++,sT1++)
  {
    int row=0;
    for(sT=T0Map->begin();sT!=T0Map->end();sT++)
    {
      if(*(sT1->first)==*(sT->first))
      row=sT->second->probs->dim1();
      break;
    }
    
    int row1=-1;
    for(sT2=sLsT1->second->begin();sT2 !=sLsT1->second->end();sT2++)//针对每一个ao
    {      
      for(int i=0;i<sT2->second->probs->dim1();i++)
      {
        row1++;
        for(int j=0;j<sT2->second->probs->dim2();j++)
        {
          (*sT1->second->probs)[row+row1][j]=(*sT2->second->probs)[i][j];//把Tao内容复制过来
        }
      }
    }
  }

  return LmTmatrixMap;
}









/*********************************************************************************************
函数功能：每一子状态空间的概率矩阵p(Q|BAO),即每一子状态空间所有的Tao的集合
输入参数：
fillProbmatrix——p(ao|Q),p(Q|B),p(aoQ|B)及其相关计数概率矩阵;
Qtest--检验的集合Qtest
输出：
每一子状态空间的概率矩阵p(Q|BAO),即每一子状态空间所有的Tao的集合
**********************************************************************************************/
symList2symListTMap *psr_pca::getTMap(ProbMAP *fillProbmatrix, symSet *Qtest, symSet *AOset, symList2TMap *T0Map)
{
  symList2symListTMap *LmTaoMap=new symList2symListTMap;//存储Tao

  ofstream LmTaoMapData("LmTaoMapData.txt");//存储Qtest
  int size_n=Qtest->size();
  LmTaoMapData<<"size_n="<<size_n<<endl;

  for(s2Es=fillProbmatrix->QaoQProb->begin();s2Es!=fillProbmatrix->QaoQProb->end();s2Es++)//针对每一组子状态空间,即每一个landmark
  {
    LmTaoMapData<<"landmark="<<(*s2Es->first)<<endl;
    int size_m=s2Es->second->probs->dim1();
    LmTaoMapData<<"size_m="<<size_m<<" size_n="<<size_n<<endl;


    symList2TMap *TaoMap=new symList2TMap;//p(Q|BAO)
    for(iCurr=AOset->begin();iCurr!=AOset->end();iCurr++)//针对每一个ao
    {
      LmTaoMapData<<"ao="<<**iCurr<<endl;

      tEstimateT *Tao=new tEstimateT;
      Tao->probs=new Array2D<double>(size_m, size_n);  *(Tao->probs) = 0.0;

      symList *ao=new symList(*iCurr);
      tEstimateProb *ctTao=getTao(s2Es->second, Qtest, ao, T0Map);//取每一个p(Q|Bao)     
      
      for(piH=ctTao->ctProb->begin();piH!=ctTao->ctProb->end();piH++)
      {
        LmTaoMapData<<"his="<<*(piH->first)<<endl;
        for(piT=piH->second->begin();piT!=piH->second->end();piT++)
        {
          LmTaoMapData<<*(piT->second)<<" ";
        } 
        LmTaoMapData<<endl;
      }
      LmTaoMapData<<endl;
      


      for(int i=0;i<ctTao->probs->dim1();i++)
      {
        for(int j=0;j<ctTao->probs->dim2();j++)
        {
          LmTaoMapData<<(*ctTao->probs)[i][j]<<" ";
          (*Tao->probs)[i][j]=(*ctTao->probs)[i][j];
        }
        LmTaoMapData<<";"<<endl;
      }
      TaoMap->insert(pair<symList*, tEstimateT* >(ao,Tao));//ao-Tao
    }
    LmTaoMap->insert(pair<symList*, symList2TMap* >(new symList(s2Es->first),TaoMap));//landmark-ao-Tao
  }

  return LmTaoMap;
}








/*********************************************************************************************
函数功能：给定p(ao|Q),p(Q|B),p(aoQ|B)及其相关计数概率矩阵,得到单纯的p(Q|Bao),即Tao
输入参数：
fillProbmatrix——p(ao|Q),p(Q|B),p(aoQ|B)及其相关计数概率矩阵;
Qtest--检验的集合Qtest;
ao--B的一步扩展动作-观测对
T0Map--每一子状态空间的T0(p(Q|B))
输出：
概率矩阵p(Q|Bao),即Tao
**********************************************************************************************/
tEstimateProb *psr_pca::getTao(tEstimateData *matData, symSet *Qtest, symList* var1, symList2TMap *T0Map)
{
  tEstimateProb *Tao=new tEstimateProb;
  Tao->ctProb=new symListDblMat;

  ofstream augHistData("augHistData.txt");//存储augHist

  //symList *aug=new symList(var1);
  //augHistData<<"ao="<<*aug<<endl;

  symSet *baseHist=new symSet();//B的集合
  for(piH=matData->ctProb->begin();piH!=matData->ctProb->end();piH++)
  {
    baseHist->insert(new symList(piH->first));
  }

  for(iCurrH = baseHist->begin(); iCurrH != baseHist->end();iCurrH++)
  {
    augHistData<<"baseHist="<<**iCurrH<<endl;
  }
  augHistData<<endl;

  symSet *augHist=new symSet();//Bao的集合
  for(iCurrH = baseHist->begin(); iCurrH != baseHist->end();iCurrH++)
  {
    symList *aug1=new symList(*iCurrH);
    aug1->append(new symList(var1));
    if (augHist->find(aug1)==augHist->end())//若即将存入augHist中的检验aug与已存入augHist中的检验不相同
       augHist->insert(new symList(aug1));//在augHist中插入该检验
    else 
       delete aug1;
  }

  for(iCurrH = augHist->begin(); iCurrH != augHist->end();iCurrH++)
  {
    augHistData<<"augHist="<<**iCurrH<<endl;
  }

  for(iCurrH = augHist->begin(); iCurrH != augHist->end();iCurrH++)//填充p(Q|Bao)矩阵对应的行经历(bao)-列检验(q)
  {
    symList2DblMap *rowDblMap = new symList2DblMap();//开辟symList2DblMap型内存空间（由<symList*, double*>组成的映射）
    for (iCurr1 = Qtest->begin(); iCurr1 != Qtest->end(); iCurr1++)
    {
      rowDblMap->insert(pair<symList*, double*>(*iCurr1, new double(0.0)));//在rowDblMap中逐对插入由symList型检验和0.0组成的映射
    }
    Tao->ctProb->insert(pair<symList*, symList2DblMap*>(new symList(*iCurrH), rowDblMap));
  }
  
  int len=var1->size();
  symList *mark=getMemoryL(var1, len-1, 1);  //取var1(ao)的观测值       
  if(T0Map->find(mark) != T0Map->end() )//如果bao中的o是landmark,则该组Tao里的值全部为該landmark对应的T0中第一行元素值
  {
    for(sT=T0Map->begin();sT != T0Map->end();sT++)//针对每一组子状态空间,即每一个landmark
    {
      if(*(sT->first)==*mark)
      {
        for(piH1=Tao->ctProb->begin();piH1 !=Tao->ctProb->end();piH1++)
        { 
          int j;
          for(j=0, piT1=piH1->second->begin();piT1!=piH1->second->end();j++, piT1++)
          {
            *(piT1->second)= (*sT->second->probs)[0][j];
          }
        }
        break;
      }     
    }
  }
 
  else
  {
  for(piH=matData->ctProb->begin(),piH1=Tao->ctProb->begin();piH!=matData->ctProb->end(),piH1 !=Tao->ctProb->end();piH++,piH1++)//求每一个p(q|bao)=p(aoq|h)/p(ao|h)
  {
    double denominator=0.0,numerator=0.0;
    for(piT=piH->second->begin();piT!=piH->second->end();piT++)//求出每一行中p(ao|h)
    {
      if(*(var1)==*(piT->first))
      {
        denominator=*(piT->second);
        augHistData<<"p(ao|h)="<<denominator<<endl;
        break;
      }
    }

    for(piT1=piH1->second->begin();piT1!=piH1->second->end();piT1++)//对每一个q进行ao扩展
    {
      symList *aug1=new symList(piT1->first);//q
      aug1->append(new symList(var1));//aoq
      for(piT=piH->second->begin();piT!=piH->second->end();piT++)//查找p(aoq|h)
      {
        if(*(aug1)==*(piT->first))
        {
          numerator=*(piT->second);
          augHistData<<"p(aoq|h)="<<numerator<<endl;
          break;
        }
      }

      if(denominator!=0.0)//p(ao|h)!=0.0
      *(piT1->second)= double (numerator/denominator);

      if(*(piT1->second)>1.0)//p(q|hao)>1.0
      *(piT1->second)= 1.0;
    }

  }
  }

  int size_m=Tao->ctProb->size();
  piH=Tao->ctProb->begin();
  int size_n=piH->second->size(); 
  Tao->probs= new Array2D<double>(size_m, size_n);	*(Tao->probs) = 0.0;

  for(size_m = 0, piH = Tao->ctProb->begin();piH != Tao->ctProb->end(); size_m++, piH++)
  { 
    for(size_n= 0,  piT = piH->second->begin(); piT != piH->second->end(); size_n++, piT++)
    {
        (*Tao->probs)[size_m][size_n] = *(piT->second);//定义toret->probs中的元素为toret->ctProb中在经历下，对应的检验发生的概率
    }
  }

  return Tao;
}





/*********************************************************************************************
函数功能：得到所有动作-观测ao对的集合AO
输入参数：
无
输出：
得到所有动作-观测ao对的集合AO
**********************************************************************************************/
symSet *psr_pca::getAOset()
{ 
  symSet *AOset = new symSet();//得到所有动作-观测ao对的集合AO
  symList *actlist = env->getActions(), *obslist = env->getObservations();
  symList *curract,*currobs,*aoTest;
  curract = actlist;     //tests 中保存了所有一步的 ao        
  while ((curract != NULL) && (curract->s != NULL)) 
  { 
    currobs = obslist;     
    while ((currobs != NULL) && (currobs->s != NULL)) 
    { 
      symList *Test = new symList(curract->s, new symList(currobs->s));cout<<"Test="<<*Test<<endl;
      if(AOset->find(Test) == AOset->end())
	   AOset->insert(Test);
      else
	   delete aoTest;
      currobs = currobs->n;
    } 
    curract = curract->n;
  } 
  //上面tests保存了所有一步的 ao
  delete actlist;
 
  return AOset;
}











/*********************************************************************************************
函数功能：给定p(ao|Q),p(Q|B),p(aoQ|B)及其相关计数概率矩阵,得到单纯的p(Q|B),即T0
输入参数：
fillProbmatrix——p(ao|Q),p(Q|B),p(aoQ|B)及其相关计数概率矩阵;
Qtest--检验的集合Qtest
输出：
每一子状态空间的概率矩阵p(Q|B),即T0
**********************************************************************************************/
symList2TMap *psr_pca::getT0Map(ProbMAP *fillProbmatrix,symSet *Qtest)
{
  symList2TMap *T0Map= new symList2TMap; //存储LM-T0

  ofstream T0MapData("T0MapData.txt");//存储Qtest
  int size_n=Qtest->size();
  T0MapData<<"size_n="<<size_n<<endl;

  for(s2Es=fillProbmatrix->QaoQProb->begin();s2Es!=fillProbmatrix->QaoQProb->end();s2Es++)
  {
    T0MapData<<"landmark="<<(*s2Es->first)<<endl;
    int size_m=s2Es->second->probs->dim1();
    T0MapData<<"size_m="<<size_m<<" size_n="<<size_n<<endl;

    tEstimateT *T0=new tEstimateT;//T0
    T0->probs=new Array2D<double>(size_m, size_n);  *(T0->probs) = 0.0;
    for(int i=0;i<s2Es->second->probs->dim1();i++)
    {
      for(int j=0;j<size_n;j++)
      {
        (*T0->probs)[i][j]=(*s2Es->second->probs)[i][j];
        T0MapData<<(*T0->probs)[i][j]<<" ";
      }
      T0MapData<<";"<<endl;
    }
    T0Map->insert(pair<symList*, tEstimateT* >(new symList(s2Es->first),T0));//每一个landmark对应一个T0
  }
  return T0Map;
}



/*********************************************************************************************
函数功能：概率矩阵p(Q|B)中检验的集合Qtest
输入参数：
fillProbmatrix——p(ao|Q),p(Q|B),p(aoQ|B)及其相关计数概率矩阵
输出：
概率矩阵p(Q|B)中检验的集合Qtest
**********************************************************************************************/
symSet *psr_pca::getQtest(ProbMAP *fillProbmatrix,int step)
{
  symSet  *Qtest= new symSet(); 
  ofstream QtestData("QtestData.txt");//存储Qtest
  QtestData<<"step="<<step<<endl;
  for(piT=fillProbmatrix->QaoQProb->begin()->second->ctProb->begin()->second->begin();piT!=fillProbmatrix->QaoQProb->begin()->second->ctProb->begin()->second->end();piT++)
  {
  QtestData<<"test="<<(*piT->first)<<"size="<<piT->first->size()<<endl;
      int len=piT->first->size()/2;
      if(len<step||len==step)//Qtest中检验的长度最大为step
      {
        QtestData<<"Qtest="<<(*piT->first)<<endl;
        Qtest->insert(new symList(piT->first));
      }
      if(len>step)break;//Qtest中检验的长度大于step时,则此检验不在Qtest中
  }
  return Qtest;
}




/************************************************************
 函数功能：补充p(ao|Q),p(Q|B),p(aoQ|B)的缺值
 输入参数：
 landmarkbe--每个landmark对应的信念状态      
 Probmatrix--所有子状态空间对应的经历核--检验核的估计概率矩阵
 输出：补充缺值后的p(ao|Q),p(Q|B),p(aoQ|B)
/************************************************************/
ProbMAP *psr_pca::fillProb(symList2VfMap *landmarkbe,ProbMAP *Probmatrix)
{
  ProbMAP *toret=new ProbMAP;
  toret->aoProb=new symList2EstMap(); //存储补充缺值后估计概率矩阵p(ao|B)
  toret->QaoQProb=new symList2EstMap();//存储补充缺值后概率矩阵p(Q|B),p(aoQ|B)

   map<symList*,vector<float>*>::iterator lmb=landmarkbe->begin();   
   for(lmb=landmarkbe->begin();lmb!=landmarkbe->end();lmb++)
   {
     tEstimateData *P1=fillValue(lmb->first,Probmatrix->aoProb);//补充概率矩阵p(ao|B)缺值
     toret->aoProb->insert(pair<symList* ,tEstimateData*>(new symList(lmb->first),P1)); 
 
     tEstimateData *P2=fillValue(lmb->first,Probmatrix->QaoQProb);
     toret->QaoQProb->insert(pair<symList* ,tEstimateData*>(new symList(lmb->first),P2));//补充概率矩阵p(Q|B),p(aoQ|B)缺值
  }
  return toret;
}




/************************************************************
 函数功能：每个子状态空间对应的Cao集合
 输入参数：    
 fillProbmatrix--补充缺值后的p(ao|Q),p(Q|B),p(aoQ|B)
 输出：每个子状态空间对应的Cao集合
/************************************************************/
symList2DblMatMap *psr_pca::getCaoMap(ProbMAP *fillProbmatrix)
{
  symList2DblMatMap *CaoMap=new symList2DblMatMap;

  for(s2Es=fillProbmatrix->aoProb->begin();s2Es!=fillProbmatrix->aoProb->end();s2Es++)
  {
    symListDblMat *Cao=getCao(s2Es->second);//概率矩阵p(ao|Q)
    CaoMap->insert(pair<symList*, symListDblMat*>(new symList(s2Es->first),Cao));
  }

  return CaoMap;
}


/*********************************************************************************************
函数功能：给定p(ao|Q)及其相关计数概率矩阵,得到单纯的p(ao|Q)
输入参数：
Pao——p(ao|Q)及其相关计数概率矩阵
输出：
概率矩阵p(ao|Q)
**********************************************************************************************/
symListDblMat *psr_pca::getCao(tEstimateData *matData)
{
  symListDblMat	*Cao=new symListDblMat();

  symSet	*augHist= new symSet(); 
  for(piH=matData->ctProb->begin(); piH!=matData->ctProb->end(); piH++)//將matData->ctProb对应的经历和检验及概率复制给Cao
  {
    augHist->insert(new symList(piH->first));//为经历分配的内存,Cao对应的经历是这一块,
  }
  symSet	*augTest= new symSet();
  for(piT=matData->ctProb->begin()->second->begin(); piT!=matData->ctProb->begin()->second->end(); piT++)
  {
    augTest->insert(new symList(piT->first));//为检验分配的内存,toret->ctProb,toret->ctMat,toret->ctHists对应的检验都是这一块,
  }

  int i;
  for(i=0, iCurr = augHist->begin(); iCurr != augHist->end(); i++, iCurr++)//以toret->probs,toret->counts为基准给toret->ctProb,toret->ctMat赋值
  {
    rowDbl    = (*matData->probs)[i];   //以matData->probs为基准给Cao赋值
    rowDblMap = new symList2DblMap();  
    for(iCurr1 = augTest->begin(); iCurr1 != augTest->end(); iCurr1++)
    {
      rowDblMap->insert(pair<symList*, double*>(*iCurr1,new double(*(rowDbl++))));   //第i行经历下对应的检验及各检验对应的概率
    }   
    Cao->insert(pair<symList*, symList2DblMap*>(*iCurr, rowDblMap));         //第i行经历及第i行经历下对应的检验和各检验对应的概率
  }
  return Cao;
}






/*********************************************************************************************
函数功能：给定经历的集合landmarkHist和检验的集合Q，求概率矩阵p(ao|Q),p(Q|B),p(aoQ|B)
输入参数：
divideData——划分后的子状态空间模型；
landmarkHist——删除掉发生次数少于CUTOFF的经历后剩下的状态空间对应的经历的集合B；
step--给定检验的最大步数
输出：
概率矩阵p(ao|Q),p(Q|B),p(aoQ|B)
**********************************************************************************************/
ProbMAP *psr_pca::getProb(symList2Vec2Map *divideData,symList2symListSet *landmarkHist,int step)
{
  ProbMAP *toret=new ProbMAP;
  toret->aoProb=new symList2EstMap(); //存储经历核-一步检验的估计概率矩阵p(ao|B)
  toret->QaoQProb=new symList2EstMap();//存储概率矩阵p(Q|B),p(aoQ|B)估计概率矩阵

  ofstream probMatrix("probQMatrix.txt");//存储概率矩阵p(ao|B)
  ofstream testsData("testsData.txt");
  ofstream aotestData("aotestData.txt");
  ofstream histsData("histsData.txt");

  ofstream probQaoQMatrix("probQaoQMatrix.txt");//存储概率矩阵p(Q|B),p(aoQ|B)

  symList2Vec2Map::iterator vi=divideData->begin();
  map<symList* ,symSet*>::iterator lh=landmarkHist->begin();
  for(vi=divideData->begin(),lh=landmarkHist->begin();vi!=divideData->end(),lh!=landmarkHist->end(); vi++,lh++)
  {  
    symSet *baseHist=new symSet();
    for(iCurr = lh->second->begin(); iCurr != lh->second->end(); iCurr++)
    {
      baseHist->insert(new symList(*iCurr));//将相应landmark对应的子空间里的经历存入baseHist,即给定经历的集合
      histsData<<"histories="<<**iCurr<<endl;
    }

    symSet *tests = new symSet();//得到所有一步检验的集合ao
    symList *actlist = env->getActions(), *obslist = env->getObservations();
    symList *curract,*currobs,*aoTest;
    curract = actlist;     //tests 中保存了所有一步的 ao        
    while ((curract != NULL) && (curract->s != NULL)) 
    { 
      currobs = obslist;     
      while ((currobs != NULL) && (currobs->s != NULL)) 
      { 
	symList *Test = new symList(curract->s, new symList(currobs->s));cout<<"Test="<<*Test<<endl;
	if(tests->find(Test) == tests->end())
	   tests->insert(Test);
	else
	   delete aoTest;
	currobs = currobs->n;
       } 
       curract = curract->n;
    } 
    //上面tests保存了所有一步的 ao
    delete actlist; 

    testsData<<"landmark="<<*(vi->first)<<endl;
    for(iCurr = tests->begin(); iCurr != tests->end(); iCurr++)
    {
       testsData<<"tests="<<**iCurr<<endl;
    }        

    int testLen=1;
    tEstimateData *aoP=augEstimate(env,tests,baseHist,vi->second,testLen);//求概率矩阵p(ao|B)

    probMatrix<<"landmark="<<*(vi->first)<<endl;
    for(int i=0;i<aoP->probs->dim1();i++)
    {
      for(int j=0;j<aoP->probs->dim2();j++)
      {
        probMatrix<<(*aoP->probs)[i][j]<<" ";//將概率矩阵p(ao|B)的元素值输出到probQMatrix.txt中

      }
      probMatrix<<";";
      probMatrix<<endl;
    }

    toret->aoProb->insert(pair<symList* ,tEstimateData*>(new symList(vi->first),aoP));       


    switch(step)//根据具体环境给定检验的集合:cheese-maze选所有一步和两步检验;较复杂的大规模系统只取所有的一步检验作为给定经历的集合
    {
      case 1:
        break;
      case 2:
      {
        symSet *baseTest=new symSet();//两步检验
        for(iCurr = tests->begin(); iCurr != tests->end(); iCurr++)
        {
           for (iCurr1 = tests->begin(); iCurr1 != tests->end(); iCurr1++)
           {
             symList* aug=new symList();
             aug=new symList(*iCurr);//cout<<"aug111="<<*aug<<endl;
             (aug)->append(new symList(*iCurr1));//cout<<"aug="<<*aug<<endl;
             baseTest->insert(aug); 
           }
        }

        for (iCurr1 = baseTest->begin(); iCurr1 != baseTest->end(); iCurr1++)
        {
             if(tests->find(*iCurr1)==tests->end())
             tests->insert(new symList(*iCurr1));//將baseTest复制到tests,即tests中包含所有一步和两步的检验
        }
    
        for (iCurr1 = baseTest->begin(); iCurr1 != baseTest->end(); iCurr1++)//释放baseTest所占内存
        {
             delete *iCurr1;
        }
        delete baseTest;
        break;
      }
    }
    aotestData<<"landmark="<<*(vi->first)<<endl;
    for(iCurr = tests->begin(); iCurr != tests->end(); iCurr++)
    {
      aotestData<<"aotest="<<**iCurr<<endl;
    }

    testLen++;
    tEstimateData *QaoQP=augEstimate(env,tests,baseHist,vi->second,testLen);//求概率矩阵p(Q|B),p(aoQ|B)

    probQaoQMatrix<<"landmark="<<*(vi->first)<<endl;
    probQaoQMatrix<<"Row X Column:"<<QaoQP->probs->dim1()<<" X "<<QaoQP->probs->dim2()<<endl;
    for(int i=0;i<QaoQP->probs->dim1();i++)
    {
      for(int j=0;j<QaoQP->probs->dim2();j++)
      {
        probQaoQMatrix<<(*QaoQP->probs)[i][j]<<" ";
      }
      probQaoQMatrix<<";";
      probQaoQMatrix<<endl;
    }
    toret->QaoQProb->insert(pair<symList* ,tEstimateData*>(new symList(vi->first),QaoQP));
  }
  return toret;
}





psr_pca::psr_pca(Environment *e): psr(e)
{

}


psr_pca::~psr_pca() 
{   }




#endif
