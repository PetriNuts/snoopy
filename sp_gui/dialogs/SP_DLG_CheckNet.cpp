//////////////////////////////////////////////////////////////////////
// $Author: liu $
// $Version: 0.1 $
// $Date: 2010/08/17 $
// Short Description: dialog to check a net
//////////////////////////////////////////////////////////////////////
#include "sp_gui/dialogs/SP_DLG_CheckNet.h"

#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/extensions/SP_DS_Coarse.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gui/mdi/SP_MDI_CoarseDoc.h"
#include "sp_gui/mdi/SP_MDI_View.h"
#include "sp_ds/attributes/SP_DS_IdAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_core/base/SP_Graphic.h"
#include "sp_core/SP_Core.h"

#include <wx/spinctrl.h>
#include <wx/busyinfo.h>

#include "spsim/helpers/parser.h"
#include "spsim/helpers/utilities.h"

#include "sp_ds/extensions/Col_PN/SyntaxChecking/SP_CPN_SyntaxChecking.h"
#include "sp_ds/extensions/SP_DS_Transformer.h"
#include "sp_gui/dialogs/SP_DLG_Simulation.h"

IMPLEMENT_CLASS(SP_DLG_CheckNet, wxDialog)

enum
{

	SP_ID_CHECKLISTBOX_DUPLICATE,
	SP_ID_CHECKLISTBOX_SQUEESE,
	SP_ID_CHECKLISTBOX_SYNTAX,
	SP_ID_CHECKLISTBOX_INTEGRITY,
	SP_ID_CHECKLISTBOX_POSITION
	

};


BEGIN_EVENT_TABLE(SP_DLG_CheckNet, wxDialog)
	EVT_BUTTON(wxID_OK, SP_DLG_CheckNet::OnDlgOk)
	EVT_BUTTON(wxID_CLOSE, SP_DLG_CheckNet::OnDlgClose)
	EVT_CHECKBOX(SP_ID_CHECKLISTBOX_INTEGRITY, SP_DLG_CheckNet::OnCKBoxChange)
END_EVENT_TABLE()

SP_DLG_CheckNet::SP_DLG_CheckNet( SP_MDI_View* p_pcView, wxWindow *p_parent, const wxString& p_title) :
	wxDialog(p_parent, -1, p_title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxSTAY_ON_TOP | wxRESIZE_BORDER),
	m_nodeList(NULL)
{
	m_sNetClassName = SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNetclass()->GetName();

	m_pcView = p_pcView;

	wxSizer* l_pcSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer *elemsizer = new wxBoxSizer(wxVERTICAL);

	m_cbIntegrity = new wxCheckBox(this, SP_ID_CHECKLISTBOX_INTEGRITY, wxT("Check Integrity"));
	m_cbIntegrity->SetValue( 1 );
	elemsizer->Add(m_cbIntegrity, 0, wxALL, 10);

	m_cbPosition = new wxCheckBox(this, SP_ID_CHECKLISTBOX_POSITION, wxT("Repositioning Attributes"));
	m_cbPosition->SetValue( 0 );
	elemsizer->Add(m_cbPosition, 0, wxALL, 10);

	m_cbDuplicate = new wxCheckBox(this, SP_ID_CHECKLISTBOX_DUPLICATE, wxT("Check Duplicate Nodes"));
	m_cbDuplicate->SetValue( 1 );
	elemsizer->Add(m_cbDuplicate, 0, wxALL, 10);

	bool m_bHasIdAttribute = SP_Core::Instance()->GetRootDocument()->GetGraph()->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID);
	if( m_bHasIdAttribute )
	{
		m_cbSqueeze = new wxCheckBox(this, SP_ID_CHECKLISTBOX_SQUEESE, wxT("Squeeze Numbers"));
		m_cbSqueeze->SetValue( 1 );
		elemsizer->Add(m_cbSqueeze, 0, wxALL, 10);
	}	

	if( m_sNetClassName == SP_DS_COLEPN_CLASS 
		|| m_sNetClassName == SP_DS_COLSPN_CLASS 
		|| m_sNetClassName == SP_DS_COLCPN_CLASS 
		|| m_sNetClassName == SP_DS_COLPN_CLASS 
		|| m_sNetClassName == SP_DS_COLHPN_CLASS 
		|| m_sNetClassName == SP_DS_CONTINUOUSPED_CLASS
		|| m_sNetClassName == SP_DS_HYBRIDPN_CLASS
		|| m_sNetClassName == SP_DS_SPN_CLASS)
	{
		m_cbSyntax = new wxCheckBox(this, SP_ID_CHECKLISTBOX_SYNTAX, wxT("Check Syntax (Consistency)"));
		m_cbSyntax->SetValue( 1 );
		elemsizer->Add(m_cbSyntax, 0, wxALL, 10);
	}

	l_pcSizer->Add(elemsizer);

    /* Buttons in the lower right hand corner */
    wxSizer *l_pcButtonSizer = new wxBoxSizer( wxHORIZONTAL );
	l_pcButtonSizer->Add(this->CreateButtonSizer(wxOK|wxCANCEL), 0, wxALL, 5);

    l_pcSizer->Add(l_pcButtonSizer, 0, wxALIGN_RIGHT);

	SetSizerAndFit(l_pcSizer);

	Centre(wxBOTH);
	
}

