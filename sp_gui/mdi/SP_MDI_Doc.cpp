//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_gui/mdi/SP_MDI_View.h"
#include "sp_gui/management/SP_GM_DocTemplate.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "sp_gui/dialogs/SP_DLG_SearchId.h"
#include "sp_gui/dialogs/SP_DLG_ReduceFTree.h"
#include "sp_gui/dialogs/SP_DLG_ChooseInvariant.h"
#include "sp_gui/windows/SP_GUI_Mainframe.h"
#include "sp_gui/windows/SP_GUI_LogWindow.h"
#include "sp_gui/mdi/SP_MDI_CoarseDoc.h"
#include "sp_core/SP_Core.h"
#include "sp_core/SP_GPR_Canvas.h"
#include "sp_core/tools/SP_StopWatch.h"
#include "sp_utilities.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_core/SP_Signer.h"
#include <wx/file.h>
#include <wx/regex.h>
#include <wx/busyinfo.h>

#include "snoopy.h"
#include "sp_gui/interaction/SP_IA_Manager.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"

#include "sp_gui/windows/SP_GUI_DevbarContainer.h"

#include "sp_ds/extensions/SP_DS_Transformer.h"


//by george for constants harmonizing
#include "sp_ds/extensions/SP_DS_FunctionRegistry.h"
#include "sp_ds/extensions/SP_DS_FunctionEvaluator.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"

namespace {
// panel with custom controls for file dialog
class SP_CompressPanel : public wxPanel
{
public:
	SP_CompressPanel(wxWindow *parent);
    bool GetCompress() const
    {
        return m_cb->GetValue();
    }

private:
    wxCheckBox *m_cb;
};

SP_CompressPanel::SP_CompressPanel(wxWindow *parent)
            : wxPanel(parent)
{
    m_cb = new wxCheckBox(this, -1, wxT("Compress File"));
    if(wxGetApp().GetCanvasPrefs()->GetCompressFile())
    {
        m_cb->SetValue(SP_Core::Instance()->GetRootDocument()->GetCompress());
    }
    else
    {
        m_cb->SetValue(false);
        m_cb->Disable();
    }
    wxBoxSizer *sizerTop = new wxBoxSizer(wxHORIZONTAL);
    sizerTop->Add(m_cb, wxSizerFlags().Centre().Border());

    SetSizerAndFit(sizerTop);
}

// a static method can be used instead of a function with most of compilers
static wxWindow* createCompressPanel(wxWindow *parent)
{
    return new SP_CompressPanel(parent);
}

}

IMPLEMENT_DYNAMIC_CLASS(SP_MDI_Doc, wxDocument)

SP_MDI_Doc::SP_MDI_Doc() :
	m_pcGraph(NULL), m_pcDiagram(this), m_pcGraphTreectrl(NULL),
			m_pcCoarseTreectrl(NULL),m_pcColorSetTreectrl(NULL), m_SearchIdDialog(NULL),
			m_ReduceFTDialog(NULL), m_ChooseInvariantDialog(NULL),
			m_eType(SP_DOC_SIMPLE), m_bClose(TRUE), m_pcParentDoc(NULL),
			m_sNetclassName(wxT("")), m_bCompress(wxGetApp().GetCanvasPrefs()->GetCompressFile())
{
}

SP_MDI_Doc::~SP_MDI_Doc()
{
	if(m_pcGraph)//by george
	wxDELETE(m_pcGraph);
}

SP_DS_Graph* SP_MDI_Doc::InitGraph(SP_DS_Netclass* p_pcNetclass)
{
	SP_LOGDEBUG(wxT("InitGraph"));

	if (!p_pcNetclass)
		return NULL;

	m_pcParentDoc = this;
	m_sNetclassName = p_pcNetclass->GetName();

	SP_Core::Instance()->SetDocumentReactivatingFlag( true);
	SP_Core::Instance()->ManageDocChange(this);

	if (m_pcGraph)
		return m_pcGraph;

	m_pcGraph = new SP_DS_Graph(p_pcNetclass);
	// new graph means new coarse tree
	m_pcCoarseTreectrl = m_pcGraph->CreateCoarseTree();
	
	//if(m_pcGraph->GetNetclass()->GetName()==SP_DS_COLEPN_CLASS
	//	||m_pcGraph->GetNetclass()->GetName()==SP_DS_COLSPN_CLASS)
	{
		wxGetApp().GetMainframe()->GetDeclarationContainer()->Show(true);	
		m_pcColorSetTreectrl = m_pcGraph->CreateDeclarationTree();//liu
	}	

	SP_MDI_View* l_pcView = dynamic_cast<SP_MDI_View*>(GetFirstView());
	if (l_pcView && l_pcView->GetFrame())
	{
		l_pcView->GetFrame()->SetCoarseTreectrl(m_pcCoarseTreectrl);

		//if(m_pcGraph->GetNetclass()->GetName()==SP_DS_COLEPN_CLASS
		//   ||m_pcGraph->GetNetclass()->GetName()==SP_DS_COLSPN_CLASS)
		{
			l_pcView->GetFrame()->SetColorSetTreectrl(m_pcColorSetTreectrl); 
		}

		l_pcView->GetFrame()->AddGraphToMenu(m_pcGraph);	
		
	}

	if (m_pcGraphTreectrl)
		m_pcGraphTreectrl->Initialise(m_pcGraph);

	if (m_pcColorSetTreectrl)
		m_pcColorSetTreectrl->Initialise(m_pcGraph);

	SetNetnumber(m_pcGraph->GetNewNetnumber());
	DrawAllElements();
	return m_pcGraph;
}

