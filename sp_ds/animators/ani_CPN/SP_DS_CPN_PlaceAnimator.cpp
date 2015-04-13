//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2009/10/28 10:20:00 $
// Short Description: colored place animator class
//////////////////////////////////////////////////////////////////////
#include "sp_ds/extensions/SP_DS_Animation.h"
#include "sp_ds/animators/ani_CPN/SP_DS_CPN_PlaceAnimator.h"
#include "sp_ds/animators/ani_CPN/SP_DS_CPN_TransAnimator.h"
#include "sp_ds/attributes/SP_DS_MultiplicityAttribute.h"


#include "sp_ds/animators/ani_CPN/SP_DS_CPN_Animation.h"
#include "sp_ds/extensions/Col_PN/SyntaxChecking/SP_CPN_SyntaxChecking.h"

#include "sp_core/SP_Core.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"

#include "sp_gui/dialogs/dia_CPN/SP_DLG_RandomColor.h"



SP_DS_CPN_PlaceAnimator::SP_DS_CPN_PlaceAnimator(const wxString& p_pchAttributeName, SP_DS_Animation* p_pcAnim, SP_DS_Node* p_pcParent):
SP_DS_Animator(p_pcParent, p_pcAnim),
m_pcProtoshape(NULL),
m_pcNode(NULL),
m_sAttributeName(p_pchAttributeName),
m_pcColList(NULL),
m_pcAttribute(NULL),
m_pcNameAttibute(NULL),
m_nDefaultValue(0)
{
    SetAnimatorType(SP_DS_ANIMATOR_PLACE);

    if (p_pcParent)
    {
        m_pcNode = p_pcParent;
        m_pcAttribute = dynamic_cast<SP_DS_MarkingAttribute*>(p_pcParent->GetAttribute(p_pchAttributeName));
        if (m_pcAttribute)
            m_nDefaultValue = m_pcAttribute->GetValue(true);

		m_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(p_pcParent->GetAttribute(SP_DS_CPN_MARKINGLIST) );
		m_pcNameAttibute = dynamic_cast< SP_DS_TextAttribute* >(p_pcParent->GetAttribute(SP_DS_CPN_COLORSETNAME) );

		if(m_pcNameAttibute)
		{
			m_cPlaceMultiSet.SetColorSetName(m_pcNameAttibute->GetValue());			
		}

		if(m_pcColList)
		{
			UpdateMultiSet();
			m_mDefaultValue.clear();
			UpdateDefaultMarking();
		}

        m_pcProtoshape = new SP_GR_MarkAnimator();
    }
}

SP_DS_CPN_PlaceAnimator::~SP_DS_CPN_PlaceAnimator()
{
    if (m_pcProtoshape)
        wxDELETE(m_pcProtoshape);

    m_pcProtoshape = NULL;
}

SP_DS_Animator*
SP_DS_CPN_PlaceAnimator::Clone(SP_Data* p_pcParent, SP_DS_Animation* p_pcAnim)
{
    if (p_pcParent->GetElementType() != SP_ELEMENT_NODE)
        return NULL;

    SP_DS_CPN_PlaceAnimator* l_pcNew = new SP_DS_CPN_PlaceAnimator(m_sAttributeName.c_str(), p_pcAnim, dynamic_cast<SP_DS_Node*>(p_pcParent));

    return dynamic_cast<SP_DS_Animator*>(l_pcNew);
}



