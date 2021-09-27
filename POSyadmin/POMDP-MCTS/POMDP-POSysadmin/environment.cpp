/*
************************************************************************
*                           厦门大学系统工程研究所
*文件名:environment.cpp
*文件描述：类 Environment，主要具体环境来确定各个函数的具体功能和所
*对应的不同状态，动作等。
*************************************************************************
*/

#include <fstream>
#include <iostream>
#include <time.h>
#include <vector>
#include "unistd.h"
#include "environment.h"

#include "psr-sysdyn.h"
#include"symbol.h"
#include "cassandra_grammar.h"


using namespace std;


/************************************************************
 函数功能：得到每一个landmark对应的信念状态
 输入：landmark的集合
 输出：所有landmark对应的信念状态集合
/************************************************************/
symList2VfMap *Environment::getLMbelief(symSet* landmarkSet)
{
  char name[80];
  cout <<"the format of input filename is:/environment-LMState.txt"<<endl;
  cout<<"please input filename of landmarkState:";
  cin >>name;                       //输入存放对应landmark的状态的文件
  cout<<name<<endl;
  char buffer[160];
  getcwd(buffer, 160);           
  printf("The current directory is: %s\n", buffer);
  char *workdir=strcat(buffer,name); 
  cout<<workdir<<endl;  
  ifstream in(workdir);//读入文件绝对路徑
  vector<int> landmarkStateId;
  for(int id;in>>id;)//读取输入文件内容
 		 {
      landmarkStateId.push_back(id);
  	 }
  vector<int>::iterator vid=landmarkStateId.begin();
  for(vid=landmarkStateId.begin();vid!=landmarkStateId.end();vid++)
  	 {
   	  cout<<"landmarkStateId="<<*vid<<endl;
  	 }
  symList2VfMap *landmarkbe=new symList2VfMap;
  symIter iCurr;
  for(iCurr=landmarkSet->begin();iCurr!=landmarkSet->end();iCurr++)//初始化landmarkbe，为存放各个landmark对应的状态分配空间
  	 {
    	vector<float> *belief=new vector<float>;
    	landmarkbe->insert(pair<symList*,vector<float>*>(new symList(*iCurr),belief));
 		 }
  map<symList*,vector<float>*>::iterator lmb=landmarkbe->begin(); 
  int i;
  //int x;
  //x = landmarkbe->size();
  //cout << "landmarkbe->size= " << x << endl;
  for(lmb=landmarkbe->begin(),i=0;lmb!=landmarkbe->end();lmb++,i++)//将各个landmark对应的状态存到landmarkbe中
     {  
      int j=landmarkStateId.at(i);
      int k;
      for(k=0;k<j;k++)
         {
          lmb->second->push_back(0.0);
         }
      lmb->second->push_back(1.0);//landmark对应的信念状态,landmark所在位置为1
      for(k=0;k<(S_sz-j-1);k++)
         {
          lmb->second->push_back(0.0);
         }
      landmarkbe->insert(pair<symList*,vector<float>*>(lmb->first,lmb->second));
     }
  return landmarkbe;
 }


/*******************************************************************************************/
/*函数功能： 
  对完整数据Data利用landmark划分为不同的状态空间，把所有以相同landmark开头的数据存在一起（TY）
  输入：训练数据
  输出：划分后的状态空间
/*******************************************************************************************/
symList2Vec2Map* Environment::divideData(vector<Symbol*>* Data)
{
  int size=Data->size();                    //size为训练数据的长度
  cout<<"Data->size()="<<Data->size()<<endl;
  symSet *landmarkSet= new symSet();
  symList *landmark;
  Symbol *mark;                             //mark为数据中的观测值缓存
  symList2Vec2Map *Matrix = new symList2Vec2Map;
  symbol_type LANDMARK=SYM_LANDMARK;        //symbol.h中定义了symbol_type，观测值的类型，当观测值的类型为LANDMARK时，则该观测是landmark
  cout<<"SYM_LANDMARK="<<LANDMARK<<endl;
  for(int k=0;k<O_sz;k++)                   // 获得landmark的集合.如果观测值的类型为LANDMARK，则把这个观测存在landmark的集合中
     {
      if(O[k]->type==LANDMARK)
    		{
      	 landmark=new symList(O[k]);
      	 landmarkSet->insert(landmark);
    		}
  	 }
  int lmSize=(*landmarkSet->begin())->size();//landmark的长度
  symIter s;
  for(s=landmarkSet->begin();s!=landmarkSet->end();s++)
  	 {
   	  vector<symList*>* elementVec=new vector<symList*>;
    	Matrix->insert(pair<symList*, vec2*>(new symList(*s),elementVec ));//初始化存放划分后数据的空间
  	 }   
  for(int pos=1;pos<size-2;pos+=2)  //Data中的每一个观测值与landmark进行比较,每找到一个landmark则把该landmark到下一个landmark之间的数据存放在该landmark对应的状态空间中
 		 {  
      symList *currData= new symList();
      symList *currMemory1=getMemory(Data,pos,lmSize);        //此函数在sample.cpp中可以找到，获取Data在位置POS处长度为lmSize的数据
      if(landmarkSet->find(currMemory1) != landmarkSet->end())//表示当前观测值currMemory1是landmark
      	{    
         landmark=currMemory1;
         pos=pos+2;
         symList *currMemory2=getMemory(Data,pos,lmSize);    //currMemory2为紧接着currMemory1的观测值
         mark=currMemory2->s;   //把symList型的currMemory变为Symbol型的mark,判断landmark后一个观测值currMemory2是否为landmark
         while(mark->type!=LANDMARK&&pos!=size-1)            //如果landmark后一个观测值currMemory2不是landmark且没有到达数据末端 
      			  { 
         			 currData->append( new symList(Data->at(pos-1), new symList(Data->at(pos))) );//把currMemory2对应o及前一个a放入currData         
           		 pos=pos+2;
          		 symList *currMemory3=getMemory(Data,pos,lmSize); //继续找到下一个观测，循环判断是否为landmark，如果不是则把该观测和动作加到currData中
               mark=currMemory3->s;
               delete currMemory3;
              }
         delete currMemory2;
         if(mark->type==LANDMARK) // 从该landmark不断向后寻找，直到遇到下一个landmark，则把两个landmark之间的数据存放到对应的状态空间中
       		 {
            currData->append( new symList(Data->at(pos-1), new symList(Data->at(pos))) );//把currMemory对应o及前一个a放入currData        
          	symList2Vec2Map::iterator vv=Matrix->begin();
          	for(vv=Matrix->begin();vv!=Matrix->end(); vv++)
	 						 {
            		if(*landmark==*(vv->first))                    //把通过landmark划分后的数据相应存放在landmark对应的数据组中
            			{  
               		 vv->second->push_back(currData);
               		 Matrix->insert(pair<symList*, vec2*>(vv->first,vv->second));
            			}
          		 }
            if(pos!=size-1)  //还未到达数据最后一个观測值位置
               pos=pos-2;    //因为上面的pos=pos+2了
            else             //到达数据最后一个观測值位置
               break; 
           }
        } 
      delete currMemory1;
     }
  cout<<'\n'<<endl; 
  for(s=landmarkSet->begin();s!=landmarkSet->end();s++)
  	 {
      delete *s;
 		 }
  delete landmarkSet;      //在调用函数中再delete elementVec
  symList2Vec2Map::iterator vv=Matrix->begin();
  cout<<"The Matrix:"<<endl;
  for(vv=Matrix->begin();vv!=Matrix->end(); vv++)
 		 {
    	cout<<"landmark="<<*(vv->first)<<endl;
    	for(int j=0;j<vv->second->size();j++)
    	cout<<"list"<<j<<"="<<**(vv->second->begin()+j)<<endl;
     	cout<<'\n'<<endl;
  	 }
  cout<<'\n'<<endl;  
  return Matrix;
}


