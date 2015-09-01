//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_core/SP_Core.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_core/SP_Set.h"
#include "sp_defines.h"
#include "sp_ds/SP_DS_Netclass.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/SP_DS_Nodeclass.h"
#include "sp_ds/extensions/SP_DS_Coarse.h"
#include "sp_ds/extensions/SP_DS_Animation.h"

#include "sp_gui/dialogs/SP_DLG_IAConfigure.h"
#include "snoopy.h"

// Netclasses
#include "sp_ds/netclasses/SP_DS_SimpleGraph.h"
#include "sp_ds/netclasses/SP_DS_ReachabilityGraph.h"
#include "sp_ds/netclasses/SP_DS_BipartGraph.h"
#include "sp_ds/netclasses/SP_DS_SimplePed.h"
#include "sp_ds/netclasses/SP_DS_ExtPT.h"
#include "sp_ds/netclasses/SP_DS_TimePT.h"
#include "sp_ds/netclasses/SP_DS_FaultTree.h"
#include "sp_ds/netclasses/SP_DS_ExtendedFaultTree.h"
#include "sp_ds/netclasses/SP_DS_ContinuousPed.h"
#include "sp_ds/netclasses/SP_DS_MTBDD.h"
#include "sp_ds/netclasses/SP_DS_MTIDD.h"
#include "sp_ds/netclasses/SP_DS_EventSPN.h"
#include "sp_ds/netclasses/SP_DS_ModuloNets.h"
#include "sp_ds/netclasses/SP_DS_MusicPed.h"
#include "sp_ds/netclasses/SP_DS_Freestyle.h"
#include "sp_ds/netclasses/SP_DS_HybridPN.h"

//Colored netclasses
#include "sp_ds/netclasses/SP_DS_ColPT.h"
#include "sp_ds/netclasses/SP_DS_ColExtPT.h"
#include "sp_ds/netclasses/SP_DS_ColSPN.h"
#include "sp_ds/netclasses/SP_DS_ColCPN.h"
#include "sp_ds/netclasses/SP_DS_ColHPN.h"

// standard widget class
#include "sp_gui/widgets/dialogs/SP_WDG_DialogText.h"
#include "sp_gui/dialogs/SP_DLG_Animation.h"
#include "sp_core/base/SP_Graphic.h"

unsigned int
GetPasteNetnumber(SP_MapUInt2UInt* p_pmNet2Net, unsigned int p_nNumber, SP_DS_Graph* p_pcGraph)
{
	SP_MapUInt2UInt::iterator l_itN2N = p_pmNet2Net->find(p_nNumber);
	unsigned int l_nNr = 0;
	if(l_itN2N != p_pmNet2Net->end())
	{
		l_nNr = l_itN2N->second;
	}
	else
	{
        l_nNr = p_pcGraph->GetNewNetnumber();
        p_pmNet2Net->insert(make_pair(p_nNumber, l_nNr));
    }

    return l_nNr;
}

SP_Core* SP_Core::m_pcInstance = NULL;

SP_Core::SP_Core():
m_pcActiveRootDocument(NULL),
m_nCopyFromNetnumber(0),
m_nPasteOffset(20),
m_pcCopyGraph(NULL),
m_bAnimMode(FALSE),
m_pcAnimControl(NULL),
m_pcAnimation(NULL),
m_bDocumentReactivatingFlag( false ),
m_bSimulationMode(false)
{
}