SP_MapString2Int SP_DS_CPN_PlaceAnimator::SumCandidatesNew()
{
	SP_MapString2Int l_RetMap;
	l_RetMap.clear();

    map<SP_DS_CPN_TransAnimator*, SP_ListEdge >::iterator l_Iter;

    for (l_Iter = m_mlCandidates.begin(); l_Iter != m_mlCandidates.end(); ++l_Iter)
    {
		SP_MapString2Int l_RetMapEdges;
		l_RetMapEdges = SumEdgesNew((*l_Iter).first,&((*l_Iter).second));

		SP_MapString2Int::iterator itMap;
		for(itMap = l_RetMapEdges.begin(); itMap != l_RetMapEdges.end(); ++itMap)
		{
			if( l_RetMap.find(itMap->first) != l_RetMap.end() )
			{
				l_RetMap[itMap->first] = l_RetMap.find(itMap->first)->second + itMap->second;
			}
			else
			{
				l_RetMap[itMap->first] = itMap->second;
			}
		}		       
    }

    return l_RetMap;

}
SP_MapString2Int SP_DS_CPN_PlaceAnimator::SumEdgesNew(SP_DS_CPN_TransAnimator* p_pcTransAnimator, const SP_ListEdge* p_plEdges)
{
	SP_MapString2Int l_RetMap;
	l_RetMap.clear();

	if (!p_plEdges)
        return l_RetMap;
	//
	
    SP_ListEdge::const_iterator l_itEdge;

   for (l_itEdge = p_plEdges->begin(); l_itEdge != p_plEdges->end(); ++l_itEdge)
    {
		SP_MapString2Int l_RetMapEdges;
		l_RetMapEdges = SumEdgeNew(p_pcTransAnimator,(*l_itEdge));

		SP_MapString2Int::iterator itMap;
		for(itMap = l_RetMapEdges.begin(); itMap != l_RetMapEdges.end(); ++itMap)
		{
			if( l_RetMap.find(itMap->first) != l_RetMap.end() )
			{
				l_RetMap[itMap->first] = l_RetMap.find(itMap->first)->second + itMap->second;
			}
			else
			{
				l_RetMap[itMap->first] = itMap->second;
			}
		}	       
    }

    return l_RetMap;
}
SP_MapString2Int SP_DS_CPN_PlaceAnimator::SumEdgeNew(SP_DS_CPN_TransAnimator* p_pcTransAnimator, SP_DS_Edge* p_pcEdge)
{
	SP_MapString2Int l_RetMap;
	l_RetMap.clear();

	if (!p_pcEdge)
        return l_RetMap;

	if (p_pcEdge->GetClassName() == wxT("Edge"))
	{
		/*begin new*/
		map<wxString, map<SP_DS_Edge*, map<wxString,int> > >* l_mmmBind2Edge2Mult2Color = p_pcTransAnimator->GetBind2Edge2Mult2Color();
		map<wxString, map<SP_DS_Edge*, map<wxString,int> > >::iterator itMap1;
		for(itMap1 = l_mmmBind2Edge2Mult2Color->begin(); itMap1 != l_mmmBind2Edge2Mult2Color->end(); itMap1++)
		{
			if( itMap1->second.find(p_pcEdge) != itMap1->second.end() )
			{
				map<wxString, int>::iterator itMap2;
				for(itMap2 = itMap1->second.find(p_pcEdge)->second.begin(); itMap2 != itMap1->second.find(p_pcEdge)->second.end(); itMap2++)
				{
					l_RetMap[itMap2->first] = itMap2->second;
				}
			}
		}
		/*end new*/

/*
		SP_DS_ColListAttribute* l_pcColList	= dynamic_cast< SP_DS_ColListAttribute* >(p_pcEdge->GetAttribute(SP_DS_CPN_INSCRIPTION) );

		if(l_pcColList && l_pcColList->GetRowCount() > 1)
		{	
			for (unsigned int i = 1; i < l_pcColList->GetRowCount(); i++)
			{
				wxString l_sColor = l_pcColList->GetCell(i, 0);		
				wxString l_sAppearance = l_pcColList->GetCell(i, 1);
				long l_nAppearance;
				l_sAppearance.ToLong(&l_nAppearance);
				l_RetMap[l_sColor] = l_nAppearance;
			}
		}
*/
	}

	return l_RetMap;
}


bool
SP_DS_CPN_PlaceAnimator::TestMarking()
{
    if (!m_pcNode || !m_pcAttribute || (m_mlCandidates.size() == 0))
        return FALSE;

    map<SP_DS_CPN_TransAnimator*, SP_ListEdge >::iterator l_Iter;
		for (l_Iter = m_mlCandidates.begin(); l_Iter != m_mlCandidates.end(); ++l_Iter) 
		{
			// tell target animator to be enabled by me if we have enough markings			
			l_Iter->first->Enable(this);  // Enable it, not need to test enable
		}

		return (m_mlCandidates.size() > 0);
}

