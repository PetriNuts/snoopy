//////////////////////////////////////////////////////////////////////
// $Source: SP_DLG_SMART.cpp
// $Author: SLiese $
// $Version: $
// $Revision: $
// $Date: 2009/03/22 $
// Short Description:  The dialog for the SMART export
//////////////////////////////////////////////////////////////////////

#include "snoopy.h"
#include "sp_core/SP_Core.h"
#include "sp_core/SP_GPR_Canvas.h"
#include "sp_core/SP_GPR_Animation.h"
#include "sp_core/SP_GPR_Elements.h"
#include "sp_core/SP_GPR_Fonts.h"

#include "sp_gui/dialogs/SP_DLG_SMART.h"
#include "sp_gui/windows/SP_GUI_Mainframe.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gui/mdi/SP_MDI_View.h"
#include <time.h>


#include "sp_utilities.h"


IMPLEMENT_CLASS(SP_DLG_SMART, wxDialog)

enum { //control id's
	SP_ID_RADIOBOX_NETCLASSES
};

BEGIN_EVENT_TABLE(SP_DLG_SMART, wxDialog)
EVT_BUTTON(wxID_OK, SP_DLG_SMART::OnDlgOk)
EVT_BUTTON(wxID_CANCEL, SP_DLG_SMART::OnDlgCancel)
EVT_CHECKBOX(SP_ID_CHECKBOX_Part_1, SP_DLG_SMART::OnKlick_checkbox_partition1)
EVT_CHECKBOX(SP_ID_CHECKBOX_Part_2, SP_DLG_SMART::OnKlick_checkbox_partition2)
EVT_CHECKBOX(SP_ID_CHECKBOX_Part_3, SP_DLG_SMART::OnKlick_checkbox_partition3)
EVT_CHECKBOX(SP_ID_CHECKBOX_Part_4, SP_DLG_SMART::OnKlick_checkbox_partition4)
EVT_BUTTON(SP_ID_BUTTON_DELETE_PARTITION, SP_DLG_SMART::Partition_Button2)
EVT_BUTTON(SP_ID_BUTTON_NEW_PARTITION, SP_DLG_SMART::Partition_Button1)
EVT_BUTTON(SP_ID_BUTTON_ELEMENTTO_PARTITION, SP_DLG_SMART::Partition_Button3)
EVT_BUTTON(SP_ID_BUTTON_ELEMENTFROM_PARTITION, SP_DLG_SMART::Partition_Button4)
EVT_BUTTON(SP_ID_BUTTON_PARTITION_SAVE, SP_DLG_SMART::Partition_Save)
EVT_BUTTON(SP_ID_BUTTON_PARTITION_LOAD, SP_DLG_SMART::Partition_Load)
EVT_LISTBOX(SP_ID_LISTBOX_PARTITIONLIST,SP_DLG_SMART::PartitionList_Click)
EVT_BUTTON(SP_ID_BUTTON_CTL_LOAD,SP_DLG_SMART::CTL_LOAD)
EVT_BUTTON(SP_ID_BUTTON_CTL_SAVE,SP_DLG_SMART::CTL_SAVE)
EVT_BUTTON(SP_ID_BUTTON_CTL_CLEAR,SP_DLG_SMART::CTL_CLEAR)
EVT_BUTTON(SP_ID_BUTTON_CTL_CHECK,SP_DLG_SMART::CTL_CHECK)
EVT_BUTTON(SP_ID_BUTTON_STEADYSTATE_ADD,SP_DLG_SMART::SteadyState_ADD)
EVT_BUTTON(SP_ID_BUTTON_STEADYSTATE_REMOVE,SP_DLG_SMART::SteadyState_REMOVE)
EVT_BUTTON(SP_ID_BUTTON_STEADYSTATE_CLEAR,SP_DLG_SMART::SteadyState_CLEAR)
EVT_BUTTON(SP_ID_BUTTON_STEADYSTATE_CHECK,SP_DLG_SMART::SteadyState_CHECK)
EVT_BUTTON(SP_ID_BUTTON_STEADYSTATE_LOAD,SP_DLG_SMART::SteadyState_LOAD)
EVT_BUTTON(SP_ID_BUTTON_STEADYSTATE_SAVE,SP_DLG_SMART::SteadyState_SAVE)
EVT_LISTBOX(SP_ID_LISTBOX_STEADYSTATE,SP_DLG_SMART::SteadyStateList_Click)
EVT_BUTTON(SP_ID_BUTTON_TRANSIENTE_ADD,SP_DLG_SMART::Transiente_ADD)
EVT_BUTTON(SP_ID_BUTTON_TRANSIENTE_REMOVE,SP_DLG_SMART::Transiente_REMOVE)
EVT_BUTTON(SP_ID_BUTTON_TRANSIENTE_CLEAR,SP_DLG_SMART::Transiente_CLEAR)
EVT_BUTTON(SP_ID_BUTTON_TRANSIENTE_CHECK,SP_DLG_SMART::Transiente_CHECK)
EVT_BUTTON(SP_ID_BUTTON_TRANSIENTE_LOAD,SP_DLG_SMART::Transiente_LOAD)
EVT_BUTTON(SP_ID_BUTTON_TRANSIENTE_SAVE,SP_DLG_SMART::Transiente_SAVE)
EVT_LISTBOX(SP_ID_LISTBOX_TRANSIENTE,SP_DLG_SMART::TransienteList_Click)

//EVT_RADIOBOX(SP_ID_RADIOBOX_FTREE_ANALYSIS, SP_DLG_SMART::OnKlick)

//EVT_RADIOBOX(SP_ID_RADIOBOX_NETCLASSES, SP_DLG_SMART::OnUpdateNetClass)
//EVT_RADIOBOX(SP_ID_RADIOBOX_TRANSSHAPE, SP_DLG_SMART::OnUpdateTransShape)
//EVT_SPINCTRL(SP_ID_SPINCTRL_TRANSWIDTH, SP_DLG_SMART::OnUpdateTransWidth)
//EVT_SPINCTRL(SP_ID_SPINCTRL_TRANSHEIGHT, SP_DLG_SMART::OnUpdateTransHeight)
//EVT_COMMAND_RANGE(SP_ID_BUTTON_FONT, SP_ID_BUTTON_FONT + 9, 
//				  wxEVT_COMMAND_BUTTON_CLICKED , SP_DLG_SMART::OnUpdateFont)
END_EVENT_TABLE()

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


SP_DLG_SMART::SP_DLG_SMART(wxWindow* p_pcParent)
: wxDialog(p_pcParent, -1, wxT("SMART Export Configurator"), wxDefaultPosition, wxDefaultSize, 
		   wxDEFAULT_DIALOG_STYLE | wxSTAY_ON_TOP | wxRESIZE_BORDER | wxMAXIMIZE_BOX)
{
	
	
	lastSelectedPartition=-1;
	/*int l_nNCCount = SP_Core::Instance()->GetNetclassCount();
	m_sNetClasses = new wxString[l_nNCCount];
	for(int i = 0; i < l_nNCCount; i++) {
		m_sNetClasses[i] = SP_Core::Instance()->GetNetclassName(i);
	}*/
	
	//preselect currently active netclass
	/*if (SP_Core::Instance()->GetRootDocument()) {
		m_sCurrentNetClass = SP_Core::Instance()->GetRootDocument()->GetNetclassName();
	} else {
		m_sCurrentNetClass = m_sNetClasses[0];
	}*/
	
    //dialog's main sizer and sizer for top section   
    m_pcMainSizer = new wxBoxSizer(wxVERTICAL);
	m_pcTopSizer = new wxBoxSizer(wxHORIZONTAL);
	
    m_pcNotebook = new wxNotebook(this, -1, wxDefaultPosition, wxDefaultSize);
	
	//CreateNotebookPages();
	
	m_pcTopSizer->Add(m_pcNotebook, 1, wxALL | wxEXPAND, 5);
	
    /* Buttons in the lower right hand corner */
    wxBoxSizer *l_pcButtonSizer = new wxBoxSizer( wxHORIZONTAL );
	l_pcButtonSizer->Add(this->CreateButtonSizer(wxOK|wxCANCEL), 0, wxEXPAND | wxALL, 5);
	m_pcMainSizer->Add(m_pcTopSizer, 1, wxEXPAND);
    m_pcMainSizer->Add(l_pcButtonSizer, 0, wxEXPAND);
	
    SetAutoLayout(true);
	//seems to be a reasonable dimension
	SetSizeHints(600, 450);
    SetSizer(m_pcMainSizer);
    m_pcMainSizer->Fit(this);
	
    Centre(wxBOTH);
}

SP_DLG_SMART::~SP_DLG_SMART()
{
	//delete[] m_sNetClasses;
}


