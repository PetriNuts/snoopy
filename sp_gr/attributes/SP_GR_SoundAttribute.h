//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Mark (PED marking) attribute Base Header
//////////////////////////////////////////////////////////////////////
#ifndef __SP_GR_SOUNDATTRIBUTE_H__
#define __SP_GR_SOUNDATTRIBUTE_H__

#include "sp_gr/SP_GR_Attribute.h"

class SP_GR_BaseDrawn;

class SP_GR_SoundAttribute: public SP_GR_Attribute
{
private:
protected:
    SP_GR_BaseDrawn* m_pcPrimitive;

    virtual bool Draw();
public:
    SP_GR_SoundAttribute(SP_DS_Attribute* p_pcParent,
        double p_nWidth = 0.0,
        double p_nHeight = 0.0);
    virtual ~SP_GR_SoundAttribute();

    virtual bool Update(bool p_bLocalOnly = FALSE);

    virtual SP_Graphic* Clone(SP_Data* p_pcParent);

    virtual wxShape* GetPrimitive();
};

#endif // __SP_GR_SOUNDATTRIBUTE_H__


