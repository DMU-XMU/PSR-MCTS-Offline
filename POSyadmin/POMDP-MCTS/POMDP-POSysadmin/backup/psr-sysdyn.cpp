#ifndef __PSR_SYSDYN_CPP_
#define __PSR_SYSDYN_CPP_

#include <fstream>
#include <iostream>

#include "psr-sysdyn.h"

using namespace std;
#define TEST_LENGTH 1000000                             //�������ݵĳ���
//using namespace Sampler;

//typedef map<int, double*> int2DblMap;
void psr_sysdyn::learn() {
  // ---------------
  // -  DISCOVERY  -
  // ---------------
  ofstream saveData  ("saveData.txt");                  //����ӵģ�Ŀ����Ϊ�˽���ȡ�������ݱ����� saveData.txt ��
  ofstream saveStep  ("steps.txt");
  ofstream paraData      ("data.txt");                   //���ѧϰ�ú�ĸ�������
  ofstream stateData ("stateData.txt");
  ofstream resultData("resultData.txt");

  symSet *tests, *lastTests = new symSet(), *histories, *augHistories, *eraseTest = new symSet(), *tempHistories = new symSet(), *tempAugHistories = new symSet();

  int k=0, oldK=0, testk=0, oldRowK=0, rowK=1;        //ע�� rowK �� 1
  int columnNum, actCol;
  
  state_size = 11;                                    //��� cheese maze ����, ���� 11 ��״̬

  Matrix<double> *matrix_mao;                         //���� mao �ľ���
  int* columnsToRem;
  Array2D<int> *actcounts, *tempActcounts;
  
  tEstimateData *P;                                   //oldP ���������ȥ��
  vector<Symbol*>* conData;
  symSet* landmarkSet;
  //����Ķ�����Ϊ�������ݲ��ֶ������
  Array1D<double> *currState = new Array1D<double>(state_size, 0.0);   //POMDP ģ�͵ĵ�ǰ״̬
  Array1D<double> *currProb  = new Array1D<double>(state_size, 0.0);   //���� POMDP ģ�͸��ʼ���ľ���
  Array1D<double> *tempProb  = new Array1D<double>(state_size, 0.0);   //���� POMDP ģ�͸��ʼ���ľ���
  Array3D<double> *stateTran = new Array3D<double>(env->nrActions(), state_size, state_size, 0.0); //״̬ת�ƾ���
  Array2D<double> *obsTran   = new Array2D<double>(state_size, env->nrObservs(), 0.0);//�۲����,ע����Ϊ2ά,������Ϊ obs
  Array2D<double> *probsD,                            //��ʾ���� p(Qt/Qh) ����
  inverse_probsD;                                     //��ʾ���� p(Qt/Qh) ������
  Array2D<double> *statesProb;                        //��ŵ���ÿһ�� state-history ��Ӧ�� core-test ��ֵ
  Array1D<double> *firstProb;                         //��ų�ʼ״̬�� core-test ��ֵ,
  
  symSet *qtests;                                     //��Ӧ����Q tests
  symList2VecMap *maoMatrix;
  symList2VecMap* rowDblMapQ;
  symListVecMat *maoqMatrix; 
  vector<double> *elementVec;
  double sum, sumq, temp, tempP;

  Array1D<int>*    rowZero    = new Array1D<int>(size_n, 0);		
  Array1D<double>* rowDblZero = new Array1D<double>(size_n, 0.0);
  symList2IntMap *rowMap;    
  symList2DblMap *rowDblMap;
  row    = &(*rowZero)[0];
  rowDbl = &(*rowDblZero)[0];
  int  nrrows, nrcols; 
  initPomdp(stateTran, obsTran);                      //��ʼ�� pomdp
  //�˴���һѭ�����,ͬʱ�� continueData ��ʼ��,��ԭ���ڹ��캯����ĳ�ʼ��ȥ��,��ѭ������� continueData ɾ��
  while(iterations <= TEST_LENGTH)                    //�� 1000 ��ʼ,ÿ�ε���1000,��˸�ʽ��ʾȡ 100 ����
  {
    saveData<<"Before continueData intia"<<endl;
    continueData = new vector<Symbol*>;
    continueData->reserve(iterations+100);
    conData = getData(iterations);                     //�����˸ı�
    //cout<<"end of getData"<<endl;
    saveData << "\n\n === new iteration === \n\n";
    augHistories = new symSet();
  
    //��Թ̶����ȵľ����� tests ��Ϊ�ռ���ͬʱ�� sampler.cpp �й��� histLength ���ȵ� test
    tests = new symSet();
    tests->insert(new symList());
    //cout<<"Before break"<<endl;
    //break;
    //testLength = 2;
    cout<<"Before augEstimate"<<endl;
    P = augEstimate(tests, env, conData, iterations, histLength, testLength, true, NULL); 
    cout<<"Before landmarkSet"<<endl;
    landmarkSet = getLandmark(P, 1);                  //����� 1 �������Ҫ���¿���һ��
    cout<<"landmark set is "<<endl;
    for(iCurr=landmarkSet->begin(); iCurr!=landmarkSet->end(); iCurr++)
      cout<<**iCurr<<endl;
    freeData(P);  
    tests->clear();
    tests->insert(new symList());
    histories = new symSet();
    augTest   = new symSet();
    eraseHistories = new symSet();
    testLength = 0;
    cout<<"Before while"<<endl;
    //break;//��ʱ���
    while(1)
    {
      //�ڻ�ȡ landmark �󣬻�þ�������һ����չ
      testLength++;
      P = augEstimate(tests, env, conData, iterations, histLength, testLength, false,landmarkSet); 
      oldK = k;
      k = getRank(P);
      /**************************************************************************
       *****************************�е��Ƴ�����*********************************
       **************************************************************************/
      histories->clear();
      for(iH=P->ctMat->begin(); iH!=P->ctMat->end(); iH++)
	histories->insert(new symList(iH->first));
      augTest->clear();
      iH = P->ctMat->begin();
      for(iT=iH->second->begin(); iT!=iH->second->end(); iT++)
	augTest->insert(new symList(iT->first));
      ctMatP  = new symListMatrix();
      ctProbP = new symListDblMat();
      size_m = P->ctMat->size();
      size_n = P->ctMat->begin()->second->size();
      for(iCurr=histories->begin(); iCurr!=histories->end(); iCurr++)
      {
	rowMap = new symList2IntMap();
	for(iCurr2=augTest->begin(); iCurr2!=augTest->end(); iCurr2++)
	  rowMap->insert(pair<symList*, int*>(new symList(*iCurr2), row++));
	ctMatP->insert(pair<symList*, symList2IntMap*>(new symList(*iCurr), rowMap));
	rowDblMap = new symList2DblMap();
	for(iCurr2=augTest->begin(); iCurr2!=augTest->end(); iCurr2++)
	  rowDblMap->insert(pair<symList*, double*>(new symList(*iCurr2), rowDbl++));
	ctProbP->insert(pair<symList*, symList2DblMap*>(new symList(*iCurr), rowDblMap));
      }
      for(iH=P->ctMat->begin(), piH=P->ctProb->begin(); iH!=P->ctMat->end(), piH!=P->ctProb->end(); iH++, piH++)
	for(iT=iH->second->begin(), piT=piH->second->begin(); iT!=iH->second->end(), piT!=piH->second->end(); iT++, piT++)
	{
	  *(ctMatP->find(iH->first)->second->find(iT->first)->second) = *iT->second;
	  *(ctProbP->find(piH->first)->second->find(piT->first)->second) = *piT->second;
	}

      //����� ctMatP �� ctProbP ����Ķ���ĳһ�У�������Ӧ�����е�Ԫ�ض�Ϊ�㣬����ɾȥ
      for (iCurr = augTest->begin(); iCurr != augTest->end(); iCurr++)
      {
	zeroColumn = true;
	for (piH = ctProbP->begin(); piH != ctProbP->end();  piH++)  
	  if(*(piH->second->find(*iCurr)->second))
	    zeroColumn = false;
	if(zeroColumn)
	  for (piH = ctProbP->begin(),iH = ctMatP->begin(); piH != ctProbP->end(), iH != ctMatP->end();  piH++, iH++)
	  {
	    piH->second->erase(*iCurr);
	    iH->second->erase(*iCurr);
	  
	  }
      }
      iH = ctMatP->begin();
      size_n = iH->second->size();
      probsP  = new Array2D<double>(size_m, size_n);     //��Ӧ����
      countsP = new Array2D<int>(size_m, size_n);        //��Ӧ��ֵ
      for(i=0, piH=ctProbP->begin(), iH = ctMatP->begin(); piH != ctProbP->end(), iH != ctMatP->end(); i++, piH++, iH++)
	for(j=0, piT = piH->second->begin(), iT = iH->second->begin(); piT != piH->second->end(), iT != iH->second->end(); j++, piT++, iT++)
	{
	  (*probsP)[i][j]  = *(piT->second);
	  (*countsP)[i][j] = *(iT->second);
	}
      tests->clear();
      eraseTest->clear();
      iH=ctMatP->begin();
      for (iT = iH->second->begin(); iT != iH->second->end(); iT++)
      {
	tests->insert(iT->first);                       //test �д�ŵ��ǽ���������ɾ����ļ���,�� augTest ��ͬ
	eraseTest->insert(iT->first);
      }
      int     rowRemIndex, columnRemIndex, leftRow, leftColumn;
      int     bestValue, tempValue;
      int     bvIndex       = -1;
      int     probRowSize, probColumnSize;
      Array2D<double> *probLeft;                      //��ŵ���������Ƴ��������
      int *columnsToRem = (int*)malloc(size_n * sizeof(int)); //�е��Ƴ�
      tiH = P->ctAct->begin();
      size_act = tiH->second->size();
      actcounts     = new Array2D<int>(size_m, size_act);
      tempActcounts = new Array2D<int>(size_m, size_act);
      for(i=0; i<size_m; i++)                         //�����������ĳ�ʼ��
	for(j=0; j<size_act; j++)
	  (*actcounts)[i][j]  =(*tempActcounts)[i][j] = (*P->actcounts)[i][j];
      for(i = 0; i<size_n; i++)                       //����ط������Ĺ����ǽ����ʼ��Ϊ����Ӧ���е�����
	columnsToRem[i] = i;                         //�Ƚ������ʼ����Ԫ�ذ��մ����Ұ���С�����˳������       
      //double cond, bestcond=DBL_MAX;
      rowRemIndex = 0;
      leftRow = leftColumn = probRowSize = 0;
      probColumnSize    = size_n -1;
      columnRemIndex    = size_n -1;
      if(size_n > k)
      {
	saveData<<"Now column removing"<<endl;
	int swapCol;
	while(columnRemIndex > -1){                     //���������ǽ����ᵼ�¾�����ȼ��ٵ�����ȥ 
	  probLeft = new Array2D<double>(size_m, probColumnSize);
	  leftRow = 0;
	  columnsToRem[columnRemIndex] = -1;
	  for(i=0, iH = ctMatP->begin(); iH != ctMatP->end(); i++, iH++)  //��ȡ probLeft ����ֵ
	  {
	    tH = iH;
	    leftColumn     = 0;
	    for(j=0, iT = tH->second->begin(); iT != tH->second->end(); j++, iT++)
	    {
	      if(j == columnRemIndex)                   //��ҪĿ���ǽ���ǰ�Ƴ��������Ӧ�Ķ���������������
	      {
		del = iT->first->filter(SYM_ACTION); 
		TH = (double) (*countsP)[i][j];
		for(actCol = 0, tiT = tiH->second->begin(); tiT != tiH->second->end(); actCol++, tiT++) 
		{	      
		  if(*del == *tiT->first)        
		  {
		    THa = (double) (*actcounts)[i][actCol];
		    (*tempActcounts)[i][actCol] = (int)(THa-TH);  
		    swapCol = actCol;
		    break;
		  }
		}
		delete del;
	      }
	      else if( columnsToRem[j] == -1 )               
		continue;
	      else
		(*probLeft)[i][leftColumn++] = (*probsP)[i][j];
	    }
	    //leftRow++;
	  }
	
	  epsilon = 0.0;                                  //���������Ĺ����ǻ�ȡ����Ӧ�����Ƴ���� rank                    
	  for (i = 0, iH = ctMatP->begin(); iH != ctMatP->end(); i++, iH++) {  
	    for (j = 0, iT = iH->second->begin(); iT != iH->second->end(); j++, iT++) {
	      if(columnsToRem[j] == j)                  
	      {
		del = iT->first->filter(SYM_ACTION); 
		TH = (double) (*countsP)[i][j];
		for(actCol = 0, tiT = tiH->second->begin(); tiT != tiH->second->end(); actCol++, tiT++)
		  if(*del == *tiT->first)        //��ô��û����ȵ����
		  {
		    THa = (double) (*tempActcounts)[i][actCol];
		    break;
		  }
		delete del;
		if (TH==0 || THa==0) continue;
		epsilon += sqrt(((TH/THa)*(1-(TH/THa)))/(THa*(1-CONFIDENCE)));
	      }
	    }
	  }
	  epsilon /= (probLeft->dim1() * probLeft->dim2());  //�����Ҫ�� epsilon ��ֵ
	  norm = infNorm(probLeft);                            //�õ� norm ֵ
	  singular = new SVD<double>(*probLeft);              
	  Array1D<double> *singvals = new Array1D<double>();
	  singular->getSingularValues(*singvals);
	  delete singvals;
	  testk = singular->rank(epsilon*norm);         //���������жϽ���Ӧ���к����Ƴ���õ��� rank �Ƿ��ԭ�������
	  if(testk < k)                                 //������ȷ����˱仯�����䱣��
	  {
	    for (i=0, iH=ctMatP->begin(); iH!=ctMatP->end(); i++, iH++) 
	      (*tempActcounts)[i][swapCol] = (*actcounts)[i][swapCol];
	    columnsToRem[columnRemIndex] = columnRemIndex;
	  }
	  else
	  {
	    probColumnSize--;                           //��ʾ�����ֽ�һ���Ƴ�
	    for (i=0, iH=ctMatP->begin(); iH!=ctMatP->end(); i++, iH++)   
	      (*actcounts)[i][swapCol] = (*tempActcounts)[i][swapCol];
	  }
	  delete probLeft;
	  delete(singular);
	  columnRemIndex--;
	}
	saveData<<"After column removing, the column index is"<<endl;
	for(i =0; i<size_n; i++)
	  saveData<<columnsToRem[i]<<" ";
	saveData<<endl;
	for (i=0, iCurr=eraseTest->begin(); iCurr!=eraseTest->end(); i++, iCurr++) 
	  if (columnsToRem[i] == -1) 
	  {
	    tests->erase(*iCurr);
	    for(iH=ctMatP->begin(), piH=ctProbP->begin(); iH!=ctMatP->end(), piH!=ctProbP->end();iH++, piH++)
	    {
	      //del = iH->second->find(*iCurr)->second;
	      iH->second->erase(*iCurr);
	      //delete del;
	      //del = piH->second->find(*iCurr)->second;
	      piH->second->erase(*iCurr);
	      //delete del;
	    }
	  }
      }
//#if 0
      delete probsP;
      delete countsP;
      delete ctMatP;
      delete ctProbP;
//#endif    

      if(k != oldK)                                     //ֻ�е���Ҫѭ��ʱ���Ž���test��ӣ���һ�����㷨��Ҫ��Ҫ?
      {
	freeData(P);                                    //�����         
	delete actcounts;                               //�����
	delete tempActcounts;                           //�����
	lastTests->clear();
	for(iCurr=tests->begin(); iCurr!=tests->end(); iCurr++)
	  lastTests->insert(new symList(*iCurr));
	tests->insert(new symList());          
	saveData << "Tests:\n";
	for (iCurr = tests->begin(); iCurr != tests->end(); iCurr++) saveData << "[" << **iCurr << "]\n";
      }
      else                                              //����ط��ǵ� k==oldk ʱ�����
	break;
    }
    tests->clear();
    delete tests;
    free(columnsToRem);
    saveData<<"Now the discover part is done"<<endl;

    /***************************************************************************************************************
     *****************                           ѧϰ����(8.24)            *******************************************
     *****************************************************************************************************************/
    tiH = P->ctAct->begin();                            
    size_act = tiH->second->size();
    size_m = P->ctMat->size();
    size_n = lastTests->size();
    saveData<<"Row size: "<<size_m<<" col size: "<<size_n<<" k " <<k<<endl;
    //delete probsP;
    //delete countsP;
    probsP  = new Array2D<double>(size_m, size_n);     
    countsP = new Array2D<int>(size_m, size_n);     
    //delete ctMatP;
    //delete ctProbP;
    ctMatP  = new symListMatrix();                      //���к�P�ж���һ��,��Ϊ lastTests ��Ӧ֮��
    ctProbP = new symListDblMat();                      //ͬ��
    //���³�ʼ�� ctMatP, ctProbP
    for(piH=P->ctProb->begin(), iH = P->ctMat->begin(); piH!=P->ctProb->end(), iH != P->ctMat->end(); piH++, iH++)
    {
      rowDblMap = new symList2DblMap();           
      rowMap    = new symList2IntMap();    
      for(iCurr=lastTests->begin(); iCurr!=lastTests->end(); iCurr++)
	for(iCurr2=augTest->begin(); iCurr2!=augTest->end(); iCurr2++)
	  if(**iCurr2 == **iCurr)
	  {
	    rowDblMap->insert(pair<symList*, double*>(new symList(*iCurr), new double(*(piH->second->find(*iCurr2)->second))));
	    rowMap->insert(pair<symList*, int*>(new symList(*iCurr), new int(*(iH->second->find(*iCurr2)->second))));
	    break;
	  }
      ctProbP->insert(pair<symList*, symList2DblMap*>(new symList(piH->first), rowDblMap));
      ctMatP->insert(pair<symList*, symList2IntMap*>(new symList(iH->first), rowMap));
    }

    for(i=0, piH=ctProbP->begin(), iH = ctMatP->begin(); piH != ctProbP->end(), iH != ctMatP->end(); i++, piH++, iH++)
      for(j=0, piT = piH->second->begin(), iT = iH->second->begin(); piT != piH->second->end(), iT != iH->second->end(); j++, piT++, iT++)
      {
	(*probsP)[i][j]  = *(piT->second);
	(*countsP)[i][j] = *(iT->second);
      }
    double cond, bestcond=DBL_MAX;
    int rowIndex, columnIndex, smlRow, smlCol;
    bool zeroRow;
    Array2D<double> *smlArr = new Array2D<double>(k, k),
    *IDENT = new Array2D<double>(k, k, 0.0),
    inverse;
    for (i=0; i<k; i++) (*IDENT)[i][i] = 1.0;
    double* probs = (double*)malloc(size_m*size_n*sizeof(double));
    double* dblRow;
    for(i=0; i<size_m; i++)
      for(j=0; j<size_n; j++)
	probs[i*size_n+j] = (*probsP)[i][j];
    int *rowToRem, *columnToRem, *bestRows, *bestCols;
    int remRowIndex = 0, remColumnIndex = 0;  
    saveData<<"Before sizem"<<endl;
    if(size_m > k)
    {
      rowToRem    = (int*)malloc((size_m-k)*sizeof(int));
      bestRows    = (int *) malloc((size_m-k)*sizeof(int)); //bestRows ��ʾ�������ո��������������Ƴ�������
      rowToRem[0] = -1;
    }
    else                                                //ֻ���Զ�����һ�Σ���Ϊ�˺�����ͳһ����Ҫ
    {
      rowToRem = (int*)malloc(1*sizeof(int));
      rowToRem[0] = -2;
    }
    if(size_n > k)
    {
      columnToRem    = (int*)malloc((size_n-k)*sizeof(int));
      bestCols       = (int *) malloc((size_n-k)*sizeof(int));
    }
    else
      columnToRem = (int*)malloc(1*sizeof(int));        //ͬ�еĶ���ԭ��һ��
    while(remRowIndex != -1)
    {
      rowToRem[remRowIndex]++;
      if(rowToRem[remRowIndex] == size_m)
      {
	remRowIndex--;
	continue;
      }
      remRowIndex++;
      if(remRowIndex == (size_m - k) || (size_m == k) )               //��ʾ�Ѿ���ʼ������ size_m-k ����
      {
	if(size_n > k)                              //�����
	  columnToRem[0] = -1;                   
	else                                        //�����
	  columnToRem[0] = -2;                      //�����
	remColumnIndex = 0;
	while(remColumnIndex != -1)
	{
	  columnToRem[remColumnIndex]++;
	  if(columnToRem[remColumnIndex] == size_n)
	  {
	    remColumnIndex--;
	    continue;
	  }
	  remColumnIndex++;
	  if(remColumnIndex == (size_n-k) || (size_n == k))
	  {
	    rowIndex = 0;
	    for (i=0; i<size_m; i++) 
	    {
	      if( (rowIndex<(size_m-k)) && (i==rowToRem[rowIndex]) ) 
		rowIndex++;
	      else 
	      {
		zeroRow = true;
		columnIndex=0;
		dblRow = &probs[i*size_n];
		for (j=0; j<size_n; j++)
		  if ( (columnIndex<(size_n-k)) && (j==columnToRem[columnIndex]) )
		    columnIndex++;
		  else if (dblRow[j]!=0) 
		    zeroRow=false;
		if (zeroRow) 
		  break;
	      }
	    }
	    if (zeroRow && ( (size_n-k)!=0 ))       //����ط�Ҫ��Ҫ���� size_n-k!=0 �����ж�
	    {
	      remColumnIndex--;
	      continue;
	    }
	  
	    // get everything that's not chosen for removal into an Array2D
	    //cout<<"Before removal in Array"<<endl;
	    rowIndex = 0;
	    smlRow = 0;
	    for (i=0; i<size_m; i++) 
	    {
	      if ( (rowIndex<(size_m-k)) && (i==rowToRem[rowIndex]) )
		rowIndex++;
	      else 
	      {
		smlCol = 0; columnIndex = 0;
		dblRow = &probs[i*size_n];
		for (j=0; j<size_n; j++) 
		{
		  if ( (columnIndex<(size_n-k)) && (j == columnToRem[columnIndex]) )
		    columnIndex++;
		  else
		    (*smlArr)[smlRow][smlCol++] = dblRow[j];
		}
		smlRow++;
	      }
	    }
	    for(i=0; i<size_m; i++)                         //�����������ĳ�ʼ��
	      for(j=0; j<size_act; j++)
		(*tempActcounts)[i][j]  =(*actcounts)[i][j];
//#if 0
	    rowIndex = 0;
	    for(i=0, iH = ctMatP->begin(); iH != ctMatP->end(); i++, iH++)  
	    {
	      if ( (rowIndex<(size_m-k)) && (i==rowToRem[rowIndex]) )
		rowIndex++;
	      else
	      {
		tH = iH;
		columnIndex = 0;
		for(j=0, iT = tH->second->begin(); iT != tH->second->end(); j++, iT++)
		  if ( (columnIndex<(size_n-k)) && (j == columnToRem[columnIndex]) )
		  {
		    columnIndex++;
		    del = iT->first->filter(SYM_ACTION); 
		    TH = (double) (*countsP)[i][j];
		    for(actCol = 0, tiT = tiH->second->begin(); tiT != tiH->second->end(); actCol++, tiT++) 
		    {	      
		      if(*del == *tiT->first)        
		      {
			THa = (double) (*actcounts)[i][actCol];
			(*tempActcounts)[i][actCol] = (int)(THa-TH);  
			break;
		      }
		    }
		    delete del;
		  }
	      }
	    }
	    //cout<<"Before epsilon 0"<<endl;
	    epsilon = 0.0;                          //��ȡ������
	    rowIndex = 0;

	    for (i = 0, iH = ctMatP->begin(); iH != ctMatP->end(); i++, iH++) 
	      if ( (rowIndex<(size_m-k)) && (i==rowToRem[rowIndex]) )
		rowIndex++;
	      else
	      {
		columnIndex = 0;
		for (j = 0, iT = iH->second->begin(); iT != iH->second->end(); j++, iT++) 
		  if ( (columnIndex<(size_n-k)) && (j == columnToRem[columnIndex]) )         
		    columnIndex++;
		  else
		  {
		    del = iT->first->filter(SYM_ACTION); 
		    TH = (double) (*countsP)[i][j];
		    for(actCol = 0, tiT = tiH->second->begin(); tiT != tiH->second->end(); actCol++, tiT++)
		      if(*del == *tiT->first)        
		      {
			THa = (double) (*tempActcounts)[i][actCol];
			break;
		      }
		    delete del;
		    if (TH==0 || THa==0) continue;
		    //saveData<<"TH "<<TH<<" THa"<<THa<<" i "<<i<<" actCol"<<actCol<<endl;
		    epsilon += sqrt(((TH/THa)*(1-(TH/THa)))/(THa*(1-CONFIDENCE)));
		  }
	      }
	  
	    //saveData<<"epsilon "<<epsilon<<endl;
	    epsilon /= (k * k);  
	    //saveData << "Got epsilon: "<<epsilon<<endl;
	    norm = infNorm(smlArr);                            //�õ� norm ֵ
	    //saveData << "Got norm: "<<norm<<endl;
	    //saveData << "  -> Cutoff value is "<<epsilon*norm<<endl; //�õ���ֵ Cutoff
	    singular = new SVD<double>(*smlArr);              
	    //saveData << "  == The singular values are: ";
	    Array1D<double> *singvals = new Array1D<double>();
	    singular->getSingularValues(*singvals);
	    //for (i=0; i<singvals->dim1(); i++) saveData << (*singvals)[i] << ", ";
	    //saveData << endl;
	    testk = singular->rank(epsilon*norm);   
	    if(testk < k)
	      cond = DBL_MAX;
	    else
	      cond = ((*singvals)[0])/((*singvals)[k-1]);
	    delete singvals;
//#endif
	    if (cond < bestcond) 
	    {
	      saveData << "   Better conditioned matrix found, bestcond = "<<cond<<endl;
	      bestcond = cond;
	      if(size_m > k)
		for(i=0; i<(size_m-k); i++)
		  bestRows[i] = rowToRem[i];
	      //memcpy(bestRows, rowToRem, (size_m-k)*sizeof(int));
	      saveData<<"Removing column "<<endl;
	      if(size_n > k)
		for(i=0; i<(size_n-k); i++)
		{
		  bestCols[i] = columnToRem[i];
		  saveData<<columnToRem[i]<<" ";
		}
	      saveData<<endl;
	      //memcpy(bestCols, columnToRem, (size_n-k)*sizeof(int));
	    }
	    if(size_n != k)
	      remColumnIndex--;
	    else 
	      break;
	  }
	  else
	    columnToRem[remColumnIndex] = columnToRem[remColumnIndex-1];
	}
	if(size_m != k)
	  remRowIndex--;
	else 
	  break;
      }
      else
	rowToRem[remRowIndex] = rowToRem[remRowIndex-1];
    }
    free(rowToRem);
    free(columnToRem);
    saveData<<"The removing element is "<<endl;
    if(size_n > k)
      for(i=0; i<(size_n-k); i++)
	saveData<<bestCols[i]<<" ";
    saveData<<endl;
    j = 0;
    if(size_n > k)                                      //��������Ĳ����� ctMatP �ж�Ӧ��Ԫ��Ϊ P(Qi|Sh),ShΪstate-history
      for (i=0, iCurr=lastTests->begin(); iCurr!=lastTests->end(); i++, iCurr++) 
      {
	if( (i==bestCols[j]))                          
	{
	  for(iH=ctMatP->begin(), piH=ctProbP->begin(); iH!=ctMatP->end(), piH!=ctProbP->end();iH++, piH++)
	  {
	    iH->second->erase(*iCurr);
	    piH->second->erase(*iCurr);
	  }
	  j++;
	  if( j==(size_n-k) )
	    break;
	}
      }
    saveData<<"Now column size "<<ctProbP->begin()->second->size()<<endl;
    free(bestCols);
    augTest->clear();
    delete augTest;
  
    lastTests->clear();
    delete lastTests;
    eraseTest->clear();
    delete eraseTest;
    delete probsP;
    delete countsP;
    delete smlArr;
    delete actcounts;
    delete tempActcounts;
    probsD = new Array2D<double>(k, k);                 //��ʾ���� p(Qt/Qh) ����
    statesProb = new Array2D<double>(size_m, k, 0.0);   //��ŵ���ÿһ�� state-history ��Ӧ�� core-test ��ֵ
    firstProb  = new Array1D<double>(k, 0.0);           //��ų�ʼ״̬�� core-test ��ֵ,
    smlRow = 0;
    piH = ctProbP->begin(); 
    for(i=0, piT=piH->second->begin(); piT!=piH->second->end(); i++, piT++)//��ʼ�� firstProb����������Ƴ��㷨���Ա�֤
      (*firstProb)[i] = *(piT->second);                                    //��һ�ж�Ӧ��Ϊ����ʷ�� core-test ��ֵ
    for(i=0, piH=ctProbP->begin(); piH!=ctProbP->end(); i++, piH++) //��ʼ�� stateMatrix
      for(j=0, piT=piH->second->begin(); piT!=piH->second->end(); j++, piT++)
	(*statesProb)[i][j] = *(piT->second);
    if(size_m > k)
      for(i=0, iCurr = augHistories->begin(), piH=ctProbP->begin(); iCurr != augHistories->end(), piH!=ctProbP->end(); i++, iCurr++, piH++)
	if(i == bestRows[i])
	{
	  P->ctMat->erase(*iCurr);                      //ʹ�� P->ctMat �ȶ�Ӧ���н�Ϊ Qh, ������ Sh
	  P->ctProb->erase(*iCurr);                     //ע�������ط���ʼ ctMatP �Ⱥ� P->ctMat ���п�ʼ��ͬ
	  P->ctAct->erase(*iCurr);                      //��Ӧ�� P->ctAct ����Ҫ��Ҫɾ��?
	  //continue; 
	}
	else
	{
	  for(j=0, piT=piH->second->begin(); piT!=piH->second->end(); j++, piT++)
	    (*probsD)[smlRow][j] = *(piT->second);
	  smlRow++;
	}
    else
      for(i=0, piH=ctProbP->begin(); piH!=ctProbP->end(); i++, piH++)
	for(j=0, piT=piH->second->begin(); piT!=piH->second->end(); j++, piT++)
	  (*probsD)[i][j] = *(piT->second);
    decomp = new LU<double>(*probsD);
    inverse_probsD = decomp->solve(*IDENT);
    delete(decomp);                                   //����ɾ���ŵ������
    delete IDENT;                                     //����ɾ���ŵ������
    //���probsD�� inverse_probsD ��ֵ
    //cout<<"probsD matrix"<<endl;
#if 0
    paraData<<"first history "<<endl;
    for(i=0; i<k; i++)
      paraData<<(*firstProb)[i]<<" ";
    paraData<<endl;
    paraData<<"probsD matrix"<<endl;
    for(i=0; i<k; i++)
    {
      //pata<<i<<" ";
      for(j=0; j<k; j++)
	paraData<<(*probsD)[i][j]<<" ";
      paraData<<endl;
    }
    paraData<<"inverse_probsD matrix"<<endl;
    for(i=0; i<k; i++)
    {
      //saveData<<i<<" ";
      for(j=0; j<k; j++)
	paraData<<(inverse_probsD)[i][j]<<" ";
      paraData<<endl;
    }
#endif
    //cout<<"Before tests"<<endl;
    tests = new symSet();
    symList	*actlist = env->getActions(), *curract,
    *obslist = env->getObservations(), *currobs,*aug, *aoTest;

    curract = actlist;     //tests �б���������һ���� ao        

    while ((curract != NULL) && (curract->s != NULL)) 
    { 
      currobs = obslist;     
   
      while ((currobs != NULL) && (currobs->s != NULL)) 
      { 
	aoTest = new symList(new Symbol(curract->s), new symList(new Symbol(currobs->s)));
	//aoTest->append(new symList((curract->s, new symList(currobs->s, NULL))));
	if(tests->find(aoTest) == tests->end())
	  tests->insert(aoTest);
	else
	  delete aoTest;
	currobs = currobs->n;
      } // obs
      curract = curract->n;
    } // act

    qtests = new symSet();                      //��Ӧ����Q tests
    iH = ctMatP->begin();
    for(iT=iH->second->begin(); iT!=iH->second->end(); iT++)
      qtests->insert(new symList(iT->first));

    maoMatrix = new symList2VecMap;
    //symList2VecMap* rowDblMapQ;
    maoqMatrix = new symListVecMat; 
    //vector<double> *elementVec;
    saveData<<"inverseProb "<<"piH "<<"temp "<<endl;
    for(iCurr=tests->begin(); iCurr!=tests->end(); iCurr++)           //��ʼ�� mao 
    {
      elementVec = new vector<double>;
      for(j=0; j<k; j++)
      {
	sum = 0.0;
	for(m=0, piH=P->ctProb->begin(); piH!=P->ctProb->end(); m++, piH++)
	{
	  tempP = *(piH->second->find(*iCurr)->second);
	  temp = (inverse_probsD)[j][m] * tempP;
	  saveData<<(inverse_probsD)[j][m]<<" "<<tempP<<" "<<temp<<endl;
	  if(temp != 0)
	    sum += temp;
	}
	saveData<<"sum "<<sum<<endl;
	if(fabs(sum) < 0.000001)
	  sum = 0.0;
	elementVec->push_back(sum);
      }
      maoMatrix->insert(pair<symList*, vec*>(new symList(*iCurr), elementVec));
    } 
    saveData<<"inverseProb "<<"piH "<<"temp "<<endl;
    for(i=0, iCurr=tests->begin(); iCurr!=tests->end(); i++, iCurr++) //��ʼ�� maoq
    {
      saveData<<**iCurr<<endl;
      rowDblMapQ = new symList2VecMap;
      for(iCurr2=qtests->begin(); iCurr2!=qtests->end(); iCurr2++)
      {
	//qElementDbl = (double*)malloc( k*sizeof(double) );
   
	elementVec = new vector<double>;
	aug = new symList(*iCurr);
	aug->append(new symList(*iCurr2));
	for(j=0; j<k; j++)
	{
	  sumq = 0.0;
	  for(m=0, piH=P->ctProb->begin(); piH!=P->ctProb->end(); m++, piH++)
	  {
	    tempP =  *(piH->second->find(aug)->second);
	    temp = (inverse_probsD)[j][m] * tempP; 
	    saveData<<(inverse_probsD)[j][m]<<" "<<tempP<<" "<<temp<<endl;
	    if(temp != 0)
	      sumq += temp;
	    //sumq += (inverse_probsD)[j][m] * ( *(piH->second->find(aug)->second) ); 
	  }
	  saveData<<"sumq "<<sumq<<endl;
	  if(fabs(sumq) < 0.000001)
	    sumq = 0.0;
	  elementVec->push_back(sumq);
	}
	saveData<<**iCurr2<<" ";
	for(j=0; j<k; j++)
	  saveData<<elementVec->at(j)<<" ";
	saveData<<endl;
	//rowDblMapQ->insert(pair<symList*, double*>(new symList(*iCurr2), qElementDbl));
	rowDblMapQ->insert(pair<symList*, vec*>(new symList(*iCurr2), elementVec));
	delete aug;
      }
      //maoqMatrix->insert(pair<symList*, symList2DblMap*>(new symList(*iCurr), rowDblMapQ));
      maoqMatrix->insert(pair<symList*, symList2VecMap*>(new symList(*iCurr), rowDblMapQ));
    }
    saveData<<"Learning part is done"<<endl;
#if 0
    //��� P->ctProb �е�ֵ
    piH = P->ctProb->begin();
    //saveData<<"P->ctProb col size "<<piH->second->size()<<" row size "<<P->ctProb->size()<<" element"<<endl;
    //���ctProbP�е�����
    paraData<<"ctProbP element"<<endl;
    piH = ctProbP->begin();
    for(piH=ctProbP->begin(); piH!=ctProbP->end(); piH++)
    {
      //paraData<<i<<" "<<*(piH->first)<<" ";
      for(j=0, piT=piH->second->begin(); piT!=piH->second->end(); j++, piT++)
	paraData<<*(piT->second)<<" ";
      paraData<<endl;
    }
    //���maoMatrix�е�����
    paraData<<"maoMatrix element"<<endl;
    for(viT=maoMatrix->begin(); viT!=maoMatrix->end(); viT++)
    {
      paraData<<*(viT->first)<<" ";
      for(j=0; j<k; j++)
	paraData<<viT->second->at(j)<<" ";
      paraData<<endl;
    }
    paraData<<"maoqMatrix element"<<endl;
    //for(i=0, viH=maoqMatrix->begin(); viH!=maoqMatrix->end(); i++, viH++)
    for(i=0,viH=maoqMatrix->begin(); viH!=maoqMatrix->end(); i++,viH++) 
    {
      paraData<<*(viH->first)<<endl;
      for(viT=viH->second->begin(); viT!=viH->second->end(); viT++)
      {
	paraData<<*(viT->first)<<" ";
	for(j=0; j<k; j++)
	  paraData<<viT->second->at(j)<<" ";
	paraData<<endl;
      
      }

    }
#endif
    viH=maoqMatrix->begin();

/****************************************************************************************
 ********************************���ݲ��Բ��� cheese maze,���ɷ�ת��Ϊһ������ **********
 ****************************************************************************************
 */ 
    //����Ĵ����ǽ����������ʼ��,�Ƿ�Ӧ������һ������?
    //intia
    int lmSize = (*landmarkSet->begin())->size();       //���� landmark �Ѷ�Ϊ������һ�µ�
    symList* startLandmark;
    startLandmark = getStartLandmark(conData,landmarkSet); 
//#if 0
    Symbol* actionToSend, *actionReceived;
    vector<Symbol*> *testData = new vector<Symbol*>;                      
    tempCoreValue = new Array1D<double>(k, 0.0);       //���ڴ�� p(Q|h)��ֵ,��ͬ
    coreValue = new Array1D<double>(k, 0.0);           //������������ͬ symSet ע���ڲ��Բ������Ҫɾ��,��Ϊ�滮����Ҳ��
  
    int prob, currPos, envState, belState, obsId;       //���� env_state ��ʾ���������嵱ǰ����״̬�� ID,belState �Ŷ�״̬
    double pomPre, psrPre, errSum, probSum, normal;                                //
    currPos = 1;
    while(1)
    {
      do
      {
	prob = (int) trunc((float) random() * (float) (env->nrActions()) / (float) RAND_MAX);
      } while (prob==env->nrActions()||(env->crumpus_open(prob)));    //crumpus_open ��������ȥ��
      actionToSend = env->getAction(prob);       
      testData->push_back(actionToSend);
      actionReceived = env->sendAction(actionToSend); 
      testData->push_back(actionReceived);
      if( *(getMemory(testData, currPos, lmSize)) == *startLandmark ) //��ʾ��ǰ�Ѿ������趨�ĳ�ʼ״̬��
      {
	envState = env->crumpus_stateid( );            //��ȡ��ǰ״̬ id,���� POMDP ģ��
	//��� envState �� belState ��ת������
	belState = getBelState(envState);
	break;     
      }                                     
      currPos += 2;                                     //��Ϊʼ��Ҫ�Թ۲�ֵΪ��ȡ�ĵ�
    }
    delete testData;
    for(i=0; i<state_size; i++)                       //��ʼ�Ŷ�״̬
      (*currState)[i] = 0.0;
    (*currState)[belState] = 1.0;                     //��ʼ�Ŷ�״̬
    for(i=0; i<k; i++)
      (*tempCoreValue)[i] = (*coreValue)[i] = (*firstProb)[i];
    errSum = 0.0;
    for(i=0; i<TEST_LENGTH; i++)                             //10000 ����Ϊһ����
    {
      do
      {
	prob = (int) trunc((float) random() * (float) (env->nrActions()) / (float) RAND_MAX);
      } while (prob==env->nrActions()||(env->crumpus_open(prob)));    //crumpus_open ��������ȥ��
      actionToSend = env->getAction(prob);     
      //���濪ʼ��ȡ��ǰ PSR ģ�ͺ;�ȷģ��Ԥ��֮��Ĳ�ֵ
      for(m=0; m<state_size; m++)                     //�õ� b(h)*T^a
      {
	probSum = 0.0;
	for(n=0; n<state_size; n++)
	  probSum += (*currState)[n] * (*stateTran)[prob][n][m];
	(*tempProb)[m] = probSum;
      }
      //��Ե������еĹ۲�ֵ,�ʴ˴�Ҫ��һ�� for ���
      nextAoSet = new symSet();
      currobs = obslist;                            //�˴��ĳ�ʼ���м�Ҫ���ϣ�Ҫ��Ȼ��һ��ѭ����ʱ�� currobs Ϊ NULL
      while ((currobs != NULL) && (currobs->s != NULL)) 
      { 
	nextAoSet->insert(new symList(actionToSend, new symList(new Symbol(currobs->s))));
	currobs = currobs->n;
      } // obs
      for(iCurr=nextAoSet->begin(); iCurr!=nextAoSet->end(); iCurr++)
      {
	obsId = (*iCurr)->n->s->id;                     //��ǰ�۲�ֵ�� ID ֵ
	for(m=0; m<state_size; m++)                     //�õ� b(h)*T^a*O^ao,���� O^ao ֻ�����Խ������з���ֵ������
	  (*currProb)[m] = (*tempProb)[m]*(*obsTran)[m][obsId];//ע�� obsTran ��Ӧ O,��ÿһ�ж�Ӧ O^o
	pomPre = 0.0;
	for(m=0; m<state_size; m++)                     //�õ���ǰ pomdp ģ�͵�Ԥ��ֵ����ֵ
	  pomPre +=  (*currProb)[m];
	//������� PSR ģ�͵�Ԥ��ֵ
	psrPre = 0.0;
	for(m=0; m<k; n++)                              //�õ� psr ģ�͵�Ԥ��ֵ
	  psrPre += ((*coreValue)[m] * (maoMatrix->find(*iCurr)->second->at(m)));
	if(psrPre != pomPre)                           //��ʾԤ��Ĳ���ͬ
	  errSum += pow( (psrPre-pomPre), 2 );
      }
      delete nextAoSet;
      actionReceived = env->sendAction(actionToSend); 
      //������Ҫ�����Ǹ��ݲ�ȡ�Ķ���ֵ�͵õ��Ĺ۲�ֵ���� POMDP ģ�ͺ� PSR ģ��
      normal = 0.0;
      obsId = actionToSend->id;
      for(m=0; m<state_size; m++)
      {
	(*currState)[m] = ( (*tempProb)[m]*(*obsTran)[m][obsId] ); //δ��һ����ֵ
	normal += (*currState)[m];                             //Ŀ�Ĺ�һ��
      }
      for(m=0; m<state_size; m++)
	(*currState)[m] /= normal;                             //��ȡ�����õ��۲�ֵ����Ŷ�״̬
    
      newAO = new symList(actionToSend, new symList(actionReceived, NULL));  //�ǵ�ɾ��
      denominator = 0;
      for(n=0; n<k; n++)
	denominator += ((*tempCoreValue)[n] * (maoMatrix->find(newAO)->second->at(n)));
      if(denominator == 0)                              //�˴��Ƿ�Ҫ�ӵ�Ԥ����ʩ
	for(m=0; m<k; m++)
	  (*coreValue)[m] = 0.0;
      else
      {
	for(m=0, viT=viH->second->begin(); viT!=viH->second->end(); m++, viT++) //��ȡ��ǰʱ�̵� core-test ��ֵ
	{
	  numerator   = 0;
	  for(n=0; n<k; n++)
	    numerator += (*tempCoreValue)[n] * (maoqMatrix->find(newAO)->second->find(viT->first)->second->at(n));
	  (*coreValue)[m] = (double)(numerator/denominator);
	  if( (*coreValue)[m] > 1 )
	    (*coreValue)[m] = 1;
	}
      }
      //tempCoreValue = coreValue->copy();              //�ɷ��ø�ʽ?copy ������õ�?
      for(m=0; m<k; m++)
	(*tempCoreValue)[m] = (*coreValue)[m];         
      delete newAO;
    
    }
    delete tempCoreValue;
    delete coreValue;
    errSum /= (env->nrObservs() * TEST_LENGTH);          //���� nrObservs Ϊ�۲�ֵ������,TEST_LENGTH Ϊ�������ݵĳ���
    resultData<<errSum<<endl;                            //�� resultData ���������ֵ
    //���ں���Ĺ滮��ʱ����,���Կ��Խ�����ı���ɾ��
    delete ctMatP;
    delete ctProbP;
    delete probsD;
    delete statesProb;
    delete firstProb;
    delete qtests;
    delete maoMatrix;
    delete maoqMatrix;
    free(P);                                            //�����201
    delete continueData;
    iterations += 1000;                                // while ����������
  }//�ô�Ӧ�ö�Ӧ��ǰ����Ǹ� while ���
  delete currState;
  delete currProb;
  delete tempProb;
  delete stateTran;
  delete obsTran;
  
//#endif  

#if 0                                                 //�Ƚ��滮����ȥ�� 2007.2.01
/****************************************************************************************
 *������Ĺ滮����, �����޸�λϵͳ,Ҫ�����趨,Ҫ�Ի�ȡ�Ľ���ֵ��Ϊ�жϵı�׼,Ҫ���޸�****
 ****************************************************************************************
 */ 
  int stateId, goalId, nextStateId, id;                   //��ʾ�����ڵ�״̬�������б𷨾�������ȷ�����ID��
  int actionId;                                       //��ʾ������ID
  int stateNum  = ctMatP->size();
  size_m = stateNum;
  int actionNum = NUM_ACTIONS;                        //�������Ķ������� 4,����Ĺ����пɽ���������Ϊһ���������
  Array2D<double> *Q = new Array2D<double>(stateNum, actionNum, 0.0);  //��ʼ�� Q ֵ
  Array2D<double> *e = new Array2D<double>(stateNum, actionNum, 0.0);
  *tempCoreValue = new Array1D<double>(k, 0.0);
  *coreValue = new Array1D<double>(k, 0.0);
  
  int  steps;                                         //��ʾ����ѵ���Ĵ���
  int  reward;                                        //��������ֵ��ÿǰ��һ������0�� �����յ����10
  double  delta, gamma=0.9, alpha=0.125, lambda=0.9;
  int     action, observation, tempAction;
  Symbol  *actionSend, *observationReceived, *lastObservation;                //���� observation ��ʾ���ǵõ��Ĺ۲�ֵ
  double sumNextQ, aoProb, eps;                      //���ӣ���ĸ
  double distance, bestDistance;
  symList *interact, *tempInteract;           //ԭ������� newAO �ŵ�ͷ�ļ��ж���
  //obslist = env->getObservations();                 //��ʱȥ�� 2007.1.31 1151
  char* nn    = "observation";
  saveData<<"size_m "<<size_m<<"stateNum "<<stateNum<<endl;
  for(int runNum=0; runNum<NUM_RUNS; runNum++)         //һ��ѵ���Ĵ���
  {
    //saveStep<<"runNum is "<<runNum<<endl;
    for(i=0; i<size_m; i++)
      for(j=0; j<NUM_ACTIONS; j++)
	(*Q)[i][j] = 0.0;
    eps = 1.0;
    for(i=0; i<NUM_EPISODES; i++)
    {
      env->reset();
      lastObservation = new Symbol(0, nn,  SYM_OBSERVATION);
      steps = 0;
      eps -= 0.1;
      if(eps < 0.01)
	eps = 0.01;
      interact = new symList(); 
      //interact->append(new symList());
      stateId = 0;                                    //������������㷨����ȷ����ʼ״̬ sateId Ϊ 0
      for(j=0; j<k; j++)
      	(*tempCoreValue)[j] = (*firstProb)[j];
      for(j=0; j<NUM_ACTIONS; j++)
	qValue[j] = (*Q)[stateId][j];
      do
      {
	action = selectAction(qValue, eps);
      }while(env->crumpus_open(action));              //�ж�action�������Ƿ���ǽ�ڣ�����ǵĻ�����ѡ����
      //saveData<<"Before while "<<endl;
      while(1)
      {
	//saveData<<interact->seekEnd()->s->id<<" "<<stateId<<" "<<action<<endl;//ֻ�������������
	steps++;
	actionSend = env->getAction(action);
	nextAoSet = new symSet();
	currobs = obslist;                            //�˴��ĳ�ʼ���м�Ҫ���ϣ�Ҫ��Ȼ��һ��ѭ����ʱ�� currobs Ϊ NULL
	while ((currobs != NULL) && (currobs->s != NULL)) 
	{ 
	  //nextAoSet->insert(new symList((actionSend, new symList(currobs->s, NULL))));//ò������������
	  nextAoSet->insert(new symList(new Symbol(actionSend), new symList(new Symbol(currobs->s))));
	  currobs = currobs->n;
	} // obs
	sumNextQ = 0.0;
	for(iCurr=nextAoSet->begin(); iCurr!=nextAoSet->end(); iCurr++)
	{
	  tempInteract = new symList(interact);
	  tempInteract->append(new symList(*iCurr));
	  //saveData<<"Before ctProbP finding"<<endl;
	  if(ctProbP->find(tempInteract) != ctProbP->end())   //�жϵ�ǰʱ������״̬�� core-test ��ֵ
	    for(m=0, piH=ctProbP->begin(); piH!=ctProbP->end(); m++, piH++)
	    {
	      if(*(piH->first) == *tempInteract)
	      {
		nextStateId = m;
		for(n=0; n<k; n++)
		  (*coreValue)[n] = (*statesProb)[nextStateId][n];
		break;
	      }
	    }
	  else
	  {
	    denominator = 0;
	    for(n=0; n<k; n++)
	      denominator += ((*tempCoreValue)[n] * (maoMatrix->find(*iCurr)->second->at(n)));
	    if(denominator == 0)
	      for(m=0; m<k; m++)
		(*coreValue)[m] = 0.0;
	    else
	    {
	      //for(m=0, iCurr2=qtests->begin(); iCurr2!=qtests->end(); m++, iCurr2++) //��ȡ��ǰʱ�̵� core-test ��ֵ
	      for(m=0, viT=viH->second->begin(); viT!=viH->second->end(); m++, viT++) //��ȡ��ǰʱ�̵� core-test ��ֵ
	      {
		numerator   = 0;
		for(n=0; n<k; n++)
		  numerator += (*tempCoreValue)[n] * (maoqMatrix->find(*iCurr)->second->find(viT->first)->second->at(n));
		(*coreValue)[m] = (double)(numerator/denominator);
		if( (*coreValue)[m] > 1 )
		  (*coreValue)[m] = 1;
	      }
	    }
	    //������Ǹ��ݵ�ǰʱ�� core-test ��ֵ�жϵ�ǰʱ�������ĸ�״̬,�þ����б�
	    bestDistance = DBL_MAX;
	    for(m=0;m<size_m; m++)
	    {
	      distance = 0.0;
	      for(n=0; n<k; n++)                          //���� pow(x,y) ��ԭ������ pow �� x ����Ϊ��
		distance += (((*coreValue)[n] - (*statesProb)[m][n]) * ((*coreValue)[n] - (*statesProb)[m][n]));
	      if(distance < bestDistance)
	      {
		bestDistance = distance;
		nextStateId = m;
	      }
	    }
	  }
	  //saveData<<"Before aoProb == 0"<<endl;
	  aoProb = 0.0;
	  for(n=0; n<k; n++)
	    aoProb += (*tempCoreValue)[n] * (maoMatrix->find(*iCurr)->second->at(n));
	  if((*iCurr)->n->s->id == 7) //��ʾ��һ��״̬ΪĿ��״̬�������ж� nextStateId�Ƿ�� goalId ���,ԭ��Ϊ 7
	  {
	    reward = 10;
	    sumNextQ += aoProb * (reward + gamma*0);
	  }
	  else
	  {
	    reward = 0;
	    for(m=0; m<NUM_ACTIONS; m++)
	      qValue[m] = (*Q)[nextStateId][m];
	    sumNextQ += aoProb *(reward + gamma*((*Q)[nextStateId][argmax(qValue)]));//�õ��� Q ѧϰ��
	  }
	  delete tempInteract;
	  //saveData<<"After tempInteract deleting"<<endl;
	}
	//saveData<<"Before delete nextAoSet"<<endl;
	delete nextAoSet;
	//delta = sumNextQ - (*Q)[stateId][action]; //ע�������� stateId ������ nextstateId,ȥ�� reward
	(*Q)[stateId][action] = sumNextQ;

	//saveData<<"Before get observationReceived"<<endl;
	observationReceived = env->sendAction(actionSend);
	id = observationReceived->id;
	if(env->crumpus_goal() || (steps>MAX_STEPS))                         //�ж��Ƿ񵽴�Ŀ��״̬
	{
	  saveStep<<i<<" "<<steps<<endl;
	  delete lastObservation;
	  break;
	}
	
	//������������岻��ײǽ����������״̬�ǳ�ʼ״̬����Ϊ������ĳ�ʼ��Ŀ��״̬��ȷ���ģ�ͬʱҲ֪���Ƿ�ײǽ
	//Ӧ����˵�ھ�ȷ��ȡ����ģ�͵�ǰ���£��������е��ж϶��ǲ���Ҫ�ģ�����ʱ�������Ժ�ȥ��ʵ��
	//if( (observationReceived != lastObservation)&&(observationReceived != env->getObservation(0)) )//�˴�������
	if( (observationReceived->id != lastObservation->id)&&(observationReceived->id != 0) )
	{
	  delete lastObservation;
	  lastObservation = new Symbol(id, nn,  SYM_OBSERVATION);
	  //saveData<<"After crumpus_goal"<<endl;
	  newAO = new symList(new Symbol(actionSend), new symList(new Symbol(observationReceived)));
	  interact->append(new symList(newAO));
	  if(ctProbP->find(interact) != ctProbP->end())   //�����ǰ��������״̬�� ctMatP ������ͬ�ĵط�����ֵ��λ
	    for(m=0, piH=ctProbP->begin(); piH!=ctProbP->end(); m++, piH++)
	    {
	      if(*(piH->first) == *interact)
	      {
		stateId = m;
		break;
	      }
	    }
	  else
	  {
	    denominator = 0.0;
	    for(n=0; n<k; n++)
	      denominator += (*tempCoreValue)[n] * (maoMatrix->find(newAO)->second->at(n));
	    if(denominator == 0.0)
	      for(m=0; m<k; m++)
		(*coreValue)[m] = 0.0;
	    else
	    {
	      //for(m=0, iCurr2=qtests->begin(); iCurr2!=qtests->end(); m++, iCurr2++) //��ȡ��ǰʱ�̵� core-test ��ֵ
	      for(m=0,viT=viH->second->begin();viT!=viH->second->end(); m++, viT++) //��ȡ��ǰʱ�̵� core-test ��ֵ
	      {
		numerator   = 0;
		for(n=0; n<k; n++)
		  numerator   += (*tempCoreValue)[n] * (maoqMatrix->find(newAO)->second->find(viT->first)->second->at(n));
		(*coreValue)[m] = numerator/denominator;
		if((*coreValue)[m] > 1)
		  (*coreValue)[m] = 1;
	      }
	    }
	    //������Ǹ��ݵ�ǰʱ�� core-test ��ֵ�жϵ�ǰʱ�������ĸ�״̬,�þ����б�
	    bestDistance = DBL_MAX;
	    for(m=0;m<size_m; m++)
	    {
	      distance = 0.0;
	      for(n=0; n<k; n++)                          //���� pow(x,y) ��ԭ������ pow �� x ����Ϊ��
		distance += ((*coreValue)[n] - (*statesProb)[m][n]) * ((*coreValue)[n] - (*statesProb)[m][n]);
	      if(distance < bestDistance)
	      {
		bestDistance = distance;
		stateId = m;
	      }
	    }

	  }
	  delete newAO;
	  for(m=0; m<k; m++)
	    (*tempCoreValue)[m] = (*statesProb)[stateId][m]; //�� tempCoreValue ��ֵ�� statesProb �����ֵ��λ     
	}
	else
	  if(observationReceived->id == 0)            //��ʾ����������״̬Ϊ��ʼ״̬
	  {
	    stateId = 0;
	    delete interact;
	    interact = new symList();
	    delete lastObservation;
	    //lastObservation = new Symbol(observationReceived);
	    lastObservation = new Symbol(0, nn,  SYM_OBSERVATION);
	    for(m=0; m<k; m++)
	      (*tempCoreValue)[m] = (*firstProb)[m];
	  }

	//stateData<<observationReceived->id<<" "<<stateId<<endl;
	for(j=0; j<NUM_ACTIONS; j++)
	  qValue[j] = (*Q)[stateId][j];
	do
	{
	  action = selectAction(qValue, eps);
	}while(env->crumpus_open(action));    
	
      }
      delete interact;
      //delete lastObservation;
      //saveData<<"After interact deleting"<<endl;
    }
    saveData<<endl;
  }
  //saveData<<"Before decomp deleting"<<endl;
  paraData<<"Q data "<<endl;
  for(i=0; i<size_m; i++)
  {
    for(j=0; j<NUM_ACTIONS; j++)
      paraData<<(*Q)[i][j]<<" ";
    paraData<<endl;
  }
  //delete(decomp);
  //delete IDENT;
  delete maoMatrix;
  delete maoqMatrix;
#endif
  saveData<<"CONGRATUALATION, YOU HAVE FINISHED THE WORK"<<endl;
}