SP_DLG_CheckNet::~SP_DLG_CheckNet()
{
}

void SP_DLG_CheckNet::OnDlgClose(wxCommandEvent& p_cEvent)
{
	SetReturnCode(wxID_CANCEL);
	this->Show( FALSE);
}


void SP_DLG_CheckNet::CheckDuplicateNodes()

{	
	m_graph = SP_Core::Instance()->GetRootDocument()->GetGraph();

	bool l_bUniqueNodes = true;
	map<wxString, SP_Data*> l_mpNodes;

	for (SP_DS_Nodeclass* nc : *(m_graph->GetNodeclasses()))
	{
		if (nc->GetShowInElementTree())
		{
			if (nc->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID) && nc->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))
			{
				for(SP_DS_Node* node : *(nc->GetElements()))
				{
					wxString l_sNodeclass = nc->GetDisplayName();
					wxString l_sName = node->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME)->GetValueString();
					if(l_sName.IsEmpty())
					{

						wxString l_sMessage = l_sNodeclass + wxT(" with ID=")
								+ node->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID)->GetValueString()
								+ wxT(" has no name!");
						SP_LOGWARNING( l_sMessage);
					}
					else
					{
						if(l_mpNodes[l_sName])
						{
							wxString l_sMessage = l_sNodeclass + wxT(" ") + l_sName + wxT(" is not unique!");
							SP_LOGWARNING( l_sMessage);
							l_bUniqueNodes = false;
						}
						l_mpNodes[l_sName] = node;
					}
				}
			}
		}
	}	

	for (SP_DS_Metadataclass* mc : *(m_graph->GetMetadataclasses()))
	{
		if (mc->GetShowInDeclarationTreeOther()
			|| mc->GetShowInDeclarationTreeColorSet())
		{
			if (mc->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID) && mc->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))
			{
				for(SP_DS_Metadata* meta : *(mc->GetElements()))
				{
					wxString l_sMetaclass = mc->GetDisplayName();
					wxString l_sName = meta->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME)->GetValueString();
					if(l_sName.IsEmpty())
					{

						wxString l_sMessage = l_sMetaclass + wxT(" with ID=")
								+ meta->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID)->GetValueString()
								+ wxT(" has no name!");
						SP_LOGWARNING( l_sMessage);
					}
					else
					{
						if(l_mpNodes[l_sName])
						{
							wxString l_sMessage = l_sMetaclass + wxT(" ") + l_sName + wxT(" is not unique!");
							SP_LOGWARNING( l_sMessage);
							l_bUniqueNodes = false;
						}
						l_mpNodes[l_sName] = meta;
					}
				}
			}
		}
	}

	if (l_bUniqueNodes)
	{
		wxString l_sError = wxT(" All Nodes have unique names.\n");
		SP_LOGMESSAGE(l_sError);		
	}
	else
	{
		wxString l_sError = wxT(" Not all Nodes have unique names!\n\nSee log (Alt+3) for more details!\n");
		SP_LOGERROR(l_sError);		
		SP_DS_Transformer transform;
		transform.CheckUniqueNames(m_graph);
	}

}

