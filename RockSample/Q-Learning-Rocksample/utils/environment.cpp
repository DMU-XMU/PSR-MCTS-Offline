#include "environment.h"
#include "symbol.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <algorithm>
#include "utils2.h"
using namespace std;
using namespace UTILS;

/****************************************************************************
 函数功能：构造函数，设定landmark和初始状态
 输入输出:无
**************************************************************************/
Environment::Environment(int size, int rocks):
		Grid(size, size),
    Size(size),
    NumRocks(rocks)
{


	srand((unsigned int)time(NULL));
	S_sz =1000000;


	A_sz = rocks+5;
	O_sz = 6;




/*
	S_sz = 1000000;//
	A_sz = 4; //4个动作
	O_sz = 8192;//2的10次方个观测值*/
	int i;
	S = (Symbol **) malloc(S_sz * sizeof(Symbol *));
	for (i=0; i<S_sz; i++)
		S[i] = new Symbol(i, SYM_CUSTOM);      //S[i]存放所有状态

	A = (Symbol **) malloc(A_sz * sizeof(Symbol *));
	for (i=0; i<A_sz; i++)
		A[i] = new Symbol(i, SYM_ACTION);         //A[i]存放所有动作

	O = (Symbol **) malloc(O_sz * sizeof(Symbol *));
	for (i=0; i<O_sz; i++)
		O[i] = new Symbol(i, SYM_OBSERVATION);    //O[i]存放所有观测
}

Environment::~Environment( )
{
}


/******************************生成训练数据**************************************************************/


symList2Vec2Map * Environment::getPocmanData(int Len,int MaxLength)
{
	symList2Vec2Map *divideData=new symList2Vec2Map;//存储训练数据

	vector<symList*>* elementVec=new vector<symList*>;

	vector<int> *pos;//表示当前状态的各个参数的值，每走一步更新一下，初始为空

	symList*History=new symList;//当前的经历，每走一步更新一下，初始为空

	int len=History->size();//当前经历的长度，经历为空时长度为0

	StateClass* stateclass=new StateClass;//存储训练数据中的所有出现过的状态（状态-内容）

	stateclass->insert(pair<symList*, vector<int>*>(new symList(S[0]),pos ));//初始化stateclass，（状态为S0,内容为空）

	StateCount* statecount=new StateCount;//存储状态及其发生次数（状态-状态发生次数）

	statecount->insert(pair<symList*, int*>(new symList(S[0]),new int(0)));//初始化statecount，（状态S0，发生次数0）

	HistState* histstate=new HistState;//存储训练数据中所有经历对应的状态（经历-状态-状态发生次数）

	histstate->insert(pair<symList*, StateCount*>(new symList(History),statecount));//初始化histstate，（经历为空，状态为S0,发生次数为0）

	LENHistState* lenhiststate=new LENHistState;//存储训练数据中所有经历对应的状态（经历长度-经历-状态-状态发生次数）

	lenhiststate->insert(pair<int,HistState*>(len,histstate));//初始化lenhiststate，（长度为0,经历为空，状态为S0,发生次数为0）

	delete History;//释放当前History空间

	initRun();//初始化

	pos=CreateState();//pos:当前状态的各个参数的值

	stateclass->begin()->second=pos;//游戏初始处于状态S0,且S0内容为pos

	Symbol *actionToSend, *obsReceived;//暂存当前a,o

	StateClass* historypos;//存储训练数据中的所有出现过的状态（状态-内容）

	divideData->insert(pair<symList*, vec2*>(new symList(O[5]),elementVec ));//初始化存放训练数据的空间（landmark(设为O0)，（第一串训练数据，第二串训练数据，...））

	symList2Vec2Map::iterator div = divideData->begin();


	//for(int nl=0,Length=0;Length<MaxLength;nl++)//训练数据总长度最长MaxLength
	int Length=0;
	for(int nl=0;nl<7000;nl++)
  {


		initRun();
		History=new symList;


		pos=CreateState();

		historypos=new StateClass;
		historypos->insert(pair<symList*, vector<int>*>(new symList(History),pos));//初始化stateclass，（状态为S0,内容为空）

		int l;
		int flag=0;//每串数据结束标志

		for(l=0;l<Len;l++)//每串数据最长Len
    {

			//actionToSend=executeAction();  //发生动作
			actionToSend=executeActionRandom();
			obsReceived=getCurrentObservation();//产生观测值
			History->append(actionToSend);//更新当前经历
			History->append(obsReceived);

			if(inTerminalState) //如果游戏结束或本串数据长度超过Len，初始化
      			{
				//initRun();
				flag=1;
      			}

			pos=CreateState();
			historypos->insert(pair<symList*, vector<int>*>(new symList(History),pos ));//初始化stateclass,状态为S0,内容为空）
          //Update(History,lenhiststate,stateclass,pos);

			if(flag==1)//重新开始游戏
      {
				break;
      }
		}



		if(l<=Len) //如果游戏结束或本串数据长度超过Len，初始化
    {

			l+=1;
			Length+=l;//训练数据总长度

			ofstream hist("history.txt");
			hist << History << endl;
			StateClass::iterator k;
			for(k=historypos->begin(); k!=historypos->end();k++)
      {

				Update(k->first,lenhiststate,stateclass,k->second);
      }
			div->second->push_back(new symList(History));

			divideData->insert(pair<symList*, vec2*>(div->first,div->second));

			for(StateClass::iterator k=historypos->begin(); k!=historypos->end();k++)
			{
				delete k->first;
			}
			delete historypos;
    }
		else
		{
			for(StateClass::iterator k=historypos->begin(); k!=historypos->end();k++)
			{
				delete k->first;delete k->second;
			}
			delete historypos;
		}
		delete History;//释放当前History空间

	}
	string datatxt = "Psrdataid.txt";
	SaveDividata(divideData,datatxt);//文件保存训练数据//$**
	SaveLHSdata(lenhiststate);//文件保存lenhiststate
	SaveSCdata(stateclass);//文件保存stateclass
	Relize_LHS(lenhiststate);//释放lenhiststate空间
	Relize_SC(stateclass);//释放stateclass空间
	return divideData;
}




