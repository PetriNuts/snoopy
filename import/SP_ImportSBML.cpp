//////////////////////////////////////////////////////////////////////
// $Author: schroed $
// $Version: 0.1 $
// $Date: 2007/02/03 $
// Short Description: creates a continious petrinet on a SBML file
//////////////////////////////////////////////////////////////////////

#include "SP_ImportSBML.h"
#include "dssz/auxi/math.h"

wxString
SP_ImportSBML::formulaToString(const ASTNode* p_Math)
{
	CHECK_POINTER(p_Math, return wxEmptyString);
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
		p_Id = p_Species->getId();
		ret = true;
	}
	if (p_Species->isSetName())
	{
		p_Name = p_Species->getName();
		ret = true;
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
		p_Id = p_Event->getId();
		ret = true;
	}
	if (p_Event->isSetName())
	{
		p_Name = p_Event->getName();
		ret = true;
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
		p_Id = p_Reaction->getId();
		ret = true;
	}
	if (p_Reaction->isSetName())
	{
		p_Name = p_Reaction->getName();
		ret = true;
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
		p_Id = p_Parameter->getId();
		ret = true;
	}
	if (p_Parameter->isSetName())
	{
		p_Name = p_Parameter->getName();
		ret = true;
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
		p_Id = p_Compartment->getId();
		ret = true;
	}
	if (p_Compartment->isSetName())
	{
		p_Name = p_Compartment->getName();
		ret = true;
	}
	return ret;
}

SP_DS_Node* SP_ImportSBML::getSpeciesNode(const wxString& searchName)
{
	auto speciesIt = m_Species.find(searchName);
	if (speciesIt != std::end(m_Species))
	{
		return speciesIt->second;
	}
	return nullptr;
}

SP_DS_Node* SP_ImportSBML::getReactionNode(const wxString& searchName)
{
	auto speciesIt = m_Reactions.find(searchName);
	if (speciesIt != std::end(m_Reactions))
	{
		return speciesIt->second;
	}
	return nullptr;
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
			if(p_sName == l_sName)
			{
				l_bReturn = true;
			}
		}
	}
	return l_bReturn;
}

bool SP_ImportSBML::changeInReactionFormula(const wxString& p_sOld, const wxString& p_sNew, ASTNode* p_sbmlFormula)
{
	CHECK_POINTER(p_sbmlFormula, return false);

	bool l_bReturn = false;
	for(unsigned int i = 0; i < p_sbmlFormula->getNumChildren(); i++)
	{
		if(changeInReactionFormula(p_sOld, p_sNew, p_sbmlFormula->getChild(i)))
		{
			l_bReturn = true;
		}
	}

	if(p_sbmlFormula->isName())
	{
		const char* l_sName = p_sbmlFormula->getName();
		if(p_sOld == l_sName)
		{
			p_sbmlFormula->setName(p_sNew.mb_str());
			l_bReturn = true;
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

	p_sbmlDoc->checkL2v4Compatibility();

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

	return l_nSeriousErrors == 0;
}

bool SP_ImportSBML::NormalizeStoichiometries(const std::map<SP_DS_Edge*, double>& s)
{
	//check values for non-integer ones
	double minimum = 1.0;
	for (const auto & s_it : s)
	{
		minimum = std::min(minimum, s_it.second);
	}
	if(minimum < 1.0)
	{
		double faktor = 1.0 / minimum;
		for (const auto & s_it : s)
		{
			long m = lround(s_it.second * faktor);
			SP_DS_Edge* l_pcEdge = s_it.first;
			l_pcEdge->GetAttribute(wxT("Multiplicity"))->SetValueString(
					wxString::Format(wxT("%li"), m));
		}
	}
	return true;
}
