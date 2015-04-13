//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/SP_DS_Node.h"
#include "sp_ds/SP_DS_Edge.h"
#include "sp_ds/SP_DS_Metadata.h"
#include "sp_core/base/SP_Graphic.h"
#include "sp_core/SP_Core.h"

#include "sp_gr/SP_GR_Attribute.h"

SP_DS_Attribute::SP_DS_Attribute(const wxString& p_pchName, SP_ATTRIBUTE_TYPE p_eType)
	:SP_Data(SP_ELEMENT_ATTRIBUTE), SP_Name(p_pchName),
	 m_pcParent((SP_Data*)NULL), m_eAttributeType(p_eType),
	 m_bShowInGlobalDialog(FALSE)
{
}

SP_DS_Attribute::~SP_DS_Attribute()
{
    for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
    {
		wxDELETE(l_pcAttr);
    }
    m_lAttributes.clear();

    RemoveAllGraphics(true);
}

SP_DS_Attribute*
SP_DS_Attribute::CloneBase(SP_DS_Attribute* p_pcAttr, bool p_bCloneGr)
{
    if (GetGraphics() && p_bCloneGr)
    {
        SP_ListGraphic::iterator l_Iter;

        for (l_Iter = GetGraphics()->begin(); l_Iter != GetGraphics()->end(); ++l_Iter)
        {
        	SP_Graphic* l_pcGr = *l_Iter;
            if (l_pcGr->GetGraphicType() == SP_GRAPHIC_ATTRIBUTE)
                p_pcAttr->AddGraphic( l_pcGr->Clone(p_pcAttr) );
            else
            {
                // found a wrong associated graphic
                // should never happen, really
                //wxASSERT_MSG(1 == 0, wxT("Something went terribly wrong in SP_DS_Attribute::CloneBase()"));
                wxDELETE(p_pcAttr);
                SetLastError(wxT("Wrong type for the graphic associated in SP_DS_Attribute"));
                return NULL;
            }
        }
    }
    //clone its attributes, too
	for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
	{
		p_pcAttr->AddAttribute(l_pcAttr->Clone(p_bCloneGr));
	}

    p_pcAttr->SetGlobalShow(m_bShowInGlobalDialog);

    return p_pcAttr;
}

bool
SP_DS_Attribute::MergeData(SP_Data* p_pcElement)
{
    CHECK_POINTER(p_pcElement, return FALSE);
    CHECK_BOOL((p_pcElement->GetElementType() == SP_ELEMENT_ATTRIBUTE), return FALSE);

    SP_DS_Attribute* l_pcAttribute = dynamic_cast<SP_DS_Attribute*>(p_pcElement);

    CHECK_BOOL((GetName()== l_pcAttribute->GetName()), return FALSE);

    bool l_bReturn = SP_Data::MergeData(p_pcElement) && l_pcAttribute->Remove();

    l_pcAttribute->SetDelete(TRUE);

    return l_bReturn && OnMerge(p_pcElement);
}

bool
SP_DS_Attribute::Split(SP_DS_Attribute* p_pcAttr, SP_ListGraphic* p_plGraphics)
{
    CHECK_POINTER(p_pcAttr, return TRUE);
    CHECK_POINTER(p_plGraphics, return TRUE);

    // search the list of graphics for the one, that has me as data parent
    SP_Graphic* l_pcGraphic = NULL;
    SP_ListGraphic::iterator l_Iter;
    for (l_Iter = p_plGraphics->begin(); l_Iter != p_plGraphics->end(); ++l_Iter)
    {
        if ((*l_Iter)->GetParent() == this)
        {
            l_pcGraphic = (*l_Iter);
            break;
        }
    }
    if (l_pcGraphic)
    {
        RemoveGraphic(l_pcGraphic, FALSE);
        p_pcAttr->AddGraphic(l_pcGraphic);
        l_pcGraphic->SetDataParent(p_pcAttr);
    }
    return OnSplit(p_pcAttr);
}

