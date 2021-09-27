#ifndef __HAND_POMDP_CPP_
#define __HAND_POMDP_CPP_


#include <fstream>
#include <iostream>
#include <time.h>
#include <iomanip>
#include "symbol.h"
#include <math.h>
#include "unistd.h"

#include "hand-pomdp.h"

using namespace std;

#define Length_TestData 5100

void hand_pomdp::learn()
 {
  ofstream ERRORS("ERRORS.txt"); 
  int Length_Data;
  cout<<"please input the Length_Data=";//Length_Data:希望得到ao序列的数量
  cin>>Length_Data;
  srand((unsigned int)time(NULL));
  int size_NewAct=6;
  int size_newS=45;
  int size_NewOb=6;
  vector<float> *belief=new vector<float>;
  belief->push_back(1.0);
  for(int i=1;i<45;i++)
     { 
      belief->push_back(0.0);
     }
  env->resert(belief);
  vector<Symbol*> *conData=env->getData(Length_Data);
  //vector<Symbol*> *conData=env->getconData(); //读取Psrdataid.txt文件中的训练数据
  HandPomdpData *RandomPOMDP=getRandomPomdp(size_NewAct,size_newS,size_NewOb);//获取初始随机的POMDP模型
  symSet *interestTests=getAOset();
  symList* handconData=new symList(); 
  for(int k=0;k<1;k++)
     {
      //vector<Symbol*> *conData=env->getconData(); //读取Psrdataid.txt文件中的训练数据
     
     symList* handData = getHistory(conData, 20000*(k+1)-1, 10000);
      //symList*handData=getMemory(conData, 0, 200); //获得currpos的下一个检验
  
      //cout<<*handData<<endl;
      handconData->append(new symList(handData));
      HandPomdpData*HandRandomPOMDP=EM(RandomPOMDP,handconData,size_NewAct,size_newS,size_NewOb,belief);//利用EM算法获得POMDP模型
      double errsum=getError(HandRandomPOMDP,interestTests,size_newS,belief);//求EM算法建立的POMDP模型与准确模型之间的误差值
     ERRORS<<"errsum"<<k+1<<"="<<errsum<<endl;
      
		 }
 }



