#ifndef __PSR_CPP_
#define __PSR_CPP_

#include "psr.h"
#include "symbol.h"
#include "sampler.h"
#include <fstream>

using namespace Sampler;

/************************************************************************************************
  找到testData的第一个landmark及其在testData中所处的位置
  输入参数：
  testData——测试数据
  landmarkSet——landmark的集合
  返回值：
  startLMPos——map< int,symList *> int2SymListMap类型的返回值，包含了testData中第一个landmark的具体值以及第一个landmark所处的位置。
************************************************************************************************/
int2SymListMap *psr::getStartLMPos(vector<Symbol*> *testData, symSet *landmarkSet)
{   
    int2SymListMap *startLMPos=new  int2SymListMap();
    int size_T=testData->size();
    int lmSize = (*landmarkSet->begin())->size();    //得到landmark的长度，所有 landmark 已定为长度是一致的
    int currPos=0;
    symList *startLandmark;
    Symbol *actionToSend, *actionReceived; 

    for(int pos=1;pos<size_T;pos+=2)                 //从testData的第一个观测值开始判断是否为landmark
    {
      cout<<"pos="<<pos<<endl;
      symList *currMemory=getMemory(testData, pos, lmSize);    //找到testData中pos位置的观测值
      if(landmarkSet->find(currMemory) != landmarkSet->end())  //表示当前已经到了第一个landmark
      {
        startLandmark=new symList(currMemory);
        currPos=pos;cout<<"currPos="<<currPos<<endl;   
        break;    
      }                                     
      delete currMemory;
   }
   cout<<"startLandmark="<<*startLandmark<<endl;   
   startLMPos->insert(pair<int , symList*>(currPos,startLandmark));
   return  startLMPos;
}



