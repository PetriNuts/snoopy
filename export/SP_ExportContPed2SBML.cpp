//////////////////////////////////////////////////////////////////////
// $Author: dscheibl $
// $Version: 0.1 $
// $Date: 2005/08/17 $
// Short Description: export to sbml
//////////////////////////////////////////////////////////////////////

#include "export/SP_ExportContPed2SBML.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/netclasses/SP_DS_ContinuousPed.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/extensions/continuous/SP_DS_ContinuousPlaceFormula.h"

bool
SP_ExportContPed2SBML::AddToDialog(SP_DLG_ExportProperties* p_pcDlg, SP_MDI_Doc* p_pcDoc)
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
SP_ExportContPed2SBML::OnDialogOk()
{
	if(m_pRadioButton1->GetValue())
		level=2;
	else if(m_pRadioButton2->GetValue())
		level=1;
	else     //else run sbml level 2 by default
		level=2;

	return true;
}


bool SP_ExportContPed2SBML::AcceptsDoc(SP_MDI_Doc* p_doc)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	wxString cName = p_doc->GetNetclassName();
	return (cName == SP_DS_CONTINUOUSPED_CLASS);
}

bool SP_ExportContPed2SBML::DoWrite()
{
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

	SP_DS_Metadataclass* l_pcMC = m_graph->GetMetadataclass(wxT("General"));
	if(l_pcMC)
	{
		SP_DS_Metadata* l_pcGeneral = l_pcMC->GetElements()->front();
		wxString description = l_pcGeneral->GetAttribute(wxT("Description"))->GetValueString();
		wxString l_sLevel = SP_ExtractAttribute(wxT("level"), description);
		wxString l_sVersion = SP_ExtractAttribute(wxT("version"), description);

		if(l_sLevel.IsNumber() && l_sVersion.IsNumber())
		{
			l_sLevel.ToULong(&l_nLevel);
			l_sVersion.ToULong(&l_nVersion);
		}
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

	l_bReturn = WriteGraph();

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



	return !m_bError;
}

bool SP_ExportContPed2SBML::WritePlaces()
{
	const Compartment* l_pcCompartment = m_pcSbmlModel->getCompartment(0);

	ContPlaceIdMap::iterator pIt;
	SP_ContPlace* p;
	for (pIt = m_contPlaceIdMap.begin(); pIt != m_contPlaceIdMap.end(); pIt++)
	{
		p = (*pIt).second;

		Species* l_pcSpecies = m_pcSbmlModel->createSpecies();

		SP_DS_Attribute* l_pcAttrComment = p->m_node->GetAttribute(wxT("Comment"));
		wxString comment = l_pcAttrComment->GetValueString();

		wxString compartment = SP_ExtractAttribute(wxT("compartment"), comment);
		if(compartment.IsEmpty())
		{
			l_pcSpecies->setCompartment(l_pcCompartment->getId());
		}
		else
		{
			l_pcSpecies->setCompartment(compartment);
		}

		l_pcSpecies->setCompartment(l_pcCompartment->getId());
		l_pcSpecies->setId(p->m_name);
		wxString name = SP_ExtractAttribute(wxT("name"), comment);
		if(!name.IsEmpty())
		{
			l_pcSpecies->setName(name);
		}

		l_pcSpecies->setInitialConcentration(p->m_marking);

		l_pcSpecies->setBoundaryCondition(p->m_fixed);

		wxString notes = SP_ExtractNode(wxT("notes"), comment);
		if(!notes.IsEmpty())
		{
			l_pcSpecies->setNotes(notes);
		}
		wxString annotation = SP_ExtractNode(wxT("annotation"), comment);
		if(!annotation.IsEmpty())
		{
			l_pcSpecies->setAnnotation(annotation);
		}
	}
	return TRUE;
}

bool SP_ExportContPed2SBML::WriteTransitions()
{

	ContTransitionIdMap::iterator tIt;
	SP_ContTransition* t;
	list<SP_ContArc*>* aList;
	list<SP_ContArc*>::iterator aIt;
	SP_ContArc* a;
	int l_nWrittenPrePostPlaces;

	wxString l_sEquation;
	for (tIt = m_contTrIdMap.begin(); tIt != m_contTrIdMap.end(); tIt++)
	{
		t = (*tIt).second;
		SP_DS_Attribute* l_pcAttrComment = t->m_node->GetAttribute(wxT("Comment"));
		wxString comment = l_pcAttrComment->GetValueString();

		Reaction* l_pcReaction = m_pcSbmlModel->createReaction();
		l_pcReaction->setId(t->m_name);
		l_pcReaction->setReversible(t->m_reversible);

		wxString name = SP_ExtractAttribute(wxT("name"), comment);
		if(!name.IsEmpty())
		{
			l_pcReaction->setName(name);
		}

		wxString notes = SP_ExtractNode(wxT("notes"), comment);
		if(!notes.IsEmpty())
		{
			l_pcReaction->setNotes(notes);
		}

		wxString annotation = SP_ExtractNode(wxT("annotation"), comment);
		if(!annotation.IsEmpty())
		{
			l_pcReaction->setAnnotation(annotation);
		}

		l_sEquation = t->m_function;
		if (!l_sEquation.IsEmpty())
		{
			SP_DS_ContinuousPlaceFormula l_pFormula(l_sEquation);
			l_sEquation = l_pFormula.ReplacePatternInTransitionFormula(t->m_node);
		}

		KineticLaw* l_pcKineticLaw = l_pcReaction->createKineticLaw();
		l_pcKineticLaw->setFormula(l_sEquation);

		aList = m_preContTrMap[t->m_id];
		l_nWrittenPrePostPlaces = 0;
		if (aList)
		{
			for (aIt = aList->begin(); aIt != aList->end(); aIt++)
			{
				a = (*aIt);
				if (a->m_edge->GetEdgeclass()->GetName() == SP_DS_EDGE)
				{
					SpeciesReference* l_pcReactant = l_pcReaction->createReactant();
					l_pcReactant->setSpecies((m_contPlaceIdMap[a->m_id])->m_name.utf8_str().data());
					l_pcReactant->setStoichiometry(a->m_mult);

					l_nWrittenPrePostPlaces++;
				}
			}
		}

		aList = m_postContTrMap[t->m_id];
		l_nWrittenPrePostPlaces = 0;
		if (aList)
		{
			for (aIt = aList->begin(); aIt != aList->end(); aIt++)
			{
				a = (*aIt);
				if (a->m_edge->GetEdgeclass()->GetName() == SP_DS_EDGE)
				{
					SpeciesReference* l_pcProduct = l_pcReaction->createProduct();
					l_pcProduct->setSpecies((m_contPlaceIdMap[a->m_id])->m_name.utf8_str().data());
					l_pcProduct->setStoichiometry(a->m_mult);

					l_nWrittenPrePostPlaces++;
				}
			}
		}

		aList = m_preContTrMap[t->m_id];
		l_nWrittenPrePostPlaces = 0;
		if (aList)
		{
			for (aIt = aList->begin(); aIt != aList->end(); aIt++)
			{
				a = (*aIt);
				if(a->m_edge->GetEdgeclass()->GetName() == SP_DS_READ_EDGE)
				{
					SpeciesReference* l_Product = l_pcReaction->getProduct((m_contPlaceIdMap[a->m_id])->m_name.utf8_str().data());
					if(!l_Product)
					{
						ModifierSpeciesReference* l_pcModifier = l_pcReaction->createModifier();
						l_pcModifier->setSpecies((m_contPlaceIdMap[a->m_id])->m_name.utf8_str().data());

						l_nWrittenPrePostPlaces++;
					}
				}
			}
		}

	}

	return TRUE;
}

bool SP_ExportContPed2SBML::WriteGraph()
{
	bool l_bReturn = true;

	l_bReturn = WriteConstants();
	l_bReturn = WritePlaces();
	l_bReturn = WriteTransitions();

	return l_bReturn;
}

bool SP_ExportContPed2SBML::WriteConstants()
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
			if(!notes.IsEmpty())
			{
				l_pcComp->setNotes(notes);
			}
			if(!annotation.IsEmpty())
			{
				l_pcComp->setAnnotation(annotation);
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
			if(!notes.IsEmpty())
			{
				l_pcParam->setNotes(notes);
			}
			if(!annotation.IsEmpty())
			{
				l_pcParam->setAnnotation(annotation);
			}
		}
	}

	return l_bReturn;
}

bool SP_ExportContPed2SBML::ValidateSBML()
{
	CHECK_POINTER(m_pcSbmlDoc, return false);

	m_pcSbmlDoc->checkL2v3Compatibility();

	unsigned int l_nErrors = m_pcSbmlDoc->getNumErrors();
	bool l_nSeriousErrors = false;
	if (l_nErrors > 0)
	{
		l_nSeriousErrors = true;
		for (unsigned int i = 0; i < l_nErrors; i++)
		{
			wxString l_sBuffer = wxT("Error ");
			l_sBuffer << i + 1;
			SP_LOGERROR(l_sBuffer);
			l_sBuffer = wxString(m_pcSbmlDoc->getError(i)->getMessage().data(), wxConvUTF8);
			SP_LOGERROR(l_sBuffer);
		}
		wxString l_sErrors = wxT("");
		l_sErrors << wxT("Encountered ") << l_nErrors << wxT(" ")
				<< (l_nSeriousErrors ? wxT("error") : wxT("warning"))
				<< (l_nErrors == 1 ? wxT("") : wxT("s")) << wxT(" in this file.");
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
