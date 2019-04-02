//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_gui/mdi/SP_MDI_View.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
////////#include "wx/choicdlg.h"
#include "wx/msgdlg.h"
#include "sp_utilities.h"
#include "sp_gui/management/SP_GM_Docmanager.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "sp_gui/dialogs/SP_DLG_GraphProperties.h"
#include "sp_gui/dialogs/SP_DLG_TransformGraphic.h"
#include "sp_gui/dialogs/SP_DLG_ChoseCoarse.h"
#include "sp_gui/dialogs/SP_DLG_SearchId.h"
#include "sp_gui/dialogs/SP_DLG_SortNodes.h"
#include "sp_gui/dialogs/SP_DLG_CheckDuplicateNodes.h"
#include "sp_gui/dialogs/SP_DLG_NetInformations.h"
#include "sp_gui/dialogs/SP_DLG_ChooseInvariant.h"
#include "sp_gui/dialogs/SP_DLG_IAConfigure.h"
#include "sp_gui/dialogs/SP_DLG_IARegCmdList.h"
#include "sp_gui/dialogs/SP_DLG_ReduceFTree.h"
#include "sp_gui/dialogs/SP_DLG_LayoutProperties.h"
#include "sp_gui/dialogs/SP_DLG_DuplicateNodes.h"
#include "sp_gui/dialogs/SP_DLG_MergeNodes.h"
#include "sp_ds/extensions/FTree/SP_DS_FTreeDeMorganTransformer.h"
#include "sp_gui/dialogs/dia_SPN/SP_DLG_StSimulationResults.h"
#include "sp_gui/dialogs/dia_ContinuousPN/SP_DLG_CPNSimulationResults.h"
#include "sp_gui/dialogs/dia_ColCPN/SP_DLG_ColCPNSimulationResults.h"
#include "sp_gui/dialogs/dia_ColSPN/SP_DLG_ColStSimulationResults.h"
#include "sp_gui/dialogs/dia_ColHPN/SP_DLG_ColHPNSimultionResults.h"
#include"sp_gui/dialogs/dia_FPN/SP_DLG_FCPNSimulationResults.h"
#include "sp_gui/dialogs/dia_FPN/SP_DLG_FspnSimResult.h"
#include "sp_gui/dialogs/dia_FPN/SP_DLG_FHybridSimultionResults.h"

//AMR
#include "sp_gui/dialogs/dia_ColCPN/SP_DLG_ColCPNDirSimulationResults.h"
//#include "sp_gui/dialogs/dia_ColHPN/SP_DLG_ColHPNDirSimulationResults.h"
//#include "sp_gui/dialogs/dia_ColSPN/SP_DLG_ColStDirSimResults.h"



#include "sp_gui/dialogs/dia_HybridPN/SP_DLG_HybridSimultionResults.h"
#include "sp_gui/dialogs/SP_DLG_ConvertElements.h"

#include "sp_core/SP_Core.h"
#include "sp_ds/extensions/SP_DS_Coarse.h"
#include "sp_gr/SP_GR_Node.h"
#include "sp_gr/SP_GR_Edge.h"

#include <wx/filename.h>
#include <wx/wupdlock.h>

#include "snoopy.h"

#include "sp_core/SP_GPR_Canvas.h"
#include "sp_gui/commands/SP_CMD_Edit.h"




#include "sp_ds/attributes/SP_DS_NameAttribute.h"

#include "sp_ds/extensions/Col_PN/Folding/SP_CPN_GenMasterNet.h"
#include "sp_ds/extensions/Col_PN/Folding/SP_CPN_GenTwinNet.h"
#include "sp_gui/dialogs/dia_CPN/SP_DLG_TwinNetSetting.h"

#include "sp_ds/extensions/Col_PN/SyntaxChecking/SP_CPN_SyntaxChecking.h"

#include "sp_gui/dialogs/SP_DLG_CheckNet.h"

#include "sp_ds/extensions/HybridPN/SP_DS_CheckTransitions.h"

//Amr
#include "sp_gui/dialogs/SP_DLG_DirColorSimulation.h"
#include "sp_gui/dialogs/dia_SteeringGUI/SP_GUI_ConnectToServer.h"



#include <wx/busyinfo.h> 


IMPLEMENT_DYNAMIC_CLASS(SP_MDI_View, wxView)

BEGIN_EVENT_TABLE(SP_MDI_View, wxView)
EVT_MENU(wxID_CUT, SP_MDI_View::OnCut)
EVT_MENU(wxID_COPY, SP_MDI_View::OnCopy)
EVT_MENU(wxID_PASTE, SP_MDI_View::OnPaste)
EVT_MENU(SP_EDIT_COPY_IN_NEW_NET, SP_MDI_View::OnCopyInNewNet)
EVT_MENU(SP_EDIT_SELECT_ALL_ID, SP_MDI_View::OnSelectAll)
EVT_MENU(SP_EDIT_PROPERTIES_ID, SP_MDI_View::OnEditProperties)
EVT_MENU(SP_EDIT_TRANSFORM_ID, SP_MDI_View::OnTransformShapes)
EVT_MENU(SP_ID_DELETE, SP_MDI_View::OnDelete)
EVT_MENU(SP_MENU_ITEM_DELETE_ALL, SP_MDI_View::OnDeleteAll)
EVT_MENU(SP_EDIT_HIDE_ID, SP_MDI_View::OnHide)
EVT_MENU(SP_EDIT_HIDEALL_ID, SP_MDI_View::OnHide)
EVT_MENU(SP_EDIT_UNHIDE_ID, SP_MDI_View::OnUnHide)
EVT_MENU(SP_EDIT_UNHIDEALL_ID, SP_MDI_View::OnUnHide)
// special, generated from inside the tree control that holds the graph elements
EVT_MENU(SP_MENU_ITEM_TREE_SELECT_ALL, SP_MDI_View::OnSelectAllClass)
EVT_MENU(SP_MENU_ITEM_TREE_EDIT_PROPERTIES, SP_MDI_View::OnEditPropertiesClass)
// in case, a graph has global options in the view menu of the frame
EVT_MENU(SP_MENU_ITEM_EDIT_SHOW, SP_MDI_View::OnEditGlobalOptions)
// in case the graph has SP_DS_IdAttributes with the possibility to be 'squeezed'
EVT_MENU(SP_MENU_ITEM_SQUEEZE, SP_MDI_View::OnSqueeze)
EVT_MENU(SP_MENU_ITEM_SORTNODES, SP_MDI_View::OnSortNodes)
EVT_MENU(SP_MENU_ITEM_CHECKDUPLICATENODES, SP_MDI_View::OnCheckDuplicateNodes)
EVT_MENU(SP_MENU_ITEM_NETINFORMATIONS, SP_MDI_View::OnNetInformations)
EVT_MENU(wxID_ZOOM_100, SP_MDI_View::OnZoom)
EVT_MENU(wxID_ZOOM_IN, SP_MDI_View::OnZoom)
EVT_MENU(wxID_ZOOM_OUT, SP_MDI_View::OnZoom)
EVT_MENU(wxID_REFRESH, SP_MDI_View::OnRefresh)
EVT_MENU(SP_MENU_ITEM_LAYOUT, SP_MDI_View::OnLayout)
// displaying everytime
EVT_MENU(SP_MENU_ITEM_FLATTEN, SP_MDI_View::OnFlatten)
EVT_MENU(SP_MENU_ITEM_COARSE, SP_MDI_View::OnCoarse)
// view animation
EVT_MENU(SP_MENU_ITEM_TOGGLEANIM, SP_MDI_View::OnToggleAnim)

//by sl
//highleiten marking edges
EVT_MENU(SP_MENU_ITEM_HIGHMARKING, SP_MDI_View::OnToggleMarking)

// view animation
EVT_MENU(SP_MENU_ITEM_TOGGLESIMULATION, SP_MDI_View::OnStartSimulation)
//by Amr
EVT_MENU(SP_MENU_ITEM_TOGGLEDIRSIMULATION, SP_MDI_View::OnStartDirSimulation)
// search id
EVT_MENU(SP_MENU_ITEM_SEARCHID, SP_MDI_View::OnSearchId)
EVT_MENU(SP_MENU_ITEM_LOAD_INVARIANT_FILE, SP_MDI_View::OnLoadInvariantFile)
EVT_MENU(SP_MENU_ITEM_IA_TRIGGERGENERAL, SP_MDI_View::OnIATriggerGeneral)
EVT_MENU(SP_MENU_ITEM_IA_TRIGGERSINGLE, SP_MDI_View::OnIATriggerSingle)
EVT_MENU(SP_MENU_ITEM_IA_TRIGGERMAKRO, SP_MDI_View::OnIATriggerMakro)
EVT_MENU(SP_MENU_ITEM_IA_UNDOLASTACTION, SP_MDI_View::OnIAUndoLastAction)
EVT_MENU(SP_MENU_ITEM_IA_CONFIGURE, SP_MDI_View::OnIAConfigure)
EVT_MENU(SP_MENU_ITEM_IA_REGCMDLIST, SP_MDI_View::OnIARegCmdList)
EVT_MENU_RANGE(SP_MENU_ITEM_IA_SUBMENUITEM, SP_MENU_ITEM_IA_SUBMENUITEM_LAST, SP_MDI_View::OnIASubMenuSelection)

EVT_MENU(SP_MENU_ITEM_MERGE, SP_MDI_View::OnMergeNodes)
EVT_MENU(SP_MENU_ITEM_DUPLICATE, SP_MDI_View::OnDuplicateNodes)

EVT_MENU(SP_MENU_CONVERT_ELEMENT_MENU, SP_MDI_View::OnConvertElement)

//folding
EVT_MENU(SP_MENU_ITEM_GENERATE_MASTER_NET, SP_MDI_View::OnGenerateMasterNets)
EVT_MENU(SP_MENU_ITEM_GENERATE_TWIN_NET, SP_MDI_View::OnGenerateTwinNets)
EVT_MENU(SP_MENU_ITEM_COLORIZE_ANY_NET, SP_MDI_View::OnColorizeAnyNets)

//metadata
EVT_MENU(SP_MENU_ITEM_MD_GENERAL, SP_MDI_View::OnMDEditGeneral)
//Fault Tree
EVT_MENU(SP_MENU_ITEM_REDUCTION_RULES, SP_MDI_View::OnReduceFTree)
EVT_MENU(SP_MENU_ITEM_DEMORGAN_TRANSFORMER, SP_MDI_View::OnTransformFTree)

EVT_UPDATE_UI_RANGE(SP_EDIT_PROPERTIES_ID, SP_ID_DELETE, SP_MDI_View::OnUpdateUI)
EVT_UPDATE_UI_RANGE(SP_MENU_ITEM_TREE_SELECT_ALL, SP_MENU_ITEM_FLATTEN, SP_MDI_View::OnUpdateUI)
EVT_UPDATE_UI(wxID_CLEAR, SP_MDI_View::OnUpdateUI)
EVT_UPDATE_UI(wxID_CUT, SP_MDI_View::OnUpdateUI)
EVT_UPDATE_UI(wxID_COPY, SP_MDI_View::OnUpdateUI)
EVT_UPDATE_UI(wxID_PASTE, SP_MDI_View::OnUpdateUI)
EVT_UPDATE_UI(SP_MENU_ITEM_IA_TRIGGERGENERAL, SP_MDI_View::OnUpdateUI)
EVT_UPDATE_UI(SP_MENU_ITEM_IA_TRIGGERSINGLE, SP_MDI_View::OnUpdateUI)
EVT_UPDATE_UI(SP_MENU_ITEM_IA_TRIGGERMAKRO, SP_MDI_View::OnUpdateUI)
EVT_UPDATE_UI(SP_MENU_ITEM_TOGGLESIMULATION, SP_MDI_View::OnUpdateUI)
EVT_UPDATE_UI(SP_MENU_CONVERT_ELEMENT_MENU, SP_MDI_View::OnUpdateUI)

