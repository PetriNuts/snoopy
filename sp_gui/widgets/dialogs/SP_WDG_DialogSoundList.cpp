//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: dbayer $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2008/09/10 11:55:00 $
// Short Description: widget displays sound files and handles association
//					  to the transition
//////////////////////////////////////////////////////////////////////

#include "sp_gui/widgets/dialogs/SP_WDG_DialogSoundList.h"
#include "sp_gui/dialogs/SP_DLG_ColListOverview.h"
#include "sp_gui/dialogs/SP_DLG_ApplySound.h"

#include "sp_ds/SP_DS_Node.h"

#include <wx/event.h>
#include <wx/dir.h>
#include "snoopy.h"
#include <wx/filename.h>

#include "sp_core/SP_GPR_Animation.h"

enum
{
	SP_ID_BUTTON_OVERVIEW,
	SP_ID_BUTTON_SELECT_SOUND,
	SP_ID_BUTTON_PLAY_SOUND,
	SP_ID_BUTTON_STOP_SOUND,
	SP_ID_LISTBOX_SOUNDS,
	SP_ID_BUTTON_APPLY_SOUND,
	SP_ID_GRID_PARAMETER
};

BEGIN_EVENT_TABLE( SP_WDG_DialogSoundList, SP_WDG_DialogBase )
#if wxABI_VERSION > 30000
	EVT_GRID_CELL_CHANGED( SP_WDG_DialogSoundList::OnGridCellValueChanged )
#else
	EVT_GRID_CELL_CHANGE( SP_WDG_DialogSoundList::OnGridCellValueChanged )
#endif
	EVT_GRID_SELECT_CELL( SP_WDG_DialogSoundList::OnGridCellSelected )

	EVT_LISTBOX(SP_ID_LISTBOX_SOUNDS, SP_WDG_DialogSoundList::OnListBoxSelect)

	EVT_BUTTON(SP_ID_BUTTON_SELECT_SOUND, SP_WDG_DialogSoundList::OnSelectSound)
	EVT_BUTTON(SP_ID_BUTTON_PLAY_SOUND, SP_WDG_DialogSoundList::OnPlaySound)
	EVT_BUTTON(SP_ID_BUTTON_STOP_SOUND, SP_WDG_DialogSoundList::OnStopSound)
	EVT_BUTTON(SP_ID_BUTTON_APPLY_SOUND, SP_WDG_DialogSoundList::OnApplySound)

END_EVENT_TABLE()

SP_WDG_DialogSoundList::SP_WDG_DialogSoundList(const wxString& p_sPage, unsigned int p_nOrdering) :
SP_WDG_DialogBase(p_sPage, p_nOrdering),
m_pcColList(NULL)
{
	m_pcSoundEngine = irrklang::createIrrKlangDevice();
}

SP_WDG_DialogSoundList::~SP_WDG_DialogSoundList()
{
	if (m_pcSoundEngine)
		m_pcSoundEngine->drop();
}

SP_WDG_DialogBase*
SP_WDG_DialogSoundList::Clone()
{

	return new SP_WDG_DialogSoundList(GetName(), GetDialogOrdering());

}


