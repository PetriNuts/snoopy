//////////////////////////////////////////////////////////////////////
// $Author: kw $
// $Version: 0.1 $
// $Date: 2005/12/28 $
// Short Description: dialog to browse through Invariants
//////////////////////////////////////////////////////////////////////

#include "sp_gui/dialogs/SP_DLG_ChooseInvariant.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/extensions/SP_DS_Coarse.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gui/mdi/SP_MDI_CoarseDoc.h"
#include "sp_gui/mdi/SP_MDI_View.h"
#include "sp_ds/attributes/SP_DS_IdAttribute.h"
#include "sp_core/base/SP_Graphic.h"
#include "sp_core/SP_Core.h"
#include "sp_gui/dialogs/SP_DLG_ShapeProperties.h"

#include "sp_core/vector/SP_VectorReader.h"
#include "sp_core/vector/SP_VectorWriter.h"

#include <wx/spinctrl.h>
#include <algorithm>

IMPLEMENT_CLASS(SP_DLG_ChooseInvariant, wxDialog)

BEGIN_EVENT_TABLE(SP_DLG_ChooseInvariant, wxDialog)
//  EVT_UPDATE_UI(wxID_SAVE, SP_DLG_ChooseInvariant::OnSave)
EVT_BUTTON(wxID_CANCEL, SP_DLG_ChooseInvariant::OnDlgOk)
EVT_BUTTON(SP_ID_BUTTON_FIRST_VEC, SP_DLG_ChooseInvariant::OnFirst)
EVT_BUTTON(SP_ID_BUTTON_NEXT_VEC, SP_DLG_ChooseInvariant::OnNext)
EVT_BUTTON(SP_ID_BUTTON_PREV_VEC, SP_DLG_ChooseInvariant::OnPrev)
EVT_BUTTON(SP_ID_BUTTON_LAST_VEC, SP_DLG_ChooseInvariant::OnLast)
EVT_COLOURPICKER_CHANGED(SP_ID_BUTTON_COLOUR, SP_DLG_ChooseInvariant::OnColour)
EVT_UPDATE_UI(SP_ID_BUTTON_NEXT_VEC, SP_DLG_ChooseInvariant::OnUpdateUI)
EVT_UPDATE_UI(SP_ID_BUTTON_PREV_VEC, SP_DLG_ChooseInvariant::OnUpdateUI)
EVT_LISTBOX(SP_ID_LISTBOX_VEC, SP_DLG_ChooseInvariant::OnKlick)
EVT_CHECKBOX(SP_ID_CHECKBOX_GRADIENT_VEC, SP_DLG_ChooseInvariant::OnKlick)
EVT_RADIOBOX(SP_ID_CHANGED_VEC, SP_DLG_ChooseInvariant::OnKlick)

EVT_LISTBOX_DCLICK(SP_ID_LISTBOX_VEC, SP_DLG_ChooseInvariant::OnDoubleKlick)
//EVT_KEY_DOWN(SP_DLG_ChooseInvariant::OnKeyPressed)


END_EVENT_TABLE()

SP_DLG_ChooseInvariant::SP_DLG_ChooseInvariant(SP_DS_Graph *p_pcGraph,
		wxWindow *p_parent, const wxString& p_title) :
	wxDialog(p_parent, -1, p_title, wxDefaultPosition, wxDefaultSize,
	wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER), m_pcNodeColourizer(NULL),
			m_colorize(FALSE), m_bChanged(false), m_pcListBox(NULL),
			m_pcCheckBoxGradientColouring(NULL), m_pcKeepInvariant(NULL),
			m_pcSelectNodes(NULL),
			m_pcMapListNum2NodeVectorNum(NULL), m_title(p_title), m_id(-1),
			m_pcGraph(p_pcGraph), m_nodeList(NULL)

{
	m_sIntersection = wxString(wxT("intersection"));
	m_sUnion = wxString(wxT("union"));

	m_pcNvs = new SP_DS_NodeVectorSystem(m_pcGraph);
	m_pcMapListNum2NodeVectorNum = new SP_MapLong2Long();
	m_pcNodeColourizer = new SP_NodeColourizer(m_pcGraph);
	m_nodeList = new SP_ListNode();
	InitGUI();

}

