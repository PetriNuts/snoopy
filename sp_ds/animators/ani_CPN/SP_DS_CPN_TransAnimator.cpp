//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2009/10/28 10:20:00 $
// Short Description: colored transition animator class
//////////////////////////////////////////////////////////////////////
#include "sp_ds/extensions/SP_DS_Animation.h"
#include "sp_ds/animators/ani_CPN/SP_DS_CPN_TransAnimator.h"
#include "sp_ds/animators/ani_CPN/SP_DS_CPN_PlaceAnimator.h"
#include "sp_ds/animators/ani_CPN/SP_DS_CPN_Animation.h"
#include "sp_gui/dialogs/dia_CPN/SP_DLG_BindingSelection.h"

SP_DS_CPN_TransAnimator::SP_DS_CPN_TransAnimator(SP_DS_Animation* p_pcAnim, SP_DS_Node* p_pcParent):
SP_DS_Animator(p_pcParent, p_pcAnim),
m_pcColList(NULL),
m_pcProtoshape(NULL)
{
    SetAnimatorType(SP_DS_ANIMATOR_TRANS);

	//m_pcAnimation = p_pcAnim;

	m_bSingleClick = false;
	m_nBindingChoice = 2;

    if (p_pcParent)
    {
        m_pcNode = p_pcParent;
        m_pcProtoshape = new SP_GR_MarkAnimator();
    }
}

SP_DS_CPN_TransAnimator::~SP_DS_CPN_TransAnimator()
{
    if (m_pcProtoshape)
        wxDELETE(m_pcProtoshape);

    m_pcProtoshape = NULL;
}

SP_DS_Animator*
SP_DS_CPN_TransAnimator::Clone(SP_Data* p_pcParent, SP_DS_Animation* p_pcAnim)
{
    if (p_pcParent->GetElementType() != SP_ELEMENT_NODE)
        return NULL;

    SP_DS_CPN_TransAnimator* l_pcNew = new SP_DS_CPN_TransAnimator(p_pcAnim, dynamic_cast<SP_DS_Node*>(p_pcParent));

    return dynamic_cast<SP_DS_Animator*>(l_pcNew);
}

