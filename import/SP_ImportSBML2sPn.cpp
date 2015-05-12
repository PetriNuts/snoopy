//////////////////////////////////////////////////////////////////////
// $Author: crohr $
// $Version: 0.1 $
// $Date: 2009/03/25 $
// Short Description: creates a stochastic petrinet on a SBML file
//////////////////////////////////////////////////////////////////////

#include "import/SP_ImportSBML2sPn.h"

#include "sp_ds/netclasses/SP_DS_EventSPN.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_gui/dialogs/SP_DLG_ImportSBML2extPN.h"

SP_ImportSBML2sPn::SP_ImportSBML2sPn()
{
	yComRea = 40;
	xPara = 1;
	yPara = 1;
}

SP_ImportSBML2sPn::~SP_ImportSBML2sPn()
{

}

bool SP_ImportSBML2sPn::ReadFile(const wxString& p_sFile)
{
	g_CompoundList.clear();
	g_ParameterList.clear();
	g_ReactionList.clear();

	SBMLDocument* l_sbmlDocument;

	SP_DLG_ImportSBML2extPN l_cDlg(NULL);

	if (l_cDlg.ShowModal() ==  wxID_OK)
	{
		m_CreateReverseReactions = l_cDlg.GetCreateReverseReactions();
		m_HighlightReversibleReactions = l_cDlg.GetHighlightReverseReactions();

		l_sbmlDocument = readSBML(p_sFile.mb_str());
		CHECK_POINTER(l_sbmlDocument, return FALSE);

		m_sbmlModel = l_sbmlDocument->getModel();
		CHECK_POINTER(m_sbmlModel, return FALSE);

		if(ValidateSBML(l_sbmlDocument))
		{
			m_pcGraph = CreateDocument(SP_DS_SPN_CLASS);

			//SP_MDI_View* l_pcView = dynamic_cast<SP_MDI_View*>(m_pcMyDoc->GetFirstView());

			getModelCompartments();
			// first we have to get model parameters, later the reaction parameters
			getModelParameters();
			getSpecies();
			getReactions();
			//TODO check events
			//getEvents();

			//ConvertIds2Names();

			DoVisualization();

			SP_MESSAGEBOX(wxT("The SBML import is experimental!\nPlease note: rules, events and functions are not supported yet.\nPlease check the result!"),wxT("Notice"), wxOK | wxICON_INFORMATION);

			m_pcMyDoc->Modify(true);
		}
		wxDELETE(l_sbmlDocument);

		return true;
	}
	return false;
}


void SP_ImportSBML2sPn::getSpecies()
{
    for (unsigned int n = 0; n < m_sbmlModel->getNumSpecies() ; ++n)
    {
		Species* l_sbmlSpecies = m_sbmlModel->getSpecies(n);

		wxString l_speciesId;
		wxString l_speciesName;

		getSBMLSpeciesName(l_sbmlSpecies, l_speciesId, l_speciesName);

		SP_DS_Nodeclass* l_pcNodeClass = m_pcGraph->GetNodeclass(wxT("Place"));
		SP_DS_Node* l_pcNode = l_pcNodeClass->NewElement(m_pcCanvas->GetNetnumber());
		if (l_pcNode)
		{
			++yComRea;
			SP_DS_Attribute* l_pcAttrName = l_pcNode->GetAttribute(wxT("Name"));
			l_pcAttrName->SetValueString(l_speciesId);
			l_pcAttrName->SetShow(true);

			SP_DS_Attribute* l_pcAttrComment = l_pcNode->GetAttribute(wxT("Comment"));
			wxString l_comment = l_pcAttrComment->GetValueString();

			wxString l_sCompartment = l_sbmlSpecies->getCompartment();
			if(!l_sCompartment.IsEmpty())
			{
				l_comment << wxT("compartment: ") << l_sCompartment << wxT("\n");
			}
			if(!l_speciesName.IsEmpty())
			{
				l_comment << wxT("name: ") << l_speciesName << wxT("\n");
			}
			l_pcAttrComment->SetValueString(l_comment);
			l_pcAttrComment->SetShow(false);

			if (l_sbmlSpecies->isSetInitialAmount())
			{
				wxString l_sMarking = wxString::Format(wxT("%.0f"),l_sbmlSpecies->getInitialAmount());
				l_pcNode->GetAttribute(wxT("Marking"))->SetValueString(l_sMarking);
			}
			else if(l_sbmlSpecies->isSetInitialConcentration())
			{
				wxString l_sMarking = wxString::Format(wxT("%.0f"),l_sbmlSpecies->getInitialConcentration());
				l_pcNode->GetAttribute(wxT("Marking"))->SetValueString(l_sMarking);
			}

			l_pcNode->ShowOnCanvas(m_pcCanvas, FALSE, 50, yComRea,0);

			g_CompoundList.push_back(l_pcNode);
		}
    }
}

