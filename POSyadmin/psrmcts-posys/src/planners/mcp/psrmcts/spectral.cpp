
#include "spectral.h"
#include <math.h>
#include <algorithm>

#include <iomanip>




using namespace std;


//-----------------------------------------------------------------------------


SPECTRAL::SPECTRAL(const SIMULATOR& simulator)
:   Simulator(simulator)
{

	int i;
	A_sz = Simulator.GetNumActions(); O_sz = Simulator.GetNumObservations();
	S_sz=Simulator.GetNumStates();	
	S = (Symbol **) malloc(S_sz * sizeof(Symbol *));
    for (i=0; i<S_sz; i++) S[i] = new Symbol(i, SYM_CUSTOM);      //states
 
    A = (Symbol **) malloc(A_sz * sizeof(Symbol *));                                 
    for (i=0; i<A_sz; i++) A[i] = new Symbol(i, SYM_ACTION);         //actions

    O = (Symbol **) malloc(O_sz * sizeof(Symbol *));                                  
    for (i=0; i<O_sz; i++) O[i] = new Symbol(i, SYM_OBSERVATION);    //observations

	
 
    	Tr=new double**[A_sz];
	for(int i=0;i<A_sz;i++)
	{
		Tr[i]=new double*[S_sz];
	}
	for(int i=0;i<A_sz;i++)
		for(int j=0;j<S_sz;j++)
			Tr[i][j]=new double[S_sz];

 	Ob=new double**[A_sz];
	for(int i=0;i<A_sz;i++)
	{
		Ob[i]=new double*[S_sz];
	}
	for(int i=0;i<A_sz;i++)
		for(int j=0;j<S_sz;j++)
			Ob[i][j]=new double[O_sz];


	ofstream TrTXT("TR.txt");
	for(int a=0;a<A_sz;a++)
	{
		for(int s1=0;s1<S_sz;s1++)
		{
			for(int s2=0;s2<S_sz;s2++)
			{

				Tr[a][s1][s2]=Simulator.getT(s1*S_sz*A_sz+a*S_sz+s2);
				TrTXT<<Tr[a][s1][s2]<<" ";
			}
		}
		TrTXT<<std::endl;
	}	
	for(int a=0;a<A_sz;a++)
	{
		for(int s=0;s<S_sz;s++)
		{
			for(int o=0;o<O_sz;o++)
			{
				Ob[a][s][o]=Simulator.getO(s*O_sz*A_sz+a*O_sz+o);
			}
		}
	}	


	int startS=Simulator.CreateStartState();	
	
	currState=S[startS];
	prevState=NULL;

}

SPECTRAL::~SPECTRAL()
{
}




