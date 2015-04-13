//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: akurth $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2005/01/05 11:55:00 $
// Short Description: Gate Base Header
//////////////////////////////////////////////////////////////////////
#ifndef __SP_GR_GATERECTANGLE_H__
#define __SP_GR_GATERECTANGLE_H__

#include "sp_gr/shapes/SP_GR_NamedRectangle.h"


class SP_GR_GateRectangle: public SP_GR_NamedRectangle
{

public:
    SP_GR_GateRectangle(SP_DS_Node* p_pcParent,
    		const wxString& newName = wxT("%"),
    		double p_nWidth = 20.0,
			double p_nHeight = 20.0);
    virtual ~SP_GR_GateRectangle();

    virtual SP_Graphic* Clone(SP_Data* p_pcParent);
	virtual bool Merge(SP_Graphic* p_pcTarget);


};

#endif // __SP_GR_GATERECTANGLE_H__


