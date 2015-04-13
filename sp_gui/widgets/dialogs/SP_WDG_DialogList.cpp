//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: dscheibl $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2005/08/11 14:25:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#include "sp_gui/widgets/dialogs/SP_WDG_DialogList.h"

enum
{
  SP_ID_DIALOGCHOICEADD,
  SP_ID_DIALOGCHOICEREMOVE,
  SP_ID_DIALOGCHOICEEDIT,
};

BEGIN_EVENT_TABLE(SP_WDG_DialogList, SP_WDG_DialogBase)
// intentionally left blank
END_EVENT_TABLE()


SP_WDG_DialogList::SP_WDG_DialogList(const wxString& p_sPage, unsigned int p_nOrdering)
:SP_WDG_DialogBase(p_sPage, p_nOrdering)
{
}

SP_WDG_DialogList::~SP_WDG_DialogList()
{
  DisconnectEvents();
}

SP_WDG_DialogBase*
SP_WDG_DialogList::Clone()
{
  return new SP_WDG_DialogList(GetName(), GetDialogOrdering());
}

bool
SP_WDG_DialogList::AddToDialog(const SP_ListAttribute* p_ptlAttributes,
                                 SP_DLG_ShapeProperties* p_pcDlg,
                                 bool p_bOverview)
{
  CHECK_POINTER(p_ptlAttributes, return FALSE);
  CHECK_BOOL(!p_ptlAttributes->empty(), return FALSE);
  CHECK_POINTER(p_pcDlg, return FALSE);
  m_pcDlg = p_pcDlg;

  // remember the list of attributes, we display
  m_tlAttributes = *p_ptlAttributes;

  wxBoxSizer* l_pcSizer = new wxBoxSizer(wxHORIZONTAL);
  m_bMultiple = (p_ptlAttributes->size() > 1);

  SP_DS_Attribute* l_pcAttr = (*p_ptlAttributes->begin());
  CHECK_BOOL(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_LIST == l_pcAttr->GetAttributeType(), return FALSE);

  wxString l_sPage = GetName() + wxT(":") + l_pcAttr->GetParent()->GetClassName();
  SP_WDG_NotebookPage* l_pcPage = p_pcDlg->AddPage(l_sPage, GetDialogOrdering());
  CHECK_POINTER(l_pcPage, return FALSE);

  SP_DS_ListAttribute* l_pcListAttr = dynamic_cast<SP_DS_ListAttribute*>(l_pcAttr);

  if ( m_bMultiple){
		 m_pcListBoxValues.Clear();
		 m_pcListBoxValues.Add(wxT("*"));
  }	else{
		m_pcListBoxValues = FillValues(l_pcListAttr);
  }
  int l_nSelectedValueIndex = l_pcListAttr->GetChoosenValueIndex();

  l_pcSizer->Add(new wxStaticText(l_pcPage,
                                  -1,
                                  l_pcListAttr->GetName()),
                 1,
                 wxALL | wxALIGN_CENTER_VERTICAL,
                 5);
  AddShowFlag(l_pcPage, l_pcSizer, l_pcListAttr);
  l_pcPage->AddControl(l_pcSizer, 0, wxEXPAND);

  l_pcSizer = new wxBoxSizer(wxHORIZONTAL);
  m_pcListBox = new wxListBox(l_pcPage, -1);
  m_pcListBox->InsertItems(m_pcListBoxValues, 0);
  if (m_pcListBoxValues.GetCount() > 0) {
    if (m_pcListBoxValues.GetCount() > (unsigned long) l_nSelectedValueIndex) {
      m_pcListBox->SetSelection(l_nSelectedValueIndex);
    } else {
      m_pcListBox->SetSelection((int) m_pcListBoxValues.GetCount() - 1);
    }
  }
  l_pcSizer->Add(m_pcListBox, 1, wxALL, 5);
  l_pcPage->AddControl(l_pcSizer, 0, wxEXPAND);

  l_pcSizer = new wxBoxSizer(wxHORIZONTAL);
  l_pcSizer->Add(new wxButton(l_pcPage,
                              SP_ID_DIALOGCHOICEADD + m_nDialogID + wxID_HIGHEST,
                              wxT("Add")),
                 1,
                 wxLEFT | wxRIGHT | wxTOP,
                 5);
  l_pcSizer->Add(new wxButton(l_pcPage,
                              SP_ID_DIALOGCHOICEREMOVE + m_nDialogID + wxID_HIGHEST,
                              wxT("Remove")),
                 1,
                 wxLEFT | wxRIGHT | wxTOP,
                 5);
  l_pcSizer->Add(new wxButton(l_pcPage,
                              SP_ID_DIALOGCHOICEEDIT + m_nDialogID + wxID_HIGHEST,
                              wxT("Edit")),
                 1,
                 wxLEFT | wxRIGHT | wxTOP,
                 5);

  l_pcPage->AddControl(l_pcSizer, 0, wxEXPAND);
  ConnectEvents();
  p_pcDlg->PushEventHandler(this);

  return TRUE;
}

