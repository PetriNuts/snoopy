/*
* SP_ExportHPN2CPN.cpp
* $Author: George Assaf
* $Version: 0.0 $
* $Revision: 0.0 $
* $Date: 29.01.2019
* Short Description: FCPN==> FSPN
*/
//======================================================================================
#include "export/SP_ExportFCPN2FSPN.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_utilities.h"

#include "sp_revision.h"
//


#include "snoopy.h"
//#include "sp_ds/SP_DS_Graph.h"
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
#include "sp_gr/shapes/SP_GR_Ellipse.h"
#include "sp_ds/animators/ani_HybridPN/SP_DS_HybridAnimation.h"
bool SP_ExportFCPN2FSPN::Write(SP_MDI_Doc* p_doc, const wxString& p_fileName)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	CHECK_BOOL((!p_fileName.IsEmpty()), return false);

	m_doc = p_doc;

	m_graph = m_doc->GetGraph();

	m_fileName = p_fileName;

	//Get the total number of place
	m_nPlaceCount = m_graph->GetNodeclass(SP_DS_CONTINUOUS_PLACE)->GetElements()->size();
	//m_nPlaceCount += m_graph->GetNodeclass(SP_DS_DISCRETE_PLACE)->GetElements()->size();

	//Get the total number of transitions
	m_mTransitionCount = m_graph->GetNodeclass(SP_DS_CONTINUOUS_TRANS)->GetElements()->size();
	//m_mTransitionCount += m_graph->GetNodeclass(SP_DS_STOCHASTIC_TRANS)->GetElements()->size();
///	m_mTransitionCount += m_graph->GetNodeclass(SP_DS_DETERMINISTIC_TRANS)->GetElements()->size();
	//m_mTransitionCount += m_graph->GetNodeclass(SP_DS_IMMEDIATE_TRANS)->GetElements()->size();
//	m_mTransitionCount += m_graph->GetNodeclass(SP_DS_SCHEDULED_TRANS)->GetElements()->size();

	return SP_XmlWriter::Write(m_graph, m_fileName);
}

