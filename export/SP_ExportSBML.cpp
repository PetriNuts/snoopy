//////////////////////////////////////////////////////////////////////
// $Author: crohr $
// $Version: 0.1 $
// $Date: 2008/05/01 $
//////////////////////////////////////////////////////////////////////
#include "export/SP_ExportSBML.h"
#include <wx/filename.h>
#include <wx/tokenzr.h>
#include <wx/sstream.h>
#include <wx/regex.h>

#include "sp_core/base/SP_Graphic.h"

#include "sp_gui/mdi/SP_MDI_Doc.h"

#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"
#include "sp_ds/attributes/SP_DS_NumberAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/attributes/SP_DS_MarkingAttribute.h"

bool
SP_ExportSBML::AddToDialog(SP_DLG_ExportProperties* p_pcDlg, SP_MDI_Doc* p_pcDoc)
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
SP_ExportSBML::OnDialogOk()
{
	if(m_pRadioButton1->GetValue())
		level=2;
	else if(m_pRadioButton2->GetValue())
		level=1;
	else     //else run sbml level 2 by default
		level=2;

	return true;
}




bool
SP_ExportSBML::DoWrite()
{
	bool l_bReturn = true;
	unsigned long l_nLevel;
	unsigned long l_nVersion;

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

	wxString l_sModelname = m_doc->GetUserReadableName();
	l_sModelname.Replace(wxT("."), wxT("_"));

	SP_DS_Metadataclass* l_pcMC = m_graph->GetMetadataclass(wxT("General"));
	CHECK_POINTER(l_pcMC, return false);

	SP_DS_Metadata* l_pcGeneral = l_pcMC->GetElements()->front();
	wxString description = l_pcGeneral->GetAttribute(wxT("Description"))->GetValueString();
	wxString l_sLevel = SP_ExtractAttribute(wxT("level"), description);
	wxString l_sVersion = SP_ExtractAttribute(wxT("version"), description);

	if(l_sLevel.IsNumber() && l_sVersion.IsNumber())
	{
		l_sLevel.ToULong(&l_nLevel);
		level = l_nLevel;
		l_sVersion.ToULong(&l_nVersion);
	}

	wxString l_sName = l_pcGeneral->GetAttribute(wxT("Name"))->GetValueString();
	if(!l_sName.IsEmpty())
	{
		l_sModelname = l_sName;
	}

	m_pcSbmlDoc = new SBMLDocument(l_nLevel, l_nVersion);
	CHECK_POINTER(m_pcSbmlDoc, return false);

	m_pcSbmlModel = m_pcSbmlDoc->createModel(l_sModelname);
	CHECK_POINTER(m_pcSbmlModel, return false);

	// model description
	wxString name = SP_ExtractAttribute(wxT("name"), description);
	if(!name.IsEmpty())
	{
		m_pcSbmlModel->setName(name);
	}
	wxString metaid = SP_ExtractAttribute(wxT("metaid"), description);
	if(!metaid.IsEmpty())
	{
		m_pcSbmlModel->setMetaId(metaid);
	}
	wxString notes = SP_ExtractNode(wxT("notes"), description);
	if(!notes.IsEmpty())
	{
		if(m_pcSbmlModel->setNotes(notes) != LIBSBML_OPERATION_SUCCESS)
		{
			SP_LOGWARNING(wxT("model notes not exported!\n") + notes);
		}
	}
	wxString annotation = SP_ExtractNode(wxT("annotation"), description);
	if(!annotation.IsEmpty())
	{
		if(m_pcSbmlModel->setAnnotation(annotation) != LIBSBML_OPERATION_SUCCESS)
		{
			SP_LOGWARNING(wxT("model annotation not exported!\n") + annotation);
		}
	}

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

	WriteConstants();
	WritePlaces();
	WriteTransitions();

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
}