wxArrayString
SP_WDG_DialogList::FillValues(SP_DS_ListAttribute* p_pcListAttr)
{
  wxArrayString l_pcResult;
  l_pcResult.Clear();
  wxArrayInt l_lMultiples;
	wxString value;
	int index;
	for (int i = 0; i < p_pcListAttr->Size(); i++)
  {
		index = l_pcResult.Index(p_pcListAttr->GetValue(i));
    if (index == wxNOT_FOUND) {
			l_pcResult.Add(p_pcListAttr->GetValue(i));
			l_lMultiples.Add(1);
		} else {
			l_lMultiples[index]++;
		}
  }
	//now appending the found multiples
	for (unsigned int j = 0; j < l_pcResult.GetCount(); j++) {
		if (l_lMultiples[j] > 1) {
			value.Printf(wxT("(%d) %s"), l_lMultiples[j], l_pcResult[j].c_str());
			l_pcResult[j] = value;
		}
	}
  return l_pcResult;
}

bool
SP_WDG_DialogList::OnDlgOk()
{
  CHECK_BOOL(!m_tlAttributes.empty(), return FALSE);
  CHECK_POINTER(m_pcListBox, return FALSE);

  bool l_bReturn = TRUE;
  int l_nSelectedValueIndex = m_pcListBox->GetSelection();
  wxString l_sValue;
  if (0 < m_pcListBox->GetCount())
  {
    l_sValue.Printf(wxT("%s"), m_pcListBox->GetString(0).c_str());
    for (unsigned int i = 1; i < m_pcListBox->GetCount(); i++)
    {
      l_sValue.Append(wxT("; "));
      l_sValue.Append(m_pcListBox->GetString(i));
    }
  }

  if (m_bMultiple && (l_sValue.Cmp(wxT("*")) == 0))
    return SP_WDG_DialogBase::OnDlgOk();

  SP_ListAttribute::const_iterator l_Iter;
  SP_DS_ListAttribute* l_pcListAttr;
  for (l_Iter = m_tlAttributes.begin(); l_Iter != m_tlAttributes.end(); ++l_Iter)
  {
    if (SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_LIST == (*l_Iter)->GetAttributeType())
    {
    	SP_DS_Attribute* l_pcAttr = *l_Iter;
    	l_pcListAttr = dynamic_cast<SP_DS_ListAttribute*>(l_pcAttr);
    	l_bReturn &= (l_pcListAttr)->SetChoosenValueIndex(l_nSelectedValueIndex);
    }
    l_bReturn &= (*l_Iter)->SetValueString(l_sValue);
  }

  return l_bReturn && SP_WDG_DialogBase::OnDlgOk();
}

void
SP_WDG_DialogList::OnAdd(wxCommandEvent& p_cEvent)
{
  wxTextEntryDialog l_pcDialog(NULL, wxT("Add a value:"), wxT("Add a value"));
  int l_nModalResult = l_pcDialog.ShowModal();
  if (wxID_OK == l_nModalResult)
  {
		m_pcListBox->Append(l_pcDialog.GetValue());
  }
}

void
SP_WDG_DialogList::OnRemove(wxCommandEvent& p_cEvent)
{
  unsigned int l_nSelection = m_pcListBox->GetSelection();
  m_pcListBox->Delete(m_pcListBox->GetSelection());
  if (l_nSelection < m_pcListBox->GetCount())
  {
    m_pcListBox->SetSelection(l_nSelection);
  } else {
    if (0 < m_pcListBox->GetCount())
    {
      m_pcListBox->SetSelection(m_pcListBox->GetCount() - 1);
    }
  }
}

void
SP_WDG_DialogList::OnEdit(wxCommandEvent& p_cEvent)
{
  wxTextEntryDialog l_pcDialog(NULL, wxT("Edit a value:"), wxT("Edit a value"));
  l_pcDialog.SetValue(m_pcListBox->GetString(m_pcListBox->GetSelection()));
  int l_nModalResult = l_pcDialog.ShowModal();
  if (wxID_OK == l_nModalResult)
  {
    m_pcListBox->SetString(m_pcListBox->GetSelection(), l_pcDialog.GetValue());
  }
}

void
SP_WDG_DialogList::ConnectEvents()
{
    Connect(SP_ID_DIALOGCHOICEADD + m_nDialogID + wxID_HIGHEST,
        wxEVT_COMMAND_BUTTON_CLICKED,
        (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_DialogList::OnAdd );

    Connect(SP_ID_DIALOGCHOICEREMOVE + m_nDialogID + wxID_HIGHEST,
        wxEVT_COMMAND_BUTTON_CLICKED,
        (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_DialogList::OnRemove );

    Connect(SP_ID_DIALOGCHOICEEDIT + m_nDialogID + wxID_HIGHEST,
        wxEVT_COMMAND_BUTTON_CLICKED,
        (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_DialogList::OnEdit );
}

void
SP_WDG_DialogList::DisconnectEvents()
{
    Disconnect(SP_ID_DIALOGCHOICEADD + m_nDialogID + wxID_HIGHEST,
        wxEVT_COMMAND_BUTTON_CLICKED,
        (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_DialogList::OnAdd );

    Disconnect(SP_ID_DIALOGCHOICEREMOVE + m_nDialogID + wxID_HIGHEST,
        wxEVT_COMMAND_BUTTON_CLICKED,
        (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_DialogList::OnRemove );

    Disconnect(SP_ID_DIALOGCHOICEEDIT + m_nDialogID + wxID_HIGHEST,
        wxEVT_COMMAND_BUTTON_CLICKED,
        (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_DialogList::OnEdit );
}