/********************************
函数功能:在各landmark对应的信念状态集合中找到startLandmark对应的信念状态
输入：
startLandmark--测试数据中第一个landmark;
landmarkbe--各landmark对应的信念状态集合;
输出：startLandmark对应的信念状态
***********************************/
vector<float> *psr::getStartLMbelief(symList *startLandmark, symList2VfMap *landmarkbe)//得到startLandmark对应的信念状态
{
  vector<float> *startLMbelief=new vector<float>;
  map<symList*,vector<float>*>::iterator lmb=landmarkbe->begin();   
  for(lmb=landmarkbe->begin();lmb!=landmarkbe->end();lmb++)
  {
     if(*(lmb->first)==*startLandmark)
     {
        vector<float>::iterator vs=lmb->second->begin();
        for(vs=lmb->second->begin();vs!=lmb->second->end();vs++)
        {
          startLMbelief->push_back(*vs);
        }
        break;
     }     
  }
  return startLMbelief;
}
/************************************************************
 函数功能：得到整体状态空间发生的经历和经历发生的次数
 输入参数：
 divideData--划分后的子状态空间;
 输出：得到整体状态空间发生的经历和经历发生的次数
/************************************************************/
symList2IntMap *psr::getHistcout(symList2Vec2Map *divideData)
{
     ofstream historyData("historyData.txt");
     symList2VecSetMap *landmarkHist=new  symList2VecSetMap;//存儲每个landmark及其对应的经历的集合
    
    symList2Vec2Map::iterator vi=divideData->begin();
   
    symList2IntMap *Histcout=new symList2IntMap;
    for(vi=divideData->begin();vi!=divideData->end(); vi++)
    { 
      
       
       for(vector<symList*>::iterator begvec = vi->second->begin();begvec != vi->second->end();begvec++)//针对每一个landmark所对应的子状态空间
       { 
	  historyData<<"data="<<**begvec<<endl;
          int n=(*begvec)->size();
          vector<Symbol*>* datavec=env->getVecdata(*begvec);//將symList型数据转换成vector<Symbol*>
          symList* watchedHist0= new symList();//此landmark对应的实际起始经历
	  historyData<<" watchedHist000000000000="<<*watchedHist0<<endl;
          if(Histcout->find(watchedHist0) == Histcout->end())
          {
            Histcout->insert(pair<symList*, int*>(new symList(watchedHist0), new int(0)));
          }
          *(Histcout->find(watchedHist0)->second) += 1;//累计实际起始经历发生次数
         delete watchedHist0;
    
         for(int i=1; i<n; i+=2)               
         {
	    
            symList *watchedHist1= getHistory(datavec, i, (i+1)/2);//获取每一串数据对应的所有可能经历(除了空经历) 
	   //  historyData<<" watchedHist111111111="<<*watchedHist1<<endl;
            symList*watchedHist=new symList();
 	    watchedHist->append(watchedHist1);
	    historyData<<" watchedHistttttttttt="<<*watchedHist<<endl;
            if(Histcout->find(watchedHist) == Histcout->end())//如果出现新的经历则將新的经历插入到集合Histcout中
            {
              Histcout->insert(pair<symList*, int*>(new symList(watchedHist), new int(0)));
            }
            *(Histcout->find(watchedHist)->second) += 1;//累计Histcout中经历watchedHist发生次数
            delete watchedHist;
         }  
         delete datavec;
         datavec=NULL;
       } 
     }
	
     
  return Histcout;
}
symList2IntMap *psr::getHistcout1(vector<symList*> *divideData)
{
	ofstream historyData("historyData.txt");
   
    symList2IntMap *Histcout=new symList2IntMap;
	for(vector<symList*>::iterator begvec = divideData->begin();begvec != divideData->end();begvec++)
	{ 
		//historyData<<"data = "<<**begvec<<endl;
		int n=(*begvec)->size();
        //historyData<<"n = "<<n<<endl;
        vector<Symbol*>* datavec=env->getVecdata(*begvec);//將symList型数据转换成vector<Symbol*>
        symList* watchedHist0= new symList();//此landmark对应的实际起始经历
	  		//historyData<<" watchedHist000000000000="<<*watchedHist0<<endl;
        if(Histcout->find(watchedHist0) == Histcout->end())
        {
            Histcout->insert(pair<symList*, int*>(new symList(watchedHist0), new int(0)));
        }
        *(Histcout->find(watchedHist0)->second) += 1;//累计实际起始经历发生次数
        delete watchedHist0;
    
        for(int i=1; i<n; i+=2)               
        {
        	symList *watchedHist1= getHistory(datavec, i, (i+1)/2);//获取每一串数据对应的所有可能经历(除了空经历) 
            symList*watchedHist=new symList();
 	    	watchedHist->append(watchedHist1);
	    	
            if(Histcout->find(watchedHist) == Histcout->end())//如果出现新的经历则將新的经历插入到集合Histcout中
            {
              		Histcout->insert(pair<symList*, int*>(new symList(watchedHist), new int(0)));
            }
            	*(Histcout->find(watchedHist)->second) += 1;//累计Histcout中经历watchedHist发生次数
            	delete watchedHist;
         	}  
         	delete datavec;
         	datavec=NULL;
       	}      
     
     //输出Histcout的内容到文件historyDate.txt中
     int hsize = Histcout->size();
     symList2IntMap::iterator Hi = Histcout->begin();
     
     for(Hi = Histcout->begin(); Hi != Histcout->end(); Hi++)
     {
     	historyData << *(Hi->first) << " " << *(Hi->second) << endl;
     }
     
	return Histcout;
}
/************************************************************
 函数功能：得到发生次数大于CUTOFF的经历的集合(CZZ)
 输入参数：
 divideData--划分后的子状态空间;
 CUTOFF--经历最小发生次数;
 输出：子状态空间中删除掉发生次数少于CUTOFF的经历后剩余的状态空间
/************************************************************/
symList2symListSet *psr::getHistorySet(symList2Vec2Map *divideData,int CUTOFF)
{
    ofstream historyData("historyData.txt");
    symList2symListSet *landmarkHist=new symList2symListSet;//存儲每个landmark及其对应的满足条件的经历的集合
    map<symList* ,symSet*>::iterator lh=landmarkHist->begin();
    symList2Vec2Map::iterator vi=divideData->begin();
    for(vi=divideData->begin();vi!=divideData->end(); vi++)//针对每一个landmark所对应的子状态空间,首先获取所有可能的经历,再保留满足条件的经历(发生次数大于CUTOFF的经历)
    { 
       symList2IntMap *Histcout=new symList2IntMap;
       for(vector<symList*>::iterator begvec = vi->second->begin();begvec != vi->second->end();begvec++)//针对每一个landmark所对应的子状态空间
       { 
          int n=(*begvec)->size();
          vector<Symbol*>* datavec=env->getVecdata(*begvec);//將symList型数据转换成vector<Symbol*>
          symList* watchedHist0= getHistoryL(*begvec, 0, 0); //获取每一串数据的空经历    
          if(Histcout->find(watchedHist0) == Histcout->end())
          {
            Histcout->insert(pair<symList*, int*>(new symList(watchedHist0), new int(0)));
          }
          *(Histcout->find(watchedHist0)->second) += 1;//累计空经历发生次数
         delete watchedHist0;
    
         for(int i=1; i<n; i+=2)               
         {
            symList *watchedHist= getHistory(datavec, i, (i+1)/2);//获取每一串数据对应的所有可能经历(除了空经历) 
            if(Histcout->find(watchedHist) == Histcout->end())//如果出现新的经历则將新的经历插入到集合Histcout中
            {
              Histcout->insert(pair<symList*, int*>(new symList(watchedHist), new int(0)));
            }
            *(Histcout->find(watchedHist)->second) += 1;//累计Histcout中经历watchedHist发生次数
            delete watchedHist;
         }  
         delete datavec;
         datavec=NULL;
       } 
  
       symList2IntMap::iterator histIterator, tempHistIterator;
       for(histIterator=Histcout->begin(); histIterator!=Histcout->end();)//针对Histcout集合中每一个经历
       {
          if( *(histIterator->second) < CUTOFF)    //將Histcout中发生次数小于CUTOFF的经历从集合Histcout中剔除掉      
          {
            tempHistIterator = histIterator++;
            delete tempHistIterator->first;//释放内存
            delete tempHistIterator->second;
            Histcout->erase(tempHistIterator);
         }
         else
         {
           ++histIterator;//如果Histcout中经历发生次数大于CUTOFF,则將指针指向下一位置
         }
      }
      historyData<<"landmark="<<*vi->first<<endl;
      int row;
      for(row=0,histIterator=Histcout->begin();histIterator!=Histcout->end();histIterator++,row++)
      {
         cout<<"histIterator->first222="<<*(histIterator->first)<<"  histIterator->secondaaaaa="<<*(histIterator->second)<<endl;
         historyData<<"row="<<row<<"  history="<<*(histIterator->first)<<"  history--number="<<*(histIterator->second)<<endl;
      }
      historyData<<endl;
      cout<<"the size of Histcout After erase is : "<<Histcout->size()<<endl;

      symSet *baseHist=new symSet();       
      for(histIterator=Histcout->begin();histIterator!=Histcout->end();histIterator++)
      {
         baseHist->insert(new symList(histIterator->first));//將满足条件的经历放入baseHist
      }
      landmarkHist->insert(pair<symList*,symSet*>(new symList(vi->first),baseHist));//將每一个landmark对应的子空间内的landmark及其所有满足条件的经历
  
      for(histIterator=Histcout->begin();histIterator!=Histcout->end();histIterator++)
      {
         delete histIterator->first;
         delete histIterator->second;
      }
      delete Histcout;
  }
  return landmarkHist;
}


