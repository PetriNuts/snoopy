//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: liu $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2010/04/15 10:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#include <wx/valgen.h>
#include <wx/tipwin.h>



#include "sp_gui/dialogs/dia_CPN/SP_DLG_CPNDeclaration.h"
#include "sp_gui/dialogs/dia_SPN/SP_DLG_StFunctionAssistent.h"

#include "sp_ds/attributes/SP_DS_ColListAttribute.h"

#include "sp_gui/windows/SP_GUI_Mainframe.h"

#include "sp_gui/mdi/SP_MDI_Doc.h"

#include "sp_core/SP_Core.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "snoopy.h"
#include "sp_ds/SP_DS_Node.h"

IMPLEMENT_CLASS( SP_DLG_CPNDeclaration, wxDialog )

enum
{
	SP_ID_GRID_MARKING
};

BEGIN_EVENT_TABLE( SP_DLG_CPNDeclaration, wxDialog )

EVT_BUTTON( wxID_OK, SP_DLG_CPNDeclaration::OnDlgOk )
EVT_BUTTON( wxID_CANCEL, SP_DLG_CPNDeclaration::OnDlgCancel )


END_EVENT_TABLE()


SP_DLG_CPNDeclaration::SP_DLG_CPNDeclaration( wxWindow* p_pcParent,
		const wxString& p_sTitle, long p_nStyle) :

	wxDialog(p_pcParent, -1, p_sTitle, wxPoint( 120, 120), wxSize( 1500, 400),
			p_nStyle | wxSTAY_ON_TOP | wxRESIZE_BORDER | wxMAXIMIZE_BOX)

{

	m_pcParent = p_pcParent;

	m_pcSizer = new wxBoxSizer( wxVERTICAL );

	// set / name components
	wxBoxSizer* l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );


	l_pcRowSizer->Add(new wxStaticText( this, -1, wxT("Filename:") ), 0, wxALL | wxEXPAND, 5);

	m_pcFilePickerCtrl = new wxFilePickerCtrl( this, -1, wxT(""), wxT("Browse"), wxT("Text files(*.txt)|*.txt"), wxDefaultPosition,
			wxDefaultSize, wxFLP_OPEN | wxFLP_USE_TEXTCTRL | wxFLP_CHANGE_DIR);

	m_pcFilePickerCtrl->SetPath(wxT("Declaration.txt"));
	
	l_pcRowSizer->Add(m_pcFilePickerCtrl, 1, wxEXPAND | wxALL, 5);
    
   //SP_AutoSizeRowLabelSize(m_pcFunctionGrid);

	wxBoxSizer* l_pcStaticSizer = new wxBoxSizer( wxHORIZONTAL );
	wxStaticText* l_StaticText = new wxStaticText(this, -1, wxT(""));
	l_pcStaticSizer->Add(l_StaticText, 1, wxEXPAND | wxALL, 5);




	wxBoxSizer* l_pcButtonSizer = new wxBoxSizer( wxHORIZONTAL );
	
	wxSizer *l_pcBottomButtonSizer = new wxBoxSizer( wxHORIZONTAL );

	l_pcBottomButtonSizer->Add(this->CreateButtonSizer(wxOK|wxCANCEL), 0,wxEXPAND | wxALL, 5);
	
	l_pcButtonSizer->Add(l_pcBottomButtonSizer, 0, wxEXPAND | wxALIGN_BOTTOM | wxALIGN_CENTER);

	m_pcSizer->Add(l_pcRowSizer, 1, wxEXPAND | wxALIGN_RIGHT);

	m_pcSizer->Add(l_pcStaticSizer, 1, wxEXPAND | wxALIGN_RIGHT);

	m_pcSizer->Add(l_pcButtonSizer, 0, wxEXPAND | wxALIGN_RIGHT);

	SetSizerAndFit(m_pcSizer);

}

void SP_DLG_CPNDeclaration::OnDlgOk(wxCommandEvent& p_cEvent)
{

	if (Validate() && TransferDataFromWindow() )
	{
		if (IsModal() )
		{			
			//RefreshFrame();
			WriteDeclaration() ;
				

			EndModal(wxID_OK);
		}
		else
		{
			SetReturnCode(wxID_OK);
			this->Show( FALSE);
		}
	}

}

