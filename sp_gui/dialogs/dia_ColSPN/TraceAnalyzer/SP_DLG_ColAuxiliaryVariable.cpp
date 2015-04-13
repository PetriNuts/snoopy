//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.0 $
// $Date: 2009/10/11 11:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#include <wx/valgen.h>
#include <wx/tipwin.h>

#include "sp_gui/dialogs/dia_ColSPN/TraceAnalyzer/SP_DLG_ColAuxiliaryVariable.h"

#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_gui/windows/SP_GUI_Mainframe.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_core/SP_Core.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "snoopy.h"
#include "sp_ds/SP_DS_Node.h"

#include "sp_core/SP_Vector.h"


#include "sp_gui/dialogs/dia_CPN/SP_DLG_ColorSetSelection.h"

#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ValueAssign.h"
#include "sp_ds/extensions/Col_PN/SyntaxChecking/SP_CPN_SyntaxChecking.h"
#include <wx/busyinfo.h> 

#include <wx/colordlg.h>
#include "sp_gui/dialogs/dia_ColSPN/TraceAnalyzer/SP_DLG_ColColoredPlaceSelection.h"

#include "sp_gui/dialogs/dia_CPN/SP_DLG_ExpressionAssistent.h"

#include "sp_ds/attributes/SP_DS_NameAttribute.h"

IMPLEMENT_CLASS( SP_DLG_ColAuxiliaryVariable, wxDialog )

enum SP_CPN_ID
{
	SP_ID_BUTTON_ADD,
	SP_ID_BUTTON_DELETE,
	SP_ID_BUTTON_CHECK,
	SP_ID_GRID_TRACER,
	SP_ID_BUTTON_ASSISTENT
};

BEGIN_EVENT_TABLE( SP_DLG_ColAuxiliaryVariable, wxDialog )

EVT_BUTTON( wxID_OK, SP_DLG_ColAuxiliaryVariable::OnDlgOk )
EVT_BUTTON( wxID_CANCEL, SP_DLG_ColAuxiliaryVariable::OnDlgCancel )
EVT_BUTTON( wxID_APPLY, SP_DLG_ColAuxiliaryVariable::OnDlgApply )
EVT_BUTTON( SP_ID_BUTTON_CHECK, SP_DLG_ColAuxiliaryVariable::OnCheck )
EVT_BUTTON( SP_ID_BUTTON_ADD, SP_DLG_ColAuxiliaryVariable::OnAdd )
EVT_BUTTON( SP_ID_BUTTON_DELETE, SP_DLG_ColAuxiliaryVariable::OnDelete )
EVT_BUTTON( SP_ID_BUTTON_ASSISTENT, SP_DLG_ColAuxiliaryVariable::OnAssistent )

#if wxABI_VERSION > 30000
    EVT_GRID_CELL_CHANGED( SP_DLG_ColAuxiliaryVariable::OnCellValueChanged )
#else
    EVT_GRID_CELL_CHANGE( SP_DLG_ColAuxiliaryVariable::OnCellValueChanged )
#endif

EVT_GRID_CELL_LEFT_CLICK( SP_DLG_ColAuxiliaryVariable::OnEditorShown )

END_EVENT_TABLE()



