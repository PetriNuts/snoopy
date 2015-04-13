//////////////////////////////////////////////////////////////////////
// $Author: akurth $
// $Version: 0.1 $
// $Date: 2006/03/11 $
// Short Description: creates Files for SyRePa
//////////////////////////////////////////////////////////////////////
#include "export/SP_ExportSyRePa.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_ds/SP_DS_Graph.h"
#include <wx/filename.h>
#include "sp_ds/attributes/SP_DS_ListAttribute.h"
#include "sp_ds/attributes/SP_DS_DoubleListAttribute.h"
#include "sp_ds/extensions/FTree/SP_DS_FTreeLogicalExpressionReader.h"

Basic::Basic(const wxString& num, const wxString& p_name, SP_DS_Node* p_node) :
	m_number(num), m_name(p_name), m_node(p_node)
{
}

SP_ExportSyRePa::SP_ExportSyRePa() :
	m_doc(NULL), m_graph(NULL)
{
}

SP_ExportSyRePa::~SP_ExportSyRePa()
{
}

void SP_ExportSyRePa::ClearMap()
{
	// we store the same pointers as m_logExp and listElements,
	// so don't delete them once again, just clear the map
	m_logExp.clear();
	listElements.clear();
}

bool SP_ExportSyRePa::AcceptsDoc(SP_MDI_Doc* p_doc)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	wxString cName = p_doc->GetNetclassName();
	return (cName == SP_DS_FAULTTREE_CLASS || cName == SP_DS_EXTFAULTTREE_CLASS);
}

bool SP_ExportSyRePa::Write(SP_MDI_Doc* p_doc, const wxString& p_fileName)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	CHECK_BOOL((!p_fileName.IsEmpty()), return false);

	m_doc = p_doc;
	m_graph = m_doc->GetGraph();

	f_fileName = p_fileName;

	ClearMap();
	if (!checkSemantic())
		return false;

	wxFileName fn = wxFileName(p_doc->GetFilename());
	wxFileName tmpFN = wxFileName(f_fileName);

	tmpFN.SetExt(wxT("DAT"));
	wxString f_name = tmpFN.GetFullName();

	data_fileName = tmpFN.GetFullPath();

	if (!f_file.Open(f_fileName.c_str(), wxT("wt")) )
	{
		return FALSE;
	}
	if (!data_file.Open(data_fileName.c_str(), wxT("wt")) )
	{
		return FALSE;
	}

	// will be implemented in successors
	bool res = DoWrite();

	// close file handle and check if this was ok
	CHECK_BOOL((f_file.Close()), return FALSE);
	CHECK_BOOL((data_file.Close()), return FALSE);

	return res && !m_bError;
}

bool SP_ExportSyRePa::DoWrite()
{
	SP_DS_FTreeLogicalExpressionReader* logEx =
			new SP_DS_FTreeLogicalExpressionReader();
	m_logExp = logEx->FindLogicalExpression(SearchRoot());
	ReplaceNames();
	return !m_bError;
}

bool SP_ExportSyRePa::checkSemantic()
{
	CHECK_POINTER(m_graph, return FALSE);

	// gets the iterator of nodeclass
	SP_ListNodeclass::const_iterator ncIt;
	const SP_ListNodeclass* ncList = m_graph->GetNodeclasses();

	CHECK_POINTER(ncList, return FALSE);

	SP_DS_Nodeclass* nc;
	SP_ListNode::const_iterator nIt;
	const SP_ListNode* nList;

	const SP_ListEdge* edgesSrc;
	const SP_ListEdge* edgesTrg;
	bool wasFound= FALSE;

	//iterate the nodeclasses
	for (ncIt = ncList->begin(); ncIt != ncList->end(); ncIt++)
	{
		nc = (*ncIt);
		nList = nc->GetElements();

		for (nIt = nList->begin(); nIt != nList->end(); nIt++)
		{
			//checks, if each basic event/undeveloped event and COND-parameter
			//has an out going edge
			edgesSrc = (*nIt)->GetSourceEdges();
			edgesTrg = (*nIt)->GetTargetEdges();
			if ((nc->GetName() == wxT("Basic Event")) ||
				(nc->GetName() == wxT("Undeveloped Event")) ||
				(nc->GetName() == wxT("COND-Parameter")))
			{
				if (edgesSrc->size() == 0)
				{
					SP_LOGMESSAGE(
							wxString::Format(wxT("FaultTree not complete!")));
					SP_MESSAGEBOX(wxT("  FaultTree not complete! "), wxT("Error"),
							wxOK | wxICON_ERROR);
					return FALSE;
				}
			}
			else
			{
				if (edgesSrc->size() == 0 || edgesTrg->size() == 0)
				{
					SP_LOGMESSAGE(
							wxString::Format(wxT("FaultTree not complete!")));
					SP_MESSAGEBOX(wxT("  FaultTree not complete! "), wxT("Error"),
							wxOK | wxICON_ERROR);
					return FALSE;
				}
			}
		}
	}
	if (!wasFound)
	{
		SP_LOGMESSAGE( wxString::Format(wxT("FaultTree not complete! ")));
		SP_MESSAGEBOX(wxT("  FaultTree not complete! FaultTree without top event."),
				wxT("Error"), wxOK | wxICON_ERROR);
		return FALSE;
	}
	return TRUE;
}

