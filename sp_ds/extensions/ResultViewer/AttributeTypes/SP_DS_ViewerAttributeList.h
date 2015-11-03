/*
 * SP_DS_ViewerAttributeList.h
 * SP_DS_ViewerAttributeText.h
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 15.06.2011
 * Short Description:
 *///============================================

#ifndef SP_DS_VIEWERATTRIBUTELIST_H_
#define SP_DS_VIEWERATTRIBUTELIST_H_

#include <wx/combobox.h>
#include "sp_core/SP_Vector.h"
#include "sp_ds/extensions/ResultViewer/AttributeTypes/SP_DS_ViewerAttribute.h"

class SP_DS_ViewerAttributeList: public SP_DS_ViewerAttribute
{
	      wxChoice*  m_pcPossibleValuesBox;

	      SP_VectorString m_asPossibleValues;

	      //index of selected index
	      unsigned int m_nSelection;
public:
		  SP_DS_ViewerAttributeList(const wxString& p_sName,const wxString& p_sCategory=wxT(""),
								    const wxString& p_sDisplayName=wxT(""));;
		  virtual ~SP_DS_ViewerAttributeList();

		  virtual void Create(wxWindow* p_pcParent,wxSizer* p_pcSizer);

		  virtual void Update();

		  void SetPossibleValues(const SP_VectorString& p_asPossibleValues){m_asPossibleValues=p_asPossibleValues;}

		  int GetSelection(){return m_nSelection;}

		  void SetSelection(const unsigned int& p_nSelection){m_nSelection=p_nSelection;}

		  void ClearPossibleValues(){m_asPossibleValues.clear();}

		  void AddToPossibleValues(const wxString& p_sValue){m_asPossibleValues.push_back(p_sValue);}
};

#endif /* SP_DS_VIEWERATTRIBUTELIST_H_ */