int   psr_sysdyn::selectAction(double qValue[NUM_ACTIONS], double p) //��״̬ sta ʱ�ж�ѡ���ĸ�����
{
  int action=argmax(qValue);
  if(withProbability(p))
    action=rand()%NUM_ACTIONS;
  return action;
}

bool  psr_sysdyn::withProbability( double p )             //��ʾ�¼������ĸ���Ϊ p, ���� epsion-greedy ����
{
  return p > ((float)rand()) / RAND_MAX;                  //����rand()���ص���ֵΪ0-RAND_MAX
}

int   psr_sysdyn::argmax(double qValue[NUM_ACTIONS])       //ѡ����ȡ�����ֵ�Ķ���
{
  int best_action = 0;
  float best_value = qValue[0];
  int num_ties = 1;
  for ( int a = 1; a < NUM_ACTIONS; a++ )
  {
    float value = qValue[a];
    if ( value > best_value)                         //�ж�Q[NUM_ACTIONS]������Qֵ��Ӧ��������
    {
      best_value = value;
      best_action = a;
    }
    else if( value == best_value )                                              //����Ĵ����ʾʲô��˼��
    {
      num_ties++;
      if ( 0 == rand() % num_ties ) 
      {
	best_value = value;
	best_action = a;
      }

    }
    
  }
  return best_action;
}

