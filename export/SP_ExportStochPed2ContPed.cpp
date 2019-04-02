//////////////////////////////////////////////////////////////////////
// $Author: slehrack $
// $Version: 0.1 $
// $Date: 2007/12/01 $
//////////////////////////////////////////////////////////////////////

#include "export/SP_ExportStochPed2ContPed.h"
#include "sp_core/base/SP_Graphic.h"

#include "sp_ds/netclasses/SP_DS_ContinuousPed.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"

#include "sp_ds/attributes/SP_DS_TextAttribute.h"
#include "sp_ds/attributes/SP_DS_NumberAttribute.h"
#include "sp_ds/attributes/SP_DS_EquationAttribute.h"

#include "sp_gr/attributes/SP_GR_DoubleMarkAttribute.h"
#include "sp_gr/attributes/SP_GR_TextAttribute.h"
#include "sp_ds/attributes/SP_DS_IdAttribute.h"

#include "snoopy.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_core/SP_GPR_Elements.h"
#include "sp_core/SP_GPR_Animation.h"
#include "sp_ds/netclasses/SP_DS_HybridPN.h"

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


bool SP_ExportStochPed2ContPed::WriteNodeclass(SP_DS_Nodeclass* p_pcVal,
		wxXmlNode* p_pcRoot)
{

	CHECK_POINTER( p_pcVal, return FALSE );
	CHECK_POINTER( p_pcRoot, return FALSE );
	

	SP_ListNode::const_iterator l_Iter;
	const SP_ListNode* l_plElements = p_pcVal->GetElements();

	CHECK_POINTER( l_plElements, return FALSE );
	wxString l_sNodeclassName = p_pcVal->GetName();

	if (wxT("Place") == l_sNodeclassName)
	{
		l_sNodeclassName = SP_DS_CONTINUOUS_PLACE;
		wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE,  wxT("nodeclass"));
	    l_pcElem->AddAttribute( wxT("count"), wxString::Format( wxT("%d"),  l_plElements->size()));
		l_pcElem->AddAttribute( wxT("name"),  l_sNodeclassName );
		p_pcRoot->AddChild(l_pcElem);

		for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
		{
			SP_DS_Nodeclass*	l_pcConvertToNodeClass;// = m_graph->AddNodeclass(new SP_DS_Nodeclass(m_graph, wxT("Continuous Place")));
			l_pcConvertToNodeClass = m_graph->AddNodeclass(new SP_DS_Nodeclass(m_graph, SP_DS_CONTINUOUS_PLACE, m_graph->GetNodeclass(SP_DS_DISCRETE_PLACE)->GetIdCountPtr()));

			/* there is no Continuous place in our current (SPN) Ped, so we need to
			create a new one and initialize it with its default attributes, so that
			we can make copy of the attributes of Discrete place to the new Continuous Place.

			*/
			SP_DS_Nodeclass* l_pcNC;
			SP_DS_Attribute* l_pcAttr;
			SP_GR_Node* l_pcGr;
			SP_Graphic* l_pcGrAttr;
			l_pcNC = m_graph->AddNodeclass(new SP_DS_Nodeclass(m_graph, SP_DS_CONTINUOUS_PLACE, m_graph->GetNodeclass(SP_DS_DISCRETE_PLACE)->GetIdCountPtr()));
			l_pcNC->SetShortcut(wxT("Shift+P"));
			l_pcNC->SetDisplayName(wxT("Continuous Place"));

			l_pcAttr = l_pcNC->AddAttribute(new SP_DS_BoolAttribute(wxT("Fixed"), FALSE));//Fixed
			l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogBool(wxT("General"), 1));

			l_pcAttr = l_pcNC->AddAttribute(new SP_DS_NameAttribute(wxT("Name")));
			l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General")));
			l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
			l_pcGrAttr->SetOffsetY(20);
			l_pcGrAttr->SetShow(TRUE);
			l_pcAttr->SetGlobalShow();

			l_pcAttr = l_pcNC->AddAttribute(new SP_DS_IdAttribute(wxT("ID")));
			l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogShowOnly(wxT("General")));
			l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_NumberAttribute(l_pcAttr, wxT("_pa%_")));
			l_pcGrAttr->SetOffsetX(20);
			l_pcGrAttr->SetShow(FALSE);
			l_pcAttr->SetGlobalShow();


			l_pcAttr = l_pcNC->AddAttribute(new SP_DS_DoubleMarkingAttribute(wxT("Marking"), 0.0));
			l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General")));
			l_pcAttr->AddGraphic(new SP_GR_MarkNumberAttribute(l_pcAttr));


			int pwidth = wxGetApp().GetElementPrefs()->GetNodeWidth(GetName(), SP_DS_CONTINUOUS_PLACE);
			l_pcGr = new SP_GR_ExtendedCircle(l_pcNC->GetPrototype(), pwidth, SP_EXTENDED_TYPE_DEFAULT, 3, wxColour(128, 128, 128));
			l_pcGr->SetDefaultPen(wxThePenList->FindOrCreatePen(wxColour(128, 128, 128), 3));
			l_pcGr->SetFixedSize(wxGetApp().GetElementPrefs()->GetNodeFixed(GetName(), l_pcNC->GetName()));
			l_pcNC->SetGraphic(l_pcGr);
			l_pcNC->RegisterGraphicWidget(new SP_WDG_DialogGraphic(wxT("Graphic")));

			l_pcAttr = l_pcNC->AddAttribute(new SP_DS_LogicAttribute(wxT("Logic"), wxT("Name")));
			l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogBool(wxT("General")));

			l_pcAttr = l_pcNC->AddAttribute(new SP_DS_TextAttribute(wxT("Comment"), wxT("")));
			//l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
			l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
			l_pcGrAttr->SetOffsetY(40);
			l_pcAttr->SetGlobalShow();
			/* to this point, a new Cont. place is ready to be created from Discrete one via convert method in our converter*/
			SP_DS_Nodeclass* l_pcConvertToNodeClass1 = m_graph->GetNodeclassByDisplayedName(SP_DS_CONTINUOUS_PLACE);
			SP_DS_Node* ConvertedNode1 = m_converter.Clone((**l_Iter), *l_pcNC);
			SP_DS_Attribute* nameAttr = ConvertedNode1->GetAttribute(wxT("ID"));
			wxString valString = nameAttr->GetValueString();
			m_sIds.push_back(valString);
			WritePlace(ConvertedNode1, l_pcElem);
		}
	}
	else if (wxT("Transition") == l_sNodeclassName)
	{
		l_sNodeclassName = SP_DS_CONTINUOUS_TRANS;
		wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE,  wxT("nodeclass"));		
		m_pcTransitionNodeclass = l_pcElem;

		m_nTransitionNumber=m_nTran+m_nImmediateTran+m_nDeterministicTran+m_nScheduledTran;

	    l_pcElem->AddAttribute( wxT("count"), wxString::Format( wxT("%d"), m_nTransitionNumber));
		l_pcElem->AddAttribute( wxT("name"),  l_sNodeclassName );
		p_pcRoot->AddChild(l_pcElem);

		m_nTransitionNumber=0;
		for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
		{
			SP_DS_Nodeclass* l_pcNC;
			SP_DS_Attribute* l_pcAttr;
			SP_GR_Node* l_pcGr;
			SP_Graphic* l_pcGrAttr;
			SP_DS_ColListAttribute* l_pcColList;
			unsigned int l_nNewRow;

			SP_DS_Node* l_pcNode = *l_Iter;
			wxString l_sTransitionName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
			wxString m_newType = wxT("Continuous Transition");//to be removed
			SP_DS_Nodeclass* l_pcConvertToNodeClass = m_graph->GetNodeclassByDisplayedName(m_newType);//to be removed

			l_pcNC = m_graph->AddNodeclass(new SP_DS_Nodeclass(m_graph,
				SP_DS_CONTINUOUS_TRANS,
				m_graph->GetNodeclass(SP_DS_STOCHASTIC_TRANS)->GetIdCountPtr()));
			l_pcNC->SetDisplayName(wxT("Continuous Transition"));

			l_pcNC->SetShortcut(wxT("Shift+T"));
			l_pcAttr = l_pcNC->AddAttribute(new SP_DS_NameAttribute(wxT("Name"), wxT("")));
			l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General"), 1));
			l_pcAttr->SetGlobalShow();
			l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
			l_pcGrAttr->SetOffsetX(25);
			l_pcGrAttr->SetOffsetY(20);

			l_pcAttr = l_pcNC->AddAttribute(new SP_DS_IdAttribute(wxT("ID")));
			l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogShowOnly(wxT("General")));
			l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_NumberAttribute(l_pcAttr, wxT("_it%_")));
			l_pcGrAttr->SetOffsetX(25);
			l_pcGrAttr->SetOffsetY(20);
			l_pcGrAttr->SetShow(FALSE);
			l_pcAttr->SetGlobalShow();
			l_pcAttr = l_pcNC->AddAttribute(new SP_DS_LogicAttribute(wxT("Logic"), wxT("Name")));
			l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogBool(wxT("General")));
			l_pcAttr = l_pcNC->AddAttribute(new SP_DS_TextAttribute(wxT("Comment"), wxT("")));
			l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
			l_pcAttr->SetGlobalShow();
			l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
			l_pcGrAttr->SetOffsetX(25);
			l_pcGrAttr->SetOffsetX(40);

			l_pcAttr = l_pcNC->AddAttribute(new SP_DS_ColListAttribute(wxT("FunctionList"), SP_COLLIST_STRING, 2));
			l_pcAttr->SetDisplayName(wxT("Function set"));
			l_pcAttr->RegisterDialogWidget(new SP_WDG_StFunctionList(wxT("Functions")));
			l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_ColListAttribute(l_pcAttr));
			l_pcGrAttr->SetOffsetX(25);
			l_pcGrAttr->SetOffsetY(30);
			l_pcGrAttr->SetShow(FALSE);
			l_pcAttr->SetGlobalShow();
			l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcAttr);
			l_pcColList->SetColLabel(0, wxT("Function set"));
			l_pcColList->SetColLabel(1, wxT("Function"));
			l_nNewRow = l_pcColList->AppendEmptyRow();
			l_pcColList->SetCell(l_nNewRow, 0, wxT("Main"));
			l_pcColList->SetCell(l_nNewRow, 1, wxT("MassAction(1)"));
			int twidth = wxGetApp().GetElementPrefs()->GetNodeWidth(GetName(), SP_DS_CONTINUOUS_TRANS);
			int theight = wxGetApp().GetElementPrefs()->GetNodeHeight(GetName(), SP_DS_CONTINUOUS_TRANS);
			l_pcGr = new SP_GR_ExtendedRectangle(l_pcNC->GetPrototype(), twidth, theight, TRUE, SP_EXTENDED_TYPE_DEFAULT, 3, wxColour(128, 128, 128));
			l_pcGr->SetDefaultPen(wxThePenList->FindOrCreatePen(wxColour(128, 128, 128), 3));
			l_pcGr->SetFixedSize(wxGetApp().GetElementPrefs()->GetNodeFixed(GetName(), l_pcNC->GetName()));
			l_pcNC->SetGraphic(l_pcGr);
			l_pcNC->RegisterGraphicWidget(new SP_WDG_DialogGraphic(wxT("Graphic")));
			SP_DS_Node* ConvertedNode = m_converter.Clone((**l_Iter), *l_pcNC);
			SP_DS_Attribute* nameAttr = ConvertedNode->GetAttribute(wxT("ID"));
			wxString valString = nameAttr->GetValueString();
			m_sIds.push_back(valString);
			WriteTransition( (ConvertedNode), l_pcElem);
		}
	}
	else if((wxT("Immediate Transition") == l_sNodeclassName ) )
	{
		l_sNodeclassName = SP_DS_CONTINUOUS_TRANS;
		wxXmlNode* l_pcElem = m_pcTransitionNodeclass;			
		m_nTransitionNumber=m_nTran;

		for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
		{
			SP_DS_Nodeclass* l_pcNC;
			SP_DS_Attribute* l_pcAttr;
			SP_GR_Node* l_pcGr;
			SP_Graphic* l_pcGrAttr;
			SP_DS_ColListAttribute* l_pcColList;
			unsigned int l_nNewRow;

			SP_DS_Node* l_pcNode = *l_Iter;
			wxString l_sTransitionName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
			wxString m_newType = wxT("Continuous Transition");//to be removed
			SP_DS_Nodeclass* l_pcConvertToNodeClass = m_graph->GetNodeclassByDisplayedName(m_newType);//to be removed

			l_pcNC = m_graph->AddNodeclass(new SP_DS_Nodeclass(m_graph,
				SP_DS_CONTINUOUS_TRANS,
				m_graph->GetNodeclass(SP_DS_STOCHASTIC_TRANS)->GetIdCountPtr()));
			l_pcNC->SetDisplayName(wxT("Continuous Transition"));

			l_pcNC->SetShortcut(wxT("Shift+T"));
			l_pcAttr = l_pcNC->AddAttribute(new SP_DS_NameAttribute(wxT("Name"), wxT("")));
			l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General"), 1));
			l_pcAttr->SetGlobalShow();
			l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
			l_pcGrAttr->SetOffsetX(25);
			l_pcGrAttr->SetOffsetY(20);

			l_pcAttr = l_pcNC->AddAttribute(new SP_DS_IdAttribute(wxT("ID")));
			l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogShowOnly(wxT("General")));
			l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_NumberAttribute(l_pcAttr, wxT("_it%_")));
			l_pcGrAttr->SetOffsetX(25);
			l_pcGrAttr->SetOffsetY(20);
			l_pcGrAttr->SetShow(FALSE);
			l_pcAttr->SetGlobalShow();
			l_pcAttr = l_pcNC->AddAttribute(new SP_DS_LogicAttribute(wxT("Logic"), wxT("Name")));
			l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogBool(wxT("General")));
			l_pcAttr = l_pcNC->AddAttribute(new SP_DS_TextAttribute(wxT("Comment"), wxT("")));
			l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
			l_pcAttr->SetGlobalShow();
			l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
			l_pcGrAttr->SetOffsetX(25);
			l_pcGrAttr->SetOffsetX(40);

			l_pcAttr = l_pcNC->AddAttribute(new SP_DS_ColListAttribute(wxT("FunctionList"), SP_COLLIST_STRING, 2));
			l_pcAttr->SetDisplayName(wxT("Function set"));
			l_pcAttr->RegisterDialogWidget(new SP_WDG_StFunctionList(wxT("Functions")));
			l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_ColListAttribute(l_pcAttr));
			l_pcGrAttr->SetOffsetX(25);
			l_pcGrAttr->SetOffsetY(30);
			l_pcGrAttr->SetShow(FALSE);
			l_pcAttr->SetGlobalShow();
			l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcAttr);
			l_pcColList->SetColLabel(0, wxT("Function set"));
			l_pcColList->SetColLabel(1, wxT("Function"));
			l_nNewRow = l_pcColList->AppendEmptyRow();
			l_pcColList->SetCell(l_nNewRow, 0, wxT("Main"));
			l_pcColList->SetCell(l_nNewRow, 1, wxT("MassAction(1)"));
			int twidth = wxGetApp().GetElementPrefs()->GetNodeWidth(GetName(), SP_DS_CONTINUOUS_TRANS);
			int theight = wxGetApp().GetElementPrefs()->GetNodeHeight(GetName(), SP_DS_CONTINUOUS_TRANS);
			l_pcGr = new SP_GR_ExtendedRectangle(l_pcNC->GetPrototype(), twidth, theight, TRUE, SP_EXTENDED_TYPE_DEFAULT, 3, wxColour(128, 128, 128));
			l_pcGr->SetDefaultPen(wxThePenList->FindOrCreatePen(wxColour(128, 128, 128), 3));
			l_pcGr->SetFixedSize(wxGetApp().GetElementPrefs()->GetNodeFixed(GetName(), l_pcNC->GetName()));
			l_pcNC->SetGraphic(l_pcGr);
			l_pcNC->RegisterGraphicWidget(new SP_WDG_DialogGraphic(wxT("Graphic")));
			SP_DS_Node* ConvertedNode = m_converter.Clone((**l_Iter), *l_pcNC);
			SP_DS_Attribute* nameAttr = ConvertedNode->GetAttribute(wxT("ID"));
			wxString valString = nameAttr->GetValueString();
			m_sIds.push_back(valString);
			WriteTransition( (ConvertedNode), l_pcElem);
		}
	}
	else if((wxT("Deterministic Transition") == l_sNodeclassName ) )
	{
		l_sNodeclassName = SP_DS_CONTINUOUS_TRANS;
		wxXmlNode* l_pcElem = m_pcTransitionNodeclass;
		m_nTransitionNumber=m_nTran+m_nImmediateTran;
		for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
		{

			SP_DS_Nodeclass* l_pcNC;
			SP_DS_Attribute* l_pcAttr;
			SP_GR_Node* l_pcGr;
			SP_Graphic* l_pcGrAttr;
			SP_DS_ColListAttribute* l_pcColList;
			unsigned int l_nNewRow;

			SP_DS_Node* l_pcNode = *l_Iter;
			wxString l_sTransitionName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
			wxString m_newType = wxT("Continuous Transition");//to be removed
			SP_DS_Nodeclass* l_pcConvertToNodeClass = m_graph->GetNodeclassByDisplayedName(m_newType);//to be removed

			l_pcNC = m_graph->AddNodeclass(new SP_DS_Nodeclass(m_graph,
				SP_DS_CONTINUOUS_TRANS,
				m_graph->GetNodeclass(SP_DS_STOCHASTIC_TRANS)->GetIdCountPtr()));
			l_pcNC->SetDisplayName(wxT("Continuous Transition"));

			l_pcNC->SetShortcut(wxT("Shift+T"));
			l_pcAttr = l_pcNC->AddAttribute(new SP_DS_NameAttribute(wxT("Name"), wxT("")));
			l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General"), 1));
			l_pcAttr->SetGlobalShow();
			l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
			l_pcGrAttr->SetOffsetX(25);
			l_pcGrAttr->SetOffsetY(20);

			l_pcAttr = l_pcNC->AddAttribute(new SP_DS_IdAttribute(wxT("ID")));
			l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogShowOnly(wxT("General")));
			l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_NumberAttribute(l_pcAttr, wxT("_it%_")));
			l_pcGrAttr->SetOffsetX(25);
			l_pcGrAttr->SetOffsetY(20);
			l_pcGrAttr->SetShow(FALSE);
			l_pcAttr->SetGlobalShow();
			l_pcAttr = l_pcNC->AddAttribute(new SP_DS_LogicAttribute(wxT("Logic"), wxT("Name")));
			l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogBool(wxT("General")));
			l_pcAttr = l_pcNC->AddAttribute(new SP_DS_TextAttribute(wxT("Comment"), wxT("")));
			l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
			l_pcAttr->SetGlobalShow();
			l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
			l_pcGrAttr->SetOffsetX(25);
			l_pcGrAttr->SetOffsetX(40);

			l_pcAttr = l_pcNC->AddAttribute(new SP_DS_ColListAttribute(wxT("FunctionList"), SP_COLLIST_STRING, 2));
			l_pcAttr->SetDisplayName(wxT("Function set"));
			l_pcAttr->RegisterDialogWidget(new SP_WDG_StFunctionList(wxT("Functions")));
			l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_ColListAttribute(l_pcAttr));
			l_pcGrAttr->SetOffsetX(25);
			l_pcGrAttr->SetOffsetY(30);
			l_pcGrAttr->SetShow(FALSE);
			l_pcAttr->SetGlobalShow();
			l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcAttr);
			l_pcColList->SetColLabel(0, wxT("Function set"));
			l_pcColList->SetColLabel(1, wxT("Function"));
			l_nNewRow = l_pcColList->AppendEmptyRow();
			l_pcColList->SetCell(l_nNewRow, 0, wxT("Main"));
			l_pcColList->SetCell(l_nNewRow, 1, wxT("MassAction(1)"));
			int twidth = wxGetApp().GetElementPrefs()->GetNodeWidth(GetName(), SP_DS_CONTINUOUS_TRANS);
			int theight = wxGetApp().GetElementPrefs()->GetNodeHeight(GetName(), SP_DS_CONTINUOUS_TRANS);
			l_pcGr = new SP_GR_ExtendedRectangle(l_pcNC->GetPrototype(), twidth, theight, TRUE, SP_EXTENDED_TYPE_DEFAULT, 3, wxColour(128, 128, 128));
			l_pcGr->SetDefaultPen(wxThePenList->FindOrCreatePen(wxColour(128, 128, 128), 3));
			l_pcGr->SetFixedSize(wxGetApp().GetElementPrefs()->GetNodeFixed(GetName(), l_pcNC->GetName()));
			l_pcNC->SetGraphic(l_pcGr);
			l_pcNC->RegisterGraphicWidget(new SP_WDG_DialogGraphic(wxT("Graphic")));
			SP_DS_Node* ConvertedNode = m_converter.Clone((**l_Iter), *l_pcNC);
			SP_DS_Attribute* nameAttr = ConvertedNode->GetAttribute(wxT("ID"));
			wxString valString = nameAttr->GetValueString();
			m_sIds.push_back(valString);
			WriteTransition((ConvertedNode), l_pcElem);
		}
	}
			  
	else if((wxT("Scheduled Transition") == l_sNodeclassName ) )
	{
		l_sNodeclassName = SP_DS_CONTINUOUS_TRANS;
		wxXmlNode* l_pcElem = m_pcTransitionNodeclass;
		m_nTransitionNumber=m_nTran+m_nImmediateTran+m_nDeterministicTran;
		for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
		{

			SP_DS_Nodeclass* l_pcNC;
			SP_DS_Attribute* l_pcAttr;
			SP_GR_Node* l_pcGr;
			SP_Graphic* l_pcGrAttr;
			SP_DS_ColListAttribute* l_pcColList;
			unsigned int l_nNewRow;

			SP_DS_Node* l_pcNode = *l_Iter;
			wxString l_sTransitionName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
			wxString m_newType = wxT("Continuous Transition");//to be removed
			SP_DS_Nodeclass* l_pcConvertToNodeClass = m_graph->GetNodeclassByDisplayedName(m_newType);//to be removed

			l_pcNC = m_graph->AddNodeclass(new SP_DS_Nodeclass(m_graph,
				SP_DS_CONTINUOUS_TRANS,
				m_graph->GetNodeclass(SP_DS_STOCHASTIC_TRANS)->GetIdCountPtr()));
			l_pcNC->SetDisplayName(wxT("Continuous Transition"));

			l_pcNC->SetShortcut(wxT("Shift+T"));
			l_pcAttr = l_pcNC->AddAttribute(new SP_DS_NameAttribute(wxT("Name"), wxT("")));
			l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General"), 1));
			l_pcAttr->SetGlobalShow();
			l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
			l_pcGrAttr->SetOffsetX(25);
			l_pcGrAttr->SetOffsetY(20);

			l_pcAttr = l_pcNC->AddAttribute(new SP_DS_IdAttribute(wxT("ID")));
			l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogShowOnly(wxT("General")));
			l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_NumberAttribute(l_pcAttr, wxT("_it%_")));
			l_pcGrAttr->SetOffsetX(25);
			l_pcGrAttr->SetOffsetY(20);
			l_pcGrAttr->SetShow(FALSE);
			l_pcAttr->SetGlobalShow();
			l_pcAttr = l_pcNC->AddAttribute(new SP_DS_LogicAttribute(wxT("Logic"), wxT("Name")));
			l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogBool(wxT("General")));
			l_pcAttr = l_pcNC->AddAttribute(new SP_DS_TextAttribute(wxT("Comment"), wxT("")));
			l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
			l_pcAttr->SetGlobalShow();
			l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
			l_pcGrAttr->SetOffsetX(25);
			l_pcGrAttr->SetOffsetX(40);

			l_pcAttr = l_pcNC->AddAttribute(new SP_DS_ColListAttribute(wxT("FunctionList"), SP_COLLIST_STRING, 2));
			l_pcAttr->SetDisplayName(wxT("Function set"));
			l_pcAttr->RegisterDialogWidget(new SP_WDG_StFunctionList(wxT("Functions")));
			l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_ColListAttribute(l_pcAttr));
			l_pcGrAttr->SetOffsetX(25);
			l_pcGrAttr->SetOffsetY(30);
			l_pcGrAttr->SetShow(FALSE);
			l_pcAttr->SetGlobalShow();
			l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcAttr);
			l_pcColList->SetColLabel(0, wxT("Function set"));
			l_pcColList->SetColLabel(1, wxT("Function"));
			l_nNewRow = l_pcColList->AppendEmptyRow();
			l_pcColList->SetCell(l_nNewRow, 0, wxT("Main"));
			l_pcColList->SetCell(l_nNewRow, 1, wxT("MassAction(1)"));
			int twidth = wxGetApp().GetElementPrefs()->GetNodeWidth(GetName(), SP_DS_CONTINUOUS_TRANS);
			int theight = wxGetApp().GetElementPrefs()->GetNodeHeight(GetName(), SP_DS_CONTINUOUS_TRANS);
			l_pcGr = new SP_GR_ExtendedRectangle(l_pcNC->GetPrototype(), twidth, theight, TRUE, SP_EXTENDED_TYPE_DEFAULT, 3, wxColour(128, 128, 128));
			l_pcGr->SetDefaultPen(wxThePenList->FindOrCreatePen(wxColour(128, 128, 128), 3));
			l_pcGr->SetFixedSize(wxGetApp().GetElementPrefs()->GetNodeFixed(GetName(), l_pcNC->GetName()));
			l_pcNC->SetGraphic(l_pcGr);
			l_pcNC->RegisterGraphicWidget(new SP_WDG_DialogGraphic(wxT("Graphic")));
			SP_DS_Node* ConvertedNode = m_converter.Clone((**l_Iter), *l_pcNC);
			SP_DS_Attribute* nameAttr = ConvertedNode->GetAttribute(wxT("ID"));
			wxString valString = nameAttr->GetValueString();
			m_sIds.push_back(valString);
			WriteTransition((ConvertedNode), l_pcElem);
		}
	} 
	else if (wxT("Parameter") == l_sNodeclassName)
	{
		l_sNodeclassName = SP_DS_PARAM;

		wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE,  wxT("nodeclass"));
	    l_pcElem->AddAttribute( wxT("count"), wxString::Format( wxT("%d"),  l_plElements->size()));
		l_pcElem->AddAttribute( wxT("name"),  l_sNodeclassName );
		p_pcRoot->AddChild(l_pcElem);

		for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
		{
			WriteParameter( ( *l_Iter ), l_pcElem);
		}
	}
	else if((wxT("Coarse Parameter") == l_sNodeclassName ) ||
			(wxT("Coarse Place") == l_sNodeclassName ) ||
			(wxT("Coarse Transition") == l_sNodeclassName )	||
			(wxT("Comment") == l_sNodeclassName ))
	{
		if (wxT("Coarse Parameter") == l_sNodeclassName)
		{
			l_sNodeclassName = SP_DS_COARSE_PARAMETER;
		}
		else if (wxT("Coarse Place") == l_sNodeclassName)
		{
			l_sNodeclassName = SP_DS_COARSE_PLACE;
		}
		else if (wxT("Coarse Transition") == l_sNodeclassName)
		{
			l_sNodeclassName = SP_DS_COARSE_TRANSITION;
		}
		else if (wxT("Comment") == l_sNodeclassName)
		{
			l_sNodeclassName = wxT("Comment");
		}

		wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE,  wxT("nodeclass"));
	    l_pcElem->AddAttribute( wxT("count"), wxString::Format( wxT("%d"),  l_plElements->size()));
		l_pcElem->AddAttribute( wxT("name"),  l_sNodeclassName );
		p_pcRoot->AddChild(l_pcElem);

		for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
		{
			WriteNode( ( *l_Iter ), l_pcElem);
		}
	}

	return TRUE;

}