//training data
symList2IntMap* SPECTRAL::createPsrdata2(int num_Data)
{
	std::vector<int> actionset;
	
	actionset.push_back(3);
	actionset.push_back(5);
	actionset.push_back(6);
	actionset.push_back(4);
	actionset.push_back(2);
	actionset.push_back(1);
	actionset.push_back(0);
	actionset.push_back(0);

	symList *toret=NULL;
	for(int i=0;i<8;i++)
	{
	 int action=actionset[i];
	 toret=new symList(A[action],toret);

	}
	std::vector<Symbol*>* continueData=new std::vector<Symbol*>;
	for(int j=0;j<num_Data;j++)
	{
		Symbol *actionToSend, *obsReceived;
  		for(int i=0; i<8; i++)               
 	 	{	

    			actionToSend = getAction(toret->s->id);
			continueData->push_back(actionToSend);
    			obsReceived = sendAction1(actionToSend); 
    			continueData->push_back(obsReceived);   		
			toret=toret->n;
  		}
		for(int i=0;i<8;i++)
		{
	 		int action=actionset[i];
	 		toret=new symList(A[action],toret);

		}
		
		int startS=Simulator.CreateStartState();	
		currState=S[startS];

	}
	int size=continueData->size();                    
  	symSet *landmarkSet= new symSet();
  	symList *landmark;

  	symList2Vec2Map *Matrix = new symList2Vec2Map;


  	for(int k=0;k<O_sz+1;k++)                  
  	{
    
      		landmark=new symList(O[0]);
      		landmarkSet->insert(landmark);
  	}


  	symIter s;
	for(s=landmarkSet->begin();s!=landmarkSet->end();s++)
	{
	    std::vector<symList*>* elementVec=new std::vector<symList*>;
	    Matrix->insert(pair<symList*, vec2*>(new symList(O[0]),elementVec ));
  	}
  	symList2Vec2Map::iterator vv=Matrix->begin();
  
  
  	
 	 for(int pos=0;pos<size-1;pos+=16) 
  	{  
    		symList *currData= new symList();
		for(int pos1=pos;pos1<pos+16;pos1+=2)
		{
		
			currData->append( new symList(continueData->at(pos1), new symList(continueData->at(pos1+1))) ); 

  		}
		for(vv=Matrix->begin();vv!=Matrix->end(); vv++)
		{
            
        	       	vv->second->push_back(currData);
		      	Matrix->insert(pair<symList*, vec2*>(vv->first,vv->second ));
               
   		 }


	
  	}	
  
   
 

  for(s=landmarkSet->begin();s!=landmarkSet->end();s++)
  {
    delete *s;
  }
  delete landmarkSet;        

  string TXTNAME="historyData1.txt";
  symList2IntMap *Histcout=new symList2IntMap;
  Histcout=getHistcout1(Matrix,2,TXTNAME);

   return Histcout;
	
	

}

//perform an action and get an observation
Symbol* SPECTRAL::sendAction1(Symbol* act)     
{    
  float prob, t;
  int a, s1, s2, o;
  a = act->id;       
  s1 = currState->id;

  prob = (float) random() * 100.0 / (float) RAND_MAX;

  s2 = 0; t = 0.0;
  while ((prob > t + Tr[a][s1][s2]*100) && (s2 < S_sz))
  {
    t += Tr[a][s1][s2]*100; s2++;
  }
  if (s2 == S_sz) s2 = S_sz-1;

  prevState = currState; currState = S[s2];
  prob = (float) random() * 100.0 / (float) RAND_MAX;
   o = 0; t = 0.0;
  while ((prob > t + Ob[a][s2][o]*100) && (o < O_sz))
  { 
    t += Ob[a][s2][o]*100; o++;
  }
  if (o == O_sz) o = O_sz-1;
  return O[o];
}



symList2IntMap *SPECTRAL::getHistcout1(symList2Vec2Map *divideData,int CUTOFF,string txtname)
{
    ofstream TXTN(txtname.c_str());  
    symList2Vec2Map::iterator vi=divideData->begin();
   
    symList2IntMap *Histcout=new symList2IntMap;
    for(vi=divideData->begin();vi!=divideData->end(); vi++)
    {        
       for(std::vector<symList*>::iterator begvec = vi->second->begin();begvec != vi->second->end();begvec++)
       { 
            if(Histcout->find(*begvec) == Histcout->end())
            {
              Histcout->insert(pair<symList*, int*>(new symList(*begvec), new int(1)));
            }
            else *(Histcout->find(*begvec)->second) += 1;
         
         
       } 
     }

  symList2IntMap::iterator histIterator,tempHistIterator;

       for(histIterator=Histcout->begin(); histIterator!=Histcout->end();)
       {
          if( *(histIterator->second) < CUTOFF)        
          {
            tempHistIterator = histIterator++;
            delete tempHistIterator->first;
            delete tempHistIterator->second;
            Histcout->erase(tempHistIterator);
         }
         else
         {
           ++histIterator;
         }
      }
  for( histIterator=Histcout->begin(); histIterator!=Histcout->end();histIterator++)
     {
      TXTN<<*(histIterator->first)<<" "<<*(histIterator->second)<<endl;
     }

  return Histcout;
}









symList* SPECTRAL::getMemory(std::vector<Symbol*>* data, int pos, int len) 
{
  int startH = pos - len + 1;                  
  symList* currMemory = new symList();       
  if((pos+1) >= len)                           
  {
    for(int i=startH; i<=pos; i++)            
    {
      currMemory->append(data->at(startH));   
    }
      return currMemory;                       
  }
  else
    return NULL;                               
}



