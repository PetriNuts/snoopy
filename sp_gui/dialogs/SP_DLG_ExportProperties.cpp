//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: cr $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2009/03/30 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "export/SP_ExportRoutine.h"

#include <wx/filename.h>

#include "sp_gui/dialogs/SP_DLG_ExportProperties.h"
#include "sp_gui/windows/SP_GUI_Mainframe.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_core/tools/SP_StopWatch.h"

#include <wx/busyinfo.h>

IMPLEMENT_CLASS(SP_DLG_ExportProperties, wxDialog)

BEGIN_EVENT_TABLE(SP_DLG_ExportProperties, wxDialog)
EVT_BUTTON(wxID_OK, SP_DLG_ExportProperties::OnDlgOk)
EVT_BUTTON(wxID_CANCEL, SP_DLG_ExportProperties::OnDlgCancel)
END_EVENT_TABLE()

SP_DLG_ExportProperties::SP_DLG_ExportProperties(SP_ExportRoutine* p_pcExport,
                                                       wxWindow* p_pcParent,
                                                       SP_MDI_Doc* p_pcDoc,
                                                       const wxString& p_sFilename,
                                                       const wxString& p_sTitle,
                                                       long p_nStyle)
  : wxDialog(p_pcParent, -1, p_sTitle, wxDefaultPosition, wxDefaultSize,
	     p_nStyle | wxRESIZE_BORDER | wxMAXIMIZE_BOX),
    m_pcExport(p_pcExport),
    m_pcDoc(p_pcDoc)
{
	
    /* top level sizer */
    m_pcSizer = new wxBoxSizer(wxVERTICAL);

    /* notebook to show multiple pages on */
    m_pcNotebook = new SP_WDG_Notebook(this, wxT("Export"), wxDefaultPosition, wxSize(300,300));
    m_pcSizer->Add(m_pcNotebook, 1, wxEXPAND | wxALL, 5);

    SP_WDG_NotebookPage* l_pcNotebookPage = AddPage(wxT("General"));

    wxSizer* l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);

	l_pcRowSizer->Add(new wxStaticText( l_pcNotebookPage, -1, wxT("Filename:") ), 0, wxALL | wxEXPAND, 5);

    //SetFileName( p_sFilename );
	wxString l_sPath = p_sFilename + wxT(".") + m_pcExport->GetExtension();
	m_pcFilePickerCtrl = new wxFilePickerCtrl( l_pcNotebookPage, -1, l_sPath, wxT("Browse"),
			m_pcExport->GetFileDescr() + wxT("(*.")	+ m_pcExport->GetExtension() + wxT(")|*.") + m_pcExport->GetExtension(),
			wxDefaultPosition, wxDefaultSize, wxFLP_SAVE | wxFLP_USE_TEXTCTRL);
	m_pcFilePickerCtrl->SetPath(l_sPath);

	l_pcRowSizer->Add(m_pcFilePickerCtrl, 1, wxALL | wxEXPAND, 5);

	l_pcNotebookPage->AddControl(l_pcRowSizer, 0, wxALL | wxEXPAND, 5);

	wxString l_sNetClassName = m_pcDoc->GetGraph()->GetNetclass()->GetName();

	//search for collist attributes
	for (SP_ListNodeclass::const_iterator itNC = m_pcDoc->GetGraph()->GetNodeclasses()->begin(); itNC != m_pcDoc->GetGraph()->GetNodeclasses()->end(); itNC++)
	{
		if(!(*itNC)->GetElements()->empty() && (*itNC)->GetShowInElementTree())
		{
			SP_DS_Node* l_pcProtoNode = (*itNC)->GetPrototype();
			SP_DS_Node* l_pcNode = (*itNC)->GetElements()->front();
			SP_ListAttribute::const_iterator itAttr = l_pcNode->GetAttributes()->begin();
			SP_ListAttribute::const_iterator itProtoAttr = l_pcProtoNode->GetAttributes()->begin();
			for(;itAttr != l_pcNode->GetAttributes()->end(); itAttr++, itProtoAttr++)
			{
				SP_DS_Attribute* l_pcAttr = *itAttr;
				SP_DS_Attribute* l_pcProtoAttr = *itProtoAttr;
				if (l_pcAttr->GetAttributeType() == SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_COLLIST)
				{
					vector<wxString> l_vSetNames;
					SP_DS_ColListAttribute* l_pcColAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcAttr);					
					SP_DS_ColListAttribute* l_pcProtoColAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcProtoAttr);
					
					if(l_pcProtoColAttr->GetColLabel(1).Find(wxT(":")) != wxNOT_FOUND)
					{
						for(unsigned i = 1; i < l_pcColAttr->GetColCount(); i++)
						{
							wxString l_sSetName = l_pcColAttr->GetColLabel(i);
							l_sSetName = l_sSetName.BeforeFirst(wxT(':'));
							if(SP_Find(l_vSetNames, l_sSetName) == l_vSetNames.end())
							{
								l_vSetNames.push_back(l_sSetName);
							}
						}
					}
					else
					{
						for(unsigned i = 0; i < l_pcColAttr->GetRowCount(); i++)
						{
							wxString l_sSetName = l_pcColAttr->GetCell(i,0);
							if(SP_Find(l_vSetNames, l_sSetName) == l_vSetNames.end())
							{
								l_vSetNames.push_back(l_sSetName);
							}
						}
					}

					// rows of SetsSizer

					l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );
					wxString l_sName;
					l_sName << l_pcProtoColAttr->GetDisplayName() << wxT(":");
					l_pcRowSizer->Add(new wxStaticText( l_pcNotebookPage, -1, l_sName ), 1, wxALL|wxEXPAND, 5);

					wxComboBox* l_pcComboBox = new wxComboBox( l_pcNotebookPage, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY );

				
					
					for (unsigned int i = 0; i < l_vSetNames.size(); i++)
					{
						l_pcComboBox->Append( l_vSetNames[i]);
					}					

					
					l_pcComboBox->SetSelection(l_pcColAttr->GetActiveList());

					m_mColListComboBoxes[l_pcColAttr] = l_pcComboBox;

					l_pcRowSizer->Add(l_pcComboBox, 1, wxALL|wxEXPAND | wxALIGN_RIGHT, 5);

					l_pcNotebookPage->AddControl(l_pcRowSizer, 0, wxALL | wxEXPAND, 5);
				}
			}
		}
	}


	/*
	 * comboboxes for constants
	 */
	SP_DS_Graph* l_pcGraph = m_pcDoc->GetGraph();
	SP_DS_Metadataclass* mc = l_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);
	if (mc && !mc->GetElements()->empty() && !l_pcGraph->GetName().Contains(wxT("Colored")) )
	{
		////////////////////////////////////////////////////////////////////////////////////////////////////
		// load groups
		////////////////////////////////////////////////////////////////////////
		SP_ListMetadata::const_iterator it;
		SP_DS_Metadata* l_pcMetadata;

		wxArrayString m_choices;
		for (it = mc->GetElements()->begin(); it != mc->GetElements()->end(); ++it)
		{
			l_pcMetadata = *it;
			wxString l_sGroup = dynamic_cast<SP_DS_TextAttribute*>(l_pcMetadata->GetAttribute(wxT("Group")))->GetValue();
			SP_DS_ColListAttribute* l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*> (l_pcMetadata->GetAttribute(wxT("ValueList")));
			if (SP_Find(m_choices, l_sGroup) == m_choices.end())
			{
				m_choices.Add(l_sGroup);
				l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );
				l_pcRowSizer->Add( new wxStaticText( l_pcNotebookPage, -1, m_choices.Last() ), 1, wxALL | wxEXPAND, 5 );
				wxComboBox* l_pcComboBox = new wxComboBox( l_pcNotebookPage, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY );

				for(unsigned int i = 0; i < l_pcAttr->GetRowCount(); i++ )
				{
					wxString l_sSetName = l_pcAttr->GetCell(i,0);
					l_pcComboBox->Append(l_sSetName);
				}

				l_pcComboBox->SetSelection(l_pcAttr->GetActiveList());
				m_mColListComboBoxes[l_pcAttr] = l_pcComboBox;
				l_pcRowSizer->Add(l_pcComboBox, 1, wxALL|wxEXPAND | wxALIGN_RIGHT, 5);
				l_pcNotebookPage->AddControl(l_pcRowSizer, 0, wxALL | wxEXPAND, 5);
			}
		}

	}

	//////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////

    /* Buttons in the lower right hand corner */
    wxSizer *l_pcButtonSizer = new wxBoxSizer( wxHORIZONTAL );
	l_pcButtonSizer->Add(this->CreateButtonSizer(wxOK|wxCANCEL), 0, wxALL, 5);

    m_pcSizer->Add(l_pcButtonSizer, 0, wxALIGN_RIGHT);

    // export properties come from the export, of course
    m_pcExport->AddToDialog(this, m_pcDoc);

    SetSizerAndFit(m_pcSizer);