END_EVENT_TABLE()

SP_MDI_View::SP_MDI_View() :
	m_pcFrame(NULL), m_pcCanvas(NULL), m_nNetnumber(0),
			m_eType(SP_VIEW_SIMPLE), m_bClose(TRUE),
			m_bFrameWasMaximized(FALSE)
{
}

SP_MDI_View::~SP_MDI_View()
{
}

bool SP_MDI_View::OnCreate(wxDocument *p_pcDoc, long p_nFlags)
{
	if (!p_pcDoc)
		return FALSE;

	wxMenu* l_pcEditMenu= NULL;
	SP_MDI_Doc* l_pcDoc = dynamic_cast<SP_MDI_Doc*>(p_pcDoc);

	wxString l_Netclass = l_pcDoc->GetDocumentTemplate()->GetDescription();
	if(l_pcDoc->GetType() == SP_DOC_COARSE)
	{
		l_Netclass = l_pcDoc->GetParentDoc()->GetDocumentTemplate()->GetDescription();
	}

	wxSize l_Size(wxGetApp().GetCanvasPrefs()->GetWindowSizeX(l_Netclass),
					wxGetApp().GetCanvasPrefs()->GetWindowSizeY(l_Netclass));
	m_pcFrame = new SP_GUI_Childframe(l_pcDoc, this, &l_pcEditMenu, l_Size);

	int l_nWidth = 100, l_nHeight = 100;
	m_pcFrame->GetClientSize(&l_nWidth, &l_nHeight);

	m_pcCanvas
			= new SP_GUI_Canvas(this, m_pcFrame, 1000);
//					, wxPoint(0, 0), wxSize(l_nWidth, l_nHeight), wxSUNKEN_BORDER | wxHSCROLL | wxVSCROLL);

	SetFrame(m_pcFrame);
	ShowFrame();

	m_pcCanvas->SetDiagram(l_pcDoc->GetDiagram());
	l_pcDoc->GetDiagram()->SetCanvas(m_pcCanvas);

	l_pcDoc->GetDiagram()->SetGridSpacing((double) (wxGetApp().GetCanvasPrefs())
								->GetGridSpacing(l_Netclass));
	l_pcDoc->GetDiagram()->SetGridVisible((wxGetApp().GetCanvasPrefs())
								->GridVisible(l_Netclass));
	l_pcDoc->GetDiagram()->SetSnapToGrid((wxGetApp().GetCanvasPrefs())
								->GridSnappingEnabled(l_Netclass));

	l_pcDoc->DrawAllElements();

	return TRUE;
}

bool SP_MDI_View::SetEditElement(SP_Type* p_pcElement)
{
	if (!m_pcCanvas)
		return FALSE;

	return (m_pcCanvas->SetEditElement(p_pcElement) != NULL);
}

bool SP_MDI_View::ShowFrame()
{
	if (!m_pcFrame)
		return FALSE;

	m_pcFrame->Show(TRUE);
	m_pcFrame->Raise();
	m_pcFrame->Activate();

	if (m_bFrameWasMaximized)
		m_pcFrame->Maximize(TRUE);

	return TRUE;
}

bool SP_MDI_View::DrawAllElements(SP_DS_Graph* p_pcGraph, bool p_bLocalOnly)
{
	if (!m_pcCanvas)
		return FALSE;

	return m_pcCanvas->DrawAllElements(p_pcGraph, p_bLocalOnly);
}

// Sneakily gets used for default print/preview
void SP_MDI_View::OnDraw(wxDC* p_pcDC)
{
	SP_MDI_Doc *l_pcDoc = dynamic_cast<SP_MDI_Doc*>(GetDocument());
	wxNode *l_pcNode = l_pcDoc->GetDiagram()->GetShapeList()->GetFirst();
	while (l_pcNode)
	{
		wxShape *l_pcShape = (wxShape*)l_pcNode->GetData();
		l_pcShape->Select(FALSE);
		l_pcShape->Draw(*p_pcDC);
		l_pcNode = l_pcNode->GetNext();
	}
}

void SP_MDI_View::OnUpdate(wxView *p_pcSender, wxObject *p_pcHint)
{
	m_pcFrame->Show(true);
}

void SP_MDI_View::OnUpdateUI(wxUpdateUIEvent& p_cEvent)
{
	SP_MDI_Doc *l_pcDoc = dynamic_cast<SP_MDI_Doc*>(GetDocument());

	switch (p_cEvent.GetId())
	{
	case SP_MENU_ITEM_IA_TRIGGERGENERAL:
	case SP_MENU_ITEM_IA_TRIGGERSINGLE:
	case SP_MENU_ITEM_IA_TRIGGERMAKRO:
		if (l_pcDoc && wxGetApp().GetIAManager())
		{
			p_cEvent.Enable(wxGetApp().GetIAManager()->IAModeEnabled(l_pcDoc->GetGraph()));
		}
		break;
	default:
		p_cEvent.Enable(!(SP_Core::Instance()->GetAnimMode() || SP_Core::Instance()->GetSimulationMode()));
	    break;
	}
}

bool SP_MDI_View::OnClose(bool p_bDeleteWindow)
{
	// animation mode active?
	if (SP_Core::Instance()->GetAnimMode())
		return FALSE;

	// simulation mode active?
	if (SP_Core::Instance()->GetSimulationMode())
		return FALSE;

	// no document?! or the document (the user) vetoes the closing
	if (!GetDocument() || !GetDocument()->Close())
		return FALSE;

	SelectAll(FALSE);

	SP_MDI_Doc* l_pcDoc = dynamic_cast<SP_MDI_Doc*>(GetDocument());
	l_pcDoc->GetDiagram()->SetCanvas(NULL);

	//    m_pcCanvas->Clear();
	m_pcCanvas->SetDiagram(NULL);
	m_pcCanvas->SetView(NULL);
	m_pcCanvas = NULL;

	wxMenu* l_pcFileMenu = m_pcFrame->GetMenuBar()->GetMenu(0);

	// Remove file menu from those managed by the command history
	wxGetApp().GetDocmanager()->FileHistoryRemoveMenu(l_pcFileMenu);

	Activate(FALSE);
	m_pcFrame->Show(FALSE);
	m_pcFrame->CloseWindow();

	if (p_bDeleteWindow)
	{
		m_pcFrame->Destroy();
	}

	if ((l_pcDoc->GetType() == SP_DOC_SIMPLE) && (l_pcDoc == SP_Core::Instance()->GetRootDocument()))
		SP_Core::Instance()->SetRootDocument(NULL);

	return TRUE;
}

void SP_MDI_View::OnSelectAll(wxCommandEvent& p_cEvent)
{
	SelectAll(TRUE);

	Refresh();
}

void SP_MDI_View::OnEditProperties(wxCommandEvent& p_cEvent)
{
	wxList l_lShapes;
	SP_ListGraphic l_lGraphic;
	wxString l_sLastClass = wxT("");
	bool l_bDifferent= FALSE;
	FindSelectedShapes(l_lShapes);

	l_lGraphic.clear();
	// filter out attributes
	wxNode* l_pcNode = l_lShapes.GetFirst();
	while (l_pcNode)
	{
		SP_Graphic* l_pcGraphic = SP_Core::Instance()->ResolveExtern(dynamic_cast<wxShape*>(l_pcNode->GetData()));
		if (l_pcGraphic && l_pcGraphic->GetGraphicType()
				!= SP_GRAPHIC_ATTRIBUTE)
		{
			if (l_sLastClass.IsEmpty())
				l_sLastClass = l_pcGraphic->GetParent()->GetClassName();

			l_lGraphic.push_back(l_pcGraphic);
			// try to determine whether we have different shapes selected
			l_bDifferent = l_bDifferent ? l_bDifferent
					: l_sLastClass.Cmp(l_pcGraphic->GetParent()->GetClassName()) != 0;
		}

		l_pcNode = l_pcNode->GetNext();
	}

	DoEditProperties(&l_lGraphic, l_lGraphic.size() > 1);

	Refresh();
}

void SP_MDI_View::OnEditGlobalOptions(wxCommandEvent& p_cEvent)
{
	SP_MDI_Doc* l_pcDoc = dynamic_cast<SP_MDI_Doc*>(GetDocument());
	if (l_pcDoc && l_pcDoc->GetGraph())
	{
		SP_DLG_GraphProperties l_cDlg(NULL, l_pcDoc->GetGraph());
		if (l_cDlg.ShowModal() == wxID_OK)
		{
			l_pcDoc->GetGraph()->OnGlobalOptions();
		}
		l_cDlg.Destroy();

		RefreshAllViews();
	}
}

void SP_MDI_View::OnSqueeze(wxCommandEvent& p_cEvent)
{
	SP_MDI_Doc* l_pcDoc = dynamic_cast<SP_MDI_Doc*>(GetDocument());
	if (l_pcDoc && l_pcDoc->GetGraph())
	{
		l_pcDoc->GetGraph()->SqueezeIdAttributes();
		Modify(TRUE);
		Refresh(TRUE);
	}
}

void SP_MDI_View::OnLayout(wxCommandEvent& p_cEvent)
{
	SP_MDI_Doc* l_pcDoc = dynamic_cast<SP_MDI_Doc*>(GetDocument());
	if (l_pcDoc)
	{
		SP_DLG_LayoutProperties* l_pcDlg = new SP_DLG_LayoutProperties(NULL, l_pcDoc);
		l_pcDlg->ShowModal();
		l_pcDlg->Destroy();
	}

}

void SP_MDI_View::OnZoom(wxCommandEvent& p_cEvent)
{
	double l_nScaleX = m_pcCanvas->GetScaleX();
	double l_nScaleY = m_pcCanvas->GetScaleY();

	switch (p_cEvent.GetId())
	{
	case wxID_ZOOM_IN:
		m_pcCanvas->SetScale(l_nScaleX*1.10, l_nScaleY*1.10);
		break;
	case wxID_ZOOM_OUT:
		m_pcCanvas->SetScale(l_nScaleX/1.10, l_nScaleY/1.10);
		break;
	default:
		m_pcCanvas->SetScale(1, 1);
	    break;
	}
	m_pcCanvas->Refresh();
}

void SP_MDI_View::OnRefresh(wxCommandEvent& p_cEvent)
{
	DoRefresh();
}

void SP_MDI_View::DoRefresh()
{
	if (m_pcCanvas)
	{
		SelectAll(true);
		m_pcCanvas->MoveShapes(0,0);
		SelectAll(false);
	}

	SP_MDI_Doc* l_pcDocument = dynamic_cast<SP_MDI_Doc*>(GetDocument());
	l_pcDocument->GetGraph()->Update();
}

