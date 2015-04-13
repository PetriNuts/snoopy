//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: cr $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2009/03/27 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_ds/SP_DS_Metadata.h"
#include "sp_ds/SP_DS_Metadataclass.h"
#include "sp_ds/attributes/SP_DS_IdAttribute.h"
#include "sp_ds/attributes/SP_DS_LogicAttribute.h"
#include "sp_ds/extensions/SP_DS_Coarse.h"
#include "sp_ds/SP_DS_Graph.h"

#include "sp_gr/SP_GR_Metadata.h"
#include "sp_gr/SP_GR_Attribute.h"

#include "sp_gui/windows/SP_GUI_Canvas.h"
#include "sp_core/SP_Core.h"

SP_DS_Metadata::SP_DS_Metadata(SP_DS_Metadataclass* p_pcMetadataclass, unsigned int p_nNetnumber) :
	SP_Data(SP_ELEMENT_METADATA)
{
	m_pcMetadataclass = p_pcMetadataclass;
	SetNetnumber(p_nNetnumber);
}

SP_DS_Metadata::~SP_DS_Metadata()
{
	for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
	{
		wxDELETE(l_pcAttr);
	}
	m_lAttributes.clear();

	RemoveAllGraphics(true);
}

SP_DS_Metadata* SP_DS_Metadata::Clone(bool p_bCloneGr)
{
	SP_DS_Metadata* l_pcNewMetadata = new SP_DS_Metadata(GetMetadataclass(), GetNetnumber());

	for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
	{
		l_pcNewMetadata->AddAttribute(l_pcAttr->Clone(p_bCloneGr));
	}
	l_pcNewMetadata->SetLogical(GetLogical());

	// don't clone the list of graphics, too
	if (!p_bCloneGr)
		return l_pcNewMetadata;

    if (m_pcCoarse)
        l_pcNewMetadata->SetCoarse(m_pcCoarse->Clone());

    SP_ListGraphic* l_plGraphic = GetGraphics();
	SP_Graphic* l_pcTemp;
	if (l_plGraphic)
	{
		// check all graphics for having the proper type
		for (SP_Graphic* l_pcGr : *l_plGraphic)
		{
			if (l_pcGr->GetGraphicType() == SP_GRAPHIC_METADATA)
			{
				l_pcTemp = l_pcGr->Clone(l_pcNewMetadata);
				if (l_pcTemp)
					l_pcTemp->ShowAttributeGraphic();

				l_pcNewMetadata->AddGraphic(l_pcTemp);
			}
			else
			{
				// found a wrong associated graphic
				// should never happen, really
				wxASSERT_MSG(FALSE, wxT("Something went wrong in SP_DS_Metadata::Clone()"));
				wxDELETE(l_pcNewMetadata);
				SetLastError(wxT("Wrong type for the graphic associated in SP_DS_Metadata"));
				return NULL;
			}
		}
	}

	return l_pcNewMetadata;
}

bool SP_DS_Metadata::Equals(SP_Data* p_pcVal)
{
	if (!p_pcVal || (p_pcVal->GetElementType() != m_eElementType)
			|| !GetMetadataclass())
		return FALSE;

	SP_DS_Metadata* l_pcMetadata = dynamic_cast<SP_DS_Metadata*>(p_pcVal);

	if (!l_pcMetadata->GetMetadataclass())
		return FALSE;

	if (GetMetadataclass()->GetName()== l_pcMetadata->GetMetadataclass()->GetName())
		return TRUE;

	return FALSE;
}

SP_Data* SP_DS_Metadata::AddToCopyMap(SP_DS_Graph** p_ppcGraph,
		SP_Graphic* p_pcGraphic,
		SP_MapGraphic2Graphic* p_pmGraphic2Graphic,
		SP_MapData2Data* p_pmData2Data)
{
	CHECK_POINTER(p_ppcGraph, return NULL);
	CHECK_POINTER(p_pmData2Data, return NULL);

	SP_DS_Metadata* l_pcNewMetadata;
	// try to find the mapping to myself in the parameter
	SP_MapData2Data::const_iterator l_it = p_pmData2Data->find(this);
	if (l_it == p_pmData2Data->end())
	{
		// if not found, clone a new one and insert it into the map
		// clone the node without any graphics, because there can be
		// much more than we have selected. Check p_pcGraphic afterwards.
		l_pcNewMetadata = this->Clone(FALSE);
		// new element insert into the copy graph
		(*p_ppcGraph)->AddElement(l_pcNewMetadata);
		p_pmData2Data->insert(make_pair(this,l_pcNewMetadata));
	}
	else
	{
		l_pcNewMetadata = dynamic_cast<SP_DS_Metadata*>((*l_it).second);
	}

	return SynchronizeChildren(l_pcNewMetadata, p_pcGraphic, p_pmGraphic2Graphic);
}

