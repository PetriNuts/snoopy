//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: schroed $
// $Version: 0.0 $
// $Revision: 0.00 $
// $Date: 2007/01/08 07:31:00 $
// Short Description: create a net
//////////////////////////////////////////////////////////////////////
#include "sp_ds/extensions/SP_DS_NetCreater.h"

#include "sp_core/SP_Core.h"
#include "sp_gui/dialogs/SP_DLG_LayoutProperties.h"
#include "sp_gr/SP_GR_Edge.h"
#include "sp_gr/SP_GR_Node.h"

SP_DS_NetCreater::SP_DS_NetCreater()
{
}

SP_DS_NetCreater::~SP_DS_NetCreater()
{

}


SP_DS_Graph*
SP_DS_NetCreater::CreateDocument(const wxString& netName)
{
	m_Edges.clear();

	SP_DS_Graph* l_pcGraph;
	SP_GM_Docmanager* l_pcDM = wxGetApp().GetDocmanager();
	SP_GM_DocTemplate* l_pcTemplate = l_pcDM->GetTemplate(netName);
	SP_DS_Netclass* newClass = l_pcTemplate->GetNetclass();

	wxDocument *newDoc = l_pcTemplate->CreateDocument(wxT(""), 1);

	if (newDoc)
	{
		newDoc->SetDocumentName(l_pcTemplate->GetDocumentName());
        newDoc->SetDocumentTemplate(l_pcTemplate);
        newDoc->OnNewDocument();
    }

	if (l_pcTemplate)
	{
        m_pcMyDoc = dynamic_cast<SP_MDI_Doc*>(newDoc);
        m_pcMyDoc->InitGraph(newClass);
		l_pcGraph = m_pcMyDoc->GetGraph();
	}

	SP_MDI_View* l_pcView = dynamic_cast<SP_MDI_View*>(m_pcMyDoc->GetFirstView());
	m_pcCanvas = l_pcView->GetCanvas();

	return l_pcGraph;
}


void
SP_DS_NetCreater::drawEdge(SP_DS_Node* source, SP_DS_Node* target, const wxString& EdgeClass, const wxString& stoichiometry)
{
	//connects two nodes with each other
	SP_DS_Edgeclass* l_pcEdgeclass = m_pcMyDoc->GetGraph()->GetEdgeclass(EdgeClass);
	if(!l_pcEdgeclass) return;

	edge_t id = make_pair(l_pcEdgeclass, make_pair(source, target));
	SP_MapEdges::iterator eIt = m_Edges.find(id);
	if(eIt != m_Edges.end())
	{
		if(stoichiometry != wxT("0"))
		{
			SP_DS_Edge* l_pcEdge = eIt->second;
			wxString multiplicity = l_pcEdge->GetAttribute(wxT("Multiplicity"))->GetValueString();
			double m = 0.0;
			double n = 0.0;
			if(multiplicity.ToDouble(&m) && stoichiometry.ToDouble(&n))
			{
				m += n;
				l_pcEdge->GetAttribute(wxT("Multiplicity"))->SetValueString(wxString() << m);
			}
			else
			{
				SP_LOGWARNING( wxT("\"") + stoichiometry + wxT("\" is not combinable with \"") + multiplicity + wxT("\""));
			}
			l_pcEdge->ShowOnCanvas(m_pcCanvas, FALSE);
		}
	}
	else
	{
		SP_ListGraphic* sGraphic = source->GetGraphics();
		SP_ListGraphic::iterator sIt = sGraphic->begin();
		SP_ListGraphic* tGraphic = target->GetGraphics();
		SP_ListGraphic::iterator tIt = tGraphic->begin();
		unsigned int l_nNet = source->GetNetnumber();

		SP_DS_Edge* l_pcEdge = l_pcEdgeclass->GetPrototype()->Clone();
		l_pcEdge->SetNetnumber(l_nNet);
		l_pcEdge->SetNodes(source, target);
		if (stoichiometry != wxT("0"))
		{
			l_pcEdge->GetAttribute(wxT("Multiplicity"))->SetValueString(stoichiometry);
		}
		l_pcEdgeclass->AddElement(l_pcEdge);

		SP_GR_Edge *l_pcEdgeGr =
					dynamic_cast<SP_GR_Edge*>(l_pcEdge->AddGraphicInSubnet(l_nNet));
		l_pcEdgeGr->SetSource((*sIt));
		l_pcEdgeGr->SetTarget((*tIt));

		l_pcEdge->ShowOnCanvas(m_pcCanvas, FALSE);

		m_Edges[id] = l_pcEdge;
	}
}

