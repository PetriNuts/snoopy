/*
 * SP_DS_TableViewer.h
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 06.06.2011
 * Short Description:
 *///=================================================
#ifndef SP_DS_TABLEVIEWER_H_
#define SP_DS_TABLEVIEWER_H_

#include <wx/grid.h>

class SP_DS_TableViewer: public SP_DS_ResultViewer
{
protected:

	      wxStaticText* m_pcStaticTextBox;

		  wxGrid* m_pcTableViewGrid;
protected:

		 // void OnLabelDClick(wxGridEvent& p_cEvent);
public:
		 SP_DS_TableViewer(wxWindow* p_pcParent,wxSizer* p_pcSizer);
		 virtual ~SP_DS_TableViewer();

		 virtual void Update();
		 void  UpdateMembershipViewer(double TimePoint);
		 virtual void Create();

		 virtual void Destroy();

		 virtual void UpdateAttributes();

		 virtual void SaveOutput(const wxString& p_sFileName,const int& p_sFileType);
};

#endif /* SP_DS_TABLEVIEWER_H_ */