/************************************************************
 函数功能：得到整体状态空间发生次数大于CUTOFF的经历的集合和发生次数少于CUTOFF的集合
 输入参数：
 divideData--划分后的子状态空间;
 CUTOFF--经历最小发生次数;
landmark对应的起始经历;
 输出：子状态空间中发生次数大于CUTOFF的经历的集合和发生次数少于CUTOFF的集合
/************************************************************/
symList2VecSetMap *psr::getHistorySet2(symList2Vec2Map *divideData,int CUTOFF,symList2ListMap*landmarkhis,symList*startPosLm)
{
     ofstream historyData("historyData.txt");
     symList2VecSetMap *landmarkHist=new  symList2VecSetMap;//存儲每个landmark及其对应的经历的集合
    symList2VecSetMap::iterator lh=landmarkHist->begin();
    symList2Vec2Map::iterator vi=divideData->begin();
    symList2ListMap::iterator s2=landmarkhis->begin();//landmark对应的起始经历
    symList2IntMap *Histcout=new symList2IntMap;
    for(vi=divideData->begin(),s2=landmarkhis->begin();vi!=divideData->end(),s2!=landmarkhis->end(); vi++,s2++)
    { 
       symList* lm_history=new symList(s2->second);//此landmark对应的起始经历
       historyData<<" landmarkhis="<<*lm_history<<endl;
       for(vector<symList*>::iterator begvec = vi->second->begin();begvec != vi->second->end();begvec++)//针对每一个landmark所对应的子状态空间
       { 
	  historyData<<"data="<<**begvec<<endl;
          int n=(*begvec)->size();
          vector<Symbol*>* datavec=env->getVecdata(*begvec);//將symList型数据转换成vector<Symbol*>
          symList* watchedHist0= new symList(lm_history);//此landmark对应的实际起始经历
	  historyData<<" watchedHist000000000000="<<*watchedHist0<<endl;
          if(Histcout->find(watchedHist0) == Histcout->end())
          {
            Histcout->insert(pair<symList*, int*>(new symList(watchedHist0), new int(0)));
          }
          *(Histcout->find(watchedHist0)->second) += 1;//累计实际起始经历发生次数
         delete watchedHist0;
    
         for(int i=1; i<n; i+=2)               
         {
	    
            symList *watchedHist1= getHistory(datavec, i, (i+1)/2);//获取每一串数据对应的所有可能经历(除了空经历) 
	   //  historyData<<" watchedHist111111111="<<*watchedHist1<<endl;
            symList*watchedHist=new symList(lm_history);
 	    watchedHist->append(watchedHist1);
	    historyData<<" watchedHistttttttttt="<<*watchedHist<<endl;
            if(Histcout->find(watchedHist) == Histcout->end())//如果出现新的经历则將新的经历插入到集合Histcout中
            {
              Histcout->insert(pair<symList*, int*>(new symList(watchedHist), new int(0)));
            }
            *(Histcout->find(watchedHist)->second) += 1;//累计Histcout中经历watchedHist发生次数
            delete watchedHist;
         }  
         delete datavec;
         datavec=NULL;
       } 
     }
	
       symSet *histLess=new symSet();  
       symSet *baseHist=new symSet();    
       symList2IntMap::iterator histIterator, tempHistIterator;
       for(histIterator=Histcout->begin(); histIterator!=Histcout->end();histIterator++)//针对Histcout集合中每一个经历
       {
          if( *(histIterator->second) < CUTOFF)    //將Histcout中发生次数小于CUTOFF的经历从集合Histcout中剔除掉      
          {
            histLess->insert(new symList(histIterator->first));//將不满足条件的经历放入HistLess
         }
         else
         {
            baseHist->insert(new symList(histIterator->first));//將满足条件的经历放入baseHist
         }
      }

     
        vector<symSet*>* HisDivided=new vector<symSet*>; 
	HisDivided->push_back(baseHist);
	HisDivided->push_back(histLess);
	
     for(vi=divideData->begin();vi!=divideData->end(); vi++)
    { 
    if(*(vi->first)==*startPosLm)
    landmarkHist->insert(pair<symList*,vector<symSet*>*>(new symList(vi->first),HisDivided));//將起始landmark对应经历存起来
    /*else 
    {vector<symSet*>* HisDivided1=new vector<symSet*>; 
     symSet *histLess1=new symSet();  
     symSet *baseHist1=new symSet();  
     HisDivided->push_back(baseHist1);
     HisDivided->push_back(histLess1);
     landmarkHist->insert(pair<symList*,vector<symSet*>*>(new symList(vi->first),HisDivided1));//其他landmark对应空
     }*/
    }
    for(histIterator=Histcout->begin();histIterator!=Histcout->end();histIterator++)
      {
         delete histIterator->first;
         delete histIterator->second;
      }
      delete Histcout;
/*
  for(lh=landmarkHist->begin();lh!=landmarkHist->end();lh++)
	{
	 cout<<"landmark="<<*(lh->first)<<endl;
	 for(iCurr=(*(lh->second->begin()))->begin();iCurr!=(*(lh->second->begin()))->end();iCurr++)
		cout<<**iCurr<<endl;
	}
*/
  return landmarkHist;
}
/************************************************************
 函数功能：得到发生次数大于CUTOFF的经历的集合
 输入参数：
 子状态空间中发生次数大于CUTOFF的经历的集合和发生次数少于CUTOFF的集合
 输出：发生次数大于CUTOFF的经历的集合
/************************************************************/
symList2symListSet *psr::getMHistorySet(symList2VecSetMap*landmarkHist)
{
 symList2symListSet *landmarkMHist=new symList2symListSet;//存儲每个landmark及其对应的满足条件的经历的集合
 symList2VecSetMap::iterator lh=landmarkHist->begin();
 map<symList* ,symSet*>::iterator lmh;
 for(lh=landmarkHist->begin();lh!=landmarkHist->end();lh++)
	{
	 symSet *baseHist=new symSet(); 
	 for(iCurr=(*(lh->second->begin()))->begin();iCurr!=(*(lh->second->begin()))->end();iCurr++)
		baseHist->insert(new symList(*iCurr));
	landmarkMHist->insert(pair<symList*,symSet*>(new symList(lh->first),baseHist));
	}
for(lmh=landmarkMHist->begin();lmh!=landmarkMHist->end();lmh++)
	{
	 cout<<"landmark="<<*(lmh->first)<<endl;
	 cout<<"the size of Mhist is: "<<lmh->second->size()<<endl;;
	}
return landmarkMHist;
}
/************************************************************
 函数功能：得到发生次数小于CUTOFF的经历的集合
 输入参数：
 子状态空间中发生次数大于CUTOFF的经历的集合和发生次数少于CUTOFF的集合
 输出：发生次数小于CUTOFF的经历的集合
/************************************************************/
symList2symListSet *psr::getLHistorySet(symList2VecSetMap*landmarkHist)
{
 symList2symListSet *landmarkLHist=new symList2symListSet;//存儲每个landmark及其对应的满足条件的经历的集合
 symList2VecSetMap::iterator lh=landmarkHist->begin();
 map<symList* ,symSet*>::iterator llh;
 for(lh=landmarkHist->begin();lh!=landmarkHist->end();lh++)
	{
	 symSet *baseHist=new symSet();
	 vector<symSet*>::iterator vs=lh->second->begin();vs++;
	 for(iCurr=(*vs)->begin();iCurr!=(*vs)->end();iCurr++)
		baseHist->insert(new symList(*iCurr));
	landmarkLHist->insert(pair<symList*,symSet*>(new symList(lh->first),baseHist));
	}
for(llh=landmarkLHist->begin();llh!=landmarkLHist->end();llh++)
	{
	 cout<<"landmark="<<*(llh->first)<<endl;
	 cout<<"the size of Lhist is: "<<llh->second->size()<<endl;;
	}
return landmarkLHist;
}
/*******************************************************************
  函数功能：补充缺值。
 输入参数：
 Landmark——需要补充缺值的子状态空间对应的landmark；
 PSRS——未补充缺值前的每个子状态空间的经历核-检验核的概率矩阵；
 输出：
 补充缺值后的子状态空间。
********************************************************************/
tEstimateData *psr::fillValue(symList* landmark,symList2EstMap* PSRS)
{
  cout<<"landmark="<<*landmark<<endl;
  tEstimateData* matData1=NULL;

  symList2EstMap::iterator Ei=PSRS->begin();
  for(Ei=PSRS->begin();Ei!=PSRS->end(); Ei++)//找到指定landmark对应的子状态空间
  {  //cout<<"Ei->first="<<*Ei->first<<endl;
    if(*landmark==*Ei->first)
     {
      //cout<<"Ei->first="<<*Ei->first<<"  landmark="<<*landmark<<endl;
      matData1=Ei->second;
      break;
     }
   }

  tEstimateData *toret=new tEstimateData;//重新生成内存,把补充缺值后的结构体内容复制进去
  toret->ctProb = new symListDblMat();//里面的经历及检验都重新分配了内存,与PSRS无关了
  toret->ctMat	= new symListMatrix();
  toret->ctAct	= new symListMatrix();
  toret->ctHists= new symList2IntMap();
 
  symSet	*augHist1 = new symSet();    //存放pomDatak对应的PSR模型的经历集合
  augHist1->clear();
  for(iH1=matData1->ctMat->begin(); iH1!=matData1->ctMat->end(); iH1++)
  {
    augHist1->insert(new symList(iH1->first));//为经历分配的内存,toret->ctProb,toret->ctMat,toret->ctAct,对应的经历都是这一块,
  }

  symSet   *augTest1 = new symSet(); //存放某个PSR模型的检验集合
  augTest1->clear();
  for(iT1=matData1->ctMat->begin()->second->begin(); iT1!=matData1->ctMat->begin()->second->end(); iT1++)
  {
    augTest1->insert(new symList(iT1->first));//为检验分配的内存,toret->ctProb,toret->ctMat,toret->ctHists对应的检验都是这一块,
  }

  symSet   *augAct1 = new symSet(); //存放某个PSR模型的检验集合
  augAct1->clear();
  for(tiT=matData1->ctAct->begin()->second->begin(); tiT!=matData1->ctAct->begin()->second->end(); tiT++)
  {
    augAct1->insert(new symList(tiT->first));//为动作分配的内存,toret->ctAct对应的动作都是这一块,
  }
cout<<"landmark1111111="<<*landmark<<endl;
  int size_m=matData1->ctProb->size();                  //得到经历核-检验核矩阵的行数
  int size_n=matData1->ctProb->begin()->second->size(); //得到经历核-检验核矩阵的列数
  int size_act=matData1->ctAct->begin()->second->size();//得到检验核中动作的个数
 
  toret->probs	= new Array2D<double>(size_m, size_n);	*(toret->probs)=0.0;  //给toret->probs申请空间,并赋初值0.0
  toret->counts	= new Array2D<int>(size_m, size_n);	*(toret->counts)=0;   //给toret->counts申请空间,并赋初值0
  toret->actcounts= new Array2D<int>(size_m, size_act);	*(toret->actcounts)=0;//给toret->actcounts申请空间,并赋初值0
  toret->hcounts  = new Array1D<int>(size_m, 0);        *(toret->hcounts)=0;  //给toret->hcounts申请空间,并赋初值0
  
  int row_size, column_size;
  for(row_size = 0;row_size<size_m;row_size++)
  { 
    for(column_size = 0;column_size<size_n; column_size++)
    { 
      (*toret->probs)[row_size][column_size]= (*matData1->probs)[row_size][column_size];   //將pomData->probs复制给toret->probs
      (*toret->counts)[row_size][column_size] = (*matData1->counts)[row_size][column_size];//將pomData->counts复制给toret->counts
  
    }
  }

  for(row_size = 0;row_size<size_m;row_size++)
  {
    for(column_size = 0;column_size<size_act; column_size++)
    {
      (*toret->actcounts)[row_size][column_size] =(*matData1->actcounts)[row_size][column_size];//將pomData->actcounts复制给toret->actcounts
    }
  }

  for(row_size = 0;row_size<size_m;row_size++)
  {
    (*toret->hcounts)[row_size] = (*matData1->hcounts)[row_size];
  }
  
  for(i=0, iCurr = augHist1->begin(); iCurr != augHist1->end(); i++, iCurr++)//以toret->probs,toret->counts为基准给toret->ctProb,toret->ctMat赋值
  {
    rowDbl    = (*toret->probs)[i];   //以toret->probs为基准给toret->ctProb赋值
    row       = (*toret->counts)[i];  //以toret->counts为基准给toret->ctMat赋值 
    rowDblMap = new symList2DblMap();  
    rowMap    = new symList2IntMap();           
    for(iCurr1 = augTest1->begin(); iCurr1 != augTest1->end(); iCurr1++)
    {
      rowDblMap->insert(pair<symList*, double*>(*iCurr1,new double(*(rowDbl++))));   //第i行经历下对应的检验及各检验对应的概率
      rowMap->insert(pair<symList*, int*>(*iCurr1, new int(*(row++))));
    }   
    toret->ctMat->insert(pair<symList*, symList2IntMap*>(*iCurr, rowMap));    //第i行经历及第i行经历下对应的检验和各检验发生的次数
    toret->ctProb->insert(pair<symList*, symList2DblMap*>(*iCurr, rowDblMap));         //第i行经历及第i行经历下对应的检验和各检验对应的概率
  }
  
  for(i=0, iCurr = augHist1->begin(); iCurr != augHist1->end(); i++, iCurr ++)//以toret->ctAct为基准给toret->ctAct赋值
  {
      row       = (*toret->actcounts)[i];
      rowMap = new symList2IntMap();
      for(iCurr1 = augAct1->begin(); iCurr1 != augAct1->end(); iCurr1++)
      {
         rowMap->insert(pair<symList*, int*>(*iCurr1, new int(*(row++))));
      }
      toret->ctAct->insert(pair<symList*, symList2IntMap*>(*iCurr, rowMap));    //第i行经历及第i行经历下对应的动作和各动作发生的次数
  }

 
  for(i=0, iCurr = augHist1->begin(); iCurr != augHist1->end(); i++, iCurr++) //以toret->hcounts为基准给toret->ctHists赋值
  {
     toret->ctHists->insert(pair<symList*, int*>(*iCurr, new int((*toret->hcounts)[i])));
  }
cout<<"landmark22222222="<<*landmark<<endl;
  for(i=0, piH=toret->ctProb->begin(),tiH = toret->ctMat->begin(),iH=toret->ctAct->begin(); piH != toret->ctProb->end(),tiH != toret->ctMat->end(),iH!=toret->ctAct->end(); i++, piH++,tiH++,iH++)
  { 
    int len=piH->first->size();
    symList *mark=getMemoryL(piH->first, len-1, 1);  //取经历的最后一个观测值
    for(Ei=PSRS->begin();Ei!=PSRS->end(); Ei++)
    {
       if(*mark==*(Ei->first))                     //如果经历的最后一个观测值为landmark,则取该landmark对应的PSR子模型下的空经历下各检验对应的概率
       {         
          int column_size;
          for( column_size= 0,  piT1= Ei->second->ctProb->begin()->second->begin(); piT1!= Ei->second->ctProb->begin()->second->end(); column_size++, piT1++)
          {
                for(piT = piH->second->begin(),tiT=tiH->second->begin(); piT != piH->second->end(),tiT!=tiH->second->end();piT++,tiT++)
                {
                   if(*(piT1->first)==*(piT->first))//取对应的检验
                   {
                        *(piT->second)=(*Ei->second->probs)[0][column_size];//p(t|h)=p1(t|空经历)
			*(tiT->second)=(*Ei->second->counts)[0][column_size];//CTMAT矩阵也要补充
                   }
                }	
          }
	for(column_size= 0,iT=Ei->second->ctAct->begin()->second->begin(); iT!= Ei->second->ctAct->begin()->second->end(); column_size++, iT++)
		for(tiT=iH->second->begin(); tiT!=iH->second->end();tiT++)
		if(*(iT->first)==*(tiT->first))//若动作一样
			*(tiT->second)=(*Ei->second->actcounts)[0][column_size];//CTACT矩阵也要补充
	
         break;
       } 
    }
  }
cout<<"landmark333333333="<<*landmark<<endl;

  for(i=0, piH=toret->ctProb->begin(),iH = toret->ctMat->begin(); piH != toret->ctProb->end(),iH != toret->ctMat->end(); i++, piH++,iH++)//经过补充缺值,toret->ctProb发生变化;把最后的toret->ctProb结果为基准给toret->probs赋值,把toret->ctMat结果为基准给toret->counts赋值
  { 
    for(j=0, piT = piH->second->begin(),iT = iH->second->begin(); piT != piH->second->end(),iT != iH->second->end(); j++, piT++,iT++)
    {
        (*toret->probs)[i][j]=*(piT->second);
	(*toret->counts)[i][j] = *(iT->second);
    }
  }
   for(i=0,tiH=toret->ctAct->begin(); tiH!=toret->ctAct->end();i++,tiH++)
     {
        for(j=0,tiT=tiH->second->begin();tiT!=tiH->second->begin();j++,tiT++)
        {
           (*toret->actcounts)[i][j] = *(tiT->second);
        }
     }
cout<<"landmark4444444444="<<*landmark<<endl;  
 
  column_size=-1;
  for (iCurrT = augTest1->begin(); iCurrT != augTest1->end(); iCurrT++) //考虑检验集合augTest中的某些检验中存在landmark-----另一种缺值情况---检验t中存在landmark
  {
    column_size++; double denom1=1.0; 
    int len=(*iCurrT)->size();

    for(int pos1=1; pos1<len; pos1+=2)            //取Test中的一个观测值
    {
      symList *mark1=getMemoryL(*iCurrT, pos1, 1);
      if(PSRS->find(mark1)!=PSRS->end())//如果Test中的一个观测值为landmark,则取该landmark对应的PSR子模型下的相同经历下該检验对应的概率
       {  
         symList *Test1=getHistoryL(*iCurrT,pos1,(pos1+1)/2);      //取landmark前对应的一部分检验
         symList *Test2=getTestL(*iCurrT,pos1,(len-pos1-1)/2);     //取landmark后对应的一部分检验
         if((Test2->size())>1)  //分隔检验后剩余检验不为空,Test被分隔成两部分,求denom1=p(t2|kong)*p(t2|kong)....p(tn|kong)
           { 
           vector<int> v1;
           for(int pos2=pos1;pos2<len-2;pos2=pos2+2)//pos所在位置为观测值
             { 
               symList* mark2=getMemoryL(*iCurrT, pos2, 1); // pos对应观测值
               if(PSRS->find(mark2)!=PSRS->end())//判断观测值是否为landmark
               v1.push_back(pos2);//将landmark所在位置依次存入容器中
               delete mark2;
             }
           v1.push_back(len-1);//將最后一个观测值所在位置放入
           
           symList2symListVec *v2=new symList2symListVec;//存储landmark,及这一landmark开头后一landmark结尾的数据         
           for(int i=0;i<v1.size()-1; i++)
           {
             int j=v1.at(i);  
             int k=v1.at(++i);
             symList* landmark1=getMemoryL(*iCurrT, j, 1);    //j对应的观测值是landmark
             symList* test=getTestL(*iCurrT,j,(k-j)/2);     //取两个landmark之间的一部分检验或是最后一个landmark后的部分检验
             if(test->size()>1)
             {
                     symList2ListMap *landmarkMap = new symList2ListMap();//用来存放landmark及对应的test
                     landmarkMap->insert(pair<symList*, symList*>(landmark1,test));//把landmark及对应的test存入
                     v2->push_back(landmarkMap);
             }
             i--; 
             //delete landmark;
             //delete test;
           } 

           vector<symList2ListMap*>::iterator iter2=v2->begin();
           for(iter2=v2->begin();iter2!=v2->end(); iter2++)
           {
             symList2ListMap::iterator lm=(*iter2)->begin();//landamrkMap中只有一组map
             for(Ei=PSRS->begin();Ei!=PSRS->end(); Ei++)
             {
                 if(*lm->first==*Ei->first)
                 {
                   int column_size1,column_size2;
                   for(column_size1 = 0,  piT1= Ei->second->ctProb->begin()->second->begin(); piT1!=Ei->second->ctProb->begin()->second->end(); column_size1++, piT1++)
                   {
                     if((*(piT1->first)==*lm->second)) //寻找在空经历下检验Test2下对应的概率p2(t2|空经历)
                     {
                       denom1=denom1*(*Ei->second->probs)[0][column_size1];
                            break;
                     }
                   }
                   break;
                 }
               }
               delete lm->first;
               delete lm->second;
               delete *iter2;//delete landmarkMap;
            }
            delete v2;
         }  //denom1
           
         int row_size1,column_size1;
         for(row_size1 = 0, piH1=toret->ctProb->begin(); piH1!= toret->ctProb->end(); row_size1++, piH1++)//求p1(t1|h)
         { 
             for(column_size1 = 0,  piT1= piH1->second->begin(); piT1!= piH1->second->end(); column_size1++, piT1++)
                {
               if(*(piT1->first)==*Test1)  //寻找在经历augHist下检验Test1下对应的概率p1(t1|h)
                  {  
                 (*toret->probs)[row_size1][column_size1] = *(piT1->second);  //p1(t1|h)
                 (*toret->probs)[row_size1][column_size]=(*toret->probs)[row_size1][column_size1]*denom1;  //p1(t1|h)p2(t2|空经历)
                  }
                }
         }
         delete Test2;
         delete Test1;
           
           break;
       }
       delete mark1;
    }
    //delete Test;
  }

  
  for(i=0, piH=toret->ctProb->begin(); piH != toret->ctProb->end(); i++, piH++)//经过再次补充缺值,toret->probs发生变化;把toret->probs最后的结果为基准给toret->ctProb赋值
  { 
    for(j=0, piT = piH->second->begin(); piT != piH->second->end(); j++, piT++)
    {
        *(piT->second)=(*toret->probs)[i][j];
    }
  }



  delete augAct1;
  delete augHist1;
  delete augTest1;
cout<<"landmark5555555="<<*landmark<<endl;  
  return toret;
}