bool
SP_DS_CPN_TransAnimator::InformPrePlaces()
{
    Reset();
    if (!m_pcNode || !m_pcAnimation)
        return FALSE;

    // i inform all nodes at my incoming edges of wanting their markings
    SP_ListEdge::const_iterator l_itEdges;
    SP_DS_CPN_PlaceAnimator* l_pcAnim;
		if (m_pcAnimation->GetDirection() == FORWARD)
		{		

			//////////////////////
			// Colored Petri nets
			//push back the preplace and edges
			m_ExprInfoList.clear();

			//push back the preplace and edges
			for (l_itEdges = m_pcNode->GetTargetEdges()->begin(); l_itEdges != m_pcNode->GetTargetEdges()->end(); ++l_itEdges)
			{
					if ((*l_itEdges)->GetSource())
					{
						l_pcAnim = dynamic_cast<SP_DS_CPN_PlaceAnimator*>(m_pcAnimation->GetAnimator((*l_itEdges)->GetSource(), SP_DS_ANIMATOR_PLACE));
						if (l_pcAnim)
						{
							m_mlPrePlaces[l_pcAnim].push_back((*l_itEdges));

							m_cExprInfo.m_eExprType = CPN_INPUT_EXPR;
							m_cExprInfo.m_pcPlAnimator = dynamic_cast<SP_DS_Animator*>(l_pcAnim);
							m_cExprInfo.m_pcEdge = *l_itEdges;							
							m_pcColList = dynamic_cast< SP_DS_ColListAttribute* >((*l_itEdges)->GetAttribute(SP_DS_CPN_INSCRIPTION) );
							if( ! m_pcColList)
								return false;
							if(m_pcColList->GetCell(0, 1) == wxT(""))
							{
								wxString l_sError;					
								l_sError = wxT("Arc exprssion should not be empty");
								SP_MESSAGEBOX(l_sError, wxT("Expression checking"), wxOK | wxICON_ERROR);
								return false;
							}
							
							m_cExprInfo.m_sExpression = m_pcColList->GetCell(0, 1);								
							m_ExprInfoList.push_back(m_cExprInfo);
						}
					}
			}

			//push back the postplace and edges
			for (l_itEdges = m_pcNode->GetSourceEdges()->begin(); l_itEdges != m_pcNode->GetSourceEdges()->end(); ++l_itEdges)
			{
					if ((*l_itEdges)->GetTarget())
					{
						l_pcAnim = dynamic_cast<SP_DS_CPN_PlaceAnimator*>(m_pcAnimation->GetAnimator((*l_itEdges)->GetTarget(), SP_DS_ANIMATOR_PLACE));
						if (l_pcAnim)
						{
							m_cExprInfo.m_eExprType = CPN_OUTPUT_EXPR;
							m_cExprInfo.m_pcPlAnimator = dynamic_cast<SP_DS_Animator*>(l_pcAnim);
							m_cExprInfo.m_pcEdge = *l_itEdges;							
							m_pcColList = dynamic_cast< SP_DS_ColListAttribute* >((*l_itEdges)->GetAttribute(SP_DS_CPN_INSCRIPTION) );
							if( ! m_pcColList)
								return false;
							if(m_pcColList->GetCell(0, 1) == wxT(""))
								return false;
							m_cExprInfo.m_sExpression = m_pcColList->GetCell(0, 1);	

							m_ExprInfoList.push_back(m_cExprInfo);
						}
					}
			}

			//push back the guard
			m_cExprInfo.m_eExprType = CPN_GUARD_EXPR;				
			m_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(m_pcNode->GetAttribute(SP_DS_CPN_GUARDLIST) );
			if(m_pcColList)
				m_cExprInfo.m_sExpression = m_pcColList->GetCell(0, 1);	

			if(m_cExprInfo.m_sExpression != wxT(""))
				m_ExprInfoList.push_back(m_cExprInfo);

			//begin to binding and enable test
			SP_CPN_Binding l_cBinding;		
			SP_DS_Animator* l_pcAnimator = dynamic_cast<SP_DS_CPN_TransAnimator*>(this);

			map<wxString, map<SP_DS_Edge*, map<wxString, int> > > l_mmmBind2Edge2Mult2Color;
			bool l_bEnableTest = l_cBinding.EnableTest(&m_ExprInfoList,false,l_pcAnimator,m_nBindingChoice,l_mmmBind2Edge2Mult2Color);
			if(!l_bEnableTest)
				return false;

			m_mmmBind2Edge2Mult2Color = l_mmmBind2Edge2Mult2Color;

			vector<wxString> l_EnabledBindings;
			map<wxString, map<SP_DS_Edge*, map<wxString, int> > >::iterator itMap3;
			for(itMap3 = m_mmmBind2Edge2Mult2Color.begin(); itMap3 != m_mmmBind2Edge2Mult2Color.end(); itMap3++)
			{
				l_EnabledBindings.push_back(itMap3->first);
			}
			
			
			if(m_bSingleClick)
			{
				m_bSingleClick = false;				

				int l_nChose = 0;				
				if( m_nBindingChoice == 2 || (m_nBindingChoice == 1 && l_EnabledBindings.size()>1) )
				{
					SP_DLG_BindingSelection* l_pcDlg = new SP_DLG_BindingSelection(l_EnabledBindings, NULL);

					if (l_pcDlg->ShowModal() == wxID_OK)
					{
						l_nChose = (int)(l_pcDlg->GetReturnSelection());
					}
					else
					{
						l_nChose = 0;
					}
					l_pcDlg->Destroy();
				}
				else if( m_nBindingChoice == 1 && l_EnabledBindings.size()==1 )
				{
					l_nChose = 0;
				}
				else
				{
					//Randomly select one binding to enable
					l_nChose = SP_RandomLong(l_EnabledBindings.size());
				}

				itMap3 = m_mmmBind2Edge2Mult2Color.begin();
				for (int i = 0; i < l_nChose; i++)
					itMap3++;
				wxString l_sSelBinding = itMap3->first;
				map<SP_DS_Edge*, map<wxString, int> >  l_mmEdge2Color2Mult = itMap3->second;
				m_mmmBind2Edge2Mult2Color.clear();
				m_mmmBind2Edge2Mult2Color[l_sSelBinding] = l_mmEdge2Color2Mult;
				m_vsSelBindings.clear();
				m_vsSelBindings.push_back(l_sSelBinding);

			}
			
			

			
/*			//begin to write the colors to each corresponding edge
			
			for(itExprList = m_ExprInfoList.begin(); itExprList != m_ExprInfoList.end(); itExprList++)
			{
				if( itExprList->m_eExprType == CPN_INPUT_EXPR || itExprList->m_eExprType == CPN_OUTPUT_EXPR)
				{
					//first clear the m_pcColList only one line is left.
					m_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(itExprList->m_pcEdge->GetAttribute( SP_DS_CPN_INSCRIPTION));
					if(m_pcColList)
					{
						if(m_pcColList->GetRowCount() > 1 )
						{
							wxString l_sColor = m_pcColList->GetCell(0,0);
							wxString l_sAppearance = m_pcColList->GetCell(0,1);
							m_pcColList->Clear();
							unsigned int l_nNewRow = m_pcColList->AppendEmptyRow();
							m_pcColList->SetCell(l_nNewRow, 0, l_sColor);
							m_pcColList->SetCell(l_nNewRow, 1, l_sAppearance);					
						}
					}

					//Then write the colors to each corresponding edge					
					vector<SP_CPN_ParsedColors>::iterator itColorList;

					for (itColorList = itExprList->m_ParsedColorsList.begin();
						itColorList != itExprList->m_ParsedColorsList.end();
						itColorList++)
					{
						unsigned int l_nNewRow = m_pcColList->AppendEmptyRow();
						wxString l_sColor = itColorList->m_sColorValue;
						wxString l_sAppearance = wxString::Format(wxT("%d"),itColorList->m_nMultiplicity);
						m_pcColList->SetCell(l_nNewRow, 0, l_sColor);
						m_pcColList->SetCell(l_nNewRow, 1, l_sAppearance);			
					}
					
				}
			}
*/
			//call AddCandidate			
			vector<SP_CPN_ExpressionInfo>::iterator itExprList;
			for(itExprList = m_ExprInfoList.begin(); itExprList != m_ExprInfoList.end(); itExprList++)
			{
				if( itExprList->m_eExprType == CPN_INPUT_EXPR )
				{
					dynamic_cast<SP_DS_CPN_PlaceAnimator*>(itExprList->m_pcPlAnimator)->AddCandidate(itExprList->m_pcEdge, this);
				}
			}

			m_bEnabled = TRUE;   // Let it enable
			return TRUE;     		


		} 
		

		// backwards animation
		else {
			if (m_pcNode->GetSourceEdges()->size() == 0) {
					m_bEnabled = TRUE;
					return TRUE;
			}

			//////////////////////
			// Colored Petri nets
			//push back the preplace and edges
			m_ExprInfoList.clear();
			for (l_itEdges = m_pcNode->GetSourceEdges()->begin(); l_itEdges != m_pcNode->GetSourceEdges()->end(); ++l_itEdges)
			{
					if ((*l_itEdges)->GetTarget())
					{
							l_pcAnim = dynamic_cast<SP_DS_CPN_PlaceAnimator*>(m_pcAnimation->GetAnimator((*l_itEdges)->GetTarget(), SP_DS_ANIMATOR_PLACE));
							if (l_pcAnim)
							{
									//l_pcAnim->AddCandidate((*l_itEdges), this);
									//m_mlPrePlaces[l_pcAnim].push_back((*l_itEdges));
								m_mlPrePlaces[l_pcAnim].push_back((*l_itEdges));
								m_cExprInfo.m_eExprType = CPN_INPUT_EXPR;
								m_cExprInfo.m_pcPlAnimator = dynamic_cast<SP_DS_Animator*>(l_pcAnim);
								m_cExprInfo.m_pcEdge = *l_itEdges;							
								m_pcColList = dynamic_cast< SP_DS_ColListAttribute* >((*l_itEdges)->GetAttribute(SP_DS_CPN_INSCRIPTION) );
								if( ! m_pcColList)
									return false;
								if(m_pcColList->GetCell(0, 1) == wxT(""))
								{
									wxString l_sError;					
									l_sError = wxT("Arc exprssion should not be empty");
									SP_MESSAGEBOX(l_sError, wxT("Expression checking"), wxOK | wxICON_ERROR);
									return false;
								}
							
								m_cExprInfo.m_sExpression = m_pcColList->GetCell(0, 1);								
								m_ExprInfoList.push_back(m_cExprInfo);
							}
					}
			}

			//push back the postplace and edges
			for (l_itEdges = m_pcNode->GetTargetEdges()->begin(); l_itEdges != m_pcNode->GetTargetEdges()->end(); ++l_itEdges)
			{
					if ((*l_itEdges)->GetSource())
					{
						l_pcAnim = dynamic_cast<SP_DS_CPN_PlaceAnimator*>(m_pcAnimation->GetAnimator((*l_itEdges)->GetSource(), SP_DS_ANIMATOR_PLACE));
						if (l_pcAnim)
						{
							m_cExprInfo.m_eExprType = CPN_OUTPUT_EXPR;
							m_cExprInfo.m_pcPlAnimator = dynamic_cast<SP_DS_Animator*>(l_pcAnim);
							m_cExprInfo.m_pcEdge = *l_itEdges;							
							m_pcColList = dynamic_cast< SP_DS_ColListAttribute* >((*l_itEdges)->GetAttribute(SP_DS_CPN_INSCRIPTION) );
							if( ! m_pcColList)
								return false;
							if(m_pcColList->GetCell(0, 1) == wxT(""))
								return false;
							m_cExprInfo.m_sExpression = m_pcColList->GetCell(0, 1);	

							m_ExprInfoList.push_back(m_cExprInfo);
						}
					}
			}

			//push back the guard
			m_cExprInfo.m_eExprType = CPN_GUARD_EXPR;				
			m_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(m_pcNode->GetAttribute(SP_DS_CPN_GUARDLIST) );
			if(m_pcColList)
				m_cExprInfo.m_sExpression = m_pcColList->GetCell(0, 1);	

			if(m_cExprInfo.m_sExpression != wxT(""))
				m_ExprInfoList.push_back(m_cExprInfo);

			//begin to binding and enable test
			SP_CPN_Binding l_cBinding;		
			SP_DS_Animator* l_pcAnimator = dynamic_cast<SP_DS_CPN_TransAnimator*>(this);
			map<wxString, map<SP_DS_Edge*, map<wxString, int> > > l_mmmBind2Edge2Mult2Color;
			bool l_bEnableTest = l_cBinding.EnableTest(&m_ExprInfoList,false,l_pcAnimator,m_nBindingChoice,l_mmmBind2Edge2Mult2Color);
			if(!l_bEnableTest)
				return false;

			m_mmmBind2Edge2Mult2Color = l_mmmBind2Edge2Mult2Color;

			vector<wxString> l_EnabledBindings;
			map<wxString, map<SP_DS_Edge*, map<wxString, int> > >::iterator itMap3;
			for(itMap3 = m_mmmBind2Edge2Mult2Color.begin(); itMap3 != m_mmmBind2Edge2Mult2Color.end(); itMap3++)
			{
				l_EnabledBindings.push_back(itMap3->first);
			}
			
			
			if(m_bSingleClick)
			{
				m_bSingleClick = false;				

				int l_nChose = 0;				
				if( m_nBindingChoice == 2 || (m_nBindingChoice == 1 && l_EnabledBindings.size()>1) )
				{
					SP_DLG_BindingSelection* l_pcDlg = new SP_DLG_BindingSelection(l_EnabledBindings, NULL);

					if (l_pcDlg->ShowModal() == wxID_OK)
					{
						l_nChose = (int)(l_pcDlg->GetReturnSelection());
					}
					else
					{
						l_nChose = 0;
					}
					l_pcDlg->Destroy();
				}
				else if( m_nBindingChoice == 1 && l_EnabledBindings.size()==1 )
				{
					l_nChose = 0;
				}
				else
				{
					//Randomly select one binding to enable
					l_nChose = SP_RandomLong(l_EnabledBindings.size());
				}

				itMap3 = m_mmmBind2Edge2Mult2Color.begin();
				for (int i = 0; i < l_nChose; i++)
					itMap3++;
				wxString l_sSelBinding = itMap3->first;
				map<SP_DS_Edge*, map<wxString, int> >  l_mmEdge2Color2Mult = itMap3->second;
				m_mmmBind2Edge2Mult2Color.clear();
				m_mmmBind2Edge2Mult2Color[l_sSelBinding] = l_mmEdge2Color2Mult;
				m_vsSelBindings.clear();
				m_vsSelBindings.push_back(l_sSelBinding);

			}

/*
			//begin to write the colors to each corresponding edge
			vector<SP_CPN_ExpressionInfo>::iterator itExprList;
			for(itExprList = m_ExprInfoList.begin(); itExprList != m_ExprInfoList.end(); itExprList++)
			{
				if( itExprList->m_eExprType == CPN_INPUT_EXPR || itExprList->m_eExprType == CPN_OUTPUT_EXPR)
				{
					//first clear the m_pcColList only one line is left.
					m_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(itExprList->m_pcEdge->GetAttribute( SP_DS_CPN_INSCRIPTION));
					if(m_pcColList)
					{
						if(m_pcColList->GetRowCount() > 1 )
						{
							wxString l_sColor = m_pcColList->GetCell(0,0);
							wxString l_sAppearance = m_pcColList->GetCell(0,1);
							m_pcColList->Clear();
							unsigned int l_nNewRow = m_pcColList->AppendEmptyRow();
							m_pcColList->SetCell(l_nNewRow, 0, l_sColor);
							m_pcColList->SetCell(l_nNewRow, 1, l_sAppearance);					
						}
					}

					//Then write the colors to each corresponding edge					
					vector<SP_CPN_ParsedColors>::iterator itColorList;

					for (itColorList = itExprList->m_ParsedColorsList.begin();
						itColorList != itExprList->m_ParsedColorsList.end();
						itColorList++)
					{
						unsigned int l_nNewRow = m_pcColList->AppendEmptyRow();
						wxString l_sColor = itColorList->m_sColorValue;
						wxString l_sAppearance = wxString::Format(wxT("%d"),itColorList->m_nMultiplicity);
						m_pcColList->SetCell(l_nNewRow, 0, l_sColor);
						m_pcColList->SetCell(l_nNewRow, 1, l_sAppearance);			
					}
					
				}
			}
*/
			//call AddCandidate
			//list<SP_CPN_ExpressionInfo>::iterator itExprList;
			vector<SP_CPN_ExpressionInfo>::iterator itExprList;
			for(itExprList = m_ExprInfoList.begin(); itExprList != m_ExprInfoList.end(); itExprList++)
			{
				if( itExprList->m_eExprType == CPN_INPUT_EXPR )
				{
					dynamic_cast<SP_DS_CPN_PlaceAnimator*>(itExprList->m_pcPlAnimator)->AddCandidate(itExprList->m_pcEdge, this);
				}
			}

			m_bEnabled = TRUE;   // Let it enable
			return TRUE;     
				




			//////////////////////////////////
		}
    return TRUE;
}