void SP_ImportSBML2sPn::getEvents()
{
    for (unsigned int n = 0; n < m_sbmlModel->getNumEvents() ; ++n)
    {
		Event* l_sbmlEvent = m_sbmlModel->getEvent(n);

		wxString l_eventId;
		wxString l_eventName;
		getSBMLEventName(l_sbmlEvent, l_eventId, l_eventName);

		SP_DS_Nodeclass* l_pcNodeClass = m_pcGraph->GetNodeclass(SP_DS_DETERMINISTIC_TRANS);
		SP_DS_Node* l_pcNode = l_pcNodeClass->NewElement(m_pcCanvas->GetNetnumber());
		if (l_pcNode)
		{
			++yComRea;
			SP_DS_Attribute* l_pcAttrName = l_pcNode->GetAttribute(wxT("Name"));
			l_pcAttrName->SetValueString(l_eventId);
			l_pcAttrName->SetShow(true);

			SP_DS_Attribute* l_pcAttrComment = l_pcNode->GetAttribute(wxT("Comment"));
			wxString l_comment = l_pcAttrComment->GetValueString();

			if(!l_eventName.IsEmpty())
			{
				l_comment << wxT("name: ") << l_eventName << wxT("\n");
			}

			if(l_sbmlEvent->isSetDelay())
			{
				const Delay* l_sbmlDelay = l_sbmlEvent->getDelay();
				if(l_sbmlDelay->isSetMath())
				{
					const ASTNode* l_sbmlMath = l_sbmlDelay->getMath();
					wxString l_delay = formulaToString(l_sbmlMath);
					SP_DS_ColListAttribute* l_pcColAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("DelayList")));
					l_pcColAttr->SetCell(0, 1, l_delay);
				}
			}

			if(l_sbmlEvent->isSetTrigger())
			{
				const Trigger* l_sbmlTrigger = l_sbmlEvent->getTrigger();
				if(l_sbmlTrigger->isSetMath())
				{
					const ASTNode* l_sbmlMath = l_sbmlTrigger->getMath();
					wxString l_trigger = formulaToString(l_sbmlMath);
					l_comment << wxT("Trigger: ")
						<< l_trigger << wxT("\n");
					//TODO add edges; what about "or" expressions?
					//TODO events only fire once, if they got enabled!

				}
			}

			for(unsigned int k = 0; k < l_sbmlEvent->getNumEventAssignments(); ++k)
			{
				const EventAssignment* l_sbmlEventAssignment = l_sbmlEvent->getEventAssignment(k);

				if ( l_sbmlEventAssignment->isSetMath() )
				{
					wxString variable = wxString(l_sbmlEventAssignment->getVariable().c_str(),wxConvUTF8);
					wxString formula  = formulaToString(l_sbmlEventAssignment->getMath());

					l_comment << wxT("EventAssignment ") << (k+1)
						<< wxT(": ") << variable << wxT(" = ") << formula << wxT("\n");

					SP_DS_Node* l_pcPlace = existInCompoundList(variable);
					if(l_pcPlace)
					{
						drawEdge(l_pcPlace, l_pcNode, SP_DS_RESET_EDGE, wxT("0"));
						if(formula != wxT("0"))
						{
							drawEdge(l_pcNode, l_pcPlace, SP_DS_EDGE, formula);
						}
					}
				}
			}

			l_pcAttrComment->SetValueString(l_comment);
			l_pcAttrComment->SetShow(false);
			l_pcNode->ShowOnCanvas(m_pcCanvas, FALSE, 50, yComRea,0);
		}
    }
}