symList2Vec2Map * Environment::getPocmanData1(int Len,int MaxLength)
{
  //cout<<"/getPocmanData/"<<endl;
  Symbol *actionToSend, *obsReceived;
  //srand((unsigned int)time(NULL));       //若是不加这句 得到的动作序列就是固定的
  symList2Vec2Map *divideData=new symList2Vec2Map;
  vector<symList*>* elementVec=new vector<symList*>;
  divideData->insert(pair<symList*, vec2*>(new symList(O[0]),elementVec ));//初始化存放划分后数 的空间
  int i,j;int flag=0;

  vector<int> *pos;

  symList*History=new symList;
  int len=History->size();
  StateCount* statecount=new StateCount;
  HistState* histstate=new HistState;

  StateClass* stateclass=new StateClass;//存储训练数据中的所有出现过的状态（状态-内容）
  LENHistState* lenhiststate=new LENHistState;//存储训练数据中所有经历对应的状态（经历长度-经历-状态-状态发生次数）
  stateclass->insert(pair<symList*, vector<int>*>(new symList(S[0]),pos ));
  statecount->insert(pair<symList*, int*>(new symList(S[0]),new int(0)));
  histstate->insert(pair<symList*, StateCount*>(new symList(History),statecount));
  lenhiststate->insert(pair<int,HistState*>(len,histstate));
  delete History;
  initRun();


  pos=CreateState();

  stateclass->begin()->second=pos;
  int Length=0;int s=0;


  for(int nl=0;nl<800;nl++)
     {
      initRun();
      History=new symList;

      pos=CreateState();
      Update(History,lenhiststate,stateclass,pos);
      int l;
      flag=0;
      for(l=0;l<Len;l++)
         {
          s++;

          actionToSend=executeAction();
          obsReceived=getCurrentObservation();
          if(inTerminalState||(l==Len-1))
            {
             initRun();
             flag=1;
            }

          History->append(actionToSend);
          History->append(obsReceived);

          pos=CreateState();

          Update(History,lenhiststate,stateclass,pos);

          if(flag==1)
            {
             break;
            }
        }
      if(flag==1) l+=1;
      Length+=l;
      cout<<"nl="<<nl<<" l="<<l<<" Length="<<Length<<endl;
      divideData->begin()->second->push_back(new symList(History));

      delete History;
     }

	string datatxt = "Psrdataid.txt";
  SaveDividata(divideData,datatxt);
  SaveLHSdata(lenhiststate);
  SaveSCdata(stateclass);

  Relize_LHS(lenhiststate);
  Relize_SC(stateclass);

  return divideData;
}