/*********************************************************************************************
函数功能：得到所有动作-观测ao对的集合AO
输入参数：无
输出：
	得到所有动作-观测ao对的集合AO
**********************************************************************************************/
symSet *hand_pomdp::getAOset()
 { 
  symSet *AOset = new symSet();//得到所有动作-观测ao对的集合AO
  symList *actlist = env->getActions(), *obslist = env->getObservations();
  symList *curract,*currobs,*aoTest;
  curract = actlist;     //tests 中保存了所有一步的 ao        
  while((curract != NULL) && (curract->s != NULL)) 
       { 
        currobs = obslist;     
        while((currobs != NULL) && (currobs->s != NULL)) 
             { 
              symList *Test = new symList(curract->s, new symList(currobs->s));//cout<<"Test="<<*Test<<endl;
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


/************************************
函数功能：找到当前ao对应的新动作
函数输入：待抽象原始数据ao；
	       原始ao与new动作对应关系newAct；
函数输出：抽象后新动作
*************************************/
symList*hand_pomdp::getA(symList* ao,symList2SymbolMap *newAct)
 {
  symList*newA=new symList();
  symList2SymbolMap::iterator ls;
  for(ls=newAct->begin();ls!=newAct->end();ls++)
	   {
		  if(*(ls->first)==*ao){ newA=new symList(ls->second);break;}
	   }
  return newA;	
 }


/************************************
函数功能：找到当前经历h对应的新观测
函数输入：当前经历h；
	       h与新观测的对应关系S_h_newB；
	       当前landmark
函数输出：抽象后的新观测
*************************************/
symList*hand_pomdp::getO(symList*history,symList_sym2symMap *S_h_newB,symList*landmark)
 {
	symList*newO=new symList();
	symList_sym2symMap::iterator lhb;
	map<symList*, symList*>::iterator hb;
	for(lhb=S_h_newB->begin();lhb!=S_h_newB->end();lhb++)
	   {	
      if(*(lhb->first)==*landmark)
			  {	
				 for(hb=lhb->second->begin();hb!=lhb->second->end();hb++)
				 if(*(hb->first)==*history) { newO=new symList(hb->second);break;}
				 break;
			  }
	   }
  return newO;
 }


/************************************
函数功能：获取一个初始随机的POMDP模型
函数输入：动作个数，观测个数，状态个数
函数输出：初始随机的POMDP模型
*************************************/
HandPomdpData *hand_pomdp::getRandomPomdp(int Asize,int Ssize,int Osize)
 {
  ofstream RPOMDP("RPOMDP.txt");
	HandPomdpData *toret = new HandPomdpData;
  toret->TMatrix= new Array3D<double>(Asize, Ssize,Ssize);	*(toret->TMatrix) = 0.0;
  toret->OMatrix= new Array3D<double>(Asize, Ssize,Osize);	*(toret->OMatrix) = 0.0;
  for(int o=0;o<Asize;o++)/**********给T矩阵赋随机数值*******************/
     {
      cout<<"TMatrix"<<o<<":"<<endl;
      RPOMDP<<"TMatrix"<<o<<":"<<endl;
      for(int i=0;i<Ssize;i++)
         { 
          double sum=0.0;
          for(int j=0;j<Ssize;j++)
             { 
              (*toret->TMatrix)[o][i][j]=1+( rand()/double(20) );
              sum+=(*toret->TMatrix)[o][i][j];
             }
          for(int j=0;j<Ssize;j++)
             { 
              (*toret->TMatrix)[o][i][j]=(*toret->TMatrix)[o][i][j]/sum;
              cout<<(*toret->TMatrix)[o][i][j]<<"  ";RPOMDP<<(*toret->TMatrix)[o][i][j]<<"  ";
             }
          cout<<endl;RPOMDP<<endl;
         }
      cout<<endl;RPOMDP<<endl;
     }    
  for(int o=0;o<Asize;o++)/*************给O矩阵赋随机数值************/
     {
      cout<<"OMatrix"<<o<<":"<<endl;
      RPOMDP<<"TMatrix"<<o<<":"<<endl;
      for(int i=0;i<Ssize;i++)
         {
          double sum1=0.0;
          for(int j=0;j<Osize;j++)
             {   
              (*toret->OMatrix)[o][i][j]=1+( rand()/double(20) );
              sum1+=(*toret->OMatrix)[o][i][j];
             }
          for(int j=0;j<Osize;j++)
             {
              (*toret->OMatrix)[o][i][j]=(*toret->OMatrix)[o][i][j]/sum1;
              cout<<(*toret->OMatrix)[o][i][j]<<"  ";
		          RPOMDP<<(*toret->OMatrix)[o][i][j]<<"  ";
             }
          cout<<endl;RPOMDP<<endl;
         }
      cout<<endl;RPOMDP<<endl;
     }
  cout<<"size_NewAct="<<Asize<<"size_newS="<<Ssize<<"size_NewOb="<<Osize<<endl;
  return toret;
 }


/**********************************************
函数功能：针对一长串抽象的数据，求出经过所有数据更新后所得到的向前变量alpha【t】【i】的值

输入： 1.包含状态转移矩阵T和观测之间转移矩阵O的结构体RandomPOMDP

      2.未抽象数据的起始landmark
    
      3.一长串抽象后的数据NewAbstractData
     
      4.抽象数据的动作的个数size_NewAct
 
      5.抽象数据的观测值的个数size_NewOb
      
      6.待抽象数据的动作action

      7.待抽象数据的观测observation
  
      8.待抽象数据的初始beliefState

输出： 一长串数据对应的向前变量alpha【t】【i】

******************************************************************/
Array2D<double>*hand_pomdp::getAlpha(HandPomdpData *RandomPOMDP,symList *NewAbstractData, int size_NewS,Array1D<int>*action,Array1D<int>*observation,vector<float> *belief)
 {          
  int Size_n; //定义Size_n为每个landmark对应的抽象数据的总长度
  Size_n=NewAbstractData->size()/2;
  int N=size_NewS;//N为新定义的观测值的个数
  float pi[N];  //初始信念状态矩阵
  vector<float>::iterator be;
  int m;
  for(m=0,be=belief->begin();be!=belief->end();be++,m++)//存储landmark对应的初始信念状态 到数组pi中
  	 {
   		pi[m]=*be;  
 		 }
  symList2Vec2Map::iterator s1;
  vec2::iterator vs,vs1;
  Array2D<double>*alpha;             
  alpha=new Array2D<double>(Size_n+1,size_NewS);//定义alpha为二维数组
  *alpha = 0.0;  //数组初始化为0
  int t=0;//初始时刻
	int a0=(*action)[0];//获得动作对应的下标值
  for(int i=0;i<size_NewS;i++)//计算alpha【t】【j】
     {
      (*alpha)[0][i]=pi[i]*(*RandomPOMDP->OMatrix)[a0][i][0];
     }
  double sum0=0.0;
  for(int i=0;i<size_NewS;i++)
     {
      sum0+=(*alpha)[0][i];
     }
  for(int i = 0; i <size_NewS; i++)//对alpha[o][i]进行归一化 
     {
      (*alpha)[0][i]=(*alpha)[0][i]/sum0;
      if((*alpha)[0][i]<0.0001) (*alpha)[0][i]=0.0;
      if((*alpha)[0][i]>1.0)  (*alpha)[0][i]=1.0;
     } 
  for(t=1;t<=Size_n;t++)//对第一串数据的剩余部分
     {    
      int a1=(*action)[t-1];//获得动作对应的下标值
      int o1=(*observation)[t];//获得观测值对应的下标值
      double sum=0.0;
      for(int i=0;i<size_NewS;i++)   
         {  
          for(int j=0;j<size_NewS;j++)
             { 
              (*alpha)[t][i]+= (*alpha)[t-1][j] *( (*RandomPOMDP->TMatrix)[a1][j][i] );
             }
          (*alpha)[t][i] *=(*RandomPOMDP->OMatrix)[a1][i][o1]; 
          sum+=(*alpha)[t][i];//求出alpha对应的各项总和
         }
      for(int i=0;i<size_NewS;i++)   
         {  
          (*alpha)[t][i]=(*alpha)[t][i]/sum ;  
         }
     }
  ofstream alpah("alpha.txt");//输出alpha【t】【i】的结果
  for(int t=0;t<Size_n+1;t++)
     {
      for(int i=0;i<size_NewS;i++)
         { 
          alpah<<(*alpha)[t][i]<<" ";
         }
      alpah<<endl;
     }
  return  alpha;
 }


/**************************************************
函数功能：获取抽象数据的所有动作
函数输入：抽象数据
函数输出：抽象数据的动作
****************************************************/
Array1D<int>*hand_pomdp::getAction(symList*NewAbstractData)
 {
	int T=(NewAbstractData->size())/2;//计算一共有多少时间步
	int t,pos;
	Array1D<int>*action=new Array1D<int>(T);*action=0;
	for(pos=0,t=0;pos<2*T;pos=pos+2,t++)
	   {
		  symList*data=getMemoryL(NewAbstractData,pos,1);//获得一个动作值
		  int act=data->s->id;//找到该步动作action
		  delete data;
		  (*action)[t]=act;
	   }
  return action;
 }


/**************************************************
函数功能：获取抽象数据的所有观测
函数输入：抽象数据
函数输出：抽象数据的观测
****************************************************/
Array1D<int>*hand_pomdp::getObservation(symList*NewAbstractData)
 {
	int T=(NewAbstractData->size())/2;//计算一共有多少时间步
	int t,pos;
	Array1D<int>*observation=new Array1D<int>(T+1);*observation=0;
	(*observation)[0]=0;
	for(pos=0,t=1;pos<2*T;pos=pos+2,t++)
	   {
		  symList*data=getMemoryL(NewAbstractData,pos+1,1);//获得一个动作值
		  int obs=data->s->id;//找到该步动作action
		  delete data;
		  (*observation)[t]=obs;
	   }
	return observation;
 }


/**********************************************************

函数功能：针对每一个landmark对应的所有抽象数据，求出经过所有数据所得到的向前变量beta【t】【i】的值

输入： 1.包含状态转移矩阵T和观测之间转移矩阵O的结构体RandomPOMDP

      2.未抽象数据的起始landmark
    
      3.一长串的抽象数据的NewAbstractData
     
      4.抽象数据的动作的个数size_NewAct
 
      5.抽象数据的观测值的个数size_NewOb
      
      6.待抽象数据的动作action

      7.待抽象数据的观测observation

      8.待抽象数据的初始beliefState

输出： 一长串抽象数据的向前变量beta【t】【i】
*****************************************************************************/
Array2D<double>*hand_pomdp::getBeta(HandPomdpData *RandomPOMDP,symList*NewAbstractData,int size_NewAct,int size_NewS,Array1D<double>*scale,Array1D<int>*action,Array1D<int>*observation)
 {      
  int Size_n; //用来表示数据的总长度
  Size_n=NewAbstractData->size()/2; //Size_n为对应landmark下抽象数据的总长度   
  Array2D<double>*beta;
                                
  beta=new Array2D<double>(Size_n+1,size_NewS);//用来定义beta的二维数组
  *(beta) = 0.0;  //数组初始化为0
  for(int i=0;i<size_NewS;i++)   //开始计算beta【t】【i】
     {
      (*beta)[Size_n][i]=1.0/(*scale)[Size_n];
     } //最后一串数据的末尾初始化beta【t】【i】为1.0/scare【t】
  for(int t=Size_n-1;t>=0;t--)//从抽象数据的末尾开始
     {   
      int a0=(*action)[t];//获得动作对应的下标值
      int o0=(*observation)[t+1];//获得观测值对应的下标值
      double sum=0.0;
      for(int i=0;i<size_NewS;i++)  
         {   
          for(int j=0;j<size_NewS;j++) //计算beta[t][i]的值        
        	   {
              (*beta)[t][i]+=(*RandomPOMDP->TMatrix)[a0][i][j]*(*RandomPOMDP->OMatrix)[a0][j][o0]*(*beta)[t+1][j];
             }               
          (*beta)[t][i]=(*beta)[t][i]/(*scale)[t];
          sum+=(*beta)[t][i];
         } 
      for(int i=0;i<size_NewS;i++)  
         { 
          (*beta)[t][i]= (*beta)[t][i]/sum ;                  
         }           
     }
  ofstream betadata("beta.txt");//输出beta【T】【i】的值到txt文件中
  for(int t=0;t<Size_n+1;t++)
     { 
      for(int i=0;i<size_NewS;i++)
         { 
          betadata<<(*beta)[t][i]<<" ";
         }
      betadata<<endl;
     }      
  return beta;
 }


/********************************************************************
函数功能：计算scale【t】

输入：1.包含状态转移矩阵T和观测之间转移矩阵O的结构体RandomPOMDP

     2.一长串抽象的数据NewAbstractData

     3. 观测值的个数size_NewOb

     4.起始landmark--startPosLm
    
     5.未抽象数据的起始landmark

     6.待抽象数据的动作action

     7.待抽象数据的观测observation

输出： 一维数组scale【t】
***************************************************************************************/
Array1D<double>*hand_pomdp::getScale(HandPomdpData *RandomPOMDP,symList*NewAbstractData, int size_NewS,Array1D<int>*action,Array1D<int>*observation)
 {        
  int j;
  int Size_n=NewAbstractData->size()/2;  //定义Size_n的大小为数据长度的一半                   
  Array1D<double>*scale= new Array1D<double>(Size_n+1); //定义新的数组
  *scale=0.0; //数组初始化为0                   
  int t0=0;  //定义t0为初始时刻
	int pos=0;
	int a0=(*action)[0];//获得动作对应的下标值
	for(int j=0;j<size_NewS;j++)
     {
      (*scale)[0]+=(*RandomPOMDP->OMatrix)[a0][j][0];
     }  		   
  for(t0=1;t0<=Size_n;t0++)
     {    
      int a2=(*action)[t0-1];//获得动作对应的下标值
      int o2=(*observation)[t0];//获得观测值对应的下标值
      for(int j=0;j<size_NewS;j++)
    		 (*scale)[t0]+=(*RandomPOMDP->OMatrix)[a2][j][o2];
     }   
  ofstream scaledata("scale.txt");//输出scale的值到txt文件中
	for(j=0;j<Size_n;j++)
			scaledata<<(*scale)[j]<<" ";
      return scale;
 }


/********************************************************************
函数功能：求参数西各马，表达智能体在t时刻在状态i,t+1时刻在状态j的概率
函数输入：抽象后的数据NewAbstractData
	  随机POMDP数据包括T和O矩阵
	  动作和观测的个数
	  向前概率和向后概率
函数输出：西各马
***********************************************************************/
Array3D<double>*hand_pomdp::getSigma(HandPomdpData *RandomPOMDP,symList*NewAbstractData,int size_NewAct,int size_NewS,Array2D<double>*beta,Array2D<double>*alpha,Array1D<int>*action,Array1D<int>*observation)
 {	 
  ofstream Sigmatxt("Sigma.txt");
	int T=(NewAbstractData->size())/2;//计算一共有多少时间步
	Array3D<double>*sigma=new Array3D<double>(T,size_NewS,size_NewS);//用来定义sigma的三维数组
  *(sigma) = 0.0; 
	int i ,j,t;
	for( t = 0; t < T; t++)
		 { 
      Sigmatxt<<"t="<<t<<endl; 
      int a1=(*action)[t];//获得动作对应的下标值
      int o1=(*observation)[t+1];//获得观测值对应的下标值
      double frac = 0.0;
      for(i = 0; i < size_NewS; i++)
			   {
          for(j = 0; j < size_NewS; j++)
				     {
              frac += (*alpha)[t][i] *(*RandomPOMDP->TMatrix)[a1][i][j] * (*RandomPOMDP->OMatrix)[a1][j][o1]*(*beta)[t+1][j];
             }
         }
      for(i = 0; i < size_NewS; i++)
			   {
          for(j = 0; j <size_NewS; j++) 
				     {
            	if(frac!=0)
              (*sigma)[t][i][j] = (*alpha)[t][i] * (*RandomPOMDP->TMatrix)[a1][i][j]*(*RandomPOMDP->OMatrix)[a1][j][o1] * (*beta)[t+1][j] / frac;
              Sigmatxt<< (*sigma)[t][i][j]<<" ";                               
             }
          Sigmatxt<<endl; 
         } 
     }
	return sigma;
 }


/***************************************************
函数功能：求参数gama，表达智能体在t时刻在状态i的概率
函数输入：抽象后的数据NewAbstractData
	      动作和观测的个数
	      向前概率和向后概率
	     参数西各马(智能体在t时刻在状态i,t+1时刻在状态j的概率)
	     scale
函数输出：gama
****************************************************/
Array2D<double>*hand_pomdp::getGama(symList*NewAbstractData,int size_NewAct,int size_NewOb,Array2D<double>*beta,Array2D<double>*alpha,Array1D<double>*scale)
 {   
  ofstream Gammatxt("Gamma.txt");
	int T=(NewAbstractData->size())/2;//计算一共有多少时间步
	Array2D<double>*gama=new Array2D<double>(T+1,size_NewOb);//用来定义Gama的二维数组
  *(gama) = 0.0; 
	int i,t;
	for(t = 0; t < T+1 ; t++) 
		 {
      double frac = 0.0;
			for(i = 0; i < size_NewOb; i++)
			   {
          frac += (*alpha)[t][i] * (*beta)[t][i]*(*scale)[t];
         }
      for(i = 0; i < size_NewOb; i++) 
         {
          if(frac!=0) 
             (*gama)[t][i] =(*alpha)[t][i] * (*beta)[t][i]*(*scale)[t] / frac;
          Gammatxt<<(*gama)[t][i]<<" ";
         }
      Gammatxt<<endl;                     
     }
	return gama;
 }


/************************************************************
函数功能：更新状态转移矩阵
函数输入：抽象后的数据NewAbstractData
	      等待更新的POMDP数据包括T和O矩阵
	      动作和观测的个数
	      参数sigma(智能体在t时刻在状态i,t+1时刻在状态j的概率)
	      参数gama(表达智能体在t时刻在状态i的概率)
        待抽象数据的动作action
函数输出：状态转移矩阵更新后的POMDP结构体
************************************************************/
HandPomdpData *hand_pomdp::upDataTMatrix(HandPomdpData *RandomPOMDP,symList*NewAbstractData,int size_NewAct,int size_NewOb,Array3D<double>*sigma,Array2D<double>*gama,Array1D<int>*action)
 {
	int T=(NewAbstractData->size())/2;//计算一共有多少时间步
	int a,i,j,t,pos;
	for(a=0;a<size_NewAct;a++)
		 {
     	for( i = 0; i < size_NewOb; i++) 
			   {	  
          double frac = 0.0;
          for(t = 0; t < T ; t++) 
					   { 	
            	if((*action)[t] == a)	
              frac += (*gama)[t][i];
             }
          for( j = 0; j < size_NewOb; j++) 
					   {
              double dem = 0.0; 
              for(t = 0; t < T ; t++) 
							   {
								  if((*action)[t] == a)	  
                     dem += (*sigma)[t][i][j];
					       }
              if(frac!=0)
						    	(*RandomPOMDP->TMatrix)[a][i][j] = dem / frac;
             }
         }
     }
	return RandomPOMDP;
 }


/************************************************************
函数功能：更新观测值转移矩阵
函数输入：抽象后的数据NewAbstractData
	      等待更新的POMDP数据包括T和O矩阵
	      动作和观测的个数
	      参数gama(表达智能体在t时刻在状态i的概率)
        待抽象数据的观测observation
函数输出：状态观测值矩阵更新后的POMDP结构体
************************************************************/
HandPomdpData *hand_pomdp::upDataOMatrix(HandPomdpData *RandomPOMDP,symList*NewAbstractData,int size_NewAct,int size_newS,int size_NewOb,Array2D<double>*gama,Array1D<int>*observation)
 {
	int T=(NewAbstractData->size())/2;//计算一共有多少时间步
	int a,i,j,t,pos;	
	for(a=0;a<size_NewAct;a++)
		 {
      for(i = 0; i < size_newS; i++)
				 {
          double frac = 0.0;
          for (t = 0; t < T+1; t++)
               frac += (*gama)[t][i];
          for(j = 0; j < size_NewOb; j++) 
				     {
              double dem = 0.0;
              for(t = 0; t < T+1; t++)
							   {
				        	if((*observation)[t] == j)
                     dem += (*gama)[t][i];
                 }
              if(frac!=0)
                 (*RandomPOMDP->OMatrix)[a][i][j] = dem / frac;
             }
      	 }
     }
 	return RandomPOMDP;
 }


/*******************************************************
函数功能：利用EM算法更新POMDP模型
函数输入：包含状态转移矩阵T和观测之间转移矩阵O的结构体RandomPOMDP
        未抽象数据的起始landmark
        一长串抽象后的数据NewAbstractData
        抽象数据的动作的个数size_NewAct
        抽象数据的观测值的个数size_NewOb
        抽象数据的状态个数size_S
        抽象数据的初始信念状态belief
函数输出：更新后的POMDP模型
**********************************************************/
HandPomdpData *hand_pomdp::EM(HandPomdpData *RandomPOMDP,symList *NewAbstractData,int size_NewAct, int size_newS,int size_NewOb,vector<float> *belief)
 {  
  int iter =10; // 迭代次数
  Array1D<int>*action=getAction(NewAbstractData);
  Array1D<int>*observation=getObservation(NewAbstractData);
  while(iter >0)
	     {  
        cout<<"iter=="<<iter<<endl;
	      Array2D<double>*alpha=getAlpha(RandomPOMDP,NewAbstractData,size_newS,action,observation,belief);
	      Array1D<double>*scale=getScale(RandomPOMDP,NewAbstractData, size_newS, action,observation); //获取scale[t]的值  
 	      Array2D<double>*beta=getBeta(RandomPOMDP,NewAbstractData,size_NewAct,size_newS,scale,action,observation);
	      Array3D<double>*sigma=getSigma(RandomPOMDP,NewAbstractData,size_NewAct,size_newS,beta,alpha,action,observation);
	      Array2D<double>*gama=getGama(NewAbstractData,size_NewAct,size_newS,beta,alpha,scale);
	      RandomPOMDP=upDataTMatrix(RandomPOMDP,NewAbstractData,size_NewAct,size_newS,sigma,gama,action);
	      RandomPOMDP=upDataOMatrix(RandomPOMDP,NewAbstractData,size_NewAct,size_newS,size_NewOb,gama,observation);
	      iter--;
	      delete alpha;
	      delete  beta;
	      delete	sigma;
	      delete	gama;
	     }
  RandomPOMDP=youhua(RandomPOMDP,size_NewAct,size_newS,size_NewOb);//对POMDP模型进行优化
  outputPOMDP(RandomPOMDP, size_NewAct,size_newS,size_NewOb);//输出POMDP模型至txt文件上
  return RandomPOMDP;
 }


/************************************************
函数功能：对POMDP模型中的T，O矩阵的元素进行优化

  输入：  1.POMDP模型
 
         2.动作的个数

         3.观测值的个数

         4.状态个数

  输出：优化后的POMDP模型

*****************************************************************/
HandPomdpData*hand_pomdp::youhua(HandPomdpData *RandomPOMDP ,int size_NewAct,int size_newS,int size_NewOb)
 {        
  for(int a=0;a<size_NewAct;a++)  //对T矩阵进行优化
     {
      for(int i=0;i<size_newS;i++) 
         {  
          double sum=0.0;
          for(int j=0;j<size_newS;j++)
             { 
              if((*RandomPOMDP->TMatrix)[a][i][j]<0.001 )  
                {
                 (*RandomPOMDP->TMatrix)[a][i][j]=0.0;
                }//若值小于0.001，将其置为0.0
              sum+=(*RandomPOMDP->TMatrix)[a][i][j];                 	               
             }
          for(int j=0;j<size_newS;j++)
             {
              (*RandomPOMDP->TMatrix)[a][i][j]=(*RandomPOMDP->TMatrix)[a][i][j]/sum ; //归一化
             }
         }
     }
  for(int a=0;a<size_NewAct;a++)  //对O矩阵进行优化
     {
			for(int i=0;i<size_newS;i++) 
				 {  
					double sum1=0.0;
          for(int j=0;j<size_NewOb;j++)
             {  
							if( (*RandomPOMDP->OMatrix)[a][i][j]<0.001 )  
                {
 								 (*RandomPOMDP->OMatrix)[a][i][j]=0.0;
								}//若值小于0.001，将其置为0.0
              sum1+=(*RandomPOMDP->OMatrix)[a][i][j];                 	               
             }
          for(int j=0;j<size_NewOb;j++)
             {
              (*RandomPOMDP->OMatrix)[a][i][j]=(*RandomPOMDP->OMatrix)[a][i][j]/sum1 ; //归一化
             }
         }
     }
  return RandomPOMDP;
 }


/************************************************
函数功能：对POMDP模型中的T，O矩阵的元素输出到POMDP.TXT文件中
  输入：  1.POMDP模型
          2.动作的个数
          3.观测值的个数
          4.状态个数
  输出：POMDP.TXT
*****************************************************************/
void *hand_pomdp::outputPOMDP(HandPomdpData *RandomPOMDP,int size_NewAct,int size_newS, int size_NewOb)
{       
 ofstream POMDPTXT("POMDP.txt");
 for(int a=0;a<size_NewAct;a++)  //对O矩阵进行优化
    {   
		 POMDPTXT<<"a="<<a<<":"<<endl;   
     POMDPTXT<<"T矩阵为"<<":"<<endl;         
     for(int i=0;i<size_newS;i++) 
        {  
         for(int j=0;j<size_newS;j++)
            { 
						 POMDPTXT<<(*RandomPOMDP->TMatrix)[a][i][j]<<"  ";
					  }
         POMDPTXT<<endl;             
        }   
     cout<<endl;
     cout<<endl;         
     POMDPTXT<<"O矩阵为"<<":"<<endl;  
     for(int i=0;i<size_newS;i++) 
        {  
         for(int j=0;j<size_NewOb;j++)
            {
					   POMDPTXT<<(*RandomPOMDP->OMatrix)[a][i][j]<<"  ";
					  }
         POMDPTXT<<endl;             
        }     
    }  
}
  



/*************************************************************************************************
函数功能：求EM算法建立的POMDP模型与准确模型之间的误差值。每一步经历下计算所有感兴趣的检验的误差
函数输入：
自己建立的POMDP模型RandomPOMDP
感兴趣的检验集合symSet *interestTests
每个新的观测值对应的感兴趣的检验的概率newB_prob
每个A-O和新定义的动作的一一对应关系newAct
得到每个landmark对应的信念状态landmarkbe
Landmark集合landmarkSet
抽象数据的初始landmark startPosLm
函数输出：模型误差
*****************************************************************************************************/
double hand_pomdp::getError(HandPomdpData *RandomPOMDP,symSet *interestTests,int size_newS,vector<float> *belief)
 {  
  ofstream ERROS("ERROS.txt");
  ofstream ERRORTXT("ERROR.txt");ofstream TESTDATATXT("TESTDATATXT.txt");
  env->resert(belief);
  //vector<Symbol*> *testData=env->getData(Length_TestData);
  //env->createTestdata(Length_TestData);
  vector<Symbol*> *testData=env->getconData(); 
  vector<Symbol*>::iterator vt=testData->begin();
  for(vt=testData->begin();vt!=testData->end();vt++)
     {
      TESTDATATXT<<**vt<<" ";
     }   
  vector<float> *belief_start=new vector<float>;
  vector<float> *belief_end=new vector<float>;
  vector<float> *Handbelief_start=new vector<float>;   
  vector<float>::iterator be;
  for(be=belief->begin();be!=belief->end();be++)
     {
			Handbelief_start->push_back(*be);
    	belief_start->push_back(*be);  
			ERRORTXT<<"belifstart= "<<*be<<endl;
   	 } 

  int size_T=testData->size();
 
  double  errSum=0.0;
  int num=0;//用来计算比较的步数
 
	vector<double>::iterator vd;
  vector<float>::iterator it,is;
  symIter s; 
  for(int posT=0;posT<size_T;posT+=1)      //获取感兴趣的检验的local预测值和POMDP准確值的差值
     {
      num+=1;
      
			int i,j;float pomdpProb=0.0,pomPre=0.0;
      for(s=interestTests->begin(),i=0;s!=interestTests->end();s++,i++)//对每一个感兴趣的检验
      	 {
          pomPre=env->getPomdpProb(belief_start, *s);//求出准确POMDP检验发生的概率 
          symList*a=new symList((*s)->s);
          symList*o=new symList((*s)->n->s);
          pomdpProb=getabstractBeliefprob(RandomPOMDP,Handbelief_start,a,o,size_newS);
					ERRORTXT<<"pomPre="<<pomPre<<"                 "<<"pomdpProb="<<pomdpProb<<"                  ";       
          errSum += fabs(pomPre-pomdpProb );
					ERRORTXT<<"errSum="<<errSum<<endl;
            
         }
      symList*ao=getMemory(testData, posT, 1); //获得currpos的下一个检验
      posT+=1;
      symList*ao1=getMemory(testData, posT, 1); //获得currpos的下一个检验
      ao->append(ao1);
      vector<Symbol*> *test1=env->getVecdata(ao);//将symList型的检验转化为vector<Symbol*>型
		  ERRORTXT<<"ao= "<<*ao<<endl;
      belief_end=env->getBeliefState(belief_start,test1);//在当前信念状态belief_start下经过经历test1后的新信念状态
      delete belief_start;
		  belief_start=new vector<float>;
      for(is=belief_end->begin();is!=belief_end->end();is++)//重新初始化belief_start
 	       {
					belief_start->push_back(*is);ERRORTXT<<"belifstart= "<<*is<<endl;
				 }
			cout<<endl;
		  delete belief_end;               
      symList*newA=new symList(ao->s);
      symList*newO=new symList(ao->n->s);
      ERRORTXT<<"newA="<<*newA<<endl;
      ERRORTXT<<"newO="<<*newO<<endl;
      ERRORTXT<<"size_newS"<<size_newS<<endl;
      for(is=Handbelief_start->begin();is!=Handbelief_start->end();is++)//重新初始化Abstractbelief_start
 	       {
          ERRORTXT<<"handbelifstart= "<<*is<<endl;
				 }
      vector<float>* Handbelief_end=new vector<float>; //表示每一步结束后的状态
      Handbelief_end=getAbstractBeliefState(RandomPOMDP,Handbelief_start,newA,newO,size_newS);             
      delete newA,newO; 
      delete Handbelief_start;              //清空belief_start
		  Handbelief_start=new vector<float>;
      for(is=Handbelief_end->begin();is!=Handbelief_end->end();is++)//重新初始化Abstractbelief_start
 	       {
					Handbelief_start->push_back(*is);
          ERRORTXT<<"handbelifend= "<<*is<<endl;
				 }
	    delete Handbelief_end ;     
      if(num==5000) break;        
     }
  errSum /= ((interestTests->size()) *num);    
  cout<<"errSum="<<errSum<<endl;
  ERROS<<"errSum="<<errSum<<endl;
  return errSum;                 
 }
/*************************
	次数NUM++
	1.从NOW-NEWO->ID作为本次的状态i
	2.利用symList_symListDblMap *newB_prob找到新状态i对应的各个感兴趣的检验的概率prob
	3.for(从感兴趣的检验集合interestTests的第一个到最后一个t)
		{
			计算准确的belief_start下感兴趣的检验t出现的概率(pomPre=env->getPomdpProb(belief_start, t);  
			prob中找t对应的概率 AbstractPre;
			errSum += pow( (psrPre-AbstractPre), 2 );
		}
       4.得到当前POST后一步ao，求出在此步ao后准确的pomdp模型的新belief_start（env->getBeliefState(belief_start,ao);
       5.得到ao=NEWA，for(从新观测的第一个到最后一个NEWO）
			{
				计算Abstractbelief_start下，NEWANEWO发生的概率(写成函数）
			}
		     概率最大的NEWO即是下一个时刻的新NOW-NEWO
       6. if(num==10000)break;
***************************/


/*************************************************************************************************
函数功能：抽象POMDP，在当前信念状态下，采取抽象动作newA,得到观测obs的概率
函数输入：
抽象POMDP
抽象数据的当前信念状态
采取的抽象动作
得到的抽象观测
抽象数据状态个数
输出:抽象POMDP，在当前信念状态下，采取抽象动作newA,得到观测obs的概率
*****************************************************************************************************/
float  hand_pomdp::getabstractBeliefprob(HandPomdpData *RandomPOMDP,vector<float>*belief ,symList*newA,symList*obs,int size_NewS)
 {  
  float pomdpProb;
  float pomdpProb1=1.0;
  int a,o,i,j; 
  vector<float>* belief_start= new vector<float>;//表示每一步开始的状态
  vector<float>* belief_end=new vector<float>;   //表示每一步结束后的状态
  vector<float>::iterator it,is;
  for(is=belief->begin();is!=belief->end();is++) //初始化belief_start，把当前belief状态赋值给belief_start
     {
      belief_start->push_back(*is);
     }
  for(i=0;i<size_NewS;i++)  //初始化belief_end
     {
      belief_end->push_back(0.0);
     }
  pomdpProb=0.0;
  a=newA->s->id;  //当前步采取的动作的id
  o=obs->s->id;  //当前步获得的观测值的id 
  for( i=0,it=belief_end->begin();it!=belief_end->end();it++,i++)
     {
      for( j=0,is=belief_start->begin();is!=belief_start->end();j++,is++)
	    	 {   
		      *it=*is*( (*RandomPOMDP->TMatrix)[a][j][i] )*( (*RandomPOMDP->OMatrix)[a][i][o]);
          pomdpProb=*it+pomdpProb;
         }
     }           //计算在当前步的belief_start下，经历该步的ao后的新状态belief_end，以及该步发生的概率pomdpProb
  pomdpProb1=pomdpProb1*pomdpProb;
  delete belief_start;
  delete belief_end;
  return pomdpProb1;
 }


/*************************************************************************************************
函数功能：计算抽象数据的信念状态
函数输入：
抽象POMDP
抽象数据的当前
采取的抽象动作
得到的抽象观测
抽象数据状态个数
输出:在当前信念状态下，采取抽象的动作和观测得到的新的信念状态
*****************************************************************************************************/
vector<float> *hand_pomdp::getAbstractBeliefState(HandPomdpData *RandomPOMDP,vector<float>*belief ,symList*newA,symList*obs,int size_NewS)
 {
  int a,o,i,j;  
  vector<float>* belief_end=new vector<float>;      //每一步结束后的信念状态
  vector<float>* belief_start= new vector<float>;   //每一步开始的信念状态
  vector<float>::iterator it,is;
  for(i=0;i<size_NewS;i++)                               //初始化belief_end
 		 {
		  belief_end->push_back(0.0);
     }
  for(is=belief->begin();is!=belief->end();is++)    //初始化belief_start
 	   {
			belief_start->push_back(*is);
     }
  a=newA->s->id;                            //当前步采取的动作的id
  o=obs->s->id;                             //当前步观测值的id
  for( i=0,it=belief_end->begin();it!=belief_end->end(),i<size_NewS;it++,i++)
	   {	
      for( j=0,is=belief_start->begin();j<size_NewS,is!=belief_start->end();j++,is++)
	       {
          *it=*is*( (*RandomPOMDP->TMatrix)[a][j][i] )*( (*RandomPOMDP->OMatrix)[a][i][o] )+*it;
         }
     }                                       //计算在当前步的belief_start下，经历该步的ao后的新状态belief_end
  float normal=0.0;                       //归一因子
  for(it=belief_end->begin();it!=belief_end->end();it++)
		 { 
			normal=normal+(*it);
		 }
	for(it=belief_end->begin();it!=belief_end->end();it++)//归一化
		 {
      *it=(*it)/normal;
     }
  delete belief_start;
  return belief_end;
 }






hand_pomdp::hand_pomdp(Environment *e): psr(e)
{

}


hand_pomdp::~hand_pomdp() 
{   }
#endif