bool
SP_WDG_DialogSoundList::AddToDialog(
		const SP_ListAttribute* p_ptlAttributes,
		SP_DLG_ShapeProperties* p_pcDlg,
		bool p_bOverview)
{

	CHECK_POINTER( p_ptlAttributes, return FALSE );
	CHECK_BOOL( ! p_ptlAttributes->empty(), return FALSE );
	CHECK_POINTER( p_pcDlg, return FALSE );

	m_pcDlg = p_pcDlg;

	// remember the list of attributes, we display
	m_tlAttributes = *p_ptlAttributes;
	m_bMultiple = (p_ptlAttributes->size() > 1 );

	for(SP_ListAttribute::const_iterator it = m_tlAttributes.begin();
			it != m_tlAttributes.end(); it++)
	{
		SP_DS_Attribute* l_pcAttr = *it;
		if((m_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcAttr)))
			break;
	}

    wxString l_sPage = GetName() + wxT(":") + m_pcColList->GetParent()->GetClassName();
    SP_WDG_NotebookPage* l_pcPage = p_pcDlg->AddPage(l_sPage, GetDialogOrdering());
	CHECK_POINTER( l_pcPage, return FALSE );

	wxBoxSizer* l_pcSizer = new wxBoxSizer( wxHORIZONTAL );
	l_pcSizer->Add(new wxStaticText(l_pcPage, -1, m_pcColList->GetName()), 1, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	AddShowFlag(l_pcPage, l_pcSizer, m_pcColList);
	l_pcPage->AddControl(l_pcSizer, 0, wxEXPAND);

	l_pcSizer = new wxBoxSizer( wxHORIZONTAL );

	m_pcValueGrid = new wxGrid( l_pcPage, SP_ID_GRID_PARAMETER + m_nDialogID + wxID_HIGHEST,
			wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER );

	m_pcValueGrid->CreateGrid( 0, 0);

	m_pcValueGrid->EnableEditing( true);
	m_pcValueGrid->SetSelectionMode(wxGrid::wxGridSelectCells);

	m_pcValueGrid->SetDefaultColSize( 120, TRUE);
	m_pcValueGrid->SetDefaultRowSize( 20, TRUE);

	m_pcValueGrid->SetColLabelSize( 16);
	m_pcValueGrid->SetRowLabelSize( 0);

	LoadData();

	l_pcSizer->Add(m_pcValueGrid, 1, wxALL | wxEXPAND, 5);
	l_pcPage->AddControl(l_pcSizer, 1, wxEXPAND);

	l_pcSizer = new wxBoxSizer( wxHORIZONTAL );
	l_pcSizer->Add(new wxButton( l_pcPage, SP_ID_BUTTON_OVERVIEW + m_nDialogID +
			wxID_HIGHEST, wxT( "Lists Overview" ) ), 0, wxALL, 5);
	l_pcPage->AddControl(l_pcSizer, 0, wxEXPAND);

	/////////////////////////////////////////

	l_pcSizer->Add(new wxButton( l_pcPage, SP_ID_BUTTON_APPLY_SOUND, wxT( "Apply Sounds by Algorithm" ) ), 0, wxALL, 5);

	l_pcSizer = new wxBoxSizer( wxVERTICAL );
	l_pcPage->AddControl(l_pcSizer, 0, wxEXPAND);

	wxString l_sSoundPath = wxGetApp().GetAnimationPrefs()->GetSoundPath(SP_DS_MUSICPN_CLASS);
	wxArrayString l_aSounds = GetSoundsFromDir(l_sSoundPath);

	m_pcSoundBox = new wxListBox(l_pcPage,
									SP_ID_LISTBOX_SOUNDS,
									wxDefaultPosition,
									wxDefaultSize,
									0,
									NULL,
									wxLB_SINGLE | wxLB_NEEDED_SB | wxLB_SORT);
	m_pcSoundBox->Set(l_aSounds);
	m_pcSoundBox->SetMaxSize(wxSize(400, 400));
	wxStaticBoxSizer *l_pcSoundSectionSizer = new wxStaticBoxSizer(new wxStaticBox(
			l_pcPage, -1, wxT("Available Sounds")), wxVERTICAL);

	l_pcSoundSectionSizer->Add(m_pcSoundBox, 0, wxEXPAND | wxFIXED_MINSIZE, 5);


	wxBoxSizer *l_pcButtonSizer = new wxBoxSizer(wxHORIZONTAL);

	m_pcSelectSoundButton = new wxButton( l_pcPage, SP_ID_BUTTON_SELECT_SOUND, wxT( "Select" ) );
	m_pcSelectSoundButton->Enable(false);
	l_pcButtonSizer->Add(m_pcSelectSoundButton, 0, wxTOP, 5);

	m_pcPlaySoundButton = new wxButton( l_pcPage, SP_ID_BUTTON_PLAY_SOUND, wxT( "Play" ) );
	m_pcPlaySoundButton->Enable(false);
	l_pcButtonSizer->Add(m_pcPlaySoundButton, 0, wxALL, 5);

	m_pcStopSoundButton = new wxButton( l_pcPage, SP_ID_BUTTON_STOP_SOUND, wxT( "Stop" ) );
	m_pcStopSoundButton->Enable(false);
	l_pcButtonSizer->Add(m_pcStopSoundButton, 0, wxALL, 5);

	l_pcSoundSectionSizer->Add(l_pcButtonSizer, 1,  wxEXPAND | wxALL );
	l_pcSizer->Add(l_pcSoundSectionSizer, 1,  wxEXPAND | wxALL );

	/////////////////////////////////////////

	ConnectEvents();
	p_pcDlg->PushEventHandler( this);

	return TRUE;

}