unsigned int SP_MDI_Doc::GetNetnumber()
{
	SP_MDI_View* l_pcView = dynamic_cast<SP_MDI_View*>(GetFirstView());

	wxASSERT(l_pcView);

	if (l_pcView)
		return l_pcView->GetNetnumber();

	return 0;
}

void SP_MDI_Doc::SetNetnumber(unsigned int p_nVal)
{
	wxList& l_lViews = GetViews();
	wxNode* l_pcNode = l_lViews.GetFirst();
	while (l_pcNode)
	{
		SP_MDI_View* l_pcView = dynamic_cast<SP_MDI_View*>(l_pcNode->GetData());
		l_pcView->SetNetnumber(p_nVal);
		l_pcNode = l_pcNode->GetNext();
	}
}

bool SP_MDI_Doc::DrawAllElements(bool p_bLocalOnly, bool p_bIncludeChildDocs)
{
	if (!m_pcGraph)
		return FALSE;

	bool l_bReturn= TRUE;
	wxList& l_lViews = GetViews();
	wxNode* l_pcNode = l_lViews.GetFirst();
	while (l_pcNode)
	{
		SP_MDI_View* l_pcView = dynamic_cast<SP_MDI_View*>(l_pcNode->GetData());

		if (l_pcView->GetFrame() && (!p_bLocalOnly || l_pcView->GetFrame()->IsShown()))
			l_bReturn &= l_pcView->DrawAllElements(m_pcGraph, TRUE);

		l_pcNode = l_pcNode->GetNext();
	}

	if (!p_bIncludeChildDocs)
		return l_bReturn;

	SP_SetCoarseDoc::iterator l_Iter;
	for (l_Iter = m_tsChildDocs.begin(); l_Iter != m_tsChildDocs.end(); ++l_Iter)
		l_bReturn &= (*l_Iter)->DrawAllElements(false, true);
	//----------------------------------------------------------------------------------

	return l_bReturn;
}

void SP_MDI_Doc::Refresh()
{
	if (m_pcParentDoc)
		if (m_pcParentDoc != this)
		{
			m_pcParentDoc->Refresh();
			return;
		}

	SP_SetCoarseDoc::iterator l_Iter;
	for (l_Iter = m_tsChildDocs.begin(); l_Iter != m_tsChildDocs.end(); ++l_Iter)
		(*l_Iter)->RefreshAllViews();

	RefreshAllViews();
}

void SP_MDI_Doc::RefreshAllViews()
{
	wxList& l_lViews = GetViews();
	wxNode* l_pcNode = l_lViews.GetFirst();
	while (l_pcNode)
	{
		SP_MDI_View* l_pcView = dynamic_cast<SP_MDI_View*>(l_pcNode->GetData());

		if (l_pcView)
			l_pcView->Refresh();

		l_pcNode = l_pcNode->GetNext();
	}
}

bool SP_MDI_Doc::ShowAll()
{
	bool l_bReturn= TRUE;
	wxList& l_lViews = GetViews();
	wxNode* l_pcNode = l_lViews.GetFirst();
	while (l_pcNode)
	{
		SP_MDI_View* l_pcView = dynamic_cast<SP_MDI_View*>(l_pcNode->GetData());
		l_bReturn &= l_pcView->ShowFrame();
		l_pcNode = l_pcNode->GetNext();
	}
	return l_bReturn;
}

bool SP_MDI_Doc::SetEditElement(SP_Type* p_pcElement)
{
	bool l_bReturn= TRUE;
	wxList& l_lViews = GetViews();
	wxNode* l_pcNode = l_lViews.GetFirst();
	while (l_pcNode)
	{
		SP_MDI_View* l_pcView = dynamic_cast<SP_MDI_View*>(l_pcNode->GetData());

		l_bReturn &= l_pcView->SetEditElement(p_pcElement);

		l_pcNode = l_pcNode->GetNext();
	}
	return l_bReturn;
}