symList2Vec2Map* SPECTRAL::divideData(std::vector<Symbol*>* Data)
{
  int size=Data->size();                    
  symSet *landmarkSet= new symSet();
  symList *landmark;

  symList2Vec2Map *Matrix = new symList2Vec2Map;
  for(int k=0;k<O_sz+1;k++)                  
  {
    
      landmark=new symList(O[0]);
      landmarkSet->insert(landmark);
  }



  symIter s;
  for(s=landmarkSet->begin();s!=landmarkSet->end();s++)
  {
    std::vector<symList*>* elementVec=new std::vector<symList*>;
    Matrix->insert(pair<symList*, vec2*>(new symList(O[0]),elementVec ));
  }
  symList2Vec2Map::iterator vv=Matrix->begin();
  
  


  for(int pos=0;pos<size-1;pos+=40) 
  	{  
    		symList *currData= new symList();
		for(int pos1=pos;pos1<pos+40;pos1+=2)
		{
		
			currData->append( new symList(Data->at(pos1), new symList(Data->at(pos1+1))) ); 

  		}
		for(vv=Matrix->begin();vv!=Matrix->end(); vv++)
		{
            
        	       	vv->second->push_back(currData);
		      	Matrix->insert(pair<symList*, vec2*>(vv->first,vv->second ));
               
   		 }


	
  	}		


  
   
 


  for(s=landmarkSet->begin();s!=landmarkSet->end();s++)
  {
    delete *s;
  }
  delete landmarkSet;      

  
  return Matrix;
}







symList2IntMap *SPECTRAL::getHistcout(symList2Vec2Map *divideData,int CUTOFF,string txtname)
{
    ofstream TXTN(txtname.c_str());  
    symList2Vec2Map::iterator vi=divideData->begin();
   
    symList2IntMap *Histcout=new symList2IntMap;
    for(vi=divideData->begin();vi!=divideData->end(); vi++)
    {        
       for(std::vector<symList*>::iterator begvec = vi->second->begin();begvec != vi->second->end();begvec++)
       { 

          symList* datavec = new symList(*begvec);

          symList* watchedHist0= new symList();
          if(Histcout->find(watchedHist0) == Histcout->end())
          {
            Histcout->insert(pair<symList*, int*>(new symList(watchedHist0), new int(1)));
          }
          else *(Histcout->find(watchedHist0)->second) += 1;
         delete watchedHist0;

         symList*watchedHist=new symList();

         int n=(*begvec)->size();
         for(int i=1; i<n; i+=2)               
         {
            symList *watchedHist1= getHistoryL(datavec, i, 1); 
 	    watchedHist->append(new symList(watchedHist1));
            delete watchedHist1;

            if(Histcout->find(watchedHist) == Histcout->end())
            {
              Histcout->insert(pair<symList*, int*>(new symList(watchedHist), new int(1)));
            }
            else *(Histcout->find(watchedHist)->second) += 1;
         }  
         delete watchedHist;
         delete datavec;
       } 
     }

  symList2IntMap::iterator histIterator,tempHistIterator;

       for(histIterator=Histcout->begin(); histIterator!=Histcout->end();)
       {
          if( *(histIterator->second) < CUTOFF)        
          {
            tempHistIterator = histIterator++;
            delete tempHistIterator->first;
            delete tempHistIterator->second;
            Histcout->erase(tempHistIterator);
         }
         else
         {
           ++histIterator;
         }
      }
  for( histIterator=Histcout->begin(); histIterator!=Histcout->end();histIterator++)
     {
      TXTN<<*(histIterator->first)<<" "<<*(histIterator->second)<<endl;
     }

  return Histcout;
}