bool
SP_DS_CPN_PlaceAnimator::ResolveConflicts()
{  
	/*begin new code*/
	vector<SP_DS_CPN_TransAnimator*> l_vpcTransAnimator;
	vector<SP_ListEdge> l_vListEdge;
	map<SP_DS_CPN_TransAnimator*, SP_ListEdge >::iterator l_Iter;
	for( l_Iter = m_mlCandidates.begin(); l_Iter != m_mlCandidates.end(); ++l_Iter )
	{
		l_vpcTransAnimator.push_back(l_Iter->first);
		l_vListEdge.push_back(l_Iter->second);
	}
	SP_MapString2Int l_mOriginalColorMultiset = *(m_cPlaceMultiSet.GetMultiSetMap());

	while(l_vpcTransAnimator.size() != 0)
	{
		int l_nCurSel = SP_RandomLong(l_vpcTransAnimator.size());
		SP_MapString2Int l_mSumMultiplicities = SumEdgesNew(l_vpcTransAnimator[l_nCurSel],&(l_vListEdge[l_nCurSel]));
		if( m_cPlaceMultiSet.IsMarking( l_mSumMultiplicities ) )
		{			
			m_cPlaceMultiSet.RemoveMultiSet(l_mSumMultiplicities);
		}
		else
		{
			map<wxString, map<SP_DS_Edge*, map<wxString,int> > >* l_mmmBind2Edge2Mult2Color = l_vpcTransAnimator[l_nCurSel]->GetBind2Edge2Mult2Color();
			map<wxString, map<SP_DS_Edge*, map<wxString,int> > >::iterator itMap1;
			vector<wxString> l_vConflictBindings;
			for(itMap1 = l_mmmBind2Edge2Mult2Color->begin(); itMap1 != l_mmmBind2Edge2Mult2Color->end(); itMap1++)
			{
				SP_ListEdge::const_iterator l_itEdge;
				SP_MapString2Int l_RetMap;
				for (l_itEdge = l_vListEdge[l_nCurSel].begin(); l_itEdge != l_vListEdge[l_nCurSel].end(); ++l_itEdge)
				{
					if( itMap1->second.find(*l_itEdge) != itMap1->second.end() &&  (*l_itEdge)->GetClassName() == wxT("Edge") )
					{
						map<wxString, int>::iterator itMap2;
						for(itMap2 = itMap1->second.find(*l_itEdge)->second.begin(); itMap2 != itMap1->second.find(*l_itEdge)->second.end(); itMap2++)
						{
							l_RetMap[itMap2->first] = itMap2->second;
						}
					}
				}

				if( m_cPlaceMultiSet.IsMarking( l_RetMap ) )
				{
					m_cPlaceMultiSet.RemoveMultiSet(l_mSumMultiplicities);
				}
				else
				{
					l_vConflictBindings.push_back(itMap1->first);					
				}					
			}
			for(unsigned int i = 0; i < l_vConflictBindings.size(); i++)
			{
				map<wxString, map<SP_DS_Edge*, map<wxString,int> > >::iterator itMap3;
				itMap3 = m_mlCandidates.find(l_vpcTransAnimator[l_nCurSel])->first->GetBind2Edge2Mult2Color()->find(l_vConflictBindings[i]);
				m_mlCandidates.find(l_vpcTransAnimator[l_nCurSel])->first->GetBind2Edge2Mult2Color()->erase(itMap3);
			}
			if( m_mlCandidates.find(l_vpcTransAnimator[l_nCurSel])->first->GetBind2Edge2Mult2Color()->size() == 0 )
			{
				m_mlCandidates.find(l_vpcTransAnimator[l_nCurSel])->first->Reset();
			}
		}

		l_vpcTransAnimator.erase (l_vpcTransAnimator.begin()+l_nCurSel);
		l_vListEdge.erase (l_vListEdge.begin()+l_nCurSel);
	}

	m_cPlaceMultiSet.SetMultiSet(l_mOriginalColorMultiset);

	for (l_Iter = m_mlCandidates.begin(); l_Iter != m_mlCandidates.end(); ++l_Iter)
	{
        l_Iter->first->SetEnabled(TRUE);
	}


	/*end new code*/

/*
	int l_nDelete = 0, i;
    map<SP_DS_CPN_TransAnimator*, SP_ListEdge >::iterator l_Iter;
    
	while ( ! m_cPlaceMultiSet.IsMarking( SumCandidatesNew()))
    {
        l_nDelete = SP_RandomLong(m_mlCandidates.size());
        l_Iter = m_mlCandidates.begin();
        for (i = 0; i < l_nDelete; i++)
            l_Iter++;
        l_Iter->first->Reset();
    }
    for (l_Iter = m_mlCandidates.begin(); l_Iter != m_mlCandidates.end(); ++l_Iter)
        l_Iter->first->SetEnabled(TRUE);
*/
    return TRUE;
}


bool
SP_DS_CPN_PlaceAnimator::ResolveConflicts1()
{

  map<SP_DS_CPN_TransAnimator*, SP_ListEdge >::iterator l_Iter;

  for( l_Iter = m_mlCandidates.begin(); l_Iter != m_mlCandidates.end(); ++l_Iter ) {

    l_Iter->first->SetEnabled( TRUE );

  }

  return TRUE;

}

// animate
bool
SP_DS_CPN_PlaceAnimator::PreStep(unsigned int p_nSteps, SP_DIRECTION_T p_nDir)
{
    if (!m_bEnabled || !m_pcNode || !m_pcAttribute || !m_pcAnimation ||
         (m_mlCandidates.size() == 0))
        return FALSE;

    m_nSteps = p_nSteps;
    SP_ListGraphic* l_plGraphic;
    map<SP_DS_CPN_TransAnimator*, SP_ListEdge >::iterator l_Iter;
    SP_ListEdge::const_iterator l_itEdge;
    SP_GR_MarkAnimator* l_pcAnim;
    bool l_bReturn = FALSE;

    for (l_Iter = m_mlCandidates.begin(); l_Iter != m_mlCandidates.end(); ++l_Iter)
    {
        if (!l_Iter->first->GetEnabled())
            continue;
        l_bReturn = TRUE;
        for (l_itEdge = l_Iter->second.begin(); l_itEdge != l_Iter->second.end(); ++l_itEdge)
        {
            l_plGraphic = (*l_itEdge)->GetGraphics();
            if (l_plGraphic)
            	for (SP_Graphic* l_pcGraphic : *l_plGraphic)
                {
                    l_pcAnim = dynamic_cast<SP_GR_MarkAnimator*>(m_pcProtoshape->Clone(this));
                    if (l_pcAnim && l_pcAnim->SetPath(l_pcGraphic, m_nSteps, p_nDir))
                    {
                        AddGraphic(l_pcAnim);
                        m_pcAnimation->AddCanvas(l_pcAnim->GetCanvas());
                    }
                    else
                        if (l_pcAnim)
                            wxDELETE(l_pcAnim);
                }
        }
    }

		DecrementMark();

    return l_bReturn;
}

