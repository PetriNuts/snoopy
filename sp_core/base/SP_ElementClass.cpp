//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_core/base/SP_ElementClass.h"
#include "sp_core/SP_Core.h"

#include <wx/wx.h>

SP_ElementClass::SP_ElementClass(SP_ELEMENT_TYPE p_eType, const wxString& p_pchName, const LongPtr& p_nIdCounter):
SP_Type(p_eType),
SP_Name(p_pchName),
m_nIdCounter(p_nIdCounter),
m_bShowInElementTree(true),
m_bShowInDeclarationTreeOther(false),
m_bShowInDeclarationTreeColorSet(false)
{
	if(!m_nIdCounter)
	{
		m_nIdCounter = LongPtr(new long(-1));
		m_bIdOwner = true;
	}
	else
	{
		m_bIdOwner = false;
	}
}

SP_ElementClass::~SP_ElementClass()
{
}

void SP_ElementClass::ResetIdCount()
{
	if(m_bIdOwner)
	{
		*m_nIdCounter = 0;
	}
}

long
SP_ElementClass::GetNewIdCount()
{
	long l_nReturn = *m_nIdCounter;
	++(*m_nIdCounter);
	return l_nReturn;
}


bool
SP_ElementClass::TestIdCount(long p_nVal)
{
	if (p_nVal > *m_nIdCounter) *m_nIdCounter = p_nVal;
	return TRUE;
}

const LongPtr& SP_ElementClass::GetIdCountPtr()
{
	return m_nIdCounter;
}

bool
SP_ElementClass::AddAnimator(SP_DS_Animator* p_pcAnimator)
{
    if (!SP_Core::Instance()->RegisterAnimator(this, p_pcAnimator))
        return FALSE;

    if (m_pcGraph)
        m_pcGraph->SetHasAnimators(TRUE);

    return TRUE;
}

SP_WDG_DialogBase*
SP_ElementClass::RegisterGraphicWidget(SP_WDG_DialogBase* p_pcWidget)
{
    if (!m_pcGraph || !m_pcGraph->GetNetclass())
        return NULL;

    wxString l_sKey;
	//  << _T("|") << p_pcWidget->GetName()
    l_sKey << m_pcGraph->GetNetclass()->GetName() << _T("|")
        << GetName();

    return SP_Core::Instance()->RegisterDialogWidget(l_sKey, p_pcWidget);
}