bool SP_ExportStochPed2ContPed::WritePlace(SP_DS_Node* p_pcVal,
		wxXmlNode* p_pcRoot)
{

	CHECK_POINTER( p_pcVal, return FALSE );
	CHECK_POINTER( p_pcRoot, return FALSE );
	

	SP_ListAttribute::const_iterator l_Iter;
	const SP_ListAttribute* l_plAttributes = p_pcVal->GetAttributes();
	CHECK_POINTER( l_plAttributes, return FALSE );

	wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE,  wxT("node"));
	p_pcRoot->AddChild(l_pcElem);

	for (l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter)
	{

			WriteAttribute( ( *l_Iter ), l_pcElem);

	}

	return WriteData(p_pcVal, l_pcElem);

}

bool SP_ExportStochPed2ContPed::WriteTransition(SP_DS_Node* p_pcVal,
		wxXmlNode* p_pcRoot)
{
	CHECK_POINTER( p_pcVal, return FALSE );
	CHECK_POINTER( p_pcRoot, return FALSE );
	

	SP_ListAttribute::const_iterator l_Iter;
	const SP_ListAttribute* l_plAttributes = p_pcVal->GetAttributes();
	CHECK_POINTER( l_plAttributes, return FALSE );

	wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE,  wxT("node"));
	p_pcRoot->AddChild(l_pcElem);

	for (l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter)
	{

	    if (( *l_Iter )->GetName() == wxT("DelayList"))
		{

		}
		else if (( *l_Iter )->GetName() == wxT("PeriodicList"))
		{

		}
		else
		{
			WriteAttribute( ( *l_Iter ), l_pcElem);
		}
	}
	return WriteData(p_pcVal, l_pcElem);
}

