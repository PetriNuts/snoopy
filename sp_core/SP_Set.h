//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_SET_H__
#define __SP_SET_H__

#include <set>

using namespace std;

class wxString;

typedef set<wxString> SP_SetString;
typedef set<int> SP_SetInt;
typedef set<unsigned int> SP_SetUInt;
typedef set<long> SP_SetLong;
typedef set<unsigned long> SP_SetULong;

/*
template <typename T> class SP_Set: public set<T>
{
};
*/

#endif // __SP_SET_H__