symList2Vec2Map * Environment::getPocmanData2(int Len,int MaxLength)
{
	symList2Vec2Map *divideData=new symList2Vec2Map;//存储训练数据

	vector<symList*>* elementVec=new vector<symList*>;

	vector<int> *pos;//表示当前状态的各个参数的值，每走一步更新一下，初始为空

	symList*History=new symList;//当前的经历，每走一步更新一下，初始为空

	int len=History->size();//当前经历的长度，经历为空时长度为0

	StateClass* stateclass=new StateClass;//存储训练数据中的所有出现过的状态（状态-内容）

	stateclass->insert(pair<symList*, vector<int>*>(new symList(S[0]),pos ));//初始化stateclass，（状态为S0,内容为空）

	StateCount* statecount=new StateCount;//存储状态及其发生次数（状态-状态发生次数）

	statecount->insert(pair<symList*, int*>(new symList(S[0]),new int(0)));//初始化statecount，（状态S0，发生次数0）

	HistState* histstate=new HistState;//存储训练数据中所有经历对应的状态（经历-状态-状态发生次数）

	histstate->insert(pair<symList*, StateCount*>(new symList(History),statecount));//初始化histstate，（经历为空，状态为S0,发生次数为0）

	LENHistState* lenhiststate=new LENHistState;//存储训练数据中所有经历对应的状态（经历长度-经历-状态-状态发生次数）

	lenhiststate->insert(pair<int,HistState*>(len,histstate));//初始化lenhiststate，（长度为0,经历为空，状态为S0,发生次数为0）

	delete History;//释放当前History空间

	initRun();//初始化

	pos=CreateState();//pos:当前状态的各个参数的值

	stateclass->begin()->second=pos;//游戏初始处于状态S0,且S0内容为pos

	Symbol *actionToSend, *obsReceived;//暂存当前a,o

	StateClass* historypos;//存储训练数据中的所有出现过的状态（状态-内容）

	divideData->insert(pair<symList*, vec2*>(new symList(O[5]),elementVec ));//初始化存放训练数据的空间（landmark(设为O0)，（第一串训练数据，第二串训练数据，...））

	symList2Vec2Map::iterator div = divideData->begin();



	vector<int> actionSet;
	actionSet.push_back(0);
	actionSet.push_back(6);
	actionSet.push_back(0);
	actionSet.push_back(4);
	actionSet.push_back(1);
	actionSet.push_back(1);
	actionSet.push_back(5);
	actionSet.push_back(4);
	actionSet.push_back(1);
	actionSet.push_back(2);
	actionSet.push_back(7);
	actionSet.push_back(4);
	actionSet.push_back(2);
	actionSet.push_back(3);
	actionSet.push_back(8);
	actionSet.push_back(4);
	actionSet.push_back(2);
	actionSet.push_back(1);
	actionSet.push_back(1);
	actionSet.push_back(9);
	actionSet.push_back(4);

	int Length=0;
	for(int nl=0;nl<3000;nl++)
  {
		initRun();
		History=new symList;


		pos=CreateState();

		historypos=new StateClass;
		historypos->insert(pair<symList*, vector<int>*>(new symList(History),pos));//初始化stateclass，（状态为S0,内容为空）

		int l;
		int flag=0;//每串数据结束标志

		for(l=0;l<actionSet.size();l++)//每串数据最长Len
    {
			int act=actionSet.at(l);
			actionToSend=executeAction2(act);  //发生动作
			obsReceived=getCurrentObservation();//产生观测值
			History->append(actionToSend);//更新当前经历
			History->append(obsReceived);

			if(inTerminalState) //如果游戏结束或本串数据长度超过Len，初始化
      {
				//initRun();
				flag=1;
      }

			pos=CreateState();
			historypos->insert(pair<symList*, vector<int>*>(new symList(History),pos ));//初始化stateclass,状态为S0,内容为空）
			if(flag==1)//重新开始游戏
      {
				break;
      }
		}

			l+=1;
			Length+=l;//训练数据总长度
			ofstream hist("history.txt");
			hist << History << endl;
			StateClass::iterator k;
			for(k=historypos->begin(); k!=historypos->end();k++)
      {
				Update(k->first,lenhiststate,stateclass,k->second);
      }
			div->second->push_back(new symList(History));

			divideData->insert(pair<symList*, vec2*>(div->first,div->second));
			//divideData->begin()->second->push_back(new symList(History));//本串训练数据存入divideData
			//std::cout << divideData->begin()->second->size() << '\n';
			for(StateClass::iterator k=historypos->begin(); k!=historypos->end();k++)
			{
				delete k->first;
			}
			delete historypos;

		delete History;//释放当前History空间

	}
	string datatxt = "Psrdataid.txt";
	SaveDividata(divideData,datatxt);//文件保存训练数据//$**
	SaveLHSdata(lenhiststate);//文件保存lenhiststate
	SaveSCdata(stateclass);//文件保存stateclass
	Relize_LHS(lenhiststate);//释放lenhiststate空间
	Relize_SC(stateclass);//释放stateclass空间
	return divideData;
}