bool SP_ExportStochPed2ContPed::WriteParameter(SP_DS_Node* p_pcVal,
		wxXmlNode* p_pcRoot)
{

	CHECK_POINTER( p_pcVal, return FALSE );
	CHECK_POINTER( p_pcRoot, return FALSE );
	

	SP_ListAttribute::const_iterator l_Iter;
	const SP_ListAttribute* l_plAttributes = p_pcVal->GetAttributes();
	CHECK_POINTER( l_plAttributes, return FALSE );

	wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE,  wxT("node"));
	p_pcRoot->AddChild(l_pcElem);

	for (l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter)
	{


		WriteAttribute( ( *l_Iter ), l_pcElem);

	}

	return WriteData(p_pcVal, l_pcElem);

}

bool SP_ExportStochPed2ContPed::Write(SP_MDI_Doc* p_doc,
		const wxString& p_fileName)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	CHECK_BOOL((!p_fileName.IsEmpty()), return false);

	m_doc = p_doc;
	m_graph = m_doc->GetGraph();

	/////By Liu
	m_nTran = m_graph->GetNodeclass(wxT("Transition"))->GetElements()->size();
	m_nImmediateTran = m_graph->GetNodeclass(wxT("Immediate Transition"))->GetElements()->size();
	m_nDeterministicTran = m_graph->GetNodeclass(wxT("Deterministic Transition"))->GetElements()->size();
    m_nScheduledTran = m_graph->GetNodeclass(wxT("Scheduled Transition"))->GetElements()->size();		 
	/////
	m_fileName = p_fileName;
	return SP_XmlWriter::Write(m_graph, m_fileName);
}