bool
SP_DS_CPN_PlaceAnimator::Step(unsigned int p_nStep)
{
    if (!GetGraphics() || (p_nStep >= m_nSteps))
        return TRUE;

    bool l_bReturn = TRUE;
    SP_ListGraphic::iterator l_Iter;
    for (l_Iter = GetGraphics()->begin(); l_Iter != GetGraphics()->end(); ++l_Iter)
    {
    	SP_Graphic* l_pcGr = *l_Iter;
        l_bReturn &= dynamic_cast<SP_GR_Animator*>(l_pcGr)->Step(p_nStep);
    }
    return l_bReturn;
}

bool
SP_DS_CPN_PlaceAnimator::PostStep()
{
    SP_ListGraphic* l_plGraphics = GetGraphics();
    // clean up temps
    m_mlCandidates.clear();
    m_bEnabled = FALSE;

    if (!l_plGraphics)
        return TRUE;

    SP_ListGraphic::iterator l_Iter;
    SP_GR_MarkAnimator* l_pcGr;
    for (l_Iter = l_plGraphics->begin(); l_Iter != l_plGraphics->end(); l_Iter = l_plGraphics->begin())
    {
    	SP_Graphic* l_pcGrIt = *l_Iter;
        l_pcGr = dynamic_cast<SP_GR_MarkAnimator*>(l_pcGrIt);
        l_plGraphics->erase(l_Iter);
        wxDELETE(l_pcGr);
    }

    return RemoveAllGraphics();
}

bool
SP_DS_CPN_PlaceAnimator::OnLeftClick()
{
    CHECK_POINTER(m_pcAttribute, return FALSE);
    CHECK_POINTER(m_pcNode, return FALSE);

	SP_DS_CPN_Animation* l_pcCPNanimation = dynamic_cast<SP_DS_CPN_Animation*>(m_pcAnimation);

	m_bChooseRandomColor = l_pcCPNanimation->GetChooseRandomColorFlag();

	vector<wxString> l_ColorVector;

	if( ! l_pcCPNanimation->GetColorSetClass() )
		return false;

	if(!l_pcCPNanimation->GetColorSetClass()->LookupColorSet(m_pcNameAttibute->GetValue()))
		return false;
	
	
	SP_CPN_ColorSet* l_pcColorSet =l_pcCPNanimation->GetColorSetClass()->LookupColorSet(m_pcNameAttibute->GetValue());
	    
	if(l_pcColorSet->GetDataType() == CPN_UNION)
	{
		l_ColorVector = l_pcColorSet->GetStringValue(); //get the union colors
	}
	else
	{
		l_ColorVector = l_pcColorSet->GetStringValue();
	}
	
	//
	wxString l_sColor;	
	long l_nNumber;
	if(m_bChooseRandomColor)
	{
		int l_nChose = SP_RandomLong(l_ColorVector.size());
		l_sColor = l_ColorVector[l_nChose];
		l_nNumber = 1;
	}
	else
	{
		if(l_ColorVector.size() == 0)
		{
			SP_MESSAGEBOX(wxT("No colors for choosing"), wxT("Notification"), wxOK | wxICON_INFORMATION );
			return true;
		}
		SP_DLG_RandomColor l_cDlg(&l_ColorVector,NULL);
		if (l_cDlg.ShowModal() == wxID_OK)
		{			
			
			l_sColor = l_cDlg.GetColor();
			l_nNumber = l_cDlg.GetNumber();	
			l_cDlg.Destroy();			
		}
		else
		{
			l_cDlg.Destroy();
			return false;
		}	
	}

	m_cPlaceMultiSet.AddMultiSet(l_sColor,l_nNumber);
	
	SP_MapString2Int* l_pMultiSet = m_cPlaceMultiSet.GetMultiSetMap();
	SP_MapString2Int::iterator itMap;
	int l_nMultiSetNum = 0;
	m_pcColList->Clear();
	for(itMap = l_pMultiSet->begin(); itMap != l_pMultiSet->end(); itMap++)
	{		
		l_nMultiSetNum += itMap->second;
		wxString l_sAppearance = wxString::Format(wxT("%d"),itMap->second);
		unsigned int l_nNewRow = m_pcColList->AppendEmptyRow();
		m_pcColList->SetCell(l_nNewRow, 0, itMap->first);
		m_pcColList->SetCell(l_nNewRow, 1, l_sAppearance);		
	}	
    m_pcAttribute->SetValue(m_pcAttribute->GetValue() + 1);
    m_pcNode->Update(TRUE);

	m_pcParent->Update(TRUE);
	SP_Core::Instance()->GetRootDocument()->Modify(TRUE);

    return TRUE;
}