bool SP_MDI_Doc::SetClose(bool p_bVal)
{
	bool l_bReturn= TRUE;

	m_bClose = p_bVal;

	wxNode* l_pcNode = GetViews().GetFirst();
	while (l_pcNode)
	{
		SP_MDI_View* l_pcView = dynamic_cast<SP_MDI_View*>(l_pcNode->GetData());
		if(l_pcView)
			l_bReturn &= l_pcView->SetClose(m_bClose);
		l_pcNode = l_pcNode->GetNext();
	}

	return l_bReturn;
}

void SP_MDI_Doc::Modify(bool p_bVal)
{
	SP_SetCoarseDoc::iterator l_Iter;
	for (l_Iter = m_tsChildDocs.begin(); l_Iter != m_tsChildDocs.end(); ++l_Iter)
		(*l_Iter)->Modify(p_bVal);

	m_documentModified = p_bVal;
}

void SP_MDI_Doc::DoSetModify(bool p_bVal, SP_MDI_Doc* p_pcSender)
{
	SP_SetCoarseDoc::iterator l_Iter;
	for (l_Iter = m_tsChildDocs.begin(); l_Iter != m_tsChildDocs.end(); ++l_Iter)
		if ((*l_Iter) != p_pcSender)
			(*l_Iter)->Modify(p_bVal);

	wxDocument::Modify(p_bVal);
}

void SP_MDI_Doc::SetFilename(const wxString& p_sName, bool p_bNotify)
{
	SP_SetCoarseDoc::iterator l_Iter;
	for (l_Iter = m_tsChildDocs.begin(); l_Iter != m_tsChildDocs.end(); ++l_Iter)
	{
		(*l_Iter)->SetFilename(p_sName, p_bNotify);
		(*l_Iter)->UpdateTitle();
	}

	wxFileName l_cFilename(p_sName);
	wxDocument::SetTitle(l_cFilename.GetFullName());
	wxDocument::SetFilename(p_sName, p_bNotify);

	SP_MDI_View* l_pcView = dynamic_cast<SP_MDI_View*>(GetFirstView());
	if (l_pcView && l_pcView->GetFrame())
	{
		l_pcView->GetFrame()->SetTitle(GetUserReadableName());
	}

	if (m_SearchIdDialog)
	{
		m_SearchIdDialog->SetTitlePrefix(GetTitle() + wxT(": "));
	}
}

bool SP_MDI_Doc::OnCloseDocument()
{
	if (wxGetApp().GetIAManager())
		wxGetApp().GetIAManager()->DisconnectNet(GetFilename(), true);

	SP_Core::Instance()->RemoveFromDocChange(this);

	return wxDocument::OnCloseDocument();
}

bool SP_MDI_Doc::SaveAs()
{
#if 1
    wxDocTemplate *docTemplate = GetDocumentTemplate();
    if (!docTemplate)
        return false;

    wxString filter = docTemplate->GetFileFilter() ;

    wxString defaultDir = docTemplate->GetDirectory();
    if ( defaultDir.empty() )
    {
        defaultDir = wxPathOnly(GetFilename());
        if ( defaultDir.empty() )
            defaultDir = GetDocumentManager()->GetLastDirectory();
    }

/*    wxString fileName = wxFileSelector(_("Save As"),
            defaultDir,
            wxFileNameFromPath(GetFilename()),
            docTemplate->GetDefaultExtension(),
            filter,
            wxFD_SAVE | wxFD_OVERWRITE_PROMPT,
            GetDocumentWindow());
*/
    wxFileDialog
            saveFileDialog(GetDocumentWindow(), _("Save As"),
            		defaultDir, wxFileNameFromPath(GetFilename()),
            		filter, wxFD_SAVE|wxFD_OVERWRITE_PROMPT);

    saveFileDialog.SetExtraControlCreator(&createCompressPanel);

    if (saveFileDialog.ShowModal() == wxID_CANCEL)
    	return false;     // the user changed idea...

    wxString fileName = saveFileDialog.GetPath();
    wxWindow* extra = saveFileDialog.GetExtraControl();
    if(extra)
    	SetCompress(static_cast<SP_CompressPanel*>(extra)->GetCompress());

    if (fileName.empty())
        return false; // cancelled by user

    // Files that were not saved correctly are not added to the FileHistory.
    if (!OnSaveDocument(fileName))
        return false;

    SetTitle(wxFileNameFromPath(fileName));
    SetFilename(fileName, true);    // will call OnChangeFileName automatically

    // A file that doesn't use the default extension of its document template
    // cannot be opened via the FileHistory, so we do not add it.
    if (docTemplate->FileMatchesTemplate(fileName))
    {
        GetDocumentManager()->AddFileToHistory(fileName);
    }
    else //: the user will probably not be able to open the file again, so we
    	//      could warn about the wrong file-extension here
    {
        SP_MESSAGEBOX( wxT("The extension name should not be changed while saving as. Please choose export if you want to change from one net class to another."),
        					wxT("Error while saving"), wxOK | wxICON_EXCLAMATION);

    }
    return true;

#else
	wxString l_sOldFileFilter = this->GetDocumentTemplate()->GetFileFilter();
	bool l_bReturn = wxDocument::SaveAs();
	if(l_bReturn)
	{
		SetFilename(GetFilename());

		wxFileName l_cFilename(GetFilename());
		wxString l_sExt = l_cFilename.GetExt();

		wxRegEx l_cRegEx(wxT("\\.") + l_sExt + wxT("\\W|$"), wxRE_ADVANCED);

		if( !l_cRegEx.Matches(l_sOldFileFilter) )
		{
			SP_MESSAGEBOX( wxT("The extension name should not be changed while saving as. Please choose export if you want to change from one net class to another."),
					wxT("Error while saving"), wxOK | wxICON_EXCLAMATION);
		}
	}
	return l_bReturn;
#endif
}

