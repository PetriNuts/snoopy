//////////////////////////////////////////////////////////////////////
// $Author: schroed $
// $Version: 0.1 $
// $Date: 2007/02/03 $
// Short Description: creates a continious petrinet on a SBML file
//////////////////////////////////////////////////////////////////////

#include "SP_ImportSBML.h"

wxString
SP_ImportSBML::formulaToString(const ASTNode* p_Math)
{
	char* formula = SBML_formulaToString(p_Math);
	wxString ret = wxString(formula, wxConvUTF8);
	free(formula);
	return ret;
}

bool
SP_ImportSBML::getSBMLSpeciesName(Species* p_Species, wxString& p_Id, wxString& p_Name)
{
	CHECK_POINTER(p_Species, return false);
	bool ret = false;
	// is ID or Name set
	wxString l_Name;
	if (p_Species->isSetId())
	{
		p_Id = wxString(p_Species->getId().c_str(), wxConvUTF8);
		ret = true;
	}
	if (p_Species->isSetName())
	{
		p_Name = wxString(p_Species->getName().c_str(), wxConvUTF8);
		ret = true;
		//add to conversion map
		wxString l_Name = p_Name;
		if(p_Species->isSetCompartment())
		{
			wxString l_Comp = wxString(p_Species->getCompartment().c_str(), wxConvUTF8);
			l_Name << wxT("__") << m_Id2Name[l_Comp];
		}
		m_Id2Name[p_Id] = l_Name;
	}
	return ret;
}

bool
SP_ImportSBML::getSBMLEventName(Event* p_Event, wxString& p_Id, wxString& p_Name)
{
	CHECK_POINTER(p_Event, return false);
	bool ret = false;
	// is ID or Name set
	wxString l_Name;
	if (p_Event->isSetId())
	{
		p_Id = wxString(p_Event->getId().c_str(), wxConvUTF8);
		ret = true;
	}
	if (p_Event->isSetName())
	{
		p_Name = wxString(p_Event->getName().c_str(), wxConvUTF8);
		ret = true;
		//add to conversion map
		m_Id2Name[p_Id] = p_Name;
	}
	return ret;
}

bool
SP_ImportSBML::getSBMLReactionName(Reaction* p_Reaction, wxString& p_Id, wxString& p_Name)
{
	CHECK_POINTER(p_Reaction, return false);
	bool ret = false;
	// is ID or Name set
	wxString l_Name;
	if (p_Reaction->isSetId())
	{
		p_Id = wxString(p_Reaction->getId().c_str(), wxConvUTF8);
		ret = true;
	}
	if (p_Reaction->isSetName())
	{
		p_Name = wxString(p_Reaction->getName().c_str(), wxConvUTF8);
		ret = true;
		//add to conversion map
		wxString l_Name = p_Name;
		if(p_Reaction->isSetCompartment())
		{
			wxString l_Comp = wxString(p_Reaction->getCompartment().c_str(), wxConvUTF8);
			l_Name << wxT("__") << m_Id2Name[l_Comp];
		}
		m_Id2Name[p_Id] = l_Name;
	}
	return ret;
}


bool
SP_ImportSBML::getSBMLParameterName(Parameter* p_Parameter, wxString& p_Id, wxString& p_Name)
{
	CHECK_POINTER(p_Parameter, return false);
	bool ret = false;
	// is ID or Name set
	wxString l_Name;
	if (p_Parameter->isSetId())
	{
		p_Id = wxString(p_Parameter->getId().c_str(), wxConvUTF8);
		ret = true;
	}
	if (p_Parameter->isSetName())
	{
		p_Name = wxString(p_Parameter->getName().c_str(), wxConvUTF8);
		ret = true;
		//add to conversion map
		m_Id2Name[p_Id] = p_Name;
	}
	return ret;
}

bool
SP_ImportSBML::getSBMLCompartmentName(Compartment* p_Compartment, wxString& p_Id, wxString& p_Name)
{
	CHECK_POINTER(p_Compartment, return false);
	bool ret = false;
	// is ID or Name set
	wxString l_Name;
	if (p_Compartment->isSetId())
	{
		p_Id = wxString(p_Compartment->getId().c_str(), wxConvUTF8);
		ret = true;
	}
	if (p_Compartment->isSetName())
	{
		p_Name = wxString(p_Compartment->getName().c_str(), wxConvUTF8);
		ret = true;
		//add to conversion map
		m_Id2Name[p_Id] = p_Name;
	}
	return ret;
}


// search in compoundlist
SP_DS_Node* SP_ImportSBML::existInCompoundList(const wxString& searchName)
{
	SP_ListNode::const_iterator speciesIt;
	for (speciesIt = g_CompoundList.begin(); speciesIt != g_CompoundList.end(); ++speciesIt)
	{
		wxString speciesName = (*speciesIt)->GetAttribute(wxT("Name"))->GetValueString();
		if (speciesName == searchName)
		{
			return (*speciesIt);
		}
	}
	return NULL;
}

