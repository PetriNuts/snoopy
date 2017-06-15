
#include"sp_gui/widgets/dialogs/SP_WDG_SpinCtrlDouble.h"
#include "sp_ds/SP_DS_Graph.h"

IMPLEMENT_DYNAMIC_CLASS(SP_WDG_SpinCtrlDouble, wxPanel)

BEGIN_EVENT_TABLE(SP_WDG_SpinCtrlDouble,wxPanel)
//EVT_SCROLL(SP_WDG_SpinCtrlDouble::OnSpinValueChanged)
EVT_TEXT_ENTER(wxID_ANY, SP_WDG_SpinCtrlDouble::OnEnterCommand)
END_EVENT_TABLE()

SP_WDG_SpinCtrlDouble::SP_WDG_SpinCtrlDouble(wxWindow *pParent, 
			   wxWindowID nId,
			   const double& p_nSpinIncrease,
			   const wxPoint & point, 
			   const wxSize & size,
			   long nTextStyle)
	:
		wxPanel(pParent, nId, point, size, wxNO_BORDER),
		m_DoubleValidator()
{
	m_nIncrement=p_nSpinIncrease;

	Init();
	CreateCotrols(nId,nTextStyle, size);
	ConnectEvents();
}

SP_WDG_SpinCtrlDouble::SP_WDG_SpinCtrlDouble()
	:
		wxPanel(),
		m_DoubleValidator()
{
	Init();
}

SP_WDG_SpinCtrlDouble::~SP_WDG_SpinCtrlDouble()
{
	
}
void SP_WDG_SpinCtrlDouble::Init()
{
	m_dwValue 		= 0L;
	m_pTextCtrl		= 0L;
	m_pSpinBtn		= 0L;
	
	// set up validators
	m_DoubleValidator.SetStyle(wxFILTER_INCLUDE_CHAR_LIST);
	
	wxArrayString stringList;
	stringList.Add(_T("0"));
	stringList.Add(_T("1"));
	
	for(int i = 2; i <= 7; i++) 
	{
		stringList.Add(wxString::Format(wxT("%d"), i));
	}
	
	stringList.Add(_T("8"));
	stringList.Add(_T("9"));
	stringList.Add(_T("-"));
	m_DoubleValidator.SetIncludes(stringList);
	stringList.RemoveAt(stringList.Count() - 1);
	
	 //m_pTextCtrl->SetValidator(m_DoubleValidator);
	
	// min, max range
	m_dwMax = 999999999999999999LL;
	m_dwMin = 0;
	
}

void SP_WDG_SpinCtrlDouble::CreateCotrols(const wxWindowID& p_ID, long nTextStyle, const wxSize & size)
{
	SetOwnBackgroundColour(wxColour(255, 255, 255));
	
	wxBoxSizer * pSizer = new wxBoxSizer(wxHORIZONTAL);
	
	m_pTextCtrl = new wxTextCtrl(this, p_ID, _T("0"), wxDefaultPosition, wxDefaultSize, nTextStyle|wxTE_PROCESS_ENTER);
	pSizer->Add(m_pTextCtrl, 1, wxGROW|wxALL, 0);
	m_pTextCtrl->SetToolTip(wxT("You can use the keyboard to enter a value and then press the Enter key"));
 	Fit();
	
	m_pSpinBtn = new wxSpinButton(this, p_ID, wxDefaultPosition, wxSize(-1, size.y) );
	m_pSpinBtn->SetRange(INT_MIN,INT_MAX);
	pSizer->Add(m_pSpinBtn, 0, wxGROW|wxLEFT, 3);
	
	SetSizer(pSizer);
	
	SetInitialSize(size);
	
	m_pTextCtrl->SetFocus();
}

void SP_WDG_SpinCtrlDouble::SetEditable(bool bEditable)
{
	if(m_pTextCtrl)
		m_pTextCtrl->SetEditable(bEditable);
	
	if(m_pSpinBtn)
		m_pSpinBtn->Enable(bEditable);
}

