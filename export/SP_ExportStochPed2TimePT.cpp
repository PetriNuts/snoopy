//////////////////////////////////////////////////////////////////////
// $Author: Liu $
// $Version: 0.1 $
// $Date: 2009/3/13 $
//////////////////////////////////////////////////////////////////////

#include "export/SP_ExportStochPed2TimePT.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"

#include "sp_ds/attributes/SP_DS_NumberAttribute.h"
#include "sp_ds/attributes/SP_DS_IdAttribute.h"
#include <algorithm>

bool SP_ExportStochPed2TimePT::WriteNodeclass(SP_DS_Nodeclass* p_pcVal,
		wxXmlNode* p_pcRoot)
{

	CHECK_POINTER( p_pcVal, return FALSE );
	CHECK_POINTER( p_pcRoot, return FALSE );
	
	SP_ListNode::const_iterator l_Iter;
	const SP_ListNode* l_plElements = p_pcVal->GetElements();
	CHECK_POINTER( l_plElements, return FALSE );
	wxString l_sNodeclassName = p_pcVal->GetName();


	if((wxT("Transition") == l_sNodeclassName ))
	{

		wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE,  wxT("nodeclass"));
		m_pcTransitionNodeclass = l_pcElem;
		m_nTransitionNumber=m_nTran+m_nImmediateTran+m_nDeterministicTran+m_nScheduledTran;
	    l_pcElem->AddAttribute( wxT("count"), wxString::Format( wxT("%d"), m_nTransitionNumber));
		l_pcElem->AddAttribute( wxT("name"),  wxT("Transition" ));
		p_pcRoot->AddChild(l_pcElem);
		m_nTransitionNumber=0;
		for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
		{
			WriteTransition( ( *l_Iter ), l_pcElem);
		}

	}
    else if((wxT("Immediate Transition") == l_sNodeclassName ) )
	{
		wxXmlNode* l_pcElem = m_pcTransitionNodeclass;

		m_nTransitionNumber=m_nTran;
		for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
		{
			WriteTransition( ( *l_Iter ), l_pcElem);
		}
	}

    else if((wxT("Deterministic Transition") == l_sNodeclassName ) )
	{
		wxXmlNode* l_pcElem = m_pcTransitionNodeclass;
		m_nTransitionNumber=m_nTran+m_nImmediateTran;
		for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
		{
			WriteTransition( ( *l_Iter ), l_pcElem);
		}
	}

	else if((wxT("Scheduled Transition") == l_sNodeclassName ) )
	{
		wxXmlNode* l_pcElem = m_pcTransitionNodeclass;
		m_nTransitionNumber=m_nTran+m_nImmediateTran+m_nDeterministicTran;
		for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
		{
			WriteTransition( ( *l_Iter ), l_pcElem);
		}
	}
	else
	{

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

bool SP_ExportStochPed2TimePT::Write(SP_MDI_Doc* p_doc,
		const wxString& p_fileName)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	CHECK_BOOL((!p_fileName.IsEmpty()), return false);
	m_pcTransitionNodeclass = NULL;
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

bool SP_ExportStochPed2TimePT::WriteNetclass(SP_DS_Netclass* p_pcVal,
		wxXmlNode* p_pcRoot)
{
	CHECK_POINTER(p_pcVal, return FALSE);
	CHECK_POINTER(p_pcRoot, return FALSE);
	

	wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("netclass"));
	l_pcElem->AddAttribute(wxT("name"), SP_DS_TIMEPN_CLASS);
	p_pcRoot->AddChild(l_pcElem);

	return TRUE;
}

