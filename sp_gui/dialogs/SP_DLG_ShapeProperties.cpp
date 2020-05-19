//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_gui/dialogs/SP_DLG_ShapeProperties.h"
#include "sp_core/SP_Core.h"


#include "snoopy.h"
#include "sp_core/SP_Core.h"
#include "sp_core/SP_GPR_Canvas.h"
#include "sp_core/SP_GPR_Animation.h"
#include "sp_core/SP_GPR_Elements.h"
#include "sp_core/SP_GPR_Fonts.h"
#include "sp_gui/dialogs/SP_DLG_GlobalPreferences.h"
#include "sp_gui/windows/SP_GUI_Mainframe.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gui/mdi/SP_MDI_View.h"
#include <wx/fileconf.h>
#include "sp_utilities.h"
#include "sp_gui/interaction/SP_IA_Manager.h"

#include "sp_gui/widgets/dialogs/SP_WDG_DialogChoice.h"


#include "sp_gui/windows/SP_GUI_DevbarContainer.h"

#include "sp_ds/extensions/SP_DS_Transformer.h"

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

#include "sp_core/SP_Signer.h"
#include <wx/file.h>
#include <wx/regex.h>
#include <wx/busyinfo.h>
#include "sp_gui/interaction/SP_IA_Manager.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gui/windows/SP_GUI_DevbarContainer.h"
#include "sp_ds/extensions/SP_DS_Transformer.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/attributes/SP_DS_NodeTypeAttribute.h"
#include "sp_ds/attributes/SP_DS_IdAttribute.h"

IMPLEMENT_CLASS(SP_DLG_ShapeProperties, wxDialog)

BEGIN_EVENT_TABLE(SP_DLG_ShapeProperties, wxDialog)
EVT_BUTTON(wxID_APPLY, SP_DLG_ShapeProperties::OnDlgApply)
EVT_BUTTON(wxID_OK, SP_DLG_ShapeProperties::OnDlgOk)
EVT_BUTTON(wxID_CANCEL, SP_DLG_ShapeProperties::OnDlgCancel)
EVT_RADIOBOX(SP_ID_RADIOBOX_SELECT, SP_DLG_ShapeProperties::OnRadio)
END_EVENT_TABLE()

SP_DLG_ShapeProperties::SP_DLG_ShapeProperties(SP_GUI_Mainframe* p_pcParent,
                                               const wxString& p_sTitle,
                                               bool p_bShowDifferent,
                                               long p_nStyle)
:wxDialog(p_pcParent, -1, p_sTitle, wxDefaultPosition, wxDefaultSize,
          p_nStyle | wxSTAY_ON_TOP | wxRESIZE_BORDER | wxMAXIMIZE_BOX),
m_bShowsDifferent(p_bShowDifferent),
m_bShowOverview(false)
{
	Init();
}

void
SP_DLG_ShapeProperties::Init()
{
    /* top level sizer */
    m_pcSizer = new wxBoxSizer(wxVERTICAL);

    /* notebook to show multiple pages on */
    if(m_bShowsDifferent)
    {
    	wxArrayString choices;
    	choices.Add(wxT("Standard        "));
    	choices.Add(wxT("Overview"));

    	wxRadioBox* rb = new wxRadioBox(this, SP_ID_RADIOBOX_SELECT, wxT("Select Dialog Mode"),
    				wxDefaultPosition, wxDefaultSize, choices);
    	m_pcSizer->Add(rb, 0, wxCENTER | wxALL, 5);

    	m_pcNotebook = new SP_WDG_Treebook(this, wxT("Properties"), wxDefaultPosition, wxSize(600,500));
        m_pcNotebook->SetMinSize(wxSize(600,500));
    }
    else
    {
    	m_pcNotebook = new SP_WDG_Notebook(this, wxT("Properties"), wxDefaultPosition, wxSize(400,500));
        m_pcNotebook->SetMinSize(wxSize(400,500));
    }
    m_pcSizer->Add(m_pcNotebook, 1, wxEXPAND | wxALL, 5);

    /* Buttons in the lower right hand corner */
    wxSizer* l_pcButtonSizer = CreateButtonSizer(wxOK|wxCANCEL|wxAPPLY);

    m_pcSizer->Add(l_pcButtonSizer, 0, wxEXPAND);

    SetSizerAndFit(m_pcSizer);
    Layout();
    Centre(wxBOTH);
}