bool SP_ExportSBML::WritePlaces()
{

	bool l_bReturn = true;
	const Compartment* l_pcCompartment = m_pcSbmlModel->getCompartment(0);

	PlaceIdMap::iterator pIt;

	for (pIt = m_placeIdMap.begin(); pIt != m_placeIdMap.end(); pIt++)
	{
		SP_Place* p = (*pIt).second;

		SP_DS_Attribute* l_pcAttrComment = p->m_node->GetAttribute(wxT("Comment"));
		wxString comment = l_pcAttrComment->GetValueString();

		Species* l_pcSpecies = m_pcSbmlModel->createSpecies();

		wxString compartment = SP_ExtractAttribute(wxT("compartment"), comment);
		if(compartment.IsEmpty())
		{
			l_pcSpecies->setCompartment(l_pcCompartment->getId());
		}
		else
		{
			l_pcSpecies->setCompartment(compartment);
		}
		l_pcSpecies->setHasOnlySubstanceUnits(true);

		wxString id = p->m_name;
		l_pcSpecies->setId(id);

		l_pcSpecies->setInitialAmount(p->m_marking);

		wxString name = SP_ExtractAttribute(wxT("name"), comment);
		if(!name.IsEmpty())
		{
			l_pcSpecies->setName(name);
		}
		wxString metaid = SP_ExtractAttribute(wxT("metaid"), comment);
		if(!metaid.IsEmpty())
		{
			l_pcSpecies->setMetaId(metaid);
		}
		wxString boundaryCondition = SP_ExtractAttribute(wxT("boundaryCondition"), comment);
		if(boundaryCondition.IsSameAs(wxT("true"), false))
		{
			l_pcSpecies->setBoundaryCondition(true);
		}

		wxString notes = SP_ExtractNode(wxT("notes"), comment);
		if(!notes.IsEmpty())
		{
			if(l_pcSpecies->setNotes(notes) != LIBSBML_OPERATION_SUCCESS)
			{
				SP_LOGWARNING(id + wxT(" notes not exported!\n") + notes);
			}
		}
		wxString annotation = SP_ExtractNode(wxT("annotation"), comment);
		if(!annotation.IsEmpty())
		{
			if(l_pcSpecies->setAnnotation(annotation) != LIBSBML_OPERATION_SUCCESS)
			{
				SP_LOGWARNING(id + wxT(" annotation not exported!\n") + annotation);
			}
		}

	}

	return l_bReturn;
}