// get reactions, rectants, products, modifiers and kineticlaw
void SP_ImportSBML2sPn::getReactions ()
{
	// reads the Reactions from the SBML File
	yComRea = 40;
	SP_DS_Nodeclass* l_pcNodeClass = m_pcGraph->GetNodeclass(wxT("Transition"));
	for (unsigned int i = 0; i < m_sbmlModel->getNumReactions() ; ++i)
    {
		Reaction* l_sbmlReaction = m_sbmlModel->getReaction(i);

		wxString l_ReactionId;
		wxString l_ReactionName;
		getSBMLReactionName(l_sbmlReaction, l_ReactionId, l_ReactionName);

		SP_DS_Node* l_reactionNode = l_pcNodeClass->NewElement(m_pcCanvas->GetNetnumber());
		SP_DS_Node* l_revReactionNode = NULL;
		if (l_reactionNode)
		{
			++yComRea;
			SP_DS_Attribute* l_pcAttrName = l_reactionNode->GetAttribute(wxT("Name"));
			l_pcAttrName->SetValueString(l_ReactionId);
			l_pcAttrName->SetShow(true);

			SP_DS_Attribute* l_pcAttrComment = l_reactionNode->GetAttribute(wxT("Comment"));
			wxString l_comment = l_pcAttrComment->GetValueString();

			if(!l_ReactionName.IsEmpty())
			{
				l_comment << wxT("name: ") << l_ReactionName << wxT("\n");
			}
			l_pcAttrComment->SetValueString(l_comment);
			l_pcAttrComment->SetShow(false);

			// get KineticLaw
			wxString l_kinetic;
			if (l_sbmlReaction->isSetKineticLaw())
			{
				KineticLaw* l_sbmlKineticLaw =  l_sbmlReaction->getKineticLaw();
				ASTNode* l_sbmlMath = l_sbmlKineticLaw->getMath()->deepCopy();

				l_kinetic =  formulaToString(getSBMLFormula(l_sbmlMath));
				SP_DS_ColListAttribute* l_pcColAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_reactionNode->GetAttribute(wxT("FunctionList")));
				l_pcColAttr->SetCell(0, 1, l_kinetic);

				wxDELETE(l_sbmlMath);

				getReactionParameters(l_sbmlReaction);
			}

			// is reversible (0,1 for false,true) or 0 for default (false)
			bool b_IsReversible = l_sbmlReaction->getReversible();

			if(b_IsReversible)
			{
				SP_LOGWARNING( l_ReactionId + wxT(" has set reversible == true. Stochastic Petri Nets doesn't support reversible Transitions."));
			}

			if(b_IsReversible && m_HighlightReversibleReactions)
			{
				l_reactionNode->GetGraphics()->front()->SetBrushColour(*wxBLUE);
			}

			l_reactionNode->ShowOnCanvas(m_pcCanvas, FALSE, 100, yComRea, 0);

			if (b_IsReversible && m_CreateReverseReactions)
			{
				++yComRea;
				l_revReactionNode = l_pcNodeClass->NewElement(m_pcCanvas->GetNetnumber());
				l_revReactionNode->GetAttribute(wxT("Name"))->SetValueString(wxT("re_")+l_ReactionId);
				l_revReactionNode->GetAttribute(wxT("Name"))->SetShow(TRUE);
				SP_DS_ColListAttribute* l_pcFuncAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_revReactionNode->GetAttribute(wxT("FunctionList")));
				//set ratefunction the same as reversible transition, because we dont know it
				l_pcFuncAttr->SetCell(0,1, l_kinetic);
				l_revReactionNode->GetGraphics()->front()->SetBrushColour(*wxRED);
				l_revReactionNode->ShowOnCanvas(m_pcCanvas, FALSE, 100, yComRea, 0);
			}

		 	// get reactants, products and modifiers
			const ListOfSpeciesReferences* rectants = l_sbmlReaction->getListOfReactants();
			const ListOfSpeciesReferences* products = l_sbmlReaction->getListOfProducts();
			const ListOfSpeciesReferences* modifiers = l_sbmlReaction->getListOfModifiers();

			// search in compoundlist
			SP_ListNode::const_iterator cIt;
			for (cIt = g_CompoundList.begin(); cIt != g_CompoundList.end(); ++cIt)
			{
				SP_DS_Node* l_pcNode = (*cIt);
				wxString sId = l_pcNode->GetAttribute(wxT("Name"))->GetValueString();
				// get rectants of actual reaction
				for (unsigned int lor = 0; lor < rectants->size() ; ++lor)
				{
					wxString l_eductName = wxString(l_sbmlReaction->getReactant(lor)->getSpecies().c_str(), wxConvUTF8);
					if (sId == l_eductName)
					{
						wxString l_stoichiometry =
								wxString::Format(wxT("%.0f"),l_sbmlReaction->getReactant(lor)->getStoichiometry());
						drawEdge(l_pcNode, l_reactionNode, wxT("Edge"),l_stoichiometry);
						if (b_IsReversible && l_revReactionNode && m_CreateReverseReactions)
						{
							drawDpnEdge(l_revReactionNode, l_pcNode,wxT("Edge"),l_stoichiometry);
						}
					}
				}

				// get products of actual reaction
				for (unsigned int lop = 0; lop < products->size() ; ++lop)
				{
					wxString l_productName = wxString(l_sbmlReaction->getProduct(lop)->getSpecies().c_str(), wxConvUTF8);
					if (sId == l_productName)
					{
						wxString l_stoichiometry =
								wxString::Format(wxT("%.0f"),l_sbmlReaction->getProduct(lop)->getStoichiometry());
						drawEdge(l_reactionNode, l_pcNode,wxT("Edge"),l_stoichiometry);

						if (b_IsReversible && l_revReactionNode && m_CreateReverseReactions)
						{
							drawDpnEdge(l_pcNode, l_revReactionNode, wxT("Edge"),l_stoichiometry);
						}
						if(l_sbmlReaction->isSetKineticLaw())
						{
							const ASTNode* l_sbmlMath = l_sbmlReaction->getKineticLaw()->getMath();
							if(existInReactionFormula(l_productName, l_sbmlMath) &&
								!l_reactionNode->IsTargetOf(l_pcNode))
							{
								drawEdge( l_pcNode, l_reactionNode,  wxT("Modifier Edge"), wxT("0"));
							}
						}
					}
				}

				// get modifiers of actual reaction
				for (unsigned int lom = 0; lom < modifiers->size() ; ++lom)
				{
					wxString l_modifierName = wxString(l_sbmlReaction->getModifier(lom)->getSpecies().c_str(), wxConvUTF8);
					if (sId == l_modifierName)
					{
						drawEdge( l_pcNode, l_reactionNode, wxT("Modifier Edge"), wxT("0"));
						if (b_IsReversible && l_revReactionNode && m_CreateReverseReactions)
						{
							drawEdge( l_pcNode, l_reactionNode, wxT("Modifier Edge"), wxT("0"));
						}
					}
				}
			}
		}
    }
}

