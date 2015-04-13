//////////////////////////////////////////////////////////////////////
// $Author: CR $
// $Version: 0.1 $
// $Date: 2013/05/21 $
// Short Description: export to sbml
//////////////////////////////////////////////////////////////////////

#include "export/SP_ExportColPN2SBML.h"
#include "sp_defines.h"

#include "sp_ds/SP_DS_Edge.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_ds/SP_DS_Graph.h"


#include <wx/tokenzr.h>
#include <wx/filename.h>
#include <wx/tokenzr.h>
#include <wx/sstream.h>
#include "sp_ds/extensions/continuous/SP_DS_ContinuousPlaceFormula.h"

bool
SP_ExportColPN2SBML::AddToDialog(SP_DLG_ExportProperties* p_pcDlg, SP_MDI_Doc* p_pcDoc)
{
	SP_WDG_NotebookPage* l_pcNotebookPage = p_pcDlg->AddPage(wxT("SBML LEVELS"));

	wxSizer* l_pcMainTablePropertiesSizer =	new wxStaticBoxSizer( new wxStaticBox( l_pcNotebookPage, -1,
											wxT("Choose Your Option") ), wxVERTICAL );
	wxSizer* l_pcRowSizer = new wxBoxSizer( wxVERTICAL );

	m_pRadioButton1=new wxRadioButton(l_pcNotebookPage, -1, wxT("&SBML LEVEL2 V3"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	m_pRadioButton2=new wxRadioButton(l_pcNotebookPage, -1, wxT("&SBML LEVEL1 V2"), wxDefaultPosition, wxDefaultSize);
	m_pRadioButton1->SetValue(true);

	l_pcRowSizer->Add(m_pRadioButton1, 0, wxEXPAND | wxALL, 5);
	l_pcRowSizer->Add(m_pRadioButton2, 0, wxEXPAND | wxALL, 5);
	l_pcMainTablePropertiesSizer->Add(l_pcRowSizer, 0, wxEXPAND | wxALL, 5);

	l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );

	l_pcMainTablePropertiesSizer->Add(l_pcRowSizer, 0, wxEXPAND | wxALL, 5);

	l_pcNotebookPage->AddControl(l_pcMainTablePropertiesSizer, 0, wxEXPAND | wxALL, 5);

	 return true;
}

bool
SP_ExportColPN2SBML::OnDialogOk()
{
	if(m_pRadioButton1->GetValue())
		level=2;
	else if(m_pRadioButton2->GetValue())
		level=1;
	else     //else run sbml level 2 by default
		level=2;

	return true;
}




SP_ExportColPN2SBML::SP_ExportColPN2SBML()
{
}

SP_ExportColPN2SBML::~SP_ExportColPN2SBML()
{
}

bool SP_ExportColPN2SBML::AcceptsDoc(SP_MDI_Doc* p_doc)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	wxString cName = p_doc->GetNetclassName();
	return (cName == SP_DS_COLPN_CLASS ||
			cName == SP_DS_COLEPN_CLASS ||
			cName == SP_DS_COLSPN_CLASS ||
			cName == SP_DS_COLCPN_CLASS ||
			cName == SP_DS_COLHPN_CLASS);
}