bool SP_MDI_Doc::OnSaveDocument(const wxString& p_sFile)
{
	if (m_pcGraph)
	{
		SP_StopWatch sw(wxT("Start saving file ") + p_sFile, wxT("Finished saving in"));

        wxBusyInfo wait("Please wait, saving...");

    	if (GetDiagram() && GetDiagram()->GetShapeList())
    	{
    		SP_ListGraphic l_lGraphics;
            SP_MDI_View* l_pcView = dynamic_cast<SP_MDI_View*>(GetFirstView());
        	wxNode *l_pcNode = GetDiagram()->GetShapeList()->GetFirst();
        	while (l_pcNode)
        	{
    			wxShape *l_pcShape = dynamic_cast<wxShape*>(l_pcNode->GetData());
    			if (l_pcShape && l_pcShape->GetParent() == NULL
    				&& !l_pcShape->IsKindOf(CLASSINFO(wxControlPoint))
					&& !l_pcShape->IsKindOf(CLASSINFO(wxLabelShape)))
    			{
    				SP_Graphic* l_pcGraphic = SP_Core::Instance()->ResolveExtern(l_pcShape);
					if (l_pcGraphic && l_pcGraphic->GetParent())
					{
						l_lGraphics.push_back(l_pcGraphic);
					}
    			}
        		l_pcNode = l_pcNode->GetNext();
        	}
        	l_pcView->DoUnHide(l_lGraphics);
    	}

		m_pcGraph->SqueezeIdAttributes();
		if (!m_pcGraph->OnSaveDocument(p_sFile))
		{
			SP_MESSAGEBOX(m_pcGraph->GetLastError(), wxT("Error while saving"),
					wxOK | wxICON_EXCLAMATION);
			return FALSE;
		}
		Modify(FALSE);
		m_savedYet = TRUE;
		SetFilename(p_sFile);
	}

	return TRUE;
}
void SP_MDI_Doc::HarmonizeConstantsForColPN()
{
	std::vector<wxString> l_vParamNames;
	size_t l_nConstantsCount = 0;
	SP_DS_Metadataclass* l_pcMetadataclass;
	l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_CONSTANTCLASS);

	SP_DS_Metadata* l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("ConstantList")));

	//initially assign all old constants to the default constants group ALL
	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		wxString l_sGroup = l_pcColList->GetCell(i, 4);
		if (l_sGroup.IsEmpty())
			l_pcColList->SetCell(i, 4, "all");
		else {
			l_pcColList->SetCell(i, 4, l_sGroup);
		}
		l_vParamNames.push_back(l_pcColList->GetCell(i, 0));
	}

 
	///////////////////////////////////////
	//get number of rows of each old param (number of value sets)
	std::vector<wxString> l_vSetsName;
	if (m_pcGraph->GetNetclass()->GetName() == SP_DS_COLSPN_CLASS || m_pcGraph->GetNetclass()->GetName() == SP_DS_COLHPN_CLASS || m_pcGraph->GetNetclass()->GetName() == SP_DS_COLCPN_CLASS)
	{
		SP_DS_ColListAttribute * l_pcColListOldParam1;
		SP_DS_Nodeclass* l_pcNodeclass1;
		l_pcNodeclass1 = m_pcGraph->GetNodeclass(SP_DS_PARAM);
		SP_ListNode::const_iterator l_itElem1;
		int l_nValueSetsCount = 0;
		
		for (l_itElem1 = l_pcNodeclass1->GetElements()->begin(); l_itElem1 != l_pcNodeclass1->GetElements()->end(); ++l_itElem1)
		{

		 
			l_pcColListOldParam1 = dynamic_cast<SP_DS_ColListAttribute*>((*l_itElem1)->GetAttribute(wxT("ParameterList")));
			l_nValueSetsCount = l_pcColListOldParam1->GetRowCount();
			for (unsigned int i = 1; i < l_pcColListOldParam1->GetRowCount(); i++)
			{
				l_vSetsName.push_back(l_pcColListOldParam1->GetCell(i, 0));
			}
			break;
		}
	}//end if
		////////////////////////////////////////
		//next step: copy the old constant list to the new DS
		SP_DS_Metadata* l_pcConstant;
		SP_DS_Metadataclass* m_pcConstants;

		m_pcConstants = m_pcGraph->GetMetadataclass(SP_DS_CPN_CONSTANT_HARMONIZING);
		SP_ListMetadata::const_iterator l_itElem;
		if (m_pcConstants->GetElements()->size() == 0)
		{
			while (m_pcConstants->GetElements()->size() < l_pcColList->GetRowCount())
			{
				m_pcConstants->NewElement(1);
			}

		
			l_itElem = m_pcConstants->GetElements()->begin();

			for (unsigned int l_nRow = 0; l_nRow < l_pcColList->GetRowCount(); l_nRow++)
			{
				l_nConstantsCount++;
				l_pcConstant = *l_itElem;
				 
				wxString l_sName = l_pcColList->GetCell(l_nRow, 0);
				wxString l_sGroup = l_pcColList->GetCell(l_nRow, 4);
				wxString l_sType = l_pcColList->GetCell(l_nRow, 1);
				wxString l_sComment = l_pcColList->GetCell(l_nRow, 3);
				wxString l_sMainVal = l_pcColList->GetCell(l_nRow, 2);

				if (l_sType != wxT("int"))
				{
					l_pcConstant->Remove();//remove the constant if it is not int, old versions only support constants of type int, otherwise it is wrong constant
					l_nConstantsCount--;
					std::vector<wxString>::iterator position = std::find(l_vParamNames.begin(), l_vParamNames.end(), l_sName);
					if (position != l_vParamNames.end())//prevent doublicate constants with parameter nodes  
						l_vParamNames.erase(position);
					continue;
				}
				l_pcConstant->GetAttribute(wxT("Name"))->SetValueString(l_sName);
				l_pcConstant->GetAttribute(wxT("Group"))->SetValueString(l_sGroup);
				l_pcConstant->GetAttribute(wxT("Comment"))->SetValueString(l_sComment);

				

				bool l_bValue = l_pcConstant->GetAttribute(wxT("Type"))->SetValueString(l_sType);

				//asssume only the main value, because there are no sets in old constant def
				SP_DS_ColListAttribute * l_pcColList1 = dynamic_cast<SP_DS_ColListAttribute*>(l_pcConstant->GetAttribute(wxT("ValueList")));
				 
				l_pcColList1->SetCell(0, 0, wxT("Main"));
				l_pcColList1->SetCell(0, 1, l_sMainVal);

				//////////////////
				 
				for (unsigned i = 0; i < l_vSetsName.size(); i++)
				{
					int l_nRowCol = l_pcColList1->AppendEmptyRow();
					l_pcColList1->SetCell(l_nRowCol, 0, l_vSetsName[i]);
					l_pcColList1->SetCell(l_nRowCol, 1, l_sMainVal);
				}

				m_pcGraph->GetFunctionRegistry()->registerFunction(l_sName, l_sMainVal);

				l_pcColList1->UpdateActiveListColumnPtr();

				l_pcConstant->SetShow(true);
				l_pcConstant->Update();

				++l_itElem;

			}
		}
		m_pcGraph->Update();

		/**************move on the list of old parameters**********************/


		//add param nodes to old constant list
		if (m_pcGraph->GetNetclass()->GetName() == SP_DS_COLSPN_CLASS || m_pcGraph->GetNetclass()->GetName() == SP_DS_COLHPN_CLASS || m_pcGraph->GetNetclass()->GetName() == SP_DS_COLCPN_CLASS)
		{
			SP_DS_Metadata* l_pcConstant2;
			 
			//get the old param nodes
			SP_DS_ColListAttribute * l_pcColListOldParam;
			SP_DS_Nodeclass* l_pcNodeclass;
			l_pcNodeclass = m_pcGraph->GetNodeclass(SP_DS_PARAM);
			SP_ListNode::const_iterator l_itElem;


			size_t xxx=l_pcColList->GetRowCount() + l_pcNodeclass->GetElements()->size();
			if (m_pcConstants->GetElements()->size() >= l_pcColList->GetRowCount()+ l_pcNodeclass->GetElements()->size())
			{//prevent doublicate the constants
				m_pcGraph->CreateDeclarationTree()->UpdateColorSetTree();
				return;
			}


			wxString l_sParameterName;
			for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
			{

				l_pcConstant2 = m_pcConstants->NewElement(1);
				l_sParameterName = dynamic_cast<SP_DS_NameAttribute*>((*l_itElem)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
				l_pcConstant2->GetAttribute(wxT("Name"))->SetValueString(l_sParameterName);
				l_pcConstant2->GetAttribute(wxT("Group"))->SetValueString(wxT("parameter"));
				l_pcConstant2->GetAttribute(wxT("Type"))->SetValueString(wxT("double"));
				//comment attribute
				wxString l_scomment;
				SP_DS_Attribute* l_pcAttrComment = dynamic_cast<SP_DS_Attribute*>((*l_itElem)->GetAttribute(wxT("Comment")));
				l_scomment << l_pcAttrComment->GetValueString();

				l_pcConstant2->GetAttribute(wxT("Comment"))->SetValueString(l_scomment);

				SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcConstant2->GetAttribute(wxT("ValueList")));
				l_pcColList->Clear();
				wxString l_sMainVal;
				wxString Name;
				////////////////////////////
				auto it = std::find(l_vParamNames.begin(), l_vParamNames.end(), l_sParameterName);
				if (it == l_vParamNames.end())
				{
					l_pcColListOldParam = dynamic_cast<SP_DS_ColListAttribute*>((*l_itElem)->GetAttribute(wxT("ParameterList")));
					for (unsigned int i = 0; i < l_pcColListOldParam->GetRowCount(); i++)
					{

						wxString l_sParName;
						wxString l_sParVal;
						wxString l_sParGroup;
						wxString l_sParType;
						

						l_sParGroup << l_pcColListOldParam->GetCell(i, 0).c_str();//set name
						l_sParVal = l_pcColListOldParam->GetCell(i, 1).c_str();//value of the set
						l_sParName << l_sParameterName;

						if (i == 0)
						{
							l_sMainVal << l_sParVal;
							Name << l_sParameterName;
						}

						int l_nRowCol = l_pcColList->AppendEmptyRow();
						l_pcColList->SetCell(i, 0, l_sParGroup);
						l_pcColList->SetCell(i, 1, l_sParVal);
						  
						//adjust type attribute
						if (l_sParType.IsEmpty())
						{
							l_sParType = wxT("double");

						}
						else {
							l_sParType = l_pcColListOldParam->GetCell(i, 2).c_str();

						}

						//comment attribute
						wxString l_scomment;
						SP_DS_Attribute* l_pcAttrComment = dynamic_cast<SP_DS_Attribute*>((*l_itElem)->GetAttribute(wxT("Comment")));
						l_scomment << l_pcAttrComment->GetValueString();

 
						if (l_sParGroup.IsEmpty())
						{
							 //do nothing
						}
						else 
						{
							if (l_sParGroup == wxT("Main"))
							{
								l_sParGroup = wxT("parameter");
							}
							 
						}
						Modify(true);
					}

				}//end if

				 

				m_pcGraph->GetFunctionRegistry()->registerFunction(Name, l_sMainVal );
				
				wxString mainv = l_pcColList->GetCell(0, 1);
				l_pcColList->UpdateActiveListColumnPtr();

				 
				l_pcConstant2->Update();


			}//end for
		}//end outer if

	
   /***delete param nodes for colCPN/ColHPN/ColSPN************************************/
		if (m_pcGraph->GetNetclass()->GetName() == SP_DS_COLHPN_CLASS
			|| m_pcGraph->GetNetclass()->GetName() == SP_DS_COLCPN_CLASS
			|| m_pcGraph->GetNetclass()->GetName() == SP_DS_COLSPN_CLASS)
		{
			SP_DS_Nodeclass* l_pcNodeclass1;
			l_pcNodeclass1 = m_pcGraph->GetNodeclass(SP_DS_PARAM);
			SP_DS_Graph*	l_pcDelGraph = m_pcGraph->CloneDefinition();
			for (auto l_itElem1 = l_pcNodeclass1->GetElements()->begin(); l_itElem1 != l_pcNodeclass1->GetElements()->end(); ++l_itElem1)
			{
				 
				std::map<SP_Graphic*, SP_Data*> l_mGraphic2Data;

				//prepare the graph we will copy to
				if (!l_pcDelGraph)
				{
					continue;
				}
				l_pcDelGraph->SetIsCopy(true);
				l_mGraphic2Data.clear();

				auto l_Gr = dynamic_cast<SP_DS_Node*>(*l_itElem1);
				SP_ListGraphic* l_ListParamGrapics = l_Gr->GetGraphics();
				SP_ListGraphic::iterator itG;
				for (itG = l_ListParamGrapics->begin(); itG != l_ListParamGrapics->end(); ++itG)
				{
					SP_Graphic* l_pcGr = *itG;
					SP_Data *l_pcParent = l_pcGr->GetParent();
					//every graphic should have a parent, so this shouldn't happen, but check it anyway
					if (!l_pcParent)
						continue;

					//	RemoveGraphicsFromCanvas(l_pcParent);
					if (!l_pcParent || !l_pcParent->GetGraphics())
						return;

					//we don't actually remove any graphics from the graph
					//we just remove them from canvas
					for (SP_Graphic* l_pcGr : *l_pcParent->GetGraphics())
					{

						RemoveGraphicFromCanvas(l_pcGr);
					}
					//now actually remove the elements from the graph
					//and with it automatically all of its graphics
					switch (l_pcParent->GetElementType())
					{
					case SP_ELEMENT_NODE:
					{
						SP_DS_Node *l_pcNodeCopy = dynamic_cast<SP_DS_Node*> (l_pcParent);
						l_pcDelGraph->AddElement(l_pcNodeCopy);
						m_pcGraph->GetParentDoc()->Refresh();
						m_pcGraph->GetParentDoc()->Modify(true);
					}

					}
				}
			}


			wxDELETE(l_pcDelGraph);

		}


	/********************************/
	m_pcGraph->CreateDeclarationTree()->UpdateColorSetTree();
}
bool SP_MDI_Doc::OnOpenDocument(const wxString& p_sFile)
{
	SP_StopWatch sw(wxT("Start loading file ") + p_sFile, wxT("Finished loading in"));

	// we are now right inside SP_GM_Docmanager::CreateDocument but
	// before the initialising of the graph member, so we have to do it ourself
	SP_GM_DocTemplate* l_pcDocTemplate =
			dynamic_cast<SP_GM_DocTemplate*>(GetDocumentTemplate());

	if (l_pcDocTemplate)
	{
		SP_DS_Netclass* l_pcNS = l_pcDocTemplate->GetNetclass();
		InitGraph(l_pcNS);
	}
	// don't know, whether it should fail, but check it anyway
	if (!m_pcGraph)
		return FALSE;

	// we will show the log window if needed
	SP_GUI_LogWindow* logWindow = wxGetApp().GetMainframe()->GetExportLogWindow();
	if (logWindow)
		logWindow->ResetNewMessages();

	if (!m_pcGraph->OnOpenDocument(p_sFile))
	{
		SP_LOGERROR( m_pcGraph->GetLastError());
		if (logWindow && logWindow->NewMessagesAdded())
			logWindow->Show();
		return FALSE;
	}
	else
	{
		if(!m_pcGraph->GetLastError().IsEmpty())
			wxGetApp().GetMainframe()->SetStatusText(m_pcGraph->GetLastError());
	}

	Modify(FALSE);
	m_savedYet = TRUE;

    //Add codes that deal with the transformation
	{
		SP_DS_Transformer l_cTransformer;
		l_cTransformer.Check(m_pcGraph);
	}
//	else
    if(m_pcGraph->GetNetclass()->GetName()==SP_DS_COLEPN_CLASS
    	|| m_pcGraph->GetNetclass()->GetName()==SP_DS_COLSPN_CLASS
		|| m_pcGraph->GetNetclass()->GetName()==SP_DS_COLPN_CLASS
		|| m_pcGraph->GetNetclass()->GetName()==SP_DS_COLHPN_CLASS
    	|| m_pcGraph->GetNetclass()->GetName()==SP_DS_COLCPN_CLASS)
	{
		m_pcGraph->CreateDeclarationTree()->UpdateColorSetTree();//liu
		HarmonizeConstantsForColPN();// by george for harmonize constants
  
	}
    //by sl
    if(m_pcGraph->GetNetclass()->GetName()==SP_DS_PN_CLASS
    		|| m_pcGraph->GetNetclass()->GetName()==SP_DS_EXTPN_CLASS
    		|| m_pcGraph->GetNetclass()->GetName()==SP_DS_SPN_CLASS
    		|| m_pcGraph->GetNetclass()->GetName()==SP_DS_HYBRIDPN_CLASS
    		|| m_pcGraph->GetNetclass()->GetName()==SP_DS_CONTINUOUSPED_CLASS || m_pcGraph->GetNetclass()->GetName() == SP_DS_FUZZYSPN_CLASS || m_pcGraph->GetNetclass()->GetName() == SP_DS_FUZZYCPN_CLASS || m_pcGraph->GetNetclass()->GetName() == SP_DS_FUZZYHPN_CLASS)
    	m_pcGraph->CreateDeclarationTree()->UpdateOtherTree();

	SP_MDI_View* l_pcView = dynamic_cast<SP_MDI_View*>(GetFirstView());
	m_pcGraph->ShowOnCanvas(l_pcView->GetCanvas());

	if (wxFile::Exists(p_sFile+wxT(".sgn")))
	{
		SP_Signer* sgn = NULL;
		try
		{
			sgn = new SP_Signer(p_sFile);
			if (sgn->verifyFile())
			{
				SP_MESSAGEBOX(wxString::Format(wxT("The graph has not been changed.\n\n")
								wxT("Date of signature: %s"),
								sgn->getTimeStampInSgnFile().c_str()),
						wxT("Verification result..."),
						wxOK | wxICON_INFORMATION);
			}
			else if (sgn->verifyStructure())
			{
				SP_MESSAGEBOX(wxString::Format(wxT("The graph's layout has been changed,\n")
								wxT("but the structure is still unchanged.\n\n")
								wxT("Date of signature: %s"),
								sgn->getTimeStampInSgnFile().c_str()),
						wxT("Verification result..."),
						wxOK | wxICON_EXCLAMATION);
			}
			else
			{
				SP_MESSAGEBOX(wxString::Format(wxT("The graph has been changed.\n\n")
								wxT("Date of signature: %s"),
								sgn->getTimeStampInSgnFile().c_str()),
						wxT("Verification result..."),
						wxOK | wxICON_ERROR);
			}
		}
		catch(const wxString& ex)
		{
			SP_MESSAGEBOX(ex, wxT("Error..."), wxOK | wxICON_EXCLAMATION);
		}
		wxDELETE(sgn);
	}
	if (logWindow && logWindow->NewMessagesAdded())
		logWindow->Show();
	return TRUE;
}