SP_DLG_ColAuxiliaryVariable::SP_DLG_ColAuxiliaryVariable( wxWindow* p_pcParent,const wxString& p_sTitle, long p_nStyle) 
		:wxDialog(p_pcParent, -1, p_sTitle, wxPoint( 120, 120),  wxSize( 1500, 400),
			p_nStyle | wxRESIZE_BORDER | wxMAXIMIZE_BOX)
{
	m_pcParent = p_pcParent;	

	m_pcSizer = new wxBoxSizer( wxVERTICAL );

	// grid
	wxStaticBox *l_pcGridBox = new wxStaticBox( this, -1, wxT("") );
	wxSizer *l_pcGridSizer = new wxStaticBoxSizer( l_pcGridBox, wxHORIZONTAL );	

	m_pcGrid = new wxGrid( this, SP_ID_GRID_TRACER, wxDefaultPosition, wxSize( 700, 300 ), wxSUNKEN_BORDER );
	m_pcGrid->CreateGrid( 0, 0);
	m_pcGrid->EnableEditing( true);	
	m_pcGrid->SetDefaultColSize( 220, TRUE);
	m_pcGrid->SetDefaultRowSize( 20, TRUE);
	m_pcGrid->SetColLabelSize( 16);
	m_pcGrid->SetRowLabelSize( 0); //do not let it show.

	m_pcGrid->EnableEditing(true);
	m_pcGrid->SetSelectionMode(wxGrid::wxGridSelectCells);	
	m_pcGrid->AppendCols(5);
	m_pcGrid->SetColLabelValue(0, wxT("Variable name") );
	m_pcGrid->SetColSize(0, 100);
	m_pcGrid->SetColLabelValue(1, wxT("Selected places") );
	m_pcGrid->SetColSize(1, 200);
	m_pcGrid->SetColLabelValue(2, wxT("Function") );
	m_pcGrid->SetColSize(2, 50);
	m_pcGrid->SetColLabelValue(3, wxT("Predicate") );
	m_pcGrid->SetColSize(3, 200);
	m_pcGrid->SetColLabelValue(4, wxT("Comments") );
	m_pcGrid->SetColSize(4, 140);

	l_pcGridSizer->Add(m_pcGrid, 1, wxALL | wxEXPAND, 2);
    SP_AutoSizeRowLabelSize(m_pcGrid);

	m_pcSizer->Add(l_pcGridSizer, 0, wxEXPAND | wxALIGN_RIGHT);

	//checkbox
	wxStaticBox *l_pcCheckBoxBox = new wxStaticBox( this, -1, wxT("") );
	wxSizer *l_pcCheckBoxSizer = new wxStaticBoxSizer( l_pcCheckBoxBox, wxHORIZONTAL );	

	m_cbComSingleRun = new wxCheckBox(this, -1, wxT("Compute for each simulation run"));
	m_cbComSingleRun->SetValue(false);
	l_pcCheckBoxSizer->Add(m_cbComSingleRun, 1, wxALL | wxEXPAND, 2);

	m_pcSizer->Add(l_pcCheckBoxSizer, 0, wxALL | wxEXPAND, 5);


	//bottons
	wxBoxSizer* l_pcButtonSizer = new wxBoxSizer(wxHORIZONTAL );

	wxStaticBox *l_pcBottomButtonBox1 = new wxStaticBox( this, -1, wxT("") );
	wxSizer *l_pcSizer = new wxStaticBoxSizer( l_pcBottomButtonBox1, wxHORIZONTAL );	

	l_pcSizer->Add(new wxButton(this, SP_ID_BUTTON_ADD, wxT("Add") ), 1, wxLEFT | wxRIGHT | wxTOP, 5);	
	l_pcSizer->Add(new wxButton(this, SP_ID_BUTTON_DELETE, wxT("Delete") ), 1, wxLEFT | wxRIGHT | wxTOP, 5);
	l_pcSizer->Add(new wxButton(this, SP_ID_BUTTON_CHECK, wxT("Check") ), 1, wxLEFT | wxRIGHT | wxTOP, 5);
	l_pcSizer->Add(new wxButton(this, SP_ID_BUTTON_ASSISTENT, wxT("Assistant") ), 1, wxLEFT | wxRIGHT | wxTOP, 5);
	
	wxStaticBox *l_pcBottomButtonBox = new wxStaticBox( this, -1, wxT("") );
	wxSizer *l_pcBottomButtonSizer = new wxStaticBoxSizer( l_pcBottomButtonBox, wxHORIZONTAL );

	l_pcBottomButtonSizer->Add(this->CreateButtonSizer(wxOK|wxCANCEL), 0,wxEXPAND | wxALL, 5);
	//l_pcBottomButtonSizer->Add(new wxButton(this,wxID_APPLY,wxT("Apply")), 0, wxALL, 5);
	
	wxBoxSizer* l_pcSpaceSizer = new wxBoxSizer(wxHORIZONTAL );
	l_pcSpaceSizer->Add(30,0,0);

	l_pcButtonSizer->Add(l_pcSizer, 0, wxEXPAND | wxALIGN_BOTTOM | wxALIGN_LEFT);
	l_pcButtonSizer->Add(l_pcSpaceSizer, 0, wxEXPAND | wxALIGN_BOTTOM | wxALIGN_LEFT);
	l_pcButtonSizer->Add(l_pcBottomButtonSizer, 0, wxEXPAND | wxALIGN_BOTTOM | wxALIGN_RIGHT);

	
	m_pcSizer->Add(l_pcButtonSizer, 0, wxEXPAND | wxALIGN_RIGHT);

	SetAutoLayout( TRUE);
	SetSizer(m_pcSizer);

	Initialize();

	LoadData();

	m_pcSizer->Fit( this);
	m_pcSizer->SetSizeHints( this);

	Layout();
}


