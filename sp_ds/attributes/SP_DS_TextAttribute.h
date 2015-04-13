//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_TEXTATTRIBUTE_H__
#define __SP_DS_TEXTATTRIBUTE_H__

#include "sp_ds/SP_DS_Attribute.h"

class SP_DS_TextAttribute: public SP_DS_Attribute
{
private:
protected:
    wxString m_sValue;
    bool m_bMultiline;
public:
    SP_DS_TextAttribute(const wxString& p_pchName, const wxString& p_pchVal = wxT(""));
    virtual ~SP_DS_TextAttribute() { }

    virtual bool SetValue(const wxString& p_pchVal);
    virtual wxString GetValue() { return m_sValue; }

    virtual wxString GetValueString() { return GetValue(); }
    virtual bool SetValueString(const wxString& p_pchVal, bool p_bCheckValue = true) { return SetValue(p_pchVal); }

    virtual SP_DS_Attribute* Clone(bool p_bCloneGr = TRUE);
};


#endif // __SP_DS_TEXTATTRIBUTE_H__
