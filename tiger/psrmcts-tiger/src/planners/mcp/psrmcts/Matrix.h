// Matrix.h: interface for the CMatrix class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MATRIX_H__ACEC32EA_5254_4C23_A8BD_12F9220EF43A__INCLUDED_)
#define AFX_MATRIX_H__ACEC32EA_5254_4C23_A8BD_12F9220EF43A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <math.h>
#include <string>
#include <cstdint>

#if !defined(_BITSET_)
#include <bitset>
#endif // !defined(_BITSET_)
using namespace std;
///////////////////////////////////////////////////////////////////////////////
///////
//
//(-- class CTokenizer
//

//
//--) // class CTokenizer
//
///////////////////////////////////////////////////////////////////////////////
///////

///////////////////////////////////////////////////////////////////////////////
///////
//
//(-- class CMatrix
//
class CMatrix  
{
	//
	// 
	//
public:
	
	//
	// 
	//
	
	
	CMatrix();                                                                     
	// 
	CMatrix(int nRows, int nCols);                                  // 
//	
        CMatrix(int nRows, int nCols, double value[]);  // 
	CMatrix(int 
		nSize);                                                             // 
//	
        CMatrix(int nSize, double value[]);                             // 
//	
        CMatrix(const CMatrix& other);                                  // 
//	
        bool    Init(int nRows, int nCols);                             // 
//	   
        bool    MakeUnitMatrix(int nSize);                              // 
//	
        virtual 
		~CMatrix();                                                             // 
//	
		
        //
        // 
        //
		
        // 
        bool FromString(string s, const string& sDelim = " ", bool 
		bLineBreak = true);        
	// 
	string ToString(const string& sDelim = " ", bool bLineBreak = true) 
		const;
	// 
	string RowToString(int nRow, const string& sDelim = " ") const;
	// 
	string ColToString(int nCol, const string& sDelim = " ") const;
	
	//
	// 
	//
	
	bool    SetElement(int nRow, int nCol, double value);   // 
    double  GetElement(int nRow, int nCol) const;                   // 
    void    SetData(double value[]);                                                // 
	int     GetNumColumns() const;
		                                                  // 
	int     GetNumRows() const; 
		                                                            // 
    int     GetRowVector(int nRow, double* pVector) const;  // 
	
    int     GetColVector(int nCol, double* pVector) const;  // 
	
    double* GetData() const; 
		                                                               // 
	
		
        //
        // 
        //
		
    CMatrix& operator=(const CMatrix& other);
	bool operator==(const CMatrix& other) const;
	bool operator!=(const CMatrix& other) const;
	CMatrix operator+(const CMatrix& other) const;
	CMatrix operator-(const CMatrix& other) const;
	CMatrix operator*(double value) const;
	CMatrix operator*(const CMatrix& other) const;
	// 
	bool CMul(const CMatrix& AR, const CMatrix& AI, const CMatrix& BR, 
		const CMatrix& BI, CMatrix& CR, CMatrix& CI) const;
	// 
	CMatrix Transpose() const;
	
	//
	// 
	//
	
	// 
	bool 
		InvertGaussJordan();                                               
	// 
	bool InvertGaussJordan(CMatrix& 
		mtxImag);                                 
	// 
	bool InvertSsgj();                                              
	// 
	bool 
		InvertTrench();                                                    
	// 
	double 
		DetGauss();                                                              
	// 
	int RankGauss();
	// 
	bool DetCholesky(double* 
		dblDet);                                                               
	// 
	bool SplitLU(CMatrix& mtxL, CMatrix& 
		mtxU);                                     
	// 
	bool SplitQR(CMatrix& 
		mtxQ);                                                      
	// 
	bool SplitUV(CMatrix& mtxU, CMatrix& mtxV, double eps = 
		0.000001);                                       
	// 
	bool GInvertUV(CMatrix& mtxAP, CMatrix& mtxU, CMatrix& mtxV, double 
		eps = 0.000001);
	// 
	bool MakeSymTri(CMatrix& mtxQ, CMatrix& mtxT, double dblB[], double 
		dblC[]);
	// 
	bool SymTriEigenv(double dblB[], double dblC[], CMatrix& mtxQ, int 
		nMaxIt = 60, double eps = 0.000001);
	// 
	void MakeHberg();
	// 
	bool HBergEigenv(double dblU[], double dblV[], int nMaxIt = 60, 
		double eps = 0.000001);
	// 
	bool JacobiEigenv(double dblEigenValue[], CMatrix& mtxEigenVector, 
		int nMaxIt = 60, double eps = 0.000001);
	// 
	bool JacobiEigenv2(double dblEigenValue[], CMatrix& mtxEigenVector, 
		double eps = 0.000001);
	
	//
	// 
	//
protected:
	int     m_nNumColumns;                  // 
	int     m_nNumRows;                             // 
	double* m_pData;                        // 
	
	//
	// 
	//
private:
	void ppp(double a[], double e[], double s[], double v[], int m, int 
		n);
	void sss(double fg[2], double cs[2]);
	
};
//
//--) // class CMatrix
//
///////////////////////////////////////////////////////////////////////////////
///////

#endif // !defined(AFX_MATRIX_H__ACEC32EA_5254_4C23_A8BD_12F9220EF43A__INCLUDED_)