bool SP_ExportFCPN2FSPN::WriteNodeclass(SP_DS_Nodeclass* p_pcVal, wxXmlNode* p_pcRoot)
{

	CHECK_POINTER(p_pcVal, return FALSE);
	CHECK_POINTER(p_pcRoot, return FALSE);

	SP_ListNode::const_iterator l_Iter;
	const SP_ListNode* l_plElements = p_pcVal->GetElements();

	CHECK_POINTER(l_plElements, return FALSE);
	wxString l_sNodeclassName = p_pcVal->GetName();

	if (l_sNodeclassName == SP_DS_DISCRETE_PLACE)
	{
		m_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("nodeclass"));
		m_pcElem->AddAttribute(wxT("count"), wxString::Format(wxT("%d"), m_nPlaceCount));
		m_pcElem->AddAttribute(wxT("name"), SP_DS_DISCRETE_PLACE);
		p_pcRoot->AddChild(m_pcElem);

		for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
		{
			//SP_DS_Attribute* nameAttr = (*l_Iter)->GetAttribute(wxT("Name"));//
			//wxString valString = nameAttr->GetValueString();//
			///bool isConv = m_converter.IsConverted(valString);//
			//if (isConv) { return TRUE; }//
			SP_DS_Attribute* nameAttr = (*l_Iter)->GetAttribute(wxT("ID"));//
			wxString valString = nameAttr->GetValueString();//

			for (int i = 0; i<m_sIds.size(); i++)
			{
				if (m_sIds[i] == valString) return TRUE;
			}
			WritePlace((*l_Iter), m_pcElem);
		}
	}
	else
		if (l_sNodeclassName == SP_DS_CONTINUOUS_PLACE)
		{

			m_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("nodeclass"));
			m_pcElem->AddAttribute(wxT("count"), wxString::Format(wxT("%d"), m_nPlaceCount));
			m_pcElem->AddAttribute(wxT("name"), SP_DS_DISCRETE_PLACE);
			p_pcRoot->AddChild(m_pcElem);
			for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
			{
				SP_DS_Attribute* l_pcAttr;
				SP_GR_Node* l_pcGr;
				SP_Graphic* l_pcGrAttr;
				SP_DS_Nodeclass* l_pcNC;

				////////////

				l_pcNC = m_graph->AddNodeclass(new SP_DS_Nodeclass(m_graph, SP_DS_DISCRETE_PLACE, m_graph->GetNodeclass(SP_DS_CONTINUOUS_PLACE)->GetIdCountPtr()));
				l_pcNC->SetDisplayName(wxT("Discrete Place"));
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



				int pwidth = wxGetApp().GetElementPrefs()->GetNodeWidth(GetName(), wxT("Place"));
				int pheight = wxGetApp().GetElementPrefs()->GetNodeHeight(GetName(), wxT("Place"));

				l_pcGr = new SP_GR_Ellipse(l_pcNC->GetPrototype(), pwidth, pheight);
				l_pcNC->SetGraphic(l_pcGr);
				l_pcGr->SetFixedSize(wxGetApp().GetElementPrefs()->GetNodeFixed(GetName(), l_pcNC->GetName()));
				l_pcNC->RegisterGraphicWidget(new SP_WDG_DialogGraphic(wxT("Graphic")));

				l_pcAttr = l_pcNC->AddAttribute(new SP_DS_LogicAttribute(wxT("Logic"), wxT("Name")));
				l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogBool(wxT("General")));

				l_pcAttr = l_pcNC->AddAttribute(new SP_DS_TextAttribute(wxT("Comment"), wxT("")));
				l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
				l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
				l_pcGrAttr->SetOffsetY(40);
				l_pcAttr->SetGlobalShow();


				/////
				SP_DS_Nodeclass* l_pcConvertToNodeClass1 = m_graph->GetNodeclassByDisplayedName(wxT("Discrete Place"));
				SP_DS_Node* ConvertedNode1 = m_converter.Clone((**l_Iter), *l_pcNC);
				//SP_DS_Attribute* nameAttr = ConvertedNode1->GetAttribute(wxT("Name"));

				SP_DS_Attribute* nameAttr = ConvertedNode1->GetAttribute(wxT("ID"));
				wxString valString1 = nameAttr->GetValueString();
				m_sIds.push_back(valString1);
				/*change marking to zero*/
				wxString l_sAttributeName = wxT("Marking");

				SP_DS_Attribute* l_pcNewAttribute = ConvertedNode1->GetAttribute(l_sAttributeName);
				wxString valString = l_pcNewAttribute->GetValueString();
				double marking;
				SP_DS_Attribute* att;
				valString.ToDouble(&marking);
				if (floor(marking) == ceil(marking))
				{
				 att = new SP_DS_MarkingAttribute(wxT("Marking"), (int)marking);
				}
				else
				{
					int roundedVal = round(marking);
				 att = new SP_DS_MarkingAttribute(wxT("Marking"), roundedVal);
			     }

                l_pcNewAttribute->CopyValueFrom(att);
				WritePlace(ConvertedNode1, m_pcElem);//WritePlace((*l_Iter), m_pcElem);
			}
		}
		else
			if (l_sNodeclassName == SP_DS_STOCHASTIC_TRANS)
			{
				unsigned long l_mStochTransitionCount = m_graph->GetNodeclass(SP_DS_CONTINUOUS_TRANS)->GetElements()->size();
				//l_mStochTransitionCount += m_graph->GetNodeclass(SP_DS_STOCHASTIC_TRANS)->GetElements()->size();
			//	l_mStochTransitionCount += m_graph->GetNodeclass(SP_DS_STOCHASTIC_TRANS)->GetElements()->size();
				m_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("nodeclass"));//
				m_pcElem->AddAttribute(wxT("count"), wxString::Format(wxT("%d"), l_mStochTransitionCount));///
				m_pcElem->AddAttribute(wxT("name"), SP_DS_STOCHASTIC_TRANS);//
				p_pcRoot->AddChild(m_pcElem);/////

				for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
				{
					SP_DS_Attribute* nameAttr = (*l_Iter)->GetAttribute(wxT("ID"));//
					wxString valString = nameAttr->GetValueString();//
				 
					for(int i=0;i<m_sIds.size();i++)
					{
						if (m_sIds[i] == valString) return TRUE;
					}
					WriteTransition((*l_Iter), m_pcElem);
				}
			}
			else if (l_sNodeclassName == SP_DS_CONTINUOUS_TRANS)
			{
				unsigned long l_mStochTransitionCount = m_graph->GetNodeclass(SP_DS_CONTINUOUS_TRANS)->GetElements()->size();
			//	l_mStochTransitionCount = m_graph->GetNodeclass(SP_DS_STOCHASTIC_TRANS)->GetElements()->size();
				l_mStochTransitionCount += m_graph->GetNodeclass(SP_DS_STOCHASTIC_TRANS)->GetElements()->size();
				m_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("nodeclass"));
				m_pcElem->AddAttribute(wxT("count"), wxString::Format(wxT("%d"), l_mStochTransitionCount));
				m_pcElem->AddAttribute(wxT("name"), SP_DS_STOCHASTIC_TRANS);
				p_pcRoot->AddChild(m_pcElem);


				for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
				{
					SP_DS_Nodeclass* l_pcNC;
					SP_DS_Attribute* l_pcAttr;
					SP_GR_Node* l_pcGr;
					SP_Graphic* l_pcGrAttr;
					SP_DS_ColListAttribute* l_pcColList;
					unsigned int l_nNewRow;

					l_pcNC = m_graph->AddNodeclass(new SP_DS_Nodeclass(m_graph,
						SP_DS_STOCHASTIC_TRANS,
						m_graph->GetNodeclass(SP_DS_CONTINUOUS_TRANS)->GetIdCountPtr()));
					l_pcNC->SetDisplayName(wxT("Stochastic Transition")); 

					//l_pcNC->SetShortcut(wxT("Shift+T"));
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

					int twidth = wxGetApp().GetElementPrefs()->GetNodeWidth(GetName(), SP_DS_STOCHASTIC_TRANS); //  wxT("Immediate Transition")
					int theight = wxGetApp().GetElementPrefs()->GetNodeHeight(GetName(), SP_DS_STOCHASTIC_TRANS);//  wxT("Immediate Transition")

					l_pcGr = new SP_GR_Rectangle(l_pcNC->GetPrototype(), twidth, theight);
					l_pcGr->SetHeight(20.0);
					l_pcGr->SetWidth(10.0);
					//l_pcGr->SetDefaultBrush(wxTheBrushList->FindOrCreateBrush(wxColour(0, 0, 0)));
					l_pcGr->SetFixedSize(wxGetApp().GetElementPrefs()->GetNodeFixed(GetName(), l_pcNC->GetName()));
					l_pcNC->SetGraphic(l_pcGr);
					l_pcNC->RegisterGraphicWidget(new SP_WDG_DialogGraphic(wxT("Graphic")));

					/*
					int twidth = wxGetApp().GetElementPrefs()->GetNodeWidth(GetName(), SP_DS_STOCHASTIC_TRANS);
					int theight = wxGetApp().GetElementPrefs()->GetNodeHeight(GetName(), SP_DS_STOCHASTIC_TRANS);
					l_pcGr = new SP_GR_ExtendedRectangle(l_pcNC->GetPrototype(), twidth, theight, TRUE, SP_EXTENDED_TYPE_DEFAULT, 3, wxColour(128, 128, 128));
					l_pcGr->SetDefaultPen(wxThePenList->FindOrCreatePen(wxColour(128, 128, 128), 3));
					l_pcGr->SetFixedSize(wxGetApp().GetElementPrefs()->GetNodeFixed(GetName(), l_pcNC->GetName()));
					l_pcNC->SetGraphic(l_pcGr);
					l_pcNC->RegisterGraphicWidget(new SP_WDG_DialogGraphic(wxT("Graphic")));
					*/
					SP_DS_Node* ConvertedNode = m_converter.Clone((**l_Iter),*l_pcNC);

					WriteTransition(ConvertedNode, m_pcElem); 
					SP_DS_Attribute* nameAttr = ConvertedNode->GetAttribute(wxT("ID"));
					wxString valString = nameAttr->GetValueString();
					m_sIds.push_back(valString);
				}
			}
			else
			{

				wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("nodeclass"));
				l_pcElem->AddAttribute(wxT("count"), wxString::Format(wxT("%d"), (unsigned long)l_plElements->size()));
				l_pcElem->AddAttribute(wxT("name"), l_sNodeclassName);
				p_pcRoot->AddChild(l_pcElem);

				for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
				{
					WriteNode((*l_Iter), l_pcElem);
				}
			}

	return TRUE;

}

