#include <fstream>
#include <iostream>
#include "sampler.h"

using namespace Sampler;
/*输入感兴趣的检验，输出感兴趣的检验的动作序列*/
symSet*Sampler:: getaugAct(symSet* baseTest,int testLen,Environment *env)
{

symIter iCurr,iCurr1,iCurr2;
  symSet *augTest = new symSet(), *augAct = new symSet();
  if(testLen==1)//若检验的长度为1
  {
    cout<<"testLen="<<testLen<<endl;

    for (iCurr1 = baseTest->begin(); iCurr1 != baseTest->end(); iCurr1++)
    {
       cout<<"baseTest="<<**iCurr1<<endl;//输出baseTest中所包含的检验  
    }
    for (iCurr1 = baseTest->begin(); iCurr1 != baseTest->end(); iCurr1++)
    {
      augTest->insert(new symList(*iCurr1));//在augTest中插入baseTest中所包含的检验
    }

    for (iCurr1 = baseTest->begin(); iCurr1 != baseTest->end(); iCurr1++)//遍历baseTest中的每个检验
    {
       symList *aug = (*iCurr1)->filter(SYM_ACTION);//在检验中取出其所对应的动作
       if (augAct->find(aug)==augAct->end())//若即将插入augAct中的动作与augAct中已经插入的动作不相同
       {
         augAct->insert(new symList(aug));//则在augAct中插入该动作
       }
       delete aug;//否则删除该动作    
    }
  }

  if(testLen>1)//若检验的长度大于1
  { 
    symList *emptyList = new symList();//开辟空经历emptyList的内存空间
    symList *del;
    
    symList *actlist = env->getActions();//获得连续的动作序列，如a1a2...an
    cout<<"actlist="<<*actlist<<endl;
    symList *obslist = env->getObservations();//获得连续的观测值序列，如o1o2...on
    cout<<"obslist="<<*obslist<<endl;
    symList *curract,*currobs,*acttest,*aug;

    symIter iTBegin = baseTest->begin(), iTEnd = baseTest->end(),iCurr, iCurr2;

    if((iCurr = augTest->find(emptyList)) != augTest->end())//删除augTest中的空检验
         { //将 empty test从 augTest中抹去
      del = *iCurr;
      augTest->erase(iCurr);
      delete del;
    }


    curract = actlist;//将连续的动作序列赋予curract
    for (iCurr = iTBegin; iCurr != iTEnd; iCurr++) 
    {
      acttest = (*iCurr)->filter(SYM_ACTION);//在检验中取出对应的动作
      if(augTest->find(*iCurr)==augTest->end())//若即将插入augTest中的检验与augTest中已经插入的检验不相同
        augTest->insert(new symList(*iCurr));//则在augTest中插入该检验
      if(acttest!=NULL && augAct->find(acttest)==augAct->end())//若即将插入augAct中的动作不为空且与augAct中已经插入的动作不相同
        augAct->insert(new symList(acttest));//则在augAct中插入该动作
      curract = actlist;//将连续的动作序列赋予curract
      while ((curract != NULL) && (curract->s != NULL))//若curract中所包含的连续动作序列不为空且其首个动作不为空？  
      { 
        currobs = obslist;//将连续的观测序列赋予currobs
        while ((currobs != NULL) && (currobs->s != NULL))//若curract中所包含的连续动作序列不为空且其首个动作不为空？  
        {
	  del = NULL;
	  aug = new symList(curract->s, new symList(currobs->s, NULL));//开辟内存空间，存储检验
	  aug->append(new symList(*iCurr)); //在aug中存入baseTest对应的检验
	  if (augTest->find(aug)==augTest->end())//若即将存入augTest中的检验aug与已存入augTest中的检验不相同
            augTest->insert(new symList(aug));//在augTest中插入该检验
	  else 
            del = aug;//否则将该检验赋予del
	  aug = aug->filter(SYM_ACTION);//从检验中取出对应的动作
	  if (augAct->find(aug)==augAct->end()) //若即将插入augAct中的动作与augAct中已经插入的动作不相同
	    augAct->insert(new symList(aug));//则在augAct中插入该动作
	  else 
            delete aug;//否则删除该动作
	  if (del!=NULL) delete del;//若del所代表的检验不为空，则删除del所代表的检验
	  currobs = currobs->n;//删除连续观测序列中的首个观测值，将剩余观测值序列赋予currobs
        } // obs
        curract = curract->n;//删除连续动作序列中的首个动作，将剩余动作序列赋予curract
      } // act
      //delete acttest;
    }//上面的工作完成的是将所有baseTest及其扩展放到augTest中去，同时将baseTest中的动作序列提取出来及其一步的动作扩展放到 augAct中去
 
    // Even if the empty test was given, it should not occur among tests.注意空历史是允许的
    if((iCurr = augTest->find(emptyList)) != augTest->end())//删除augTest中的空检验
         { //将 empty test从 augTest中抹去
      del = *iCurr;
      augTest->erase(iCurr);
      delete del;
    }
    delete emptyList;
  
   
  }  
 return augAct;
}



