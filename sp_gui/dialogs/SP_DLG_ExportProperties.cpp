//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: cr $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2009/03/30 11:55:00 $
// $Modified: George Assaf$
// $Date: 01.12.2020$
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "export/SP_ExportRoutine.h"

#include <wx/filename.h>
#include <wx/gbsizer.h>
#include <wx/busyinfo.h>

#include "sp_gui/dialogs/SP_DLG_ExportProperties.h"
#include "sp_gui/windows/SP_GUI_Mainframe.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_core/tools/SP_StopWatch.h"

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
	     p_nStyle | wxSTAY_ON_TOP | wxRESIZE_BORDER | wxMAXIMIZE_BOX),
    m_pcExport(p_pcExport),
    m_pcDoc(p_pcDoc)
{
	
    /* top level sizer */
    m_pcSizer = new wxBoxSizer(wxVERTICAL);

    /* notebook to show multiple pages on */
    m_pcNotebook = new SP_WDG_Notebook(this, wxT("Export"), wxDefaultPosition, wxSize(400,300));
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

	std::map<wxString, std::vector<wxString>> l_mGroups2Vsets = GetGroup2ValueSets();

	l_pcRowSizer->Add(m_pcFilePickerCtrl, 1, wxALL | wxEXPAND, 5);

	l_pcNotebookPage->AddControl(l_pcRowSizer, 0, wxALL | wxEXPAND, 5);

	wxString l_sNetClassName = m_pcDoc->GetGraph()->GetNetclass()->GetName();

	bool l_bIsExportfromColpnToAndLUsingDSSD = false;

	wxString l_sExportName = m_pcExport->GetName();


	wxString l_sExporttoANDL = wxT("Export to ANDL using dssd_util");

	if (l_sExportName == l_sExporttoANDL)
	{
		l_bIsExportfromColpnToAndLUsingDSSD = true;
	}

	auto l_pcSetsSizer = new wxGridBagSizer(5,5);
	int row = 0;
	vector<wxString> l_vGroupNames;
	//search for collist attributes
	for (SP_ListNodeclass::const_iterator itNC = m_pcDoc->GetGraph()->GetNodeclasses()->begin(); itNC != m_pcDoc->GetGraph()->GetNodeclasses()->end(); itNC++)
	{
		if (!(*itNC)->GetElements()->empty() && (*itNC)->GetShowInElementTree())
		{
			if (!l_sNetClassName.Contains(wxT("Colored")) && (*itNC)->GetDisplayName().Contains(wxT("Transition"))
				&& m_pcExport->GetExtension() != wxT("andl"))
			{
				continue;
				/*export rate funs from uncolored net to all other net classes should not allow a user to choose a certain v-set to be exported,
				because all vsets has to be kept in the target net class
				*/
			}

			if (!l_bIsExportfromColpnToAndLUsingDSSD && l_sNetClassName.Contains(wxT("Colored")) && ((*itNC)->GetDisplayName().Contains(wxT("Transition")) || (*itNC)->GetDisplayName().Contains(wxT("Place")))
				&& ((m_pcExport->GetExtension() == wxT("andl") || m_pcExport->GetExtension() == wxT("spn") || m_pcExport->GetExtension() == wxT("cpn")
					|| m_pcExport->GetExtension() == wxT("hpn") || m_pcExport->GetExtension() == wxT("xpn") || m_pcExport->GetExtension() == wxT("pn"))))
			{
				//for exporting to net class requiring unfolding of the coloured model, we give a user to choose the rate funs and marking from unfolding window.
				continue;
			}

			if (l_sNetClassName.Contains(wxT("Colored")) && (*itNC)->GetDisplayName().Contains(wxT("Transition"))
				&& !((m_pcExport->GetExtension() == wxT("andl") || m_pcExport->GetExtension() == wxT("candl")
					 || m_pcExport->GetExtension() == wxT("spn") || m_pcExport->GetExtension() == wxT("cpn")
					|| m_pcExport->GetExtension() == wxT("hpn") || m_pcExport->GetExtension() == wxT("xpn") ||
					m_pcExport->GetExtension() == wxT("pn"))) && !l_bIsExportfromColpnToAndLUsingDSSD)
			{
				continue;
				/*export rate funs from colored net to all other coloured net classes should not allow a user to choose a certain v-set to be exported,
				because all vsets has to be kept in the target net class
				The exception includes, uncoloured nets and candl/andl format as there must be choice for which rate fun we should do unfold
				*/
			}

			if (!l_sNetClassName.Contains(wxT("Colored")) && (*itNC)->GetDisplayName().Contains(wxT("Place")))
			{
				continue;
				/*in uncolored nets, there is only one marking set, no choices to choose,
				so we do not make it occur in the export properties window
				*/
			}

			if (l_sNetClassName.Contains(wxT("Colored")) && (*itNC)->GetDisplayName().Contains(wxT("Place"))
				&& (m_pcExport->GetExtension() == wxT("colspn") || m_pcExport->GetExtension() == wxT("colcpn")
					|| m_pcExport->GetExtension() == wxT("colhpn") || m_pcExport->GetExtension() == wxT("colfspn")
					|| m_pcExport->GetExtension() == wxT("colfcpn") || m_pcExport->GetExtension() == wxT("colfhpn"))
				)
			{
				continue;
				/*in coloured quantative pns, marking value sets have to be exported to their quantative coloured nets as such
				there should be no choice, with exptions including ANDL/CANDL/Uncolored nets as there is no multi VSETS for marking
				*/
			}


			if (l_sNetClassName.Contains(wxT("Colored")) && (*itNC)->GetDisplayName().Contains(wxT("Transition")) &&((*itNC)->GetDisplayName().Contains("Immediate")|| (*itNC)->GetDisplayName().Contains("Scheduled")
				|| (*itNC)->GetDisplayName().Contains("Determenstic") || (*itNC)->GetDisplayName().Contains("Delay"))
				&& (m_pcExport->GetExtension() == wxT("colspn")     || m_pcExport->GetExtension() == wxT("colcpn")
					|| m_pcExport->GetExtension() == wxT("colhpn")  || m_pcExport->GetExtension() == wxT("colfspn")
					|| m_pcExport->GetExtension() == wxT("colfcpn") || m_pcExport->GetExtension() == wxT("colfhpn")
					|| m_pcExport->GetExtension() == wxT("colpn"))  || m_pcExport->GetExtension() == wxT("colxpn")
				)
			{
				continue;
				/*in coloured quantative pns, marking value sets have to be exported to their quantative coloured nets as such
				there should be no choice, with exptions including ANDL/CANDL/Uncolored nets as there is no multi VSETS for marking
				*/
			}


			for (SP_ListNodeclass::const_iterator itNC = m_pcDoc->GetGraph()->GetNodeclasses()->begin(); itNC != m_pcDoc->GetGraph()->GetNodeclasses()->end(); itNC++)
			{
				if (!(*itNC)->GetElements()->empty() && (*itNC)->GetShowInElementTree())
				{
					SP_DS_Node* l_pcProtoNode = (*itNC)->GetPrototype();
					SP_DS_Node* l_pcNode = (*itNC)->GetElements()->front();
					SP_ListAttribute::const_iterator itAttr = l_pcNode->GetAttributes()->begin();
					SP_ListAttribute::const_iterator itProtoAttr = l_pcProtoNode->GetAttributes()->begin();

					for (; itAttr != l_pcNode->GetAttributes()->end(); itAttr++, itProtoAttr++)
					{
						SP_DS_Attribute* l_pcProtoAttr = *itProtoAttr;
						SP_DS_Attribute* l_pcAttr = *itAttr;
						if (l_pcAttr->GetAttributeType() == SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_COLLIST)
						{
							if (l_pcAttr->GetName() == wxT("MarkingList") && (dynamic_cast<SP_DS_ColListAttribute*>(l_pcAttr)->GetColCount()) <= 3)
							{
								continue;
							}

							std::vector<wxString> l_vSetNames;

							SP_DS_ColListAttribute* l_pcColAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcAttr);

							if (l_pcColAttr)
							{
								for (unsigned i = 0; i < l_pcColAttr->GetColCount(); i++)
								{
									wxString l_sSetName = l_pcColAttr->GetColLabel(i);
									if (!l_sSetName.Contains(wxT(":"))) continue;
									if (l_sSetName == wxT("dot")) l_sSetName = wxT("Main");//on 19.11.20

									if (l_sSetName.Contains(wxT(":")))
									l_sSetName = l_sSetName.BeforeFirst(wxChar(':'));

									if (SP_Find(l_vSetNames, l_sSetName) == l_vSetNames.end() && l_sSetName != wxT("Product Color"))
									{
										l_vSetNames.push_back(l_sSetName.BeforeFirst(wxChar(':')));

									}

								}
								if (l_pcAttr->GetName() != wxT("MarkingList") && !l_sNetClassName.Contains(_T("Colored")))
								{
									for (unsigned i = 0; i < l_pcColAttr->GetRowCount(); i++)
									{
										wxString l_sSetName = l_pcColAttr->GetCell(i, 0);

										{
											l_sSetName = l_sSetName.BeforeFirst(wxChar(':'));
										}

										if (l_sSetName.IsEmpty()) break;

										if (SP_Find(l_vSetNames, l_sSetName) == l_vSetNames.end() && l_sSetName != wxT("true"))
										{
											if (l_sSetName.Contains(wxChar(':')))
											{
												l_sSetName.BeforeFirst(wxChar(':'));
											}

											l_vSetNames.push_back(l_sSetName.BeforeFirst(wxChar(':')));

										}

									}

								}

								if (l_pcAttr->GetName() != wxT("MarkingList") && l_sNetClassName.Contains(_T("Colored")))
								{
									for (unsigned i = 0; i < l_pcColAttr->GetColCount(); i++)
									{
										wxString l_sSetName = l_pcColAttr->GetColLabel(i);

										if (!l_sSetName.Contains(wxT(":"))) continue;

										l_sSetName = l_sSetName.BeforeFirst(wxChar(':'));

										if (SP_Find(l_vSetNames, l_sSetName) == l_vSetNames.end() && !(l_sSetName == wxT("true") || l_sSetName == wxT("Predicate")))
										{
											l_vSetNames.push_back(l_sSetName.BeforeFirst(wxChar(':')));

										}

									}
								}
								SP_DS_ColListAttribute* l_pcProtoColAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcProtoAttr);

								wxString l_sName;
								if (l_pcProtoColAttr)
								{

									l_sName << l_pcProtoColAttr->GetDisplayName() << wxT(":");
									if (l_sName == wxT("GuardList:")) 	continue;//there is only one guard always , so no choices to be occurd

								}

								auto itFindGroup = l_mGroups2Vsets.find(l_sName);
								if (itFindGroup != l_mGroups2Vsets.end())
								{
									if (itFindGroup->second.size() <= 1)
									{

										//if a group has only one value set, then there is no multi-choices to be chosen from

										continue;
									}
								}

								if (SP_Find(l_vGroupNames, l_sName) == l_vGroupNames.end())
								{
									l_vGroupNames.push_back(l_sName);
								}
								else
								{
									continue;
								}

								if (l_vSetNames.size() <= 1) continue;

								l_pcSetsSizer->Add(new wxStaticText(l_pcNotebookPage, wxID_ANY, l_sName), wxGBPosition{ row,0 });

								auto l_pcComboBox = new wxChoice(l_pcNotebookPage, wxID_ANY);

								for (auto itSet = l_vSetNames.begin(); itSet != l_vSetNames.end(); ++itSet)
								{
									l_pcComboBox->Append(*itSet);
								}


								l_pcComboBox->SetSelection(l_pcColAttr->GetActiveList());

								m_mColListComboBoxes[l_pcColAttr] = l_pcComboBox;

								l_pcSetsSizer->Add(l_pcComboBox, wxGBPosition{ row,1 }, wxDefaultSpan, wxEXPAND);
								++row;


							}

						}
					}
				}
			}
		}
	}

	/*
		 * comboboxes for constants
		 */

		SP_DS_Graph* l_pcGraph = m_pcDoc->GetGraph();

		//george new consatnts for col pn
		SP_DS_Metadataclass* mc1 = l_pcGraph->GetMetadataclass(SP_DS_CPN_CONSTANT_HARMONIZING);
		if ( !mc1->GetElements()->empty() && l_pcGraph->GetName().Contains(wxT("Colored"))  && l_sExporttoANDL==m_pcExport->GetName())
		{
			////////////////////////////////////////////////////////////////////////////////////////////////////
			// load groups
			////////////////////////////////////////////////////////////////////////
			SP_ListMetadata::const_iterator it;
			SP_DS_Metadata* l_pcMetadata;

			wxArrayString m_choices;
			for (it = mc1->GetElements()->begin(); it != mc1->GetElements()->end(); ++it)
			{
				l_pcMetadata = *it;
				wxString l_sGroup = dynamic_cast<SP_DS_TextAttribute*>(l_pcMetadata->GetAttribute(wxT("Group")))->GetValue();
				SP_DS_ColListAttribute* l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*> (l_pcMetadata->GetAttribute(wxT("ValueList")));
				if ((SP_Find(m_choices, l_sGroup) == m_choices.end()) && l_sGroup==wxT("coloring")&& l_pcAttr->GetRowCount()>1)
				{
					m_choices.Add(l_sGroup);
					l_pcSetsSizer->Add(new wxStaticText(l_pcNotebookPage, wxID_ANY, m_choices.Last() + wxT(':')), wxGBPosition{ row,0 });
					auto l_pcComboBox = new wxChoice(l_pcNotebookPage, wxID_ANY);

					for (unsigned int i = 0; i < l_pcAttr->GetRowCount(); i++)
					{
						wxString l_sSetName = l_pcAttr->GetCell(i, 0);
						l_pcComboBox->Append(l_sSetName);
					}

					l_pcComboBox->SetSelection(l_pcAttr->GetActiveList());
					m_mColListComboBoxes[l_pcAttr] = l_pcComboBox;
					l_pcSetsSizer->Add(l_pcComboBox, wxGBPosition{ row,1 }, wxDefaultSpan, wxEXPAND);
					++row;
				}
			}
		}