void SP_DLG_ColAuxiliaryVariable::OnDlgApply( wxCommandEvent& p_cEvent )
{		
	SaveData();	
}

void SP_DLG_ColAuxiliaryVariable::OnDlgOk(wxCommandEvent& p_cEvent)
{	
	if (Validate() && TransferDataFromWindow())
	{
		if (IsModal())
		{
			m_bComSingleRun = m_cbComSingleRun->GetValue();
			if (SaveData())
			{				
				EndModal(wxID_OK);
			}
		}
		else
		{
			SetReturnCode(wxID_OK);
			this->Show(FALSE);
		}
	}
}

void SP_DLG_ColAuxiliaryVariable::OnDlgCancel(wxCommandEvent& p_cEvent)
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


void SP_DLG_ColAuxiliaryVariable::OnDelete( wxCommandEvent& p_cEvent )
{	
	if (m_pcGrid->GetNumberRows() == 0)
	{
		return;
	}

	if ( m_pcGrid->IsSelection() )
    {
        m_pcGrid->BeginBatch();
        for ( int n = 0; n < m_pcGrid->GetNumberRows(); )
        {
            if ( m_pcGrid->IsInSelection( n , 0 ) )
                m_pcGrid->DeleteRows( n, 1 );
            else
                n++;
        }
        m_pcGrid->EndBatch();
    }

}

void SP_DLG_ColAuxiliaryVariable::OnAdd( wxCommandEvent& p_cEvent )
{    
	m_pcGrid->AppendRows(1);
	int l_nEditRowPos = m_pcGrid->GetNumberRows()-1;				
	m_pcGrid->SetCellEditor(l_nEditRowPos,2,new wxGridCellChoiceEditor(WXSIZEOF(m_PREDEFINEDFUNCTIONS), m_PREDEFINEDFUNCTIONS));
	m_pcGrid->SetCellSize(l_nEditRowPos, 2, 1, 1);
	m_pcGrid->SetCellValue(l_nEditRowPos, 2, wxT("SUM"));	
	m_pcGrid->SetCellOverflow(l_nEditRowPos, 2, false);	

	m_pcGrid->SetCellValue(l_nEditRowPos, 3, wxT("true"));	
}


bool SP_DLG_ColAuxiliaryVariable::LoadData()
{
	
	SP_DS_Metadataclass* l_pcMetadataclass;
	l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_AUXILIARVARIABLESCLASS);

	SP_DS_Metadata* l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("AuxiliaryVariableList")));		

	bool l_bWhite = false;

	if (m_pcGrid->GetNumberRows() > 0)
	{
		m_pcGrid->DeleteRows(0, m_pcGrid->GetNumberRows());
	}

	m_pcGrid->AppendRows(l_pcColList->GetRowCount());	
	
	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{		
		m_pcGrid->SetCellValue(i, 0, l_pcColList->GetCell(i,0));
		m_pcGrid->SetCellAlignment(i, 0, wxALIGN_LEFT, wxALIGN_CENTER);
		m_pcGrid->SetCellBackgroundColour(i, 0, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));

		m_pcGrid->SetCellValue(i, 1, l_pcColList->GetCell(i,1));
		m_pcGrid->SetCellAlignment(i, 1, wxALIGN_LEFT, wxALIGN_CENTER);
		m_pcGrid->SetCellBackgroundColour(i, 1, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));

		m_pcGrid->SetCellEditor(i,2,new wxGridCellChoiceEditor(WXSIZEOF(m_PREDEFINEDFUNCTIONS), m_PREDEFINEDFUNCTIONS));
		m_pcGrid->SetCellSize(i, 2, 1, 1);
		m_pcGrid->SetCellValue(i, 2, l_pcColList->GetCell(i,2));
		m_pcGrid->SetCellOverflow(i, 2, false);	

		m_pcGrid->SetCellValue(i, 3, l_pcColList->GetCell(i,3));
		m_pcGrid->SetCellAlignment(i, 3, wxALIGN_LEFT, wxALIGN_CENTER);
		m_pcGrid->SetCellBackgroundColour(i, 3, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));

		m_pcGrid->SetCellValue(i, 4, l_pcColList->GetCell(i,4));
		m_pcGrid->SetCellAlignment(i, 4, wxALIGN_LEFT, wxALIGN_CENTER);
		m_pcGrid->SetCellBackgroundColour(i, 4, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));

		(l_bWhite ? l_bWhite = false : l_bWhite = true);
	}
	
	return true;

}

