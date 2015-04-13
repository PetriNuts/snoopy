//////////////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: MD $
// $Version: 1.0 $
// $Revision: $
// $Date: 2006/10/05 $
// Short Description: derived interaction command for deleting graph elements
//////////////////////////////////////////////////////////////////////////////

#if !defined(__SP_IA_COMMAND_DELETE_H__)
#define __SP_IA_COMMAND_DELETE_H__


#include "sp_gui/interaction/commands/SP_IA_Command.h"
#include "sp_gui/commands/SP_CMD_Edit.h"


class SP_IA_Command_Delete : public SP_IA_Command {

private:

protected:
	//last executed deletion (for later undo)
	SP_CMD_Edit *m_pcCommandHistory;

public:
	SP_IA_Command_Delete();
	SP_IA_Command_Delete(const wxString& p_sName, bool p_bHasConfig = false, bool p_bCanUndo = true);
	virtual ~SP_IA_Command_Delete();

	//these implement the SP_IA_Command interface
	SP_IA_Command* Copy(bool p_bTryInit = false);
	bool Do(SP_ListData* p_pcSourceElements);
	bool Undo();
	void AddToConfigDialog(wxWindow *p_pcParent, wxSizer *p_pcSizer) { }
	void OnConfigDialogOk() { }

};

#endif // !defined(__SP_IA_COMMAND_DELETE_H__)