void SP_DLG_CPNDeclaration::OnDlgCancel(wxCommandEvent& p_cEvent)
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

bool SP_DLG_CPNDeclaration::WriteDeclaration()
{

	

	wxString l_sPathName = m_pcFilePickerCtrl->GetPath();

	if (!m_file.Open(l_sPathName.c_str(), wxT("wt")) )
	{
		return false;
	}

	// will be implemented in successors

	m_file.Write(wxT("Declarations:\n\n"));
	

	if( ! WriteColorSets() )
		return false;
	// close file handle and check if this was ok
	if( ! m_file.Close() )
		return false;




	return true;
}


bool SP_DLG_CPNDeclaration::WriteColorSets()
{
	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();

	SP_DS_Metadataclass* l_pcMetadataclass;
	SP_DS_Metadata* l_pcNewMetadata;
	SP_DS_ColListAttribute * l_pcColList;
	

	if(!l_pcGraph)
		return false;
	
	l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_CPN_BASICCOLORSETCLASS);
	if(!l_pcMetadataclass)
		return false;	

	l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	if(!l_pcNewMetadata)
		return false;	

	l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("ColorsetList")));
	if(!l_pcColList)
		return false;

		
	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		wxString l_sColorset;

		if( l_pcColList->GetCell(i,1) == wxT("bool") ||
			l_pcColList->GetCell(i,1) == wxT("dot") )
			l_sColorset = wxT("Colorset ") + l_pcColList->GetCell(i,0) + wxT(" = ") + l_pcColList->GetCell(i,1) + wxT(";\n");
		else
			l_sColorset = wxT("Colorset ") + l_pcColList->GetCell(i,0) + wxT(" = ") + l_pcColList->GetCell(i,1) + wxT(" with ") + l_pcColList->GetCell(i,2)+wxT(";\n");

		m_file.Write(l_sColorset);
	}


	m_file.Write(wxT("\n"));

	l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_CPN_STRUCTUREDCOLORSETCLASS);
	if(!l_pcMetadataclass)
		return false;
	l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	if(!l_pcNewMetadata)
		return false;
	l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("StructuredColorsetList")));
	if(!l_pcColList)
		return false;

	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{	
		wxString l_sColorset = wxT("Colorset ") + l_pcColList->GetCell(i,0) + wxT(" = ") + l_pcColList->GetCell(i,1) + wxT(" with ") + l_pcColList->GetCell(i,2)+wxT(";\n");
		m_file.Write(l_sColorset);
	}

	m_file.Write(wxT("\n"));

	l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_CPN_VARIABLECLASS);
	if(!l_pcMetadataclass)
		return false;

	l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());

	if(!l_pcNewMetadata)
		return false;

	l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("VariableList")));
	if(!l_pcColList)
		return false;
	
	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		wxString l_sColorset = wxT("Variable ") + l_pcColList->GetCell(i,0) + wxT(" : ") + l_pcColList->GetCell(i,1) +wxT(";\n");
		m_file.Write(l_sColorset);
	}

	m_file.Write(wxT("\n"));

	l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_CPN_CONSTANTCLASS);
	if(!l_pcMetadataclass)
		return false;

	l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	if(!l_pcNewMetadata)
		return false;

	l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("ConstantList")));
	if(!l_pcColList)
		return false;
	
	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		wxString l_sColorset = wxT("Constant ") + l_pcColList->GetCell(i,0) + wxT(" = ") + l_pcColList->GetCell(i,1) + wxT(" with ") + l_pcColList->GetCell(i,2)+wxT(";\n");
		m_file.Write(l_sColorset);
	}


	m_file.Write(wxT("\n"));

	l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_CPN_FUNCTIONCLASS);
	if(!l_pcMetadataclass)
		return false;

	l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	if(!l_pcNewMetadata)
		return false;

	l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("FunctionList")));
	if(!l_pcColList)
		return false;
	
	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		wxString l_sColorset = wxT("Function ") + l_pcColList->GetCell(i,0) + wxT(" ") + l_pcColList->GetCell(i,1) + wxT("( ") + l_pcColList->GetCell(i,2)+ wxT(") { ") + l_pcColList->GetCell(i,3)+wxT("; }\n");
		m_file.Write(l_sColorset);

	}


	return true;
}