bool SP_ExportFCPN2FSPN::WritePlace(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot)
{

	CHECK_POINTER(p_pcVal, return FALSE);
	CHECK_POINTER(p_pcRoot, return FALSE);

	SP_ListAttribute::const_iterator l_Iter;
	const SP_ListAttribute* l_plAttributes = p_pcVal->GetAttributes();
	CHECK_POINTER(l_plAttributes, return FALSE);

	wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("node"));
	p_pcRoot->AddChild(l_pcElem);

	for (l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter)
	{

		WriteAttribute((*l_Iter), l_pcElem);

	}

	return WriteData(p_pcVal, l_pcElem);

}

bool SP_ExportFCPN2FSPN::WriteTransition(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot)
{
	CHECK_POINTER(p_pcVal, return FALSE);
	CHECK_POINTER(p_pcRoot, return FALSE);

	SP_ListAttribute::const_iterator l_Iter;
	const SP_ListAttribute* l_plAttributes = p_pcVal->GetAttributes();
	CHECK_POINTER(l_plAttributes, return FALSE);

	wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("node"));
	p_pcRoot->AddChild(l_pcElem);

	for (l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter)
	{

		if ((*l_Iter)->GetName() == wxT("DelayList"))
		{

		}
		else
			if ((*l_Iter)->GetName() == wxT("PeriodicList"))
			{

			}
			else
			{
				WriteAttribute((*l_Iter), l_pcElem);
			}
	}
	return WriteData(p_pcVal, l_pcElem);
}

