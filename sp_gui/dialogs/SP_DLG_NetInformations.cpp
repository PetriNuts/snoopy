//////////////////////////////////////////////////////////////////////
// $Author: crohr $
// $Version: 0.1 $
// $Date: 2008/11/17 $
// Short Description: dialog to sort nodes by attribute
//////////////////////////////////////////////////////////////////////
#include "sp_gui/dialogs/SP_DLG_NetInformations.h"

#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/extensions/SP_DS_Coarse.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gui/mdi/SP_MDI_CoarseDoc.h"
#include "sp_gui/mdi/SP_MDI_View.h"
#include "sp_ds/attributes/SP_DS_IdAttribute.h"
#include "sp_core/base/SP_Graphic.h"
#include "sp_core/SP_Core.h"

#include <wx/spinctrl.h>
#include <wx/gbsizer.h>

IMPLEMENT_CLASS(SP_DLG_NetInformations, wxDialog)

BEGIN_EVENT_TABLE(SP_DLG_NetInformations, wxDialog)
	EVT_BUTTON(wxID_OK, SP_DLG_NetInformations::OnDlgClose)
	EVT_BUTTON(wxID_CLOSE, SP_DLG_NetInformations::OnDlgClose)
END_EVENT_TABLE()

SP_DLG_NetInformations::SP_DLG_NetInformations(SP_DS_Graph *p_graph, wxWindow *p_parent, const wxString& p_title) :
	wxDialog(p_parent, -1, p_title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxSTAY_ON_TOP | wxRESIZE_BORDER),
	m_graph(p_graph), m_nodeList(NULL)
{
	wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
	wxGridBagSizer* elemSizer = new wxGridBagSizer();

	int l_nRow, l_nCol, l_nBorder;
	l_nRow = l_nCol = 0;
	l_nBorder = 5;
	elemSizer->Add(new wxStaticText(this, -1, wxT("Nodeclass")), wxGBPosition(l_nRow, l_nCol), wxGBSpan(), wxLEFT | wxRIGHT | wxTOP, l_nBorder);
	elemSizer->Add(new wxStaticText(this, -1, wxT("Count")), wxGBPosition(l_nRow, ++l_nCol), wxGBSpan(), wxLEFT | wxRIGHT | wxTOP, l_nBorder);
	SP_ListNodeclass::const_iterator ncIt;
	for (ncIt = m_graph->GetNodeclasses()->begin(); ncIt != m_graph->GetNodeclasses()->end(); ++ncIt)
	{
		if ((*ncIt)->GetShowInElementTree())
		{
			if ((*ncIt)->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID))
			{
				l_nRow++;
				m_nodeList = (*ncIt)->GetElements();
				size_t l_nSize = m_nodeList->size();
				wxString l_sName = (*ncIt)->GetDisplayName();
				elemSizer->Add(new wxStaticText(this, -1, l_sName), wxGBPosition(l_nRow, --l_nCol), wxGBSpan(), wxLEFT | wxRIGHT | wxTOP, l_nBorder);
				elemSizer->Add(new wxStaticText(this, -1, wxString::Format(wxT("%u"), l_nSize)), wxGBPosition(l_nRow, ++l_nCol), wxGBSpan(), wxLEFT | wxRIGHT | wxTOP, l_nBorder);
			}
		}
	}
	l_nRow++,
	elemSizer->Add(new wxStaticText(this, -1, wxT("Edgeclass")), wxGBPosition(++l_nRow, --l_nCol), wxGBSpan(), wxLEFT | wxRIGHT | wxTOP, l_nBorder);
	elemSizer->Add(new wxStaticText(this, -1, wxT("Count")), wxGBPosition(l_nRow, ++l_nCol), wxGBSpan(),  wxLEFT | wxRIGHT | wxTOP, l_nBorder);
	SP_ListEdgeclass::const_iterator ecIt;
	for (ecIt = m_graph->GetEdgeclasses()->begin(); ecIt != m_graph->GetEdgeclasses()->end(); ++ecIt)
	{
		l_nRow++;
		SP_DS_Edgeclass* l_pcEC = *ecIt;
		const SP_ListEdge* l_plEdge = l_pcEC->GetElements();
		size_t l_nSize = l_plEdge->size();
		wxString l_sName = l_pcEC->GetDisplayName();
		elemSizer->Add(new wxStaticText(this, -1, l_sName), wxGBPosition(l_nRow, --l_nCol), wxGBSpan(), wxLEFT | wxRIGHT | wxTOP, l_nBorder);
		elemSizer->Add(new wxStaticText(this, -1, wxString::Format(wxT("%u"), l_nSize)), wxGBPosition(l_nRow, ++l_nCol), wxGBSpan(), wxLEFT | wxRIGHT | wxTOP, l_nBorder);
	}

	topSizer->Prepend(elemSizer, 1, wxALIGN_CENTER | wxALL, 5);

	topSizer->Add(this->CreateButtonSizer(wxOK), 0, wxALL, 5);

	SetMinSize(wxSize(200,100));
	SetSizerAndFit(topSizer);

	Centre(wxBOTH);
}

SP_DLG_NetInformations::~SP_DLG_NetInformations()
{
}

void SP_DLG_NetInformations::OnDlgClose(wxCommandEvent& p_cEvent)
{
	if (IsModal())
	{
		EndModal(wxID_CANCEL);
	}
	else
	{
		SetReturnCode(wxID_CANCEL);
		this->Show(FALSE);
	}
}
