// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2009/03/26 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_ds/extensions/SP_DS_Transformer.h"
#include "sp_gui/dialogs/dia_SPN/SP_DLG_StTransformation.h"

#include "sp_utilities.h"
#include "sp_gr/attributes/SP_GR_ColListAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "snoopy.h"
#include "sp_core/SP_GPR_Elements.h"
#include "sp_ds/SP_DS_Edgeclass.h"
#include "sp_ds/SP_DS_Nodeclass.h"
#include "sp_defines.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogGraphic.h"
#include "sp_core/SP_Core.h"
#include "sp_core/base/SP_ElementClass.h"
#include "sp_core/base/SP_Name.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gui/mdi/SP_MDI_View.h"

SP_DS_Transformer::SP_DS_Transformer()
{
}

SP_DS_Transformer::~SP_DS_Transformer()
{
}

bool
SP_DS_Transformer::Check(SP_DS_Graph* p_pcVal)
{
	bool l_bModify = false;
	if(p_pcVal)
	{
		l_bModify |= CheckElements(p_pcVal);
		l_bModify |= CheckUniqueNames(p_pcVal);
		l_bModify |= CheckTransition(p_pcVal);
		if(l_bModify)
		{
			p_pcVal->GetParentDoc()->Modify(true);
			p_pcVal->GetParentDoc()->Refresh();
		}
	}
	return !l_bModify;
}

bool
SP_DS_Transformer::CheckElements(SP_DS_Graph* p_pcVal)
{
	bool l_bReturn = false;
	list<SP_DS_NameAttribute*> l_lChangeNames;
	for(SP_DS_Nodeclass* l_NC : *(p_pcVal->GetNodeclasses()))
	{
		if(l_NC->GetShowInElementTree()
			|| l_NC->GetShowInDeclarationTreeOther()
			|| l_NC->GetShowInDeclarationTreeColorSet())
		{
			for(SP_DS_Node* l_Elem : *(l_NC->GetElements()))
			{
				SP_DS_NameAttribute* l_pcAttr =
						dynamic_cast<SP_DS_NameAttribute*>(l_Elem->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME));
				if(l_pcAttr)
				{
					if(!l_pcAttr->IsOk())
					{
						l_lChangeNames.push_back(l_pcAttr);
					}
				}
			}
		}
	}
	for(SP_DS_Metadataclass* l_MC : *(p_pcVal->GetMetadataclasses()))
	{
		if(l_MC->GetShowInElementTree()
			|| l_MC->GetShowInDeclarationTreeOther()
			|| l_MC->GetShowInDeclarationTreeColorSet())
		{
			for(SP_DS_Metadata* l_Elem : *(l_MC->GetElements()))
			{
				SP_DS_NameAttribute* l_pcAttr =
						dynamic_cast<SP_DS_NameAttribute*>(l_Elem->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME));
				if(l_pcAttr)
				{
					if(!l_pcAttr->IsOk())
					{
						l_lChangeNames.push_back(l_pcAttr);
					}
				}
			}
		}
	}
	if(!l_lChangeNames.empty())
	{
		wxString l_sMsg = wxT("There ");
		if(l_lChangeNames.size() == 1)
		{
			l_sMsg << wxT("is 1 element ");
		}
		else
		{
			l_sMsg << wxT("are ") << l_lChangeNames.size() << wxT(" elements ");
		}
		l_sMsg << wxT("found, that don't fit the requirements.\n\n")
				<< wxT("The name has to begin with a letter and \n")
				<< wxT("only letters, numbers and underscore are allowed.\n\n")
				<< wxT("After conversion the original names are saved in the Comment.");
		wxArrayString l_Choices;
		l_Choices.Add(wxT("Convert all prohibited characters into underscore."));
		l_Choices.Add(wxT("Change all names by hand."));
		l_Choices.Add(wxT("Do nothing."));
		int l_ReturnChoice = ::wxGetSingleChoiceIndex(l_sMsg, wxT("Change name."), l_Choices);
		if(l_ReturnChoice == 0
			|| l_ReturnChoice == 1)
		{
			list<SP_DS_NameAttribute*>::iterator l_it;
			for(l_it = l_lChangeNames.begin(); l_it != l_lChangeNames.end(); ++l_it)
			{
				SP_DS_NameAttribute* l_pcAttr = (*l_it);
				wxString l_sOldName = l_pcAttr->GetValueString();
				if(l_ReturnChoice == 0)
				{
					l_pcAttr->SetValueString(SP_NormalizeString(l_sOldName));
				}
				else if(l_ReturnChoice == 1)
				{
					l_pcAttr->SetValueString(l_sOldName);
				}
				if(l_sOldName != l_pcAttr->GetValueString())
				{
					SP_DS_Attribute* l_pcComAttr = l_pcAttr->GetParent()->GetAttribute(wxT("Comment"));
					if(l_pcComAttr)
					{
						wxString l_sValue = l_pcComAttr->GetValueString();
						if(!l_sValue.IsEmpty())
							l_sValue << wxT("\n");
						l_sValue << wxT("//old name:") << l_sOldName;
						l_pcComAttr->SetValueString(l_sValue);
					}
				}
			}
		}
	}
	return l_bReturn;
}