bool SP_ExportFCPN2FSPN::AcceptsDoc(SP_MDI_Doc* p_doc)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	wxString cName = p_doc->GetNetclassName();
	return (cName == SP_DS_FUZZYCPN_CLASS);
}

bool SP_ExportFCPN2FSPN::WriteMetadata(SP_DS_Metadata *p_pcVal, wxXmlNode *p_pcRoot)
{
	if (p_pcVal->GetClassName() == SP_DS_META_CONSTANT)
	{
		SP_DS_Attribute* l_pcAttr = p_pcVal->GetAttribute(wxT("Type"));
		if (l_pcAttr)
		{
			//if (l_pcAttr->GetValueString() != wxT("int"))
			//	return false;
		}
	}
	else if (p_pcVal->GetClassName() == SP_DS_META_FUNCTION)
	{
		SP_DS_Attribute* l_pcAttr = p_pcVal->GetAttribute(wxT("Return"));
		if (l_pcAttr)
		{
			if (l_pcAttr->GetValueString() != wxT("int"))
				return false;
		}
	}

	return SP_XmlWriter::WriteMetadata(p_pcVal, p_pcRoot);
}

bool
SP_ExportFCPN2FSPN::WriteStochTransition(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot)
{

	CHECK_POINTER(p_pcVal, return FALSE);
	CHECK_POINTER(p_pcRoot, return FALSE);


	SP_ListAttribute::const_iterator l_Iter;
	const SP_ListAttribute* l_plAttributes = p_pcVal->GetAttributes();
	CHECK_POINTER(l_plAttributes, return FALSE);

	wxXmlNode*  l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("node"));
	p_pcRoot->AddChild(l_pcElem);

	for (l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter)
	{
		if (!((*l_Iter)->GetName() == wxT("Reversible") || (*l_Iter)->GetName() == wxT("FunctionList")))
		{
			WriteAttribute((*l_Iter), l_pcElem);
		}

	}

	return WriteData(p_pcVal, l_pcElem);

}