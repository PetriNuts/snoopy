//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_core/base/SP_Data.h"
#include "sp_core/base/SP_Graphic.h"
#include "sp_core/SP_Core.h"

#include "sp_ds/extensions/SP_DS_Coarse.h"

unsigned long g_nIdCounter = 0;
unsigned int g_nDialogID= 0;

SP_Data::SP_Data(SP_ELEMENT_TYPE p_eType)
:SP_Type(p_eType),
m_bLogical(FALSE),
m_pcCoarse(NULL),
m_eIAState(SP_IA_NORMAL)
{
}

SP_Data::~SP_Data()
{
	wxDELETE(m_pcCoarse);
}

bool
SP_Data::Equals(SP_Data* p_pcVal)
{
    if (!p_pcVal)
        return FALSE;

    if (m_eElementType == p_pcVal->GetElementType())
        return TRUE;

    return FALSE;
}

bool
SP_Data::Update(bool p_bLocalOnly)
{
    bool l_bReturn = TRUE;

	for (SP_Graphic* l_pcGraphic : *GetGraphics())
	{
		l_bReturn &= l_pcGraphic->Update(p_bLocalOnly);
	}

    l_bReturn &= ((m_pcCoarse)?m_pcCoarse->Update(FALSE, p_bLocalOnly):l_bReturn);

    return l_bReturn;
}

SP_Graphic*
SP_Data::AddGraphic(SP_Graphic* p_pcGraphic)
{
    if (!p_pcGraphic)
        return NULL;

    // add the association to the list, IF NOT already in
    if (SP_Find(*GetGraphics(), p_pcGraphic) == GetGraphics()->end())
    {
    	GetGraphics()->push_back(p_pcGraphic);
    }
    p_pcGraphic->SetDataParent(this);

    return p_pcGraphic;
}

SP_ListGraphic*
SP_Data::GetGraphics()
{
	return &m_lGraphics;
}
SP_Graphic*
SP_Data::AddGraphicInSubnet(unsigned int p_nNetnumber)
{
    if (GetGraphics()->empty())
        return NULL;

    // first of all, take a look, whether we already have some graphic down there
    // if so, this one is prefered, as we use this method from within SP_DS_Edge::CompleteCoarse
    SP_Graphic* l_pcGraphic = GetGraphicInSubnet(p_nNetnumber);

    // otherwise clone a new one
    if (l_pcGraphic)
    {
    	return l_pcGraphic;
    }
    else
    {
    	return NewGraphicInSubnet(p_nNetnumber);
    }
}

SP_Graphic*
SP_Data::NewGraphicInSubnet(unsigned int p_nNetnumber)
{
    if (!GetGraphics() || GetGraphics()->empty())
        return NULL;

    SP_Graphic* l_pcGraphic = (*(GetGraphics()->begin()))->Clone(this);

    l_pcGraphic->SetNetnumber(p_nNetnumber);

    return AddGraphic(l_pcGraphic);
}

SP_Graphic*
SP_Data::GetGraphicInSubnet(unsigned int p_nNetnumber)
{
    if (GetGraphics()->empty())
        return NULL;

    // first of all, take a look, whether we already have some graphic down there
    // if so, this one is prefered, as we use this method from within SP_DS_Edge::CompleteCoarse
    for (SP_Graphic* l_pcGraphic : *GetGraphics())
    {
        // maybe we find a graphic that is already marked as 'delete'
        // In such a case, we have to clone a new one, too.
        if ((l_pcGraphic->GetNetnumber() == p_nNetnumber) && (!l_pcGraphic->GetDelete()))
            return l_pcGraphic;
    }

    return NULL;
}

bool
SP_Data::OnFlatten(unsigned int p_nToNet, unsigned int p_nFromNet, SP_ListGraphic* p_plGraphics)
{
    bool l_bReturn = TRUE;

    if (m_pcCoarse)
        l_bReturn &= m_pcCoarse->OnFlatten(p_nToNet, p_nFromNet);

    for (SP_Graphic* l_pcGraphic : *GetGraphics())
    {
        l_bReturn &= l_pcGraphic->OnFlatten(p_nToNet, p_nFromNet, p_plGraphics);
        if(!l_bReturn)
        	break;
    }

    return SP_NetElement::SetNetnumber(p_nToNet, p_nFromNet);
}