bool SP_DLG_ColAuxiliaryVariable::SaveData()
{	
	m_pcGrid->SaveEditControlValue();

	SP_DS_Metadataclass* l_pcMetadataclass;
	l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_AUXILIARVARIABLESCLASS);

	SP_DS_Metadata* l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("AuxiliaryVariableList")));		

	l_pcColList->Clear();

	for (int i = 0; i < m_pcGrid->GetNumberRows(); i++)
	{
		int l_nRow=l_pcColList->AppendEmptyRow();
		l_pcColList->SetCell(l_nRow, 0, m_pcGrid->GetCellValue(i, 0));		
		l_pcColList->SetCell(l_nRow, 1, m_pcGrid->GetCellValue(i, 1));		
		l_pcColList->SetCell(l_nRow, 2, m_pcGrid->GetCellValue(i, 2));
		l_pcColList->SetCell(l_nRow, 3, m_pcGrid->GetCellValue(i, 3));	
		l_pcColList->SetCell(l_nRow, 4, m_pcGrid->GetCellValue(i, 4));			
	}
	return true;

}


void SP_DLG_ColAuxiliaryVariable::OnCellValueChanged(wxGridEvent& p_gEvent)
{
//    int l_nRow = p_gEvent.GetRow();
//    int l_nCol = p_gEvent.GetCol();
	

    p_gEvent.Skip();
}


void SP_DLG_ColAuxiliaryVariable::OnEditorShown( wxGridEvent& ev )
{
	if( ev.GetCol() == 1  )	
	{		 	
		SP_DLG_ColColoredPlaceSelection l_cDlg(m_ArrayColoredPlaceNames,NULL);
			if ( l_cDlg.ShowModal() == wxID_OK )
			{
				wxArrayString l_ArrayString = l_cDlg.GetStrings();
				wxString l_sComponents;
				for( unsigned i = 0; i < l_ArrayString.size(); i++)
				{
					if(i == 0)
						l_sComponents = l_ArrayString[i];
					else
						l_sComponents = l_sComponents + wxT(",")+ l_ArrayString[i];
				}
				m_pcGrid->SetCellValue(ev.GetRow(),ev.GetCol(),l_sComponents);
			}
			l_cDlg.Destroy();
		
	}

    ev.Skip();
}




bool SP_DLG_ColAuxiliaryVariable::Initialize()
{
	m_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	if(!m_pcGraph)
		return false;	
	
	SP_DS_Nodeclass* l_pcNodeclass;
	SP_ListNode::const_iterator l_itElem;	

	int l_nColPlSize = 0;
	l_pcNodeclass= m_pcGraph->GetNodeclass( SP_DS_CONTINUOUS_PLACE );
	if(l_pcNodeclass)
	{
		l_nColPlSize = l_pcNodeclass->GetElements()->size();
	}
	l_pcNodeclass= m_pcGraph->GetNodeclass( SP_DS_DISCRETE_PLACE );
	if(l_pcNodeclass)
	{
		l_nColPlSize = l_nColPlSize + l_pcNodeclass->GetElements()->size();
	}
	
	m_ArrayColoredPlaceNames.Alloc(l_nColPlSize);
	m_ArrayColoredPlaceNames.Empty();

	l_pcNodeclass= m_pcGraph->GetNodeclass( SP_DS_CONTINUOUS_PLACE );
	if(l_pcNodeclass)
	{
		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{
			SP_DS_Node* l_pcNode = *l_itElem;	
			wxString l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
			m_ArrayColoredPlaceNames.Add(l_sPlaceName);
			
			SP_DS_TextAttribute* l_pcNameAttibute = dynamic_cast< SP_DS_TextAttribute* >(l_pcNode->GetAttribute(SP_DS_CPN_COLORSETNAME) );
			if( !l_pcNameAttibute)
				return false;
			wxString l_sColorSetName = l_pcNameAttibute->GetValue();
			m_msColPlName2ColSet[l_sPlaceName] = l_sColorSetName;
		}
	}

	l_pcNodeclass= m_pcGraph->GetNodeclass( SP_DS_DISCRETE_PLACE );
	if(l_pcNodeclass)
	{
		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{
			SP_DS_Node* l_pcNode = *l_itElem;	
			wxString l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
			m_ArrayColoredPlaceNames.Add(l_sPlaceName);
			
			SP_DS_TextAttribute* l_pcNameAttibute = dynamic_cast< SP_DS_TextAttribute* >(l_pcNode->GetAttribute(SP_DS_CPN_COLORSETNAME) );
			if( !l_pcNameAttibute)
				return false;
			wxString l_sColorSetName = l_pcNameAttibute->GetValue();
			m_msColPlName2ColSet[l_sPlaceName] = l_sColorSetName;
		}
	}

	if( !m_ValueAssign.InitializeColorset(m_ColorSetClass) )
		return false;	

	return true;
}




