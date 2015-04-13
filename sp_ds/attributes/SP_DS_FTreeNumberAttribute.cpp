//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: ak $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2006/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_ds/attributes/SP_DS_FTreeNumberAttribute.h"
#include "sp_core/SP_Core.h"
#include "sp_ds/SP_DS_Node.h"
#include "sp_ds/SP_DS_Nodeclass.h"

SP_DS_FTreeNumberAttribute::SP_DS_FTreeNumberAttribute(const wxString& p_pchName,
                                             long p_nVal)
:SP_DS_Attribute(p_pchName, SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NUMBER), m_nValue(p_nVal)
{
}

SP_DS_Attribute*
SP_DS_FTreeNumberAttribute::Clone(bool p_bCloneGr)
{
    SP_DS_FTreeNumberAttribute* l_pcAttr = new SP_DS_FTreeNumberAttribute(GetName(), GetValue());

    // just clone the graphic stuff, too
    return CloneBase(l_pcAttr, p_bCloneGr);
}

wxString
SP_DS_FTreeNumberAttribute::GetValueString()
{
    wxString l_sVal;
    l_sVal.Printf(wxT("%ld"), GetValue());
    return l_sVal;
}

bool
SP_DS_FTreeNumberAttribute::SetValueString(const wxString& p_pchVal, bool p_bCheckValue)
{
	SP_DS_Node* l_pcParent = dynamic_cast<SP_DS_Node*>(GetParent());
    long l_nVal;
    if (p_pchVal.ToLong(&l_nVal))
    {
		if (l_nVal == 0){
			SP_MESSAGEBOX(wxT(" '0' make no sense ! "), wxT("Warning"), wxOK | wxICON_ERROR);
			return FALSE;
		}

		if (GetName()== wxT("M")){
			wxString val = l_pcParent->GetAttribute(wxT("N"))->GetValueString();
			long nVal;
			val.ToLong(&nVal);
			if (nVal < l_nVal) {
				SP_MESSAGEBOX(wxT("false value: M < = N ! "), wxT("Warning"), wxOK | wxICON_ERROR);
				return FALSE;
			}else{
				return SetValue(l_nVal);
			}
		}else{
			if (GetName()== wxT("N")){
				wxString val = l_pcParent->GetAttribute(wxT("M"))->GetValueString();
				long nVal;
				val.ToLong(&nVal);
				if (nVal > l_nVal) {
					SP_MESSAGEBOX(wxT("false value: M < = N ! "), wxT("Warning"), wxOK | wxICON_ERROR);
					return FALSE;
				}else{
					return SetValue(l_nVal);
				}
			}else{
				return SetValue(l_nVal);
			}
		}
    }

    return FALSE;
}

