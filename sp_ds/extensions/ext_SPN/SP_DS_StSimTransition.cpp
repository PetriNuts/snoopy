//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: slehrack $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2007/07/12 09:35:00 $
// Short Description:
//////////////////////////////////////
#include "SP_DS_StSimTransition.h"


SP_DS_StSimTransition::SP_DS_StSimTransition() :
	arrayIndex( 0), hazardIndex(0), nonConsuming(true), hazardValue(0), cellCount(0), hazardTree( 0),
	timeEvent(0)
{
	timeEvent = new SP_DS_StSimTimeEvent();
}

SP_DS_StSimTransition::SP_DS_StSimTransition(const SP_DS_StSimTransition& p_Copy):
	arrayIndex(p_Copy.arrayIndex),
	hazardIndex(p_Copy.hazardIndex),
	nonConsuming(p_Copy.nonConsuming),
	manipulatedPlacePos(p_Copy.manipulatedPlacePos),
	deltaMarking(p_Copy.deltaMarking),
	resetMarking(p_Copy.resetMarking),
	basedPlacePos(p_Copy.basedPlacePos),
	intervalLowerBound(p_Copy.intervalLowerBound),
	intervalUpperBound(p_Copy.intervalUpperBound),
	toUpdateTransitionPos(p_Copy.toUpdateTransitionPos),
	hazardValue(p_Copy.hazardValue),
	cellCount(p_Copy.cellCount),
	hazardTree(NULL),
	hazardFunctionType(p_Copy.hazardFunctionType),
	timeEvent(new SP_DS_StSimTimeEvent(*(p_Copy.timeEvent)))
{
}

SP_DS_StSimTransition::~SP_DS_StSimTransition()
{
	FreeTree(hazardTree);

	wxDELETE(timeEvent);
}

void
SP_DS_StSimTransition::FreeTree( SP_DS_StParserNode* p_pcTree )
{

	if(p_pcTree->left)
		FreeTree(p_pcTree->left);

	if(p_pcTree->middle)
		FreeTree(p_pcTree->middle);

	if(p_pcTree->right)
		FreeTree(p_pcTree->right);

	wxDELETE(p_pcTree);
}