bool
SP_DS_Transformer::CheckUniqueNames(SP_DS_Graph* p_pcVal)
{
	bool l_bUnique = true;
	std::multimap<wxString,SP_DS_Attribute*> l_Name2Attr;
	SP_SetString l_Names;
	for(SP_DS_Nodeclass* l_NC : *(p_pcVal->GetNodeclasses()))
	{
		if(l_NC->GetShowInElementTree()
			&& l_NC->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID)
			&& l_NC->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))
		{
			for(SP_DS_Node* l_Elem : *(l_NC->GetElements()))
			{
				SP_DS_Attribute* l_pcAttr = l_Elem->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME);
				if(l_pcAttr)
				{
					wxString l_Name = l_pcAttr->GetValueString();
					if(!l_Name.IsEmpty())
					{
						auto it = l_Name2Attr.find(l_Name);
						if(it == l_Name2Attr.end())
						{
							l_Name2Attr.emplace(l_Name, l_pcAttr);
							l_Names.emplace(l_Name);
						}
						else
						{
							l_bUnique = false;
							l_Name2Attr.emplace_hint(it, l_pcAttr->GetValueString(), l_pcAttr);
						}
					}
				}
			}
		}
	}
	for(SP_DS_Metadataclass* l_MC : *(p_pcVal->GetMetadataclasses()))
	{
		if((l_MC->GetShowInDeclarationTreeOther()
			|| l_MC->GetShowInDeclarationTreeColorSet())
			&& l_MC->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID)
			&& l_MC->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))
		{
			for(SP_DS_Metadata* l_Elem : *(l_MC->GetElements()))
			{
				SP_DS_Attribute* l_pcAttr = l_Elem->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME);
				if(l_pcAttr)
				{
					wxString l_Name = l_pcAttr->GetValueString();
					if(!l_Name.IsEmpty())
					{
						auto it = l_Name2Attr.find(l_Name);
						if(it == l_Name2Attr.end())
						{
							l_Name2Attr.emplace(l_Name, l_pcAttr);
							l_Names.emplace(l_Name);
						}
						else
						{
							l_bUnique = false;
							l_Name2Attr.emplace_hint(it, l_pcAttr->GetValueString(), l_pcAttr);
						}
					}
				}
			}
		}
	}
	if(!l_bUnique)
	{
		wxString l_sMsg = wxT("There were elements found, having duplicate names.\n");
		l_sMsg << wxT("After conversion the original names are saved in the Comment.");
		wxArrayString l_Choices;
		l_Choices.Add(wxT("Make all names unique."));
		l_Choices.Add(wxT("Change all names by hand."));
		l_Choices.Add(wxT("Do nothing."));
		int l_ReturnChoice = ::wxGetSingleChoiceIndex(l_sMsg, wxT("Duplicate name."), l_Choices);
		if(l_ReturnChoice == 0
			|| l_ReturnChoice == 1)
		{
			auto m_it = l_Name2Attr.begin();
			auto s_it = m_it;
			for (; m_it != l_Name2Attr.end();  m_it = s_it)
			{
				wxString theKey = (*m_it).first;
				auto keyRange = l_Name2Attr.equal_range(theKey);
				// Iterate over all map elements with key == theKey
				s_it = keyRange.first;
				for (++s_it;  s_it != keyRange.second;  ++s_it)
				{
					SP_DS_Attribute* l_pcAttr = (*s_it).second;
					wxString l_sValue;
					if(l_ReturnChoice == 0)
					{
						l_pcAttr->SetValueString(wxT(""), false);
						l_sValue = l_pcAttr->GetParentName() + theKey;
					}
					else
					if(l_ReturnChoice == 1)
					{
						wxString l_sMsg = wxT("Please enter a unique name: ");
						wxTextEntryDialog* l_pcDlg = new wxTextEntryDialog(NULL, l_sMsg, wxT("Change name."), theKey);
						bool finished;
						do
						{
							finished = true;
							if(l_pcDlg->ShowModal() == wxID_OK)
							{
								l_sValue = l_pcDlg->GetValue();
								if(!l_sValue.IsEmpty())
								{
									if(l_Names.find(l_sValue) != l_Names.end())
									{
										finished = false;
										SP_MESSAGEBOX(wxT("Please enter a unique name."), wxT("Warning"));
									}
									else
									{
										l_Names.emplace(l_sValue);
									}
								}
							}
							else
							{
								l_sValue = theKey;
							}
						} while(!finished);
						l_pcDlg->Destroy();
					}
					l_pcAttr->SetValueString(l_sValue);
					SP_DS_Attribute* l_pcComAttr = l_pcAttr->GetParent()->GetAttribute(wxT("Comment"));
					if(l_pcComAttr && l_sValue != theKey)
					{
						l_sValue = l_pcComAttr->GetValueString();
						if(!l_sValue.IsEmpty())
							l_sValue << wxT("\n");
						l_sValue << wxT("//old name:") << theKey;
						l_pcComAttr->SetValueString(l_sValue);
					}
				}
			}
		}
	}
	return !l_bUnique;
}