// get Model Compartments
void SP_ImportSBML2sPn::getModelCompartments()
{
	const ListOfCompartments* compartments = m_sbmlModel->getListOfCompartments();
	for (unsigned int j = 0; j < compartments->size(); ++j)
	{
		// add Constant for Compartment
		SP_DS_Metadataclass* l_pcMC = m_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);

		SP_DS_Metadata* l_constant = l_pcMC->NewElement(1);

		Compartment* l_sbmlCompartment = m_sbmlModel->getCompartment(j);

		wxString l_CompId;
		wxString l_CompName;
		getSBMLCompartmentName(l_sbmlCompartment, l_CompId, l_CompName);

		l_constant->GetAttribute(wxT("Name"))->SetValueString(l_CompId);
		l_constant->GetAttribute(wxT("Group"))->SetValueString(wxT("compartment"));
		l_constant->GetAttribute(wxT("Type"))->SetValueString(wxT("double"));
		if(!l_CompName.IsEmpty())
			l_constant->GetAttribute(wxT("Comment"))->SetValueString(wxT("name: ")+l_CompName);

		wxString l_parameterValue;
		if (l_sbmlCompartment->isSetSize())
		{
			l_parameterValue << l_sbmlCompartment->getSize();
		}
		else
		{
			l_parameterValue = wxT("0");
		}
		SP_DS_ColListAttribute* l_pcColAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_constant->GetAttribute(wxT("ValueList")));
		l_pcColAttr->SetCell(0, 1, l_parameterValue);
	}
}