/*****设置rocksample初始状态*****/
void Environment::initRun()
{


	//std::cout << "initRun" << '\n';
	inTerminalState=false;



	if(Size==5&&NumRocks==5)
	{
		COORD rocks[] =
		{
				COORD(2, 4),
				COORD(0, 4),
				COORD(3, 3),
				COORD(2, 2),
				COORD(4, 1)
		};
		HalfEfficiencyDistance = 20;
		StartPos = COORD(0, 2);
		Grid.SetAllValues(-1);
		RockPos.clear();
		Valuable.clear();
		isCollected.clear();
		isChecked.clear();
		for (int i = 0; i < NumRocks; ++i)
		{
				Grid(rocks[i]) = i;
				RockPos.push_back(rocks[i]);

				Valuable.push_back(rand()%2);

				isCollected.push_back(0);
				isChecked.push_back(2);
		}

		AgentPos=StartPos;

	}



	if(Size==5&&NumRocks==7)
	{
		COORD rocks[] =
		{
				COORD(1, 0),
				COORD(2, 1),
				COORD(1, 2),
				COORD(2, 2),
				COORD(4, 2),
				COORD(0, 3),
				COORD(3, 4)
		};
		HalfEfficiencyDistance = 20;
		StartPos = COORD(0, 2);
		Grid.SetAllValues(-1);
		RockPos.clear();
		Valuable.clear();
		isCollected.clear();
		isChecked.clear();
		for (int i = 0; i < NumRocks; ++i)
		{
				Grid(rocks[i]) = i;
				RockPos.push_back(rocks[i]);
				Valuable.push_back(rand()%2);
				isCollected.push_back(0);
				isChecked.push_back(2);
		}

		AgentPos=StartPos;

	}

}


/**************************存储当前状态的内容**********************************************/
vector<int>* Environment::CreateState()
{

	vector<int> *rockstate=new vector<int>;
	rockstate->push_back(AgentPos.X);
	rockstate->push_back(AgentPos.Y);
	for (int i = 0; i < NumRocks; i++) {
		rockstate->push_back(Valuable.at(i));
	}
	for (int i = 0; i < NumRocks; i++) {
		rockstate->push_back(isCollected.at(i));
	}

	return rockstate;
}




Symbol* Environment::executeAction()
{
	Symbol *actionToSend;
	int act=SelectAction();
	actionToSend = getAction(act);
	moveRocksample(act);
	return actionToSend;
}



Symbol* Environment::executeAction2(int act)
{
	Symbol *actionToSend;
	actionToSend = getAction(act);
	moveRocksample(act);
	return actionToSend;
}



Symbol* Environment::executeActionRandom()
{
	Symbol *actionToSend;

	int act=SelectActionRandom();

	actionToSend = getAction(act);
//std::cout<<"begin"<<std::endl;
	moveRocksample(act);
//std::cout<<"end"<<std::endl;
	return actionToSend;
}




int Environment::SelectActionRandom()//
{
	int act;

	std::vector<int> preferAction;
	preferAction.clear();

	int rock=Grid(AgentPos);

	if (rock>=0&&isCollected.at(rock)==0) {
		preferAction.push_back(4);
	}
		if (AgentPos.Y+1<Size) {
			preferAction.push_back(0);
		}
		preferAction.push_back(1);
		if (AgentPos.Y-1>=0) {
			preferAction.push_back(2);
		}
		if (AgentPos.X-1>=0) {
			preferAction.push_back(3);
		}
		for (int j=0;j<NumRocks;j++) {
			if(isCollected.at(j)==0){
				preferAction.push_back(j+4+1);
			}
		}
		int temp=rand()%(preferAction.size());
		//std::cout <<preferAction.size()<< "temp"<<temp << '\n';
		act=preferAction.at(temp);
		//std::cout << "act-" <<act << '\n';




	return act;
}


int Environment::SelectAction()//
{
	int act;

	std::vector<int> preferAction;
	preferAction.clear();
	//std::cout << AgentPos.X <<AgentPos.Y<< '\n';
	int rock=Grid(AgentPos);
	double r=rand()%100/(double)101;
	if (rock>=0&&isCollected.at(rock)==0&&(r<0.5||isChecked.at(rock)<2)) {
		act=4;
	}else{
		if (AgentPos.Y+1<Size) {
			preferAction.push_back(0);
		}
		preferAction.push_back(1);
		if (AgentPos.Y-1>=0) {
			preferAction.push_back(2);
		}
		if (AgentPos.X-1>=0) {
			preferAction.push_back(3);
		}
		for (int j=0;j<NumRocks;j++) {
			if(isCollected.at(j)==0&&isChecked.at(j)>0){
				preferAction.push_back(j+4+1);
			}
		}
		int temp=rand()%(preferAction.size());
		//std::cout <<preferAction.size()<< "temp"<<temp << '\n';
		act=preferAction.at(temp);


		//std::cout << "act-" <<act << '\n';

	}

	return act;
}


