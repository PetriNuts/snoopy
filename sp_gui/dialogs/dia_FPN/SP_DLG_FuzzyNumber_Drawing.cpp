
#include "wx/scrolbar.h"
#include "sp_gui/dialogs/dia_FPN/SP_DLG_FuzzyNumber_Drawing.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"

enum {
	SP_ID_PANEL,
	SP_ID_SCROLL_A,
	SP_ID_SCROLL_B,
	SP_ID_SCROLL_C,
	SP_ID_LBL_A_LABEL,
	SP_ID_LBL_B_LABEL,
	SP_ID_LBL_C_LABEL,
	SP_ID_TXT_A,
	SP_ID_TXT_B,
	SP_ID_TXT_C,
	BUTTON_PLOT_SAVE
};
BEGIN_EVENT_TABLE(SP_DLG_FuzzyNumber_Drawing, wxDialog)

//EVT_CLOSE(SP_DLG_FuzzyNumber_Drawing::OnCloseDrsawingFrame)


END_EVENT_TABLE()

SP_DLG_FuzzyNumber_Drawing::SP_DLG_FuzzyNumber_Drawing(wxWindow *p_pcParent, wxString& p_sTitle,wxString& p_sVal, int xpos, int ypos, int width, int height)
	: wxDialog(p_pcParent, -1, p_sTitle, wxPoint(xpos, ypos), wxSize(width, height),  wxDEFAULT_FRAME_STYLE | wxSTAY_ON_TOP  |wxRESIZE_BORDER | wxMAXIMIZE_BOX)
{
	m_pcSizer = new wxBoxSizer(wxVERTICAL);

	m_pcDrawingPanel = new wxChartPanel(this, SP_ID_PANEL, NULL, wxDefaultPosition, wxSize(300, 300));//600*600
	wxColour l_nColor1(wxT("WHITE"));
	m_pcDrawingPanel->SetBackgroundColour(l_nColor1);

	init();
	 
	 
	m_pctxtCtrlA = new wxSpinCtrlDouble(this, SP_ID_TXT_A, wxEmptyString, wxPoint(20,-0), wxSize(80, 20), wxTE_PROCESS_ENTER | wxTE_CENTER,0.0,1000);
	m_pctxtCtrlA->SetIncrement(0.01);
	m_pctxtCtrlA->Enable(true);
	Connect(SP_ID_TXT_A, wxEVT_SPINCTRLDOUBLE, wxSpinDoubleEventHandler(SP_DLG_FuzzyNumber_Drawing::OnSpinChanged));
	
	m_pctxtCtrlB = new wxSpinCtrlDouble(this, SP_ID_TXT_B, wxEmptyString, wxDefaultPosition, wxSize(80, 20), wxTE_PROCESS_ENTER | wxTE_CENTER, 0.0, 1000);
	m_pctxtCtrlB->SetIncrement(0.01);
	m_pctxtCtrlB->Enable(true);


	 Connect(SP_ID_TXT_B, wxEVT_SPINCTRLDOUBLE, wxSpinDoubleEventHandler(SP_DLG_FuzzyNumber_Drawing::OnSpinChanged));
	
	m_pctxtCtrlC = new wxSpinCtrlDouble(this, SP_ID_TXT_C, wxEmptyString, wxDefaultPosition, wxSize(80, 20), wxTE_PROCESS_ENTER | wxTE_CENTER, 0.0, 1000);
	m_pctxtCtrlC->SetIncrement(0.01);
	m_pctxtCtrlC->Enable(true);
	Connect(SP_ID_TXT_C, wxEVT_SPINCTRLDOUBLE, wxSpinDoubleEventHandler(SP_DLG_FuzzyNumber_Drawing::OnSpinChanged));
	wxBoxSizer*  l_pcCartSizer = new  wxStaticBoxSizer(new wxStaticBox(this, 5000, wxT("Draw a Plot ")), wxHORIZONTAL);
	wxBoxSizer* l_pcMainSizer = new  wxBoxSizer(wxVERTICAL);
	wxBoxSizer* l_pcVluesSizer = new  wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* l_pcAsizer = new wxStaticBoxSizer(new wxStaticBox(this, 100, wxT("adjust value of A")), wxHORIZONTAL);
	wxBoxSizer* l_pcBsizer = new wxStaticBoxSizer(new wxStaticBox(this, 200, wxT("adjust value of B")), wxHORIZONTAL);
	wxBoxSizer* l_pcCsizer = new wxStaticBoxSizer(new wxStaticBox(this, 300, wxT("adjust value of C")), wxHORIZONTAL);

	l_pcCartSizer->Add(m_pcDrawingPanel, 1, wxEXPAND | wxALL, 5);
 
	l_pcAsizer->Add(m_pctxtCtrlA);
	l_pcBsizer->Add(m_pctxtCtrlB);
	l_pcCsizer->Add(m_pctxtCtrlC);

	l_pcVluesSizer->Add(l_pcAsizer);
	l_pcVluesSizer->Add(l_pcBsizer);
	l_pcVluesSizer->Add(l_pcCsizer);
	l_pcVluesSizer->Add(this->CreateButtonSizer(wxOK | wxCANCEL), 0, wxEXPAND | wxALL, 5);

	m_pcSizer->Add(l_pcCartSizer);

	m_pcSizer->Add(l_pcVluesSizer);

	l_pcMainSizer->Add(m_pcSizer);
	 
	////////////////////////////////////////////
	DrawData(p_sVal);
	Layout();
	this->SetSizerAndFit(m_pcSizer);
 
	Centre();
 
}

