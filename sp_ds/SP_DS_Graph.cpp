//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_core/SP_Core.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_utilities.h"

#include "sp_ds/extensions/SP_DS_Animation.h"
#include "sp_ds/extensions/SP_DS_FunctionRegistry.h"

#include "sp_gui/widgets/SP_WDG_GraphTreectrl.h"
#include "sp_gui/widgets/SP_WDG_CoarseTreectrl.h"
#include "sp_gui/dialogs/dia_CPN/SP_WDG_DeclarationTreectrl.h" //Added by Liu
#include "sp_gui/dialogs/SP_DLG_GraphProperties.h"

// for writing
#include "sp_core/SP_XmlWriter.h"
// for reading
#include "sp_core/SP_XmlReader.h"
// for wxGetApp()
#include "snoopy.h"
#include "sp_gui/mdi/SP_MDI_CoarseDoc.h"
#include "sp_gui/interaction/SP_IA_Manager.h"
#include "sp_gui/commands/SP_CMD_Edit.h"

#include <wx/wupdlock.h>
#include "sp_core/SP_GPR_Elements.h"

#include "snoopy.h"

SP_DS_Graph::SP_DS_Graph(SP_DS_Netclass* p_pcNetclass)
:SP_Type(SP_ELEMENT_GRAPH),
SP_Name(wxT("Graph")),
m_bDestruction(FALSE),
m_pcNetclass(p_pcNetclass),
m_pcCoarseTreectrl(NULL),
m_pcDeclarationTreectrl(NULL),
m_pcFunctionRegistry(new SP_DS_FunctionRegistry(this)),
m_nNetnumber(0),
m_bShowInGlobalDialog(FALSE),
m_bHasAnimators(FALSE),
m_bIsCopy(FALSE)
{
	if (m_pcNetclass)
    {
		map<wxString, SP_SetString> mm = wxGetApp().getNetClases();
		SP_GPR_Elements* l_pcGrElements = new SP_GPR_Elements();
		//wxConfig config(SP_APP_SHORT_NAME, SP_APP_VENDOR);
		map<wxString, int> l_mmGraphicstoPosX=   l_pcGrElements->GetNameAttGRaphicsPosXMap();
		
		SetName(m_pcNetclass->GetName());
        m_pcNetclass->CreateGraph(this, l_mmGraphicstoPosX);
        m_pcNetclass->CreateInitialElements(this);
    }
}

SP_DS_Graph::~SP_DS_Graph()
{
    m_bDestruction = TRUE;

    for (auto l_pcNodeclass : m_lNodeclass)
    {
        wxDELETE(l_pcNodeclass);
    }
	m_lNodeclass.clear();
	
    for (auto l_pcEdgeclass : m_lEdgeclass)
    {
        wxDELETE(l_pcEdgeclass);
    }
	m_lEdgeclass.clear();
	
    for (auto l_pcMetadataclass : m_lMetadataclass)
    {
        wxDELETE(l_pcMetadataclass);
    }
	m_lMetadataclass.clear();
	
    if (m_pcCoarseTreectrl && !m_bIsCopy)
        m_pcCoarseTreectrl->Remove();

	if (m_pcDeclarationTreectrl && !m_bIsCopy)
		m_pcDeclarationTreectrl->Remove();

	wxDELETE(m_pcFunctionRegistry);
}

SP_DS_Graph*
SP_DS_Graph::CloneDefinition()
{
    CHECK_POINTER(m_pcNetclass, return NULL);

    // new object, but without the call to SP_DS_Netclass::CreateGraph
    SP_DS_Graph* l_pcNewGraph = new SP_DS_Graph();
    l_pcNewGraph->SetName(GetName());
	
    for (auto l_pcNC : m_lNodeclass)
    {
		l_pcNewGraph->AddNodeclass(new SP_DS_Nodeclass(l_pcNewGraph, l_pcNC->GetName()));
	}
    for (auto l_pcEC : m_lEdgeclass)
    {
		l_pcNewGraph->AddEdgeclass(new SP_DS_Edgeclass(l_pcNewGraph, l_pcEC->GetName()));
	}
    for (auto l_pcMC : m_lMetadataclass)
    {
		l_pcNewGraph->AddMetadataclass(new SP_DS_Metadataclass(l_pcNewGraph, l_pcMC->GetName()));
	}

    // just set the pointer, avoid call to CreateGraph!
    l_pcNewGraph->SetNetclass(GetNetclass());

    return l_pcNewGraph;
}

SP_DS_Netclass*
SP_DS_Graph::GetNetclass()
{
    return m_pcNetclass;
}

/*SP_DLG_ColCPNDirSimulationResults*
SP_DS_Graph::GetSimClass()
{
	return m_pcSimClass;
}*/

SP_DS_Nodeclass*
SP_DS_Graph::AddNodeclass(SP_DS_Nodeclass* p_pcClass)
{
    CHECK_POINTER(p_pcClass, return NULL);

    m_lNodeclass.push_back(p_pcClass);

    return p_pcClass;
}

SP_DS_Nodeclass*
SP_DS_Graph::RemoveNodeclass(SP_DS_Nodeclass* p_pcClass)
{
    CHECK_POINTER(p_pcClass, return NULL);

    m_lNodeclass.remove(p_pcClass);

    return p_pcClass;
}

SP_DS_Nodeclass*
SP_DS_Graph::RenameNodeclass(const wxString& p_pchFrom, const wxString& p_pchTo, bool p_bRenameDisplyName)
{
    SP_DS_Nodeclass* l_pcNC = GetNodeclass(p_pchFrom);

    if (!l_pcNC || !p_pchTo)
        return NULL;

    l_pcNC->SetName(p_pchTo);

    //for backward Compatibility
    if(p_bRenameDisplyName)
    	l_pcNC->SetDisplayName(p_pchTo);

    return l_pcNC;
}

SP_DS_Nodeclass*
SP_DS_Graph::GetNodeclass(const wxString& p_pchName)
{
    if(p_pchName.IsEmpty()) return NULL;

	for (auto l_pcElem : m_lNodeclass)
	{
        if (p_pchName == l_pcElem->GetName())
            return l_pcElem;
	}
    return NULL;
}

SP_DS_Nodeclass*
SP_DS_Graph::GetNodeclassByDisplayedName(const wxString& p_pchName)
{
    if(p_pchName.IsEmpty()) return NULL;

 	for (auto l_pcElem : m_lNodeclass)
	{
        if (p_pchName == l_pcElem->GetDisplayName())
            return l_pcElem;
	}
    return NULL;
}

SP_DS_Edgeclass*
SP_DS_Graph::AddEdgeclass(SP_DS_Edgeclass* p_pcClass)
{
    CHECK_POINTER(p_pcClass, return NULL);

    m_lEdgeclass.push_back(p_pcClass);

    return p_pcClass;
}

