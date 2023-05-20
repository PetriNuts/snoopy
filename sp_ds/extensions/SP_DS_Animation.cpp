//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_ds/extensions/SP_DS_Animation.h"
#include "sp_ds/SP_DS_Animator.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_gui/windows/SP_GUI_Canvas.h"
#include "sp_gui/dialogs/SP_DLG_AnimationProperties.h"
#include "sp_core/SP_Core.h"

BEGIN_EVENT_TABLE(SP_DS_Animation, wxEvtHandler)
END_EVENT_TABLE()

SP_DS_Animation::SP_DS_Animation(unsigned int p_nRefresh, unsigned int p_nDuration,bool b_bColSimMode)
  : m_cTimer(this, (int)GetId()),
    m_bConnected(FALSE),
    m_bRunning(FALSE),
    m_bOneShot(FALSE),
    m_nRefreshFrequ(p_nRefresh),
    m_nStepDuration(p_nDuration),
    m_nSteps(p_nDuration / p_nRefresh),
    m_nActStep(0),
	m_nDir(FORWARD),
    m_sDlgFrequency(wxT("")),
    m_sDlgDuration(wxT("")),
	m_nPlaceCount(0),
	m_msPlaceNamesById(0),
	m_msSimulationTables(0),
	m_msSimulationPlots(0),
	m_msMarkingSets(0),
	m_msFunctionSets(0),
	m_msParameterSets(0),
	m_mnResultsMap(0),
	m_bColSimMode(b_bColSimMode)
{
	m_bIsActiveAnimator = false;
    ConnectEvent();
}

SP_DS_Animation::~SP_DS_Animation()
{
    CleanAnimators(&m_lAllAnimators);
    DisconnectEvent();
}

SP_DS_Animation*
SP_DS_Animation::Clone()
{
    return new SP_DS_Animation(m_nRefreshFrequ, m_nStepDuration);
}

bool
SP_DS_Animation::Initialise(SP_DS_Graph* p_pcGraph)
{
    if (!p_pcGraph)
        return FALSE;

	m_pcGraph = p_pcGraph;
    CleanAnimators(&m_lAllAnimators);

    SP_ListNodeclass::const_iterator l_itNC;
    SP_ListEdgeclass::const_iterator l_itEC;
    SP_ListMetadataclass::const_iterator l_itMC;
    bool l_bReturn = TRUE;
    for (l_itNC = p_pcGraph->GetNodeclasses()->begin(); l_bReturn && l_itNC != p_pcGraph->GetNodeclasses()->end(); ++l_itNC)
        l_bReturn &= FillAnimators((*l_itNC));

    for (l_itEC = p_pcGraph->GetEdgeclasses()->begin(); l_bReturn && l_itEC != p_pcGraph->GetEdgeclasses()->end(); ++l_itEC)
        l_bReturn &= FillAnimators((*l_itEC));

    for (l_itMC = p_pcGraph->GetMetadataclasses()->begin(); l_bReturn && l_itMC != p_pcGraph->GetMetadataclasses()->end(); ++l_itMC)
        l_bReturn &= FillAnimators((*l_itMC));

    return l_bReturn;
}

void
SP_DS_Animation::OnTimer(wxTimerEvent& p_cEvent)
{
	if (m_bColSimMode) return;//for colsimulation 

    bool res = TRUE;

    if (m_nActStep == 0)
	{
		res = PreStep();
	}

    res &= Step();
    Refresh();

	if (m_nActStep == m_nSteps) 
	{
		res &= PostStep();
        if (m_bOneShot)
        {
            StopTimer();
            m_bOneShot = FALSE;

            return;
        }
    }

    if (res) 
	{
		m_cTimer.Start(m_nRefreshFrequ, wxTIMER_ONE_SHOT);
    } 
	else 
	{
		StopTimer();
    }
}

void
SP_DS_Animation::Refresh()
{
    set<SP_GUI_Canvas*>::iterator l_Iter;
    for (l_Iter = m_tsCanvas.begin(); l_Iter != m_tsCanvas.end(); ++l_Iter)
        (*l_Iter)->Refresh(FALSE);
}

