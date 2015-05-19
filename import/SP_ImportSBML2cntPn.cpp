//////////////////////////////////////////////////////////////////////
// $Author: schroed $
// $Version: 0.1 $
// $Date: 2007/02/03 $
// Short Description: creates a continious petrinet on a SBML file
//////////////////////////////////////////////////////////////////////

#include "import/SP_ImportSBML2cntPn.h"

#include "sp_ds/netclasses/SP_DS_ContinuousPed.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"

#include "sp_ds/extensions/SP_DS_FunctionRegistry.h"
#include "sp_ds/extensions/SP_DS_FunctionEvaluator.h"

SP_ImportSBML2cntPn::SP_ImportSBML2cntPn()
{
	yComRea = 40;
	xPara = 1;
	yPara = 1;
}

SP_ImportSBML2cntPn::~SP_ImportSBML2cntPn()
{

}

bool SP_ImportSBML2cntPn::ReadFile(const wxString& p_sFile)
{
	g_CompoundList.clear();
	g_ParameterList.clear();
	g_ReactionList.clear();

	numReverseReactions = 0;

	SBMLDocument* l_sbmlDocument;

	l_sbmlDocument = readSBML(p_sFile.mb_str());
	CHECK_POINTER(l_sbmlDocument, return FALSE);

	m_sbmlModel = l_sbmlDocument->getModel();
	CHECK_POINTER(m_sbmlModel, return FALSE);

	if(ValidateSBML(l_sbmlDocument))
	{
		m_pcGraph = CreateDocument(SP_DS_CONTINUOUSPED_CLASS);

		//SP_MDI_View* l_pcView = dynamic_cast<SP_MDI_View*>(m_pcMyDoc->GetFirstView());

		getModelCompartments();
		// first we have to get model parameters, later the reaction parameters
		getModelParameters();
		getSpecies();
		getReactions();

		m_pcGraph->CreateDeclarationTree()->UpdateOtherTree();

		SP_LOGMESSAGE(wxString::Format(wxT("The imported SBML contains %u reversible reaction(s)."), numReverseReactions));

		DoVisualization();

		SP_MESSAGEBOX(wxT("The SBML import is experimental!\nPlease note: rules, events and functions are not supported yet.\nPlease check the result!"),wxT("Notice"), wxOK | wxICON_INFORMATION);

		m_pcMyDoc->Modify(true);
	}
	wxDELETE(l_sbmlDocument);
	return true;
}


void SP_ImportSBML2cntPn::getSpecies()
{
    for (unsigned int n = 0; n < m_sbmlModel->getNumSpecies() ; ++n)
    {
		Species* l_sbmlSpecies = m_sbmlModel->getSpecies(n);

		wxString l_speciesId;
		wxString l_speciesName;

		getSBMLSpeciesName(l_sbmlSpecies, l_speciesId, l_speciesName);

		// Species _void_ is only for Gepasi, don't import
		if (l_speciesName != wxT("_void_"))
		{
			SP_DS_Nodeclass* contPlaceClass = m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_PLACE);
			SP_DS_Node* l_pcNode = contPlaceClass->NewElement(m_pcCanvas->GetNetnumber());
			if (l_pcNode)
			{
				++yComRea;
				SP_DS_Attribute* l_pcAttrName = l_pcNode->GetAttribute(wxT("Name"));
				l_pcAttrName->SetValueString(l_speciesId);
				l_pcAttrName->SetShow(true);

				SP_DS_Attribute* l_pcAttrComment = l_pcNode->GetAttribute(wxT("Comment"));
				wxString l_comment = l_pcAttrComment->GetValueString();

				wxString l_sCompartment = wxString(l_sbmlSpecies->getCompartment().c_str(), wxConvUTF8);
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

				if(l_sbmlSpecies->isSetInitialConcentration())
				{
					wxString l_sMarking;
					l_sMarking << l_sbmlSpecies->getInitialConcentration();
					l_pcNode->GetAttribute(wxT("Marking"))->SetValueString(l_sMarking);
				}
				else if (l_sbmlSpecies->isSetInitialAmount())
				{
					wxString l_sMarking;
					l_sMarking << l_sbmlSpecies->getInitialAmount();
					l_pcNode->GetAttribute(wxT("Marking"))->SetValueString(l_sMarking);
				}

				// if set BoundaryCondition (0,1 for false,true) or 0 for default (false)
				l_pcNode->GetAttribute(wxT("Fixed"))->SetValueString(wxString::Format(wxT("%d"), l_sbmlSpecies->getBoundaryCondition()));
				l_pcNode->ShowOnCanvas(m_pcCanvas, FALSE, 50, yComRea,0);

				g_CompoundList.push_back(l_pcNode);
			}
		}
    }
}


