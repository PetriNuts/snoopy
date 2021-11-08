//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_VECTOR_H__
#define __SP_VECTOR_H__

#include <vector>
#include <string>
using namespace std;

class wxString;

typedef vector<wxString> SP_VectorString;
typedef vector<std::string> SP_VectorStdString;//by george for spsim
typedef vector<double> SP_VectorDouble;
typedef vector<long> SP_VectorLong;
typedef vector<unsigned long> SP_VectorULong;
typedef vector<bool> SP_VectorBool;
typedef vector<short> SP_VectorShort;
typedef vector<int> SP_VectorInt;//george2021

typedef vector< SP_VectorString > SP_Vector2DString;
typedef vector< SP_VectorDouble > SP_Vector2DDouble;
typedef vector< SP_VectorLong > SP_Vector2DLong;
typedef vector< SP_VectorULong > SP_Vector2DULong;
typedef vector< SP_VectorBool > SP_Vector2DBool;
typedef vector< SP_VectorInt > SP_Vector2DInt;//george2021
struct TraceElement {//Added by G.A
	std::vector<double> sample;
	double currentLevel;
	int levelNum;
	SP_Vector2DDouble fuzzyTrace;

};
typedef std::vector<TraceElement>  ResultFuzzyBand;
/*
template <class T> class SP_Vector: public vector<T>
{
};
*/

#endif // __SP_VECTOR_H__