//////////////////////////////////////////////////////////////////////
// private help methods
//////////////////////////////////////////////////////////////////////
void
SP_DLG_SMART::SetConfiguration(SP_ExportConfiguration *EC)
{	
	ExportConfig=EC;
}
void
SP_DLG_SMART::SetExportSMART(SP_ExportSMART* exp)
{
	ESMART=exp;
}
void
SP_DLG_SMART::savesettings()
{	
	ExportConfig->SetConfigurationElement(wxT("partition_depth"),false,partition_depth->GetValue());
	ExportConfig->SetConfigurationElement(wxT("partition_min_number"),false,partition_min_number->GetValue());
	ExportConfig->SetConfigurationElement(wxT("GarbageCollection"),checkbox_GarbageCollection->GetValue(),combobox_GarbageCollection->GetStringSelection());
	ExportConfig->SetConfigurationElement(wxT("ParameterWarnings"),checkbox_ParameterWarnings->GetValue(),combobox_ParameterWarnings->GetStringSelection());
	ExportConfig->SetConfigurationElement(wxT("StateStorage"),checkbox_StateStorage->GetValue(),combobox_StateStorage->GetStringSelection());
	ExportConfig->SetConfigurationElement(wxT("MarkovStorage"),checkbox_MarkovStorage->GetValue(),combobox_MarkovStorage->GetStringSelection());
	ExportConfig->SetConfigurationElement(wxT("RealFormat"),checkbox_RealFormat->GetValue(),combobox_RealFormat->GetStringSelection());	
	ExportConfig->SetConfigurationElement(wxT("Report"),checkbox_Report->GetValue(),combobox_Report->GetStringSelection());
	ExportConfig->SetConfigurationElement(wxT("Verbose"),checkbox_Verbose->GetValue(),combobox_Verbose->GetStringSelection());

	ExportConfig->SetConfigurationElement(wxT("init_option"),checkbox_init_option->GetValue(),combobox_init_option->GetStringSelection());
	ExportConfig->SetConfigurationElement(wxT("num_states"),checkbox_num_states->GetValue(),combobox_num_states->GetStringSelection());
	ExportConfig->SetConfigurationElement(wxT("num_arcs"),checkbox_num_arcs->GetValue(),combobox_num_arcs->GetStringSelection());	


	ExportConfig->SetConfigurationElement(wxT("IgnoreWeightClasses"),checkbox_IgnoreWeightClasses->GetValue(),combobox_IgnoreWeightClasses->GetStringSelection());	
	ExportConfig->SetConfigurationElement(wxT("InfinityString"),checkbox_InfinityString->GetValue(),Textbox_InfinityString->GetValue());	
	ExportConfig->SetConfigurationElement(wxT("BFSTrace"),checkbox_BFSTrace->GetValue(),combobox_BFSTrace->GetStringSelection());	
	ExportConfig->SetConfigurationElement(wxT("WriteDotOutput"),checkbox_WriteDotOutput->GetValue(),combobox_WriteDotOutput->GetStringSelection());	
	ExportConfig->SetConfigurationElement(wxT("DotOutputFile"),checkbox_DotOutputFile->GetValue(),Textbox_DotOutputFile->GetValue());	
	ExportConfig->SetConfigurationElement(wxT("DotPrecision"),checkbox_DotPrecision->GetValue(),combobox_DotPrecision->GetStringSelection());	
	ExportConfig->SetConfigurationElement(wxT("DotTitle"),checkbox_DotTitle->GetValue(),Textbox_DotTitle->GetValue());	
	ExportConfig->SetConfigurationElement(wxT("EmbedWithElim"),checkbox_EmbedWithElim->GetValue(),combobox_EmbedWithElim->GetStringSelection());	
	ExportConfig->SetConfigurationElement(wxT("EUStrategy"),checkbox_EUStrategy->GetValue(),combobox_EUStrategy->GetStringSelection());	
	ExportConfig->SetConfigurationElement(wxT("GarbageUnit"),checkbox_GarbageUnit->GetValue(),combobox_GarbageUnit->GetStringSelection());	
	ExportConfig->SetConfigurationElement(wxT("GarbageSize"),checkbox_GarbageSize->GetValue(),Textbox_GarbageSize->GetValue());	
	ExportConfig->SetConfigurationElement(wxT("MatrixByRows"),checkbox_MatrixByRows->GetValue(),combobox_MatrixByRows->GetStringSelection());	
	ExportConfig->SetConfigurationElement(wxT("MaxConvergeIters"),checkbox_MaxConvergeIters->GetValue(),Textbox_MaxConvergeIters->GetValue());	
	ExportConfig->SetConfigurationElement(wxT("MaxNumericalIters"),checkbox_MaxNumericalIters->GetValue(),Textbox_MaxNumericalIters->GetValue());	
	ExportConfig->SetConfigurationElement(wxT("MaxPrintedItems"),checkbox_MaxPrintedItems->GetValue(),Textbox_MaxPrintedItems->GetValue());	
	//ctl
	ExportConfig->SetConfigurationElement(wxT("CTL_data"),true,ctl_textbox->GetValue());
	//SteadyState
	wxString ss_data=wxT("");
	for(unsigned int i=0;i<ListBox_steadystate->GetCount();i++)
		ss_data=ss_data+wxT("\n")+ListBox_steadystate->GetString(i);
	ExportConfig->SetConfigurationElement(wxT("SteadyState_data"),true,ss_data);
	//Transiente Analyse
	wxString ta_data=wxT("");
	for(unsigned int i1=0;i1<ListBox_transiente->GetCount();i1++)
		ta_data=ta_data+wxT("\n")+ListBox_transiente->GetString(i1);
	ExportConfig->SetConfigurationElement(wxT("Transiente_data"),true,ta_data);
}
void
SP_DLG_SMART::CreateNotebookPages() {
	wxSize s;
	
	wxBoxSizer *elemsizer;
	wxBoxSizer *helemsizer;

	m_pcNotebook->DeleteAllPages();
	
	//page net - info
	m_pcPageSizer = new wxBoxSizer(wxVERTICAL);
	m_pcNotebookPage = new wxScrolledWindow(m_pcNotebook);
	m_pcNotebook->InsertPage(0, m_pcNotebookPage, wxT("net - info"));
	
	m_pcPageSizer->Add(0, 10, 0);
		//netname
		elemsizer = new wxBoxSizer(wxHORIZONTAL);
		elemsizer->Add(new wxStaticText(m_pcNotebookPage, -1, wxT("net - name: ") + ExportConfig->GetConfigurationElement(wxT("netname"))->value), 0, 
										wxRIGHT | wxLEFT , 0);
		m_pcPageSizer->Add(elemsizer, 0, wxTOP | wxBOTTOM, 5);
		//nettype
		elemsizer = new wxBoxSizer(wxHORIZONTAL);
		elemsizer->Add(new wxStaticText(m_pcNotebookPage, -1, wxT("net - type: ") + ExportConfig->GetConfigurationElement(wxT("nettype"))->value), 0, 
										wxRIGHT | wxLEFT , 0);
		m_pcPageSizer->Add(elemsizer, 0, wxTOP | wxBOTTOM, 5);
		//Count of places
		elemsizer = new wxBoxSizer(wxHORIZONTAL);
		elemsizer->Add(new wxStaticText(m_pcNotebookPage, -1, wxT("number of places: ") + ExportConfig->GetConfigurationElement(wxT("place_count"))->value), 0, 
										wxRIGHT | wxLEFT , 0);
		m_pcPageSizer->Add(elemsizer, 0, wxTOP | wxBOTTOM, 5);
		//count of transitions
		elemsizer = new wxBoxSizer(wxHORIZONTAL);
		elemsizer->Add(new wxStaticText(m_pcNotebookPage, -1, wxT("number of transitions: ") + ExportConfig->GetConfigurationElement(wxT("transitions_count"))->value), 0, 
										wxRIGHT | wxLEFT , 0);
		m_pcPageSizer->Add(elemsizer, 0, wxTOP | wxBOTTOM, 5);
		//count of Arcs
		elemsizer = new wxBoxSizer(wxHORIZONTAL);
		elemsizer->Add(new wxStaticText(m_pcNotebookPage, -1, wxT("number of arcs: ") + ExportConfig->GetConfigurationElement(wxT("arcs_count"))->value), 0, 
										wxRIGHT | wxLEFT , 0);
		m_pcPageSizer->Add(elemsizer, 0, wxTOP | wxBOTTOM, 5);
		
		m_pcNotebookPage->SetAutoLayout(true);
		m_pcNotebookPage->SetSizer(m_pcPageSizer);
#if wxABI_VERSION > 30000
		m_pcPageSizer->FitInside(m_pcNotebookPage);
#else
		m_pcPageSizer->SetVirtualSizeHints(m_pcNotebookPage);
#endif
		m_pcPageSizer->Layout();
	
	s = m_pcNotebookPage->GetBestSize();
	m_pcNotebookPage->SetScrollbars(20, 20, s.GetWidth()/20+1, s.GetHeight()/20+1);
	
	//page Partition
	m_pcPageSizer = new wxBoxSizer(wxVERTICAL);
	m_pcNotebookPage = new wxScrolledWindow(m_pcNotebook);
	//m_pcNotebook->InsertPage(1, m_pcNotebookPage, "Partition");
	m_pcNotebook->AddPage( m_pcNotebookPage, wxT("partition"));
		//none
		elemsizer = new wxBoxSizer(wxHORIZONTAL);
		CheckBox_Part_1 = new wxCheckBox(m_pcNotebookPage, SP_ID_CHECKBOX_Part_1, wxT("none"));
		CheckBox_Part_1->SetValue(true);
		elemsizer->Add(CheckBox_Part_1, 0, wxRIGHT | wxLEFT , 5);
		m_pcPageSizer->Add(elemsizer, 0, wxTOP | wxBOTTOM , 5);
		ExportConfig->SetConfigurationElement(wxT("partition"),true,wxT("none"));
		//everyplace as partition
		elemsizer = new wxBoxSizer(wxHORIZONTAL);
		CheckBox_Part_2 = new wxCheckBox(m_pcNotebookPage, SP_ID_CHECKBOX_Part_2, wxT("every place as Partition"));
		CheckBox_Part_2->SetValue(false);
		elemsizer->Add(CheckBox_Part_2, 0, wxRIGHT | wxLEFT , 5);
		m_pcPageSizer->Add(elemsizer, 0, wxTOP | wxBOTTOM , 5);
		//auto partition
		elemsizer = new wxBoxSizer(wxHORIZONTAL);
		CheckBox_Part_3 = new wxCheckBox(m_pcNotebookPage, SP_ID_CHECKBOX_Part_3, wxT("create auto Partition"));
		CheckBox_Part_3->SetValue(false);
		elemsizer->Add(CheckBox_Part_3, 0, wxRIGHT | wxLEFT , 5);
		m_pcPageSizer->Add(elemsizer, 0, wxTOP | wxBOTTOM , 5);
		elemsizer = new wxBoxSizer(wxHORIZONTAL);
		elemsizer->Add(new wxStaticText(m_pcNotebookPage,-1,wxT("   Coarse depth:")), 0, 5);
		partition_depth = new wxTextCtrl(m_pcNotebookPage,-1 ,wxT(""));
		partition_depth->SetValidator( wxTextValidator(wxFILTER_NUMERIC ));
		partition_depth->SetValue(ExportConfig->GetConfigurationElement(wxT("partition_depth_default"))->value );
		elemsizer->Add(partition_depth, 0, wxTOP | wxBOTTOM , 5);
		elemsizer->Add(new wxStaticText(m_pcNotebookPage,-1,wxT("  min number of places:")), 0, 5);
		partition_min_number= new wxTextCtrl(m_pcNotebookPage,-1 ,wxT(""));
		partition_min_number->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
		partition_min_number->SetValue(ExportConfig->GetConfigurationElement(wxT("partition_min_number_default"))->value);
		elemsizer->Add(partition_min_number,0,wxTOP|wxBOTTOM,5);
		m_pcPageSizer->Add(elemsizer, 0, wxTOP | wxBOTTOM , 0);
		//userdefined partition
		elemsizer = new wxBoxSizer(wxHORIZONTAL);
		CheckBox_Part_4 = new wxCheckBox(m_pcNotebookPage, SP_ID_CHECKBOX_Part_4, wxT("userdefined Partition"));
		CheckBox_Part_4->SetValue(false);
		elemsizer->Add(CheckBox_Part_4, 0, wxRIGHT | wxLEFT , 5);
		m_pcPageSizer->Add(elemsizer, 0, wxTOP | wxBOTTOM , 5);
		//listboxes
		//wxSize* s= new wxSize();
		s.SetHeight(170);
		s.SetWidth(125);
		elemsizer = new wxBoxSizer(wxHORIZONTAL);
		//first box
			ListBoxallpart1 = new wxListBox(m_pcNotebookPage, SP_ID_LISTBOX_PARTITIONLIST,wxDefaultPosition,s);
			
			//ListBoxallpart1->SetMinSize
			elemsizer->Add(ListBoxallpart1, 0, wxRIGHT | wxLEFT , 0);
		//Buttons	
			helemsizer = new wxBoxSizer(wxVERTICAL);
			helemsizer->Add( new wxButton( m_pcNotebookPage, SP_ID_BUTTON_DELETE_PARTITION, wxT( "delete Partition" ) ), 0, wxRIGHT | wxLEFT , 0);
			helemsizer->Add( new wxButton( m_pcNotebookPage, SP_ID_BUTTON_NEW_PARTITION, wxT( "new Partition" ) ), 0, wxRIGHT | wxLEFT , 0);
			elemsizer->Add(helemsizer, 0, wxRIGHT | wxLEFT , 5);
		//second box
			ListBoxallpart2 = new wxListBox(m_pcNotebookPage, -1,wxDefaultPosition,s);
			elemsizer->Add(ListBoxallpart2, 0, wxRIGHT | wxLEFT , 0);
		//buttons
			helemsizer = new wxBoxSizer(wxVERTICAL);
			helemsizer->Add( new wxButton( m_pcNotebookPage, SP_ID_BUTTON_ELEMENTTO_PARTITION, wxT( "<<" ) ), 0, wxRIGHT | wxLEFT , 0);
			helemsizer->Add( new wxButton( m_pcNotebookPage, SP_ID_BUTTON_ELEMENTFROM_PARTITION, wxT( ">>" ) ), 0, wxRIGHT | wxLEFT , 0);
			elemsizer->Add(helemsizer, 0, wxRIGHT | wxLEFT , 5);
		//third box
			ListBoxallpart3 = new wxListBox(m_pcNotebookPage, -1,wxDefaultPosition,s);
			elemsizer->Add(ListBoxallpart3, 0, wxRIGHT | wxLEFT , 0);
			UpdateListboxes();
		//load /save buttons partition
		m_pcPageSizer->Add(elemsizer, 0, wxTOP | wxBOTTOM , 5);
		elemsizer = new wxBoxSizer(wxHORIZONTAL);
		elemsizer->Add( new wxButton( m_pcNotebookPage, SP_ID_BUTTON_PARTITION_LOAD, wxT( "Load Partition" ) ), 0, wxRIGHT | wxLEFT , 5);
		elemsizer->Add( new wxButton( m_pcNotebookPage, SP_ID_BUTTON_PARTITION_SAVE, wxT( "Save Partition" ) ), 0, wxRIGHT | wxLEFT , 5);
		m_pcPageSizer->Add(elemsizer, 0, wxTOP | wxBOTTOM , 5);

	m_pcNotebookPage->SetAutoLayout(true);
	m_pcNotebookPage->SetSizer(m_pcPageSizer);
#if wxABI_VERSION > 30000
		m_pcPageSizer->FitInside(m_pcNotebookPage);
#else
		m_pcPageSizer->SetVirtualSizeHints(m_pcNotebookPage);
#endif
	m_pcPageSizer->Layout();
	//s = m_pcNotebookPage->GetBestSize();
	//m_pcNotebookPage->SetScrollbars(20, 20, s.GetWidth()/20+1, s.GetHeight()/20+1);
	
	//page optimisation
	m_pcPageSizer = new wxBoxSizer(wxVERTICAL);
	m_pcNotebookPage = new wxScrolledWindow(m_pcNotebook);
	//m_pcNotebook->InsertPage(2, m_pcNotebookPage, "Optimasation");
	m_pcNotebook->AddPage( m_pcNotebookPage, wxT("optimisation - options"));
		//grid
		Gridpanel = new wxGridSizer(23,2,5,5);//  (m_pcNotebookPage, -1);
		
		//garbagecollection
		elemsizer = new wxBoxSizer(wxHORIZONTAL);
		checkbox_GarbageCollection = new wxCheckBox(m_pcNotebookPage, -1, wxT("#GarbageCollection"));
		checkbox_GarbageCollection->SetValue(false);
		Gridpanel->Add(checkbox_GarbageCollection,0, wxEXPAND);
		combobox_GarbageCollection= new wxChoice( m_pcNotebookPage, -1 );
		combobox_GarbageCollection->Insert(wxT("LAZY"), 0);
		combobox_GarbageCollection->Insert(wxT("STRICT"), 1);
		combobox_GarbageCollection->Insert(wxT("OPTIMISTIC"), 2);
		combobox_GarbageCollection->SetSelection(0);
		ExportConfig->SetConfigurationElement(wxT("GarbageCollection"),checkbox_GarbageCollection->GetValue(),combobox_GarbageCollection->GetStringSelection());
		elemsizer->Add(combobox_GarbageCollection, 0, wxRIGHT | wxLEFT , 10);
		Gridpanel->Add(elemsizer,0, wxEXPAND);
			
		//IgnoreWeightClasses
		elemsizer = new wxBoxSizer(wxHORIZONTAL);
		checkbox_IgnoreWeightClasses= new wxCheckBox(m_pcNotebookPage, -1, wxT("#IgnoreWeightClasses"));
		checkbox_IgnoreWeightClasses->SetValue(false);
		Gridpanel->Add(checkbox_IgnoreWeightClasses,0, wxEXPAND);
		combobox_IgnoreWeightClasses= new wxChoice( m_pcNotebookPage, -1 );
		combobox_IgnoreWeightClasses->Insert(wxT("true"), 0);
		combobox_IgnoreWeightClasses->Insert(wxT("false"), 1);
		combobox_IgnoreWeightClasses->SetSelection(1);
		elemsizer->Add(combobox_IgnoreWeightClasses, 0, wxRIGHT | wxLEFT , 10);
		Gridpanel->Add(elemsizer,0, wxEXPAND);
		
		//statestorage
		elemsizer = new wxBoxSizer(wxHORIZONTAL);
		checkbox_StateStorage = new wxCheckBox(m_pcNotebookPage, -1, wxT("#StateStorage"));
		checkbox_StateStorage->SetValue(false);
		Gridpanel->Add(checkbox_StateStorage,0, wxEXPAND);
		combobox_StateStorage= new wxChoice( m_pcNotebookPage, -1 );
		combobox_StateStorage->Insert(wxT("HASHING"), 0);
		combobox_StateStorage->Insert(wxT("AVL"), 1);
		combobox_StateStorage->Insert(wxT("SPLAY"), 2);
		combobox_StateStorage->Insert(wxT("MULTI_LEVEL_AVL"), 3);
		combobox_StateStorage->Insert(wxT("MULTI_LEVEL_SPLAY"), 4);
		combobox_StateStorage->Insert(wxT("MULTI_LEVEL_ARRAY"), 5);
		combobox_StateStorage->Insert(wxT("MULTI_LEVEL_ARRAY_PREGEN"), 6);
		combobox_StateStorage->Insert(wxT("MULTI_LEVEL_BITMAP_PREGEN"), 7);
		combobox_StateStorage->Insert(wxT("MDD_SATURATION"), 8);
		combobox_StateStorage->Insert(wxT("MDD_SATURATION_PREGEN"), 9);
		combobox_StateStorage->SetSelection(0);
		ExportConfig->SetConfigurationElement(wxT("StateStorage"),checkbox_StateStorage->GetValue(),combobox_StateStorage->GetStringSelection());
		elemsizer->Add(combobox_StateStorage, 0, wxRIGHT | wxLEFT , 10);
		Gridpanel->Add(elemsizer,0, wxEXPAND);
		//MarkovStorage
		elemsizer = new wxBoxSizer(wxHORIZONTAL);
		checkbox_MarkovStorage = new wxCheckBox(m_pcNotebookPage, -1, wxT("#MarkovStorage"));
		checkbox_MarkovStorage->SetValue(false);
		Gridpanel->Add(checkbox_MarkovStorage,0, wxEXPAND);
		combobox_MarkovStorage= new wxChoice( m_pcNotebookPage, -1 );
		combobox_MarkovStorage->Insert(wxT("SPARSE"), 0);
		combobox_MarkovStorage->Insert(wxT("KRONECKER"), 1);
		combobox_MarkovStorage->Insert(wxT("MATRIX DIAGRAM KRONECKER"), 2);
		combobox_MarkovStorage->Insert(wxT("MATRIX DIAGRAM GENERAL"), 3);
		combobox_MarkovStorage->Insert(wxT("ACTUAL MTMDD"), 4);
		combobox_MarkovStorage->Insert(wxT("POT ACT MTMDD"), 5);
		combobox_MarkovStorage->Insert(wxT("POT POT MTMDD"), 6);
		combobox_MarkovStorage->SetSelection(0);
		ExportConfig->SetConfigurationElement(wxT("MarkovStorage"),checkbox_MarkovStorage->GetValue(),combobox_MarkovStorage->GetStringSelection());
		elemsizer->Add(combobox_MarkovStorage, 0, wxRIGHT | wxLEFT , 10);
		Gridpanel->Add(elemsizer,0, wxEXPAND);
		//RealFormat
		elemsizer = new wxBoxSizer(wxHORIZONTAL);
		checkbox_RealFormat = new wxCheckBox(m_pcNotebookPage, -1, wxT("#RealFormat"));
		checkbox_RealFormat->SetValue(false);
		Gridpanel->Add(checkbox_RealFormat,0, wxEXPAND);
		combobox_RealFormat
				= new wxChoice( m_pcNotebookPage, -1 );
		combobox_RealFormat->Insert(wxT("FIXED"), 0);
		combobox_RealFormat->Insert(wxT("SCIENTIFIC"), 1);
		combobox_RealFormat->Insert(wxT("GENERAL"), 2);
		combobox_RealFormat->SetSelection(0);
		ExportConfig->SetConfigurationElement(wxT("RealFormat"),checkbox_RealFormat->GetValue(),combobox_RealFormat->GetStringSelection());	
		elemsizer->Add(combobox_RealFormat, 0, wxRIGHT | wxLEFT , 10);
		Gridpanel->Add(elemsizer,0, wxEXPAND);
		
		// BFSTrace
		elemsizer = new wxBoxSizer(wxHORIZONTAL);
		checkbox_BFSTrace = new wxCheckBox(m_pcNotebookPage, -1, wxT("#BFSTrace"));
		checkbox_BFSTrace->SetValue(false);
		Gridpanel->Add(checkbox_BFSTrace,0, wxEXPAND);
		combobox_BFSTrace
				= new wxChoice( m_pcNotebookPage, -1 );
		combobox_BFSTrace->Insert(wxT("JOIN"), 0);
		combobox_BFSTrace->Insert(wxT("FORWARD"), 1);
		combobox_BFSTrace->Insert(wxT("BACKWARD"), 2);
		combobox_BFSTrace->SetSelection(0);
		ExportConfig->SetConfigurationElement(wxT("BFSTrace"),checkbox_BFSTrace->GetValue(),combobox_BFSTrace->GetStringSelection());
		elemsizer->Add(combobox_BFSTrace, 0, wxRIGHT | wxLEFT , 10);
		Gridpanel->Add(elemsizer,0, wxEXPAND);
		
		//EmbedWithElim
		elemsizer = new wxBoxSizer(wxHORIZONTAL);
		checkbox_EmbedWithElim = new wxCheckBox(m_pcNotebookPage, -1, wxT("#EmbedWithElim"));
		checkbox_EmbedWithElim->SetValue(false);
		Gridpanel->Add(checkbox_EmbedWithElim,0, wxEXPAND);
		combobox_EmbedWithElim
				= new wxChoice( m_pcNotebookPage, -1 );
		combobox_EmbedWithElim->Insert(wxT("0"), 0);
		combobox_EmbedWithElim->Insert(wxT("1"), 1);
		combobox_EmbedWithElim->Insert(wxT("2"), 2);
		combobox_EmbedWithElim->Insert(wxT("3"), 3);
		combobox_EmbedWithElim->SetSelection(3);
		ExportConfig->SetConfigurationElement(wxT("EmbedWithElim"),checkbox_EmbedWithElim->GetValue(),combobox_EmbedWithElim->GetStringSelection());
		elemsizer->Add(combobox_EmbedWithElim, 0, wxRIGHT | wxLEFT , 10);
		Gridpanel->Add(elemsizer,0, wxEXPAND);
		//EUStrategy
		elemsizer = new wxBoxSizer(wxHORIZONTAL);
		checkbox_EUStrategy = new wxCheckBox(m_pcNotebookPage, -1, wxT("#EUStrategy"));
		checkbox_EUStrategy->SetValue(false);
		Gridpanel->Add(checkbox_EUStrategy,0, wxEXPAND);
		combobox_EUStrategy
				= new wxChoice( m_pcNotebookPage, -1 );
		combobox_EUStrategy->Insert(wxT("BFS"), 0);
		combobox_EUStrategy->Insert(wxT("SATURATION"), 1);
		combobox_EUStrategy->SetSelection(1);
		ExportConfig->SetConfigurationElement(wxT("EUStrategy"),checkbox_EUStrategy->GetValue(),combobox_EUStrategy->GetStringSelection());
		elemsizer->Add(combobox_EUStrategy, 0, wxRIGHT | wxLEFT , 10);
		Gridpanel->Add(elemsizer,0, wxEXPAND);
		//GarbageUnit
		elemsizer = new wxBoxSizer(wxHORIZONTAL);
		checkbox_GarbageUnit = new wxCheckBox(m_pcNotebookPage, -1, wxT("#GarbageUnit"));
		checkbox_GarbageUnit->SetValue(false);
		Gridpanel->Add(checkbox_GarbageUnit,0, wxEXPAND);
		combobox_GarbageUnit
				= new wxChoice( m_pcNotebookPage, -1 );
		combobox_GarbageUnit->Insert(wxT("KBYTES"), 0);
		combobox_GarbageUnit->Insert(wxT("NODES"), 1);
		combobox_GarbageUnit->SetSelection(0);
		ExportConfig->SetConfigurationElement(wxT("GarbageUnit"),checkbox_GarbageUnit->GetValue(),combobox_GarbageUnit->GetStringSelection());
		elemsizer->Add(combobox_GarbageUnit, 0, wxRIGHT | wxLEFT , 10);
		Gridpanel->Add(elemsizer,0, wxEXPAND);
		//GarbageSize
		elemsizer = new wxBoxSizer(wxHORIZONTAL);
		checkbox_GarbageSize = new wxCheckBox(m_pcNotebookPage, -1, wxT("#GarbageSize"));
		checkbox_GarbageSize->SetValue(false);
		Gridpanel->Add(checkbox_GarbageSize,0, wxEXPAND);
		Textbox_GarbageSize = new wxTextCtrl(m_pcNotebookPage,-1 ,wxT("100"));
		Textbox_GarbageSize->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
			//Textbox_GarbageSize->SetValue(ExportConfig->GetConfigurationElement(wxT("GarbageSize"))->value );
		elemsizer->Add(Textbox_GarbageSize, 0, wxRIGHT | wxLEFT , 10);
		Gridpanel->Add(elemsizer,0, wxEXPAND);
		//IgnoreWeightClasses
		//MatrixByRows
		elemsizer = new wxBoxSizer(wxHORIZONTAL);
		checkbox_MatrixByRows = new wxCheckBox(m_pcNotebookPage, -1, wxT("#MatrixByRows"));
		checkbox_MatrixByRows->SetValue(false);
		Gridpanel->Add(checkbox_MatrixByRows,0, wxEXPAND);
		combobox_MatrixByRows
				= new wxChoice( m_pcNotebookPage, -1 );
		combobox_MatrixByRows->Insert(wxT("true"), 0);
		combobox_MatrixByRows->Insert(wxT("false"), 1);
		combobox_MatrixByRows->SetSelection(1);
		ExportConfig->SetConfigurationElement(wxT("MatrixByRows"),checkbox_MatrixByRows->GetValue(),combobox_MatrixByRows->GetStringSelection());
		elemsizer->Add(combobox_MatrixByRows, 0, wxRIGHT | wxLEFT , 10);
		Gridpanel->Add(elemsizer,0, wxEXPAND);
		//MaxConvergeIters
		elemsizer = new wxBoxSizer(wxHORIZONTAL);
		checkbox_MaxConvergeIters = new wxCheckBox(m_pcNotebookPage, -1, wxT("#MaxConvergeIters"));
		checkbox_MaxConvergeIters->SetValue(false);
		Gridpanel->Add(checkbox_MaxConvergeIters,0, wxEXPAND);
		Textbox_MaxConvergeIters = new wxTextCtrl(m_pcNotebookPage,-1 ,wxT("10000"));
		Textbox_MaxConvergeIters->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
			//Textbox_MaxConvergeIters->SetValue(ExportConfig->GetConfigurationElement(wxT("MaxConvergeIters"))->value );
		elemsizer->Add(Textbox_MaxConvergeIters, 0, wxRIGHT | wxLEFT , 10);
		Gridpanel->Add(elemsizer,0, wxEXPAND);
		//MaxNumericalIters
		elemsizer = new wxBoxSizer(wxHORIZONTAL);
		checkbox_MaxNumericalIters = new wxCheckBox(m_pcNotebookPage, -1, wxT("#MaxNumericalIters"));
		checkbox_MaxNumericalIters->SetValue(false);
		Gridpanel->Add(checkbox_MaxNumericalIters,0, wxEXPAND);
		Textbox_MaxNumericalIters = new wxTextCtrl(m_pcNotebookPage,-1 ,wxT("10000"));
		Textbox_MaxNumericalIters->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
			//Textbox_MaxNumericalIters->SetValue(ExportConfig->GetConfigurationElement(wxT("MaxNumericalIters"))->value );
		elemsizer->Add(Textbox_MaxNumericalIters, 0, wxRIGHT | wxLEFT , 10);
		Gridpanel->Add(elemsizer,0, wxEXPAND);
		//MaxPrintedItems
		elemsizer = new wxBoxSizer(wxHORIZONTAL);
		checkbox_MaxPrintedItems = new wxCheckBox(m_pcNotebookPage, -1, wxT("#MaxPrintedItems"));
		checkbox_MaxPrintedItems->SetValue(false);
		Gridpanel->Add(checkbox_MaxPrintedItems,0, wxEXPAND);
		Textbox_MaxPrintedItems = new wxTextCtrl(m_pcNotebookPage,-1 ,wxT("1000"));
		Textbox_MaxPrintedItems->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
			//Textbox_MaxPrintedItems->SetValue(ExportConfig->GetConfigurationElement(wxT("MaxPrintedItems"))->value );
		elemsizer->Add(Textbox_MaxPrintedItems, 0, wxRIGHT | wxLEFT , 10);
		Gridpanel->Add(elemsizer,0, wxEXPAND);


		m_pcPageSizer->Add(Gridpanel, 0, wxTOP | wxBOTTOM , 5);
		m_pcNotebookPage->SetAutoLayout(true);
		m_pcNotebookPage->SetSizer(m_pcPageSizer);
#if wxABI_VERSION > 30000
		m_pcPageSizer->FitInside(m_pcNotebookPage);
#else
		m_pcPageSizer->SetVirtualSizeHints(m_pcNotebookPage);
#endif
		m_pcPageSizer->Layout();
		s = m_pcNotebookPage->GetBestSize();
		m_pcNotebookPage->SetScrollbars(20, 20, s.GetWidth()/20+1, s.GetHeight()/20+1);

		//page output
		m_pcPageSizer = new wxBoxSizer(wxVERTICAL);
		m_pcNotebookPage = new wxScrolledWindow(m_pcNotebook);
		m_pcNotebook->AddPage(m_pcNotebookPage, wxT("output of SMART file"));
		Gridpanel = new wxGridSizer(23,2,5,5);//  (m_pcNotebookPage, -1);
		// initial values multi/add n
		elemsizer = new wxBoxSizer(wxHORIZONTAL);
		checkbox_init_option = new wxCheckBox(m_pcNotebookPage, -1, wxT("Initial Token (multiply/set) with n"));
		checkbox_init_option->SetValue(false);
		Gridpanel->Add(checkbox_init_option,0, wxEXPAND);

		combobox_init_option
				= new wxChoice( m_pcNotebookPage, -1 );
		combobox_init_option->Insert(wxT("multiply"), 0);
		combobox_init_option->Insert(wxT("set"), 1);
		combobox_init_option->SetSelection(0);
		ExportConfig->SetConfigurationElement(wxT("init_option"),checkbox_init_option->GetValue(),combobox_init_option->GetStringSelection());
		elemsizer->Add(combobox_init_option, 0, wxRIGHT | wxLEFT , 10);
		Gridpanel->Add(elemsizer,0, wxEXPAND);
		//numstates
		elemsizer = new wxBoxSizer(wxHORIZONTAL);
		checkbox_num_states = new wxCheckBox(m_pcNotebookPage, -1, wxT("num_states(TRUE/FALSE)"));
		checkbox_num_states->SetValue(false);
		Gridpanel->Add(checkbox_num_states, 0, wxEXPAND);
		combobox_num_states
				= new wxChoice( m_pcNotebookPage, -1 );
		combobox_num_states->Insert(wxT("TRUE"), 0);
		combobox_num_states->Insert(wxT("FALSE"), 1);
		combobox_num_states->SetSelection(1);
		ExportConfig->SetConfigurationElement(wxT("num_states"),checkbox_num_states->GetValue(),combobox_num_states->GetStringSelection());
		elemsizer->Add(combobox_num_states, 0, wxRIGHT | wxLEFT , 10);
		Gridpanel->Add(elemsizer, 0, wxEXPAND);
		//num_arcs
		elemsizer = new wxBoxSizer(wxHORIZONTAL);
		checkbox_num_arcs = new wxCheckBox(m_pcNotebookPage, -1, wxT("num_arcs(TRUE/FALSE)"));
		checkbox_num_arcs->SetValue(false);
		Gridpanel->Add(checkbox_num_arcs, 0, wxEXPAND);
		combobox_num_arcs
				= new wxChoice( m_pcNotebookPage, -1 );
		combobox_num_arcs->Insert(wxT("TRUE"), 0);
		combobox_num_arcs->Insert(wxT("FALSE"), 1);
		combobox_num_arcs->SetSelection(1);
		ExportConfig->SetConfigurationElement(wxT("num_arcs"),checkbox_num_arcs->GetValue(),combobox_num_arcs->GetStringSelection());
		elemsizer->Add(combobox_num_arcs, 0, wxRIGHT | wxLEFT , 10);
		Gridpanel->Add(elemsizer, 0, wxEXPAND);
		//Report
		elemsizer = new wxBoxSizer(wxHORIZONTAL);
		checkbox_Report = new wxCheckBox(m_pcNotebookPage, -1, wxT("#Report"));
		checkbox_Report->SetValue(false);
		Gridpanel->Add(checkbox_Report,0, wxEXPAND);
		combobox_Report
				= new wxChoice( m_pcNotebookPage, -1 );
		combobox_Report->Insert(wxT("TRUE"), 0);
		combobox_Report->Insert(wxT("FALSE"), 1);
		combobox_Report->SetSelection(0);
		ExportConfig->SetConfigurationElement(wxT("Report"),checkbox_Report->GetValue(),combobox_Report->GetStringSelection());
		elemsizer->Add(combobox_Report, 0, wxRIGHT | wxLEFT , 10);
		Gridpanel->Add(elemsizer,0, wxEXPAND);
		//Verbose
		elemsizer = new wxBoxSizer(wxHORIZONTAL);
		checkbox_Verbose = new wxCheckBox(m_pcNotebookPage, -1, wxT("#Verbose"));
		checkbox_Verbose->SetValue(false);
		Gridpanel->Add(checkbox_Verbose,0, wxEXPAND);
		combobox_Verbose
				= new wxChoice( m_pcNotebookPage, -1 );
		combobox_Verbose->Insert(wxT("TRUE"), 0);
		combobox_Verbose->Insert(wxT("FALSE"), 1);
		combobox_Verbose->SetSelection(0);
		ExportConfig->SetConfigurationElement(wxT("Verbose"),checkbox_Verbose->GetValue(),combobox_Verbose->GetStringSelection());
		elemsizer->Add(combobox_Verbose, 0, wxRIGHT | wxLEFT , 10);
		Gridpanel->Add(elemsizer,0, wxEXPAND);
		//InfinityString
		elemsizer = new wxBoxSizer(wxHORIZONTAL);
		checkbox_InfinityString = new wxCheckBox(m_pcNotebookPage, -1, wxT("#InfinityString"));
		checkbox_InfinityString->SetValue(false);
		Gridpanel->Add(checkbox_InfinityString,0, wxEXPAND);
		Textbox_InfinityString = new wxTextCtrl(m_pcNotebookPage,-1 ,wxT("infinity"));
		elemsizer->Add(Textbox_InfinityString, 0, wxRIGHT | wxLEFT , 10);
		Gridpanel->Add(elemsizer,0, wxEXPAND);
		//ParameterWarnings
		elemsizer = new wxBoxSizer(wxHORIZONTAL);
		checkbox_ParameterWarnings = new wxCheckBox(m_pcNotebookPage, -1, wxT("#ParamaterWarnings"));
		checkbox_ParameterWarnings->SetValue(false);
		Gridpanel->Add(checkbox_ParameterWarnings,0, wxEXPAND);
		combobox_ParameterWarnings= new wxChoice( m_pcNotebookPage, -1 );
		combobox_ParameterWarnings->Insert(wxT("NEVER"), 0);
		combobox_ParameterWarnings->Insert(wxT("ALWAYS"), 1);
		combobox_ParameterWarnings->Insert(wxT("OVERLOADED"), 2);
		combobox_ParameterWarnings->SetSelection(0);
		ExportConfig->SetConfigurationElement(wxT("ParameterWarnings"),checkbox_ParameterWarnings->GetValue(),combobox_ParameterWarnings->GetStringSelection());
		elemsizer->Add(combobox_ParameterWarnings, 0, wxRIGHT | wxLEFT , 10);
		Gridpanel->Add(elemsizer,0, wxEXPAND);
		//WriteDotOutput
		elemsizer = new wxBoxSizer(wxHORIZONTAL);
		checkbox_WriteDotOutput = new wxCheckBox(m_pcNotebookPage, -1, wxT("#WriteDotOutput"));
		checkbox_WriteDotOutput->SetValue(false);
		Gridpanel->Add(checkbox_WriteDotOutput,0, wxEXPAND);
		combobox_WriteDotOutput
				= new wxChoice( m_pcNotebookPage, -1 );
		combobox_WriteDotOutput->Insert(wxT("NONE"), 0);
		combobox_WriteDotOutput->Insert(wxT("MTMDD"), 1);
		combobox_WriteDotOutput->Insert(wxT("MXD_FULL"), 2);
		combobox_WriteDotOutput->Insert(wxT("MXD SPARSE"), 3);
		combobox_WriteDotOutput->SetSelection(0);
		ExportConfig->SetConfigurationElement(wxT("WriteDotOutput"),checkbox_WriteDotOutput->GetValue(),combobox_WriteDotOutput->GetStringSelection());
		elemsizer->Add(combobox_WriteDotOutput, 0, wxRIGHT | wxLEFT , 10);
		Gridpanel->Add(elemsizer,0, wxEXPAND);
		//DotOutputFile
		elemsizer = new wxBoxSizer(wxHORIZONTAL);
		checkbox_DotOutputFile = new wxCheckBox(m_pcNotebookPage, -1, wxT("#DotOutputFile"));
		checkbox_DotOutputFile->SetValue(false);
		Gridpanel->Add(checkbox_DotOutputFile,0, wxEXPAND);
		Textbox_DotOutputFile = new wxTextCtrl(m_pcNotebookPage,-1 ,wxT("out.dot"));
		elemsizer->Add(Textbox_DotOutputFile, 0, wxRIGHT | wxLEFT , 10);
		Gridpanel->Add(elemsizer,0, wxEXPAND);
		//DotPrecision
		elemsizer = new wxBoxSizer(wxHORIZONTAL);
		checkbox_DotPrecision = new wxCheckBox(m_pcNotebookPage, -1, wxT("#DotPrecision"));
		checkbox_DotPrecision->SetValue(false);
		Gridpanel->Add(checkbox_DotPrecision,0, wxEXPAND);
		combobox_DotPrecision
				= new wxChoice( m_pcNotebookPage, -1 );
		combobox_DotPrecision->Insert(wxT("0"), 0);
		combobox_DotPrecision->Insert(wxT("1"), 1);
		combobox_DotPrecision->Insert(wxT("2"), 2);
		combobox_DotPrecision->Insert(wxT("3"), 3);
		combobox_DotPrecision->Insert(wxT("4"), 4);
		combobox_DotPrecision->Insert(wxT("5"), 5);
		combobox_DotPrecision->Insert(wxT("6"), 6);
		combobox_DotPrecision->Insert(wxT("7"), 7);
		combobox_DotPrecision->Insert(wxT("8"), 8);
		combobox_DotPrecision->Insert(wxT("9"), 9);
		combobox_DotPrecision->Insert(wxT("10"), 10);
		combobox_DotPrecision->SetSelection(3);
		ExportConfig->SetConfigurationElement(wxT("DotPrecision"),checkbox_DotPrecision->GetValue(),combobox_DotPrecision->GetStringSelection());
		elemsizer->Add(combobox_DotPrecision, 0, wxRIGHT | wxLEFT , 10);
		Gridpanel->Add(elemsizer,0, wxEXPAND);
		//DotTitle
		elemsizer = new wxBoxSizer(wxHORIZONTAL);
		checkbox_DotTitle = new wxCheckBox(m_pcNotebookPage, -1, wxT("#DotTitle"));
		checkbox_DotTitle->SetValue(false);
		Gridpanel->Add(checkbox_DotTitle,0, wxEXPAND);
		Textbox_DotTitle = new wxTextCtrl(m_pcNotebookPage,-1 ,wxT(""));
		elemsizer->Add(Textbox_DotTitle, 0, wxRIGHT | wxLEFT , 10);
		Gridpanel->Add(elemsizer,0, wxEXPAND);
		
		m_pcPageSizer->Add(Gridpanel, 0, wxTOP | wxBOTTOM , 5);

	m_pcNotebookPage->SetAutoLayout(true);
	m_pcNotebookPage->SetSizer(m_pcPageSizer);
#if wxABI_VERSION > 30000
		m_pcPageSizer->FitInside(m_pcNotebookPage);
#else
		m_pcPageSizer->SetVirtualSizeHints(m_pcNotebookPage);
#endif
	m_pcPageSizer->Layout();
	s = m_pcNotebookPage->GetBestSize();
	m_pcNotebookPage->SetScrollbars(20, 20, s.GetWidth()/20+1, s.GetHeight()/20+1);

	////////////////////
	//CTL
	////////////////////
	m_pcPageSizer = new wxBoxSizer(wxVERTICAL);
	m_pcNotebookPage = new wxScrolledWindow(m_pcNotebook);
	m_pcNotebook->AddPage(m_pcNotebookPage, wxT("CTL"));
	elemsizer = new wxBoxSizer(wxHORIZONTAL);
	//buttons
	elemsizer->Add(new wxButton( m_pcNotebookPage, SP_ID_BUTTON_CTL_LOAD, wxT( "Load CTL File" ) ), 0, wxRIGHT | wxLEFT , 5);
	elemsizer->Add(new wxButton( m_pcNotebookPage, SP_ID_BUTTON_CTL_SAVE, wxT( "SAVE CTL File" ) ), 0, wxRIGHT | wxLEFT , 5);
	elemsizer->Add(new wxButton( m_pcNotebookPage, SP_ID_BUTTON_CTL_CLEAR, wxT( "CLEAR TextBox" ) ), 0, wxRIGHT | wxLEFT , 5);
	elemsizer->Add(new wxButton( m_pcNotebookPage, SP_ID_BUTTON_CTL_CHECK, wxT( "Check CTL " ) ), 0, wxRIGHT | wxLEFT , 5);
	m_pcPageSizer->Add(elemsizer, 0, wxTOP | wxBOTTOM , 5);
	elemsizer = new wxBoxSizer(wxHORIZONTAL);
	//textbox
	
	ctl_textbox= new wxTextCtrl(m_pcNotebookPage,-1 ,wxT(""),wxDefaultPosition,wxSize(550,170),wxTE_MULTILINE | wxHSCROLL | wxEXPAND );
	//ctl_textbox->SetSize(500,150);
	
	elemsizer->Add(ctl_textbox, 0,  wxALL, 0);
	//elemsizer->SetSizeHints(elemsizer->Getw);

	//elemsizer->SetSizeHints(Self);
	//SetSizer(elemsizer);
	m_pcPageSizer->Add(elemsizer, 0, wxEXPAND , 5);

	m_pcNotebookPage->SetAutoLayout(true);
	m_pcNotebookPage->SetSizer(m_pcPageSizer);
#if wxABI_VERSION > 30000
		m_pcPageSizer->FitInside(m_pcNotebookPage);
#else
		m_pcPageSizer->SetVirtualSizeHints(m_pcNotebookPage);
#endif
	m_pcPageSizer->Layout();
	s = m_pcNotebookPage->GetBestSize();
	m_pcNotebookPage->SetScrollbars(20, 20, s.GetWidth()/20+1, s.GetHeight()/20+1);
	//////////////////////////////////////
	//stochastische Analyse - Steady State
	//////////////////////////////////////
	m_pcPageSizer = new wxBoxSizer(wxHORIZONTAL);
	m_pcNotebookPage = new wxScrolledWindow(m_pcNotebook);
	m_pcNotebook->AddPage(m_pcNotebookPage, wxT("stochhastische Analyse - Steady State"));
	
	elemsizer = new wxBoxSizer(wxVERTICAL);
	elemsizer->Add(new wxStaticText(m_pcNotebookPage, -1, wxT("Intervall Logische Ausdruecke:") ), 0, 
										wxRIGHT | wxLEFT , 5);
	s.SetHeight(250);
	s.SetWidth(400);
	ListBox_steadystate = new wxListBox(m_pcNotebookPage, SP_ID_LISTBOX_STEADYSTATE,wxDefaultPosition,s);
			
	ListBox_steadystate->SetSize(300,350);
	//ListBox_steadystate->Set
	//ListBox_steadystate->
	//ListBoxsteadystate->SetSizeHints(Self);
	elemsizer->Add(ListBox_steadystate, 0, wxRIGHT | wxLEFT | wxEXPAND, 5);
	m_pcPageSizer->Add(elemsizer, 0, wxTOP | wxBOTTOM , 5);
	elemsizer = new wxBoxSizer(wxVERTICAL);
	elemsizer->Add(new wxStaticText(m_pcNotebookPage, -1, wxT("Intervall Logische Ausdruck:") ), 0, 
										wxRIGHT | wxLEFT , 5);
	Textbox_steadystate= new wxTextCtrl(m_pcNotebookPage,-1 ,wxT(""));

	Textbox_steadystate->SetSize(300,25);
	elemsizer->Add(Textbox_steadystate, 0, wxRIGHT | wxLEFT , 5);
	
	elemsizer->Add(new wxButton( m_pcNotebookPage, SP_ID_BUTTON_STEADYSTATE_ADD, wxT( "Add" ) ), 0, wxRIGHT | wxLEFT , 5);
	elemsizer->Add(new wxButton( m_pcNotebookPage, SP_ID_BUTTON_STEADYSTATE_REMOVE, wxT( "Remove" ) ), 0, wxRIGHT | wxLEFT , 5);
	elemsizer->Add(new wxButton( m_pcNotebookPage, SP_ID_BUTTON_STEADYSTATE_CLEAR, wxT( "Clear" ) ), 0, wxRIGHT | wxLEFT , 5);
	elemsizer->Add(new wxButton( m_pcNotebookPage, SP_ID_BUTTON_STEADYSTATE_CHECK, wxT( "Check" ) ), 0, wxRIGHT | wxLEFT , 5);
	elemsizer->Add(new wxButton( m_pcNotebookPage, SP_ID_BUTTON_STEADYSTATE_LOAD, wxT( "Load" ) ), 0, wxRIGHT | wxLEFT , 5);
	elemsizer->Add(new wxButton( m_pcNotebookPage, SP_ID_BUTTON_STEADYSTATE_SAVE, wxT( "Save" ) ), 0, wxRIGHT | wxLEFT , 5);
	m_pcPageSizer->Add(elemsizer, 0, wxTOP | wxBOTTOM , 5);
	//elemsizer = new wxBoxSizer(wxVERTICAL);
	//m_pcPageSizer->Add(elemsizer, 0, wxTOP | wxBOTTOM , 5);
	
	m_pcNotebookPage->SetAutoLayout(true);
	m_pcNotebookPage->SetSizer(m_pcPageSizer);
#if wxABI_VERSION > 30000
		m_pcPageSizer->FitInside(m_pcNotebookPage);
#else
		m_pcPageSizer->SetVirtualSizeHints(m_pcNotebookPage);
#endif
	m_pcPageSizer->Layout();
	s = m_pcNotebookPage->GetBestSize();
	m_pcNotebookPage->SetScrollbars(20, 20, s.GetWidth()/20+1, s.GetHeight()/20+1);
	
	
	////////////////////////////////////////////
	//stochastische Analyse - transiente Analyse
	////////////////////////////////////////////
	m_pcPageSizer = new wxBoxSizer(wxHORIZONTAL);
	m_pcNotebookPage = new wxScrolledWindow(m_pcNotebook);
	m_pcNotebook->AddPage(m_pcNotebookPage, wxT("stochastische Analyse - transiente Analyse"));
	
	elemsizer = new wxBoxSizer(wxVERTICAL);
	elemsizer->Add(new wxStaticText(m_pcNotebookPage, -1, wxT("Intervall Logische Ausdruecke:") ), 0, 
										wxRIGHT | wxLEFT , 5);
	s.SetHeight(250);
	s.SetWidth(400);
	ListBox_transiente = new wxListBox(m_pcNotebookPage, SP_ID_LISTBOX_TRANSIENTE,wxDefaultPosition,s);
			
	ListBox_transiente->SetSize(300,350);	
	elemsizer->Add(ListBox_transiente, 0, wxRIGHT | wxLEFT , 5);
	m_pcPageSizer->Add(elemsizer, 0, wxTOP | wxBOTTOM , 5);
	
	elemsizer = new wxBoxSizer(wxVERTICAL);
	elemsizer->Add(new wxStaticText(m_pcNotebookPage, -1, wxT("Intervall Logische Ausdruck:") ), 0, 
										wxRIGHT | wxLEFT , 5);
	Textbox_transiente= new wxTextCtrl(m_pcNotebookPage,-1 ,wxT(""));
	
	Textbox_transiente->SetSize(300,25);
	elemsizer->Add(Textbox_transiente, 0, wxRIGHT | wxLEFT , 5);
	elemsizer->Add(new wxStaticText(m_pcNotebookPage, -1, wxT("at time:") ), 0, 
										wxRIGHT | wxLEFT , 5);
	Textbox_transiente_time= new wxTextCtrl(m_pcNotebookPage,-1 ,wxT(""));
	Textbox_transiente_time->SetValidator( wxTextValidator(wxFILTER_NUMERIC ));
	elemsizer->Add(Textbox_transiente_time, 0, wxRIGHT | wxLEFT , 5);
	
	elemsizer->Add(new wxButton( m_pcNotebookPage, SP_ID_BUTTON_TRANSIENTE_ADD, wxT( "Add" ) ), 0, wxRIGHT | wxLEFT , 5);
	elemsizer->Add(new wxButton( m_pcNotebookPage, SP_ID_BUTTON_TRANSIENTE_REMOVE, wxT( "Remove" ) ), 0, wxRIGHT | wxLEFT , 5);
	elemsizer->Add(new wxButton( m_pcNotebookPage, SP_ID_BUTTON_TRANSIENTE_CLEAR, wxT( "Clear" ) ), 0, wxRIGHT | wxLEFT , 5);
	elemsizer->Add(new wxButton( m_pcNotebookPage, SP_ID_BUTTON_TRANSIENTE_CHECK, wxT( "Check" ) ), 0, wxRIGHT | wxLEFT , 5);
	elemsizer->Add(new wxButton( m_pcNotebookPage, SP_ID_BUTTON_TRANSIENTE_LOAD, wxT( "Load" ) ), 0, wxRIGHT | wxLEFT , 5);
	elemsizer->Add(new wxButton( m_pcNotebookPage, SP_ID_BUTTON_TRANSIENTE_SAVE, wxT( "Save" ) ), 0, wxRIGHT | wxLEFT , 5);
	m_pcPageSizer->Add(elemsizer, 0, wxTOP | wxBOTTOM , 5);
	
	
	m_pcNotebookPage->SetAutoLayout(true);
	m_pcNotebookPage->SetSizer(m_pcPageSizer);
#if wxABI_VERSION > 30000
		m_pcPageSizer->FitInside(m_pcNotebookPage);
#else
		m_pcPageSizer->SetVirtualSizeHints(m_pcNotebookPage);
#endif
	m_pcPageSizer->Layout();
	s = m_pcNotebookPage->GetBestSize();
	m_pcNotebookPage->SetScrollbars(20, 20, s.GetWidth()/20+1, s.GetHeight()/20+1);

}

