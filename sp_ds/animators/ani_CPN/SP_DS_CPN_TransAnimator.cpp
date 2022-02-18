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
SP_DS_CPN_TransAnimator::InformPrePlaces(const wxString& p_sColor)
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
								m_cExprInfo.m_sExpression.Replace(wxT(" "), wxT(""));//by george
								m_cExprInfo.m_sExpression.Replace(wxT("\t"), wxT(""));//by george
								m_cExprInfo.m_sExpression.Replace(wxT("\n"), wxT(""));//by george
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

				//SP_MESSAGEBOX(m_mmmBind2Edge2Mult2Color.begin()->first);

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
				else if (p_sColor != wxT(""))
				{//by george, take the color from the imported file
					wxString l_sColor,l_sColorFormat;
					if (l_EnabledBindings.size() > 0)
					{
						SP_VectorString l_vVars;
						if (p_sColor.Freq(wxChar(','))+1 == l_EnabledBindings[0].Freq(wxChar(';')) && p_sColor.Contains(wxT(",")))
						{
							l_sColor = l_EnabledBindings[0];

							if (p_sColor.Freq(wxChar(',')) > 0)
							{
								while (l_sColor.Freq(wxChar(';'))>0)
								{
									wxString l_sPop = l_sColor.BeforeFirst(wxChar(';'));
									wxString l_sc = l_sPop.BeforeFirst(wxChar('='));
									l_sc.Replace(wxT(" "), wxT(""));
									l_vVars.push_back(l_sc);
									l_sPop << wxT(";");
								    l_sColor.Replace(l_sPop, wxT(""),false);
								}
							}
						}
						if (!p_sColor.Contains(wxT(",")))
						{
							l_sColor = l_EnabledBindings[0].BeforeFirst(wxChar('='));
							if(l_sColor.Freq(wxChar(' '))==1)
							l_sColor << wxT("= ") << p_sColor << wxT(";");
							else
							l_sColor << wxT(" = ") << p_sColor << wxT(";");
						}

						SP_VectorString l_vValues;
						if (p_sColor.Contains(wxT(",")))
						{
							wxString l_sRawColo = p_sColor;
							l_sRawColo.Replace(wxT("("), wxT(""));
							l_sRawColo.Replace(wxT(")"), wxT(""));
							l_sRawColo << wxT(",");
							while (l_sRawColo.Freq(wxChar(',')) > 0)
							{
								wxString l_sVal = l_sRawColo.BeforeFirst(wxChar(','));
								l_vValues.push_back(l_sVal);
								l_sVal << wxT(",");
								l_sRawColo.Replace(l_sVal,wxT(""),false);
							}
						}
						if (l_vValues.size() == l_vVars.size())
						{
							for (int i = 0; i < l_vValues.size(); i++)
							{
								l_sColor << l_vVars[i] << wxT(" = ") << l_vValues[i] << wxT(";");
							}
						}
						l_sColorFormat = l_EnabledBindings[0].AfterFirst(wxChar('='));
						l_sColorFormat = l_sColorFormat.BeforeFirst(wxChar(';'));
						l_sColorFormat.Replace(wxT(" "), wxT(""));
						//l_sColor.Replace(l_sColorFormat, p_sColor);
						itMap3 = m_mmmBind2Edge2Mult2Color.begin();
						int l_nChoice1=0;
						for (auto itMap = m_mmmBind2Edge2Mult2Color.begin(); itMap != m_mmmBind2Edge2Mult2Color.end(); ++itMap)
						{
							if (itMap->first== l_sColor)
							{
								break;
							}
							++l_nChoice1;
						}

						for (int i = 0; i < l_nChoice1; i++)
							itMap3++;
						if (l_nChoice1 == m_mmmBind2Edge2Mult2Color.size())
							itMap3--;

					}
					wxString l_sSelBinding = l_sColor;
					map<SP_DS_Edge*, map<wxString, int> >  l_mmEdge2Color2Mult = itMap3->second;
					m_mmmBind2Edge2Mult2Color.clear();
					m_mmmBind2Edge2Mult2Color[l_sSelBinding] = l_mmEdge2Color2Mult;
					m_vsSelBindings.clear();
					m_vsSelBindings.push_back(l_sSelBinding);
				}


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