SP_WDG_NotebookPage*
SP_DLG_ShapeProperties::AddPage(const wxString& p_sLabel, unsigned int p_nOrdering)
{
    if (m_pcNotebook)
    {
    	if(m_bShowsDifferent)
    	{
            return static_cast<SP_WDG_Treebook*>(m_pcNotebook)->AddOrGetPage(p_sLabel, p_nOrdering);
    	}
    	else
    	{
            return static_cast<SP_WDG_Notebook*>(m_pcNotebook)->AddOrGetPage(p_sLabel, p_nOrdering);
    	}
    }

    return NULL;
}
bool
SP_DLG_ShapeProperties::UpdateDialogGraphic(SP_DS_Node* p_pcNode)
{
	if (!p_pcNode)
		return FALSE;
	SP_Graphic* p_pcVal;
	p_pcVal = (p_pcNode->GetGraphics()->front());

	if (p_pcVal->GetGraphicType() != SP_GRAPHIC_NODE &&
		p_pcVal->GetGraphicType() != SP_GRAPHIC_EDGE &&
		p_pcVal->GetGraphicType() != SP_GRAPHIC_METADATA)
		return FALSE;
   
	if (m_mNode2Type.size()==1)
	while (!m_mlGraphics.empty())
	{
		 
		m_mlGraphics.erase(m_mlGraphics.begin());
	}
	if (p_pcVal->GetParent() && p_pcVal->GetParent()->GetClassObject() &&
		p_pcVal->GetParent()->GetClassObject()->GetParentGraph())
	{
		wxString l_sKey;
		// TODO!
		l_sKey << p_pcVal->GetParent()->GetClassObject()->GetParentGraph()->GetName() << _T("|")
			<< p_pcVal->GetParent()->GetClassName();
		m_mlGraphics[l_sKey].push_back(p_pcVal);
	}

	m_pcGraphic = p_pcVal;
	SP_Core::Instance()->AddDialogGraphic(p_pcVal);

	if (m_mNode2Type.size() >1)
	for (auto l_It = m_mlGraphics.begin(); l_It != m_mlGraphics.end(); ++l_It)
	{
		SP_ListGraphic::iterator it = l_It->second.begin();
		for (; it != l_It->second.end(); ++it)
		{
			if ((*it)->GetDelete() == true && l_It->second.size() == 1)
			{
				m_mlGraphics.erase(l_It);
				return true;
			}
			else if((*it)->GetDelete() == true)
			{
				l_It->second.erase(it);
				return true;
			}

		}
	}
	return true;
}
bool
SP_DLG_ShapeProperties::AddDialogGraphic(SP_Graphic* p_pcVal)
{
    if (!p_pcVal)
        return FALSE;
	/////////////george////////////////

	SP_DS_Node* l_pcOldNode = dynamic_cast<SP_DS_Node*>(p_pcVal->GetParent());//dynamic_cast<SP_DS_Node*>(p_pcVal);
	if(l_pcOldNode!=NULL)
	m_mNode2Graphic[l_pcOldNode] = p_pcVal;

	bool l_bType;
	wxString l_sType;
	if (p_pcVal->GetGraphicType() == SP_GRAPHIC_NODE)
	{
		auto l_pcType = dynamic_cast<SP_DS_NodeTypeAttribute*>(l_pcOldNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NODETYPE));
		if (l_pcType)
		{
			l_sType = dynamic_cast<SP_DS_NodeTypeAttribute*>(l_pcOldNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NODETYPE))->GetValueString();
			m_mNode2Type[l_pcOldNode] = l_sType;
		}
	
	}
	////////////////////////
	 
    // we store only edges oder elements, no attributes or something else
    if (p_pcVal->GetGraphicType() != SP_GRAPHIC_NODE &&
        p_pcVal->GetGraphicType() != SP_GRAPHIC_EDGE &&
        p_pcVal->GetGraphicType() != SP_GRAPHIC_METADATA)
        return FALSE;

    if (p_pcVal->GetParent() && p_pcVal->GetParent()->GetClassObject() &&
        p_pcVal->GetParent()->GetClassObject()->GetParentGraph())
    {
        wxString l_sKey;
        // TODO!
        l_sKey << p_pcVal->GetParent()->GetClassObject()->GetParentGraph()->GetName() << _T("|")
            << p_pcVal->GetParent()->GetClassName();
        m_mlGraphics[l_sKey].push_back(p_pcVal);
    }
    return SP_Core::Instance()->AddDialogGraphic(p_pcVal);
}

bool
SP_DLG_ShapeProperties::ShowDialogAttributes(SP_MapString2ListAttribute* p_ptmAttributes)
{
		bool l_bReturn = FALSE;
    SP_Core* l_pcCore = SP_Core::Instance();
    SP_WDG_DialogBase* l_pcWidget;
    SP_MapString2ListAttribute::iterator l_IterMap;
	m_ptmAttributes = p_ptmAttributes;
    for (l_IterMap = p_ptmAttributes->begin(); l_IterMap != p_ptmAttributes->end(); ++l_IterMap)
    {
        l_pcWidget = l_pcCore->GetDialogWidget(l_IterMap->first);
        if (!l_pcWidget)
            continue;

        // remember the widget to get the changes back in OnDlgOk
        m_tlWidgets.push_back(l_pcWidget);

        // if any of the attribute lists has any element, that needs to be shown
        // l_bReturn gets TRUE and the dialog is shown to the user
        l_bReturn |= l_pcWidget->AddToDialog(&(l_IterMap->second), this, m_bShowOverview);
    }
    map<wxString, SP_ListGraphic >::iterator l_Iter;
    for (l_Iter = m_mlGraphics.begin(); l_Iter != m_mlGraphics.end(); ++l_Iter)
    {
        l_pcWidget = l_pcCore->GetDialogWidget(l_Iter->first);
        if (!l_pcWidget)
            continue;

        m_tlWidgets.push_back(l_pcWidget);
        l_bReturn |= l_pcWidget->AddToDialog(&(l_Iter->second), this);
    }

    //TODO: ugly hack to let wxTreebook appear ok
   	SetSize(GetSize() + wxSize(1,1));
   	SetSize(GetSize() - wxSize(1,1));

    return l_bReturn;
}