void SP_MDI_View::OnCoarse(wxCommandEvent& p_cEvent)
{
	// coarse means to take all selected nodes and test their edges,
	// connecting them. only coarse edges, if both ends are selected, too.
	wxList l_lWxShapes;
	wxShape* l_pcShape;
	SP_ListGraphic l_lShapes;
	SP_ListGraphic l_lEdges;
	SP_ListGraphic::iterator l_Iter;
	double l_nMinX = 10000, l_nMaxX = -1, l_nMinY = 10000, l_nMaxY = -1;
	double l_nX1, l_nX2, l_nY1, l_nY2;
	// get unfiltered list of selections
	FindSelectedShapes(l_lWxShapes);

	// filter out everything but nodes
	wxNode* l_pcNode = l_lWxShapes.GetFirst();
	while (l_pcNode)
	{
		l_pcShape = dynamic_cast<wxShape*>(l_pcNode->GetData());
		SP_Graphic* l_pcGraphic = SP_Core::Instance()->ResolveExtern(l_pcShape);
		if (l_pcGraphic && l_pcGraphic->GetGraphicType() == SP_GRAPHIC_NODE)
			l_lShapes.push_back(l_pcGraphic);

		l_pcNode = l_pcNode->GetNext();
	}
	SelectAll(FALSE);
	for (l_Iter = l_lShapes.begin(); l_Iter != l_lShapes.end(); ++l_Iter)
	{
		(*l_Iter)->Select(TRUE);
		(*l_Iter)->GetBoundingBox(&l_nX1, &l_nY1, &l_nX2, &l_nY2);
		l_nMinX = wxMin(l_nMinX, l_nX1);
		l_nMinY = wxMin(l_nMinY, l_nY1);
		l_nMaxX = wxMax(l_nMaxX, l_nX2);
		l_nMaxY = wxMax(l_nMaxY, l_nY2);
	}
	// add edges, that have both ends selected to the parameter
	FindConnectedEdges(&l_lEdges);
	// show selection
	for (l_Iter = l_lEdges.begin(); l_Iter != l_lEdges.end(); ++l_Iter)
	{
		l_lShapes.push_back((*l_Iter));
		(*l_Iter)->Select(TRUE);
	}

	Refresh();

	DoCoarse(&l_lShapes, l_nMinX + ((l_nMaxX - l_nMinX) / 2.0), l_nMinY
			+ ((l_nMaxY - l_nMinY) / 2.0));
}

bool SP_MDI_View::DoCoarse(SP_ListGraphic* p_plShapes, double p_nX,
		double p_nY, const wxString& p_sCoarseNode)
{
	if (!p_plShapes || p_plShapes->size() == 0 || !GetDocument())
		return FALSE;

	SP_MDI_Doc* l_pcDoc = dynamic_cast<SP_MDI_Doc*>(GetDocument());
	SP_DS_Graph* l_pcGraph = l_pcDoc->GetGraph();
	SP_ListGraphic::iterator l_Iter;
	SP_Graphic* l_pcCoarseGr;
	wxString l_sNodeClass = p_sCoarseNode;
	if(l_sNodeClass.IsEmpty())
	{
		SP_DLG_ChoseCoarse l_cDlg(NULL);
		if (!l_cDlg.SetChoices(l_pcGraph))
			return FALSE;

		int l_nRet = l_cDlg.ShowModal();
		if (l_nRet != wxID_OK)
		{
			return FALSE;
		}

		l_sNodeClass = l_cDlg.GetResult();
	}

	if (!l_pcGraph->GetNodeclass(l_sNodeClass))
		return FALSE;

	SP_DS_Node* l_pcNode = l_pcGraph->GetNodeclass(l_sNodeClass)->NewElement(m_nNetnumber);
	if (!l_pcNode || !l_pcNode->GetCoarse())
		return FALSE;

	l_pcNode->ShowOnCanvas(m_pcCanvas, TRUE, p_nX, p_nY);
	l_pcCoarseGr = (*l_pcNode->GetGraphics()->begin());

	if(!CheckCoarseConsistency(p_plShapes, l_pcNode))
	{
		l_pcNode->RemoveAllGraphics(true);
		l_pcNode->Remove();
		wxDELETE(l_pcNode);
		return false;
	}

	SelectAll(FALSE);
	for (l_Iter = p_plShapes->begin(); l_Iter != p_plShapes->end(); ++l_Iter)
	{
		(*l_Iter)->Coarse(l_pcNode->GetCoarse()->GetNetnumber(), l_pcCoarseGr, p_plShapes);
	}

	if (m_pcCanvas)
	{
		m_pcCanvas->MoveShape(l_pcCoarseGr->GetPrimitive(), 0, 0);
	}

	SP_Core::Instance()->RemoveQueuedElements();
	if (l_pcDoc)
	{
		l_pcDoc->GetGraph()->ResetCoarseTree();
		l_pcDoc->Modify(true);
	}
	Refresh();

	return TRUE;
}

bool SP_MDI_View::CheckCoarseConsistency(SP_ListGraphic* p_plShapes, SP_DS_Node* p_pcCoarseNode)
{
	//Check if course is possible
	wxString l_sCoarseName = p_pcCoarseNode->GetCoarse()->GetInnerClass();
	SP_ListGraphic::iterator l_Iter;
	if(p_plShapes->size() == 1)
	{
		l_Iter = p_plShapes->begin();
		if((*l_Iter)->GetParent()->GetCoarse())
		{
			if(!l_sCoarseName.IsSameAs((*l_Iter)->GetParent()->GetCoarse()->GetInnerClass()))
			{
				SP_LOGDEBUG(wxT("Error: You selected the wrong coarse type, which is not allowed!!"));
				SP_MESSAGEBOX(wxT("Error: You selected the wrong coarse type!!\nCoarse will be aborted!"), wxT("Error"), wxOK);
				return false;
			}
		}
	}
	for (l_Iter = p_plShapes->begin(); l_Iter != p_plShapes->end(); ++l_Iter)
	{
		SP_DS_Node* l_pcShapeNode = dynamic_cast<SP_DS_Node*>((*l_Iter)->GetParent());
		if(l_pcShapeNode)
		{
			const SP_ListEdge* l_plEdges = l_pcShapeNode->GetSourceEdges();
			SP_ListEdge::const_iterator l_eIter;
			for(l_eIter = l_plEdges->begin(); l_eIter != l_plEdges->end(); l_eIter++)
			{
				SP_GR_Edge* l_pcGrEdge = dynamic_cast<SP_GR_Edge*>((*l_eIter)->GetGraphicInSubnet(m_nNetnumber));
				if(l_pcGrEdge)
				{
					SP_Graphic* l_pcGrTrg = l_pcGrEdge->GetTarget();
					SP_Data* l_pcTrg = (*l_eIter)->GetTarget();
					if(SP_Find(*p_plShapes, l_pcGrTrg) == p_plShapes->end())
					{
						//Check if border node is coarse node, which is not allowed
						if(l_pcGrTrg->GetParent()->GetCoarse())
						{
							SP_LOGDEBUG(wxT("Error: Border node is coarse node!!"));
							SP_MESSAGEBOX(wxT("Error: Border node is coarse node!!\nCoarse will be aborted!"), wxT("Error"), wxOK);
							return false;
						}
						//Check if border node and coarse node are from the same type
						wxString l_sTrgClassName = l_pcTrg->GetClassName();
						if( l_sCoarseName.IsSameAs(l_sTrgClassName) )
						{
							SP_LOGDEBUG(wxT("Attention: Border node and coarse node are from the same type!!"));
							if(SP_MESSAGEBOX(wxT("Attention: Border node and coarse node are from the same type!!\nDo you really want to continue?"),
								wxT("Attention"), wxYES_NO) != wxYES)
							{
								return false;
							}
						}
					}
				}
			}
			l_plEdges = l_pcShapeNode->GetTargetEdges();
			for(l_eIter = l_plEdges->begin(); l_eIter != l_plEdges->end(); l_eIter++)
			{
				SP_GR_Edge* l_pcGrEdge = dynamic_cast<SP_GR_Edge*>((*l_eIter)->GetGraphicInSubnet(m_nNetnumber));
				if(l_pcGrEdge)
				{
					SP_Graphic* l_pcGrSrc = l_pcGrEdge->GetSource();
					SP_Data* l_pcSrc = (*l_eIter)->GetSource();
					if(SP_Find(*p_plShapes, l_pcGrSrc) == p_plShapes->end())
					{
						//Check if border node is coarse node, which is not allowed
						if(l_pcGrSrc->GetParent()->GetCoarse())
						{
							SP_LOGDEBUG(wxT("Error: Border node is coarse node!!"));
							SP_MESSAGEBOX(wxT("Error: Border node is coarse node!!\nCoarse will be aborted!"), wxT("Error"), wxOK);
							return false;
						}
						//Check if border node and coarse node are from the same type
						wxString l_sSrcClassName = l_pcSrc->GetClassName();
						if( l_sCoarseName.IsSameAs(l_sSrcClassName) )
						{
							SP_LOGDEBUG(wxT("Attention: Border node and coarse node are from the same type"));
							if(SP_MESSAGEBOX(wxT("Attention: Border node and coarse node are from the same type!!\nDo you really want to continue?"),
								wxT("Attention"), wxYES_NO) != wxYES)
							{
								return false;
							}
						}
					}
				}
			}
		}

	}
	//End Check
	return true;
}
void SP_MDI_View::OnFlatten(wxCommandEvent& p_cEvent)
{
	// flatten means to take all graphics in the subnet, determined by
	// the selected coarse-node(s) and add them to the actual view
	wxList l_lShapes;
	wxShape* l_pcShape;
	SP_ListGraphic l_lGraphic;
	FindSelectedShapes(l_lShapes);

	// filter out everything but coarse nodes
	wxNode* l_pcNode = l_lShapes.GetFirst();
	while (l_pcNode)
	{
		l_pcShape = dynamic_cast<wxShape*>(l_pcNode->GetData());
		SP_Graphic* l_pcGraphic = SP_Core::Instance()->ResolveExtern(l_pcShape);
		if (l_pcGraphic && l_pcGraphic->GetGraphicState() == SP_STATE_COARSECOARSE)
		{
			l_lGraphic.push_back(l_pcGraphic);
		}

		l_pcNode = l_pcNode->GetNext();
	}
	SelectAll(FALSE);

	// show selection of the chosen nodes
	SP_ListGraphic::iterator l_Iter;
	for (l_Iter = l_lGraphic.begin(); l_Iter != l_lGraphic.end(); ++l_Iter)
		(*l_Iter)->Select(TRUE);

	Refresh();

	DoFlatten(&l_lGraphic);
}

