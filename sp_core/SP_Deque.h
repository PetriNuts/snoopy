//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: cr $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2009/06/29 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DEQUE_H__
#define __SP_DEQUE_H__

#include <deque>

using namespace std;

class wxString;

typedef deque<wxString> SP_DequeString;
typedef deque<double> SP_DequeDouble;
typedef deque<long> SP_DequeLong;
typedef deque<unsigned long> SP_DequeULong;
typedef deque<bool> SP_DequeBool;

/*
template <class T> class SP_Deque: public deque<T>
{
};
*/

#endif // __SP_DEQUE_H__