SP_Core::~SP_Core()
{
    ResetCopyState(NULL);

    // clean net classes
    SP_MapString2Netclass::iterator l_itNetclass;
    for (l_itNetclass = m_mNetclasses.begin(); l_itNetclass != m_mNetclasses.end(); ++l_itNetclass)
    {
    	SP_DS_Netclass* l_pcClass = l_itNetclass->second;
        wxDELETE(l_pcClass);
    }
    // clean associations OGL -> Snoopy
    SP_MapExternObjects::iterator l_itExt;
    for (l_itExt = m_mExternObjects.begin(); l_itExt != m_mExternObjects.end(); l_itExt = m_mExternObjects.begin())
    {
        m_mExternObjects.erase(l_itExt);
    }
    // clean dialog widgets
    SP_MapString2DialogWidget::iterator l_itWdg;
    for (l_itWdg = m_mDialogWidgets.begin(); l_itWdg != m_mDialogWidgets.end(); l_itWdg = m_mDialogWidgets.begin())
    {
    	SP_WDG_DialogBase* l_pcDlg = l_itWdg->second;
        wxDELETE(l_pcDlg);
        m_mDialogWidgets.erase(l_itWdg);
    }
    // clean animator registry
    SP_MapString2ListAnimator::iterator l_itAnims;
    for (l_itAnims = m_mlAnimatorRegistry.begin(); l_itAnims != m_mlAnimatorRegistry.end(); l_itAnims = m_mlAnimatorRegistry.begin())
    {
    	SP_ListAnimator::iterator l_itAnimat;
        for (l_itAnimat = l_itAnims->second.begin(); l_itAnimat != l_itAnims->second.end(); l_itAnimat = l_itAnims->second.begin())
        {
        	SP_DS_Animator* l_pcAnim = (*l_itAnimat);
            l_itAnims->second.erase(l_itAnimat);
            wxDELETE(l_pcAnim);
        }
        m_mlAnimatorRegistry.erase(l_itAnims);
    }
    // clean animations
    SP_MapString2Animation::iterator l_itAnim;
    for (l_itAnim = m_mAnimations.begin(); l_itAnim != m_mAnimations.end(); l_itAnim = m_mAnimations.begin())
    {
    	SP_DS_Animation* l_pcAnimation = l_itAnim->second;
        m_mAnimations.erase(l_itAnim);
        wxDELETE(l_pcAnimation);
    }
}

SP_Core*
SP_Core::Instance()
{
    if (!m_pcInstance)
        m_pcInstance = new SP_Core;

    return m_pcInstance;
}