bool SP_MDI_View::DoFlatten(SP_ListGraphic* p_plShapes)
{
	if (!p_plShapes || p_plShapes->size() == 0)
		return FALSE;

	int l_nReturn = SP_MESSAGEBOX(wxT("Do you really want to flatten the selected nodes?"), wxT("Flatten"), wxYES_NO | wxICON_QUESTION);
	if (l_nReturn != wxYES)
		return FALSE;

	bool l_bReturn= TRUE;
	SP_ListGraphic::iterator l_Iter;
	SP_ListGraphic l_lGraphics;
	for (l_Iter = p_plShapes->begin(); l_bReturn && l_Iter != p_plShapes->end(); ++l_Iter)
	{
		l_bReturn &= (*l_Iter)->Flatten(GetNetnumber(), &l_lGraphics);
		if (l_bReturn)
			l_bReturn &= (*l_Iter)->AddToDeleteQueue();
	}

	if (m_pcCanvas)
	{
		for (l_Iter = l_lGraphics.begin(); l_Iter != l_lGraphics.end(); ++l_Iter)
		{
			// draw it
			(*l_Iter)->AddToCanvas(m_pcCanvas, -1, -1);
			// select it, if it isn't an attribute, because they will get selected by their parents
			if ((*l_Iter)->GetGraphicType() != SP_GRAPHIC_ATTRIBUTE)
				(*l_Iter)->Select(TRUE);
			// move it by 0,0 if it is a node
			if ((*l_Iter)->GetPrimitive() && ((*l_Iter)->GetGraphicType() == SP_GRAPHIC_NODE))
				m_pcCanvas->MoveShape((*l_Iter)->GetPrimitive(), 0, 0);
		}
	}

	l_bReturn &= SP_Core::Instance()->RemoveQueuedElements();
	SP_MDI_Doc* l_pcDoc = dynamic_cast<SP_MDI_Doc*>(GetDocument());
	if (l_pcDoc)
	{
		l_pcDoc->GetGraph()->ResetCoarseTree();
		l_pcDoc->Modify(true);
	}
	Refresh();

	return l_bReturn;
}

void SP_MDI_View::OnSelectAllClass(wxCommandEvent& p_cEvent)
{
	if (!m_pcCanvas || !m_pcCanvas->GetEditElement())
		return;

	wxString l_sClassname = wxT("");
	switch (m_pcCanvas->GetEditElement()->GetElementType())
	{
	case SP_ELEMENT_NODECLASS:
		l_sClassname = static_cast<SP_DS_Nodeclass*>(m_pcCanvas->GetEditElement())->GetName();
		break;
	case SP_ELEMENT_EDGECLASS:
		l_sClassname = static_cast<SP_DS_Edgeclass*>(m_pcCanvas->GetEditElement())->GetName();
		break;
	case SP_ELEMENT_METADATACLASS:
		l_sClassname = static_cast<SP_DS_Metadataclass*>(m_pcCanvas->GetEditElement())->GetName();
		break;
	default:
		break;
	}

	SelectAll(FALSE);
	SelectAllClass(l_sClassname);

	Refresh();
}

void SP_MDI_View::OnEditPropertiesClass(wxCommandEvent& p_cEvent)
{
	if (!m_pcCanvas || !m_pcCanvas->GetEditElement())
		return;

	wxString l_sClassname = wxT("");
	switch (m_pcCanvas->GetEditElement()->GetElementType())
	{
	case SP_ELEMENT_NODECLASS:
		l_sClassname = static_cast<SP_DS_Nodeclass*>(m_pcCanvas->GetEditElement())->GetName();
		break;
	case SP_ELEMENT_EDGECLASS:
		l_sClassname = static_cast<SP_DS_Edgeclass*>(m_pcCanvas->GetEditElement())->GetName();
		break;
	case SP_ELEMENT_METADATACLASS:
		l_sClassname = static_cast<SP_DS_Metadataclass*>(m_pcCanvas->GetEditElement())->GetName();
		break;
	default:
		break;
	}

	DoEditPropertiesClass(l_sClassname);

	Refresh();
}

bool SP_MDI_View::DoEditProperties(SP_ListGraphic* p_plElements,
		bool p_bDifferent)
{
	if (!p_plElements || p_plElements->size() == 0)
		return FALSE;

	// map of list of attributes
	SP_MapString2ListAttribute l_tmAttributes;
	// construct the dialog
	SP_DLG_ShapeProperties* l_pcDialog = new SP_DLG_ShapeProperties(NULL, wxT("Edit Properties"), p_bDifferent);

	// get the attributes to show in the dialog
	// implemented in base class 'SP_Graphic'
	SP_ListGraphic::iterator l_Iter;
	for (l_Iter = p_plElements->begin(); l_Iter != p_plElements->end(); ++l_Iter)
	{
		if ((*l_Iter)->CollectDialogAttributes(&l_tmAttributes))
		{
			// add this to the list of affected nodes
			l_pcDialog->AddDialogGraphic((*l_Iter));
		}
	}

	if (l_pcDialog->ShowDialogAttributes(&l_tmAttributes))
	{
		// show and forget about the dlg
		// the dialog manages the events (OK, CANCEL) by itself
		if (l_pcDialog->ShowModal() == wxID_OK)
		{
			Modify(TRUE);
			for (l_Iter = p_plElements->begin(); l_Iter != p_plElements->end(); ++l_Iter)
				if ((*l_Iter)->GetParent())
					(*l_Iter)->GetParent()->Update(TRUE);

		}
	}

	l_pcDialog->Destroy();
	return TRUE;
}

bool SP_MDI_View::DoEditPropertiesClass(const wxString& p_sClass)
{
	if (!m_pcCanvas || p_sClass.IsEmpty() || !GetDocument())
		return FALSE;

	SP_ListGraphic l_lElements;
	wxList l_lShapes;
	FindSelectedShapes(l_lShapes);

	// filter out all nodes, of the wrong class
	wxNode* l_pcNode = l_lShapes.GetFirst();
	while (l_pcNode)
	{
		SP_Graphic* l_pcGraphic = SP_Core::Instance()->ResolveExtern(dynamic_cast<wxShape*>(l_pcNode->GetData()));
		if (l_pcGraphic && l_pcGraphic->GetGraphicType()
				!= SP_GRAPHIC_ATTRIBUTE)
		{
			if (l_pcGraphic->GetParent()
					&& p_sClass.Cmp(l_pcGraphic->GetParent()->GetClassName()) == 0)
			{
				l_lElements.push_back(l_pcGraphic);
			}
			else
			{
				l_pcGraphic->Select(FALSE);
			}
		}

		l_pcNode = l_pcNode->GetNext();
	}

	Refresh();

	return DoEditProperties(&l_lElements, true);
}

void SP_MDI_View::OnTransformShapes(wxCommandEvent& p_cEvent)
{
	wxList l_lShapes;
	SP_ListGraphic l_lGraphic;
	SP_ListGraphic l_lLines;
	SP_ListGraphic::iterator l_Iter;
	FindSelectedShapes(l_lShapes);
	double l_nMinX = 10000, l_nMaxX = -1, l_nMinY = 10000, l_nMaxY = -1;
	double l_nX1, l_nX2, l_nY1, l_nY2;

	l_lGraphic.clear();
	// filter out attributes
	wxNode* l_pcNode = l_lShapes.GetFirst();
	while (l_pcNode)
	{
		SP_Graphic* l_pcGraphic = SP_Core::Instance()->ResolveExtern(dynamic_cast<wxShape*>(l_pcNode->GetData()));
		if (l_pcGraphic && l_pcGraphic->GetGraphicType()
				!= SP_GRAPHIC_ATTRIBUTE)
		{
			AddToSelectList(l_pcGraphic, &l_lGraphic, &l_lLines);
			l_pcGraphic->GetBoundingBox(&l_nX1, &l_nY1, &l_nX2, &l_nY2);
			l_nMinX = wxMin(l_nMinX, l_nX1);
			l_nMinY = wxMin(l_nMinY, l_nY1);
			l_nMaxX = wxMax(l_nMaxX, l_nX2);
			l_nMaxY = wxMax(l_nMaxY, l_nY2);
		}

		l_pcNode = l_pcNode->GetNext();
	}

	for (l_Iter = l_lLines.begin(); l_Iter != l_lLines.end(); ++l_Iter)
		l_lGraphic.push_back((*l_Iter));

	// dialog, getting all infos about the affected graphics and their position
	wxRealPoint l_cPos(l_nMinX + ((l_nMaxX - l_nMinX) / 2.0), l_nMinY
			+ ((l_nMaxY - l_nMinY) / 2.0));
	wxSize l_cSize( (int) (l_nMaxX - l_nMinX), (int) (l_nMaxY - l_nMinY));
	SP_DLG_TransformGraphic* l_pcDlg = new SP_DLG_TransformGraphic(NULL, this, &l_lGraphic, l_cPos, l_cSize);
	l_pcDlg->ShowModal();

	if (m_pcCanvas)
	{
		SP_ListGraphic::iterator l_Iter;
		for (l_Iter = l_lGraphic.begin(); l_Iter != l_lGraphic.end(); ++l_Iter)
		{
			if ((*l_Iter)->GetPrimitive())
				m_pcCanvas->MoveShape((*l_Iter)->GetPrimitive(), 0, 0);
		}
		Refresh();
	}
	l_pcDlg->Destroy();
}

bool SP_MDI_View::FindConnectedEdges(SP_ListGraphic* p_plEdges)
{
	if (!p_plEdges || !GetDocument())
		return FALSE;

	SP_MDI_Doc *l_pcDoc = dynamic_cast<SP_MDI_Doc*>(GetDocument());
	if (!l_pcDoc->GetDiagram() || !l_pcDoc->GetDiagram()->GetShapeList())
		return FALSE;

	wxNode *l_pcNode = l_pcDoc->GetDiagram()->GetShapeList()->GetFirst();
	SP_Graphic* l_pcGraphic;
	while (l_pcNode)
	{
		wxShape *l_pcShape = dynamic_cast<wxShape*>(l_pcNode->GetData());
		if (l_pcShape->IsKindOf(CLASSINFO(wxLineShape)) && (dynamic_cast<wxLineShape*>(l_pcShape)->GetFrom()->Selected() && dynamic_cast<wxLineShape*>(l_pcShape)->GetTo()->Selected()))
		{
			l_pcGraphic = SP_Core::Instance()->ResolveExtern(l_pcShape);
			if (l_pcGraphic)
				p_plEdges->push_back(l_pcGraphic);
		}
		l_pcNode = l_pcNode->GetNext();
	}
	return TRUE;
}

wxShape* SP_MDI_View::FindFirstSelectedShape(wxClassInfo *p_pcToFind)
{
	if (!GetDocument())
		return NULL;

	SP_MDI_Doc *l_pcDoc = dynamic_cast<SP_MDI_Doc*>(GetDocument());

	if (!l_pcDoc->GetDiagram() || !l_pcDoc->GetDiagram()->GetShapeList())
		return NULL;

	wxNode *l_pcNode = l_pcDoc->GetDiagram()->GetShapeList()->GetFirst();
	while (l_pcNode)
	{
		wxShape *l_pcShape = dynamic_cast<wxShape*>(l_pcNode->GetData());
		if (l_pcShape->Selected() && ((p_pcToFind == NULL)
				|| (l_pcShape->IsKindOf(p_pcToFind))))
		{
			return l_pcShape;
		}
		l_pcNode = l_pcNode->GetNext();
	}
	return NULL;
}

