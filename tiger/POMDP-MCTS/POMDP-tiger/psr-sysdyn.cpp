#ifndef __PSR_SYSDYN_CPP_
#define __PSR_SYSDYN_CPP_

#include <fstream>
#include <iostream>
#include <time.h>
#include <iomanip>
#include "unistd.h"

#include "psr-sysdyn.h"
#include "symbol.h"
using namespace std;

#define Length_TestData 11000


/****************************
函数功能：
首先调用getData获得训练数据，然后调用divideData对数据进行划分，调用getHistorySet得到符合要求的经历的集合，调用useMATLAB得到准确的概率矩阵，调用getPSR计算经历核-检验核的估计概率矩阵，调用getmodelPara完成了整个PSP模型学习，调用getError_oneStep和getError_fourStep计算psr模型划分算法的预测值与准确值的误差。
输入输出：无
*****************************/
void psr_sysdyn::learn()
{ 
  ofstream resultData("resultData.txt");//存储误差结果

  /*int Length_Data;
  cout<<"please input the Length_Data=";
  cin>>Length_Data;resultData<<"Length_Data"<<Length_Data<<endl;
  vector<Symbol*>* conData=env->getData(Length_Data);*///获取整个状态空间的PSR模型训练数据
  vector<Symbol*> *conData=env->getconData();//需获取整个状态空间的PSR模型时,这样获取数据,使得获取整个状态空间的PSR模型的数据和进行状态空间划分获取PSR模型的数据相同---20130822
  resultData<<"dataList--Len="<<conData->size()/2<<endl;

  symList2Vec2Map *divideData=env->divideData(conData);//分隔数据

  symSet* landmarkSet= new symSet();
  symList2Vec2Map::iterator vi=divideData->begin();
  for(vi=divideData->begin();vi!=divideData->end(); vi++)
  {
      landmarkSet->insert(new symList(vi->first));
  }//得到landmark的集合


  int CUTOFF;
  cout<<"please input the CUTOFF=";
  cin>>CUTOFF;cout<<"CUTOFF="<<CUTOFF<<endl;
  symList2symListSet *landmarkHist=getHistorySet(divideData,CUTOFF);//得到符合要求的经历的集合
    
  symList2VfMap *landmarkbe=env->getLMbelief(landmarkSet);//得到每个landmark对应的信念状态
  
  Whole_modelPara *whole_modelMatrix; 
  cout<<"please choose the method:traditonal or matlab"<<endl;
  cout<<"choose matlab method please input 1 and choose traditonal please input 2"<<endl;
  int j;
  cin>>j;cout<<"input="<<j;
  switch(j)
  {
    case 1:
    {
      useMATLAB(landmarkbe,divideData,landmarkHist);//得到概率矩阵(用于matlab提取检验核)
      cout<<"please use matlab to get History-Test"<<endl;
      cout<<"please input any number to continue after getting and storing the History-Test"<<endl;
      int j1;
      cin>>j1;

      ProbMAP *Probmatrix=getProb(divideData,landmarkHist,j);//已知经历核-检验核对应的行号和列号,求出经历核-检验核,并己算经历核-检验核的估计概率矩阵

      whole_modelMatrix=getWhole_modelPara(landmarkbe,Probmatrix);//求模型学习参数,完成了整个PSP模型学习

      deleteProbMAP(Probmatrix);//释放内存空间
      delete Probmatrix;
      Probmatrix=NULL;
      break;
    }
    case 2:      
      //whole_modelMatrix=getOld_Whole_modelPara(divideData, landmarkSet, landmarkHist);
      break;
  }


  delete conData;
   
  for(vi=divideData->begin();vi!=divideData->end();vi++)
  {
     delete vi->first;//该处内存对应数据已经copy到landmarkSet中了 //20130726CZZ
     vector<symList*>::iterator v2=vi->second->begin();
     for(v2=vi->second->begin();v2!=vi->second->end();v2++)
     {
      delete *v2;//释放每一串数据的存储内存
     }
     delete vi->second;
  }
  delete divideData;

  //vector<Symbol*> *testData=env->get1wTest();//得到固定的检验--20130821
  vector<Symbol*> *testData=env->getData(Length_TestData);

  int2SymListMap *startLMPos=getStartLMPos(testData,landmarkSet);//找到testData的第一个landmark,以此landmark作为10000步检验数据初始状态

  int currPos=startLMPos->begin()->first;//testData的第一个landmark的位置
  symList *startLandmark=startLMPos->begin()->second;//testData的第一个landmark

  LM_modelPara *startlandmark_para=getLM_modelPara(whole_modelMatrix,startLandmark);//得到检验数据中第一个landmark--startLandmark对应的子空间各个模型参数
    
  vector<float> *startLMbelief=getStartLMbelief(startLandmark,landmarkbe);//得到startLandmark对应的信念状态

  double error_one=getError_oneStep(startLMPos,startlandmark_para,startLMbelief,whole_modelMatrix,testData,landmarkSet);//获取一步检验的PSR预测值与POMDP准确值的差值
  resultData<<"error_one="<<error_one<<endl;
  cout<<"error_one="<<error_one<<endl;
   
  double error_four=getError_fourStep(startLMPos,startlandmark_para,startLMbelief,whole_modelMatrix,testData,landmarkSet);//获取四步检验的PSR预测值和POMDP准確值的差值
  resultData<<"error_four="<<error_four<<endl;
  cout<<"error_four="<<error_four<<endl;

  delete startlandmark_para;//释放存储startlandmark_para所占的内存空间
  delete startLMPos;
  delete startLMbelief;
  vector<Symbol*>::iterator vt=testData->begin();
  for(vt=testData->begin();vt!=testData->end();vt++)
  {
      delete *vt;
  }

  delete whole_modelMatrix;
  delete landmarkbe;
  delete landmarkHist;

  for(iCurr=landmarkSet->begin();iCurr!=landmarkSet->end();iCurr++)
  {
    cout<<"iCurr1="<<**iCurr<<endl;
    delete *iCurr;
  }
  delete landmarkSet;
  landmarkSet=NULL;
  cout<<'\n'<<endl;
}