bool
SP_Core::InitialiseNetclasses(SP_GM_Docmanager* p_pcDocmanager)
{
	SP_DS_Netclass* l_pcNet = NULL;
#ifdef __WXDEBUG__
    //////////////////////////////////////////////////////////////////
	l_pcNet = new SP_DS_BipartGraph();
    if (l_pcNet->AddToGui(p_pcDocmanager))
        AddNetclass(l_pcNet);
    else
        wxDELETE(l_pcNet);
#endif //__WXDEBUG__
	//////////////////////////////////////////////////////////////////	    

	l_pcNet = new SP_DS_ColPT();
    if (l_pcNet->AddToGui(p_pcDocmanager))
        AddNetclass(l_pcNet);
    else
        wxDELETE(l_pcNet);

	//////////////////////////////////////////////////////////////////

    l_pcNet = new SP_DS_ColExtPT();
    if (l_pcNet->AddToGui(p_pcDocmanager))
        AddNetclass(l_pcNet);
    else
        wxDELETE(l_pcNet);


    //////////////////////////////////////////////////////////////////

    l_pcNet = new SP_DS_ColSPN();
    if (l_pcNet->AddToGui(p_pcDocmanager))
        AddNetclass(l_pcNet);
    else
        wxDELETE(l_pcNet);

    //////////////////////////////////////////////////////////////////
		
    l_pcNet = new SP_DS_ColCPN();
    if (l_pcNet->AddToGui(p_pcDocmanager))
        AddNetclass(l_pcNet);
    else
        wxDELETE(l_pcNet);

	 //////////////////////////////////////////////////////////////////

    l_pcNet = new SP_DS_ColHPN();
    if (l_pcNet->AddToGui(p_pcDocmanager))
        AddNetclass(l_pcNet);
    else
        wxDELETE(l_pcNet);

    //////////////////////////////////////////////////////////////////
   l_pcNet = new SP_DS_ContinuousPed();
    if (l_pcNet->AddToGui(p_pcDocmanager))
        AddNetclass(l_pcNet);
    else
        wxDELETE(l_pcNet);

    //////////////////////////////////////////////////////////////////    

    l_pcNet = new SP_DS_ExtendedFaultTree();
    if (l_pcNet->AddToGui(p_pcDocmanager))
        AddNetclass(l_pcNet);
    else
        wxDELETE(l_pcNet);

	//////////////////////////////////////////////////////////////////
    l_pcNet = new SP_DS_ExtPT();
    if (l_pcNet->AddToGui(p_pcDocmanager))
        AddNetclass(l_pcNet);
    else
        wxDELETE(l_pcNet);

	//////////////////////////////////////////////////////////////////
    l_pcNet = new SP_DS_FaultTree();
    if (l_pcNet->AddToGui(p_pcDocmanager))
        AddNetclass(l_pcNet);
    else
        wxDELETE(l_pcNet);

	////////////////////////////////////////////////////////////////
    l_pcNet = new SP_DS_Freestyle();
    if (l_pcNet->AddToGui(p_pcDocmanager))
        AddNetclass(l_pcNet);
    else
        wxDELETE(l_pcNet);

    //////////////////////////////////////////////////////////////////
    l_pcNet = new SP_DS_HybridPN();
        if (l_pcNet->AddToGui(p_pcDocmanager))
            AddNetclass(l_pcNet);
         else
            wxDELETE(l_pcNet);

       //////////////////////////////////////////////////////////////////
    l_pcNet = new SP_DS_ModuloNets();
    if (l_pcNet->AddToGui(p_pcDocmanager))
        AddNetclass(l_pcNet);
    else
        wxDELETE(l_pcNet);

	////////////////////////////////////////////////////////////////
    l_pcNet = new SP_DS_MusicPed();
    if (l_pcNet->AddToGui(p_pcDocmanager))
        AddNetclass(l_pcNet);
    else
        wxDELETE(l_pcNet);


	/////////////////////////////////////////////////////////////////
    l_pcNet = new SP_DS_MTBDD();
    if (l_pcNet->AddToGui(p_pcDocmanager))
        AddNetclass(l_pcNet);
    else
        wxDELETE(l_pcNet);

	///////////////////////////////////////////////////////////////
    l_pcNet = new SP_DS_MTIDD();
    if (l_pcNet->AddToGui(p_pcDocmanager))
        AddNetclass(l_pcNet);
    else
        wxDELETE(l_pcNet);

	//////////////////////////////////////////////////////////////////
    l_pcNet = new SP_DS_SimplePed();
    if (l_pcNet->AddToGui(p_pcDocmanager))
        AddNetclass(l_pcNet);
    else
        wxDELETE(l_pcNet);

	//////////////////////////////////////////////////////////////////
    l_pcNet = new SP_DS_ReachabilityGraph();
    if (l_pcNet->AddToGui(p_pcDocmanager))
        AddNetclass(l_pcNet);
    else
        wxDELETE(l_pcNet);

#ifdef __WXDEBUG__
	//////////////////////////////////////////////////////////////////
    l_pcNet = new SP_DS_SimpleGraph();
    if (l_pcNet->AddToGui(p_pcDocmanager))
        AddNetclass(l_pcNet);
    else
        wxDELETE(l_pcNet);
#endif //__WXDEBUG__

    //////////////////////////////////////////////////////////////////
    l_pcNet = new SP_DS_EventSPN();
    if ( l_pcNet->AddToGui( p_pcDocmanager ) )
      AddNetclass(l_pcNet);
    else
      wxDELETE(l_pcNet);

    ////////////////////////////////////////////////////////////////
    l_pcNet = new SP_DS_TimePT();
    if (l_pcNet->AddToGui(p_pcDocmanager))
        AddNetclass(l_pcNet);
    else
        wxDELETE(l_pcNet);

	//////////////////////////////////////////////////////////////////


	return TRUE;
}

///////////////////////////////////////////////////////////////////////////
SP_DS_Netclass*
SP_Core::AddNetclass(SP_DS_Netclass* p_pcNetclass)
{
    CHECK_POINTER(p_pcNetclass, return NULL);

    // search the list of netclasses for one with the same name
    pair<SP_MapString2Netclass::iterator, bool> l_Return = m_mNetclasses.insert(make_pair(p_pcNetclass->GetName(),p_pcNetclass));
    if(l_Return.second == false)
    {
		SetLastError(
			wxString::Format(
			wxT("Netclass with name '%s' already in the database! Returning the old one."),
			p_pcNetclass->GetName().c_str()));
		// delete the parameter, cause it doesn't made it into the list
		wxDELETE(p_pcNetclass);
    }

	return l_Return.first->second;
}