bool
SP_WDG_DialogSoundList::OnDlgOk()
{

	SaveData();
	return SP_WDG_DialogBase::OnDlgOk();

}

bool
SP_WDG_DialogSoundList::LoadData()
{

	CHECK_POINTER(m_pcColList, return false);

	bool l_bWhite = true;

	if(m_pcValueGrid->GetNumberCols() > 0)
	{
		m_pcValueGrid->DeleteCols(0, m_pcValueGrid->GetNumberCols());
	}
	m_pcValueGrid->AppendCols(m_pcColList->GetColCount());

	for (unsigned int i=0; i<m_pcColList->GetColCount(); i++)
	{
		m_pcValueGrid->SetColLabelValue( i, m_pcColList->GetColLabel(i));
		m_pcValueGrid->SetColSize(i, 100);
	}
	if (m_pcValueGrid->GetNumberRows() > 0)
	{

		m_pcValueGrid->DeleteRows( 0, m_pcValueGrid->GetNumberRows() );

	}

	m_pcValueGrid->AppendRows(m_pcColList->GetRowCount() );

	for (unsigned int l_nRow = 0; l_nRow < m_pcColList->GetRowCount(); l_nRow++)
	{
		m_pcValueGrid->SetReadOnly(l_nRow, 0, true);
		for(unsigned int l_nCol = 0; l_nCol < m_pcColList->GetColCount(); l_nCol++)
		{
			m_pcValueGrid->SetCellValue(l_nRow, l_nCol, m_pcColList->GetCell(l_nRow, l_nCol));
			m_pcValueGrid->SetCellBackgroundColour(l_nRow, l_nCol, (l_bWhite ? *wxWHITE
							: *wxLIGHT_GREY ));
		}
		(l_bWhite ? l_bWhite = false : l_bWhite = true );
	}

	return true;

}

bool
SP_WDG_DialogSoundList::SaveData()
{

	CHECK_POINTER(m_pcColList, return false);

	m_pcValueGrid->SaveEditControlValue();

	for (int l_nRow = 0; l_nRow < m_pcValueGrid->GetNumberRows(); l_nRow++)
	{
		for(int l_nCol = 0; l_nCol < m_pcValueGrid->GetNumberCols(); l_nCol++)
		{
			m_pcColList->SetCell(l_nRow, l_nCol, m_pcValueGrid->GetCellValue(l_nRow, l_nCol));
		}
	}

	return true;

}

void
SP_WDG_DialogSoundList::OnOverview(wxCommandEvent& p_cEvent)
{
	SP_DLG_ColListOverview l_cDlg(m_pcColList->GetName(),m_pcDlg);

	if (l_cDlg.ShowModal() == wxID_OK)
	{
		LoadData();
	}
	l_cDlg.Destroy();
}

void
SP_WDG_DialogSoundList::OnApplySound(wxCommandEvent& p_cEvent)
{
	SP_DLG_ApplySound l_cDlg(m_pcColList->GetName(),m_pcDlg);

	if (l_cDlg.ShowModal() == wxID_OK)
	{
		LoadData();
	}
	l_cDlg.Destroy();

}

void
SP_WDG_DialogSoundList::ConnectEvents()
{
	Connect(
			SP_ID_BUTTON_OVERVIEW + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			( wxObjectEventFunction ) ( wxEventFunction ) (wxCommandEventFunction )
					&SP_WDG_DialogSoundList::OnOverview);
}

void
SP_WDG_DialogSoundList::DisconnectEvents()
{
	Disconnect(
			SP_ID_BUTTON_OVERVIEW + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			( wxObjectEventFunction ) ( wxEventFunction ) (wxCommandEventFunction )
					&SP_WDG_DialogSoundList::OnOverview);
}

