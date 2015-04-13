//////////////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: MD $
// $Version: 1.0 $
// $Revision: $
// $Date: 2006/10/05 $
// Short Description: derived interaction command for deleting graph elements
//////////////////////////////////////////////////////////////////////////////

#if !defined(__SP_IA_COMMAND_UNHIDE_H__)
#define __SP_IA_COMMAND_UNHIDE_H__


#include "sp_gui/interaction/commands/SP_IA_Command.h"
#include "sp_gui/commands/SP_CMD_Edit.h"


class SP_IA_Command_UnHide : public SP_IA_Command {

private:

protected:
	//last executed unhide (for later undo)
	SP_CMD_Edit *m_pcCommandHistory;

public:
	SP_IA_Command_UnHide();
	SP_IA_Command_UnHide(const wxString& p_sName, bool p_bHasConfig = false, bool p_bCanUndo = true);
	virtual ~SP_IA_Command_UnHide();

	//these implement the SP_IA_Command interface
	SP_IA_Command* Copy(bool p_bTryInit = false);
	bool Do(SP_ListData* p_pcSourceElements);
	bool Undo();
	void AddToConfigDialog(wxWindow *p_pcParent, wxSizer *p_pcSizer) { }
	void OnConfigDialogOk() { }

};

#endif // !defined(__SP_IA_COMMAND_UNHIDE_H__)