void  psr_sysdyn::decayTraces( float decay_rate )         //���� trace �� 
{
}

int   psr_sysdyn::step(double qValue[NUM_ACTIONS])         //ÿһ��ִ�еĳ���
{
}
vector<Symbol*>*  psr_sysdyn::getData(int hist_Len)
{
  Symbol* actionToSend, *actionReceived;
  int prob;
  //env->reset();
  for(i=0; i<hist_Len; i++)
  {
    do
    {
      prob = (int) trunc((float) random() * (float) (env->nrActions()) / (float) RAND_MAX);
    } while (prob==env->nrActions()||(env->crumpus_open(prob)));    //crumpus_open ��������ȥ��
    actionToSend = env->getAction(prob);       
    continueData->push_back(actionToSend);
    actionReceived = env->sendAction(actionToSend); 
    continueData->push_back(actionReceived);
  }
  return continueData;
}
symSet* psr_sysdyn::getLandmark(tEstimateData* matData, int markLen)//��ֻ���� markLen Ϊ1��2ʱ��������������������
{
  symSet* landmarkSet = new symSet();
  tEstimateData* tempMatData; 
  symList* tempList;
  symList	*actlist = env->getActions(), *curract,
  *obslist = env->getObservations(), *currobs;
  iH = matData->ctMat->begin();                           //��ʼ�� augTest
  int rank;
  if(markLen == 2)                                    //�Զ���-�۲�ֵ����Ϊ landmark,����������������
  {
    curract = actlist;
    while ((curract != NULL) && (curract->s != NULL)) 
    { 
      currobs = obslist;
      while ((currobs != NULL) && (currobs->s != NULL)) 
      { 
	tempList = new symList(new Symbol(curract->s), new symList(currobs->s, NULL));
	//tempMatData = (tEstimateData *) malloc(sizeof(tEstimateData)); 
	tempMatData = getMatchData(matData, tempList);
	rank = getRank(tempMatData);
	cout<<"tempList "<<*tempList<<"rank  "<<rank<<endl;
	if( rank==1 )                 //��ʾ��ǰ�� tempList Ϊlandmark
	  landmarkSet->insert(tempList);
	else
	  delete tempList;

	delete tempMatData->ctMat;
	delete tempMatData->ctProb;
	delete tempMatData->ctAct;
	delete tempMatData->ctHists;
	delete tempMatData->probs;
	delete tempMatData->counts;
	delete tempMatData->actcounts;
	delete tempMatData->hcounts;
	free(tempMatData);
	
	currobs = currobs->n;
      } 
      curract = curract->n;
    }
  }
  else if(markLen == 1)                                  //�����Թ۲�ֵ��Ϊ landmark
  {
    currobs = obslist;
    while((currobs != NULL)&&(currobs->s != NULL))
    {
      tempList = new symList(currobs->s, NULL);
      //tempMatData = (tEstimateData *) malloc(sizeof(tEstimateData)); 
      tempMatData = getMatchData(matData, tempList);
      if(tempMatData->ctMat->size()==0)
	cout<<"Somthing wrong"<<endl;
      rank = getRank(tempMatData);
      cout<<"tempList "<<*tempList<<"rank  "<<rank<<endl;
      if( rank==1 )                 //��ʾ��ǰ�� tempList Ϊlandmark
	landmarkSet->insert(tempList);
      else
	delete tempList;
      delete tempMatData->ctMat;
      delete tempMatData->ctProb;
      delete tempMatData->ctAct;
      delete tempMatData->ctHists;
      delete tempMatData->probs;
      delete tempMatData->counts;
      delete tempMatData->actcounts;
      delete tempMatData->hcounts;
      free(tempMatData);

      currobs = currobs->n;
    }
      
  }
  else
    cout<<"There is still no other landmark length writed"<<endl;
  return landmarkSet;
}