symList* SPECTRAL::getHistoryL(symList* data, int pos, int len) 
{
  symList* currHistory = new symList();        
  if( ((pos+1)/2) >= len)                 
  {
    pos += 1;                           
    int startH = pos - len*2;              
    for(int i=0; i<startH; i++)
    data=data->n;                         
    
    for(int i=0; i<len*2; i++)
    {
      currHistory->append(data->s) ;
      data=data->n;                        
    }
   return currHistory;                      
  }
  else                                     
    return NULL;

}






//P(H)
Array1D<double>* SPECTRAL::getPH(symList2IntMap *Histcout)
{ 
  Array1D<double> *PH=new Array1D<double>(Histcout->size());
  double NUM=0.0;

  symList2IntMap::iterator hc;
  for(hc=Histcout->begin();hc!=Histcout->end();hc++)
     {
      NUM+=*(hc->second);
     }
  cout<<"history NUM="<<NUM<<endl;

  ofstream phTXT("PH.txt");
  int i;
  for(i=0,hc=Histcout->begin();(hc!=Histcout->end());hc++)
     {
	  (*PH)[i]=*(hc->second)/NUM;
	  i++;
      phTXT<<(*(hc->second))/NUM<<"  ";
     }	  
  phTXT<<endl;   
  return PH;	            
}



//p(t|h) flag=1--P(T|H) flag=2--P(AO|H) flag=3--P(AOT|H)
Array2D<double>*SPECTRAL::getProb( symSet* interestTests,std::vector<float>*belief,int action_N,symList2IntMap *Histcout,int flag)
{	
  
  symSet*augAct= getaugAct(interestTests);
  symSet::iterator iCurr;
  int h_len=Histcout->size();
  int size_m=h_len;
  int size_n=interestTests->size();
  Array2D<double>*ctProb  = new Array2D<double>(size_n, size_m);
  *ctProb=0.0;

  symIter s;
  symList2DblMap::iterator rowDblMapite;
  symList2DblMap*rowDblMap = new symList2DblMap();
  for(s = interestTests->begin(); (s != interestTests->end()); s++)
     {
      rowDblMap->insert(pair<symList*, double*>(new symList(*s), new double(0.0)));
     }

  int i,j;
  symList2IntMap::iterator histIterator;
  for(i=0, histIterator=Histcout->begin();( histIterator!=Histcout->end())&&(i<size_m);histIterator++,i++)
     {
	
	   h_len--;

	   symList* historyL=new symList(histIterator->first);
	   std::vector<float>* newbelief=new std::vector<float>;
	   if(historyL->size()==1)
	     {  				 
		  std::vector<float>::iterator is;
 		  for(is=belief->begin();is!=belief->end();is++) 
 			  newbelief->push_back(*is);
		 }
	   else
		 {	
		  std::vector<Symbol*> *history=getVecdata(historyL);
		  
		  newbelief=getBeliefState(belief ,history);
		  delete history;	
		 }
	
		


      for(iCurr=augAct->begin();iCurr!=augAct->end();iCurr++)
		 {  			 
		  int count_a=0;
		  int testTen=(*iCurr)->size();
	      while(count_a<action_N)
			   { 
				resert(newbelief);
				symList* testao= getData2(testTen,*iCurr);	

                rowDblMapite=rowDblMap->find(testao);                      
                if(rowDblMapite!= rowDblMap->end())
		          *(rowDblMapite->second)+=1;	

		        delete testao;
				count_a++;
               }  
  		
		 }

      delete newbelief;
      newbelief=NULL;	
      delete  historyL;
      double denom =  action_N;
      for(j=0,rowDblMapite=rowDblMap->begin();(rowDblMapite!=rowDblMap->end())&&(j<size_n);rowDblMapite++,j++)
         {
          *(rowDblMapite->second) = *(rowDblMapite->second)/denom;
          (*ctProb)[j][i]=*(rowDblMapite->second);
          *(rowDblMapite->second)=0.0;
	 }  	 
     }

  for(rowDblMapite=rowDblMap->begin();(rowDblMapite!=rowDblMap->end());rowDblMapite++)
     {	
      delete rowDblMapite->first;delete rowDblMapite->second;
     } 
  delete rowDblMap;
  rowDblMap=NULL;
  Relize_symSet(augAct);
  Relize_symSet(interestTests);
  string TXTNAME;
  if(flag==1)
  {
	  TXTNAME="probT.txt";
  }
  else if(flag==2)
  {
	  TXTNAME="probAO.txt";
  }
  else if(flag==3)
  {
	  TXTNAME="probAOT.txt";
  }
  ofstream prob(TXTNAME);
  for(i=0;i<size_n;i++)
     {
      for(j=0;j<size_m;j++)
         {
          prob<<(*ctProb)[i][j]<<" ";
         }
      prob<<endl;
     }

  return ctProb;
}