bool SP_ExportColPN2SBML::DoWrite()
{

	m_sNetClass = m_graph->GetNetclass()->GetName();

	Sh_ptr<SP_DS_ColPN_Unfolding> l_pcUnfolding = Sh_ptr<SP_DS_ColPN_Unfolding>(new SP_DS_ColPN_Unfolding());

	if( !l_pcUnfolding->Unfolding() )
		return false;	

	wxString l_sType = wxT("pn");
	if(m_doc->GetNetclassName() == SP_DS_COLEPN_CLASS)
	{
		l_sType = wxT("xpn");
	}
	else if(m_doc->GetNetclassName() == SP_DS_COLSPN_CLASS)
	{
		l_sType = wxT("spn");
		if(!m_graph->GetNodeclass(SP_DS_IMMEDIATE_TRANS)->GetElements()->empty())
		{
			l_sType = wxT("gspn");
		}
		if(!m_graph->GetNodeclass(SP_DS_DETERMINISTIC_TRANS)->GetElements()->empty()
			|| !m_graph->GetNodeclass(SP_DS_SCHEDULED_TRANS)->GetElements()->empty())
		{
			l_sType = wxT("xspn");
		}
	}
	else if(m_doc->GetNetclassName() == SP_DS_COLCPN_CLASS)
	{
		l_sType = wxT("cpn");
	}
	else if(m_doc->GetNetclassName() == SP_DS_COLHPN_CLASS)
	{
		l_sType = wxT("hpn");
	}
	// m_file.Write(l_sType + wxT(" [") + GetNetName(m_fileName) + wxT("]\n") + wxT("{\n"));


	bool l_bReturn = true;
	unsigned int l_nLevel;
	unsigned int l_nVersion;
	if(level==1)
	{
			l_nLevel=1;
			l_nVersion=2;
	}
	else
	{
			l_nLevel=2;
			l_nVersion=3;
	}

	m_pcSbmlDoc = new SBMLDocument(l_nLevel, l_nVersion);
	CHECK_POINTER(m_pcSbmlDoc, return false);
	wxString l_sModelname = m_doc->GetUserReadableName();
	l_sModelname.Replace(wxT("."), wxT("_"));
	std::string l_sModel = std::string(l_sModelname.mb_str());
	m_pcSbmlModel = m_pcSbmlDoc->createModel(l_sModel);
	CHECK_POINTER(m_pcSbmlModel, return false);


	//Unit Defination
	UnitDefinition* l_pcUnitDefinition = m_pcSbmlModel->createUnitDefinition();
	l_pcUnitDefinition->setId("substance");
	Unit* l_pcUnit = m_pcSbmlModel->createUnit();
	//UnitKind_t kind=item;
	l_pcUnit->setKind(UNIT_KIND_ITEM);

	Compartment* l_pcCompartment = m_pcSbmlModel->createCompartment();
	l_pcCompartment->setId("compartment");
	unsigned int l_nDimension = 0;
	l_pcCompartment->setSpatialDimensions(l_nDimension);
	//l_pcCompartment->setSize(1);

	if( m_sNetClass ==  SP_DS_COLSPN_CLASS ||
		m_sNetClass ==  SP_DS_COLCPN_CLASS ||
		m_sNetClass ==  SP_DS_COLHPN_CLASS)
	{
		WriteConstants();
		WriteParameters(l_pcUnfolding);
	}
	WritePlaces(l_pcUnfolding);
	WriteTransitions(l_pcUnfolding);

	// m_file.Write(wxT("}\n"));
	if (ValidateSBML())
	{
				m_file.Write(wxString::FromUTF8(writeSBMLToString(m_pcSbmlDoc)));
		//		if (!writeSBML(m_pcSbmlDoc, m_fileName.utf8_str().data()))
		//			l_bReturn = false;
	}
	else
	{
			l_bReturn = false;
	}
	wxDELETE(m_pcSbmlDoc);
	return l_bReturn;
	return true;
}


bool SP_ExportColPN2SBML::WriteConstants()
{
	bool l_bReturn = true;
	ConstantMap::iterator l_itConst;

	for(l_itConst = m_constantNameMap.begin(); l_itConst != m_constantNameMap.end(); ++l_itConst)
	{
		SP_Constant *pa = l_itConst->second;
		wxString name = NormalizeString(pa->m_name);
		if (pa->m_name != name)
		{
			SP_LOGWARNING(
					wxString::Format(wxT("constant name %s was changed to %s"),
							pa->m_name.c_str(), name.c_str()));
		}

		Parameter* l_pcParam = m_pcSbmlModel->createParameter();
		l_pcParam->setConstant(true);

		l_pcParam->setId(name.utf8_str().data());
		l_pcParam->setValue(pa->m_marking);
	}

	return l_bReturn;
}


