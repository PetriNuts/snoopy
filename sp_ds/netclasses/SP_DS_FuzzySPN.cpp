//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: slehrack $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2007/06/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#include "sp_ds/netclasses/SP_DS_FuzzySPN.h"

#include "sp_ds/netclasses/SP_DS_EventSPN.h"

#include "sp_ds/SP_DS_Edgeclass.h"
#include "sp_ds/SP_DS_Nodeclass.h"
#include "sp_ds/SP_DS_Metadataclass.h"
#include "sp_ds/animators/ani_SPN/SP_DS_StPlaceAnimator.h"
#include "sp_ds/animators/ani_SPN/SP_DS_StTransAnimator.h"

//by sl
#include "sp_ds/animators/SP_DS_ExtPTPlaceAnimator.h"
#include "sp_ds/animators/SP_DS_ExtPTTransAnimator.h"
#include "sp_ds/attributes/SP_DS_TypeAttribute.h"
#include "sp_ds/attributes/SP_DS_MarkingAttribute.h"
#include "sp_ds/attributes/SP_DS_MarkingDependentMultiplicity.h"


#include "sp_ds/attributes/SP_DS_BoolAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/attributes/SP_DS_NumberAttribute.h"
#include "sp_ds/attributes/SP_DS_DoubleAttribute.h"
#include "sp_ds/attributes/SP_DS_LogicAttribute.h"
#include "sp_ds/attributes/SP_DS_IdAttribute.h"
#include "sp_ds/attributes/SP_DS_MultiplicityAttribute.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"

#include "sp_ds/attributes/SP_DS_ColListAttribute.h"

#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/extensions/SP_DS_Coarse.h"
#include "sp_ds/extensions/ext_SPN/SP_DS_StAnimation.h"

#include "sp_gr/shapes/SP_GR_Ellipse.h"
#include "sp_gr/shapes/SP_GR_Circle.h"
#include "sp_gr/shapes/SP_GR_Rectangle.h"
#include "sp_gr/shapes/SP_GR_ExtendedEllipse.h"

#include "sp_gr/edges/SP_GR_ArrowEdge.h"
#include "sp_gr/edges/SP_GR_ExtendedEdge.h"

#include "sp_gr/attributes/SP_GR_TextAttribute.h"
#include "sp_gr/attributes/SP_GR_NumberAttribute.h"
#include "sp_gr/attributes/SP_GR_MultiplicityAttribute.h"
#include "sp_gr/attributes/SP_GR_MarkAttribute.h"
#include "sp_gr/attributes/SP_GR_CommentAttribute.h"
#include "sp_gr/attributes/SP_GR_HiddenTextAttribute.h"

#include "sp_gr/attributes/gra_SPN/SP_GR_StFunctionAttribute.h"
#include "sp_gr/attributes/gra_SPN/SP_GR_StParameterListAttribute.h"

#include "sp_gui/widgets/dialogs/SP_WDG_DialogUnsigned.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogBool.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogShowOnly.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogGraphic.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogMultiline.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogExtendedGraphic.h"

#include "sp_gui/widgets/dialogs/wid_dia_SPN/SP_WDG_StFunctionList.h"
#include "sp_gui/widgets/dialogs/wid_dia_SPN/SP_WDG_StLookupTable.h"
#include "sp_gui/widgets/dialogs/wid_dia_SPN/SP_WDG_StLookupTableName.h"

#include "sp_gui/widgets/dialogs/SP_WDG_DialogColList.h" //Added By Liu on 6 Mar. 2009
#include "sp_gr/attributes/SP_GR_ColListAttribute.h" //Added By Liu on 6 Mar. 2009


#include "sp_utilities.h"

#include "snoopy.h"
#include "sp_core/SP_GPR_Elements.h"
#include "sp_core/SP_GPR_Animation.h"

#include "sp_gui/windows/SP_GUI_Mainframe.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_core/SP_Core.h"

SP_DS_FuzzySPN::SP_DS_FuzzySPN()
	:SP_DS_EventSPN(SP_DS_FUZZYSPN_CLASS)
{
	m_sExtension = wxT("fspn");
	m_sFilter = wxT("*.fspn;*.fuzzyspstochpn");

	//register node classes for editing their graphic properties

	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Place"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Transition"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Immediate Transition"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Deterministic Transition"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Scheduled Transition"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Parameter"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("LookupTable"));

}

SP_DS_FuzzySPN::SP_DS_FuzzySPN(const wxString& p_pchName)
	:SP_DS_EventSPN(p_pchName)
{
	m_sExtension = wxT("fspn");
	m_sFilter = wxT("*.fspn;*.fuzzyspstochpn");

	//register node classes for editing their graphic properties
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Place"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Transition"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Immediate Transition"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Deterministic Transition"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Scheduled Transition"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Parameter"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("LookupTable"));

}