void
SP_DLG_SMART::UpdateCanvas() {
	
	SP_GUI_Childframe *l_pcFrame = wxGetApp().GetMainframe()->GetActiveChildframe();
	if (l_pcFrame) {
		wxWindow* window = (wxWindow*)l_pcFrame;
		SP_GUI_Canvas* canvas = dynamic_cast<SP_MDI_View*>(l_pcFrame->GetView())->GetCanvas();
		wxClientDC dc(window);
		
		l_pcFrame->PrepareDC(dc);
		SP_Core::Instance()->GetRootDocument()->GetGraph()->ShowOnCanvas(canvas);
	}
}
bool
SP_DLG_SMART::PreCheck()
{
	wxString data=ctl_textbox->GetValue();
	if(data.Len()>0)
	{
		wxString erg;
		erg=ESMART->CTL_SMART->CTL_Check(data);
		if (erg!=wxT(""))
		{
			SP_MESSAGEBOX(wxT("CTL Analyse error\n")+erg, wxT("Error"), wxOK|wxICON_ERROR );
			return false;
		}
	}
	for(unsigned int i=0;i<ListBox_steadystate->GetCount();i++)
	{
		wxString line=ListBox_steadystate->GetString(i);
		wxString erg=ESMART->IL_SMART->IL_Check(line);
		if (erg!=wxT(""))
		{
			wxString l_wxS_line=wxString::Format(wxT("%d"),i+1);
			SP_MESSAGEBOX(wxT("SteadyState Analyse line:")+l_wxS_line+wxT("\n")+ erg, wxT("Error"), wxOK|wxICON_ERROR );
			return false;
		}
	}
	for(unsigned int i=0;i<ListBox_transiente->GetCount();i++)
	{
		wxString line=ListBox_transiente->GetString(i);
		line=line.SubString(line.First(wxT(" at "))+4,line.Len());
		wxString erg=ESMART->IL_SMART->IL_Check(line);
		if (erg!=wxT(""))
		{
			wxString l_wxS_line=wxString::Format(wxT("%d"),i+1);
			SP_MESSAGEBOX(wxT("Tansiente Analyse line:")+l_wxS_line+wxT("\n")+ erg, wxT("Error"), wxOK|wxICON_ERROR );
			return false;
		}
	}

	return true;
}
//////////////////////////////////////////////////////////////////////
// Dialog events
//////////////////////////////////////////////////////////////////////