/////////////////////////////////////////////////////////
	l_pcSetsSizer->AddGrowableCol(1);
	l_pcNotebookPage->AddControl(l_pcSetsSizer, 1, wxALL | wxEXPAND, 5);

    /* Buttons in the lower right hand corner */
	m_pcSizer->Add(this->CreateButtonSizer(wxOK|wxCANCEL), 0, wxEXPAND | wxALL, 5);

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

		SP_StopWatch sw(wxString::Format(wxT("Doing %s: %s\n"),
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
	for(auto l_it = m_mColListComboBoxes.begin(); l_it != m_mColListComboBoxes.end(); ++l_it)
	{
		auto l_pcCollist = (*l_it).first;
		auto l_pcComboBox = (*l_it).second;
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



std::map<wxString, std::vector<wxString>> SP_DLG_ExportProperties::GetGroup2ValueSets()
{

	std::map<wxString, std::vector<wxString>> l_mGroup2Sets;
	wxString l_sNetClassName = m_pcDoc->GetGraph()->GetNetclass()->GetName();
	vector<wxString> l_vGroupNames;

	//search for collist attributes
	for (SP_ListNodeclass::const_iterator itNC = m_pcDoc->GetGraph()->GetNodeclasses()->begin(); itNC != m_pcDoc->GetGraph()->GetNodeclasses()->end(); itNC++)
	{
		if (!(*itNC)->GetElements()->empty() && (*itNC)->GetShowInElementTree())
		{
			SP_DS_Node* l_pcProtoNode = (*itNC)->GetPrototype();
			SP_DS_Node* l_pcNode = (*itNC)->GetElements()->front();
			SP_ListAttribute::const_iterator itAttr = l_pcNode->GetAttributes()->begin();
			SP_ListAttribute::const_iterator itProtoAttr = l_pcProtoNode->GetAttributes()->begin();

			for (; itAttr != l_pcNode->GetAttributes()->end(); itAttr++, itProtoAttr++)
			{
				SP_DS_Attribute* l_pcProtoAttr = *itProtoAttr;
				SP_DS_Attribute* l_pcAttr = *itAttr;
				if (l_pcAttr->GetAttributeType() == SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_COLLIST)
				{
					if (l_pcAttr->GetName() == wxT("MarkingList") && (dynamic_cast<SP_DS_ColListAttribute*>(l_pcAttr)->GetColCount()) <= 3)
					{
						continue;
					}

					std::vector<wxString> l_vSetNames;

					SP_DS_ColListAttribute* l_pcColAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcAttr);

					if (l_pcColAttr)
					{
						if(l_pcAttr->GetName() != wxT("MarkingList"))
						{
					    	 for (unsigned i = 0; i < l_pcColAttr->GetColCount(); i++)
						     {
							   wxString l_sSetName = l_pcColAttr->GetColLabel(i);

							  if (!l_sSetName.Contains(wxT(":"))) continue;

							  if (l_sSetName.Contains(wxChar(':')))
							  {
								  l_sSetName.BeforeFirst(wxChar(':'));
							  }

							  if (l_sSetName == wxT("dot")) l_sSetName = wxT("Main");//on 19.11.20

							  if (SP_Find(l_vSetNames, l_sSetName) == l_vSetNames.end() && l_sSetName != wxT("Product Color"))
							  {
							 	l_vSetNames.push_back(l_sSetName);

							  }
						    }

						}

						if (l_pcAttr->GetName() != wxT("MarkingList")&& !l_sNetClassName.Contains(_T("Colored")))
						{
							for (unsigned i = 0; i < l_pcColAttr->GetRowCount(); i++)
							{
								wxString l_sSetName = l_pcColAttr->GetCell(i,0);

								if (l_sSetName.Contains(wxChar(':')))
								{
									l_sSetName.BeforeFirst(wxChar(':'));
								}

								if (SP_Find(l_vSetNames, l_sSetName) == l_vSetNames.end() && l_sSetName !=wxT("true") )
								{
									if (l_sSetName.Contains(wxChar(':')))
									{
										l_sSetName.BeforeFirst(wxChar(':'));
									}
									l_vSetNames.push_back(l_sSetName);

								}

							}

						}

						if (l_pcAttr->GetName() != wxT("MarkingList") && l_sNetClassName.Contains(_T("Colored")))
						{
							for (unsigned i = 0; i < l_pcColAttr->GetColCount(); i++)
							{
								wxString l_sSetName = l_pcColAttr->GetColLabel(i);

								if (!l_sSetName.Contains(wxT(":"))) continue;

								l_sSetName = l_sSetName.BeforeFirst(wxChar(':'));

								if (SP_Find(l_vSetNames, l_sSetName) == l_vSetNames.end() && !( l_sSetName == wxT("true")|| l_sSetName == wxT("Predicate")))
								{
									l_vSetNames.push_back(l_sSetName);

								}

							}
						}

						if (l_pcAttr->GetName() == wxT("MarkingList") && l_sNetClassName.Contains(_T("Colored")))
						{
						   for (unsigned i = 0; i < l_pcColAttr->GetColCount(); i++)
								{
									wxString l_sSetName = l_pcColAttr->GetColLabel(i);

									if (!l_sSetName.Contains(wxT(":"))) continue;

									if (SP_Find(l_vSetNames, l_sSetName) == l_vSetNames.end() && !(l_sSetName == wxT("true") || l_sSetName == wxT("Predicate")))
									{
											l_vSetNames.push_back(l_sSetName.BeforeFirst(wxChar(':')));

									}

								}
						}

						SP_DS_ColListAttribute* l_pcProtoColAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcProtoAttr);
						if (l_pcProtoColAttr)
						{
							wxString l_sName;
							l_sName << l_pcProtoColAttr->GetDisplayName() << wxT(":");
							l_mGroup2Sets[l_sName] = l_vSetNames;
						}
					}
					//SP_DS_ColListAttribute* l_pcProtoColAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcProtoAttr);


				}
			}
		}
	}
	return l_mGroup2Sets;

}
