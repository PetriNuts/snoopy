/*
 * SP_DS_ViewerAttribute.h
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 14.06.2011
 * Short Description:
 *///=================================================
#ifndef SP_DS_VIEWERATTRIBUTE_H_
#define SP_DS_VIEWERATTRIBUTE_H_

#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/window.h>
#include <wx/stattext.h>
#include<wx/statbox.h>

/*
 * Base class for the result viewer properties
 */
class SP_DS_ViewerAttribute
{
protected:
		  /*
		   * Attribute name
		   */
           wxString m_sName;
           /*
            * Attribute category
            */
           wxString m_sCategory;

           /*
            * Display name
            */
           wxString m_sDisplayName;

           /*
            * Value in case of simple attribute
            */
           wxString m_sValue;
public:
		  SP_DS_ViewerAttribute(const wxString& p_sName,const wxString& p_sCategory=wxT(""),
				                const wxString& p_sDisplayName=wxT(""));
		  virtual ~SP_DS_ViewerAttribute();

		  //Attribute category
		  virtual void SetCategory(const wxString& p_sCategory){m_sCategory=p_sCategory;}
		  virtual wxString GetCategory(){return m_sCategory;}

		  //Attribute Displayed name
		  virtual void SetDisplayName(const wxString& p_sDisplayName){m_sDisplayName=p_sDisplayName;}

		  virtual wxString GetDisplayName(){return m_sDisplayName;}

		  //Return the attribute value as string
		  virtual wxString GetValueString(){return m_sValue;}

		  virtual void SetValueString(const wxString& p_sValue){m_sValue=p_sValue;}

		  virtual wxString GetName(){return m_sName;}

		  //Create the control to the window
		  virtual void Create(wxWindow* p_pcParent,wxSizer* p_pcSizer)=0;

		  /*
		   * Update the attribute data from the GUI
		   */
		  virtual void Update()=0;
};

#endif /* SP_DS_VIEWERATTRIBUTE_H_ */