/****************
函数功能：得到对应环境下的动作个数
输入：无
输出：动作值的个数
*******************/
int Environment::nrActions() { return A_sz; }


/****************
函数功能：得到对应环境下的观测值个数
输入：无
输出：观测值的个数
*******************/
int Environment::nrObservs() { return O_sz; }


/******************************
函数功能： 判断输入的整数i是否小于该环境下对应的动作个数，若小于，则返回对应下标为i的动作，反之则返回NULL
输入：动作的下标
输出：下标为i的动作或NULL
*********************************/
Symbol *Environment::getAction(int i) { return i<A_sz ? A[i] : NULL; }


/***********************
函数功能： 判断输入的整数i是否小于该环境下对应的观测值个数，若小于，则返回对应下标为i的观测值，反之则返回值为空
输入：观测值的下标
输出：下标为i的观测值或NULL
****************************/
Symbol *Environment::getObservation(int i) { return i<O_sz ? O[i] : NULL; }


/**********************
函数功能： 获得环境下所有动作的连续序列（例如环境中有4个动作a1、a2、a3、a4，则得到a1-a2-a3-a4）
输入：无
输出：连续的动作序列
**********************/
symList *Environment::getActions() 
{
  int i;
  symList *toret = NULL;
	
  for (i=A_sz-1; i>=0; i--) toret = new symList(A[i], toret);
  return toret;
}

/*********************
函数功能： 获得环境下所有观测值的连续序列（例如环境中有4个观测值O1、O2、O3、O4，则得到O1-O2-O3-O4）
输入：无
输出：连续的观测值序列
******************/
symList *Environment::getObservations() 
{
  int i;
  symList *toret = NULL;	
  for (i=O_sz-1; i>=0; i--) 
       toret = new symList(O[i], toret);
  return toret;
}


/***********************************************************************
函数功能：
  设计函数getPomdpProb，vector<float>*belief,表示当前的belief状态，var表示
  动作-观测序列，可计算在当前belief state下，观测序列var出现的概率(TY)    
 输入：当前信念状态belif，经历的动作-观测序列var
 输出：当前belief state下，观测序列var出现的概率
************************************************************************/
float Environment::getPomdpProb(vector<float>*belief, symList *var)
{
  float pomdpProb;
  float pomdpProb1=1.0;
  int a,o,i,j;
  int size_var=var->size();                      //输入的动作-观测序列的长度  
  vector<float>* belief_start= new vector<float>;//表示每一步开始的状态
  vector<float>* belief_end=new vector<float>;   //表示每一步结束后的状态
  vector<float>::iterator it,is;
     for(is=belief->begin();is!=belief->end();is++) //初始化belief_start，把当前belief状态赋值给belief_start
     		{
       	 belief_start->push_back(*is);
     		}
     for(i=0;i<S_sz;i++)  //初始化belief_end
     		{
       	 belief_end->push_back(0.0);
     		}
     for(int n=0;n<size_var+2;)
     		{
      	 pomdpProb=0.0;
       	 symList *test=getTestL(var, 0, 1);
      	 a=var->s->id;  //当前步采取的动作的id
       	 var=var->n;n++;
      	 o=var->s->id;  //当前步获得的观测值的id
       	 var=var->n;n++;   
         for( i=0,it=belief_end->begin();it!=belief_end->end(),i<S_sz;it++,i++)
          	{
           	 for( j=0,is=belief_start->begin();j<S_sz,is!=belief_start->end();j++,is++)
	    					{
            		 //*it=*is*(ret->Tr[a][j][i])*(ret->Ob[a][i][o]);pomdpProb=*it+pomdpProb;
            		 *it=*is*(Tr[a][j][i])*(Ob[a][i][o]);pomdpProb=*it+pomdpProb;
            		}
          	}           //计算在当前步的belief_start下，经历该步的ao后的新状态belief_end，以及该步发生的概率pomdpProb
      	 pomdpProb1=pomdpProb1*pomdpProb;                  
      	 if(pomdpProb1==0) //概率为0，该观测序列不可能发生，结束计算
       		 {           
        		delete test;
        		delete belief_start;
        		delete belief_end;
        		return pomdpProb1;
       		 }
     		 if(n==size_var)//数据结束，输出计算结果
       		 {
        		delete test;
       		  delete belief_start;
        		delete belief_end;
        		return pomdpProb1;
       		 }
      	 else         //若数据没结束，则把此部计算的belief_end作为下次计算的belief_start
       		 {
       		  vector<Symbol*> *test1=getVecdata(test);//把symList*test转换为vector<Symbol*> *test1
        		delete test;
        		delete belief_end;
        		belief_end=getBeliefState(belief_start ,test1);
        		delete belief_start;
        		belief_start=new vector<float>;
          	for(it=belief_end->begin();it!=belief_end->end();it++)
         			 {
           			belief_start->push_back(*it);
         			 }
           	delete belief_end;
       			belief_end=new vector<float>;
         		for(i=0;i<S_sz;i++)//初始化belief_end
         			 {
          			belief_end->push_back(0.0);
         			 }
        		delete test1;
     			 }	    
      }
}