bool SP_ExportColPN2SBML::WritePlaces(Sh_ptr<SP_DS_ColPN_Unfolding> p_Unfolding)
{

	//m_file.Write(wxT("places:\n"));
	bool l_bReturn = true;

	if( m_sNetClass == SP_DS_COLPN_CLASS ||
		m_sNetClass == SP_DS_COLEPN_CLASS ||
		m_sNetClass == SP_DS_COLSPN_CLASS )
	{
		if(!p_Unfolding->GetUnfoldedDiscPlaces().empty())
		{
			//m_file.Write(wxT("  discrete:\n"));
			WritePlaceClass(p_Unfolding, p_Unfolding->GetUnfoldedDiscPlaces());
		}
	}
	else if( m_sNetClass == SP_DS_COLCPN_CLASS )
	{
		if(!p_Unfolding->GetUnfoldedContPlaces().empty())
		{
			//m_file.Write(wxT("  continous:\n"));
			WritePlaceClass(p_Unfolding, p_Unfolding->GetUnfoldedContPlaces());
		}
	}
	else if( m_sNetClass == SP_DS_COLHPN_CLASS )
	{
		if(!p_Unfolding->GetUnfoldedDiscPlaces().empty())
		{
			//  m_file.Write(wxT("  discrete:\n"));
			WritePlaceClass(p_Unfolding, p_Unfolding->GetUnfoldedDiscPlaces());
		}
		if(!p_Unfolding->GetUnfoldedContPlaces().empty())
		{
			//m_file.Write(wxT("  continous:\n"));
			WritePlaceClass(p_Unfolding, p_Unfolding->GetUnfoldedContPlaces());
		}
	}

	// m_file.Write(wxT("\n"));

		return l_bReturn;
		return TRUE;
}




bool SP_ExportColPN2SBML::WritePlaceClass(Sh_ptr<SP_DS_ColPN_Unfolding> p_Unfolding,
							const SP_CPN_UnfoldedPlaces& p_pmmUnfoldedPlaces)
{


	bool l_bReturn = true;
	const Compartment* l_pcCompartment = m_pcSbmlModel->getCompartment(0);

	SP_DS_ColListAttribute* l_pcColList = NULL;
	unsigned l_nCurrentColumn =	0;
	SP_CPN_UnfoldedPlaces::const_iterator itMap1;
	for(itMap1 = p_pmmUnfoldedPlaces.begin(); itMap1 != p_pmmUnfoldedPlaces.end(); itMap1++)
	{

		wxString l_sColPlName = itMap1->first;

		SP_CPN_UnfoldedPlace::const_iterator itMap2;
		for(itMap2 = itMap1->second.begin(); itMap2 != itMap1->second.end(); itMap2++ )
		{
			wxString l_sColor = itMap2->first;
			wxString l_sNodeType = itMap2->second.m_sNodeType;

			if(!l_pcColList)
			{
				l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >((*(m_graph->GetNodeclass(l_sNodeType)->GetElements()->begin()))->GetAttribute(wxT("MarkingList")));
				l_nCurrentColumn = l_pcColList->GetActiveColumn();
			}
			p_Unfolding->ModifyName(l_sColor);

			double l_nMarking;
			if(l_sNodeType == SP_DS_DISCRETE_PLACE)
			{
				l_nMarking = itMap2->second.m_anNetMarkings[l_nCurrentColumn];
			}
			else if(l_sNodeType == SP_DS_CONTINUOUS_PLACE)
			{
				l_nMarking = itMap2->second.m_adNetMarkings[l_nCurrentColumn];
			}

			wxString l_sName = l_sColPlName + wxT("_") + l_sColor;

			Species* l_pcSpecies = m_pcSbmlModel->createSpecies();

			l_pcSpecies->setCompartment(l_pcCompartment->getId());
			l_pcSpecies->setHasOnlySubstanceUnits(true);

			wxString nName = NormalizeString(l_sName);
			if (l_sName != nName)
			{
				SP_LOGWARNING(
						wxString::Format(wxT("place name %s was changed to %s"),
								l_sName.c_str(), nName.c_str()));
			}
			l_pcSpecies->setId(nName.utf8_str().data());

			l_pcSpecies->setInitialAmount(l_nMarking);

		}
	}

				return l_bReturn;
}