void
SP_DLG_SMART::OnDlgOk(wxCommandEvent& p_cEvent)
{
	if ( Validate() && TransferDataFromWindow() )
	{
		//check ctl, ss and transition
		if(PreCheck())
		{
			//save settings
			savesettings();
			if ( IsModal() )
				EndModal(wxID_OK);
			else
			{
				SetReturnCode(wxID_OK);
				this->Show(FALSE);
			}
		}
	}
	UpdateCanvas();
}
void
SP_DLG_SMART::OnDlgCancel(wxCommandEvent& p_cEvent)
{
	if ( Validate() && TransferDataFromWindow() )
	{
        if ( IsModal() )
            EndModal(wxID_CANCEL);
        else
        {
		    SetReturnCode(wxID_CANCEL);
		    this->Show(FALSE);
        }
	}
	UpdateCanvas();
}
//void
//SP_DLG_SMART::OnKlick(wxCommandEvent& p_cEvent)
//{
//	if ( Validate() && TransferDataFromWindow() )
//	{
//	}
//}

//void
//SP_DLG_SMART::OnUpdateNetClass(wxCommandEvent& p_cEvent) {
//	/*int sel = m_pcNotebook->GetSelection();
//	
//	if ( Validate() && TransferDataFromWindow() ) {
//	}
//	m_sCurrentNetClass = m_rbNetClasses->GetStringSelection();
//	CreateNotebookPages();
//	m_pcNotebook->SetSelection(sel);*/
//	
//}
//
//void
//SP_DLG_SMART::OnUpdateTransShape(wxCommandEvent& p_cEvent) {
//	
//	if ( Validate() && TransferDataFromWindow() ) {
//	}
//}
//
//void
//SP_DLG_SMART::OnUpdateTransWidth(wxSpinEvent& p_cEvent) {
//	
//	if ( Validate() && TransferDataFromWindow() ) {
//	}
//	wxSize size=m_pcNotebook->GetSize();
//
//}
//
//void
//SP_DLG_SMART::OnUpdateTransHeight(wxSpinEvent& p_cEvent) {
//	
//	if ( Validate() && TransferDataFromWindow() ) {
//	}
//}
//
//void
//SP_DLG_SMART::OnUpdateFont(wxCommandEvent& p_cEvent) {
//	//m_pcFontPrefs->ChooseFont(this, m_sCurrentNetClass, p_cEvent.GetId() - SP_ID_BUTTON_FONT);
//}
/////////////////////
//Partition
/////////////////////
void
SP_DLG_SMART::OnKlick_checkbox_partition1(wxCommandEvent& p_cEvent)
{
	ExportConfig->SetConfigurationElement(wxT("partition"),true,wxT("none"));
	if(CheckBox_Part_1->GetValue())
	{
		CheckBox_Part_2->SetValue(false);
		CheckBox_Part_3->SetValue(false);
		CheckBox_Part_4->SetValue(false);
	}else{
		CheckBox_Part_1->SetValue(true);
	}
}
void
SP_DLG_SMART::OnKlick_checkbox_partition2(wxCommandEvent& p_cEvent)
{
	ExportConfig->SetConfigurationElement(wxT("partition"),true,wxT("place=partition"));
	if(CheckBox_Part_2->GetValue())
	{
		CheckBox_Part_1->SetValue(false);
		CheckBox_Part_3->SetValue(false);
		CheckBox_Part_4->SetValue(false);
	}else{
		CheckBox_Part_2->SetValue(true);
	}
}
void
SP_DLG_SMART::OnKlick_checkbox_partition3(wxCommandEvent& p_cEvent)
{
	ExportConfig->SetConfigurationElement(wxT("partition"),true,wxT("autopartition"));
	if(CheckBox_Part_3->GetValue())
	{
		CheckBox_Part_2->SetValue(false);
		CheckBox_Part_1->SetValue(false);
		CheckBox_Part_4->SetValue(false);
		// check value
		//wxTextCtrl *partition_depth;
		//wxTextCtrl *partition_min_number;
		ExportConfig->SetConfigurationElement(wxT("partition_depth"),false,partition_depth->GetValue());
		ExportConfig->SetConfigurationElement(wxT("partition_min_number"),false,partition_min_number->GetValue());
	
		//autopartition
		ESMART->AutoPartition();
		//update listboxes
		UpdateListboxes();
		//set checkbox 4
		CheckBox_Part_4->SetValue(true);
		CheckBox_Part_3->SetValue(false);
	}else{
		CheckBox_Part_3->SetValue(true);
		
	}
}
void
SP_DLG_SMART::OnKlick_checkbox_partition4(wxCommandEvent& p_cEvent)
{
	ExportConfig->SetConfigurationElement(wxT("partition"),true,wxT("userdefinedpartition"));
	if(CheckBox_Part_4->GetValue())
	{
		CheckBox_Part_2->SetValue(false);
		CheckBox_Part_3->SetValue(false);
		CheckBox_Part_1->SetValue(false);
	}else{
		CheckBox_Part_4->SetValue(true);
	}
}
void
SP_DLG_SMART::ChoosePartition(bool p_value)
{
	int l_selected=combobox_StateStorage->GetSelection();
	if(!p_value)
	{
		CheckBox_Part_1->Enable(true);	
	}else
	{
		CheckBox_Part_4->SetValue(true);
		CheckBox_Part_1->Enable(false);
	}
	if((l_selected<static_cast<int>(combobox_StateStorage->GetCount()))&&(l_selected!=wxNOT_FOUND))
		combobox_StateStorage->SetSelection(l_selected);
	else
		combobox_StateStorage->SetSelection(0);
}
void
SP_DLG_SMART::UpdateListboxes()
{
//ListBoxallpart1
	int pn=wxNOT_FOUND;
	int pn1=wxNOT_FOUND;
	pn=ListBoxallpart1->GetSelection();
	pn1=ListBoxallpart3->GetSelection();
	ListBoxallpart1->Clear();
	ListBoxallpart1->InsertItems( ExportConfig->GetPartitions_aswxArrayString(), 0);
	ListBoxallpart3->Clear();
	ListBoxallpart3->InsertItems(ExportConfig->GetALLusableElementsforPartition_aswxArrayString(), 0);
	
	if  (pn!= wxNOT_FOUND)
	{
		ListBoxallpart2->Clear();
		ListBoxallpart2->InsertItems(ExportConfig->GetPartition_aswxArrayString(pn+1),0);
		if(static_cast<int>(ListBoxallpart1->GetCount())>pn)
			ListBoxallpart1->SetSelection(pn);
	}else{
		ListBoxallpart2->Clear();
	}
	if((pn1!=wxNOT_FOUND)&&(static_cast<int>(ListBoxallpart3->GetCount())>pn1))
	{
		ListBoxallpart3->SetSelection(pn1);
	}else if(ListBoxallpart3->GetCount()>0)
		ListBoxallpart3->SetSelection(0);
}
//new Partition
void
SP_DLG_SMART::Partition_Button1(wxCommandEvent& p_cEvent)
{
	if ( Validate() && TransferDataFromWindow() )
	{
		//m_pcAnimationPrefs->OnKlick(m_sCurrentNetClass);
	}
	//create new Partition
	ExportConfig->newPartition();
	//update Listboxes
	UpdateListboxes();
	//select new list box
	ListBoxallpart1->SetSelection(ExportConfig->GetPartition_count()-1);

	PartitionList_Click(p_cEvent);
}
//delete Partition
void
SP_DLG_SMART::Partition_Button2(wxCommandEvent& p_cEvent)
{
	if ( Validate() && TransferDataFromWindow() )
	{
		//m_pcAnimationPrefs->OnKlick(m_sCurrentNetClass);
	}
	//if Partition selected, delete
	//pn feststellen
	int pn=wxNOT_FOUND;
	pn=ListBoxallpart1->GetSelection();
	if(pn!= wxNOT_FOUND)
	{
		ExportConfig->deletePartition(pn+1);
		ListBoxallpart1->SetSelection(-1);
		UpdateListboxes();
	}
}
//Element to Partition
void
SP_DLG_SMART::Partition_Button3(wxCommandEvent& p_cEvent)
{
	if ( Validate() && TransferDataFromWindow() )
	{
	}
	//get pn 
	int pn=0;
	pn=ListBoxallpart1->GetSelection();
	//get place
	wxString place_name=wxT("");
	place_name=ListBoxallpart3->GetStringSelection();
	if (place_name!=wxT("")){
		//IF not Partition selected
		if (pn== wxNOT_FOUND)
		{
			Partition_Button1(p_cEvent);
		}
		pn=ListBoxallpart1->GetSelection();	
	
		//remove place
		ExportConfig->DeletefromAllusableElementsforPartition(place_name);
		ListBoxallpart3->SetSelection(0);
		//put place to partition
		ExportConfig->AddElementtoPartition(pn+1,place_name);
		UpdateListboxes();
		//ListBoxallpart3->SetSelection(0);
	}
	
}
//Element from Partition
void
SP_DLG_SMART::Partition_Button4(wxCommandEvent& p_cEvent)
{
	if ( Validate() && TransferDataFromWindow() )
	{
	}
	// get pn
	int pn=0;
	pn=ListBoxallpart1->GetSelection();
	//IF Partition selected
	if(pn!= wxNOT_FOUND)
	{
		//get place
		wxString place_name=wxT("");
		place_name=ListBoxallpart2->GetStringSelection();

		ExportConfig->DeleteElementfromPartition(pn+1,place_name);
		ListBoxallpart2->SetSelection(-1);
		UpdateListboxes();
	}
}
void
SP_DLG_SMART::Partition_Save(wxCommandEvent& p_cEvent)
{
	if ( Validate() && TransferDataFromWindow() )
	{
	}
	wxFileDialog l_cDlg(this, wxT(""), wxT(""), wxT(""), wxT("Partition Files (*.partition)|*.partition"),  wxFD_CHANGE_DIR|wxFD_SAVE);
	if (l_cDlg.ShowModal() == wxID_OK) {
		wxString p_sFilename = l_cDlg.GetPath();
		wxFFile l_file;
		if (l_file.Open(p_sFilename.c_str(),wxT("wt")) )
		{
			if (l_file.IsOpened())
			{				
				//getting the system time
				time_t systime;
				time(&systime);
				wxString TimeStamp(asctime(localtime(&systime)), wxConvUTF8);
				//asctime() puts a '\n' at the end of the time string which is deleted here:
				TimeStamp.erase(24);
				//write partition 
				l_file.Write(wxT("// netname ") + ExportConfig->GetConfigurationElement(wxT("netname"))->value + wxT("\n"));
				l_file.Write(wxT("// export time ")+TimeStamp+wxT("\n"));
				l_file.Write(wxT("partition("));
				int part_anz=ExportConfig->GetPartition_count();
				wxString parttxt=wxT("");
				for( int i=0;i<part_anz;i++)
				{
					parttxt=ExportConfig->GetPartition_aswxString(i+1);
					parttxt.Replace(wxT(","),wxT(":"));
					l_file.Write(parttxt);
					if (i<part_anz-1){
						l_file.Write(wxT(", "));
					}
				}
				l_file.Write(wxT(")\n"));
				l_file.Close();
			}
		}
	}
	l_cDlg.Destroy();
}
wxString
SP_DLG_SMART::getnextToken(wxString data ,long fileSize,long& atpos){
	wxString Token=wxT("");
	long pos=atpos;
	bool find_end_of_token=false;
	bool kommentar=false;
	if(fileSize==atpos)
	{
		return Token;
	}
	wxString zeichen=wxT("");
	wxString zeichennext=wxT("");
	zeichen=data.SubString(pos,pos);
	while(zeichen==wxT("\n"))
	{
		atpos++;
		pos++;
		if (pos==fileSize){
			atpos=pos;
			return Token;
		}
		zeichen=data.SubString(pos,pos);
	}
	
	if(atpos>=2)
	{
		if(data.SubString(atpos-2,atpos-2+1)==wxT("//"))
		{
			kommentar=true;
		}
	}
	
	while (!find_end_of_token){
		//'Tokens:
		//', Komma
		//': separator
		//'( and )
		//'// Komentra
		//'*  Element
		zeichen=data.SubString(pos,pos);
		
		if (pos<fileSize)
		{
			 zeichennext=data.SubString(pos+1,pos+1);
		}else
		{
			zeichennext=wxT("");
		}
		if((
			((zeichen==wxT("/")) && (zeichennext==wxT("/")))
			||(zeichen==wxT(","))
			||(zeichen==wxT(":"))
			||(zeichen==wxT("("))
			||(zeichen==wxT(")"))
			||(zeichen==wxT("\n"))
			||(fileSize==pos))&&(!kommentar))
		{
			find_end_of_token=true;
			if (pos==atpos){
				if (zeichen==wxT("/")){
						Token=wxT("//");
						pos=pos+2;
				}else{
					Token=zeichen;
					pos++;
				}
			}
		}else if(kommentar && zeichen==wxT("\n"))
		{
			find_end_of_token=true;
			
		}else
		{
			Token=Token+zeichen;
			pos++;
		}

	}
	atpos=pos;
	return Token;
}
void
SP_DLG_SMART::Load_Partition_fromDATA(wxString data ,long fileSize)
{
	int state=0;// 0 - start,1-Kommentar-//,2-Komentar,3-Partition-head,4-Partitionhead-(,5-Partition-head-),6-Partition,7-Partition-:,8-Partition-,,9-Partition-Kommentar
	long pos=0;
	int line=1;
	long lastlineend=0;
	long posinline=0;
	if(fileSize==0){
		SP_MESSAGEBOX(wxT("Error: file empty"), wxT("Error"), wxOK|wxICON_ERROR );
	}
	wxString Token=getnextToken(data,fileSize,pos);
	Token.Trim();
	bool end=false;
	wxString errortxt=wxT("");
	//delete all partition
	int anz_part=ExportConfig->GetPartition_count();
	for(int pn=0;pn<anz_part;pn++)
	{
		ExportConfig->deletePartition(1);
	}
	int partition=-1;
	while (!end)
	{
		if(pos==fileSize)
		{
			state=-1;
			end=true;
			errortxt=wxT("Error in structure no correct Partition found, at line ") +wxString::Format(wxT("%u"),line)+  wxT(" at position ")  + wxString::Format(wxT("%u"),posinline);
		}
		for(long p=lastlineend+1;p<=pos;p++)
		{
			if (data.SubString(p,p)==wxT("\n")){
				line++;
				lastlineend=p;
			}
		}
		if(pos>(lastlineend+1))
		{
			posinline=pos-(lastlineend+1);
		}else{
			posinline=0;
		}
		
		switch(state)
		{
			case 0:
				if (Token==wxT("//"))
				{
					state=1;
				}
				if(Token==wxT("partition"))
				{
					state=3;
				}
				if(state==0){
					errortxt=wxT("Error in structure, at line ") +wxString::Format(wxT("%u"),line)+  wxT(" at position ")  + wxString::Format(wxT("%u"),posinline);
				}
				break;
			case 1:
				state=0;
				break;
			case 3:
				if(Token==wxT("("))
				{
					state=4;
				}else{
					errortxt=wxT("Error in structure,after 'Partition must come'(' at line ") +wxString::Format(wxT("%u"),line)+  wxT(" at position ")  + wxString::Format(wxT("%u"),posinline);
				}
				break;
			case 4:
				if(Token==wxT(")"))
				{
					state=5;
					end=true;
				}
				else if(Token==wxT(":"))
				{
					errortxt=wxT("Error in structure, at line ") +wxString::Format(wxT("%u"),line)+  wxT(" at position ")  + wxString::Format(wxT("%u"),posinline);
				}else if(Token==wxT(","))
				{
					ExportConfig->newPartition();
				}else
				{
					state=6;
					if (partition==-1){
						//new partition write element
						ExportConfig->newPartition();
						partition=ExportConfig->GetPartition_count();
					}
					//check if token exist
					if(Token!=wxT(""))
					{
						if (ExportConfig->DeletefromAllusableElementsforPartition(Token))
						{
							ExportConfig->AddElementtoPartition(partition,Token);
						}else
						{
							wxString msgtxt=wxT("Place:") + Token+wxT(" not in this Graph!");
							SP_MESSAGEBOX(msgtxt, wxT("Information"), wxOK|wxICON_INFORMATION  );
						}
					}
				}
				break;
			case 6:
				if(Token==wxT(":"))
				{
					state=4;
				}else
				if(Token==wxT(","))
				{
					state=4;
					//end partition
					partition=-1;
				}else
				if(Token==wxT(")"))
				{
					state=5;
					end=true;
				}else
				if(state==6)
				{
					errortxt=wxT("Error in structure, at line ") +wxString::Format(wxT("%u"),line)+  wxT(" at position ")  + wxString::Format(wxT("%u"),posinline);
				}
				break;

		}
		if(errortxt!=wxT("")){
			end=true;
			SP_MESSAGEBOX(errortxt, wxT("Error"), wxOK|wxICON_ERROR );
		}else if(!end){
			//pos=pos+Token.Len();
			Token=getnextToken(data,fileSize,pos);
			Token=Token.Trim(false);
			Token=Token.Trim(true);
		}
	}
	UpdateListboxes();
}
void
SP_DLG_SMART::Partition_Load(wxCommandEvent& p_cEvent)
{
	if ( Validate() && TransferDataFromWindow() )
	{
	}
	
	wxFileDialog l_cDlg(this, wxT(""), wxT(""), wxT(""), wxT("Partition Files (*.partition)|*.partition|SMART Files (*.smart)|*.smart"), wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_CHANGE_DIR);
	if (l_cDlg.ShowModal() == wxID_OK) {
		wxString p_sFilename = l_cDlg.GetFilename();
		
		wxFFile l_file;
		if (l_file.Open(p_sFilename.c_str(),   wxT("rt")) )//wt
		{
			if (l_file.IsOpened())
			{				
				wxString data=wxT("");
 				l_file.ReadAll(&data,wxConvUTF8);
				//copy only partition lines
				wxStringTokenizer tkz(data, wxT("\n"));
				wxString datazeile=wxT("");
				while ( tkz.HasMoreTokens() )
				{
					wxString token = tkz.GetNextToken();
					token=token.Trim(true);
					token=token.Trim(false);
					if (datazeile==wxT(""))
					{
						if (token.Left(9)==wxT("partition"))
						{
							datazeile=token;
						}
					}else
					{
						datazeile=datazeile + wxT("\n") +token;
					}
				}
				datazeile=datazeile + wxT("\n");
				long fileSize=datazeile.Len();
				Load_Partition_fromDATA(datazeile,fileSize);
				l_file.Close();
			}
		}
	}
	l_cDlg.Destroy();
}
void
SP_DLG_SMART::PartitionList_Click(wxCommandEvent& p_cEvent)
{
	if ( Validate() && TransferDataFromWindow() )
	{
	}
	int pn=0;
	pn=ListBoxallpart1->GetSelection();
	if (pn!=lastSelectedPartition){
		lastSelectedPartition=pn;
		if  (pn!= wxNOT_FOUND)
		{
			ListBoxallpart2->Clear();
			ListBoxallpart2->InsertItems(ExportConfig->GetPartition_aswxArrayString(pn+1),0);
		}else{
			ListBoxallpart2->Clear();
		}
	}
}