SP_DS_Edgeclass*
SP_DS_Graph::RenameEdgeclass(const wxString& p_pchFrom, const wxString& p_pchTo,bool p_bRenameDisplyName)
{
    SP_DS_Edgeclass* l_pcEC = GetEdgeclass(p_pchFrom);

    if (!l_pcEC || !p_pchTo)
        return NULL;

    l_pcEC->SetName(p_pchTo);

    //for backward Compatibility
      if(p_bRenameDisplyName)
    	  l_pcEC->SetDisplayName(p_pchTo);

    return l_pcEC;
}

SP_DS_Edgeclass*
SP_DS_Graph::GetEdgeclass(const wxString& p_pchName)
{
    if(p_pchName.IsEmpty()) return NULL;

	for (auto l_pcElem : m_lEdgeclass)
	{
        if (p_pchName == l_pcElem->GetName())
            return l_pcElem;
	}
     return NULL;
}

SP_DS_Edgeclass*
SP_DS_Graph::GetEdgeclassByDisplayedName(const wxString& p_pchName)
{
    if(p_pchName.IsEmpty()) return NULL;

	for (auto l_pcElem : m_lEdgeclass)
	{
        if (p_pchName == l_pcElem->GetDisplayName())
            return l_pcElem;
	}
    return NULL;
}

SP_DS_Metadataclass*
SP_DS_Graph::AddMetadataclass(SP_DS_Metadataclass* p_pcClass)
{
    CHECK_POINTER(p_pcClass, return NULL);

    m_lMetadataclass.push_back(p_pcClass);

    return p_pcClass;
}

SP_DS_Metadataclass*
SP_DS_Graph::RemoveMetadataclass(SP_DS_Metadataclass* p_pcClass)
{
    CHECK_POINTER(p_pcClass, return NULL);

    m_lMetadataclass.remove(p_pcClass);

    return p_pcClass;
}

SP_DS_Metadataclass*
SP_DS_Graph::RenameMetadataclass(const wxString& p_pchFrom, const wxString& p_pchTo,bool p_bRenameDisplyName)
{
    SP_DS_Metadataclass* l_pcMC = GetMetadataclass(p_pchFrom);

    if (!l_pcMC || !p_pchTo)
        return NULL;

    l_pcMC->SetName(p_pchTo);

    //for backward Compatibility
      if(p_bRenameDisplyName)
    	  l_pcMC->SetDisplayName(p_pchTo);

    return l_pcMC;
}

SP_DS_Metadataclass*
SP_DS_Graph::GetMetadataclass(const wxString& p_pchName)
{
    if(p_pchName.IsEmpty()) return NULL;

	for (auto l_pcElem : m_lMetadataclass)
	{
        if (p_pchName == l_pcElem->GetName())
            return l_pcElem;
	}
    return NULL;
}

SP_DS_Metadataclass*
SP_DS_Graph::GetMetadataclassByDisplayedName(const wxString& p_pchName)
{
    if(p_pchName.IsEmpty()) return NULL;

	for (auto l_pcElem : m_lMetadataclass)
	{
        if (p_pchName == l_pcElem->GetDisplayName())
            return l_pcElem;
	}
    return NULL;
}

bool
SP_DS_Graph::SetGlobalShow(bool p_bVal)
{
    m_bShowInGlobalDialog = p_bVal;

    return m_bShowInGlobalDialog;
}

SP_Data*
SP_DS_Graph::GetCoarseNode(unsigned int p_nNetnumber)
{
    for (auto l_pcNC : m_lNodeclass)
    {
    	SP_Data* l_pcReturn = l_pcNC->GetCoarseNode(p_nNetnumber);
        if(l_pcReturn)
        	return l_pcReturn;
    }
    return nullptr;
}

bool
SP_DS_Graph::ShowOnCanvas(SP_GUI_Canvas* p_pcCanvas, bool p_bLocalOnly)
{
    CHECK_POINTER(p_pcCanvas, return FALSE);

    wxWindowUpdateLocker noUpdates(p_pcCanvas);
    bool l_bReturn = TRUE;

    for (auto l_pcNC : m_lNodeclass)
        l_bReturn &= l_pcNC->ShowOnCanvas(p_pcCanvas, p_bLocalOnly);

    for (auto l_pcEC : m_lEdgeclass)
        l_bReturn &= l_pcEC->ShowOnCanvas(p_pcCanvas, p_bLocalOnly);

    for (auto l_pcMC : m_lMetadataclass)
        l_bReturn &= l_pcMC->ShowOnCanvas(p_pcCanvas, p_bLocalOnly);

    p_pcCanvas->Refresh(TRUE);
    return l_bReturn;
}

bool
SP_DS_Graph::CheckIntegrity()
{
    bool l_bReturn = TRUE;

    for (auto l_pcNC : m_lNodeclass)
        l_bReturn &= l_pcNC->CheckIntegrity();

    for (auto l_pcEC : m_lEdgeclass)
        l_bReturn &= l_pcEC->CheckIntegrity();

    for (auto l_pcMC : m_lMetadataclass)
        l_bReturn &= l_pcMC->CheckIntegrity();

    GetParentDoc()->Refresh();

    return l_bReturn;
}


bool
SP_DS_Graph::GetGraphicsInNet(SP_ListGraphic* p_plGraphics, unsigned int p_nNet, const unsigned int p_eFilter)
{
    CHECK_POINTER(p_plGraphics, return FALSE);

    bool l_bReturn = TRUE;

    for (auto l_pcNC : m_lNodeclass)
        l_bReturn &= l_pcNC->GetGraphicsInNet(p_plGraphics, p_nNet, p_eFilter);

    for (auto l_pcEC : m_lEdgeclass)
        l_bReturn &= l_pcEC->GetGraphicsInNet(p_plGraphics, p_nNet, p_eFilter);

    for (auto l_pcMC : m_lMetadataclass)
        l_bReturn &= l_pcMC->GetGraphicsInNet(p_plGraphics, p_nNet, p_eFilter);

    return l_bReturn;
}

bool
SP_DS_Graph::OnFlatten(unsigned int p_nToNet, unsigned int p_nFromNet, SP_ListGraphic* p_plGraphics)
{
    bool l_bReturn = TRUE;

    for (auto l_pcNC : m_lNodeclass)
        l_bReturn &= l_pcNC->OnFlatten(p_nToNet, p_nFromNet, p_plGraphics);

    for (auto l_pcEC : m_lEdgeclass)
        l_bReturn &= l_pcEC->OnFlatten(p_nToNet, p_nFromNet, p_plGraphics);

    for (auto l_pcMC : m_lMetadataclass)
        l_bReturn &= l_pcMC->OnFlatten(p_nToNet, p_nFromNet, p_plGraphics);

    return l_bReturn;
}

bool
SP_DS_Graph::RemoveElements(unsigned int p_nNetnumber)
{
    if (m_bDestruction)
        return TRUE;

    bool l_bReturn = TRUE;
    SP_ListData l_lDelete;

    for (auto l_pcNC : m_lNodeclass)
    {
        l_bReturn &= l_pcNC->RemoveElements(p_nNetnumber, &l_lDelete);
    }

    for (auto l_pcEC : m_lEdgeclass)
    {
        l_bReturn &= l_pcEC->RemoveElements(p_nNetnumber, &l_lDelete);
    }

    for (auto l_pcMC : m_lMetadataclass)
    {
        l_bReturn &= l_pcMC->RemoveElements(p_nNetnumber, &l_lDelete);
    }

    // we just removed the dependencies in our DS
    // now really call delete for all queued objects
    for (SP_Data* l_pcData : l_lDelete)
    {
        wxDELETE(l_pcData);
    }
    return l_bReturn;
}