Array3D<double>*SPECTRAL::Array2TO3(Array2D<double>* PAOTH, int AOsize, int Tsize, int Hsize)
{
    Array3D<double>* AOTHProb=new Array3D<double>(AOsize,Tsize,Hsize);
    for(int ao=0;ao<AOsize;ao++)
    {
		for(int t=0;t<Tsize;t++)
		{
		    for(int h=0;h<Hsize;h++)
			{
				(*AOTHProb)[ao][t][h]=(*PAOTH)[ao*Tsize+t][h];
			}
		}

    }

	return AOTHProb;
}




std::vector<float> *SPECTRAL::getBeliefState(vector<float>*belief ,vector<Symbol*> *var)
{ 
  int a,o,i,j;  
  int size=var->size();                             
  std::vector<float>* belief_end=new std::vector<float>;      
  std::vector<float>* belief_start= new std::vector<float>;   
  std::vector<float>::iterator it,is;
  for(i=0;i<S_sz;i++)                               
 	{belief_end->push_back(0.0);}
  for(is=belief->begin();is!=belief->end();is++)    
 	{belief_start->push_back(*is);}

  for(int pos=0;pos<size;)
  { 
         symList *act = getMemory(var, pos, 1);pos++;
         a=act->s->id;                             
         symList *obs=getMemory(var,pos,1);pos++;
         o=obs->s->id;                            
         delete act;
         delete obs;

         for( i=0,it=belief_end->begin();it!=belief_end->end(),i<S_sz;it++,i++)
	 {	
              for( j=0,is=belief_start->begin();is!=belief_start->end();j++,is++)
	      {
                *it=*is*(Tr[a][j][i])*(Ob[a][i][o])+*it;
              }
         }                                       



         float normal=0.0;                       
         for(it=belief_end->begin();it!=belief_end->end();it++)
		{ normal=normal+(*it);}
	 for(it=belief_end->begin();it!=belief_end->end();it++)
		 {*it=(*it)/normal;
		
			}
	 if (pos==size)                         
	 { 
              	break;
         }
	 else                                   
	 {
                 belief_start->clear();
        	 for(is=belief_end->begin();is!=belief_end->end();is++)
 		   {belief_start->push_back(*is);}
	 	 for(it=belief_end->begin();it!=belief_end->end();it++)
 		    {*it=0.0;}	
        }
  }
  delete belief_start;
  return belief_end;
  
}
//set belief state
void SPECTRAL:: resert(std::vector<float>*belief)
{
 std::vector<float>::iterator vf;

 int i=0,j=0;
 float probtemp=0.0;
 float prob = (float) random() * 1.0 / (float) RAND_MAX; 

 for(i=0,vf=belief->begin();vf!=belief->end();vf++,i++)
 {
	probtemp=probtemp+*vf;
	if(probtemp>prob)
	{
		j=i;
		break;
	}
		
 
 }
 currState = S[j];
 prevState = NULL;
}




symSet*SPECTRAL:: getaugAct(symSet* baseTest)
{
  symIter iCurr1;
  symSet  *augAct = new symSet();
  for(iCurr1 = baseTest->begin(); iCurr1 != baseTest->end(); iCurr1++)
     {
      symList *aug = (*iCurr1)->filter(SYM_ACTION);
      if(augAct->find(aug)==augAct->end())
        {
         augAct->insert(new symList(aug));
        }
      delete aug;  
     }     
  return augAct;
}