void SP_MDI_View::FindSelectedShapes(wxList& p_lSelections,
		wxClassInfo* p_pcToFind)
{
	wxASSERT(GetDocument());
	SP_MDI_Doc *l_pcDoc = dynamic_cast<SP_MDI_Doc*>(GetDocument());

	wxASSERT(l_pcDoc->GetDiagram() && l_pcDoc->GetDiagram()->GetShapeList());
	wxNode *l_pcNode = l_pcDoc->GetDiagram()->GetShapeList()->GetFirst();
	while (l_pcNode)
	{
		wxShape *l_pcShape = dynamic_cast<wxShape*>(l_pcNode->GetData());
		if ((l_pcShape->GetParent() == NULL) && !l_pcShape->IsKindOf(CLASSINFO(wxLabelShape)) && l_pcShape->Selected()
				&& ((p_pcToFind == NULL) || (l_pcShape->IsKindOf(p_pcToFind))))
		{
			p_lSelections.Append(l_pcShape);
		}
		l_pcNode = l_pcNode->GetNext();
	}
}

void SP_MDI_View::FindSelectedGraphics(SP_ListGraphic& p_lSelectedGraphics, bool p_bAllEdges)
{
	wxList l_lShapes;
	SP_ListGraphic l_lEdges;
	SP_ListGraphic::iterator l_Iter;
	FindSelectedShapes(l_lShapes);

	// filter out attributes
	wxNode* l_pcNode = l_lShapes.GetFirst();
	while (l_pcNode)
	{
		SP_Graphic* l_pcGraphic = SP_Core::Instance()->ResolveExtern(dynamic_cast<wxShape*>(l_pcNode->GetData()));
		if (l_pcGraphic &&
			l_pcGraphic->GetGraphicType() != SP_GRAPHIC_ATTRIBUTE &&
			l_pcGraphic->GetParent()->GetClassObject()->GetShowInElementTree() == true)
		{
			if(p_bAllEdges)
			{
				p_lSelectedGraphics.push_back(l_pcGraphic);
			}
			else
			{
				// only edges, where both ends are selected, too
				AddToSelectList(l_pcGraphic, &p_lSelectedGraphics, &l_lEdges);
			}
		}

		l_pcNode = l_pcNode->GetNext();
	}

	if(!p_bAllEdges)
	{
		for (l_Iter = l_lEdges.begin(); l_Iter != l_lEdges.end(); ++l_Iter)
		{
			p_lSelectedGraphics.push_back((*l_Iter));
		}
	}
}

void SP_MDI_View::FindSelectedElements(SP_ListData &p_lSelectedElements)
{
	wxASSERT(GetDocument());
	SP_MDI_Doc *l_pcDoc = dynamic_cast<SP_MDI_Doc*>(GetDocument());
	SP_ListGraphic l_cCoarseGraphics;
	l_cCoarseGraphics.clear();

	wxASSERT(l_pcDoc->GetDiagram() && l_pcDoc->GetDiagram()->GetShapeList());
	SP_Data *l_pcElement;
	wxNode *l_pcNode = l_pcDoc->GetDiagram()->GetShapeList()->GetFirst();
	while (l_pcNode)
	{
		SP_Graphic *l_pcGraphic = SP_Core::Instance()->ResolveExtern(dynamic_cast<wxShape*>(l_pcNode->GetData()));
		if (l_pcGraphic && l_pcGraphic->GetPrimitive()->Selected())
		{
			l_pcElement = l_pcGraphic->GetParent();
			//only nodes and edges
			if (l_pcElement->GetElementType() == SP_ELEMENT_NODE
					|| l_pcElement->GetElementType() == SP_ELEMENT_EDGE)
			{
				if (l_pcElement->GetCoarse())
				{
					//also collect all elements of selected coarse nodes
					FindSelectedCoarseElements(p_lSelectedElements,
							l_pcElement->GetCoarse()->GetNetnumber());
				}
				else if (!l_pcElement->GetLogical()
						|| SP_Find(p_lSelectedElements, l_pcElement) == p_lSelectedElements.end())
				{
					//logical data should only be considered once
					p_lSelectedElements.push_back(l_pcElement);
				}
			}
		}
		l_pcNode = l_pcNode->GetNext();
	}
}

void SP_MDI_View::FindSelectedCoarseElements(
		SP_ListData &p_lSelectedElements, unsigned int p_nNetNr)
{
	wxASSERT(GetDocument());
	SP_MDI_Doc *l_pcDoc = dynamic_cast<SP_MDI_Doc*>(GetDocument());
	SP_ListGraphic l_cCoarseGraphics;
	l_cCoarseGraphics.clear();

	if (l_pcDoc->GetGraph())
		l_pcDoc->GetGraph()->GetGraphicsInNet(&l_cCoarseGraphics, p_nNetNr,
				SP_ELEMENT_NODE | SP_ELEMENT_EDGE);

	//now add the parent data of the collected coarse graphics to our elements list
	SP_Data *l_pcElement;
	SP_ListGraphic::iterator it;
	for (it = l_cCoarseGraphics.begin(); it != l_cCoarseGraphics.end(); ++it)
	{
		l_pcElement = (*it)->GetParent();
		if (l_pcElement->GetCoarse())
		{
			//we found another coarse, so do it again
			FindSelectedCoarseElements(p_lSelectedElements,
					l_pcElement->GetCoarse()->GetNetnumber());
		}
		else if (SP_Find(p_lSelectedElements, l_pcElement) == p_lSelectedElements.end())
		{
			p_lSelectedElements.push_back(l_pcElement);
		}
	}
}

// Do a cut operation for the given list of shapes
// the list contains only nodes and edges without
// the attribute shapes
bool SP_MDI_View::DoDelete(SP_ListGraphic* p_plShapes)
{
	wxASSERT(m_pcCanvas);

	if (!p_plShapes || !m_pcCanvas)
		return FALSE;

	SP_ListGraphic::iterator l_itShape = p_plShapes->begin();
	bool l_bReturn= TRUE;

	while (l_itShape != p_plShapes->end())
	{
		SP_Graphic* l_pcGraphic = (*l_itShape);

		// determine the datastructure-part and fill all in the map of lists
		if (l_pcGraphic->GetGraphicType() == SP_GRAPHIC_EDGE)
			l_bReturn &= l_pcGraphic->AddAllToDeleteQueue();
		else
			l_bReturn &= l_pcGraphic->AddToDeleteQueue();

		l_itShape++;
	}

	l_bReturn &= SP_Core::Instance()->RemoveQueuedElements();

	Refresh();
	m_pcCanvas->Modify(TRUE);

	return l_bReturn;
}

bool SP_MDI_View::DoRotate(SP_ListGraphic* p_plShapes, double p_nX,
		double p_nY, double p_nAngle)
{
	if (!p_plShapes || !m_pcCanvas)
		return FALSE;

	bool l_bReturn= TRUE;
	SP_ListGraphic::iterator l_Iter;
	for (l_Iter = p_plShapes->begin(); l_Iter != p_plShapes->end(); ++l_Iter)
	{
		l_bReturn &= (*l_Iter)->Rotate(p_nX, p_nY, p_nAngle);
	}
	return l_bReturn;
}

bool SP_MDI_View::DoFlip(SP_ListGraphic* p_plShapes, double p_nX)
{
	if (!p_plShapes || !m_pcCanvas)
		return FALSE;

	bool l_bReturn= TRUE;
	SP_ListGraphic::iterator l_Iter;
	for (l_Iter = p_plShapes->begin(); l_Iter != p_plShapes->end(); ++l_Iter)
	{
		l_bReturn &= (*l_Iter)->Flip(p_nX);
	}
	return l_bReturn;
}

bool SP_MDI_View::DoMirror(SP_ListGraphic* p_plShapes, double p_nY)
{
	if (!p_plShapes || !m_pcCanvas)
		return FALSE;

	bool l_bReturn= TRUE;
	SP_ListGraphic::iterator l_Iter;
	for (l_Iter = p_plShapes->begin(); l_Iter != p_plShapes->end(); ++l_Iter)
	{
		l_bReturn &= (*l_Iter)->Mirror(p_nY);
	}
	return l_bReturn;
}

// Do a general command
void SP_MDI_View::DoCmd(wxList& p_lShapes, wxList& p_lOldShapes, int p_nCmd,
		const wxString& p_sOp)
{
}

// Select or deselect all
void SP_MDI_View::SelectAll(bool p_bSelect)
{
	if (!m_pcCanvas || !GetDocument())
		return;

	m_pcCanvas->SelectAll(p_bSelect);
}

void SP_MDI_View::SelectAllClass(const wxString& p_sClass)
{
	if (!m_pcCanvas || p_sClass.IsEmpty() || !GetDocument())
		return;

	m_pcCanvas->SelectAllClass(p_sClass);
}


// Event handlers
void SP_MDI_View::OnDelete(wxCommandEvent& p_cEvent)
{
	SP_ListGraphic l_lGraphic;

	FindSelectedGraphics(l_lGraphic, true);

	SelectAll(false);

	SP_MDI_Doc *l_pcDoc = (dynamic_cast<SP_MDI_Doc*>(GetDocument()))->GetParentDoc();
	if (p_cEvent.GetId() == SP_ID_DELETE)
	{
		//wxT("Delete")
		l_pcDoc->GetCommandProcessor()->Submit(new SP_CMD_Edit(SP_CMD_DELETE, &l_lGraphic, wxT("Delete Element(s)")));
	}
	else
	{
		//wxT("Delete All")
		l_pcDoc->GetCommandProcessor()->Submit(new SP_CMD_Edit(SP_CMD_DELETE_ALL, &l_lGraphic, wxT("Delete Element(s)")));
	}
}

void SP_MDI_View::OnDeleteAll(wxCommandEvent& p_cEvent)
{
	//changed by MD
	//the delete-command takes care of the difference between wxT("Delete" and "DeleteAll")
	OnDelete(p_cEvent);
	return;
}

void SP_MDI_View::OnCut(wxCommandEvent& p_cEvent)
{
	SP_ListGraphic l_lGraphic;
	FindSelectedGraphics(l_lGraphic, false);
	SelectAll(false);

	//    SP_MDI_Doc *l_pcDoc = dynamic_cast<SP_MDI_Doc*>(GetDocument());
	//		l_pcDoc->GetCommandProcessor()->Submit(new SP_CMD_Edit(SP_CMD_CUT, &l_lGraphic, wxT("Cut Element(s)")));

	DoCopy(&l_lGraphic);

	SP_MDI_Doc *l_pcDoc = (dynamic_cast<SP_MDI_Doc*>(GetDocument()))->GetParentDoc();
	//wxT("Delete")
	l_pcDoc->GetCommandProcessor()->Submit(new SP_CMD_Edit(SP_CMD_DELETE, &l_lGraphic, wxT("Cut Element(s)")));

}

void SP_MDI_View::OnCopy(wxCommandEvent& p_cEvent)
{
	SP_ListGraphic l_lGraphic;
	FindSelectedGraphics(l_lGraphic, false);

	//    SP_MDI_Doc *l_pcDoc = dynamic_cast<SP_MDI_Doc*>(GetDocument());
	//		l_pcDoc->GetCommandProcessor()->Submit(new SP_CMD_Edit(SP_CMD_COPY, &l_lGraphic, wxT("Copy Element(s)")));

	DoCopy(&l_lGraphic);
}

void SP_MDI_View::OnPaste(wxCommandEvent& p_cEvent)
{
	//    SP_MDI_Doc *l_pcDoc = dynamic_cast<SP_MDI_Doc*>(GetDocument());
	//		l_pcDoc->GetCommandProcessor()->Submit(new SP_CMD_Edit(SP_CMD_PASTE, dynamic_cast<SP_Graphic*>(0), wxT("Paste Element(s)")));

	DoPaste();
}