/****************************
函数功能：
用POMDP文件内容计算给定经历和检验下的相应概率
输入参数:
env--Environment类传递的参数;
baseTest--给定的检验的集合;
baseHist--给定的经历的集合;
elementVec--每一个landmark对应的子状态空间内的经历的集合;
belief--每一个landmark对应的信念状态;
输出：给定经历和检验下的相应概率矩阵
*****************************/
POMDP_probData *Sampler::exactProb(Environment *env,symSet* baseTest,symSet* baseHist,vector<symList*>* elementVec,vector<float> *belief)
{
  symList *emptyList = new symList();
  symList *del;
  symSet	 *augTest = new symSet(), *augAct = new symSet();
  symList *actlist = env->getActions();//获得连续的动作序列，如a1a2....an
  cout<<"actlist="<<*actlist<<endl;
  symList *obslist = env->getObservations();//获得连续的观测值序列，如o1o2...on
  cout<<"obslist="<<*obslist<<endl;
  symList *curract,*currobs,*acttest,*aug;

  symIter	iTBegin = baseTest->begin(), iTEnd = baseTest->end(),iCurr, iCurr1;

  for (iCurr = iTBegin; iCurr != iTEnd; iCurr++) 
  {
    cout<<"baseTest="<<**iCurr<<endl;//输出baseTest中存放的检验
  }

  curract = actlist;
  for (iCurr = iTBegin; iCurr != iTEnd; iCurr++) 
  {
    acttest = (*iCurr)->filter(SYM_ACTION);//从baseTest包含的检验中取出对应的动作
    if(augTest->find(*iCurr)==augTest->end())//若即将存入augTest中的检验与augTest中已存入的检验不同,在augTest中插入该检验
      augTest->insert(new symList(*iCurr));
    if(acttest!=NULL && augAct->find(acttest)==augAct->end())//若从检验中取出的动作不为空并且即将存入augAct中的动作与augAct中已存入的动作不同,在augAct中插入该动作
      augAct->insert(new symList(acttest));
    curract = actlist;//将连续的动作序列赋予curract
    while ((curract != NULL) && (curract->s != NULL))//若curract中所包含的连续动作序列不为空 
    { 
      currobs = obslist;//将连续的观测序列赋予curract
      while ((currobs != NULL) && (currobs->s != NULL))//若currobs中所包含的连续观测序列不为空  
      {
	del = NULL;
	aug = new symList(curract->s, new symList(currobs->s, NULL));//开辟内存空间，存储检验
	aug->append(new symList(*iCurr));//在aug中插入检验
        if (augTest->find(aug)==augTest->end())//若即将存入augTest中的检验aug与已存入augTest中的检验不相同,则在augTest中插入该检验
	  augTest->insert(new symList(aug));
	else del = aug;//否则，将该检验aug赋予symList型del
	aug = aug->filter(SYM_ACTION);//在检验aug中取出对应的动作
	if (augAct->find(aug)==augAct->end())//若即将存入augAct中的动作与已经存入augAct中的动作不相同
	  augAct->insert(new symList(aug));//则在augAct中插入该动作
	else delete aug;//否则删除该动作
	if (del!=NULL) delete del;//若del所代表的检验不为空，则释放检验对应的内存
	currobs = currobs->n;//删除连续观测序列中的首个观测值，将剩余观测值序列赋予currobs
      } // obs
      curract = curract->n;//删除连续动作序列中的首个动作，将剩余的连续动作序列赋予curract
    } // act
    //delete acttest;
  }
  if((iCurr = augTest->find(emptyList)) != augTest->end()) //删除augTest中的空检验
    { 
      del = *iCurr;
      augTest->erase(iCurr);
      delete del;//删除augTest中的检验并释放内存空间
    }
  delete emptyList;//释放symList型内存空间


  POMDP_probData *toret1 = new POMDP_probData;//开辟内存空间用来存放定义的新结构体toret1
  toret1->ctProb	= new symListDblMat();
  toret1->ctHists	= new symList2IntMap();  

  for (iCurr = baseHist->begin(); iCurr != baseHist->end(); iCurr++)
  {
          symList2DblMap *rowDblMap = new symList2DblMap();
          for (iCurr1 = augTest->begin(); iCurr1 != augTest->end(); iCurr1++)
          {
            rowDblMap->insert(pair<symList*, double*>(*iCurr1, new double(0.0)));//初始化rowDblMap(在rowDblMap中逐对插入由symList型检验和0.0组成的映射)
          }
          toret1->ctProb->insert(pair<symList*, symList2DblMap*>(*iCurr, rowDblMap));//在 toret1->ctProb中逐对插入由symList型经历和symList2DblMap型rowDblMap组成的映射（其中rowDblMap是由检验和0.0构成的映射）
  }

  symListDblMat::iterator  piH;
  symList2DblMap::iterator piT;
  for(piH = toret1->ctProb->begin(); piH != toret1->ctProb->end();piH++)
  { 
    vector<float> *belief_end;
    vector<float>::iterator be;
    if((piH->first->size())==1)//若toret1->ctProb中存储的是空经历
    {
      belief_end=new vector<float>;
      for(be=belief->begin();be!=belief->end();be++)
      {
         belief_end->push_back(*be);//在belief_end中插入每个landmark对应的信念状态belief
      }
    }
    else//若toret1->ctProb中存储的非空经历
    {
      cout<<"piH->first="<<*(piH->first)<<endl;
      vector<Symbol*> *testData=env->getVecdata(piH->first);//将symList型的经历转化为vector<Symbol*>型的经历a，o
      belief_end=env->getBeliefState(belief,testData);//计算在当前belief下，经过经历testData后，得到观测值后的信念状态
      delete testData;//释放testData内存
    }
    for(piT= piH->second->begin(); piT!= piH->second->end();piT++)
    {     
        *(piT->second)=env->getPomdpProb(belief_end,piT->first);//在当前状态belief_end下，采取检验a-o后，观测序列出现的概率，映射中对应的double型数值0.0更新为该概率值
    }
    delete belief_end;//释放belief_end中所占用的内存
  }

  int size_m=toret1->ctProb->size();//定义size_m为toret1->ctProb的维数，其中toret1->ctProb为 <symList *,<symList *, double*> >组成的映射
  int size_n=toret1->ctProb->begin()->second->size();//定义size_n为toret1->ctProb中<symList *, double*>组成映射的维数

  toret1->probs	= new Array2D<double>(size_m, size_n); *(toret1->probs) = 0.0;//定义二维动态数组并初始化
 
  int row_size, column_size;
  for(row_size = 0, piH = toret1->ctProb->begin(); piH != toret1->ctProb->end(); row_size++,piH++)
  { 
    for(column_size = 0,piT= piH->second->begin(); piT!= piH->second->end(); column_size++,piT++)
    {
      (*toret1->probs)[row_size][column_size] = *(piT->second);//將toret1->ctProb的概率复制给toret1->probs
    }
  }

  toret1->hcounts = new Array1D<int>(size_m, 0);*(toret1->hcounts) = 0;//开辟内存空间，建立新的一维动态数组，存放的是经历发生的次数			    
  symList2IntMap::iterator histIt;
  for(row_size = 0,histIt= toret1->ctHists->begin();histIt!= toret1->ctHists->end(); row_size++,histIt++)
  {
    (*toret1->hcounts)[row_size] = *(histIt->second);//將toret1->ctHists的概率复制给toret1->hcounts
  }

  cout<<"HHHHHH"<<endl;
  delete augTest;

  return toret1;//返回更新后的结构体toret1
}