bool SP_DS_Transformer::CheckTransition(SP_DS_Graph* p_pcVal)
{
	bool l_bReturn = false;
	SP_DS_Nodeclass* l_pcNodeclass;
	SP_ListNode::const_iterator l_itElem;
	SP_DS_ColListAttribute* l_pcColList;
	wxString l_Function;
	vector<wxString> l_TransformTransition;
	list<wxString>::iterator aIt;
	wxString l_TransitionName;
	vector<wxString> l_ComboBoxValue;
	l_TransformTransition.clear();
	SP_ListNode l_lTransformingNodes;

	l_pcNodeclass= p_pcVal->GetNodeclass(wxT("Transition"));
	if(l_pcNodeclass)
	{
		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{
			l_pcColList	= dynamic_cast< SP_DS_ColListAttribute* >((*l_itElem)->GetAttribute(wxT("FunctionList")) );
			if(l_pcColList)
			{
				for(unsigned i = 0; i < l_pcColList->GetRowCount(); i++ )
				{
					l_Function= l_pcColList->GetCell(i,1);

					if ((l_Function.Find(wxT("ImmediateFiring")) != wxNOT_FOUND)||
					   (l_Function.Find(wxT("TimedFiring")) != wxNOT_FOUND)||
					   (l_Function.Find(wxT("FixedTimedFiring_Single")) != wxNOT_FOUND)||
					   (l_Function.Find(wxT("FixedTimedFiring_Periodic")) != wxNOT_FOUND))
					{
						l_lTransformingNodes.push_back(*l_itElem);
						l_bReturn = true;
						break;
					}
					else if(l_Function.Find(wxT("BioMassAction")) != wxNOT_FOUND)
					{
						l_Function.Replace(wxT("BioMassAction"), wxT("MassAction"));
						l_pcColList->SetCell(i, 1, l_Function);
						l_bReturn = true;
					}
					else if(l_Function.Find(wxT("BioLevelInterpretation")) != wxNOT_FOUND)
					{
						l_Function.Replace(wxT("BioLevelInterpretation"), wxT("LevelInterpretation"));
						l_pcColList->SetCell(i, 1, l_Function);
						l_bReturn = true;
					}
				}
			}
		}
	}

	if(l_lTransformingNodes.size()>0)
	{
		SP_DLG_StTransformation l_cDlg( &l_lTransformingNodes,NULL);

		if ( l_cDlg.ShowModal() == wxID_OK )
		{
			l_ComboBoxValue=l_cDlg.GetComboBoxValue();
			TransformTransition(p_pcVal, &l_lTransformingNodes,l_ComboBoxValue);
		}
		l_cDlg.Destroy();
	}

	return l_bReturn;
}