SP_DS_Netclass*
SP_Core::GetNetclass(const wxString& p_pchName)
{
    SP_MapString2Netclass::iterator l_itNetclass = m_mNetclasses.find(p_pchName);
    if(l_itNetclass != m_mNetclasses.end())
    {
    	return l_itNetclass->second;
    }

    SetLastError(wxT("No netclass found"));

    return NULL;
}

int
SP_Core::GetNetclassCount() {
	return m_mNetclasses.size();
}

wxString
SP_Core::GetNetclassName(int p_nItem) {

  SP_MapString2Netclass::iterator l_itNetclass;
  for (l_itNetclass = m_mNetclasses.begin(); l_itNetclass != m_mNetclasses.end(); ++l_itNetclass) {
		if(p_nItem == 0) {
          return l_itNetclass->first;
		}
		//using p_nItem as reverse counter to get the n-th item
		p_nItem--;
  }
	return wxT("");
}

///////////////////////////////////////////////////////////////////////////
bool
SP_Core::AddToDeleteQueue(SP_Graphic* p_pcGraphic)
{
    if (!p_pcGraphic)
        return TRUE;

    // if not already in the map of lists, add our own graphic
    SP_ListGraphic* l_plGraphics = &(m_pmlDeleteDataGraphic[p_pcGraphic->GetParent()]);
    if (SP_Find(*l_plGraphics, p_pcGraphic) == l_plGraphics->end())
    {
        p_pcGraphic->SetDelete(TRUE);
        l_plGraphics->push_back(p_pcGraphic);
    }

    return TRUE;
}

bool
SP_Core::RemoveQueuedElements(bool p_bNoCallRemove)
{
    SP_ListData l_lDelete;
    SP_MapData2ListGraphic::iterator l_Iter;
    for (l_Iter = m_pmlDeleteDataGraphic.begin(); l_Iter != m_pmlDeleteDataGraphic.end(); ++l_Iter)
    {
        SP_Data* l_pcData = l_Iter->first;

        if (l_pcData->RemoveGraphic(&(l_Iter->second)) == SP_NO_MORE_OBJECTS)
        {
            if (p_bNoCallRemove)
                continue;

            if (l_pcData->Remove())
                l_lDelete.push_back(l_pcData);
        }
    }
    // we just removed the dependencies in our DS
    // now really call delete for all queued objects
    SP_ListData::iterator l_Iter2;
    for (l_Iter2 = l_lDelete.begin(); l_Iter2 != l_lDelete.end(); ++l_Iter2)
    {
    	SP_Data* l_pcData = (*l_Iter2);
        wxDELETE(l_pcData);
    }
    m_pmlDeleteDataGraphic.clear();

    return TRUE;
}

///////////////////////////////////////////////////////////////////////////
bool
SP_Core::AssociateExtern(wxShape* p_pcKey, SP_Graphic* p_pcVal)
{
    CHECK_POINTER(p_pcKey, return FALSE);
    CHECK_POINTER(p_pcVal, return FALSE);

    // test, whether we already have a value at this key?
    //m_mExternObjects[p_pcKey] = p_pcVal;
    pair<SP_MapExternObjects::iterator, bool> l_Return =
    	m_mExternObjects.insert(make_pair(p_pcKey, p_pcVal));

    if(l_Return.second == false)
    {
    	SetLastError(wxT("Error while associate extern wxShape with SP_Graphic!"));
    	SP_LOGERROR( GetLastError());
    }
    return l_Return.second;
}

bool
SP_Core::RemoveExtern(wxShape* p_pcKey)
{
    m_mExternObjects.erase(p_pcKey);

    return m_mExternObjects.empty();
}