//------------------------------------------------------------------------
bool
SP_DLG_ShapeProperties::DoDlgApply()
{
    bool l_bReturn = TRUE;
	wxString l_sID;//
    list<SP_WDG_DialogBase*> l_lFirstAttributes;
    list<SP_WDG_DialogBase*> l_lMiddleAttributes;
    list<SP_WDG_DialogBase*> l_lLastAttributes;

    list<SP_WDG_DialogBase*>::iterator l_Iter;
    for (l_Iter = m_tlWidgets.begin(); l_Iter != m_tlWidgets.end(); ++l_Iter)
    {
        if ((*l_Iter)->GetEvalState() == -1)
            l_lFirstAttributes.push_back((*l_Iter));
        else
            if ((*l_Iter)->GetEvalState() == 0)
                l_lMiddleAttributes.push_back((*l_Iter));
            else
                l_lLastAttributes.push_back((*l_Iter));
    }

    for (l_Iter = l_lFirstAttributes.begin(); l_Iter != l_lFirstAttributes.end(); ++l_Iter)
    {
        // simply set values
        l_bReturn &= (*l_Iter)->OnDlgOk();
	 
    }
    for (l_Iter = l_lMiddleAttributes.begin(); l_Iter != l_lMiddleAttributes.end(); ++l_Iter)
    {
        // simply set values
        l_bReturn &= (*l_Iter)->OnDlgOk();
    }
    for (l_Iter = l_lLastAttributes.begin(); l_Iter != l_lLastAttributes.end(); ++l_Iter)
    {
			// simply set values
        l_bReturn &= (*l_Iter)->OnDlgOk();
    }
	 
	if (!SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNetclass()->GetName().Contains(_T("Hybrid")))
	{
		return l_bReturn;
	}

	///////////////
	std::map<wxString, wxString> l_mNodeName2SelectedType;

	wxString l_sSelection;
	//l_sSelection = GetCurentSelection();
	SP_WDG_DialogChoice * l_pcWDG_DialogText;
	//list<SP_WDG_DialogBase*>* l_ptlWidgets = m_tlWidgets;// m_pcDlg->GettlWidgets();
	list<SP_WDG_DialogBase*>::iterator itList;
 
	map<wxString, vector<wxString>> l_mPage2SelctedTypes;
	
	wxString l_sSelectedType;
	for (itList = m_tlWidgets.begin(); itList != m_tlWidgets.end(); itList++)
	{
		l_pcWDG_DialogText = dynamic_cast< SP_WDG_DialogChoice* >(*itList);

		if (l_pcWDG_DialogText)
		{
			SP_ListAttribute l_pclistAttt = (*itList)->GetDialogAttributes();
			for (auto it = l_pclistAttt.begin(); it != l_pclistAttt.end(); ++it)
			{
				SP_DS_Attribute* l_pcAtt = *it;
				SP_DS_Node* l_pcNode = dynamic_cast<SP_DS_Node*>(l_pcAtt->GetParent());
				if (l_pcNode && m_mNode2Graphic.size()>1)
				{
					SP_DS_NameAttribute* l_pcNameAttr = dynamic_cast<SP_DS_NameAttribute*> (l_pcNode->GetAttribute(wxT("Name")));
					if (l_pcNameAttr)
					{
						wxString l_sBaseName = l_pcNameAttr->GetParentName();
						l_mNodeName2SelectedType[l_sBaseName] = _T("??");
					}
				}
			}
			
		
			 
			SP_VectorString l_vToTypes;
			if (m_bShowOverview)
			{
				l_sSelectedType = l_pcWDG_DialogText->GetCurentSelection();
			}
			else
			l_sSelectedType = l_pcWDG_DialogText->GetCurentSelection(true);

			if(m_mNode2Graphic.size()==1) 
				l_sSelectedType = l_pcWDG_DialogText->GetCurentSelection();
			else
			{
				
			}
			bool l_bIsNewPage = false;
			std::pair<wxString,vector<wxChoice*>> ss=l_pcWDG_DialogText->GetPagePair();
			 
			for (auto it = ss.second.begin(); it != ss.second.end(); ++it)
			{
				wxString l_sSel=(*it)->GetStringSelection();
				l_vToTypes.push_back(l_sSel);
				l_bIsNewPage = true;

			}
			if (l_bIsNewPage)
			{
				l_mPage2SelctedTypes[ss.first] = l_vToTypes;
			}
			unsigned j = 0;
			for (auto it = l_mNodeName2SelectedType.begin(); it != l_mNodeName2SelectedType.end(); ++it)
			{
				if (it->second == wxT("??") && m_bShowOverview)
				{
					it->second = l_vToTypes[j];
					j++;
				}
				else if(it->second == wxT("??"))
				{
					it->second = l_sSelectedType;
				}
				else
				{
					continue;
				}
			}
		}
	}


	//////////////////////////////////////////////////////
	//either standard or overview dialogue mode
	if (l_mNodeName2SelectedType.size() > 0)
	{
		auto l_it = l_mNodeName2SelectedType.begin();
		for (; l_it != l_mNodeName2SelectedType.end(); ++l_it)
		{
			wxString l_sNodeName = l_it->first;
			wxString l_sToType = l_it->second;

			for (auto it = m_mNode2Type.begin(); it != m_mNode2Type.end(); ++it)
			{
				SP_DS_Node* l_pcNode = (it->first);
				SP_DS_NameAttribute* l_pcNameAttr = dynamic_cast<SP_DS_NameAttribute*> (l_pcNode->GetAttribute(wxT("Name")));
				if (l_pcNameAttr)
				{
					wxString l_sBaseName = l_pcNameAttr->GetParentName();
					if (l_sBaseName == l_sNodeName)
					{
						if (l_sToType != it->second)
						{//Do Conversion
							wxString  l_sOrginClassName = l_pcNode->GetClassName();
							SP_DS_Nodeclass* l_pcOldNodeClass = SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNodeclass(l_sOrginClassName);
							SP_DS_Nodeclass* l_pcConvertToNodeClass = SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNodeclassByDisplayedName(l_sToType);
							wxString l_sName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
							if (CheckEdgeRequirementofNode(l_pcNode, l_pcConvertToNodeClass->GetPrototype()) == false)
							{
								wxString l_sMsg = wxT("Can not convert this node (");
								l_sMsg << l_sName;
								l_sMsg << wxT("). This type of conversion is not allowed\n");

								SP_LOGERROR(l_sMsg);
								SP_MESSAGEBOX(l_sMsg, wxT("Error"));
								continue;
							}

							//make the conversion
							SP_DS_Node* l_pcNewNode = ConvertNode(l_pcNode, l_pcConvertToNodeClass);

							SP_Graphic* l_pcOldGraphic = (*l_pcNode->GetGraphics()->begin());
							//RemoveGraphicFromCanvas(l_pcOldGraphic);
							//delete old Node
							l_pcOldNodeClass->RemoveElement(l_pcNode);

							wxDELETE(l_pcNode);


							//refersh the new node
							l_pcNewNode->Update();
							SP_Graphic* dd = (*l_pcNewNode->GetGraphics()->begin());
							dd->Select(true);

							const_cast<SP_DS_Node*>(it->first) = l_pcNewNode;
							(it->second) = l_sToType;

							m_mConvertedNode2Graphic[l_pcNewNode] = l_pcNewNode->GetGraphics();
							bool b = UpdateDialogGraphic(l_pcNewNode);
						}
					}
				}

			}
		}

	}


	///////////////////////////////////////////////////////////
	 
	//convert only the one selected node
	if (m_mNode2Graphic.size() == 1)
	{
		for (auto it = m_mNode2Graphic.begin(); it != m_mNode2Graphic.end(); ++it)
		{
			SP_DS_Node* l_pcNode = (it->first);
			if (l_pcNode == NULL) continue;
			wxString l_sClassName;
			l_sClassName = l_pcNode->GetClassName();
			if (l_sClassName == _T("Place") && l_sSelectedType == _T("Discrete Place"))
			{
				continue;
			}
			else  if (l_sClassName == _T("Place, Continuous") && l_sSelectedType == _T("Continuous Place"))
			{
				continue;
			}
			else if (l_sClassName.Contains(_T("Place")) && l_sSelectedType.Contains(_T("Transition")))
			{
				continue;
			}
			else if (l_sClassName.Contains(_T("Transition")) && l_sSelectedType.Contains(_T("Place")))
			{
				continue;
			}
			else if (l_sClassName == _T("Transition") && l_sSelectedType.Contains(_T("Stochastic Transition")))
			{
				continue;
			}
			else
			{
				//try to convert a node if possible
				SP_DS_Nodeclass* l_pcOldNetClass = SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNodeclass(l_sClassName);
				SP_DS_Nodeclass* l_pcConvertToNodeClass = SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNodeclassByDisplayedName(l_sSelectedType);//(l_sSelectedType);
				wxString l_sName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
				if (CheckEdgeRequirementofNode(l_pcNode, l_pcConvertToNodeClass->GetPrototype()) == false)
				{
					wxString l_sMsg = wxT("Can not convert this node (");
					l_sMsg << l_sName;
					l_sMsg << wxT("). This type of conversion is not allowed\n");

					SP_LOGERROR(l_sMsg);
					SP_MESSAGEBOX(l_sMsg, wxT("Error"));
					continue;
				}

				//make the conversion
				SP_DS_Node* l_pcNewNode = ConvertNode(l_pcNode, l_pcConvertToNodeClass);

				SP_Graphic* l_pcOldGraphic = (*l_pcNode->GetGraphics()->begin());
				//RemoveGraphicFromCanvas(l_pcOldGraphic);
				//delete old Node
				l_pcOldNetClass->RemoveElement(l_pcNode);

				wxDELETE(l_pcNode);


				//refersh the new node
				l_pcNewNode->Update();

				const_cast<SP_DS_Node*>(it->first) = l_pcNewNode;
				m_bIsConverted = true;
				m_mConvertedNode2Graphic[l_pcNewNode] = l_pcNewNode->GetGraphics();
				bool b = UpdateDialogGraphic(l_pcNewNode);

			}
		}
	}
	SP_Core::Instance()->GetRootDocument()->Modify(true);
	SP_Core::Instance()->GetRootDocument()->GetGraph()->GetParentDoc()->Modify(true);
 
    return l_bReturn;
}



