//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Attribute Event Handler
//////////////////////////////////////////////////////////////////////
#include "sp_gr/eventhandler/SP_GRM_AttributeHandler.h"
#include "sp_core/SP_Core.h"

#include "sp_gui/windows/SP_GUI_Canvas.h"
#include "sp_gui/mdi/SP_MDI_View.h"

SP_GRM_AttributeHandler::SP_GRM_AttributeHandler(wxShapeEvtHandler* p_pcPrev, wxShape* p_pcShape)
:SP_GRM_EventHandler(p_pcPrev, p_pcShape)
{
}


void
SP_GRM_AttributeHandler::DoSnap(double* p_pnX, double* p_pnY, int p_nKeys)
{
    if (GetShape() &&
        GetShape()->GetCanvas())
    {
        if (p_nKeys == KEY_CTRL)
            GetShape()->GetCanvas()->Snap(p_pnX, p_pnY);
    }
}

void
SP_GRM_AttributeHandler::OnBeginDragLeft(double p_nX, double p_nY, int p_nKeys, int p_nAttach)
{
    UnSelectAll(p_nKeys);
    SP_GRM_EventHandler::OnBeginDragLeft(p_nX, p_nY, p_nKeys, p_nAttach);
}