bool SP_DS_Metadata::SynchronizeAttributes()
{
	if (!m_bLogical)
		return TRUE;

	// trick or treat
	// we cloned ourself with all the logic stuff set, but we need to
	// ensure, that we aren't the node, to be found while in SP_DS_Graph::JoinElements
	m_bLogical = FALSE;

	for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
	{
		if (l_pcAttr->GetAttributeType() == SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_LOGIC)
		{
			// immediately exit after this call, because we can be deleted appearently
			return dynamic_cast<SP_DS_LogicAttribute*>(l_pcAttr)->JoinElements();
		}
	}

	// if we didn't found an attribute, reset ourself
	m_bLogical = TRUE;

	return FALSE;
}

SP_Data* SP_DS_Metadata::SynchronizeChildren(SP_DS_Metadata* p_pcMetadata,
		SP_Graphic* p_pcGraphic,
		SP_MapGraphic2Graphic* p_pmGraphic2Graphic)
{
	CHECK_POINTER(p_pcMetadata, return NULL);
	CHECK_POINTER(p_pcGraphic, return NULL);
	CHECK_POINTER(p_pcGraphic->GetGraphicChildren(), return NULL);
	CHECK_POINTER(p_pmGraphic2Graphic, return NULL);

	SP_Data* l_pcData= NULL;
	SP_DS_Attribute* l_pcAttr= NULL;
	SP_DS_Attribute* l_pcNewAttr= NULL;
	SP_ListGraphic::iterator l_Iter;
	for (l_Iter = p_pcGraphic->GetGraphicChildren()->begin(); l_Iter != p_pcGraphic->GetGraphicChildren()->end(); ++l_Iter)
	{
		l_pcData = (*l_Iter)->GetParent();
		if (!l_pcData)
		{
			SP_LOGDEBUG(wxT("SP_DS_Metadata::SynchronizeChildren, ")
					wxT("SP_Graphic::GetParent() returned null"));
			continue;
		}
		if (l_pcData->GetElementType() != SP_ELEMENT_ATTRIBUTE)
			continue;

		l_pcAttr = dynamic_cast<SP_DS_Attribute*>(l_pcData);
		l_pcNewAttr = p_pcMetadata->GetAttribute(l_pcAttr->GetName());
		if (l_pcNewAttr)
		{
			// insert the graphic association in the graphic map
			(*p_pmGraphic2Graphic)[(*l_Iter)] = l_pcNewAttr->AddGraphic((*l_Iter)->Clone(l_pcNewAttr));
		}
	}

	return p_pcMetadata;
}

bool SP_DS_Metadata::ResetNetnumbers(
		SP_MapUInt2UInt* p_pmNet2Net, SP_DS_Graph* p_pcToGraph)
{
	CHECK_POINTER(p_pmNet2Net, return FALSE);
	CHECK_POINTER(p_pcToGraph, return FALSE);

	bool l_bReturn= TRUE;

	for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
	{
		l_bReturn &= l_pcAttr->ResetNetnumbers(p_pmNet2Net, p_pcToGraph);
	}

	return (SP_Data::ResetNetnumbers(p_pmNet2Net, p_pcToGraph) && l_bReturn);
}

SP_Graphic* SP_DS_Metadata::AddGraphicInSubnet(unsigned int p_nNetnumber)
{
	SP_Graphic* l_pcOwn = SP_Data::AddGraphicInSubnet(p_nNetnumber);
	CHECK_POINTER(l_pcOwn, return NULL);

	for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
	{
		l_pcOwn->AddGraphicChildren(l_pcAttr->AddGraphicInSubnet(p_nNetnumber));
	}

	return l_pcOwn;
}

SP_DS_Metadataclass* SP_DS_Metadata::GetMetadataclass()
{
	return m_pcMetadataclass;
}

SP_DS_Metadataclass* SP_DS_Metadata::SetMetadataclass(SP_DS_Metadataclass* p_pcVal)
{
	m_pcMetadataclass = p_pcVal;

	return m_pcMetadataclass;
}