bool SP_ExportSBML::WriteTransitions()
{
	bool l_bReturn = true;
	TransitionIdMap::iterator tIt;

	for (tIt = m_trIdMap.begin(); tIt != m_trIdMap.end(); tIt++)
	{
		SP_Transition* t = (*tIt).second;

		SP_DS_Attribute* l_pcAttrComment = t->m_node->GetAttribute(wxT("Comment"));
		wxString comment = l_pcAttrComment->GetValueString();

		Reaction* l_pcReaction = m_pcSbmlModel->createReaction();
		l_pcReaction->setReversible(false);

		wxString id = t->m_name;
		l_pcReaction->setId(id);

		wxString name = SP_ExtractAttribute(wxT("name"), comment);
		if(!name.IsEmpty())
		{
			l_pcReaction->setName(name);
		}
		wxString metaid = SP_ExtractAttribute(wxT("metaid"), comment);
		if(!metaid.IsEmpty())
		{
			l_pcReaction->setMetaId(metaid);
		}

		wxString reversible = SP_ExtractAttribute(wxT("reversible"), comment);
		if(reversible.IsSameAs(wxT("true"), false))
		{
			l_pcReaction->setReversible(true);
		}
		else
		{
			l_pcReaction->setReversible(false);
		}

		wxString notes = SP_ExtractNode(wxT("notes"), comment);
		if(!notes.IsEmpty())
		{
			if(l_pcReaction->setNotes(notes) != LIBSBML_OPERATION_SUCCESS)
			{
				SP_LOGWARNING(id + wxT(" notes not exported!\n") + notes);
			}
		}

		wxString annotation = SP_ExtractNode(wxT("annotation"), comment);
		if(!annotation.IsEmpty())
		{
			if(l_pcReaction->setAnnotation(annotation) != LIBSBML_OPERATION_SUCCESS)
			{
				SP_LOGWARNING(id + wxT(" annotation not exported!\n") + annotation);
			}
		}

		wxString l_sEquation = t->m_function;
		//check for MassAction
		bool l_bMassAction = false;
		if(l_sEquation.Find(wxT("MassAction")) != wxNOT_FOUND)
		{
			l_bMassAction = true;
			l_sEquation.Replace(wxT("MassAction"), wxT(""));
		}

		list<SP_DiscrArc*>* prePlaces = m_preTrMap[t->m_id];
		if(prePlaces)
		{
			list<SP_DiscrArc*>::iterator itA;
			for(itA = prePlaces->begin(); itA != prePlaces->end(); ++itA)
			{
				SP_DiscrArc* a = *itA;
				long l_nMultiplicity = a->m_mult;
				SP_DS_Edge* l_pcEdge = a->m_edge;
				SP_DS_Node* l_pcSource = dynamic_cast<SP_DS_Node*>(l_pcEdge->GetSource());
				wxString l_sSourceName = dynamic_cast<SP_DS_NameAttribute*>(l_pcSource->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
				if(l_pcEdge->GetEdgeclass()->GetName() == wxT("Edge"))
				{
					SpeciesReference* l_pcReactant = l_pcReaction->createReactant();
					l_pcReactant->setSpecies(l_sSourceName);
					if (l_nMultiplicity > 0)
						l_pcReactant->setStoichiometry(l_nMultiplicity);
				}
				else if(l_pcEdge->GetEdgeclass()->GetName() == wxT("Read Edge") ||
						l_pcEdge->GetEdgeclass()->GetName() == wxT("Inhibitor Edge") ||
						l_pcEdge->GetEdgeclass()->GetName() == wxT("Modifier Edge"))
				{
					ModifierSpeciesReference* l_pcModifier = l_pcReaction->createModifier();
					l_pcModifier->setSpecies(l_sSourceName);
				}
				if (l_bMassAction)
				{
					if(l_nMultiplicity == 1)
					{
						l_sEquation << wxT("*") << l_sSourceName;
					}
					else
					{
						l_sEquation << wxT("*(factorial(") << l_sSourceName << wxT(")/factorial(")
								<< l_nMultiplicity << wxT("*factorial(") << l_sSourceName << wxT("-")
								<< l_nMultiplicity << wxT(")))");
					}
				}
			}
			if(level==1)
			{
				KineticLaw* l_pcKineticLaw = l_pcReaction->createKineticLaw();
				l_pcKineticLaw->setFormula(l_sEquation);
			}
			else //for level 2 by default
			{
				KineticLaw* l_pcKineticLaw = l_pcReaction->createKineticLaw();
				l_pcKineticLaw->setMath(SBML_parseFormula(l_sEquation.mb_str()));
			}
		}

		list<SP_DiscrArc*>* postPlaces = m_postTrMap[t->m_id];
		if(postPlaces)
		{
			list<SP_DiscrArc*>::iterator itA;
			for(itA = postPlaces->begin(); itA != postPlaces->end(); ++itA)
			{
				SP_DiscrArc* a = *itA;
				long l_nMultiplicity = a->m_mult;
				SP_DS_Edge* l_pcEdge = a->m_edge;
				SP_DS_Node* l_pcTarget = dynamic_cast<SP_DS_Node*>(l_pcEdge->GetTarget());
				wxString l_sTargetName = dynamic_cast<SP_DS_NameAttribute*>(l_pcTarget->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
				SpeciesReference* l_pcProduct = l_pcReaction->createProduct();
				l_pcProduct->setSpecies(l_sTargetName);
				if (l_nMultiplicity > 0)
				{
					l_pcProduct->setStoichiometry(l_nMultiplicity);
				}

			}
		}
	}

	return l_bReturn;
}

bool SP_ExportSBML::WriteConstants()
{
	bool l_bReturn = true;
	ConstantMap::iterator l_itConst;

	for(l_itConst = m_constantNameMap.begin(); l_itConst != m_constantNameMap.end(); ++l_itConst)
	{
		SP_Constant *pa = l_itConst->second;
		wxString id = NormalizeString(pa->m_name);

		SP_DS_Attribute* l_pcAttrComment = pa->m_node->GetAttribute(wxT("Comment"));
		wxString comment = l_pcAttrComment->GetValueString();

		wxString name = SP_ExtractAttribute(wxT("name"), comment);
		wxString metaid = SP_ExtractAttribute(wxT("metaid"), comment);
		wxString notes = SP_ExtractNode(wxT("notes"), comment);
		wxString annotation = SP_ExtractNode(wxT("annotation"), comment);

		if(pa->m_group.IsSameAs(wxT("compartment"), false))
		{
			Compartment* l_pcComp = m_pcSbmlModel->createCompartment();
			l_pcComp->setConstant(true);

			l_pcComp->setId(id);
			l_pcComp->setSize(pa->m_marking);
			if(!name.IsEmpty())
			{
				l_pcComp->setName(name);
			}
			if(!metaid.IsEmpty())
			{
				l_pcComp->setMetaId(metaid);
			}
			if(!notes.IsEmpty())
			{
				if(l_pcComp->setNotes(notes) != LIBSBML_OPERATION_SUCCESS)
				{
					SP_LOGWARNING(id + wxT(" notes not exported!\n") + notes);
				}
			}
			if(!annotation.IsEmpty())
			{
				if(l_pcComp->setAnnotation(annotation) != LIBSBML_OPERATION_SUCCESS)
				{
					SP_LOGWARNING(id + wxT(" annotation not exported!\n") + annotation);
				}
			}
		}
		else
		{
			Parameter* l_pcParam = m_pcSbmlModel->createParameter();
			l_pcParam->setConstant(true);

			l_pcParam->setId(id);
			l_pcParam->setValue(pa->m_marking);
			if(!name.IsEmpty())
			{
				l_pcParam->setName(name);
			}
			if(!metaid.IsEmpty())
			{
				l_pcParam->setMetaId(metaid);
			}
			if(!notes.IsEmpty())
			{
				if(l_pcParam->setNotes(notes) != LIBSBML_OPERATION_SUCCESS)
				{
					SP_LOGWARNING(id + wxT(" notes not exported!\n") + notes);
				}
			}
			if(!annotation.IsEmpty())
			{
				if(l_pcParam->setAnnotation(annotation) != LIBSBML_OPERATION_SUCCESS)
				{
					SP_LOGWARNING(id + wxT(" annotation not exported!\n") + annotation);
				}
			}
		}
	}

	return l_bReturn;
}


bool SP_ExportSBML::ValidateSBML()
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

bool SP_ExportSBML::AcceptsDoc(SP_MDI_Doc* p_doc)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	wxString cName = p_doc->GetNetclassName();
	return (cName == SP_DS_PN_CLASS ||
			cName == SP_DS_EXTPN_CLASS ||
			cName == SP_DS_SPN_CLASS);
}