bool SP_DS_Graph::AddToDeclarationTreectrl(SP_WDG_DeclarationTreectrl* p_pcCtrl)
{
	if (!p_pcCtrl)
        return FALSE;

    bool l_bReturn = TRUE;

    wxTreeItemId l_cRootId;
    wxTreeItemId l_cIdParent;

    // build up the tree-content following the rules of wxTreeCtrl
    // this special tree just have one root node with at least two children
    // (node- and edge-class-folder), each one filled with the right number of
    // children, as given for this graph

    // the root item
    l_cRootId = p_pcCtrl->AddRoot(
        wxT("Declarations"),
        SP_WDG_GraphTreectrl::TREE_CTRL_ICON_Folder,
        SP_WDG_GraphTreectrl::TREE_CTRL_ICON_Folder,
        new SP_GraphTreeItemdata(wxT("Root item")));
    // with a folder icon
    l_bReturn &= p_pcCtrl->SetFolderIcons(l_cRootId);

	if (this->GetNetclass()->GetName().Contains(_T("Colored")))

	{
		l_cIdParent = p_pcCtrl->AppendFolderItem(l_cRootId, wxT("Constants"));
	}
    // the item for the Metadata

    SP_ListMetadataclass::const_iterator l_itMC;

	if (((this->GetNetclass()->GetName() == SP_DS_COLSPN_CLASS)
		|| (this->GetNetclass()->GetName() == SP_DS_COLCPN_CLASS)
		|| (this->GetNetclass()->GetName() == SP_DS_COLPN_CLASS)
		|| (this->GetNetclass()->GetName() == SP_DS_COLHPN_CLASS)
		|| (this->GetNetclass()->GetName() == SP_DS_FUZZY_ColCPN_CLASS)//by george
		|| (this->GetNetclass()->GetName() == SP_DS_FUZZY_ColSPN_CLASS)//by george
		|| (this->GetNetclass()->GetName() == SP_DS_FUZZY_ColHPN_CLASS)//by george
		|| (this->GetNetclass()->GetName() == SP_DS_COLEPN_CLASS)))
	{
		l_cIdParent = p_pcCtrl->AppendFolderItem(l_cRootId, wxT("Color Sets"));

		// with a folder icon
		l_bReturn &= p_pcCtrl->SetFolderIcons(l_cIdParent);

		// iterating through the list of all Metadataclasses

		for (l_itMC = m_lMetadataclass.begin(); l_itMC != m_lMetadataclass.end(); ++l_itMC)
		{
			if ((*l_itMC)->GetDisplayName()==(_T("Constants")))//by george
			{
				continue;
			}
			if ((*l_itMC)->GetShowInDeclarationTreeColorSet())
			{
				// adding the entry, by calling the function of the tree control
				p_pcCtrl->AppendFolderItem(l_cIdParent, (*l_itMC)->GetDisplayName());
			}
		}

		
		for (l_itMC = m_lMetadataclass.begin(); l_itMC != m_lMetadataclass.end(); ++l_itMC)
			{

			   if ((*l_itMC)->GetShowInDeclarationTreeOther() && (*l_itMC)->GetDisplayName() == (_T("Functions")))
				 {
							p_pcCtrl->AppendFolderItem(l_cRootId, (*l_itMC)->GetDisplayName());
				 }


				if ((*l_itMC)->GetShowInDeclarationTreeOther() && (*l_itMC)->GetDisplayName() == (_T("Variables")))
				{
					p_pcCtrl->AppendFolderItem(l_cRootId, (*l_itMC)->GetDisplayName());
				}
			}

		p_pcCtrl->Expand(l_cIdParent);

	 }

 

	for (l_itMC = m_lMetadataclass.begin(); l_itMC != m_lMetadataclass.end(); ++l_itMC)
	    {


		if (((*l_itMC)->GetDisplayName() == (_T("Constants"))
			|| (*l_itMC)->GetDisplayName() == (_T("Functions"))
			|| (*l_itMC)->GetDisplayName() == (_T("Variables")))
			&& this->GetNetclass()->GetName().Contains(_T("Colored")))//by george
				{
					continue;
				}

	      if( (*l_itMC)->GetShowInDeclarationTreeOther() )
	      {
	        // adding the entry, by calling the function of the tree control
	        p_pcCtrl->AppendFolderItem(l_cRootId, (*l_itMC)->GetDisplayName());
	      }
	    }

    p_pcCtrl->Expand(l_cRootId);

    return l_bReturn;
}


bool
SP_DS_Graph::AddToTreectrl(SP_WDG_GraphTreectrl* p_pcCtrl)
{
    if (!p_pcCtrl)
        return FALSE;

    bool l_bReturn = TRUE;

    wxTreeItemId l_cRootId;
    wxTreeItemId l_cIdParent;

    // build up the tree-content following the rules of wxTreeCtrl
    // this special tree just have one root node with at least two children
    // (node- and edge-class-folder), each one filled with the right number of
    // children, as given for this graph

    // the root item
    l_cRootId = p_pcCtrl->AddRoot(
        GetName(),
        SP_WDG_GraphTreectrl::TREE_CTRL_ICON_Folder,
        SP_WDG_GraphTreectrl::TREE_CTRL_ICON_Folder,
        new SP_GraphTreeItemdata(wxT("Root item")));
    // with a folder icon
    l_bReturn &= p_pcCtrl->SetFolderIcons(l_cRootId);

    // the item for the elements (nodes)
    l_cIdParent = p_pcCtrl->AppendFolderItem(l_cRootId, wxT("Elements"));
    // with a folder icon
    l_bReturn &= p_pcCtrl->SetFolderIcons(l_cIdParent);

    // iterating through the list of all nodeclasses
    SP_ListNodeclass::const_iterator l_itNC;
    for (l_itNC = m_lNodeclass.begin(); l_itNC != m_lNodeclass.end(); ++l_itNC)
    {
      if( (*l_itNC)->GetShowInElementTree() )
      {
        // adding the entry, by calling the function of the tree control
        p_pcCtrl->AppendFileItem(l_cIdParent, (*l_itNC)->GetDisplayName(), (*l_itNC));
      }
    }
    p_pcCtrl->Expand(l_cIdParent);

    // the entry for the edges
    l_cIdParent = p_pcCtrl->AppendFolderItem(l_cRootId, wxT("Edges"));
    // with a folder icon
    l_bReturn &= p_pcCtrl->SetFolderIcons(l_cIdParent);

    SP_ListEdgeclass::const_iterator l_itEC;
    for (l_itEC = m_lEdgeclass.begin(); l_itEC != m_lEdgeclass.end(); ++l_itEC)
    {
        // adding the entry, by calling the function of the tree control
        p_pcCtrl->AppendFileItem(l_cIdParent, (*l_itEC)->GetDisplayName(), (*l_itEC));
    }
    p_pcCtrl->Expand(l_cIdParent);

    // the item for the Metadata
    l_cIdParent = p_pcCtrl->AppendFolderItem(l_cRootId, wxT("Metadata"));
    // with a folder icon
    l_bReturn &= p_pcCtrl->SetFolderIcons(l_cIdParent);

    // iterating through the list of all Metadataclasses
    SP_ListMetadataclass::const_iterator l_itMC;
    for (l_itMC = m_lMetadataclass.begin(); l_itMC != m_lMetadataclass.end(); ++l_itMC)
    {
      if( (*l_itMC)->GetShowInElementTree() )
      {
        // adding the entry, by calling the function of the tree control
        p_pcCtrl->AppendFileItem(l_cIdParent, (*l_itMC)->GetDisplayName(), (*l_itMC));
      }
    }
    p_pcCtrl->Expand(l_cIdParent);

    p_pcCtrl->Expand(l_cRootId);

    return l_bReturn;
}

