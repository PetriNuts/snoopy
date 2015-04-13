//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: slehrack $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2007/07/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#ifndef __SP_PRIORITY_QUEUE_H__
#define __SP_PRIORITY_QUEUE_H__

#include <queue>

using namespace std;

template < class T > class SP_PriorityQueue : public priority_queue< T >
{
};

#endif // __SP_PRIORITY_QUEUE_H__