void
SP_DLG_ShapeProperties::OnDlgApply(wxCommandEvent& p_cEvent)
{
    while (GetEventHandler() != this)
        PopEventHandler();

    DoDlgApply();

    RebuildDialog();

}

void
SP_DLG_ShapeProperties::OnDlgOk(wxCommandEvent& p_cEvent)
{
    while (GetEventHandler() != this)
        PopEventHandler();

    bool l_bReturn = DoDlgApply();

    // clean up the widgets and
    // continue with the event handling
    if (l_bReturn)
    {
    	CleanUpWidgets();
		//SP_GUI_Childframe *l_pcFrame = wxGetApp().GetMainframe()->GetActiveChildframe();
		//if (l_pcFrame)
		//{
		//	SP_GUI_Canvas* canvas = dynamic_cast<SP_MDI_View*>(l_pcFrame->GetView())->GetCanvas();
		//	wxClientDC dc(canvas);

		//	canvas->DoPrepareDC(dc);
		//	SP_Core::Instance()->GetRootDocument()->GetGraph()->ShowOnCanvas(canvas, true);
		//}
		 //  SP_Core::Instance()->GetRootDocument();
		/*
		if (SP_Core::Instance()->GetRootDocument()->GetDiagram() && SP_Core::Instance()->GetRootDocument()->GetDiagram()->GetShapeList())
		{
			SP_ListGraphic l_lGraphics;
			SP_MDI_View* l_pcView = dynamic_cast<SP_MDI_View*>(SP_Core::Instance()->GetRootDocument()->GetFirstView());
			wxNode *l_pcNode = SP_Core::Instance()->GetRootDocument()->GetDiagram()->GetShapeList()->GetFirst();
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
		*/
	  
		//bug fix: show multiplicities on arcs G.Assaf
		SP_Core::Instance()->GetRootDocument()->GetGraph()->SqueezeIdAttributes();
		SP_Core::Instance()->GetRootDocument()->GetGraph()->CheckIntegrity();
		SP_Core::Instance()->GetRootDocument()->Modify(true);
		SP_Core::Instance()->GetRootDocument()->GetGraph()->GetParentDoc()->Modify(true);
		SP_Core::Instance()->GetRootDocument()->GetGraph()->GetParentDoc()->Refresh();
		 
       p_cEvent.Skip();
    }
}