/******************************
函数功能： 判断输入的整数i是否小于该环境下对应的动作个数，若小于，则返回对应下标为i的动作，反之则返回NULL
输入：动作的下标
输出：下标为i的动作或NULL
*********************************/
Symbol *Environment::getAction(int i) { return i<A_sz ? A[i] : NULL; }


void Environment::moveRocksample(int act)//执行动作
{

inTerminalState=false;
	Reward=0;
	if(act<4){
		switch (act) {
			case 0:
			{
				AgentPos.Y++;
				currentObs=0;
				Reward=0;
				break;
			}
			case 1:
			{
				if(AgentPos.X+1<Size)
				{
					AgentPos.X++;
					currentObs=0;
					Reward=0;
				}
				else
				{
					AgentPos.X++;
					currentObs=5;
					Reward=10;
					inTerminalState=true;
				}
				break;
			}
			case 2:
			{
				AgentPos.Y--;
				currentObs=0;
				Reward=0;
				break;
			}
			case 3:
			{
				AgentPos.X--;
				currentObs=0;
				Reward=0;
				break;
			}
		}

	}
	if (act==4) {
		int rock=Grid(AgentPos);
		//std::cout << "rock"<<rock << '\n';
		if(rock>=0&&isCollected.at(rock)==0){
			isCollected.at(rock)=1;
			if (Valuable.at(rock)==1) {
				Reward=-10;
				currentObs=4;
			}else if(Valuable.at(rock)==0){
				Reward=10;
				currentObs=3;
			}
		}
	}
	if (act>4) {
		Reward=0;
		int rock=act-4-1;
		if(rock>=0&&isCollected.at(rock)==0)
		{

			isChecked.at(rock)=isChecked.at(rock)-1;

			double distance = COORD::EuclideanDistance(AgentPos,RockPos.at(rock));
			double efficiency = (1 + pow(2, -distance / HalfEfficiencyDistance)) * 0.5;

			if (Bernoulli(efficiency))
					currentObs=Valuable.at(rock)+1;
	    else
	        currentObs=2-Valuable.at(rock);

		}

	}

}


Symbol* Environment::getCurrentObservation()
{
	return getObservation(currentObs);
}



/***********************
函数功能： 判断输入的整数i是否小于该环境下对应的观测值个数，若小于，则返回对应下标为i的观测值，反之则返回值为空
输入：观测值的下标
输出：下标为i的观测值或NULL
****************************/
Symbol *Environment::getObservation(int i) { return i<O_sz ? O[i] : NULL; }

/******************************更新lenhiststate，stateclass**************************************************************************/
void Environment::Update(symList* History,LENHistState* lenhiststate,StateClass* stateclass,vector<int>* pos)
{
 //cout<<"/Update/"<<endl;
 StateClass::iterator sc=stateclass->begin();
 vector<int>::iterator p=pos->begin();
 int flag=0;
 int i;
 symList *newstate;
 for(sc=stateclass->begin();sc!=stateclass->end();sc++)
    {
     vector<int>::iterator c=sc->second->begin();
     flag=0;
     for(c=sc->second->begin(),p=pos->begin();c!=sc->second->end(),p!=pos->end();c++,p++)
        {
         if((*c)!=(*p)) {flag=1;break;}
        }
     if(flag==0) {newstate=new symList(sc->first);delete pos; pos=NULL;break;}
    }
 sc--;


 if(flag==1)
    {

     newstate=new symList(S[(sc->first->s->id)+1]);

     stateclass->insert(pair<symList*, vector<int>*>(new symList(newstate),pos));

    }

 int len=History->size();
 LENHistState::iterator lhs=lenhiststate->find(len);
 if(lhs!=lenhiststate->end())
   {
    HistState::iterator hs=lhs->second->find(History);
    if(hs!=lhs->second->end())
      {
       StateCount::iterator sct=hs->second->find(newstate);
       if(sct!=hs->second->end())
         {
          *(sct->second)+=1;
         }
       else {hs->second->insert(pair<symList*, int*>(new symList(newstate),new int(1)));}
      }
    else
      {
       StateCount* statecount=new StateCount;
       statecount->insert(pair<symList*, int*>(new symList(newstate),new int(1)));
       lhs->second->insert(pair<symList*, StateCount*>(new symList(History),statecount));
      }
   }
 else
   {
    StateCount* statecount=new StateCount;
    statecount->insert(pair<symList*, int*>(new symList(newstate),new int(1)));
    HistState* histstate=new HistState;
    histstate->insert(pair<symList*, StateCount*>(new symList(History),statecount));
    lenhiststate->insert(pair<int, HistState*>(len,histstate));
   }
 delete newstate;
}