std::vector<Symbol*>*  SPECTRAL::getVecdata(symList* var)
{
  std::vector<Symbol*>* testData=new std::vector<Symbol*>;
  Symbol* mark;
  int size_var=var->size();
  for(int i=0; i<size_var; i++)          
    {
      mark=var->s;
      testData->push_back( mark) ;
      var=var->n;
     }
  return testData;
}



//perform a specified sequence of actions 
symList*  SPECTRAL::getData2(int Len,symList*actList)
{ 
  symList* continueData=new symList();
  Symbol *actionToSend, *obsReceived;
  for(int i=0; i<Len; i++)               
  {
    actionToSend = actList->s;          
    continueData->append(actionToSend);
    obsReceived = sendAction1(actionToSend); 
    continueData->append(obsReceived);   
    actList=actList->n;
  }
  return continueData;
}




Symbol *SPECTRAL::getAction(int i){return i<A_sz?A[i]:NULL;}


Symbol *SPECTRAL::getObservation(int i){return i<O_sz?O[i]:NULL;}






int SPECTRAL::nrActions() { return A_sz; }



int SPECTRAL::nrObservs() { return O_sz; }


symList *SPECTRAL::getActions() 
{
  int i;
  symList *toret = NULL;
	
  for (i=A_sz-1; i>=0; i--) toret = new symList(A[i], toret);
  return toret;
}


symList *SPECTRAL::getObservations() 
{
  int i;
  symList *toret = NULL;
	
  for (i=O_sz-1; i>=0; i--) toret = new symList(O[i], toret);
  return toret;
}



symSet *SPECTRAL::getAOset()
 { 
  ofstream aotxt("AOAll.txt");
  symSet *AOset = new symSet();
  symList *actlist = getActions(), *obslist = getObservations();
  symList *curract,*currobs;
  curract = actlist;       
  while((curract != NULL) && (curract->s != NULL)) 
       { 
        currobs = obslist;     
        while((currobs != NULL) && (currobs->s != NULL)) 
             { 
              symList *Test = new symList(curract->s, new symList(currobs->s));
              if(AOset->find(Test) == AOset->end())
	              {AOset->insert(Test);aotxt<<*Test<<endl;}
              else
	              delete Test;
              currobs = currobs->n;
             } 
        curract = curract->n;
       } 
  delete actlist;
  delete obslist;
  return AOset;
 }




symSet *SPECTRAL::getAOsethappendbyHistcout(symList2IntMap* Histcout)
{
	ofstream aohistxt("AOhappendbyHistcout.txt");
	symSet *AOset = new symSet();
	symList2IntMap::iterator sc;
	for(sc=Histcout->begin();sc!=Histcout->end();sc++)
	{
		for(int pos=1;pos<sc->first->size();pos+=2)
		{
			symList *Test= getHistoryL(sc->first, pos, 1);
			if(AOset->find(Test) == AOset->end())
	             	{
				AOset->insert(Test);
			}
              		else
			{
				delete Test;
			}
	              	
		}

	}
	for(symIter t=AOset->begin();t!=AOset->end();t++)
	{
		symList *AOTest=new symList();
		AOTest->append(new symList(*t));
		aohistxt<<*AOTest<<endl;
	}
	return AOset;

}






symSet *SPECTRAL::getAOQset(symSet* AOset,symSet* Testset)
 { 
  ofstream aoqtxt("AOQ.txt");
  symSet *AOQset = new symSet();
  for(symIter s=AOset->begin();s!=AOset->end();s++)
    {
	for(symIter t=Testset->begin();t!=Testset->end();t++)
	{
		symList *AOTest=new symList();
		AOTest->append(new symList(*s));
		AOTest->append(new symList(*t));
		AOQset->insert(AOTest);
		aoqtxt<<*AOTest<<endl;
	}
    }
  return AOQset;
 }




void SPECTRAL::Relize_symSet(symSet*symSetata)
{
  symIter s;
  for(s=symSetata->begin();s!=symSetata->end();s++)
   {
    delete *s;
   }
  delete symSetata;
  symSetata=NULL;
}


//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