/****************************
函数功能：
通过检验发生的次数和动作发生的次数,估计给定经历和检验下的相应概率
输入参数:
env--Environment类传递的参数;
baseTest--给定的检验的集合;
baseHist--给定的经历的集合;
elementVec--每一个landmark对应的子状态空间内的经历的集合;
testLen--检验的最大长度;
输出：估算得到的给定经历和检验下的概率矩阵
*****************************/
tEstimateData *Sampler::augEstimate(Environment *env, symSet* baseTest, symSet* baseHist, vector<symList*> *elementVec, int testLen)
{  
  symIter iCurr,iCurr1,iCurr2;
  symSet *augTest = new symSet(), *augAct = new symSet();
  if(testLen==1)//若检验的长度为1
  {
    cout<<"testLen="<<testLen<<endl;

    for (iCurr1 = baseTest->begin(); iCurr1 != baseTest->end(); iCurr1++)
    {
       cout<<"baseTest="<<**iCurr1<<endl;//输出baseTest中所包含的检验  
    }
    for (iCurr1 = baseTest->begin(); iCurr1 != baseTest->end(); iCurr1++)
    {
      augTest->insert(new symList(*iCurr1));//在augTest中插入baseTest中所包含的检验
    }

    for (iCurr1 = baseTest->begin(); iCurr1 != baseTest->end(); iCurr1++)//遍历baseTest中的每个检验
    {
       symList *aug = (*iCurr1)->filter(SYM_ACTION);//在检验中取出其所对应的动作
       if (augAct->find(aug)==augAct->end())//若即将插入augAct中的动作与augAct中已经插入的动作不相同
       {
         augAct->insert(new symList(aug));//则在augAct中插入该动作
       }
       delete aug;//否则删除该动作    
    }
  }

  if(testLen>1)//若检验的长度大于1
  { 
    symList *emptyList = new symList();//开辟空经历emptyList的内存空间
    symList *del;
    
    symList *actlist = env->getActions();//获得连续的动作序列，如a1a2...an
    cout<<"actlist="<<*actlist<<endl;
    symList *obslist = env->getObservations();//获得连续的观测值序列，如o1o2...on
    cout<<"obslist="<<*obslist<<endl;
    symList *curract,*currobs,*acttest,*aug;

    symIter iTBegin = baseTest->begin(), iTEnd = baseTest->end(),iCurr, iCurr2;

    if((iCurr = augTest->find(emptyList)) != augTest->end())//删除augTest中的空检验
         { //将 empty test从 augTest中抹去
      del = *iCurr;
      augTest->erase(iCurr);
      delete del;
    }


    curract = actlist;//将连续的动作序列赋予curract
    for (iCurr = iTBegin; iCurr != iTEnd; iCurr++) 
    {
      acttest = (*iCurr)->filter(SYM_ACTION);//在检验中取出对应的动作
      if(augTest->find(*iCurr)==augTest->end())//若即将插入augTest中的检验与augTest中已经插入的检验不相同
        augTest->insert(new symList(*iCurr));//则在augTest中插入该检验
      if(acttest!=NULL && augAct->find(acttest)==augAct->end())//若即将插入augAct中的动作不为空且与augAct中已经插入的动作不相同
        augAct->insert(new symList(acttest));//则在augAct中插入该动作
      curract = actlist;//将连续的动作序列赋予curract
      while ((curract != NULL) && (curract->s != NULL))//若curract中所包含的连续动作序列不为空且其首个动作不为空？  
      { 
        currobs = obslist;//将连续的观测序列赋予currobs
        while ((currobs != NULL) && (currobs->s != NULL))//若curract中所包含的连续动作序列不为空且其首个动作不为空？  
        {
	  del = NULL;
	  aug = new symList(curract->s, new symList(currobs->s, NULL));//开辟内存空间，存储检验
	  aug->append(new symList(*iCurr)); //在aug中存入baseTest对应的检验
	  if (augTest->find(aug)==augTest->end())//若即将存入augTest中的检验aug与已存入augTest中的检验不相同
            augTest->insert(new symList(aug));//在augTest中插入该检验
	  else 
            del = aug;//否则将该检验赋予del
	  aug = aug->filter(SYM_ACTION);//从检验中取出对应的动作
	  if (augAct->find(aug)==augAct->end()) //若即将插入augAct中的动作与augAct中已经插入的动作不相同
	    augAct->insert(new symList(aug));//则在augAct中插入该动作
	  else 
            delete aug;//否则删除该动作
	  if (del!=NULL) delete del;//若del所代表的检验不为空，则删除del所代表的检验
	  currobs = currobs->n;//删除连续观测序列中的首个观测值，将剩余观测值序列赋予currobs
        } // obs
        curract = curract->n;//删除连续动作序列中的首个动作，将剩余动作序列赋予curract
      } // act
      //delete acttest;
    }//上面的工作完成的是将所有baseTest及其扩展放到augTest中去，同时将baseTest中的动作序列提取出来及其一步的动作扩展放到 augAct中去
 
    // Even if the empty test was given, it should not occur among tests.注意空历史是允许的
    if((iCurr = augTest->find(emptyList)) != augTest->end())//删除augTest中的空检验
         { //将 empty test从 augTest中抹去
      del = *iCurr;
      augTest->erase(iCurr);
      delete del;
    }
    delete emptyList;

  }
   ofstream augTestData("augTestData.txt");
   for (iCurr1 = augTest->begin(); iCurr1 != augTest->end(); iCurr1++)
    {
      cout<<"augTest="<<**iCurr1<<endl;//输出baseTest中所有检验
augTestData<<"augTest="<<**iCurr1<<endl;//输出baseTest中所有检验
    }

    tEstimateData *toret = new tEstimateData;
    toret->ctProb = new symListDblMat();
    toret->ctMat  = new symListMatrix();
    toret->ctAct  = new symListMatrix();
    toret->ctHists= new symList2IntMap();

    
    for(iCurr = baseHist->begin(); iCurr != baseHist->end();iCurr++)
    {
          symList2DblMap *rowDblMap = new symList2DblMap();//开辟symList2DblMap型内存空间（由<symList*, double*>组成的映射）
          symList2IntMap *rowMap = new symList2IntMap();//开辟symList2IntMap型内存空间（由<symList*, int*>组成的映射）
          for (iCurr1 = augTest->begin(); iCurr1 != augTest->end(); iCurr1++)
          {
            rowDblMap->insert(pair<symList*, double*>(*iCurr1, new double(0.0)));//在rowDblMap中逐对插入由symList型检验和0.0组成的映射
            rowMap->insert(pair<symList*, int*>(*iCurr1, new int(0)));//在rowMap中逐对插入由symList型检验和整形数0组成的映射
          }
          toret->ctProb->insert(pair<symList*, symList2DblMap*>(new symList(*iCurr), rowDblMap));//在toret->ctProb中逐对插入由symList型经历和symList2DblMap型rowDblMap组成的映射（其中rowDblMap是由检验和0.0构成的映射）
          toret->ctMat->insert(pair<symList*, symList2IntMap*>(new symList(*iCurr), rowMap));//在toret->ctMat中逐对插入由symList型经历和symList2IntMap型rowMap组成的映射（其中rowMap是由检验和0构成的映射）
	
	  rowMap = new symList2IntMap();//开辟symList2IntMap型内存空间，由<symList*, int*>组成的映射
	  for (iCurr2 = augAct->begin(); iCurr2 != augAct->end(); iCurr2++)
	  {
             rowMap->insert(pair<symList*, int*>(*iCurr2, new int(0)));//在rowMap中插入由symList型动作和int型0构成的映射
          }
	  toret->ctAct->insert(pair<symList*, symList2IntMap*>(*iCurr,rowMap));//在toret->ctAct中插入由symList型经历和symList2IntMap型rowMap组成的映射（其中rowDblMap是由动作和0构成的映射）

         toret->ctHists->insert(pair<symList*, int*>(new symList(*iCurr),new int(0)));//在toret->ctHists中插入由symList型经历和Int型0组成的映射
    }

    for(vector<symList*>::iterator begvec = elementVec->begin();begvec != elementVec->end();begvec++)
    { 
      int n=(*begvec)->size();//elementVec中每一个经历的长度
      vector<Symbol*>* datavec=env->getVecdata(*begvec);//将symList型经历转化为vector<Symbol*>型经历，存入datavec中
      symList *watchedHist0= getHistoryL(*begvec, 0, 0); //获取每一个经历的空经历，将其赋予watchedHist0    

      if(baseHist->find(watchedHist0) != baseHist->end())
      {
         *(toret->ctHists->find(watchedHist0)->second) += 1;//toret->ctHists中对应的空经历发生次数+1

         for(int j=1;j<=testLen;j++)
         { 
           symList* watchTest0 = getTestL(*begvec, 0, j);//获取对应的经历中位置为0，长度为j的检验，将其赋予watchTest0
      
           if(watchTest0!= NULL)//if((watchTest0->size())>1)获取的检验不为空
           {
              if(augTest->find(watchTest0) != augTest->end());//在augTest中有与之相同的检验
              {
                 symList* acttest0 = watchTest0->filter(SYM_ACTION);//则在该检验中取出对应的动作，赋予acttest0
	         if((toret->ctMat->find(watchedHist0)->second->find(watchTest0))!=(toret->ctMat->find(watchedHist0)->second->end()))//toret->ctMat矩阵中存在与之相同的检验
	         *(toret->ctMat->find(watchedHist0)->second->find(watchTest0)->second) += 1;//则toret->ctMat矩阵中检验对应的发生次数+1
                 if(toret->ctAct->find(watchedHist0)->second->find(acttest0) != toret->ctAct->find(watchedHist0)->second->end())//toret->ctAct矩阵中存在与acttest0相同的动作
	         *(toret->ctAct->find(watchedHist0)->second->find(acttest0)->second) += 1;//则在toret->ctAct矩阵中该动作对应的发生次+1
	         //if(acttest0 != new symList())
	         delete acttest0;//删除acttest所对应的动作
                 acttest0=NULL;//acttest为空值
              }
	   }
           delete watchTest0;//删除watchTest0所对应的经历
           watchTest0=NULL;//watchTest0为空值
         }
         delete watchedHist0;//删除所获得的空经历
      }
             

      for(int i=1; i<n; i+=2) //开始获取长度不为0的经历              
      {
       symList *watchedHist= getHistory(datavec, i, (i+1)/2);//从位置i开始，逐步获取所有可能的经历
       if(baseHist->find(watchedHist) != baseHist->end())//若baseHist中存在与获取的经历watchedHist相同的经历
       {
          *(toret->ctHists->find(watchedHist)->second) += 1;//则在toret->ctHists中对应的经历的发生次数+1
          for(int j=1;j<=testLen;j++)
          {   
             symList *watchTest = getTest(datavec,i,j);//获取经历datavec中获取从位置i开始指定长度为j的检验，将其赋予watchTest   
             if(watchTest != NULL)//若获取的检验不为空
             {
               if(augTest->find(watchTest) != augTest->end());//若在augTest中存在与之相同的检验
               {
                 symList *acttest = watchTest->filter(SYM_ACTION);//则在检验watchTest中取出对应的动作，并将其赋予acttest
	         if((toret->ctMat->find(watchedHist)->second->find(watchTest))!=(toret->ctMat->find(watchedHist)->second->end()))//若在toret->ctMat中该经历watchedHist对应的检验与所获得的检验相同
	           *(toret->ctMat->find(watchedHist)->second->find(watchTest)->second) += 1;//则toret->ctMat矩阵中该检验，经历对应的发生次数+1
                 if(toret->ctAct->find(watchedHist)->second->find(acttest) != toret->ctAct->find(watchedHist)->second->end())//若toret->ctAct中存在与从检验中取出的动作相同的动作
	           *(toret->ctAct->find(watchedHist)->second->find(acttest)->second) += 1;//则动作对应的发生次数+1
	         delete acttest;//删除acttest所占的内存
                 acttest=NULL;//将acttest置为空
               }
             }
             delete watchTest;//删除watchTest所占的内存
             watchTest=NULL; //将watchTest置为空
          }
       } 
       delete watchedHist;//删除watchedHist所占的内存
     } 
     delete datavec;//删除datavec所占的内存
     datavec=NULL;//将datavec置为空
    }

    symList2IntMap::iterator histIterator;
    symList2DblMap::iterator probIterator, tempProbIterator;
    double denom;
    for (iCurr1 = baseHist->begin(); iCurr1 != baseHist->end(); iCurr1++)//以下是计算在经历h下，对应的检验发生概率P(t|h)
    {
      for (iCurr2 = augTest->begin(); iCurr2 != augTest->end(); iCurr2++) 
      {
        symList *acttest = (*iCurr2)->filter(SYM_ACTION);//在augTest中取出所有检验对应的动作
        denom = *(toret->ctAct->find(*iCurr1)->second->find(acttest)->second);//denom为toret->ctAct矩阵中对应的动作发生次数        
        *(toret->ctProb->find(*iCurr1)->second->find(*iCurr2)->second) = denom==0 ? 0 :(double) *(toret->ctMat->find(*iCurr1)->second->find(*iCurr2)->second)/denom;//若该检验对应的动作的发生次数为0，则在toret->ctMat中该检验发生的概率为0，反之，则toret->ctMat矩阵中该检验发生的概率=（该检验发生次数/该检验对应的动作的发生次数）
        delete acttest;//删除acttest所占的内存
      }
    }

    int size_m = toret->ctMat->size();//定义size_m为toret->ctMat中经历的数目
    int size_n = toret->ctMat->begin()->second->size();//定义size_n为toret->ctMat中由检验和其发生次数组成的映射的数目
    int size_act = toret->ctAct->begin()->second->size();//定义size_act为toret->ctAct中由动作和其发生次数组成的映射的数目

    toret->probs= new Array2D<double>(size_m, size_n);	*(toret->probs) = 0.0;//定义toret->probs为double型二维动态数组，其中行数为经历的数目，列数为检验和其发生的概率组成的映射的数目，数组中的元素初始化为0.0
    toret->counts= new Array2D<int>(size_m, size_n);	*(toret->counts) = 0;//定义toret->counts为int型二维动态数组，其中行数为经历的数目，列数为检验和其发生次数组成的映射的数目，数组中的元素初始化为0
    toret->actcounts = new Array2D<int>(size_m, augAct->size());*(toret->actcounts) = 0;//定义toret->actcounts为int型二维动态数组，其中行数为经历的数目，列数为动作和其发生次数组成的映射的数目，数组中的元素初始化为0
    toret->hcounts= new Array1D<int>(size_m, 0);*(toret->hcounts) = 0;//定义toret->hcounts为int型一维动态数组，其中其中行数为经历的数目，数组中的元素初始化为0
    int row_size, column_size;
    symListDblMat::iterator  tiH;
    symListMatrix::iterator  iH, aiH;
    for(row_size = 0, tiH = toret->ctProb->begin(), iH = toret->ctMat->begin(); tiH != toret->ctProb->end(), iH != toret->ctMat->end(); row_size++, tiH++, iH++)
    { 
      for(column_size = 0,  probIterator = tiH->second->begin(), histIterator = iH->second->begin(); probIterator != tiH->second->end(), histIterator != iH->second->end(); column_size++, probIterator++, histIterator++)
      {
        (*toret->probs)[row_size][column_size] = *(probIterator->second);//定义toret->probs中的元素为toret->ctProb中在经历下，对应的检验发生的概率

        (*toret->counts)[row_size][column_size] = *(histIterator->second);//定义toret->counts中的元素为toret->ctMat中在经历下，对应的检验发生的次数
      }
    }

    for(row_size = 0, aiH = toret->ctAct->begin(); aiH != toret->ctAct->end(); row_size++, aiH++)
    {
      for(column_size = 0, iCurr2 = augAct->begin(); iCurr2 != augAct->end(); column_size++, iCurr2++)
      {
        (*toret->actcounts)[row_size][column_size] = *(aiH->second->find(*iCurr2)->second);//定义toret->actcounts中的元素为toret->ctAct中在经历下，检验对应的动作的发生的次数
      }
    }

    for(row_size = 0, histIterator = toret->ctHists->begin(); histIterator != toret->ctHists->end(); row_size++, histIterator++)
    {
      (*toret->hcounts)[row_size] = *(histIterator->second);//定义toret->hcounts中的元素为toret->ctHist中在经历发生的次数
    }

    delete augAct; //释放augAct所占用的内存
    delete augTest;//释放augTest所占用的内存
    return toret;
}