void
SP_DS_NetCreater::drawDpnEdge(SP_DS_Node* source, SP_DS_Node* target, const wxString& EdgeClass, const wxString& arcweight)
{
	//connects two nodes with each other
	SP_DS_Edgeclass* l_pcEdgeclass = m_pcMyDoc->GetGraph()->GetEdgeclass(EdgeClass);
	if(!l_pcEdgeclass) return;

	edge_t id = make_pair(l_pcEdgeclass, make_pair(source, target));
	SP_MapEdges::iterator eIt = m_Edges.find(id);
	if(eIt != m_Edges.end())
	{
		SP_DS_Edge* l_pcEdge = eIt->second;
		double m = 0.0;
		l_pcEdge->GetAttribute(wxT("Multiplicity"))->GetValueString().ToDouble(&m);
		double n = 0.0;
		arcweight.ToDouble(&n);
		m += n;
		l_pcEdge->GetAttribute(wxT("Multiplicity"))->SetValueString(wxString() << m);

		l_pcEdge->ShowOnCanvas(m_pcCanvas, FALSE);
	}
	else
	{
		SP_ListGraphic* sGraphic = source->GetGraphics();
		SP_ListGraphic::iterator sIt = sGraphic->begin();
		SP_ListGraphic* tGraphic = target->GetGraphics();
		SP_ListGraphic::iterator tIt = tGraphic->begin();
		unsigned int l_nNet = source->GetNetnumber();

		SP_DS_Edge* l_pcEdge = l_pcEdgeclass->GetPrototype()->Clone();
		l_pcEdge->SetNetnumber(l_nNet);
		l_pcEdge->SetNodes(source, target);
		l_pcEdge->GetAttribute(wxT("Multiplicity"))->SetValueString(arcweight);
		l_pcEdgeclass->AddElement(l_pcEdge);

		SP_GR_Edge *l_pcEdgeGr =
					dynamic_cast<SP_GR_Edge*>(l_pcEdge->AddGraphicInSubnet(l_nNet));
		l_pcEdgeGr->SetSource((*sIt));
		l_pcEdgeGr->SetTarget((*tIt));

		l_pcEdge->ShowOnCanvas(m_pcCanvas, FALSE);

		m_Edges[id] = l_pcEdge;
	}
}


void
SP_DS_NetCreater::RefreshFrame(SP_MDI_Doc* l_doc)
{
	//Refresh the Frames
	SP_GUI_Childframe *l_pcFrame = wxGetApp().GetMainframe()->GetActiveChildframe();
	if (l_pcFrame) {
		SP_GUI_Canvas* canvas = dynamic_cast<SP_MDI_View*>(l_pcFrame->GetView())->GetCanvas();
		wxClientDC dc(canvas);
		canvas->DoPrepareDC(dc);
		SP_Core::Instance()->GetRootDocument()->GetGraph()->ShowOnCanvas(canvas);
	}

	l_doc->Refresh();
}

void SP_DS_NetCreater::DoVisualization()
{
	SP_DLG_LayoutProperties* l_pcDlg = new SP_DLG_LayoutProperties(NULL, m_pcMyDoc);
	l_pcDlg->ShowModal();
	l_pcDlg->Destroy();
}
