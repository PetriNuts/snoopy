//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2009/10/28 10:20:00 $
// Short Description: colored transition animator class
//////////////////////////////////////////////////////////////////////
#include "sp_ds/extensions/SP_DS_Animation.h"
#include "sp_ds/animators/ani_ColSPN/SP_DS_ColStTransAnimator.h"
#include "sp_ds/animators/ani_ColSPN/SP_DS_ColStPlaceAnimator.h"
#include "sp_ds/animators/ani_ColSPN/SP_DS_ColStAnimation.h"

#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_Binding.h"
#include "sp_gui/dialogs/dia_CPN/SP_DLG_BindingSelection.h"

SP_DS_ColStTransAnimator::SP_DS_ColStTransAnimator(SP_DS_Animation* p_pcAnim, SP_DS_Node* p_pcParent):
SP_DS_Animator(p_pcParent, p_pcAnim),
m_pcColList(NULL),
m_pcProtoshape(NULL)
{
    SetAnimatorType(SP_DS_ANIMATOR_TRANS);

	//m_pcAnimation = p_pcAnim;

	m_bSingleClick = false;

    if (p_pcParent)
    {
        m_pcNode = p_pcParent;
        m_pcProtoshape = new SP_GR_MarkAnimator();
    }
}

SP_DS_ColStTransAnimator::~SP_DS_ColStTransAnimator()
{
    if (m_pcProtoshape)
        wxDELETE(m_pcProtoshape);

    m_pcProtoshape = NULL;
}

SP_DS_Animator*
SP_DS_ColStTransAnimator::Clone(SP_Data* p_pcParent, SP_DS_Animation* p_pcAnim)
{
    if (p_pcParent->GetElementType() != SP_ELEMENT_NODE)
        return NULL;

    SP_DS_ColStTransAnimator* l_pcNew = new SP_DS_ColStTransAnimator(p_pcAnim, dynamic_cast<SP_DS_Node*>(p_pcParent));

    return dynamic_cast<SP_DS_Animator*>(l_pcNew);
}

bool
SP_DS_ColStTransAnimator::InformPrePlaces()
{
   
    return TRUE;
}

bool
SP_DS_ColStTransAnimator::TestConcession()
{
    if (m_mlPrePlaces.size() <= m_lCandidates.size())
        return TRUE;
    // no concession
    map<SP_DS_ColStPlaceAnimator*, SP_ListEdge >::iterator l_Iter;
    for (l_Iter = m_mlPrePlaces.begin(); l_Iter != m_mlPrePlaces.end(); ++l_Iter)
        l_Iter->first->RemoveCandidate(this);

    return FALSE;
}