bool SP_ExportColPN2SBML::WriteTransitions(Sh_ptr<SP_DS_ColPN_Unfolding> p_Unfolding)
{

	//m_file.Write(wxT("transitions:\n"));

	if( m_sNetClass == SP_DS_COLPN_CLASS ||
		m_sNetClass == SP_DS_COLEPN_CLASS )
	{
		WriteTransitionClass(p_Unfolding, p_Unfolding->GetUnfoldedStochTransions());
	}
	else if( m_sNetClass == SP_DS_COLSPN_CLASS )
	{
		if(!p_Unfolding->GetUnfoldedStochTransions().empty())
		{
			//m_file.Write(wxT("  stochastic:\n"));
			WriteTransitionClass(p_Unfolding, p_Unfolding->GetUnfoldedStochTransions());
		}
		if(!p_Unfolding->GetUnfoldedImmTransions().empty())
		{
			//m_file.Write(wxT("  immediate:\n"));
			WriteTransitionClass(p_Unfolding, p_Unfolding->GetUnfoldedImmTransions());
		}
		if(!p_Unfolding->GetUnfoldedDetTransions().empty())
		{
			//m_file.Write(wxT("  deterministic:\n"));
			WriteTransitionClass(p_Unfolding, p_Unfolding->GetUnfoldedDetTransions());
		}
		if(!p_Unfolding->GetUnfoldedSchedTransions().empty())
		{
			//m_file.Write(wxT("  scheduled:\n"));
			WriteTransitionClass(p_Unfolding, p_Unfolding->GetUnfoldedSchedTransions());
		}
	}
	else if( m_sNetClass == SP_DS_COLCPN_CLASS )
	{
		if(!p_Unfolding->GetUnfoldedContTransions().empty())
		{
			//m_file.Write(wxT("  continous:\n"));
			WriteTransitionClass(p_Unfolding, p_Unfolding->GetUnfoldedContTransions());
		}
	}
	else if( m_sNetClass == SP_DS_COLHPN_CLASS )
	{
		if(!p_Unfolding->GetUnfoldedStochTransions().empty())
		{
			//m_file.Write(wxT("  stochastic:\n"));
			WriteTransitionClass(p_Unfolding, p_Unfolding->GetUnfoldedStochTransions());
		}
		if(!p_Unfolding->GetUnfoldedContTransions().empty())
		{
			//m_file.Write(wxT("  continous:\n"));
			WriteTransitionClass(p_Unfolding, p_Unfolding->GetUnfoldedContTransions());
		}
		if(!p_Unfolding->GetUnfoldedImmTransions().empty())
		{
			//m_file.Write(wxT("  immediate:\n"));
			WriteTransitionClass(p_Unfolding, p_Unfolding->GetUnfoldedImmTransions());
		}
		if(!p_Unfolding->GetUnfoldedDetTransions().empty())
		{
			//m_file.Write(wxT("  deterministic:\n"));
			WriteTransitionClass(p_Unfolding, p_Unfolding->GetUnfoldedDetTransions());
		}
		if(!p_Unfolding->GetUnfoldedSchedTransions().empty())
		{
			//m_file.Write(wxT("  scheduled:\n"));
			WriteTransitionClass(p_Unfolding, p_Unfolding->GetUnfoldedSchedTransions());
		}
	}

	//m_file.Write(wxT("\n"));
	return TRUE;
}