/**************************************************************************
  函数功能：
  计算智能体在当前信念状态belief下经过经历var后，得到观测值后的信念状态(TY)
  输入：
  当前信念状态belif，经历的动作-观测序列var
  输出：
  在当前信念状态belief下经过经历var后的新信念状态
/**************************************************************************/
vector<float> *Environment::getBeliefState(vector<float>*belief ,vector<Symbol*> *var)
{ 
  int a,o,i,j;  
  int size=var->size();                             //输入经历的长度
  vector<float>* belief_end=new vector<float>;      //每一步结束后的信念状态
  vector<float>* belief_start= new vector<float>;   //每一步开始的信念状态
  vector<float>::iterator it,is;
  for(i=0;i<S_sz;i++)                               //初始化belief_end
 	{belief_end->push_back(0.0);}
  for(is=belief->begin();is!=belief->end();is++)    //初始化belief_start
 		 {
   		belief_start->push_back(*is);
 		 }
  int len=1;
  for(int pos=0;pos<size;)
  	 { 
      symList *act = getMemory(var, pos, 1);pos++;
      a=act->s->id;                             //当前步采取的动作的id
      symList *obs=getMemory(var,pos,1);pos++;
      o=obs->s->id;                             //当前步观测值的id
      delete act;
      delete obs;
      for( i=0,it=belief_end->begin();it!=belief_end->end(),i<S_sz;it++,i++)
	  		 {	
          for( j=0,is=belief_start->begin();j<S_sz,is!=belief_start->end();j++,is++)
	      		 {
              //*it=*is*(ret->Tr[a][j][i])*(ret->Ob[a][i][o])+*it;				
              *it=*is*(Tr[a][j][i])*(Ob[a][i][o])+*it;				
             } 
							
         }     
				
                                  //计算在当前步的belief_start下，经历该步的ao后的新状态belief_end
      float normal=0.0;                       //归一因子
      for(it=belief_end->begin();it!=belief_end->end();it++)
				 { 
					normal=normal+(*it);
				 }
	 		for(it=belief_end->begin();it!=belief_end->end();it++)//归一化
		 		 {
					*it=(*it)/normal;
				 }
	 		if(pos==size)                         //数据结束，输出计算结果
	 			{ 
         delete belief_start;
         return belief_end;
        }
	    else                                   //若数据没结束，则把此部计算的belief_end作为下次计算的belief_start
	 			{
         belief_start->clear();
         for(is=belief_end->begin();is!=belief_end->end();is++)//初始化belief_start
 		   			{
						 belief_start->push_back(*is);
						}
	 	 		 for(it=belief_end->begin();it!=belief_end->end();it++)//初始化belief_end
 		    		{
						 *it=0.0;
						}	
        }
     }
  
}


/**************************************************************
  函数功能:
  获得长度为Len的连续的ao序列(TY)
  输入：希望得到的动作-观测序列的长度
  输出：给定长度的动作-观测序列
/**************************************************************/
vector<Symbol*>*  Environment::getData(int Len)
{ 
  	vector<Symbol*>* continueData=new vector<Symbol*>;
  	Symbol *actionToSend, *obsReceived;
  	int prob;
  	for(int i=0; i<Len; i++)               //循环Len次，每次随机生成一个动作，再调用sendAction函数来获得观测
  	{    
      	do
    	{
      	 	prob = (int) trunc((float) random() * (float) (nrActions()) / (float) RAND_MAX);
   		} 
      	while ((prob==nrActions()));
    		actionToSend = getAction(prob);         //随机生成一个动作  
    	continueData->push_back(actionToSend);
    	obsReceived = sendAction(actionToSend); //实行这个动作产生了一个观测
    	continueData->push_back(obsReceived);   //将该步得到到动作-观测序列push_back到最终返回值continueData中
	}
	return continueData;
}

vector<Symbol*>*  Environment::getData1(vector<float>*belief, int Len)
{ 
	resert(belief);
  	vector<Symbol*>* continueData=new vector<Symbol*>;
  	Symbol *actionToSend, *obsReceived;
  	int prob;
  	for(int i=0; i<Len; i++)               //循环Len次，每次随机生成一个动作，再调用sendAction函数来获得观测
  	{    
      	do
    	{
      	 	prob = (int) trunc((float) random() * (float) (nrActions()) / (float) RAND_MAX);
   		} 
      	while ((prob==nrActions()));
    		actionToSend = getAction(prob);         //随机生成一个动作  
    	continueData->push_back(actionToSend);
    	obsReceived = sendAction(actionToSend); //实行这个动作产生了一个观测
    	continueData->push_back(obsReceived);   //将该步得到到动作-观测序列push_back到最终返回值continueData中
	}
	return continueData;
}

/**************************************************************
  函数功能:
  获得长度为Len的连续的ao序列(TY)
  输入：希望得到的动作-观测序列的长度,和固定的动作序列
  输出：给定长度的动作-观测序列
/**************************************************************/
symList*  Environment::getData2(int Len,symList*actList)
{ 
  symList* continueData=new symList();
  Symbol *actionToSend, *obsReceived;
  for(int i=0; i<Len; i++)               //循环Len次，每次随机生成一个动作，再调用sendAction函数来获得观测
  	 {
    	actionToSend = actList->s;         //随机生成一个动作  
    	continueData->append(actionToSend);
    	obsReceived = sendAction(actionToSend); //实行这个动作产生了一个观测
    	continueData->append(obsReceived);   //将该步得到到动作-观测序列push_back到最终返回值continueData中
    	actList=actList->n;//删除首个动作
  	 }
  return continueData;
}