bool
SP_DS_ColStTransAnimator::Reset()
{
    map<SP_DS_ColStPlaceAnimator*, SP_ListEdge >::iterator l_Iter;
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
SP_DS_ColStTransAnimator::PreStep(unsigned int p_nSteps, SP_DIRECTION_T p_nDir)
{
    if (!m_pcNode || !m_pcAnimation)
        return FALSE;

    if (!m_bEnabled)
        return FALSE;

    list<SP_DS_ColStPlaceAnimator*>::iterator l_Iter;
    for (l_Iter = m_lCandidates.begin(); l_Iter != m_lCandidates.end(); ++l_Iter)
        (*l_Iter)->SetEnabled(TRUE);

    m_nSteps = p_nSteps;
    SP_ListEdge::const_iterator l_itEdges;
    SP_ListGraphic::iterator l_itGraphic;
    SP_GR_MarkAnimator* l_pcAnim;
		bool l_bReturn = false;

		if (p_nDir == FORWARD) {
			for (l_itEdges = m_pcNode->GetSourceEdges()->begin(); l_itEdges != m_pcNode->GetSourceEdges()->end(); ++l_itEdges)
			{
				if ((*l_itEdges)->GetGraphics())
					for (SP_Graphic* l_pcGraphic : *(*l_itEdges)->GetGraphics()){
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
			for (l_itEdges = m_pcNode->GetTargetEdges()->begin(); l_itEdges != m_pcNode->GetTargetEdges()->end(); ++l_itEdges)
			{
				if ((*l_itEdges)->GetGraphics())
					for (SP_Graphic* l_pcGraphic : *(*l_itEdges)->GetGraphics())
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
SP_DS_ColStTransAnimator::Step(unsigned int p_nStep)
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
SP_DS_ColStTransAnimator::PostStep()
{
    SP_ListEdge::const_iterator l_itEdges;
    SP_DS_ColStPlaceAnimator* l_pcAnim;

    Reset();
		if (m_pcAnimation->GetDirection() == FORWARD) {
			for (l_itEdges = m_pcNode->GetSourceEdges()->begin(); l_itEdges != m_pcNode->GetSourceEdges()->end(); ++l_itEdges)
			{
					if ((*l_itEdges)->GetTarget())
					{
							l_pcAnim = dynamic_cast<SP_DS_ColStPlaceAnimator*>(m_pcAnimation->GetAnimator((*l_itEdges)->GetTarget(), SP_DS_ANIMATOR_PLACE));
							if (l_pcAnim)
									l_pcAnim->IncrementMark((*l_itEdges));
					}
			}
			//Reset source edges
			for (l_itEdges = m_pcNode->GetTargetEdges()->begin(); l_itEdges != m_pcNode->GetTargetEdges()->end(); ++l_itEdges)
			{
					if ((*l_itEdges)->GetSource())
					{
							l_pcAnim = dynamic_cast<SP_DS_ColStPlaceAnimator*>(m_pcAnimation->GetAnimator((*l_itEdges)->GetSource(), SP_DS_ANIMATOR_PLACE));
							if (l_pcAnim)
								l_pcAnim->ResetEdge(*l_itEdges);
					}
			}

		}
		else {
			for (l_itEdges = m_pcNode->GetTargetEdges()->begin(); l_itEdges != m_pcNode->GetTargetEdges()->end(); ++l_itEdges)
			{
					if ((*l_itEdges)->GetSource())
					{
							l_pcAnim = dynamic_cast<SP_DS_ColStPlaceAnimator*>(m_pcAnimation->GetAnimator((*l_itEdges)->GetSource(), SP_DS_ANIMATOR_PLACE));
							if (l_pcAnim)
									l_pcAnim->IncrementMark((*l_itEdges));
					}
			}

			//Reset source edges
			for (l_itEdges = m_pcNode->GetSourceEdges()->begin(); l_itEdges != m_pcNode->GetSourceEdges()->end(); ++l_itEdges)
			{
					if ((*l_itEdges)->GetTarget())
					{
							l_pcAnim = dynamic_cast<SP_DS_ColStPlaceAnimator*>(m_pcAnimation->GetAnimator((*l_itEdges)->GetTarget(), SP_DS_ANIMATOR_PLACE));
							if (l_pcAnim)
								l_pcAnim->ResetEdge(*l_itEdges);
					}
			}

		}

    return RemoveAllGraphics();
}

bool
SP_DS_ColStTransAnimator::OnLeftClick()
{
    CHECK_POINTER(m_pcNode, return FALSE);
    CHECK_POINTER(m_pcAnimation, return FALSE);
    SP_DS_ColStAnimation* l_pcAnim = dynamic_cast<SP_DS_ColStAnimation*>(m_pcAnimation);

	m_bSingleClick = true;

	m_nBindingChoice = l_pcAnim->GetBindingDialogueShowFlag();

		l_pcAnim->SetDirection(FORWARD);
    l_pcAnim->SetSingleStep(this);
    l_pcAnim->SetOneShot(TRUE);
    l_pcAnim->StartTimer();

    return TRUE;
}

bool
SP_DS_ColStTransAnimator::OnRightClick()
{
    CHECK_POINTER(m_pcNode, return FALSE);
    CHECK_POINTER(m_pcAnimation, return FALSE);
    SP_DS_ColStAnimation* l_pcAnim = dynamic_cast<SP_DS_ColStAnimation*>(m_pcAnimation);

	m_bSingleClick = true;

		l_pcAnim->SetDirection(BACKWARD);
    l_pcAnim->SetSingleStep(this);
    l_pcAnim->SetOneShot(TRUE);
    l_pcAnim->StartTimer();

    return TRUE;
}

void
SP_DS_ColStTransAnimator::Enable(SP_DS_ColStPlaceAnimator* p_pcAnimator)
{
    CHECK_POINTER(p_pcAnimator, return);
    m_lCandidates.push_back(p_pcAnimator);
}

bool SP_DS_ColStTransAnimator::IsEnabled(SP_VectorString& v) {
	Reset();

	if (!m_pcNode || !m_pcAnimation)
		return FALSE;

	vector<SP_CPN_ExpressionInfo> m_StExprInfoVector;
	SP_CPN_ExpressionInfo m_cStExprInfo;

	// i inform all nodes at my incoming edges of wanting their markings
	SP_ListEdge::const_iterator l_itEdges;
	SP_DS_ColStPlaceAnimator* l_pcAnim;
	if (m_pcAnimation->GetDirection() == FORWARD)
	{

		//////////////////////
		// Colored Petri nets
		//push back the preplace and edges

		m_StExprInfoVector.clear();

		//push back the preplace and edges
		for (l_itEdges = m_pcNode->GetTargetEdges()->begin(); l_itEdges != m_pcNode->GetTargetEdges()->end(); ++l_itEdges)
		{
			if ((*l_itEdges)->GetSource())
			{
				l_pcAnim = dynamic_cast<SP_DS_ColStPlaceAnimator*>(m_pcAnimation->GetAnimator((*l_itEdges)->GetSource(), SP_DS_ANIMATOR_PLACE));
				if (l_pcAnim)
				{
					m_mlPrePlaces[l_pcAnim].push_back((*l_itEdges));

					m_cStExprInfo.m_eExprType = CPN_INPUT_EXPR;
					m_cStExprInfo.m_pcPlAnimator = dynamic_cast<SP_DS_Animator*>(l_pcAnim);
					m_cStExprInfo.m_pcEdge = *l_itEdges;
					m_pcColList = dynamic_cast<SP_DS_ColListAttribute*>((*l_itEdges)->GetAttribute(SP_DS_CPN_INSCRIPTION));
					if (!m_pcColList)
						return false;
					if (m_pcColList->GetCell(0, 1) == wxT(""))
					{
						wxString l_sError;
						l_sError = wxT("Arc exprssion should not be empty");
						SP_MESSAGEBOX(l_sError, wxT("Expression checking"), wxOK | wxICON_ERROR);
						return false;
					}

					m_cStExprInfo.m_sExpression = m_pcColList->GetCell(0, 1);
					m_StExprInfoVector.push_back(m_cStExprInfo);
				}
			}
		}

		//push back the postplace and edges
		for (l_itEdges = m_pcNode->GetSourceEdges()->begin(); l_itEdges != m_pcNode->GetSourceEdges()->end(); ++l_itEdges)
		{
			if ((*l_itEdges)->GetTarget())
			{
				l_pcAnim = dynamic_cast<SP_DS_ColStPlaceAnimator*>(m_pcAnimation->GetAnimator((*l_itEdges)->GetTarget(), SP_DS_ANIMATOR_PLACE));
				if (l_pcAnim)
				{
					m_cStExprInfo.m_eExprType = CPN_OUTPUT_EXPR;
					m_cStExprInfo.m_pcPlAnimator = dynamic_cast<SP_DS_Animator*>(l_pcAnim);
					m_cStExprInfo.m_pcEdge = *l_itEdges;
					m_pcColList = dynamic_cast<SP_DS_ColListAttribute*>((*l_itEdges)->GetAttribute(SP_DS_CPN_INSCRIPTION));
					if (!m_pcColList)
						return false;
					if (m_pcColList->GetCell(0, 1) == wxT(""))
						return false;
					m_cStExprInfo.m_sExpression = m_pcColList->GetCell(0, 1);

					m_StExprInfoVector.push_back(m_cStExprInfo);
				}
			}
		}

		//push back the guard
		m_cStExprInfo.m_eExprType = CPN_GUARD_EXPR;
		m_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(m_pcNode->GetAttribute(SP_DS_CPN_GUARDLIST));
		if (m_pcColList)
			m_cStExprInfo.m_sExpression = m_pcColList->GetCell(0, 1);

		if (m_cStExprInfo.m_sExpression != wxT(""))
			m_StExprInfoVector.push_back(m_cStExprInfo);

		//begin to binding and enable test
		SP_CPN_Binding l_cBinding;
		SP_DS_Animator* l_pcAnimator = dynamic_cast<SP_DS_ColStTransAnimator*>(this);
		map<wxString, map<SP_DS_Edge*, map<wxString, int> > > l_mmmBind2Edge2Mult2Color;
		bool l_bEnableTest = l_cBinding.EnableTest(&m_StExprInfoVector, m_bSingleClick, l_pcAnimator, m_nBindingChoice, l_mmmBind2Edge2Mult2Color);

		v = l_cBinding.GetBindingSelection();

		return  l_bEnableTest;
	}
	return false;
}


bool
SP_DS_ColStTransAnimator::InformStPrePlaces(const wxString& p_sColor, const std::vector<wxString>& values)
{
	  Reset();

	    if (!m_pcNode || !m_pcAnimation)
	        return FALSE;

		vector<SP_CPN_ExpressionInfo> m_StExprInfoVector;
		SP_CPN_ExpressionInfo m_cStExprInfo;

	    // i inform all nodes at my incoming edges of wanting their markings
	    SP_ListEdge::const_iterator l_itEdges;
	    SP_DS_ColStPlaceAnimator* l_pcAnim;
			if (m_pcAnimation->GetDirection() == FORWARD)
			{

				//////////////////////
				// Colored Petri nets
				//push back the preplace and edges
				
				m_StExprInfoVector.clear();

				//push back the preplace and edges
				for (l_itEdges = m_pcNode->GetTargetEdges()->begin(); l_itEdges != m_pcNode->GetTargetEdges()->end(); ++l_itEdges)
				{
						if ((*l_itEdges)->GetSource())
						{
							l_pcAnim = dynamic_cast<SP_DS_ColStPlaceAnimator*>(m_pcAnimation->GetAnimator((*l_itEdges)->GetSource(), SP_DS_ANIMATOR_PLACE));
							if (l_pcAnim)
							{
								m_mlPrePlaces[l_pcAnim].push_back((*l_itEdges));

								m_cStExprInfo.m_eExprType = CPN_INPUT_EXPR;
								m_cStExprInfo.m_pcPlAnimator = dynamic_cast<SP_DS_Animator*>(l_pcAnim);
								m_cStExprInfo.m_pcEdge = *l_itEdges;
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

								m_cStExprInfo.m_sExpression = m_pcColList->GetCell(0, 1);
								m_StExprInfoVector.push_back(m_cStExprInfo);
							}
						}
				}

				//push back the postplace and edges
				for (l_itEdges = m_pcNode->GetSourceEdges()->begin(); l_itEdges != m_pcNode->GetSourceEdges()->end(); ++l_itEdges)
				{
						if ((*l_itEdges)->GetTarget())
						{
							l_pcAnim = dynamic_cast<SP_DS_ColStPlaceAnimator*>(m_pcAnimation->GetAnimator((*l_itEdges)->GetTarget(), SP_DS_ANIMATOR_PLACE));
							if (l_pcAnim)
							{
								m_cStExprInfo.m_eExprType = CPN_OUTPUT_EXPR;
								m_cStExprInfo.m_pcPlAnimator = dynamic_cast<SP_DS_Animator*>(l_pcAnim);
								m_cStExprInfo.m_pcEdge = *l_itEdges;
								m_pcColList = dynamic_cast< SP_DS_ColListAttribute* >((*l_itEdges)->GetAttribute(SP_DS_CPN_INSCRIPTION) );
								if( ! m_pcColList)
									return false;
								if(m_pcColList->GetCell(0, 1) == wxT(""))
									return false;
								m_cStExprInfo.m_sExpression = m_pcColList->GetCell(0, 1);

								m_StExprInfoVector.push_back(m_cStExprInfo);
							}
						}
				}

				//push back the guard
				m_cStExprInfo.m_eExprType = CPN_GUARD_EXPR;
				m_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(m_pcNode->GetAttribute(SP_DS_CPN_GUARDLIST) );
				if(m_pcColList)
					m_cStExprInfo.m_sExpression = m_pcColList->GetCell(0, 1);

				if(m_cStExprInfo.m_sExpression != wxT(""))
					m_StExprInfoVector.push_back(m_cStExprInfo);

				//begin to binding and enable test
				SP_CPN_Binding l_cBinding;
				SP_DS_Animator* l_pcAnimator = dynamic_cast<SP_DS_ColStTransAnimator*>(this);
				map<wxString, map<SP_DS_Edge*, map<wxString, int> > > l_mmmBind2Edge2Mult2Color;
				bool l_bEnableTest = l_cBinding.EnableTest(&m_StExprInfoVector,m_bSingleClick,l_pcAnimator,m_nBindingChoice,l_mmmBind2Edge2Mult2Color,values);
				

				if(!l_bEnableTest)
					return false;

				//if(m_bSingleClick)//by george
				m_vChosenBinding = l_cBinding.GetBindingSelection();

				//begin to write the colors to each corresponding edge
				vector<SP_CPN_ExpressionInfo>::iterator itExprList;
				for(itExprList = m_StExprInfoVector.begin(); itExprList != m_StExprInfoVector.end(); itExprList++)
				{
					if( itExprList->m_eExprType == CPN_INPUT_EXPR || itExprList->m_eExprType == CPN_OUTPUT_EXPR)
					{
						//first clear the m_pcColList only one line is left.
						m_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(itExprList->m_pcEdge->GetAttribute( SP_DS_CPN_INSCRIPTION));
						if(m_pcColList)
						{
							if(m_pcColList->GetRowCount() > 1 )
							{

								wxString l_sColor;
								if (p_sColor != wxT(""))
								{
									l_sColor = p_sColor;
								}
								else
								{
									l_sColor = m_pcColList->GetCell(0, 0);
								}

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
							wxString l_sColor;
							if (p_sColor != wxT(""))
							{
								l_sColor = p_sColor;
							}
							else
							{
								l_sColor = itColorList->m_sColorValue;
							}

							wxString l_sAppearance = wxString::Format(wxT("%d"),itColorList->m_nMultiplicity);
							m_pcColList->SetCell(l_nNewRow, 0, l_sColor);
							m_pcColList->SetCell(l_nNewRow, 1, l_sAppearance);
						}
						
					}
				}

				//call AddCandidate
				for(itExprList = m_StExprInfoVector.begin(); itExprList != m_StExprInfoVector.end(); itExprList++)
				{
					if( itExprList->m_eExprType == CPN_INPUT_EXPR )
					{
						dynamic_cast<SP_DS_ColStPlaceAnimator*>(itExprList->m_pcPlAnimator)->AddCandidate(itExprList->m_pcEdge, this);
					}
				}

				m_bEnabled = TRUE;   // Let it enable
				return TRUE;


			}
			

			// backwards animation
			else
			{
			}


	    return TRUE;
}


SP_IMPORT_STATE
SP_DS_ColStTransAnimator::CheckColour(const wxString& p_sColor)
{
	Reset();

	if (!m_pcNode || !m_pcAnimation)
		return SP_IMPORT_STATE::SP_IMPORT_ANIM_INVALID;

	vector<SP_CPN_ExpressionInfo> m_StExprInfoVector;
	SP_CPN_ExpressionInfo m_cStExprInfo;


	SP_ListEdge::const_iterator l_itEdges;
	SP_DS_ColStPlaceAnimator* l_pcAnim;


		m_StExprInfoVector.clear();

		//push back the preplace and edges
		for (l_itEdges = m_pcNode->GetTargetEdges()->begin(); l_itEdges != m_pcNode->GetTargetEdges()->end(); ++l_itEdges)
		{
			if ((*l_itEdges)->GetSource())
			{
				l_pcAnim = dynamic_cast<SP_DS_ColStPlaceAnimator*>(m_pcAnimation->GetAnimator((*l_itEdges)->GetSource(), SP_DS_ANIMATOR_PLACE));
				if (l_pcAnim)
				{
					m_mlPrePlaces[l_pcAnim].push_back((*l_itEdges));

					m_cStExprInfo.m_eExprType = CPN_INPUT_EXPR;
					m_cStExprInfo.m_pcPlAnimator = dynamic_cast<SP_DS_Animator*>(l_pcAnim);
					m_cStExprInfo.m_pcEdge = *l_itEdges;
					m_pcColList = dynamic_cast< SP_DS_ColListAttribute* >((*l_itEdges)->GetAttribute(SP_DS_CPN_INSCRIPTION));
					if (!m_pcColList)
						return SP_IMPORT_STATE::SP_IMPORT_ANIM_INVALID;
					if (m_pcColList->GetCell(0, 1) == wxT(""))
					{
						wxString l_sError;
						l_sError = wxT("Arc exprssion should not be empty");
						SP_MESSAGEBOX(l_sError, wxT("Expression checking"), wxOK | wxICON_ERROR);
						return SP_IMPORT_STATE::SP_IMPORT_ANIM_INVALID;
					}

					m_cStExprInfo.m_sExpression = m_pcColList->GetCell(0, 1);


					wxString l_sExpression = m_pcColList->GetCell(0, 1);

					if (l_sExpression.Freq(',') != p_sColor.Freq(','))
					{
						return SP_IMPORT_STATE::SP_IMPORT_ANIM_INVALID;
					}

				}
			}
		}

		//push back the postplace and edges
		for (l_itEdges = m_pcNode->GetSourceEdges()->begin(); l_itEdges != m_pcNode->GetSourceEdges()->end(); ++l_itEdges)
		{
			if ((*l_itEdges)->GetTarget())
			{
				l_pcAnim = dynamic_cast<SP_DS_ColStPlaceAnimator*>(m_pcAnimation->GetAnimator((*l_itEdges)->GetTarget(), SP_DS_ANIMATOR_PLACE));
				if (l_pcAnim)
				{
					m_cStExprInfo.m_eExprType = CPN_OUTPUT_EXPR;
					m_cStExprInfo.m_pcPlAnimator = dynamic_cast<SP_DS_Animator*>(l_pcAnim);
					m_cStExprInfo.m_pcEdge = *l_itEdges;
					m_pcColList = dynamic_cast< SP_DS_ColListAttribute* >((*l_itEdges)->GetAttribute(SP_DS_CPN_INSCRIPTION));
					if (!m_pcColList)
						return SP_IMPORT_STATE::SP_IMPORT_ANIM_INVALID;
					if (m_pcColList->GetCell(0, 1) == wxT(""))
						return SP_IMPORT_STATE::SP_IMPORT_ANIM_INVALID;
					m_cStExprInfo.m_sExpression = m_pcColList->GetCell(0, 1);

					m_StExprInfoVector.push_back(m_cStExprInfo);

					wxString l_sExpression =m_pcColList->GetCell(0, 1);

					if (l_sExpression.Freq(',') != p_sColor.Freq(','))
					{
						return SP_IMPORT_STATE::SP_IMPORT_ANIM_INVALID;
					}
				}
			}
		}

		//push back the guard
		m_cStExprInfo.m_eExprType = CPN_GUARD_EXPR;
		m_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(m_pcNode->GetAttribute(SP_DS_CPN_GUARDLIST));
		if (m_pcColList)
			m_cStExprInfo.m_sExpression = m_pcColList->GetCell(0, 1);

		if (m_cStExprInfo.m_sExpression != wxT(""))
			m_StExprInfoVector.push_back(m_cStExprInfo);

		//begin to binding and enable test
		SP_CPN_Binding l_cBinding;
		SP_DS_Animator* l_pcAnimator = dynamic_cast<SP_DS_ColStTransAnimator*>(this);
		map<wxString, map<SP_DS_Edge*, map<wxString, int> > > l_mmmBind2Edge2Mult2Color;
		bool l_bEnableTest = l_cBinding.EnableTest(&m_StExprInfoVector, m_bSingleClick, l_pcAnimator, m_nBindingChoice, l_mmmBind2Edge2Mult2Color);

		std::vector<SP_VectorString> l_vvPosibleValues = l_cBinding.GetColmpleteBinding();

		if (!l_bEnableTest  || l_vvPosibleValues.size() == 0)
			return SP_IMPORT_STATE::SP_IMPORT_ANIM_VALID;

		if (l_vvPosibleValues.size() == 1)
		{
			if (p_sColor != l_vvPosibleValues[0][0])
				return SP_IMPORT_STATE::SP_IMPORT_ANIM_NOT_MATCHING_STATE;
		}

		SP_VectorString l_vParsedColors;
		if (p_sColor.Freq(wxChar(',') )> 0)
		{
			wxString l_sColor = p_sColor;
			l_sColor.Replace(wxT("("), wxT(""));
			l_sColor.Replace(wxT(")"), wxT(""));
			while (l_sColor.Freq(wxChar(',')) > 0)
			{
				wxString l_sPre = l_sColor.BeforeFirst(wxChar(','));
				l_vParsedColors.push_back(l_sPre);
				l_sPre << wxT(",");
				l_sColor.Replace(l_sPre, wxT(""));
				if (l_sColor.Freq(wxChar(',')) == 0)
				{
					l_vParsedColors.push_back(l_sColor);
				}
			}

		}


		for (unsigned i = 0; i < l_vvPosibleValues.size(); i++)
		{
			for (unsigned j = 0; j < l_vvPosibleValues[i].size(); j++)
			{
				 if(p_sColor== l_vvPosibleValues[i][j]&& p_sColor.Freq(wxChar(','))==0)
				 {
					 return SP_IMPORT_STATE::SP_IMPORT_ANIM_VALID;
				 }
				 else if (p_sColor.Freq(wxChar(',')) > 0 && l_vParsedColors.size()== l_vvPosibleValues[i].size())
				 {
					 if (l_vvPosibleValues[i] == l_vParsedColors)
					 {
						 return SP_IMPORT_STATE::SP_IMPORT_ANIM_VALID;
					 }
				 }
			}
		}



		return SP_IMPORT_STATE::SP_IMPORT_ANIM_NOT_MATCHING_STATE;
}

