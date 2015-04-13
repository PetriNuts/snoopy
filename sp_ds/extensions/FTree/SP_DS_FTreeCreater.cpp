//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: akurth $
// $Version: 0.0 $
// $Revision: 0.00 $
// $Date: 2007/01/08 07:31:00 $
// Short Description: create a fault tree
//////////////////////////////////////////////////////////////////////
#include "sp_ds/extensions/FTree/SP_DS_FTreeCreater.h"

#include "sp_core/SP_Core.h"

#include "sp_gui/management/SP_GM_Docmanager.h"
#include "sp_gui/management/SP_GM_DocTemplate.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"

#include "sp_gui/dialogs/SP_DLG_LayoutProperties.h"

#include "snoopy.h"

#include "sp_defines.h"


SP_DS_FTreeCreater::SP_DS_FTreeCreater()
: m_Layout(wxT("dot")),
  m_Type(wxT("AGDIGRAPH"))	
{	
}

SP_DS_FTreeCreater::~SP_DS_FTreeCreater()
{
	
}

void
SP_DS_FTreeCreater::CopyAttributes(SP_DS_Node* sourceNode, SP_DS_Node* copyNode)
{
	//Copy the attributes of one node and assign the attributes to another node
	const SP_ListAttribute* attrListSourceNode = sourceNode->GetAttributes();
	SP_ListAttribute::const_iterator attr_It;
	
	const SP_ListAttribute* attrListCopyNode = copyNode->GetAttributes();
	SP_ListAttribute::const_iterator attrc_It = attrListCopyNode->begin();

	for (attr_It = attrListSourceNode->begin(); attr_It != attrListSourceNode->end(); ++attr_It){
		(*attrc_It)->SetValueString((*attr_It)->GetValueString());
		++attrc_It;
	} 
}

SP_DS_Graph*
SP_DS_FTreeCreater::CreateDocument()
{
	SP_DS_Graph* l_pcGraph;
	SP_GM_Docmanager* l_pcDM = wxGetApp().GetDocmanager();

	wxString netName = SP_DS_FAULTTREE_CLASS;
	SP_GM_DocTemplate* l_pcTemplate = l_pcDM->GetTemplate(netName);
     
	SP_DS_Netclass* newClass = l_pcTemplate->GetNetclass();

	
	wxDocument *newDoc = l_pcTemplate->CreateDocument(wxT(" "), 1);
            
	if (newDoc){
		newDoc->SetDocumentName(l_pcTemplate->GetDocumentName());
        newDoc->SetDocumentTemplate(l_pcTemplate);
        newDoc->OnNewDocument();
    }

	if (l_pcTemplate){
        m_pcMyDoc = dynamic_cast<SP_MDI_Doc*>(newDoc);
        m_pcMyDoc->InitGraph(newClass);
		l_pcGraph = m_pcMyDoc->GetGraph();
	}
	
	SP_MDI_View* l_pcView = dynamic_cast<SP_MDI_View*>(m_pcMyDoc->GetFirstView());
	m_pcCanvas = l_pcView->GetCanvas();
	
	return l_pcGraph;
}

void
SP_DS_FTreeCreater::drawEdge(SP_DS_Node* source, SP_DS_Node* target)
{
	//connects to nodes with each other
	SP_ListGraphic* sGraphic = source->GetGraphics();
	SP_ListGraphic::iterator sIt;
	sIt = sGraphic->begin();
	double lnXs = (*sIt)->GetPrimitive()->GetX();
	double lnYs = (*sIt)->GetPrimitive()->GetY();

			
	SP_ListGraphic* tGraphic = target->GetGraphics();
	SP_ListGraphic::iterator tIt;
	tIt = tGraphic->begin();
	double lnXt = (*tIt)->GetPrimitive()->GetX();
	double lnYt = (*tIt)->GetPrimitive()->GetY();

			
	SP_ListEdgeclass::const_iterator esIt;
	const SP_ListEdgeclass* eList = m_pcMyDoc->GetGraph()->GetEdgeclasses();
	esIt= eList->begin();

	(*esIt)->OnStartElement((*tIt), lnXt, lnYt, 0, 0);
	SP_DS_Edge* l_pcEdge= (*esIt)->OnEndElement((*sIt), lnXs, lnYs, 0, 0);
	(*esIt)->AddElement(l_pcEdge);
	RefreshFrame(m_pcMyDoc);
}

void
SP_DS_FTreeCreater::RefreshFrame(SP_MDI_Doc* l_doc)
{
	//Refresh the Frames
	l_doc->Refresh();

}

void 
SP_DS_FTreeCreater::DoVisualization()
{
    //Representation of the graph as a tree
	SP_DLG_LayoutProperties* l_pcDlg = new SP_DLG_LayoutProperties(NULL, m_pcMyDoc);
	l_pcDlg->ShowModal();
	l_pcDlg->Destroy();

	RefreshFrame(m_pcMyDoc);
}

SP_MapString2String
SP_DS_FTreeCreater::GetAttributes()
{

    SP_MapString2String m_lAttributes;

	m_lAttributes[wxT("rankdir")] = wxT("TB");
    
	double nodesep = 0.75;
	wxString nodesepVal;
	nodesepVal.Printf(wxT("%.2G"), nodesep);
	m_lAttributes[wxT("nodesep")] = nodesepVal;

	double rank = 0.75;
	wxString rankVal;
	rankVal.Printf(wxT("%.2G"), rank);
	m_lAttributes[wxT("ranksep")] = rankVal;

	return m_lAttributes;
}