/***************************
函数功能：获取symList数据data中位置为pos，长度为len的记忆
输入参数：
data——symList型操作数据；
pos——指定的开始位置；
len——要获取的记忆的长度
输出：data中位置为pos，长度为len的记忆
***************************/
symList* Sampler::getMemoryL(symList* data, int pos, int len) //获取symList数据data中位置为pos，长度为len的记忆，并将其类型转化为symList型
{
  int startH = pos - len + 1;//确定所要获取记忆的起始位置
  symList* currMemory = new symList();//开辟symList型内存空间，用来存放记忆
  if((pos+1) >= len)              //若所在的位置大于所要获取记忆的长度（满足获取记忆的条件）
  {
    for(int i=0; i<startH; i++)   //删除从0位置到起始位置之间的数据，使其指向起始位置   
    data=data->n;
  
    Symbol* mark;
    vector<Symbol*>* vmdata=new vector<Symbol*>;
    for(int i=0; i<len; i++)        //开始获取长度为len的记忆   
    {
      mark=data->s;                 //获取data数据的第一个元素
      vmdata->push_back( mark) ;    //将该元素插入vmdata中
      data=data->n;                 //删除该元素，此时data为剩余的数据
      //cout<<"data="<<*data<<endl;   
    } 

    vecsymIter vi;
    for(vi=vmdata->begin();vi!=vmdata->end();vi++)
    {
      currMemory->append( new symList(*vi) ) ;//将获得的记忆转化为symList型，并插入到currMemory中
    }
   delete vmdata;                             //删除 vmdata中存放的记忆，释放内存
   return currMemory;                         //返回symList型的记忆
  }
  else
    return NULL;                              //若获取不成功，则返回值为空
}

