//////////////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: MD $
// $Version: 1.0 $
// $Revision: $
// $Date: 2006/10/05 $
// Short Description: derived interaction command for coloring graph elements
//////////////////////////////////////////////////////////////////////////////

#if !defined(__SP_IA_COMMAND_COLORIZE_H__)
#define __SP_IA_COMMAND_COLORIZE_H__

#include <wx/clrpicker.h>
#include <wx/colourdata.h>
#include "sp_gui/interaction/commands/SP_IA_Command.h"
#include "sp_gui/commands/SP_CMD_ColorizeGraphic.h"

//config dialog id's
enum {
	SP_ID_BUTTON_COLOR = 1,
	SP_CHECKBOX_USESOURCE
};


class SP_IA_Command_Colorize : public SP_IA_Command {

private:
	//config dialog controls
	wxColourPickerCtrl *m_buChooseColor;
	wxCheckBox *m_cbUseSource;
	wxCheckBox *m_cbUndo;

	DECLARE_EVENT_TABLE()

protected:
	wxColourData m_cColorData;
	//this is true if the color of the elements in the source graph should be used
	//and false if the given color (stored in m_cColorData) should be used
	bool m_bUseSourceColor;
	inline void UseSourceColor(bool p_bVal) { m_bUseSourceColor = p_bVal; }

	//every element to colorize gets its own color-command
	//this list holds all these color-commands used in one interaction-command execution
	//they are saved for later undo
	list<SP_CMD_ColorizeGraphic*>* m_pcCommandHistory;

public:
	SP_IA_Command_Colorize();
	SP_IA_Command_Colorize(const wxString& p_sName, wxColour p_cColor = wxNullColour, bool p_bHasConfig = true, bool p_bCanUndo = true);
	virtual ~SP_IA_Command_Colorize();

	//these implement the SP_IA_Command interface
	SP_IA_Command* Copy(bool p_bTryInit = false);
	bool Do(SP_ListData* p_pcSourceElements);
	bool Undo();
	void AddToConfigDialog(wxWindow *p_pcParent, wxSizer *p_pcSizer);
	void OnConfigDialogOk();

	//event handling function of config dialog
	void OnChooseColor(wxColourPickerEvent& p_cEvent);
	void RefreshColor();

	//usual setters and getters
	inline wxColour GetColor() { return m_cColorData.GetColour(); }
	inline void SetColor(wxColour p_cCol) { m_cColorData.SetColour(p_cCol); }
	
};

#endif // !defined(__SP_IA_COMMAND_COLORIZE_H__)