bool SP_DS_Transformer::TransformTransition(SP_DS_Graph* p_pcVal, SP_ListNode* p_pcTransformingNodes, vector<wxString> ComboBoxValue)
{
	SP_DS_Nodeclass* l_pcNodeclass;
	SP_ListNode::const_iterator l_itElem;
	wxString l_Function;
	vector<wxString> l_TransformTransition;
	list<wxString>::iterator aIt;
	wxString l_TransitionName;
	l_TransformTransition.clear();

	SP_MDI_View* l_pcView = dynamic_cast<SP_MDI_View*>(p_pcVal->GetParentDoc()->GetFirstView());
	SP_GUI_Canvas* l_pcCanvas = l_pcView->GetCanvas();

	unsigned int l_nNewRow;

	l_pcNodeclass= p_pcVal->GetNodeclass(wxT("Transition"));
	SP_ListNode l_lOldNodes;
	int l_nNumber=0;

	for (l_itElem = p_pcTransformingNodes->begin(); l_itElem != p_pcTransformingNodes->end(); ++l_itElem)
	{
		SP_DS_Node* l_pcOldNode = *l_itElem;

		if(ComboBoxValue[l_nNumber]==wxT("Not Transform"))
		{
		}

		if(ComboBoxValue[l_nNumber]==wxT("Transition"))
		{
			SP_DS_ColListAttribute* l_pcOldAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcOldNode->GetAttribute(wxT("FunctionList")));

			for(unsigned int i = 0; i < l_pcOldAttr->GetRowCount(); i++ )
			{
				wxString l_sFunction= l_pcOldAttr->GetCell(i,1);

				l_sFunction.Replace(wxT(" "), wxT(""));
				l_sFunction.Replace(wxT("\t"), wxT(""));
				l_sFunction.Replace(wxT("\r"), wxT(""));
				l_sFunction.Replace(wxT("\n"), wxT(""));

				if ((l_sFunction.Find(wxT("ImmediateFiring")) != wxNOT_FOUND)||
			       (l_sFunction.Find(wxT("TimedFiring")) != wxNOT_FOUND)||
			       (l_sFunction.Find(wxT("FixedTimedFiring_Single")) != wxNOT_FOUND)||
			       (l_sFunction.Find(wxT("FixedTimedFiring_Periodic")) != wxNOT_FOUND))
				{
					if(i==0)
						l_pcOldAttr->SetCell(0,1,wxT("1"));
					else
						l_pcOldAttr->RemoveRow(i);
				}
			}

		}

		if(ComboBoxValue[l_nNumber]==wxT("Immediate Transition"))
		{
			SP_DS_Nodeclass* l_pcNewNC = p_pcVal->GetNodeclass(wxT("Immediate Transition"));
			SP_DS_Node* l_pcNewNode = l_pcNewNC->NewElement(l_pcOldNode->GetNetnumber());

			l_pcNewNode->GetAttribute(wxT("Name"))->SetValueString(l_pcOldNode->GetAttribute(wxT("Name"))->GetValueString());
			l_pcNewNode->GetAttribute(wxT("Comment"))->SetValueString(l_pcOldNode->GetAttribute(wxT("Comment"))->GetValueString());
			l_pcNewNode->GetAttribute(wxT("Logic"))->SetValueString(l_pcOldNode->GetAttribute(wxT("Logic"))->GetValueString());

            SP_DS_ColListAttribute* l_pcOldAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcOldNode->GetAttribute(wxT("FunctionList")));
			SP_DS_ColListAttribute* l_pcNewAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNewNode->GetAttribute(wxT("FunctionList")));

			for(unsigned int i = 0; i < l_pcOldAttr->GetRowCount(); i++ )
			{
				wxString l_sSetName = l_pcOldAttr->GetCell(i,0);
				wxString l_sFunction= l_pcOldAttr->GetCell(i,1);

				l_sFunction.Replace(wxT(" "), wxT(""));
				l_sFunction.Replace(wxT("\t"), wxT(""));
				l_sFunction.Replace(wxT("\r"), wxT(""));
				l_sFunction.Replace(wxT("\n"), wxT(""));

				if (l_sFunction.Find(wxT("ImmediateFiring(")) != wxNOT_FOUND)
				{
					wxString l_sNewFunction;
					l_sNewFunction=l_sFunction.AfterFirst('(');
					l_sNewFunction=l_sNewFunction.BeforeLast(')');
					if(l_sNewFunction.IsEmpty())
					{
						l_sNewFunction = wxT("1");
					}
					l_nNewRow=0;
					if(i>0)
					{
						l_nNewRow = l_pcNewAttr->AppendEmptyRow();
					}
					l_pcNewAttr->SetCell(l_nNewRow, 0, l_sSetName);
					l_pcNewAttr->SetCell(l_nNewRow, 1, l_sNewFunction);
				}

			}

			bool l_bFirstGr = true;
			for(SP_ListGraphic::const_iterator l_itGr = l_pcOldNode->GetGraphics()->begin();
				l_itGr != l_pcOldNode->GetGraphics()->end();
				l_itGr++)
			{
				SP_Graphic* l_pcOldGr = *l_itGr;
				SP_Graphic* l_pcNewGr = NULL;
				if(l_bFirstGr)
				{
					l_pcNewGr = *(l_pcNewNode->GetGraphics()->begin());
					l_bFirstGr = false;
				}
				else
				{
					l_pcNewGr = l_pcNewNode->NewGraphicInSubnet(l_pcOldGr->GetNetnumber());
				}
				l_pcNewGr->SetGraphicState(l_pcOldGr->GetGraphicState());
				l_pcNewGr->SetPosX(l_pcOldGr->GetPosX());
				l_pcNewGr->SetPosY(l_pcOldGr->GetPosY());
				l_pcOldGr->SetNewSource(l_pcNewGr);
				l_pcOldGr->SetNewTarget(l_pcNewGr);
			}
			l_pcOldNode->SetNewSource(l_pcNewNode);
			l_pcOldNode->SetNewTarget(l_pcNewNode);

			l_pcNewNode->ShowOnCanvas(l_pcCanvas);
			l_lOldNodes.push_back(*l_itElem);
		}

		if(ComboBoxValue[l_nNumber]==wxT("Deterministic Transition"))
		{

			SP_DS_Nodeclass* l_pcNewNC = p_pcVal->GetNodeclass(wxT("Deterministic Transition"));
			SP_DS_Node* l_pcNewNode = l_pcNewNC->NewElement(l_pcOldNode->GetNetnumber());

			l_pcNewNode->GetAttribute(wxT("Name"))->SetValueString(l_pcOldNode->GetAttribute(wxT("Name"))->GetValueString());
			l_pcNewNode->GetAttribute(wxT("Comment"))->SetValueString(l_pcOldNode->GetAttribute(wxT("Comment"))->GetValueString());
			l_pcNewNode->GetAttribute(wxT("Logic"))->SetValueString(l_pcOldNode->GetAttribute(wxT("Logic"))->GetValueString());

            SP_DS_ColListAttribute* l_pcOldAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcOldNode->GetAttribute(wxT("FunctionList")));
			SP_DS_ColListAttribute* l_pcNewAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNewNode->GetAttribute(wxT("DelayList")));

			for(unsigned int i = 0; i < l_pcOldAttr->GetRowCount(); i++ )
			{
				wxString l_sSetName = l_pcOldAttr->GetCell(i,0);
				wxString l_sFunction= l_pcOldAttr->GetCell(i,1);

				l_sFunction.Replace(wxT(" "), wxT(""));
				l_sFunction.Replace(wxT("\t"), wxT(""));
				l_sFunction.Replace(wxT("\r"), wxT(""));
				l_sFunction.Replace(wxT("\n"), wxT(""));

				if (l_sFunction.Find(wxT("TimedFiring(")) != wxNOT_FOUND)
				{
					wxString l_Delay;
					l_Delay=l_sFunction.AfterFirst('(');
					l_Delay=l_Delay.BeforeLast(')');
					l_nNewRow=0;
					if(i>0)
					{
						l_nNewRow = l_pcNewAttr->AppendEmptyRow();
					}
					l_pcNewAttr->SetCell(l_nNewRow, 0, l_sSetName);
					l_pcNewAttr->SetCell(l_nNewRow, 1, l_Delay);
				}

			}

			bool l_bFirstGr = true;
			for(SP_ListGraphic::const_iterator l_itGr = l_pcOldNode->GetGraphics()->begin();
				l_itGr != l_pcOldNode->GetGraphics()->end();
				l_itGr++)
			{
				SP_Graphic* l_pcOldGr = *l_itGr;
				SP_Graphic* l_pcNewGr = NULL;
				if(l_bFirstGr)
				{
					l_pcNewGr = *(l_pcNewNode->GetGraphics()->begin());
					l_bFirstGr = false;
				}
				else
				{
					l_pcNewGr = l_pcNewNode->NewGraphicInSubnet(l_pcOldGr->GetNetnumber());
				}
				l_pcNewGr->SetGraphicState(l_pcOldGr->GetGraphicState());
				l_pcNewGr->SetPosX(l_pcOldGr->GetPosX());
				l_pcNewGr->SetPosY(l_pcOldGr->GetPosY());
				l_pcOldGr->SetNewSource(l_pcNewGr);
				l_pcOldGr->SetNewTarget(l_pcNewGr);
			}
			l_pcOldNode->SetNewSource(l_pcNewNode);
			l_pcOldNode->SetNewTarget(l_pcNewNode);

			l_pcNewNode->ShowOnCanvas(l_pcCanvas);
			l_lOldNodes.push_back(*l_itElem);

		}

		if(ComboBoxValue[l_nNumber]==wxT("Scheduled Transition"))
		{
			SP_DS_Nodeclass* l_pcNewNC = p_pcVal->GetNodeclass(wxT("Scheduled Transition"));
			SP_DS_Node* l_pcNewNode = l_pcNewNC->NewElement(l_pcOldNode->GetNetnumber());

			l_pcNewNode->GetAttribute(wxT("Name"))->SetValueString(l_pcOldNode->GetAttribute(wxT("Name"))->GetValueString());
			l_pcNewNode->GetAttribute(wxT("Comment"))->SetValueString(l_pcOldNode->GetAttribute(wxT("Comment"))->GetValueString());
			l_pcNewNode->GetAttribute(wxT("Logic"))->SetValueString(l_pcOldNode->GetAttribute(wxT("Logic"))->GetValueString());

			SP_DS_ColListAttribute* l_pcOldAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcOldNode->GetAttribute(wxT("FunctionList")));
			SP_DS_ColListAttribute* l_pcNewAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNewNode->GetAttribute(wxT("PeriodicList")));

			for(unsigned int i = 0; i < l_pcOldAttr->GetRowCount(); i++ )
			{
				wxString l_sSetName = l_pcOldAttr->GetCell(i,0);
				wxString l_sFunction= l_pcOldAttr->GetCell(i,1);

				l_sFunction.Replace(wxT(" "), wxT(""));
				l_sFunction.Replace(wxT("\t"), wxT(""));
				l_sFunction.Replace(wxT("\r"), wxT(""));
				l_sFunction.Replace(wxT("\n"), wxT(""));

				if(l_sFunction.Find(wxT("FixedTimedFiring_Single(")) != wxNOT_FOUND)
				{
					wxString l_Single;
					l_Single=l_sFunction.AfterFirst('(');
					l_Single=l_Single.BeforeLast(')');
					l_nNewRow=0;
					if(i>0)
					{
						l_nNewRow = l_pcNewAttr->AppendEmptyRow();
					}
					l_pcNewAttr->SetCell(l_nNewRow, 0, l_sSetName);
					l_pcNewAttr->SetCell(l_nNewRow, 1, l_Single);
					l_pcNewAttr->SetCell(l_nNewRow, 2, wxT("0"));
					l_pcNewAttr->SetCell(l_nNewRow, 3, l_Single);
				}
				if(l_sFunction.Find(wxT("FixedTimedFiring_Periodic(")) != wxNOT_FOUND)
				{
					wxString l_sValue;
					wxString l_Start;
					wxString l_Repetition;
					wxString l_End;

					l_sValue=l_sFunction.AfterFirst('(');
					l_sValue=l_sValue.BeforeLast(')');

					l_Start=l_sValue.BeforeFirst(',');
					l_End=l_sValue.AfterLast(',');
					l_Repetition=l_sValue.BeforeLast(',');
					l_Repetition=l_Repetition.AfterLast(',');

					l_nNewRow=0;
					if(i>0)
					{
						l_nNewRow = l_pcNewAttr->AppendEmptyRow();
					}
					l_pcNewAttr->SetCell(l_nNewRow, 0, l_sSetName);
					l_pcNewAttr->SetCell(l_nNewRow, 1, l_Start);
					l_pcNewAttr->SetCell(l_nNewRow, 2, l_Repetition);
					l_pcNewAttr->SetCell(l_nNewRow, 3, l_End);
				}

			}

			bool l_bFirstGr = true;
			for(SP_ListGraphic::const_iterator l_itGr = l_pcOldNode->GetGraphics()->begin();
				l_itGr != l_pcOldNode->GetGraphics()->end();
				l_itGr++)
			{
				SP_Graphic* l_pcOldGr = *l_itGr;
				SP_Graphic* l_pcNewGr = NULL;
				if(l_bFirstGr)
				{
					l_pcNewGr = *(l_pcNewNode->GetGraphics()->begin());
					l_bFirstGr = false;
				}
				else
				{
					l_pcNewGr = l_pcNewNode->NewGraphicInSubnet(l_pcOldGr->GetNetnumber());
				}
				l_pcNewGr->SetGraphicState(l_pcOldGr->GetGraphicState());
				l_pcNewGr->SetPosX(l_pcOldGr->GetPosX());
				l_pcNewGr->SetPosY(l_pcOldGr->GetPosY());
				l_pcOldGr->SetNewSource(l_pcNewGr);
				l_pcOldGr->SetNewTarget(l_pcNewGr);
			}
			l_pcOldNode->SetNewSource(l_pcNewNode);
			l_pcOldNode->SetNewTarget(l_pcNewNode);

			l_pcNewNode->ShowOnCanvas(l_pcCanvas);
			l_lOldNodes.push_back(*l_itElem);
		}
		l_nNumber++;
	}

	for(SP_ListNode::const_iterator l_it = l_lOldNodes.begin();
		l_it != l_lOldNodes.end();
		l_it++)
	{
		SP_DS_Node *l_pcNode = *l_it;
		l_pcNodeclass->RemoveElement(l_pcNode);
		wxDELETE(l_pcNode);
	}

	return true;
}