bool
SP_DS_Animation::PreStep()
{
    m_lStepAnimators.clear();
    SP_ListAnimator::iterator l_itAnims;
    for (l_itAnims = m_lAllAnimators.begin(); l_itAnims != m_lAllAnimators.end(); ++l_itAnims)
    {
		if ((*l_itAnims)->PreStep(m_nStepDuration / m_nRefreshFrequ, FORWARD)) 
		{
			m_lStepAnimators.push_back((*l_itAnims));
		}
    }
	m_nActStep++;

    return (m_lStepAnimators.size() > 0);
}

bool
SP_DS_Animation::Step()
{
    bool l_bReturn = TRUE;
    SP_ListAnimator::iterator l_itAnims;
    for (l_itAnims = m_lStepAnimators.begin(); l_bReturn && l_itAnims != m_lStepAnimators.end(); ++l_itAnims)
    {
        l_bReturn &= (*l_itAnims)->Step(m_nActStep);
    }
	m_nActStep++;

    return l_bReturn;
}

bool
SP_DS_Animation::PostStep()
{
    bool l_bReturn = TRUE;
    SP_ListAnimator::iterator l_itAnims;
    for (l_itAnims = m_lStepAnimators.begin(); l_itAnims != m_lStepAnimators.end(); ++l_itAnims)
    {
        l_bReturn &= (*l_itAnims)->PostStep();
    }
    m_lStepAnimators.clear();
	m_nActStep = 0;
    Refresh();

    return TRUE;
}

bool
SP_DS_Animation::RestartTimer()
{
    return StopTimer() && StartTimer();
}

bool
SP_DS_Animation::StartTimer()
{
	m_tsCanvas.clear();
    m_cTimer.Start(m_nRefreshFrequ, wxTIMER_ONE_SHOT);

    m_bRunning = m_cTimer.IsRunning();

    return m_bRunning;
}

bool
SP_DS_Animation::StopTimer()
{
    if (m_cTimer.IsRunning())
        m_cTimer.Stop();

    m_bRunning = m_cTimer.IsRunning();

    PostStep();
    // m_tsCanvas.clear();

    return !m_bRunning;
}

bool
SP_DS_Animation::OnLeftClick(SP_Data* p_pcElement)
{
    if (!p_pcElement || !GetAnimators(p_pcElement) || m_bRunning)
        return FALSE;

    bool l_bReturn = TRUE;
    SP_ListAnimator::iterator l_itAnims;
    for (l_itAnims = GetAnimators(p_pcElement)->begin(); l_itAnims != GetAnimators(p_pcElement)->end(); ++l_itAnims)
    {
        l_bReturn &= (*l_itAnims)->OnLeftClick();
    }

    return l_bReturn;
}

bool
SP_DS_Animation::OnRightClick(SP_Data* p_pcElement)
{
    if (!p_pcElement || !GetAnimators(p_pcElement) || m_bRunning)
        return FALSE;

    bool l_bReturn = TRUE;
    SP_ListAnimator::iterator l_itAnims;
    for (l_itAnims = GetAnimators(p_pcElement)->begin(); l_itAnims != GetAnimators(p_pcElement)->end(); ++l_itAnims)
    {
        l_bReturn &= (*l_itAnims)->OnRightClick();
    }

    return l_bReturn;
}

void
SP_DS_Animation::ConnectEvent()
{
    if (!m_bConnected)
        Connect((int)GetId(), wxEVT_TIMER, (wxObjectEventFunction) (wxEventFunction) (wxTimerEventFunction) & SP_DS_Animation::OnTimer );

    m_bConnected = TRUE;
}

void
SP_DS_Animation::DisconnectEvent()
{
    if (m_bConnected)
        m_bConnected = !Disconnect((int)GetId(), wxEVT_TIMER, (wxObjectEventFunction) (wxEventFunction) (wxTimerEventFunction) & SP_DS_Animation::OnTimer );
}

bool
SP_DS_Animation::CleanAnimators(SP_ListAnimator* p_plAnims)
{
    CHECK_POINTER(p_plAnims, return TRUE);

    SP_ListAnimator::iterator l_Iter;
    SP_DS_Animator* l_pcAnim;
    for (l_Iter = p_plAnims->begin(); l_Iter != p_plAnims->end(); l_Iter = p_plAnims->begin())
    {
        l_pcAnim = (*l_Iter);
        p_plAnims->erase(l_Iter);
        wxDELETE(l_pcAnim);
    }
    return TRUE;
}