bool SP_ExportColPN2SBML::WriteTransitionClass(Sh_ptr<SP_DS_ColPN_Unfolding> p_Unfolding,
							const SP_CPN_UnfoldedTransitions& p_mmUnfoldedTransions)
{
	SP_DS_ColListAttribute* l_pcColList = NULL;
	unsigned l_nCurrentColumn =	0;
	SP_CPN_UnfoldedTransitions::const_iterator itMap1;
	for(itMap1 = p_mmUnfoldedTransions.begin(); itMap1 != p_mmUnfoldedTransions.end(); itMap1++)
	{
		wxString l_sColTRName = itMap1->first;
		SP_CPN_UnfoldedTransition::const_iterator itMap2;

		for(itMap2 = itMap1->second.begin(); itMap2 != itMap1->second.end(); itMap2++ )
		{
			wxString l_sBinding = itMap2->first;
			wxString l_NodeType = itMap2->second.m_sType;
			wxString l_sAnimName = itMap2->second.m_sAnimTransInstName;

			p_Unfolding->ModifyName(l_sBinding);

			wxString l_sBuffer = l_sColTRName + wxT("_") + l_sBinding;

			wxString l_sFunction;
			if( m_sNetClass ==  SP_DS_COLSPN_CLASS ||
				m_sNetClass ==  SP_DS_COLCPN_CLASS ||
				m_sNetClass ==  SP_DS_COLHPN_CLASS)
			{
				if(!l_pcColList)
				{
					if( l_NodeType == SP_DS_CONTINUOUS_TRANS ||
						l_NodeType == SP_DS_STOCHASTIC_TRANS ||
						l_NodeType == SP_DS_IMMEDIATE_TRANS)
					{
						l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >((*(m_graph->GetNodeclass(l_NodeType)->GetElements()->begin()))->GetAttribute(wxT("FunctionList")));
					}
					else if( l_NodeType == SP_DS_DETERMINISTIC_TRANS)
					{
						l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >((*(m_graph->GetNodeclass(l_NodeType)->GetElements()->begin()))->GetAttribute(wxT("DelayList")));
					}
					else if( l_NodeType == SP_DS_SCHEDULED_TRANS)
					{
						l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >((*(m_graph->GetNodeclass(l_NodeType)->GetElements()->begin()))->GetAttribute(wxT("PeriodicList")));
					}
					l_nCurrentColumn = l_pcColList->GetActiveColumn();
				}



				for(unsigned int i = 0; i < itMap2->second.m_anNetFunctions.size(); i++)
				{
					l_sFunction = itMap2->second.m_anNetFunctions[i];
/*
 * THIS PROGRAM WILL GIVE KINETIC LAW IN THE FORM OF MASS-ACTION.
 * SO USE EXPLICIT VERSION WHILE DOING EXPORT
 *
 * if (!l_sFunction.IsEmpty())
					{
						SP_DS_ContinuousPlaceFormula l_pFormula(l_sFunction);
						l_sFunction = l_pFormula.ReplacePatternInTransitionFormula()
					}
*
*
*/				}
			}

			Reaction* l_pcReaction = m_pcSbmlModel->createReaction();
			l_pcReaction->setReversible(false);


			wxString nName = NormalizeString(l_sBuffer);
			if (l_sBuffer != nName)
			{
					SP_LOGWARNING(
					wxString::Format(wxT("tr name %s was changed to %s"),
					l_sBuffer.c_str(), nName.c_str()));
			}
			l_pcReaction->setId(nName.utf8_str().data());


			//write arcs
			// conditions && updates
			wxString l_sConditions;
			wxString l_sUpdates;
			WriteArcs(p_Unfolding, itMap2->second.m_vInputArcs,l_sConditions,l_sUpdates,true,l_pcReaction);
			WriteArcs(p_Unfolding, itMap2->second.m_vOutputArcs,l_sConditions,l_sUpdates,false,l_pcReaction);

			l_sBuffer << wxT("\n        : ") << l_sConditions << wxT("\n        : ") << l_sUpdates << l_sFunction << wxT(";\n");




			//l_sEquation = t->m_function;


			if(level==1)
			{
				KineticLaw* l_pcKineticLaw = l_pcReaction->createKineticLaw();
				l_pcKineticLaw->setFormula(l_sFunction.utf8_str().data());
			}
			else //for level 2 by default
			{
				KineticLaw* l_pcKineticLaw = l_pcReaction->createKineticLaw();
				l_pcKineticLaw->setMath(SBML_parseFormula(l_sFunction.utf8_str().data()));
			}
			//m_file.Write(l_sBuffer);

		}
	}
	return true;
}