double SP_WDG_SpinCtrlDouble::GetValue()
{
	return m_dwValue;
}
wxString SP_WDG_SpinCtrlDouble::GetStringValue()
{
	return dssd::aux::toString(m_dwValue);
}
wxTextCtrl *SP_WDG_SpinCtrlDouble::GetText()
{
	return m_pTextCtrl;
}

void SP_WDG_SpinCtrlDouble::OnSpinValueChanged(wxSpinEvent & event)
{
          event.Skip();
}

void SP_WDG_SpinCtrlDouble::OnEnterCommand(wxCommandEvent& event)
{
	   event.Skip();
}

void SP_WDG_SpinCtrlDouble::ConnectEvents()
{
	if(m_pSpinBtn)
	{
		/*m_pSpinBtn->Connect(wxID_ANY,
				    wxEVT_SCROLL_THUMBRELEASE,
				    wxScrollEventHandler(SP_WDG_SpinCtrlDouble::OnSpinValueChanged),
				    0L,
				    this
				   );*/
				   
		m_pSpinBtn->Connect(m_pSpinBtn->GetId(),
				    wxEVT_SCROLL_LINEUP,
				    wxSpinEventHandler(SP_WDG_SpinCtrlDouble::OnSpinUp),
				    0L,
				    this
				   );
	
		m_pSpinBtn->Connect(m_pSpinBtn->GetId(),
				    wxEVT_SCROLL_LINEDOWN,
				    wxSpinEventHandler(SP_WDG_SpinCtrlDouble::OnSpinDown),
				    0L,
				    this
				   );
	}
	
	if(m_pTextCtrl)
	{
		m_pTextCtrl->Connect(m_pTextCtrl->GetId(),
				     wxEVT_COMMAND_TEXT_UPDATED,
				     wxCommandEventHandler(SP_WDG_SpinCtrlDouble::OnText),
				     0L,
				     this
				    );
		
	}
	
}

void SP_WDG_SpinCtrlDouble::OnSpinUp(wxSpinEvent & event)
{
		m_dwValue+=m_nIncrement;
	     
		SetValue(m_dwValue);
		
		event.Skip();
}

void SP_WDG_SpinCtrlDouble::OnSpinDown(wxSpinEvent & event)
{
          m_dwValue-=m_nIncrement;
	     SetValue(m_dwValue);
	     
	     event.Skip();
}
bool SP_WDG_SpinCtrlDouble::SetValue(double nValue, bool bForce)
{
	bool bRet = (nValue >= m_dwMin && nValue <= m_dwMax) || bForce;
	
	if(bRet)
	{
		m_dwValue = nValue;
		UpdateValue();
	}
	
	return bRet;
}

void SP_WDG_SpinCtrlDouble::SetRange(long long dwMin, long long dwMax)
{
	m_dwMin = dwMin;
	m_dwMax = dwMax;
	
	if(m_dwValue < m_dwMin)
	{
		m_dwValue = m_dwMin;
		UpdateValue();
	}
	else if(m_dwValue > dwMax)
	{
		m_dwValue = dwMax;
		UpdateValue();
	}
}

void SP_WDG_SpinCtrlDouble::OnText(wxCommandEvent & event)
{
	double nValue = 0;
	
	wxString sValue = m_pTextCtrl ? m_pTextCtrl->GetValue() : _T("");
	
	if(!sValue.IsEmpty())
	{
		sValue.ToDouble(&nValue);
	
/*		DEBUG_ERROR("TextWithSpin::OnText()\n\tnValue\t[%lld] \n\tmin\t[%lld] \n\tmax\t[%lld] \n\tnValue Ok ? [%s]",
			    nValue, m_dwMin, m_dwMax,
       			    (nValue >= m_dwMin && nValue <= m_dwMax) ? "YES" : "NO"
			   );*/
		if(nValue >= m_dwMin && nValue <= m_dwMax)
			m_dwValue = nValue;	
	}
	
	event.Skip();
}

void SP_WDG_SpinCtrlDouble::UpdateValue()
{
		if(m_pTextCtrl)
		{
			m_pTextCtrl->SetValue( wxString::Format(wxT("%g"),m_dwValue) );
		}
}