#if wxABI_VERSION > 30000
    m_pcSizer->FitInside(this);
#else
    m_pcSizer->SetVirtualSizeHints(this);
#endif

    Centre(wxBOTH);
}

SP_WDG_NotebookPage*
SP_DLG_ExportProperties::AddPage(const wxString& p_sLabel)
{
    if (m_pcNotebook)
        return m_pcNotebook->AddOrGetPage(p_sLabel);

    return NULL;
}
//------------------------------------------------------------------------
void
SP_DLG_ExportProperties::OnDlgOk(wxCommandEvent& p_cEvent)
{
	if ( Validate() && TransferDataFromWindow() )
	{
		wxString l_sOutName = m_pcFilePickerCtrl->GetPath();

		SP_StopWatch sw(wxString::Format(wxT("\nDoing %s: %s\n"),
							m_pcExport->GetName().c_str(),
							l_sOutName.c_str()),
						wxT("Finished export in"));

		//Add extension to the file name
		if(!l_sOutName.EndsWith(wxT(".")+m_pcExport->GetExtension()))
		    l_sOutName+=wxT(".")+m_pcExport->GetExtension();

		if (wxFileName::FileExists(l_sOutName) )
		{
			int l_nAnswer = SP_MESSAGEBOX(wxT("File ") + l_sOutName
										+ wxT(" exists. Overwrite?"),
										wxT("Overwrite?"),
										wxYES_NO | wxICON_QUESTION);
			if (l_nAnswer != wxYES)
			{
				return;
			}
			SP_LOGWARNING( wxString::Format(wxT("Overwriting file %s"), l_sOutName.c_str()));
		}
		SaveData();
        if (!m_pcExport->OnDialogOk())
        {
            return;
        }

        int l_nReturn = wxID_OK;

        wxWindowDisabler disableAll;
        wxBusyInfo wait("Please wait, exporting...");

        if(!m_pcExport->Write(m_pcDoc, l_sOutName))
        {
        	l_nReturn = wxID_CANCEL;
        }

        if ( IsModal() )
            EndModal(l_nReturn);
        else
        {
		    SetReturnCode(l_nReturn);
		    this->Show(FALSE);
        }
	}
}

void
SP_DLG_ExportProperties::OnDlgCancel(wxCommandEvent& p_cEvent)
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

void
SP_DLG_ExportProperties::SaveData()
{
	map<SP_DS_ColListAttribute*, wxComboBox*>::const_iterator l_it;
	for(l_it = m_mColListComboBoxes.begin(); l_it != m_mColListComboBoxes.end(); l_it++)
	{
		SP_DS_ColListAttribute* l_pcCollist = (*l_it).first;
		wxComboBox* l_pcComboBox = (*l_it).second;
		l_pcCollist->SetActiveList(l_pcComboBox->GetSelection());

		l_pcCollist->SetActiveColumn(l_pcComboBox->GetSelection());
	}
}


//added by anjali
void
SP_DLG_ExportProperties::SetNotebookSize(int width, int height)
{
	m_pcNotebook->SetInitialSize( wxSize(width, height) );
}
