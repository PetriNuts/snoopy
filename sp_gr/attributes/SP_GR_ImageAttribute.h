//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Comment Text attribute Base Header
//////////////////////////////////////////////////////////////////////
#ifndef __SP_GR_IMAGEATTRIBUTE_H__
#define __SP_GR_IMAGEATTRIBUTE_H__

#include "sp_gr/SP_GR_Attribute.h"

class SP_GR_BaseBitmap;

class SP_GR_ImageAttribute: public SP_GR_Attribute
{
private:
protected:
    SP_GR_BaseBitmap* m_pcPrimitive;

public:
    SP_GR_ImageAttribute(SP_DS_Attribute* p_pcParent);
    virtual ~SP_GR_ImageAttribute();

    virtual bool Update(bool p_bLocalOnly = FALSE);

    virtual SP_Graphic* Clone(SP_Data* p_pcParent);

    virtual wxShape* GetPrimitive();
};

#endif // __SP_GR_IMAGEATTRIBUTE_H__