bool
SP_DS_CPN_TransAnimator::TestConcession()
{
    if (m_mlPrePlaces.size() <= m_lCandidates.size())
        return TRUE;
    // no concession
    map<SP_DS_CPN_PlaceAnimator*, SP_ListEdge >::iterator l_Iter;
    for (l_Iter = m_mlPrePlaces.begin(); l_Iter != m_mlPrePlaces.end(); ++l_Iter)
        l_Iter->first->RemoveCandidate(this);

    return FALSE;
}

bool
SP_DS_CPN_TransAnimator::Reset()
{
    map<SP_DS_CPN_PlaceAnimator*, SP_ListEdge >::iterator l_Iter;
    SP_ListGraphic* l_plGraphics = GetGraphics();
    SP_ListGraphic::iterator l_Iter2;
    SP_GR_MarkAnimator* l_pcGr;

    for (l_Iter = m_mlPrePlaces.begin(); l_Iter != m_mlPrePlaces.end(); ++l_Iter)
        l_Iter->first->RemoveCandidate(this);

    m_lCandidates.clear();
    m_bEnabled = FALSE;
    m_mlPrePlaces.clear();

    if (!l_plGraphics)
        return TRUE;

    for (l_Iter2 = l_plGraphics->begin(); l_Iter2 != l_plGraphics->end(); l_Iter2 = l_plGraphics->begin())
    {
    	SP_Graphic* l_pcGrIt = *l_Iter2;
        l_pcGr = dynamic_cast<SP_GR_MarkAnimator*>(l_pcGrIt);
        l_plGraphics->erase(l_Iter2);
        wxDELETE(l_pcGr);
    }
    return TRUE;
}