void SP_MDI_View::OnCopyInNewNet(wxCommandEvent& p_cEvent)
{
	SP_ListGraphic l_lGraphic;
	FindSelectedGraphics(l_lGraphic, false);

	DoCopyInNewNet(&l_lGraphic);
}

void SP_MDI_View::OnHide(wxCommandEvent& p_cEvent)
{
	SP_ListGraphic l_lGraphic;
	FindSelectedGraphics(l_lGraphic, true);
	SelectAll(false);

	DoHide(l_lGraphic, (p_cEvent.GetId() == SP_EDIT_HIDEALL_ID));
}

void SP_MDI_View::DoHide(SP_ListGraphic& p_lSelectedGraphics, bool p_bAll)
{
	SP_ListGraphic::iterator l_itGr = p_lSelectedGraphics.begin();
	SP_ListGraphic::iterator l_itGrNext = p_lSelectedGraphics.begin();

	while(l_itGrNext != p_lSelectedGraphics.end())
	{
		l_itGr = l_itGrNext;
		++l_itGrNext;
		if((*l_itGr)->GetHide() == true)
		{
			p_lSelectedGraphics.erase(l_itGr);
		}
	}

	SP_MDI_Doc *l_pcDoc = (dynamic_cast<SP_MDI_Doc*>(GetDocument()))->GetParentDoc();
	//wxT("Hide")
	if(p_bAll)
		l_pcDoc->GetCommandProcessor()->Submit(new SP_CMD_Edit(SP_CMD_HIDE_ALL, &p_lSelectedGraphics, wxT("Hide Element(s)")));
	else
		l_pcDoc->GetCommandProcessor()->Submit(new SP_CMD_Edit(SP_CMD_HIDE, &p_lSelectedGraphics, wxT("Hide Element(s)")));
}

void SP_MDI_View::OnUnHide(wxCommandEvent& p_cEvent)
{
	SP_ListGraphic l_lGraphic;
	FindSelectedGraphics(l_lGraphic, true);
	SelectAll(false);

	DoUnHide(l_lGraphic, (p_cEvent.GetId() == SP_EDIT_UNHIDEALL_ID));
}

void SP_MDI_View::DoUnHide(SP_ListGraphic& p_lSelectedGraphics, bool p_bAll)
{
	SP_ListGraphic::iterator l_itGr = p_lSelectedGraphics.begin();
	SP_ListGraphic::iterator l_itGrNext = p_lSelectedGraphics.begin();

	while(l_itGrNext != p_lSelectedGraphics.end())
	{
		l_itGr = l_itGrNext;
		++l_itGrNext;
		if((*l_itGr)->GetHide() == false)
		{
			p_lSelectedGraphics.erase(l_itGr);
		}
	}

	SP_MDI_Doc *l_pcDoc = (dynamic_cast<SP_MDI_Doc*>(GetDocument()))->GetParentDoc();
	//wxT("UnHide")
	if(p_bAll)
		l_pcDoc->GetCommandProcessor()->Submit(new SP_CMD_Edit(SP_CMD_UNHIDE_ALL, &p_lSelectedGraphics, wxT("UnHide Element(s)")));
	else
		l_pcDoc->GetCommandProcessor()->Submit(new SP_CMD_Edit(SP_CMD_UNHIDE, &p_lSelectedGraphics, wxT("UnHide Element(s)")));
}

void SP_MDI_View::OnToggleAnim(wxCommandEvent& p_cEvent)
{
	DoToggleAnim(p_cEvent.IsChecked());
}

void SP_MDI_View::DoToggleAnim(bool p_bEnable)
{
	if (!GetDocument())
		return;

	SP_MDI_Doc* l_pcRootDoc = SP_Core::Instance()->GetRootDocument();
	wxNode* l_pcNode = wxGetApp().GetDocmanager()->GetDocuments().GetFirst();
	while(l_pcNode)
	{
		SP_MDI_Doc* l_pcDoc = dynamic_cast<SP_MDI_Doc*>(l_pcNode->GetData());
		SP_Core::Instance()->ManageDocChange(l_pcDoc);
		SP_DS_Graph* l_pcGraph = l_pcDoc->GetGraph();

		if (l_pcGraph)
		{
			l_pcGraph->SqueezeIdAttributes();
			l_pcGraph->OnToggleAnim(p_bEnable);
		}

		l_pcNode = l_pcNode->GetNext();
	}
	SP_Core::Instance()->ManageDocChange(l_pcRootDoc);
}

void SP_MDI_View::OnSearchId(wxCommandEvent& p_cEvent)
{
	SP_MDI_Doc* doc = dynamic_cast<SP_MDI_Doc*>(GetDocument());
	SP_DLG_SearchId* dlg = doc->GetSearchIdDialog();
	if (dlg)
	{
		if (dlg->IsShown())
			dlg->Raise();
		else
			dlg->Show();
	}
}

void SP_MDI_View::OnSortNodes(wxCommandEvent& p_cEvent)
{
	SP_MDI_Doc* doc = dynamic_cast<SP_MDI_Doc*>(GetDocument());
	SP_DLG_SortNodes* l_pcDlg = new SP_DLG_SortNodes(doc->GetGraph(), NULL);
	if (l_pcDlg)
	{
		l_pcDlg->ShowModal();
	}
	l_pcDlg->Destroy();
}

void SP_MDI_View::OnCheckDuplicateNodes(wxCommandEvent& p_cEvent)
{
	SP_DLG_CheckNet* l_pcDlg = new SP_DLG_CheckNet(this,NULL);
	if (l_pcDlg)
	{
		l_pcDlg->ShowModal();
	}
	l_pcDlg->Destroy();
}

void SP_MDI_View::OnNetInformations(wxCommandEvent& p_cEvent)
{
	SP_MDI_Doc* doc = dynamic_cast<SP_MDI_Doc*>(GetDocument());
	SP_DLG_NetInformations* l_pcDlg = new SP_DLG_NetInformations(doc->GetGraph(), NULL);
	if (l_pcDlg)
	{
		l_pcDlg->ShowModal();
	}
	l_pcDlg->Destroy();
}

void SP_MDI_View::OnReduceFTree(wxCommandEvent& p_cEvent)
{
	SP_MDI_Doc* doc = dynamic_cast<SP_MDI_Doc*>(GetDocument());
	SP_DLG_ReduceFTree* dlg = doc->GetReduceFTDialog();
	if (dlg)
	{
		if (dlg->IsShown())
			dlg->Raise();
		else
			dlg->Show();
	}
}

void SP_MDI_View::OnTransformFTree(wxCommandEvent& p_cEvent)
{
	SP_DS_FTreeDeMorganTransformer* ftree = new SP_DS_FTreeDeMorganTransformer();
	ftree->TransformGraph();
}

/**	\brief	The OnLoadInvariantFile function


 This method is called when the user choses the menuentry
 Load node vector file.

 \param	p_cEvent	    Map of lists of attributes

 */
void SP_MDI_View::OnLoadInvariantFile(wxCommandEvent& p_cEvent)
{
	SP_MDI_Doc* doc = dynamic_cast<SP_MDI_Doc*>(GetDocument());
	SP_DLG_ChooseInvariant* dlg = doc->GetChooseInvariantDialog();

	if (dlg)
	{
		if (dlg->IsShown())
			dlg->Raise();
		else
			dlg->Show();
	}
}

///////////////////////////////////////////////////////////////////////////////
bool SP_MDI_View::AddToSelectList(SP_Graphic* p_pcGraphic,
		SP_ListGraphic* p_plList, SP_ListGraphic* p_plList2)
{
	if (!p_plList || !p_plList2 || !p_pcGraphic)
		return FALSE;

	// first should be the nodes, edges (if any) should follow afterwards
	// so take care of push_front and push_back!
	if (p_pcGraphic->GetGraphicType() == SP_GRAPHIC_EDGE)
	{
		SP_GR_Edge* l_pcEdge = dynamic_cast<SP_GR_Edge*>(p_pcGraphic);
		SP_Graphic* l_pcSource = l_pcEdge->GetSource();
		SP_Graphic* l_pcTarget = l_pcEdge->GetTarget();
		if (l_pcSource && l_pcTarget)
		{
			if (l_pcSource->GetPrimitive() && l_pcTarget->GetPrimitive())
			{
				if (l_pcSource->GetPrimitive()->Selected() && l_pcTarget->GetPrimitive()->Selected())
				{
					if (p_pcGraphic->GetGraphicState() < SP_STATE_COARSEBORDERDOWN)
						p_plList2->push_front(p_pcGraphic);
					else
						p_plList2->push_back(p_pcGraphic);
				}
				else
					p_pcGraphic->GetPrimitive()->Select(FALSE);
			}
		}
	}
	else if (p_pcGraphic->GetGraphicState() < SP_STATE_COARSEBORDERDOWN)
		p_plList->push_front(p_pcGraphic);
	else
		p_plList->push_back(p_pcGraphic);

	return TRUE;
}

bool SP_MDI_View::DoCopy(SP_ListGraphic* p_plShapes)
{
	if (!GetDocument())
		return FALSE;

	SP_MDI_Doc* l_pcDocument = dynamic_cast<SP_MDI_Doc*>(GetDocument());
	if (l_pcDocument->GetGraph())
		return l_pcDocument->GetGraph()->Copy(GetNetnumber(), p_plShapes);

	return FALSE;
}

bool SP_MDI_View::DoPaste()
{
	if (!GetDocument())
		return FALSE;

	SelectAll(FALSE);
	Refresh();

	SP_MDI_Doc* l_pcDocument = dynamic_cast<SP_MDI_Doc*>(GetDocument());
	if (l_pcDocument->GetGraph())
		return l_pcDocument->GetGraph()->Paste(m_pcCanvas);

	Refresh();

	return FALSE;
}

bool SP_MDI_View::DoCopyInNewNet(SP_ListGraphic* p_plShapes)
{
	if(p_plShapes->empty())
		return false;

	SP_MDI_Doc* l_pcDoc = dynamic_cast<SP_MDI_Doc*>(GetDocument());
	SP_DS_Graph* l_pcGraph = l_pcDoc->GetGraph();

	//add metadata to selected shapes, works only for metadata with graphics
	for(SP_DS_Metadataclass* l_pcMC : *(l_pcGraph->GetMetadataclasses()))
	{
		if(!l_pcMC->GetShowInElementTree()
			&& (l_pcMC->GetShowInDeclarationTreeOther() || l_pcMC->GetShowInDeclarationTreeColorSet())
			&& !l_pcMC->GetPrototype()->GetGraphics()->empty())
		{
			for(SP_DS_Metadata* l_pcMeta : *(l_pcMC->GetElements()))
			{
				for(SP_Graphic* l_pcGr : *(l_pcMeta->GetGraphics()))
				{
					p_plShapes->push_back(l_pcGr);
				}
			}
		}
	}

	if(!DoCopy(p_plShapes))
		return false;

	SP_GM_Docmanager* l_pcDM = wxGetApp().GetDocmanager();
	SP_GM_DocTemplate* l_pcTemplate = l_pcDM->GetTemplate(l_pcDoc->GetGraph()->GetNetclass()->GetName());
	SP_DS_Netclass* newClass = l_pcTemplate->GetNetclass();

	SP_MDI_Doc *l_pcNewDoc = dynamic_cast<SP_MDI_Doc*>(((wxDocTemplate*)l_pcTemplate)->CreateDocument(wxT(""), 1));
	if (l_pcNewDoc)
	{
		l_pcNewDoc->OnNewDocument();
		l_pcNewDoc->InitGraph(newClass);
		SP_MDI_View* l_pcView = dynamic_cast<SP_MDI_View*>(l_pcNewDoc->GetFirstView());
		if(l_pcView->DoPaste())
		{
			SP_DS_FunctionRegistry* l_pcFR = l_pcNewDoc->GetGraph()->GetFunctionRegistry();
			l_pcFR->LoadFromNet(l_pcNewDoc->GetGraph());
			l_pcNewDoc->Modify(true);
			return true;
		}
	}

	return false;
}