bool SP_DS_Metadata::ClassNameChanged(const wxString& p_pchName)
{
	if(p_pchName.IsEmpty()) return FALSE;

	wxString l_sKey;
	SP_WDG_DialogBase* l_pcWdg;
	for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
	{
		l_pcWdg = SP_Core::Instance()->GetDialogWidget(l_pcAttr);
		if (l_pcWdg)
			SP_Core::Instance()->RegisterDialogWidget(wxString::Format(wxT("%s|%s|%s|%d"),
					GetClassObject()->GetParentGraph()->GetNetclass()->GetName().c_str(),
					p_pchName.c_str(),
					l_pcAttr->GetName().c_str(),
					l_pcAttr->GetAttributeType()), l_pcWdg);
	}

	return TRUE;
}

SP_DS_Attribute* SP_DS_Metadata::AddAttribute(SP_DS_Attribute* p_pcAttr)
{
	CHECK_POINTER(p_pcAttr, return NULL);
	// alert, parameter already is associated to another data-element
	CHECK_BOOL((p_pcAttr->GetParent() == NULL), return NULL);

	//if p_pcAttr already exists, don't add it again
	bool l_bAdd = true;
	for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
	{
		if ((*p_pcAttr).Equals(l_pcAttr))
			l_bAdd = false;
	}
	if (!l_bAdd)
		return 0;

	p_pcAttr->SetDataParent(this);
	//p_pcAttr->SetNetnumber(GetNetnumber());

	m_lAttributes.push_back(p_pcAttr);

	return p_pcAttr;
}

SP_DS_Attribute* SP_DS_Metadata::GetAttribute(const wxString& p_pchName)
{
	if(p_pchName.IsEmpty()) return NULL;

	for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
	{
		if (p_pchName.Cmp(l_pcAttr->GetName()) == 0)
			return l_pcAttr;
	}

	return NULL;
}

SP_DS_Attribute* SP_DS_Metadata::RemoveAttribute(SP_DS_Attribute* p_pcVal)
{
	if (!p_pcVal || (SP_Find(m_lAttributes, p_pcVal) == m_lAttributes.end()))
		return NULL;

	p_pcVal->SetDataParent(NULL);
	m_lAttributes.remove(p_pcVal);

	return p_pcVal;
}

bool SP_DS_Metadata::SetNetnumber(unsigned int p_nNewVal, unsigned int p_nOldVal)
{
	bool l_bReturn= TRUE;

	for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
	{
		l_bReturn &= l_pcAttr->SetNetnumber(p_nNewVal, p_nOldVal);
	}

	return (l_bReturn && SP_Data::SetNetnumber(p_nNewVal, p_nOldVal));
}

SP_DELETE_STATE SP_DS_Metadata::RemoveGraphic(unsigned int p_nNetnumber)
{
	for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
	{
		l_pcAttr->RemoveGraphic(p_nNetnumber);
	}

	return SP_Data::RemoveGraphic(p_nNetnumber);
}

SP_Data* SP_DS_Metadata::GetSibling(const wxString& p_pchRefAttribute)
{
	CHECK_POINTER(GetMetadataclass(), return NULL);

	SP_DS_Metadata* l_pcMetadata = GetMetadataclass()->GetRelative(this, p_pchRefAttribute, TRUE);

	return l_pcMetadata;
}

bool SP_DS_Metadata::MergeData(SP_Data* p_pcElement)
{
	CHECK_POINTER(p_pcElement, return FALSE);
	CHECK_BOOL((p_pcElement->GetElementType() == SP_ELEMENT_METADATA), return FALSE);

	bool l_bReturn= TRUE;
	SP_DS_Metadata* l_pcMetadata = dynamic_cast<SP_DS_Metadata*>(p_pcElement);
	SP_ListAttribute::const_iterator l_AttrIter;
	for (l_AttrIter = m_lAttributes.begin(); l_AttrIter != m_lAttributes.end(); ++l_AttrIter)
	{
		SP_DS_Attribute* l_pcAttribute = (*l_AttrIter);
		l_bReturn
				&= l_pcAttribute->MergeData(l_pcMetadata->GetAttribute(l_pcAttribute->GetName()));
	}

	l_bReturn &= SP_Data::MergeData(l_pcMetadata);

	return l_bReturn && l_pcMetadata->GetMetadataclass()->RemoveElement(l_pcMetadata);
}