int psr_sysdyn::getRank(tEstimateData* matData)                            
{
  int rank;
  int columnNum;
  zeroColumn = false;
  ctMatP  = new symListMatrix();
  ctProbP = new symListDblMat();
  ctActP  = new symListMatrix();
  augTest = new symSet();
  iH = matData->ctMat->begin();                           //��ʼ�� augTest
  for (i=0, iT = iH->second->begin(); iT != iH->second->end(); i++, iT++) 
    augTest->insert(iT->first);

  for(i=0, iH = matData->ctMat->begin(); iH != matData->ctMat->end(); i++, iH++)//��ʼ�� ctMatP �� ctProbP
  {
    rowDbl    = (*matData->probs)[i];
    row       = (*matData->counts)[i];
    rowDblMap = new symList2DblMap();               
    for(iCurr = augTest->begin(); iCurr != augTest->end(); iCurr++)
      rowDblMap->insert(pair<symList*, double*>(*iCurr,new double(*(rowDbl++))));
    ctProbP->insert(pair<symList*, symList2DblMap*>(iH->first, rowDblMap));
    rowMap    = new symList2IntMap();
    for(iCurr = augTest->begin(); iCurr != augTest->end(); iCurr++)
      rowMap->insert(pair<symList*, int*>(*iCurr, new int(*(row++))));
    ctMatP->insert(pair<symList*, symList2IntMap*>(iH->first, rowMap));
  }
  for(i=0, tiH = matData->ctAct->begin(); tiH != matData->ctAct->end(); i++, tiH++)//��ʼ��ctActP
  {
    row = (*matData->actcounts)[i];
    rowMap = new symList2IntMap();
    for(tiT = tiH->second->begin(); tiT != tiH->second->end(); tiT++)
      rowMap->insert(pair<symList*, int*>(tiT->first, new int(*(row++))));
    ctActP->insert(pair<symList*, symList2IntMap*>(tiH->first, rowMap));
  }

  //����� ctMatP �� ctProbP ����Ķ���ĳһ�У�������Ӧ�����е�Ԫ�ض�Ϊ�㣬����ɾȥ
  for (iCurr = augTest->begin(); iCurr != augTest->end(); iCurr++)
  {
    zeroColumn = true;
    for (piH = ctProbP->begin(); piH != ctProbP->end();  piH++)  
      if(*(piH->second->find(*iCurr)->second))
	zeroColumn = false;
    if(zeroColumn)
    {
      for (piH = ctProbP->begin(), iH = ctMatP->begin(); piH != ctProbP->end(), iH != ctMatP->end();  piH++, iH++)
      {
	piH->second->erase(*iCurr);
	iH->second->erase(*iCurr);
      }
    }
  }
#if 0
  //��� test
  piH=ctProbP->begin();
  for(piT=piH->second->begin(); piT!=piH->second->end(); piT++)
    cout<<*piT->first<<" ";
  cout<<endl;
 //��� matData->ctProb �е�ֵ
  for(piH=ctProbP->begin(); piH!=ctProbP->end(); piH++)
  {
    for(piT=piH->second->begin(); piT!=piH->second->end(); piT++)
      cout<<*piT->second<<" ";
    cout<<endl;
  }
#endif
  size_m = ctMatP->size();
  iH = ctMatP->begin();
  size_n = iH->second->size();
  if( (size_m==1)&&(size_n>0) )
  {
    rank = 1;
    return rank;
  }
  probsP  = new Array2D<double>(size_m, size_n);     //��Ӧ����
  countsP = new Array2D<int>(size_m, size_n);        //��Ӧ��ֵ
  for(i=0, piH=ctProbP->begin(), iH = ctMatP->begin(); piH != ctProbP->end(), iH != ctMatP->end(); i++, piH++, iH++)
    for(j=0, piT = piH->second->begin(), iT = iH->second->begin(); piT != piH->second->end(), iT != iH->second->end(); j++, piT++, iT++)
    {
      (*probsP)[i][j]  = *(piT->second);
      (*countsP)[i][j] = *(iT->second);
    }
  tiH = matData->ctAct->begin();
  int     *rowsReadyRem;
  rowsReadyRem = (int*)malloc(size_m * sizeof(int)); 
  int rowIndepK=0;
  Array2D<double> *rowIndep     = new Array2D<double>(2, size_n);     //������������ж��Ƿ�������ص�����������
  int *hcounts      = (int*)malloc(size_m * sizeof(int));
  for(i = 0;i < size_m; i++)                          
  {
    rowsReadyRem[i] = -1;                             //���Ƚ����е�Ԫ�س�ʼ��Ϊ-1
    hcounts[i] = (*matData->hcounts)[i];              //�������������������أ�����ȷ���Ƴ���һ�е�
  }
  rowsReadyRem[0] = 0;
  for(i=1; i<size_m; i++)                             //����һ�к������Ѿ��ж�Ϊ���������޹صĵ���������
  {                                                   //�Ƚϣ�����ȷ����ǰ��һ���Ƿ��ǰ�����жϹ��������������޹�
    for(int kk=0; kk<size_n; kk++)
      (*rowIndep)[1][kk] = (*probsP)[i][kk];
    for(j=0; j<i; j++)
    {
      if(rowsReadyRem[j] == j)
      {
	numCounts = 0;
	for(int kk=0; kk<size_n; kk++)
	{
	  (*rowIndep)[0][kk] = (*probsP)[j][kk];
	  if( ((*rowIndep)[0][kk] == (*rowIndep)[1][kk]) && ((*rowIndep)[1][kk] == 0) )
	    numCounts++;
	}

	epsilon = 0.0;
	for (m=0, iH = ctMatP->begin(); iH != ctMatP->end(); m++, iH++) 
	{   
	  if( (m==j) || (m==i) )
	  {
	    for (columnNum=0, iT = iH->second->begin(); iT != iH->second->end(); columnNum++, iT++) 
	    {
	      del = iT->first->filter(SYM_ACTION); 
	      TH = (double) (*countsP)[m][columnNum];
	      for(n = 0, tiT = tiH->second->begin(); tiT != tiH->second->end(); n++, tiT++)
		if(*del == *tiT->first)        
		{
		  THa = (double) (*matData->actcounts)[m][n];
		  break;
		}
	      delete del;                       
	      if (TH==0 || THa==0) continue;
	      epsilon += sqrt(((TH/THa)*(1-(TH/THa)))/(THa*(1-CONFIDENCE)));
	    }
	  }
	}
	epsilon /= (2 * (size_n - numCounts));
	norm = infNorm(rowIndep);                            //�õ� norm ֵ
	cutoffValue = epsilon*norm;
	singular = new SVD<double>(*rowIndep);              
	Array1D<double> *singvals = new Array1D<double>();
	singular->getSingularValues(*singvals);
	rowIndepK = singular->rank(epsilon*norm);
	delete singvals;
	delete(singular);
	//saveData << "RowIndep rank is: " <<rowIndepK << endl;
	if(rowIndepK==2)                                    //��ʾҪ�жϵ��к͵�ǰ�������޹�, ���� i ������Ԫ�ز���
	  rowsReadyRem[i] = i;
	else //if(rowIndepK==1)                                       //��ʾҪ�жϵ��к͵�ǰ���������
	{
	  if(hcounts[i] <=  hcounts[j])
	    rowsReadyRem[i] = -1;      
	  else                                      //��ʾ�������������أ�������������ı�ʾΪ�����޹�
	  {
	    rowsReadyRem[i] = i;
	    rowsReadyRem[j] = -1;
	  }
	  break;
	}

      }
    }
	
  }//����õ��Ľ���� rowsReadyRem�в�Ϊ-1�������������޹�,ͬ����Ϊ-1���������޹أ����в������������޹�
  
  free(hcounts);
  delete rowIndep;
  
  delete probsP;
  delete countsP;
  //saveData << "Histories:\n";
  eraseHistories = new symSet();
  for (iH=ctMatP->begin(); iH!=ctMatP->end(); iH++)
    eraseHistories->insert(iH->first);

  //���潫�� rowsReadyRem �� -1 ��Ӧ���д� P->ctMat ������ȥ
  for(i=0, iCurr=eraseHistories->begin(); i<size_m,iCurr!=eraseHistories->end(); i++, iCurr++)
  {
    if(rowsReadyRem[i] == -1)                       //����� erase ����Ӧ�ûᵼ���ڴ�й©����
    {
      //saveData<<"i "<<i<<**iCurr<<endl;
      ctMatP->erase(*iCurr);
      matData->ctMat->erase(*iCurr);
      ctActP->erase(*iCurr);
      matData->ctAct->erase(*iCurr);
      ctProbP->erase(*iCurr);
      matData->ctProb->erase(*iCurr);
      //augCtHists->erase(*iCurr);
    }
  }
  free(rowsReadyRem);
  /**************************************************************************
   *****************************�е��жϲ���*********************************
   **************************************************************************/
  //saveData<<"now is column removing "<<endl;
  size_m = ctMatP->size();
  size_act = ctActP->begin()->second->size();
  probsP  = new Array2D<double>(size_m, size_n);     //��Ӧ����
  countsP = new Array2D<int>(size_m, size_n);        //��Ӧ��ֵ
  augActcounts  = new Array2D<int>(size_m, size_act);
  for(i=0, piH=ctProbP->begin(), iH = ctMatP->begin(); piH != ctProbP->end(), iH != ctMatP->end(); i++, piH++, iH++)
    for(j=0, piT = piH->second->begin(), iT = iH->second->begin(); piT != piH->second->end(), iT != iH->second->end(); j++, piT++, iT++)
    {
      (*probsP)[i][j]  = *(piT->second);
      (*countsP)[i][j] = *(iT->second);
    }
  for(i=0, tiH=ctActP->begin(); tiH!=ctActP->end(); i++, tiH++)
    for(j=0, tiT=tiH->second->begin(); tiT!=tiH->second->end(); j++, tiT++)
      (*augActcounts)[i][j] = *(tiT->second);
  //����� ctMatP �� ctProbP ����Ķ���ĳһ�У�������Ӧ�����е�Ԫ�ض�Ϊ�㣬����ɾȥ
  for (iCurr = augTest->begin(); iCurr != augTest->end(); iCurr++)
  {
    zeroColumn = true;
    for (piH = ctProbP->begin(); piH != ctProbP->end();  piH++)  
      if(*(piH->second->find(*iCurr)->second))
	zeroColumn = false;
    if(zeroColumn)
      for (piH = ctProbP->begin(),iH = ctMatP->begin(); piH != ctProbP->end(), iH != ctMatP->end();  piH++, iH++)
      {
	piH->second->erase(*iCurr);
	iH->second->erase(*iCurr);
      }
  }
  iH = ctMatP->begin();
  size_n = iH->second->size();
  if( (size_m==1)&&(size_n>0) )
  {
    rank = 1;
    return rank;
  }
  delete probsP;
  delete countsP;
  probsP  = new Array2D<double>(size_m, size_n);     //��Ӧ����
  countsP = new Array2D<int>(size_m, size_n);        //��Ӧ��ֵ
  for(i=0, piH=ctProbP->begin(), iH = ctMatP->begin(); piH != ctProbP->end(), iH != ctMatP->end(); i++, piH++, iH++)
    for(j=0, piT = piH->second->begin(), iT = iH->second->begin(); piT != piH->second->end(), iT != iH->second->end(); j++, piT++, iT++)
    {
      (*probsP)[i][j]  = *(piT->second);
      (*countsP)[i][j] = *(iT->second);
    }
    
  tiH = ctActP->begin();
  int rowRemainNum = 0;
  // estimate the rank                              //�˾������
  epsilon = 0.0;
  for (i=0, iH = ctMatP->begin(); iH != ctMatP->end(); i++, iH++) 
  {   
    for (columnNum=0, iT = iH->second->begin(); iT != iH->second->end(); columnNum++, iT++) 
    {
      del = iT->first->filter(SYM_ACTION); 
      TH = (double)(*countsP)[i][columnNum];
      for(j = 0, tiT = tiH->second->begin(); tiT != tiH->second->end(); j++, tiT++)
	if(*del == *tiT->first)        
	{
	  THa = (double) (*augActcounts)[i][j];
	  break;
	}
      delete del;
      if (TH==0 || THa==0) continue;
      epsilon += sqrt(((TH/THa)*(1-(TH/THa)))/(THa*(1-CONFIDENCE)));
    }
  }
  epsilon /= (size_m * size_n);  
  norm = infNorm(probsP);                            //�õ� norm ֵ
  cutoffValue = epsilon*norm;
  singular = new SVD<double>(* probsP);              
  Array1D<double> *singvals = new Array1D<double>();
  singular->getSingularValues(*singvals);
  delete singvals;
  rank = singular->rank(epsilon*norm);
  delete(singular);
  if (rank == 0) 
  {
    cout << "Something 'Truly Bad'(TM) happened, and we are expecting a matrix of rank 0\n";
    rank = 1;
    //exit(1);
  }
 
  delete probsP;
  delete countsP;
  delete ctMatP;
  delete ctProbP;
  delete ctActP;
  delete augActcounts;
  delete augTest;
  delete eraseHistories;
  return rank;
}
tEstimateData*      psr_sysdyn::getMatchData(tEstimateData* matData, symList* tempList)
{
  tEstimateData* tempMatData = (tEstimateData *) malloc(sizeof(tEstimateData));
  tempMatData->ctMat  = new symListMatrix();
  tempMatData->ctProb = new symListDblMat();
  tempMatData->ctAct = new symListMatrix();
  tempMatData->ctHists = new symList2IntMap();
  augTest = new symSet();
  iH=matData->ctMat->begin();
  for(iT=iH->second->begin(); iT!=iH->second->end(); iT++)
  {
    augTest->insert(iT->first);
  }
  for(i=0, iH=matData->ctMat->begin(); iH!=matData->ctMat->end(); i++, iH++)
  {
    //cout<<"getMatchData "<<i<<" "<<*iH->first<<endl;
    if(iH->first->endWith(tempList))            //�� tempMatData �н����� tempList �ľ���ȡ����
    {
      //cout<<"In endWith"<<endl;
      rowDbl    = (*matData->probs)[i];
      row       = (*matData->counts)[i];
      rowDblMap = new symList2DblMap();               
      for(iCurr = augTest->begin(); iCurr != augTest->end(); iCurr++)
	rowDblMap->insert( pair<symList*, double*>(*iCurr,rowDbl++) );
      tempMatData->ctProb->insert(pair<symList*, symList2DblMap*>(iH->first, rowDblMap));
      rowMap    = new symList2IntMap();
      for(iCurr = augTest->begin(); iCurr != augTest->end(); iCurr++)
	rowMap->insert( pair<symList*, int*>(*iCurr, row++) );
      tempMatData->ctMat->insert(pair<symList*, symList2IntMap*>(iH->first, rowMap));
	      
      tiH = matData->ctAct->begin();
      row = (*matData->actcounts)[i];
      rowMap = new symList2IntMap();
      for(tiT = tiH->second->begin(); tiT != tiH->second->end(); tiT++)
	rowMap->insert( pair<symList*, int*>(tiT->first, row++) );
      tempMatData->ctAct->insert(pair<symList*, symList2IntMap*>(iH->first, rowMap));//ע���� iH ������ tiH
      tempMatData->ctHists->insert(pair<symList*, int*>(iH->first, &(*matData->hcounts)[i] ));
    }
  }
  size_m = tempMatData->ctMat->size();
  size_n = tempMatData->ctMat->begin()->second->size();
  size_act = tempMatData->ctAct->begin()->second->size();
  tempMatData->probs  = new Array2D<double>(size_m, size_n);     //��Ӧ����
  tempMatData->counts = new Array2D<int>(size_m, size_n);        //��Ӧ��ֵ
  tempMatData->actcounts  = new Array2D<int>(size_m, size_act);
  tempMatData->hcounts = new Array1D<int>(size_m, 0);
	
  for(i=0, piH=tempMatData->ctProb->begin(), iH = tempMatData->ctMat->begin(); piH != tempMatData->ctProb->end(), iH != tempMatData->ctMat->end(); i++, piH++, iH++)
    for(j=0, piT = piH->second->begin(), iT = iH->second->begin(); piT != piH->second->end(), iT != iH->second->end(); j++, piT++, iT++)
    {
      (*tempMatData->probs)[i][j]  = *(piT->second);
      (*tempMatData->counts)[i][j] = *(iT->second);
    }
  for(i=0, tiH=tempMatData->ctAct->begin(); tiH!=tempMatData->ctAct->end(); i++, tiH++)
    for(j=0, tiT=tiH->second->begin(); tiT!=tiH->second->end(); j++, tiT++)
      (*tempMatData->actcounts)[i][j] = *(tiT->second);
  for(i=0, histIt=tempMatData->ctHists->begin(); histIt!=tempMatData->ctHists->end(); i++, histIt++)
    (*tempMatData->hcounts)[i] = *(histIt->second);
  return tempMatData;
}
void psr_sysdyn::freeData(tEstimateData* matData)
{
  delete matData->probs;
  delete matData->counts;
  delete matData->hcounts;
  delete matData->actcounts;

  iH = matData->ctMat->begin();
  while (iH != matData->ctMat->end()) 
  {
    tiH = iH; tiH++;
    iT = iH->second->begin();
    while (iT != iH->second->end()) 
    {
      tiT = iT; tiT++;
      //del = iT->second;
      iH->second->erase(iT);
      //delete del;
      iT = tiT;
    }
    delete iH->second;
    //del = iH->first;
    matData->ctMat->erase(iH);
    iH = tiH;
    //delete del;
  }
  delete matData->ctMat;

  piH = matData->ctProb->begin();
  while (piH != matData->ctProb->end()) 
  {
    tempPih= piH; tempPih++;
    piT = piH->second->begin();
    while (piT != piH->second->end()) 
    {
      tempPit = piT; tempPit++;
      del = piT->first;
      delete del;
      //del = piT->second;
      //delete del;
      piH->second->erase(piT);
      piT = tempPit;
    }
    delete piH->second;
    //del = piH->first;
    matData->ctProb->erase(piH);
    piH = tempPih;
    //delete del;
  }
  delete matData->ctProb;

  tiH = matData->ctAct->begin();
  while (tiH != matData->ctAct->end()) 
  {
    tempTih=tiH; tempTih++;
    tiT = tiH->second->begin();
    while (tiT != tiH->second->end()) 
    {
      tempTit = tiT; tempTit++;
      del = tiT->first;
      delete del;
      //del = tiT->second;
      //delete del;
      tiH->second->erase(tiT);
      tiT = tempTit;
    }
    delete tiH->second;
    //del = tiH->first;
    matData->ctAct->erase(tiH);
    tiH = tempTih;
    //delete del;
  }
  delete matData->ctAct ;

  iT = matData->ctHists->begin();
  while (iT != matData->ctHists->end()) 
  {
    tiT = iT; tiT++;
    del = iT->first;
    delete del;
    //del = iT->second;
    //delete del;
    matData->ctHists->erase(iT);  //֮����Ҫ���� tiT ��ԭ���� erase ��һ�����ݺ��α�ָ��ͷλ��
    iT = tiT;
  }
  delete matData->ctHists;
  free(matData);

}
int psr_sysdyn::getBelState(int envState)   //envState->belState ��ת������
{
  int belState;
  switch(envState)
  {
  case 0:
    belState = 0;
    break;
  case 2:
    belState = 1;
    break;
  case 4:
    belState = 2;
    break;
  case 5:
    belState = 3;
    break; 
  case 7:
    belState = 4;
    break;
  case 9:
    belState = 5;
    break;
  case 10:
    belState = 6;
    break;
  case 11:
    belState = 7;
    break;
  case 12:
    belState = 8;
    break;
  case 13:
    belState = 9;
    break;
  case 14:
    belState = 10;
    break;
  default:
    printf("There is no such state\n");
    break;
  }
  return belState;
}
void psr_sysdyn::initPomdp(Array3D<double>* stateTra, Array2D<double>* obsTra) //�ٶ�ת�Ƶ�����ѡ��֮��Ķ����ĸ���Ϊ 0.3
{
  for(i=0; i<=10; i++)                                //������жԽ����ϵĳ�ʼ��
    if(i<=2)
    {
	(*stateTra)[0][i][i] = 0.3;
	(*stateTra)[3][i][i] = (*stateTra)[2][i][i] = (*stateTra)[1][i][i] = 0.9;
    }
    else if(i<=5)
    {
      (*stateTra)[2][i][i] =(*stateTra)[0][i][i] = 0.2;
      (*stateTra)[3][i][i] = (*stateTra)[1][i][i] = 0.8;
    }
    else if(i<=7)
    {
      (*stateTra)[0][i][i] = 0.8;
      (*stateTra)[1][i][i] = 0.2;
      if(i==6)
      {
	(*stateTra)[2][i][i] = 0.2;
	(*stateTra)[3][i][i] = 0.8;
      }
      else
      {
	(*stateTra)[2][i][i] = 0.8;
	(*stateTra)[3][i][i] = 0.2;
      }
    }
    else if(i==8)
    {
      (*stateTra)[0][i][i] = 0.7;
      (*stateTra)[3][i][i] = (*stateTra)[2][i][i] = (*stateTra)[1][i][i] = 0.1;
    }
    else 
    {
      (*stateTra)[0][i][i] = 0.8;
      (*stateTra)[3][i][i] = 0.2;
      if(i==9)
      {
	(*stateTra)[1][i][i] = 0.2;
	(*stateTra)[2][i][i] = 0.8;
      }
      else
      {
	(*stateTra)[1][i][i] = 0.8;
	(*stateTra)[2][i][i] = 0.2;
      }
    }
  j=0;
  for(i=3; i<=6; i++)
  {
    (*stateTra)[3][i][j] = (*stateTra)[1][i][j] = (*stateTra)[0][i][j] = 0.1;
    (*stateTra)[3][j][i] = (*stateTra)[2][j][i] = (*stateTra)[1][j][i] = 0.1;
    (*stateTra)[0][j][i] = (*stateTra)[2][i][j] = 0.7;
    ++j;
  }
  j=6;
  for(i=7; i<=10; i++)
  {
    (*stateTra)[2][i][j] = (*stateTra)[1][i][j] = (*stateTra)[0][i][j] = 0.1;
    (*stateTra)[3][j][i] = (*stateTra)[2][j][i] = (*stateTra)[0][j][i] = 0.1;
    (*stateTra)[1][j][i] = (*stateTra)[3][i][j] = 0.7;
    ++j;
  }
  for(i=0; i<=3; i++)
  {
    if( (i==1) && (i==3) )
    {
      (*stateTra)[i][8][4] = (*stateTra)[i][4][8] = 0.1;
      (*stateTra)[i][10][5] = (*stateTra)[i][5][10] = 0.1;
    }
    else
    {
      if(i==0)
      {
	(*stateTra)[i][8][4] = (*stateTra)[i][10][5] = 0.1;
	(*stateTra)[i][4][8] = (*stateTra)[i][5][10] = 0.7;
      }
      else
      {
	(*stateTra)[i][8][4] = (*stateTra)[i][10][5] = 0.7;
	(*stateTra)[i][4][8] = (*stateTra)[i][5][10] = 0.1;
      }
    }
  }
  //�����ǹ۲�ֵ����
  (*obsTra)[1][0]=1, (*obsTra)[6][1]=1, (*obsTra)[8][2]=1, (*obsTra)[7][3]=1,(*obsTra)[9][3]=1,(*obsTra)[10][4]=1,
  (*obsTra)[3][5]=1,(*obsTra)[4][5]=1,(*obsTra)[5][5]=1,(*obsTra)[0][6]=1,(*obsTra)[2][6]=1;
}



psr_sysdyn::psr_sysdyn(Environment *e, int i, int j, int jj) : psr(e), iterations(i),histLength(j),testLength(jj)
{
  //continueData = new vector<Symbol*>;
  //continueData->reserve(iterations+100);
}

psr_sysdyn::~psr_sysdyn() 
{
  //delete continueData;
}

#endif