///////////////////
//CTL
///////////////////
void 
SP_DLG_SMART::CTL_LOAD(wxCommandEvent& p_cEvent)
{
	if ( Validate() && TransferDataFromWindow() )
	{
	}
	wxFileDialog l_cDlg(this, wxT(""), wxT(""), wxT(""), wxT("CTL Files (*.ctl)|*.ctl"), wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_CHANGE_DIR);
	if (l_cDlg.ShowModal() == wxID_OK) {
		wxString p_sFilename = l_cDlg.GetFilename();
		
		wxFFile l_file;
		if (l_file.Open(p_sFilename.c_str(),   wxT("rt")) )//wt
		{
			if (l_file.IsOpened())
			{				
				wxString data=wxT("");
 				l_file.ReadAll(&data,wxConvUTF8);
				ctl_textbox->SetValue(ctl_textbox->GetValue()+data);
				l_file.Close();
			}
		}
	}
	l_cDlg.Destroy();
}
void 
SP_DLG_SMART::CTL_SAVE(wxCommandEvent& p_cEvent)
{
	wxFileDialog l_cDlg(this, wxT(""), wxT(""), wxT(""), wxT("CTL Files (*.ctl)|*.ctl"), wxFD_SAVE |  wxFD_CHANGE_DIR);
	if (l_cDlg.ShowModal() == wxID_OK) {
		wxString p_sFilename = l_cDlg.GetFilename();
		
		wxFFile l_file;
		if (l_file.Open(p_sFilename.c_str(),   wxT("wt")) )//wt
		{
			if (l_file.IsOpened())
			{				
				wxString data=ctl_textbox->GetValue();
 				l_file.Write(data);
				l_file.Close();
			}
		}
	}
	l_cDlg.Destroy();	
}
void 
SP_DLG_SMART::CTL_CLEAR(wxCommandEvent& p_cEvent)
{
	ctl_textbox->SetValue(wxT(""));
}
void 
SP_DLG_SMART::CTL_CHECK(wxCommandEvent& p_cEvent)
{
	wxString data=ctl_textbox->GetValue();
	if(data.Len()>0)
	{
		wxString erg;
		erg=ESMART->CTL_SMART->CTL_Check(data);
		wxString msg;
		
		if (erg==wxT(""))
		{
			msg=wxT("CTL conform!");
			SP_MESSAGEBOX(msg, wxT(""), wxOK );
		}else
		{
			msg=erg;
			SP_MESSAGEBOX(msg, wxT("Error"), wxOK|wxICON_ERROR );
		}
	}
}