void SP_DLG_ColAuxiliaryVariable::OnAssistent(wxCommandEvent& p_cEvent)
{
	if (m_pcGrid->GetNumberRows() == 0)
	{
		return;
	}

	int l_nEditRowPos = m_pcGrid->GetGridCursorRow();

	wxString l_pcReturnText = m_pcGrid->GetCellValue(l_nEditRowPos, 3);


	SP_DLG_ExpressionAssistent* l_pcDlg = new SP_DLG_ExpressionAssistent(EXPRESSION_TRACEANALYSIS, wxT(""), wxT("Place"), m_ColorSetClass,l_pcReturnText, this);

	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		m_pcGrid->SetCellValue(l_nEditRowPos, 3, l_pcDlg->GetReturnText());
	}

	l_pcDlg->Destroy();

}

void SP_DLG_ColAuxiliaryVariable::OnCheck( wxCommandEvent& p_cEvent )
{
	if( m_pcGrid->GetNumberRows() == 0 )
		return;

	for(int l_nRow = 0; l_nRow < m_pcGrid->GetNumberRows(); l_nRow++)
	{
		//variable
		m_sVarName = m_pcGrid->GetCellValue(l_nRow, 0);
		if(m_sVarName == wxT(""))
		{
			wxString l_sError = wxT("The variable name should not be empty.");
			SP_MESSAGEBOX(l_sError, wxT("Auxiliary variable definition"), wxOK | wxICON_ERROR);
			return;
		}

		//function
		m_sCurFunction = m_pcGrid->GetCellValue(l_nRow, 2);		

		//predicate
		m_sCurPredicate = m_pcGrid->GetCellValue(l_nRow, 3);
		GetPredicateType(m_sCurPredicate);
		//selected places
		wxString l_sSelColPlaceName = m_pcGrid->GetCellValue(l_nRow, 1);		
		if( !ProcessSelPlaces(l_sSelColPlaceName))
			return;
		

		if(m_sCurPredicate == wxT(""))
		{
			wxString l_sError = wxT("Not define a predicate.");
			SP_MESSAGEBOX(l_sError, wxT("Auxiliary variable definition"), wxOK | wxICON_ERROR);
			return;
		}

		if( !ProcessPredicate() )
			return;
	}

	new wxTipWindow(this, wxT("The definitions are correct."), 1000);
}

SP_ColPN_NeighborDim SP_DLG_ColAuxiliaryVariable::GetPredicateType(wxString p_sPredicate)
{
	if( p_sPredicate.Find(wxT("Neighbor1D")) != wxNOT_FOUND)
		return NEIGHBOR1D;
	else if( p_sPredicate.Find(wxT("Neighbor2D"))!= wxNOT_FOUND)
		return NEIGHBOR2D;
	else
		return NONEFUNCTION;
}