SP_Graphic*
SP_Core::ResolveExtern(wxShape* p_pcKey)
{
    SP_MapExternObjects::iterator l_it = m_mExternObjects.find(p_pcKey);
	if (l_it != m_mExternObjects.end())
    {
        return l_it->second;
    }

    SetLastError(wxT("unknown canvas object given in ResolveExternal"));

    return NULL;
}

///////////////////////////////////////////////////////////////////////////
SP_WDG_DialogBase*
SP_Core::RegisterDialogWidget(const wxString& p_sNameType, SP_WDG_DialogBase* p_pcWidget)
{
    // remove the old widget, registered for the name_type, if any
	SP_MapString2DialogWidget::iterator l_it = m_mDialogWidgets.find(p_sNameType);
    if(l_it!= m_mDialogWidgets.end())
    {
        SP_WDG_DialogBase* l_pcWidget = l_it->second;
        wxDELETE(l_pcWidget);
        l_it->second = p_pcWidget;
    }
    else
    {
    	m_mDialogWidgets[p_sNameType] = p_pcWidget;
    }
    return p_pcWidget;
}

SP_WDG_DialogBase*
SP_Core::RegisterDialogWidget(SP_DS_Attribute* p_pcData, SP_WDG_DialogBase* p_pcWidget)
{
    if (!p_pcData || !p_pcData->GetParent() || !p_pcWidget)
        return NULL;

    wxString l_sKey = GetDialogWidgetKey(p_pcData);

    return RegisterDialogWidget(l_sKey, p_pcWidget);
}

SP_WDG_DialogBase*
SP_Core::GetDialogWidget(const wxString& p_sNameType)
{
    if (m_mDialogWidgets.find(p_sNameType) == m_mDialogWidgets.end())
    {
        // m_mDialogWidgets[p_sNameType] = new SP_WDG_DialogText(wxT("General"));
        return NULL;
    }

    return m_mDialogWidgets[p_sNameType]->Clone();
}

SP_WDG_DialogBase*
SP_Core::GetDialogWidget(SP_DS_Attribute* p_pcData)
{
    return GetDialogWidget(GetDialogWidgetKey(p_pcData));
}

wxString
SP_Core::GetDialogWidgetKey(SP_DS_Attribute* p_pcData)
{
    wxString l_sKey = wxString::Format(wxT("%s|%s|%s|%d"),
    	p_pcData->GetParent()->GetClassObject()->GetParentGraph()->GetNetclass()->GetName().c_str(),
        p_pcData->GetParent()->GetClassName().c_str(),
        p_pcData->GetName().c_str(),
        p_pcData->GetAttributeType());

    return l_sKey;
}

///////////////////////////////////////////////////////////////////////////
bool
SP_Core::AddDialogGraphic(SP_Graphic* p_pcVal)
{
    if (SP_Find(m_lDialogGraphics, p_pcVal) == m_lDialogGraphics.end())
        m_lDialogGraphics.push_back(p_pcVal);

    return TRUE;
}

bool
SP_Core::Copy(SP_DS_Graph* p_pcGraph, unsigned int p_nNr, SP_ListGraphic* p_plShapes)
{
    CHECK_POINTER(p_pcGraph, return FALSE);
    CHECK_POINTER(p_plShapes, return FALSE);
    CHECK_BOOL((p_plShapes->size() != 0), return FALSE);

    bool l_bReturn = TRUE;
    SP_MapGraphic2Graphic l_mGraphic2Graphic;
    SP_MapData2Data l_mData2Data;
    SP_ListGraphic::iterator l_Iter;

    // prepares the new graph
    l_bReturn &= ResetCopyState(p_pcGraph);

    m_nCopyFromNetnumber = p_nNr;
    //p_pcGraph->ShowDebug();

    // collect all data into a new graph
    for (l_Iter = p_plShapes->begin(); l_Iter != p_plShapes->end(); ++l_Iter)
        l_bReturn &= (*l_Iter)->AddToCopyMaps(&m_pcCopyGraph, &l_mGraphic2Graphic, &l_mData2Data);

    return l_bReturn;
}