void SP_MDI_View::Refresh(bool p_bRealRefresh)
{
	if (m_pcCanvas)
		m_pcCanvas->Refresh(p_bRealRefresh);
}

void SP_MDI_View::RefreshAllViews()
{
	if (GetDocument())
	{
		SP_MDI_Doc* l_pcDocument = dynamic_cast<SP_MDI_Doc*>(GetDocument());
		l_pcDocument->Refresh();
	}
}

void SP_MDI_View::OnActivateView(bool activate, wxView *activeView,
		wxView *deactiveView)
{
	if(activate && this == activeView && this != deactiveView)
	{
		SP_MDI_Doc* l_pcDoc = dynamic_cast<SP_MDI_Doc*>(GetDocument());
		if (l_pcDoc && l_pcDoc->GetGraph())
		{
			// calling with show=FALSE, document is already shown
			l_pcDoc->GetGraph()->CreateCoarseTree()->SetCurrentDoc(NULL, FALSE);
		}
	}
}

void SP_MDI_View::UpdateIASubMenu()
{
	SP_MDI_Doc* l_pcDoc = dynamic_cast<SP_MDI_Doc*>(GetDocument());
	if (l_pcDoc)
	{
		l_pcDoc->GetGraph()->UpdateIASubMenu(m_pcFrame->GetVisualizationMenu()->
		FindItem(SP_MENU_ITEM_IA_MENU)->GetSubMenu());
	}
}

void SP_MDI_View::OnIASubMenuSelection(wxCommandEvent& p_cEvent)
{
	SP_LOGDEBUG(wxT("entered OnIASubMenuSelection..."));
	SP_MDI_Doc* l_pcDoc = dynamic_cast<SP_MDI_Doc*>(GetDocument());
	if (l_pcDoc)
	{
		l_pcDoc->GetGraph()->UpdateIASubMenuSelection(m_pcFrame->GetVisualizationMenu(),
				p_cEvent);
	}
}

void SP_MDI_View::OnIATriggerGeneral(wxCommandEvent& p_cEvent)
{

	SP_MDI_Doc* l_pcDoc = dynamic_cast<SP_MDI_Doc*>(GetDocument());
	if (!l_pcDoc)
		return;

	SP_ListData l_lElements;
	l_lElements.clear();
	l_lElements.push_back(new SP_DS_Node(new SP_DS_Nodeclass(l_pcDoc->GetGraph(), wxT(""))));

	wxGetApp().GetIAManager()->SubmitEvent(new SP_IA_Event(&l_lElements, SP_IA_EVT_GENERAL));
}

void SP_MDI_View::OnIATriggerSingle(wxCommandEvent& p_cEvent)
{

	SP_ListData l_lElements;
	l_lElements.clear();
	FindSelectedElements(l_lElements);

	wxGetApp().GetIAManager()->SubmitEvent(new SP_IA_Event(&l_lElements, SP_IA_EVT_SINGLE));
}

void SP_MDI_View::OnIATriggerMakro(wxCommandEvent& p_cEvent)
{

	SP_ListData l_lElements;
	l_lElements.clear();
	FindSelectedElements(l_lElements);

	wxGetApp().GetIAManager()->SubmitEvent(new SP_IA_Event(&l_lElements, SP_IA_EVT_MAKRO));
}

void SP_MDI_View::OnIAUndoLastAction(wxCommandEvent& p_cEvent)
{

	SP_MDI_Doc* l_pcDoc = dynamic_cast<SP_MDI_Doc*>(GetDocument());
	if (l_pcDoc)
	{
		wxGetApp().GetIAManager()->UndoLastSubmit(l_pcDoc->GetGraph(), true);
	}
}

void SP_MDI_View::OnIAConfigure(wxCommandEvent& p_cEvent)
{

	SP_DLG_IAConfigure l_cDialog(GetFrame(), wxGetApp().GetIAManager());
	l_cDialog.ShowModal();
	l_cDialog.Destroy();

}

void SP_MDI_View::OnIARegCmdList(wxCommandEvent& p_cEvent)
{

	SP_DLG_IARegCmdList l_cDialog(GetFrame(), wxGetApp().GetIAManager());
	l_cDialog.ShowModal();
	l_cDialog.Destroy();

}

void SP_MDI_View::OnMDEditGeneral(wxCommandEvent& p_cEvent)
{
	// map of list of attributes
	SP_MapString2ListAttribute l_tmAttributes;
	// construct the dialog
	SP_DLG_ShapeProperties* l_pcDialog = new SP_DLG_ShapeProperties(NULL, wxT("Edit Properties"));

	// get the attributes to show in the dialog
	SP_ListMetadata::const_iterator l_Iter;
	SP_MDI_Doc* l_pcDoc = dynamic_cast<SP_MDI_Doc*>(GetDocument());
	SP_DS_Graph* l_pcGraph = l_pcDoc->GetGraph();
	SP_DS_Metadataclass* l_pcMC =l_pcGraph->GetMetadataclass(wxT("General"));
	for (l_Iter = l_pcMC->GetElements()->begin(); l_Iter != l_pcMC->GetElements()->end(); ++l_Iter)
	{
		(*l_Iter)->AddDialogAttributes(&l_tmAttributes);
		if(!(*l_Iter)->GetGraphics()->empty())
		{
			// add this to the list of affected nodes
			l_pcDialog->AddDialogGraphic((*l_Iter)->GetGraphics()->front());
		}
	}

	if (l_pcDialog->ShowDialogAttributes(&l_tmAttributes))
	{
		// show and forget about the dlg
		// the dialog manages the events (OK, CANCEL) by itself
		if (l_pcDialog->ShowModal() == wxID_OK)
		{
			Modify(TRUE);
			for (l_Iter = l_pcMC->GetElements()->begin(); l_Iter != l_pcMC->GetElements()->end(); ++l_Iter)
			{
				(*l_Iter)->Update(TRUE);
			}

		}
	}

	l_pcDialog->Destroy();

}


void SP_MDI_View::OnGenerateMasterNets(wxCommandEvent& p_cEvent)
{
	//check if a petri net has been opened
	SP_MDI_Doc* l_pcDoc = dynamic_cast<SP_MDI_Doc*>(GetDocument());
	SP_DS_Graph* l_pcGraph = l_pcDoc->GetGraph();

	SP_CPN_GenMasterNet l_cGenMasterNet(l_pcGraph);
	
	//Generate a master net
	
	if( ! l_cGenMasterNet.GenerateMasterNet() )
	{
		return;
	}

	//Update it
	l_pcDoc->Modify(TRUE);
	Refresh();	

	SP_LOGMESSAGE( wxT("A master net has been generated.\n") );
}

void SP_MDI_View::OnGenerateTwinNets(wxCommandEvent& p_cEvent)
{
	SP_MDI_Doc* l_pcDoc = dynamic_cast<SP_MDI_Doc*>(GetDocument());
	SP_DS_Graph* l_pcGraph = l_pcDoc->GetGraph();
	SP_GUI_Canvas* l_pcCanvas = this->GetCanvas();
	

	SP_CPN_GenTwinNet l_cGenTwinNets;
	
	if( ! l_cGenTwinNets.GenerateTwinNet(l_pcGraph,l_pcCanvas,l_pcDoc,this) ) 
		return;

	//Update it
	l_pcDoc->Modify(TRUE);
	Refresh();	

	
	l_pcCanvas->Show();

	SP_LOGMESSAGE( wxT("A Twin net has been generated.\n") );
}



void SP_MDI_View::OnColorizeAnyNets(wxCommandEvent& p_cEvent)
{
	SP_MDI_Doc* l_pcDoc = dynamic_cast<SP_MDI_Doc*>(GetDocument());
	SP_DS_Graph* l_pcGraph = l_pcDoc->GetGraph();	

	SP_CPN_GenTwinNet l_cGenTwinNets;
	
	if( ! l_cGenTwinNets.GenerateAnyNet(l_pcGraph,this) )
		return;

	//Update it
	l_pcDoc->Modify(TRUE);
	//Refresh();	

	
	//l_pcCanvas->Show();

	SP_LOGMESSAGE( wxT("A colored net has been generated.\n") );
}

void SP_MDI_View::OnStartSimulation(wxCommandEvent& p_cEvent)
{
	try
	{
		SP_MDI_Doc* l_pcDoc = dynamic_cast<SP_MDI_Doc*>(GetDocument());
		SP_DS_Graph* l_pcGraph = l_pcDoc->GetGraph();

		SP_DLG_Simulation* l_pcDlg = NULL;

		SP_DS_ColPN_Unfolding* l_pcUnfoldedNet=NULL;

		wxString l_sName = l_pcGraph->GetNetclass()->GetName();

		if((l_sName == SP_DS_COLCPN_CLASS) ||
			(l_sName == SP_DS_COLHPN_CLASS) ||
		   (l_sName == SP_DS_COLSPN_CLASS))
		{
			//create the unfolding object
			l_pcUnfoldedNet = new SP_DS_ColPN_Unfolding();

			if(! l_pcUnfoldedNet->Unfolding() )
			{
				wxDELETE(l_pcUnfoldedNet);

				return ;
			}
			if(l_sName == SP_DS_COLSPN_CLASS)
			{
				l_pcDlg = new SP_DLG_ColStSimulationResults( l_pcGraph,l_pcUnfoldedNet, m_pcFrame );
			}
			else if(l_sName == SP_DS_COLCPN_CLASS)
			{
				
				l_pcDlg = new SP_DLG_ColCPNSimulationResults( l_pcGraph,l_pcUnfoldedNet, m_pcFrame );
			}
			else if(l_sName == SP_DS_COLHPN_CLASS)
			{
				l_pcDlg = new SP_DLG_ColHPNSimultionResults( l_pcGraph,l_pcUnfoldedNet, m_pcFrame );
			}
		}
		else if(l_sName == SP_DS_SPN_CLASS)
		{
			l_pcDlg = new SP_DLG_StSimulationResults( l_pcGraph, m_pcFrame );
		}
		//else if((l_sName == SP_DS_FUZZYPN_CLASS)) //Added by G.A
		//{
         //   l_pcDlg = new SP_DLG_StSimulationResults(l_pcGraph, m_pcFrame);
			//l_pcDlg = new SP_DLG_FstSimulationResults(l_pcGraph, m_pcFrame);
			// l_pcDlg = new SP_DLG_FspnSimResult(l_pcGraph, m_pcFrame);
		//}
		else if ((l_sName == SP_DS_FUZZYSPN_CLASS)) //Added by G.A
		{
			//l_pcDlg = new SP_DLG_StSimulationResults(l_pcGraph, m_pcFrame);
			//l_pcDlg = new SP_DLG_FspnStSimulationResults(l_pcGraph, m_pcFrame);
			l_pcDlg = new SP_DLG_FspnSimResult(l_pcGraph, m_pcFrame);
		}
		else if(l_sName == SP_DS_CONTINUOUSPED_CLASS)
		{
	/*
			SP_DS_CheckTransitions l_checkTrans(l_pcGraph);

			wxBusyInfo l_Info(wxT("Please wait while checking the net.."));

			if(!l_checkTrans.CheckRateFunction())
			 {
				 SP_MESSAGEBOX(wxT("Simulation can not initialize"),wxT("Continuous Simulation"));
				 return; //there is error in one of the transitions rate functions
			 }
	*/
			l_pcDlg = new SP_DLG_CPNSimulationResults( l_pcGraph, m_pcFrame );
		}
		else if (l_sName == SP_DS_FUZZYCPN_CLASS)
		{
			l_pcDlg = new SP_DLG_FCPNSimulationResults(l_pcGraph, m_pcFrame);


		}
		else if(l_sName == SP_DS_HYBRIDPN_CLASS)
		{
 
		    l_pcDlg = new SP_DLG_HybridSimulationResults( l_pcGraph, m_pcFrame );

		}
		else if (l_sName == SP_DS_FUZZYHPN_CLASS)
		{
			l_pcDlg = new SP_DLG_FHybridSimulationResults(l_pcGraph, m_pcFrame);
		}

		if(l_pcDlg != NULL)
		{
			l_pcDlg->Show();
		}
	}
	catch(std::exception& e)
	{
		SP_LOGERROR(e.what());
	}
	catch(...)
	{
		SP_LOGERROR(wxT("unkown exception!"));
	}
}