SP_DLG_ChooseInvariant::~SP_DLG_ChooseInvariant()
{
	wxDELETE(m_pcNvs);
	wxDELETE(m_pcNodeColourizer);
	wxDELETE(m_pcMapListNum2NodeVectorNum);
	wxDELETE(m_nodeList);
}

void SP_DLG_ChooseInvariant::UpdateList()
{

	m_pcListBox->Clear();

	if (!m_pcListBoxValues.IsEmpty())
	{
		m_pcListBox->InsertItems(m_pcListBoxValues, 0);
	}
}

void SP_DLG_ChooseInvariant::InitGUI()
{

	m_pcMapListNum2NodeVectorNum->clear();

	m_pcListBox = new wxListBox(this, SP_ID_LISTBOX_VEC, wxDefaultPosition,
			wxSize(150,200), 0, NULL, wxLB_EXTENDED);

	wxArrayString l_choices;
	l_choices.Add(m_sIntersection);
	l_choices.Add(m_sUnion);
	m_pcChoice = new wxRadioBox(this,
			SP_ID_CHANGED_VEC,
			wxT("Colourize"),
			wxDefaultPosition,
			wxDefaultSize,
			l_choices,
			1,
			wxRA_SPECIFY_COLS);

	wxBoxSizer* l_pcSizerTop = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* l_pcSizerNorth = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* l_pcSizerButtons = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* l_pcSizerList = new wxBoxSizer(wxVERTICAL);
	//wxBoxSizer* l_pcSizerSouth = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* l_pcSizerColour = new wxBoxSizer(wxHORIZONTAL);

	l_pcSizerList->Add(m_pcListBox, 1, wxEXPAND, 1);

	l_pcSizerButtons->Add(new wxButton(this, SP_ID_BUTTON_FIRST_VEC, wxT("&First")), 0, wxALL, 5);
	l_pcSizerButtons->Add(new wxButton(this, SP_ID_BUTTON_NEXT_VEC, wxT("&Next")), 0, wxALL, 5);
	l_pcSizerButtons->Add(new wxButton(this, SP_ID_BUTTON_PREV_VEC, wxT("&Prev")), 0, wxALL, 5);
	l_pcSizerButtons->Add(new wxButton(this, SP_ID_BUTTON_LAST_VEC, wxT("&Last")), 0, wxALL, 5);
	l_pcSizerButtons->Add(m_pcChoice, 0, wxALL, 5);

	l_pcSizerColour->Add(new wxStaticText(this, -1, _T("Colour:")), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	m_pcCheckBoxGradientColouring = new wxCheckBox(this, SP_ID_CHECKBOX_GRADIENT_VEC, wxT("gradient colouring"));
	l_pcSizerButtons->Add(m_pcCheckBoxGradientColouring, 0, wxALL, 5);

	m_pcKeepInvariant = new wxCheckBox(this, -1, wxT("keep colouring"));
	l_pcSizerButtons->Add(m_pcKeepInvariant, 0, wxALL, 5);

	m_pcSelectNodes = new wxCheckBox(this, -1, wxT("select nodes"));
	l_pcSizerButtons->Add(m_pcSelectNodes, 0, wxALL, 5);

	m_pcButtonColour = new wxColourPickerCtrl(this, SP_ID_BUTTON_COLOUR, *wxRED,
			wxDefaultPosition, wxDefaultSize,
			wxCLRP_DEFAULT_STYLE);

	l_pcSizerColour->Add(m_pcButtonColour, 0, wxALL, 5);
	l_pcSizerButtons->Add(l_pcSizerColour, 0, wxALL, 5);
	l_pcSizerButtons->Add(new wxButton(this, wxID_CANCEL, wxT("&Close")), 0, wxALL, 5);

	l_pcSizerNorth->Add(l_pcSizerList, 1, wxEXPAND, 1);
	l_pcSizerNorth->Add(l_pcSizerButtons, 0, wxALIGN_RIGHT, 0);

	// @todo insert color choosing
	l_pcSizerTop->Add(l_pcSizerNorth, 1, wxALL | wxEXPAND, 1);

	SetSizerAndFit(l_pcSizerTop);

	Centre(wxBOTH);
	//m_invariant_sort = SP_T_INVARIANT;

}

void SP_DLG_ChooseInvariant::OnDlgOk(wxCommandEvent& p_cEvent)
{
	// @todo discard the old colours
	if(!m_pcKeepInvariant->IsChecked())
	{
		m_pcNodeColourizer->RestoreColours();
		m_pcNodeColourizer->DiscardOldColours();
	}

	if (m_bChanged)
	{
		// @todo ask if changes should be saved
		int l_pcAsk = SP_MESSAGEBOX(
				wxString::Format(wxT("Do you want to save changes to %s"), m_fileName.c_str()),
				wxT("Snoopy"),
				wxYES_NO|wxYES_DEFAULT);
		if (l_pcAsk == wxID_YES)
		{
			SP_VectorWriter l_pcWriter;
			l_pcWriter.Write(m_pcNvs, m_fileName.c_str());
		}

	}
	this->Show(false);
}

void SP_DLG_ChooseInvariant::DiscardColours()
{
	// discard the old colours
	m_pcNodeColourizer->DiscardOldColours();
}

void SP_DLG_ChooseInvariant::OnFirst(wxCommandEvent& p_cEvent)
{
	CHECK_POINTER(m_pcListBox,return);

	m_aVectorNumbers.Clear();
	if (m_pcListBox->GetSelections(m_aVectorNumbers)>0)
	{
		for (size_t Counter = 0; Counter < m_aVectorNumbers.GetCount(); Counter++)
		{
			int l_nNum = m_aVectorNumbers.Item(Counter);
			m_pcListBox->Deselect(l_nNum);
		}
	}

	if (m_pcListBox->GetCount() > 0)
	{
		m_pcListBox->SetSelection(0);
		OnChoosingInvariant();
	}
}

void SP_DLG_ChooseInvariant::OnLast(wxCommandEvent& p_cEvent)
{
	CHECK_POINTER(m_pcListBox,return);

	m_aVectorNumbers.Clear();
	if (m_pcListBox->GetSelections(m_aVectorNumbers)>0)
	{
		for (size_t Counter = 0; Counter < m_aVectorNumbers.GetCount(); Counter++)
		{
			int l_nNum = m_aVectorNumbers.Item(Counter);
			m_pcListBox->Deselect(l_nNum);
		}
	}

	if (m_pcListBox->GetCount() > 0)
	{
		m_pcListBox->SetSelection(m_pcListBox->GetCount()-1);
		OnChoosingInvariant();
	}
}

void SP_DLG_ChooseInvariant::OnNext(wxCommandEvent& p_cEvent)
{
	CHECK_POINTER(m_pcListBox,return);
	m_aVectorNumbers.Clear();
	int l_nCount = m_pcListBox->GetSelections(m_aVectorNumbers);
	if (l_nCount != 1)
	{
		//do nothing
	}
	else
	{
		unsigned int l_nSel = m_aVectorNumbers.Item(0);
		if (l_nSel+1 < m_pcListBox->GetCount())
		{
			m_pcListBox->SetSelection(l_nSel+1);
			m_pcListBox->Deselect(l_nSel);

			OnChoosingInvariant();
		}
	}
}

void SP_DLG_ChooseInvariant::OnPrev(wxCommandEvent& p_cEvent)
{
	CHECK_POINTER(m_pcListBox,return);
	m_aVectorNumbers.Clear();
	int l_nCount = m_pcListBox->GetSelections(m_aVectorNumbers) ;
	if (l_nCount == 1)
	{
		//one item selected
		int l_nSel = m_aVectorNumbers.Item(0);
		if (l_nSel > 0)
		{
			m_pcListBox->SetSelection(l_nSel-1);
			m_pcListBox->Deselect(l_nSel);

			OnChoosingInvariant();
		}
	}

}

void SP_DLG_ChooseInvariant::OnColour(wxColourPickerEvent& p_cEvent)
{
	RefreshColour();
	OnKlick(p_cEvent);
}

void SP_DLG_ChooseInvariant::RefreshColour()
{
	m_pcButtonColour->Refresh();
}

void SP_DLG_ChooseInvariant::OnKlick(wxCommandEvent& p_cEvent)
{
	CHECK_POINTER(m_pcListBox,return);

	m_aVectorNumbers.Clear();
	m_pcNodeColourizer->RestoreColours();

	unsigned int l_nCount = m_pcListBox->GetSelections(m_aVectorNumbers) ;

	if (l_nCount == 0)
	{
		return;
	}

	if (l_nCount == 1)
	{
		//one item selected
		OnChoosingInvariant();
	}
	else
	{ // colour union or intersection of the chosen nodevectors
		const SP_ListNode& l_pclAllNodes = m_pcNvs->GetNodes();

		// to colour the sums in gradient colours
		SP_VectorLong l_apSumVector(l_pclAllNodes.size(), 0);
		SP_VectorLong::iterator l_itSumVector = l_apSumVector.begin();

		// clear m_nodeList for 'union 'to put all nodes occurring in node vectors
		// put all nodes in m_nodeList for 'intersection' to remove those who doen't occur
		if (m_pcChoice->GetStringSelection() == m_sUnion)
		{
			m_nodeList->clear();
		}
		else if (m_pcChoice->GetStringSelection() == m_sIntersection)
		{
			for(m_nodeIt = l_pclAllNodes.begin();
				m_nodeIt != l_pclAllNodes.end(); m_nodeIt++)
			{
				m_nodeList->push_back(*m_nodeIt);
			}
		}

		SP_DS_NodeVector *l_pcNodeVector;

		// remind min and max of the nodes to show during looking at the last vector
		int l_min = 32767;
		int l_max = 0;

		for (size_t Counter = 0; Counter < l_nCount; Counter++)
		{
			int l_nNum = m_aVectorNumbers.Item(Counter);

			l_pcNodeVector = m_pcNvs->GetNodeVectorById((*m_pcMapListNum2NodeVectorNum)[l_nNum]);
			if (!l_pcNodeVector)
			{
				return;
			}

			SP_VectorLong::const_iterator l_aVector = l_pcNodeVector->GetVector().begin();
			l_itSumVector = l_apSumVector.begin();

			for(m_nodeIt = l_pclAllNodes.begin();
				m_nodeIt != l_pclAllNodes.end(); m_nodeIt++)
			{
				*l_itSumVector += *l_aVector;

				if (l_max < *l_itSumVector)
				{
					l_max = *l_itSumVector;
				}

				if (l_min > *l_itSumVector && *l_itSumVector > 0)
				{
					l_min = *l_itSumVector;
				}

				if ( *l_aVector > 0)
				{
					if (m_pcChoice->GetStringSelection() == m_sUnion)
					{
						// add only
						if (SP_Find(*m_nodeList, *m_nodeIt)	== m_nodeList->end())
						{
							m_nodeList->push_back(*m_nodeIt);
						}
					}
				}
				else if (*l_aVector == 0)
				{
					if (m_pcChoice->GetStringSelection() == m_sIntersection)
					{
						m_nodeList->remove(*m_nodeIt);

					}
				}
				else
				{//shouldn't be
					return;
				}
				l_aVector++;
				l_itSumVector++;
			}
		}

		if (m_pcCheckBoxGradientColouring->GetValue())
		{
			SP_DS_NodeVectorSystem* l_pcNvs = new SP_DS_NodeVectorSystem(m_pcGraph);
			l_pcNvs->SetNodeList(m_pcNvs->GetNodes());
			l_pcNvs->SetOccurrenceRangeMin(l_min);
			l_pcNvs->SetOccurrenceRangeMax(l_max);

			SP_DS_NodeVector* l_pcNodeVector = new SP_DS_NodeVector(m_pcGraph, l_pcNvs);

			// when intersection is used, some entrys have to be set on null
			l_itSumVector = l_apSumVector.begin();
			for(m_nodeIt = l_pclAllNodes.begin(); m_nodeIt != l_pclAllNodes.end(); m_nodeIt++)
			{
				if(SP_Find(*m_nodeList, *m_nodeIt) == m_nodeList->end())
				{
					*l_itSumVector = 0;
				}
				l_itSumVector++;
			}

			l_pcNodeVector->SetVector(l_apSumVector);
			l_pcNvs->AddElement(l_pcNodeVector);

			Visit(l_pcNodeVector);
			wxDELETE(l_pcNvs);
		}
		else
		{
			m_pcNodeColourizer->ColourNodeSet(m_nodeList, m_pcButtonColour->GetColour());
		}

	}
	this->Raise();
}

void SP_DLG_ChooseInvariant::OnDoubleKlick(wxCommandEvent& p_cEvent)
{
	// map of list of attributes
	SP_MapString2ListAttribute l_tmAttributes;
	// construct the dialog

	m_aVectorNumbers.Clear();
	// get chosen Nodevector
	if (m_pcListBox->GetSelections(m_aVectorNumbers) == 1)
	{
		SP_DS_NodeVector * l_pcVec = m_pcNvs->GetNodeVectorById((*m_pcMapListNum2NodeVectorNum)[ m_aVectorNumbers.Item(0)]);
		CHECK_POINTER(l_pcVec, return);

		//l_pcVec->AddDialogAttributes(&l_tmAttributes);
		//SP_DLG_ShapeProperties(null);
		//ShowDialogAttributes(l_tmAttributes);
		if (l_pcVec->EditProperties())
		{
			// set changed-flag
			m_bChanged = TRUE;
			// new entry
			SP_DS_IdAttribute* l_pcIdAttr =
							dynamic_cast<SP_DS_IdAttribute*>(l_pcVec->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID));

			SP_DS_Attribute* l_pcAttr = l_pcVec->GetAttribute(wxT("Name"));
			long l_nNvId = -1;
			if (l_pcIdAttr)
			{
				l_nNvId = l_pcIdAttr->GetValue();
			}
			if (l_pcAttr)
			{
				m_pcListBoxValues[m_aVectorNumbers.Item(0)] = wxString::Format(wxT("%ld: %s"), l_nNvId,
						l_pcAttr->GetValueString().c_str());
			}
			else
			{
				m_pcListBoxValues[m_aVectorNumbers.Item(0)] = wxString::Format(wxT("%ld:"), l_nNvId);
			}
			m_pcListBox->Set(m_pcListBoxValues);
		}
	}
}