void SP_DLG_CheckNet::Squeeze()
{
	SP_MDI_Doc* l_pcDoc = SP_Core::Instance()->GetRootDocument();
	if (l_pcDoc && l_pcDoc->GetGraph())
	{
		l_pcDoc->GetGraph()->SqueezeIdAttributes();
		m_pcView->Modify(TRUE);
		m_pcView->Refresh(TRUE);
	}

	wxString l_sError = wxT(" The net has been squeezed.\n");
	SP_LOGMESSAGE(l_sError);	
}



bool SP_DLG_CheckNet::Check()
{
	wxString l_sNotice;
	l_sNotice = wxT(" The check results for this net are as follows: \n");
	SP_LOGMESSAGE(l_sNotice);

	if(m_cbDuplicate->IsChecked())
	{
		CheckDuplicateNodes();
	}

	
	if(m_cbSqueeze && m_cbSqueeze->IsChecked())
	{
		Squeeze();
	}

	bool l_bCheck = false;

	if(m_cbIntegrity && m_cbIntegrity->IsChecked())
	{
		double saveDelta = wxGetApp().getDelta();
		if(m_cbPosition->IsEnabled() && m_cbPosition->IsChecked())
		{
			wxGetApp().setDelta(20);
		}
		l_bCheck = m_graph->CheckIntegrity();
		if(m_cbPosition->IsEnabled() && m_cbPosition->IsChecked())
		{
			wxGetApp().setDelta(saveDelta);
		}
	}


	if( m_sNetClassName == SP_DS_COLEPN_CLASS 
		|| m_sNetClassName == SP_DS_COLSPN_CLASS 
		|| m_sNetClassName == SP_DS_COLPN_CLASS 
		|| m_sNetClassName == SP_DS_COLHPN_CLASS 
		|| m_sNetClassName == SP_DS_COLCPN_CLASS)
	{
		if(m_cbSyntax->IsChecked())
		{
			SP_CPN_SyntaxChecking l_cSyntaxChecking;
			l_bCheck = l_cSyntaxChecking.SyntaxChecking();
		}
	}

	if( m_sNetClassName == SP_DS_SPN_CLASS
		|| m_sNetClassName == SP_DS_HYBRIDPN_CLASS
		|| m_sNetClassName == SP_DS_CONTINUOUSPED_CLASS)
	{
		if(m_cbSyntax->IsChecked())
		{
			 l_bCheck = CheckSPN();
		}
	}

	if( l_bCheck )
	{
		l_sNotice = wxT(" No errors found. \n");
		SP_LOGMESSAGE(l_sNotice);
	}
	else
	{
		l_sNotice = wxT(" Errors found. \n");
		SP_LOGERROR(l_sNotice);
	}

	return true;
}



void SP_DLG_CheckNet::OnDlgOk(wxCommandEvent& p_cEvent)
{
    wxBusyInfo wait("Please wait, checking net...");

	if (Validate() && TransferDataFromWindow())
	{

		if (IsModal())
		{

			if (Check())
			{
			}

			EndModal(wxID_OK);
		}
		else
		{

			SetReturnCode(wxID_OK);
			this->Show(FALSE);

		}

	}

}


