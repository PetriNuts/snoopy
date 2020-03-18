//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: G. Assaf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 02.03.2020 13:28:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#include "sp_ds/netclasses/SP_DS_Fuzzy_ColCPN.h"
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
#include "sp_ds/attributes/SP_DS_EquationAttribute.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"

#include "sp_gr/attributes/gra_SPN/SP_GR_StParameterListAttribute.h"
//for ColList
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"

#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/extensions/SP_DS_Coarse.h"
#include "sp_ds/extensions/SP_DS_ContinuousPedAnimation.h"

// extended graphical representation
#include "sp_gr/SP_GR_Node.h"
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


#include "sp_gr/attributes/gra_SPN/SP_GR_StFunctionAttribute.h"
//For list Dialog
#include "sp_gui/widgets/dialogs/wid_dia_SPN/SP_WDG_StDummyMarking.h"
#include "sp_gui/widgets/dialogs/wid_dia_SPN/SP_WDG_StParameterList.h"

#include "sp_gui/widgets/dialogs/SP_WDG_DialogChoice.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogText.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogBool.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogShowOnly.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogGraphic.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogExtendedGraphic.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogMultiline.h"

#include "sp_utilities.h"

#include "snoopy.h"
#include "sp_core/SP_GPR_Elements.h"
#include "sp_core/SP_GPR_Animation.h"

#include "sp_gui/widgets/dialogs/SP_WDG_DialogUnsigned.h"

#include "sp_ds/attributes/SP_DS_DoubleAttribute.h"

#include "sp_gr/attributes/SP_GR_ContinuousMarkAttribute.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"


//Special for colored SPN
#include "sp_gui/dialogs/dia_ColSPN/SP_WDG_ColStMarkingList.h"
#include "sp_gui/dialogs/dia_ColSPN/SP_WDG_ColStFunctionList.h"
#include "sp_gui/dialogs/dia_CPN/SP_WDG_ArcInscription.h"
#include "sp_gui/dialogs/dia_CPN/SP_WDG_Guard.h"

#include "sp_ds/attributes/SP_DS_TypeAttribute.h"//by george, for constants harmonizing 


SP_DS_Fuzzy_ColCPN::SP_DS_Fuzzy_ColCPN() :
	SP_DS_ColCPN(SP_DS_FUZZY_ColCPN_CLASS)
{
	//m_sNameVal = wxT("Colored Fuzzy Continuous Petri Net");
	m_sExtension = wxT("colfcpn");
	m_sFilter = wxT("*.colfcpn;*.colcontfped");
	//wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Continuous Place"));
	//wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Continuous Transition"));
	//wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), SP_DS_PARAM);
}

SP_DS_Fuzzy_ColCPN::SP_DS_Fuzzy_ColCPN(const wxString& p_pchName) :
	SP_DS_ColCPN(p_pchName)
{
	 
	m_sExtension = wxT("colfcpn");
	m_sFilter = wxT("*.colfcpn;*.colcontfped");
//	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Continuous Place"));
//	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Continuous Transition"));
//	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), SP_DS_PARAM);
}

SP_DS_Graph*
SP_DS_Fuzzy_ColCPN::CreateGraph(SP_DS_Graph* p_pcGraph)
{

	if (!SP_DS_ColCPN::CreateGraph(p_pcGraph))
		return NULL;

	SP_DS_Nodeclass* l_pcNC;
	SP_GR_Node* l_pcGr;
	SP_DS_Coarse* l_pcCoarse;
	SP_DS_Attribute* l_pcAttr;
	SP_Graphic* l_pcGrAttr;
	SP_DS_ColListAttribute* l_pcColList;
	unsigned int l_nNewRow;

	//////////////////////////////////////////////////////////////////////////////
 
	/***********george constants harmonizing************/
SP_DS_Metadataclass*	l_pcMC = p_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);

