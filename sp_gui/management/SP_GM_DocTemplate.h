//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: 
//////////////////////////////////////////////////////////////////////
#ifndef __SP_GM_DOCTEMPLATE_H__
#define __SP_GM_DOCTEMPLATE_H__

#include <wx/docview.h>

#include "sp_ds/SP_DS_Netclass.h"
#include "sp_gui/management/SP_GM_Docmanager.h"

class SP_GM_DocTemplate:public wxDocTemplate
{
private:
protected:
    SP_DS_Netclass* m_pcNetclass;
public:
    SP_GM_DocTemplate(SP_GM_Docmanager* p_pcDocmanager,
        SP_DS_Netclass* p_pcNetclass,
        const wxString& p_sDescr,
        const wxString& p_sFilter,
        const wxString& p_sDir,
        const wxString& p_sExt,
        const wxString& p_sDocTypename,
        const wxString& p_sViewTypename,
        wxClassInfo* p_pcDocClassInfo = NULL,
        wxClassInfo* p_pcViewClassInfo = NULL,
        long p_nFlags = wxDEFAULT_TEMPLATE_FLAGS);

    inline SP_DS_Netclass* GetNetclass() { return m_pcNetclass; }
};

#endif // __SP_GM_DOCTEMPLATE_H__
