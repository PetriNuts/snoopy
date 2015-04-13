//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: slehrack $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2007/07/12 09:35:00 $
// Short Description:
//////////////////////////////////////
#include "SP_DS_StAnimationRunElem.h"

SP_DS_StAnimationRunElem::SP_DS_StAnimationRunElem() :
	timepoint( 0), next( 0), prev( 0), transitionArrayPos( 0),
			actionDescription( 0)
{
	actionDescription = new SP_ListString();
}

SP_DS_StAnimationRunElem::~SP_DS_StAnimationRunElem()
{

	wxDELETE(prev);
	wxDELETE(next);

	actionDescription->clear();

	wxDELETE(actionDescription);

}