void
SP_WDG_DialogSoundList::OnGridCellSelected(wxGridEvent& ev)
{
	ev.Skip();
}

void
SP_WDG_DialogSoundList::OnGridCellValueChanged(wxGridEvent& ev)
{
	ev.Skip();
}


void
SP_WDG_DialogSoundList::OnSelectSound(wxCommandEvent& p_cEvent)
{
	wxString l_sSound = m_pcSoundBox->GetString(m_pcSoundBox->GetSelection());


	int l_nRow = m_pcValueGrid->GetGridCursorRow();
	int l_nCol = m_pcValueGrid->GetGridCursorCol();

	if (!m_pcValueGrid->IsReadOnly(l_nRow, l_nCol))
			m_pcValueGrid->SetCellValue(l_nRow, l_nCol, l_sSound);
	else
	{
		SP_MESSAGEBOX(wxT("Please select a grid cell from the second column!"),
				wxT("Invalid Cell"),
			wxOK | wxICON_INFORMATION);
	}
}

void
SP_WDG_DialogSoundList::OnPlaySound(wxCommandEvent& p_cEvent)
{
	//get path to sound file
	wxString l_sSound = wxGetApp().GetAnimationPrefs()->GetSoundPath(SP_DS_MUSICPN_CLASS) + wxFileName::GetPathSeparator();
	//add selected sound file name
	l_sSound += m_pcSoundBox->GetString(m_pcSoundBox->GetSelection());
	//load sound
	m_pcSound = m_pcSoundEngine->play2D(l_sSound.mb_str(wxConvUTF8), false, true);

	if (m_pcSound)
	{
		m_pcSound->setSoundStopEventReceiver(this);
		m_pcPlaySoundButton->Enable(false); //prevent user from starting sound several times at once
		m_pcStopSoundButton->Enable();
		m_pcSound->setIsPaused(false);	//start sound eventually
		m_pcSound->drop();
	}
}


void
SP_WDG_DialogSoundList::OnStopSound(wxCommandEvent& p_cEvent)
{
	m_pcSoundEngine->stopAllSounds();
}

void
SP_WDG_DialogSoundList::OnListBoxSelect(wxCommandEvent& p_cEvent)
{
	m_pcPlaySoundButton->Enable();
	m_pcSelectSoundButton->Enable();
}

void
SP_WDG_DialogSoundList::OnSoundStopped(irrklang::ISound* sound, irrklang::E_STOP_EVENT_CAUSE reason, void* userData)
{
	m_pcPlaySoundButton->Enable();
	m_pcStopSoundButton->Enable(false);
}

wxArrayString
SP_WDG_DialogSoundList::GetSoundsFromDir(wxString& p_Dir)
{
	wxArrayString l_aSounds;
	wxString l_sLibrary = wxGetApp().GetAnimationPrefs()->GetSoundPath(SP_DS_MUSICPN_CLASS) + wxFileName::GetPathSeparator();

	//write all files from all subdirectories into the array
	wxDir::GetAllFiles(p_Dir, &l_aSounds);

	// remove file types that are not supported by the audio library
	// and remove the absolute path
	for (int i = l_aSounds.size()-1; i >= 0; --i)
	{
		wxFileName name(l_aSounds[i]);
		if (!IsSupportedFile(name))
		{
			l_aSounds.RemoveAt(i);
		} else
		{
			l_aSounds[i].Remove(0, l_sLibrary.Length());
		}
	}
	return l_aSounds;
}


bool
SP_WDG_DialogSoundList::IsSupportedFile(wxFileName& p_fileName)
{
	// supported formats
	wxArrayString l_aFormats;
	l_aFormats.push_back(wxT("wav"));
	l_aFormats.push_back(wxT("ogg"));
	l_aFormats.push_back(wxT("mod"));
	l_aFormats.push_back(wxT("it"));
	l_aFormats.push_back(wxT("s3m"));
	l_aFormats.push_back(wxT("xm"));

	// check file extension against list above
	for (size_t i = 0; i < l_aFormats.size(); ++i)
	{
		if (p_fileName.GetExt().CmpNoCase(l_aFormats[i]) == 0)
		{
			return true;
		}
	}
	return false;
}


