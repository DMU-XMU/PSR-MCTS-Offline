#ifndef __ENVIRONMENT_H_
#define __ENVIRONMENT_H_

#include "symbol.h"
#include "coord.h"
#include "grid.h"

#include <set>
#include <map>
#include <vector>

//using namespace std;

typedef set<symList *, symbolComparator> symSet; // a set of symbol strings
typedef symSet::iterator symIter; // an iterator over a set of symbol strings

typedef vector<Symbol*>::iterator vecsymIter;//
typedef vector<symList*> vec2;//数据集
typedef map<symList *,vec2*, symbolComparator> symList2Vec2Map;//landmark-(数据集)

//typedef map<symList*,vector<float>*,symbolComparator> symList2VfMap;

typedef map<symList*, int *, symbolComparator> symList2IntMap;//
//typedef map<symList*, double, symbolComparator> symList_DblMap; //
//typedef map<symList*, symList_DblMap*,symbolComparator> symList2Prob;//
//typedef map<symList*,vector<symList2Prob*>*,symbolComparator>symList2Group;//

typedef map<symList*, Symbol*,symbolComparator> symList2SymbolMap;//

typedef map<symList*,int*,symbolComparator>StateCount;//存储状态及其发生次数（状态-状态发生次数）
typedef map<symList*,vector<int>*,symbolComparator> StateClass;//存储训练数据中的所有出现过的状态（状态-内容）
typedef map<symList*,StateCount*,symbolComparator>HistState;//存储训练数据中所有经历对应的状态（经历-状态-状态发生次数）
typedef map<int,HistState*>LENHistState;//存储训练数据中所有经历对应的状态（经历长度-经历-状态-状态发生次数）

class Environment{
	public:
		Environment(int size, int rocks);//构造函数
		~Environment();//析构函数
		Symbol **S ,**A, **O, **N, **B;
		int S_sz ,A_sz, O_sz, N_sz, B_sz;
		int	  nrActions();
  		int	  nrObservs();
		int	  nrStates();
		//pacman相关参数
		int X,Y,Dime, NumGhosts, PassageY, GhostRange, SmellRange, HearRange,PowerSteps,foodLeft,foodNum;

		double CHASE_PROB,DEFENSIVE_SLIP;
		int PocmanPos[2], GhostPos[4][2],GhostDir[4],maze[19][17],gameMap[19][17],GhostHome[4][2],PocmanHome[2];
		int minfood[3][2];

		void SeecurMaze();
		void Build_PacmanMaze();//生成pacman游戏用矩阵存于全局变量maze中
		vector<int>* ReadFoodPlace();//提取食物位置
		symList2Vec2Map* getPocmanData(int Len,int MaxLength);//得到训练数据
		symList2Vec2Map* getPocmanData1(int Len,int MaxLength);//得到训练数据
		symList2Vec2Map* getPocmanData2(int Len,int MaxLength);//得到训练数据
		void initRun();//
		void RankGhosts();//
		vector<int>* CreateState();//
		Symbol* executeAction();//
		Symbol* executeAction2(int act);//
		Symbol* executeActionRandom();
		void computeNewStateInformation();
		int SelectAction();
		int SelectActionRandom();

		void moveRocksample(int act);
		bool CheckAction(int act);
		Symbol  *getAction(int i);
		void moveGhosts(int g);
		void resetGhost(int pGhost);

		int moveGhostRandom(int pGhost);
		int moveGhostDefensive(int flag,int pGhost);
		int moveGhostAggressive(int flag,int pGhost);
		int movefoodAggressive(int lMinDist);

		vector<int>* getValidMovements(int pXPos, int pYPos);
		int oppositeDir(int dir);
  		int directionalDistance(int lhs[], int rhs[], int dir);
		void makeMove(int pMove,int Pos[2]);

		Symbol* getCurrentObservation();
		//int intgetCurrentObservation();
		int computeIntFromBinary(bool pBinary[]);
		Symbol  *getObservation(int i);
		int computeFoodManhattanDist();
		int computeGhostDist(int& lGhost);

		void Update(symList* History,LENHistState* lenhiststate,StateClass* stateclass,vector<int>* pos);

		void SaveDividata(symList2Vec2Map *divideData, string txtname);
		vector<Symbol*>   *getVecdata(symList* var);
		symList* getMemory(vector<Symbol*>* data, int pos, int len);

		void SaveLHSdata(LENHistState* lenhiststate);
		void Relize_LHS(LENHistState*lenhiststate);

		void SaveSCdata(StateClass* stateclass);
		void Relize_SC(StateClass*stateclass);

		symList *getActions();		// get a symlist with all the actions, useful for iterating through all acts.
		symList *getObservations();	// idem, observations

		StateClass* ReadSCdata();
		LENHistState* ReadLHSdata();
		symList * getOneData(StateClass* stateclass,symList *schose,symList* act);

		int Reward;
		int computercurreward();


		bool checkTerminal(int obs) {
			if (obs==6) {
				return true;
			}else{
				return false;
			}
		}


		bool inTerminalState;



		GRID<int> Grid;
		std::vector<COORD> RockPos;
		int Size, NumRocks;
		COORD StartPos;
		double HalfEfficiencyDistance;


		COORD AgentPos;

		std::vector<int> Valuable;
		std::vector<int> isCollected;
		std::vector<int> isChecked;


		int currentObs;


		COORD AgentPosCopy;
		std::vector<int> isCollectedCopy;


		void copyCurrentState() {
			AgentPosCopy.X=AgentPos.X;
			AgentPosCopy.Y=AgentPos.Y;
			isCollectedCopy.clear();
			for (int i = 0; i < NumRocks; i++) {
				//isCollectedCopy.at(i)=isCollected.at(i);
				isCollectedCopy.push_back(isCollected.at(i));
			}
		}

		std::vector<int> legalAction;

		void getLegalAction() {

			legalAction.clear();
			if (AgentPosCopy.Y+1<Size) {
				legalAction.push_back(0);
			}
			legalAction.push_back(1);
			if (AgentPosCopy.Y-1>=0) {
				legalAction.push_back(2);
			}
			if (AgentPosCopy.X-1>=0) {
				legalAction.push_back(3);
			}
			int rock=Grid(AgentPosCopy);
			if (rock>=0&&isCollectedCopy.at(rock)==0){
				legalAction.push_back(4);
			}
			for (int i = 5; i < 5+NumRocks; i++) {
				int rocknum=i-5;
				if (isCollectedCopy[rocknum]==0) {
					legalAction.push_back(i);
				}
			}


		}

		void usedStateUpdate(int act) {
			if(act<4){
				switch (act) {
					case 0:
					{
						AgentPosCopy.Y++;
						break;
					}
					case 1:
					{
						AgentPosCopy.X++;
						break;
					}
					case 2:
					{
						AgentPosCopy.Y--;
						break;
					}
					case 3:
					{
						AgentPosCopy.X--;
						break;
					}
				}

			}else if (act==4) {
				int rock=Grid(AgentPosCopy);
				isCollectedCopy.at(rock)==1;
			}
		}
};


#endif