bool
SP_DS_Animation::FillAnimators(SP_DS_Nodeclass* p_pcClass)
{
    CHECK_POINTER(p_pcClass, return FALSE);

    bool l_bReturn = TRUE;
    SP_ListAnimator* l_plAnims = SP_Core::Instance()->GetAnimators(p_pcClass);
    SP_ListNode::const_iterator l_itElem;
    for (l_itElem = p_pcClass->GetElements()->begin(); l_bReturn && l_itElem != p_pcClass->GetElements()->end(); ++l_itElem)
        l_bReturn &= FillAnimators((*l_itElem), l_plAnims);

    return l_bReturn;
}

bool
SP_DS_Animation::FillAnimators(SP_DS_Node* p_pcNode, SP_ListAnimator* p_plAnims)
{
    CHECK_POINTER(p_pcNode, return FALSE);
    if (NULL == p_plAnims)
    {
      return TRUE;
    }
    //CHECK_POINTER(p_plAnims, return TRUE);

    SP_ListAnimator::iterator l_itAnim;
    for (l_itAnim = p_plAnims->begin(); l_itAnim != p_plAnims->end(); ++l_itAnim)
        m_lAllAnimators.push_back((*l_itAnim)->Clone(p_pcNode, this));

    return TRUE;
}

bool
SP_DS_Animation::FillAnimators(SP_DS_Edgeclass* p_pcClass)
{
    CHECK_POINTER(p_pcClass, return FALSE);

    bool l_bReturn = TRUE;

    SP_ListAnimator* l_plAnims = SP_Core::Instance()->GetAnimators(p_pcClass);
    SP_ListEdge::const_iterator l_itElem;

    for (l_itElem = p_pcClass->GetElements()->begin(); l_bReturn && l_itElem != p_pcClass->GetElements()->end(); ++l_itElem)
        l_bReturn &= FillAnimators((*l_itElem), l_plAnims);

    return l_bReturn;
}

bool
SP_DS_Animation::FillAnimators(SP_DS_Edge* p_pcEdge, SP_ListAnimator* p_plAnims)
{
    CHECK_POINTER(p_pcEdge, return FALSE);
    if (NULL == p_plAnims)
    {
      return TRUE;
    }
    //CHECK_POINTER(p_plAnims, return TRUE);

    SP_ListAnimator::iterator l_itAnim;
    for (l_itAnim = p_plAnims->begin(); l_itAnim != p_plAnims->end(); ++l_itAnim)
        m_lAllAnimators.push_back((*l_itAnim)->Clone(p_pcEdge, this));

    return TRUE;
}

bool
SP_DS_Animation::FillAnimators(SP_DS_Metadataclass* p_pcClass)
{
    CHECK_POINTER(p_pcClass, return FALSE);

    bool l_bReturn = TRUE;
    SP_ListAnimator* l_plAnims = SP_Core::Instance()->GetAnimators(p_pcClass);
    SP_ListMetadata::const_iterator l_itElem;
    for (l_itElem = p_pcClass->GetElements()->begin(); l_bReturn && l_itElem != p_pcClass->GetElements()->end(); ++l_itElem)
        l_bReturn &= FillAnimators((*l_itElem), l_plAnims);

    return l_bReturn;
}

bool
SP_DS_Animation::FillAnimators(SP_DS_Metadata* p_pcMetadata, SP_ListAnimator* p_plAnims)
{
    CHECK_POINTER(p_pcMetadata, return FALSE);
    if (NULL == p_plAnims)
    {
      return TRUE;
    }
    //CHECK_POINTER(p_plAnims, return TRUE);

    SP_ListAnimator::iterator l_itAnim;
    for (l_itAnim = p_plAnims->begin(); l_itAnim != p_plAnims->end(); ++l_itAnim)
        m_lAllAnimators.push_back((*l_itAnim)->Clone(p_pcMetadata, this));

    return TRUE;
}