/**********************************************************
 函数功能：释放POMDP_probData* pomData的内存空间。
 输入参数：
 pomData——由POMDP文件得到的准确的经历-检验概率矩阵；
 输出：无。
***********************************************************/
void psr::deleteStruct_pom(POMDP_probData* pomData)
{ 
  delete pomData->probs;
  delete pomData->hcounts;
  piH=pomData->ctProb->begin();
  for (piT = piH->second->begin(); piT != piH->second->end(); piT++) //delete掉矩阵对应的检验,delete一次就好
  {
    delete piT->first;
  }
  for(piH=pomData->ctProb->begin();piH!=pomData->ctProb->end();piH++)
  {
    for(piT=piH->second->begin();piT!=piH->second->end();piT++)
     {
       delete piT->second;
     }
     delete piH->second;
  }
  delete pomData->ctProb;
  
  for(iT=pomData->ctHists->begin();iT!=pomData->ctHists->end();iT++)
  {
     delete iT->first; //delete掉矩阵对应的经历,delete一次就好
     delete iT->second;
  }
  delete pomData->ctHists;
  delete pomData;//通过以上步骤delete掉了PSRS中某个结构体的所有内容
}


/************************************************************
  函数功能：释放各个子空间模型中矩阵所占的内存(CZZ)
  输入参数：
  PSRmatrix——所有子状态空间对应的经历核--检验核的估计概率矩阵；
  输出：无
/************************************************************/
void psr::deleteProbMAP(ProbMAP *Probmatrix)
{
  map<symList* ,tEstimateData*>::iterator psrs=Probmatrix->aoProb->begin();
  for(psrs=Probmatrix->aoProb->begin();psrs!=Probmatrix->aoProb->end();psrs++)
  {
     delete psrs->first;
     deleteStruct_tEs(psrs->second);//delete结构体的成员变量及其内部数据对应参内存
  }
  delete Probmatrix->aoProb;
  Probmatrix->aoProb=NULL;

  map<symList* ,tEstimateData*>::iterator aopsrs=Probmatrix->QaoQProb->begin();
  for(aopsrs=Probmatrix->QaoQProb->begin();aopsrs!=Probmatrix->QaoQProb->end();aopsrs++)
  {
     delete aopsrs->first;
     //deleteStruct(aopsrs->second);//delete结构体的成员变量及其内部数据对应参内存
  }
  delete Probmatrix->QaoQProb;
  Probmatrix->QaoQProb=NULL;
}