void
SP_DLG_ShapeProperties::OnDlgCancel(wxCommandEvent& p_cEvent)
{
    while (GetEventHandler() != this)
        PopEventHandler();

    // clean up the widgets and
    // continue with the event handling
	CleanUpWidgets();
    
	SP_Core::Instance()->ClearDialogGraphicList();
	 
    p_cEvent.Skip();
}

void
SP_DLG_ShapeProperties::CleanUpWidgets()
{
    list<SP_WDG_DialogBase*>::iterator l_Iter = m_tlWidgets.begin();
    while (l_Iter != m_tlWidgets.end())
    {
        SP_WDG_DialogBase* l_pcWidget = (*l_Iter);
        m_tlWidgets.erase(l_Iter);
		l_pcWidget->CleanUpAttributes();
        wxDELETE(l_pcWidget);
        l_Iter = m_tlWidgets.begin();
    }	
}

void
SP_DLG_ShapeProperties::OnRadio(wxCommandEvent& p_cEvent)
{
    while (GetEventHandler() != this)
        PopEventHandler();

    if(p_cEvent.GetSelection() == 0)
	{
		m_bShowOverview = false;
	}
	else
	{
		m_bShowOverview = true;
	}
	RebuildDialog();
}

void
SP_DLG_ShapeProperties::RebuildDialog()
{
    // clean up the widgets and
	// display new widgets again, because of Logicattribute
	CleanUpWidgets();
	int l_nCurrentPage = m_pcNotebook->GetSelection();
	  m_pcNotebook->DeleteAllPages();
	// m_pcNotebook->DeletePage(0);
	// map of list of attributes
	SP_MapString2ListAttribute l_tmAttributes;

	if (m_bShowOverview && m_mConvertedNode2Graphic.size()>0)
	{
		SP_ListGraphic* l_lGraphics;
		auto l_It = m_mlGraphics.begin();
		int i = 0;
		for (; l_It != m_mlGraphics.end(); ++l_It)
		{
			 
			//	m_mlGraphics.erase(l_It);
			//	break;
			//	if(l_It->first)

			 
			 
		}

		l_It = m_mlGraphics.begin();
		 
		for (; l_It != m_mlGraphics.end(); ++l_It)
		{
			 
			 
			SP_ListGraphic::iterator l_Iter;
			l_Iter =  l_It->second.begin();
			for ( ; l_Iter != l_It->second.end(); ++l_Iter)
			{
				if ((*l_Iter)->GetDelete() == true) {  continue; }
				(*l_Iter)->CollectDialogAttributes(&l_tmAttributes);
			}
			 
		}

		ShowDialogAttributes(&l_tmAttributes);
		m_pcNotebook->SetSelection(l_nCurrentPage);
		return;
	}

	//l_tmAttributes = *m_ptmAttributes;
	////////////////////////////////////////////////
	if (m_mConvertedNode2Graphic.size() > 0 &&   (m_mNode2Graphic.size() == 1))
	{
		
		for (auto it = m_mConvertedNode2Graphic.begin(); it != m_mConvertedNode2Graphic.end(); ++it)
		{
		 	SP_ListGraphic* l_lGraphics;
		 	l_lGraphics = it->second;
			 SP_Core::Instance()->AddDialogGraphic(*it->first->GetGraphics()->begin());
		 	SP_ListGraphic::iterator l_Iter;
		 	for (l_Iter = l_lGraphics->begin(); l_Iter != l_lGraphics->end(); ++l_Iter)
		 	{
			 	(*l_Iter)->CollectDialogAttributes(&l_tmAttributes);
		 	}
			 ShowDialogAttributes(&l_tmAttributes);
			 m_pcNotebook->SetSelection(l_nCurrentPage);
			 return;
		}

	}

	///////////////////////////////////
	// get the attributes to show in the dialog
	// implemented in base class 'SP_Graphic'
	SP_ListGraphic* l_lGraphics;
	l_lGraphics = SP_Core::Instance()->GetDialogGraphicList();
	SP_ListGraphic::iterator l_Iter; 
	for (l_Iter = l_lGraphics->begin(); l_Iter != l_lGraphics->end(); ++l_Iter)
	{ 
		(*l_Iter)->CollectDialogAttributes(&l_tmAttributes);
	}
	ShowDialogAttributes(&l_tmAttributes);
	m_pcNotebook->SetSelection(l_nCurrentPage);
	
}



