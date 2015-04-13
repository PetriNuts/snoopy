#include "sp_gui/management/SP_GM_DocTemplate.h"
#include "sp_gui/management/SP_GM_Docmanager.h"

#include "sp_ds/SP_DS_Netclass.h"

SP_GM_DocTemplate::SP_GM_DocTemplate(SP_GM_Docmanager* p_pcDocmanager,
                                     SP_DS_Netclass* p_pcNetclass,
                                     const wxString& p_sDescr,
                                     const wxString& p_sFilter,
                                     const wxString& p_sDir,
                                     const wxString& p_sExt,
                                     const wxString& p_sDocTypename,
                                     const wxString& p_sViewTypename,
                                     wxClassInfo* p_pcDocClassInfo,
                                     wxClassInfo* p_pcViewClassInfo,
                                     long p_nFlags)
:wxDocTemplate(p_pcDocmanager, p_sDescr, p_sFilter, p_sDir, p_sExt,
               p_sDocTypename, p_sViewTypename, p_pcDocClassInfo, p_pcViewClassInfo, p_nFlags)
{
    m_pcNetclass = p_pcNetclass;
}

