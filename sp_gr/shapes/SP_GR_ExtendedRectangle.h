//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: ckuege2 $
// $Version: 0.0 $
// $Date: 2009/07/21 
// Short Description:  Drawn Rectangle Base Header
//////////////////////////////////////////////////////////////////////
#ifndef __SP_GR_EXTENDEDRECTANGLE_H__
#define __SP_GR_EXTENDEDRECTANGLE_H__

#include "sp_gr/shapes/SP_GR_ExtendedDrawnShape.h"

class SP_GR_BaseDrawn;

class SP_GR_ExtendedRectangle: public SP_GR_ExtendedDrawnShape
{
private:
	bool m_bSquare;
protected:
	virtual bool Draw();
public:
    SP_GR_ExtendedRectangle(SP_DS_Node* p_pcParent,
                            double p_nWidth,
                            double p_nHeight,
							bool p_bSquare=FALSE,
                            SP_EXTENDED_TYPE p_nType = SP_EXTENDED_TYPE_DEFAULT,
							size_t p_nThickness=1,wxColour p_pcColur= wxColour(0, 0, 0));
    virtual ~SP_GR_ExtendedRectangle();

    virtual SP_Graphic* Clone(SP_Data* p_pcParent);
};

#endif // __SP_GR_EXTENDEDRECTANGLE_H__