bool
SP_DS_Graph::HasAttributeType(SP_ATTRIBUTE_TYPE p_eVal)
{
    SP_ListNodeclass::const_iterator l_itNC;
    for (l_itNC = m_lNodeclass.begin(); l_itNC != m_lNodeclass.end(); ++l_itNC)
    {
        if ((*l_itNC)->HasAttributeType(p_eVal))
            return TRUE;
    }

    SP_ListEdgeclass::const_iterator l_itEC;
    for (l_itEC = m_lEdgeclass.begin(); l_itEC != m_lEdgeclass.end(); ++l_itEC)
    {
        if ((*l_itEC)->HasAttributeType(p_eVal))
            return TRUE;
    }

    SP_ListMetadataclass::const_iterator l_itMC;
    for (l_itMC = m_lMetadataclass.begin(); l_itMC != m_lMetadataclass.end(); ++l_itMC)
    {
        if ((*l_itMC)->HasAttributeType(p_eVal))
            return TRUE;
    }
   return FALSE;
}

bool
SP_DS_Graph::AddToViewMenu(wxMenu* p_pcMenu)
{
	CHECK_POINTER(p_pcMenu, return FALSE);

	bool m_bHasIdAttribute = HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID);

    if (m_bShowInGlobalDialog || m_bHasIdAttribute)
    {
        p_pcMenu->AppendSeparator();

        if(m_bShowInGlobalDialog)
            p_pcMenu->Append(SP_MENU_ITEM_EDIT_SHOW,
			     wxT("Show Attributes..."),
			     wxT("Edit global settings"));
    }

    if (m_bHasAnimators)
    {
		p_pcMenu->AppendSeparator();
		wxMenuItem* l_pcMenuItem = new wxMenuItem(p_pcMenu,
					  SP_MENU_ITEM_TOGGLEANIM,
					  wxT("Start Anim-Mode\tF5"),
					  wxT("Toggle animation mode"),
					  wxITEM_CHECK);
		p_pcMenu->Append(l_pcMenuItem);
		l_pcMenuItem->Check(SP_Core::Instance()->GetAnimMode());
	}

    wxString l_sName = this->GetNetclass()->GetName();
    if((l_sName == SP_DS_SPN_CLASS) ||
       (l_sName == SP_DS_CONTINUOUSPED_CLASS) ||
       (l_sName == SP_DS_COLSPN_CLASS) ||
       (l_sName == SP_DS_COLCPN_CLASS) ||
	    (l_sName == SP_DS_COLHPN_CLASS) ||
		(l_sName==SP_DS_FUZZY_ColHPN_CLASS)||//by george
		(l_sName == SP_DS_FUZZY_ColCPN_CLASS) ||//by george
		(l_sName == SP_DS_FUZZY_ColSPN_CLASS) ||//by george
       (l_sName == SP_DS_HYBRIDPN_CLASS) || (l_sName == SP_DS_FUZZYCPN_CLASS) || (l_sName == SP_DS_FUZZYSPN_CLASS) || (l_sName==SP_DS_FUZZYHPN_CLASS))//Added by G.A
    {
    	//Simulation mode
		p_pcMenu->AppendSeparator();
		if (l_sName == SP_DS_COLSPN_CLASS) {
		
		}
		else {

		}
		wxMenuItem* l_pcMenuItem = new wxMenuItem(p_pcMenu,
					  SP_MENU_ITEM_TOGGLESIMULATION,
					  wxT("Start Simulation-Mode\tF6"),
					  wxT("Start Simulation mode"));
		p_pcMenu->Append(l_pcMenuItem);

		
		
	
    }
//Amr
	//wxString l_sName = this->GetNetclass()->GetName();
	if ((l_sName == SP_DS_COLSPN_CLASS) ||
		(l_sName == SP_DS_COLCPN_CLASS) ||
		(l_sName == SP_DS_COLHPN_CLASS) )
	{
		//Direct Simulation mode
		p_pcMenu->AppendSeparator();
		wxMenuItem* l_pcMenuItem = new wxMenuItem(p_pcMenu,
			SP_MENU_ITEM_TOGGLEDIRSIMULATION,
			wxT("&Start Color Simulation-Mode\tCtrl+F6"),
			wxT("Color Simulation mode"));
		p_pcMenu->Append(l_pcMenuItem);
		
	}
    //by sl
    if((l_sName == SP_DS_EXTPN_CLASS) ||
       (l_sName == SP_DS_SPN_CLASS) ||
       (l_sName == SP_DS_CONTINUOUSPED_CLASS) ||
       (l_sName == SP_DS_HYBRIDPN_CLASS) ||( l_sName == SP_DS_FUZZYSPN_CLASS)  || (l_sName == SP_DS_FUZZYCPN_CLASS)|| (l_sName==SP_DS_FUZZYHPN_CLASS))
    {
    	p_pcMenu->AppendSeparator();
		wxMenuItem* l_pcMenuItem = new wxMenuItem(p_pcMenu,
					  SP_MENU_ITEM_HIGHMARKING,
					  wxT("Show marking dependent edges\tF8"),
					  wxT("Toggle marking dependent edges"),
					  wxITEM_CHECK);
		p_pcMenu->Append(l_pcMenuItem);
    }

    return TRUE;
}

bool
SP_DS_Graph::AddToEditMenu(wxMenu* p_pcMenu)
{
    CHECK_POINTER(p_pcMenu, return FALSE);
 
    wxString l_sName = GetNetclass()->GetName();
   if(l_sName == SP_DS_CONTINUOUSPED_CLASS || l_sName==SP_DS_SPN_CLASS ||l_sName==SP_DS_HYBRIDPN_CLASS || l_sName==SP_DS_EXTPN_CLASS || l_sName==SP_DS_FUZZYHPN_CLASS|| l_sName==SP_DS_FUZZYCPN_CLASS||l_sName==SP_DS_FUZZYSPN_CLASS||
	  l_sName == SP_DS_COLEPN_CLASS || l_sName == SP_DS_COLCPN_CLASS || l_sName==SP_DS_COLSPN_CLASS ||l_sName==SP_DS_COLHPN_CLASS)
     {
       p_pcMenu->AppendSeparator();
       p_pcMenu->Append(SP_MENU_CONVERT_ELEMENT_MENU,wxT("Convert to... \tShift+Ctrl+V"));
     }
 


    return TRUE;
}

