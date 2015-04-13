//////////////////////////////////////////////////////////////////////
// $Source: SP_GPR_Base.h
// $Author: MD $
// $Version: $
// $Revision: $
// $Date: 2005/09/10 $
// Short Description: base class for all pref categories
//////////////////////////////////////////////////////////////////////

#if !defined(__SP_GPR_BASE_H__)
#define __SP_GPR_BASE_H__

#include <wx/wx.h>
#include <wx/config.h>
#include "sp_defines.h"

#include "sp_gui/widgets/SP_WDG_NotebookPage.h"

class SP_GPR_Base {

protected:
	//list of registered net classes
	SP_SetString m_lsNCNames;

public:
	SP_GPR_Base();
	virtual ~SP_GPR_Base();

	//virtual bool Init()=0;
	//by this the class adds its controls to the global prefs dialog
	virtual bool AddToDialogPage(const wxString& p_sNetClass, SP_WDG_NotebookPage *p_pcPage)=0;
	virtual bool OnDialogOk(const wxString& p_sNetClass)=0;
	//read values for member variables from config
	virtual bool UpdateFromConfig(wxConfig& p_config)=0;
	//write values to config
	virtual bool WriteToConfig(wxConfig& p_config)=0;

};

#endif // !defined(__SP_GPR_BASE_H__)
