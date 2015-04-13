//////////////////////////////////////////////////////////////////////
// $Author: alextov $
// $Version: 0.1 $
// $Date: 2005/05/24 $
// Short Description: log window that shows only selected messages
//////////////////////////////////////////////////////////////////////
#ifndef __SP_GUI_LOGWINDOW_H__
#define __SP_GUI_LOGWINDOW_H__

#include <wx/log.h>
#include "sp_defines.h"

class SP_GUI_LogWindow : public wxLogWindow
{
public:
  SP_GUI_LogWindow(wxFrame *pParent,         // the parent frame (can be NULL)
		   const wxChar *szTitle,    // the title of the frame
		   bool bShow = TRUE);        // show window immediately?

  void AddLevelToShow(wxLogLevel);
  void DelLevelToShow(wxLogLevel);

  inline bool NewMessagesAdded() {return m_bNewMessages; }
  inline void ResetNewMessages() {m_bNewMessages = FALSE; }

  virtual ~SP_GUI_LogWindow();

  virtual void OnFrameDelete(wxFrame *frame);

protected:
  bool m_bNewMessages;
#if wxABI_VERSION > 30000
  virtual void DoLogTextAtLevel(wxLogLevel level, const wxString& msg);
#else
  virtual void DoLog(wxLogLevel level, const wxChar *szString, time_t t);
#endif
  list<wxLogLevel> m_lLevelToShow;

};

#endif // __SP_GUI_LOGWINDOW_H__
