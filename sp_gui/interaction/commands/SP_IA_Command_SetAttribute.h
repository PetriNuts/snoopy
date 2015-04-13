//////////////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: MD $
// $Version: 1.0 $
// $Revision: $
// $Date: 2006/10/05 $
// Short Description: derived interaction command for setting element attributes
//////////////////////////////////////////////////////////////////////////////

#if !defined(__SP_IA_COMMAND_SETATTRIBUTE_H__)
#define __SP_IA_COMMAND_SETATTRIBUTE_H__


#include "sp_gui/interaction/commands/SP_IA_Command.h"

class SP_MDI_Doc;


class SP_IA_Command_SetAttribute : public SP_IA_Command {

private:
	wxCheckBox *m_cbUndo;
	//saved attribute values for later undo
	map<SP_DS_Attribute*, wxString> m_mSavedValues;
	//needed for updating the document's views after setting the attribute values
	SP_MDI_Doc *m_pcTargetDoc;

protected:
	//attribute to set, provided by the constructor
	SP_DS_Attribute *m_pcAttribute;

public:
	SP_IA_Command_SetAttribute();
	SP_IA_Command_SetAttribute(const wxString& p_sName, SP_DS_Attribute *p_pcAttribute = 0, bool p_bHasConfig = true, bool p_bCanUndo = true);
	virtual ~SP_IA_Command_SetAttribute();

	//these implement the SP_IA_Command interface
	SP_IA_Command* Copy(bool p_bTryInit = false);
	bool Do(SP_ListData* p_pcSourceElements);
	bool Undo();
	void AddToConfigDialog(wxWindow *p_pcParent, wxSizer *p_pcSizer);
	void OnConfigDialogOk();
};

#endif // !defined(__SP_IA_COMMAND_SETATTRIBUTE_H__)