//	l_pcMC->SetDisplayName(wxT("Constants"));

	//l_pcMC->SetShowInElementTree(false);
	//l_pcMC->SetShowInDeclarationTreeOther(true);

	//l_pcAttr = l_pcMC->AddAttribute(new SP_DS_NameAttribute(wxT("Name")));
	//l_pcAttr = l_pcMC->AddAttribute(new SP_DS_IdAttribute(wxT("ID")));
	//l_pcAttr = l_pcMC->AddAttribute(new SP_DS_TextAttribute(wxT("Group")));
	//l_pcAttr = l_pcMC->AddAttribute(new SP_DS_TypeAttribute(wxT("Type"), wxT("int")));
     l_pcAttr = l_pcMC->AddAttribute(new SP_DS_TypeAttribute(wxT("Type"), wxT("int")));
     l_pcAttr = l_pcMC->GetPrototype()->GetAttribute(wxT("Type"));
     SP_DS_TypeAttribute* l_pcType = dynamic_cast< SP_DS_TypeAttribute* >(l_pcAttr);
      //l_pcType->AddPossibleValue(wxT("double"));
	  l_pcType->AddPossibleValue(wxT("TFN"));

	//l_pcAttr = l_pcMC->AddAttribute(new SP_DS_ColListAttribute(wxT("ValueList"), SP_COLLIST_STRING, 2, wxT("Group")));
	//l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcAttr);
	//l_pcColList->SetColLabel(0, wxT("Value set"));
	//l_pcColList->SetColLabel(1, wxT("Value"));
	//unsigned int l_nRow = l_pcColList->AppendEmptyRow();
	//l_pcColList->SetCell(l_nRow, 0, wxT("Main"));
	//l_pcColList->SetCell(l_nRow, 1, wxT(""));

	//l_pcAttr = l_pcMC->AddAttribute(new SP_DS_TextAttribute(wxT("Comment")));

	/**********************/
 
	/////////////////////////////////////////////////////////////////////////////





	return p_pcGraph;
}
bool
SP_DS_Fuzzy_ColCPN::CreateInitialElements(SP_DS_Graph* p_pcGraph)
{
	SP_DS_ContinuousPed::CreateInitialElements(p_pcGraph);

	SP_DS_Metadataclass* l_pcMC = (SP_DS_Metadataclass*)0;

	l_pcMC = p_pcGraph->GetMetadataclass(SP_DS_CPN_BASICCOLORSETCLASS);
	SP_DS_Metadata* l_pcColorset = l_pcMC->NewElement(1);
	if (l_pcColorset)
	{
		//l_pcColorset->GetAttribute( wxT("Name") )->SetValueString( wxT("NewColorset") );
		SP_DS_ColListAttribute* l_pcColAttr =
			dynamic_cast<SP_DS_ColListAttribute*>(l_pcColorset->GetAttribute(wxT("ColorsetList")));
		l_pcColAttr->AppendEmptyRow();
		l_pcColAttr->SetCell(0, 0, wxT("Dot"));
		l_pcColAttr->SetCell(0, 1, wxT("dot"));
		l_pcColAttr->SetCell(0, 2, wxT("dot"));
	}

	l_pcMC = p_pcGraph->GetMetadataclass(SP_DS_CPN_STRUCTUREDCOLORSETCLASS);
	SP_DS_Metadata* l_pcStructuredColorset = l_pcMC->NewElement(1);
	if (l_pcStructuredColorset)
	{
		//l_pcStructuredColorset->GetAttribute( wxT("Name") )->SetValueString( wxT("NewColorset") );
	}

	l_pcMC = p_pcGraph->GetMetadataclass(SP_DS_CPN_ALIASCOLORSETCLASS);
	SP_DS_Metadata* l_pcAliasColorset = l_pcMC->NewElement(1);
	if (l_pcAliasColorset)
	{
		//l_pcAliasColorset->GetAttribute( wxT("Name") )->SetValueString( wxT("NewColorset") );
	}

	//l_pcMC = p_pcGraph->GetMetadataclass(SP_DS_CPN_CONSTANTCLASS);
	//SP_DS_Metadata* l_pcConstant = l_pcMC->NewElement(1);
	//if (l_pcConstant)
	//{
		//l_pcConstant->GetAttribute( wxT("Name") )->SetValueString( wxT("NewConstant") );
	//}

	l_pcMC = p_pcGraph->GetMetadataclass(SP_DS_CPN_VARIABLECLASS);
	SP_DS_Metadata* l_pcVariable = l_pcMC->NewElement(1);
	if (l_pcVariable)
	{
		//l_pcVariable->GetAttribute( wxT("Name") )->SetValueString( wxT("NewVariable") );
	}

	l_pcMC = p_pcGraph->GetMetadataclass(SP_DS_CPN_FUNCTIONCLASS);
	SP_DS_Metadata* l_pcFunction = l_pcMC->NewElement(1);
	if (l_pcFunction)
	{
		//l_pcFunction->GetAttribute( wxT("Name") )->SetValueString( wxT("NewFunction") );
	}

	l_pcMC = p_pcGraph->GetMetadataclass(SP_DS_CPN_AUXILIARVARIABLESCLASS);
	SP_DS_Metadata* l_pcAuxiVar = l_pcMC->NewElement(1);
	if (l_pcAuxiVar)
	{
		//l_pcAuxiVar->GetAttribute( wxT("Name") )->SetValueString( wxT("NewAuxiliaryVariable") );
	}
	return true;
}

bool SP_DS_Fuzzy_ColCPN::NodeRequirement(SP_DS_Node* p_pcNode)
{
	return SP_DS_ContinuousPed::NodeRequirement(p_pcNode);
}

bool SP_DS_Fuzzy_ColCPN::EdgeRequirement(SP_DS_Edgeclass* p_pcClass, SP_Data* p_pcSrc, SP_Data* p_pcTrg)
{
	return SP_DS_ColCPN::EdgeRequirement(p_pcClass, p_pcSrc, p_pcTrg);
}

bool SP_DS_Fuzzy_ColCPN::MetadataRequirement(SP_DS_Metadata* p_pcMetadata)
{
	if (!SP_DS_ColCPN::MetadataRequirement(p_pcMetadata))
	{
		return false;
	}

	wxString l_sName1 = p_pcMetadata->GetClassName();//george for constnt harmonizing
	if (l_sName1 == wxT("Constant Class"))//george for constnt harmonizing
	{
		return true;
	}

	wxString l_sName = p_pcMetadata->GetClassName();
	if ((l_sName == SP_DS_CPN_BASICCOLORSETCLASS
		|| l_sName == SP_DS_CPN_STRUCTUREDCOLORSETCLASS
		|| l_sName == SP_DS_CPN_ALIASCOLORSETCLASS
		|| l_sName == SP_DS_CPN_CONSTANTCLASS
		|| l_sName == SP_DS_CPN_VARIABLECLASS
		|| l_sName == SP_DS_CPN_FUNCTIONCLASS
		|| l_sName == SP_DS_CPN_AUXILIARVARIABLESCLASS)
		&& !p_pcMetadata->GetMetadataclass()->GetElements()->empty())
	{
		return false;
	}

	return true;
}