SP_IMPORT_STATE
SP_DS_CPN_TransAnimator::CheckColor(const wxString& p_sColor)
{


	Reset();
	if (!m_pcNode || !m_pcAnimation)
		return SP_IMPORT_STATE::SP_IMPORT_ANIM_INVALID;

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
					m_pcColList = dynamic_cast<SP_DS_ColListAttribute*>((*l_itEdges)->GetAttribute(SP_DS_CPN_INSCRIPTION));
					if (!m_pcColList)
						SP_IMPORT_STATE::SP_IMPORT_ANIM_INVALID;
					if (m_pcColList->GetCell(0, 1) == wxT(""))
					{
						wxString l_sError;
						l_sError = wxT("Arc exprssion should not be empty");
						SP_MESSAGEBOX(l_sError, wxT("Expression checking"), wxOK | wxICON_ERROR);
						return SP_IMPORT_STATE::SP_IMPORT_ANIM_INVALID;
					}

						m_cExprInfo.m_sExpression = m_pcColList->GetCell(0, 1);
						m_ExprInfoList.push_back(m_cExprInfo);

					wxString ArcExpr = m_pcColList->GetCell(0, 1);
					//m_ExprInfoList.push_back(m_cExprInfo);
					if (ArcExpr.Freq(',') != p_sColor.Freq(','))
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
				l_pcAnim = dynamic_cast<SP_DS_CPN_PlaceAnimator*>(m_pcAnimation->GetAnimator((*l_itEdges)->GetTarget(), SP_DS_ANIMATOR_PLACE));
				if (l_pcAnim)
				{
					m_cExprInfo.m_eExprType = CPN_OUTPUT_EXPR;
					m_cExprInfo.m_pcPlAnimator = dynamic_cast<SP_DS_Animator*>(l_pcAnim);
					m_cExprInfo.m_pcEdge = *l_itEdges;
					m_pcColList = dynamic_cast<SP_DS_ColListAttribute*>((*l_itEdges)->GetAttribute(SP_DS_CPN_INSCRIPTION));
					if (!m_pcColList)
						SP_IMPORT_STATE::SP_IMPORT_ANIM_INVALID;
					if (m_pcColList->GetCell(0, 1) == wxT(""))
						SP_IMPORT_STATE::SP_IMPORT_ANIM_INVALID;
						m_cExprInfo.m_sExpression = m_pcColList->GetCell(0, 1);
					wxString l_sArcExpr = m_pcColList->GetCell(0, 1);

					if (l_sArcExpr.Freq(',') != p_sColor.Freq(','))
					{
						return SP_IMPORT_STATE::SP_IMPORT_ANIM_INVALID;
					}
					m_ExprInfoList.push_back(m_cExprInfo);
				}
			}
		}

	}

	//push back the guard
	m_cExprInfo.m_eExprType = CPN_GUARD_EXPR;
	m_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(m_pcNode->GetAttribute(SP_DS_CPN_GUARDLIST));
	if (m_pcColList)
		m_cExprInfo.m_sExpression = m_pcColList->GetCell(0, 1);

	if (m_cExprInfo.m_sExpression != wxT(""))
		m_ExprInfoList.push_back(m_cExprInfo);

	//begin to binding and enable test
	SP_CPN_Binding l_cBinding;
	SP_DS_Animator* l_pcAnimator = dynamic_cast<SP_DS_CPN_TransAnimator*>(this);

	map<wxString, map<SP_DS_Edge*, map<wxString, int> > > l_mmmBind2Edge2Mult2Color;
	bool l_bEnableTest = l_cBinding.EnableTest(&m_ExprInfoList, false, l_pcAnimator, m_nBindingChoice, l_mmmBind2Edge2Mult2Color);

	std::vector<SP_VectorString> l_vvPosibleValues = l_cBinding.GetColmpleteBinding();

	if (!l_bEnableTest || l_vvPosibleValues.size()==0)
	{
		return SP_IMPORT_STATE::SP_IMPORT_ANIM_VALID;
	}

	if (  l_vvPosibleValues.size() == 1)
	{
		if (p_sColor != l_vvPosibleValues[0][0])
			return SP_IMPORT_STATE::SP_IMPORT_ANIM_NOT_MATCHING_STATE;
	}


	SP_VectorString l_vParsedColors;
	if (p_sColor.Freq(wxChar(',')) > 0)
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
			if (p_sColor == l_vvPosibleValues[i][j] && p_sColor.Freq(wxChar(',')) == 0)
			{
				return SP_IMPORT_STATE::SP_IMPORT_ANIM_VALID;
			}
			else if (p_sColor.Freq(wxChar(',')) > 0 && l_vParsedColors.size() == l_vvPosibleValues[i].size())
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

