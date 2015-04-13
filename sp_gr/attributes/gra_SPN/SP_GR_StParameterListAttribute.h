//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: slehrack $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2007/10/01 14:07:00 $
//////////////////////////////////////////////////////////////////////
#ifndef __SP_GR_STPARAMETERLISTATTRIBUTE_H__
#define __SP_GR_STPARAMETERLISTATTRIBUTE_H__

#include "sp_gr/SP_GR_Attribute.h"

class SP_GR_BaseDrawn;

class SP_GR_StParameterListAttribute: public SP_GR_Attribute
{
private:
protected:
    SP_GR_BaseDrawn* m_pcPrimitive;

    virtual bool DrawValue(double p_nValue);
public:
    SP_GR_StParameterListAttribute(SP_DS_Attribute* p_pcParent,
        double p_nWidth = 0.0,
        double p_nHeight = 0.0);
    virtual ~SP_GR_StParameterListAttribute();

    virtual bool Update(bool p_bLocalOnly = FALSE);

    virtual SP_Graphic* Clone(SP_Data* p_pcParent);

    virtual wxShape* GetPrimitive();
};

#endif // __SP_GR_StParameterListAttribute_H__