/*************************************************************************************************
  函数功能：获取一步检验的PSR预测值与POMDP准确值的差值（TY）
  参数：  
  startLMPos——testData的第一个landmark及其在testData中所处的位置
  startlandmark_para——测試数据testData中第一个landmark对应的子空间模型参数(Mao,Maoq,firstprob) 
  startLMbelief——测試数据中第一个landmark对应的信念状态 
  whole_modelMatrix——整个空间模型参数(由各个子空间模型参数组合成)
  testData——测试数据
  landmarkSet——landmark的集合 
  返回值：
  errSum：一步检验的PSR预测值与POMDP准确值的差值
/*************************************************************************************************/
double psr_sysdyn::getError_oneStep(int2SymListMap *startLMPos, LM_modelPara *startlandmark_para, vector<float> *startLMbelief, Whole_modelPara *whole_modelMatrix, vector<Symbol*> *testData, symSet* landmarkSet)
{   
  vector<float> *belief_start=new vector<float>;
  vector<float>::iterator be;
  for(be=startLMbelief->begin();be!=startLMbelief->end();be++)
  {
    belief_start->push_back(*be);
  }
  vector<float> *belief_end=new vector<float>;
  int size_T=testData->size();
  int lmSize = (*landmarkSet->begin())->size();                       //所有 landmark 已定为长度是一致的
  symList2VecMap *maoMatrix=startlandmark_para->maoMatrix;
  symListVecMat *maoqMatrix=startlandmark_para->maoqMatrix;
  vector<double> *first=startlandmark_para->firstprob;
  int k=startlandmark_para->maoMatrix->begin()->second->size();cout<<"maoMatrix----k="<<k<<endl;
  Array1D<double> *tempCoreValue = new Array1D<double>(k, 0.0);       //用于存放 p(Q|h)的值
  Array1D<double> *coreValue = new Array1D<double>(k, 0.0);           //用于存放 p(Q|h)的值,(*coreValue)[hao]=P(Q|hao)=P(Q|h)Maoq(hao)/P(Q|h)m(hao)
  for(i=0; i<k; i++)
  {(*tempCoreValue)[i] = (*coreValue)[i] = (*startlandmark_para->firstprob)[i];}
  for(i=0; i<k; i++)
  {cout<<"(*tempCoreValue)[i]="<<(*tempCoreValue)[i]<<endl;}
  ofstream one_errorData("one_errorData.txt"); 
  viH=maoqMatrix->begin();cout<<"maoqMatrix----k="<<maoqMatrix->begin()->second->size()<<endl;
  int currPos=startLMPos->begin()->first;                   //testData的第一个landmark的位置

  symList *startLandmark=startLMPos->begin()->second;       //testData的第一个landmark  
  double pomPre, psrPre, errSum; 
  errSum=0.0;
  int num=0;
   
  for(int posT=currPos+1;posT<size_T;posT+=2)                          //获取一步检验的PSR预测值和POMDP准確值的差值
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
         for(m=0; m<k; m++)                                             //得到 psr 模型的预测值
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
         for(m=0; m<k; m++)                              
	 {
            psrPre += ((*coreValue)[m] * (maoMatrix->find(*iCurr)->second->at(m))); //得到 psr 模型的预测值
         }
         if(psrPre>1.0)psrPre=1.0;
         if(psrPre<0.0)psrPre=0.0;

         if(sum3>1)
         {
           psrPre=psrPre/sum3;
         }

	 if(psrPre != pomPre)  //表示预测的不相同
	 errSum += pow( (psrPre-pomPre), 2 );
          cout<<"test="<<**iCurr<<" psrPre="<<psrPre<<"    pomPre="<<pomPre<<"    errSum="<<errSum<<endl;
          one_errorData<<"test="<<**iCurr<<" psrPre="<<psrPre<<"    pomPre="<<pomPre<<"    errSum="<<errSum<<endl;
      }
      for(iCurr=nextAoSet->begin(); iCurr!=nextAoSet->end(); iCurr++)
      {
         delete *iCurr;
      }
      delete nextAoSet;
      cout<<endl;
      
      newAO=getHistory(testData,posT+1,1);                                    //经历进行一步扩展h到hao,根据采取的动作值a和得到的观测值o更新POMDP状态和PSR状态
      vector<Symbol*>* testData1=env->getVecdata(newAO);                      //testData1,经历经过一步ao,用于更新 POMDP
      symList *currMemory=getMemory(testData1, 1, lmSize);                    //取观测值o(symList型)---20130821

      if(landmarkSet->find(currMemory) != landmarkSet->end())                 //说明当前的 memory 为 landmark//更新 POMDP 模型(从P(Q|AO)到P(Q|newAO),(*coreValue)[newAO]=P(Q|newAO)=P(Q|AO)Maoq(newAO)/P(Q|AO)m(newAO))20130623
      {
        symList *Landmark=new symList(currMemory);cout<<"Landmark="<<*Landmark<<endl;
        
         LM_modelPara *Landmark_para=getLM_modelPara(whole_modelMatrix,Landmark);           //*Landmark对应的landmark的子空间的模型参数 //碰到landmark重新给(*first)[j],maoMatrix,maoqMatrix赋值 
         maoMatrix=Landmark_para->maoMatrix;
         maoqMatrix=Landmark_para->maoqMatrix;
         first=Landmark_para->firstprob;
         k=maoMatrix->begin()->second->size();cout<<"maoMatrix----k="<<k<<endl;
         viH=maoqMatrix->begin();cout<<"maoqMatrix----k="<<maoqMatrix->begin()->second->size()<<endl;

         delete tempCoreValue;
         delete coreValue;
         tempCoreValue = new Array1D<double>(k, 0.0);      
         coreValue = new Array1D<double>(k, 0.0);
         for(int m1=0; m1<k; m1++) (*tempCoreValue)[m1] = (*coreValue)[m1] = (*first)[m1];

         delete Landmark;
      }
      delete currMemory;

      denominator = 0;
      for(n=0; n<k; n++)
      {
         denominator += ((*tempCoreValue)[n] * (maoMatrix->find(newAO)->second->at(n)));  //P(Q|AO)m(newAO)
      }
      
      delete coreValue;
      coreValue = new Array1D<double>(k, 0.0);
      if(denominator == 0)       //除数为0,此处要加点预防措施
      {
         for(m=0; m<k; m++)
	 (*coreValue)[m] = 0.0;
      }
      else
      {
	  for(m=0, viT=viH->second->begin(); viT!=viH->second->end(); m++, viT++)        //获取当前时刻的 core-test 的值
	  {
             numerator=0;
	     for(n=0; n<k; n++)
             {
               numerator += (*tempCoreValue)[n] * (maoqMatrix->find(newAO)->second->find(viT->first)->second->at(n));//P(Q|AO)Maoq(newAO)
             }
             (*coreValue)[m] = (double)(numerator/denominator);  //P(Q|newAO)=P(Q|AO)Maoq(newAO)/P(Q|AO)m(newAO)
             if( (*coreValue)[m] > 1 )
              (*coreValue)[m] = 1;
	     if( (*coreValue)[m] < 0 )
	      (*coreValue)[m] = 0;
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
  errSum /= (env->nrObservs() *num);                       //其中 nrObservs 为观测值的数量,DATA_LENGTH=10000为检验数据的长度
  cout<<"一步检验误差率Result data is "<<errSum<<endl;
  delete coreValue;
  delete tempCoreValue;
  delete belief_start;
  return errSum;
}


/*************************************************************************************************
  函数功能：获取四步检验的PSR预测值与POMDP准确值的差值（CZZ）
  输入参数：  
  startLMPos——testData的第一个landmark及其在testData中所处的位置;
  startlandmark_para——测試数据testData中第一个landmark对应的子空间模型参数(Mao,Maoq,firstprob); 
  startLMbelief——测試数据中第一个landmark对应的信念状态;
  whole_modelMatrix——整个空间模型参数(由各个子空间模型参数组合成);
  testData——测试数据;
  landmarkSet——landmark的集合; 
  返回值：
  四步检验的PSR预测值与POMDP准确值的差值.
/*************************************************************************************************/
double psr_sysdyn::getError_fourStep(int2SymListMap *startLMPos, LM_modelPara *startlandmark_para, vector<float> *startLMbelief, Whole_modelPara *whole_modelMatrix, vector<Symbol*> *testData, symSet* landmarkSet)
{
  double pomPre, psrPre, errSum=0.0; 
  int num=0;
  vector<float> *belief_start=new vector<float>;
  vector<float>::iterator be;
  for(be=startLMbelief->begin();be!=startLMbelief->end();be++)   //初始化到startLandmark对应的状态----29130821czz
  { belief_start->push_back(*be);}

  symList2VecMap *maoMatrix=startlandmark_para->maoMatrix;       //得到检验数据中第一个landmark--startLandmark对应的子空间模型参数maoMatrix
  symListVecMat *maoqMatrix=startlandmark_para->maoqMatrix;      //得到检验数据中第一个landmark--startLandmark对应的子空间模型参数maoqMatrix
  vector<double> *first=startlandmark_para->firstprob;           //得到检验数据中第一个landmark--startLandmark对应的子空间模型参数first
  int k=maoMatrix->begin()->second->size();cout<<"maoMatrix----k="<<k<<endl;
  Array1D<double> *tempCoreValue = new Array1D<double>(k, 0.0);  //用于存放 p(Q|h)的值
  Array1D<double> *coreValue = new Array1D<double>(k, 0.0);      //用于存放 p(Q|h)的值,(*coreValue)[hao]=P(Q|hao)=P(Q|h)Maoq(hao)/P(Q|h)m(hao)
  for(i=0; i<k; i++)
  {(*tempCoreValue)[i] = (*coreValue)[i] = (*first)[i];}
  viH=maoqMatrix->begin();cout<<"maoqMatrix----k="<<maoqMatrix->begin()->second->size()<<endl;

  int currPos=startLMPos->begin()->first;             //testData的第一个landmark的位置
  symList *startLandmark=startLMPos->begin()->second; //testData的第一个landmark
  int size_T=testData->size();                                   //testData的长度

  vector<float> *belief_end=new vector<float>;
  for(int posT=currPos+2;posT<size_T;posT+=2)                    //获取四步检验的PSR预测值和POMDP准確值的差值
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
              for(m=0, viT=viH->second->begin(); viT!=viH->second->end(); m++, viT++) //获取经过test后的 core-test 的值
	      {
	          numerator   = 0;
	          for(n=0; n<k; n++)
                  {
                    numerator += (*tempTest1Value)[n] * (maoqMatrix->find(test)->second->find(viT->first)->second->at(n));
                  }   //P(Q|h)Maoq(ao)
                  (*tempValue1)[m] = (double)(numerator);
                  if( (*tempValue1)[m]>1.0) (*tempValue1)[m]=1.0;
                  if( (*tempValue1)[m]<0.0) (*tempValue1)[m]=0.0;	             
              }
              for(n=0; n<k; n++)
              {
               (*tempTest1Value)[n]=(*tempValue1)[n];  //把(*tempValue1)[m]的值赋给(*tempTestValue)[m],用于上面循环的下一次(*tempTestValue)[m]
              }
              delete test;
          }//已经得到P(Q|h)M(ao)M(ao)M(ao)的值,存于(*tempTestValue)[m]中,此时(*tempTestValue)[m]=(*tempValue1)[m]
           
          symList *test=getHistory(Test,len-1,1);      //得到最后一步ao
          for(m=0; m<k; m++)                           //得到 psr 模型的预测值
          {
            psrPre += ((*tempValue1)[m] * (maoMatrix->find(test)->second->at(m)));//p(Test|h)=p(Q|h)M(a1o1)M(a2o2)M(a3o3)m(a4o4)
          }
          if(psrPre>1.0)psrPre=1.0/env->nrObservs();
          if(psrPre<0.0)psrPre=0.0;
          delete test;
       }
       else                                                //即Test=Test4ao中含landmark,Test被分隔成两部份Test1,Test2
       {  
          double psrPre1=0.0,psrPre2=0.0;
          
          if((Test1->size())>2)//计算psrPre1=p(Test1|h),分两种情况:(1)(Test1->size())>2,p(Test1|h)=p(Q|h)M(ao)...m(ao);(2)(Test1->size())==2,p(Test1|h)=p(Q|h)m(ao)p(Test1|h)=p(Q|h)M(ao)...m(ao)
          {
             for(int pos1=1;pos1<(Test1->size())-2;pos1+=2)//依次取ao...,进而得到M(ao)...
             {
                symList* test=getHistoryL(Test1,pos1,1);   //cout<<"test="<<*test<<endl;
                for( m=0, viT=viH->second->begin(); viT!=viH->second->end(); m++, viT++) //获取当前时刻的 core-test 的值
	        {
	            numerator   = 0;
	            for(n=0; n<k; n++)
	            {
                       numerator += (*tempTest1Value)[n] * (maoqMatrix->find(test)->second->find(viT->first)->second->at(n));
                    } //P(Q|h)Maoq(ao)
                    (*tempValue1)[m] = (double)(numerator);
                   if( (*tempValue1)[m]>1.0) (*tempValue1)[m]=1.0;                     
	           if( (*tempValue1)[m]<0.0) (*tempValue1)[m]=0.0;	             
                }
                delete test;  
                for(n=0; n<k; n++)
                {
                  (*tempTest1Value)[n]=(*tempValue1)[n];//把(*coreValue)[m]的值赋给(*tempTestValue)[m],用于上面循环的下一次(*tempTestValue)[m]
                }
             }//已经得到P(Q|h)M(ao)M(ao)M(ao)的值,存于(*tempTestValue)[m]中,此时(*tempTestValue)[m]=(*coreValue)[m]
           
             symList* test=getHistory(Test,len-1,1);   //得到最后一步ao
             for(m=0; m<k; m++)                              
             {
               psrPre1 += ((*tempValue1)[m] * (maoMatrix->find(test)->second->at(m)));//p(Test|h)=p(Q|h)M(a1o1)M(a2o2)M(a3o3)m(a4o4)
             }
             if(psrPre1>1.0)psrPre1=1.0;
             if(psrPre1<0.0)psrPre1=0.0;
             delete test;
          }
          else//(2)(Test1->size())==2,p(Test1|h)=p(Q|h)m(ao)
          {  
             for(m=0; m<k; m++)                              
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
             LM_modelPara *startlandmark4_para=getLM_modelPara(whole_modelMatrix,lt->first);//*(lt->first)对应的landmark的子空间的模型参数,针对划分后的每一个检验(其中再无landmark),求p(t|空经历),保持初始的maoMatrix,maoqMatrix在检验中不变,所以重新定义mao4Matrix,maoq4Matrix
             
             symList2VecMap *mao4Matrix=startlandmark4_para->maoMatrix;
             symListVecMat *maoq4Matrix=startlandmark4_para->maoqMatrix;
             vector<double> *firstProb4=startlandmark4_para->firstprob;
             int k4=mao4Matrix->begin()->second->size();cout<<"mao4Matrix----k4="<<k4<<endl;
             Array1D<double> *tempTest2Value=new Array1D<double>(k4, 0.0); 
             Array1D<double> *tempValue2=new Array1D<double>(k4, 0.0);
             for(int m1=0; m1<k4; m1++)
             {  
               (*tempTest2Value)[m1] = (*tempValue2)[m1]=(*firstProb4)[m1];               //此时经历h对应的p(Q|h)
             }
             viH4=maoq4Matrix->begin();cout<<"maoq4Matrix----k4="<<maoq4Matrix->begin()->second->size()<<endl;


             if((lt->second)->size()>2)
             {
                for(int pos1=1;pos1<((lt->second)->size())-2;pos1+=2)                       //依次取ao...,进而得到M(ao)...
                {
                  symList* test=getHistoryL((lt->second),pos1,1);
                  for(m=0, viT=viH4->second->begin(); viT!=viH4->second->end(); m++, viT++) //获取当前时刻的core-test 的值(tempValue2)
	          {
	              numerator=0;
	              for(n=0; n<k4; n++)
	              {
                         numerator += (*tempTest2Value)[n] * (maoq4Matrix->find(test)->second->find(viT->first)->second->at(n));
                      }//P(Q|h)Maoq(ao)
                      (*tempValue2)[m] = (double)(numerator);
                      if( (*tempValue2)[m]>1.0)(*tempValue2)[m]=1.0;                      
	              if( (*tempValue2)[m]<0.0)(*tempValue2)[m]=0.0;	              
                  }
                  for(n=0; n<k4; n++)
                  {
                     (*tempTest2Value)[n]=(*tempValue2)[n];//把(*tempValue2)[m]的值赋给(*tempTestValue)[m],用于上面循环的下一次(*tempTestValue)[m]
                  }
                  delete test;
                }//已经得到P(Q|空经历)M(ao)M(ao)M(ao)的值,存于(*tempTest2Value)[m]中,此时(*tempTest2Value)[m]=(*tempValue2)[m]
                symList* test=getHistory(Test,len-1,1);//得到最后一步ao
                for(m=0; m<k4; m++) //得到 psr 模型的预测值
                {psrPre2 += ((*tempTest2Value)[m] * (mao4Matrix->find(test)->second->at(m)));}//p((lt->second)|空经历)=p(Q|空经历)M(ao)...m(ao)
                if(psrPre2>1.0)psrPre2=1.0;
                if(psrPre2<0.0)psrPre2=0.0;
                delete test;
             }
             else
             { 
                for(m=0; m<k4; m++)                              
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
  
      if(psrPre != pomPre)  errSum += pow( (psrPre-pomPre), 2 );  //表示预测的不相同
      delete Test4ao;     
     
      newAO=getHistory(testData,posT,1);//经过一步经历,根据采取的动作值和得到的观测值更新POMDP状态和PSR状态
      vector<Symbol*> *testData1=env->getVecdata(newAO);//testData1,经历经过一步ao,用于更新 POMDP
      int lmSize=1;//只获取长度为1的记忆
      symList *currMemory = getMemory(testData1, 1, lmSize); cout<<"currMemory="<<*currMemory<<endl; //位置pos=1对应testData1的观测值      
      
      if(landmarkSet->find(currMemory) != landmarkSet->end())//说明当前的 memory 为 landmark,更新 POMDP 模型(从P(Q|AO)到P(Q|newAO),(*coreValue)[newAO]=P(Q|newAO)=P(Q|AO)Maoq(newAO)/P(Q|AO)m(newAO))20130623
      {
         symList *Landmark=new symList(currMemory);//得到检测数据testData中下一个landmark,可能和前一个landmark相同,也肯能不同,碰到landmark重新给(*first)[j],maoMatrix,maoqMatrix赋值
         
         LM_modelPara *Landmark_para=getLM_modelPara(whole_modelMatrix,Landmark);//*Landmark对应的landmark的子空间的模型参数  
         maoMatrix=Landmark_para->maoMatrix;
         maoqMatrix=Landmark_para->maoqMatrix;
         first=Landmark_para->firstprob;
         k=maoMatrix->begin()->second->size();cout<<"maoMatrix----k="<<k<<endl;
         viH=maoqMatrix->begin();cout<<"maoqMatrix----k="<<maoqMatrix->begin()->second->size()<<endl;

         delete Landmark;

         delete tempCoreValue;
         delete coreValue;
         tempCoreValue = new Array1D<double>(k, 0.0);      
         coreValue = new Array1D<double>(k, 0.0);
         for(int m1=0; m1<k; m1++) (*tempCoreValue)[m1] = (*coreValue)[m1] = (*first)[m1];
      }
      delete currMemory;
    
      denominator = 0.0;
      for(n=0; n<k; n++)
      {denominator += ((*tempCoreValue)[n] * (maoMatrix->find(newAO)->second->at(n)));}//P(Q|h)m(newAO)
      cout<<"denominator="<<denominator<<endl;
      if(denominator == 0.0)   //如除数为0,则此处要加点预防措施
      {for(m=0; m<k; m++)(*coreValue)[m] = 0.0;}
      else
      {
	  for(m=0, viT=viH->second->begin(); viT!=viH->second->end(); m++, viT++) //获取当前时刻的 core-test 的值
	  {
	    numerator   = 0.0;
	    for(n=0; n<k; n++)
            {
               numerator += (*tempCoreValue)[n] * (maoqMatrix->find(newAO)->second->find(viT->first)->second->at(n));
            }//P(Q|AO)Maoq(newAO)
	    (*coreValue)[m] = (double)(numerator/denominator);//P(Q|hAO)=P(Q|h)Maoq(newAO)/P(Q|h)m(newAO)
	    if( (*coreValue)[m]>1.0) (*coreValue)[m]=1.0;	      
	    if( (*coreValue)[m]<0.0) (*coreValue)[m]=0.0;	      
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
 函数功能：在整个空间模型whole_modelMatrix中找到指定Landmark对应的子空间模型参数LM_modelPara(包括Mao,Maoq,firstprob)
 输入参数:
 whole_modelMatrix--整个空间模型参数;
 Landmark--指定的landmark
 输出:
 LM_modelPara--该Landmark对应的子空间模型(包含Mao,Maoq,firstprob)
/*************************************************************************************************/
LM_modelPara *psr_sysdyn::getLM_modelPara(Whole_modelPara *whole_modelMatrix, symList *Landmark)
{
   LM_modelPara *toret=new LM_modelPara;  //存储Landmark对应子状态空间模型的各个学习参数   
   toret->maoMatrix=NULL;         //Landmark对应状态的初始学习模型参数mao
   for(lmMao=whole_modelMatrix->landmarkMaoMap->begin();lmMao!=whole_modelMatrix->landmarkMaoMap->end();lmMao++)
   {
     if(*Landmark==*(lmMao->first))
     {
      cout<<"startLandmark="<<*(lmMao->first)<<endl;
      toret->maoMatrix=(lmMao->second);
      break;
    }
   }
   int k=toret->maoMatrix->begin()->second->size();cout<<"maoMatrix----k="<<k<<endl;

   toret->firstprob=NULL;       //Landmark对应状态的初始学习模型参数core-test,存放初始状态的 core-test 的值,
   for(lmfP=whole_modelMatrix->landmarkfirstProbMap->begin();lmfP!=whole_modelMatrix->landmarkfirstProbMap->end();lmfP++)
   {
      cout<<"landmark="<<*(lmfP->first)<<endl;
      if(*Landmark==*(lmfP->first))
      {
          toret->firstprob=(lmfP->second);
          break; 
      }
   }
  
   toret->maoqMatrix=NULL;    //Landmark对应状态的初始学习模型参数maoq
   for(lmMaoq=whole_modelMatrix->landmarkMaoqMap->begin();lmMaoq!=whole_modelMatrix->landmarkMaoqMap->end();lmMaoq++)
   {
    cout<<"landmarkq="<<*(lmMaoq->first)<<endl;
    if(*Landmark==*(lmMaoq->first))
    {
       toret->maoqMatrix=(lmMaoq->second);
       break;
    }
   }
   return toret;
}










/************************************************************
 函数功能：得到各个子空间模型参数,并组合成整个空间模型参数
 输入参数：
 landmarkbe--每个landmark对应的信念状态      
 PSRmatrix--所有子状态空间对应的经历核--检验核的估计概率矩阵
 输出：整个空间模型参数Whole_modelPara(每个landmark对应的Mao,Maoq,firstprob)
/************************************************************/
Whole_modelPara *psr_sysdyn::getWhole_modelPara(symList2VfMap *landmarkbe,ProbMAP *Probmatrix)
{
   ofstream paraData      ("data.txt");                   //输出学习好后的各个参数
   Whole_modelPara *toret=new Whole_modelPara;
   toret->landmarkMaoqMap=new symList2symListVecMatMap;//存储landmark及学习参数Maoq
   toret->landmarkMaoMap=new symList2symListVecMatMapMap;//存储landmark及学习参数Mao
   toret->landmarkfirstProbMap=new symList2VecMap;//存储landmark及每个子模型的初始状态;
   toret->lmqtestProbMap=new symListDblMat;//存储landmark及每个子模型的初始状态中每个检验及检验对应的概率;

   map<symList*,vector<float>*>::iterator lmb=landmarkbe->begin();   
   for(lmb=landmarkbe->begin();lmb!=landmarkbe->end();lmb++)
   {
        tEstimateData *P2=fillValue(lmb->first,Probmatrix->aoProb);
        tEstimateData *P3=fillValue(lmb->first,Probmatrix->QaoQProb);

        symSet *qtests = new symSet();//此时qtests不包含空检验-----20130913
        piH=P2->ctProb->begin();
        for (piT = piH->second->begin(); piT != piH->second->end(); piT++) 
        { 
          qtests->insert(new symList(piT->first));
        }

        int k=P2->ctProb->size();
        paraData<<"landamrk="<<*(lmb->first)<<"rank="<<k<<endl;
       
        Array2D<double> *probsD= new Array2D<double>(k, k);                            //表示的是 p(Qt/Qh) 矩阵
        for(i=0, piH=P2->ctProb->begin(); piH!=P2->ctProb->end(); i++, piH++)
        {
          for(j=0, piT=piH->second->begin(); piT!=piH->second->end(); j++, piT++)
          { 
            (*probsD)[i][j] = *(piT->second);
          }
        }
        piH = P2->ctProb->begin(); 
        Array1D<double> *firstProb= new Array1D<double>(k, 0.0);           //存放初始状态的 core-test 的值,
        vector<double> *first = new vector<double>;
        symList2DblMap *qtestProb=new symList2DblMap;
        for(i=0, piT=piH->second->begin(); piT!=piH->second->end(); i++, piT++)//初始化 firstProb，上面的行移除算法可以保证
        {
         (*firstProb)[i] = *(piT->second); 
         first->push_back(*(piT->second));
         qtestProb->insert(pair<symList*, double*>(new symList(piT->first),new double(*(piT->second))));
        }

        paraData<<endl;
        paraData<<"first history (firstProb[i])"<<endl;//输出初始预测向量p=(Q|空经历)=firstProb
        for(i=0; i<k; i++)
        {paraData<<(*firstProb)[i]<<" ";}
        paraData<<endl;paraData<<endl;
        paraData<<"qtestProb element"<<endl;
        for(qP=qtestProb->begin(); qP!=qtestProb->end(); qP++)
        {
         paraData<<"qtest="<<*(qP->first)<<"  Prob="<<*(qP->second)<<endl; 
        }
        paraData<<endl;
    
        toret->landmarkfirstProbMap->insert(pair<symList*, vec*>(new symList(lmb->first), first));
        toret->lmqtestProbMap->insert(pair<symList*, symList2DblMap*>(new symList(lmb->first),qtestProb));

      
        paraData<<"probsD matrix"<<endl; //输出probsD的值
        for(i=0; i<k; i++)
        {
          for(j=0; j<k; j++)
	  {
            paraData<<(*probsD)[i][j]<<"   ";
            cout<<(*probsD)[i][j]<<" ";
          }
          paraData<<endl;
        }
        paraData<<endl;

         int i,j,k1;
         singular = new SVD<double>(*probsD);          //利用svd分解方法求矩阵probsP的奇异值--[U,S,V]=svd(probsD),则有probsD=U*S*V',inverse_probsD=V*(inv(S))*U'
         Array2D<double> *U=new Array2D<double>();
         singular->getU(*U);//得到U矩阵

         Array2D<double> *V=new Array2D<double>();
         singular->getV(*V);//得到V矩阵 
       
         Array2D<double> *S=new Array2D<double>();
         singular->getS(*S);//得到S矩阵

         Array2D<double> *inv_S=new Array2D<double>();
         singular->getinv_S(*inv_S);//得到S的逆矩阵inv_S

         cout<<"U="<<U->dim1()<<"x"<<U->dim2()<<" V="<<V->dim1()<<"x"<<V->dim2()<<"S="<<S->dim1()<<"x"<<S->dim2()<<"inv_S="<<inv_S->dim1()<<"x"<<inv_S->dim2()<<" k="<<k<<endl;

         Array2D<double> *Vinv_S=new Array2D<double>(V->dim1(),V->dim2());

         for(i=0;i<V->dim1();i++)
         {
            for(j=0;j<V->dim2();j++)
            {
              (*Vinv_S)[i][j]=0;
              for(k1=0;k1<V->dim2();k1++)
              {
                 (*Vinv_S)[i][j]+=(*V)[i][k1]*(*inv_S)[k1][j];    //计算V*inv_S
              }
           }        
         }

         Array2D<double> *UT=new Array2D<double>(U->dim1(),U->dim2());//求U矩阵的转置矩阵UT
         for(i=0;i<U->dim1();i++)
         for(j=0;j<U->dim2();j++)
         {
           (*UT)[i][j]=(*U)[j][i];             
         }

         Array2D<double> *A1=new Array2D<double>(Vinv_S->dim1(),Vinv_S->dim2());//计算probsD的逆矩阵A1
         for(i=0;i<Vinv_S->dim1();i++)
         for(j=0;j<Vinv_S->dim2();j++)
         {
            (*A1)[i][j]=0;
            for(int k2=0;k2<UT->dim2();k2++)
            (*A1)[i][j]+=(*Vinv_S)[i][k2]*(*UT)[k2][j];   //A1=V*(inv(S))*UT         
         }

         double inverse_probsD[k][k];
         for(i=0;i<A1->dim1();i++)
         {
           for(j=0;j<A1->dim2();j++)
           {
             (inverse_probsD)[i][j]=(*A1)[i][j];//將A1内的元素值赋给inverse_probsD
           }
         }
         delete A1;
         delete Vinv_S;
         delete inv_S; 
         delete S;
         delete V;
         delete U;
         delete UT;
         delete singular;
        
        paraData<<"inverse_probsD matrix"<<endl;//输出inverse_probsD的值
        for(i=0; i<k; i++)
        {
         for(j=0; j<k; j++)
	 paraData<<(inverse_probsD)[i][j]<<"  ";
         paraData<<endl;
        }
        paraData<<endl;

    symSet *tests = new symSet();
    symList *actlist = env->getActions(), *obslist = env->getObservations();
    symList *curract,*currobs,*aoTest;
    curract = actlist;     //tests 中保存了所有一步的 ao        
    while ((curract != NULL) && (curract->s != NULL)) 
    { 
      currobs = obslist;     
      while ((currobs != NULL) && (currobs->s != NULL)) 
      { 
	aoTest = new symList(curract->s, new symList(currobs->s));cout<<"aoTest="<<*aoTest<<endl;
	if(tests->find(aoTest) == tests->end())
	  tests->insert(aoTest);
	else
	  delete aoTest;
	currobs = currobs->n;
      } 
      curract = curract->n;
    }//上面tests保存了所有一步的 ao
      delete actlist; 
      delete obslist;

    double sum, sumq, temp, tempP;
    symList2VecMap *maoMatrix= new symList2VecMap;
    for(iCurr=tests->begin(); iCurr!=tests->end(); iCurr++)           //初始化 mao 
    {
       cout<<"iCurr="<<**iCurr<<endl;
       vector<double> *elementVec= new vector<double>;
       for(j=0; j<k; j++)
       {
	  sum = 0.0;
	  for(m=0, piH=P3->ctProb->begin(); piH!=P3->ctProb->end(); m++, piH++)
	  {

           tempP=0.0;temp=0.0;
           if((piH->second->find(*iCurr))!=piH->second->end())
           {
	       tempP = *(piH->second->find(*iCurr)->second);
           }
	    temp = (inverse_probsD)[j][m] * tempP;
           if(temp != 0)
	    sum += temp;
	  }
          if(fabs(sum) < 0.000001)
	  sum = 0.0;
	  elementVec->push_back(sum);
	}
       maoMatrix->insert(pair<symList*, vec*>(new symList(*iCurr), elementVec));
    }
    //上面的没问题了20130604
    toret->landmarkMaoMap->insert(pair<symList* ,symList2VecMap*>(new symList(lmb->first), maoMatrix));//存储landmark及学习参数Mao

    symList *aug;
    symListVecMat *maoqMatrix= new symListVecMat;//paraData<<"maoqMatrixchushi---k="<<maoqMatrix->begin()->second->size()<<endl;
    for(i=0, iCurr=tests->begin(); iCurr!=tests->end(); i++, iCurr++) //初始化 maoq
    {
       symList2VecMap *rowDblMapQ = new symList2VecMap;
       for(iCurr2=qtests->begin(); iCurr2!=qtests->end(); iCurr2++)
       {
	  vector<double> *elementVec= new vector<double>;
	  aug = new symList(*iCurr);
	  aug->append(new symList(*iCurr2));               //aug=aoq
          for(j=0; j<k; j++)
	  {
	    sumq = 0.0;
	    for(m=0, piH=P3->ctProb->begin(); piH!=P3->ctProb->end(); m++, piH++)//求学习参数Maoq
	    {
              tempP=0.0;temp=0.0;
              if((piH->second->find(aug))!=piH->second->end())
              {tempP =  *(piH->second->find(aug)->second);}
	      temp = (inverse_probsD)[j][m] * tempP; 
              if(temp != 0)
	      sumq += temp;
	    }
	    if(fabs(sumq) < 0.000001)
	    sumq = 0.0;
	    elementVec->push_back(sumq);
	 }
	 
	  for(j=0; j<k; j++)
	  rowDblMapQ->insert(pair<symList*, vec*>(new symList(*iCurr2), elementVec));
	  delete aug;
      }
      maoqMatrix->insert(pair<symList*, symList2VecMap*>(new symList(*iCurr), rowDblMapQ));
    }
      toret->landmarkMaoqMap->insert(pair<symList* ,symListVecMat*>(new symList(lmb->first), maoqMatrix));
      delete firstProb;

     
     paraData<<"maoMatrix element"<<endl;//输出maoMatrix中的内容
     for(viT=maoMatrix->begin(); viT!=maoMatrix->end(); viT++)
     {
        paraData<<*(viT->first)<<" ";
        for(j=0; j<k; j++)
	paraData<<viT->second->at(j)<<" ";
        paraData<<endl;
     }
     
     paraData<<"maoqMatrix element"<<endl;//输出maoqiMatrix中的内容
     for(i=0,viH=maoqMatrix->begin(); viH!=maoqMatrix->end(); i++,viH++) 
     {
       paraData<<*(viH->first)<<endl;
       for(viT=viH->second->begin(); viT!=viH->second->end(); viT++)
       {
	  for(j=0; j<k; j++)
	  paraData<<setw(6)<<viT->second->at(j);
	  paraData<<endl;
       }
     }
     paraData<<endl;

    deleteStruct_tEs(P3);
    P3=NULL;
    deleteStruct_tEs(P2);
    P2=NULL;

  }
  return toret;
}


/********************************************************************************
  函数功能：获取给定经历和检验下准确的概率矩阵，此矩阵用于MATLAB提取经历核和检验核(CZZ）
  输入参数：
  landmarkbe--每个landmark对应的信念状态的集合;
  divideData--进行状态空间划分后每一个landmark对应的子状态空间;
  landmarkHist--每一个landmark对应的子状态空间经历集合
  输出：无
/********************************************************************************/
void psr_sysdyn::useMATLAB(symList2VfMap *landmarkbe,symList2Vec2Map *divideData,symList2symListSet *landmarkHist)
{
    ofstream matrixData  ("matrixData.txt");
    map<symList*,vector<float>*>::iterator lmb=landmarkbe->begin();
    map<symList* ,symSet*>::iterator lh=landmarkHist->begin();
    symList2Vec2Map::iterator vi=divideData->begin();
    for(lmb=landmarkbe->begin();lmb!=landmarkbe->end();lmb++)//获取每一组landmark对应的经历核-检验核对应的行号和列号
    {  
            symSet *tests = new symSet();
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
	         tests->insert(Test);//tests保存了所有一步的 ao
	         else
	         delete aoTest;
	         currobs = currobs->n;
             } 
             curract = curract->n;
           } 
           
           delete actlist; 
          symSet *baseTest=new symSet();//两步检验
          for (iCurr1 = tests->begin(); iCurr1 != tests->end(); iCurr1++)
           {
             baseTest->insert(new symList(*iCurr1));
           }

           for(iCurr = tests->begin(); iCurr != tests->end(); iCurr++)
           {
              delete *iCurr;
           }
           delete tests;

           baseTest->insert(new symList());

          for(vi=divideData->begin(),lh=landmarkHist->begin();vi!=divideData->end(),lh!=landmarkHist->end(); vi++,lh++)//针对每一个landmark对应的子状态空间状态集合landmarkHist和子状态空间数据divideData
          {
             if(*(lmb->first)==*(vi->first))//当是同一个landmark对应的子状态空间时,获取这一组子状态空间下给定经历和检验下对应的概率矩阵
             {
               POMDP_probData *P=exactProb(env,baseTest,lh->second,vi->second,lmb->second);//获取这一组子状态空间下给定经历和检验下对应的概率矩阵

               matrixData<<"landmark="<<*(lmb->first)<<" row="<<P->probs->dim1()<<" column="<<P->probs->dim2()<<"=[";
               for(int row_size = 0;row_size<P->probs->dim1(); row_size++)
               { 
                  for(int column_size = 0;column_size<P->probs->dim2(); column_size++)
                  {
                     matrixData<<std::fixed<<(*P->probs)[row_size][column_size]<<" ";//输出概率矩阵,以便后续用matlab提取经历核和检验核
                  }
                  matrixData<<";";
                  matrixData<<endl;
               }
               matrixData<<"]";
               matrixData<<endl<<endl<<endl; 
               deleteStruct_pom(P);
               break;
           }
          } 
    }
}








/**********************************
函数功能:通过MTALAB提取了检验核和经历核对应的列号和行号后，将列号和行号存放在txt文件中，准备好后，调用此函数可以从存储行号或者列号的txt中读取经历核在矩阵中对应的行或列
输入：无
输出：经历核或检验核
********************************/
set<int> *psr_sysdyn::getHroworTcolumn()//把经历对应的行号存如set<int>--20130909czz
{
  char name[80];
  cin >>name;                    //输入存储经历核对应的行号的txt文件或存储检验核对应的列号的txt文件
  cout<<name<<endl;
  char buffer[160];
  getcwd(buffer, 160);           
  char *workdir=strcat(buffer,name);  //读取文件绝对路徑
  cout<<workdir<<endl;  

  ifstream in(workdir);

  set<int> *HT=new set<int>;
  for(int t;in>>t;)
  {
    HT->insert(t);            //将txt中的整数依次插入HT中
  }
  return HT;
}















/*********************************************************************************************
函数功能：已知经历核-检验核对应的行号和列号，求出经历核-检验核，并且计算经历核-检验核的估计概率矩阵。
输入参数：
divideData——划分后的子状态空间模型；
landmarkHist——删除掉发生次数少于CUTOFF的经历后剩下的状态空间；
输出：
经历核-检验核的估计概率矩阵。
**********************************************************************************************/
ProbMAP *psr_sysdyn::getProb(symList2Vec2Map *divideData,symList2symListSet *landmarkHist,int step)
{
  ProbMAP *toret=new ProbMAP;
  toret->aoProb=new symList2EstMap(); //存储经历核-检验核的估计概率矩阵
  toret->QaoQProb=new symList2EstMap();//存储经历核--检验核的一步扩展和所有一步检验的估计概率矩阵

  symList2Vec2Map::iterator vi=divideData->begin();
  map<symList* ,symSet*>::iterator lh=landmarkHist->begin();
  int number=0;
  for(vi=divideData->begin(),lh=landmarkHist->begin();vi!=divideData->end(),lh!=landmarkHist->end(); vi++,lh++)
  {  
       number++;    
     symSet *tests = new symSet();
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

           for(iCurr = tests->begin(); iCurr != tests->end(); iCurr++)
           {
              baseTest->insert(new symList(*iCurr));//將tests复制到baseTest
           }
           for(iCurr = tests->begin(); iCurr != tests->end(); iCurr++)//释放tests内存
           {
              delete *iCurr;
           }
           delete tests;

           tests=new symSet();
           for (iCurr1 = baseTest->begin(); iCurr1 != baseTest->end(); iCurr1++)
           {
             tests->insert(new symList(*iCurr1));//將baseTest复制到tests
           }
    
           for (iCurr1 = baseTest->begin(); iCurr1 != baseTest->end(); iCurr1++)
           {
             delete *iCurr1;
           }
           delete baseTest;
           cout<<"please input the number of landmark:"<<number<<endl;
           cout <<"the format of input filename is:/name"<<endl;
           cout<<"please input Tcolumnname:";
           set<int> *Tcolumn=getHroworTcolumn();//读取使用matlab提取的检验核
           set<int>::iterator it=Tcolumn->begin();
           int column,row;
           baseTest=new symSet();
           for(column=0,iCurr = tests->begin(); iCurr != tests->end(); iCurr++,column++)
           {
                if(Tcolumn->find(column)!=Tcolumn->end())
                baseTest->insert(new symList(*iCurr));//将相应的列号对应的检验存入baseTest
           }
           for(iCurr = tests->begin(); iCurr != tests->end(); iCurr++)
           {
              delete *iCurr;
           }
           delete tests;   
    
           cout <<"the format of input filename is:/name"<<endl;
           cout<<"please input Hrowname:";
           set<int> *Hrow=getHroworTcolumn();//读取使用matlab提取的经历核
           symSet *baseHist=new symSet();
           for(row=0,iCurr = lh->second->begin(); iCurr != lh->second->end(); iCurr++,row++)
           {
                if(Hrow->find(row)!=Hrow->end())
                {
                 baseHist->insert(new symList(*iCurr));//将相应的行号对应的经历存入baseHist
                }
           }

           int testLen=1;
           tEstimateData *aoP=augEstimate(env,baseTest,baseHist,vi->second,testLen);//经历核-检验核的估计概率矩阵
           toret->aoProb->insert(pair<symList* ,tEstimateData*>(new symList(vi->first),aoP));

           testLen++;
           baseTest->insert(new symList());
           tEstimateData *QaoQP=augEstimate(env,baseTest,baseHist,vi->second,testLen);//经历核--检验核的一步扩展和所有一步检验的估计概率矩阵
           toret->QaoQProb->insert(pair<symList* ,tEstimateData*>(new symList(vi->first),QaoQP));
  }
  return toret;
}









psr_sysdyn::psr_sysdyn(Environment *e): psr(e)
{


}


psr_sysdyn::~psr_sysdyn() 
{   }



#endif

