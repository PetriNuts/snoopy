//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_LIST_H__
#define __SP_LIST_H__

#include <list>

using namespace std;

class wxString;

typedef list<wxString> SP_ListString;
typedef list<long> SP_ListLong;
typedef list<double> SP_ListDouble;

/*
template <class T> class SP_List: public list<T>
{
public:
    typename list<T>::const_iterator find(const T& p_Param) const
    {
        typename list<T>::const_iterator l_itElem;
        for (l_itElem = this->begin(); l_itElem != this->end(); ++l_itElem)
        {
            if ((*l_itElem) == p_Param)
                return l_itElem;
        }
        return this->end();
    }
};
*/
#endif // __SP_LIST_H__
