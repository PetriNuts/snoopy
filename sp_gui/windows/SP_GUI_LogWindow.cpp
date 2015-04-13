//////////////////////////////////////////////////////////////////////
// $Author: alextov $
// $Version: 0.1 $
// $Date: 2005/05/24 $
// Short Description: log window that shows only selected messages
//////////////////////////////////////////////////////////////////////

#include "sp_gui/windows/SP_GUI_LogWindow.h"
#include "sp_core/SP_GPR_Canvas.h"
#include "sp_core/SP_Core.h"
#include "snoopy.h"
#include <wx/frame.h>


SP_GUI_LogWindow::~SP_GUI_LogWindow()
{}


SP_GUI_LogWindow::SP_GUI_LogWindow(wxFrame *pParent,
				   const wxChar *szTitle,
				   bool bShow)
  : wxLogWindow(pParent, szTitle, bShow, false)
{
	m_bNewMessages = FALSE;
	wxGetApp().GetCanvasPrefs()->LoadWindowOptions(GetFrame());
}

void
SP_GUI_LogWindow::AddLevelToShow(wxLogLevel p_l)
{
  if (SP_Find(m_lLevelToShow,p_l) == m_lLevelToShow.end()) {
    m_lLevelToShow.push_back(p_l);
  }
}

void
SP_GUI_LogWindow::DelLevelToShow(wxLogLevel p_l)
{
  m_lLevelToShow.remove(p_l);
}

#if wxABI_VERSION > 30000
void
SP_GUI_LogWindow::DoLogTextAtLevel(wxLogLevel level, const wxString& msg)
{
	wxLogWindow::DoLogTextAtLevel(level, msg);
	m_bNewMessages = TRUE;
}
#else
void
SP_GUI_LogWindow::DoLog(wxLogLevel level,
			const wxChar *szString, time_t t)
{


  if ( GetFrame() ) {

     // first let the previous logger show it
     wxLogPassThrough::DoLog(level, szString, t);


    if (SP_Find(m_lLevelToShow,level) != m_lLevelToShow.end()) {
      // and this will format it nicely and call DoLogString()
      wxLog::DoLog(level, szString, t);
      //      m_bHasMessages = TRUE;
      m_bNewMessages = TRUE;
    }
    wxLogLevel l_LogStatus = wxLOG_Status;
    if (SP_Find(m_lLevelToShow,l_LogStatus) != m_lLevelToShow.end() ) {
      // by default, these messages are ignored by wxLog, so process
      // them ourselves
      if (!wxIsEmpty(szString)) {
    	  wxString str;
    	  str << _T("Status: ") << szString;
    	  DoLogString(str, t);
      }
      //      m_bHasMessages = TRUE;
      m_bNewMessages = TRUE;
    }
  }
}
#endif

void
SP_GUI_LogWindow::OnFrameDelete(wxFrame *frame)
{
	wxGetApp().GetCanvasPrefs()->SaveWindowOptions(frame);
	wxLogWindow::OnFrameDelete(frame);
}
