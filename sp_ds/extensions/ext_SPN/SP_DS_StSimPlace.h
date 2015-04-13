//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: slehrack $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2007/07/12 09:35:00 $
// Short Description:
//////////////////////////////////////
#ifndef __SP_DS_STSIMPLACE_H__
#define __SP_DS_STSIMPLACE_H__

#include "sp_utilities.h"

class SP_DS_StSimPlace {

  public :
	  SP_DS_StSimPlace();

	  virtual ~SP_DS_StSimPlace();

	  long arrayIndex;

	  SP_VectorLong toUpdateTransitionPos;

};

typedef vector<SP_DS_StSimPlace*> SP_VectorSimPlace;

#endif