bool SP_DLG_ChooseInvariant::LoadInvariantFile()
{
	m_pcMapListNum2NodeVectorNum->clear();
	m_pcListBoxValues.Clear();

	wxString fileName;
	m_fileName = fileName;

	wxString l_sSelName = wxT("");
	wxString l_sFileExt = wxT("");
	wxString l_sFileExtFilter =
			wxT("Invariant and node set file (*.inv;*.res)|*.inv;*.res|Invariant file (*.inv)|*.inv|Node set file (*.res)|*.res|All files|*");
	wxString l_sSelDir = wxT("");
	fileName = wxFileSelector(wxT("Choose a file with node sets"), l_sSelDir,
			l_sSelName, l_sFileExt, l_sFileExtFilter, wxFD_OPEN);
	if (fileName.empty())
	{
		return FALSE;
	}

	m_fileName = fileName;
	SP_VectorReader l_pcVectorReader(m_pcGraph);
	if (m_pcNvs)
	{
		m_pcNvs->Clear();
		if (!l_pcVectorReader.Read(m_pcNvs, fileName))
		{
			SP_LOGDEBUG(wxT("Error parsing node set file! Please try Edit->Check Net!"));
			SP_MESSAGEBOX(wxT("Error parsing node set file!\nPlease try Edit->Check Net!"), wxT("Error parsing invariant file!"));

			return FALSE;
		}
	}
	SetTitle(fileName);
	//initialize list with numbers
	m_colorize = FALSE;
	m_bChanged = false;
	m_pcNvs->AcceptAll(this);

	UpdateList();
	return TRUE;
}