/**********************************************************
  函数功能：释放tEstimateData* matData的内存空间。
 输入参数：
 matData--估计的经历核-检验核概率矩阵；
 输出：无。
***********************************************************/
void psr::deleteStruct_tEs(tEstimateData* matData)
{ 
  delete matData->probs;
  delete matData->counts;
  delete matData->hcounts;
  delete matData->actcounts;
  for(iH=matData->ctMat->begin();iH!=matData->ctMat->end();iH++)
  {
    for(iT=iH->second->begin();iT!=iH->second->end();iT++)
     {
       delete iT->second;
     }
     delete iH->second;//ctHists,ctMat等经历对应的檢验只dekete一次就好，因为都存放在 augTest 里面
  }
  delete matData->ctMat;
  
  piH=matData->ctProb->begin();
  for (piT = piH->second->begin(); piT != piH->second->end(); piT++) //delete掉矩阵对应的检验,delete一次就好
  {
    delete piT->first;
  }
  for(piH=matData->ctProb->begin();piH!=matData->ctProb->end();piH++)
  {
    for(piT=piH->second->begin();piT!=piH->second->end();piT++)
     {
       delete piT->second;
     }
     delete piH->second;
  }
  delete matData->ctProb;

  tiH=matData->ctAct->begin();
  for(tiT=tiH->second->begin();tiT!=tiH->second->end();tiT++)//delete掉矩阵对应的动作,delete一次就好
  {
    delete tiT->first;
  }
  for(tiH=matData->ctAct->begin();tiH!=matData->ctAct->end();tiH++)
  {
      for(tiT=tiH->second->begin();tiT!=tiH->second->end();tiT++)
      {
         delete tiT->second;
      }
      delete tiH->second;
  }
  delete matData->ctAct ;

  for(iT=matData->ctHists->begin();iT!=matData->ctHists->end();iT++)
  {
     delete iT->first; //delete掉矩阵对应的经历,delete一次就好
     delete iT->second;
  }
  delete matData->ctHists;
  delete matData;//通过以上步骤delete掉了PSRS中某个结构体的所有内容
}






psr::psr(Environment *e) {
  env = e;
} 

psr::~psr() { }                  


#endif