SP_DLG_SearchId* SP_MDI_Doc::GetSearchIdDialog()
{
	if (!m_pcParentDoc)
		return NULL;
	if (m_pcParentDoc != this)
	{
		return m_pcParentDoc->GetSearchIdDialog();
	}

	if (!m_SearchIdDialog)
	{
		SP_MDI_View* view = dynamic_cast<SP_MDI_View*>(GetFirstView());
		if (!view || !view->GetFrame())
			return NULL;
		m_SearchIdDialog = new SP_DLG_SearchId (m_pcGraph, view->GetFrame());
		m_SearchIdDialog->SetTitlePrefix(view->GetFrame()->GetTitle() + wxT(": "));
		SP_LOGDEBUG(wxT("new search dialog created"));
	}
	return m_SearchIdDialog;
}

SP_DLG_ChooseInvariant* SP_MDI_Doc::GetChooseInvariantDialog()
{
	if (!m_pcParentDoc)
		return NULL;
	if (m_pcParentDoc != this)
	{
		return m_pcParentDoc->GetChooseInvariantDialog();
	}

	if (!m_ChooseInvariantDialog)
	{
		SP_MDI_View* view = dynamic_cast<SP_MDI_View*>(GetFirstView());
		if (!view || !view->GetFrame())
			return NULL;

		m_ChooseInvariantDialog = new SP_DLG_ChooseInvariant(m_pcGraph, view->GetFrame());
		SP_LOGDEBUG(wxT("new Invariant Choosing Dialog created"));
	}
	if (!m_ChooseInvariantDialog->LoadInvariantFile())
	{
		return NULL;
	}

	return m_ChooseInvariantDialog;
}