bool SP_DS_Metadata::Split(SP_ListGraphic* p_plGraphics)
{
	CHECK_POINTER(p_plGraphics, return TRUE);
	CHECK_POINTER(GetGraphics(), return TRUE);
	CHECK_POINTER(GetMetadataclass(), return FALSE);

	bool l_bReturn= TRUE;
	SP_ListGraphic* l_plGraphics = GetGraphics();
	SP_ListGraphic::iterator l_Iter;

	for (l_Iter = p_plGraphics->begin(); l_Iter != p_plGraphics->end(); ++l_Iter)
	{
		SP_ListGraphic::iterator l_itGr = SP_Find(*l_plGraphics, *l_Iter);
		if (l_itGr != l_plGraphics->end())
		{
			l_bReturn &= GetMetadataclass()->AddElement(Split(*l_itGr));
		}
	}

	return l_bReturn;
}

SP_DS_Metadata* SP_DS_Metadata::Split(SP_Graphic* p_pcGraphic)
{
	SP_DS_Metadata* l_pcNew= NULL;
	SP_ListAttribute::const_iterator l_AttrIter;

	CHECK_POINTER(p_pcGraphic, return this);
	CHECK_POINTER(GetMetadataclass(), return NULL);

	// if this node has only one graphic left, we don't need to create a new one
	// or it is the last ordinary node and all other graphics are coarse ones
	if (GetGraphics())
	{
		SP_ListGraphic::iterator l_Iter;
		int l_nOrdinary = 0;
		for (l_Iter = GetGraphics()->begin(); l_Iter != GetGraphics()->end(); ++l_Iter)
			if ((*l_Iter)->GetGraphicState() == SP_STATE_NORMAL)
				l_nOrdinary++;
		if (l_nOrdinary <= 1)
		{
			SetLogical(FALSE);
			return this;
		}
	}

	// clone new node based on this, but without any graphics
	l_pcNew = this->Clone(FALSE);
	for (l_AttrIter = m_lAttributes.begin(); l_AttrIter != m_lAttributes.end(); ++l_AttrIter)
	{
		SP_DS_Attribute* l_pcAttribute = (*l_AttrIter);
		l_pcAttribute->Split(l_pcNew->GetAttribute(l_pcAttribute->GetName()),
				p_pcGraphic->GetGraphicChildren());
	}

	SP_Data::RemoveGraphic(p_pcGraphic, FALSE);
	l_pcNew->AddGraphic(p_pcGraphic);
	p_pcGraphic->SetDataParent(l_pcNew);
	// the value of the attribute was taken care of by OnSplit
	// now, reflect the state in the new node
	l_pcNew->SetLogical(FALSE);

	return l_pcNew;
}

bool SP_DS_Metadata::HasEqualAttribute(SP_DS_Attribute* p_pcAttribute)
{
	CHECK_POINTER(p_pcAttribute, return FALSE);
	CHECK_POINTER(GetAttribute(p_pcAttribute->GetName()), return FALSE);

	wxString l_sFirst = GetAttribute(p_pcAttribute->GetName())->GetValueString();
	wxString l_sSecond = p_pcAttribute->GetValueString();

	bool l_bReturn = l_sFirst.IsSameAs(l_sSecond);

	return l_bReturn;
}

bool SP_DS_Metadata::HasAttributeType(SP_ATTRIBUTE_TYPE p_eVal)
{
	for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
	{
		if (l_pcAttr->GetAttributeType() == p_eVal)
			return TRUE;
	}
	return FALSE;
}

SP_DS_Attribute* SP_DS_Metadata::GetFirstAttributeByType(SP_ATTRIBUTE_TYPE p_eVal)
{
	for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
	{
		if (l_pcAttr->GetAttributeType() == p_eVal)
			return l_pcAttr;
	}
	return NULL;
}

bool SP_DS_Metadata::SqueezeIdAttribute()
{
	for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
	{
		if (l_pcAttr->GetAttributeType() == SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID)
			return dynamic_cast<SP_DS_IdAttribute*>(l_pcAttr)->Squeeze();
	}
	return FALSE;
}

bool SP_DS_Metadata::ShowOnCanvas(SP_GUI_Canvas* p_pcCanvas, bool p_bLocalOnly,
		double p_nX, double p_nY, int p_nKeys)
{
	CHECK_POINTER(p_pcCanvas, return FALSE);

	bool l_bReturn= TRUE;
	SP_ListGraphic* l_plGraphic = GetGraphics();
	if (l_plGraphic)
	{
		for (SP_Graphic* l_pcGr : *l_plGraphic)
		{
			l_bReturn &= l_pcGr->AddToCanvas(p_pcCanvas, p_nX, p_nY, p_nKeys);
		}
	}

	return l_bReturn && Update(p_bLocalOnly);
}