void Environment::SaveDividata(symList2Vec2Map *divideData, string txtname)
{
	//ofstream PsrDataId  ("Psrdataid.txt");
	ofstream PsrDataId  (txtname.c_str());
	Symbol *actionToSend,*obsReceived;
	int i;
	vector<symList*>::iterator ele;
	for(i=0,ele=divideData->begin()->second->begin();ele!=divideData->begin()->second->end();i++,ele++)
    	{
				//std::cout << i << '\n';
		symList *history=new symList(*ele);
		vector<Symbol*>* his=getVecdata(history);
		PsrDataId<<history->size()<<" ";
		for(int pos=0;pos<history->size();)
		{
			symList *act = getMemory(his, pos, 1);
			pos++;
          //cout<<"act="<<*act<<endl;
			actionToSend=act->s;
			int aId=actionToSend->id;//当前步采取的动作的id
			PsrDataId<<aId<<" ";
			delete act;
			symList *obs=getMemory(his,pos,1);
			pos++;
        //  cout<<"obs="<<*obs<<endl;
			obsReceived=obs->s;
			int oId=obsReceived->id;//当前步观测值的id
			PsrDataId<<oId<<" ";
			delete obs;
		}
		PsrDataId<<endl;
		delete history;
		delete his;
    	}
}

/*******************************************************
 函数功能：
 把symList型的动作-观测序列数据转换成vector<Symbol*>型
 输入：symList型的动作-观测序列
 输出：vector<Symbol*>型的动作-观测序列
******************************************************/
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

/***************************
函数功能：获取vector<Symbol*>数据data中位置为pos，长度为len的记忆
输入参数：
data——vector<Symbol*>型操作数据；
pos——指定的开始位置；
len——要获取的记忆的长度
输出：data中位置为pos，长度为len的记忆
***************************/
symList* Environment::getMemory(vector<Symbol*>* data, int pos, int len) //获取vector<Symbol*>型数据从位置pos开始指定长度的记忆
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

/*****************************"lenhiststateid.txt"文件保存lenhiststate******************************************************************/
void Environment::SaveLHSdata(LENHistState* lenhiststate)
{
	ofstream HiststateId("lenhiststateid.txt");
	LENHistState::iterator lhs;
	HistState::iterator hs;
	StateCount::iterator sct;
	Symbol *actionToSend,*obsReceived;
	for(lhs=lenhiststate->begin(); lhs!=lenhiststate->end();lhs++)
    {
		int len=lhs->first;
		for(hs=lhs->second->begin(); hs!=lhs->second->end();hs++)
        {
			HiststateId<<len<<" ";
			symList *history=new symList(hs->first);
			vector<Symbol*>* his=getVecdata(history);
			if(len>1)
			{
				for(int pos=0;pos<len;)
				{
					symList *act = getMemory(his, pos, 1);pos++;
                //cout<<"act="<<*act<<endl;
					actionToSend=act->s;
					int aId=actionToSend->id;//当前步采取的动作的id
					HiststateId<<aId<<" ";
					delete act;
					symList *obs=getMemory(his,pos,1);pos++;
                //cout<<"ob="<<*obs<<endl;
					obsReceived=obs->s;
					int oId=obsReceived->id;//当前步观测值的id
					HiststateId<<oId<<" ";
					delete obs;
               }
			}
			delete history;
			delete his;
			sct=hs->second->begin();
			HiststateId<<hs->second->size()<<" ";
			for(sct=hs->second->begin();sct!=hs->second->end();sct++)
            {
				symList *state=new symList(sct->first);
				int sId=state->s->id;//当前步采取的动作的id
				HiststateId<<sId<<" ";
				delete state;
				int cId=*(sct->second);
				HiststateId<<cId<<" ";
            }
			HiststateId<<endl;
        }
    }
}