bool SP_ExportStochPed2TimePT::WriteEdgeclass(SP_DS_Edgeclass* p_pcVal,
		wxXmlNode* p_pcRoot)
{
	CHECK_POINTER(p_pcVal, return FALSE);
	CHECK_POINTER(p_pcRoot, return FALSE);
	

	SP_ListEdge::const_iterator l_Iter;
	const SP_ListEdge* l_plElements = p_pcVal->GetElements();
	CHECK_POINTER(l_plElements, return FALSE);
	wxString l_sEdgeclassName = p_pcVal->GetName();
	m_edgeClass = p_pcVal->GetName();

	if (wxT("Edge") == l_sEdgeclassName )
	{
		wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("edgeclass"));
		m_pcEdgeclass=l_pcElem;
		l_pcElem->AddAttribute(wxT("count"), wxString::Format( wxT("%d"), l_plElements->size()));
		for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
		{
			WriteEdge((*l_Iter), l_pcElem);
		}

		l_pcElem->AddAttribute(wxT("name"), l_sEdgeclassName);
		p_pcRoot->AddChild(l_pcElem);
	}
	else if (wxT("Inhibitor Edge") == l_sEdgeclassName ||
		     wxT("Equal Edge") == l_sEdgeclassName||
		     wxT("Reset Edge") == l_sEdgeclassName)
	{
		wxXmlNode* l_pcElem =m_pcEdgeclass;
		for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
		{
			WriteEdge((*l_Iter), l_pcElem);
		}
	}
	else if (wxT("Read Edge") == l_sEdgeclassName)
	{
		wxXmlNode* l_pcElem =m_pcEdgeclass;
		for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
		{
			WriteEdge((*l_Iter), l_pcElem);
		}
	}

	return TRUE;
}

bool SP_ExportStochPed2TimePT::AcceptsDoc(SP_MDI_Doc* p_doc)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	wxString cName = p_doc->GetNetclassName();
	return (cName == SP_DS_SPN_CLASS);
}


bool SP_ExportStochPed2TimePT::WriteTransition(SP_DS_Node* p_pcVal,
		wxXmlNode* p_pcRoot)
{

    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);

	SP_ListAttribute::const_iterator l_Iter;
	const SP_ListAttribute* l_plAttributes = p_pcVal->GetAttributes();
	CHECK_POINTER( l_plAttributes, return FALSE );

	wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE,  wxT("node"));
	p_pcRoot->AddChild(l_pcElem);

	SP_DS_ColListAttribute* l_pcColList = NULL;

	//unsigned l_nNetNumber;


	for (l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter)
	{
		if (( *l_Iter )->GetName() == wxT("DelayList"))
		{

		}
		else if (( *l_Iter )->GetName() == wxT("PeriodicList"))
		{

		}
		else if (( *l_Iter )->GetName() == wxT("FunctionList"))
		{

		}
		else
		{
			WriteAttribute( ( *l_Iter ), l_pcElem);
		}
	}

	/////
		unsigned int l_nNewRow;
		l_pcColList = new SP_DS_ColListAttribute( wxT("Duration"), SP_COLLIST_STRING, 2 );
		l_pcColList->SetColLabel( 0, wxT("Duration List"));
		l_pcColList->SetColLabel( 1, wxT("Duration"));
		//l_pcColList->SetNetnumber(l_nNetNumber);
		if(wxT("Deterministic Transition")==p_pcVal->GetClassName())
		{
			for (l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter)
			{
				if (( *l_Iter )->GetName() == wxT("DelayList"))
				{
					SP_DS_Attribute* l_pcAttribute = *l_Iter;
					SP_DS_ColListAttribute* l_pcColListDelay = dynamic_cast< SP_DS_ColListAttribute* >(l_pcAttribute);
					for (unsigned int i = 0; i < l_pcColListDelay->GetRowCount(); i++)
					{
						l_nNewRow = l_pcColList->AppendEmptyRow();
						l_pcColList->SetCell(i, 0, l_pcColListDelay->GetCell(i, 0));
						l_pcColList->SetCell(i, 1, l_pcColListDelay->GetCell(i, 1));
					}
					break;
				}
			}
		}
		else
		{
			l_nNewRow = l_pcColList->AppendEmptyRow();
			l_pcColList->SetCell(l_nNewRow, 0, wxT("Main"));
			l_pcColList->SetCell(l_nNewRow, 1, wxT("0"));
		}

		WriteAttribute(l_pcColList, l_pcElem);


		l_pcColList =new SP_DS_ColListAttribute( wxT("Interval"), SP_COLLIST_STRING, 3 );
		l_pcColList->SetColLabel( 0, wxT("Interval List"));
		l_pcColList->SetColLabel( 1, wxT("EFT"));
		l_pcColList->SetColLabel( 2, wxT("LFT"));
		//l_pcColList->SetNetnumber(l_nNetNumber);

		if(wxT("Scheduled Transition")==p_pcVal->GetClassName())
		{
			for (l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter)
			{
				if (( *l_Iter )->GetName() == wxT("PeriodicList"))
				{
					SP_DS_Attribute* l_pcAttribute = *l_Iter;
					SP_DS_ColListAttribute* l_pcColListDelay = dynamic_cast< SP_DS_ColListAttribute* >(l_pcAttribute);
					for (unsigned int i = 0; i < l_pcColListDelay->GetRowCount(); i++)
					{
						l_nNewRow = l_pcColList->AppendEmptyRow();
						l_pcColList->SetCell(i, 0, l_pcColListDelay->GetCell(i, 0));
						l_pcColList->SetCell(i, 1, l_pcColListDelay->GetCell(i, 1));
						l_pcColList->SetCell(i, 2, l_pcColListDelay->GetCell(i, 3));
					}
					break;
				}
			}
		}
		else
		{
			l_nNewRow = l_pcColList->AppendEmptyRow();
			l_pcColList->SetCell(l_nNewRow, 0, wxT("Main"));
			l_pcColList->SetCell(l_nNewRow, 1, wxT("?"));
			l_pcColList->SetCell(l_nNewRow, 2, wxT("?"));
		}

		WriteAttribute(l_pcColList, l_pcElem);

    return WriteData(p_pcVal, l_pcElem);
}


