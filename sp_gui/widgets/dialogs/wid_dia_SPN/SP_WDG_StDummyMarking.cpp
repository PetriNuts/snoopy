//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: slehrack $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2007/06/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#include "sp_gui/widgets/dialogs/wid_dia_SPN/SP_WDG_StDummyMarking.h"
#include <wx/event.h>

BEGIN_EVENT_TABLE( SP_WDG_StDummyMarking, SP_WDG_DialogBase )

END_EVENT_TABLE()


SP_WDG_StDummyMarking::SP_WDG_StDummyMarking(const wxString& p_sPage, unsigned int p_nOrdering)
:SP_WDG_DialogBase(p_sPage, p_nOrdering),
 m_pcDlg(NULL)
{
}

SP_WDG_StDummyMarking::~SP_WDG_StDummyMarking()
{
}

SP_WDG_DialogBase*
SP_WDG_StDummyMarking::Clone()
{

  return new SP_WDG_StDummyMarking(GetName(), GetDialogOrdering());

}

bool
SP_WDG_StDummyMarking::AddToDialog( const SP_ListAttribute* p_ptlAttributes,
  SP_DLG_ShapeProperties* p_pcDlg, bool p_bOverview )
{

  return TRUE;

}

bool
SP_WDG_StDummyMarking::OnDlgOk()
{

  return SP_WDG_DialogBase::OnDlgOk();

}