bool
SP_DS_Graph::AddToExtrasMenu(wxMenu* p_pcMenu)
{
    CHECK_POINTER(p_pcMenu, return FALSE);

    wxMenu *l_pcMenu = new wxMenu();
    UpdateIASubMenu(l_pcMenu);
    l_pcMenu->AppendSeparator();
    l_pcMenu->Append(SP_MENU_ITEM_IA_TRIGGERGENERAL, wxT("Trigger General-Action\tCtrl+Shift+G"));
    l_pcMenu->Append(SP_MENU_ITEM_IA_TRIGGERSINGLE, wxT("Trigger Single-Action(s)\tCtrl+Shift+S"));
    l_pcMenu->Append(SP_MENU_ITEM_IA_TRIGGERMAKRO, wxT("Trigger Makro-Action\tCtrl+Shift+M"));
    l_pcMenu->Append(SP_MENU_ITEM_IA_UNDOLASTACTION, wxT("Undo Last Action"));
    l_pcMenu->AppendSeparator();
    l_pcMenu->Append(SP_MENU_ITEM_IA_CONFIGURE, wxT("Configure..."));
    l_pcMenu->Append(SP_MENU_ITEM_IA_REGCMDLIST, wxT("Registered Commands..."));
    p_pcMenu->Append(SP_MENU_ITEM_IA_MENU, wxT("Interaction"), l_pcMenu);

    p_pcMenu->AppendSeparator();
    p_pcMenu->Append(SP_MENU_ITEM_DUPLICATE,
                     wxT("Duplicate Into Logical Nodes..."));
    p_pcMenu->Append(SP_MENU_ITEM_MERGE,
                     wxT("Merge Logical Nodes..."));

    wxString l_sName = GetNetclass()->GetName();

	if(l_sName == SP_DS_PN_CLASS ||
	   l_sName == SP_DS_SPN_CLASS ||
		l_sName==SP_DS_FUZZYSPN_CLASS||
		l_sName==SP_DS_FUZZYCPN_CLASS ||
		l_sName==SP_DS_FUZZYHPN_CLASS ||
	   l_sName == SP_DS_EXTPN_CLASS ||
	   l_sName == SP_DS_TIMEPN_CLASS ||
	   l_sName == SP_DS_CONTINUOUSPED_CLASS ||
       l_sName == SP_DS_HYBRIDPN_CLASS ||
	   l_sName == SP_DS_MODULOPN_CLASS ||
	   l_sName == SP_DS_MUSICPN_CLASS){
        p_pcMenu->AppendSeparator();
        p_pcMenu->Append(SP_MENU_ITEM_LOAD_INVARIANT_FILE,
					wxT("Load node set file..."));
    }

	if(l_sName == SP_DS_FAULTTREE_CLASS ||
       l_sName == SP_DS_EXTFAULTTREE_CLASS){
        p_pcMenu->AppendSeparator();
        p_pcMenu->Append(SP_MENU_ITEM_REDUCTION_RULES,
					wxT("Reduction"));
    }
	if(l_sName == SP_DS_FAULTTREE_CLASS){
		p_pcMenu->Append(SP_MENU_ITEM_DEMORGAN_TRANSFORMER,
					wxT("Use de Morgan"));
	}


	if(l_sName == SP_DS_COLSPN_CLASS 
		|| l_sName == SP_DS_COLPN_CLASS
		||l_sName == SP_DS_COLEPN_CLASS
		||l_sName == SP_DS_COLHPN_CLASS
		||l_sName == SP_DS_COLCPN_CLASS)
	{
        p_pcMenu->AppendSeparator();
        wxMenu *l_pcMenu = new wxMenu();
		l_pcMenu->Append(SP_MENU_ITEM_GENERATE_MASTER_NET,wxT("Generate master nets"));

		if(l_sName == SP_DS_COLSPN_CLASS )
			l_pcMenu->Append(SP_MENU_ITEM_GENERATE_TWIN_NET,wxT("Generate twin nets"));	
		
		l_pcMenu->Append(SP_MENU_ITEM_COLORIZE_ANY_NET,wxT("Colorize"));	  
		
		p_pcMenu->Append(SP_MENU_ITEM_FOLDING, wxT("Folding"), l_pcMenu);
    }


	//metadata menu entries
	p_pcMenu->AppendSeparator();
	p_pcMenu->Append(SP_MENU_ITEM_MD_GENERAL, wxT("General Informations..."));

    return TRUE;
}


void
SP_DS_Graph::UpdateIASubMenu(wxMenu *p_pcIAMenu) {
//SP_LOGDEBUG(wxT("entered UpdateIASubMenu..."));
	if (!p_pcIAMenu) return;
	if (p_pcIAMenu->FindItem(SP_MENU_ITEM_IA_SUBMENU)) p_pcIAMenu->Destroy(SP_MENU_ITEM_IA_SUBMENU);
	//building up from scratch, to prevent messing up the id's of the menu items
	wxMenu *l_pcSubMenu = new wxMenu();
	SP_IA_Manager *l_pcIAMgr = wxGetApp().GetIAManager();
	if (!l_pcIAMgr) {
		wxDELETE(l_pcSubMenu);
		return;
	}
	wxString p_sSource = l_pcIAMgr->GetFilenameFromGraph(this);
	wxArrayString l_sConnectedNets = l_pcIAMgr->GetConnectedNets(p_sSource);
	for (unsigned int i = 0; i < l_sConnectedNets.GetCount(); i++) {
		l_pcSubMenu->AppendCheckItem(SP_MENU_ITEM_IA_SUBMENUITEM + i,	l_sConnectedNets[i]);
		l_pcSubMenu->Check(SP_MENU_ITEM_IA_SUBMENUITEM + i, l_pcIAMgr->IAModeEnabled(p_sSource, l_sConnectedNets[i]));
	}
	if (l_pcSubMenu->GetMenuItemCount() == 0) {
		l_pcSubMenu->Append(SP_MENU_ITEM_IA_SUBMENUITEM, wxT("No nets connected"));
		l_pcSubMenu->Enable(SP_MENU_ITEM_IA_SUBMENUITEM, false);
	}
	wxMenuItem *l_pcSubMenuItem = new wxMenuItem(p_pcIAMenu, SP_MENU_ITEM_IA_SUBMENU, wxT("Enable/Disable connections"));
	l_pcSubMenuItem->SetSubMenu(l_pcSubMenu);
	p_pcIAMenu->Prepend(l_pcSubMenuItem);
}