bool SP_ExportColPN2SBML::WriteArcs(Sh_ptr<SP_DS_ColPN_Unfolding> p_Unfolding,
							const vector<SP_CPN_UnfoldedArcInfo>& p_vArcs,
							wxString& p_sConditions, wxString& p_sUpdates, bool p_bInput,Reaction* l_pcReaction )
{
	wxString l_sSep = wxT(" & ");

	for(unsigned i = 0; i < p_vArcs.size(); i++)
	{
		const SP_CPN_UnfoldedArcInfo& l_ArcInfo = p_vArcs[i];
		wxString l_sColoredPlName = l_ArcInfo.m_sColPlaceName;
		wxString l_sColor = l_ArcInfo.m_sColor;
		wxString l_sDiscContType = l_ArcInfo.m_sDiscContType;
		wxString l_sArcType = l_ArcInfo.m_sArcType ;

		wxString l_sMult;

		if(l_sDiscContType == wxT("Place"))
		{
			l_sMult << l_ArcInfo.m_nMultiplicity;
		}
		else
		{
			l_sMult << l_ArcInfo.m_dMultiplicity;
		}

		p_Unfolding->ModifyName(l_sColor);

		wxString l_sName = l_sColoredPlName + wxT("_") + l_sColor;
		double multiplicity = l_ArcInfo.m_nMultiplicity;

		if(p_bInput)
		{
			if (l_sArcType == SP_DS_EDGE)
			{
				if( !p_sUpdates.IsEmpty() )
				{
					p_sUpdates << l_sSep;
				}

				p_sUpdates << wxT("[");
				p_sUpdates << l_sName;
				p_sUpdates << wxT(" - ") << l_sMult;
				p_sUpdates << wxT("]");

				SpeciesReference* l_pcReactant = l_pcReaction->createReactant();
				l_pcReactant->setSpecies(l_sName.utf8_str().data());
				if (multiplicity > 0)
					l_pcReactant->setStoichiometry(multiplicity);

			}
			else if(l_sArcType == SP_DS_READ_EDGE)
			{
				if( !p_sConditions.IsEmpty() )
				{
					p_sConditions << l_sSep;
				}
				p_sConditions << wxT("[");
				p_sConditions << l_sName;
				p_sConditions << wxT(" >= ") << l_sMult;
				p_sConditions << wxT("]");

				ModifierSpeciesReference* l_pcModifier = l_pcReaction->createModifier();
				l_pcModifier->setSpecies(l_sName.utf8_str().data());

			}
			else if(l_sArcType == SP_DS_INHIBITOR_EDGE )
			{
				if( !p_sConditions.IsEmpty() )
				{
					p_sConditions << l_sSep;
				}
				p_sConditions << wxT("[");
				p_sConditions << l_sName;
				p_sConditions << wxT(" < ") << l_sMult;
				p_sConditions << wxT("]");

				ModifierSpeciesReference* l_pcModifier = l_pcReaction->createModifier();
				l_pcModifier->setSpecies(l_sName.utf8_str().data());

			}
			else if (l_sArcType == SP_DS_EQUAL_EDGE )
			{
				if( !p_sConditions.IsEmpty() )
				{
					p_sConditions << l_sSep;
				}
				p_sConditions << wxT("[");
				p_sConditions << l_sName;
				p_sConditions << wxT(" = ") << l_sMult;
				p_sConditions << wxT("]");

				ModifierSpeciesReference* l_pcModifier = l_pcReaction->createModifier();
				l_pcModifier->setSpecies(l_sName.utf8_str().data());
			}
			else if (l_sArcType == SP_DS_RESET_EDGE )
			{
				if( !p_sUpdates.IsEmpty() )
				{
					p_sUpdates << l_sSep;
				}
				p_sUpdates << wxT("[");
				p_sUpdates << l_sName;
				p_sUpdates << wxT(" = 0");
				p_sUpdates << wxT("]");

				ModifierSpeciesReference* l_pcModifier = l_pcReaction->createModifier();
				l_pcModifier->setSpecies(l_sName.utf8_str().data());
			}
			else if (l_sArcType == SP_DS_MODIFIER_EDGE )
			{
				if( !p_sConditions.IsEmpty() )
				{
					p_sConditions << l_sSep;
				}
				p_sConditions << wxT("[");
				p_sConditions << l_sName;
				p_sConditions << wxT("]");

				ModifierSpeciesReference* l_pcModifier = l_pcReaction->createModifier();
				l_pcModifier->setSpecies(l_sName.utf8_str().data());

			}
		}
		else
		{
			if( !p_sUpdates.IsEmpty() )
			{
				p_sUpdates << l_sSep;
			}

			p_sUpdates << wxT("[");
			p_sUpdates << l_sName;
			p_sUpdates << wxT(" + ") << l_sMult;
			p_sUpdates << wxT("]");


			SpeciesReference* l_pcProduct = l_pcReaction->createProduct();
			l_pcProduct->setSpecies(l_sName.utf8_str().data());
			if (multiplicity >= 0)
			{
					l_pcProduct->setStoichiometry(multiplicity);
			}

		}


	}
	return TRUE;
}

