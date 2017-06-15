//////////////////////////////////////////////////////////////////////
// $Author: schroed $
// $Version: 0.1 $
// $Date: 2007/02/03 $
// Short Description: creates a continious petrinet on a SBML file
//////////////////////////////////////////////////////////////////////

#include <dssd/auxi/auxi.h>
#include "import/SP_ImportSBML2cntPn.h"

#include "sp_gui/dialogs/SP_DLG_ImportSBML2extPN.h"
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
	m_Species.clear();
	m_Reactions.clear();

	numReverseReactions = 0;
	numBoundaryConditions = 0;

	SBMLDocument* l_sbmlDocument;

	SP_DLG_ImportSBML2extPN l_cDlg(NULL);

	if (l_cDlg.ShowModal() ==  wxID_OK) {
		m_CreateReverseReactions = l_cDlg.GetCreateReverseReactions();
		m_HighlightReversibleReactions = l_cDlg.GetHighlightReverseReactions();
		m_CreateBoundaryConditions = l_cDlg.GetCreateBoundaryConditions();
		m_NormalizeStoichiometries = l_cDlg.GetNormalizeStoichiometries();


		l_sbmlDocument = readSBML(p_sFile.mb_str());
		CHECK_POINTER(l_sbmlDocument, return FALSE);

		if (ValidateSBML(l_sbmlDocument)) {
			m_sbmlModel = l_sbmlDocument->getModel();
			CHECK_POINTER(m_sbmlModel, return FALSE);

			m_pcGraph = CreateDocument(SP_DS_CONTINUOUSPED_CLASS);

			getModelDescription();
			getModelCompartments();
            getFunctionDefinitions();
			// first we have to get model parameters, later the reaction parameters
			getModelParameters();
			getSpecies();
			getReactions();

            SP_DS_FunctionRegistry* l_pcFR = m_pcGraph->GetFunctionRegistry();
            l_pcFR->LoadFromNet(m_pcGraph);
			m_pcGraph->CreateDeclarationTree()->UpdateOtherTree();

			SP_LOGMESSAGE(wxString::Format(
					wxT("The imported SBML contains:\n\t - %u boundary condition(s),\n\t - %u reversible reaction(s)."),
					numBoundaryConditions, numReverseReactions));

			DoVisualization();

			SP_MESSAGEBOX(
					wxT("The SBML import is experimental!\nPlease note: rules, events and functions are not supported yet.\nPlease check the result!"),
					wxT("Notice"), wxOK | wxICON_INFORMATION);

			m_pcMyDoc->Modify(true);

			wxDELETE(l_sbmlDocument);
			return true;
		} else {
			wxDELETE(l_sbmlDocument);
			return false;
		}
	}
    return false;
}

// get Model Description
void SP_ImportSBML2cntPn::getModelDescription()
{
	// add Constant for Compartment
	SP_DS_Metadataclass* l_pcMC = m_pcGraph->GetMetadataclass(wxT("General"));

	SP_DS_Metadata* l_general = l_pcMC->GetElements()->front();

	wxString l_Id = m_sbmlModel->getId();

	l_general->GetAttribute(wxT("Name"))->SetValueString(l_Id);

	SP_DS_Attribute* l_pcAttrComment = l_general->GetAttribute(wxT("Description"));
	wxString description = l_pcAttrComment->GetValueString();

	wxString l_Name;
	if(m_sbmlModel->isSetName())
	{
		l_Name = wxT("name=\"") + m_sbmlModel->getName() + wxT("\"\n");
	}
	wxString l_metaid;
	if(m_sbmlModel->isSetMetaId())
	{
		l_metaid << wxT("metaid=\"") << m_sbmlModel->getMetaId() << wxT("\"\n");
	}

	wxString l_Level = wxT("level=\"") + wxString::Format(wxT("%u"), m_sbmlModel->getLevel()) + wxT("\"\n");
	wxString l_Version = wxT("version=\"") + wxString::Format(wxT("%u"), m_sbmlModel->getVersion()) + wxT("\"\n");

	wxString l_sNotes;
	if(m_sbmlModel->isSetNotes())
	{
		l_sNotes = m_sbmlModel->getNotesString()+wxT("\n");
	}

	wxString l_sAnnotation;
	if(m_sbmlModel->isSetAnnotation())
	{
		l_sAnnotation = m_sbmlModel->getAnnotationString()+wxT("\n");
	}

	l_pcAttrComment->SetValueString(l_Name+l_metaid+l_Level+l_Version+l_sNotes+l_sAnnotation);
	l_pcAttrComment->SetShow(false);
}