void
SP_DS_Graph::UpdateIASubMenuSelection(wxMenu *p_pcVisMenu, wxCommandEvent &p_cEvent) {
	if (!p_pcVisMenu) return;

	wxMenuItem *l_pcItem = p_pcVisMenu->FindItem(p_cEvent.GetId());
	SP_IA_Manager *l_pcIAMgr = wxGetApp().GetIAManager();
	if (!l_pcIAMgr) return;

	wxString p_sSource = l_pcIAMgr->GetFilenameFromGraph(this);
	l_pcIAMgr->EnableConnection(p_cEvent.IsChecked(), p_sSource, l_pcItem->GetItemLabel());
}


bool
SP_DS_Graph::AddToElementsMenu(wxMenu* p_menu)
{
	CHECK_POINTER(p_menu, return FALSE);

	int id = SP_MENU_ITEM_ELEMENTS_FIRST;

	p_menu->AppendRadioItem(id, _T("Select\tS"));
	id++;

	wxString tmpS;

	// iterating through the list of all nodeclasses
	SP_ListNodeclass::const_iterator l_itNC;
	for (l_itNC = m_lNodeclass.begin(); l_itNC != m_lNodeclass.end(); ++l_itNC)
	{
		if ((*l_itNC)->GetShowInElementTree())
		{
			tmpS = wxString::Format(wxT("%s\t%s"), (*l_itNC)->GetDisplayName().c_str(), (*l_itNC)->GetShortcut().c_str());
			p_menu->AppendRadioItem(id, tmpS);
			id++;
		}
	}
	SP_ListEdgeclass::const_iterator l_itEC;
	for (l_itEC = m_lEdgeclass.begin(); l_itEC != m_lEdgeclass.end(); ++l_itEC)
	{
		tmpS = wxString::Format(wxT("%s\t%s"), (*l_itEC)->GetDisplayName().c_str(), (*l_itEC)->GetShortcut().c_str());
		p_menu->AppendRadioItem(id, tmpS);
		id++;
	}
	SP_ListMetadataclass::const_iterator l_itMC;
	for (l_itMC = m_lMetadataclass.begin(); l_itMC != m_lMetadataclass.end(); ++l_itMC)
	{
		if ((*l_itMC)->GetShowInElementTree())
		{
			tmpS = wxString::Format(wxT("%s\t%s"), (*l_itMC)->GetDisplayName().c_str(), (*l_itMC)->GetShortcut().c_str());
			p_menu->AppendRadioItem(id, tmpS);
			id++;
		}
	}

	return TRUE;
}


bool
SP_DS_Graph::SqueezeIdAttributes()
{
    bool l_bReturn = TRUE;

    // iterating through the list of all node classes
    SP_ListNodeclass::const_iterator l_itNC;
    for (l_itNC = m_lNodeclass.begin(); l_itNC != m_lNodeclass.end(); ++l_itNC)
    {
        l_bReturn &= (*l_itNC)->SqueezeIdAttributes();
    }
    // iterating through the list of all edge classes
    SP_ListEdgeclass::const_iterator l_itEC;
    for (l_itEC = m_lEdgeclass.begin(); l_itEC != m_lEdgeclass.end(); ++l_itEC)
    {
        l_bReturn &= (*l_itEC)->SqueezeIdAttributes();
    }
    // iterating through the list of all Metadata classes
    SP_ListMetadataclass::const_iterator l_itMC;
    for (l_itMC = m_lMetadataclass.begin(); l_itMC != m_lMetadataclass.end(); ++l_itMC)
    {
        l_bReturn &= (*l_itMC)->SqueezeIdAttributes();
    }

    return l_bReturn;
}

bool
SP_DS_Graph::ShowGlobalOptions(SP_DLG_GraphProperties* p_pcDlg)
{
    CHECK_POINTER(p_pcDlg, return FALSE);

    if (!m_bShowInGlobalDialog)
        return FALSE;

    bool l_bReturn = TRUE;
    // iterating through the list of all node classes
    SP_ListNodeclass::const_iterator l_itNC;
    for (l_itNC = m_lNodeclass.begin(); l_itNC != m_lNodeclass.end(); ++l_itNC)
    {
      if( (*l_itNC)->GetShowInElementTree() )
      {
        l_bReturn &= (*l_itNC)->ShowGlobalOptions(p_pcDlg);
      }
    }
    // iterating through the list of all edge classes
    SP_ListEdgeclass::const_iterator l_itEC;
    for (l_itEC = m_lEdgeclass.begin(); l_itEC != m_lEdgeclass.end(); ++l_itEC)
    {
        l_bReturn &= (*l_itEC)->ShowGlobalOptions(p_pcDlg);
    }
    // iterating through the list of all Metadata classes
    SP_ListMetadataclass::const_iterator l_itMC;
    for (l_itMC = m_lMetadataclass.begin(); l_itMC != m_lMetadataclass.end(); ++l_itMC)
    {
      if( (*l_itMC)->GetShowInElementTree() )
      {
        l_bReturn &= (*l_itMC)->ShowGlobalOptions(p_pcDlg);
      }
    }

    return l_bReturn;
}

bool
SP_DS_Graph::OnGlobalOptions()
{
    // we never showed a dialog
    if (!m_bShowInGlobalDialog)
        return FALSE;

    bool l_bReturn = TRUE;
    // iterating through the list of all node classes
    SP_ListNodeclass::const_iterator l_itNC;
    for (l_itNC = m_lNodeclass.begin(); l_itNC != m_lNodeclass.end(); ++l_itNC)
    {
        l_bReturn &= (*l_itNC)->OnGlobalOptions();
    }
    // iterating through the list of all edge classes
    SP_ListEdgeclass::const_iterator l_itEC;
    for (l_itEC = m_lEdgeclass.begin(); l_itEC != m_lEdgeclass.end(); ++l_itEC)
    {
        l_bReturn &= (*l_itEC)->OnGlobalOptions();
    }
    // iterating through the list of all Metadata classes
    SP_ListMetadataclass::const_iterator l_itMC;
    for (l_itMC = m_lMetadataclass.begin(); l_itMC != m_lMetadataclass.end(); ++l_itMC)
    {
        l_bReturn &= (*l_itMC)->OnGlobalOptions();
    }

    return l_bReturn;
}

SP_WDG_CoarseTreectrl*
SP_DS_Graph::CreateCoarseTree()
{
    if (!m_pcCoarseTreectrl)
    {
        m_pcCoarseTreectrl = new SP_WDG_CoarseTreectrl(
            wxGetApp().GetMainframe()->GetHierarchyContainer(),
            SP_ID_COARSETREE_CTRL_ID,
            wxDefaultPosition,
            wxDefaultSize,
            wxTR_DEFAULT_STYLE);
        m_pcCoarseTreectrl->SetGraph(this);
    }
    return m_pcCoarseTreectrl;
}

SP_WDG_DeclarationTreectrl*
SP_DS_Graph::CreateDeclarationTree()
{
    if (!m_pcDeclarationTreectrl)
    {
        m_pcDeclarationTreectrl = new SP_WDG_DeclarationTreectrl(
            wxGetApp().GetMainframe()->GetDeclarationContainer(),
            SP_ID_COLORSETTREE_CTRL_ID,
            wxDefaultPosition,
            wxDefaultSize,
            wxTR_DEFAULT_STYLE);
        m_pcDeclarationTreectrl->SetGraph(this);
		
    }
	
    return m_pcDeclarationTreectrl;
}
 