bool SP_DLG_ColAuxiliaryVariable::ProcessSelPlaces(wxString p_sSelColPlaceName)
{
	if(p_sSelColPlaceName == wxT(""))
	{
		wxString l_sError = wxT("The selected colored places should not be empty.");
		SP_MESSAGEBOX(l_sError, wxT("Auxiliary variable definition"), wxOK | wxICON_ERROR);
		return false;
	}
	
	//to get colored places
	vector<wxString> l_vColoredPlName;
	while(p_sSelColPlaceName.Find(wxT(",")) != wxNOT_FOUND)
	{
		wxString l_sPlName = p_sSelColPlaceName.BeforeFirst(wxT(','));
		l_vColoredPlName.push_back(l_sPlName);
		p_sSelColPlaceName = p_sSelColPlaceName.AfterFirst(wxT(','));		
	}
	l_vColoredPlName.push_back(p_sSelColPlaceName);

	//to check if these places have an identical color set	
	wxString l_sFirstColSet;
	for(unsigned i=0; i < l_vColoredPlName.size(); i++)
	{
		wxString l_sColPlName = l_vColoredPlName[i];
		wxString l_sColSet = m_msColPlName2ColSet[l_sColPlName];
		if(i==0)
		{
			l_sFirstColSet = l_sColSet;
		}
		if(l_sFirstColSet != l_sColSet)
		{
			wxString l_sError = wxT("The selected colored places should have an identical color set.");
			SP_MESSAGEBOX(l_sError, wxT("Auxiliary variable definition"), wxOK | wxICON_ERROR);
			return false;
		}
	}

	m_sSelColorSet = l_sFirstColSet;

	SP_CPN_ColorSet* l_pcColorSet =  m_ColorSetClass.LookupColorSet(m_sSelColorSet);
	if( ! l_pcColorSet )
		return false;

	SP_CPN_DATATYPE l_DataType = l_pcColorSet->GetDataType();

	if(m_PredType == NEIGHBOR1D && l_DataType != CPN_INTEGER)
	{
		wxString l_sError = wxT("The color set of selected places mismatches the function Neighbor(x)");
		SP_MESSAGEBOX(l_sError, wxT("Auxiliary variable definition"), wxOK | wxICON_ERROR);
		return false;
	}
	if(m_PredType == NEIGHBOR1D && l_DataType == CPN_INTEGER)
	{
		unsigned int l_nSize = l_pcColorSet->GetStringValue().size();
		if(l_nSize == 0)
			m_n1Dimenstion = 0;
		else
		{
			wxString l_sLast = (l_pcColorSet->GetStringValue())[l_nSize-1];
			l_sLast.ToLong(&m_n1Dimenstion);
		}
			 //take out of the last value
	}

	if(m_PredType == NEIGHBOR2D && l_DataType != CPN_PRODUCT)
	{
		//also we can further test its subcomponents
		wxString l_sError = wxT("The color set of selected places mismatches the function Neighbor(x,y)");
		SP_MESSAGEBOX(l_sError, wxT("Auxiliary variable definition"), wxOK | wxICON_ERROR);
		return false;
	}
	if(m_PredType == NEIGHBOR2D && l_DataType == CPN_PRODUCT)
	{
		map<wxString, SP_CPN_ColorSet*> l_mCSName2Pointer;
		l_mCSName2Pointer = l_pcColorSet->GetComponentMap();
		if( l_mCSName2Pointer.size() != 2 )
			return false;
		map<wxString, SP_CPN_ColorSet*>::iterator itMap;
		itMap = l_mCSName2Pointer.begin();
		vector<wxString> l_sColors1 = itMap->second->GetStringValue();
		if(l_sColors1.size() ==0)
			return false;
		wxString l_sLast1 = l_sColors1[l_sColors1.size()-1];
		l_sLast1.ToLong(&m_n1Dimenstion);
		itMap++;
		vector<wxString> l_sColors2 =itMap->second->GetStringValue();
		if(l_sColors2.size() ==0)
			return false;
		wxString l_sLast2 = l_sColors2[l_sColors1.size()-1];
		l_sLast2.ToLong(&m_n2Dimenstion);
	}

	return true;
}

bool SP_DLG_ColAuxiliaryVariable::ProcessPredicate()
{
	wxString p_sPredicate = m_sCurPredicate;
	m_pcParseContext = make_CPN_Parse_Context();
	m_pcDriver = make_CPN_Driver((*m_pcParseContext));

	//Set the ColorSetClass and colorset name for the parsing context	
	m_pcParseContext->SetErrorPosition(p_sPredicate);
	m_pcParseContext->SetColorSetClass(&m_ColorSetClass);	
	m_pcParseContext->SetColorSetName(wxT(""));	
	m_pcParseContext->SetFunctionFlag(false);
	//m_pcParseContext->SetNetClassName( wxT("") );
	m_pcParseContext->SetPlaceType(wxT("Place"));
	

	std::string l_ParseString;
	l_ParseString = string( p_sPredicate.mb_str());
	bool l_bSuccess = m_pcDriver->parse_string(l_ParseString, "input");		

	if(!l_bSuccess)
	{
		wxString l_sParseError = m_pcDriver->GetError();
		wxString l_sError = l_sParseError +  wxT(" | ") + p_sPredicate;		
		SP_MESSAGEBOX(l_sError, wxT("Auxiliary variable definition"), wxOK | wxICON_ERROR);
		return false;
	}

	// call the check function
	if( !m_pcParseContext->CheckExpression())
		return false;
	
	if( m_pcParseContext->GetHighLevelNodeDataType() != CPN_BOOLEAN )
	{
		wxString l_sError = wxT("predicate should be a boolean expression. ") ;
		SP_MESSAGEBOX(l_sError, wxT("Auxiliary variable definition"), wxOK | wxICON_ERROR);
		return false;
	}	

	return true;
}
