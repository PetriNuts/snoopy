//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: 
//////////////////////////////////////////////////////////////////////
#ifndef __SP_WDG_DIALOGBASE_H__
#define __SP_WDG_DIALOGBASE_H__

#include "sp_core/base/SP_Error.h"
#include "sp_core/base/SP_Name.h"
#include "sp_core/base/SP_Type.h"
#include "sp_core/SP_List.h"
#include "sp_ds/SP_DS_Attribute.h"
#include "sp_gui/dialogs/SP_DLG_ShapeProperties.h"

#include <wx/wx.h>

extern unsigned int g_nDialogID;

/**	\brief	The SP_WDG_DialogBase: class

  Used as member of each attribute to determine its look and behaviour
  in dialogs, while editing the properties
*/
class SP_WDG_DialogBase: public wxEvtHandler, public SP_Error, public SP_Name, public SP_IdCounter
{
private:
    DECLARE_EVENT_TABLE()
protected:
    SP_ListAttribute m_tlAttributes;
    std::vector<wxCheckBox*> m_pcShow;
    //! flag if we display multiple attributes
    bool m_bMultiple;

	unsigned int m_nDialogID;

	unsigned int m_nDialogOrdering;

    bool AddShowFlag(SP_WDG_NotebookPage* p_pcPage, wxBoxSizer* p_pcSizer, SP_DS_Attribute* p_pcAttr);

public:

    SP_WDG_DialogBase(const wxString& p_sPage = wxT("Page 1"), unsigned int p_nOrdering = std::numeric_limits<unsigned int>::max());
    virtual ~SP_WDG_DialogBase();

    virtual SP_WDG_DialogBase* Clone();

    virtual bool AddToDialog(const SP_ListAttribute* p_ptlAttributes, 
        SP_DLG_ShapeProperties* p_pcDlg, bool p_bOverview);
    virtual bool AddToDialog(SP_ListGraphic* p_plGraphics,
        SP_DLG_ShapeProperties* p_pcDlg) { return TRUE; }

    virtual bool OnDlgOk();

    bool CleanUpAttributes();

    /* ACCESSORS */
    virtual int GetEvalState() { return 0; }

    inline unsigned int GetDialogOrdering() { return m_nDialogOrdering; }

	SP_ListAttribute GetDialogAttributes() { return m_tlAttributes; };//by george, we need this getter for editing node Type attribute 
};

#endif // __SP_WDG_DIALOGBASE_H__
