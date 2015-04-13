//////////////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: MD $
// $Version: 1.0 $
// $Revision: $
// $Date: 2006/10/05 $
// Short Description: base class for interaction commands
//////////////////////////////////////////////////////////////////////////////


#if !defined(__SP_IA_COMMAND_H__)
#define __SP_IA_COMMAND_H__

#include <wx/wx.h>
#include <wx/cmdproc.h>
#include "sp_gui/interaction/data_processors/SP_IA_DataProcessor.h"
#include "sp_gui/interaction/SP_IA_Event.h"

//command types
//note: order is synchronized with m_lsTypeStrings[]
enum SP_IA_CMD_T {
	SP_IA_CMD_GENERAL = 0,
	SP_IA_CMD_COLORIZE,
	SP_IA_CMD_INSERT,
	SP_IA_CMD_DELETE,
	SP_IA_CMD_SETATTRIBUTE,
	SP_IA_CMD_HIDE,
	SP_IA_CMD_UNHIDE
};

class SP_DS_Graph;


class SP_IA_Command: public wxCommand, public wxEvtHandler {

private:

protected:
	//command type
	SP_IA_CMD_T m_eType;
	//decribes what this command actually does
	wxString m_sDescription;
	//the command knows about its source and target netclasses
	wxString m_sSourceNetclass;
	wxString m_sTargetNetclass;
	//the target graph which this command works on
	SP_DS_Graph *m_pcTarget;
	//the event which triggers execution of this command
	SP_IA_Event *m_pcTriggerEvent;
	//a queue of data processors which prepare the data this command finally works with
	list<SP_IA_DataProcessor*> *m_pcDataProcessors;
	//indicates whether this command is configurable or not
	//if so, AddToConfigDialog() and OnConfigDialogOk() should be implemented
	bool m_bHasConfig;
	//indicates whether this command is a copy
	bool m_bIsCopy;
	inline void SetCopyStamp(bool isCopy) { m_bIsCopy = isCopy; }

	SP_IA_Command();
	SP_IA_Command(SP_IA_CMD_T p_eType, const wxString& p_sName, bool p_bHasConfig = false, bool p_bCanUndo = false);

	//this function just completes the wxCommand interface
	inline bool Do() { return true; }

public:
	virtual ~SP_IA_Command();

	//this function makes a copy of an activated command
	//which is used for actual (multiple) execution of a command
	//must be implemented in derived classes
	virtual SP_IA_Command* Copy(bool p_bTryInit = false) = 0;
	//this function is used to make a copy of a registered command prototype
	//which is used when activating commands
	SP_IA_Command* InitializedCopy(SP_DS_Graph* p_pcSource, SP_DS_Graph* p_pcTarget);

	//prepares incoming data (from source net) for actual command execution
	void DoDataProcessing(SP_ListData* p_pcData);
	//actual command do-function
	//must be implemented in derived classes
	virtual bool Do(SP_ListData* p_pcSourceElements) = 0;
	//command undo-function
	//can be implemented in derived classes (if needed)
	virtual bool Undo() { return true; }

	//the next two provide the possibility to configure a command individually
	virtual void AddToConfigDialog(wxWindow *p_pcParent, wxSizer *p_pcSizer) = 0;
	virtual void OnConfigDialogOk() = 0;
	inline bool HasConfig() const { return m_bHasConfig; }
	//copies don't have their own data processors.
	//so this is important to know when destructing the object
	inline bool IsCopy() const { return m_bIsCopy; }

	//compares two commands (name, target graph and triggering event (includes source graph))
	virtual bool Equals(SP_IA_Command* p_pcCmd) const;

	//usual setters
	inline void SetCommandType(SP_IA_CMD_T p_eType) { m_eType = p_eType; }
	void SetTargetGraph(SP_DS_Graph *p_pcGraph);
	inline void SetDescription(const wxString& p_sVal) { m_sDescription = p_sVal; }
	void SetNetclassNames(const wxString& p_sSource = wxT(""), const wxString& p_sTarget = wxT(""));
	inline void SetTriggerEvent(SP_IA_Event* p_pcEvent) { m_pcTriggerEvent = p_pcEvent; }
	inline void SetDataProcessors(list<SP_IA_DataProcessor*> *p_pcList) { m_pcDataProcessors = p_pcList; }
	void AddDataProcessor(SP_IA_DataProcessor *p_pcDP, bool p_bAdoptSelect = true);

	//usual getters
	inline SP_IA_CMD_T GetCommandType() const { return m_eType; }
	wxString GetCommandTypeString() const;
	inline SP_DS_Graph *GetTargetGraph() const { return m_pcTarget; }
	SP_DS_Graph *GetSourceGraph() const;
	inline wxString GetDescription() const { return m_sDescription; }
	inline wxString GetSourceNetclassName() const { return m_sSourceNetclass; }
	inline wxString GetTargetNetclassName() const { return m_sTargetNetclass; }
	inline SP_IA_Event* GetTriggerEvent() const { return m_pcTriggerEvent; }
	inline list<SP_IA_DataProcessor*> *GetDataProcessors() const { return m_pcDataProcessors; }

};

#endif // !defined(__SP_IA_COMMAND_H__)