bool
SP_ExportStochPed2TimePT::WriteEdgeGraphic(SP_GR_Edge* p_pcVal,
				     wxXmlNode* p_pcRoot)
{
  if (m_edgeClass != wxT("Edge")) m_changeColour = true;
  bool res = SP_XmlWriter::WriteEdgeGraphic(p_pcVal, p_pcRoot);
  m_changeColour = false;
  return res;
}


bool
SP_ExportStochPed2TimePT::WriteEdge(SP_DS_Edge* p_pcVal,
					wxXmlNode* p_pcRoot)
{
    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);
    

    bool l_bReturn = SP_XmlWriter::WriteEdge(p_pcVal, p_pcRoot);

	if (m_edgeClass == wxT("Read Edge")) {

		SP_DS_Edge* l_pcNewEdge = p_pcVal->Clone(true);
		l_pcNewEdge->SetNodes(p_pcVal->GetTarget(), p_pcVal->GetSource());
		for(SP_ListGraphic::iterator l_itGr = p_pcVal->GetGraphics()->begin();
			l_itGr != p_pcVal->GetGraphics()->end();
			l_itGr++)
		{
			SP_Graphic* l_pcGraphic = *l_itGr;
			SP_GR_Edge* l_pcGr = dynamic_cast<SP_GR_Edge*>(l_pcGraphic);
			long l_nNet = l_pcGr->GetNetnumber();
			SP_GR_Edge* l_pcNewGraphic = dynamic_cast<SP_GR_Edge*>(l_pcNewEdge->GetGraphicInSubnet(l_nNet));
			l_pcNewGraphic->SetSource(l_pcGr->GetTarget());
			l_pcNewGraphic->SetTarget(l_pcGr->GetSource());
			wxList* l_pcList = dynamic_cast<wxLineShape*>(l_pcGr->GetPrimitive())->GetLineControlPoints();
			l_pcNewGraphic->SetControlPoints(l_pcList);
		}
		l_bReturn = l_bReturn && SP_XmlWriter::WriteEdge(l_pcNewEdge, p_pcRoot);
		l_pcNewEdge->Remove();
		wxDELETE(l_pcNewEdge);
	}
	return l_bReturn;
}


bool
SP_ExportStochPed2TimePT::WriteColourInformation(SP_Graphic* p_pcVal,
					   wxXmlNode* p_pcNode)
{
  CHECK_POINTER(p_pcVal, return FALSE);
  CHECK_POINTER(p_pcNode, return FALSE);

  if (m_changeColour) { // write our own colors

    CHECK_BOOL(m_edgeClass != wxT("Edge"),
	       return SP_XmlWriter::WriteColourInformation(p_pcVal, p_pcNode));

    wxString colour;
    if (m_edgeClass == wxT("Read Edge")) {
      colour=wxT("143,188,143");
    } else if (m_edgeClass == wxT("Inhibitor Edge")) {
      colour=wxT("255,0,0");
    } else if (m_edgeClass == wxT("Reset Edge")) {
      colour=wxT("255,0,255");
    } else if (m_edgeClass == wxT("Equal Edge")) {
      colour=wxT("255,220,0");
    }
    p_pcNode->AddAttribute(wxT("pen"), colour);
    p_pcNode->AddAttribute(wxT("brush"), colour);
  } else {
    return SP_XmlWriter::WriteColourInformation(p_pcVal, p_pcNode);
  }
  return true;
}