bool SP_ExportColPN2SBML::WriteParameters(Sh_ptr<SP_DS_ColPN_Unfolding> p_Unfolding)
{

	 //  m_file.Write(wxT("constants:\n"));
	wxString l_sParameterName;
	SP_DS_Nodeclass* l_pcNodeclass;
	l_pcNodeclass= m_graph->GetNodeclass( SP_DS_PARAM );

	SP_ListNode::const_iterator l_itElem;
	for(l_itElem = l_pcNodeclass->GetElements()->begin();
		l_itElem != l_pcNodeclass->GetElements()->end();
		++l_itElem )
	{
		SP_DS_Node* l_pcNode = (*l_itElem);
		wxString l_sBuffer = wxT("    ");
		l_sBuffer << wxT("double ");

		l_sParameterName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
		l_sBuffer << wxT(" = ");
		SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcNode->GetAttribute(wxT("ParameterList")));
		double l_nParamValue = l_pcColList->GetActiveCellValueDouble(1);

		//l_sBuffer << wxT(";\n");
		 // m_file.Write(l_sBuffer);
		//m_file.Write(wxT("\n"));
		Parameter* l_pcParam = m_pcSbmlModel->createParameter();
		l_pcParam->setConstant(true);

		wxString name = NormalizeString(l_sParameterName);
		if (l_sParameterName != name)
		{
			SP_LOGWARNING(
			wxString::Format(wxT("parameter name %s was changed to %s"),
						l_sParameterName.c_str(), name.c_str()));
		}

		l_pcParam->setId(name.utf8_str().data());
		l_pcParam->setValue(l_nParamValue);
	}
	return TRUE;
}

bool SP_ExportColPN2SBML::ValidateSBML()
{
	CHECK_POINTER(m_pcSbmlDoc, return false);
	if(level==2)
	{
		m_pcSbmlDoc->checkL2v3Compatibility();
	}
	else if(level==1)
	{
		m_pcSbmlDoc->checkL1Compatibility();
	}
	else
		m_pcSbmlDoc->checkL2v3Compatibility();


	unsigned int l_nErrors = m_pcSbmlDoc->getNumErrors();
	bool l_nSeriousErrors = false;
	if (l_nErrors > 0)
	{
		l_nSeriousErrors = true;
		for (unsigned int i = 0; i < l_nErrors; i++)
		{
			wxString l_sBuffer = wxT("Error ");
			l_sBuffer << i+1;
			SP_LOGERROR(l_sBuffer);
			l_sBuffer = wxString(m_pcSbmlDoc->getError(i)->getMessage().data(), wxConvUTF8);
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
		if(wxYES == SP_MESSAGEBOX(wxT("Several errors are encountered while creating an SBML file.\nDo you really want to continue?"),wxT("Error"), wxYES_NO))
		{
			l_nSeriousErrors = 0;
		}
	}

	return !l_nSeriousErrors;
}