bool
SP_DS_CPN_PlaceAnimator::OnRightClick()
{
    CHECK_POINTER(m_pcAttribute, return FALSE);
    CHECK_POINTER(m_pcNode, return FALSE);

	SP_DS_CPN_Animation* l_pcCPNanimation = dynamic_cast<SP_DS_CPN_Animation*>(m_pcAnimation);

	m_bChooseRandomColor = l_pcCPNanimation->GetChooseRandomColorFlag();


	SP_MapString2Int* l_pcMultiMap;
	l_pcMultiMap = m_cPlaceMultiSet.GetMultiSetMap();
	
	if(l_pcMultiMap->size() == 0)
		return true;

	vector<wxString> l_ColorVector;
	SP_MapString2Int::iterator itMap;
	for(itMap = l_pcMultiMap->begin(); itMap != l_pcMultiMap->end(); itMap++)
	{
		l_ColorVector.push_back(itMap->first);
	}

	wxString l_sColor;	
	long l_nNumber;
	if(m_bChooseRandomColor)
	{
		int l_nChose = SP_RandomLong(l_ColorVector.size());
		l_sColor = l_ColorVector[l_nChose];
		l_nNumber = 1;
	}
	else
	{
		if(l_ColorVector.size() == 0)
		{
			SP_MESSAGEBOX(wxT("No colors for choosing"), wxT("Notification"), wxOK | wxICON_INFORMATION );
			return true;
		}
		SP_DLG_RandomColor l_cDlg(&l_ColorVector,NULL);
		if (l_cDlg.ShowModal() == wxID_OK)
		{			
			
			l_sColor = l_cDlg.GetColor();
			l_nNumber = l_cDlg.GetNumber();	
			l_cDlg.Destroy();			
		}
		else
		{
			l_cDlg.Destroy();
			return false;
		}	
	}

	if( l_pcMultiMap->find(l_sColor)->second < l_nNumber )
	{
		SP_MESSAGEBOX(wxT("Not enough tokens for choosing"), wxT("Notification"), wxOK | wxICON_INFORMATION );
		return true;
	}
	
	m_cPlaceMultiSet.RemoveMultiSet(l_sColor,l_nNumber);


	l_pcMultiMap = m_cPlaceMultiSet.GetMultiSetMap();	
	int l_nMultiSetNum = 0;
	m_pcColList->Clear();
	for(itMap = l_pcMultiMap->begin(); itMap != l_pcMultiMap->end(); itMap++)
	{		
		l_nMultiSetNum += itMap->second;
		wxString l_sAppearance = wxString::Format(wxT("%d"),itMap->second);
		unsigned int l_nNewRow = m_pcColList->AppendEmptyRow();
		m_pcColList->SetCell(l_nNewRow, 0, itMap->first);
		m_pcColList->SetCell(l_nNewRow, 1, l_sAppearance);		
	}	
    m_pcAttribute->SetValue(m_pcAttribute->GetValue() - 1);
    m_pcNode->Update(TRUE);

	SP_Core::Instance()->GetRootDocument()->Modify(TRUE);


    return TRUE;
}

bool
SP_DS_CPN_PlaceAnimator::Enable(SP_DS_CPN_TransAnimator* p_pcAnim)
{
    if (m_mlCandidates.find(p_pcAnim) != m_mlCandidates.end())
        m_bEnabled = TRUE;

    return TRUE;
}

