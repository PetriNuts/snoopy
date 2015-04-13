//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: 
//////////////////////////////////////////////////////////////////////
#ifndef __SP_MDI_COARSEDOC_H__
#define __SP_MDI_COARSEDOC_H__

#include "sp_gui/mdi/SP_MDI_Doc.h"

class SP_MDI_CoarseDoc: public SP_MDI_Doc
{
private:
    DECLARE_DYNAMIC_CLASS(SP_MDI_CoarseDoc)
protected:
public:
    SP_MDI_CoarseDoc();
    ~SP_MDI_CoarseDoc();

    bool OnCloseDocument();
    void Modify(bool p_bVal);

    inline SP_DS_Graph* InitGraph(SP_DS_Netclass* p_pcNetclass) { return m_pcGraph; }
    SP_DS_Graph* SetGraph(SP_DS_Graph* p_pcVal);

    void SetNameSuffix(wxString& p_sVal);
    
    bool OnSaveDocument(const wxString& p_sFile);
    bool SaveAs();

    void UpdateTitle();
};

#endif // __SP_MDI_COARSEDOC_H__