bool SP_DLG_ShapeProperties::CheckEdgeRequirementofNode(SP_DS_Node* p_pcNodeToCheck, SP_DS_Node* p_pcToConvertType)
{
	SP_ListEdge::const_iterator l_itEdge;

	const SP_ListEdge* l_pcEdgeList;

	//check source edges
	l_pcEdgeList = p_pcNodeToCheck->GetSourceEdges();

	for (l_itEdge = l_pcEdgeList->begin(); l_itEdge != l_pcEdgeList->end(); l_itEdge++)
	{
		if (EdgeRequirement((*l_itEdge)->GetEdgeclass(), p_pcToConvertType, (*l_itEdge)->GetTarget()) == false)
		{
			return false;
		}
	}

	//check target edges
	l_pcEdgeList = p_pcNodeToCheck->GetTargetEdges();

	for (l_itEdge = l_pcEdgeList->begin(); l_itEdge != l_pcEdgeList->end(); l_itEdge++)
	{
		if (EdgeRequirement((*l_itEdge)->GetEdgeclass(), (*l_itEdge)->GetSource(), p_pcToConvertType) == false)
		{
			return false;
		}
	}

	return true;

}

bool SP_DLG_ShapeProperties::EdgeRequirement(SP_DS_Edgeclass* p_pcEdgeclass, SP_Data* p_pcSrc, SP_Data* p_pcTrg)
{
	SP_DS_Netclass* l_pcNetClass = SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNetclass();

	//CHECK_POINTER(l_pcNetClass, return false);
	if (l_pcNetClass == NULL)
		return false;

	return l_pcNetClass->EdgeRequirement(p_pcEdgeclass, p_pcSrc, p_pcTrg);
}