void SP_MDI_Doc::ResetChooseInvariantColours()
{
	if (m_ChooseInvariantDialog)
	{
		m_ChooseInvariantDialog->DiscardColours();
	}
}

SP_DLG_ReduceFTree* SP_MDI_Doc::GetReduceFTDialog()
{
	if (!m_pcParentDoc)
		return NULL;
	if (m_pcParentDoc != this)
	{
		return m_pcParentDoc->GetReduceFTDialog();
	}

	if (!m_ReduceFTDialog)
	{
		SP_MDI_View* view = dynamic_cast<SP_MDI_View*>(GetFirstView());
		if (!view || !view->GetFrame())
			return NULL;
		m_ReduceFTDialog = new SP_DLG_ReduceFTree(m_pcGraph, view->GetFrame());
		SP_LOGDEBUG(wxT("new search dialog created"));
	}

	return m_ReduceFTDialog;
}

void SP_MDI_Doc::RemoveGraphicFromCanvas(SP_Graphic *p_pcGraphic)//by george
{
	  
	p_pcGraphic->RemoveFromCanvas();
	//also removing graphic children (e.g. token graphics, labels)
	SP_ListGraphic *gr = p_pcGraphic->GetGraphicChildren();
	if (!gr)
		return;
	SP_ListGraphic::iterator itg;
	for (SP_Graphic* l_pcGr : *gr)
	{
		RemoveGraphicFromCanvas(l_pcGr);
	}
}