/**************************************************************
  函数功能:
  函数功能:获得一组数量为count,指定从某状态开始并且从某状态结束的连续的ao序列，并将这组数据首尾相接
  输入：希望得到ao序列的数量,初始信念状态以及其对应的起始观测值
  输出：一串某状态开始并且从某状态结束的动作-观测序列
/**************************************************************/
vector<Symbol*>*  Environment::getData3(symList *startPosLm,vector<float>*belief,int count)
{ 
  symList* continueData=new symList();
  vector<Symbol*>* continueData1=new vector<Symbol*>;
  symList* passData=new symList();
  Symbol *actionToSend, *obsReceived;
  int prob;int i=0,j=0;
  while(i<count) 
  {   
        do
    	{
      		 prob = (int) trunc((float) random() * (float) (nrActions()) / (float) RAND_MAX);
    	}
		while ((prob==nrActions()));
    		actionToSend = getAction(prob);         //随机生成一个动作  
    		passData->append(actionToSend);
    		obsReceived = sendAction(actionToSend); //实行这个动作产生了一个观测
    		passData->append(obsReceived);
    		if(passData->size()>=200) {resert(belief);delete passData;passData= new symList();continue;}
    		if((obsReceived->id)==(startPosLm->s->id))
       		{    
	   			 if((passData->size()<30))continueData->append(passData);
	   			 if((passData->size()>30)&&(passData->size()<110)){i++;continueData->append(passData); }
	 				 passData= new symList();
        	 resert(belief);
       		}   
  }
  continueData1=getVecdata(continueData);
  return continueData1;
}

vector<symList*>* Environment::getData5(vector<float>*belief, int count, int length)
{
	vector<symList*>* conData = new vector<symList*>;
	Symbol *actionToSend, *obsReceived;
	ofstream getdiviData("getdivData.txt");
	int prob;
	for(int i = 0; i < count; i++)
	{
		resert(belief);
		getdiviData << "List : " << i << endl;
		getdiviData << "S : " << currState->id << endl;
		symList* currData = new symList();
		for(int j = 0; j < length; j++)
		{
			do
    		{
      			prob = (int) trunc((float) rand() * (float) (nrActions()) / (float) RAND_MAX);
    		} 
    		while ((prob==nrActions()));
    		
    		actionToSend = getAction(prob);
    		currData->append(actionToSend);
    		getdiviData << *actionToSend << " ";
    		
    		obsReceived = sendAction(actionToSend); 
    		currData->append(obsReceived);
    		getdiviData << *obsReceived << endl;
    	}
		conData->push_back(currData);
	}
	
	return conData;	
}
/**************************************************************
函数功能：将智能体的初始状态设定为当前需要的信念状态
函数输入：指定信念状态
函数输出：无
**************************************************************/
void Environment:: resert(vector<float>*belief)
{
  vector<float>::iterator vf;
  float maxdata=0.0;int i,j;
  for(i=0,vf=belief->begin();vf!=belief->end();vf++,i++)
    {   
    // cout<<"*vf="<<*vf<<endl;  
		 if(*vf>maxdata)
        {
         j=i;
         //cout << i << endl;
         maxdata=*vf;
				}
    }
  currState = S[j];
  //cout<<"s======"<<j<<endl;
  prevState = NULL;
}


/*******************************************************
 函数功能：
 把vector<Symbol*>型的动作-观测序列数据转换成symList型
 输入：vector<Symbol*>型的动作-观测序列
 输出：symList型的动作-观测序列
/*******************************************************/
symList*  Environment::getDataList(vector<Symbol*>* var)
{
  symList *dataList=new symList();
  vecsymIter vi;
  for(vi=var->begin();vi!=var->end();vi++)  //从容器的第一个数据开始利用append函数依次连接到dataList中，实现类型的转换
  	 {
    	dataList->append(new symList(*vi));
  	 }
  return dataList;
}


/*******************************************************
 函数功能：
 把symList型的动作-观测序列数据转换成vector<Symbol*>型
 输入：symList型的动作-观测序列 
 输出：vector<Symbol*>型的动作-观测序列
/*******************************************************/
vector<Symbol*>*  Environment::getVecdata(symList* var)
{
  vector<Symbol*>* testData=new vector<Symbol*>;
  Symbol* mark;
  int size_var=var->size();
  for(int i=0; i<size_var; i++)          //把symList型的数据从开头到结尾一个个拆分为Symbol型，依次存在容器中，实现类型的转换
     {
      mark=var->s;
      testData->push_back( mark) ;
      var=var->n;
     }
  return testData;
}


/****************************************************************************************************
  函数功能：
  给定一个的Symbol型的动作a，求出经过a后从当前状态s1得到的状态s2，并根据s2得到经过动作a后获得的观测值
  输入：给定的动作
  输出：智能体在当前状态下采取给定的动作最终产生的观测值
/****************************************************************************************************/
Symbol* Environment::sendAction(Symbol* act)     
{    
  float prob, t;
  int a /* action */ , s1 /* state */, s2, o;
  a = act->id;       //动作下标值
  s1 = currState->id;//currState下标值
  prob = (float) random() * 100.0 / (float) RAND_MAX;//产生0-100的随机数
  s2 = 0; t = 0.0;
  //while ((prob > t + ret->Tr[a][s1][s2]*100) && (s2 < S_sz))//根据产生的随机值得到变化后的状态s2
  while ((prob > t + Tr[a][s1][s2]*100) && (s2 < S_sz))//根据产生的随机值得到变化后的状态s2
  {
    //t += ret->Tr[a][s1][s2]*100; s2++;
    t += Tr[a][s1][s2]*100; s2++;
  }
  if (s2 == S_sz) 
      s2 = S_sz-1;// 若s2的值与状态的个数相等, 令s2指向最后一个状态
  prevState = currState;
  currState = S[s2];//将s2的值作为状态s的下标，并将s作为新的currState，原先的currState作为prevState
  prob = (float) random() * 100.0 / (float) RAND_MAX;//产生0-100的浮点型随机数
  o = 0; t = 0.0;
  //while ((prob > t + ret->Ob[a][s2][o]*100) && (o < O_sz))
  while ((prob > t + Ob[a][s2][o]*100) && (o < O_sz))
  { 
    //t += ret->Ob[a][s2][o]*100; o++;//根据输入的a和得到的变化后的状态s2获得观测值
    t += Ob[a][s2][o]*100; o++;//根据输入的a和得到的变化后的状态s2获得观测值
  }
  if (o == O_sz) 
      o = O_sz-1;//若o的值与状态的个数相等，将下标值指向最后一个o
  return O[o];//返回观测值o
}


