//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: dscheibl $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2005/04/27 16:37:00 $
// Short Description:  Drawn Circle Base Header
//////////////////////////////////////////////////////////////////////
#ifndef __SP_GR_EXTENDEDCIRCLE_H__
#define __SP_GR_EXTENDEDCIRCLE_H__

#include "sp_gr/shapes/SP_GR_ExtendedDrawnShape.h"

class SP_GR_BaseDrawn;

class SP_GR_ExtendedCircle: public SP_GR_ExtendedDrawnShape
{
private:
protected:
	virtual bool Draw();
public:
    SP_GR_ExtendedCircle(SP_DS_Node* p_pcParent, double p_nRadius = 40.0, SP_EXTENDED_TYPE p_nType = SP_EXTENDED_TYPE_DEFAULT, size_t p_nThickness=1,wxColour p_pcColur= wxColour(0, 0, 0));
    virtual ~SP_GR_ExtendedCircle();

    virtual SP_Graphic* Clone(SP_Data* p_pcParent);
};

#endif // __SP_GR_EXTENDEDCIRCLE_H__


