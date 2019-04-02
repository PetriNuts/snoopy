
//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: George Assaf $
// $Version: 0.0 $
// $Revision:  $
// $Date: 26.01.2019 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "SP_DS_FuzzyCPN.h"
#include "sp_ds/netclasses/SP_DS_ContinuousPed.h"
#include "sp_ds/SP_DS_Edgeclass.h"
#include "sp_ds/SP_DS_Nodeclass.h"
#include "sp_ds/SP_DS_Metadataclass.h"

#include "sp_ds/attributes/SP_DS_BoolAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/attributes/SP_DS_NumberAttribute.h"
#include "sp_ds/attributes/SP_DS_DoubleAttribute.h"
#include "sp_ds/attributes/SP_DS_LogicAttribute.h"
#include "sp_ds/attributes/SP_DS_IdAttribute.h"
#include "sp_ds/attributes/SP_DS_MultiplicityAttribute.h"

// by sl
#include "sp_ds/attributes/SP_DS_DoubleMarkingDependentMultiplicity.h"
#include "sp_ds/attributes/SP_DS_DoubleMarkingAttribute.h"


#include "sp_ds/attributes/SP_DS_EquationAttribute.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"
#include "sp_ds/attributes/SP_DS_TypeAttribute.h"

#include "sp_gr/attributes/gra_SPN/SP_GR_StParameterListAttribute.h"
//for ColList
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"

#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/extensions/SP_DS_Coarse.h"
#include "sp_ds/extensions/SP_DS_ContinuousPedAnimation.h"

// extended graphical representation
#include "sp_gr/shapes/SP_GR_ExtendedCircle.h"
#include "sp_gr/shapes/SP_GR_ExtendedRectangle.h"
#include "sp_gr/shapes/SP_GR_ExtendedEllipse.h"
#include "sp_gr/metadata/SP_GR_PlotSymbol.h"
#include "sp_gr/shapes/SP_GR_Rectangle.h"

#include "sp_gr/edges/SP_GR_ArrowEdge.h"
#include "sp_gr/edges/SP_GR_ExtendedEdge.h"

#include "sp_gr/attributes/SP_GR_TextAttribute.h"
#include "sp_gr/attributes/SP_GR_NumberAttribute.h"
#include "sp_gr/attributes/SP_GR_DoubleAttribute.h"
#include "sp_gr/attributes/SP_GR_MultiplicityAttribute.h"
#include "sp_gr/attributes/SP_GR_DoubleMultiplicityAttribute.h"
#include "sp_gr/attributes/SP_GR_MarkAttribute.h"
#include "sp_gr/attributes/SP_GR_DoubleMarkAttribute.h"
#include "sp_gr/attributes/SP_GR_CommentAttribute.h"
#include "sp_gr/attributes/SP_GR_HiddenTextAttribute.h"
#include "sp_gr/attributes/SP_GR_ColListAttribute.h"
#include "sp_gr/attributes/SP_GR_MarkNumberAttribute.h"
#include "sp_gr/metadata/SP_GR_MetaComposite.h"

#include "sp_gr/attributes/gra_SPN/SP_GR_StFunctionAttribute.h"

#include "sp_gui/widgets/dialogs/SP_WDG_DialogText.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogBool.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogShowOnly.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogGraphic.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogExtendedGraphic.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogMultiline.h"
#include "sp_gui/widgets/dialogs/wid_dia_SPN/SP_WDG_StFunctionList.h"

#include "sp_utilities.h"

#include "snoopy.h"
#include "sp_core/SP_GPR_Elements.h"
#include "sp_core/SP_GPR_Animation.h"

SP_DS_FuzzyCPN::SP_DS_FuzzyCPN() :
	SP_DS_ContinuousPed(SP_DS_FUZZYCPN_CLASS)
{
	m_sExtension = wxT("fcpn");
	m_sFilter = wxT("*.fcpn;*.fspcontped");

	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Continuous Place"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Continuous Transition"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), SP_DS_PARAM);
}

SP_DS_FuzzyCPN::SP_DS_FuzzyCPN(const wxString& p_pchName) :
	SP_DS_ContinuousPed(p_pchName)
{
	m_sExtension = wxT("fcpn");
	m_sFilter = wxT("*.fcpn;*.fspcontped");

	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Continuous Place"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Continuous Transition"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), SP_DS_PARAM);
}

SP_DS_Graph* SP_DS_FuzzyCPN::CreateGraph(SP_DS_Graph* p_pcGraph)
{

	if (!SP_DS_ContinuousPed::CreateGraph(p_pcGraph))
		return NULL;


	SP_DS_Attribute* l_pcAttr;
	SP_DS_Metadataclass* l_pcMC;
	l_pcMC = p_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);

	l_pcAttr = l_pcMC->GetPrototype()->GetAttribute(wxT("Type"));
	SP_DS_TypeAttribute* l_pcType = dynamic_cast< SP_DS_TypeAttribute* >(l_pcAttr);
	l_pcType->AddPossibleValue(wxT("double"));
	l_pcType->AddPossibleValue(wxT("TFN"));
	//l_pcType->AddPossibleValue(wxT("BFN"));
	//l_pcType->AddPossibleValue(wxT("TZN"));

	p_pcGraph->SetAnimation(new SP_DS_ContinuousPedAnimation(1, 2, 50, 1.0));

	return p_pcGraph;

}


