//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: akurth $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2006/08/07 10:00:00 $
// Short Description:  Determine of all cut sets of a fault tree
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_FTREECUTSETSCREATER_H__
#define __SP_DS_FTREECUTSETSCREATER_H__

#include "sp_ds/extensions/FTree/SP_DS_FTreeDNFCreater.h"
#include <wx/wx.h>

	/**
	\brief Find all cut sets on the basis of the disjunctive normalform.
	*/

class SP_DS_FTreeCutSetsCreater: public SP_DS_FTreeDNFCreater
{
private:
 
	//list of cut sets
	list<SP_ListNode*>* cutSets;

protected:
	
	/**
	\brief Find cut sets
	Searching the nodes of the fault tree which belongs to the name of the literals .
	
	*/
	void FindAllCutSets();


public:
    SP_DS_FTreeCutSetsCreater(SP_DS_Graph* p_pcGraph);

    ~SP_DS_FTreeCutSetsCreater();

	/**
	\brief Select all cut sets of the fault tree
	
	\param		mMap		logical expression
	\returns	dnfForm		disjunctive normalform
	*/
	SP_ListString* SelectCutSets(LiteralMap mMap);
	
	//returns list of cut sets of the fault tree
	list<SP_ListNode*>* GetCutSets();
};

#endif // __SP_DS_FTREECUTSETSCREATER_H__