/****************************************************************************
 函数功能：
 首先可以自己设定landmark的总数，然后输入希望设置为landmark的观测值的脚标，
 将该观测值的类型改为SYM_LANDMARK，实现landmark的设定
 输入：所有的观测值，类型都是SYM_OBSERVATION
 输出：所有的观测值，一部分特殊的观测值（landmark)的类型为SYM_LANDMARK
/***************************************************************************/
Symbol **Environment::setLandmark(Symbol **Obs)
{
  int j,i; 
  cout<<"input the total number of landmark"<<endl;
  cin>>j;cout<<"j="<<j<<endl;              //设定landmark的总数
  for(j;j>0;j--)
  	 {   
   		cout<<"please input landmark id"<<endl;
   		cin>>i;cout<<"i="<<i<<endl;
   		//Obs[i] = new Symbol(i, ret->arr_obs[i],SYM_LANDMARK);//改变该观测值的数据类型
   		Obs[i] = new Symbol(i, arr_obs,SYM_LANDMARK);//改变该观测值的数据类型
     } 
  return Obs;
}


/*******************************************************************************
 读取文件PsrDataId.txt的用来进行PSR建模的训练数据存放到vector<Symbol*>*conData中
 输入：无
 输出：训练数据
/*******************************************************************************/
vector<Symbol*>*  Environment::getconData()
{
  symSet *landmarkSet= new symSet();
  symList *landmark;
  symbol_type LANDMARK=SYM_LANDMARK;
  for(int k=0;k<O_sz;k++)    // 获得landmark的集合
  	 {
    	if(O[k]->type==LANDMARK)
    		{
      	 landmark=new symList(O[k]);
      	 landmarkSet->insert(landmark);
    		}
  	 }
  set<int> LmId;             //存储landmark对应的id
  symIter s;
  for(s=landmarkSet->begin();s!=landmarkSet->end();s++)
  	 {
    	landmark=*s;
    	int id=landmark->s->id;    
    	LmId.insert(id);                 //将landmark的id存储到LmId中
  	 }
  set<int>::iterator li=LmId.begin();
  char name[80];  //存放PSR训练数据的文件名,存放的是Symbol对应的id(int)
  cout<<"please input filename:";                          //输入POMDP文件名
  cin >>name;                       
  cout<<name<<endl;  
  char buffer[160];
  getcwd(buffer, 160);          
  printf("The current directory is: %s\n", buffer);
  char *workdir=strcat(buffer,name);  
  cout<<workdir<<endl;  
  ifstream in(workdir);
  vector<int> symbolId;
  for(int id;in>>id;)
  	 {
   	  symbolId.push_back(id);
 		 }
  int n=symbolId.size();
  vector<Symbol*> *conData=new vector<Symbol*>;
  for(int i=0;i<n-1;i+=2)
  	 {
     	int a=symbolId.at(i);
     	//Symbol *act = new Symbol(a, ret->arr_act[a], SYM_ACTION);//cout<<"act="<<*act<<endl;
     	Symbol *act = new Symbol(a, arr_act, SYM_ACTION);//cout<<"act="<<*act<<endl;
     	conData->push_back(act);     //得到动作a
     	i++;
     	int o=symbolId.at(i);        //得到观测值o,o的type有两种情况:普通的观測值(type=SYM_OBSERVATION==1)和landmark对应的观測值
      if(LmId.find(o)!=LmId.end()) //若观测值为landmark
     		{
         //Symbol *obs= new Symbol(o, ret->arr_obs[o],SYM_LANDMARK);//cout<<"landmark--obs="<<*obs<<endl;
         Symbol *obs= new Symbol(o, arr_obs,SYM_LANDMARK);//cout<<"landmark--obs="<<*obs<<endl;
         conData->push_back(obs);
     		}
     	else                         //若观测值不是landmark
     		{
         //Symbol *obs= new Symbol(o, ret->arr_obs[o],SYM_OBSERVATION);//cout<<"obs="<<*obs<<endl;
         Symbol *obs= new Symbol(o, arr_obs,SYM_OBSERVATION);//cout<<"obs="<<*obs<<endl;
         conData->push_back(obs);
     		}
      i--;
   	 } 
  for(s=landmarkSet->begin();s!=landmarkSet->end();s++)
   	 {
      delete *s;
   	 }
  delete landmarkSet;
  return conData;
}


/****************************************************************************
 函数功能：利用getData3()生成一串训练数据，并且存在名为Psrdataid.txt文件中
函数输入：需要生成的训练数据的长度
函数输出：一个txt文件
/***************************************************************************/
void Environment::createPsrdata(symList *startPosLm,vector<float>*belief,int Length_Data)
{
  ofstream PsrDataId  ("Psrdataid.txt");//获取固定长度的获取PSR模型所需的数据----20130822czz---每次获取模型之前都要生成这个数据,在正式运行时间之前先运行下面一段,生成数据之后再注释掉
  ofstream PsrData  ("Psrdata.txt");
  vector<Symbol*> *conData=getData3(startPosLm,belief,Length_Data);//获取PSR模型的数据长度
  vector<Symbol*>::iterator vi=conData->begin();
  for(vi=conData->begin();vi!=conData->end();vi++)
  	 {
      PsrData<<**vi<<endl;
   	 }
  vector<int> symbolId;
  for(int pos=0;pos<conData->size();)
  	 {
   	  symList *act = getMemory(conData, pos, 1);pos++;
    	int aId=act->s->id;//当前步采取的动作的id
    	symbolId.push_back(aId);
    	PsrDataId<<aId<<endl;
   	  delete act;
    	symList *obs=getMemory(conData,pos,1);pos++;
    	int oId=obs->s->id;//当前步观测值的id
   	  symbolId.push_back(oId);
    	PsrDataId<<oId<<endl;
    	delete obs;
  	 }
  delete conData;
}