bool SP_ImportSBML::existInReactionList(const wxString& searchName)
{
	SP_ListNode::const_iterator reacIt;
	for (reacIt = g_ReactionList.begin(); reacIt != g_ReactionList.end(); ++reacIt)
	{
		wxString reacName = (*reacIt)->GetAttribute(wxT("Name"))->GetValueString();
		if (reacName == searchName)
		{
			return true;
		}
	}
	return false;
}

bool SP_ImportSBML::existInReactionFormula(const wxString& p_sName, const ASTNode* p_sbmlFormula)
{
	CHECK_POINTER(p_sbmlFormula, return false);

	bool l_bReturn = false;
	for(unsigned int i = 0; i < p_sbmlFormula->getNumChildren(); i++)
	{
		if(existInReactionFormula(p_sName, p_sbmlFormula->getChild(i)))
		{
			l_bReturn = true;
		}
	}
	if(!l_bReturn)
	{
		if(p_sbmlFormula->isName())
		{
			const char* l_sName = p_sbmlFormula->getName();
			if(p_sName == wxString(l_sName, wxConvUTF8))
			{
				l_bReturn = true;
			}
		}
	}
	return l_bReturn;
}

const ASTNode*
SP_ImportSBML::getSBMLFormula(ASTNode* p_sbmlFormula)
{
	CHECK_POINTER(p_sbmlFormula, return NULL);

	ASTNode* l_sbmlFormula = p_sbmlFormula;

	if(l_sbmlFormula->getType() == AST_FUNCTION_POWER)
	{
		l_sbmlFormula->setType(AST_POWER);
	}
	else if(l_sbmlFormula->isUMinus())
	{
		//TODO: need to be fixed, is only temp workaround!!!!
		//continuous simulation can't deal with unary minus
		//i.e. a + -b dont work so we insert a zero
		// ->  a + 0 - b should be the same at all
		ASTNode* l_ASTLeft = new ASTNode(AST_INTEGER);
		l_ASTLeft->setValue(0);
		l_sbmlFormula->prependChild(l_ASTLeft);
	}

	for(unsigned int i = 0; i < l_sbmlFormula->getNumChildren(); i++)
	{

		getSBMLFormula(l_sbmlFormula->getChild(i));

	}
	return l_sbmlFormula;
}


bool SP_ImportSBML::ValidateSBML(SBMLDocument* p_sbmlDoc)
{
	CHECK_POINTER(p_sbmlDoc, return false);

	p_sbmlDoc->checkL2v3Compatibility();

	unsigned int l_nErrors = p_sbmlDoc->getNumErrors();
	bool l_nSeriousErrors = false;
	if (l_nErrors > 0)
	{
		l_nSeriousErrors = true;
		for (unsigned int i = 0; i < l_nErrors; i++)
		{
			wxString l_sBuffer = wxT("Error ");
			l_sBuffer << i+1;
			SP_LOGERROR(l_sBuffer);
			l_sBuffer = wxString(p_sbmlDoc->getError(i)->getMessage().data(), wxConvUTF8);
			SP_LOGERROR(l_sBuffer);
		}
		wxString l_sErrors = wxT("");
		l_sErrors << wxT("Encountered ") << l_nErrors << wxT(" ")
				<< (l_nSeriousErrors ? wxT("error") : wxT("warning")) << (l_nErrors == 1 ? wxT("")
				: wxT("s")) << wxT(" in this file.");
		SP_LOGERROR( l_sErrors);
	}

	if(l_nSeriousErrors)
	{
		if(wxYES == SP_MESSAGEBOX(wxT("Several errors are encountered while reading an SBML file.\nDo you really want to continue?"),wxT("Error"), wxYES_NO))
		{
			l_nSeriousErrors = 0;
		}
	}

	return !l_nSeriousErrors;
}

bool SP_ImportSBML::ConvertIds2Names()
{
	for(SP_DS_Metadataclass* l_pcMC : *(m_pcGraph->GetMetadataclasses()))
	{
		for(SP_DS_Metadata* l_pcMeta : *(l_pcMC->GetElements()))
		{
			SP_DS_Attribute* l_pcAttr = l_pcMeta->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME);
			if(l_pcAttr)
			{
				wxString l_sId = l_pcAttr->GetValueString();
				wxString l_sName = m_Id2Name[l_sId];
				if(l_sName != wxT(""))
				{
					l_pcAttr->SetValueString(l_sName);
				}
			}
		}
	}
	for(SP_DS_Nodeclass* l_pcNC : *(m_pcGraph->GetNodeclasses()))
	{
		for(SP_DS_Node* l_pcNode : *(l_pcNC->GetElements()))
		{
			SP_DS_Attribute* l_pcAttr = l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME);
			if(l_pcAttr)
			{
				wxString l_sId = l_pcAttr->GetValueString();
				wxString l_sName = m_Id2Name[l_sId];
				if(l_sName != wxT(""))
				{
					l_pcAttr->SetValueString(l_sName);
				}
			}
		}
	}
	return true;
}