SP_DELETE_STATE
SP_Data::RemoveGraphic(SP_ListGraphic* p_plGraphic, bool p_bDelete)
{
    if (!p_plGraphic)
        return SP_DELETE_ERROR;

    for (SP_Graphic* l_pcGraphic : *p_plGraphic)
	{
    	RemoveGraphic(l_pcGraphic, p_bDelete);
	}
    p_plGraphic->clear();
	return GetGraphics()->empty() ? SP_NO_MORE_OBJECTS : SP_MORE_OBJECTS;
}

bool
SP_Data::RemoveGraphic(SP_Graphic* p_pcGraphic, bool p_bDelete)
{
    if (!p_pcGraphic)
        return TRUE;

	// remove the graphic from the list
    GetGraphics()->remove(p_pcGraphic);
    // remove from graphics parent childrens list
	p_pcGraphic->GetGraphicParent()->RemoveGraphicChildren(p_pcGraphic);
  	//
    p_pcGraphic->SetDataParent(NULL);
	// delete the graphic
	if (p_bDelete)
	{
		wxDELETE(p_pcGraphic);
	}

    return TRUE;
}

bool
SP_Data::HasUndeletedGraphics()
{
	for (SP_Graphic* l_pcGraphic : *GetGraphics())
    {
        // maybe we find a graphic that is already marked as 'delete'
        if (!l_pcGraphic->GetDelete())
            return TRUE;
    }
    return FALSE;
}

SP_DELETE_STATE
SP_Data::RemoveGraphic(unsigned int p_nNetnumber)
{
    // search this in the global association
    SP_ListGraphic l_plGraphic;
    for (SP_Graphic* l_pcGr : *GetGraphics())
    {
    	if (l_pcGr->GetNetnumber() == p_nNetnumber)
		{
    		l_plGraphic.push_back(l_pcGr);
		}
    }
    return RemoveGraphic(&l_plGraphic, true);
}

bool
SP_Data::RemoveAllGraphics(bool p_bDelete)
{
    SP_ListGraphic* l_plGraphics = GetGraphics();
    if(p_bDelete)
    {
        for (SP_Graphic* l_pcGr : *l_plGraphics)
        {
            wxDELETE(l_pcGr);
        }
    }
    l_plGraphics->clear();
    return TRUE;
}

bool
SP_Data::GetGraphicsInNet(SP_ListGraphic* p_plGraphics, unsigned int p_nNet, const unsigned int p_eFilter)
{
    if (!p_plGraphics)
        return FALSE;

    if (!(GetElementType() & p_eFilter))
        return TRUE;

    bool l_bReturn = TRUE;

	for (SP_Graphic* l_pcGraphic : *GetGraphics())
	{
		if (l_pcGraphic->GetNetnumber() == p_nNet)
		{
			p_plGraphics->push_back(l_pcGraphic);
		}
    }

    return l_bReturn;
}

bool
SP_Data::SetNetnumber(unsigned int p_nNewVal, unsigned int p_nOldVal)
{
    bool l_bReturn = TRUE;

	for (SP_Graphic* l_pcGraphic : *GetGraphics())
	{
		l_bReturn &= l_pcGraphic->SetNetnumber(p_nNewVal, p_nOldVal);
	}

    return (l_bReturn && SP_NetElement::SetNetnumber(p_nNewVal, p_nOldVal));
}

bool
SP_Data::AddToCopyMap(SP_DS_Graph** p_ppcGraph,
                      unsigned int p_nNetnumber,
                      SP_MapGraphic2Graphic* p_pmGraphic2Graphic,
                      SP_MapData2Data* p_pmData2Data)
{
    bool l_bReturn = TRUE;

	for (SP_Graphic* l_pcGraphic : *GetGraphics())
	{
		if ((p_nNetnumber == 0) ||
			(l_pcGraphic->GetNetnumber() == p_nNetnumber))
		{
			l_bReturn &= l_pcGraphic->AddToCopyMaps(p_ppcGraph, p_pmGraphic2Graphic, p_pmData2Data);
		}
	}

    return l_bReturn;
}

bool
SP_Data::ResetNetnumbers(SP_MapUInt2UInt* p_pmNet2Net, SP_DS_Graph* p_pcToGraph)
{
    if (!p_pmNet2Net || !p_pcToGraph)
        return FALSE;

    bool l_bReturn = TRUE;

	for (SP_Graphic* l_pcGraphic : *GetGraphics())
	{
		l_bReturn &= l_pcGraphic->ResetNetnumbers(p_pmNet2Net, p_pcToGraph);
	}

    SP_NetElement::SetNetnumber(GetPasteNetnumber(p_pmNet2Net, GetNetnumber(), p_pcToGraph));

    if (m_pcCoarse)
        m_pcCoarse->SetNetnumber(GetPasteNetnumber(p_pmNet2Net, m_pcCoarse->GetNetnumber(), p_pcToGraph), m_pcCoarse->GetNetnumber());

    return l_bReturn;
}