void Environment::createPsrHanddata(int Length_Data)
{
  ofstream PsrDataId  ("Psrdataid.txt");//获取固定长度的获取PSR模型所需的数据----20130822czz---每次获取模型之前都要生成这个数据,在正式运行时间之前先运行下面一段,生成数据之后再注释掉
  ofstream PsrData  ("Psrdata.txt");
  vector<Symbol*> *conData=getData(Length_Data);//获取PSR模型的数据长度
  vector<Symbol*>::iterator vi=conData->begin();
  for(vi=conData->begin();vi!=conData->end();vi++)
  	 {
      PsrData<<**vi<<endl;
   	 }
  vector<int> symbolId;
  for(int pos=0;pos<conData->size();)
  	 {
   	  symList *act = getMemory(conData, pos, 1);pos++;
    	int aId=act->s->id;//当前步采取的动作的id
    	symbolId.push_back(aId);
    	PsrDataId<<aId<<endl;
   	  delete act;
    	symList *obs=getMemory(conData,pos,1);pos++;
    	int oId=obs->s->id;//当前步观测值的id
   	  symbolId.push_back(oId);
    	PsrDataId<<oId<<endl;
    	delete obs;
  	 }
  delete conData;
}

/****************************************************************************
 函数功能：利用getData()生成一串训练数据，并且存在名为Testdataid.txt文件中
函数输入：需要生成的训练数据的长度
函数输出：一个txt文件 
/***************************************************************************/
void Environment::createTestdata(int Length_Data)
{
  ofstream PsrDataId  ("Testdataid.txt");//获取固定长度的获取PSR模型所需的数据----20130822czz---每次获取模型之前都要生成这个数据,在正式运行时间之前先运行下面一段,生成数据之后再注释掉
  ofstream PsrData  ("Testdata.txt");
  vector<Symbol*> *conData=getData(Length_Data);//获取PSR模型的数据长度
  vector<Symbol*>::iterator vi=conData->begin();
  for(vi=conData->begin();vi!=conData->end();vi++)
  	 {
    	PsrData<<**vi<<endl;
  	 }
  vector<int> symbolId;
  for(int pos=0;pos<conData->size();)
  	 {
    	symList *act = getMemory(conData, pos, 1);pos++;
    	int aId=act->s->id;//当前步采取的动作的id
    	symbolId.push_back(aId);
    	PsrDataId<<aId<<endl;
    	delete act;
    	symList *obs=getMemory(conData,pos,1);pos++;
    	int oId=obs->s->id;//当前步观测值的id
    	symbolId.push_back(oId);
    	PsrDataId<<oId<<endl;
    	delete obs;
 		 }
  delete conData;
}


/****************************************************************************
 函数功能：构造函数，读取POMDP文件，存储POMDP文件的内容，设定landmark和初始状态
 输入输出:无
/***************************************************************************/
/*
Environment::Environment( )                         
{   
  char name[80];
  cout <<"the format of input filename is:/name"<<endl;
  cout<<"please input filename:";                          //输入POMDP文件名
  cin >>name;                       
  cout<<name<<endl;
  char buffer[160];
  getcwd(buffer, 160);          
  printf("The current directory is: %s\n", buffer);
  char *workdir=strcat(buffer,name);  
  cout<<workdir<<endl;   
  if((cass=fopen(workdir,"r"))==NULL)
 	  {
     cout<<"cannot open file  "<<endl;
     exit(0);
  	}
  else
     cout<<"success"<<endl;
  ret = parse_Cassandra(cass);//解析POMDP文件
  int i;
  S_sz =ret->nrstates; A_sz = ret->nract; O_sz = ret->nrobs;
  S = (Symbol **) malloc(S_sz * sizeof(Symbol *));
  for (i=0; i<S_sz; i++) S[i] = new Symbol(i, ret->arr_states[i], SYM_CUSTOM);      //S[i]存放所有状态 
  A = (Symbol **) malloc(A_sz * sizeof(Symbol *));                                 
  for (i=0; i<A_sz; i++) A[i] = new Symbol(i, ret->arr_act[i], SYM_ACTION);         //A[i]存放所有动作
  O = (Symbol **) malloc(O_sz * sizeof(Symbol *));                                  
  for (i=0; i<O_sz; i++) O[i] = new Symbol(i, ret->arr_obs[i], SYM_OBSERVATION);    //O[i]存放所有观测
	B_sz=ret->nrnewobs;
  B = (Symbol **) malloc(B_sz * sizeof(Symbol *));
  for (i=0; i<B_sz; i++) B[i] = new Symbol(i, ret->arr_newobs[i], SYM_NEWOBSER);      //B[i]存放POMDP文件中所有新观测(大于真实个数)
	N_sz=ret->nrnewact;
  N = (Symbol **) malloc(N_sz * sizeof(Symbol *));
  for (i=0; i<N_sz; i++) N[i] = new Symbol(i, ret->arr_newact[i], SYM_NEWACTION);      //N[i]存放所有新动作
  //O=setLandmark(O); 
	currState = S[0];
  prevState = NULL;
}
*/