SP_DS_Graph*
SP_DS_FuzzySPN::CreateGraph(SP_DS_Graph* p_pcGraph)
{

	//return SP_DS_EventSPN::CreateGraph(p_pcGraph);

	if (!SP_DS_EventSPN::CreateGraph(p_pcGraph))
	{
		return NULL;
	}
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

	return p_pcGraph;

}

bool
SP_DS_FuzzySPN::CreateInitialElements(SP_DS_Graph* p_pcGraph)
{
	SP_DS_EventSPN::CreateInitialElements(p_pcGraph);

	SP_DS_Metadataclass* l_pcMC;

	l_pcMC = p_pcGraph->GetMetadataclass(wxT("Simulation Properties"));
	l_pcMC->NewElement(1);

	l_pcMC = p_pcGraph->GetMetadataclass(wxT("Simulation Results"));
	l_pcMC->NewElement(1);

	l_pcMC = p_pcGraph->GetMetadataclass(wxT("FuzzySettings"));
	l_pcMC->NewElement(1);

	return true;
}

bool
  SP_DS_FuzzySPN::NodeRequirement(SP_DS_Node* p_pcNode)
{
	return SP_DS_EventSPN::NodeRequirement(p_pcNode);
}

bool
SP_DS_FuzzySPN::EdgeRequirement(SP_DS_Edgeclass* p_pcClass, SP_Data* p_pcSrc, SP_Data* p_pcTrg)
{

	SP_LOGDEBUG(wxString::Format(wxT("EdgeRequirement %p->%p %s"), p_pcSrc, p_pcTrg, p_pcClass->GetName().c_str()));
	if (!SP_DS_EventSPN::EdgeRequirement(p_pcClass, p_pcSrc, p_pcTrg))
		return false;

	if (p_pcSrc->GetClassName() == wxT("Parameter") ||
		p_pcSrc->GetClassName() == wxT("Coarse Parameter"))
		return FALSE;
	if (p_pcTrg->GetClassName() == wxT("Parameter") ||
		p_pcTrg->GetClassName() == wxT("Coarse Parameter"))
		return FALSE;

	// Added by Liu on 22 Mar. 2009
	if (((p_pcSrc->GetClassName() == wxT("Transition")) ||
		(p_pcSrc->GetClassName() == wxT("Immediate Transition")) ||
		(p_pcSrc->GetClassName() == wxT("Deterministic Transition")) ||
		(p_pcSrc->GetClassName() == wxT("Scheduled Transition"))) &&
		((p_pcTrg->GetClassName() == wxT("Transition")) ||
		(p_pcTrg->GetClassName() == wxT("Immediate Transition")) ||
			(p_pcTrg->GetClassName() == wxT("Deterministic Transition")) ||
			(p_pcTrg->GetClassName() == wxT("Scheduled Transition"))))
		return FALSE;

	if (p_pcClass->GetName() == wxT("Modifier Edge"))
	{
		if (!p_pcSrc->GetClassName().Contains(wxT("Place")) &&
			!p_pcTrg->GetClassName().Contains(wxT("Transition")))
		{
			SP_MESSAGEBOX(wxString(p_pcClass->GetName()) + wxT(" must connect places and transitions"), wxT("Error"), wxOK | wxICON_ERROR);

			return false;
		}
	}

	const SP_ListEdge* l_lpEdges = (dynamic_cast< SP_DS_Node* >(p_pcSrc))->GetSourceEdges();
	SP_ListEdge::const_iterator l_pIt;
	for (l_pIt = l_lpEdges->begin(); l_pIt != l_lpEdges->end(); ++l_pIt)
	{
		if ((*l_pIt)->GetTarget() == p_pcTrg
			&& (p_pcClass->GetName() == wxT("Modifier Edge")
				|| (*l_pIt)->GetClassName() == wxT("Modifier Edge")))
		{
			wxString l_sMsg = wxT("The nodes ");
			l_sMsg << dynamic_cast<SP_DS_NameAttribute*>(p_pcSrc->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
			l_sMsg << wxT(" and ");
			l_sMsg << dynamic_cast<SP_DS_NameAttribute*>(p_pcTrg->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
			l_sMsg << wxT(" are already connected via a ");
			l_sMsg << (*l_pIt)->GetClassName();
			l_sMsg << wxT(".");
			SP_MESSAGEBOX(l_sMsg, wxT("Error"),
				wxOK | wxICON_ERROR);
			return false;
		}
	}


	return true;
}

bool
SP_DS_FuzzySPN::MetadataRequirement(SP_DS_Metadata* p_pcMetadata)
{
	if (!SP_DS_EventSPN::MetadataRequirement(p_pcMetadata))
	{
		return false;
	}

	wxString l_sName = p_pcMetadata->GetClassName();
	if ((l_sName == wxT("Simulation Properties") || l_sName == wxT("Simulation Results"))
		&& !p_pcMetadata->GetMetadataclass()->GetElements()->empty())
	{
		return false;
	}

	return true;
}