// get Modelparameters, no multiple parameters expected
void SP_ImportSBML2sPn::getModelParameters()
{
	const ListOfParameters* parameters = m_sbmlModel->getListOfParameters();
	for (unsigned int j = 0; j < parameters->size(); ++j)
	{
		// add Constant for Parameter
		SP_DS_Metadataclass* l_pcMC = m_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);

		SP_DS_Metadata* l_constant = l_pcMC->NewElement(1);

		Parameter* l_sbmlParameter = m_sbmlModel->getParameter(j);

		wxString l_ParamId;
		wxString l_ParamName;
		getSBMLParameterName(l_sbmlParameter, l_ParamId, l_ParamName);

		l_constant->GetAttribute(wxT("Name"))->SetValueString(l_ParamId);
		l_constant->GetAttribute(wxT("Group"))->SetValueString(wxT("parameter"));
		l_constant->GetAttribute(wxT("Type"))->SetValueString(wxT("double"));
		if(!l_ParamName.IsEmpty())
			l_constant->GetAttribute(wxT("Comment"))->SetValueString(wxT("name: ")+l_ParamName);

		if (l_sbmlParameter->isSetValue())
		{
			wxString l_parameterValue;
			l_parameterValue << l_sbmlParameter->getValue();
			SP_DS_ColListAttribute* l_pcColAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_constant->GetAttribute(wxT("ValueList")));
			l_pcColAttr->SetCell(0, 1, l_parameterValue);
		}
	}
}


// get reaction parameters, multiple parameters possible
void SP_ImportSBML2sPn::getReactionParameters(Reaction*  l_sbmlReaction)
{
	ListOfParameters* parameters = l_sbmlReaction->getKineticLaw()->getListOfParameters();
	for (unsigned int j = 0; j < parameters->size(); ++j)
	{
		Parameter* l_sbmlParameter = l_sbmlReaction->getKineticLaw()->getParameter(j);
		// Parameter Name or Id
		wxString l_ParamId;
		wxString l_ParamName;
		getSBMLParameterName(l_sbmlParameter, l_ParamId, l_ParamName);

		// don't add multiple parameters
		if (!existParameterNode(l_ParamId))
		{
			// add Constant for Parameter
			SP_DS_Metadataclass* l_pcMC = m_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);

			SP_DS_Metadata* l_constant = l_pcMC->NewElement(1);

			l_constant->GetAttribute(wxT("Name"))->SetValueString(l_ParamId);
			l_constant->GetAttribute(wxT("Group"))->SetValueString(wxT("parameter"));
			l_constant->GetAttribute(wxT("Type"))->SetValueString(wxT("double"));
			if(!l_ParamName.IsEmpty())
				l_constant->GetAttribute(wxT("Comment"))->SetValueString(wxT("name: ")+l_ParamName);

			if (l_sbmlParameter->isSetValue())
			{
				wxString l_parameterValue;
				l_parameterValue << l_sbmlParameter->getValue();
				SP_DS_ColListAttribute* l_pcColAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_constant->GetAttribute(wxT("ValueList")));
				l_pcColAttr->SetCell(0, 1, l_parameterValue);
			}
			g_ParameterList.push_back(l_constant);
		}
	}
}

bool SP_ImportSBML2sPn::existParameterNode(const wxString& l_parameterName)
{
	SP_ListMetadata::const_iterator pIt;
	pIt = g_ParameterList.begin();
	bool exists = false;
	while (pIt != g_ParameterList.end())
	{
		wxString sId = (*pIt)->GetAttribute(wxT("Name"))->GetValueString();
		if (sId == l_parameterName)
		{
			exists = true;
			break;
		}
		else
		{
			++pIt;
		}
	}
	return exists;
}
