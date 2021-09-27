#ifndef __PSR_LOCAL_CPP_
#define __PSR_LOCAL_CPP_


#include <fstream>
#include <iostream>
#include <time.h>
#include <iomanip>
#include "symbol.h"
#include <math.h>
#include "unistd.h"

#include "psr-local.h"

using namespace std;

#define Length_TestData 21000
//1:结果为nan,去除landmark

void psr_local::learn()
{   
  	srand((unsigned int)time(NULL));
  	vector<float> *belief=getStartbelief();
  	cout << "Generate training data" << endl;
  	vector<symList*>* divide = env->getData5(belief, 300, 8);
  	int S = 8, O = 12, A = 7;
	PomdpData * RandomPOMDP=getRandomPomdp(A,S,O);//获取初始随机的POMDP模型

	symList* NewAbstractData = new symList;//所有串数据合成一串
	for(vector<symList*>::iterator begvec = divide->begin();begvec != divide->end();begvec++)
	{ 
 		NewAbstractData->append(*begvec);
 	}

	
	RandomPOMDP = EM1(RandomPOMDP,NewAbstractData,A,S,O,belief);
	
  	ofstream ERRORTXT("error_pomdp.txt");
  	vector<float> *belief_start = new vector<float>;
  	vector<float>::iterator be;
  	ERRORTXT<<"belifstart= " ;
  	for(int i = 0; i < S; i++)
	{
		if(i == S-1)	
		{
			belief_start->push_back(1);
			ERRORTXT<<" "<< 1;
			break;
		}
    	belief_start->push_back(0);  
		ERRORTXT<<" "<< 0;
	} 
  	ERRORTXT<< endl;
  	
  	symSet *AOset = getAOset();
  	symIter s; 
  	float realpro = 0.0;
  	float modelpro = 0.0;
  	float accuracy = 0.0;
  	ERRORTXT<<"realpro modelpro" << endl ;
  	for(s = AOset->begin(); s != AOset->end(); s++)
  	{
  		realpro = env->getPomdpProb(belief_start, *s);
  		
  		modelpro = getabstractBeliefprob(RandomPOMDP,belief_start,*s,2);
  		
  		accuracy += pow((realpro-modelpro),2);
  		ERRORTXT << " " << realpro  << " " << modelpro << endl;
  	}
  	ERRORTXT<< endl;
  	accuracy = sqrt(accuracy);
  	ERRORTXT << accuracy << endl;
  	
/*  	  
  	vector<int>*interestTest_id=new vector<int>;
  	int j,i;
  	cout<<"input the total number of interestTest"<<endl;//对于固定感兴趣的检验,输入固定的观测总个数
  	cin>>j;
  	cout<<"j="<<j<<endl;     
  	ofstream Final("Finalresult.txt");//Finalresult.txt保存最终结果
  	Final<<"感兴趣检验观测的个数为："<<j<<endl;         
  	for(j;j>0;j--)
   	{
      	cout<<"please input interestTest id(0-6)"<<endl;//对于固定感兴趣的检验,输入固定的检验对应的观测值下标(感兴趣的检验为个数为A_sz*j)
      	cin>>i;
      	cout<<"i="<<i<<endl;
      	interestTest_id->push_back(i);
    } 
  	symSet *interestTests= env->getInterestTest(interestTest_id);//得到固定的感兴趣的检验
 
  	ofstream InterestTest("InterestTest.txt");//保存感兴趣的检验 
  	for(iCurr1=interestTests->begin();iCurr1!=interestTests->end();iCurr1++)
      	InterestTest<<**iCurr1<<endl;    
      	    
  	int action_N,hist_N,hist_maxlen;
  	cout<<endl<<"input the action_N(100-2000)"<<endl;//设定感兴趣的检验的动作循环次数
  	cin>>action_N;
  	cout<<"action_N="<<action_N<<endl;
 	Final<<"动作循环的次数为："<<action_N<<endl;  
 		clock_t start = clock();//计算分组及建模的时间
 	symList2IntMap *Histcout = getHistcout1(divide);//得到整体状态空间发生的经历和经历发生的次数  
  	tEstimateData *Probmatrix=getProb(interestTests,belief,action_N,Histcout);//估计待分组的p(t|h)矩阵
  	vector<symList2Prob*> * H_Groups;
  	ofstream ERROS("ERROS.txt");
  	
   
	H_Groups=find_H_Groups1(Probmatrix);//线性无关方法
	//H_Groups=find_H_Groups21(Probmatrix);//似然比检验方法
	
	ERROS << "find_H_Groups1:线性无关" << endl;
	symSet *AOset1=getAOset();//得到所有动作-观测ao对的集合AO 
//	cout << "AOset1->size() =" << AOset1->size() << endl;
	symList2SymbolMap *newAct=env->getNewAct(AOset1);//获取所有新动作及其对应内容
	int size_NewAct=newAct->size();//获取新动作的个数
	cout << "newAct->size() = " << newAct->size() << endl;
	
	map<symList*, Symbol*>::iterator na;
	for(na=newAct->begin();na != newAct->end();na++) 
	{
		
		cout<<"ao="<<*(na->first)<<"newAct="<<*(na->second)<<endl;
	} 
	                       
	int size_NewOb=getTrueNum_NewObs1(H_Groups);
	int size_newS=env->getStatesize();//这里的状态数=原始系统中的状态数，而不是等于观测数，值得考虑
	
	symListDblMap *newB_prob=getNewObs_prob1(H_Groups, size_NewOb);
	symList_symListMap *S_h_newB =getH_NewObs1( H_Groups,size_NewOb);

//后面都是一串数据处理的，考虑一下能不能分几串处理
	vector<symList*>* NewAbstractData = new vector<symList*>;
	symList* NewAbstractData1 = new symList;//所有串数据合成一串
	for(vector<symList*>::iterator begvec = divide->begin();begvec != divide->end();begvec++)
	{ 
 		symList* NewData=getAbstract1(*begvec, S_h_newB, newAct);//一串ao序列进行抽象
 		NewAbstractData->push_back(NewData);
 		NewAbstractData1->append(NewData);
 	}
	
//	symList*NewAbstractData=getAbstract(conData,landmarkSet,landmarkhis,S_h_newB,newAct,startPosLm,startPos);
	//获得一长串完整的抽象后的经历(symList型)
	PomdpData * RandomPOMDP=getRandomPomdp(size_NewAct,size_newS,size_NewOb);//获取初始随机的POMDP模型
	RandomPOMDP = EM1(RandomPOMDP,NewAbstractData1,size_NewAct,size_newS,size_NewOb,belief);

 //	vector<symList*>::iterator begvec = NewAbstractData->begin();
// 	RandomPOMDP = EM1(RandomPOMDP,*begvec,size_NewAct,size_newS,size_NewOb,belief);
//	PomdpData* POMDPmodle=EM(RandomPOMDP,startPosLm,NewAbstractData,size_NewAct,size_newS,size_NewOb,belief);//利用EM算法获得POMDP模型
	clock_t end = clock();
	float average= 0.0;
	for(int e=0; e < 10; e++)
	{
		double errsum=getError1(RandomPOMDP,newB_prob,newAct,interestTests, size_newS );
	      ERROS << "errsum = " << errsum << endl;
	      average += errsum;
      //求EM算法建立的POMDP模型与准确模型之间的误差值
	}
	double aver_err = average/10;
	ERROS << "average = " << aver_err << endl;
	cout << "average = " << aver_err << endl;
	
	int ways = 2;
	getFinalresult(Final,ways,aver_err,size_NewOb,start,end);//将实验结果存储起来
*/	
	
	
	
}

int psr_local::getTrueNum_NewObs1(vector<symList2Prob*>* H_Groups)
 {
  	int size_NewOb=0;
  
		  for(vs=H_Groups->begin();vs!=H_Groups->end();vs++)
		     {	
			    size_NewOb++;
		     }

  return size_NewOb;
 }


vector<float> *psr_local::getStartbelief()
{
	vector<float> *startbelief=new vector<float>;
	ifstream in("startbelief.txt");//读入文件绝对路徑
  	for(float id;in>>id;)//读取输入文件内容
  	{
    	startbelief->push_back(id);
    	//cout << id << endl;
  	}	
	return startbelief;
}
/*********************************************************************************************
函数功能：获取所有新测值及其对应内容
函数输入:分组后的H_Groups,新观測真实个数size_NewOb
函数输出：所有新观测值及其对应内容
**********************************************************************************************/
symList_symListDblMap *psr_local::getNewObs_prob(symList2Group* H_Groups,int size_NewOb)
 {
	ofstream newBtxt("newB.txt");
	Symbol **newB = (Symbol **) malloc(size_NewOb * sizeof(Symbol *));//newB[i]存放所有新观測(真实个数) 
	int i=0;
  symList_symListDblMap *newB_prob=new symList_symListDblMap;
	for( shp=H_Groups->begin();shp!=H_Groups->end();shp++)
	   {  
		  symListDblMap *B_prob=new symListDblMap;
		  for(vs=shp->second->begin();vs!=shp->second->end();vs++)
		     {
			    newB[i]=env->getNewObservation(i);  //新观測
			    vec_dou *prob=new vec_dou;//新测值对应内容
			    sp=(*vs)->begin();		
			    for(sd=sp->second->begin();sd!=sp->second->end();sd++)
			       {
				      prob->push_back(sd->second);
			       }
			    B_prob->insert(pair<symList*, vec_dou*>(new symList(newB[i]),prob));
			    i++;
		     }
		  newB_prob->insert(pair<symList*, symListDblMap*>(new symList(shp->first),B_prob));
	   }  
	map<symList*,symListDblMap*>::iterator ssv;
	map<symList*, vec_dou*>::iterator sv;
	vector<double>::iterator vd;
	for(ssv=newB_prob->begin();ssv!=newB_prob->end();ssv++)
	   {
      newBtxt<<"landmark="<<*(ssv->first)<<endl;
		  for(sv=ssv->second->begin();sv!=ssv->second->end();sv++)
		     {
		      newBtxt<<"newB="<<*(sv->first)<<endl;
		      for(vd=sv->second->begin();vd!=sv->second->end();vd++)
		         {
			        newBtxt<<*vd<<" ";
		         }
	      	newBtxt<<endl;
		     }
     }
  return newB_prob;
 }

symListDblMap *psr_local::getNewObs_prob1(vector<symList2Prob*>* H_Groups,int size_NewOb)
 {
	ofstream newBtxt("newB1.txt");
	Symbol **newB = (Symbol **) malloc(size_NewOb * sizeof(Symbol *));//newB[i]存放所有新观測(真实个数) 
	int i=0;
  	//symListDblMap *newB_prob=new symList_symListDblMap;
	//for( shp=H_Groups->begin();shp!=H_Groups->end();shp++)
	//{  
		  symListDblMap *B_prob=new symListDblMap;
		  for(vs=H_Groups->begin();vs!=H_Groups->end();vs++)
		  {
			    newB[i]=env->getNewObservation(i);  //新观測
			    vec_dou *prob=new vec_dou;//新测值对应内容
			    sp=(*vs)->begin();		
			    for(sd=sp->second->begin();sd!=sp->second->end();sd++)
			    {
				      prob->push_back(sd->second);
			    }
			    B_prob->insert(pair<symList*, vec_dou*>(new symList(newB[i]),prob));
			    i++;
		     }
		//  newB_prob->insert(pair<symList*, symListDblMap*>(new symList(shp->first),B_prob));
	  // }  
	//map<symList*,symListDblMap*>::iterator ssv;
	map<symList*, vec_dou*>::iterator sv;
	vector<double>::iterator vd;
	//for(ssv=newB_prob->begin();ssv!=newB_prob->end();ssv++)
	//   {
    //  newBtxt<<"landmark="<<*(ssv->first)<<endl;
		  for(sv=B_prob->begin();sv!=B_prob->end();sv++)
		  {
		      	newBtxt<<"newB="<<*(sv->first)<<endl;
		      	for(vd=sv->second->begin();vd!=sv->second->end();vd++)
		         {
			        newBtxt<<*vd<<" ";
		         }
	      		newBtxt<<endl;
		  }
    // }
  return B_prob;
 }

/*********************************************************************************************
函数功能：获取经历h及其对应新观测值
函数输入:分组后的H_Groups,新观測真实个数size_NewOb
函数输出：经历h及其对应的新观测值
**********************************************************************************************/
symList_sym2symMap *psr_local::getH_NewObs(symList2Group* H_Groups,int size_NewOb)
 {	 
	ofstream h_newB("h_newB.txt");
	Symbol **newB = (Symbol **) malloc(size_NewOb * sizeof(Symbol *));  //newB[i]存放所有新观測(真实个数) 
	int i=0;
  symList_sym2symMap *S_h_newB=new symList_sym2symMap;
	for( shp =H_Groups->begin();shp!=H_Groups->end();shp++)
	   {  
	   	symList_symListMap *h_newB=new symList_symListMap;
		  for(vs=shp->second->begin();vs!=shp->second->end();vs++)
		     {
			    newB[i] = env->getNewObservation(i);  //新观測
			    //vec_dou *prob=new 	vec_dou;//新测值对应内容
			    for(sp=(*vs)->begin();sp!=(*vs)->end();sp++)	
			        h_newB->insert(pair<symList*,symList*>(new symList(sp->first),new symList(newB[i])));
			    i++;
		     }
		  S_h_newB->insert(pair<symList*, symList_symListMap*>(new symList(shp->first),h_newB));
	   }
	symList_sym2symMap::iterator lhb;
	map<symList*, symList*>::iterator hb;
	for(lhb=S_h_newB->begin();lhb!=S_h_newB->end();lhb++)
	   {	
      h_newB<<"landmark="<<*(lhb->first)<<endl;
		  for(hb=lhb->second->begin();hb!=lhb->second->end();hb++)
			    h_newB<<"h="<<*(hb->first)<<" newb="<<*(hb->second)<<endl;
	   }
  return S_h_newB;
 }