bool SP_DS_Metadata::CheckIntegrity()
{
	bool l_bReturn= TRUE;

	for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
	{
		l_bReturn &= (l_pcAttr)->CheckIntegrity();
	}

	SP_ListGraphic* l_plGraphic = GetGraphics();
	SP_SetUInt l_Netnumbers;
	if (l_plGraphic)
	{
		if(GetNetnumber() != 1)
		{
			if(!GetMetadataclass()->GetParentGraph()->GetCoarseNode(GetNetnumber()))
			{
				SetNetnumber(1, GetNetnumber());
			}
		}
		for (SP_Graphic* l_pcGraphic : *l_plGraphic)
		{
			l_bReturn &= l_pcGraphic->CheckIntegrity();
			l_Netnumbers.insert(l_pcGraphic->GetNetnumber());
		}
	}

	if(!l_Netnumbers.empty())
	{
		if(l_Netnumbers.find(GetNetnumber()) == l_Netnumbers.end())
		{
			SetNetnumber(*(l_Netnumbers.begin()), GetNetnumber());
		}
	}

	return l_bReturn;
}

bool SP_DS_Metadata::GetGraphicsInNet(SP_ListGraphic* p_plGraphics,
		unsigned int p_nNet, const unsigned int p_eFilter)
{
	if (!p_plGraphics)
		return FALSE;

	if (!(GetElementType() & p_eFilter))
		return TRUE;

	bool l_bReturn= TRUE;
	for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
	{
		l_bReturn &= l_pcAttr->GetGraphicsInNet(p_plGraphics, p_nNet, p_eFilter);
	}

	return SP_Data::GetGraphicsInNet(p_plGraphics, p_nNet, p_eFilter)
			&& l_bReturn;
}

bool SP_DS_Metadata::OnFlatten(unsigned int p_nToNet, unsigned int p_nFromNet,
		SP_ListGraphic* p_plGraphics)
{
	bool l_bReturn= TRUE;
	for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
	{
		l_bReturn &= l_pcAttr->OnFlatten(p_nToNet, p_nFromNet, p_plGraphics);
	}

	return SP_Data::OnFlatten(p_nToNet, p_nFromNet, p_plGraphics) && l_bReturn;
}

bool SP_DS_Metadata::ShowAttributeGraphic(SP_GR_Metadata* p_pcGraphic)
{
	bool l_bReturn= TRUE;

	for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
	{
		l_bReturn &= l_pcAttr->ShowGraphic(p_pcGraphic);
	}

	return l_bReturn;
}

bool SP_DS_Metadata::AddDialogAttributes(SP_MapString2ListAttribute* p_ptmAttributes)
{
	CHECK_POINTER(p_ptmAttributes, return FALSE);

	// iterate over the list of attributes and insert them into the value of
	// the map, i.e. the list of attributes under the key, combined of
	// wxT("<name of the class>|<name of the attribute>|<type of the attribute>")
	for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
	{
    	wxString l_sKey = SP_Core::Instance()->GetDialogWidgetKey(l_pcAttr);
        //(*p_ptmAttributes)[l_sKey].push_back(l_pcAttr);
        SP_ListAttribute& l_lAttr = (*p_ptmAttributes)[l_sKey];
        if(SP_Find(l_lAttr, l_pcAttr) == l_lAttr.end())
        {
        	l_lAttr.push_back(l_pcAttr);
        }
	}

	return TRUE;
}

bool SP_DS_Metadata::Remove()
{
	bool l_bReturn= TRUE;
	// remove me from the MetadataClass list
	if (m_pcMetadataclass)
		l_bReturn &= m_pcMetadataclass->RemoveElement(this);

	return l_bReturn;
}

const wxString SP_DS_Metadata::GetClassName()
{
	if (m_pcMetadataclass)
		return m_pcMetadataclass->GetName();
	return wxT("");
}

SP_ElementClass*
SP_DS_Metadata::GetClassObject()
{
	return m_pcMetadataclass;
}

bool SP_DS_Metadata::Update(bool p_bLocalOnly)
{
	bool l_bReturn= TRUE;

	for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
	{
		l_bReturn &= l_pcAttr->Update(p_bLocalOnly);
	}

	return l_bReturn && SP_Data::Update(p_bLocalOnly);
}

