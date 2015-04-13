//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: dscheibl $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2005/04/05 14:07:00 $
// Short Description: Mark (PED marking) attribute Base Header
//////////////////////////////////////////////////////////////////////
#ifndef __SP_GR_DOUBLEMARKATTRIBUTE_H__
#define __SP_GR_DOUBLEMARKATTRIBUTE_H__

#include "sp_gr/SP_GR_Attribute.h"

class SP_GR_BaseDrawn;

class SP_GR_DoubleMarkAttribute: public SP_GR_Attribute
{
private:
protected:
    SP_GR_BaseDrawn* m_pcPrimitive;

    virtual bool DrawValue(double p_nValue);
public:
    SP_GR_DoubleMarkAttribute(SP_DS_Attribute* p_pcParent,
        double p_nWidth = 0.0,
        double p_nHeight = 0.0);
    virtual ~SP_GR_DoubleMarkAttribute();

    virtual bool Update(bool p_bLocalOnly = FALSE);

    virtual SP_Graphic* Clone(SP_Data* p_pcParent);

    virtual wxShape* GetPrimitive();
};

#endif // __SP_GR_DOUBLEMARKATTRIBUTE_H__