/***************************
函数功能：获取symList数据data中位置为pos，长度为len的经历
输入参数：
data——symList型操作数据；
pos——指定的开始位置；
len——要获取的经历的长度
输出：data中位置为pos，长度为len的经历
***************************/
symList* Sampler::getHistoryL(symList* data, int pos, int len) //获取symList数据data中位置为pos，长度为len的经历，并将其类型转化为symList型
{
  symList* currHistory = new symList();        
  if( ((pos+1)/2) >= len)                  //若所在的位置大于所要获取经历的长度（满足获取经历的条件）
  {
    pos += 1;                           
    int startH = pos - len*2;              //确定所要获取经历的起始位置startH
    Symbol* mark;
    for(int i=0; i<startH; i++)
    data=data->n;                          //删除从0位置到起始位置之间的数据，使其指向起始位置
    
    vector<Symbol*>* vhdata=new vector<Symbol*>;
    for(int i=0; i<len*2; i++)
    {
      mark=data->s;                        //获得所要获取数据的第一个元素
      vhdata->push_back( mark) ;           //在vhdata中插入该元素
      data=data->n;                        //删除该元素，并指向下一个
    }

    vecsymIter vi;
    for(vi=vhdata->begin();vi!=vhdata->end();vi++)
    {
     currHistory->append(*vi) ;            //将获得的经历转化为symList型，并插入到currHistory中
    //currHistory->append( new symList(*vi) ) ;
    }
   delete vhdata;                          //删除 vhdata中存放的经历，释放内存
   return currHistory;                     //返回symList型的经历              
  }
  else                                     //若获取不成功，则返回值为空
    return NULL;

}