// animate
bool
SP_DS_CPN_TransAnimator::PreStep(unsigned int p_nSteps, SP_DIRECTION_T p_nDir)
{
    if (!m_pcNode || !m_pcAnimation)
        return FALSE;

    if (!m_bEnabled)
        return FALSE;

    list<SP_DS_CPN_PlaceAnimator*>::iterator l_Iter;
    for (l_Iter = m_lCandidates.begin(); l_Iter != m_lCandidates.end(); ++l_Iter)
        (*l_Iter)->SetEnabled(TRUE);

    m_nSteps = p_nSteps;
    SP_GR_MarkAnimator* l_pcAnim;
		bool l_bReturn = false;

		if (p_nDir == FORWARD) {
			for (SP_DS_Edge* l_pcEdge : *m_pcNode->GetSourceEdges())
			{
				if (l_pcEdge->GetGraphics())
					for (SP_Graphic* l_pcGraphic : *l_pcEdge->GetGraphics())
					{
						l_pcAnim = dynamic_cast<SP_GR_MarkAnimator*>(m_pcProtoshape->Clone(this));
						if (l_pcAnim->SetPath(l_pcGraphic, m_nSteps, p_nDir))
						{
							AddGraphic(l_pcAnim);
							m_pcAnimation->AddCanvas(l_pcAnim->GetCanvas());
						}
						else
							wxDELETE(l_pcAnim);
					}
			}

			l_bReturn = m_pcNode->GetSourceEdges()->size() == 0;

		} else {
			for (SP_DS_Edge* l_pcEdge : *m_pcNode->GetTargetEdges())
			{
				if (l_pcEdge->GetGraphics())
					for (SP_Graphic* l_pcGraphic : *l_pcEdge->GetGraphics())
					{
						l_pcAnim = dynamic_cast<SP_GR_MarkAnimator*>(m_pcProtoshape->Clone(this));
						if (l_pcAnim->SetPath(l_pcGraphic, m_nSteps, p_nDir))
						{
							AddGraphic(l_pcAnim);
							m_pcAnimation->AddCanvas(l_pcAnim->GetCanvas());
						}
						else
							wxDELETE(l_pcAnim);
					}
			}

			l_bReturn = m_pcNode->GetTargetEdges()->size() == 0;

		}

    return (l_bReturn || (GetGraphics() && (GetGraphics()->size() > 0)));
}