bool SP_DLG_CheckNet :: CheckSPN( )
{
	m_graph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	SP_DS_Nodeclass* l_pcNodeclass;	
	SP_ListNode::const_iterator l_itElem;
	SP_DS_ColListAttribute* l_pcColList;
	wxString l_sFormula = wxT("");

	bool l_bNoError = true;
	
	spsim::Parser l_cParser;
	SP_VectorString l_Constants;

	SP_DS_Metadataclass* mc = m_graph->GetMetadataclass(SP_DS_META_CONSTANT);
	if(mc)
	{
		for(SP_DS_Metadata* l_pcMeta : *(mc->GetElements()))
		{
			wxString l_Name = dynamic_cast<SP_DS_NameAttribute*>(l_pcMeta->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
			l_Constants.push_back(l_Name);
		}
		l_cParser.SetParameterNames(&l_Constants);
	}

	l_pcNodeclass = m_graph->GetNodeclass(SP_DS_STOCHASTIC_TRANS);
	if( l_pcNodeclass )
	{
		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{

			SP_DS_Node* l_pcNode = *l_itElem;
			l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcNode->GetAttribute(wxT("FunctionList")) );

			for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
			{
				l_sFormula = l_pcColList->GetCell(i, 1);
				if( ! CheckRateFunction(l_cParser, l_sFormula,l_pcNode) )
					l_bNoError = false;
			}
		}
	}

	l_pcNodeclass = m_graph->GetNodeclass(SP_DS_CONTINUOUS_TRANS);
	if( l_pcNodeclass )
	{
		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{
			
			SP_DS_Node* l_pcNode = *l_itElem;			
			l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcNode->GetAttribute(wxT("FunctionList")) );

			for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
			{
				l_sFormula = l_pcColList->GetCell(i, 1);
				if( ! CheckRateFunction(l_cParser, l_sFormula,l_pcNode) )
					l_bNoError = false;
			}
		}
	}

	l_pcNodeclass = m_graph->GetNodeclass(SP_DS_IMMEDIATE_TRANS);
	if( l_pcNodeclass )
	{
		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{
			
			SP_DS_Node* l_pcNode = *l_itElem;			
			l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcNode->GetAttribute(wxT("FunctionList")) );

			for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
			{
				l_sFormula = wxT("ImmediateFiring(") + l_pcColList->GetCell(i, 1) + wxT(")");
				if( ! CheckRateFunction(l_cParser, l_sFormula,l_pcNode) )
					l_bNoError = false;
			}
		}

	}

	l_pcNodeclass= m_graph->GetNodeclass(SP_DS_DETERMINISTIC_TRANS);
	if( l_pcNodeclass )
	{
		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{
			
			SP_DS_Node* l_pcNode = *l_itElem;
			l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcNode->GetAttribute(wxT("DelayList")) );

			for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
			{
				l_sFormula = wxT("TimedFiring(")+ l_pcColList->GetCell(i, 1) +wxT(")");
				if( ! CheckRateFunction(l_cParser, l_sFormula,l_pcNode) )
					l_bNoError = false;
			}
		}

	}

	l_pcNodeclass= m_graph->GetNodeclass(SP_DS_SCHEDULED_TRANS);
	if( l_pcNodeclass )
	{
		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{
			
			SP_DS_Node* l_pcNode = *l_itElem;			
			l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcNode->GetAttribute(wxT("PeriodicList")) );

			for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
			{
				wxString l_Begin=l_pcColList->GetCell(i, 1);
				wxString l_Repetition=l_pcColList->GetCell(i, 2); 
				wxString l_End=l_pcColList->GetCell(i, 3);
				l_sFormula = wxT("FixedTimedFiring_Periodic(")+ l_Begin +wxT(",")+ l_Repetition+wxT(",")+l_End+wxT(")");
				if( ! CheckRateFunction(l_cParser, l_sFormula,l_pcNode) )
					l_bNoError = false;
			}
		}
	}

	return l_bNoError;
}

bool SP_DLG_CheckNet::CheckRateFunction(spsim::Parser& p_Parser, const wxString& p_sFormula, SP_DS_Node* p_pcNode )
{
	SP_VectorString l_Places;
    if(p_pcNode)
	{
    	wxString l_sTransitionName = dynamic_cast<SP_DS_NameAttribute*>(p_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		for(SP_DS_Edge* l_pcEdge : *(p_pcNode->GetTargetEdges()))
		{
			SP_DS_Node* l_pcPlace = dynamic_cast<SP_DS_Node*>(l_pcEdge->GetSource());
			wxString l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>(l_pcPlace->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
			if(SP_Find(l_Places, l_sPlaceName) == l_Places.end())
				l_Places.push_back(l_sPlaceName);
		}
		p_Parser.SetPlaceNames(&l_Places);
	}

	return p_Parser.Parse(p_sFormula, static_cast<spsim::Transition*>(NULL));
}

void SP_DLG_CheckNet::OnCKBoxChange(wxCommandEvent& p_cEvent)
{

	if( m_cbIntegrity->GetValue())
	{
		m_cbPosition->Enable();
	}
	else
	{
		m_cbPosition->Disable();
	}

}
