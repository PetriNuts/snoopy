//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Multiplicity graphic implementation
//////////////////////////////////////////////////////////////////////
#include "sp_gr/attributes/SP_GR_MultiplicityAttribute.h"
#include "sp_gr/base/SP_GR_BaseText.h"
#include "sp_ds/attributes/SP_DS_MultiplicityAttribute.h"
#include "sp_ds/attributes/SP_DS_MarkingDependentMultiplicity.h"
#include "sp_ds/attributes/SP_DS_DoubleMarkingDependentMultiplicity.h"



//by sl
#include "sp_core/SP_Core.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"

//---------------------------------------------------------------
//
// Creation (c'tor and clone)
//
//---------------------------------------------------------------
SP_GR_MultiplicityAttribute::SP_GR_MultiplicityAttribute(SP_DS_Attribute* p_pcParent,
                                             const wxString& p_pchFormat,
                                             double p_nWidth,
                                             double p_nHeight)
:SP_GR_NumberAttribute(p_pcParent, p_pchFormat, p_nWidth, p_nHeight)
{
}

SP_Graphic*
SP_GR_MultiplicityAttribute::Clone(SP_Data* p_pcParent)
{
    if (!p_pcParent || p_pcParent->GetElementType() != SP_ELEMENT_ATTRIBUTE)
        return NULL;

    SP_DS_Attribute* l_pcParent = dynamic_cast<SP_DS_Attribute*>(p_pcParent);

    SP_GR_MultiplicityAttribute* l_pcNew = new SP_GR_MultiplicityAttribute(l_pcParent,
        m_sFormat,
        this->GetWidth(),
        this->GetHeight());

    CloneBase(l_pcNew);

    return l_pcNew;
}

wxString
SP_GR_MultiplicityAttribute::FormatText()
{
    if (!GetParent() ||
        (GetParent()->GetElementType() != SP_ELEMENT_ATTRIBUTE) ||
        (dynamic_cast<SP_DS_Attribute*>(GetParent())->GetAttributeType() != SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_MULTIPLICITY &&
        dynamic_cast<SP_DS_Attribute*>(GetParent())->GetAttributeType() != SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_MARKINGDEPENDETMULTIPLICITY))
        return wxT("");

    SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
    wxString l_sName = l_pcGraph->GetNetclass()->GetName();

    SP_DS_Attribute* l_pcAttr = dynamic_cast<SP_DS_Attribute*>(GetParent());
    SP_DS_Edge* l_edge = NULL;

    if (l_pcAttr->GetParent()->GetElementType() == SP_ELEMENT_EDGE)
   	    l_edge = dynamic_cast<SP_DS_Edge*>((l_pcAttr)->GetParent());

    wxString l_pchVal = wxEmptyString;
    long l_nVal = 0;
    bool l_bShow = m_bShow;

    //check iff animation mode
    if(SP_Core::Instance()->GetAnimMode()) {
    	long l_nValue = 0;
    	if(dynamic_cast<SP_DS_Attribute*>(GetParent())->GetAttributeType() == SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_MULTIPLICITY)
    	{
    		l_nValue = dynamic_cast<SP_DS_MultiplicityAttribute*>(l_pcAttr)->GetValue();
    	}
    	else if(dynamic_cast<SP_DS_Attribute*>(GetParent())->GetAttributeType() == SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_MARKINGDEPENDETMULTIPLICITY)
    	{
    		l_nValue = dynamic_cast<SP_DS_MarkingDependentMultiplicity*>(l_pcAttr)->GetValue();
    	}
    	l_pchVal << l_nValue;

		if (l_edge != NULL)
		{
			if (l_nValue == 0)
			{
				if (!l_edge->GetHide())
				{
					l_pcGraph->SetHide(l_edge, true);
				}
			}
			else
			{
				if (l_pcAttr->GetHide())
				{
					l_pcGraph->SetHide(l_edge, false);
				}
			}
    	}
    }
    else
    {
    	l_pchVal = l_pcAttr->GetValueString();

    	if (l_pcAttr->GetHide())
    		l_pcGraph->SetHide(l_edge, false);
    }

	if (l_pchVal.ToLong(&l_nVal))
	{
		l_bShow &= (l_nVal > 1);
	}
	l_bShow &= (l_pchVal.Len() != 0);

	//iff number, then show value string if number greater 1
	if (m_pcPrimitive)
		m_pcPrimitive->Show(l_bShow);

    SP_FormatWithComma(l_pchVal);

	wxString l_sVal = m_sFormat;
	l_sVal.Replace(wxT("%"), l_pchVal);

    return l_sVal;
}
