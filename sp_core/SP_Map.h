//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_MAP_H__
#define __SP_MAP_H__

#include <map>

using namespace std;

class wxString;

typedef map<unsigned int, unsigned int> SP_MapUInt2UInt;
typedef map<wxString, wxString> SP_MapString2String;
typedef map<wxString, bool> SP_MapString2Bool;
typedef map<wxString, int> SP_MapString2Int;
typedef map<wxString, long> SP_MapString2Long;
typedef map<wxString, unsigned int> SP_MapString2UInt;
typedef map<wxString, unsigned long> SP_MapString2ULong;
typedef map<wxString, double> SP_MapString2Double;
typedef map<int, wxString> SP_MapInt2String;
typedef map<long, wxString> SP_MapLong2String;
typedef map<unsigned int, wxString> SP_MapUInt2String;
typedef map<unsigned long, wxString> SP_MapULong2String;
typedef map<int, int> SP_MapInt2Int;
typedef map<int, double> SP_MapInt2Double;
typedef map<long, long> SP_MapLong2Long;
typedef map<long, double> SP_MapLong2Double;
typedef map<unsigned long, double> SP_MapULong2Double;

typedef map<unsigned int, SP_MapUInt2String> SP_MapUInt2DString;
typedef map<unsigned long, SP_MapULong2String> SP_MapULong2DString;
typedef map<unsigned long, SP_MapULong2Double> SP_MapULong2DDouble;
typedef map<long, SP_MapLong2Double> SP_MapLong2DDouble;

/*
template <class T, class V> class map: public map<T, V>
{
};
*/

#endif // __SP_LIST_H__