bool
SP_DS_Graph::ResetCoarseTree()
{
    bool l_bReturn = TRUE;
    // iterating through the list of all node classes, searching for the one with m_pcCoarse
    SP_ListNodeclass::const_iterator l_itNC;
    do
    {
        l_bReturn = TRUE;
        for (l_itNC = m_lNodeclass.begin(); l_itNC != m_lNodeclass.end(); ++l_itNC)
        {
            l_bReturn &= (*l_itNC)->ResetCoarseTree();
        }
    }
    while (!l_bReturn);

    return l_bReturn;
}

bool
SP_DS_Graph::JoinElements(SP_Data* p_pcRefElem, const wxString& p_pchRefAttribute)
{
    CHECK_POINTER(p_pcRefElem, return FALSE);
    if(p_pchRefAttribute.IsEmpty()) return FALSE;

    SP_Data* l_pcNew = p_pcRefElem->GetSibling(p_pchRefAttribute);

    // nothing found means, we are the first one
    if (!l_pcNew)
    {
        p_pcRefElem->SetLogical(TRUE);

        return TRUE;
    }

    if (l_pcNew->MergeData(p_pcRefElem))
    {
        wxDELETE(p_pcRefElem);

        return TRUE;
    }

    return FALSE;
}

bool
SP_DS_Graph::AddToCopyMap(SP_DS_Graph** p_ppcGraph,
                          unsigned int p_nNetnumber,
                          SP_MapGraphic2Graphic* p_pmGraphic2Graphic,
                          SP_MapData2Data* p_pmData2Data)
{
    bool l_bReturn = TRUE;

    SP_ListNodeclass::const_iterator l_itNC;
    for (l_itNC = m_lNodeclass.begin(); l_itNC != m_lNodeclass.end(); ++l_itNC)
        l_bReturn &= (*l_itNC)->AddToCopyMap(p_ppcGraph, p_nNetnumber, p_pmGraphic2Graphic, p_pmData2Data);

    SP_ListEdgeclass::const_iterator l_itEC;
    for (l_itEC = m_lEdgeclass.begin(); l_itEC != m_lEdgeclass.end(); ++l_itEC)
        l_bReturn &= (*l_itEC)->AddToCopyMap(p_ppcGraph, p_nNetnumber, p_pmGraphic2Graphic, p_pmData2Data);

    SP_ListMetadataclass::const_iterator l_itMC;
    for (l_itMC = m_lMetadataclass.begin(); l_itMC != m_lMetadataclass.end(); ++l_itMC)
        l_bReturn &= (*l_itMC)->AddToCopyMap(p_ppcGraph, p_nNetnumber, p_pmGraphic2Graphic, p_pmData2Data);

    return l_bReturn;
}

bool
SP_DS_Graph::SetNetnumbers(unsigned int p_nNewVal, unsigned int p_nOldVal)
{
    bool l_bReturn = TRUE;

    SP_ListNodeclass::const_iterator l_itNC;
    for (l_itNC = m_lNodeclass.begin(); l_itNC != m_lNodeclass.end(); ++l_itNC)
        l_bReturn &= (*l_itNC)->SetNetnumbers(p_nNewVal, p_nOldVal);

    SP_ListEdgeclass::const_iterator l_itEC;
    for (l_itEC = m_lEdgeclass.begin(); l_itEC != m_lEdgeclass.end(); ++l_itEC)
        l_bReturn &= (*l_itEC)->SetNetnumbers(p_nNewVal, p_nOldVal);

    SP_ListMetadataclass::const_iterator l_itMC;
    for (l_itMC = m_lMetadataclass.begin(); l_itMC != m_lMetadataclass.end(); ++l_itMC)
        l_bReturn &= (*l_itMC)->SetNetnumbers(p_nNewVal, p_nOldVal);

    return l_bReturn;
}

bool
SP_DS_Graph::ResetNetnumbers(unsigned int p_nFromNr, unsigned int p_nToNr, SP_DS_Graph* p_pcToGraph)
{
    CHECK_POINTER(p_pcToGraph, return FALSE);

    bool l_bReturn = TRUE;
    SP_MapUInt2UInt l_mNet2Net;
    // initial values are always the netnumber, we copied from associated
    // to the one, we copy too, now.
    l_mNet2Net[p_nFromNr] = p_nToNr;

    SP_ListNodeclass::const_iterator l_itNC;
    for (l_itNC = m_lNodeclass.begin(); l_itNC != m_lNodeclass.end(); ++l_itNC)
        l_bReturn &= (*l_itNC)->ResetNetnumbers(&l_mNet2Net, p_pcToGraph);

    SP_ListEdgeclass::const_iterator l_itEC;
    for (l_itEC = m_lEdgeclass.begin(); l_itEC != m_lEdgeclass.end(); ++l_itEC)
        l_bReturn &= (*l_itEC)->ResetNetnumbers(&l_mNet2Net, p_pcToGraph);

    SP_ListMetadataclass::const_iterator l_itMC;
    for (l_itMC = m_lMetadataclass.begin(); l_itMC != m_lMetadataclass.end(); ++l_itMC)
        l_bReturn &= (*l_itMC)->ResetNetnumbers(&l_mNet2Net, p_pcToGraph);

    return l_bReturn;
}

bool
SP_DS_Graph::Copy(unsigned int p_nNr, SP_ListGraphic* p_plShapes)
{
	return SP_Core::Instance()->Copy(this, p_nNr, p_plShapes);
}

bool
SP_DS_Graph::Paste(SP_GUI_Canvas* p_pcCanvas)
{
    return SP_Core::Instance()->Paste(this, p_pcCanvas);
}

bool
SP_DS_Graph::AddElement(SP_Data* p_pcElement)
{
    CHECK_POINTER(p_pcElement, return FALSE);
    bool l_bReturn = false;

    if(p_pcElement->GetElementType() == SP_ELEMENT_NODE)
    {
    	SP_DS_Node* l_pcNode = dynamic_cast<SP_DS_Node*>(p_pcElement);
        if (l_pcNode->GetNodeclass())
        {
        	SP_DS_Nodeclass* l_pcNC = GetNodeclass(l_pcNode->GetNodeclass()->GetName());
            if (l_pcNC)
            	l_bReturn = l_pcNC->AddElement(l_pcNode);
        }
    }
    else if(p_pcElement->GetElementType() ==  SP_ELEMENT_EDGE)
    {
    	SP_DS_Edge* l_pcEdge = dynamic_cast<SP_DS_Edge*>(p_pcElement);
        if (l_pcEdge->GetEdgeclass())
        {
        	SP_DS_Edgeclass* l_pcEC = GetEdgeclass(l_pcEdge->GetEdgeclass()->GetName());
            if (l_pcEC)
                l_bReturn = l_pcEC->AddElement(l_pcEdge);
        }
    }
    else if(p_pcElement->GetElementType() == SP_ELEMENT_METADATA)
	{
		SP_DS_Metadata* l_pcMetadata = dynamic_cast<SP_DS_Metadata*>(p_pcElement);
		if (l_pcMetadata->GetMetadataclass())
		{
			SP_DS_Metadataclass* l_pcMC = GetMetadataclass(l_pcMetadata->GetMetadataclass()->GetName());
			if (l_pcMC)
				l_bReturn = l_pcMC->AddElement(l_pcMetadata);
		}
	}

    return l_bReturn;
}

