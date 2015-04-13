//////////////////////////////////////////////////////////////////////
// $Author: crohr $
// $Version: 0.1 $
// $Date: 2008/11/17 $
// Short Description: dialog to sort nodes by attribute
//////////////////////////////////////////////////////////////////////
#include "sp_gui/dialogs/SP_DLG_CheckDuplicateNodes.h"

#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/extensions/SP_DS_Coarse.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gui/mdi/SP_MDI_CoarseDoc.h"
#include "sp_gui/mdi/SP_MDI_View.h"
#include "sp_ds/attributes/SP_DS_IdAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_core/base/SP_Graphic.h"
#include "sp_core/SP_Core.h"

#include <wx/spinctrl.h>

IMPLEMENT_CLASS(SP_DLG_CheckDuplicateNodes, wxDialog)

BEGIN_EVENT_TABLE(SP_DLG_CheckDuplicateNodes, wxDialog)
	EVT_BUTTON(wxID_OK, SP_DLG_CheckDuplicateNodes::OnDlgClose)
	EVT_BUTTON(wxID_CLOSE, SP_DLG_CheckDuplicateNodes::OnDlgClose)
END_EVENT_TABLE()

SP_DLG_CheckDuplicateNodes::SP_DLG_CheckDuplicateNodes(SP_DS_Graph *p_graph, wxWindow *p_parent, const wxString& p_title) :
	wxDialog(p_parent, -1, p_title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
	m_graph(p_graph), m_nodeList(NULL)
{
	wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* elemSizer = new wxBoxSizer(wxHORIZONTAL);

	bool l_bUniqueNodes = true;
	SP_ListNodeclass::const_iterator ncIt;

	for (ncIt = m_graph->GetNodeclasses()->begin(); ncIt != m_graph->GetNodeclasses()->end(); ++ncIt)
	{
		if ((*ncIt)->GetShowInElementTree())
		{
			if ((*ncIt)->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID) && (*ncIt)->GetPrototype()->GetAttribute(wxT("Name")))
			{
				map<wxString, SP_DS_Node*> l_mpNodes;
				for(m_nodeIt = (*ncIt)->GetElements()->begin(); m_nodeIt != (*ncIt)->GetElements()->end(); m_nodeIt++)
				{
					wxString l_sNodeclass = (*ncIt)->GetDisplayName();
					wxString l_sName = dynamic_cast<SP_DS_NameAttribute*>((*m_nodeIt)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
					if(l_mpNodes[l_sName])
					{
						wxString l_sMessage = l_sNodeclass + wxT(" ") + l_sName + wxT(" is not unique!");
						SP_LOGWARNING( l_sMessage);
						l_bUniqueNodes = false;
					}
					l_mpNodes[l_sName] = (*m_nodeIt);
				}
			}
		}
	}

	if (l_bUniqueNodes)
	{
		elemSizer->Add(new wxStaticText(this, -1, wxT("Notice: All Nodes have unique names.")));
	}
	else
	{
		elemSizer->Add(new wxStaticText(this, -1, wxT("Warning: Not all Nodes have unique names!\n\nSee log (Alt+3) for more details!")));
	}

	topSizer->Prepend(elemSizer, 1, wxALL, 5);

	topSizer->Add(this->CreateButtonSizer(wxOK), 0, wxALIGN_RIGHT | wxLEFT | wxRIGHT | wxTOP, 5);

	SetSizerAndFit(topSizer);

	Centre(wxBOTH);
}

SP_DLG_CheckDuplicateNodes::~SP_DLG_CheckDuplicateNodes()
{
}

void SP_DLG_CheckDuplicateNodes::OnDlgClose(wxCommandEvent& p_cEvent)
{
	SetReturnCode(wxID_CANCEL);
	this->Show( FALSE);
}
