//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: cr $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2009/03/06 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_NAMEATTRIBUTE_H__
#define __SP_DS_NAMEATTRIBUTE_H__

#include "sp_ds/SP_DS_Attribute.h"

class SP_DS_NameAttribute: public SP_DS_Attribute
{
private:
	bool ReplaceNameInNode(const wxString& p_sOld, const wxString& p_sNew);
	bool ReplaceValueInColListAttribute(SP_DS_Attribute* p_pcAttr, const wxString& p_sOldValue, const wxString& p_sNewValue);
	bool SetValueInAttribute(const wxString& p_pchVal);

protected:
	wxString m_sValue;

public:
    SP_DS_NameAttribute(const wxString& p_pchName, const wxString& p_pchVal = wxT(""));
    virtual ~SP_DS_NameAttribute() { }

    virtual bool SetValue(const wxString& p_pchVal);
    virtual wxString GetValue();

    virtual bool SetValueString(const wxString& p_pchVal, bool p_bCheckValue = true);
    virtual wxString GetValueString() { return m_sValue; }

    virtual SP_DS_Attribute* Clone(bool p_bCloneGr = TRUE);

    bool IsOk();
    void ReplaceProhibitedChars();

};


#endif // __SP_DS_NAMEATTRIBUTE_H__