void SP_DLG_ChooseInvariant::OnChoosingInvariant()
{
	m_colorize = TRUE;
	m_pcNodeColourizer->RestoreColours();
	// @todo eigentlich Nvs.visitwhere("id = liste.aktuelleId, this);
	m_aVectorNumbers.Clear();
	m_pcListBox->GetSelections(m_aVectorNumbers);
	SP_DS_NodeVector* l_pcVec= NULL;
	if (!m_aVectorNumbers.IsEmpty())
	{
		l_pcVec = m_pcNvs->GetNodeVectorById((*m_pcMapListNum2NodeVectorNum)[m_aVectorNumbers.Item(0)]);
	}
	if (l_pcVec)
	{
		Visit(l_pcVec);
	}

}

/*
 * -checks if the next or prevous button has to be grey
 */
void SP_DLG_ChooseInvariant::OnUpdateUI(wxUpdateUIEvent& p_event)
{
	m_pcNodeColourizer->SetSelectNodes(m_pcSelectNodes->GetValue());

	// @todo implementieren: listenelement eins weiter setzen, ggf buttons ausblenden,
	//
	m_aVectorNumbers.Clear();
	m_pcListBox->GetSelections(m_aVectorNumbers);
	if (m_aVectorNumbers.IsEmpty())
	{
		return;
	}

	switch (p_event.GetId())
	{
	case SP_ID_BUTTON_NEXT:
		p_event.Enable(m_pcListBox && m_pcListBox->GetCount()
				> (unsigned int)m_aVectorNumbers.Item(0)+1);

		break;
	case SP_ID_BUTTON_PREV:
		p_event.Enable(m_pcListBox && m_aVectorNumbers.Item(0) != 0);
		break;
	}
}

