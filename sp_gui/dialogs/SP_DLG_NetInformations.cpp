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

//by george
#include "sp_ds/extensions/SP_DS_FunctionRegistry.h"
#include "sp_ds/extensions/SP_DS_FunctionEvaluator.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"
#include "sp_ds/attributes/SP_DS_TypeAttribute.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"

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
	long l_lTokenNum=ComputeTokenNUM();//added by G.A
	l_nRow++;
	elemSizer->Add(new wxStaticText(this, -1, wxT("Token Count")), wxGBPosition(l_nRow, --l_nCol), wxGBSpan(), wxLEFT | wxRIGHT | wxTOP, l_nBorder);
	elemSizer->Add(new wxStaticText(this, -1, wxString::Format(wxT("%u"), l_lTokenNum)), wxGBPosition(l_nRow, ++l_nCol), wxGBSpan(), wxLEFT | wxRIGHT | wxTOP, l_nBorder);

	topSizer->Prepend(elemSizer, 1, wxEXPAND | wxALL, 5);

	topSizer->Add(this->CreateButtonSizer(wxOK), 0, wxEXPAND | wxALL, 5);

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
void SP_DLG_NetInformations::GetConstantValue(const wxString& p_sName, double & val)
{
	SP_DS_Metadataclass* mc = m_graph->GetMetadataclass(SP_DS_META_CONSTANT);
	SP_ListMetadata::const_iterator it;
	for (it = mc->GetElements()->begin(); it != mc->GetElements()->end(); ++it) {

		SP_DS_Metadata* l_pcMetadata = *it;
		wxString l_sMetadataName = dynamic_cast<SP_DS_NameAttribute*>(l_pcMetadata->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
		SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcMetadata->GetAttribute(wxT("ValueList")));
		double l_nDouble = l_pcColList->GetActiveCellValueDouble(1);
		val = 0.0;
		if (l_sMetadataName == p_sName)
		{
			val = l_nDouble;
		}
	}
}

long SP_DLG_NetInformations::ComputeTokenNUM()
{

	long l_num = 0;
	
	SP_ListNodeclass::const_iterator ncIt;

	for (ncIt = m_graph->GetNodeclasses()->begin(); ncIt != m_graph->GetNodeclasses()->end(); ++ncIt)
	{
		if ((*ncIt)->GetShowInElementTree())
		{
			if ((*ncIt)->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID))
			{
				 
				wxString l_sClassName = (*ncIt)->GetDisplayName();
				 
				const SP_ListNode* l_pcElements = (*ncIt)->GetElements();
				for (auto l = l_pcElements->begin(); l != l_pcElements->end(); ++l)
				{
					wxString l_sNodeclass =(*l)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME)->GetValueString();
					
					const SP_ListAttribute* l_pcAttr=(*l)->GetAttributes();
					for (auto it_attr = l_pcAttr->begin(); it_attr != l_pcAttr->end(); ++it_attr)
					{
						SP_DS_Attribute* l_att = (*it_attr);
						const wxString l_sAttName=l_att->GetName();
						if (l_sClassName != wxT("Continuous Place")&& l_sAttName==wxT("Marking"))
						{//discrete places
						   long l_lval;
						   wxString l_smarking=l_att->GetValueString();
						   bool l_b= l_smarking.ToLong(&l_lval);
						   if (l_b)
						   {
							   l_num += l_lval;
						   }
						   else
						   {
							   double l_dVal = 0.0;
							   GetConstantValue(l_smarking, l_dVal);
							   l_num += l_dVal;
							  
						   }
						}
						else {

							if (l_sAttName == wxT("Marking"))
							{//for continious marking val
								double l_dDoubleMarking;
								wxString l_smarking = l_att->GetValueString();
								bool l_b = l_smarking.ToDouble(&l_dDoubleMarking);

								if (l_b)
								{
									double l_d = std::ceil(l_dDoubleMarking);
									long l_lval = l_d;
									l_num += l_lval;
								}
								else
								{
									double l_dVal = 0.0;
									GetConstantValue(l_smarking, l_dVal);
									l_num += l_dVal;
									 
								}
							}
						}
					}
					
					
				}
				
			}
		}
	}
	 
	return l_num;
}