symList_symListMap *psr_local::getH_NewObs1(vector<symList2Prob*>* H_Groups,int size_NewOb)
 {	 
	ofstream h_newB1("h_newB1.txt");
	Symbol **newB = (Symbol **) malloc(size_NewOb * sizeof(Symbol *));  //newB[i]存放所有新观測(真实个数) 
	int i=0;
  	//symList_sym2symMap *S_h_newB=new symList_sym2symMap;
	//for( shp =H_Groups->begin();shp!=H_Groups->end();shp++)
	 //  {  
	   	symList_symListMap *h_newB=new symList_symListMap;
		  for(vs=H_Groups->begin();vs!=H_Groups->end();vs++)
		     {
			    newB[i] = env->getNewObservation(i);  //新观測
			    //vec_dou *prob=new 	vec_dou;//新测值对应内容
			    for(sp=(*vs)->begin();sp!=(*vs)->end();sp++)	
			        h_newB->insert(pair<symList*,symList*>(new symList(sp->first),new symList(newB[i])));
			    i++;
		     }
	//	  S_h_newB->insert(pair<symList*, symList_symListMap*>(new symList(shp->first),h_newB));
	//   }
	//symList_sym2symMap::iterator lhb;
	map<symList*, symList*>::iterator hb;
	//for(lhb=S_h_newB->begin();lhb!=S_h_newB->end();lhb++)
	//   {	
   //   h_newB<<"landmark="<<*(lhb->first)<<endl;
		  for(hb=h_newB->begin();hb!=h_newB->end();hb++)
			    h_newB1<<"h="<<*(hb->first)<<" newb="<<*(hb->second)<<endl;
	//   }
  return h_newB;
 }

/*********************************************************************************************
函数功能：获取新观測真实个数(与POMDP文件中不一样) 
输入参数：
	H_Groups--
输出：
	新观測真实个数 
**********************************************************************************************/
int psr_local::getTrueNum_NewObs(symList2Group* H_Groups)
 {
  int size_NewOb=0;
	for(shp =H_Groups->begin();shp!=H_Groups->end();shp++)//获取新的观測总数
	   {  
		  for(vs=shp->second->begin();vs!=shp->second->end();vs++)
		     {	
			    size_NewOb++;
		     }
	   }
  return size_NewOb;
 }


/*********************************************************************************************
函数功能：得到所有动作-观测ao对的集合AO
输入参数：无
输出：
	得到所有动作-观测ao对的集合AO
**********************************************************************************************/
symSet *psr_local::getAOset()
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


