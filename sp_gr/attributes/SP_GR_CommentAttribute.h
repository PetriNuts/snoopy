//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Comment Text attribute Base Header
//////////////////////////////////////////////////////////////////////
#ifndef __SP_GR_COMMENTATTRIBUTE_H__
#define __SP_GR_COMMENTATTRIBUTE_H__

#include "sp_gr/SP_GR_Attribute.h"

class SP_GR_BaseText;

class SP_GR_CommentAttribute: public SP_GR_Attribute
{
private:
protected:
    SP_GR_BaseText* m_pcPrimitive;

    virtual wxString FormatText();
public:
    SP_GR_CommentAttribute(SP_DS_Attribute* p_pcParent);
    virtual ~SP_GR_CommentAttribute();

    virtual bool Update(bool p_bLocalOnly = FALSE);

    virtual SP_Graphic* Clone(SP_Data* p_pcParent);

    virtual wxShape* GetPrimitive();
};

#endif // __SP_GR_COMMENTATTRIBUTE_H__