bool
SP_Data::MergeData(SP_Data* p_pcElement)
{
    if (!p_pcElement)
        return TRUE;
    if (GetElementType() != p_pcElement->GetElementType())
        return FALSE;

    for (SP_Graphic* l_pcGraphic : *p_pcElement->GetGraphics())
    {
        AddGraphic(l_pcGraphic);
        l_pcGraphic->SetDataParent(this);
    }

    return p_pcElement->RemoveAllGraphics();
}

bool
SP_Data::SetShow(bool p_bVal)
{
    bool l_bReturn = TRUE;

	for (SP_Graphic* l_pcGraphic : *GetGraphics())
	{
		l_bReturn &= (l_pcGraphic->SetShow(p_bVal) == p_bVal);
	}
    return l_bReturn;
}

bool
SP_Data::GetShow()
{
    bool l_bReturn = !GetGraphics()->empty();

    for (SP_Graphic* l_pcGraphic : *GetGraphics())
    {
		l_bReturn &= l_pcGraphic->GetShow();
	}
    return l_bReturn;
}

SP_DS_Coarse*
SP_Data::SetCoarse(SP_DS_Coarse* p_pcVal)
{
    wxASSERT(!m_pcCoarse);
    if (!p_pcVal)
        return NULL;

    m_pcCoarse = p_pcVal;
    m_pcCoarse->SetParent(this);

    return m_pcCoarse;
}

bool
SP_Data::Flatten(unsigned int p_nNewNet, SP_ListGraphic* p_plGraphics)
{
    if (m_pcCoarse)
        return m_pcCoarse->Flatten(p_nNewNet, p_plGraphics);

    return FALSE;
}

bool
SP_Data::Coarse(unsigned int p_nNewNet, SP_Graphic* p_pcGraphic, SP_Graphic* p_pcCoarseGr, SP_ListGraphic* p_plShapes)
{
    if (!p_pcGraphic)
        return FALSE;

    bool l_bReturn = p_pcGraphic->OnCoarse(p_nNewNet, p_pcCoarseGr, p_plShapes);

    SP_NetElement::SetNetnumber(p_nNewNet);

    if (m_pcCoarse)
        l_bReturn &= m_pcCoarse->OnCoarse(p_nNewNet);

    return l_bReturn;
}

bool
SP_Data::ResetCoarseTree()
{
    if (!m_pcCoarse)
        return TRUE;

    m_pcCoarse->SetUpdate(FALSE);
    return m_pcCoarse->Update(FALSE, TRUE);
}

bool
SP_Data::OnDoubleClick(SP_Graphic* p_pcTarget, int p_nKeys)
{
    if (m_pcCoarse && p_nKeys == KEY_SHIFT)
        return m_pcCoarse->Show();

    if (SP_Core::Instance()->GetAnimMode())
        return TRUE;

    return FALSE;
}

bool
SP_Data::ShowDebug()
{
    bool l_bReturn = TRUE;

/*    SP_LOGDEBUG(wxString::Format(wxT("\t\tSP_Data %p with Coarse %p")
				   wxT(" in net %u"), this, m_pcCoarse,
				   GetNetnumber()));

    SP_ListGraphic* l_plGraphics = GetGraphics();
    if (!l_plGraphics)
        return TRUE;

    SP_ListGraphic::iterator l_Iter;
    for (l_Iter = l_plGraphics->begin(); l_Iter != l_plGraphics->end(); ++l_Iter)
    {
        l_bReturn &= (*l_Iter)->ShowDebug();
    }*/
	SP_LOGDEBUG(wxString::Format(wxT("Elementclass: %s, element type %d"), GetClassName().c_str(), GetElementType()));
	if (GetAttributes()) {
	for (SP_ListAttribute::const_iterator it = GetAttributes()->begin();it != GetAttributes()->end();++it) {
		SP_LOGDEBUG(wxString::Format(wxT("attrib: %s value: %s"), (*it)->GetName().c_str(), (*it)->GetValueString().c_str()));
		if ((*it)->GetAttributes()) {
			for (SP_ListAttribute::const_iterator it2 = (*it)->GetAttributes()->begin();it2 != (*it)->GetAttributes()->end();++it2) {
				SP_LOGDEBUG(wxString::Format(wxT("\tsub-attrib: %s value: %s"), (*it2)->GetName().c_str(), (*it2)->GetValueString().c_str()));
			}
		}
	}}

    return l_bReturn;
}