SP_DS_Node* SP_DLG_ShapeProperties::ConvertNode(SP_DS_Node* p_pcOldNode, SP_DS_Nodeclass* p_pcNewNodeClass)
{
	SP_ListGraphic::const_iterator l_itOldGraphic;
	SP_ListGraphic::const_iterator l_itNewGraphic;

	//CHECK_POINTER(p_pcOldNode, return NULL);
	if (p_pcOldNode == NULL)
		return NULL;
	SP_DS_Node* l_pcNewNode = p_pcNewNodeClass->NewElement(p_pcOldNode->GetNetnumber());

	//copy  values of attributes with same names
	const SP_ListAttribute* l_pcOldAttributeList = p_pcOldNode->GetAttributes();
	SP_ListAttribute::const_iterator l_itAttribute;

	for (l_itAttribute = l_pcOldAttributeList->begin();
		l_itAttribute != l_pcOldAttributeList->end();
		l_itAttribute++)
	{
		wxString l_sAttributeName = (*l_itAttribute)->GetName();

		SP_DS_Attribute* l_pcNewAttribute = l_pcNewNode->GetAttribute(l_sAttributeName);

		if (l_pcNewAttribute != NULL)
		{
			l_pcNewAttribute->CopyValueFrom((*l_itAttribute));
			l_pcNewAttribute->SetShow((*l_itAttribute)->GetShow());
		}
	}

	const SP_ListGraphic* l_pcOldGraphicList = p_pcOldNode->GetGraphics();
	const SP_ListGraphic* l_pcNewGraphicList = l_pcNewNode->GetGraphics();

	//CHECK_POINTER(l_pcOldGraphicList, return NULL);
	//CHECK_POINTER(l_pcNewGraphicList, return NULL);

	if (l_pcOldGraphicList == NULL) return NULL;
	if (l_pcNewGraphicList == NULL) return NULL;

	//add graphic to the new node as in the old one. We already have one
	for (l_itOldGraphic = l_pcOldGraphicList->begin();
		l_itOldGraphic != l_pcOldGraphicList->end();
		l_itOldGraphic++)
	{
		SP_Graphic* l_pcOldGraphic = (*l_itOldGraphic);

		SP_Graphic* l_pcNewGraphic;
		if (l_itOldGraphic == l_pcOldGraphicList->begin())
		{
			l_pcNewGraphic = l_pcNewGraphicList->front();
			l_pcNewGraphic->SetNetnumber(l_pcOldGraphic->GetNetnumber());
		}
		else
		{
			l_pcNewGraphic = l_pcNewNode->NewGraphicInSubnet(l_pcOldGraphic->GetNetnumber());
		}

		//l_pcNewGraphic->SetBrush(l_pcOldGraphic->GetBrush());

		//l_pcNewGraphic->SetPen(l_pcOldGraphic->GetPen());

		l_pcNewGraphic->SetHeight(l_pcOldGraphic->GetHeight());
		l_pcNewGraphic->SetWidth(l_pcOldGraphic->GetWidth());

		l_pcNewGraphic->SetPosX(l_pcOldGraphic->GetPosX());
		l_pcNewGraphic->SetPosY(l_pcOldGraphic->GetPosY());
		l_pcNewGraphic->SetOffsetX(l_pcOldGraphic->GetOffsetX());
		l_pcNewGraphic->SetOffsetY(l_pcOldGraphic->GetOffsetY());

		SP_ListGraphic::const_iterator l_itOldGraphicChild = l_pcOldGraphic->GetGraphicChildren()->begin();
		SP_ListGraphic::const_iterator l_itNewGraphicChild = l_pcNewGraphic->GetGraphicChildren()->begin();

		for (; l_itNewGraphicChild != l_pcNewGraphic->GetGraphicChildren()->end();
			++l_itNewGraphicChild, ++l_itOldGraphicChild)
		{
			SP_Graphic* l_pcOldGraphicChild = (*l_itOldGraphicChild);
			SP_Graphic* l_pcNewGraphicChild = (*l_itNewGraphicChild);
			l_pcNewGraphicChild->SetPosX(l_pcOldGraphicChild->GetPosX());
			l_pcNewGraphicChild->SetPosY(l_pcOldGraphicChild->GetPosY());
			l_pcNewGraphicChild->SetOffsetX(l_pcOldGraphicChild->GetOffsetX());
			l_pcNewGraphicChild->SetOffsetY(l_pcOldGraphicChild->GetOffsetY());
		}
		//SP_MDI_View* l_pcView = dynamic_cast<SP_MDI_View*>(SP_Core::Instance()->GetRootDocument()->vi
		//SP_MDI_View* l_pcView = dynamic_cast<SP_MDI_View*>(SP_Core::Instance()->GetRootDocument()->GetFirstView());
		//l_pcNewGraphic->AddToCanvas(l_pcView->GetCanvas(), l_pcOldGraphic->GetPrimitive()->GetX(), l_pcOldGraphic->GetPrimitive()->GetY(), false);
		SP_GUI_Canvas* l_pcCanvas = dynamic_cast<SP_GUI_Canvas*>(l_pcOldGraphic->GetPrimitive()->GetCanvas());
		l_pcCanvas->GetView()->Refresh();
		l_pcNewGraphic->AddToCanvas(l_pcCanvas, l_pcOldGraphic->GetPrimitive()->GetX(), l_pcOldGraphic->GetPrimitive()->GetY(), false);
		l_pcOldGraphic->GetPrimitive()->GetShape()->RemoveFromCanvas(l_pcCanvas);
		 
	}

	//copy source and targets edges
	const SP_ListEdge* l_pcEdgeList = p_pcOldNode->GetSourceEdges();
	SP_ListEdge::const_iterator l_itEdge;

	for (l_itEdge = l_pcEdgeList->begin(); l_itEdge != l_pcEdgeList->end(); l_itEdge++)
	{
		l_pcNewNode->AddSourceEdge((*l_itEdge));

		UpdateSourceEdgeGraphics((*l_itEdge), l_pcOldGraphicList, l_pcNewGraphicList);
	}

	//change edge list
	l_pcEdgeList = p_pcOldNode->GetTargetEdges();

	for (l_itEdge = l_pcEdgeList->begin(); l_itEdge != l_pcEdgeList->end(); l_itEdge++)
	{
		l_pcNewNode->AddTargetEdge((*l_itEdge));

		UpdateTargetEdgeGraphics((*l_itEdge), l_pcOldGraphicList, l_pcNewGraphicList);
	}


	//remove the node graphics from the canvas
	SP_Graphic* l_pcOldGraphic = (*l_pcOldGraphicList->begin());

	 

	RemoveGraphicFromCanvas(l_pcOldGraphic); //do not remove them now
	
	 
	return l_pcNewNode;
}