bool SP_ExportStochPed2ContPed::WriteNetclass(SP_DS_Netclass* p_pcVal,
		wxXmlNode* p_pcRoot)
{
	CHECK_POINTER(p_pcVal, return FALSE);
	CHECK_POINTER(p_pcRoot, return FALSE);
	

	wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("netclass"));
	l_pcElem->AddAttribute(wxT("name"), SP_DS_CONTINUOUSPED_CLASS);
	p_pcRoot->AddChild(l_pcElem);

	return TRUE;
}

bool SP_ExportStochPed2ContPed::WriteEdgeclass(SP_DS_Edgeclass* p_pcVal,
		wxXmlNode* p_pcRoot)
{
	CHECK_POINTER(p_pcVal, return FALSE);
	CHECK_POINTER(p_pcRoot, return FALSE);
	

	SP_ListEdge::const_iterator l_Iter;
	const SP_ListEdge* l_plElements = p_pcVal->GetElements();
	CHECK_POINTER(l_plElements, return FALSE);
	wxString l_sEdgeclassName = p_pcVal->GetName();

	if (wxT("Edge") == l_sEdgeclassName ||
		wxT("Inhibitor Edge") == l_sEdgeclassName ||
		wxT("Read Edge") == l_sEdgeclassName||
		wxT("Equal Edge") == l_sEdgeclassName||
		wxT("Reset Edge")== l_sEdgeclassName||
		wxT("Modifier Edge") == l_sEdgeclassName)
	{
		wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("edgeclass"));
		l_pcElem->AddAttribute(wxT("count"), wxString::Format( wxT("%d"), l_plElements->size()));
		for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
		{
			WriteEdge((*l_Iter), l_pcElem);
		}

		if(wxT("Edge") == l_sEdgeclassName||
			wxT("Equal Edge") == l_sEdgeclassName)
		{
			l_sEdgeclassName = SP_DS_EDGE;
		}
		else if(wxT("Inhibitor Edge") == l_sEdgeclassName)
		{
			l_sEdgeclassName = SP_DS_INHIBITOR_EDGE;
		}
		else if( wxT("Read Edge") == l_sEdgeclassName||
			wxT("Reset Edge")== l_sEdgeclassName)
		{
			l_sEdgeclassName = SP_DS_READ_EDGE;
		}
		else if(wxT("Modifier Edge") == l_sEdgeclassName)
		{

		}
		l_pcElem->AddAttribute(wxT("name"), l_sEdgeclassName);
		p_pcRoot->AddChild(l_pcElem);
	}

	return TRUE;
}

bool
SP_ExportStochPed2ContPed::WriteEdge(SP_DS_Edge* p_pcVal, wxXmlNode* p_pcRoot)
{
    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);
    

    SP_ListAttribute::const_iterator l_Iter;
    const SP_ListAttribute* l_plAttributes = p_pcVal->GetAttributes();
    CHECK_POINTER(l_plAttributes, return FALSE);

    wxXmlNode*  l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("edge"));
    if (p_pcVal->GetSource())
        l_pcElem->AddAttribute(wxT("source"), wxString::Format( wxT("%ld"), p_pcVal->GetSource()->GetId()));
    if (p_pcVal->GetTarget())
        l_pcElem->AddAttribute(wxT("target"), wxString::Format( wxT("%ld"), p_pcVal->GetTarget()->GetId()));
    p_pcRoot->AddChild(l_pcElem);

    for (l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter)
    {
      WriteAttribute((*l_Iter), l_pcElem);
    }

    return WriteData(p_pcVal, l_pcElem);
}

bool SP_ExportStochPed2ContPed::AcceptsDoc(SP_MDI_Doc* p_doc)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	wxString cName = p_doc->GetNetclassName();
	return (cName == SP_DS_SPN_CLASS);
}