bool SP_ExportSyRePa::ReplaceNames()
{
	NodeMap::iterator pIt;
	lNode* p;
	wxString s;
	wxString sId;
	Basic* bEvent;

	long id = 1;
	for (pIt = m_logExp.begin(); pIt != m_logExp.end(); ++pIt)
	{
		p = (*pIt).second;

		if (p->m_name == wxT("AND"))
		{
			s += wxT("a");
		}
		if (p->m_name == wxT("OR"))
		{
			s += wxT("o");
		}
		if (p->m_name == wxT("NEG"))
		{
			s += wxT("-");
		}
		if (p->m_name == wxT("("))
		{
			s += wxT("(");
		}
		if (p->m_name == wxT(")"))
		{
			s += wxT(")");
		}

		if (p->m_name.Contains(wxT("X")) )
		{

			sId = p->m_node->GetAttribute(wxT("ID"))->GetValueString();
			s += sId;
			if (p->m_node->GetLogical())
			{
				SP_LOGMESSAGE(
						wxString::Format(wxT("FaulTree with dependent basic events!")));
			}

			bEvent = new Basic(sId,p->m_name, p->m_node);
			listElements[id] = bEvent;
			id++;
		}
	}
	f_file.Write(s);
	ReadData(listElements);
	return TRUE;
}

SP_DS_Node* SP_ExportSyRePa::SearchRoot()
{

	CHECK_POINTER(m_graph, return FALSE);

	// gets the iterator of nodeclass
	SP_ListNodeclass::const_iterator ncIt;
	const SP_ListNodeclass* ncList = m_graph->GetNodeclasses();

	CHECK_POINTER(ncList, return FALSE);

	SP_DS_Nodeclass* nc;
	SP_ListNode::const_iterator nIt;
	const SP_ListNode* nList;

	SP_DS_Node* root = NULL;

	//iterate the nodeclasses
	for (ncIt = ncList->begin(); ncIt != ncList->end(); ncIt++)
	{
		nc = (*ncIt);
		nList = nc->GetElements();

		for (nIt = nList->begin(); nIt != nList->end(); nIt++)

			if ((*nIt)->GetClassName() == wxT("Top Event"))
				root = (*nIt);
	}

	return root;
}

bool SP_ExportSyRePa::ReadData(BasicMap &elist)
{
	BasicMap elements = elist;
	BasicMap::iterator pIt;
	Basic* bEvent;

	for (pIt = elist.begin(); pIt != elist.end(); pIt++)
	{
		bEvent = (*pIt).second;
		SP_DS_ListAttribute
				* l_pcListAttr =
						dynamic_cast<SP_DS_ListAttribute*> (bEvent->m_node->GetAttribute(wxT("Failure Rate")));

		SP_DS_DoubleListAttribute* l_pcFAttr =
				dynamic_cast<SP_DS_DoubleListAttribute*>(l_pcListAttr);

		double fR = l_pcFAttr->GetValueDouble();

		SP_DS_ListAttribute
				* l_pcListRAttr =
						dynamic_cast<SP_DS_ListAttribute*> (bEvent->m_node->GetAttribute(wxT("Repair Rate")));

		SP_DS_DoubleListAttribute* l_pcRAttr =
				dynamic_cast<SP_DS_DoubleListAttribute*>(l_pcListRAttr);

		double rR = l_pcRAttr->GetValueDouble();

		wxString fRate;
		wxString rRate;
		fRate.Printf(wxT("%.16G"), fR);
		rRate.Printf(wxT("%.16G"), rR);

		data_file.Write(wxT("F") + bEvent->m_number + wxT(":") + fRate);
		data_file.Write(wxT("\n"));
		data_file.Write(wxT("R") + bEvent->m_number + wxT(":") + rRate);
		data_file.Write(wxT("\n"));
	}
	return TRUE;
}
