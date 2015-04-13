/*
 * SP_DS_ViewerAttributeCheckBox.h
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 14.06.2011
 * Short Description:
 * *///=========================================================


#ifndef SP_DS_VIEWERATTRIBUTECHECKBOX_H_
#define SP_DS_VIEWERATTRIBUTECHECKBOX_H_

#include <wx/checkbox.h>
#include "sp_ds/extensions/ResultViewer/AttributeTypes/SP_DS_ViewerAttribute.h"

class SP_DS_ViewerAttributeCheckBox: public SP_DS_ViewerAttribute
{
private:
	      wxCheckBox* m_pcValueCheckBox;
	      bool m_bValue;
public:
		  SP_DS_ViewerAttributeCheckBox(const wxString& p_sName,const wxString& p_sCategory=wxT(""),
                                        const wxString& p_sDisplayName=wxT(""));
		  virtual ~SP_DS_ViewerAttributeCheckBox();

		  virtual void Create(wxWindow* p_pcParent,wxSizer* p_pcSizer);

		  //update the attribute value from the GUI
		  virtual void Update();

		  //return the current state
		  bool GetValue(){return m_bValue;}

		  //set the current state
		  void SetValue(const bool& p_bValue){m_bValue=p_bValue;}
};

#endif /* SP_DS_VIEWERATTRIBUTECHECKBOX_H_ */