/*********************************************************************************************
函数功能：估计待分组的p(t|h)矩阵
函数输入：感兴趣的检验；初始信念状态，动作发生次数，经历及其发生次数
函数输出：结构体：经历对应的感兴趣检验发生的次数矩阵；经历对应的感兴趣检验发生的概率矩阵，经历对应的各个动作发生的次数矩阵
**********************************************************************************************/
tEstimateData *psr_local::getProb(symSet *interestTests,vector<float>*belief,int action_N,symList2IntMap *Histcout)
 {
  ofstream htxt("h.txt");
  tEstimateData *toret = new tEstimateData;
  toret->ctProb = new symListDblMat();
  toret->ctMat  = new symListMatrix();
  toret->ctAct  = new symListMatrix();
  toret->ctHists= new symList2IntMap();
  symIter s;
  int h_len=Histcout->size();
  symIter iCurr,iCurr2;
  symSet*augAct= getaugAct(interestTests,1,env);//得到动作的集合,1为检验的最大长度，可能会变化
  symList2IntMap::iterator histIterator, tempHistIterator;
  for(histIterator=Histcout->begin(); histIterator!=Histcout->end();histIterator++)//针对Histcout集合中每一个经历
     {		  
      h_len--;
		  cout<<"h_len="<<h_len<<endl;
		  symList* historyL=new symList(histIterator->first);//初始化经历和经历发生后的信念状态
		  vector<float>* newbelief=new vector<float>;
		  if(historyL->size()==1)//空经历经历不变
        {  
				 vector<float>::iterator is;
         cout<<"belief=";
 				 for(is=belief->begin();is!=belief->end();is++)    //newbelief和初始belif一样
 					   {newbelief->push_back(*is);cout<<*is<<" ";}
         cout<<endl;
        }
		  else
        {	
		 		 vector<Symbol*> *history=env->getVecdata(historyL);
         vector<Symbol*>::iterator vt=history->begin();
         cout<<"history=";
         vector<float>::iterator is;		
		 		 newbelief=env->getBeliefState(belief ,history);//的到经历history后的信念状态
				 delete history;	
			  }
		  htxt<<*historyL<<endl;
		  toret->ctHists->insert(pair<symList*, int*>(new symList(historyL),new int(*(histIterator->second))));//经历第一次发生次数为1
		  rowMap = new symList2IntMap();//开辟symList2IntMap型内存空间，由<symList*, int*>组成的映射
		  for(iCurr = augAct->begin(); iCurr != augAct->end(); iCurr++)
	 		   {
          rowMap->insert(pair<symList*, int*>(*iCurr, new int(action_N)));//经历第一次发生，储存此经历对应的动作的次数
         }
	 	  toret->ctAct->insert(pair<symList*, symList2IntMap*>(new symList(historyL),rowMap));//所有动作都发生action_N次，每次得到一个经历就扩展这个矩阵//第一次出现的经历才需要计算
		  symList2DblMap *rowDblMap = new symList2DblMap();//开辟symList2DblMap型内存空间（由<symList*, double*>组成的映射）
	    symList2IntMap *rowMap = new symList2IntMap();//开辟symList2IntMap型内存空间（由<symList*, int*>组成的映射）
      for(s=interestTests->begin();s!=interestTests->end();s++)//对每一个感兴趣的检验
       	 {
          rowDblMap->insert(pair<symList*, double*>(*s, new double(0.0)));//在rowDblMap中逐对插入由symList型检验和0.0组成的映射
     			rowMap->insert(pair<symList*, int*>(*s, new int(0)));//在rowMap中逐对插入由symList型检验和整形数0组成的映射
   		   }
		  toret->ctMat->insert(pair<symList*, symList2IntMap*>(new symList(historyL), rowMap));//此经历下此感兴趣的检验发生次数初始化成功
		  toret->ctProb->insert(pair<symList*, symList2DblMap*>(new symList(historyL), rowDblMap));//此经历下感兴趣的检验发生的概率初始化成功
      for(iCurr=augAct->begin();iCurr!=augAct->end();iCurr++)//对每一个动作，(每个检验的动作需要重复action_N次，求概率值）
			   {  
			    //symList* actList= (*s)->filter(SYM_ACTION);//则在检验*is中取出对应的动作，并将其赋予act
			    int count_a=0;//动作发生次数初始化为0；
			    int testTen=(*iCurr)->size();
	        while(count_a<action_N)//动作发生未到最大
				       { 
				        env->resert(newbelief);//从h开始获取数据
				        symList* testao= env->getData2(testTen,*iCurr);//得到此感兴趣的检验的动作下发生的一次检验	
				        if(interestTests->find(testao) != interestTests->end()) *(toret->ctMat->find(historyL)->second->find(testao)->second) += 1;//如产生的检验与感兴趣的相同，则感兴趣的检验发生次数+1				  
				           delete testao;
				        count_a++;
				       }
			   }
      for(s=interestTests->begin();s!=interestTests->end();s++)//对每一个感兴趣的检验
			   {	
				  double denom =  action_N;
      	  *(toret->ctProb->find(historyL)->second->find(*s)->second) = (denom==0)? 0 :(double) *(toret->ctMat->find(historyL)->second->find(*s)->second)/denom;//若该检验对应的动作的发生次数为0，则在toret->ctMat中该检验发生的概率为0，反之，则toret->ctMat矩阵中该检验发生的概率=（该检验发生次数/该检验对应的动作的发生次数）	
			   }
     }
  int size_m = toret->ctMat->size();//定义size_m为toret->ctMat中经历的数目
  int size_n = toret->ctMat->begin()->second->size();//定义size_n为toret->ctMat中由检验和其发生次数组成的映射的数目
  int size_act = toret->ctAct->begin()->second->size();//定义size_act为toret->ctAct中由动作和其发生次数组成的映射的数目
  symList2DblMap::iterator probIterator, tempProbIterator;
  toret->probs= new Array2D<double>(size_m, size_n);	
  *(toret->probs) = 0.0;//定义toret->probs为double型二维动态数组，其中行数为经历的数目，列数为检验和其发生的概率组成的映射的数目，数组中的元素初始化为0.0
  toret->counts= new Array2D<int>(size_m, size_n);
  *(toret->counts) = 0;//定义toret->counts为int型二维动态数组，其中行数为经历的数目，列数为检验和其发生次数组成的映射的数目，数组中的元素初始化为0
  toret->actcounts = new Array2D<int>(size_m, augAct->size());
  *(toret->actcounts) = 0;//定义toret->actcounts为int型二维动态数组，其中行数为经历的数目，列数为动作和其发生次数组成的映射的数目，数组中的元素初始化为0
  toret->hcounts= new Array1D<int>(size_m, 0);
  *(toret->hcounts) = 0;//定义toret->hcounts为int型一维动态数组，其中其中行数为经历的数目，数组中的元素初始化为0
  int row_size, column_size;
  symListDblMat::iterator  tiH;
  symListMatrix::iterator  iH, aiH;
  htxt << "*toret->probs: " << endl;
  for(row_size=0,tiH=toret->ctProb->begin(),iH=toret->ctMat->begin();tiH!=toret->ctProb->end(),iH!=toret->ctMat->end();row_size++,tiH++,iH++)
     { 
      for(column_size = 0,probIterator = tiH->second->begin(),histIterator = iH->second->begin(); probIterator != tiH->second->end(), histIterator != iH->second->end(); column_size++, probIterator++, histIterator++)
         {
          (*toret->probs)[row_size][column_size] = *(probIterator->second);//定义toret->probs中的元素为toret->ctProb中在经历下，对应的检验发生的概率
          (*toret->counts)[row_size][column_size] = *(histIterator->second);//定义toret->counts中的元素为toret->ctMat中在经历下，对应的检验发生的次数
          htxt << (*toret->probs)[row_size][column_size] << " ";
         }
         htxt << endl;
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
  return toret;
 }


/******************************
函数功能：对得到的p（t|h）概率矩阵利用似然比检验的方法进行分组
函数输入：
结构体：经历对应的感兴趣检验发生的次数矩阵；经历对应的感兴趣检验发生的概率矩阵，经历对应的各个动作发生的次数矩阵。
初始状态的观测值
函数输出：初始状态的观测值及分组结果
/******************************/
symList2Group* psr_local::find_H_Groups2(tEstimateData * matData,symList*startPosLm)
 {
  int  i,j,k;
  ofstream RANK2TXT("RANKK.txt");
  symList2Group*S_H_P=new symList2Group();
  symListDblMat *ctProbP = new symListDblMat();
  symListMatrix* ctActP  = new symListMatrix();
  symListMatrix*  ctMatP  = new symListMatrix();
  symList2IntMap*ctHists	= new symList2IntMap();  
  symSet *augTest   = new symSet();
  symSet *histories = new symSet();
  symSet *augAct = new symSet();
  for(iH=matData->ctMat->begin(); iH!=matData->ctMat->end(); iH++)//<经历，<检验，检验发生的次数> >
  	 {
    	histories->insert(new symList(iH->first));
 	   }  
  iH=matData->ctMat->begin(); 
  for(iT = iH->second->begin(); iT != iH->second->end(); iT++) 
     {
   	  augTest->insert(new symList(iT->first));
	   }
  for(tiT=matData->ctAct->begin()->second->begin();tiT!=matData->ctAct->begin()->second->end(); tiT++)//<经历，<动作，动作发生的次数> >
     {
  	  augAct->insert(new symList(tiT->first));
     } 
  int size_m=matData->ctProb->size();//<经历，<检验，检验发生的概率> > 
  int size_n=matData->ctProb->begin()->second->size();
  int counts[size_m][2*size_n];//得到经历对应的每个感兴趣的检验分别对应的检验发生和步发生次数
  double denom;
  for(i=0,iCurr = histories->begin(); iCurr != histories->end(); iCurr++,i++)//以下是计算在经历h下，对应的检验发生概率P(t|h)
     {
      for(j=0,iCurr1 = augTest->begin(); iCurr1 != augTest->end(); iCurr1++,j++) 
         {
          symList *acttest = (*iCurr1)->filter(SYM_ACTION);//在augTest中取出所有检验对应的动作
          denom = *(matData->ctAct->find(*iCurr)->second->find(acttest)->second);//denom为matData->ctAct矩阵中对应的动作发生次数   
	        counts[i][2*j] =  *(matData->ctMat->find(*iCurr)->second->find(*iCurr1)->second);
	        counts[i][2*j+1]= denom-*(matData->ctMat->find(*iCurr)->second->find(*iCurr1)->second);
          delete acttest;//删除acttest所占的内存
         }
     }
  for(i = 0;i<size_m;i++)
  	 {
      for(j = 0;j<2*size_n; j++)
   		   {
      	  cout<<counts[i][j]<<" ";
    		 }
	    cout<<endl;
  	 }
  cout<<"******************"<<endl;
  double prob[size_m][size_n];//得到经历对应的每个感兴趣的检验发生的概率
  for(i=0;i<size_m;i++)
     {
      for(j=0; j<size_n;j++)
         {
       		prob[i][j]=(*matData->probs)[i][j];
   	     }
     }
  vector<symList2Prob*>* H_Group=new vector<symList2Prob*>;
  IntSet* H_zero=new IntSet();//存放对应概率全为0的行
  IntSet* inGrouph=new IntSet();// 存放已经归类的经历的行号
  #if 01  
  for(i=0;i<size_m;i++)
     {
      if(inGrouph->find(i) != inGrouph->end()) 
         continue;//当前经历已经归类
      else//未分类则建立一个新Group
        {  
		     int maxi=0;int maxcount=0;float sum=0.0;
		     vector<symList*>* h_List=new vector<symList*>;
         symList2Prob*H_P=new  symList2Prob();   //map<symList*, <symList*, double,> > 
		     symIter is;
		     for(k=0;k<size_n;k++)
			       sum+=	prob[i][k];
		     if (sum==0.0) {H_zero->insert(i);inGrouph->insert(i);continue;}//全零行不统计
         RANK2TXT<<"**********************"<<endl; for(k=0;k<size_n;k++)   //将两行经历中两两相同的检验放在一起(包含发生和不发生的次数) 
    		 RANK2TXT<<prob[i][k]<<"  ";
     		 RANK2TXT<<endl;
         for(k=0,is=histories->begin();is!=histories->end();is++,k++) //判断从当前行开始的经历是否被分组，若未被分组，则新建一个组将其储存在内
  		      {
			       if(k==i) 
               { 
				        maxi=i;
				        int count=(* matData->hcounts)[i];maxcount=(* matData->hcounts)[i];
				        h_List->push_back(*is);				 
				        inGrouph->insert(i);//把第i行对应的行号存入已经归类的集合中
				       }
		        }              
		     for(j=i+1;j<size_m;j++)   //每个经历只与目标组的第一个经历进行卡方比较(一次)
		     if(inGrouph->find(j) != inGrouph->end()) 
            continue;
		     else//未分类,则与上面的经历的概率比较,两两求秩
           {
			      sum=0.0;
			      for(k=0;k<size_n;k++)
			          sum+=	prob[j][k];
			      if(sum==0.0) 
              {
               H_zero->insert(j);inGrouph->insert(j);continue;}//全零行不统计
			         vector<int>*comparecounts=new vector<int>;
		       	   for(k=0;k<2*size_n;k++)   //将两行经历中两两相同的检验放在一起(包含发生和不发生的次数)
				          { 
				           comparecounts->push_back(counts[i][k]);
				           comparecounts->push_back(counts[j][k]);
				          }	
			         int test=similarityTest(comparecounts);if(test==1){
							 for(k=0;k<size_n;k++)   //将两行经历中两两相同的检验放在一起(包含发生和不发生的次数) 
    					   	 RANK2TXT<<prob[j][k]<<"  ";
        					 RANK2TXT<<endl;
              }
			      delete comparecounts; 
			      if(test==1)//两个h对应的概率向量相似，则属于同一个Group
              {
					     for(k=0,is=histories->begin();is!=histories->end();k++,is++)
  		 				    {
							     if(k==j) 
                     { 				  				
								      if((* matData->hcounts)[j]>maxcount)//若归入该分组的经历所对应的的发生次数大于已经分类的经历的发生次数，则将其作为目标经历
									      {
                         maxcount=(* matData->hcounts)[j];
									       maxi=j;
									      }
				 				      h_List->push_back(*is);				 				
				              inGrouph->insert(j);//把第j行对应的行号存入已经归类的集合中
								     }
		   				    }   
				      }                       
           }           
         symList_DblMap *row_DblMap = new symList_DblMap();    //将新Group对应的概率提取出来
         for(k=0,iCurr1 = augTest->begin(); iCurr1 != augTest->end(); iCurr1++,k++)
            {
             row_DblMap->insert(pair<symList*, double>(*iCurr1,prob[maxi][k]));   //第i行经历下对应的检验及各检验对应的概率
            }
	       vector<symList*>::iterator it;
	       for(it= h_List->begin();it!=h_List->end();it++)
		        {
			       H_P->insert(pair<symList*, symList_DblMap*>(*it,row_DblMap));
		        }
             H_Group->push_back(H_P);
        }
     }
  if((H_zero->size())!=0)
    {
     symList_DblMap *row_0Map = new symList_DblMap();    //将全为0的概率提取出来
     for(iCurr1 = augTest->begin(); iCurr1 != augTest->end(); iCurr1++)
        {
         row_0Map->insert(pair<symList*, double>(*iCurr1,0.0));   //检验-0
        }
     symList2Prob*H_P0=new  symList2Prob(); symIter is;
     for(k=0,is=histories->begin();is!=histories->end();k++,is++)//将对应概率为0的行组成一组存起来
        {
		     if(H_zero->find(k) != H_zero->end())//第K个经历对应为0行
           {
		        H_P0->insert(pair<symList*, symList_DblMap*>(*is,row_0Map));
		       }
	      }
     H_Group->push_back(H_P0);//将全为0组存起来
    }
  S_H_P->insert(pair<symList*,vector<symList2Prob*>*>(new symList(startPosLm),H_Group));              
  delete inGrouph;
  delete augTest   ;
  delete histories ;
  # endif
  ofstream RANKK2("RANKK2.txt")  ;   
  for(shp =S_H_P->begin();shp!=S_H_P->end();shp++)
		 {  
      int g=1;
			RANKK2<<"landmark="<<*(shp->first)<<endl;
			for(vs=shp->second->begin();vs!=shp->second->end();vs++)
			   {
			    for(sp=(*vs)->begin();sp!=(*vs)->end();sp++)
				     {
				      for(sd=sp->second->begin();sd!=sp->second->end();sd++)
					       {
                  cout<<sd->second<<" ";RANKK2<<sd->second<<" ";
                 }
					    //cout<<endl;
					    RANKK2<<endl;
				     }
          RANKK2<<"****************groupend "<<g<<"************"<<endl;
			    g++;
			   }
		 }
  return S_H_P;
 }


vector<symList2Prob*> * psr_local::find_H_Groups21(tEstimateData * matData)
 {
  int  i,j,k;
  ofstream RANK2TXT("RANKK1.txt");
  symListDblMat *ctProbP = new symListDblMat();
  symListMatrix* ctActP  = new symListMatrix();
  symListMatrix*  ctMatP  = new symListMatrix();
  symList2IntMap*ctHists	= new symList2IntMap();  
  symSet *augTest   = new symSet();
  symSet *histories = new symSet();
  symSet *augAct = new symSet();
  for(iH=matData->ctMat->begin(); iH!=matData->ctMat->end(); iH++)//<经历，<检验，检验发生的次数> >
  	 {
    	histories->insert(new symList(iH->first));
 	   }  
  iH=matData->ctMat->begin(); 
  for(iT = iH->second->begin(); iT != iH->second->end(); iT++) 
     {
   	  augTest->insert(new symList(iT->first));
	   }
  for(tiT=matData->ctAct->begin()->second->begin();tiT!=matData->ctAct->begin()->second->end(); tiT++)//<经历，<动作，动作发生的次数> >
     {
  	  augAct->insert(new symList(tiT->first));
     } 
  int size_m=matData->ctProb->size();//<经历，<检验，检验发生的概率> > 
  int size_n=matData->ctProb->begin()->second->size();
  int counts[size_m][2*size_n];//得到经历对应的每个感兴趣的检验分别对应的检验发生和步发生次数
  double denom;
  for(i=0,iCurr = histories->begin(); iCurr != histories->end(); iCurr++,i++)//以下是计算在经历h下，对应的检验发生概率P(t|h)
     {
      for(j=0,iCurr1 = augTest->begin(); iCurr1 != augTest->end(); iCurr1++,j++) 
         {
          symList *acttest = (*iCurr1)->filter(SYM_ACTION);//在augTest中取出所有检验对应的动作
          denom = *(matData->ctAct->find(*iCurr)->second->find(acttest)->second);//denom为matData->ctAct矩阵中对应的动作发生次数   
	        counts[i][2*j] =  *(matData->ctMat->find(*iCurr)->second->find(*iCurr1)->second);
	        counts[i][2*j+1]= denom-*(matData->ctMat->find(*iCurr)->second->find(*iCurr1)->second);
          delete acttest;//删除acttest所占的内存
         }
     }
  for(i = 0;i<size_m;i++)
  	 {
      for(j = 0;j<2*size_n; j++)
   		   {
      	  cout<<counts[i][j]<<" ";
    		 }
	    cout<<endl;
  	 }
  cout<<"******************"<<endl;
  double prob[size_m][size_n];//得到经历对应的每个感兴趣的检验发生的概率
  for(i=0;i<size_m;i++)
     {
      for(j=0; j<size_n;j++)
         {
       		prob[i][j]=(*matData->probs)[i][j];
   	     }
     }
  vector<symList2Prob*>* H_Group=new vector<symList2Prob*>;
  IntSet* H_zero=new IntSet();//存放对应概率全为0的行
  IntSet* inGrouph=new IntSet();// 存放已经归类的经历的行号
  #if 01  
  for(i=0;i<size_m;i++)
     {
      if(inGrouph->find(i) != inGrouph->end()) 
         continue;//当前经历已经归类
      else//未分类则建立一个新Group
        {  
		     int maxi=0;int maxcount=0;float sum=0.0;
		     vector<symList*>* h_List=new vector<symList*>;
         symList2Prob*H_P=new  symList2Prob();   //map<symList*, <symList*, double,> > 
		     symIter is;
		     for(k=0;k<size_n;k++)
			       sum+=	prob[i][k];
		     if (sum==0.0) {H_zero->insert(i);inGrouph->insert(i);continue;}//全零行不统计
         RANK2TXT<<"**********************"<<endl; for(k=0;k<size_n;k++)   //将两行经历中两两相同的检验放在一起(包含发生和不发生的次数) 
    		 RANK2TXT<<prob[i][k]<<"  ";
     		 RANK2TXT<<endl;
         for(k=0,is=histories->begin();is!=histories->end();is++,k++) //判断从当前行开始的经历是否被分组，若未被分组，则新建一个组将其储存在内
  		      {
			       if(k==i) 
               { 
				        maxi=i;
				        int count=(* matData->hcounts)[i];maxcount=(* matData->hcounts)[i];
				        h_List->push_back(*is);				 
				        inGrouph->insert(i);//把第i行对应的行号存入已经归类的集合中
				       }
		        }              
		     for(j=i+1;j<size_m;j++)   //每个经历只与目标组的第一个经历进行卡方比较(一次)
		     if(inGrouph->find(j) != inGrouph->end()) 
            continue;
		     else//未分类,则与上面的经历的概率比较,两两求秩
           {
			      sum=0.0;
			      for(k=0;k<size_n;k++)
			          sum+=	prob[j][k];
			      if(sum==0.0) 
              {
               H_zero->insert(j);inGrouph->insert(j);continue;}//全零行不统计
			         vector<int>*comparecounts=new vector<int>;
		       	   for(k=0;k<2*size_n;k++)   //将两行经历中两两相同的检验放在一起(包含发生和不发生的次数)
				          { 
				           comparecounts->push_back(counts[i][k]);
				           comparecounts->push_back(counts[j][k]);
				          }	
			         int test=similarityTest(comparecounts);if(test==1){
							 for(k=0;k<size_n;k++)   //将两行经历中两两相同的检验放在一起(包含发生和不发生的次数) 
    					   	 RANK2TXT<<prob[j][k]<<"  ";
        					 RANK2TXT<<endl;
              }
			      delete comparecounts; 
			      if(test==1)//两个h对应的概率向量相似，则属于同一个Group
              {
					     for(k=0,is=histories->begin();is!=histories->end();k++,is++)
  		 				    {
							     if(k==j) 
                     { 				  				
								      if((* matData->hcounts)[j]>maxcount)//若归入该分组的经历所对应的的发生次数大于已经分类的经历的发生次数，则将其作为目标经历
									      {
                         maxcount=(* matData->hcounts)[j];
									       maxi=j;
									      }
				 				      h_List->push_back(*is);				 				
				              inGrouph->insert(j);//把第j行对应的行号存入已经归类的集合中
								     }
		   				    }   
				      }                       
           }           
         symList_DblMap *row_DblMap = new symList_DblMap();    //将新Group对应的概率提取出来
         for(k=0,iCurr1 = augTest->begin(); iCurr1 != augTest->end(); iCurr1++,k++)
            {
             row_DblMap->insert(pair<symList*, double>(*iCurr1,prob[maxi][k]));   //第i行经历下对应的检验及各检验对应的概率
            }
	       vector<symList*>::iterator it;
	       for(it= h_List->begin();it!=h_List->end();it++)
		        {
			       H_P->insert(pair<symList*, symList_DblMap*>(*it,row_DblMap));
		        }
             H_Group->push_back(H_P);
        }
     }
  if((H_zero->size())!=0)
    {
     symList_DblMap *row_0Map = new symList_DblMap();    //将全为0的概率提取出来
     for(iCurr1 = augTest->begin(); iCurr1 != augTest->end(); iCurr1++)
        {
         row_0Map->insert(pair<symList*, double>(*iCurr1,0.0));   //检验-0
        }
     symList2Prob*H_P0=new  symList2Prob(); symIter is;
     for(k=0,is=histories->begin();is!=histories->end();k++,is++)//将对应概率为0的行组成一组存起来
        {
		     if(H_zero->find(k) != H_zero->end())//第K个经历对应为0行
           {
		        H_P0->insert(pair<symList*, symList_DblMap*>(*is,row_0Map));
		       }
	      }
     H_Group->push_back(H_P0);//将全为0组存起来
    }
            
  delete inGrouph;
  delete augTest   ;
  delete histories ;
  # endif
  ofstream RANKK2("RANKK21.txt")  ;   
	int g=1;
	for(vs=H_Group->begin();vs!=H_Group->end();vs++)
	{
		for(sp=(*vs)->begin();sp!=(*vs)->end();sp++)
		{
			RANKK2<< *(sp->first) <<endl;
			for(sd=sp->second->begin();sd!=sp->second->end();sd++)
			{
                RANKK2<<sd->second<<" ";
            }
			RANKK2<<endl;
		}
       	RANKK2<<"****************groupend "<<g<<"************"<<endl;
		g++;
	}
  	return H_Group;
}

/******************************
函数功能：对得到的p（t|h）概率矩阵利用线性无关的方法进行分组
函数输入：
结构体：经历对应的感兴趣检验发生的次数矩阵；经历对应的感兴趣检验发生的概率矩阵，经历对应的各个动作发生的次数矩阵。
初始状态的观测值
函数输出：初始状态的观测值及分组结果
/******************************/
symList2Group* psr_local::find_H_Groups(tEstimateData * matData,symList*startPosLm)
 {
  int  i,j,k;
  ofstream RANKTXT("RANK.txt");
  symList2Group*S_H_P=new symList2Group();
  symListDblMat *ctProbP = new symListDblMat();
  symListMatrix* ctActP  = new symListMatrix();
  symListMatrix*  ctMatP  = new symListMatrix();
  symList2IntMap*ctHists	= new symList2IntMap();  
  symSet *augTest   = new symSet();
  symSet *histories = new symSet();
  symSet *augAct = new symSet();
  for(iH=matData->ctMat->begin(); iH!=matData->ctMat->end(); iH++)//<经历，<检验，检验发生的次数> >
  	 {
    	histories->insert(new symList(iH->first));
 	   }   
  iH = matData->ctMat->begin(); 
  for(iT = iH->second->begin(); iT != iH->second->end(); iT++) 
     {
   	  augTest->insert(new symList(iT->first));
	   }
  for(tiT=matData->ctAct->begin()->second->begin(); tiT!=matData->ctAct->begin()->second->end();tiT++)//<经历，<动作，动作发生的次数> >
     {
  	  augAct->insert(new symList(tiT->first));
     } 
  int size_m=matData->ctProb->size();//<经历，<检验，检验发生的概率> > 
  int size_n=matData->ctProb->begin()->second->size();
  int counts[size_m][2*size_n];//得到经历对应的每个感兴趣的检验分别对应的检验发生和步发生次数
  double denom;double countp[size_m][size_n];
  for(i=0,iCurr = histories->begin(); iCurr != histories->end(); iCurr++,i++)//以下是计算在经历h下，对应的检验发生概率P(t|h)
     {
      for(j=0,iCurr1 = augTest->begin(); iCurr1 != augTest->end(); iCurr1++,j++) 
         {	
          symList *acttest = (*iCurr1)->filter(SYM_ACTION);//在augTest中取出所有检验对应的动作
          denom = *(matData->ctAct->find(*iCurr)->second->find(acttest)->second);//denom为matData->ctAct矩阵中对应的动作发生次数   
          countp[i][j]=denom;
          delete acttest;//删除acttest所占的内存
         }
     }
  double prob[size_m][size_n];//得到经历对应的每个感兴趣的检验发生的概率
  for(i=0,piH=matData->ctProb->begin(); piH != matData->ctProb->end();i++,piH++)
     {
      for(j=0,piT = piH->second->begin(); piT != piH->second->end();j++, piT++)
         {
     		  prob[i][j]=*(piT->second);
   	     }
     }
  vector<symList2Prob*>* H_Group=new vector<symList2Prob*>;
  IntSet* H_zero=new IntSet();//存放对应概率全为0的行
  IntSet* inGrouph=new IntSet();// 存放已经归类的经历的行号 
  #if 01  
  for(i=0;i<size_m;i++)
     {        
      if(inGrouph->find(i) != inGrouph->end()) 
         continue;//当前经历已经归类
      else//未分类则建立一个新Group
        {  
		     int maxi=0;int maxcount=0;float sum=0.0;
		     vector<symList*>* h_List=new vector<symList*>;
         symList2Prob*H_P=new  symList2Prob();   //map<symList*, <symList*, double,> > 
		     symIter is;
		     for(k=0;k<size_n;k++)
			       sum+=prob[i][k];
		     if(sum==0.0) 
           {
            H_zero->insert(i);inGrouph->insert(i);continue;
           }//全零行不统计
         RANKTXT<<"**********************"<<endl; 
         for(k=0;k<size_n;k++)   //将两行经历中两两相同的检验放在一起(包含发生和不发生的次数) 
    				 RANKTXT<<prob[i][k]<<"  ";
        		 RANKTXT<<endl;
         for(k=0,is=histories->begin();is!=histories->end();is++,k++) //判断从当前行开始的经历是否被分组，若未被分组，则新建一个组将其储存在内
  		      {
			       if(k==i) 
               { 
				        maxi=i;
				        int count=(* matData->hcounts)[i];maxcount=(* matData->hcounts)[i];
				        h_List->push_back(*is);				 
				        inGrouph->insert(i);//把第i行对应的行号存入已经归类的集合中
				       }
		        }               
		     for(j=i+1;j<size_m;j++)   //每个经历只与目标组的第一个经历进行卡方比较(一次)
		     if(inGrouph->find(j) != inGrouph->end()) continue;
		     else//未分类,则与上面的经历的概率比较,两两求秩
           {
			      sum=0.0;
			      for(k=0;k<size_n;k++)
			          sum+=	prob[j][k];
			      if(sum==0.0) 
              {
               H_zero->insert(j);inGrouph->insert(j);continue;
              }//全零行不统计
            int test;
            Array2D<double>*probsP  = new Array2D<double>(2, size_n);
            Array2D<double>*countsP  = new Array2D<double>(2, size_n);
            for(k=0;k<size_n;k++)   //将两行经历中两两相同的检验放在一起(包含发生和不发生的次数)
			{ 
                (*probsP)[0][k]= prob[i][k];
                (*probsP)[1][k]= prob[j][k];
                (*countsP)[0][k]= countp[i][k];
                (*countsP)[1][k]= countp[j][k];        
			}    
            for(k=0;k<size_n;k++)        
            test=findtherank(probsP,countsP);
            delete probsP;
            delete countsP;
			      if(test==1)
              {			 
							 for(k=0;k<size_n;k++)   //将两行经历中两两相同的检验放在一起(包含发生和不发生的次数) 								
    					   	 RANKTXT<<prob[j][k]<<"  ";
        					 RANKTXT<<endl;
              }			
			      if(test==1)//两个h对应的概率向量相似，则属于同一个Group
              {
					     for(k=0,is=histories->begin();is!=histories->end();k++,is++)
  		 				    {
							     if(k==j)
                     { 				  				
								      if((* matData->hcounts)[j]>maxcount)//若归入该分组的经历所对应的的发生次数大于已经分类的经历的发生次数，则将其作为目标经历
									      {
                         maxcount=(* matData->hcounts)[j];
									       maxi=j;
									      }
				 				      h_List->push_back(*is);				 				
				              inGrouph->insert(j);//把第j行对应的行号存入已经归类的集合中
								     }
		   				    }   
				      }
                        
           }    
         symList_DblMap *row_DblMap = new symList_DblMap();    //将新Group对应的概率提取出来
         for(k=0,iCurr1 = augTest->begin(); iCurr1 != augTest->end(); iCurr1++,k++)
            {
             row_DblMap->insert(pair<symList*, double>(*iCurr1,prob[maxi][k]));   //第i行经历下对应的检验及各检验对应的概率
            }
	       vector<symList*>::iterator it;
	       for(it= h_List->begin();it!=h_List->end();it++)
		        {
			       H_P->insert(pair<symList*, symList_DblMap*>(*it,row_DblMap));
		        }
         H_Group->push_back(H_P);
        }
     }
  symList_DblMap *row_0Map = new symList_DblMap();    //将全为0的概率提取出来
  for(iCurr1 = augTest->begin(); iCurr1 != augTest->end(); iCurr1++)
     {
      row_0Map->insert(pair<symList*, double>(*iCurr1,0.0));   //检验-0
     }
  symList2Prob*H_P0=new  symList2Prob(); symIter is;
  for(k=0,is=histories->begin();is!=histories->end();k++,is++)//将对应概率为0的行组成一组存起来
	   {
		  if(H_zero->find(k) != H_zero->end())//第K个经历对应为0行
		    {
			   H_P0->insert(pair<symList*, symList_DblMap*>(*is,row_0Map));
		    }
   	 }
  if((H_zero->size())!=0)  H_Group->push_back(H_P0);//将全为0组存起来
  S_H_P->insert(pair<symList*,vector<symList2Prob*>*>(new symList(startPosLm),H_Group));              
  delete inGrouph;
  delete augTest   ;
  delete histories ;
  # endif 
  ofstream RANK2TXT("RANK2.txt");
  for(shp =S_H_P->begin();shp!=S_H_P->end();shp++)
		 {  
      int g=1;
			RANK2TXT<<"landmark="<<*(shp->first)<<endl;
			for(vs=shp->second->begin();vs!=shp->second->end();vs++)
			   {			   
			    for(sp=(*vs)->begin();sp!=(*vs)->end();sp++)
				     {
				      for(sd=sp->second->begin();sd!=sp->second->end();sd++)
					        RANK2TXT<<sd->second<<" ";
					    RANK2TXT<<endl;
				     }		       
          RANK2TXT<<"****************groupend "<<g<<"************"<<endl;
			    g++;
			   }
		 }
  return S_H_P;
}


vector<symList2Prob*> *psr_local::find_H_Groups1(tEstimateData * matData)
 {
  	int  i,j,k;
  	ofstream findG("find_H_G1.txt");//输出这个函数中的一些参数到此文件中，检验函数的正确性
  	ofstream RANKTXT("RANK1.txt");
  	
	//symList2Prob*S_H_P=new symList2Group();
	symListDblMat *ctProbP = new symListDblMat();
	symListMatrix* ctActP  = new symListMatrix();
	symListMatrix*  ctMatP  = new symListMatrix();
	symList2IntMap*ctHists	= new symList2IntMap();  
	symSet *augTest   = new symSet();
	symSet *histories = new symSet();
	symSet *augAct = new symSet();
	
	findG << "经历histories：" << endl;
  	for(iH=matData->ctMat->begin(); iH!=matData->ctMat->end(); iH++)//<经历，<检验，检验发生的次数> >
  	{
    	histories->insert(new symList(iH->first));
    	findG << *(iH->first) << endl;
 	}
 	
 	findG << "检验augTest：" << endl;
  	iH = matData->ctMat->begin(); 
  	for(iT = iH->second->begin(); iT != iH->second->end(); iT++) 
    {
   	  	augTest->insert(new symList(iT->first));
   	  	findG << *(iT->first) << endl;
	}
	
	findG << "动作augAct：" << endl;
  	for(tiT=matData->ctAct->begin()->second->begin(); tiT!=matData->ctAct->begin()->second->end();tiT++)//<经历，<动作，动作发生的次数> >
    {
  	  	augAct->insert(new symList(tiT->first));
  	  	findG << *(tiT->first) << endl;
    }
    
  	int size_m=matData->ctProb->size();//<经历，<检验，检验发生的概率> > 
  	int size_n=matData->ctProb->begin()->second->size();
  	int counts[size_m][2*size_n];//得到经历对应的每个感兴趣的检验分别对应的检验发生和步发生次数
  	double denom;
  	double countp[size_m][size_n];
  	for(i=0,iCurr = histories->begin(); iCurr != histories->end(); iCurr++,i++)//以下是计算在经历h下，对应的检验发生概率P(t|h)
    {
      	for(j=0,iCurr1 = augTest->begin(); iCurr1 != augTest->end(); iCurr1++,j++) 
         {	
          symList *acttest = (*iCurr1)->filter(SYM_ACTION);//在augTest中取出所有检验对应的动作
          denom = *(matData->ctAct->find(*iCurr)->second->find(acttest)->second);//denom为matData->ctAct矩阵中对应的动作发生次数   
          countp[i][j]=denom;
          delete acttest;//删除acttest所占的内存
         }
    }
  double prob[size_m][size_n];//得到经历对应的每个感兴趣的检验发生的概率
  for(i=0,piH=matData->ctProb->begin(); piH != matData->ctProb->end();i++,piH++)
     {
      for(j=0,piT = piH->second->begin(); piT != piH->second->end();j++, piT++)
         {
     		  prob[i][j]=*(piT->second);
   	     }
     }
  vector<symList2Prob*>* H_Group=new vector<symList2Prob*>;
  IntSet* H_zero=new IntSet();//存放对应概率全为0的行
  IntSet* inGrouph=new IntSet();// 存放已经归类的经历的行号 
  #if 01  
  for(i=0;i<size_m;i++)
     {        
      if(inGrouph->find(i) != inGrouph->end()) 
         continue;//当前经历已经归类
      else//未分类则建立一个新Group
        {  
		     int maxi=0;int maxcount=0;float sum=0.0;
		     vector<symList*>* h_List=new vector<symList*>;
         symList2Prob*H_P=new  symList2Prob();   //map<symList*, <symList*, double,> > 
		     symIter is;
		     for(k=0;k<size_n;k++)
			       sum+=prob[i][k];
		     if(sum==0.0) 
           {
            H_zero->insert(i);inGrouph->insert(i);continue;
           }//全零行不统计
         RANKTXT<<"**********************"<<endl; 
         for(k=0;k<size_n;k++)   //将两行经历中两两相同的检验放在一起(包含发生和不发生的次数) 
    				 RANKTXT<<prob[i][k]<<"  ";
        		 RANKTXT<<endl;
         for(k=0,is=histories->begin();is!=histories->end();is++,k++) //判断从当前行开始的经历是否被分组，若未被分组，则新建一个组将其储存在内
  		      {
			       if(k==i) 
               { 
				        maxi=i;
				        int count=(* matData->hcounts)[i];maxcount=(* matData->hcounts)[i];
				        h_List->push_back(*is);				 
				        inGrouph->insert(i);//把第i行对应的行号存入已经归类的集合中
				       }
		        }               
		     for(j=i+1;j<size_m;j++)   //每个经历只与目标组的第一个经历进行卡方比较(一次)
		     if(inGrouph->find(j) != inGrouph->end()) continue;
		     else//未分类,则与上面的经历的概率比较,两两求秩
           {
			      sum=0.0;
			      for(k=0;k<size_n;k++)
			          sum+=	prob[j][k];
			      if(sum==0.0) 
              {
               H_zero->insert(j);inGrouph->insert(j);continue;
              }//全零行不统计
            int test;
            Array2D<double>*probsP  = new Array2D<double>(2, size_n);
            Array2D<double>*countsP  = new Array2D<double>(2, size_n);
            for(k=0;k<size_n;k++)   //将两行经历中两两相同的检验放在一起(包含发生和不发生的次数)
			         { 
                (*probsP)[0][k]= prob[i][k];
                (*probsP)[1][k]= prob[j][k];
                (*countsP)[0][k]= countp[i][k];
                (*countsP)[1][k]= countp[j][k];        
				       }    
            for(k=0;k<size_n;k++)        
            test=findtherank(probsP,countsP);
            delete probsP;
            delete countsP;
			      if(test==1)
              {			 
							 for(k=0;k<size_n;k++)   //将两行经历中两两相同的检验放在一起(包含发生和不发生的次数) 								
    					   	 RANKTXT<<prob[j][k]<<"  ";
        					 RANKTXT<<endl;
              }			
			      if(test==1)//两个h对应的概率向量相似，则属于同一个Group
              {
					     for(k=0,is=histories->begin();is!=histories->end();k++,is++)
  		 				    {
							     if(k==j)
                     { 				  				
								      if((* matData->hcounts)[j]>maxcount)//若归入该分组的经历所对应的的发生次数大于已经分类的经历的发生次数，则将其作为目标经历
									      {
                         maxcount=(* matData->hcounts)[j];
									       maxi=j;
									      }
				 				      h_List->push_back(*is);				 				
				              inGrouph->insert(j);//把第j行对应的行号存入已经归类的集合中
								     }
		   				    }   
				      }
                        
           }    
         symList_DblMap *row_DblMap = new symList_DblMap();    //将新Group对应的概率提取出来
         for(k=0,iCurr1 = augTest->begin(); iCurr1 != augTest->end(); iCurr1++,k++)
            {
             row_DblMap->insert(pair<symList*, double>(*iCurr1,prob[maxi][k]));   //第i行经历下对应的检验及各检验对应的概率
            }
	       vector<symList*>::iterator it;
	       for(it= h_List->begin();it!=h_List->end();it++)
		        {
			       H_P->insert(pair<symList*, symList_DblMap*>(*it,row_DblMap));
		        }
         H_Group->push_back(H_P);
        }
     }
  symList_DblMap *row_0Map = new symList_DblMap();    //将全为0的概率提取出来
  for(iCurr1 = augTest->begin(); iCurr1 != augTest->end(); iCurr1++)
     {
      row_0Map->insert(pair<symList*, double>(*iCurr1,0.0));   //检验-0
     }
  symList2Prob*H_P0=new  symList2Prob(); symIter is;
  for(k=0,is=histories->begin();is!=histories->end();k++,is++)//将对应概率为0的行组成一组存起来
	   {
		  if(H_zero->find(k) != H_zero->end())//第K个经历对应为0行
		    {
			   H_P0->insert(pair<symList*, symList_DblMap*>(*is,row_0Map));
		    }
   	 }
  if((H_zero->size())!=0)  H_Group->push_back(H_P0);//将全为0组存起来
  //S_H_P->insert(pair<symList*,vector<symList2Prob*>*>(new symList(startPosLm),H_Group));              
  delete inGrouph;
  delete augTest   ;
  delete histories ;
  # endif 
  ofstream RANK2TXT("RANK21.txt");

      	int g=1;

		for(vs=H_Group->begin();vs!=H_Group->end();vs++)
		{	
			for(sp=(*vs)->begin();sp!=(*vs)->end();sp++)
			{
				RANK2TXT<< *(sp->first) <<endl;
				for(sd=sp->second->begin();sd!=sp->second->end();sd++)
					RANK2TXT<<sd->second<<" ";
				RANK2TXT<<endl;
			}		       
          	RANK2TXT<<"****************groupend "<<g<<"************"<<endl;
			g++;
		}

  	return H_Group;
}



/******************************
函数功能：利用似然比检验判断矩阵的两行是否相似。（详细见附1）
函数输入：2行矩阵的数值
函数输出：1：相似；0：不相似。
/******************************/
int psr_local:: similarityTest(vector<int>*compareCounts)
 {
  int test=1;
  vector<int>::iterator in=compareCounts->begin();
  int size_n=compareCounts->size();//矩阵的列数
  int j=0;
  for(j=0;j<size_n;j=j+4)
	   {	
	    vector<int>*TCompare=new vector<int>;
	    TCompare->push_back(*in); in++;
	    TCompare->push_back(*in); in++;
      TCompare->push_back(*in); in++;
      TCompare->push_back(*in); in++;
	    test=likelihood_ratioTest(TCompare);//利用似然比检验判断在对两个不同经历来说某个检验是否相似。相似test=1，不相似test=0.
	    delete TCompare ;	
	    if(test==0) break;//一旦某个检验不相似，则这两行步不相似
	   }
  return test;
 }


/******************************
函数功能：利用对数似然比检验检验一个四格表的相似度，判断第一行2个数与第二行两个数是否相似
函数输入：四个数值形成的四格表
函数输出：1代表相似，0代表不相似。
/******************************/
int psr_local::likelihood_ratioTest(vector<int>*TCompare)
 {      
  vector<int>::iterator in=TCompare->begin();
	int testCompare[2][2]={0};
	testCompare[0][0]=*in;in++;
	testCompare[1][0]=*in;in++;
	testCompare[0][1]=*in;in++;
	testCompare[1][1]=*in;
	int test=1;//假设两行相似
	int a1=testCompare[0][0]+testCompare[0][1];//a1为第一行的和
	int a2=testCompare[1][0]+testCompare[1][1];//a2为第二行的和
	int b1=testCompare[0][0]+testCompare[1][0];//b1为第一列的和
	int b2=testCompare[0][1]+testCompare[1][1];//b2为第二列的和
	int sum=a1+a2;//sum为四格表的总和
	double X2=f(testCompare[0][0])+f(testCompare[0][1])+f(testCompare[1][0])+f(testCompare[1][1])+f(sum)-f(a1)-f(a2)-f(b1)-f(b2);
	if(X2>19.51) test=0;//19.51为a=0.00001，自由度取1的时候的卡方值
	return test;
 }


/******************************
函数功能：实现函数f(x)=2xlnx
函数输入：x
函数输出：f(x)的值
/******************************/
double psr_local::f(int x)
 {       
  double fx=0.0;
	if(x==0||x==1) fx=0.0;
	else  fx=2*x*log((long double)x);
	return fx;
 }


/******************************
函数功能：获取各个landmark对应的经历
函数输入：vector<Symbol*> 型的训练数据conData
        存放landmark的集合landmarkSet   
        起始landmark在训练数据中所在的位置startPos
函数输出：各个landmark以及各个landmark对应的经历
/******************************/
symList2ListMap* psr_local::getLandmarkhist(vector<Symbol*>* conData, symSet* landmarkSet,int startPos)
 {    
  symList2ListMap* landmarkMap = new symList2ListMap();//用来存放 landmark 对应的经历
  symList2ListMap::iterator sym2LM;
  symSet* tempLandmark = new symSet();                 //用于暂时存放 landmark 里面的数据
  for(iCurr=landmarkSet->begin(); iCurr!=landmarkSet->end(); iCurr++)
     {
      tempLandmark->insert(new symList(*iCurr));                     //insert 插入 *iCurr 的一个复本
     }
  int len = (*landmarkSet->begin())->size();          //landmark 里面的记忆长度都是相同的
  int dataPos, hisLen=0, initPos;                 //其中 initPos 表示的是最近一个初始 landmark 的位置
  symList* currMemory, *currHistory;
  symList* startHistory = new symList();
  symList *startPosLm1= getMemory(conData, startPos, len);//输出第一个landmark
  landmarkMap->insert(pair<symList*, symList*>(startPosLm1, startHistory));//首先将第一个landmark和空经历插入到该映射中
  tempLandmark->erase(startPosLm1);//在landmark集合中将第一个landmark删除
  initPos = startPos; //第一个landmark位置
  dataPos = startPos+2;//landmark位置对应的下一个观测值
  while(((dataPos<conData->size())) && (!(tempLandmark->empty())) )//若没有达到数据末端并且landmark集合中还有值
       {                                                                                                    
        currMemory = getMemory(conData, dataPos, len);//获得起始landmark的下一个观测值
        if(*currMemory == *startPosLm1)         //若该观测值与第一个landmark相同
           initPos = dataPos;                    //将该观测值的位置作为起始landmark的位置
        else 
          if((currMemory!=NULL)&&(tempLandmark->find(currMemory)!=tempLandmark->end()) )//若未达到数据末端并且发现新的landmark
            {
             hisLen = (dataPos-initPos)/2;   //长度为新的landmark与其实landmark之间的距离
             currHistory = getHistory(conData, dataPos, hisLen);//获取该距离的经历
             landmarkMap->insert(pair<symList*,symList*>(new symList(currMemory),currHistory));//将该经历存入新的landmark一一对应储存起来      
             tempLandmark->erase(currMemory);              
            }
        dataPos += 2;       //开始寻找下一个观测值
        delete currMemory;
       }
  delete tempLandmark;
  for(sym2LM=landmarkMap->begin();sym2LM!=landmarkMap->end();sym2LM++)
     {
      cout<<"landmark is:"<<*(sym2LM->first)<<endl;
      cout<<"history is:"<<*(sym2LM->second)<<endl;
     }
  return landmarkMap;
 }


/*************************获得整体状态空间的经历( 另一方案)***********************
函数功能：针对一整串数据，对其进行抽象，其中获得的动作为逐步获取，根据每个经历对应的landmark，获得观测值时，在数据之前添加起始经历，再对其进行抽象

函数输入：1.一长串数据(未划分vector<Symbol*>* conData)
  
        2.landmark的集合landmarkSet
  
        3.每个landmark对应的起始经历landmarkhis
      
        4.每个经历和新的观测值的一一对应关系S_h_newB
      
        5.每个A-O和新定义的动作的一一对应关系newAct

        6.这一长串数据的第一个landmark---startPosLm

        7.第一个landmark所在的位置startPos

函数输出： 一长串完整的抽象后的经历(symList型)
*****************************************************************************/
symList*psr_local::getAbstract(vector<Symbol*>* conData,symSet* landmarkSet,symList2ListMap*landmarkhis,symList_sym2symMap *S_h_newB,symList2SymbolMap *newAct,symList *startPosLm,int startPos)//获得整体状态空间的经历
 {   
	ofstream Abstractdata("abstractdata.txt");
  symList*abstractdata=new symList;//存放新的经历的集合
  symList2Vec2Map::iterator s1;
  symList2ListMap::iterator s2;
  vec2::iterator vs;
  symList*currlandmark,*currMemory,*currHistory;
  currlandmark=startPosLm;//定义当前的landmark为起始landmark
  int len = (*landmarkSet->begin())->size(); //定义len为一个landmark的长度 里面的记忆长度都是相同的 
  int dataPos=startPos+2;  //dataPos为起始landmark所在的位置的下一个
  while(dataPos<conData->size())//若没有达到数据末端
       {   
        currMemory = getMemory(conData, dataPos, len);//获得从dataPos位置起的下一个观测值   
        if((currMemory!=NULL)&&(landmarkSet->find(currMemory)!=landmarkSet->end() ) )//若未达到数据末端并且发现landmark
          {     
           Abstractdata<<"******************输出**************"<<endl; 
           int hisLen = (dataPos-startPos)/2;   //长度为新的landmark与起始landmark之间的距离
           currHistory = getHistory(conData, dataPos, hisLen);//获取该距离的经历
           Abstractdata<<"currHistory="<<*currHistory<<endl;
		       symList*his2;
	         for(s2=landmarkhis->begin();s2!=landmarkhis->end();s2++)
           if(*currMemory==*(s2->first) )//在landmarkhis中查找与下一个landmark相同的一个landmark
             {   
              his2=new symList(s2->second); //定义starthis为当前landmark对应的起始经历
		         }
           for(s2=landmarkhis->begin();s2!=landmarkhis->end();s2++)
              {  
               if(*currlandmark==*(s2->first) )//在landmarkhis中查找与当前landmark相同的一个landmark
                 {   
                  symList*his=new symList(s2->second); //定义starthis为当前landmark对应的起始经历
		              Abstractdata<<"his="<<*his<<endl;
                  int sizen=currHistory->size();//sizen获取的经历的长度
                  symList* newdata=new symList; //用来存放新抽象的ao
                  for(int pos=1;pos<sizen;pos+=2)//对经历currHistory进行抽象
                     { 
                      symList*ao=new symList;        
                      ao=getHistoryL(currHistory, pos, 1); //从dataPos位置开始获取长度为1的检验
                      if(ao==NULL) break;
                      Abstractdata<<"ao="<<*ao<<endl;
                      symList*newA=getA(ao,newAct);//对动作进行抽象
                      Abstractdata<<"newA="<<*newA<<endl;
                      his->append(ao);
		                  Abstractdata<<"his++="<<*his<<endl;
		                  symList* newO;
		                  if(pos!=sizen-1)
                         newO=getO(his,S_h_newB,startPosLm);//得到抽象后的新观测 			
		                  else   
                         newO=getO(his2,S_h_newB,startPosLm);//得到抽象后的新观测                
                      symList* test_newO1=new symList(newO);
		                  symList* test_newO2=new symList(newO);
                      test_newO2->append(test_newO1);
                      if((test_newO2->size())==1)
				                {
					               Abstractdata<<"xxxxx"<<endl;
					               delete test_newO2 ;
					               delete newA;
					               delete newO;
				      	         break;
				                }
		                  else
				                { 
                         Abstractdata<<"newO="<<*newO<<endl;
					               newdata->append(new symList(newA));
					               newdata->append(new symList(newO));
					               delete test_newO2 ;
					               delete newA;
					               delete newO;
				                }
                     }
                  abstractdata->append(newdata);      //将完成后的经历插入abstractdata中     
                  currlandmark=currMemory;               //将currMemory定义为当前landmark，用于获取后面对应的该landmark经历
                  break;                                   //跳出循环
                 }    
              } 
           startPos=dataPos;   //重新初始化startPos
          } 
        dataPos += 2;       //开始寻找下一个观测值  
       }           
  return abstractdata;
 }

symList*psr_local::getAbstract1(symList* conData,symList_symListMap *S_h_newB,symList2SymbolMap *newAct)//获得整体状态空间的经历
{   
	ofstream Abstractdata("abstractdata1.txt");
	ofstream absd("returnAbsd1.txt");//返回的经过转换的训练数据
  	symList*abstractdata=new symList;//存放新的经历的集合
   	int Pos=1;  //dataPos为起始landmark所在的位置的下一个
  	symList*his=new symList();
  	
  	cout << "conData->size() = " << conData->size() << endl;
  	while( Pos<conData->size())//若没有达到数据末端
    {     
		symList*ao=new symList;  
		symList* newdata=new symList; //用来存放新抽象的ao      
        ao=getHistoryL(conData, Pos, 1); //从dataPos位置开始获取长度为1的检验
        if(ao==NULL) break;
        Abstractdata<<"ao="<<*ao<<endl;
        symList*newA=getA(ao,newAct);//对动作进行抽象
        	Abstractdata<<"newA="<<*newA<<endl;
        his->append(ao);
			Abstractdata<<"his++="<<*his<<endl;
		symList* newO;
		newO=getO1(his,S_h_newB);//得到抽象后的新观测 			
		              
		Abstractdata<< "newO=" << *newO << endl;
		
		newdata->append(new symList(newA));
		newdata->append(new symList(newO));

		delete newA;
		delete newO;
				
        abstractdata->append(newdata);      //将完成后的经历插入abstractdata中     
                  		
		Pos += 2;       //开始寻找下一个观测值  
	}
       	absd << *(abstractdata) << endl;       
  	return abstractdata;
}

/************************************
函数功能：找到当前ao对应的新动作
函数输入：待抽象原始数据ao；
	       原始ao与new动作对应关系newAct；
函数输出：抽象后新动作
*************************************/
symList*psr_local::getA(symList* ao,symList2SymbolMap *newAct)
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
symList*psr_local::getO(symList*history,symList_sym2symMap *S_h_newB,symList*landmark)
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

symList*psr_local::getO1(symList*history,symList_symListMap *S_h_newB)
{
	symList*newO=new symList();
	
	map<symList*, symList*>::iterator hb;	
	for(hb=S_h_newB->begin();hb!=S_h_newB->end();hb++)
	{
		if(*(hb->first)==*history)
		{ 
			newO=new symList(hb->second);
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
PomdpData *psr_local::getRandomPomdp(int Asize,int Ssize,int Osize)
{
 	ofstream RPOMDP("RPOMDP.txt");
	PomdpData *toret = new PomdpData;
  	toret->TMatrix= new Array3D<double>(Asize, Ssize,Ssize);	
  	*(toret->TMatrix) = 0.0;
  	toret->OMatrix= new Array3D<double>(Asize, Ssize,Osize);	
  	*(toret->OMatrix) = 0.0;
  	for(int o=0;o<Asize;o++)/**********给T矩阵赋随机数值*******************/
	{
//      cout<<"TMatrix"<<o<<":"<<endl;
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
              	//cout<<(*toret->TMatrix)[o][i][j]<<"  ";
              	RPOMDP<<(*toret->TMatrix)[o][i][j]<<"  ";
         	}
          	//cout<<endl;
          	RPOMDP<<endl;
		}
      	//cout<<endl;
      	RPOMDP<<endl;
	}    
  	for(int o=0;o<Asize;o++)/*************给O矩阵赋随机数值************/
 	{
		//cout<<"OMatrix"<<o<<":"<<endl;
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
              	//cout<<(*toret->OMatrix)[o][i][j]<<"  ";
		  		RPOMDP<<(*toret->OMatrix)[o][i][j]<<"  ";
          	}
          	//cout<<endl;
          	RPOMDP<<endl;
     	}
      	//cout<<endl;
      	RPOMDP<<endl;
	}
  	cout<<" size_NewAct="<<Asize<<" size_newS="<<Ssize<<" size_NewOb="<<Osize<<endl;
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
Array2D<double>*psr_local::getAlpha(PomdpData *RandomPOMDP,symList*startPosLm,symList *NewAbstractData,int size_NewAct, int size_NewOb,Array1D<int>*action,Array1D<int>*observation,vector<float> *belief)
 {          
  int Size_n; //定义Size_n为每个landmark对应的抽象数据的总长度
  Size_n=NewAbstractData->size()/2;
  int N=size_NewOb;//N为新定义的观测值的个数
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
  alpha=new Array2D<double>(Size_n+1,size_NewOb);//定义alpha为二维数组
  *alpha = 0.0;  //数组初始化为0
  int t=0;//初始时刻
	int a0=(*action)[0];//获得动作对应的下标值
  for(int i=0;i<size_NewOb;i++)//计算alpha【t】【j】
     {
      (*alpha)[0][i]=pi[i]*(*RandomPOMDP->OMatrix)[a0][i][0];
     }
  double sum0=0.0;
  for(int i=0;i<size_NewOb;i++)
     {
      sum0+=(*alpha)[0][i];
     }
  for(int i = 0; i <size_NewOb; i++)//对alpha[o][i]进行归一化 
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
      for(int i=0;i<size_NewOb;i++)   
         {  
          for(int j=0;j<size_NewOb;j++)
             { 
              (*alpha)[t][i]+= (*alpha)[t-1][j] *( (*RandomPOMDP->TMatrix)[a1][j][i] );
             }
          (*alpha)[t][i] *=(*RandomPOMDP->OMatrix)[a1][i][o1]; 
          sum+=(*alpha)[t][i];//求出alpha对应的各项总和
         }
      for(int i=0;i<size_NewOb;i++)   
         {  
          (*alpha)[t][i]=(*alpha)[t][i]/sum ;  
         }
     }
  ofstream alpah("alpha.txt");//输出alpha【t】【i】的结果
  for(int t=0;t<Size_n+1;t++)
     {
      for(int i=0;i<size_NewOb;i++)
         { 
          alpah<<(*alpha)[t][i]<<" ";
         }
      alpah<<endl;
     }
  return  alpha;
 }

