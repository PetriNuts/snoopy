//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_NETCLASS_H__
#define __SP_DS_NETCLASS_H__

#include "sp_core/base/SP_Error.h"
#include "sp_core/base/SP_Type.h"
#include "sp_core/base/SP_Name.h"

#include "export/SP_ExportRoutine.h"

#include <wx/string.h>

#include "sp_ds/SP_DS_Node.h"
#include "sp_ds/SP_DS_Edgeclass.h"
#include "sp_core/base/SP_Data.h"

#include "sp_gui/management/SP_GM_Docmanager.h"

class SP_DS_Graph;
class SP_GM_DocTemplate;

class SP_DS_Netclass: public SP_Error, public SP_Type, public SP_Name
{
private:
protected:

    wxString m_sExtension;
    wxString m_sFilter;
    SP_GM_DocTemplate* m_pcDocTemplate;

public:
    SP_DS_Netclass(const wxString& p_pchName);
    virtual ~SP_DS_Netclass();

    virtual bool AddToGui(SP_GM_Docmanager* p_pcDocmanager);
    virtual bool CreateCoarse();

    virtual SP_DS_Graph* CreateGraph(SP_DS_Graph* p_pcGraph,SP_MapString2Int p_mapAttribute2Value = SP_MapString2Int());

    virtual bool CreateInitialElements(SP_DS_Graph* p_pcGraph) { return true; }

    virtual bool NodeRequirement(SP_DS_Node* p_pcNode);
    virtual bool EdgeRequirement(SP_DS_Edgeclass* p_pcEdgeclass, SP_Data* p_pcNode1, SP_Data* p_pcNode2);
    virtual bool MetadataRequirement(SP_DS_Metadata* p_pcMetadata);

    inline wxString GetExtension() const { return m_sExtension; }

    inline wxString GetFilter() const { return m_sFilter.IsEmpty() ? wxT("*.")+m_sExtension : m_sFilter; }

    inline SP_GM_DocTemplate* GetDocTemplate()  const { return m_pcDocTemplate; }


};

#endif // __SP_DS_NETCLASS_H__