/***************************
函数功能：获取symList数据data中位置为pos，长度为len的检验(data以o开头）
输入参数：
data——symList型操作数据；
pos——指定的开始位置；
len——要获取的经历的检验
输出：data中位置为pos，长度为len的检验
***************************/
symList* Sampler::getTestL(symList* data, int pos, int len)//获取symList数据中位置为pos，长度为len的检验，并将其类型转化为symList型
{ 
  symList* currTest = new symList();
  if((pos+len*2) <= data->size())       //若从获取位置开始，要获取的检验没有达到数据末端（满足获取检验的条件）           
  { 
    Symbol* mark;
    for(int i=0; i<pos; i++)
    data=data->n;                       //删除指定获取位置之前的数据ao    
    
    vector<Symbol*>* vtdata=new vector<Symbol*>;
    if(pos==0)                          //若指定位置为从0   
    { 
      for(int i=0; i<len*2; i++)        //从0位置开始获取      
      {
        mark=data->s;                   //获得数据data中的元素第一个元素
        vtdata->push_back( mark) ;      //将第一个元素插入vtdata中
        data=data->n;                   //删除该元素，开始下一个获取的循环，直到获取完指定的长度为止
      }
    }
    else                                //若指定位置不为0                                
    {
      for(int i=0; i<len*2; i++)        //从指定位置开始获取    
      {
        data=data->n;                   //先删除第一个元素                    
        mark=data->s;                   //获取下一个元素
        vtdata->push_back( mark) ;      //将获取的该元素插入vtdata，开始下一个循环，知道获取完指定长度为止
      }
    }

    vecsymIter vi;
    for(vi=vtdata->begin();vi!=vtdata->end();vi++)
    {
      currTest->append(*vi) ;;          //在symList型的currTest中插入获取的检验，使检验变为symList型
    }
    delete vtdata;
    return currTest;                    //返回symList型的检验                
  }
  else
    return NULL;                        //否则返回为空
}
/***************************
函数功能：获取symList数据data中位置为pos，长度为len的检验(data以a开头）
输入参数：
data——symList型操作数据；
pos——指定的开始位置；
len——要获取的经历的检验
输出：data中位置为pos，长度为len的检验
***************************/
symList* Sampler::getTestL2(symList* data, int pos, int len)//获取symList数据中位置为pos，长度为len的检验，并将其类型转化为symList型
{ 
  symList* currTest = new symList();
  if((pos+len*2) <= data->size())       //若从获取位置开始，要获取的检验没有达到数据末端（满足获取检验的条件）           
  { 
    Symbol* mark;
    for(int i=1; i<pos; i++)
    data=data->n;                       //删除指定获取位置之前的数据ao    
    
    vector<Symbol*>* vtdata=new vector<Symbol*>;
    if(pos==0)                          //若指定位置为从0   
    { 
      for(int i=0; i<len*2; i++)        //从0位置开始获取      
      {
        mark=data->s;                   //获得数据data中的元素第一个元素
        vtdata->push_back( mark) ;      //将第一个元素插入vtdata中
        data=data->n;                   //删除该元素，开始下一个获取的循环，直到获取完指定的长度为止
      }
    }
    else                                //若指定位置不为0                                
    {
      for(int i=0; i<len*2; i++)        //从指定位置开始获取    
      {
        data=data->n;                   //先删除第一个元素                    
        mark=data->s;                   //获取下一个元素
        vtdata->push_back( mark) ;      //将获取的该元素插入vtdata，开始下一个循环，知道获取完指定长度为止
      }
    }

    vecsymIter vi;
    for(vi=vtdata->begin();vi!=vtdata->end();vi++)
    {
      currTest->append(*vi) ;;          //在symList型的currTest中插入获取的检验，使检验变为symList型
    }
    delete vtdata;
    return currTest;                    //返回symList型的检验                
  }
  else
    return NULL;                        //否则返回为空
}











