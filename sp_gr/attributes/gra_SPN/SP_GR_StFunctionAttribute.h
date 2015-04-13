//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: slehrack $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2007/09/23 15:35:00 $
//////////////////////////////////////////////////////////////////////

#ifndef __SP_GR_STFUNCTIONATTRIBUTE_H__
#define __SP_GR_STFUNCTIONATTRIBUTE_H__

#include "sp_gr/SP_GR_Attribute.h"

class SP_GR_BaseText;

class SP_GR_StFunctionAttribute: public SP_GR_Attribute
{
private:
protected:
    SP_GR_BaseText* m_pcPrimitive;
    wxString m_sFormat;
    virtual wxString FormatText();
public:
    SP_GR_StFunctionAttribute(SP_DS_Attribute* p_pcParent,
        const wxString& p_pchFormat = wxT("%"),
        double p_nWidth = 0.0,
        double p_nHeight = 0.0);
    virtual ~SP_GR_StFunctionAttribute();

    virtual bool Update(bool p_bLocalOnly = FALSE);

    virtual SP_Graphic* Clone(SP_Data* p_pcParent);

    virtual wxShape* GetPrimitive();
};

#endif // __SP_GR_StFunctionAttribute_H__
