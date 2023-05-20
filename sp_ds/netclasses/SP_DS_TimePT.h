//////////////////////////////////////////////////////////////////////
// $Author: r2 $
// $Version: 0.1 $
// $Date: 2007/01/26 $
// Short Description: PT nets with time extensions
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_TIMEPT_H__
#define __SP_DS_TIMEPT_H__

#include "sp_ds/netclasses/SP_DS_SimplePed.h"

class SP_DS_TimePT: public SP_DS_SimplePed
{
private:
protected:
public:
    SP_DS_TimePT();
    SP_DS_TimePT(const wxString& p_pchName);
	 
	virtual SP_DS_Graph* CreateGraph(SP_DS_Graph* p_pcGraph, SP_MapString2Int p_mgraphicsPosMap = SP_MapString2Int());

};

#endif // __SP_DS_TIMEPT_H__
