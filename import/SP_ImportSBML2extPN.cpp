//////////////////////////////////////////////////////////////////////
// $Author: schroed $
// $Version: 0.2 $
// $Date: 2007/03/27 $
// Short Description: creates a extended (discrete) petrinet on a SBML file
//////////////////////////////////////////////////////////////////////


#include "import/SP_ImportSBML2extPN.h"
#include "sp_gui/dialogs/SP_DLG_ImportSBML2extPN.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"

SP_ImportSBML2extPN::SP_ImportSBML2extPN()
{
	yComRea = 40;
	xPara = 4;
	yPara = 1;

}

SP_ImportSBML2extPN::~SP_ImportSBML2extPN()
{

}

bool SP_ImportSBML2extPN::ReadFile(const wxString& p_sFile)
{
	g_CompoundList.clear();
	g_ReactionList.clear();

	numReverseReactions = 0;

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
			m_pcGraph = CreateDocument(SP_DS_EXTPN_CLASS);

			getModelCompartments();
			getSpecies();
			getReactions();

			SP_LOGMESSAGE(wxString::Format(wxT("The imported SBML contains %u reversible reaction(s)."), numReverseReactions));

			DoVisualization();

			SP_MESSAGEBOX(wxT("The SBML import is experimental!\nPlease note: rules, events and functions are not supported yet.\nPlease check the result!"),wxT("Notice"), wxOK | wxICON_INFORMATION);

			m_pcMyDoc->Modify(true);
		}
		wxDELETE(l_sbmlDocument);
		l_cDlg.Destroy();
		return true;
	}
	l_cDlg.Destroy();
	return false;
}

// get Model Compartments
void SP_ImportSBML2extPN::getModelCompartments()
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
		l_constant->GetAttribute(wxT("Type"))->SetValueString(wxT("int"));
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

void SP_ImportSBML2extPN::getSpecies()
{
	SP_DS_Nodeclass* placeClass = m_pcGraph->GetNodeclass(wxT("Place"));
	for (unsigned int n = 0; n < m_sbmlModel->getNumSpecies() ; ++n)
    {
		Species* l_sbmlSpecies = m_sbmlModel->getSpecies(n);

		wxString l_speciesId;
		wxString l_speciesName;

		getSBMLSpeciesName(l_sbmlSpecies, l_speciesId, l_speciesName);

		// Species _void_ is only for Gepasi, don't import
		if (l_speciesName != wxT("_void_"))
		{
			SP_DS_Node* l_compoundNode = placeClass->NewElement(m_pcCanvas->GetNetnumber());
			if (l_compoundNode)
			{
				++yComRea;
				SP_DS_Attribute* l_pcAttrName = l_compoundNode->GetAttribute(wxT("Name"));
				l_pcAttrName->SetValueString(l_speciesId);
				l_pcAttrName->SetShow(true);

				SP_DS_Attribute* l_pcAttrComment = l_compoundNode->GetAttribute(wxT("Comment"));
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

				l_compoundNode->ShowOnCanvas(m_pcCanvas, FALSE, 50, yComRea,0);
				if (l_sbmlSpecies->isSetInitialAmount())
				{
					wxString l_sMarking = wxString::Format(wxT("%.0f"),l_sbmlSpecies->getInitialAmount());
					l_compoundNode->GetAttribute(wxT("Marking"))->SetValueString(l_sMarking);
				}
				else if(l_sbmlSpecies->isSetInitialConcentration())
				{
					wxString l_sMarking = wxString::Format(wxT("%.0f"),l_sbmlSpecies->getInitialConcentration());
					l_compoundNode->GetAttribute(wxT("Marking"))->SetValueString(l_sMarking);
				}

				g_CompoundList.push_back(l_compoundNode);
			}
		}
    }
}

// get reactions, rectants, products, modifiers and kineticlaw
void SP_ImportSBML2extPN::getReactions ()
{
	// reads the Reactions from the SBML File
	yComRea = 40;
 	SP_DS_Nodeclass* extTransitionClass = m_pcGraph->GetNodeclass(wxT("Transition"));
	for (unsigned int i = 0; i < m_sbmlModel->getNumReactions() ; ++i)
    {
		Reaction* l_sbmlReaction = m_sbmlModel->getReaction(i);

		wxString l_ReactionId;
		wxString l_ReactionName;
		getSBMLReactionName(l_sbmlReaction, l_ReactionId, l_ReactionName);

		SP_DS_Node* l_reactionNode = extTransitionClass->NewElement(m_pcCanvas->GetNetnumber());
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

			if(l_sbmlReaction->isSetKineticLaw())
			{
				wxString l_kinetic(l_sbmlReaction->getKineticLaw()->getFormula().c_str(), wxConvUTF8);
				l_comment << wxT("rate: ") << l_kinetic << wxT("\n");
			}
			l_pcAttrComment->SetValueString(l_comment);
			l_pcAttrComment->SetShow(false);

			// is reversible (0,1 for false,true) or 0 for default (false)
			bool b_IsReversible = l_sbmlReaction->getReversible();

			if(b_IsReversible)
			{
				++numReverseReactions;
			}

			if(b_IsReversible && m_HighlightReversibleReactions)
			{
				l_reactionNode->GetGraphics()->front()->SetBrushColour(*wxBLUE);
			}

			l_reactionNode->ShowOnCanvas(m_pcCanvas, FALSE, 100, yComRea, 0);

			if (b_IsReversible && m_CreateReverseReactions)
			{
				++yComRea;
				l_revReactionNode = extTransitionClass->NewElement(m_pcCanvas->GetNetnumber());
				l_revReactionNode->GetAttribute(wxT("Name"))->SetValueString(wxT("re_")+l_ReactionId);
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
						drawDpnEdge(l_pcNode, l_reactionNode,wxT("Edge"),l_stoichiometry);
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
						drawDpnEdge(l_reactionNode, l_pcNode,wxT("Edge"),l_stoichiometry);
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
								drawEdge( l_pcNode, l_reactionNode, wxT("Read Edge"), wxT("0"));
								wxString l_sText = wxT("The semantics of reaction \"") + l_ReactionName +
									wxT("\" is defined by it's kinetic law\nand there is no known way to decide the structure in general.\n") +
									wxT("Please check the resulting transition.");
								SP_LOGWARNING( l_sText);
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
						wxString l_stoichiometry = wxT("0"); // no info about in sbml
						drawDpnEdge( l_pcNode, l_reactionNode, wxT("Read Edge"), l_stoichiometry);
						if (b_IsReversible && l_revReactionNode && m_CreateReverseReactions)
						{
							drawDpnEdge(l_pcNode, l_revReactionNode,wxT("Read Edge"),l_stoichiometry);
						}
					}
				}
			}
		}
    }
}

