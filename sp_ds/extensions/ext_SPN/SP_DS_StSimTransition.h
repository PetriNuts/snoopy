//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: slehrack $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2007/07/12 09:35:00 $
// Short Description:
//////////////////////////////////////
#ifndef __SP_DS_STSIMTRANSITION_H__
#define __SP_DS_STSIMTRANSITION_H__

#include "sp_utilities.h"
#include "sp_ds/extensions/ext_SPN/SP_DS_StSimTimeEvent.h"
#include "sp_ds/extensions/ext_SPN/SP_DS_StParser.h"

class SP_DS_StSimTransition {

  public :

    SP_DS_StSimTransition();
    SP_DS_StSimTransition(const SP_DS_StSimTransition& p_Copy);

    virtual ~SP_DS_StSimTransition();

    long arrayIndex;
	long hazardIndex;
	bool nonConsuming;

    SP_VectorLong manipulatedPlacePos;
    SP_VectorDouble deltaMarking;
    SP_VectorBool resetMarking;
    SP_VectorLong basedPlacePos;
    SP_VectorDouble intervalLowerBound;
    SP_VectorDouble intervalUpperBound;
    SP_VectorLong toUpdateTransitionPos;

	double hazardValue;
	unsigned long cellCount;
    SP_DS_StParserNode* hazardTree;
    SP_DS_StSimHazardFunctionType hazardFunctionType;

    SP_DS_StSimTimeEvent* timeEvent;

  private:
	void FreeTree( SP_DS_StParserNode* p_pcTree );


};

typedef vector<SP_DS_StSimTransition*> SP_VectorSimTransition;
typedef deque<SP_DS_StSimTransition*> SP_DequeSimTransition;

#endif