SP_WDG_DialogBase*
SP_DS_Attribute::RegisterDialogWidget(SP_WDG_DialogBase* p_pcWidget)
{
    // just insert into the registry
    return SP_Core::Instance()->RegisterDialogWidget(this, p_pcWidget);
}

bool
SP_DS_Attribute::SetGlobalShow(bool p_bVal)
{
    if (!GetParent() || !GetParent()->GetClassObject())
        return FALSE;

    m_bShowInGlobalDialog = p_bVal;
    SP_DS_Graph* l_pcGraph = GetParent()->GetClassObject()->GetParentGraph();

    CHECK_POINTER(l_pcGraph, return FALSE);

    return l_pcGraph->SetGlobalShow(p_bVal);
}

bool
SP_DS_Attribute::ShowGraphic(SP_Graphic* p_pcGraphic)
{
    CHECK_POINTER(p_pcGraphic, return FALSE);

    bool l_bReturn = TRUE;

    SP_ListGraphic* l_plGraphic = GetGraphics();
    if (l_plGraphic)
    {
        // check all graphics for having the proper type
        for (SP_Graphic* l_pcGraphic : *l_plGraphic)
        {
            // if they are both shown in the same net, add the graphic
            if ((l_pcGraphic->GetNetnumber() == p_pcGraphic->GetNetnumber()) &&
                (l_pcGraphic->GetGraphicParent() == l_pcGraphic
                		|| l_pcGraphic->GetGraphicParent() == p_pcGraphic))
            {
                l_bReturn &= p_pcGraphic->AddChildToCanvas(l_pcGraphic);
                l_pcGraphic->Update(TRUE);
            }
        }
    }

    return l_bReturn;
}

bool
SP_DS_Attribute::CheckIntegrity()
{
	CHECK_POINTER(GetParent(), return FALSE);

	bool l_bReturn = true;

	SP_ListGraphic* l_plGraphic = GetGraphics();
	if (l_plGraphic->size()
			== GetParent()->GetGraphics()->size())
	{
		return true;
	}
	else if (l_plGraphic->size() > GetParent()->GetGraphics()->size())
	{
		SP_MapGraphic2Graphic  l_GrPar2Gr;
		SP_ListGraphic l_Gr2Delete;
		for (SP_Graphic* l_pcGr : *l_plGraphic)
		{
			SP_Graphic* l_pcGrPar = l_pcGr->GetGraphicParent();
			if(l_pcGr != l_pcGrPar)
			{
				SP_MapGraphic2Graphic::iterator itM = l_GrPar2Gr.find(l_pcGrPar);
				if(itM == l_GrPar2Gr.end())
				{
					l_GrPar2Gr.insert(make_pair(l_pcGrPar, l_pcGr));
					l_bReturn &= l_pcGr->CheckIntegrity();
				}
				else
				{
					SP_LOGDEBUG(wxT("Found double graphics and removed it."));
					l_Gr2Delete.push_back(l_pcGr);
				}
			}
			else
			{
				SP_LOGDEBUG(wxT("Found double graphics and removed it."));
				l_Gr2Delete.push_back(l_pcGr);
			}
		}

		SP_ListGraphic::iterator itD = l_Gr2Delete.begin();
		while(itD != l_Gr2Delete.end())
		{
			SP_Graphic* l_pcGr = *itD;
			l_pcGr->RemoveFromCanvas();
			RemoveGraphic(l_pcGr);
			itD = l_Gr2Delete.erase(itD);
		}
	}
	else if(!l_plGraphic->empty())
	{
		SP_ListGraphic::iterator l_itGrPar = GetParent()->GetGraphics()->begin();
		SP_ListGraphic::iterator l_endGrPar = GetParent()->GetGraphics()->end();
		for(; l_itGrPar != l_endGrPar; ++l_itGrPar)
		{
			SP_Graphic* l_pcGrPar = (*l_itGrPar);
			bool l_bFound = false;
			for (SP_Graphic* l_pcGraphic : *l_plGraphic)
			{
				if(l_pcGraphic->GetGraphicParent() == l_pcGrPar)
				{
					l_bFound = true;
					break;
				}
			}
			if(!l_bFound)
			{
				SP_Graphic* l_pcGr = NewGraphicInSubnet(l_pcGrPar->GetNetnumber());
				l_pcGrPar->AddChildToCanvas(l_pcGr);
				l_pcGr->Update(true);
			}
		}
	}

	return l_bReturn;
}