bool
SP_DS_Graph::OnSaveDocument(const wxString& p_sFile)
{
    if (p_sFile.IsEmpty())
        return FALSE;

    CheckIntegrity();

    SP_XmlWriter l_cWriter;
    if (l_cWriter.GetErrorSet())
    {
        SetLastError(l_cWriter.GetLastError());
        return FALSE;
    }

	if (!l_cWriter.Write(this, p_sFile))
	{
		SetLastError(l_cWriter.GetLastError());
		return FALSE;
	}

    return TRUE;
}

bool
SP_DS_Graph::OnOpenDocument(const wxString& p_sFile)
{
    if (p_sFile.IsEmpty())
        return FALSE;

    SP_XmlReader l_cReader;
    l_cReader.Read(this, p_sFile);

    if (l_cReader.GetErrorSet())
    {
        SetLastError(l_cReader.GetLastError());
        //we load the file anyway
        //return FALSE;
    }

    //this call is needed for converted files
    m_pcNetclass->CreateInitialElements(this);

    m_pcFunctionRegistry->LoadFromNet(this);

    CheckIntegrity();

	return TRUE;
}

bool
SP_DS_Graph::OnToggleAnim(bool p_bVal)
{
    if (SP_Core::Instance()->GetAnimMode() == p_bVal)
        return p_bVal;

    SP_Core::Instance()->SetAnimMode(p_bVal, this);

    return SP_Core::Instance()->GetAnimMode();
}

bool
SP_DS_Graph::SetAnimation(SP_DS_Animation* p_pcAnim)
{
    return SP_Core::Instance()->RegisterAnimation(GetNetclass(), p_pcAnim);
}

bool
SP_DS_Graph::ShowDebug()
{
    bool l_bReturn = TRUE;

    SP_LOGDEBUG(wxString::Format(wxT("Graph: %s (%p) of Netclass %s (%p)"),
				   GetName().c_str(), this, m_pcNetclass->GetName().c_str(), m_pcNetclass));

    SP_ListNodeclass::const_iterator l_itNC;
    for (l_itNC = m_lNodeclass.begin(); l_itNC != m_lNodeclass.end(); ++l_itNC)
        l_bReturn &= (*l_itNC)->ShowDebug();

    SP_ListEdgeclass::const_iterator l_itEC;
    for (l_itEC = m_lEdgeclass.begin(); l_itEC != m_lEdgeclass.end(); ++l_itEC)
        l_bReturn &= (*l_itEC)->ShowDebug();

    SP_ListMetadataclass::const_iterator l_itMC;
    for (l_itMC = m_lMetadataclass.begin(); l_itMC != m_lMetadataclass.end(); ++l_itMC)
        l_bReturn &= (*l_itMC)->ShowDebug();

    return l_bReturn;
}

SP_MDI_Doc*
SP_DS_Graph::GetParentDoc()
{
	wxNode *l_pcNode = wxGetApp().GetDocmanager()->GetDocuments().GetFirst();
	SP_MDI_Doc *l_pcDoc;

	while (l_pcNode)
	{
		l_pcDoc = dynamic_cast<SP_MDI_Doc*>(l_pcNode->GetData());
		if (l_pcDoc->GetGraph() == this) return l_pcDoc;
		l_pcNode = l_pcNode->GetNext();
	}
	return NULL;
}

void
SP_DS_Graph::SetHide(SP_Data* p_pcData, bool p_bHide, bool p_bIncludeGraphics)
{
	p_pcData->SetHide(p_bHide);

	if(p_pcData->GetAttributes())
	{
		for(SP_DS_Attribute* l_pcAttr : *(p_pcData->GetAttributes()))
		{
			SetHide(l_pcAttr, p_bHide);
		}
	}

	if(p_bIncludeGraphics)
	{
		for(SP_Graphic* l_pcGr : *(p_pcData->GetGraphics()))
		{
			if(p_pcData->GetNetnumber() != l_pcGr->GetNetnumber())
			{
				SP_Data* l_pcNode = GetCoarseNode(l_pcGr->GetNetnumber());
				if(l_pcNode)
				{
					SP_DS_Coarse* l_pcCoarse = l_pcNode->GetCoarse();
					if(!l_pcCoarse->GetCoarseDoc())
					{
						l_pcCoarse->SetUpdate(false);
						l_pcCoarse->Show();
						l_pcCoarse->GetCoarseDoc()->SetClose(false);
						l_pcCoarse->GetCoarseDoc()->Modify(FALSE);
						l_pcCoarse->GetCoarseDoc()->Close();
					}
				}
			}
			l_pcGr->SetHide(p_bHide);

			for(SP_Graphic* l_pcGrChild : *(l_pcGr->GetGraphicChildren()))
			{
				SP_Data* l_pcNode = GetCoarseNode(l_pcGrChild->GetNetnumber());
				if(l_pcNode)
				{
					SP_DS_Coarse* l_pcCoarse = l_pcNode->GetCoarse();
					if(!l_pcCoarse->GetCoarseDoc())
					{
						l_pcCoarse->SetUpdate(false);
						l_pcCoarse->Show();
						l_pcCoarse->GetCoarseDoc()->SetClose(false);
						l_pcCoarse->GetCoarseDoc()->Modify(FALSE);
						l_pcCoarse->GetCoarseDoc()->Close();
					}
				}
				l_pcGrChild->SetHide(p_bHide);
			}
		}
	}
	p_pcData->Update(false);
}

bool
SP_DS_Graph::Update()
{
    bool l_bReturn = TRUE;

    SP_ListNodeclass::const_iterator l_itNC;
    for (l_itNC = m_lNodeclass.begin(); l_itNC != m_lNodeclass.end(); ++l_itNC)
    	l_bReturn &= (*l_itNC)->Update();

    SP_ListEdgeclass::const_iterator l_itEC;
    for (l_itEC = m_lEdgeclass.begin(); l_itEC != m_lEdgeclass.end(); ++l_itEC)
    	l_bReturn &= (*l_itEC)->Update();

    SP_ListMetadataclass::const_iterator l_itMC;
    for (l_itMC = m_lMetadataclass.begin(); l_itMC != m_lMetadataclass.end(); ++l_itMC)
    	l_bReturn &= (*l_itMC)->Update();

    GetParentDoc()->Refresh();

    return l_bReturn;
}

SP_DS_FunctionRegistry*
SP_DS_Graph::GetFunctionRegistry()
{
	return m_pcFunctionRegistry;
}
