//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: slehrack $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2007/07/12 09:35:00 $
// Short Description:
//////////////////////////////////////
#ifndef __SP_DS_STANIMATIONRUNELEM_H__
#define __SP_DS_STANIMATIONRUNELEM_H__

#include "sp_utilities.h"

typedef enum {

  SP_ST_ANIM_RUN_FIRE_TRANSITION,
  SP_ST_ANIM_RUN_EVENT

} SP_DS_StAnimationRunElemType;


class SP_DS_StAnimationRunElem
{

  public :

    SP_DS_StAnimationRunElem();

    virtual ~SP_DS_StAnimationRunElem();
    double timepoint;
    SP_DS_StAnimationRunElem* next;
    SP_DS_StAnimationRunElem* prev;
    long transitionArrayPos;
    SP_ListString* actionDescription;

    SP_DS_StAnimationRunElemType type;

};


#endif