bool
SP_DS_Attribute::Update(bool p_bLocalOnly)
{
	bool l_bReturn = TRUE;

    SP_ListGraphic* l_plGraphic = GetGraphics();
    if (l_plGraphic)
    {
        // check all graphics for having the proper type
        for (SP_Graphic* l_pcGraphic : *l_plGraphic)
        {
            l_bReturn &= l_pcGraphic->Update(p_bLocalOnly);
        }
    }

    return l_bReturn;
}

bool
SP_DS_Attribute::Remove()
{
    if (m_pcParent)
    {
        switch (m_pcParent->GetElementType())
        {
        case SP_ELEMENT_NODE:
            dynamic_cast<SP_DS_Node*>(m_pcParent)->RemoveAttribute(this);
            break;
        case SP_ELEMENT_EDGE:
            dynamic_cast<SP_DS_Edge*>(m_pcParent)->RemoveAttribute(this);
            break;
        case SP_ELEMENT_METADATA:
            dynamic_cast<SP_DS_Metadata*>(m_pcParent)->RemoveAttribute(this);
            break;
        default:
	    break;
        };
        m_pcParent = NULL;
    }

    return TRUE;
}

bool
SP_DS_Attribute::Equals(SP_Data *p_pcToCheck)
{
	return Equals(static_cast<SP_DS_Attribute*>(p_pcToCheck));
}

bool
SP_DS_Attribute::Equals(SP_DS_Attribute *p_pcToCheck, bool p_bCheckValue, bool p_bCheckName, bool p_bCheckType) {

	if(!SP_Data::Equals(p_pcToCheck)) return false;

	if (this == p_pcToCheck) return true;

	if (p_bCheckName) {
		if (GetName() !=  p_pcToCheck->GetName()) return false;
	}

	if (p_bCheckType) {
		if (m_eAttributeType != p_pcToCheck->GetAttributeType()) return false;
	}

	if (!p_bCheckValue) return true;

	return (this->GetValueString().Cmp(p_pcToCheck->GetValueString()) == 0);
}

const wxString
SP_DS_Attribute::GetClassName()
{
	return GetName();
}


SP_DS_Attribute*
SP_DS_Attribute::AddAttribute(SP_DS_Attribute* p_pcAttr)
{
    CHECK_POINTER(p_pcAttr, return NULL);
    // alert, parameter already is associated to another data-element
    CHECK_BOOL((p_pcAttr->GetParent() == NULL), return NULL);

	//if p_pcAttr already exists, don't add it again
	bool l_bAdd = true;
	for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
    {
        if ((*p_pcAttr).Equals(l_pcAttr)) l_bAdd = false;
    }
	if (!l_bAdd) return 0;

    p_pcAttr->SetDataParent(this);
    //p_pcAttr->SetNetnumber(GetNetnumber());

    m_lAttributes.push_back(p_pcAttr);

    return p_pcAttr;
}

SP_DS_Attribute*
SP_DS_Attribute::GetAttribute(const wxString& p_pchName)
{
    if(p_pchName.IsEmpty()) return NULL;

    for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
    {
        if (p_pchName.Cmp(l_pcAttr->GetName()) == 0)
            return l_pcAttr;
    }

    return NULL;
}