bool
SP_DS_FuzzyCPN::CreateInitialElements(SP_DS_Graph* p_pcGraph)
{
	SP_DS_ContinuousPed::CreateInitialElements(p_pcGraph);
	/*
	SP_DS_Metadataclass* l_pcMC;

	l_pcMC = p_pcGraph->GetMetadataclass(wxT("Simulation Properties"));
	l_pcMC->NewElement(1);

	l_pcMC = p_pcGraph->GetMetadataclass(wxT("Simulation Results"));
	l_pcMC->NewElement(1);
	*/
	return true;
}

bool
SP_DS_FuzzyCPN::MetadataRequirement(SP_DS_Metadata* p_pcMetadata)
{
	if (!SP_DS_ContinuousPed::MetadataRequirement(p_pcMetadata))
	{
		return false;
	}
	/*
	wxString l_sName = p_pcMetadata->GetClassName();
	if ((l_sName == wxT("Simulation Properties") || l_sName == wxT("Simulation Results"))
		&& !p_pcMetadata->GetMetadataclass()->GetElements()->empty())
	{
		return false;
	}
	*/
	return true;
}


bool SP_DS_FuzzyCPN::NodeRequirement(SP_DS_Node* p_pcNode)
{
	return SP_DS_ContinuousPed::NodeRequirement(p_pcNode);
}

bool SP_DS_FuzzyCPN::EdgeRequirement(SP_DS_Edgeclass* p_pcClass, SP_Data* p_pcSrc, SP_Data* p_pcTrg)
{
	if (!SP_DS_ContinuousPed::EdgeRequirement(p_pcClass, p_pcSrc, p_pcTrg))
		return FALSE;

	/*
	if (p_pcSrc->GetElementType() != SP_ELEMENT_NODE || p_pcTrg->GetElementType() != SP_ELEMENT_NODE)
		return FALSE;

	if (p_pcSrc->GetClassName() == wxT("Comment"))
		return FALSE;
	if (p_pcTrg->GetClassName() == wxT("Comment"))
		return FALSE;

	SP_DS_Node* l_pcSrc = dynamic_cast<SP_DS_Node*> (p_pcSrc);
	SP_DS_Node* l_pcTrg = dynamic_cast<SP_DS_Node*> (p_pcTrg);
	if (l_pcSrc->GetNodeclass() == l_pcTrg->GetNodeclass())
		return FALSE;

	// Test and Inhibitor Edge only from Place to Transition
	if ((p_pcClass->GetName() == SP_DS_INHIBITOR_EDGE) || (p_pcClass->GetName() == SP_DS_READ_EDGE))
	{
		if (l_pcSrc->GetNodeclass()->GetName() == SP_DS_CONTINUOUS_TRANS)
			return FALSE;
		if (l_pcTrg->GetNodeclass()->GetName() == SP_DS_CONTINUOUS_PLACE)
			return FALSE;
		if (l_pcSrc->GetNodeclass()->GetName() == SP_DS_PARAM)
			return FALSE;
		if (l_pcTrg->GetNodeclass()->GetName() == SP_DS_PARAM)
			return FALSE;
	}

	// Egde only from Place to Transition or Transition to Place
	if (((l_pcSrc->GetNodeclass()->GetName() == SP_DS_PARAM) ||
		(l_pcTrg->GetNodeclass()->GetName() == SP_DS_PARAM)))
	{
		return FALSE;
	}

	if (l_pcSrc->GetCoarse() && l_pcTrg->GetCoarse())
		return FALSE;
	if (l_pcSrc->GetCoarse())
		return (l_pcSrc->GetCoarse()->GetInnerClass() != l_pcTrg->GetNodeclass()->GetName());

	if (l_pcTrg->GetCoarse())
		return (l_pcTrg->GetCoarse()->GetInnerClass() != l_pcSrc->GetNodeclass()->GetName());

	SP_ListEdge::const_iterator eIt;
	const SP_ListEdge* edges;
	edges = (dynamic_cast<SP_DS_Node*> (p_pcSrc))->GetSourceEdges();

	//check if there are another arc of the same type between these nodes
	for (eIt = edges->begin(); eIt != edges->end(); ++eIt)
	{
		SP_LOGDEBUG(wxString::Format(wxT("%p->%p (%p), %s"), (*eIt)->GetSource(), (*eIt)->GetTarget(), p_pcTrg, (*eIt)->GetClassName().c_str()));

		if ((*eIt)->GetTarget() == p_pcTrg && (*eIt)->GetClassName() == p_pcClass->GetName())
		{
			SP_MESSAGEBOX(wxString::Format(wxT("Only one edge of the type \"%s\" ")
				wxT("is allowed between these nodes"), (*eIt)->GetClassName().c_str()), wxT("Error"), wxOK | wxICON_ERROR);
			return false;
		}
	}

	//Modifier arc
	if (p_pcClass->GetName() == SP_DS_MODIFIER_EDGE)
	{
		if (p_pcSrc->GetClassName().Contains(wxT("Transition")))
			return false;
	}

	*/
	return TRUE;
}
