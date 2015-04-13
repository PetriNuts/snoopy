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

SP_ExportContPed2SBML::SP_ExportContPed2SBML()
{
}

SP_ExportContPed2SBML::~SP_ExportContPed2SBML()
{
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
	unsigned int l_nLevel = 2;
	unsigned int l_nVersion = 3;
	m_pcSbmlDoc = new SBMLDocument(l_nLevel, l_nVersion);
	CHECK_POINTER(m_pcSbmlDoc, return false);
	wxString l_sModelname = m_fileName.AfterLast(wxT('/'));
	l_sModelname = l_sModelname.AfterLast(wxT('\\'));
	l_sModelname.Replace(wxT("."), wxT("_"));
	m_pcSbmlModel = m_pcSbmlDoc->createModel(l_sModelname.utf8_str().data());
	CHECK_POINTER(m_pcSbmlModel, return false);

	bool l_bReturn = WriteGraph();

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
	ContPlaceIdMap::iterator pIt;
	SP_ContPlace* p;
	for (pIt = m_contPlaceIdMap.begin(); pIt != m_contPlaceIdMap.end(); pIt++)
	{
		p = (*pIt).second;

		wxString l_sCompId = wxT("compartment");
		if(!p->m_node->GetAttribute(wxT("Comment"))->GetValueString().IsEmpty())
		{
			l_sCompId = p->m_node->GetAttribute(wxT("Comment"))->GetValueString();
		}
		Compartment* l_pcCompartment = m_pcSbmlModel->getCompartment(l_sCompId.utf8_str().data());
		if(!l_pcCompartment)
		{
			l_pcCompartment = m_pcSbmlModel->getCompartment("compartment");
		}
		Species* l_pcSpecies = m_pcSbmlModel->createSpecies();

		l_pcSpecies->setCompartment(l_pcCompartment->getId());
		l_pcSpecies->setId(p->m_name.utf8_str().data());

		l_pcSpecies->setInitialConcentration(p->m_marking);

		l_pcSpecies->setBoundaryCondition(p->m_fixed);
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
		Reaction* l_pcReaction = m_pcSbmlModel->createReaction();
		t = (*tIt).second;
		l_pcReaction->setId(t->m_name.utf8_str().data());
		l_pcReaction->setReversible(t->m_reversible);

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

		l_sEquation = t->m_function;
		if (!l_sEquation.IsEmpty())
		{
			SP_DS_ContinuousPlaceFormula l_pFormula(l_sEquation);
			l_sEquation = l_pFormula.ReplacePatternInTransitionFormula(t->m_node);
		}

		KineticLaw* l_pcKineticLaw = l_pcReaction->createKineticLaw();
		l_pcKineticLaw->setFormula(l_sEquation.utf8_str().data());

	}

	return TRUE;
}

bool SP_ExportContPed2SBML::WriteGraph()
{
	bool l_bReturn = true;

	l_bReturn = WriteCompartments();
	l_bReturn = WritePlaces();
	l_bReturn = WriteParameters();
	l_bReturn = WriteTransitions();

	return l_bReturn;
}

bool SP_ExportContPed2SBML::WriteCompartments()
{
	bool l_bReturn = false;

	ConstantIdMap::iterator l_itConst;
	SP_Constant* l_pcConst;

	for (l_itConst = m_constantIdMap.begin(); l_itConst != m_constantIdMap.end(); l_itConst++)
	{
		l_pcConst = l_itConst->second;
		if(	l_pcConst->m_node->GetAttribute(wxT("Comment"))->GetValueString() == wxT("compartment"))
		{
			Compartment* l_sbmlComp = m_pcSbmlModel->createCompartment();
			l_sbmlComp->setId(l_pcConst->m_name.utf8_str().data());
			l_sbmlComp->setSize(l_pcConst->m_marking);
			l_bReturn = true;
		}
	}

	ParameterIdMap::iterator l_itParam;
	SP_Parameter* l_pcParam;

	for (l_itParam = m_parameterIdMap.begin(); l_itParam != m_parameterIdMap.end(); l_itParam++)
	{
		l_pcParam = l_itParam->second;
		if(	l_pcParam->m_node->GetAttribute(wxT("Comment"))->GetValueString() == wxT("compartment"))
		{
			Compartment* l_sbmlComp = m_pcSbmlModel->createCompartment();
			l_sbmlComp->setId(l_pcParam->m_name.utf8_str().data());
			l_sbmlComp->setSize(l_pcParam->m_marking);
			l_bReturn = true;
		}
	}
	if(!l_bReturn)
	{
		Compartment* l_sbmlComp = m_pcSbmlModel->createCompartment();
		l_sbmlComp->setId("compartment");
		l_sbmlComp->setSize(1);
		l_bReturn = true;
	}

	return l_bReturn;

}

bool SP_ExportContPed2SBML::WriteParameters()
{
	bool l_bReturn = true;

	ConstantIdMap::iterator l_itConst;
	SP_Constant* l_pcConst;

	for (l_itConst = m_constantIdMap.begin(); l_itConst != m_constantIdMap.end(); l_itConst++)
	{
		l_pcConst = l_itConst->second;
		Parameter* l_sbmlParam = m_pcSbmlModel->createParameter();
		l_sbmlParam->setConstant(true);
		l_sbmlParam->setId(l_pcConst->m_name.utf8_str().data());
		l_sbmlParam->setValue(l_pcConst->m_marking);
	}

	ParameterIdMap::iterator l_itParam;
	SP_Parameter* l_pcParam;

	for (l_itParam = m_parameterIdMap.begin(); l_itParam != m_parameterIdMap.end(); l_itParam++)
	{
		l_pcParam = l_itParam->second;
		Parameter* l_sbmlParam = m_pcSbmlModel->createParameter();
		l_sbmlParam->setConstant(true);
		l_sbmlParam->setId(l_pcParam->m_name.utf8_str().data());
		l_sbmlParam->setValue(l_pcParam->m_marking);
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