bool SP_DLG_ShapeProperties::UpdateSourceEdgeGraphics(SP_DS_Edge* p_pcEdge, const SP_ListGraphic* p_pcOldGraphicList, const SP_ListGraphic* p_pcNewGraphicList)
{
	SP_Graphic* l_pcNodeGraphics;

	//CHECK_POINTER(p_pcEdge, return false);
	if (p_pcEdge == NULL) return false;

	const SP_ListGraphic* l_pcGraphicList = p_pcEdge->GetGraphics();

	//CHECK_POINTER(l_pcGraphicList, return false);
	if (l_pcGraphicList == NULL) return false;

	for (SP_Graphic* l_pcGraphic : *l_pcGraphicList)
	{
		if (l_pcGraphic->GetGraphicType() == SP_GRAPHIC_EDGE)
		{
			SP_GR_Edge* l_pcGrEdge = static_cast<SP_GR_Edge*>(l_pcGraphic);

			l_pcNodeGraphics = FindGraphic(p_pcOldGraphicList, p_pcNewGraphicList, l_pcGrEdge->GetSource());

			//CHECK_POINTER(l_pcNodeGraphics, return false);
			if (l_pcNodeGraphics == NULL)
				return false;

			l_pcGrEdge->SetSource(l_pcNodeGraphics);
		}
	}

	return true;
}


bool SP_DLG_ShapeProperties::UpdateTargetEdgeGraphics(SP_DS_Edge* p_pcEdge, const SP_ListGraphic* p_pcOldGraphicList, const SP_ListGraphic* p_pcNewGraphicList)
{
	SP_Graphic* l_pcNodeGraphics;

	//CHECK_POINTER(p_pcEdge, return false);
	if (p_pcEdge == NULL)
		return false;

	const SP_ListGraphic* l_pcGraphicList = p_pcEdge->GetGraphics();

	//CHECK_POINTER(l_pcGraphicList, return false);
	if (l_pcGraphicList == NULL)
		return false;

	for (SP_Graphic* l_pcGraphic : *l_pcGraphicList)
	{
		if (l_pcGraphic->GetGraphicType() == SP_GRAPHIC_EDGE)
		{
			SP_GR_Edge* l_pcGrEdge = static_cast<SP_GR_Edge*>(l_pcGraphic);

			l_pcNodeGraphics = FindGraphic(p_pcOldGraphicList, p_pcNewGraphicList, l_pcGrEdge->GetTarget());

			//CHECK_POINTER(l_pcNodeGraphics, return false);
			if (l_pcNodeGraphics == NULL) return false;
			l_pcGrEdge->SetTarget(l_pcNodeGraphics);
		}
	}

	return true;
}


SP_Graphic* SP_DLG_ShapeProperties::FindGraphic(const SP_ListGraphic* p_pcOldGraphicList, const SP_ListGraphic* p_pcNewGraphicList, SP_Graphic* p_pcOldGraphic)
{
	SP_ListGraphic::const_iterator l_itNewGraphic = p_pcNewGraphicList->begin();

	for (SP_Graphic* l_pcOldGr : *p_pcOldGraphicList)
	{
		if (l_pcOldGr == p_pcOldGraphic)
		{
			return (*l_itNewGraphic);
		}
		++l_itNewGraphic;
	}

	return NULL;
}


void SP_DLG_ShapeProperties::RemoveGraphicFromCanvas(SP_Graphic *p_pcGraphic)
{
	//SP_LOGDEBUG(wxT("entered RemoveGraphicFromCanvas(graphic)..."));
	p_pcGraphic->GetPrimitive()->GetCanvas()->Refresh();
	p_pcGraphic->RemoveFromCanvas();
	p_pcGraphic->SetDelete(true);//
	//also removing graphic children (e.g. token graphics, labels)
	SP_ListGraphic *gr = p_pcGraphic->GetGraphicChildren();
	if (!gr)
		return;
	SP_ListGraphic::iterator itg;
	for (itg = gr->begin(); itg != gr->end(); ++itg)
	{
		RemoveGraphicFromCanvas(*itg);
	}
}

 