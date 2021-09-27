#ifndef __PSR_SYSDYN_CPP_
#define __PSR_SYSDYN_CPP_

#include <fstream>
#include <iostream>

#include "psr-sysdyn.h"

using namespace std;
#define TEST_LENGTH 1000000                             //检验数据的长度
//using namespace Sampler;

//typedef map<int, double*> int2DblMap;
void psr_sysdyn::learn() {
  // ---------------
  // -  DISCOVERY  -
  // ---------------
  ofstream saveData  ("saveData.txt");                  //刚添加的，目的是为了将获取到的数据保存在 saveData.txt 中
  ofstream saveStep  ("steps.txt");
  ofstream paraData      ("data.txt");                   //输出学习好后的各个参数
  ofstream stateData ("stateData.txt");
  ofstream resultData("resultData.txt");

  symSet *tests, *lastTests = new symSet(), *histories, *augHistories, *eraseTest = new symSet(), *tempHistories = new symSet(), *tempAugHistories = new symSet();

  int k=0, oldK=0, testk=0, oldRowK=0, rowK=1;        //注意 rowK 是 1
  int columnNum, actCol;
  
  state_size = 11;                                    //针对 cheese maze 环境, 共有 11 个状态

  Matrix<double> *matrix_mao;                         //代表 mao 的矩阵
  int* columnsToRem;
  Array2D<int> *actcounts, *tempActcounts;
  
  tEstimateData *P;                                   //oldP 是新填加上去的
  vector<Symbol*>* conData;
  symSet* landmarkSet;
  //下面的定义是为检验数据部分而定义的
  Array1D<double> *currState = new Array1D<double>(state_size, 0.0);   //POMDP 模型的当前状态
  Array1D<double> *currProb  = new Array1D<double>(state_size, 0.0);   //用于 POMDP 模型概率计算的矩阵
  Array1D<double> *tempProb  = new Array1D<double>(state_size, 0.0);   //用于 POMDP 模型概率计算的矩阵
  Array3D<double> *stateTran = new Array3D<double>(env->nrActions(), state_size, state_size, 0.0); //状态转移矩阵
  Array2D<double> *obsTran   = new Array2D<double>(state_size, env->nrObservs(), 0.0);//观测矩阵,注意其为2维,并且列为 obs
  Array2D<double> *probsD,                            //表示的是 p(Qt/Qh) 矩阵
  inverse_probsD;                                     //表示的是 p(Qt/Qh) 的逆阵
  Array2D<double> *statesProb;                        //存放的是每一个 state-history 对应的 core-test 的值
  Array1D<double> *firstProb;                         //存放初始状态的 core-test 的值,
  
  symSet *qtests;                                     //对应的是Q tests
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
  initPomdp(stateTran, obsTran);                      //初始化 pomdp
  //此处加一循环语句,同时将 continueData 初始化,将原来在构造函数里的初始化去掉,在循环的最后将 continueData 删除
  while(iterations <= TEST_LENGTH)                    //从 1000 开始,每次递增1000,因此该式表示取 100 个点
  {
    saveData<<"Before continueData intia"<<endl;
    continueData = new vector<Symbol*>;
    continueData->reserve(iterations+100);
    conData = getData(iterations);                     //已做了改变
    //cout<<"end of getData"<<endl;
    saveData << "\n\n === new iteration === \n\n";
    augHistories = new symSet();
  
    //针对固定长度的经历， tests 需为空集，同时在 sampler.cpp 中构造 histLength 长度的 test
    tests = new symSet();
    tests->insert(new symList());
    //cout<<"Before break"<<endl;
    //break;
    //testLength = 2;
    cout<<"Before augEstimate"<<endl;
    P = augEstimate(tests, env, conData, iterations, histLength, testLength, true, NULL); 
    cout<<"Before landmarkSet"<<endl;
    landmarkSet = getLandmark(P, 1);                  //后面的 1 这个参数要重新考虑一下
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
    //break;//临时添加
    while(1)
    {
      //在获取 landmark 后，获得经历及其一步扩展
      testLength++;
      P = augEstimate(tests, env, conData, iterations, histLength, testLength, false,landmarkSet); 
      oldK = k;
      k = getRank(P);
      /**************************************************************************
       *****************************列的移除部分*********************************
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

      //下面对 ctMatP 和 ctProbP 里面的对于某一列，如果其对应所有行的元素都为零，则将其删去
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
      probsP  = new Array2D<double>(size_m, size_n);     //对应概率
      countsP = new Array2D<int>(size_m, size_n);        //对应数值
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
	tests->insert(iT->first);                       //test 中存放的是将零向量列删除后的检验,和 augTest 不同
	eraseTest->insert(iT->first);
      }
      int     rowRemIndex, columnRemIndex, leftRow, leftColumn;
      int     bestValue, tempValue;
      int     bvIndex       = -1;
      int     probRowSize, probColumnSize;
      Array2D<double> *probLeft;                      //存放的是相关列移除后的数据
      int *columnsToRem = (int*)malloc(size_n * sizeof(int)); //列的移除
      tiH = P->ctAct->begin();
      size_act = tiH->second->size();
      actcounts     = new Array2D<int>(size_m, size_act);
      tempActcounts = new Array2D<int>(size_m, size_act);
      for(i=0; i<size_m; i++)                         //完成两个矩阵的初始化
	for(j=0; j<size_act; j++)
	  (*actcounts)[i][j]  =(*tempActcounts)[i][j] = (*P->actcounts)[i][j];
      for(i = 0; i<size_n; i++)                       //这个地方所做的工作是将其初始化为所对应的列的列数
	columnsToRem[i] = i;                         //先将数组初始化，元素按照从左到右按从小到大的顺序排列       
      //double cond, bestcond=DBL_MAX;
      rowRemIndex = 0;
      leftRow = leftColumn = probRowSize = 0;
      probColumnSize    = size_n -1;
      columnRemIndex    = size_n -1;
      if(size_n > k)
      {
	saveData<<"Now column removing"<<endl;
	int swapCol;
	while(columnRemIndex > -1){                     //下面的语句是将不会导致矩阵的秩减少的列移去 
	  probLeft = new Array2D<double>(size_m, probColumnSize);
	  leftRow = 0;
	  columnsToRem[columnRemIndex] = -1;
	  for(i=0, iH = ctMatP->begin(); iH != ctMatP->end(); i++, iH++)  //获取 probLeft 的数值
	  {
	    tH = iH;
	    leftColumn     = 0;
	    for(j=0, iT = tH->second->begin(); iT != tH->second->end(); j++, iT++)
	    {
	      if(j == columnRemIndex)                   //主要目的是将当前移除的列相对应的动作发生次数减掉
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
	
	  epsilon = 0.0;                                  //下面所做的工作是获取将相应的列移除后的 rank                    
	  for (i = 0, iH = ctMatP->begin(); iH != ctMatP->end(); i++, iH++) {  
	    for (j = 0, iT = iH->second->begin(); iT != iH->second->end(); j++, iT++) {
	      if(columnsToRem[j] == j)                  
	      {
		del = iT->first->filter(SYM_ACTION); 
		TH = (double) (*countsP)[i][j];
		for(actCol = 0, tiT = tiH->second->begin(); tiT != tiH->second->end(); actCol++, tiT++)
		  if(*del == *tiT->first)        //怎么会没有相等的情况
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
	  epsilon /= (probLeft->dim1() * probLeft->dim2());  //这才是要的 epsilon 的值
	  norm = infNorm(probLeft);                            //得到 norm 值
	  singular = new SVD<double>(*probLeft);              
	  Array1D<double> *singvals = new Array1D<double>();
	  singular->getSingularValues(*singvals);
	  delete singvals;
	  testk = singular->rank(epsilon*norm);         //下面的语句判断将相应的行和列移除后得到的 rank 是否和原来的相等
	  if(testk < k)                                 //矩阵的秩发生了变化，将其保留
	  {
	    for (i=0, iH=ctMatP->begin(); iH!=ctMatP->end(); i++, iH++) 
	      (*tempActcounts)[i][swapCol] = (*actcounts)[i][swapCol];
	    columnsToRem[columnRemIndex] = columnRemIndex;
	  }
	  else
	  {
	    probColumnSize--;                           //表示的是又将一列移除
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

      if(k != oldK)                                     //只有当需要循环时，才将空test添加，这一句在算法中要不要?
      {
	freeData(P);                                    //新添加         
	delete actcounts;                               //新添加
	delete tempActcounts;                           //新添加
	lastTests->clear();
	for(iCurr=tests->begin(); iCurr!=tests->end(); iCurr++)
	  lastTests->insert(new symList(*iCurr));
	tests->insert(new symList());          
	saveData << "Tests:\n";
	for (iCurr = tests->begin(); iCurr != tests->end(); iCurr++) saveData << "[" << **iCurr << "]\n";
      }
      else                                              //这个地方是当 k==oldk 时的情况
	break;
    }
    tests->clear();
    delete tests;
    free(columnsToRem);
    saveData<<"Now the discover part is done"<<endl;

    /***************************************************************************************************************
     *****************                           学习部分(8.24)            *******************************************
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
    ctMatP  = new symListMatrix();                      //其行和P中定义一致,列为 lastTests 对应之列
    ctProbP = new symListDblMat();                      //同上
    //重新初始化 ctMatP, ctProbP
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
      bestRows    = (int *) malloc((size_m-k)*sizeof(int)); //bestRows 表示的是最终根据条件数决定移除的行数
      rowToRem[0] = -1;
    }
    else                                                //只所以定义这一段，是为了后面编程统一的需要
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
      columnToRem = (int*)malloc(1*sizeof(int));        //同行的定义原因一样
    while(remRowIndex != -1)
    {
      rowToRem[remRowIndex]++;
      if(rowToRem[remRowIndex] == size_m)
      {
	remRowIndex--;
	continue;
      }
      remRowIndex++;
      if(remRowIndex == (size_m - k) || (size_m == k) )               //表示已经初始化好了 size_m-k 个行
      {
	if(size_n > k)                              //新添加
	  columnToRem[0] = -1;                   
	else                                        //新添加
	  columnToRem[0] = -2;                      //新添加
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
	    if (zeroRow && ( (size_n-k)!=0 ))       //这个地方要不要加上 size_n-k!=0 这条判断
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
	    for(i=0; i<size_m; i++)                         //完成两个矩阵的初始化
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
	    epsilon = 0.0;                          //获取条件数
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
	    norm = infNorm(smlArr);                            //得到 norm 值
	    //saveData << "Got norm: "<<norm<<endl;
	    //saveData << "  -> Cutoff value is "<<epsilon*norm<<endl; //得到阏值 Cutoff
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
    if(size_n > k)                                      //经过下面的操作后 ctMatP 中对应的元素为 P(Qi|Sh),Sh为state-history
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
    probsD = new Array2D<double>(k, k);                 //表示的是 p(Qt/Qh) 矩阵
    statesProb = new Array2D<double>(size_m, k, 0.0);   //存放的是每一个 state-history 对应的 core-test 的值
    firstProb  = new Array1D<double>(k, 0.0);           //存放初始状态的 core-test 的值,
    smlRow = 0;
    piH = ctProbP->begin(); 
    for(i=0, piT=piH->second->begin(); piT!=piH->second->end(); i++, piT++)//初始化 firstProb，上面的行移除算法可以保证
      (*firstProb)[i] = *(piT->second);                                    //第一行对应的为空历史的 core-test 的值
    for(i=0, piH=ctProbP->begin(); piH!=ctProbP->end(); i++, piH++) //初始化 stateMatrix
      for(j=0, piT=piH->second->begin(); piT!=piH->second->end(); j++, piT++)
	(*statesProb)[i][j] = *(piT->second);
    if(size_m > k)
      for(i=0, iCurr = augHistories->begin(), piH=ctProbP->begin(); iCurr != augHistories->end(), piH!=ctProbP->end(); i++, iCurr++, piH++)
	if(i == bestRows[i])
	{
	  P->ctMat->erase(*iCurr);                      //使得 P->ctMat 等对应的行仅为 Qh, 而不是 Sh
	  P->ctProb->erase(*iCurr);                     //注意从这个地方开始 ctMatP 等和 P->ctMat 中行开始不同
	  P->ctAct->erase(*iCurr);                      //对应于 P->ctAct 的行要不要删掉?
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
    delete(decomp);                                   //将其删除放到了最后
    delete IDENT;                                     //将其删除放到了最后
    //输出probsD和 inverse_probsD 的值
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

    curract = actlist;     //tests 中保存了所有一步的 ao        

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

    qtests = new symSet();                      //对应的是Q tests
    iH = ctMatP->begin();
    for(iT=iH->second->begin(); iT!=iH->second->end(); iT++)
      qtests->insert(new symList(iT->first));

    maoMatrix = new symList2VecMap;
    //symList2VecMap* rowDblMapQ;
    maoqMatrix = new symListVecMat; 
    //vector<double> *elementVec;
    saveData<<"inverseProb "<<"piH "<<"temp "<<endl;
    for(iCurr=tests->begin(); iCurr!=tests->end(); iCurr++)           //初始化 mao 
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
    for(i=0, iCurr=tests->begin(); iCurr!=tests->end(); i++, iCurr++) //初始化 maoq
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
    //输出 P->ctProb 中的值
    piH = P->ctProb->begin();
    //saveData<<"P->ctProb col size "<<piH->second->size()<<" row size "<<P->ctProb->size()<<" element"<<endl;
    //输出ctProbP中的内容
    paraData<<"ctProbP element"<<endl;
    piH = ctProbP->begin();
    for(piH=ctProbP->begin(); piH!=ctProbP->end(); piH++)
    {
      //paraData<<i<<" "<<*(piH->first)<<" ";
      for(j=0, piT=piH->second->begin(); piT!=piH->second->end(); j++, piT++)
	paraData<<*(piT->second)<<" ";
      paraData<<endl;
    }
    //输出maoMatrix中的内容
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
 ********************************数据测试部分 cheese maze,看可否转化为一个函数 **********
 ****************************************************************************************
 */ 
    //下面的代码是将两个矩阵初始化,是否应该设置一个函数?
    //intia
    int lmSize = (*landmarkSet->begin())->size();       //所有 landmark 已定为长度是一致的
    symList* startLandmark;
    startLandmark = getStartLandmark(conData,landmarkSet); 
//#if 0
    Symbol* actionToSend, *actionReceived;
    vector<Symbol*> *testData = new vector<Symbol*>;                      
    tempCoreValue = new Array1D<double>(k, 0.0);       //用于存放 p(Q|h)的值,下同
    coreValue = new Array1D<double>(k, 0.0);           //这两个矩阵连同 symSet 注意在测试部分完后要删除,因为规划部分也有
  
    int prob, currPos, envState, belState, obsId;       //其中 env_state 表示的是智能体当前所处状态的 ID,belState 信度状态
    double pomPre, psrPre, errSum, probSum, normal;                                //
    currPos = 1;
    while(1)
    {
      do
      {
	prob = (int) trunc((float) random() * (float) (env->nrActions()) / (float) RAND_MAX);
      } while (prob==env->nrActions()||(env->crumpus_open(prob)));    //crumpus_open 后来加上去的
      actionToSend = env->getAction(prob);       
      testData->push_back(actionToSend);
      actionReceived = env->sendAction(actionToSend); 
      testData->push_back(actionReceived);
      if( *(getMemory(testData, currPos, lmSize)) == *startLandmark ) //表示当前已经到了设定的初始状态了
      {
	envState = env->crumpus_stateid( );            //获取当前状态 id,用于 POMDP 模型
	//添加 envState 到 belState 的转换函数
	belState = getBelState(envState);
	break;     
      }                                     
      currPos += 2;                                     //因为始终要以观测值为获取的点
    }
    delete testData;
    for(i=0; i<state_size; i++)                       //初始信度状态
      (*currState)[i] = 0.0;
    (*currState)[belState] = 1.0;                     //初始信度状态
    for(i=0; i<k; i++)
      (*tempCoreValue)[i] = (*coreValue)[i] = (*firstProb)[i];
    errSum = 0.0;
    for(i=0; i<TEST_LENGTH; i++)                             //10000 可设为一个宏
    {
      do
      {
	prob = (int) trunc((float) random() * (float) (env->nrActions()) / (float) RAND_MAX);
      } while (prob==env->nrActions()||(env->crumpus_open(prob)));    //crumpus_open 后来加上去的
      actionToSend = env->getAction(prob);     
      //下面开始获取当前 PSR 模型和精确模型预测之间的差值
      for(m=0; m<state_size; m++)                     //得到 b(h)*T^a
      {
	probSum = 0.0;
	for(n=0; n<state_size; n++)
	  probSum += (*currState)[n] * (*stateTran)[prob][n][m];
	(*tempProb)[m] = probSum;
      }
      //针对的是所有的观测值,故此处要加一个 for 语句
      nextAoSet = new symSet();
      currobs = obslist;                            //此处的初始化切记要加上，要不然下一次循环的时候 currobs 为 NULL
      while ((currobs != NULL) && (currobs->s != NULL)) 
      { 
	nextAoSet->insert(new symList(actionToSend, new symList(new Symbol(currobs->s))));
	currobs = currobs->n;
      } // obs
      for(iCurr=nextAoSet->begin(); iCurr!=nextAoSet->end(); iCurr++)
      {
	obsId = (*iCurr)->n->s->id;                     //当前观测值的 ID 值
	for(m=0; m<state_size; m++)                     //得到 b(h)*T^a*O^ao,利用 O^ao 只有主对角线上有非零值的特性
	  (*currProb)[m] = (*tempProb)[m]*(*obsTran)[m][obsId];//注意 obsTran 对应 O,其每一列对应 O^o
	pomPre = 0.0;
	for(m=0; m<state_size; m++)                     //得到当前 pomdp 模型的预测值即真值
	  pomPre +=  (*currProb)[m];
	//下面的是 PSR 模型的预测值
	psrPre = 0.0;
	for(m=0; m<k; n++)                              //得到 psr 模型的预测值
	  psrPre += ((*coreValue)[m] * (maoMatrix->find(*iCurr)->second->at(m)));
	if(psrPre != pomPre)                           //表示预测的不相同
	  errSum += pow( (psrPre-pomPre), 2 );
      }
      delete nextAoSet;
      actionReceived = env->sendAction(actionToSend); 
      //下面所要做的是根据采取的动作值和得到的观测值更新 POMDP 模型和 PSR 模型
      normal = 0.0;
      obsId = actionToSend->id;
      for(m=0; m<state_size; m++)
      {
	(*currState)[m] = ( (*tempProb)[m]*(*obsTran)[m][obsId] ); //未归一化的值
	normal += (*currState)[m];                             //目的归一化
      }
      for(m=0; m<state_size; m++)
	(*currState)[m] /= normal;                             //采取动作得到观测值后的信度状态
    
      newAO = new symList(actionToSend, new symList(actionReceived, NULL));  //记得删掉
      denominator = 0;
      for(n=0; n<k; n++)
	denominator += ((*tempCoreValue)[n] * (maoMatrix->find(newAO)->second->at(n)));
      if(denominator == 0)                              //此处是否要加点预防措施
	for(m=0; m<k; m++)
	  (*coreValue)[m] = 0.0;
      else
      {
	for(m=0, viT=viH->second->begin(); viT!=viH->second->end(); m++, viT++) //获取当前时刻的 core-test 的值
	{
	  numerator   = 0;
	  for(n=0; n<k; n++)
	    numerator += (*tempCoreValue)[n] * (maoqMatrix->find(newAO)->second->find(viT->first)->second->at(n));
	  (*coreValue)[m] = (double)(numerator/denominator);
	  if( (*coreValue)[m] > 1 )
	    (*coreValue)[m] = 1;
	}
      }
      //tempCoreValue = coreValue->copy();              //可否用该式?copy 是如何用的?
      for(m=0; m<k; m++)
	(*tempCoreValue)[m] = (*coreValue)[m];         
      delete newAO;
    
    }
    delete tempCoreValue;
    delete coreValue;
    errSum /= (env->nrObservs() * TEST_LENGTH);          //其中 nrObservs 为观测值的数量,TEST_LENGTH 为检验数据的长度
    resultData<<errSum<<endl;                            //在 resultData 中输出错误值
    //由于后面的规划暂时不用,所以可以将下面的变量删掉
    delete ctMatP;
    delete ctProbP;
    delete probsD;
    delete statesProb;
    delete firstProb;
    delete qtests;
    delete maoMatrix;
    delete maoqMatrix;
    free(P);                                            //新添加201
    delete continueData;
    iterations += 1000;                                // while 结束的条件
  }//该处应该对应最前面的那个 while 语句
  delete currState;
  delete currProb;
  delete tempProb;
  delete stateTran;
  delete obsTran;
  
//#endif  

#if 0                                                 //先将规划部分去掉 2007.2.01
/****************************************************************************************
 *智能体的规划部分, 对于无复位系统,要重新设定,要以获取的奖励值作为判断的标准,要作修改****
 ****************************************************************************************
 */ 
  int stateId, goalId, nextStateId, id;                   //表示的所在的状态，距离判别法就是用来确定这个ID的
  int actionId;                                       //表示动作的ID
  int stateNum  = ctMatP->size();
  size_m = stateNum;
  int actionNum = NUM_ACTIONS;                        //本环境的动作数是 4,后面的工作中可将动作数改为一个传入变量
  Array2D<double> *Q = new Array2D<double>(stateNum, actionNum, 0.0);  //初始化 Q 值
  Array2D<double> *e = new Array2D<double>(stateNum, actionNum, 0.0);
  *tempCoreValue = new Array1D<double>(k, 0.0);
  *coreValue = new Array1D<double>(k, 0.0);
  
  int  steps;                                         //表示的是训练的次数
  int  reward;                                        //立即奖励值，每前进一步给予0， 到达终点给予10
  double  delta, gamma=0.9, alpha=0.125, lambda=0.9;
  int     action, observation, tempAction;
  Symbol  *actionSend, *observationReceived, *lastObservation;                //其中 observation 表示的是得到的观测值
  double sumNextQ, aoProb, eps;                      //分子，分母
  double distance, bestDistance;
  symList *interact, *tempInteract;           //原来定义的 newAO 放到头文件中定义
  //obslist = env->getObservations();                 //暂时去掉 2007.1.31 1151
  char* nn    = "observation";
  saveData<<"size_m "<<size_m<<"stateNum "<<stateNum<<endl;
  for(int runNum=0; runNum<NUM_RUNS; runNum++)         //一共训练的次数
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
      stateId = 0;                                    //从上面的搜索算法可以确保初始状态 sateId 为 0
      for(j=0; j<k; j++)
      	(*tempCoreValue)[j] = (*firstProb)[j];
      for(j=0; j<NUM_ACTIONS; j++)
	qValue[j] = (*Q)[stateId][j];
      do
      {
	action = selectAction(qValue, eps);
      }while(env->crumpus_open(action));              //判断action方向上是否是墙壁，如果是的话重新选择动作
      //saveData<<"Before while "<<endl;
      while(1)
      {
	//saveData<<interact->seekEnd()->s->id<<" "<<stateId<<" "<<action<<endl;//只能这儿出了问题
	steps++;
	actionSend = env->getAction(action);
	nextAoSet = new symSet();
	currobs = obslist;                            //此处的初始化切记要加上，要不然下一次循环的时候 currobs 为 NULL
	while ((currobs != NULL) && (currobs->s != NULL)) 
	{ 
	  //nextAoSet->insert(new symList((actionSend, new symList(currobs->s, NULL))));//貌似这样有问题
	  nextAoSet->insert(new symList(new Symbol(actionSend), new symList(new Symbol(currobs->s))));
	  currobs = currobs->n;
	} // obs
	sumNextQ = 0.0;
	for(iCurr=nextAoSet->begin(); iCurr!=nextAoSet->end(); iCurr++)
	{
	  tempInteract = new symList(interact);
	  tempInteract->append(new symList(*iCurr));
	  //saveData<<"Before ctProbP finding"<<endl;
	  if(ctProbP->find(tempInteract) != ctProbP->end())   //判断当前时刻所处状态的 core-test 的值
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
	      //for(m=0, iCurr2=qtests->begin(); iCurr2!=qtests->end(); m++, iCurr2++) //获取当前时刻的 core-test 的值
	      for(m=0, viT=viH->second->begin(); viT!=viH->second->end(); m++, viT++) //获取当前时刻的 core-test 的值
	      {
		numerator   = 0;
		for(n=0; n<k; n++)
		  numerator += (*tempCoreValue)[n] * (maoqMatrix->find(*iCurr)->second->find(viT->first)->second->at(n));
		(*coreValue)[m] = (double)(numerator/denominator);
		if( (*coreValue)[m] > 1 )
		  (*coreValue)[m] = 1;
	      }
	    }
	    //下面的是根据当前时刻 core-test 的值判断当前时刻属于哪个状态,用距离判别法
	    bestDistance = DBL_MAX;
	    for(m=0;m<size_m; m++)
	    {
	      distance = 0.0;
	      for(n=0; n<k; n++)                          //不用 pow(x,y) 的原因在于 pow 中 x 不能为零
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
	  if((*iCurr)->n->s->id == 7) //表示下一个状态为目标状态，或者判断 nextStateId是否和 goalId 相等,原来为 7
	  {
	    reward = 10;
	    sumNextQ += aoProb * (reward + gamma*0);
	  }
	  else
	  {
	    reward = 0;
	    for(m=0; m<NUM_ACTIONS; m++)
	      qValue[m] = (*Q)[nextStateId][m];
	    sumNextQ += aoProb *(reward + gamma*((*Q)[nextStateId][argmax(qValue)]));//用的是 Q 学习吧
	  }
	  delete tempInteract;
	  //saveData<<"After tempInteract deleting"<<endl;
	}
	//saveData<<"Before delete nextAoSet"<<endl;
	delete nextAoSet;
	//delta = sumNextQ - (*Q)[stateId][action]; //注意这里是 stateId 而不是 nextstateId,去掉 reward
	(*Q)[stateId][action] = sumNextQ;

	//saveData<<"Before get observationReceived"<<endl;
	observationReceived = env->sendAction(actionSend);
	id = observationReceived->id;
	if(env->crumpus_goal() || (steps>MAX_STEPS))                         //判断是否到达目标状态
	{
	  saveStep<<i<<" "<<steps<<endl;
	  delete lastObservation;
	  break;
	}
	
	//下面表明智能体不是撞墙并且所到达状态非初始状态，因为智能体的初始，目标状态是确定的，同时也知道是否撞墙
	//应该来说在精确获取环境模型的前提下，下面所有的判断都是不需要的，先临时保留，以后去掉实验
	//if( (observationReceived != lastObservation)&&(observationReceived != env->getObservation(0)) )//此处有问题
	if( (observationReceived->id != lastObservation->id)&&(observationReceived->id != 0) )
	{
	  delete lastObservation;
	  lastObservation = new Symbol(id, nn,  SYM_OBSERVATION);
	  //saveData<<"After crumpus_goal"<<endl;
	  newAO = new symList(new Symbol(actionSend), new symList(new Symbol(observationReceived)));
	  interact->append(new symList(newAO));
	  if(ctProbP->find(interact) != ctProbP->end())   //如果当前所经历的状态和 ctMatP 中有相同的地方，则将值复位
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
	      //for(m=0, iCurr2=qtests->begin(); iCurr2!=qtests->end(); m++, iCurr2++) //获取当前时刻的 core-test 的值
	      for(m=0,viT=viH->second->begin();viT!=viH->second->end(); m++, viT++) //获取当前时刻的 core-test 的值
	      {
		numerator   = 0;
		for(n=0; n<k; n++)
		  numerator   += (*tempCoreValue)[n] * (maoqMatrix->find(newAO)->second->find(viT->first)->second->at(n));
		(*coreValue)[m] = numerator/denominator;
		if((*coreValue)[m] > 1)
		  (*coreValue)[m] = 1;
	      }
	    }
	    //下面的是根据当前时刻 core-test 的值判断当前时刻属于哪个状态,用距离判别法
	    bestDistance = DBL_MAX;
	    for(m=0;m<size_m; m++)
	    {
	      distance = 0.0;
	      for(n=0; n<k; n++)                          //不用 pow(x,y) 的原因在于 pow 中 x 不能为零
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
	    (*tempCoreValue)[m] = (*statesProb)[stateId][m]; //将 tempCoreValue 的值用 statesProb 里面的值复位     
	}
	else
	  if(observationReceived->id == 0)            //表示智能体所在状态为初始状态
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

int   psr_sysdyn::selectAction(double qValue[NUM_ACTIONS], double p) //在状态 sta 时判断选择哪个动作
{
  int action=argmax(qValue);
  if(withProbability(p))
    action=rand()%NUM_ACTIONS;
  return action;
}

bool  psr_sysdyn::withProbability( double p )             //表示事件发生的概率为 p, 用于 epsion-greedy 策略
{
  return p > ((float)rand()) / RAND_MAX;                  //其中rand()返回的数值为0-RAND_MAX
}

int   psr_sysdyn::argmax(double qValue[NUM_ACTIONS])       //选择会获取最大励值的动作
{
  int best_action = 0;
  float best_value = qValue[0];
  int num_ties = 1;
  for ( int a = 1; a < NUM_ACTIONS; a++ )
  {
    float value = qValue[a];
    if ( value > best_value)                         //判断Q[NUM_ACTIONS]中最大的Q值对应的索引。
    {
      best_value = value;
      best_action = a;
    }
    else if( value == best_value )                                              //下面的代码表示什么意思。
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

void  psr_sysdyn::decayTraces( float decay_rate )         //缩减 trace 用 
{
}

int   psr_sysdyn::step(double qValue[NUM_ACTIONS])         //每一步执行的程序
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
    } while (prob==env->nrActions()||(env->crumpus_open(prob)));    //crumpus_open 后来加上去的
    actionToSend = env->getAction(prob);       
    continueData->push_back(actionToSend);
    actionReceived = env->sendAction(actionToSend); 
    continueData->push_back(actionReceived);
  }
  return continueData;
}
symSet* psr_sysdyn::getLandmark(tEstimateData* matData, int markLen)//先只考虑 markLen 为1和2时的情况，其余情况待考虑
{
  symSet* landmarkSet = new symSet();
  tEstimateData* tempMatData; 
  symList* tempList;
  symList	*actlist = env->getActions(), *curract,
  *obslist = env->getObservations(), *currobs;
  iH = matData->ctMat->begin();                           //初始化 augTest
  int rank;
  if(markLen == 2)                                    //以动作-观测值对作为 landmark,两种情况代码可缩减
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
	if( rank==1 )                 //表示当前的 tempList 为landmark
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
  else if(markLen == 1)                                  //仅仅以观测值作为 landmark
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
      if( rank==1 )                 //表示当前的 tempList 为landmark
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
  iH = matData->ctMat->begin();                           //初始化 augTest
  for (i=0, iT = iH->second->begin(); iT != iH->second->end(); i++, iT++) 
    augTest->insert(iT->first);

  for(i=0, iH = matData->ctMat->begin(); iH != matData->ctMat->end(); i++, iH++)//初始化 ctMatP 和 ctProbP
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
  for(i=0, tiH = matData->ctAct->begin(); tiH != matData->ctAct->end(); i++, tiH++)//初始化ctActP
  {
    row = (*matData->actcounts)[i];
    rowMap = new symList2IntMap();
    for(tiT = tiH->second->begin(); tiT != tiH->second->end(); tiT++)
      rowMap->insert(pair<symList*, int*>(tiT->first, new int(*(row++))));
    ctActP->insert(pair<symList*, symList2IntMap*>(tiH->first, rowMap));
  }

  //下面对 ctMatP 和 ctProbP 里面的对于某一列，如果其对应所有行的元素都为零，则将其删去
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
  //输出 test
  piH=ctProbP->begin();
  for(piT=piH->second->begin(); piT!=piH->second->end(); piT++)
    cout<<*piT->first<<" ";
  cout<<endl;
 //输出 matData->ctProb 中的值
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
  probsP  = new Array2D<double>(size_m, size_n);     //对应概率
  countsP = new Array2D<int>(size_m, size_n);        //对应数值
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
  Array2D<double> *rowIndep     = new Array2D<double>(2, size_n);     //用来存放用来判断是否线性相关的两个向量的
  int *hcounts      = (int*)malloc(size_m * sizeof(int));
  for(i = 0;i < size_m; i++)                          
  {
    rowsReadyRem[i] = -1;                             //首先将所有的元素初始化为-1
    hcounts[i] = (*matData->hcounts)[i];              //如果两个行向量线性相关，用来确定移除哪一行的
  }
  rowsReadyRem[0] = 0;
  for(i=1; i<size_m; i++)                             //将下一行和上面已经判断为两两线性无关的的行向量相
  {                                                   //比较，用来确定当前这一行是否和前面已判断过的行向量线性无关
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
	norm = infNorm(rowIndep);                            //得到 norm 值
	cutoffValue = epsilon*norm;
	singular = new SVD<double>(*rowIndep);              
	Array1D<double> *singvals = new Array1D<double>();
	singular->getSingularValues(*singvals);
	rowIndepK = singular->rank(epsilon*norm);
	delete singvals;
	delete(singular);
	//saveData << "RowIndep rank is: " <<rowIndepK << endl;
	if(rowIndepK==2)                                    //表示要判断的行和当前行线性无关, 将第 i 行所有元素插入
	  rowsReadyRem[i] = i;
	else //if(rowIndepK==1)                                       //表示要判断的行和当前行线性相关
	{
	  if(hcounts[i] <=  hcounts[j])
	    rowsReadyRem[i] = -1;      
	  else                                      //表示如果两行线性相关，将发生次数多的表示为线性无关
	  {
	    rowsReadyRem[i] = i;
	    rowsReadyRem[j] = -1;
	  }
	  break;
	}

      }
    }
	
  }//上面得到的结果是 rowsReadyRem中不为-1的行两两线性无关,同样不为-1的列线性无关，但列不是两两线性无关
  
  free(hcounts);
  delete rowIndep;
  
  delete probsP;
  delete countsP;
  //saveData << "Histories:\n";
  eraseHistories = new symSet();
  for (iH=ctMatP->begin(); iH!=ctMatP->end(); iH++)
    eraseHistories->insert(iH->first);

  //下面将跟 rowsReadyRem 中 -1 对应的行从 P->ctMat 等中移去
  for(i=0, iCurr=eraseHistories->begin(); i<size_m,iCurr!=eraseHistories->end(); i++, iCurr++)
  {
    if(rowsReadyRem[i] == -1)                       //下面的 erase 方法应该会导致内存泄漏问题
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
   *****************************列的判断部分*********************************
   **************************************************************************/
  //saveData<<"now is column removing "<<endl;
  size_m = ctMatP->size();
  size_act = ctActP->begin()->second->size();
  probsP  = new Array2D<double>(size_m, size_n);     //对应概率
  countsP = new Array2D<int>(size_m, size_n);        //对应数值
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
  //下面对 ctMatP 和 ctProbP 里面的对于某一列，如果其对应所有行的元素都为零，则将其删去
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
  probsP  = new Array2D<double>(size_m, size_n);     //对应概率
  countsP = new Array2D<int>(size_m, size_n);        //对应数值
  for(i=0, piH=ctProbP->begin(), iH = ctMatP->begin(); piH != ctProbP->end(), iH != ctMatP->end(); i++, piH++, iH++)
    for(j=0, piT = piH->second->begin(), iT = iH->second->begin(); piT != piH->second->end(), iT != iH->second->end(); j++, piT++, iT++)
    {
      (*probsP)[i][j]  = *(piT->second);
      (*countsP)[i][j] = *(iT->second);
    }
    
  tiH = ctActP->begin();
  int rowRemainNum = 0;
  // estimate the rank                              //此矩阵的秩
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
  norm = infNorm(probsP);                            //得到 norm 值
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
    if(iH->first->endWith(tempList))            //将 tempMatData 中结束于 tempList 的经历取出来
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
      tempMatData->ctAct->insert(pair<symList*, symList2IntMap*>(iH->first, rowMap));//注意是 iH 而不是 tiH
      tempMatData->ctHists->insert(pair<symList*, int*>(iH->first, &(*matData->hcounts)[i] ));
    }
  }
  size_m = tempMatData->ctMat->size();
  size_n = tempMatData->ctMat->begin()->second->size();
  size_act = tempMatData->ctAct->begin()->second->size();
  tempMatData->probs  = new Array2D<double>(size_m, size_n);     //对应概率
  tempMatData->counts = new Array2D<int>(size_m, size_n);        //对应数值
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
    matData->ctHists->erase(iT);  //之所以要定义 tiT 的原因是 erase 掉一个内容后，游标指向开头位置
    iT = tiT;
  }
  delete matData->ctHists;
  free(matData);

}
int psr_sysdyn::getBelState(int envState)   //envState->belState 的转换函数
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
void psr_sysdyn::initPomdp(Array3D<double>* stateTra, Array2D<double>* obsTra) //假定转移到除了选择之外的动作的概率为 0.3
{
  for(i=0; i<=10; i++)                                //完成所有对角线上的初始化
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
  //下面是观测值矩阵
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