bool
SP_DS_CPN_PlaceAnimator::DecrementMark()
{
    CHECK_POINTER(m_pcAttribute, return FALSE);
    CHECK_POINTER(m_pcNode, return FALSE);

	//
	map<SP_DS_CPN_TransAnimator*, SP_ListEdge>::iterator it;
	for (it = m_mlCandidates.begin(); it != m_mlCandidates.end(); ++it)
	{
		SP_ListEdge* l_pcEdges = &(it->second); 
		SP_ListEdge::const_iterator eIt;
		for (eIt = l_pcEdges->begin(); eIt != l_pcEdges->end(); ++eIt)
		{
			if ((*eIt)->GetClassName() == wxT("Edge"))
			{
				/*begin new code*/
				for( unsigned int i = 0; i < it->first->GetSelBindings()->size(); i++ )
				{
					wxString l_sBinding = (*(it->first->GetSelBindings()))[i];					
					map<wxString, map<SP_DS_Edge*, map<wxString,int> > >* l_mmmBind2Edge2Mult2Color = it->first->GetBind2Edge2Mult2Color();
					map<wxString, map<SP_DS_Edge*, map<wxString,int> > >::iterator itMap1;
					itMap1 = l_mmmBind2Edge2Mult2Color->find(l_sBinding);
					map<wxString, int> l_mEdgeColor2Mult;
					l_mEdgeColor2Mult  = itMap1->second.find((*eIt))->second;
					map<wxString, int>::iterator itMap2;
					for(itMap2 = l_mEdgeColor2Mult.begin(); itMap2 != l_mEdgeColor2Mult.end(); itMap2++)
					{
						m_cPlaceMultiSet.RemoveMultiSet(itMap2->first,itMap2->second);	
					}
				}
				/*end new code*/
/*
				SP_DS_ColListAttribute* l_pcColList	= dynamic_cast< SP_DS_ColListAttribute* >((*eIt)->GetAttribute(SP_DS_CPN_INSCRIPTION) );
				if(l_pcColList && l_pcColList->GetRowCount() > 1)
				{	
					for (unsigned int i = 1; i < l_pcColList->GetRowCount(); i++)
					{
						wxString l_sColor = l_pcColList->GetCell(i, 0);		
						wxString l_sAppearance = l_pcColList->GetCell(i, 1);
						long l_nAppearance;
						l_sAppearance.ToLong(&l_nAppearance);
						m_cPlaceMultiSet.RemoveMultiSet(l_sColor,l_nAppearance);						
					}
				}	
*/
			}
			else if ((*eIt)->GetClassName() == wxT("Reset Edge"))
			{
				/*begin new code*/
				for( unsigned int i = 0; i < it->first->GetSelBindings()->size(); i++ )
				{
					wxString l_sBinding = (*(it->first->GetSelBindings()))[i];					
					map<wxString, map<SP_DS_Edge*, map<wxString,int> > >* l_mmmBind2Edge2Mult2Color = it->first->GetBind2Edge2Mult2Color();
					map<wxString, map<SP_DS_Edge*, map<wxString,int> > >::iterator itMap1;
					itMap1 = l_mmmBind2Edge2Mult2Color->find(l_sBinding);
					map<wxString, int> l_mEdgeColor2Mult;
					l_mEdgeColor2Mult  = itMap1->second.find((*eIt))->second;
					map<wxString, int>::iterator itMap2;
					for(itMap2 = l_mEdgeColor2Mult.begin(); itMap2 != l_mEdgeColor2Mult.end(); itMap2++)
					{
						wxString l_sColor = itMap2->first;		
						if(m_cPlaceMultiSet.GetMultiSetMap()->find(l_sColor) != m_cPlaceMultiSet.GetMultiSetMap()->end())
						{
							int l_nAppearance = m_cPlaceMultiSet.GetMultiSetMap()->find(l_sColor)->second;
							m_cPlaceMultiSet.RemoveMultiSet(l_sColor,l_nAppearance);
						}
					}
				}
				/*end new code*/

/*				SP_DS_ColListAttribute* l_pcColList	= dynamic_cast< SP_DS_ColListAttribute* >((*eIt)->GetAttribute(SP_DS_CPN_INSCRIPTION) );
				if(l_pcColList && l_pcColList->GetRowCount() > 1)
				{	
					for (unsigned int i = 1; i < l_pcColList->GetRowCount(); i++)
					{
						wxString l_sColor = l_pcColList->GetCell(i, 0);		
						if(m_cPlaceMultiSet.GetMultiSetMap()->find(l_sColor) != m_cPlaceMultiSet.GetMultiSetMap()->end())
						{
							int l_nAppearance = m_cPlaceMultiSet.GetMultiSetMap()->find(l_sColor)->second;
							m_cPlaceMultiSet.RemoveMultiSet(l_sColor,l_nAppearance);
						}
					}
				}
*/
			}
			else
			{
				continue;
			}
		}
		if(m_cPlaceMultiSet.GetMultiSetMap()->size() == 0)
			break;
	}

	//	
	SP_MapString2Int::iterator itMap;
	SP_MapString2Int* l_pMultiSet = m_cPlaceMultiSet.GetMultiSetMap();
	int l_nMultiSetNum = 0;
	m_pcColList->Clear();
	for(itMap = l_pMultiSet->begin(); itMap != l_pMultiSet->end(); itMap++)
	{		
		l_nMultiSetNum += itMap->second;
		wxString l_sAppearance = wxString::Format(wxT("%d"),itMap->second);
		unsigned int l_nNewRow = m_pcColList->AppendEmptyRow();
		m_pcColList->SetCell(l_nNewRow, 0, itMap->first);
		m_pcColList->SetCell(l_nNewRow, 1, l_sAppearance);		
	}	
    
    m_pcAttribute->SetValue((long)l_nMultiSetNum);
	m_pcNode->Update(TRUE);

    return TRUE;
}