void SP_DLG_FuzzyNumber_Drawing::init()
{
	 m_pcDataSet = new XYDynamicDataset();
	 

}
 
Chart* SP_DLG_FuzzyNumber_Drawing::CreateFuzzzyNumChart()
{
	 
		init();
		XYDynamicSerie *l_pcSerie = new XYDynamicSerie();
		l_pcSerie->AddXY(m_dA, 0);
		l_pcSerie->AddXY(m_dB, 1);
		l_pcSerie->AddXY(m_dC, 0);

		l_pcSerie->SetName("Triangular Fuzzy Number");

		// and add serie to it
		m_pcDataSet->AddSerie(l_pcSerie);

		XYPlot *l_pcPlot = new XYPlot();

		XYLineRenderer* l_pcLineRenderer = new XYLineRenderer(false, true, 1, 100);

		wxColour l_nColor(wxT("RED"));

		int l_nLineWidth = 2;


		int l_nLineStyle = wxSOLID;

		//create a pen
		wxPen l_pen(l_nColor, l_nLineWidth, l_nLineStyle);

		l_pcLineRenderer->SetSeriePen(0, &l_pen);

		// set line renderer to dataset
		m_pcDataSet->SetRenderer(l_pcLineRenderer);

		//create x and y axes
		NumberAxis *l_pcXAxis = NULL;
		NumberAxis *l_pcYAxis = NULL;

		l_pcXAxis = new NumberAxis(AXIS_BOTTOM);
		l_pcXAxis->SetTitle(wxT("Universe"));
		l_pcXAxis->SetLabelCount(3);

		l_pcYAxis = new NumberAxis(AXIS_LEFT);
		l_pcYAxis->SetTitle(wxT("Level"));
		l_pcYAxis->SetLabelCount(10);

		l_pcXAxis->SetFixedBounds(m_dA, m_dC);

		l_pcYAxis->SetFixedBounds(0, 1);

		// add axes and dataset to plot
		l_pcPlot->AddObjects(m_pcDataSet, l_pcYAxis, l_pcXAxis);
		m_pcChart = new Chart(l_pcPlot, wxT("Fuzzy Number Chart"));
		//m_pcDrawingPanel->SetChart(m_pcChart);
		return m_pcChart;
}
void SP_DLG_FuzzyNumber_Drawing::SetAVal(wxString& l_pTexTA)
{
	m_pctxtCtrlA->SetValue(l_pTexTA);
}
void SP_DLG_FuzzyNumber_Drawing::SetBVal(wxString& l_pTexTB)
{
	m_pctxtCtrlB->SetValue(l_pTexTB);
}
void SP_DLG_FuzzyNumber_Drawing::SetCVal(wxString& l_pTexTC)
{
	m_pctxtCtrlC->SetValue(l_pTexTC);
}
void SP_DLG_FuzzyNumber_Drawing::OnCloseDrsawingFrame(wxCloseEvent& ev)
{
	this->GetParent()->GetEventHandler()->ProcessEvent(ev);
	ev.Skip();
	Destroy();
}
void  SP_DLG_FuzzyNumber_Drawing::OnSpinChanged(wxSpinDoubleEvent& event)
{
	m_dA = m_pctxtCtrlA->GetValue();
	m_dB = m_pctxtCtrlB->GetValue();
	m_dC = m_pctxtCtrlC->GetValue();
	if (m_dA <= m_dB &&  m_dB< m_dC)
	{
		m_pcChart = CreateFuzzzyNumChart();
		m_pcDrawingPanel->SetChart(m_pcChart);
	}
}
 



void SP_DLG_FuzzyNumber_Drawing::DrawData(const wxString& p_stfnVal)
{
	if (!p_stfnVal.IsEmpty())
	{
		wxString l_sVal = p_stfnVal;
		wxString l_sAval = l_sVal.BeforeFirst(',');
		l_sVal.Replace(l_sAval, "", false);
		l_sVal.Replace(",", "", false);

		wxString l_sBval=l_sVal.BeforeFirst(',');
		wxString l_sCval = l_sVal.AfterLast(',');

 		if (!l_sAval.ToDouble(&m_dA)) { return; }
		if (!l_sBval.ToDouble(&m_dB)) { return; }
		if (!l_sCval.ToDouble(&m_dC)) { return; }

		//assign the values to the text control objects
		m_pctxtCtrlA->SetValue(l_sAval);
		m_pctxtCtrlB->SetValue(l_sBval);
		m_pctxtCtrlC->SetValue(l_sCval);

		//draw the coresponding TFN
		m_pcChart = CreateFuzzzyNumChart();
		m_pcDrawingPanel->SetChart(m_pcChart);

	}
}
void SP_DLG_FuzzyNumber_Drawing::OnDlgOk(wxCommandEvent& event)
{
	if (!(m_dA <= m_dB &&  m_dB < m_dC))
	{
		m_dA = 0;
		m_dB = 0;
		m_dC = 0;
	}
}
void SP_DLG_FuzzyNumber_Drawing::OnDlgCancel(wxCommandEvent& event)
{

		m_dA = 0;
		m_dB = 0;
		m_dC = 0;
 
}