// get reactions, rectants, products, modifiers and kineticlaw
void SP_ImportSBML2cntPn::getReactions ()
{
	// reads the Reactions from the SBML File
	yComRea = 40;
	SP_DS_Nodeclass* contTransitionClass = m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_TRANS);
	for (unsigned int i = 0; i < m_sbmlModel->getNumReactions() ; ++i)
    {
		Reaction*    l_sbmlReaction;
		l_sbmlReaction = m_sbmlModel->getReaction(i);

		wxString l_ReactionId;
		wxString l_ReactionName;
		getSBMLReactionName(l_sbmlReaction, l_ReactionId, l_ReactionName);

		SP_DS_Node* l_reactionNode = contTransitionClass->NewElement(m_pcCanvas->GetNetnumber());
		if (l_reactionNode)
		{
			++yComRea;
			SP_DS_Attribute* l_pcAttrName = l_reactionNode->GetAttribute(wxT("Name"));
			l_pcAttrName->SetValueString(l_ReactionId);
			l_pcAttrName->SetShow(true);

			SP_DS_Attribute* l_pcAttrComment = l_reactionNode->GetAttribute(wxT("Comment"));
			wxString l_comment = l_pcAttrComment->GetValueString();
			l_pcAttrComment->SetValueString(l_comment);
			l_pcAttrComment->SetShow(false);

			// is reversible (0,1 for false,true) or 0 for default (false)
			bool b_IsReversible = l_sbmlReaction->getReversible();

			if(b_IsReversible)
			{
				++numReverseReactions;
			}

			l_reactionNode->GetAttribute(wxT("Reversible"))->SetValueString(wxString::Format(wxT("%d"), b_IsReversible));

			// get KineticLaw
			if (l_sbmlReaction->isSetKineticLaw())
			{
				KineticLaw* l_sbmlKineticLaw =  l_sbmlReaction->getKineticLaw();
				ASTNode* l_sbmlMath = l_sbmlKineticLaw->getMath()->deepCopy();

				wxString l_kinetic =  formulaToString(getSBMLFormula(l_sbmlMath));
				SP_DS_ColListAttribute* l_pcColAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_reactionNode->GetAttribute(wxT("FunctionList")));
				l_pcColAttr->SetCell(0, 1, l_kinetic);

				wxDELETE(l_sbmlMath);

				getReactionParameters(l_sbmlReaction);
			}

			l_reactionNode->ShowOnCanvas(m_pcCanvas, FALSE, 100, yComRea, 0);

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
					auto l_sbmlReactant = l_sbmlReaction->getReactant(lor);
					wxString l_eductName = l_sbmlReactant->getSpecies();
					if (sId == l_eductName)
					{
						wxString l_stoichiometry =
								wxString::Format(wxT("%g"),l_sbmlReactant->getStoichiometry());
						drawEdge(l_pcNode, l_reactionNode,SP_DS_EDGE,l_stoichiometry);
						if(l_sbmlReactant->isSetConstant())
						{
							drawEdge(l_reactionNode,l_pcNode,SP_DS_EDGE,l_stoichiometry);
						}
					}
				}

				// get products of actual reaction
				for (unsigned int lop = 0; lop < products->size() ; ++lop)
				{
					auto l_sbmlProduct = l_sbmlReaction->getProduct(lop);
					wxString l_productName = l_sbmlProduct->getSpecies();
					if (sId == l_productName)
					{
						wxString l_stoichiometry =
								wxString::Format(wxT("%g"),l_sbmlProduct->getStoichiometry());
						drawEdge(l_reactionNode, l_pcNode,SP_DS_EDGE,l_stoichiometry);
						if(l_sbmlProduct->isSetConstant())
						{
							drawEdge(l_reactionNode,l_pcNode,SP_DS_EDGE,l_stoichiometry);
						}
						if(l_sbmlReaction->isSetKineticLaw())
						{
							const ASTNode* l_sbmlMath = l_sbmlReaction->getKineticLaw()->getMath();
							if(existInReactionFormula(l_productName, l_sbmlMath) &&
								!l_reactionNode->IsTargetOf(l_pcNode))
							{
								drawEdge( l_pcNode, l_reactionNode, SP_DS_MODIFIER_EDGE, wxT("0"));
							}
						}
					}
				}

				// get modifiers of actual reaction
				for (unsigned int lom = 0; lom < modifiers->size() ; ++lom)
				{
					auto l_sbmlModifier = l_sbmlReaction->getModifier(lom);
					wxString l_modifierName = l_sbmlModifier->getSpecies();
					if (sId == l_modifierName)
					{
						wxString l_stoichiometry = wxT("0"); // no info about in sbml
						drawEdge( l_pcNode, l_reactionNode, SP_DS_MODIFIER_EDGE, l_stoichiometry);
					}
				}
			}
		}
    }
}