bool
SP_DS_CPN_PlaceAnimator::IncrementMark(SP_DS_CPN_TransAnimator* p_pcTransAnimator, SP_DS_Edge* p_pcEdge)
{
    CHECK_POINTER(m_pcAttribute, return FALSE);
    CHECK_POINTER(m_pcNode, return FALSE);

	/*begin new code*/
	for( unsigned int i = 0; i < p_pcTransAnimator->GetSelBindings()->size(); i++ )
	{
		wxString l_sBinding = (*(p_pcTransAnimator->GetSelBindings()))[i];					
		map<wxString, map<SP_DS_Edge*, map<wxString,int> > >* l_mmmBind2Edge2Mult2Color = p_pcTransAnimator->GetBind2Edge2Mult2Color();
		map<wxString, map<SP_DS_Edge*, map<wxString,int> > >::iterator itMap1;
		itMap1 = l_mmmBind2Edge2Mult2Color->find(l_sBinding);
		map<wxString, int> l_mEdgeColor2Mult;
		l_mEdgeColor2Mult  = itMap1->second.find(p_pcEdge)->second;
		map<wxString, int>::iterator itMap2;
		for(itMap2 = l_mEdgeColor2Mult.begin(); itMap2 != l_mEdgeColor2Mult.end(); itMap2++)
		{
			m_cPlaceMultiSet.AddMultiSet(itMap2->first,itMap2->second);	
		}
	}
	/*end new code*/
/*	
	SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(p_pcEdge->GetAttribute(SP_DS_CPN_INSCRIPTION) );

	if(!l_pcColList)
		return false;
	 if(l_pcColList->GetRowCount() <= 1)
		return false;

	for (unsigned int i = 1; i < l_pcColList->GetRowCount(); i++)
	{
		wxString l_sColor = l_pcColList->GetCell(i, 0);		
		wxString l_sAppearance = l_pcColList->GetCell(i, 1);	
		long l_nAppearance;
		l_sAppearance.ToLong(&l_nAppearance);
		m_cPlaceMultiSet.AddMultiSet(l_sColor,l_nAppearance);
	}
	//Delete the marking mark in the arc
	wxString l_sColor = l_pcColList->GetCell(0, 0);		
	wxString l_sAppearance = l_pcColList->GetCell(0, 1);	
	l_pcColList->Clear();
	unsigned int l_nNewRow = l_pcColList->AppendEmptyRow();
	l_pcColList->SetCell(l_nNewRow, 0, l_sColor);
	l_pcColList->SetCell(l_nNewRow, 1, l_sAppearance);
*/

	//
	SP_MapString2Int::iterator itMap;
	SP_MapString2Int* l_pMultiSet = m_cPlaceMultiSet.GetMultiSetMap();
	int l_nMultiSetNum = 0;
	m_pcColList->Clear();
	for(itMap = l_pMultiSet->begin(); itMap != l_pMultiSet->end(); itMap++)
	{		
		l_nMultiSetNum += itMap->second;
		wxString l_sAppearance = wxString::Format(wxT("%d"),itMap->second);
		unsigned int l_nNewRow = m_pcColList->AppendEmptyRow();
		m_pcColList->SetCell(l_nNewRow, 0, itMap->first);
		m_pcColList->SetCell(l_nNewRow, 1, l_sAppearance);		
	}	
	
	/////////////////////////////////////////////////
   
	m_pcAttribute->SetValue((long)l_nMultiSetNum);

    SP_ListGraphic::iterator l_Iter;
    SP_ListGraphic* l_pcGraphic = m_pcNode->GetGraphics();
    if (l_pcGraphic)
    {
        for (l_Iter = l_pcGraphic->begin(); l_Iter != l_pcGraphic->end(); ++l_Iter)
            m_pcAnimation->AddCanvas((*l_Iter)->GetCanvas());
    }
    m_pcNode->Update(TRUE);

    return TRUE;
}

bool
SP_DS_CPN_PlaceAnimator::AddCandidate(SP_DS_Edge* p_pcEdge, SP_DS_CPN_TransAnimator* p_pcAnimator)
{
    CHECK_POINTER(p_pcEdge, return FALSE);
    CHECK_POINTER(p_pcAnimator, return FALSE);

    m_mlCandidates[p_pcAnimator].push_back(p_pcEdge);
    return TRUE;
}

bool
SP_DS_CPN_PlaceAnimator::RemoveCandidate(SP_DS_CPN_TransAnimator* p_pcAnimator)
{
    if (m_mlCandidates.find(p_pcAnimator) != m_mlCandidates.end())
        m_mlCandidates.erase(p_pcAnimator);

    return TRUE;
}

bool
SP_DS_CPN_PlaceAnimator::ResetMarking()
{
	if( !m_pcAttribute && m_pcNode && m_pcColList)
		return false;
    
	m_pcColList->Clear();	

	map<wxString, wxString>::iterator itMap;
	for( itMap = m_mDefaultMarking.begin(); itMap != m_mDefaultMarking.end(); itMap++)
	{
		unsigned l_nRow = m_pcColList->AppendEmptyRow();

		m_pcColList->SetCell(l_nRow,0, itMap->first);
		m_pcColList->SetCell(l_nRow,1, itMap->second);		
	}
	
	m_pcAttribute->SetValue(m_nDefaultValue);
	m_pcNode->Update(TRUE);

	UpdateMultiSet();

    return TRUE;
}