// Amr
void SP_MDI_View::OnStartDirSimulation(wxCommandEvent& p_cEvent)
{
	try
	{
		SP_MDI_Doc* l_pcDoc = dynamic_cast<SP_MDI_Doc*>(GetDocument());
		SP_DS_Graph* l_pcGraph = l_pcDoc->GetGraph();

		SP_DLG_DirColorSimulation* l_pccolDiDlg = NULL;


		SP_DS_ColPN_Coloring* l_pcoloredNet = NULL;
		
		wxString l_sName = l_pcGraph->GetNetclass()->GetName();
		if ((l_sName == SP_DS_COLCPN_CLASS) ||
			(l_sName == SP_DS_COLHPN_CLASS) ||
			(l_sName == SP_DS_COLSPN_CLASS))
		{
			l_pcoloredNet = new SP_DS_ColPN_Coloring();
			if (!l_pcoloredNet->Coloring())
			{
				wxDELETE(l_pcoloredNet);

				return;
			}

			if (l_sName == SP_DS_COLHPN_CLASS)
			{
				
					SP_LOGERROR(wxT("Sorry, The Method for this class  under preparing"));
				

			}
			else if (l_sName == SP_DS_COLSPN_CLASS)
			{

				SP_LOGERROR(wxT("Sorry, The Method for this class  under preparing"));
				//l_pccolDiDlg = new SP_DLG_ColStDirSimResults(l_pcGraph, l_pcoloredNet, m_pcFrame);
			}
			else if (l_sName == SP_DS_COLCPN_CLASS)
			{
				l_pccolDiDlg = new SP_DLG_ColCPNDirSimulationResults(l_pcGraph, l_pcoloredNet, m_pcFrame);
			}
			 if (l_pccolDiDlg != NULL)
			{
				l_pccolDiDlg->Show();
			}
			
		}
	}
	catch (std::exception& e)
	{
		SP_LOGERROR(e.what());
	}
	catch (...)
	{
		SP_LOGERROR(wxT("unkown exception!"));
	}
}
void SP_MDI_View::OnConvertElement(wxCommandEvent& p_cEvent)
{
	SP_DLG_ConvertElements* l_pcDlg = new SP_DLG_ConvertElements(this,NULL);

	if (l_pcDlg && l_pcDlg->CanConvert())
	{
		if(l_pcDlg->ShowModal()==wxID_OK)
		{
			l_pcDlg->DoConversion();
		}
	}
	l_pcDlg->Destroy();

	RefreshAllViews();
}



void
SP_MDI_View::OnToggleMarking(wxCommandEvent& p_cEvent)
{
	SP_MDI_Doc* l_pcDoc = dynamic_cast<SP_MDI_Doc*>(GetDocument());
	SP_DS_Graph* l_pcGraph = l_pcDoc->GetGraph();

	//load places
	list<wxString> l_lPlaces;

	SP_ListNodeclass::const_iterator itNC;
	for(itNC = l_pcGraph->GetNodeclasses()->begin();
			itNC != l_pcGraph->GetNodeclasses()->end();
			++itNC)
	{
		SP_DS_Nodeclass* l_pcNodeclass = *itNC;
		if(l_pcNodeclass->GetName() == SP_DS_DISCRETE_PLACE
			|| l_pcNodeclass->GetName() == SP_DS_CONTINUOUS_PLACE)
		{
			SP_ListNode::const_iterator l_itElem;
			for(l_itElem = l_pcNodeclass->GetElements()->begin();
				l_itElem != l_pcNodeclass->GetElements()->end();
				++l_itElem)
			{
				wxString l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>((*l_itElem)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
				l_lPlaces.push_front(l_sPlaceName);
			}
		}
	}
	ColorEdges(l_pcGraph, l_lPlaces, p_cEvent.IsChecked());
}


void
SP_MDI_View::ColorEdges(SP_DS_Graph* l_pcGraph, list<wxString>& l_lPlaces, bool isChecked) {

	SP_ListEdgeclass::const_iterator l_itEC;
	const SP_ListEdge* m_edgeList;
	SP_ListEdge::const_iterator m_edgeIt;

	SP_ListGraphic* m_grList;
	SP_ListGraphic::const_iterator m_grIt;
	SP_DS_Attribute* attr;

	m_grList = NULL;

	l_itEC = l_pcGraph->GetEdgeclasses()->begin();
	for(; l_itEC != l_pcGraph->GetEdgeclasses()->end(); ++l_itEC)
	{
		m_edgeList = (*l_itEC)->GetElements();
		m_edgeIt = m_edgeList->begin();
		for (; m_edgeIt != m_edgeList->end(); m_edgeIt++)
		{
			attr = (*m_edgeIt)->GetAttribute(wxT("Multiplicity"));
			if(attr)
			{
				wxString l_sMulti = attr->GetValueString();

				for (list<wxString>::iterator iter = l_lPlaces.begin();  iter != l_lPlaces.end();  ++iter) {
				   if (l_sMulti.Contains(*iter)) {
					   m_grList = (*m_edgeIt)->GetGraphics();
					   m_grIt = m_grList->begin();
					   if (isChecked) {
						   wxBrush* l_pcBrush= wxTheBrushList->FindOrCreateBrush(*wxRED, (*m_grIt)->GetBrush()->GetStyle());
						   (*m_grIt)->SetBrush(l_pcBrush);
						   (*m_grIt)->SetPen(wxThePenList->FindOrCreatePen(*wxRED, (*m_grIt)->GetPen()->GetWidth(), (*m_grIt)->GetPen()->GetStyle()));
						   (*m_grIt)->SetPenColour(*wxRED);
						} else {
							wxBrush* l_pcBrush= wxTheBrushList->FindOrCreateBrush(*wxBLACK, (*m_grIt)->GetBrush()->GetStyle());
							(*m_grIt)->SetBrush(l_pcBrush);
							(*m_grIt)->SetPen(wxThePenList->FindOrCreatePen(*wxBLACK, (*m_grIt)->GetPen()->GetWidth(), (*m_grIt)->GetPen()->GetStyle()));
							(*m_grIt)->SetPenColour(*wxBLACK);
						}
					   (*m_grIt)->Update();
				   }
				}
			}
		}
	}
	Refresh();
}


void SP_MDI_View::OnMergeNodes(wxCommandEvent &p_cEvent) {
	SP_ListGraphic l_lGraphic;
	FindSelectedGraphics(l_lGraphic, false);

	if(l_lGraphic.empty())
	{
		SP_MDI_Doc* l_pcDoc = dynamic_cast<SP_MDI_Doc*>(GetDocument());
		SP_DS_Graph* l_pcGraph = l_pcDoc->GetGraph();

		SP_DLG_MergeNodes l_Dlg(l_pcGraph, GetNetnumber(), &l_lGraphic, m_pcFrame);
		l_Dlg.ShowModal();

	}

	DoMergeNodes(l_lGraphic);
}

void SP_MDI_View::DoMergeNodes(SP_ListGraphic &p_lSelectedGraphics) {
	if (p_lSelectedGraphics.empty())
		return;

	int l_nNumUnified = 0;
	SP_MapData2Graphic l_mSelectedParents;
	for (SP_Graphic* l_pcGraphic : p_lSelectedGraphics)
	{
		l_pcGraphic->Select(false);
		if (l_pcGraphic->GetGraphicType() == SP_GRAPHIC_NODE)
		{
			l_mSelectedParents.insert(std::make_pair(l_pcGraphic->GetParent(),l_pcGraphic));
		}
	}

	for (auto it : l_mSelectedParents)
	{
		SP_DS_Node* l_pcParent = static_cast<SP_DS_Node*>(it.first);
		SP_GR_Node* l_pcGraphic = static_cast<SP_GR_Node*>(it.second);
		l_nNumUnified += l_pcParent->Merge(l_pcGraphic, m_pcCanvas);
	}

	Refresh();
	SP_LOGMESSAGE(wxString::Format(wxT("Merged %i logical nodes."), l_nNumUnified));
	m_pcCanvas->Modify(TRUE);
}

void SP_MDI_View::OnDuplicateNodes(wxCommandEvent &p_cEvent) {
	SP_ListGraphic l_lGraphic;
	FindSelectedGraphics(l_lGraphic, false);

	if(l_lGraphic.empty())
	{
		SP_MDI_Doc* l_pcDoc = dynamic_cast<SP_MDI_Doc*>(GetDocument());
		SP_DS_Graph* l_pcGraph = l_pcDoc->GetGraph();

		SP_DLG_DuplicateNodes l_Dlg(l_pcGraph, GetNetnumber(), &l_lGraphic, m_pcFrame);
		l_Dlg.ShowModal();

	}
	DoDuplicateNodes(l_lGraphic);
}

void SP_MDI_View::DoDuplicateNodes(SP_ListGraphic &p_lSelectedGraphics) {
	if (p_lSelectedGraphics.empty())
		return;

	int l_nNumDuplicated = 0;
	for (SP_Graphic* l_pcGraphic : p_lSelectedGraphics)
	{
		l_pcGraphic->Select(false);
		if (l_pcGraphic->GetGraphicType() == SP_GRAPHIC_NODE)
		{
			l_nNumDuplicated +=
					static_cast<SP_DS_Node*>(l_pcGraphic->GetParent())->
							Duplicate(static_cast<SP_GR_Node*>(l_pcGraphic), m_pcCanvas);
		}
	}

	Refresh();
	SP_LOGMESSAGE(wxString::Format(wxT("Duplicated into %i logical nodes."), l_nNumDuplicated));
	m_pcCanvas->Modify(TRUE);
}
