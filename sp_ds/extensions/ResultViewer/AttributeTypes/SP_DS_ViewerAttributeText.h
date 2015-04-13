/*
 * SP_DS_ViewerAttributeText.h
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 14.06.2011
 * Short Description:
 *///============================================

#ifndef SP_DS_VIEWERATTRIBUTETEXT_H_
#define SP_DS_VIEWERATTRIBUTETEXT_H_

#include <wx/textctrl.h>
#include "sp_ds/extensions/ResultViewer/AttributeTypes/SP_DS_ViewerAttribute.h"

class SP_DS_ViewerAttributeText: public SP_DS_ViewerAttribute
{
protected:
	       wxTextCtrl* m_pcValueTxtCtrl;
public:
		  SP_DS_ViewerAttributeText(const wxString& p_sName,const wxString& p_sCategory=wxT(""),
	                                const wxString& p_sDisplayName=wxT(""));
		  virtual ~SP_DS_ViewerAttributeText();

		  virtual void Create(wxWindow* p_pcParent,wxSizer* p_pcSizer);

		  virtual void Update();
};

#endif /* SP_DS_VIEWERATTRIBUTETEXT_H_ */