SP_DS_Attribute*
SP_DS_Attribute::RemoveAttribute(SP_DS_Attribute* p_pcVal)
{
    if (!p_pcVal || (SP_Find(m_lAttributes, p_pcVal) == m_lAttributes.end()))
        return NULL;

    p_pcVal->SetDataParent(NULL);
    m_lAttributes.remove(p_pcVal);

    return p_pcVal;
}

bool SP_DS_Attribute::CopyValueFrom(SP_DS_Attribute* p_pcAttribute)
{
	if(! Equals(p_pcAttribute, false)) return false;

	SetValueString(p_pcAttribute->GetValueString());

	return true;
}

wxString
SP_DS_Attribute::GetParentName()
{
	if(GetAttributeType() == SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME)
	{
		SP_DS_NameAttribute* l_pcNameAttr =
				dynamic_cast<SP_DS_NameAttribute*>(this);
		return l_pcNameAttr->GetValue();
	}
	else if(GetParent())
	{
		if(GetParent()->GetElementType() == SP_ELEMENT_EDGE)
		{
			SP_DS_Edge* l_pcEdge = dynamic_cast<SP_DS_Edge*>(GetParent());
			SP_DS_Node* l_pcSource = dynamic_cast<SP_DS_Node*>(l_pcEdge->GetSource());
			SP_DS_Node* l_pcTarget = dynamic_cast<SP_DS_Node*>(l_pcEdge->GetTarget());

			wxString l_sSource = l_pcSource->GetAttributes()->front()->GetParentName();
			wxString l_sTarget = l_pcTarget->GetAttributes()->front()->GetParentName();
			wxString l_sName = l_sSource;
			l_sName << (l_sSource.Length() > 10 ? wxT("\n") : wxT("") );
			l_sName << wxT(" -> ");
			l_sName << (l_sTarget.Length() > 10 ? wxT("\n") : wxT("") );
			l_sName << l_sTarget;
			return l_sName;
		}
		else
		{
			SP_DS_Attribute* l_pcAttr = NULL;
			if(GetParent()->GetElementType() == SP_ELEMENT_NODE)
			{
				l_pcAttr = dynamic_cast<SP_DS_Node*>(GetParent())->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME);
			}
			else if(GetParent()->GetElementType() == SP_ELEMENT_METADATA)
			{
				l_pcAttr = dynamic_cast<SP_DS_Metadata*>(GetParent())->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME);
			}
			if(l_pcAttr)
			{
				return l_pcAttr->GetParentName();
			}
			else
			{
				return MakeParentName();
			}
		}
	}

	return wxEmptyString;
}

wxString
SP_DS_Attribute::MakeParentName()
{
	wxString l_sName;

	if(GetParent() && GetParent()->GetClassObject())
	{
		SP_Data* l_pcParent = GetParent();
		l_sName = wxT("_") + l_pcParent->GetClassName().Left(1) + wxT("_");
		SP_DS_Attribute* l_pcIdAttr = NULL;
		if(GetParent()->GetElementType() == SP_ELEMENT_NODE)
		{
			l_pcIdAttr = dynamic_cast<SP_DS_Node*>(GetParent())->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID);
		}
		else if(GetParent()->GetElementType() == SP_ELEMENT_EDGE)
		{
			l_pcIdAttr = dynamic_cast<SP_DS_Edge*>(GetParent())->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID);
		}
		else if(GetParent()->GetElementType() == SP_ELEMENT_METADATA)
		{
			l_pcIdAttr = dynamic_cast<SP_DS_Metadata*>(GetParent())->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID);
		}
		if(l_pcIdAttr)
		{
			l_sName << l_pcIdAttr->GetValueString();
		}
		else
		{
			l_sName << l_pcParent->GetId();
		}
		l_sName << wxT("_");
	}
	else
	{
		SP_LOGERROR( wxT("Error in MakeParentName"));
	}

	return l_sName;
}