void SP_DLG_ChooseInvariant::SetTitlePrefix(const wxString& p_val)
{
	m_titlePrefix = p_val;
	SetTitle(p_val + m_title);
}

wxString SP_DLG_ChooseInvariant::GetTitlePrefix()
{
	return m_titlePrefix;
}

bool SP_DLG_ChooseInvariant::Visit(SP_DS_NodeVector * p_pcNodeVector)
{
	CHECK_POINTER(m_pcNvs, return FALSE);
	bool l_bReturn = true;
	m_nodeList->clear();

	if (m_colorize)
	{
		m_pcNodeColourizer->ColourNodeVector(p_pcNodeVector,
											m_pcButtonColour->GetColour(),
											m_pcCheckBoxGradientColouring->GetValue());
	}
	else
	{

		// get id or name from invariant and put it in the show list
		// the show list is the list where all invariants are shown to the user

		long l_listSize = m_pcListBoxValues.GetCount();
		long l_nNvId = -1;
		if (dynamic_cast<SP_DS_IdAttribute*>(p_pcNodeVector->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID)))
		{
			l_nNvId = dynamic_cast<SP_DS_IdAttribute*>(
					p_pcNodeVector->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID))->GetValue();
		}
		// if a name exists, add name, else add the number of the node vector
		SP_DS_Attribute* l_pcAttr = p_pcNodeVector->GetAttribute(wxT("Name"));
		if (l_pcAttr)
		{
				m_pcListBoxValues.Add(wxString::Format(wxT("%ld: %s"), l_nNvId,
						l_pcAttr->GetValueString().c_str()));
		}
		else
		{
			m_pcListBoxValues.Add(wxString::Format(wxT("%ld"), l_nNvId));
		}
		m_pcMapListNum2NodeVectorNum->insert(SP_PairLong(l_listSize, l_nNvId));

		l_bReturn = false;
	}

	this->Raise();

	return l_bReturn;
}

/*
 void
 SP_DLG_ChooseInvariant::OnKeyPressed(wxCommandEvent& p_cEvent){
 SP_LOGDEBUG(wxT("key pressed"));
 }*/