/***********************************"stateclassid.txt"文件保存stateclass***********************************************************/
void Environment::SaveSCdata(StateClass* stateclass)
{
	ofstream StateclassId  ("stateclassid.txt");
	StateClass::iterator sc;
	for(sc=stateclass->begin();sc!=stateclass->end();sc++)
    {
		symList *state=new symList(sc->first);
		int sId=state->s->id;//当前步采取的动作的id
		StateclassId<<sId<<" ";
		delete state;
		StateclassId<<sc->second->size()<<" ";
		vector<int>::iterator c=sc->second->begin();
		for(c=sc->second->begin();c!=sc->second->end();c++)
        {
			int cId=*c;
			StateclassId<<cId<<" ";
        }
		StateclassId<<endl;
    }
}

void Environment::Relize_LHS(LENHistState*lenhiststate)
{
	LENHistState::iterator lhs;
	HistState::iterator hs;
	StateCount::iterator sct;
	for(lhs=lenhiststate->begin(); lhs!=lenhiststate->end();lhs++)
    {
		for(hs=lhs->second->begin(); hs!=lhs->second->end();hs++)
        {
			for(sct=hs->second->begin();sct!=hs->second->end();sct++)
            {
				delete sct->first;
				delete sct->second;
				sct->second=NULL;
            }
			delete hs->second;
			hs->second=NULL;
			delete hs->first;
        }
		delete lhs->second;
		lhs->second=NULL;
     }
	delete lenhiststate;
	lenhiststate=NULL;
}

void Environment::Relize_SC(StateClass*stateclass)
{
	StateClass::iterator sc=stateclass->begin();
	for(sc=stateclass->begin();sc!=stateclass->end();sc++)
    {
		delete sc->second;
		sc->second=NULL;
		delete sc->first;
    }
	delete stateclass;
	stateclass=NULL;
}