void SP_ImportSBML2cntPn::getSpecies()
{
    SP_DS_Nodeclass* contPlaceClass = m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_PLACE);
    for (unsigned int n = 0; n < m_sbmlModel->getNumSpecies() ; ++n)
    {
		Species* l_sbmlSpecies = m_sbmlModel->getSpecies(n);

		wxString l_speciesId;
		wxString l_speciesName;

        getSBMLName(l_sbmlSpecies, l_speciesId, l_speciesName);

		// Species _void_ is only for Gepasi, don't import
		if (l_speciesName != wxT("_void_"))
		{
			SP_DS_Node* l_pcNode = contPlaceClass->NewElement(m_pcCanvas->GetNetnumber());
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
					l_comment << wxT("compartment=\"") << l_sCompartment << wxT("\"\n");
				}
				if(!l_speciesName.IsEmpty())
				{
					l_comment << wxT("name=\"") << l_speciesName << wxT("\"\n");
				}
				if(l_sbmlSpecies->isSetMetaId())
				{
					l_comment << wxT("metaid=\"") << l_sbmlSpecies->getMetaId() << wxT("\"\n");
				}
				if(l_sbmlSpecies->isSetNotes())
				{
					l_comment << l_sbmlSpecies->getNotesString()+wxT("\n");
				}
				if(l_sbmlSpecies->isSetAnnotation())
				{
					l_comment << l_sbmlSpecies->getAnnotationString()+wxT("\n");
				}
				l_pcAttrComment->SetValueString(l_comment);
				l_pcAttrComment->SetShow(false);

				if(l_sbmlSpecies->isSetInitialConcentration())
				{
					wxString l_sMarking = dssd::aux::toString(l_sbmlSpecies->getInitialConcentration());
					l_pcNode->GetAttribute(wxT("Marking"))->SetValueString(l_sMarking);
				}
				else if (l_sbmlSpecies->isSetInitialAmount())
				{
					wxString l_sMarking = dssd::aux::toString(l_sbmlSpecies->getInitialAmount());
					l_pcNode->GetAttribute(wxT("Marking"))->SetValueString(l_sMarking);
				}

				// if set BoundaryCondition (0,1 for false,true) or 0 for default (false)
				numBoundaryConditions += l_sbmlSpecies->getBoundaryCondition();
				if(m_CreateBoundaryConditions
				   && l_sbmlSpecies->getBoundaryCondition())
				{
					SP_DS_Nodeclass* contTransitionClass = m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_TRANS);
					SP_DS_Node* l_inNode = contTransitionClass->NewElement(m_pcCanvas->GetNetnumber());
					l_inNode->GetAttribute(wxT("Name"))->SetValueString(wxT("in_")+l_speciesId);
					l_inNode->GetGraphics()->front()->SetBrushColour(*wxGREEN);
					l_inNode->ShowOnCanvas(m_pcCanvas, FALSE, 100, yComRea, 0);
					drawEdge(l_inNode,l_pcNode,wxT("Edge"),wxT("1"));

					SP_DS_Node* l_outNode = contTransitionClass->NewElement(m_pcCanvas->GetNetnumber());
					l_outNode->GetAttribute(wxT("Name"))->SetValueString(wxT("out_")+l_speciesId);
					l_outNode->GetGraphics()->front()->SetBrushColour(*wxGREEN);
					l_outNode->ShowOnCanvas(m_pcCanvas, FALSE, 100, yComRea, 0);
					drawEdge(l_pcNode,l_outNode,wxT("Edge"),wxT("1"));
                }
                else
                {
                    l_pcNode->GetAttribute(wxT("Fixed"))->SetValueString(wxString::Format(wxT("%d"), l_sbmlSpecies->getBoundaryCondition() || l_sbmlSpecies->getConstant()));
                }
				l_pcNode->ShowOnCanvas(m_pcCanvas, FALSE, 50, yComRea,0);

				m_Species.insert(std::make_pair(l_speciesId,l_pcNode));
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
        getSBMLName(l_sbmlReaction, l_ReactionId, l_ReactionName);

		SP_DS_Node* l_reactionNode = contTransitionClass->NewElement(m_pcCanvas->GetNetnumber());
		SP_DS_Node* l_revReactionNode = NULL;
		if (l_reactionNode)
		{
			++yComRea;
			SP_DS_Attribute* l_pcAttrName = l_reactionNode->GetAttribute(wxT("Name"));
			l_pcAttrName->SetValueString(l_ReactionId);
			l_pcAttrName->SetShow(true);

			SP_DS_Attribute* l_pcAttrComment = l_reactionNode->GetAttribute(wxT("Comment"));

			if(!l_ReactionName.IsEmpty())
			{
				l_ReactionName = wxT("name=\"") + l_ReactionName + wxT("\"\n");
			}

			wxString l_metaid;
			if(l_sbmlReaction->isSetMetaId())
			{
				l_metaid << wxT("metaid=\"") << l_sbmlReaction->getMetaId() << wxT("\"\n");
			}

			wxString l_sNotes;
			if(l_sbmlReaction->isSetNotes())
			{
				l_sNotes = l_sbmlReaction->getNotesString()+wxT("\n");
			}

			wxString l_sAnnotation;
			if(l_sbmlReaction->isSetAnnotation())
			{
				l_sAnnotation = l_sbmlReaction->getAnnotationString()+wxT("\n");
			}

			// get KineticLaw
			wxString l_kinetic;
			if (l_sbmlReaction->isSetKineticLaw())
			{
				KineticLaw* l_sbmlKineticLaw =  l_sbmlReaction->getKineticLaw();
				ASTNode* l_sbmlMath = nullptr;
				if(l_sbmlKineticLaw->getMath())
				{
					l_sbmlMath = l_sbmlKineticLaw->getMath()->deepCopy();
				}
				//position is important, because we change id of reaction parameters
				getReactionParameters(l_sbmlReaction, l_sbmlMath);

				l_kinetic =  formulaToString(getSBMLFormula(l_sbmlMath));
				if(!l_kinetic.IsEmpty())
				{
					SP_DS_ColListAttribute* l_pcColAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_reactionNode->GetAttribute(wxT("FunctionList")));
					l_pcColAttr->SetCell(0, 1, l_kinetic);
				}
				wxDELETE(l_sbmlMath);
			}

			// is reversible (0,1 for false,true) or 0 for default (false)
			bool b_IsReversible = l_sbmlReaction->getReversible();
			wxString l_sReversible = wxT("reversible=\"false\"\n");

			if(b_IsReversible)
			{
				++numReverseReactions;
				l_sReversible = wxT("reversible=\"true\"\n");
			}

			if(b_IsReversible && m_HighlightReversibleReactions)
			{
				l_reactionNode->GetGraphics()->front()->SetBrushColour(*wxBLUE);
			}

			if (b_IsReversible && m_CreateReverseReactions)
			{
				++yComRea;
				l_revReactionNode = contTransitionClass->NewElement(m_pcCanvas->GetNetnumber());
				l_revReactionNode->GetAttribute(wxT("Name"))->SetValueString(wxT("re_")+l_ReactionId);
				l_revReactionNode->GetAttribute(wxT("Name"))->SetShow(TRUE);
				SP_DS_ColListAttribute* l_pcFuncAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_revReactionNode->GetAttribute(wxT("FunctionList")));
				//set ratefunction the same as reversible transition, because we dont know it
				if(!l_kinetic.IsEmpty())
				{
					l_pcFuncAttr->SetCell(0,1, l_kinetic);
				}
				l_revReactionNode->GetGraphics()->front()->SetBrushColour(*wxRED);
				SP_DS_Attribute* l_pcAttrRevComment = l_revReactionNode->GetAttribute(wxT("Comment"));
				l_pcAttrRevComment->SetValueString(l_sReversible);
				l_pcAttrRevComment->SetShow(false);
				l_revReactionNode->ShowOnCanvas(m_pcCanvas, FALSE, 100, yComRea, 0);
			}
			else
			{
				l_reactionNode->GetAttribute(wxT("Reversible"))->SetValueString(wxString::Format(wxT("%d"), b_IsReversible));
			}

			l_pcAttrComment->SetValueString(l_ReactionName+l_metaid+l_sReversible+l_sNotes+l_sAnnotation);
			l_pcAttrComment->SetShow(false);

			l_reactionNode->ShowOnCanvas(m_pcCanvas, FALSE, 100, yComRea, 0);

		 	// get reactants, products and modifiers
			const ListOfSpeciesReferences* rectants = l_sbmlReaction->getListOfReactants();
			const ListOfSpeciesReferences* products = l_sbmlReaction->getListOfProducts();
			const ListOfSpeciesReferences* modifiers = l_sbmlReaction->getListOfModifiers();

			// get reactants of actual reaction
			for (unsigned int lor = 0; lor < rectants->size() ; ++lor)
			{
				auto l_sbmlReactant = l_sbmlReaction->getReactant(lor);
				wxString l_eductName = l_sbmlReactant->getSpecies();
				auto l_sbmlSpecies = m_sbmlModel->getSpecies(l_eductName);
				SP_DS_Node* l_pcNode = getSpeciesNode(l_eductName);

				wxString l_stoichiometry = dssd::aux::toString(l_sbmlReactant->getStoichiometry());
				drawEdge(l_pcNode,l_reactionNode,SP_DS_EDGE,l_stoichiometry);
				if (b_IsReversible && l_revReactionNode && m_CreateReverseReactions)
				{
					drawEdge(l_revReactionNode, l_pcNode, SP_DS_EDGE, l_stoichiometry);
                    drawEdge(l_pcNode, l_revReactionNode, SP_DS_MODIFIER_EDGE, wxT("0"));
				}
			}

			// get products of actual reaction
			for (unsigned int lop = 0; lop < products->size() ; ++lop)
			{
				auto l_sbmlProduct = l_sbmlReaction->getProduct(lop);
				wxString l_productName = l_sbmlProduct->getSpecies();
				auto l_sbmlSpecies = m_sbmlModel->getSpecies(l_productName);
				SP_DS_Node* l_pcNode = getSpeciesNode(l_productName);

				wxString l_stoichiometry = dssd::aux::toString(l_sbmlProduct->getStoichiometry());
				drawEdge(l_reactionNode, l_pcNode,SP_DS_EDGE,l_stoichiometry);
				if (b_IsReversible && l_revReactionNode && m_CreateReverseReactions)
				{
					drawEdge(l_pcNode,l_revReactionNode, SP_DS_EDGE, l_stoichiometry);
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

			// get modifiers of actual reaction
			for (unsigned int lom = 0; lom < modifiers->size() ; ++lom)
			{
				auto l_sbmlModifier = l_sbmlReaction->getModifier(lom);
				wxString l_modifierName = l_sbmlModifier->getSpecies();
				SP_DS_Node* l_pcNode = getSpeciesNode(l_modifierName);

                if(!l_reactionNode->IsTargetOf(l_pcNode))
                {
                    wxString l_stoichiometry = wxT("0"); // no info about in sbml
                    drawEdge(l_pcNode, l_reactionNode, SP_DS_MODIFIER_EDGE, l_stoichiometry);
                    if (b_IsReversible && l_revReactionNode && m_CreateReverseReactions)
                    {
                        drawEdge(l_pcNode, l_revReactionNode, SP_DS_MODIFIER_EDGE, l_stoichiometry);
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
        getSBMLName(l_sbmlCompartment, l_CompId, l_CompName);

		l_constant->GetAttribute(wxT("Name"))->SetValueString(l_CompId);
		l_constant->GetAttribute(wxT("Group"))->SetValueString(wxT("compartment"));
		l_constant->GetAttribute(wxT("Type"))->SetValueString(wxT("double"));

		SP_DS_Attribute* l_pcAttrComment = l_constant->GetAttribute(wxT("Comment"));

		if(!l_CompName.IsEmpty())
		{
			l_CompName = wxT("name=\"") + l_CompName + wxT("\"\n");
		}

		wxString l_metaid;
		if(l_sbmlCompartment->isSetMetaId())
		{
			l_metaid << wxT("metaid=\"") << l_sbmlCompartment->getMetaId() << wxT("\"\n");
		}

		wxString l_sNotes;
		if(l_sbmlCompartment->isSetNotes())
		{
			l_sNotes = l_sbmlCompartment->getNotesString()+wxT("\n");
		}

		wxString l_sAnnotation;
		if(l_sbmlCompartment->isSetAnnotation())
		{
			l_sAnnotation = l_sbmlCompartment->getAnnotationString()+wxT("\n");
		}

		l_pcAttrComment->SetValueString(l_CompName+l_metaid+l_sNotes+l_sAnnotation);
		l_pcAttrComment->SetShow(false);
		wxString l_parameterValue;
		if (l_sbmlCompartment->isSetSize())
		{
			l_parameterValue = dssd::aux::toString(l_sbmlCompartment->getSize());
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
        getSBMLName(l_sbmlParameter, l_ParamId, l_ParamName);

		l_constant->GetAttribute(wxT("Name"))->SetValueString(l_ParamId);
		l_constant->GetAttribute(wxT("Group"))->SetValueString(wxT("parameter"));
		l_constant->GetAttribute(wxT("Type"))->SetValueString(wxT("double"));

		SP_DS_Attribute* l_pcAttrComment = l_constant->GetAttribute(wxT("Comment"));

		if(!l_ParamName.IsEmpty())
		{
			l_ParamName = wxT("name=\"") + l_ParamName + wxT("\"\n");
		}

		wxString l_metaid;
		if(l_sbmlParameter->isSetMetaId())
		{
			l_metaid << wxT("metaid=\"") << l_sbmlParameter->getMetaId() << wxT("\"\n");
		}

		wxString l_sNotes;
		if(l_sbmlParameter->isSetNotes())
		{
			l_sNotes = l_sbmlParameter->getNotesString()+wxT("\n");
		}

		wxString l_sAnnotation;
		if(l_sbmlParameter->isSetAnnotation())
		{
			l_sAnnotation = l_sbmlParameter->getAnnotationString()+wxT("\n");
		}

		l_pcAttrComment->SetValueString(l_ParamName+l_metaid+l_sNotes+l_sAnnotation);
		l_pcAttrComment->SetShow(false);

		wxString l_parameterValue;
		if (l_sbmlParameter->isSetValue())
		{
            l_parameterValue = dssd::aux::toString(l_sbmlParameter->getValue());
		}
		else
		{
			l_parameterValue = wxT("0");
		}
		SP_DS_ColListAttribute* l_pcColAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_constant->GetAttribute(wxT("ValueList")));
		l_pcColAttr->SetCell(0, 1, l_parameterValue);
	}
}


// get reaction parameters, multiple parameters possible
void SP_ImportSBML2cntPn::getReactionParameters(Reaction*  l_sbmlReaction, ASTNode* l_sbmlMath)
{
	wxString l_ReactionId = l_sbmlReaction->getId();

	ListOfParameters* parameters = l_sbmlReaction->getKineticLaw()->getListOfParameters();
	for (unsigned int j = 0; j < parameters->size(); ++j)
	{
		Parameter* l_sbmlParameter = l_sbmlReaction->getKineticLaw()->getParameter(j);
		// Parameter Name or Id
		wxString l_ParamId;
		wxString l_ParamName;
        getSBMLName(l_sbmlParameter, l_ParamId, l_ParamName);

		wxString l_ParamIdNew = l_ReactionId+wxT("_")+l_ParamId;
		changeInReactionFormula(l_ParamId, l_ParamIdNew, l_sbmlMath);

		// add Constant for Parameter
		SP_DS_Metadataclass* l_pcMC = m_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);

		SP_DS_Metadata* l_constant = l_pcMC->NewElement(1);

		l_constant->GetAttribute(wxT("Name"))->SetValueString(l_ParamIdNew);
		l_constant->GetAttribute(wxT("Group"))->SetValueString(wxT("parameter"));
		l_constant->GetAttribute(wxT("Type"))->SetValueString(wxT("double"));

		SP_DS_Attribute* l_pcAttrComment = l_constant->GetAttribute(wxT("Comment"));

		if(!l_ParamName.IsEmpty())
		{
			l_ParamName = wxT("name=\"") + l_ParamName + wxT("\"\n");
		}

		wxString l_metaid;
		if(l_sbmlParameter->isSetMetaId())
		{
			l_metaid << wxT("metaid=\"") << l_sbmlParameter->getMetaId() << wxT("\"\n");
		}

		wxString l_sNotes;
		if(l_sbmlParameter->isSetNotes())
		{
			l_sNotes = l_sbmlParameter->getNotesString()+wxT("\n");
		}

		wxString l_sAnnotation;
		if(l_sbmlParameter->isSetAnnotation())
		{
			l_sAnnotation = l_sbmlParameter->getAnnotationString()+wxT("\n");
		}

		l_pcAttrComment->SetValueString(l_ParamName+l_metaid+l_sNotes+l_sAnnotation);
		l_pcAttrComment->SetShow(false);

		wxString l_parameterValue;
		if (l_sbmlParameter->isSetValue())
		{
			l_parameterValue = dssd::aux::toString(l_sbmlParameter->getValue());
        }
		else
		{
			l_parameterValue = wxT("0");
		}
		SP_DS_ColListAttribute* l_pcColAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_constant->GetAttribute(wxT("ValueList")));
		l_pcColAttr->SetCell(0, 1, l_parameterValue);
	}
}

void SP_ImportSBML2cntPn::getFunctionDefinitions() {
    auto l_sbmlFunctions = m_sbmlModel->getListOfFunctionDefinitions();

    for (unsigned int i = 0; i < l_sbmlFunctions->size(); ++i) {
        auto l_sbmlFunction = l_sbmlFunctions->get(i);

        // add Constant for Compartment
        SP_DS_Metadataclass* l_pcMC = m_pcGraph->GetMetadataclass(SP_DS_META_FUNCTION);

        SP_DS_Metadata* l_function = l_pcMC->NewElement(1);

        wxString l_FuncId;
        wxString l_FuncName;
        getSBMLName(l_sbmlFunction, l_FuncId, l_FuncName);

        l_function->GetAttribute(wxT("Name"))->SetValueString(l_FuncId);

        SP_DS_Attribute* l_pcAttrComment = l_function->GetAttribute(wxT("Comment"));

        if(!l_FuncName.IsEmpty())
        {
            l_FuncName = wxT("name=\"") + l_FuncName + wxT("\"\n");
        }

        wxString l_metaid;
        if(l_sbmlFunction->isSetMetaId())
        {
            l_metaid << wxT("metaid=\"") << l_sbmlFunction->getMetaId() << wxT("\"\n");
        }

        wxString l_sNotes;
        if(l_sbmlFunction->isSetNotes())
        {
            l_sNotes = l_sbmlFunction->getNotesString()+wxT("\n");
        }

        wxString l_sAnnotation;
        if(l_sbmlFunction->isSetAnnotation())
        {
            l_sAnnotation = l_sbmlFunction->getAnnotationString()+wxT("\n");
        }

        l_pcAttrComment->SetValueString(l_FuncName+l_metaid+l_sNotes+l_sAnnotation);
        l_pcAttrComment->SetShow(false);

        wxString l_functionParam;
        for (unsigned int j = 0; j < l_sbmlFunction->getNumArguments(); ++j) {
            if(j > 0) l_functionParam << ",";
            l_functionParam << formulaToString(l_sbmlFunction->getArgument(j));
        }
        wxString l_functionBody;
        if (l_sbmlFunction->isSetBody())
        {
            l_functionBody = formulaToString(l_sbmlFunction->getBody());
        }
        l_function->GetAttribute(wxT("Parameter"))->SetValueString(l_functionParam);
        l_function->GetAttribute(wxT("Body"))->SetValueString(l_functionBody);

    }

}
