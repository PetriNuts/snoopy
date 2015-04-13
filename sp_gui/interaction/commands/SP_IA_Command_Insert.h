//////////////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: MD $
// $Version: 1.0 $
// $Revision: $
// $Date: 2006/10/05 $
// Short Description: derived interaction command for inserting graph elements
//										during one execution this command inserts nodes and
//										connects every one to the same predefined source
//////////////////////////////////////////////////////////////////////////////

#if !defined(__SP_IA_COMMAND_INSERT_H__)
#define __SP_IA_COMMAND_INSERT_H__

//default values if nothing else was provided by the constructor
#define DEFAULT_OFFSET 50
#define DEFAULT_POS 50
#define OFFSET_MAX 500

#include "sp_gui/interaction/commands/SP_IA_Command.h"
#include "sp_gui/commands/SP_CMD_ColorizeGraphic.h"

class SP_MDI_Doc;
class wxSpinCtrl;

//config dialog id's
enum {
	SP_ID_SPINCTRL_POS_X = 1,
	SP_ID_SPINCTRL_POS_Y,
	SP_ID_SPINCTRL_OFF_X,
	SP_ID_SPINCTRL_OFF_Y,
	SP_ID_BUTTON_SETNODE
};


class SP_IA_Command_Insert : public SP_IA_Command {

private:
	//needed for updating the document's views after creating the elements
	SP_MDI_Doc *m_pcTargetDoc;

	//the following two functions are used to make sure that we don't insert anything twice
	//finding already existing nodes of a nodeclass and with certain attributes
	SP_DS_Node *FindNode(SP_DS_Nodeclass *p_pcNC, const SP_ListAttribute *p_pcAttributes);
	//finding already existing edges of an edgeclass and with a particular source and target
	bool FindEdge(SP_DS_Edgeclass *p_pcEC, SP_Data *p_pcSource, SP_Data *p_pcTarget);

	//config dialog controls
	wxSpinCtrl *m_scPosX, *m_scPosY, *m_scOffsetX, *m_scOffsetY;

	DECLARE_EVENT_TABLE()

	wxString m_sSearchAttributeName;

protected:
	//current position for new nodes
	int m_nPosX, m_nPosY;
	//offsets for placing the new nodes
	int m_nOffsetX, m_nOffsetY;

	//node that indicates the common source of edges for connecting new elements
	SP_Graphic *m_pcEdgeSourceGraphic;
	wxColour m_cColor;
	SP_CMD_ColorizeGraphic *m_pcColorCmd;

public:
	SP_IA_Command_Insert();
	SP_IA_Command_Insert(wxString p_sName, int p_nPosX = DEFAULT_POS, int p_nPosY = DEFAULT_POS,
												int p_nOffsetX = DEFAULT_OFFSET, int p_nOffsetY = DEFAULT_OFFSET, 
												bool p_bHasConfig = true, bool p_bCanUndo = true);
	virtual ~SP_IA_Command_Insert();

	//these implement the SP_IA_Command interface
	SP_IA_Command* Copy(bool p_bTryInit = false);
	bool Do(SP_ListData* p_pcSourceElements);
	bool Undo();
	void AddToConfigDialog(wxWindow *p_pcParent, wxSizer *p_pcSizer);
	void OnConfigDialogOk();

	SP_Graphic *GetEdgeSource();
	inline int GetPosX() { return m_nPosX; }
	inline int GetPosY() { return m_nPosY; }
	//usual setters
	void SetEdgeSource(SP_Graphic *p_pcESG, bool p_bDoColoring = true);
	void SetPosition(int p_nX, int p_nY);
	void SetOffset(int p_nX, int p_nY);
	inline void SetColor(wxColour p_cColor = wxNullColour) { m_cColor = p_cColor; }

	void OnSetCurrentNode(wxCommandEvent& p_cEvent);
	inline void SetSearchAttributeName(const wxString& p_sName) { m_sSearchAttributeName = p_sName; }
};

#endif // !defined(__SP_IA_COMMAND_INSERT_H__)