Environment::Environment( )                         
{ 
	cout << "environment : POSystem" << endl;
 	int i, j, k;
	S_sz = 8; A_sz = 7; O_sz = 12;
	arr_states = "s"; // arr_states[i] is a NULL-terminated string representing mnemonic of state i
	arr_act = "a"; // arr_act[i] is a NULL-terminated string representing mnemonic of action i
	arr_obs = "o"; // arr_obs[i] is a NULL-terminated string representing mnemonic of observation i

	arr_newact = "c";
	arr_newobs = "b";

  	S = (Symbol **) malloc(S_sz * sizeof(Symbol *));
    for (i=0; i<S_sz; i++) 
    	S[i] = new Symbol(i, arr_states, SYM_CUSTOM);      //S[i]存放所有状态,存在segmentation fault
	
  	A = (Symbol **) malloc(A_sz * sizeof(Symbol *));                                 
    for (i=0; i<A_sz; i++) A[i] = new Symbol(i, arr_act, SYM_ACTION);         //A[i]存放所有动作

 	O = (Symbol **) malloc(O_sz * sizeof(Symbol *));                                  
    for (i=0; i<O_sz; i++) O[i] = new Symbol(i, arr_obs, SYM_OBSERVATION);    //O[i]存放所有观测

	//B_sz = 300;
  	//B = (Symbol **) malloc(B_sz * sizeof(Symbol *));
//  for (i=0; i<B_sz; i++) B[i] = new Symbol(i, ret->arr_newobs[i], SYM_NEWOBSER);      //B[i]存放POMDP文件中所有新观测(大于真实个数)
    //for (i=0; i<B_sz; i++) B[i] = new Symbol(i, arr_newobs, SYM_NEWOBSER);      //B[i]存放POMDP文件中所有新观测(大于真实个数)
	//N_sz = 28;

  	//N = (Symbol **) malloc(N_sz * sizeof(Symbol *));
// for (i=0; i<N_sz; i++) N[i] = new Symbol(i, ret->arr_newact[i], SYM_NEWACTION);      //N[i]存放所有新动作
    //for (i=0; i<N_sz; i++) N[i] = new Symbol(i, arr_newact, SYM_NEWACTION);      //N[i]存放所有新动作

  	//O=setLandmark(O);
 


//初始化三元数组
	//一元数组
	float a[448]= {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 
0.1, 0.9, 0, 0, 0, 0, 0, 0, 0.1, 0.9, 0, 0, 0, 0, 0, 0, 0.1, 0.9, 0, 0, 0, 0, 0, 0, 0.1, 0.9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.1, 0.9, 0, 0, 0, 0, 0.1, 0.9, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 
0.1, 0, 0.9, 0, 0, 0, 0, 0, 0.1, 0, 0.9, 0, 0, 0, 0, 0, 0.1, 0, 0.9, 0, 0, 0, 0, 0, 0.1, 0, 0.9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.1, 0, 0.9, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0.1, 0, 0.9, 0, 0, 0, 0, 
0.01, 0.09, 0.09, 0.81, 0, 0, 0, 0, 0.01, 0.09, 0.09, 0.81, 0, 0, 0, 0, 0.01, 0.09, 0.09, 0.81, 0, 0, 0, 0, 0.01, 0.09, 0.09, 0.81, 0, 0, 0, 0, 0, 0, 0, 0, 0.01, 0.09, 0.09, 0.81, 0, 0, 0.1, 0.9, 0, 0, 0, 0, 0, 0.1, 0, 0.9, 0, 0, 0, 0, 
0.1, 0, 0, 0, 0.9, 0, 0, 0, 0.1, 0, 0, 0, 0.9, 0, 0, 0, 0.1, 0, 0, 0, 0.9, 0, 0, 0, 0.1, 0, 0, 0, 0.9, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0.1, 0, 0, 0, 0.9, 0, 0, 0.1, 0, 0, 0, 0.9, 0, 0, 
0.01, 0.09, 0, 0, 0.09, 0.81, 0, 0, 0.01, 0.09, 0, 0, 0.09, 0.81, 0, 0, 0.01, 0.09, 0, 0, 0.09, 0.81, 0, 0, 0.01, 0.09, 0, 0, 0.09, 0.81, 0, 0, 0, 0, 0, 0, 0.1, 0.9, 0, 0, 0, 0, 0.01, 0.09, 0, 0, 0.09, 0.81, 0, 0.1, 0, 0, 0, 0.9, 0, 0, 
0.01, 0, 0.09, 0, 0.09, 0, 0.81, 0, 0.01, 0, 0.09, 0, 0.09, 0, 0.81, 0, 0.01, 0, 0.09, 0, 0.09, 0, 0.81, 0, 0.01, 0, 0.09, 0, 0.09, 0, 0.81, 0, 0, 0, 0, 0, 0.1, 0, 0.9, 0, 0, 0, 0.1, 0, 0, 0, 0.9, 0, 0, 0.01, 0, 0.09, 0, 0.09, 0, 0.81, 
0.001, 0.009, 0.009, 0.081, 0.009, 0.081, 0.081, 0.729, 0.001, 0.009, 0.009, 0.081, 0.009, 0.081, 0.081, 0.729, 0.001, 0.009, 0.009, 0.081, 0.009, 0.081, 0.081, 0.729, 0.001, 0.009, 0.009, 0.081, 0.009, 0.081, 0.081, 0.729, 0, 0, 0, 0, 0.01, 0.09, 0.09, 0.81, 0, 0, 0.01, 0.09, 0, 0, 0.09, 0.81, 0, 0.01, 0, 0.09, 0, 0.09, 0, 0.81
	};	
	//指针申请空间
	Tr = (float ***) malloc(A_sz * sizeof(float **));
	for (i=0; i<A_sz; i++) 
	{ 
		Tr[i] = (float **) malloc(S_sz * sizeof(float *));
		for (j=0; j<S_sz; j++)
				Tr[i][j] = (float *) malloc(S_sz * sizeof(float));
	}
	//赋值
	int g;
	for(i = 0; i < A_sz; i++)
	{
		for(j = 0; j < S_sz; j++)
		{
			for(k = 0; k < S_sz; k++)
			{
				g = j*A_sz*S_sz+i*S_sz+k;
				Tr[i][j][k] = a[g];				
			}
		}
	}
	//cout << "Tr初始化完毕" << endl;
	//Ob同理进行初始化	
	float b[672]= { 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
				};
	Ob = (float ***) malloc(A_sz * sizeof(float **));
	for (i=0; i<A_sz; i++) 
	{
		Ob[i] = (float **) malloc(S_sz * sizeof(float *));
		for (j=0; j<S_sz; j++)
			Ob[i][j] = (float *) malloc(O_sz * sizeof(float));
	}
  	
	for(i = 0; i < A_sz; i++)
	{
		for(j = 0; j < S_sz; j++)
		{
			for(k = 0; k < O_sz; k++)
			{
				g = j*A_sz*O_sz+i*S_sz+k;
				Ob[i][j][k] = b[g];
			}
		}
	}

	currState = S[7];
  	prevState = NULL;	
}