/////////////////////
//SteadyState Analyse
/////////////////////
void
SP_DLG_SMART::SteadyState_ADD(wxCommandEvent& p_cEvent)
{
	wxString txt=Textbox_steadystate->GetValue();
	int mark=ListBox_steadystate->GetSelection();
	if(mark!=wxNOT_FOUND)
	{
		ListBox_steadystate->SetString(mark,txt);
	}else
	{
		ListBox_steadystate->AppendString(txt);
	}
}
void
SP_DLG_SMART::SteadyState_REMOVE(wxCommandEvent& p_cEvent)
{
	int max_anz=ListBox_steadystate->GetCount();
	int mark=ListBox_steadystate->GetSelection();
		//wxString
	if(mark<max_anz)
		ListBox_steadystate->Delete(mark);
}
void
SP_DLG_SMART::SteadyState_CLEAR(wxCommandEvent& p_cEvent)
{
	ListBox_steadystate->Clear();
}
void
SP_DLG_SMART::SteadyState_CHECK(wxCommandEvent& p_cEvent)
{
	int mark=ListBox_steadystate->GetSelection();
	if(mark!=wxNOT_FOUND)
	{
		wxString line=ListBox_steadystate->GetStringSelection();
		wxString erg=ESMART->IL_SMART->IL_Check(line);
		wxString msg=wxT("");
		if (erg==wxT(""))
		{
			msg=wxT("IL conform!");
			SP_MESSAGEBOX(msg, wxT(""), wxOK );
		}else
		{
			msg=erg;
			SP_MESSAGEBOX(msg, wxT("Error"), wxOK|wxICON_ERROR );
		}
	}else
	{
		SP_MESSAGEBOX(wxT("no line is choose"), wxT("Information"), wxOK);
	}
}
void
SP_DLG_SMART::SteadyStateList_Click(wxCommandEvent& p_cEvent)
{
	if ( Validate() && TransferDataFromWindow() )
	{
	}
	int mark=ListBox_steadystate->GetSelection();
	if(mark!=wxNOT_FOUND)
		Textbox_steadystate->SetValue(ListBox_steadystate->GetStringSelection());
	
}
void
SP_DLG_SMART::SteadyState_LOAD(wxCommandEvent& p_cEvent)
{
	wxString txt=IL_LOAD();
	if(txt!=wxT(""))
	{
		int mark=ListBox_steadystate->GetSelection();
		if(mark!=wxNOT_FOUND)
		{
			ListBox_steadystate->SetString(mark,txt);
		}else
		{
			ListBox_steadystate->AppendString(txt);
		}
	}
}
void
SP_DLG_SMART::SteadyState_SAVE(wxCommandEvent& p_cEvent)
{
	int mark=ListBox_steadystate->GetSelection();
	if(mark!=wxNOT_FOUND)
		IL_SAVE( ListBox_steadystate->GetStringSelection());
}
////////////////////
//Transiente Analyse
////////////////////
void
SP_DLG_SMART::Transiente_ADD(wxCommandEvent& p_cEvent)
{
	wxString l_wxS_htime=Textbox_transiente_time->GetValue();
	l_wxS_htime.Trim(false);
	l_wxS_htime.Trim(true);
	if(l_wxS_htime.Len()>0)
	{
		long time=0;
		if(!l_wxS_htime.ToLong(&time))
			Textbox_transiente_time->SetValue(wxT(""));
		else
		{
			if(time<0)
				time=time*(-1);
			Textbox_transiente_time->SetValue(wxString::Format(wxT("%d"),time));
		}
	}
	wxString txt= Textbox_transiente_time->GetValue() + wxT(" at ")+Textbox_transiente->GetValue();
	int mark=ListBox_transiente->GetSelection();
	if(mark!=wxNOT_FOUND)
	{
		ListBox_transiente->SetString(mark,txt);
	}else
	{
		ListBox_transiente->AppendString(txt);
	}
}
void
SP_DLG_SMART::Transiente_REMOVE(wxCommandEvent& p_cEvent)
{
	int max_anz=ListBox_transiente->GetCount();
	int mark=ListBox_transiente->GetSelection();
		//wxString
	if(mark<max_anz)
		ListBox_transiente->Delete(mark);
}
void
SP_DLG_SMART::Transiente_CLEAR(wxCommandEvent& p_cEvent)
{
	ListBox_transiente->Clear();
}
void
SP_DLG_SMART::Transiente_CHECK(wxCommandEvent& p_cEvent)
{
	int mark=ListBox_transiente->GetSelection();
	if(mark!=wxNOT_FOUND)
	{
		wxString line=ListBox_transiente->GetStringSelection();
		line=line.SubString(line.First(wxT(" at "))+4,line.Len());
		wxString erg=ESMART->IL_SMART->IL_Check(line);
		wxString msg=wxT("");
		if (erg==wxT(""))
		{
			msg=wxT("IL conform!");
			SP_MESSAGEBOX(msg, wxT(""), wxOK );
		}else
		{
			msg=erg;
			SP_MESSAGEBOX(msg, wxT("Error"), wxOK|wxICON_ERROR );
		}
	}else
	{
		SP_MESSAGEBOX(wxT("no line is choose"), wxT("Information"), wxOK);
	}
}
void
SP_DLG_SMART::TransienteList_Click(wxCommandEvent& p_cEvent)
{
	if ( Validate() && TransferDataFromWindow() )
	{
	}
	int mark=ListBox_transiente->GetSelection();
	if(mark!=wxNOT_FOUND)
	{
		wxString txt=ListBox_transiente->GetStringSelection();
		wxString t1=txt.SubString(0,txt.First(wxT(" at "))-1);
		wxString t2=txt.SubString(txt.First(wxT(" at "))+4,txt.Len());
		Textbox_transiente->SetValue(t2);
		Textbox_transiente_time->SetValue(t1);
	}
}