bool
SP_Core::Paste(SP_DS_Graph* p_pcGraph, SP_GUI_Canvas* p_pcCanvas)
{
    CHECK_POINTER(p_pcCanvas, return FALSE);
    CHECK_POINTER(p_pcGraph, return FALSE);
    CHECK_POINTER(p_pcGraph->GetNetclass(), return FALSE);
    CHECK_POINTER(m_pcCopyGraph, return FALSE);
    CHECK_POINTER(m_pcCopyGraph->GetNetclass(), return FALSE);
	CHECK_BOOL((p_pcGraph->GetNetclass()->GetName().IsSameAs(m_pcCopyGraph->GetNetclass()->GetName())), return FALSE);

    // m_pcCopyGraph->OnSaveDocument(wxString::Format(wxT("c:\\%s.pn", wxDateTime::Now().Format("%H%M%S")).c_str()));
    bool l_bReturn = TRUE;
    // reorder the net number members of the elements
    unsigned int l_nNr = p_pcCanvas->GetNetnumber();
    m_pcCopyGraph->ResetNetnumbers(m_nCopyFromNetnumber, l_nNr, p_pcGraph);
    //p_pcGraph->ShowDebug();
    // repeat the steps, done in Copy
    SP_MapGraphic2Graphic l_mGraphic2Graphic;
    SP_MapData2Data l_mData2Data;
    SP_MapData2Data::iterator l_Iter;
    SP_MapGraphic2Graphic::iterator l_Iter2;
    // reuse the code from 'copy' just in the other direction
    m_pcCopyGraph->AddToCopyMap(&p_pcGraph, l_nNr, &l_mGraphic2Graphic, &l_mData2Data);

    wxClientDC l_cDC(p_pcCanvas);
    p_pcCanvas->DoPrepareDC(l_cDC);
    l_bReturn = p_pcGraph->ShowOnCanvas(p_pcCanvas);

    // reset every special meaning in the attributes (i.e. beeing logic)
    for (l_Iter = l_mData2Data.begin(); l_Iter != l_mData2Data.end(); ++l_Iter)
        (*l_Iter).second->SynchronizeAttributes();

    // additional traversing to get the values of associated attributes right.
    // there is a different in the values of SP_DS_IdAttributes after pasting,
    // if they belong to logical nodes without this round.
    // watch performance issues here!
    for (l_Iter2 = l_mGraphic2Graphic.begin(); l_Iter2 != l_mGraphic2Graphic.end(); ++l_Iter2)
        if ((*l_Iter2).second->GetParent())
            (*l_Iter2).second->GetParent()->Update();

    // traverse again to select all new created graphics in the actual canvas
    for (l_Iter2 = l_mGraphic2Graphic.begin(); l_Iter2 != l_mGraphic2Graphic.end(); ++l_Iter2)
    {
        // actual canvas nr only and only nodes and edges, they
        // will select their attributes itself
        if (((*l_Iter2).second->GetNetnumber() == l_nNr) &&
            ((*l_Iter2).second->GetGraphicType() != SP_GRAPHIC_ATTRIBUTE) )
        {
            (*l_Iter2).second->Translate(m_nPasteOffset, m_nPasteOffset);
            (*l_Iter2).second->Select(TRUE, &l_cDC);
        }
    }

    // multiple paste operations can occur but we don't know in what net we will be next.
    // but we know the pseudo 'from' netnumber, we just reordered our copy graph to.
    // so, the next paste, if we don't have a copy before, will start the reorganisation
    // with this new number.
    m_nCopyFromNetnumber = l_nNr;
    m_nPasteOffset += 20;

    return l_bReturn;
}

