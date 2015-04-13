//////////////////////////////////////////////////////////////////////
// $Author: alextov $
// $Version: 0.1 $
// $Date: 2005/06/23 $
// Short Description: writes SimplePed from Extended PT net
//////////////////////////////////////////////////////////////////////
#include "export/SP_ExportExtPT2Ped.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"


bool
SP_ExportExtPT2Ped::AcceptsDoc(SP_MDI_Doc* p_doc)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	wxString cName = p_doc->GetNetclassName();
	return (cName == SP_DS_EXTPN_CLASS);
}


bool
SP_ExportExtPT2Ped::Write(SP_MDI_Doc* p_doc, const wxString& p_fileName)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	CHECK_BOOL((!p_fileName.IsEmpty()), return false);

	m_changeColour = false;
	return SP_XmlWriter::Write(p_doc->GetGraph(), p_fileName);
}


bool
SP_ExportExtPT2Ped::WriteNetclass(SP_DS_Netclass* p_pcVal,
				  wxXmlNode* p_pcRoot)
{
	CHECK_POINTER(p_pcVal, return false);
	CHECK_POINTER(p_pcRoot, return false);
	

	wxXmlNode*  l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("netclass"));
	l_pcElem->AddAttribute(wxT("name"), SP_DS_PN_CLASS);
	p_pcRoot->AddChild(l_pcElem);

	return true;
}


bool
SP_ExportExtPT2Ped::WriteEdgeclass(SP_DS_Edgeclass* p_pcVal,
				   wxXmlNode* p_pcRoot)
{
	CHECK_POINTER(p_pcVal, return false);
	CHECK_POINTER(p_pcRoot, return false);
	

	SP_ListEdge::const_iterator l_Iter;
	const SP_ListEdge* l_plElements = p_pcVal->GetElements();
	CHECK_POINTER(l_plElements, return false);

	m_edgeClass = p_pcVal->GetName();

	wxXmlNode*  l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("edgeclass"));
	l_pcElem->AddAttribute(wxT("count"), wxString::Format( wxT("%lu"), l_plElements->size()));

	l_pcElem->AddAttribute(wxT("name"), wxT("Edge"));
	p_pcRoot->AddChild(l_pcElem);

	for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
		WriteEdge((*l_Iter), l_pcElem);

	return TRUE;
}


bool
SP_ExportExtPT2Ped::WriteEdgeGraphic(SP_GR_Edge* p_pcVal,
				     wxXmlNode* p_pcRoot)
{
	if (m_edgeClass != wxT("Edge")) m_changeColour = true;
	bool res = SP_XmlWriter::WriteEdgeGraphic(p_pcVal, p_pcRoot);
	m_changeColour = false;
	return res;
}


bool
SP_ExportExtPT2Ped::WriteEdge(SP_DS_Edge* p_pcVal,
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
SP_ExportExtPT2Ped::WriteColourInformation(SP_Graphic* p_pcVal,
					   wxXmlNode* p_pcNode)
{
	CHECK_POINTER(p_pcVal, return FALSE);
	CHECK_POINTER(p_pcNode, return FALSE);

	if (m_changeColour)
	{ // write our own colors

		CHECK_BOOL(m_edgeClass != wxT("Edge"),
				return SP_XmlWriter::WriteColourInformation(p_pcVal, p_pcNode));

		wxString colour;
		if (m_edgeClass == wxT("Read Edge"))
		{
			colour = wxT("143,188,143");
		}
		else if (m_edgeClass == wxT("Inhibitor Edge"))
		{
			colour = wxT("255,0,0");
		}
		else if (m_edgeClass == wxT("Reset Edge"))
		{
			colour = wxT("255,0,255");
		}
		else if (m_edgeClass == wxT("Equal Edge"))
		{
			colour = wxT("255,220,0");
		}
		p_pcNode->AddAttribute(wxT("pen"), colour);
		p_pcNode->AddAttribute(wxT("brush"), colour);
	}
	else
	{
		return SP_XmlWriter::WriteColourInformation(p_pcVal, p_pcNode);
	}
	return true;
}