bool
SP_DS_CPN_TransAnimator::Step(unsigned int p_nStep)
{
    if (!GetGraphics())
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
SP_DS_CPN_TransAnimator::PostStep()
{
    SP_ListEdge::const_iterator l_itEdges;
    SP_DS_CPN_PlaceAnimator* l_pcAnim;

	Reset();
	if (m_pcAnimation->GetDirection() == FORWARD)
	{
		for (l_itEdges = m_pcNode->GetSourceEdges()->begin(); l_itEdges != m_pcNode->GetSourceEdges()->end(); ++l_itEdges)
		{
			if ((*l_itEdges)->GetTarget())
			{
				l_pcAnim = dynamic_cast<SP_DS_CPN_PlaceAnimator*>(m_pcAnimation->GetAnimator((*l_itEdges)->GetTarget(), SP_DS_ANIMATOR_PLACE));
				if (l_pcAnim)
					l_pcAnim->IncrementMark(this, (*l_itEdges));
			}
		}
		//Reset source edges
		for (l_itEdges = m_pcNode->GetTargetEdges()->begin(); l_itEdges != m_pcNode->GetTargetEdges()->end(); ++l_itEdges)
		{
			if ((*l_itEdges)->GetSource())
			{
				l_pcAnim = dynamic_cast<SP_DS_CPN_PlaceAnimator*>(m_pcAnimation->GetAnimator((*l_itEdges)->GetSource(), SP_DS_ANIMATOR_PLACE));
				if (l_pcAnim)
					l_pcAnim->ResetEdge(*l_itEdges);
			}
		}

	}
	else
	{
		for (l_itEdges = m_pcNode->GetTargetEdges()->begin(); l_itEdges != m_pcNode->GetTargetEdges()->end(); ++l_itEdges)
		{
			if ((*l_itEdges)->GetSource())
			{
				l_pcAnim = dynamic_cast<SP_DS_CPN_PlaceAnimator*>(m_pcAnimation->GetAnimator((*l_itEdges)->GetSource(), SP_DS_ANIMATOR_PLACE));
				if (l_pcAnim)
					l_pcAnim->IncrementMark(this, (*l_itEdges));
			}
		}

		//Reset source edges
		for (l_itEdges = m_pcNode->GetSourceEdges()->begin(); l_itEdges != m_pcNode->GetSourceEdges()->end(); ++l_itEdges)
		{
			if ((*l_itEdges)->GetTarget())
			{
				l_pcAnim = dynamic_cast<SP_DS_CPN_PlaceAnimator*>(m_pcAnimation->GetAnimator((*l_itEdges)->GetTarget(), SP_DS_ANIMATOR_PLACE));
				if (l_pcAnim)
					l_pcAnim->ResetEdge(*l_itEdges);
			}
		}

	}

    return RemoveAllGraphics();
}

bool
SP_DS_CPN_TransAnimator::OnLeftClick()
{
    CHECK_POINTER(m_pcNode, return FALSE);
    CHECK_POINTER(m_pcAnimation, return FALSE);
    SP_DS_CPN_Animation* l_pcAnim = dynamic_cast<SP_DS_CPN_Animation*>(m_pcAnimation);

	m_bSingleClick = true;
	m_nBindingChoice = l_pcAnim->GetBindingDialogueShowFlag();

		l_pcAnim->SetDirection(FORWARD);
    l_pcAnim->SetSingleStep(this);
    l_pcAnim->SetOneShot(TRUE);
    l_pcAnim->StartTimer();

    return TRUE;
}

bool
SP_DS_CPN_TransAnimator::OnRightClick()
{
    CHECK_POINTER(m_pcNode, return FALSE);
    CHECK_POINTER(m_pcAnimation, return FALSE);
    SP_DS_CPN_Animation* l_pcAnim = dynamic_cast<SP_DS_CPN_Animation*>(m_pcAnimation);

	m_bSingleClick = true;
	m_nBindingChoice = l_pcAnim->GetBindingDialogueShowFlag();

	l_pcAnim->SetDirection(BACKWARD);
    l_pcAnim->SetSingleStep(this);
    l_pcAnim->SetOneShot(TRUE);
    l_pcAnim->StartTimer();

    return TRUE;
}

void
SP_DS_CPN_TransAnimator::Enable(SP_DS_CPN_PlaceAnimator* p_pcAnimator)
{
    CHECK_POINTER(p_pcAnimator, return);
    m_lCandidates.push_back(p_pcAnimator);
}