Array2D<double>*psr_local::getAlpha1(PomdpData *RandomPOMDP, symList *NewAbstractData,int size_NewAct, int size_NewOb, Array1D<int>*action, Array1D<int>*observation, vector<float> *belief)
{          
  	int Size_n; //定义Size_n为每个landmark对应的抽象数据的总长度
  	Size_n=NewAbstractData->size()/2;
  	int N=size_NewOb;//N为新定义的观测值的个数
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
  	alpha=new Array2D<double>(Size_n+1,size_NewOb);//定义alpha为二维数组
  	*alpha = 0.0;  //数组初始化为0
  	int t=0;//初始时刻
	int a0=(*action)[0];//获得动作对应的下标值
  	for(int i=0;i<size_NewOb;i++)//计算alpha【t】【j】
	{
		(*alpha)[0][i]=pi[i]*(*RandomPOMDP->OMatrix)[a0][i][0];
	}
  	double sum0=0.0;
  	for(int i=0;i<size_NewOb;i++)
	{
      	sum0+=(*alpha)[0][i];
    }
  	for(int i = 0; i <size_NewOb; i++)//对alpha[o][i]进行归一化 
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
     	for(int i=0;i<size_NewOb;i++)   
		{  
			for(int j=0;j<size_NewOb;j++)
            { 
            	(*alpha)[t][i]+= (*alpha)[t-1][j] *( (*RandomPOMDP->TMatrix)[a1][j][i] );
            }
	        (*alpha)[t][i] *=(*RandomPOMDP->OMatrix)[a1][i][o1]; 
    	    sum+=(*alpha)[t][i];//求出alpha对应的各项总和
        }
	   	for(int i=0;i<size_NewOb;i++)   
		{  
          	(*alpha)[t][i]=(*alpha)[t][i]/sum ;  
		}
	}
  	ofstream alpah("alpha1.txt");//输出alpha【t】【i】的结果
  	for(int t=0;t<Size_n+1;t++)
	{
		for(int i=0;i<size_NewOb;i++)
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
Array1D<int>*psr_local::getAction(symList*NewAbstractData)
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
Array1D<int>*psr_local::getObservation(symList*NewAbstractData)
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
Array2D<double>*psr_local::getBeta(PomdpData *RandomPOMDP,symList*startPosLm,symList*NewAbstractData,int size_NewAct,int size_NewOb,Array1D<double>*scale,Array1D<int>*action,Array1D<int>*observation)
 {      
  int Size_n; //用来表示数据的总长度
  Size_n=NewAbstractData->size()/2; //Size_n为对应landmark下抽象数据的总长度   
  Array2D<double>*beta;
  int len1=startPosLm->size();//len1为一个landmark的大小                              
  beta=new Array2D<double>(Size_n+1,size_NewOb);//用来定义beta的二维数组
  *(beta) = 0.0;  //数组初始化为0
  for(int i=0;i<size_NewOb;i++)   //开始计算beta【t】【i】
     {
      (*beta)[Size_n][i]=1.0/(*scale)[Size_n];
     } //最后一串数据的末尾初始化beta【t】【i】为1.0/scare【t】
  for(int t=Size_n-1;t>=0;t--)//从抽象数据的末尾开始
     {   
      int a0=(*action)[t];//获得动作对应的下标值
      int o0=(*observation)[t+1];//获得观测值对应的下标值
      double sum=0.0;
      for(int i=0;i<size_NewOb;i++)  
         {   
          for(int j=0;j<size_NewOb;j++) //计算beta[t][i]的值        
        	   {
              (*beta)[t][i]+=(*RandomPOMDP->TMatrix)[a0][i][j]*(*RandomPOMDP->OMatrix)[a0][j][o0]*(*beta)[t+1][j];
             }               
          (*beta)[t][i]=(*beta)[t][i]/(*scale)[t];
          sum+=(*beta)[t][i];
         } 
      for(int i=0;i<size_NewOb;i++)  
         { 
          (*beta)[t][i]= (*beta)[t][i]/sum ;                  
         }           
     }
  ofstream betadata("beta.txt");//输出beta【T】【i】的值到txt文件中
  for(int t=0;t<Size_n+1;t++)
     { 
      for(int i=0;i<size_NewOb;i++)
         { 
          betadata<<(*beta)[t][i]<<" ";
         }
      betadata<<endl;
     }      
  return beta;
 }

Array2D<double>*psr_local::getBeta1(PomdpData *RandomPOMDP, symList*NewAbstractData, int size_NewAct,int size_NewOb, Array1D<double>* scale, Array1D<int>*action, Array1D<int>*observation)
{      
	int Size_n; //用来表示数据的总长度
  	Size_n=NewAbstractData->size()/2; //Size_n为对应landmark下抽象数据的总长度   
  	Array2D<double>*beta;                            
  	beta=new Array2D<double>(Size_n+1,size_NewOb);//用来定义beta的二维数组
 	*(beta) = 0.0;  //数组初始化为0
  	for(int i=0;i<size_NewOb;i++)   //开始计算beta【t】【i】
	{
		(*beta)[Size_n][i]=1.0/(*scale)[Size_n];
	} //最后一串数据的末尾初始化beta【t】【i】为1.0/scare【t】
	for(int t=Size_n-1;t>=0;t--)//从抽象数据的末尾开始
	{   
		int a0=(*action)[t];//获得动作对应的下标值
		int o0=(*observation)[t+1];//获得观测值对应的下标值
		double sum=0.0;
		for(int i=0;i<size_NewOb;i++)  
		{   
			for(int j=0;j<size_NewOb;j++) //计算beta[t][i]的值        
			{
				(*beta)[t][i]+=(*RandomPOMDP->TMatrix)[a0][i][j]*(*RandomPOMDP->OMatrix)[a0][j][o0]*(*beta)[t+1][j];
			}               
          	(*beta)[t][i]=(*beta)[t][i]/(*scale)[t];
          	sum+=(*beta)[t][i];
		} 
      	for(int i=0;i<size_NewOb;i++)  
		{ 
          	(*beta)[t][i]= (*beta)[t][i]/sum ;                  
        }           
	}
  	ofstream betadata("beta1.txt");//输出beta【T】【i】的值到txt文件中
  	for(int t=0;t<Size_n+1;t++)
	{ 
		for(int i=0;i<size_NewOb;i++)
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
Array1D<double>*psr_local::getScale(PomdpData *RandomPOMDP,symList*NewAbstractData, int size_NewOb, symList*startPosLm,Array1D<int>*action,Array1D<int>*observation)
 {        
  int j;
  int Size_n=NewAbstractData->size()/2;  //定义Size_n的大小为数据长度的一半                   
  Array1D<double>*scale= new Array1D<double>(Size_n+1); //定义新的数组
  *scale=0.0; //数组初始化为0                 
  int len1=startPosLm->size();//len1为一个landmark的大小   
  int t0=0;  //定义t0为初始时刻
	int pos=0;
	int a0=(*action)[0];//获得动作对应的下标值
	for(int j=0;j<size_NewOb;j++)
     {
      (*scale)[0]+=(*RandomPOMDP->OMatrix)[a0][j][0];
     }  		   
  for(t0=1;t0<=Size_n;t0++)
     {    
      int a2=(*action)[t0-1];//获得动作对应的下标值
      int o2=(*observation)[t0];//获得观测值对应的下标值
      for(int j=0;j<size_NewOb;j++)
    		 (*scale)[t0]+=(*RandomPOMDP->OMatrix)[a2][j][o2];
     }   
  ofstream scaledata("scale.txt");//输出scale的值到txt文件中
	for(j=0;j<Size_n;j++)
			scaledata<<(*scale)[j]<<" ";
      return scale;
 }

Array1D<double>*psr_local::getScale1(PomdpData *RandomPOMDP, symList* NewAbstractData, int size_NewOb, Array1D<int>* action, Array1D<int>* observation)
{        
  	int j;
  	int Size_n=NewAbstractData->size()/2;  //定义Size_n的大小为数据长度的一半                   
  	Array1D<double>* scale= new Array1D<double>(Size_n+1); //定义新的数组
  	*scale = 0.0; //数组初始化为0                 
  	int t0=0;  //定义t0为初始时刻
	int pos=0;
	int a0=(*action)[0];//获得动作对应的下标值
	for(int j=0;j<size_NewOb;j++)
	{
		(*scale)[0]+=(*RandomPOMDP->OMatrix)[a0][j][0];
	}  		   
  	for(t0=1;t0<=Size_n;t0++)
	{    
		int a2=(*action)[t0-1];//获得动作对应的下标值
		int o2=(*observation)[t0];//获得观测值对应的下标值
		for(int j=0;j<size_NewOb;j++)
			(*scale)[t0]+=(*RandomPOMDP->OMatrix)[a2][j][o2];
	}   
  	ofstream scaledata("scale1.txt");//输出scale的值到txt文件中
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
Array3D<double>*psr_local::getSigma(PomdpData *RandomPOMDP,symList*NewAbstractData,int size_NewAct,int size_NewOb,Array2D<double>*beta,Array2D<double>*alpha,Array1D<int>*action,Array1D<int>*observation)
 {	 
  ofstream Sigmatxt("Sigma.txt");
	int T=(NewAbstractData->size())/2;//计算一共有多少时间步
	Array3D<double>*sigma=new Array3D<double>(T,size_NewOb,size_NewOb);//用来定义sigma的三维数组
  *(sigma) = 0.0; 
	int i ,j,t;
	for( t = 0; t < T; t++)
		 { 
      Sigmatxt<<"t="<<t<<endl; 
      int a1=(*action)[t];//获得动作对应的下标值
      int o1=(*observation)[t+1];//获得观测值对应的下标值
      double frac = 0.0;
      for(i = 0; i < size_NewOb; i++)
			   {
          for(j = 0; j < size_NewOb; j++)
				     {
              frac += (*alpha)[t][i] *(*RandomPOMDP->TMatrix)[a1][i][j] * (*RandomPOMDP->OMatrix)[a1][j][o1]*(*beta)[t+1][j];
             }
         }
      for(i = 0; i < size_NewOb; i++)
			   {
          for(j = 0; j <size_NewOb; j++) 
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
Array2D<double>*psr_local::getGama(symList*NewAbstractData,int size_NewAct,int size_NewOb,Array2D<double>*beta,Array2D<double>*alpha,Array1D<double>*scale)
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
PomdpData *psr_local::upDataTMatrix(PomdpData *RandomPOMDP,symList*NewAbstractData,int size_NewAct,int size_NewOb,Array3D<double>*sigma,Array2D<double>*gama,Array1D<int>*action)
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
PomdpData *psr_local::upDataOMatrix(PomdpData *RandomPOMDP,symList*NewAbstractData,int size_NewAct,int size_newS,int size_NewOb,Array2D<double>*gama,Array1D<int>*observation)
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
PomdpData *psr_local::EM(PomdpData *RandomPOMDP,symList*startPosLm,symList *NewAbstractData,int size_NewAct, int size_newS,int size_NewOb,vector<float> *belief)
{  
	int iter =10; // 迭代次数
	Array1D<int>*action=getAction(NewAbstractData);
	Array1D<int>*observation=getObservation(NewAbstractData);
	while(iter >0)
	{  
		cout<<"iter=="<<iter<<endl;
		//向前概率alpha<时刻t,状态i>
		Array2D<double>*alpha=getAlpha(RandomPOMDP,startPosLm,NewAbstractData,size_NewAct,size_newS,action,observation,belief);
		//scale<>
		Array1D<double>*scale=getScale(RandomPOMDP,NewAbstractData, size_newS, startPosLm,action,observation); //获取scale[t]的值  
 	      Array2D<double>*beta=getBeta(RandomPOMDP,startPosLm,NewAbstractData,size_NewAct,size_newS,scale,action,observation);
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
  //outputPOMDP(RandomPOMDP, size_NewAct,size_newS,size_NewOb);//输出POMDP模型至txt文件上
  return RandomPOMDP;
 }

PomdpData *psr_local::EM1(PomdpData *RandomPOMDP,symList* NewAbstractData,int size_NewAct, int size_newS,int size_NewOb,vector<float> *belief)
{  
	


	int iter = 1000; // 迭代次数
	
	Array1D<int>*action=getAction(NewAbstractData);
	
	Array1D<int>*observation=getObservation(NewAbstractData);
	string pomdptxt = "POMDP";
  	
  	ofstream ERRORTXT("error_pomdp.txt");
 	
	while(iter >0)
	{  
		pomdptxt = "POMDP";
		stringstream s1;
		s1 << iter;
		string s2 = s1.str();
		pomdptxt.append(s2);
		cout << pomdptxt << endl;
		cout<<"iter=="<<iter<<endl;
		Array2D<double>* alpha=getAlpha1(RandomPOMDP,NewAbstractData,size_NewAct,size_newS,action,observation,belief);
		Array1D<double>* scale=getScale1(RandomPOMDP,NewAbstractData, size_newS, action,observation); //获取scale[t]的值  
		Array2D<double>* beta=getBeta1(RandomPOMDP,NewAbstractData,size_NewAct,size_newS,scale,action,observation);	
		Array3D<double>* sigma=getSigma(RandomPOMDP,NewAbstractData,size_NewAct,size_newS,beta,alpha,action,observation);
		Array2D<double>* gama=getGama(NewAbstractData,size_NewAct,size_newS,beta,alpha,scale);
		
		RandomPOMDP=upDataTMatrix(RandomPOMDP,NewAbstractData,size_NewAct,size_newS,sigma,gama,action);
		RandomPOMDP=upDataOMatrix(RandomPOMDP,NewAbstractData,size_NewAct,size_newS,size_NewOb,gama,observation);
		iter--;
		delete alpha;
		delete  beta;
		delete	sigma;
		delete	gama;
  	RandomPOMDP=youhua(RandomPOMDP,size_NewAct,size_newS,size_NewOb);//对POMDP模型进行优化
  	outputPOMDP(RandomPOMDP, size_NewAct,size_newS,size_NewOb,pomdptxt);//输出POMDP模型至txt文件上	
	}

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
PomdpData*psr_local::youhua(PomdpData *RandomPOMDP ,int size_NewAct,int size_newS,int size_NewOb)
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
void *psr_local::outputPOMDP(PomdpData *RandomPOMDP,int size_NewAct,int size_newS, int size_NewOb,string pomdptxt)
{       
 ofstream POMDPTXT(pomdptxt.c_str());
 for(int a=0;a<size_NewAct;a++)  //对O矩阵进行优化
    {   
		 //POMDPTXT<<"a="<<a<<":"<<endl;   
     //POMDPTXT<<"T矩阵为"<<":"<<endl;         
     for(int i=0;i<size_newS;i++) 
        {  
         for(int j=0;j<size_newS;j++)
            { 
						 POMDPTXT<<(*RandomPOMDP->TMatrix)[a][i][j]<<"  ";
					  }
         POMDPTXT<<endl;             
        }   
     //cout<<endl;
     //cout<<endl;         
     //POMDPTXT<<"O矩阵为"<<":"<<endl;  
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
  


/************获取指定landmark所在的位置***********
输入：vector<Symbol*> 型的训练数据conData
     指定的landmark 

输出：训练数据中第一次出现指定l观测值所在的位置

*************************************************/
int psr_local::getSympos(vector<Symbol*>* conData,symList*startPosLm)
 {    
  int startpos=0;
  int length=conData->size();
  int len=startPosLm->size();
  for(int i=1;i<=length;i=i+2)
     {  
      symList *mark= getMemory(conData, i, len);//求取当前的观测值对应的pos(位置)        
      if(*mark==*startPosLm)  //如果当前观测在landmarkSet中，表示当前已经到了第一个landmark
        {
         startpos=i;
         cout<<"currPos="<<startpos<<endl;   
         break;    
        }                                     
     }
  return startpos;
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
double psr_local::getError(PomdpData *RandomPOMDP,symList_symListDblMap *newB_prob,symList2SymbolMap *newAct,symSet *interestTests,symList2VfMap *landmarkbe,int size_newS,symSet* landmarkSet,symList*startPosLm)
 {  
  //ofstream ERROS("ERROS.txt");
  ofstream ERRORTXT("ERROR.txt");ofstream TESTDATATXT("TESTDATATXT.txt");
  vector<float> *startLMbelief=getStartLMbelief(startPosLm,landmarkbe);//得到startPosLm对应的信念状态
  vector<Symbol*> *testData=env->getData(Length_TestData); 
  //env->createTestdata(Length_TestData);
 // vector<Symbol*> *testData=env->getconData(); 
  vector<Symbol*>::iterator vt=testData->begin();
  for(vt=testData->begin();vt!=testData->end();vt++)
     {
      TESTDATATXT<<**vt<<" ";
     }   
  int currPos= getSympos(testData,startPosLm);//找到testData中起始landmark出现的位置
  ERRORTXT<<"currPOS="<<currPos<<endl;   
  vector<float> *belief_start=new vector<float>;
  vector<float> *belief_end=new vector<float>;
  vector<float> *Abstractbelief_start=new vector<float>;   
  vector<float>::iterator be;
  map<symList*, vec_dou*>::iterator sv;
  for(be=startLMbelief->begin();be!=startLMbelief->end();be++)
     {
			Abstractbelief_start->push_back(*be);
    	belief_start->push_back(*be);  
			ERRORTXT<<"belifstart= "<<*be<<endl;
   	 } 
  int size_T=testData->size();
  int lmSize =startPosLm->size();                       //所有 landmark 已定为长度是一致的
  double  errSum=0.0;
  int num=0;//用来计算比较的步数
	symList*newO=new symList();
  map<symList*,symListDblMap*>::iterator ssv;
	vector<double>::iterator vd;
	for(ssv=newB_prob->begin();ssv!=newB_prob->end();ssv++)//得到空经历对应的NOW-NEWO  
		 {
		  if((*startPosLm)==*(ssv->first)) 
		    {
		     sv=newB_prob->begin()->second->begin();
  	     newO=new symList(sv->first);            //(newB[0]);//刚开始定义b0为newO
		     cout<<"newo="<<*newO<<endl;
		     break;
				}
     } 
  vector<float>::iterator it,is;
  symList_symListDblMap::iterator sym_symDbl;
  symListDblMap::iterator symd;
  vec_dou::iterator vs;
  map<symList*, Symbol*>::iterator na;
  symIter s; 
  for(int posT=currPos;posT<size_T;posT+=2)      //获取感兴趣的检验的local预测值和POMDP准確值的差值
     {
      num+=1;
      vector<float>* AbstractProb=new vector<float>;//存放的是newo对应的概率
	    for(ssv=newB_prob->begin();ssv!=newB_prob->end();ssv++)
	       {
	        if((*startPosLm)==*(ssv->first)) 
    	    for(symd=ssv->second->begin();symd!=ssv->second->end();symd++)
             {   
					    if(*newO==(*(symd->first)) )   //若找到相同的新定义的观测值
            		{
			 					 for(vs=symd->second->begin();vs!=symd->second->end();vs++)
                	  { 
  									 AbstractProb->push_back(*vs);//将newo对应的概率存入AbstractProb中
                    } 
            		 break;
			          }
        		 }
       	 }
			int i,j;float AbstractPre=0.0,pomPre=0.0;
      for(s=interestTests->begin(),i=0;s!=interestTests->end();s++,i++)//对每一个感兴趣的检验
      	 {
          pomPre=env->getPomdpProb(belief_start, *s);//求出准确POMDP检验发生的概率  
					ERRORTXT<<"pomPre="<<pomPre<<"                 ";       
          for(vf=AbstractProb->begin(),j=0;vf!=AbstractProb->end();vf++,j++)
          	 {      
							if(i==j)     //找到向量中与检验相同的序号
                { 
								 AbstractPre=*vf;  //找到该检验对应的概率  
			           ERRORTXT<<"ABSPRE="<<AbstractPre<<"  "; 
                 errSum += fabs(pomPre-AbstractPre );
								 ERRORTXT<<"errSum="<<errSum<<endl;
                }
             }
         }
	    delete AbstractProb;
      symList*ao=getTest(testData, posT, 1); //获得currpos的下一个检验
      vector<Symbol*> *test1=env->getVecdata(ao);//将symList型的检验转化为vector<Symbol*>型
		  ERRORTXT<<"ao= "<<*ao<<endl;
      belief_end=env->getBeliefState(belief_start,test1);//求出在belief_start下，检验test1发生的概率
      delete belief_start;
		  belief_start=new vector<float>;
      for(is=belief_end->begin();is!=belief_end->end();is++)//重新初始化belief_start
 	       {
					belief_start->push_back(*is);ERRORTXT<<"belifstart= "<<*is<<endl;
				 }
			//cout<<endl;
		  delete belief_end;               
		  symList*newA=getA(ao,newAct);//得到该步的抽象动作
		  delete ao;
      float pomdpProb,maxProb=0.0; int max=0;//用来表示概率最大的newO的序号
      for(ssv=newB_prob->begin();ssv!=newB_prob->end();ssv++)
		     {
	 	      if((*startPosLm)==*(ssv->first)) 
    	   	  {
		  			 for(symd=ssv->second->begin(),i=0;symd!=ssv->second->end();symd++,i++)  
                {      
								 symList*Ob=new symList(symd->first);//对每个新的取观测值，将其赋予ob
                 //pomdpProb=getabstractBeliefprob(RandomPOMDP,Abstractbelief_start,newA,Ob,size_newS);
				         delete Ob;				
                 if(pomdpProb>maxProb)  
                   { 
										maxProb=pomdpProb; //每发现概率比之前最大的概率大，则将其设为最大概率                        
                    max=i;//获得最大newo的下标值
                   }
                }                      
			       for(symd=ssv->second->begin(),i=0;symd!=ssv->second->end();symd++,i++)  
			          {
				         if(max==i)
				           {
                    delete newO; newO=new symList(symd->first);
                   }			
			          }
						}
		     }
      vector<float>* Abstractbelief_end=new vector<float>; //表示每一步结束后的状态
      Abstractbelief_end=getAbstractBeliefState(RandomPOMDP,Abstractbelief_start,newA,newO,size_newS);             
      delete newA,newO; 
      delete Abstractbelief_start;              //清空belief_start
		  Abstractbelief_start=new vector<float>;
      for(is=Abstractbelief_end->begin();is!=Abstractbelief_end->end();is++)//重新初始化Abstractbelief_start
 	       {
					Abstractbelief_start->push_back(*is);
				 }
	    delete Abstractbelief_end ;     
      if(num==10000) break;        
     }
  errSum /= ((interestTests->size()) *num);    
  cout<<"errSum="<<errSum<<endl;
  ERRORTXT<<"errSum="<<errSum<<endl;
  return errSum;                 
 }

double psr_local::getError1(PomdpData *RandomPOMDP,symListDblMap *newB_prob,symList2SymbolMap *newAct,symSet *interestTests, int size_newS )
{  
  	ofstream ERRORTXT("ERROR1.txt");
  	ofstream TESTDATATXT("TESTDATATXT1.txt");
  	vector<float> *startLMbelief=getStartbelief();//得到信念状态
  	vector<Symbol*> *testData=env->getData1(startLMbelief,50);//产生训练数据
  	
  	vector<Symbol*>::iterator vt;
  	TESTDATATXT << " testData: " << endl;
  	
  	for(vt=testData->begin();vt!=testData->end();vt++)
	{
		TESTDATATXT << **vt << " ";
	}   
	TESTDATATXT << endl;
	
  	vector<float> *belief_start=new vector<float>;
  	vector<float> *belief_end=new vector<float>;
  	vector<float> *Abstractbelief_start=new vector<float>;   
  	vector<float>::iterator be;
  	
  	for(be=startLMbelief->begin();be!=startLMbelief->end();be++)
	{
		Abstractbelief_start->push_back(*be);
    	belief_start->push_back(*be);  
		ERRORTXT<<"belifstart= "<<*be<<endl;
	} 
	
  	int size_T=testData->size();
  	cout << "size_T = " << size_T << endl;
  	double  errSum=0.0;
  	int num=0;//用来计算比较的步数
  	map<symList*, vec_dou*>::iterator sv;
	symList*newO=new symList();
	vector<double>::iterator vd;
	
	sv=newB_prob->begin();
  	newO=new symList(sv->first);            //(newB[0]);//刚开始定义b0为newO
	cout<<"newo="<<*newO<<endl;


  	vector<float>::iterator it,is;
  	symList_symListDblMap::iterator sym_symDbl;
  	symListDblMap::iterator symd;
  	vec_dou::iterator vs;
  	map<symList*, Symbol*>::iterator na;
  	symIter s; 
  	for(int posT = -1;posT < size_T-1;posT += 2)      //获取感兴趣的检验的local预测值和POMDP准確值的差值
	{
		num+=1;
		vector<float>* AbstractProb=new vector<float>;//存放的是newo对应的概率
    	for(symd=newB_prob->begin(); symd!=newB_prob->end(); symd++)
        {   
			if(*newO==(*(symd->first)) )   //若找到相同的新定义的观测值newO
            {
				for(vs=symd->second->begin();vs!=symd->second->end();vs++)
                { 
  					AbstractProb->push_back(*vs);//将newo对应的概率存入AbstractProb中
                } 
            	break;
			}
        }
        
		int i,j;
		float AbstractPre=0.0,pomPre=0.0;
      	for(s=interestTests->begin(),i=0;s!=interestTests->end();s++,i++)//对每一个感兴趣的检验
      	{
          	pomPre=env->getPomdpProb(belief_start, *s);//求出准确POMDP检验发生的概率  
			ERRORTXT<<"pomPre="<<pomPre<<"                 ";       
          	for(vf=AbstractProb->begin(),j=0;vf!=AbstractProb->end();vf++,j++)
          	{      
				if(i==j)     //找到向量中与检验相同的序号
                { 
					AbstractPre=*vf;  //找到该检验对应的概率  
			        ERRORTXT<<"ABSPRE="<<AbstractPre<<"  "; 
                 	errSum += fabs(pomPre-AbstractPre );
					ERRORTXT<<"errSum="<<errSum<<endl;
                }
            }
        }
	    delete AbstractProb;
	    
      	symList*ao=getTest(testData, posT, 1); //获得currpos的下一个检验
      	vector<Symbol*> *test1=env->getVecdata(ao);//将symList型的检验转化为vector<Symbol*>型
		ERRORTXT<<"next ao= "<<*ao<<endl;
		ERRORTXT << "posT : " << posT << endl;
      	belief_end=env->getBeliefState(belief_start,test1);//求出在belief_start下，检验test1发生的概率
      	delete belief_start;
		belief_start=new vector<float>;
      	for(is=belief_end->begin();is!=belief_end->end();is++)//重新初始化belief_start
 	    {
			belief_start->push_back(*is);ERRORTXT<<"belifstart= "<<*is<<endl;
		}
		delete belief_end;   
		
		symList*newA=getA(ao,newAct);//得到该步的抽象动作
		delete ao;
      	float pomdpProb,maxProb=0.0; int max=0;//用来表示概率最大的newO的序号
      	
		for(symd=newB_prob->begin(),i=0;symd!=newB_prob->end();symd++,i++)  
       	{      
			symList*Ob=new symList(symd->first);//对每个新的取观测值，将其赋予ob
            //pomdpProb=getabstractBeliefprob(RandomPOMDP,Abstractbelief_start,newA,Ob,size_newS);
			delete Ob;				
            if(pomdpProb>maxProb)  
            { 
				maxProb=pomdpProb; //每发现概率比之前最大的概率大，则将其设为最大概率                        
                max=i;//获得最大newo的下标值
            }
       	}                      
		for(symd=newB_prob->begin(),i=0;symd!=newB_prob->end();symd++,i++)  
		{
			if(max==i)
			{
                delete newO; 
                newO=new symList(symd->first);
            }			
		}
      	vector<float>* Abstractbelief_end=new vector<float>; //表示每一步结束后的状态
      	Abstractbelief_end=getAbstractBeliefState(RandomPOMDP,Abstractbelief_start,newA,newO,size_newS);             
      	delete newA; 
      	delete Abstractbelief_start;              //清空belief_start
		Abstractbelief_start=new vector<float>;
		for(is=Abstractbelief_end->begin();is!=Abstractbelief_end->end();is++)//重新初始化Abstractbelief_start
		{
			Abstractbelief_start->push_back(*is);
		}
	    delete Abstractbelief_end ;     
      	if(num==10000) break;        
	}
  	errSum /= ((interestTests->size()) *num);    
  	cout<<"errSum="<<errSum<<endl;
  	ERRORTXT<<"errSum="<<errSum<<endl;
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
float  psr_local::getabstractBeliefprob(PomdpData *RandomPOMDP,vector<float>*belief ,symList *var,int size_NewS)
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
       	 symList *test=getTestL(var, 0, 1);
      	 a=var->s->id;  //当前步采取的动作的id
       	 var=var->n;
      	 o=var->s->id;  //当前步获得的观测值的id
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
vector<float> *psr_local::getAbstractBeliefState(PomdpData *RandomPOMDP,vector<float>*belief ,symList*newA,symList*obs,int size_NewS)
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


/*************************************************************************************************
函数功能：利用线性无关的方法判断有噪声的矩阵的2行是否相似
输入：概率矩阵某2行的概率和动作发生次数
输出：1代表相似，其他不相似
*****************************************************************************************************/
int psr_local::findtherank(Array2D<double>*probsP,Array2D<double>*countsP )
 {  
  int size_m=probsP->dim1();//矩阵的行数，为2
  int size_n=probsP->dim2();//矩阵的列数
  int i,j;
  int rank=0,rankff=0;
  int k=0;
  float sum=0.0;
  for(i=0;i<(size_m-1);i++)
      for( j=0;j<size_n;j++)
          if((*probsP)[i][j]!=(*probsP)[i+1][j])
             k+=1;
  if(k==0)
     rankff=1;
  if(k!=0)
    {      
     for(i=0;i<(size_m-1);i++)
         for( j=0;j<size_n;j++)
            {
             sum+=((*probsP)[i][j]-(*probsP)[i+1][j])*((*probsP)[i][j]-(*probsP)[i+1][j]);
	          }
         if(sum>=0.25) 
            rankff=2;
         else 
           {   
            double avg=0.0;
            double bound=0.0,boundp=0.0;
            float PROBAB=0.985;
            double cutoffp=0.0;
    				for( i=0;i<size_m;i++)
      				  for( j=0;j<size_n;j++)
                   {
                    if(((*countsP)[i][j]==0)||((*probsP)[i][j]==0)) 
                       continue;
                    avg+=sqrt(((*probsP)[i][j])*(1-((*probsP)[i][j]))/((*countsP)[i][j])/(1- PROBAB));
	                 }
            avg/=size_m*size_n;
            boundp=infNorm(probsP);
            cutoffp=boundp*avg;
            singular = new SVD<double>(*probsP);
            Array1D<double> *singvals = new Array1D<double>();
            singular->getSingularValues(*singvals);
            delete singvals;
            rankff = singular->rank(cutoffp);
            delete singular;
           }
    }
  rank=rankff;
  return rank;
 }
  

/*************************************************************************************************
函数功能：将实验结果的数据存储在txt文件中
输入：需要存储的数据
输出：无
*****************************************************************************************************/
 void psr_local::getFinalresult(ofstream& Finalresult1,int j,double errsum,int size_NewOb,clock_t start,clock_t end)
 { 
  // Finalresult1<<"次数为"<<times;
   Finalresult1<<"方法"<<j<<":"<<endl;
   Finalresult1<<"组数为："<<size_NewOb<<" "<<"误差为："<<errsum<<endl;
   Finalresult1<<"时间为："<<(end-start)/CLOCKS_PER_SEC<<"    "<<endl;  
   Finalresult1<<"******************"<<endl;
 }


psr_local::psr_local(Environment *e): psr(e)
{

}


psr_local::~psr_local() 
{   }
#endif

