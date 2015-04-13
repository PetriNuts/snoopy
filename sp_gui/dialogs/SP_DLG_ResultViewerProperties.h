/*
 * SP_DLG_ResultViewerProperties.h
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 14.06.2011
 * Short Description:
 *///=================================================

#ifndef SP_DLG_RESULTVIEWERPROPERTIES_H_
#define SP_DLG_RESULTVIEWERPROPERTIES_H_

#include <wx/notebook.h>
#include <wx/dialog.h>

class SP_DLG_ResultViewerProperties: public wxDialog
{
private:
	      SP_DS_ResultViewer* m_pcResultViewer;
	      wxNotebook* m_pcNoteBook;
protected:
	      void OnUpdateViewerAttributes(wxCommandEvent& event);
public:
		  SP_DLG_ResultViewerProperties(SP_DS_ResultViewer* p_pcResultViewer,wxWindow* p_pcParent);
		  virtual ~SP_DLG_ResultViewerProperties();

		  DECLARE_EVENT_TABLE()
};

#endif /* SP_DLG_RESULTVIEWERPROPERTIES_H_ */
