
/* SP_DS_ThreadEvent.cpp
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 22.10.2011
 * Short Description:
 */
 //==============================================================================

#include "sp_ds/extensions/SP_DS_ThreadEvent.h"

DEFINE_EVENT_TYPE(spEVT_THREAD)

SP_DS_ThreadEvent::SP_DS_ThreadEvent(wxEventType eventType,const int& p_nID)
:wxEvent(p_nID,eventType)
{
}

SP_DS_ThreadEvent::~SP_DS_ThreadEvent()
{
}