/***************************
函数功能：获取vector<Symbol*>数据data中位置为pos，长度为len的检验
输入参数：
data——vector<Symbol*>型操作数据；
pos——指定的开始位置；
len——要获取的检验的长度
输出：data中位置为pos，长度为len的检验
***************************/
symList* Sampler::getTest(vector<Symbol*>* data, int pos, int len) //获取vector<Symbol*>型数据从位置pos开始指定长度（len）的检验
{
  if((pos+len*2) < data->size())               //若获取的检验没有达到数据末端（满足获取检验的条件）      
  { 
    pos += 1;                                  //指向下一个位置
    symList* currTest = new symList();
    for(int i=0; i<len; i++)
    {
      
      currTest->append( new symList(data->at(pos), new symList(data->at(pos+1), NULL)) ) ;//在currTest中插入检验ao，将其转化为 symList型
      pos += 2;                   
    }
    return currTest;                            //若获取成功，则返回 所获得的symList型检验 
  }
  else
    return NULL;                                //若获取不成功，则返回值为空
}




/***************************
函数功能：获取vector<Symbol*>数据data中位置为pos，长度为len的经历
输入参数：
data——vector<Symbol*>型操作数据；
pos——指定的开始位置；
len——要获取的经历的长度
输出：data中位置为pos，长度为len的经历
***************************/
symList* Sampler::getHistory(vector<Symbol*>* data, int pos, int len) //获取vector<Symbol*>型数据从位置pos开始指定长度（len）的经历
{
  if( ((pos+1)/2) >= len)                       //若所在的位置大于所获取的长度（满足获取条件）                     
  {
    pos += 1;                                   //真实的位置为pos+1
    int startH = pos - len*2;                   //确定所要获取的初始位置
    symList* currHistory = new symList();
    for(int i=0; i<len; i++)
    {
      currHistory->append( new symList(data->at(startH), new symList(data->at(startH+1))) );//在currHistory中插入经历ao
      startH += 2;                              //指向下一个所要获取的位置
    }
    return currHistory;                         //返回获取完后的经历                     
  }
  else
    return NULL;                               //若获取不成功，则返回值为空
}



/***************************
函数功能：获取vector<Symbol*>数据data中位置为pos，长度为len的记忆
输入参数：
data——vector<Symbol*>型操作数据；
pos——指定的开始位置；
len——要获取的记忆的长度
输出：data中位置为pos，长度为len的记忆
***************************/
symList* Sampler::getMemory(vector<Symbol*>* data, int pos, int len) //获取vector<Symbol*>型数据从位置pos开始指定长度的记忆
{
  int startH = pos - len + 1;                  //该式子表示指定初始位置startH
  symList* currMemory = new symList();       
  if((pos+1) >= len)                           //若所在位置大于获取记忆的长度（满足获取记忆的条件）
  {
    for(int i=startH; i<=pos; i++)             //从指定初始位置开始获取记忆
    {
      currMemory->append(data->at(startH));    //在currMemory后插入所获得的记忆？？？？
      //currMemory->append(new symList(data->at(startH)));
    }
      return currMemory;                       //若获取成功，则返回所获得的symList型记忆
  }
  else
    return NULL;                               //若获取不成功，则返回值为空
}




  

  

