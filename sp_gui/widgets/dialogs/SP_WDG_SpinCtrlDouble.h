/*
 * SP_WDG_SpinCtrlDouble.h
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date:11.01.2012
 * Short Description:
 */
//=================================================

#ifndef SP_WDG_SPINCTRLDOUBLE_H_
#define SP_WDG_SPINCTRLDOUBLE_H_

#include "wx/gdicmn.h"
#include "wx/wx.h"
#include <wx/control.h>
#include <wx/spinbutt.h>

#include "wx/bmpbuttn.h"
#include "wx/spinctrl.h"
#include "limits.h"
/*
 * This class implements a double version of wxSpinCtrl. So far wxWideget 2.8 supports only
 * integer version, however version 2.9 do.
 * TODO: replace this class with wxWidget wxSpinCtrlDouble when upgrading to version 2.9
 */
class SP_WDG_SpinCtrlDouble : public wxPanel
{		
private:
		double		m_dwValue;
		long long		m_dwMin;
		long long		m_dwMax;
		
		double m_nIncrement;
		
		wxSpinButton		*m_pSpinBtn;
		
		wxTextCtrl		*m_pTextCtrl;
		
		// validators
		wxTextValidator		m_DoubleValidator;
		
	protected:
		
	private:
		void	Init();
		void	CreateCotrols(const wxWindowID& p_ID,long nTextStyle, const wxSize & size);
		void	UpdateValue();
		void	ConnectEvents();
		
	protected:
		SP_WDG_SpinCtrlDouble();
		
		void	OnSpinUp(wxSpinEvent & event);
		void	OnSpinDown(wxSpinEvent & event);
		
		void	OnText(wxCommandEvent & event);
		
		void OnSpinValueChanged(wxSpinEvent& event);
		
		void OnEnterCommand(wxCommandEvent& event);

	public:
		SP_WDG_SpinCtrlDouble(wxWindow *pParent, 
			     wxWindowID nId = wxID_ANY, 
			     const double& p_nSpinIncrease=0.1,
			     const wxPoint & point = wxDefaultPosition, 
			     const wxSize & size = wxDefaultSize,
			     long nTextStyle = 0
			    );
		~SP_WDG_SpinCtrlDouble();
		
		void	SetEditable(bool bEditable = true);
		
		double	GetValue();
		wxString	GetStringValue();
		wxTextCtrl	*GetText();
		
		bool		SetValue(double nValue, bool bForce = false);
		void		SetRange(long long dwMin, long long dwMax);
		
		
		DECLARE_DYNAMIC_CLASS(TextWithSpin)
		DECLARE_EVENT_TABLE();
};
#endif /* SP_WDG_SPINCTRLDOUBLE_H_ */