bool
SP_DS_CPN_PlaceAnimator::SetMarking()
{
    if (m_pcAttribute)
        m_nDefaultValue = m_pcAttribute->GetValue();

    return TRUE;
}

void
SP_DS_CPN_PlaceAnimator::SetMarking( long p_nMarking )
{

  if( m_pcAttribute && m_pcNode ) {

    m_pcAttribute->SetValue( p_nMarking );
    m_pcNode->Update( TRUE );

  }

}

void
SP_DS_CPN_PlaceAnimator::SetDefaultMarking( long p_nMarking )
{

  m_nDefaultValue = p_nMarking;

}

void
SP_DS_CPN_PlaceAnimator :: InitCanvas()
{

  SP_ListGraphic::iterator l_Iter;
  SP_ListGraphic* l_pcGraphic = m_pcNode->GetGraphics();

  if ( l_pcGraphic ) {

    for ( l_Iter = l_pcGraphic->begin(); l_Iter != l_pcGraphic->end(); ++l_Iter ) {

      m_pcAnimation->AddCanvas( ( *l_Iter )->GetCanvas() );

    }

  }

}

long
SP_DS_CPN_PlaceAnimator :: GetMarking()
{

  if( m_pcAttribute ) {

    return m_pcAttribute->GetValue();

  }

  return 0;

}

void SP_DS_CPN_PlaceAnimator::UpdateMultiSet()
{	
	wxString l_sColor;
	long l_nAppearance;
	m_cPlaceMultiSet.clear();

	SP_CPN_SyntaxChecking l_cSyntaxChecking;
	if( !l_cSyntaxChecking.Initialize())
			return;


	////////////////////////////////////
	map<wxString, vector<SP_CPN_TokenNum> > l_mColorToMarkingMap;
	
	if( ! l_cSyntaxChecking.ComputeInitialMarking(m_pcNode,l_mColorToMarkingMap,false) )
		return;

	map<wxString, vector<SP_CPN_TokenNum> >::iterator itMap;						
	for(itMap = l_mColorToMarkingMap.begin(); itMap != l_mColorToMarkingMap.end(); itMap++)
	{		
		l_sColor = itMap->first;
		l_nAppearance = itMap->second[0].m_intMultiplicity;
		m_cPlaceMultiSet.AddMultiSet(l_sColor,l_nAppearance);
	}
	////////////////////////////////////

}


void SP_DS_CPN_PlaceAnimator::ResetEdge(SP_DS_Edge* p_pcEdge)
{	
	if (!p_pcEdge)
        return;
	
	SP_DS_ColListAttribute* l_pcColList	= dynamic_cast< SP_DS_ColListAttribute* >(p_pcEdge->GetAttribute(SP_DS_CPN_INSCRIPTION) );

	if (!l_pcColList)
        return;

	if(l_pcColList->GetRowCount() > 1)
	{	
		//Delete the marking mark in the arc

		wxString l_sColor = l_pcColList->GetCell(0, 0);		
		wxString l_sAppearance = l_pcColList->GetCell(0, 1);	
		l_pcColList->Clear();
		unsigned int l_nNewRow = l_pcColList->AppendEmptyRow();
		l_pcColList->SetCell(l_nNewRow, 0, l_sColor);
		l_pcColList->SetCell(l_nNewRow, 1, l_sAppearance);
	}
	
}


bool
SP_DS_CPN_PlaceAnimator::UpdateMarking()
{
    CHECK_POINTER(m_pcAttribute, return FALSE);
    CHECK_POINTER(m_pcNode, return FALSE);

	//
	SP_MapString2Int::iterator itMap;
	SP_MapString2Int* l_pMultiSet = m_cPlaceMultiSet.GetMultiSetMap();
	int l_nMultiSetNum = 0;
	m_pcColList->Clear();

	for(itMap = l_pMultiSet->begin(); itMap != l_pMultiSet->end(); itMap++)
	{		
		l_nMultiSetNum += itMap->second;
		wxString l_sAppearance = wxString::Format(wxT("%d"),itMap->second);
		unsigned int l_nNewRow = m_pcColList->AppendEmptyRow();
		m_pcColList->SetCell(l_nNewRow, 0, itMap->first);
		m_pcColList->SetCell(l_nNewRow, 1, l_sAppearance);		
	}	
	
	/////////////////////////////////////////////////  
	m_pcAttribute->SetValue(l_nMultiSetNum);
   
    m_pcNode->Update(TRUE);

    return TRUE;
}



void SP_DS_CPN_PlaceAnimator::UpdateDefaultMarking()
{
	if(!m_pcColList)
		return;


	m_mDefaultMarking.clear();

	for (unsigned int i = 0; i < m_pcColList->GetRowCount(); i++)
	{
		wxString l_sColor = m_pcColList->GetCell(i, 0);

		m_mDefaultMarking[l_sColor] = m_pcColList->GetCell(i, 1);
	}

}