/**********************
函数功能： 获得环境下所有动作的连续序列（例如环境中有4个动作a1、a2、a3、a4，则得到a1-a2-a3-a4）
输入：无
输出：连续的动作序列
**********************/
symList *Environment::getActions()
{
	int i;
	symList *toret = NULL;

	for (i=A_sz-1; i>=0; i--)
		toret = new symList(A[i], toret);
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

StateClass* Environment::ReadSCdata()
{
  	StateClass* stateclass=new StateClass;
  	char namesc[80]="/stateclassid.txt";  //存放PSR训练数据的文件名,存放的是Symbol对应的id(int)
  	//cout<<namesc<<endl;
  	char buffer[160];
  	getcwd(buffer, 160);
  	//printf("The current directory is: %s\n", buffer);
   	char *workdir=strcat(buffer,namesc);
  	//cout<<workdir<<endl;
  	ifstream ins(workdir);
	if(!ins)
	{
		cerr << "open error! " << endl;
		exit(1);
	}
  	vector<int> scId;
  	for(int id;ins>>id;)
  	{
    		scId.push_back(id);
  	}
  	int n=scId.size();
  	int num,i,j;
  	vector<int> *pos;
  	for(j=0;j<n-1;j+=num+2)
     	{
      		symList *state=new symList(S[scId.at(j)]);
      		pos=new vector<int>;
      		num=scId.at(j+1);
      		for( i=j+2;i<j+2+num;i+=1)
         	{
          		int a=scId.at(i);
          		pos->push_back(a);
         	}
      		stateclass->insert(pair<symList*, vector<int>*>(new symList(state),pos));
      		delete state;
     	}
  	return stateclass;
}


bool Environment::CheckAction(int act)
{
	bool result=true;

	if (act<4) {
		switch (act) {
			case 0:{
				if (AgentPos.Y+1<Size) {
					result=true;
				}else{
					result=false;
				}
				break;
			}
			case 1:{
				result=true;
				break;
			}
			case 2:{
				if (AgentPos.Y-1>=0) {
					result=true;
				}else{
					result=false;
				}
				break;
			}
			case 3:{
				if (AgentPos.X-1>=0) {
					result=true;
				}else{
					result=false;
				}
				break;
			}
		}
	}else if(act==4){
		int rock=Grid(AgentPos);
		if(rock>=0&&isCollected.at(rock)==0){
				result=true;
		}else{
			result=false;
		}
	}else if(act>4){
		int j=act-4-1;

			if(isCollected.at(j)==0){
				result=true;
			}else{
				result=false;
			}
		}



	return result;
}


symList * Environment::getOneData(StateClass* stateclass,symList *schose,symList* act)
{
	initRun();
	vector<int>::iterator pos=stateclass->find(schose)->second->begin();
	AgentPos.X=*pos;
	pos++;
	AgentPos.Y=*pos;
	pos++;
	for (int i = 0; i < NumRocks; i++) {
		Valuable.at(i)=*pos;
		pos++;
	}
	for (int i = 0; i < NumRocks; i++) {
		isCollected.at(i)=*pos;
		pos++;
	}

	int actnum=act->size();//actnum：动作个数
	symList *test=new symList;
	bool isLegal;

	for(int i=0;i<actnum && inTerminalState==false ;i++)
  {

		if (AgentPos.X>=Size) {
			isLegal=false;
			test=new symList;
			break;
		}
		/*
		if (AgentPos.X>=Size) {
			initRun();
		}*/

		Symbol *actionToSend,*obsReceived;

		actionToSend=act->s;//提取动作

		int a=actionToSend->id;//提取动作下标

		isLegal=CheckAction(a);
		if (isLegal==true) {
			moveRocksample(a);
			obsReceived=getCurrentObservation();//如果执行完动作后游戏结束，记录此时观测值

			test->append(actionToSend);
			test->append(obsReceived);//存储当前步得到的动作和观测

			act=act->n;//下一个动作
		}else{
			test=new symList;
			break;
		}


  }
	return test;
}

LENHistState* Environment::ReadLHSdata()
{
  LENHistState* lenhiststate=new LENHistState;
  LENHistState::iterator lhs;
  char namehs[80]="/lenhiststateid.txt";  //存放PSR训练数据的文件名,存放的是Symbol对应的id(int)
  //cout<<namehs<<endl;
  char buffer[160];
  getcwd(buffer, 160);
  //printf("The current directory is: %s\n", buffer);
  char *workdir=strcat(buffer,namehs);
  //cout<<workdir<<endl;
  ifstream inh(workdir);
	if(!inh)
	{
		cerr << "open error! " << endl;
		exit(1);
	}
  vector<int> hsId;
  for(int id;inh>>id;)
  {
    hsId.push_back(id);
  }
  int n=hsId.size();

  int ssize,i,j,hsize;
  for(j=0;j<n-1;j+=(hsize+ssize*2+2))
     {
      hsize=hsId.at(j);
      int len=hsize;
      symList *history=new symList;
      if(hsize>1)
        {
         for( i=j+1;i<j+hsize;i+=2)
            {
             int a=hsId.at(i);
             //Symbol *act = new Symbol(a, ret->arr_act[a], SYM_ACTION);//cout<<"act="<<*act<<endl;
             Symbol *act =getAction(a);
             history->append(act);
             i++;
             int o=hsId.at(i);        //得到观测值o,o的type有两种情况:普通的观測值(type=SYM_OBSERVATION==1)和landmark对应的观測值(type=SYM_LANDMARK==5)
             //Symbol *obs;
             //if(o==0) obs= new Symbol(o, ret->arr_obs[o],SYM_LANDMARK);
             //else
            //obs= new Symbol(o, ret->arr_obs[o],SYM_OBSERVATION);//cout<<"landmark--obs="<<*obs<<endl;
             Symbol *obs=getObservation(o);
             history->append(obs);
             i--;
            }
        }
      if(hsize==1) hsize-=1;

      ssize=hsId.at(j+hsize+1);
      StateCount* statecount=new StateCount;
      for( i=j+hsize+2;i<j+hsize+ssize*2+2;i+=2)
         {
          symList *state=new symList(S[hsId.at(i)]);
          i++;
          int count=hsId.at(i);
          i--;
          statecount->insert(pair<symList*, int*>(new symList(state),new int(count)));
          delete state;
         }
      lhs=lenhiststate->find(len);
      if(lhs==lenhiststate->end())
        {
         HistState* histstate=new HistState;
         histstate->insert(pair<symList*, StateCount*>(new symList(history),statecount));
         lenhiststate->insert(pair<int,HistState*>(len,histstate));
        }
      else lhs->second->insert((pair<symList*, StateCount*>(new symList(history),statecount)));
      delete history;
     }

  return lenhiststate;
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

int Environment::nrStates() { return S_sz; }

int Environment::computercurreward()
{
	int reward = 0;
	reward = reward - 1;//default
	for(int lGhost = 0; lGhost < NumGhosts; lGhost++)
    	{
		if(GhostPos[lGhost][0] == PocmanPos[0] &&(GhostPos[lGhost][1] == PocmanPos[1]))
		{
			if(PowerSteps >= 0)
				reward = reward + 25;//EatGhost
			else
			{
				reward = reward - 100;//Die
			}
		}
    	}
	if(gameMap[PocmanPos[0]][PocmanPos[1]] == 5)//吃到食物，5食物
	{
		reward = reward + 10;
	}
	return reward;
}