// get Model Compartments
void SP_ImportSBML2cntPn::getModelCompartments()
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

		SP_FunctionPtr l_pcName = m_pcGraph->GetFunctionRegistry()->parseFunctionString(l_CompId);
		SP_FunctionPtr l_pcValue = m_pcGraph->GetFunctionRegistry()->parseFunctionString(l_parameterValue);
		m_pcGraph->GetFunctionRegistry()->registerFunction(l_pcName, l_pcValue);

		g_ParameterList.push_back(l_constant);
	}
}


// get Modelparameters, no multiple parameters expected
void SP_ImportSBML2cntPn::getModelParameters()
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

		wxString l_parameterValue;
		if (l_sbmlParameter->isSetValue())
		{
			l_parameterValue << l_sbmlParameter->getValue();
		}
		else
		{
			l_parameterValue = wxT("0");
		}
		SP_DS_ColListAttribute* l_pcColAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_constant->GetAttribute(wxT("ValueList")));
		l_pcColAttr->SetCell(0, 1, l_parameterValue);

		SP_FunctionPtr l_pcName = m_pcGraph->GetFunctionRegistry()->parseFunctionString(l_ParamId);
		SP_FunctionPtr l_pcValue = m_pcGraph->GetFunctionRegistry()->parseFunctionString(l_parameterValue);
		m_pcGraph->GetFunctionRegistry()->registerFunction(l_pcName, l_pcValue);

		g_ParameterList.push_back(l_constant);
	}
}


// get reaction parameters, multiple parameters possible
void SP_ImportSBML2cntPn::getReactionParameters(Reaction*  l_sbmlReaction)
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

			wxString l_parameterValue;
			if (l_sbmlParameter->isSetValue())
			{
				l_parameterValue << l_sbmlParameter->getValue();
			}
			else
			{
				l_parameterValue = wxT("0");
			}
			SP_DS_ColListAttribute* l_pcColAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_constant->GetAttribute(wxT("ValueList")));
			l_pcColAttr->SetCell(0, 1, l_parameterValue);

			SP_FunctionPtr l_pcName = m_pcGraph->GetFunctionRegistry()->parseFunctionString(l_ParamId);
			SP_FunctionPtr l_pcValue = m_pcGraph->GetFunctionRegistry()->parseFunctionString(l_parameterValue);
			m_pcGraph->GetFunctionRegistry()->registerFunction(l_pcName, l_pcValue);

			g_ParameterList.push_back(l_constant);
		}
	}
}

bool SP_ImportSBML2cntPn::existParameterNode(const wxString& l_parameterName)
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

