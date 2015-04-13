//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: ckruege2 $
// $Version: 0.0 $
// $Date: 2009/07/21 $
//////////////////////////////////////////////////////////////////////
#ifndef __SP_GR_EXTENDEDACTION_H__
#define __SP_GR_EXTENDEDACTION_H__

#include "sp_gr/shapes/SP_GR_ExtendedDrawnShape.h"

class SP_GR_BaseDrawn;

class SP_GR_ExtendedAction: public SP_GR_ExtendedDrawnShape
{
private:
protected:
	virtual bool Draw();
public:
    SP_GR_ExtendedAction(SP_DS_Node* p_pcParent,
                            double p_nWidth = 40.0,
                            double p_nHeight = 30.0,
                            SP_EXTENDED_TYPE p_nType = SP_EXTENDED_TYPE_DEFAULT,
							size_t p_nThickness=1,wxColour p_pcColur= wxColour(0, 0, 0));
    virtual ~SP_GR_ExtendedAction();

    virtual SP_Graphic* Clone(SP_Data* p_pcParent);
};

#endif // __SP_GR_EXTENDEDACTION_H__