// temporary map of sp_data 2 anim pointers
bool
SP_DS_Animation::AddAnimator(SP_Data* p_pcParent, SP_DS_Animator* p_pcAnimator)
{
    CHECK_POINTER(p_pcParent, return FALSE);
    CHECK_POINTER(p_pcAnimator, return FALSE);

    if (m_mlData2Animator.find(p_pcParent) != m_mlData2Animator.end())
        m_mlData2Animator[p_pcParent].push_back(p_pcAnimator);

    if (SP_Find(m_mlData2Animator[p_pcParent], p_pcAnimator) == m_mlData2Animator[p_pcParent].end())
        m_mlData2Animator[p_pcParent].push_back(p_pcAnimator);

    return TRUE;
}

SP_ListAnimator*
SP_DS_Animation::GetAnimators(SP_Data* p_pcParent)
{
    CHECK_POINTER(p_pcParent, return NULL);

    if (m_mlData2Animator.find(p_pcParent) == m_mlData2Animator.end())
        return NULL;

    return &(m_mlData2Animator[p_pcParent]);
}

SP_DS_Animator*
SP_DS_Animation::GetAnimator(SP_Data* p_pcParent, SP_DS_ANIMATOR_TYPE p_eType)
{
    SP_ListAnimator* p_plAnims = GetAnimators(p_pcParent);
    SP_ListAnimator::iterator l_Iter;
    if (p_plAnims)
    {
        for (l_Iter = p_plAnims->begin(); l_Iter != p_plAnims->end(); ++l_Iter)
            if ((*l_Iter)->GetAnimatorType() == p_eType)
                return (*l_Iter);
    }

    return NULL;
}

bool
SP_DS_Animation::AddToDialog(SP_DLG_AnimationProperties* p_pcDlg, wxSizer* p_pcSizer)
{
    if (!p_pcDlg || !p_pcSizer)
        return FALSE;

    m_sDlgFrequency = wxString::Format(wxT("%d"), m_nRefreshFrequ);
    m_sDlgDuration = wxString::Format(wxT("%d"), m_nStepDuration);

    wxBoxSizer* l_pcSizer = new wxBoxSizer(wxHORIZONTAL);
    l_pcSizer->Add(new wxStaticText(p_pcDlg, -1, wxT("Refresh (ms)")), 0, wxALL , 2);
    l_pcSizer->Add(new wxTextCtrl(p_pcDlg, -1, m_sDlgFrequency, wxDefaultPosition, wxDefaultSize, 0,
        wxTextValidator(wxFILTER_NUMERIC, &m_sDlgFrequency)), 1, wxALL, 2);
    p_pcSizer->Add(l_pcSizer, 0, wxALL | wxEXPAND, 5);

    l_pcSizer = new wxBoxSizer(wxHORIZONTAL);
    l_pcSizer->Add(new wxStaticText(p_pcDlg, -1, wxT("Duration (ms)")), 0, wxALL , 2);
    l_pcSizer->Add(new wxTextCtrl(p_pcDlg, -1, m_sDlgDuration, wxDefaultPosition, wxDefaultSize, 0,
        wxTextValidator(wxFILTER_NUMERIC, &m_sDlgDuration)), 1, wxALL, 2);
    p_pcSizer->Add(l_pcSizer, 0, wxALL | wxEXPAND, 5);

    return TRUE;
}

bool
SP_DS_Animation::OnDialogOk()
{
    ResetErrors();
    long l_nVal = m_nRefreshFrequ;
    if (m_sDlgFrequency.ToLong(&l_nVal) && (l_nVal > 0))
        m_nRefreshFrequ = (unsigned int)l_nVal;
    else
    {
        SetLastError(wxT("Please enter a positive, non-zero value for Refresh"));
        return FALSE;
    }

    l_nVal = m_nStepDuration;
    if (m_sDlgDuration.ToLong(&l_nVal) && (l_nVal > 0))
        m_nStepDuration = (unsigned int)l_nVal;
    else
    {
        SetLastError(wxT("Please enter a positive, non-zero value for Duration"));
        return FALSE;
    }

    if((m_nRefreshFrequ*4) > m_nStepDuration)
    {
        SetLastError(wxT("Duration has to be at least 4 * Refresh"));
        return FALSE;
    }

    m_nSteps = m_nStepDuration / m_nRefreshFrequ;

    return TRUE;
}

void
SP_DS_Animation::OnReset()
{
	Refresh();
}
