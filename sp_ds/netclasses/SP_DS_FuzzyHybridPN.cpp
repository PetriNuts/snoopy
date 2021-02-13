/*
* SP_DS_HybridPN.cpp
*
* $Author: George Assaf
* $Version: 0.0 $
* $Revision: 0.0 $
* $Date: 27.01.2019
* Short Description: Fuzzy Hybrid Petri Net
*/
//======================================================================================

#include "sp_utilities.h"

#include "snoopy.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_core/SP_GPR_Elements.h"
#include "sp_core/SP_GPR_Animation.h"
#include "sp_ds/netclasses/SP_DS_HybridPN.h"
#include "sp_ds/netclasses/SP_DS_FuzzyHybridPN.h"

#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/extensions/SP_DS_Coarse.h"

#include "sp_ds/SP_DS_Edgeclass.h"
#include "sp_ds/SP_DS_Nodeclass.h"
#include "sp_ds/SP_DS_Metadataclass.h"

// extended graphical representation
#include "sp_gr/shapes/SP_GR_ExtendedCircle.h"
#include "sp_gr/shapes/SP_GR_ExtendedRectangle.h"
#include "sp_gr/shapes/SP_GR_ExtendedEllipse.h"
#include "sp_gr/metadata/SP_GR_PlotSymbol.h"
#include "sp_gr/shapes/SP_GR_Rectangle.h"

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

#include "sp_gr/attributes/gra_SPN/SP_GR_StFunctionAttribute.h"
#include "sp_gr/attributes/gra_SPN/SP_GR_StParameterListAttribute.h"

#include "sp_ds/attributes/SP_DS_BoolAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/attributes/SP_DS_NumberAttribute.h"
#include "sp_ds/attributes/SP_DS_DoubleAttribute.h"
#include "sp_ds/attributes/SP_DS_LogicAttribute.h"
#include "sp_ds/attributes/SP_DS_IdAttribute.h"
#include "sp_ds/attributes/SP_DS_MultiplicityAttribute.h"
#include "sp_ds/attributes/SP_DS_DoubleMultiplicityAttribute.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"

//by sl
#include "sp_ds/attributes/SP_DS_DoubleMarkingDependentMultiplicity.h"
#include "sp_ds/attributes/SP_DS_DoubleMarkingAttribute.h"
#include "sp_ds/attributes/SP_DS_MarkingAttribute.h"
#include "sp_ds/attributes/SP_DS_TypeAttribute.h"

#include "sp_gui/widgets/dialogs/SP_WDG_DialogText.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogBool.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogShowOnly.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogGraphic.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogExtendedGraphic.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogMultiline.h"
#include "sp_gui/widgets/dialogs/wid_dia_SPN/SP_WDG_StFunctionList.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogColList.h"

#include "sp_gr/edges/SP_GR_ArrowEdge.h"
#include "sp_gr/edges/SP_GR_ExtendedEdge.h"

#include "sp_ds/animators/ani_HybridPN/SP_DS_HybridAnimation.h"

SP_DS_FuzzyHybridPN::SP_DS_FuzzyHybridPN() :
	SP_DS_HybridPN(SP_DS_FUZZYHPN_CLASS)
{
	m_sExtension = wxT("fhpn");
	m_sFilter = wxT("*.fhpn;*.fsphybrid");

	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Discrete Place"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Continuous Place"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Discrete Transition"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Continuous Transition"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), SP_DS_PARAM);

}

SP_DS_FuzzyHybridPN::SP_DS_FuzzyHybridPN(const wxString& p_pchName) :
	SP_DS_HybridPN(p_pchName)
{
	m_sExtension = wxT("fhpn");
	m_sFilter = wxT("*.fhpn;*.fsphybrid");

	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Discrete Place"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Continuous Place"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Discrete Transition"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Continuous Transition"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), SP_DS_PARAM);

}

SP_DS_Graph* SP_DS_FuzzyHybridPN::CreateGraph(SP_DS_Graph* p_pcGraph)
{

	
	if (!SP_DS_HybridPN::CreateGraph(p_pcGraph))
		return NULL;

	

	//////////////////////////////////////////////////////////////////////////////
	SP_DS_Attribute* l_pcAttr;
	SP_DS_Metadataclass* l_pcMC;
	l_pcMC = p_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);

	l_pcAttr = l_pcMC->GetPrototype()->GetAttribute(wxT("Type"));
	SP_DS_TypeAttribute* l_pcType = dynamic_cast< SP_DS_TypeAttribute* >(l_pcAttr);
	l_pcType->AddPossibleValue(wxT("double"));
	l_pcType->AddPossibleValue(wxT("TFN"));
	//l_pcType->AddPossibleValue(wxT("BFN"));
	//l_pcType->AddPossibleValue(wxT("TZN"));

	//where we keep fuzzy settings
	l_pcMC = p_pcGraph->AddMetadataclass(new SP_DS_Metadataclass(p_pcGraph, wxT("FuzzySettings")));

	l_pcMC->SetShowInElementTree(false);

	l_pcAttr = l_pcMC->AddAttribute(new SP_DS_NumberAttribute(wxT("AlphaLevels"), 10));
	l_pcAttr = l_pcMC->AddAttribute(new SP_DS_NumberAttribute(wxT("SamplingPoints"), 10));
	l_pcAttr = l_pcMC->AddAttribute(new SP_DS_TextAttribute(wxT("Comment"), wxT("")));

	l_pcAttr = l_pcMC->AddAttribute(new SP_DS_NumberAttribute(wxT("SamplingAlgo"), 0));

	//////////////////////////////////////////////////////////////////////////////


	//Set Animator
	p_pcGraph->SetAnimation(new SP_DS_HybridAnimation(1, 2, 50, 1.0));

	return p_pcGraph;
}

bool SP_DS_FuzzyHybridPN::NodeRequirement(SP_DS_Node* p_pcNode)
{
	return SP_DS_HybridPN::NodeRequirement(p_pcNode);
}

bool SP_DS_FuzzyHybridPN::EdgeRequirement(SP_DS_Edgeclass* p_pcClass, SP_Data* p_pcSrc, SP_Data* p_pcTrg)
{
	return SP_DS_HybridPN::EdgeRequirement(p_pcClass, p_pcSrc, p_pcTrg);
	
	
}

bool SP_DS_FuzzyHybridPN::CreateInitialElements(SP_DS_Graph* p_pcGraph)
{

	bool l_bISOk=	SP_DS_HybridPN::CreateInitialElements(p_pcGraph);

	SP_DS_Metadataclass* l_pcMC;
	l_pcMC = p_pcGraph->GetMetadataclass(wxT("FuzzySettings"));
	l_pcMC->NewElement(1);
	return l_bISOk;
}

bool
SP_DS_FuzzyHybridPN::MetadataRequirement(SP_DS_Metadata* p_pcMetadata)
{
	return SP_DS_HybridPN::MetadataRequirement(p_pcMetadata);
}

