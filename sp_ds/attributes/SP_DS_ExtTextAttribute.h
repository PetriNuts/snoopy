//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_EXTTEXTATTRIBUTE_H__
#define __SP_DS_EXTTEXTATTRIBUTE_H__

#include "sp_ds/SP_DS_Attribute.h"

class SP_DS_ExtTextAttribute: public SP_DS_Attribute
{
private:
protected:
    wxString m_sValue;
    bool m_bMultiline;

public:
    SP_DS_ExtTextAttribute(const wxString& p_pchName, const wxString& p_pchVal = wxT(""));
    virtual ~SP_DS_ExtTextAttribute() { }

    virtual bool SetValue(const wxString& p_pchVal);
    virtual wxString GetValue() { return m_sValue; }

    virtual wxString GetValueString() { return GetValue(); }
    virtual bool SetValueString(const wxString& p_pchVal, bool p_bCheckValue = true) { return SetValue(p_pchVal); }

    virtual SP_DS_Attribute* Clone(bool p_bCloneGr = TRUE);

	bool CheckDuplicateNodes(const wxString& p_pchVal);
};


#endif // __SP_DS_TEXTATTRIBUTE_H__