bool
SP_Core::ResetCopyState(SP_DS_Graph* p_pcGraph)
{
    if (m_pcCopyGraph)
    {
        wxDELETE(m_pcCopyGraph);
        m_pcCopyGraph = NULL;
    }
    if (p_pcGraph)
    {
        m_pcCopyGraph = p_pcGraph->CloneDefinition();
        m_pcCopyGraph->SetIsCopy(TRUE);
    }

    m_nCopyFromNetnumber = 0;
    m_nPasteOffset = 20;

    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
void
SP_Core::SetAnimMode(bool p_bVal, SP_DS_Graph* p_pcGraph)
{
    if (m_bAnimMode == p_bVal)
        return;

    if (p_bVal)
    {
        CleanUpAnimation();
        m_pcAnimation = GetAnimation(p_pcGraph);
        if (!m_pcAnimation)
            return;

        m_pcAnimControl = new SP_DLG_Animation(m_pcAnimation, NULL);
        m_pcAnimControl->Show(TRUE);
        m_bAnimMode = TRUE;
    }
    else
    {
        if (m_pcAnimControl)
        {
			m_pcAnimation->StopTimer();
			m_pcAnimControl->CleanUp();
            m_pcAnimControl->Show(FALSE);
            m_pcAnimControl->Destroy();
            m_pcAnimControl = NULL;
        }


	// Undo Last Action in connected Nets
	//this means we will uncolour the green actual state in RG

	SP_MDI_Doc* l_pcMyDoc = m_pcInstance->GetRootDocument();
	CHECK_POINTER(l_pcMyDoc, return );

	SP_IA_Manager* l_pcIAManager = wxGetApp().GetIAManager();
	if (l_pcIAManager && l_pcIAManager->IAModeEnabled(l_pcMyDoc->GetFilename())){
		wxArrayString l_AStrConnectedNets = l_pcIAManager->GetConnectedNets(l_pcMyDoc->GetFilename());
		for (unsigned int i = 0; i < l_AStrConnectedNets.GetCount(); i++) {
			SP_DS_Graph* l_pcTargetGraph = l_pcIAManager->GetGraphFromFilename(l_AStrConnectedNets[i]);
			wxString l_sTargetNetName =	 l_pcTargetGraph->GetNetclass()->GetName();

			if (l_sTargetNetName.CmpNoCase(SP_DS_REACHABILITY_CLASS) == 0){

				 l_pcIAManager->UndoLastSubmit(l_pcTargetGraph, true);

			} //end if
		}	//end for

	}  //end if



        m_bAnimMode = !CleanUpAnimation();
        m_pcActiveRootDocument->DrawAllElements();
    }
}

bool
SP_Core::CleanUpAnimation()
{
    if (m_pcAnimation)
    {
        wxDELETE(m_pcAnimation);
        m_pcAnimation = NULL;
    }
    return TRUE;
}

bool
SP_Core::GetAnimRunning() const
{
    return (m_pcAnimation)?m_pcAnimation->GetRunning():FALSE;
}

void
SP_Core::SetAnimRunning(bool p_bVal)
{
    if (m_pcAnimation)
        m_pcAnimation->SetRunning(p_bVal);
}

bool
SP_Core::RegisterAnimation(SP_DS_Netclass* p_pcNetclass, SP_DS_Animation* p_pcAnimation)
{
    if (!p_pcNetclass)
        return FALSE;

    return RegisterAnimation(p_pcNetclass->GetName(), p_pcAnimation);
}

bool
SP_Core::RegisterAnimation(const wxString& p_sKey, SP_DS_Animation* p_pcAnimation)
{
    if (!p_pcAnimation)
        return FALSE;

    SP_MapString2Animation::iterator l_it = m_mAnimations.find(p_sKey);
    if(l_it != m_mAnimations.end())
    {
        SP_DS_Animation* l_pcAnim = l_it->second;
        wxDELETE(l_pcAnim);
        l_it->second = p_pcAnimation;
    }
    else
    {
    	m_mAnimations.insert(make_pair(p_sKey, p_pcAnimation));
    }
    return TRUE;
}

SP_DS_Animation*
SP_Core::GetAnimation(SP_DS_Graph* p_pcGraph)
{
    if (!p_pcGraph || !p_pcGraph->GetNetclass())
        return NULL;

    SP_DS_Animation* l_pcReturn = NULL;
    bool l_bInitialResult = FALSE;
    SP_MapString2Animation::iterator l_it = m_mAnimations.find(p_pcGraph->GetNetclass()->GetName());
    if (l_it != m_mAnimations.end())
    {
        l_pcReturn = l_it->second->Clone();
        l_bInitialResult = l_pcReturn->Initialise(p_pcGraph);
    }
    if (l_bInitialResult)
    {
    	return l_pcReturn;
    }
    else
    {
    	wxDELETE(l_pcReturn);
    	return NULL;
    }
}

bool
SP_Core::RegisterAnimator(const wxString& p_sKey, SP_DS_Animator* p_pcAnimator)
{
    if (!p_pcAnimator)
        return FALSE;

    SP_MapString2ListAnimator::iterator l_it = m_mlAnimatorRegistry.find(p_sKey);
    if(l_it != m_mlAnimatorRegistry.end())
    {
    	SP_ListAnimator* l_plAnimators = &(l_it->second);
        SP_ListAnimator::iterator l_Iter;
        for (l_Iter = l_plAnimators->begin(); l_Iter != l_plAnimators->end(); ++l_Iter)
        {
            if ((*l_Iter)->GetAnimatorType() == p_pcAnimator->GetAnimatorType())
            {
				SP_DS_Animator* tmpA = *l_Iter;
				l_plAnimators->erase(l_Iter);
				wxDELETE(tmpA);
				break;
            }
        }
        l_plAnimators->push_back(p_pcAnimator);
    }
    else
    {
    	m_mlAnimatorRegistry[p_sKey].push_back(p_pcAnimator);
    }
    return TRUE;
}

bool
SP_Core::RegisterAnimator(SP_DS_Node* p_pcNode, SP_DS_Animator* p_pcAnimator)
{
    if (!p_pcNode || !p_pcAnimator)
        return FALSE;

    return RegisterAnimator(p_pcNode->GetNodeclass(), p_pcAnimator);
}

bool
SP_Core::RegisterAnimator(SP_DS_Edge* p_pcEdge, SP_DS_Animator* p_pcAnimator)
{
    if (!p_pcEdge || !p_pcAnimator)
        return FALSE;

    return RegisterAnimator(p_pcEdge->GetEdgeclass(), p_pcAnimator);
}

bool
SP_Core::RegisterAnimator(SP_ElementClass* p_pcClass, SP_DS_Animator* p_pcAnimator)
{
    if (!p_pcClass || !p_pcAnimator || !p_pcClass->GetParentGraph())
        return FALSE;

    wxString l_sKey = wxString::Format(wxT("%s|%s"),
        p_pcClass->GetParentGraph()->GetName().c_str(),
        p_pcClass->GetName().c_str());

    return RegisterAnimator(l_sKey, p_pcAnimator);
}

SP_ListAnimator*
SP_Core::GetAnimators(SP_ElementClass* p_pcClass)
{
    if (!p_pcClass || !p_pcClass->GetParentGraph())
        return NULL;

    wxString l_sKey = wxString::Format(wxT("%s|%s"),
        p_pcClass->GetParentGraph()->GetName().c_str(),
        p_pcClass->GetName().c_str());

    SP_MapString2ListAnimator::iterator l_it = m_mlAnimatorRegistry.find(l_sKey);
    if(l_it == m_mlAnimatorRegistry.end())
    {
        return NULL;
    }
    else
    {
    	return &(l_it->second);
    }
}

bool
SP_Core::ManageDocChange(SP_MDI_Doc* p_pcDoc) {

	if (m_pcActiveRootDocument != p_pcDoc)
	{
		if (GetAnimMode())
		{
			SetAnimMode(false, m_pcActiveRootDocument->GetGraph());
			m_mRestartAnimMode[m_pcActiveRootDocument] = true;
		}
		else
		{
			m_mRestartAnimMode[m_pcActiveRootDocument] = false;
		}
		m_pcActiveRootDocument = p_pcDoc;
		if (m_mRestartAnimMode[m_pcActiveRootDocument])
			SetAnimMode(true, m_pcActiveRootDocument->GetGraph());
	}

	return true;

}

void
SP_Core::SetSimulationMode(bool p_bVal)
{
	m_bSimulationMode = p_bVal;
}

bool
SP_Core::GetSimulationMode() const
{
	return m_bSimulationMode;
}