/***********************
函数功能： 判断输入的整数i是否小于该环境下对应的新观测值个数，若小于，则返回对应下标为i的新观测值，反之则返回值为空
输入：新观测值的下标
输出：下标为i的新观测值或NULL
****************************/
Symbol *Environment::getNewObservation(int i) 
{ 
	return i<B_sz ? B[i] : NULL; 
}


/*********************************************************************************************
函数功能：获取所有新动作及其对应内容
输入参数：
	所有动作-观测ao对的集合AO
输出：
	所有新动作及其对应内容
**********************************************************************************************/
symList2SymbolMap *Environment::getNewAct(symSet *AOset)
{
	symList2SymbolMap *NewAct=new symList2SymbolMap;
	int i=0;
	symIter iCurr;
	int size_AOset=AOset->size();
	N = (Symbol **) malloc(size_AOset * sizeof(Symbol *));                                  
	for(i=0,iCurr=AOset->begin();iCurr != AOset->end();i++, iCurr++)
		 {
			//N[i] = new Symbol(i, ret->arr_newact[i], SYM_NEWACTION);    //N[i]存放所有新动作
			N[i] = new Symbol(i, arr_newact, SYM_NEWACTION);    //N[i]存放所有新动作
			NewAct->insert(pair<symList*, Symbol*>(new symList(*iCurr),N[i]));
		 }
	return NewAct;
}


/*********************************************************************************************
函数功能：得到感兴趣的检验,这里我们设定为对某些观测值感兴趣
函数输入：固定感兴趣的观测值的下标
函数输出：所有动作与感兴趣的观测值的组合生成的感兴趣的检验的集合
**********************************************************************************************/
symSet * Environment::getInterestTest(vector<int>*interestTest_id)
{
   symSet *interestTests = new symSet();
   vector<int>::iterator i;
   for(i=interestTest_id->begin();i!=interestTest_id->end();i++)
   	 	 for(int j=0;j<A_sz;j++)
          {
					 symList*test=new symList();
           int n=*i;
					 test->append(A[j]);
           test->append(O[n]);
           interestTests->insert(test);                               
	   			}
   symIter s;
   for(s=interestTests->begin();s!=interestTests->end();s++)
   		{
    	 cout<<**s<<"  "<<endl;
   		}
   cout<<endl;
   return interestTests;
}


/*********************************************************************************************
函数功能：获得指定个数的随机感兴趣的检验
输入：随机感兴趣检验的个数
输出：指定个数的感兴趣的检验
**********************************************************************************************/
/*
symSet * Environment::getInterestTest(symSet *AOset,int testnum)
{ 
  symSet*testSet=new symSet; int i;
  symIter iCurr,iCurr1; 
  int N=AOset->size();int cycle=0;//定义循环次数
  ofstream testfile("testnum.txt");
  while(testnum)
   		 {   
     		int randnumber=1+( rand()%int(N-cycle) );//此处可以保证所获取的检验为目标值，且不重复(防止产生的随机数大于所剩的检验的个数)
       	testfile<<randnumber<<" ";
       	cout<<"随机值为："<<randnumber;
      	for(i=1,iCurr=AOset->begin();iCurr!=AOset->end();i++,iCurr++)
      		 {  
            if(i==randnumber)       
         			{ 
           		 cout<<"检验为："<<**iCurr<<" "<<endl;
           		 testSet->insert(new symList(*iCurr));
           		 AOset->erase(iCurr);//将AOset中已选择的感兴趣检验剔除(保证所获得的感兴趣检验不重复)
           		 break;
         			}
       			else continue;
     			 }
        cycle++;
       	testnum--;
  		 }
  for(iCurr=AOset->begin();iCurr!=AOset->end();iCurr++) //清空AOset数据，释放内存
      AOset->erase(iCurr);    
  delete AOset;
  cout<<"后来的检验大小为："<<AOset->size();
  cout<<"刚兴趣的检验为："<<endl;   
  for(iCurr1=testSet->begin();iCurr1!=testSet->end();iCurr1++)
      cout<<**iCurr1<<"--";
  return testSet;
}
*/

/*****************************************************
函数功能：得到模型的状态个数
输入：无
输出：模型的状态个数
******************************************************/
int  Environment::getStatesize()
{
  int Ssize=S_sz;
  return Ssize;
}


/*****************************************************
函数功能：得到landmark的集合（已经设定过landmark）
输入：无
输出：landmark的集合
******************************************************/
symSet* Environment::findLandmarkSet()
{
  symbol_type LANDMARK=SYM_LANDMARK;
  symSet* landmarkSet= new symSet();
  symList* landmark;
  for(int k=0;k<O_sz;k++)                   // 获得landmark的集合.如果观测值的类型为LANDMARK，则把这个观测存在landmark的集合中
  	 {
    	if(O[k]->type==LANDMARK)
    		{
      	 landmark=new symList(O[k]);
      	 landmarkSet->insert(landmark);
    		}
  	 }
 	return landmarkSet;
}


/************************
 函数功能：析构函数，释放内存
 输入输出:无
*************************/
Environment::~Environment( )
{
  for (int i=0; i<A_sz; i++) delete A[i]; free(A);
  for (int i=0; i<O_sz; i++) delete O[i]; free(O);
  for (int i=0; i<S_sz; i++) delete S[i]; free(S);
  for (int i=0; i<B_sz; i++) delete B[i]; free(B);
  for (int i=0; i<N_sz; i++) delete N[i]; free(N);
  /*
  for (int i=0; i<ret->nrstates; i++) free(ret->arr_states[i]); free(ret->arr_states);
  for (int i=0; i<ret->nract; i++) free(ret->arr_act[i]); free(ret->arr_act);
  for (int i=0; i<ret->nrobs; i++) free(ret->arr_obs[i]); free(ret->arr_obs);
  for (int i=0; i<ret->nrnewobs; i++) free(ret->arr_newobs[i]); free(ret->arr_newobs);
  for (int i=0; i<ret->nrnewact; i++) free(ret->arr_newact[i]); free(ret->arr_newact);
  free(ret);
  */
}