void
SP_DLG_SMART::Transiente_LOAD(wxCommandEvent& p_cEvent)
{
	wxString txt=IL_LOAD();
	if (txt!=wxT(""))
	{
		txt=  wxT("1 at ") + txt;
		int mark=ListBox_transiente->GetSelection();
		if(mark!=wxNOT_FOUND)
		{
			ListBox_transiente->SetString(mark,txt);
		}else
		{
			ListBox_transiente->AppendString(txt);
		}
	}
}
void
SP_DLG_SMART::Transiente_SAVE(wxCommandEvent& p_cEvent)
{
	int mark=ListBox_transiente->GetSelection();
	if(mark!=wxNOT_FOUND)
	{
		wxString txt=ListBox_transiente->GetStringSelection();
		wxString token2=txt.SubString(txt.First(wxT(" at "))+1,txt.Len());
		IL_SAVE(token2);
	}
}
void
SP_DLG_SMART::IL_SAVE(wxString p_line)
{
	wxFileDialog l_cDlg(this, wxT(""), wxT(""), wxT(""), wxT("IL Files (*.il)|*.il"), wxFD_SAVE |  wxFD_CHANGE_DIR);
	if (l_cDlg.ShowModal() == wxID_OK) {
		wxString p_sFilename = l_cDlg.GetFilename();
		
		wxFFile l_file;
		if (l_file.Open(p_sFilename.c_str(),   wxT("wt")) )//wt
		{
			if (l_file.IsOpened())
			{				
				wxString data=p_line;
 				l_file.Write(data);
				l_file.Close();
			}
		}
	}
	l_cDlg.Destroy();	
}
wxString
SP_DLG_SMART::IL_LOAD()
{
	wxString data=wxT("");
	wxString line=wxT("");
	wxFileDialog l_cDlg(this, wxT(""), wxT(""), wxT(""), wxT("IL Files (*.il)|*.il"), wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_CHANGE_DIR);
	if (l_cDlg.ShowModal() == wxID_OK) {
		wxString p_sFilename = l_cDlg.GetFilename();
		wxFFile l_file;
		if (l_file.Open(p_sFilename.c_str(),   wxT("rt")) )//wt
		{
			if (l_file.IsOpened())
			{				
 				l_file.ReadAll(&data,wxConvUTF8);
				l_file.Close();
			}
		}
		wxArrayString split_data=ESMART->SplitwxString( data,wxT("\n"));
		line=wxT("");
		for(unsigned int i=0;i<split_data.Count();i++)
		{
			line=split_data.Item(i);
			line=line.Trim(true);
			line=line.Trim(false);
			if(line.Len()>0)
				if(line.SubString(0,1)!=wxT("//"))
				{
					break;
				}
		}
	}
	l_cDlg.Destroy();
	return line;

}

