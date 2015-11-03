/*
 * SP_DLG_ChangeCurveAttributes.h
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 23.03.2011
 * Short Description:
 */
//=================================================

#ifndef SP_DLG_CHANGECURVEATTRIBUTES_H_
#define SP_DLG_CHANGECURVEATTRIBUTES_H_

#include<wx/wx.h>
#include <wx/clrpicker.h>

class SP_DLG_ChangeCurveAttributes: public wxDialog
{
	      wxChoice* m_pcLineWidthBox;
	      wxChoice* m_pcLineStyleBox;
	      wxColourPickerCtrl* m_pcColourPickerCtrl;

	      //use default setting
	      wxCheckBox* m_pcUseDefaultSetting;

	      int m_nLineWidth;

	      int m_nLineStyle;

	      //new line color
	      wxColour m_nLineColor;

	      //old line color
	      wxColour m_nOldLineColor;
protected:
	      void OnOk(wxCommandEvent& event);

	      void OnComboBoxChange(wxCommandEvent& event);

	      //on changing the curve color
	      void OnCurveColorChanged(wxColourPickerEvent& event);

	      DECLARE_EVENT_TABLE();
public:
	      //Line width
	      void SetLineWidth(const int& p_nLineWidth){m_nLineWidth=p_nLineWidth;}
	      int GetLineWidth(){return m_nLineWidth;}

	      //line style
	      void SetLineStyle(const int& p_nLineStyle){m_nLineStyle=p_nLineStyle;}
	      int GetLineStyle(){return m_nLineStyle;}

	      wxColour GetLineColor(){return m_nLineColor;}

		  SP_DLG_ChangeCurveAttributes(wxWindow* p_pcParent,const wxString& p_sColor,
				                       const int& p_nLineWidth,const int p_nLineStyle);

		  virtual ~SP_DLG_ChangeCurveAttributes();

};

#endif /* SP_DLG_CHANGECURVEATTRIBUTES_H_ */
