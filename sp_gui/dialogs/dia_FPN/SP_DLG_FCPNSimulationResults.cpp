/*
* SP_DLG_FCPNSimulationResults.cpp
* $Author: Geoerge Assaf
* $Version: 0.0 $
* $Revision: 0.0 $
* $Date: 27.01.2019
* Short Description:
* Implementation file for the Fuzzy Continuous result dialog
*/
//===============================================================================================
#include <wx/wx.h>
#include <wx/statline.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/valgen.h>
#include <wx/gbsizer.h>
#include <wx/ffile.h>
#include <wx/busyinfo.h>
#include "sp_core/base/SP_Error.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/extensions/SP_DS_Simulation.h"
#include "sp_gui/dialogs/dia_ContinuousPN/SP_DLG_CPNSimulationResults.h"
#include "sp_gui/dialogs/SP_DLG_MarkingOverview.h"
#include "sp_gui/dialogs/dia_SPN/SP_DLG_StFunctionOverview.h"
#include "sp_gui/dialogs/dia_SPN/SP_DLG_StParameterOverview.h"

#include "sp_gui/dialogs/SP_DLG_NewConstantDefinition.h"

#include "sp_gui/dialogs/SP_DLG_SimulationProperties.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/SP_DS_Node.h"
#include "sp_gr/SP_GR_Node.h"
#include "sp_core/SP_Core.h"
#include "sp_ds/extensions/continuous/SP_DS_PlaceODE.h"
#include "sp_ds/attributes/SP_DS_BoolAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"

#include "sp_ds/attributes/SP_DS_TypeAttribute.h"

#include "sp_ds/attributes/SP_DS_DoubleMarkingAttribute.h"
#include "sp_ds/attributes/SP_DS_DoubleMarkingDependentMultiplicity.h"

#include "sp_gui/dialogs/dia_SPN/SP_DLG_StDirectExportProperties.h"
#include "sp_gui/dialogs/dia_ContinuousPN/SP_DLG_CSVExport.h"
#include "sp_gui/dialogs/dia_FPN/SP_DLG_FCPNSimulationResults.h"

//#include "sp_ds/extensions/SP_DS_ThreadEvent.h"
//New simulator header file
#include "spsim/spsim.h"
#if !defined(__WXMSW__) && !defined(__WXPM__)
#include "sp_gui/widgets/sp_plot/plot_enl.xpm"
#include "sp_gui/widgets/sp_plot/plot_shr.xpm"
#include "sp_gui/widgets/sp_plot/plot_zin.xpm"
#include "sp_gui/widgets/sp_plot/plot_zot.xpm"
#endif
IMPLEMENT_CLASS(SP_DLG_FCPNSimulationResults, SP_DLG_CPNSimulationResults)

enum
{
	SAMPLING_CHOICES_ID2,
	TOTAL_TIMER,
	SP_ID_BUTTON_SAMPLING_PROP
};

BEGIN_EVENT_TABLE(SP_DLG_FCPNSimulationResults, SP_DLG_CPNSimulationResults)
EVT_SIMTHREAD(SP_SIMULATION_THREAD_EVENT, SP_DLG_FCPNSimulationResults::OnSimulatorThreadEvent)
EVT_CHOICE(SAMPLING_CHOICES_ID2, SP_DLG_FCPNSimulationResults::OnSamplingAlgoChanged)
EVT_BUTTON(SP_ID_BUTTON_SAMPLING_PROP, SP_DLG_FCPNSimulationResults::OnSamplingProperties)
END_EVENT_TABLE()
SP_DLG_FCPNSimulationResults::SP_DLG_FCPNSimulationResults(SP_DS_Graph* p_pcGraph, wxWindow* p_pcParent, wxString p_sHelpText, wxString p_sTitle, long p_nStyle) :
	SP_DLG_CPNSimulationResults(p_pcGraph, p_pcParent, p_sHelpText, p_sTitle, p_nStyle)//, m_bIsSimulatorInitialized(false), m_nRedraw(true), m_nSimulationSemantics(0)
{
	render_loop_on = false;
	m_pcCompressedBand = new SP_Compressed_Fuzzy_Band();

	wxSizer* l_pcRowSizer = NULL;
	wxSizer* l_pcRowSizer1 = NULL;
	wxSizer* l_pcRowSizer2 = NULL;
	wxSizer* l_pcRowSizer3 = NULL;
	m_fr = nullptr;
	m_bIsAbort = false;
	m_lSamplingStrategyselection = 0;
	wxString l_sAlphaLevels;
	wxString l_sSamplingPoints;

	m_lTotalSimRuns = 0;
	m_lruncounter = 0;
	m_pcSamplingchoices = new wxChoice(m_pcPropertyWindowPropertySizer, SAMPLING_CHOICES_ID2, wxDefaultPosition, wxSize(100, -1));

	m_pcSamplingchoices->Append(wxT("Basic"));
	m_pcSamplingchoices->Append(wxT("Reduced"));
	m_pcSamplingchoices->Append(wxT("Random LHS"));
	m_pcSamplingchoices->Append(wxT("Improved LHS"));
	m_pcSamplingchoices->Append(wxT("Optimum LHS"));
	m_pcSamplingchoices->Append(wxT("Genetic LHS"));
	m_pcSamplingchoices->Append(wxT("Maximin LHS"));

	 m_pcFuzzySimProp = *(m_pcGraph->GetMetadataclass(wxT("FuzzySettings"))->GetElements()->begin());
		if (m_pcFuzzySimProp)
		{
			SP_DS_Attribute* l_pcAttr = m_pcFuzzySimProp->GetAttribute(wxT("SamplingAlgo"));
			if (l_pcAttr)
			{
				SP_DS_NumberAttribute* l_pcAttr = dynamic_cast<SP_DS_NumberAttribute*>(m_pcFuzzySimProp->GetAttribute(wxT("SamplingAlgo")));
				if (l_pcAttr)
				{
					if (l_pcAttr->GetValue() >= 0 && l_pcAttr->GetValue() <= 2)
					{
						m_lSamplingStrategyselection = l_pcAttr->GetValue();
					}
				}
			}

			//SP_DS_Attribute* l_pcAttrAlpha = l_pcSimProp->GetAttribute(wxT("AlphaLevels"));
			l_sAlphaLevels = GetFuzzySettingAttribute(*m_pcGraph->GetMetadataclass(wxT("FuzzySettings"))->GetElements()->begin(), wxT("AlphaLevels"));

			if (l_sAlphaLevels.IsEmpty())
			{
				l_sAlphaLevels = wxT("10");
			}

			l_sSamplingPoints= GetFuzzySettingAttribute(*m_pcGraph->GetMetadataclass(wxT("FuzzySettings"))->GetElements()->begin(), wxT("SamplingPoints"));

			if (l_sSamplingPoints.IsEmpty())
			{
				l_sSamplingPoints = wxT("10");
			}
		}

	m_pcSamplingchoices->SetSelection(m_lSamplingStrategyselection);
	l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
	l_pcRowSizer->Add(new wxStaticText(m_pcPropertyWindowPropertySizer, -1, wxT("Fuzzy Settings:")), wxSizerFlags(1).Expand().Border(wxALL, 2));
	///////////////////////////////////////
	l_pcRowSizer1 = new wxBoxSizer(wxHORIZONTAL);
	l_pcRowSizer2 = new wxBoxSizer(wxHORIZONTAL);
	l_pcRowSizer3 = new wxBoxSizer(wxHORIZONTAL);
	l_pcRowSizer1->Add(new wxStaticText(m_pcPropertyWindowPropertySizer, -1, wxT("alpha levels")), wxSizerFlags(1).Expand().Border(wxALL, 2));
	m_lalphaLevels = new  wxTextCtrl(m_pcPropertyWindowPropertySizer, -1, l_sAlphaLevels, wxDefaultPosition, wxDefaultSize, 0);
	l_pcRowSizer1->Add(m_lalphaLevels, wxSizerFlags(1).Expand().Border(wxALL, 2));
	/////////////////////
	l_pcRowSizer2->Add(new wxStaticText(m_pcPropertyWindowPropertySizer, -1, wxT("discretisation points")), wxSizerFlags(1).Expand().Border(wxALL, 2));

	m_lSamplePoints = new  wxTextCtrl(m_pcPropertyWindowPropertySizer, -1, l_sSamplingPoints, wxDefaultPosition, wxDefaultSize, 0);


	l_pcRowSizer2->Add(m_lSamplePoints, wxSizerFlags(1).Expand().Border(wxALL, 2));


	l_pcRowSizer3->Add(new wxStaticText(m_pcPropertyWindowPropertySizer, -1, wxT("sampling strategy")), wxSizerFlags(1).Expand().Border(wxALL, 2));
	l_pcRowSizer3->Add(m_pcSamplingchoices, 1, wxALL, 5);

	wxButton* m_pcSamplingProp = new wxButton(m_pcPropertyWindowPropertySizer, SP_ID_BUTTON_SAMPLING_PROP, wxT("Properties"));
	l_pcRowSizer3->Add(m_pcSamplingProp, 0, wxALL
	#if wxCHECK_VERSION(2,8,8)
			| wxRESERVE_SPACE_EVEN_IF_HIDDEN
	#endif
			, 5);
	m_pcSamplingchoices->SetSelection(m_lSamplingStrategyselection);


	m_pcPropertySizer->Add(l_pcRowSizer, wxSizerFlags(0).Expand().Border(wxALL, 2));
	m_pcPropertySizer->Add(l_pcRowSizer1, wxSizerFlags(0).Expand().Border(wxALL, 2));
	m_pcPropertySizer->Add(l_pcRowSizer2, wxSizerFlags(0).Expand().Border(wxALL, 2));
	m_pcPropertySizer->Add(l_pcRowSizer3, wxSizerFlags(0).Expand().Border(wxALL, 2));

	SP_DS_Nodeclass* l_pcNodeclass = m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_PLACE);
	m_lpCount = l_pcNodeclass->GetElements()->size();
	m_initialRun = false;
	m_lnFuzzyNum = 0;
	m_lSimTim = 0;

	SetSizerAndFit(m_pcMainSizer);

	Layout();
	activateRenderLoop(true);
}

void SP_DLG_FCPNSimulationResults::activateRenderLoop(bool on)
{
	if (on && !render_loop_on)
	{
		Connect(wxID_ANY, wxEVT_IDLE, wxIdleEventHandler(SP_DLG_FCPNSimulationResults::onIdle));
		render_loop_on = true;
	}
	else if (!on && render_loop_on)
	{
		Disconnect(wxEVT_IDLE, wxIdleEventHandler(SP_DLG_FCPNSimulationResults::onIdle));
		render_loop_on = false;
	}
}
void SP_DLG_FCPNSimulationResults::onIdle(wxIdleEvent& evt)
{
	if (render_loop_on)
	{
		//m_pcStartButton->SetBackgroundColour(*wxRED);
		Update();
		evt.RequestMore(); // render continuously, not only once on idle
	}
}
SP_DLG_FCPNSimulationResults::~SP_DLG_FCPNSimulationResults()
{
	SaveFuzzySimulatorProperties();
	wxDELETE(m_fr);
	m_pcGraph->GetParentDoc()->Modify(true);
}

void SP_DLG_FCPNSimulationResults::InitializeParamMatrix()
{
	unsigned long num_lparam = 0;
	SP_DS_Metadataclass* l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);
	num_lparam = l_pcMetadataclass->GetElements()->size();
	m_paramMatrix.resize(num_lparam, std::vector<double>(3, 0));

}

void SP_DLG_FCPNSimulationResults::ConvertTFNListToParamMatrix(std::vector<TriangularFN> asTFNParams)
{
	m_paramMatrix.clear();
	TFN_List::iterator it;

	SP_DS_Metadataclass* l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);

	long row = l_pcMetadataclass->GetElements()->size(); int num = 0;
	for (it = asTFNParams.begin(); it != asTFNParams.end(); ++it)
	{
		std::vector<double> myvector;

		if (num < row)
		{

			myvector.push_back(asTFNParams[num].getLeft());
			myvector.push_back(asTFNParams[num].getMiddle());
			myvector.push_back(asTFNParams[num].getRight());
			num++;
		}
		m_paramMatrix.push_back(myvector);
	}
}

bool SP_DLG_FCPNSimulationResults::InitializeFuzzySetting()
{
	long m_lAlphaLevels;
	long m_lnDiscPoints;
	m_lnFuzzyNum=0;
	m_fuzzyParams.clear();
	m_fuzzyParamPositions.clear();
	m_mFuzzyParam2Position.clear();

	if ((m_lalphaLevels->GetValue().ToLong(&m_lAlphaLevels)) && (m_lSamplePoints->GetValue().ToLong(&m_lnDiscPoints)))
	{
		if (m_lAlphaLevels > 0 && m_lnDiscPoints > 0)
		{

			LoadUsedParams();
			ConvertTFNListToParamMatrix(LoadParams());


			fuzzification::SAMPLING_TYPE l_tSamplingType;


			l_tSamplingType = GetSelectedSamplingMethod();

			  if (m_fr == nullptr)
			  {
				  m_fr = new FuzzyReasoning(m_lAlphaLevels, m_lnDiscPoints, m_paramMatrix, m_lnFuzzyNum, m_lpCount, l_tSamplingType);

			  }
			  else
			  {
				  m_fr->SetAlphaLevels(m_lAlphaLevels);
				  m_fr->SetDiscretisingPoints(m_lnDiscPoints);
				  m_fr->SetParamMatrix(m_paramMatrix);
				  m_fr->SetNumberFuzzyParams(m_lnFuzzyNum);
				  m_fr->SetPlaceNum(m_lpCount);
				  //m_fr->SetSamplingMethod(l_tSamplingType);
				  m_fr->InitialiseCombinationMatrix();

			  }
			  return true;
		}
		else {
			SP_MESSAGEBOX(wxT("The intered values of Alpha levels or Sample points must be larger than 0"));
			return false;
		}
	}
	else {
		SP_MESSAGEBOX(wxT("The intered values of Alpha levels or Sample points must be numerical"));
		return false;
	}

}

std::vector<TriangularFN> SP_DLG_FCPNSimulationResults::LoadParams()
{
	std::vector<TriangularFN>  asTFNParams;

		int iPos = 0;
		SP_DS_Metadataclass* mc = m_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);
		if (mc)
		{
			//go through all groups and activate the current selected sets
			for (size_t i = 0; i < m_apcColListAttr.size(); i++)
			{
				if (m_apcColListAttr[i])
				{
					if (i < m_apcComboBoxes.size())
					{
						m_apcColListAttr[i]->SetActiveList(m_apcComboBoxes[i]->GetSelection());
					}
					else
					{
						m_apcColListAttr[i]->SetActiveList(m_apcComboBoxes[m_apcComboBoxes.size() - 1]->GetSelection());
					}

				}
			}

			/******************************/
			SP_ListMetadata::const_iterator it;
			for (it = mc->GetElements()->begin(); it != mc->GetElements()->end(); ++it)
			{
				SP_DS_Metadata* l_pcConstant = *it;
				wxString l_sName = dynamic_cast<SP_DS_NameAttribute*>(l_pcConstant->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
				wxString l_sType = dynamic_cast<SP_DS_TypeAttribute*>(l_pcConstant->GetAttribute(wxT("Type")))->GetValue();///Added by G.A

				SP_DS_FunctionRegistry* l_pcFR = m_pcGraph->GetFunctionRegistry();

				wxString l_sValue;
				if (l_sType == wxT("TFN")&&  (m_mTransParamNames.find(l_sName) != m_mTransParamNames.end()))
				{
					 l_sValue = dynamic_cast<SP_DS_ColListAttribute*>(l_pcConstant->GetAttribute(wxT("ValueList")))->GetActiveCellValue(1);

					 SP_VectorDouble constants = GetFNConstants(l_sValue);
					 double a;
					 double b;
					 double c;
					 a = constants[0];
					 b = constants[1];
					 c = constants[2];
					 if (a&&b&&c) {
						 TriangularFN ttf(a, c, b);
						 asTFNParams.push_back(ttf);
						 m_lnFuzzyNum++;
						 m_mFuzzyParam2Position[l_sName] = iPos;
						 m_fuzzyParams.push_back(l_sName);
						 m_fuzzyParamPositions.push_back(iPos);
						 iPos++;
						 continue;
					 }
				}


				SP_DS_FunctionRegistryEntry l_FE = l_pcFR->lookUpFunction(l_sName);
				if (l_FE.IsOk()&& (m_mTransParamNames.find(l_sName) != m_mTransParamNames.end()))
				{
					SP_FunctionPtr l_Function = l_FE.getFunction();

					double l_nValue = 0.0;
					if (l_Function->isValue())
					{
						if (l_sType == wxT("int"))
						{
							l_nValue = (int)l_Function->getValue();

						}
						else if (l_sType == wxT("double"))
						{
							l_nValue = l_Function->getValue();

						}

						if (l_nValue)
						{
							TriangularFN ttf(l_nValue, l_nValue, l_nValue);
							asTFNParams.push_back(ttf);
						}
					}

				}
				iPos++;
			}
		}
		return asTFNParams;
}
SP_VectorDouble   SP_DLG_FCPNSimulationResults::GetFNConstants(const wxString &val)
{
	SP_VectorDouble v_dConstants;
	std::string s = val;
	wxString delimiter = ",";
	bool isOk = true;
	size_t pos = 0;
	wxString token;
	int numConst = 0;
	while ((pos = s.find(delimiter)) != std::string::npos) {
		numConst++;
		token = s.substr(0, pos);
		double value;
		if (token.ToDouble(&value)) { v_dConstants.push_back(value); }
		s.erase(0, pos + delimiter.length());

	}
	wxString ss = s;
	double d;

	if (ss.ToDouble(&d)) { v_dConstants.push_back(d); }


	return v_dConstants;
}

void SP_DLG_FCPNSimulationResults::LoadParameters(unsigned long lIteration, double dAlpha)
{
	SP_DS_Metadataclass* mc = m_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);
	SP_ListMetadata::const_iterator it;

	SP_VectorStdString l_asParameterNames;
	SP_VectorDouble l_anParameterValue;

	m_msParameterName2Value.clear();

	for (it = mc->GetElements()->begin(); it != mc->GetElements()->end(); ++it)
	{
		SP_DS_Metadata* l_pcConstant = *it;
		wxString l_sName = dynamic_cast<SP_DS_NameAttribute*>(l_pcConstant->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
		wxString l_sType = dynamic_cast<SP_DS_TypeAttribute*>(l_pcConstant->GetAttribute(wxT("Type")))->GetValue();///Added by G.A
		if (m_mTransParamNames.find(l_sName) != m_mTransParamNames.end() )//&& l_sType==wxT("TFN"))//13.11																										   //if(l_sName.Cmp("TFN")!=0 || l_sName.Cmp("BFN") != 0|| l_sName.Cmp("TZN") != 0)																						///
		{
			l_asParameterNames.push_back(l_sName);
		}


	}
	/*when alpha is exactly equals 1 then we need only one combination*/
	if (dAlpha == 1)
	{
		std::vector<double>  params = GetCombinationVectorForTopLevel();
		for (int y = 0; y < l_asParameterNames.size(); y++)
		{

			l_anParameterValue.push_back(params[y]);

			m_msParameterName2Value[l_asParameterNames[y]] = params[y];

			wxString ss;
			ss << params[y];
			m_pcGraph->GetFunctionRegistry()->registerFunction(l_asParameterNames[y], ss);


		}

		m_pcMainSimulator->SetParameterNames(l_asParameterNames);
		m_pcMainSimulator->SetParameterValues(l_anParameterValue);

		return;
	}



	std::vector<double> c;
	c.clear();
	c = m_fr->GetCombinationMatrix()[lIteration];
	m_vCurrentSample.clear();
	for (int y = 0; y < l_asParameterNames.size(); y++)
	{
		if (m_mFuzzyParam2Position.find(l_asParameterNames[y]) != m_mFuzzyParam2Position.end())//if (l_asParameterNames[y] == m_fuzzyParams[y])
		{
			int pos = m_mFuzzyParam2Position[l_asParameterNames[y]];
			l_anParameterValue.push_back(c[pos]);

			m_msParameterName2Value[l_asParameterNames[y]] = c[pos];

			wxString ss1;
			ss1 << c[pos];
			m_pcGraph->GetFunctionRegistry()->registerFunction(l_asParameterNames[y], ss1);
			m_vCurrentSample.push_back(c[pos]);
			continue;
		}

		l_anParameterValue.push_back(c[y]);

		m_msParameterName2Value[l_asParameterNames[y]] = c[y];

		wxString ss;
		ss << c[y];
		m_pcGraph->GetFunctionRegistry()->registerFunction(l_asParameterNames[y], ss);


	}

	m_pcMainSimulator->SetParameterNames(l_asParameterNames);
	m_pcMainSimulator->SetParameterValues(l_anParameterValue);

}
 



std::vector<double> SP_DLG_FCPNSimulationResults::GetCombinationVectorForTopLevel()
{
	std::vector<double> param_top(m_paramMatrix.size(), 0);
	for (int i = 0; i < m_paramMatrix.size(); i++)
		param_top[i] = m_paramMatrix[i][1];

	return param_top;


}


void SP_DLG_FCPNSimulationResults::OnStartAbortSimulation(wxCommandEvent& p_cEvent)
{

	wxString strButtonStatus = m_pcStartButton->GetLabel();

	if (strButtonStatus.compare(wxT("Abort Simulation")) == 0 || strButtonStatus.compare(wxT("Abort Processing")) == 0)
	{

		wxString s_warningMsg = "You are going to halt the simulation, please confirm!";
		wxMessageDialog* pwarningSimStopDialog = new wxMessageDialog(this, s_warningMsg, "Warning", wxOK | wxCANCEL | wxICON_WARNING);
		if (pwarningSimStopDialog->ShowModal() == wxID_OK)
		{
			m_bIsAbort = true;
			m_fr->SetAbort(true);
			m_pcStartButton->SetLabel(wxT("Start Simulation"));
			m_pcStartButton->SetBackgroundColour(*wxGREEN);
			Update();
		}
	}
	else {
		//Export Type
		if (m_pcDirectExportRadioButton->GetValue())
			m_ExportType = SP_SIM_DIRECT_EXPORT;
		else
			m_ExportType = SP_SIM_NO_EXPORT;



		fuzzification::SAMPLING_TYPE l_typeSampling = GetSelectedSamplingMethod();

		render_loop_on = true;
		activateRenderLoop(true);

		if (l_typeSampling == fuzzification::SAMPLING_TYPE::REDUCED)
		{
			RunReducedSamplingFCPNThreadSim();
		}
		else
		{
			RunBasicSamplingFcpnThreadSim();
		}

		//export final result
		if (m_ExportType == SP_SIM_DIRECT_EXPORT)
		{
			//open export file
			OpenExportFile();

			//do the export
			DirectExportToCSV();

			//close the export
			CloseExportFile();
		}
	}

}


void* SP_DLG_FCPNSimulationResults::RunReducedSamplingFCPNThreadSim()
{

	bool bisInitialized = InitializeFuzzySetting();
	m_fcpnThreadVector.clear();
	if (!bisInitialized)

	{
		return 0;
	}
	/*clear the previous result band if it was filled out*/
	m_vResultFBand.clear();

	/*inform the user to the  simulation running times*/
	long l_numofLevels = m_fr->GetNumFuzzyLevels();
	long l_numofSamples = m_fr->GetNumSamplePoints();
	long l_numFN = m_fr->GetNumFuzzyNum();

	/*calculate how many times the simulation should be run*/
	long l_nRunningtimes = pow(l_numofSamples, l_numFN) + (l_numofLevels - 1) * 2 + 1;//pow(numofSamples, numFN)* numofLevels+1;
	m_lTotalSimRuns = l_nRunningtimes;
	m_lruncounter = 0;
	const std::string sWarnMesag = "The total simulation number is: ";

	std::string s_warningMsg = sWarnMesag + std::to_string(l_nRunningtimes);

	SP_LOGWARNING(s_warningMsg);// add the message to log window

	wxMessageDialog* p_messageDialog = new wxMessageDialog(this, s_warningMsg, "High number of Simulation Runs", wxOK | wxCANCEL | wxICON_WARNING);

	if (p_messageDialog->ShowModal() == wxID_OK)
	{
		m_stopWatch.Start();
		long lRunCount = 0;
		m_pcStartButton->SetLabel(wxT("Abort Simulation"));
		m_pcStartButton->SetBackgroundColour(*wxRED);
		//m_pcStartButton->SetForegroundColour(*wxRED);
		Update();
		m_fr->CalculateAlphaLevels();

		for (int ialpha = 0; ialpha < m_fr->GetAlphaSet().size() - 1; ialpha++)
		{
			if (m_bIsAbort)
			{
				break;
			}
			/*do simulation on all sample points of the very firt level*/
			if (ialpha == 0 && !m_bIsAbort)
			{
				/*do sample point combination*/
				m_fr->DoSamplePointsCombination(m_fr->GetAlphaSet()[ialpha]);

				/*calculate the number of simulation in the current level*/
				long m_lNumCombinedPoints = pow(m_fr->GetNumSamplePoints(), m_fr->GetNumFuzzyNum());

				for (long literation = 0; literation < m_lNumCombinedPoints; literation++)
				{

					bool m_bIsInitialized = InitializeSimulator(literation, m_fr->GetAlphaSet()[ialpha]);

					if (m_bIsInitialized == true && !m_bIsAbort)
					{
						m_fcpnThreadVector.push_back(new SP_DS_FcpnSimulThread(this, m_pcMainSimulator, m_fr->GetAlphaSet()[ialpha], m_vCurrentSample));


						if (m_fcpnThreadVector[lRunCount]->Create() != wxTHREAD_NO_ERROR)
						{
						}

						m_fcpnThreadVector[lRunCount]->SetPriority(WXTHREAD_MAX_PRIORITY);

						if (m_fcpnThreadVector[lRunCount]->Run())
						{

						}
						lRunCount++;
						//m_lruncounter = lRunCount;
						m_fcpnThreadVector[lRunCount - 1]->Wait();

						wxYieldIfNeeded();

						wxDELETE(m_fcpnThreadVector[lRunCount - 1]);
					}
				}
			}
			else {// alpha >0
				m_fr->DoSamplePointsCombination(m_fr->GetAlphaSet()[ialpha]);
				long m_lNumCombinedPoints = pow(m_fr->GetNumSamplePoints(), m_fr->GetNumFuzzyNum());

				/*do simulation on the first and last sample combination and check those in between*/

				bool m_bIsInitialized = InitializeSimulator(0, m_fr->GetAlphaSet()[ialpha]);

				if (m_bIsInitialized == true && !m_bIsAbort)
				{
					m_fcpnThreadVector.push_back(new SP_DS_FcpnSimulThread(this, m_pcMainSimulator, m_fr->GetAlphaSet()[ialpha], m_vCurrentSample));
					if (m_fcpnThreadVector[lRunCount]->Create() != wxTHREAD_NO_ERROR)
					{
					}

					m_fcpnThreadVector[lRunCount]->SetPriority(WXTHREAD_MAX_PRIORITY);

					if (m_fcpnThreadVector[lRunCount]->Run())
					{

					}
					lRunCount++;
					//m_lruncounter = lRunCount;
					m_fcpnThreadVector[lRunCount - 1]->Wait();

					wxYieldIfNeeded();

					wxDELETE(m_fcpnThreadVector[lRunCount - 1]);

				}

				vector<double> vdLow = GetCurentSamples(0);
				vector<double> vdUp = GetCurentSamples(m_lNumCombinedPoints - 1);
				//TraceElement traceElement;
				for (long literation = 1; literation < m_lNumCombinedPoints - 1; literation++)
				{
					if (m_bIsAbort)
					{
						break;
					}
					int icount = 0;

					for (int j = 0; j < m_fr->GetNumFuzzyNum(); j++)
					{

						if (m_vResultFBand[literation].sample[j] > vdLow[j] && m_vResultFBand[literation].sample[j] < vdUp[j])
						{
							icount++;
							if (icount == m_fr->GetNumFuzzyNum() && m_fr->GetAlphaSet()[ialpha] != 1)
							{

								TraceElement structTraceElement = m_vResultFBand[literation];
								structTraceElement.currentLevel = m_fr->GetAlphaSet()[ialpha];;
								structTraceElement.levelNum = ialpha;
								m_vResultFBand.push_back(structTraceElement);
							}

						}

					}
				}

				bool m_bIsInitialized1 = InitializeSimulator(m_lNumCombinedPoints - 1, m_fr->GetAlphaSet()[ialpha]);
				m_initialRun = true;
				if (m_bIsInitialized1 == true && !m_bIsAbort)
				{
					m_vCurrentSample = GetCurentSamples(m_lNumCombinedPoints - 1);
					m_fcpnThreadVector.push_back(new SP_DS_FcpnSimulThread(this, m_pcMainSimulator, m_fr->GetAlphaSet()[ialpha], m_vCurrentSample));
					if (m_fcpnThreadVector[lRunCount]->Create() != wxTHREAD_NO_ERROR)
					{
					}

					m_fcpnThreadVector[lRunCount]->SetPriority(WXTHREAD_MAX_PRIORITY);

					if (m_fcpnThreadVector[lRunCount]->Run())
					{

					}
					lRunCount++;
					//m_lruncounter = lRunCount;
					m_fcpnThreadVector[lRunCount - 1]->Wait();

					wxYieldIfNeeded();

					wxDELETE(m_fcpnThreadVector[lRunCount - 1]);
				}

			}

		}//for alpha

		 /*do simulation on the very last level alpha=1*/
		bool m_bIsInitialized = InitializeSimulator(0, 1);
		m_initialRun = true;
		if (m_bIsInitialized == true && !m_bIsAbort)
		{
			m_fcpnThreadVector.push_back(new SP_DS_FcpnSimulThread(this, m_pcMainSimulator, 1, m_vCurrentSample));
			if (m_fcpnThreadVector[lRunCount]->Create() != wxTHREAD_NO_ERROR)
			{
			}

			m_fcpnThreadVector[lRunCount]->SetPriority(WXTHREAD_MAX_PRIORITY);

			if (m_fcpnThreadVector[lRunCount]->Run())
			{

			}
			lRunCount++;

			m_fcpnThreadVector[lRunCount - 1]->Wait();

			wxYieldIfNeeded();

			wxDELETE(m_fcpnThreadVector[lRunCount - 1]);
		}


		if (m_bIsAbort)
		{
			m_pcSimulationProgressGauge->SetValue(0);
			m_pcSimulationProgressText->SetLabel(wxString::Format(wxT("%i %%"), 0));
			m_bIsAbort = false;
			m_vResultFBand.clear();
			m_stopWatch.Pause();
			m_pcMainSimulator->AbortSimulation();
			Update();

		}
	}
	else {
		return NULL;
	}

	return NULL;
}

bool SP_DLG_FCPNSimulationResults::InitializeSimulator(unsigned long ite, double a)
{

	double l_nOutputEndPoint = 0;
	double l_nOutputStartPoint;
	long l_nLong0;

	if (m_pcIntervalEndTextCtrl->GetValue().ToDouble(&l_nOutputEndPoint) && l_nOutputEndPoint > 0)
	{
		m_pcMainSimulator->SetOutputEndPoint(l_nOutputEndPoint);

		SP_DS_Metadata* l_pcSimProp = *(m_pcGraph->GetMetadataclass(wxT("Simulation Properties"))->GetElements()->begin());
		SP_DS_Attribute* l_pcAttr = l_pcSimProp->GetAttribute(wxT("interval end"));
		l_pcAttr->SetValueString(m_pcIntervalEndTextCtrl->GetValue());
	}
	else
	{
		SP_MESSAGEBOX(wxT("The interval end value must be non-negative real number >0. The inputed value is invalid."), wxT("Error"), wxOK | wxICON_ERROR);
		return false;
	}

	if (m_pcIntervalStartTextCtrl->GetValue().ToDouble(&l_nOutputStartPoint) && l_nOutputStartPoint >= 0 && l_nOutputStartPoint < l_nOutputEndPoint)
	{
		m_pcMainSimulator->SetOutputStartPoint(l_nOutputStartPoint);

		SP_DS_Metadata* l_pcSimProp = *(m_pcGraph->GetMetadataclass(wxT("Simulation Properties"))->GetElements()->begin());
		SP_DS_Attribute* l_pcAttr = l_pcSimProp->GetAttribute(wxT("interval start"));
		l_pcAttr->SetValueString(m_pcIntervalStartTextCtrl->GetValue());
	}
	else
	{
		SP_MESSAGEBOX(wxT("The interval start value must be smaller than the interval end value. The inputed values are invalid."), wxT("Error"), wxOK | wxICON_ERROR);
		return false;
	}

	if (m_pcResultPointCountTextCtrl->GetValue().ToLong(&l_nLong0) && l_nLong0 > 0)
	{
		m_nResultPointCount = l_nLong0 + 1;

		m_pcMainSimulator->SetOutputSampleSize((l_nOutputEndPoint - l_nOutputStartPoint) / (m_nResultPointCount - 1));

		SP_DS_Metadata* l_pcSimProp = *(m_pcGraph->GetMetadataclass(wxT("Simulation Properties"))->GetElements()->begin());
		SP_DS_Attribute* l_pcAttr = l_pcSimProp->GetAttribute(wxT("output step"));
		l_pcAttr->SetValueString(m_pcResultPointCountTextCtrl->GetValue());
	}
	else
	{
		SP_MESSAGEBOX(wxT("The output interval count value must be positive real number. The inputed value is invalid."), wxT("Error"), wxOK | wxICON_ERROR);
		return false;
	}

	if (!m_bIsSimulatorInitialized)
	{
		if (m_initialRun == false) {
			//wxBusyInfo l_Info(wxT("Loading data, please wait...."), this);
		}
		//Parameters
		LoadParameters(ite, a);

		//Places
		LoadPlaces();

		//Transitions
		LoadTransitions();

		//Load Connections
		LoadConnections();

		// load observers
		LoadObservers();
	}
	wxBusyInfo* l_pcInfo;
	if (m_initialRun == false)
	{
		l_pcInfo = new wxBusyInfo(wxT("Initializing the ODEs solver, please wait...."), this);
		wxDELETE(l_pcInfo);
	}
	bool l_bIsInitialized = m_pcMainSimulator->Initialise(true);



	//Get some properties from the user
	if (m_pcMainSimulator->GetSimulatorOptions()->GetOption("InBetweenVisualization") != NULL)
	{
		m_nRedraw = m_pcMainSimulator->GetSimulatorOptions()->GetOption("InBetweenVisualization")->GetValueBool();
	}

	if (m_pcMainSimulator->GetSimulatorOptions()->GetOption("Refreshrate") != NULL)
	{
		m_nRefreshRate = m_pcMainSimulator->GetSimulatorOptions()->GetOption("Refreshrate")->GetValuelong();
	}

	//SetSimulationProgressGaugeRange(l_nLong0);//


	return l_bIsInitialized;
}

void  SP_DLG_FCPNSimulationResults::LoadTransitions()
{

	SP_DS_Nodeclass* l_pcNodeclass;
	l_pcNodeclass = m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_TRANS);
	SP_ListNode::const_iterator l_itElem;

	SP_DS_ColListAttribute* l_pcColList;

	m_pcMainSimulator->SetTransitionCount(l_pcNodeclass->GetElements()->size());

	unsigned long l_nPosition = 0;

	//clear old transitions
	m_pcMainSimulator->ClearTransitions();

	SP_DS_FunctionRegistry* l_pcFR = m_pcGraph->GetFunctionRegistry();

	//Go through all the transition nodes
	for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); l_itElem++)
	{
		wxString l_sName = dynamic_cast<SP_DS_NameAttribute*>((*l_itElem)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		//Get the transition rate function
		l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>((*l_itElem)->GetAttribute(wxT("FunctionList")));
		//double vv = m_msParameterName2Value[""];
		wxString l_sTransitionFunction = l_pcColList->GetActiveCellValue(1);
		/********************/
		/*
		string l_str = l_sTransitionFunction.ToStdString();
		typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
		boost::char_separator<char> sep("(+-/%*) ");
		tokenizer tokens(l_str, sep);
		
		for (tokenizer::iterator beg = tokens.begin(); beg != tokens.end(); ++beg) {
			auto pos = m_msParameterName2Value.find(*beg);
			wxString result=*beg;
			if (pos != m_msParameterName2Value.end())
			{
				double l_dValue = m_msParameterName2Value[*beg];
				wxString l_sVal;
				l_sVal << l_dValue;
				l_sTransitionFunction.Replace(*beg, l_sVal);
		 
			}

		}
		*/
		wxString l_sTobeTokenized=l_sTransitionFunction;
		wxStringTokenizer tokenize(l_sTobeTokenized,"(+-/%*) ");
		while(tokenize.HasMoreTokens())
		{
			wxString token=tokenize.GetNextToken();
			auto pos = m_msParameterName2Value.find(token);
			if (pos != m_msParameterName2Value.end())
				{
					double l_dValue = m_msParameterName2Value[token];
					wxString l_sVal;
					l_sVal << l_dValue;
					l_sTransitionFunction.Replace(token, l_sVal);

				}

		}
		/**********************/
		SP_FunctionPtr l_pcFunction = l_pcFR->parseFunctionString(l_sTransitionFunction);
		wxString l_sExpanded;
		if (l_pcFunction)
		{
			SP_FunctionPtr l_pcExpanded = l_pcFR->substituteFunctions(l_pcFunction);
			l_sExpanded = l_pcExpanded->toString();
		}
		else
		{
			l_sExpanded = l_sTransitionFunction;
		}

		//add a transition
		m_pcMainSimulator->AddTransition(l_sName, l_sExpanded, spsim::TRANSITION_TYPE_CONTINUOUS);

		m_asTransitionNames.push_back(l_sName);
		m_mTransitionName2Position[l_sName] = l_nPosition;

		l_nPosition++;
	}

}



void SP_DLG_FCPNSimulationResults::LoadUsedParams()
{

	SP_DS_Nodeclass* l_pcNodeclass; m_mTransParamNames.clear();
	l_pcNodeclass = m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_TRANS);
	SP_ListNode::const_iterator l_itElem;

	SP_DS_ColListAttribute* l_pcColList;

	m_pcMainSimulator->SetTransitionCount(l_pcNodeclass->GetElements()->size());

	unsigned long l_nPosition = 0;

	//Go through all the transition nodes
	for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); l_itElem++)
	{
		wxString l_sName = dynamic_cast<SP_DS_NameAttribute*>((*l_itElem)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		//Get the transition rate function
		l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>((*l_itElem)->GetAttribute(wxT("FunctionList")));

		const wxString l_sTransitionFunction = l_pcColList->GetActiveCellValue(1);

		wxString m_sParamName = GetKParameter(l_sTransitionFunction);
		/***/
		string l_str = m_sParamName.ToStdString();
		/*
		typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
		boost::char_separator<char> sep("(+-/%*) ");
		tokenizer tokens(l_str, sep);

		for (tokenizer::iterator beg = tokens.begin(); beg != tokens.end(); ++beg) {
			//auto pos = m_msParameterName2Value.find(*beg);
			wxString result = *beg;
			
			m_mTransParamNames[result] = l_sName;
			

		}
		*/
		wxString l_sTobeTokenized=m_sParamName;
				wxStringTokenizer tokenize(l_sTobeTokenized,"(+-/%*) ");
				while(tokenize.HasMoreTokens())
				{
					wxString l_sToken=tokenize.GetNextToken();
					m_mTransParamNames[l_sToken] = l_sName;

				}
		/***/



	}

}


wxString SP_DLG_FCPNSimulationResults::GetKParameter(const wxString& func)
{
	wxString m_sKparam;
	wxString l_msfun = func;
	l_msfun.Replace("MassAction", "");
	l_msfun.Replace("pow", "");
	l_msfun.Replace("(", "");
	l_msfun.Replace(")", "");
	m_sKparam = l_msfun;

	return m_sKparam;
}


void SP_DLG_FCPNSimulationResults::DirectExportToCSV()
{

	if (!m_pcExport)
		return;

	*m_pcExport << wxT("Time");


	wxString l_sSpacer = GetSpacer(m_nExportSpacer);
	wxString l_sCurrentRow, l_sOutput;

	SP_DS_Attribute* l_pcAttribute = m_pcCurrentTablePlot->GetAttribute(wxT("Nodeclass"));
	CHECK_POINTER(l_pcAttribute, return);
	wxString l_sElementType = l_pcAttribute->GetValueString();

	std::map<wxString, wxString> mPlaces2PosMap;
	std::vector<wxString> vstrPlaces;


	SP_DS_ColListAttribute* l_pcCurveInfoList = dynamic_cast<SP_DS_ColListAttribute*>(m_pcCurrentTablePlot->GetAttribute(wxT("CurveInfo")));
	CHECK_POINTER(l_pcCurveInfoList, return);

	for (unsigned int l_n = 0; l_n < l_pcCurveInfoList->GetRowCount(); l_n++)
	{

		wxString l_sPos = l_pcCurveInfoList->GetCell(l_n, 0);

		long l_nPos;
		if (!l_sPos.ToLong(&l_nPos))
			return;
		wxString l_sOutType = l_pcCurveInfoList->GetCell(l_n, 1);
		wxString l_sCheckState = l_pcCurveInfoList->GetCell(l_n, 2);
		if (l_sCheckState == wxT("1"))
		{
			wxString l_sName = l_pcCurveInfoList->GetCell(l_n, 6);
			mPlaces2PosMap[l_sName] = l_sPos;
			vstrPlaces.push_back(l_sName);

		}

	}



	if (!m_bExportAllTracesForFuzzy) { //export min/max bands over time

		for (int iCount = 0; iCount < mPlaces2PosMap.size(); iCount++)
		{
			wxString l_sName = vstrPlaces[iCount];


			wxString sCol, sCol1;
			sCol << l_sSpacer << l_sName << "_Min";
			*m_pcExport << sCol;
			sCol1 << l_sSpacer << l_sName << "_Max";
			*m_pcExport << sCol1;
		}
		*m_pcExport << wxT("\n");
		std::vector<SP_Vector2DDouble> m_nFuzzyResultBand;
		m_nFuzzyResultBand.clear();
		m_nFuzzyResultBand.push_back(m_pcCompressedBand->GetMinimumCurve());
		m_nFuzzyResultBand.push_back(m_pcCompressedBand->GetMaximumCurve());

		for (unsigned long l_nRow = 0; l_nRow < m_nFuzzyResultBand[0].size(); l_nRow++)
		{
			l_sOutput = wxT("");
			l_sOutput << dssd::aux::toString(m_pcMainSimulator->GetOutputStartPoint() + m_pcMainSimulator->GetOutputSampleSize() * l_nRow);
			l_sCurrentRow = wxT("");
			for (int j = 0; j < mPlaces2PosMap.size(); j++)
			{
				wxString l_sName = vstrPlaces[j];
				wxString l_sPos = mPlaces2PosMap[l_sName];
				long l_nPos;
				if (!l_sPos.ToLong(&l_nPos))
					return;
				double l_dResult = 0;

				for (int i = 0; i < m_nFuzzyResultBand.size(); i++)
				{
					SP_Vector2DDouble currentMat = m_nFuzzyResultBand[i];
					l_dResult = currentMat[l_nRow][l_nPos];
					//write 

					l_sCurrentRow << l_sSpacer;
					l_sCurrentRow << wxString::Format(wxT("%.16g"), l_dResult);
				}

			}
			l_sOutput << l_sCurrentRow;
			*m_pcExport << l_sOutput;// << wxT("\n");
			*m_pcExport << wxT("\n");
		}

	}
	else {// export all traces of each band

		for (int iCount = 0; iCount < mPlaces2PosMap.size(); iCount++)
		{
			wxString l_sName = vstrPlaces[iCount];

			for (unsigned i = 0; i < m_vResultFBand.size(); i++)
			{
				wxString sCol;
				sCol << l_sSpacer << l_sName << wxT("_") << i;
				*m_pcExport << sCol;
			}


		}
		*m_pcExport << wxT("\n");

		if (m_vResultFBand.size() == 0) return;

		for (unsigned int l_nTime = 0; l_nTime < m_vResultFBand[0].fuzzyTrace.size(); ++l_nTime)
		{
			l_sOutput = wxT("");
			l_sOutput << dssd::aux::toString(m_pcMainSimulator->GetOutputStartPoint() + m_pcMainSimulator->GetOutputSampleSize() * l_nTime);
			l_sCurrentRow = wxT("");

			for (int j = 0; j < mPlaces2PosMap.size(); j++)
			{
				wxString l_sName = vstrPlaces[j];
				wxString l_sPos = mPlaces2PosMap[l_sName];
				long l_nPos;
				if (!l_sPos.ToLong(&l_nPos))
					return;
				double l_dResult = 0;


				for (unsigned l_nRun = 0; l_nRun < m_vResultFBand.size(); ++l_nRun) {

					SP_Vector2DDouble currentMat = m_vResultFBand[l_nRun].fuzzyTrace;
					l_dResult = currentMat[l_nTime][l_nPos];
					l_sCurrentRow << l_sSpacer;
					l_sCurrentRow << wxString::Format(wxT("%.16g"), l_dResult);

				}
			}

			l_sOutput << l_sCurrentRow;
			*m_pcExport << l_sOutput;
			*m_pcExport << wxT("\n");

		}

	}

	if (m_bExportMembershipFunction) {//export all membership functions over time


		for (int j = 0; j < mPlaces2PosMap.size(); j++)
		{

			wxString l_sPlaceName = vstrPlaces[j];

			wxString l_sMembershipFunctions = m_sExportFilename.Before(wxChar('.'));

			l_sMembershipFunctions << wxT("_") << l_sPlaceName << wxT("_TfnList") << wxT(".csv");

			m_pcExportFileOutputStreamMembershipFun = new wxFileOutputStream(l_sMembershipFunctions);

			if (!m_pcExportFileOutputStreamMembershipFun->IsOk())
			{
				m_pcExportFileOutputStreamMembershipFun = new wxFileOutputStream(l_sMembershipFunctions);

				if (!m_pcExportFileOutputStreamMembershipFun->IsOk())
				{
					return;
				}
			}

			m_pcExportBufferdOutputStreamMembershipFun = new wxBufferedOutputStream(*m_pcExportFileOutputStreamMembershipFun);
			m_pcExportMembershipfuns = new wxTextOutputStream(*m_pcExportBufferdOutputStreamMembershipFun);

			if (!m_pcExportMembershipfuns) return;

			*m_pcExportMembershipfuns << wxT("Time_point") << l_sSpacer << wxT("Level") << l_sSpacer << l_sPlaceName;

			*m_pcExportMembershipfuns << wxT("\n");

			///
			SP_VectorDouble l_vAlphaLevels = m_pcCompressedBand->GetAlphaLevels();

			std::vector<Time2Membership> l_vAllVariablesMMf;

			wxString l_sPos = mPlaces2PosMap[l_sPlaceName];
			long l_nPos;
			if (!l_sPos.ToLong(&l_nPos))
				return;
			Time2Membership tfn_list = m_pcCompressedBand->GetTimeMembershipofPlace(l_nPos);

			l_vAllVariablesMMf.push_back(tfn_list);



			for (int level = 0; level < l_vAlphaLevels.size(); ++level)
			{

				for (int j = 0; j < mPlaces2PosMap.size(); j++)
				{
					wxString l_sName = vstrPlaces[j];
					wxString l_sPos = mPlaces2PosMap[l_sName];
					long l_nPos;
					if (!l_sPos.ToLong(&l_nPos))
						return;
					Time2Membership tfn_list = m_pcCompressedBand->GetTimeMembershipofPlace(l_nPos);

					for (int timePoint = 0; timePoint < tfn_list.size(); ++timePoint) {

						for (int level = 0; level < l_vAlphaLevels.size(); ++level)
						{

							wxString l_sExport;

							l_sExport << timePoint << l_sSpacer << l_vAlphaLevels[level] << l_sSpacer;

							for (int mf = 0; mf < l_vAllVariablesMMf.size(); ++mf) {

								Time2Membership l_mfTemp = l_vAllVariablesMMf[mf];

								auto tfn_vec = l_mfTemp[timePoint];
								l_sExport << tfn_vec[level][0];// << l_sSpacer;
							}

							l_sExport << wxT("\n");


							*m_pcExportMembershipfuns << l_sExport;
						}

						for (int level = l_vAlphaLevels.size() - 1; level >= 0; --level)
						{
							auto tfn = tfn_list[timePoint];
							wxString l_sExport;

							l_sExport << timePoint << l_sSpacer << l_vAlphaLevels[level] << l_sSpacer;

							for (int mf = 0; mf < l_vAllVariablesMMf.size(); ++mf) {

								Time2Membership l_mfTemp = l_vAllVariablesMMf[mf];

								auto tfn_vec = l_mfTemp[timePoint];
								l_sExport << tfn_vec[level][1];// << l_sSpacer;
							}

							l_sExport << wxT("\n");
							*m_pcExportMembershipfuns << l_sExport;
						}

						*m_pcExportMembershipfuns << wxT("\n");
						*m_pcExportMembershipfuns << wxT("\n");
						*m_pcExportMembershipfuns << wxT("\n");
					}

				}
			}

			wxDELETE(m_pcExportMembershipfuns);
			wxDELETE(m_pcExportBufferdOutputStreamMembershipFun);
			wxDELETE(m_pcExportFileOutputStreamMembershipFun);
		}
	}


	/**

	if (m_bExportMembershipFunction) {//export all membership functions over time

		if (!m_pcExportMembershipfuns) return;
		*m_pcExportMembershipfuns << wxT("Time_point") << l_sSpacer << wxT("Level") << l_sSpacer;

		for (int j = 0; j < mPlaces2PosMap.size(); j++)
		{
			wxString l_sName = vstrPlaces[j];

			wxString l_sHeader;

			l_sHeader << l_sName << l_sSpacer;

			*m_pcExportMembershipfuns << l_sHeader;
		}

		*m_pcExportMembershipfuns << wxT("\n");

		SP_VectorDouble l_vAlphaLevels = m_pcCompressedBand->GetAlphaLevels();

		std::vector<Time2Membership> l_vAllVariablesMMf;

		for (int j = 0; j < mPlaces2PosMap.size(); j++)
		{
			wxString l_sName = vstrPlaces[j];
			wxString l_sPos = mPlaces2PosMap[l_sName];
			long l_nPos;
			if (!l_sPos.ToLong(&l_nPos))
				return;
			Time2Membership tfn_list = m_pcCompressedBand->GetTimeMembershipofPlace(l_nPos);

			l_vAllVariablesMMf.push_back(tfn_list);
		}


		for (int level = 0; level < l_vAlphaLevels.size(); ++level)
		{
			//*m_pcExportMembershipfuns << l_vAlphaLevels[level] << l_sSpacer;

			for (int j = 0; j < mPlaces2PosMap.size(); j++)
			{
				wxString l_sName = vstrPlaces[j];
				wxString l_sPos = mPlaces2PosMap[l_sName];
				long l_nPos;
				if (!l_sPos.ToLong(&l_nPos))
					return;
				Time2Membership tfn_list = m_pcCompressedBand->GetTimeMembershipofPlace(l_nPos);

				for (int timePoint = 0; timePoint < tfn_list.size(); ++timePoint) {

					for (int level = 0; level < l_vAlphaLevels.size(); ++level)
					{
						auto tfn = tfn_list[timePoint];
						wxString l_sExport;

						//l_sExport << timePoint << l_sSpacer << l_vAlphaLevels[level] << l_sSpacer << tfn[level][0] << wxT("\n");
						l_sExport << timePoint << l_sSpacer << l_vAlphaLevels[level] << l_sSpacer;

						for (int mf = 0; mf < l_vAllVariablesMMf.size(); ++mf) {

							Time2Membership l_mfTemp = l_vAllVariablesMMf[mf];

							auto tfn_vec = l_mfTemp[timePoint];
							l_sExport << tfn_vec[level][0] << l_sSpacer;
						}

						l_sExport << wxT("\n");

						*m_pcExportMembershipfuns << l_sExport;
					}

					for (int level = l_vAlphaLevels.size() - 1; level >= 0; --level)
					{
						 
						wxString l_sExport;

						 
						l_sExport << timePoint << l_sSpacer << l_vAlphaLevels[level] << l_sSpacer;

						for (int mf = 0; mf < l_vAllVariablesMMf.size(); ++mf) {

							Time2Membership l_mfTemp = l_vAllVariablesMMf[mf];

							auto tfn_vec = l_mfTemp[timePoint];
							l_sExport << tfn_vec[level][1] << l_sSpacer;
						}

						l_sExport << wxT("\n");
						*m_pcExportMembershipfuns << l_sExport;
					}

				}

			}
		}

		wxDELETE(m_pcExportMembershipfuns);
		wxDELETE(m_pcExportBufferdOutputStreamMembershipFun);
		wxDELETE(m_pcExportFileOutputStreamMembershipFun);
	}
	*/
	 
}

void SP_DLG_FCPNSimulationResults::
OnExportToCSV()
{
	wxString l_sFilename = m_sExportFilename;
	bool     l_bCompressExact = false;
	bool l_bAlltraces = false;

	SP_DLG_CSVExport* l_pcDlg = new SP_DLG_CSVExport(SP_ST_SIM_EXPORT_CSV_EDIT_DLG_EXPLICIT, this,
		&l_sFilename, &m_nExportSpacer,
		&l_bCompressExact, &l_bAlltraces, m_bReplaceValue,
		m_nReplacedVaue);

	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		wxString l_sBackupFilename = m_sExportFilename;
		m_sExportFilename = l_sFilename;
		m_nReplacedVaue = l_pcDlg->GetReplacedValues();
		m_bExportAllTracesForFuzzy = l_pcDlg->IsAllTracesChecked();
		m_bExportMembershipFunction = l_pcDlg->IsMembershipFunctionChecked();
	 
		OpenExportFile();
		DirectExportToCSV();
		CloseExportFile();

		m_sExportFilename = l_sBackupFilename;

		m_bReplaceValue = l_pcDlg->GetReplaceValue();

		m_nReplacedVaue = l_pcDlg->GetReplacedValues();
	}

	l_pcDlg->Destroy();
}
std::vector<double> SP_DLG_FCPNSimulationResults::GetCurentSamples(int iteration)
{

	SP_DS_Metadataclass* mc = m_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);

	SP_ListMetadata::const_iterator it;

	SP_VectorStdString l_asParameterNames;

	for (it = mc->GetElements()->begin(); it != mc->GetElements()->end(); ++it)
	{
		SP_DS_Metadata* l_pcConstant = *it;
		wxString l_sName = dynamic_cast<SP_DS_NameAttribute*>(l_pcConstant->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
		wxString l_sType = dynamic_cast<SP_DS_TypeAttribute*>(l_pcConstant->GetAttribute(wxT("Type")))->GetValue();///Added by G.A
		if (m_mTransParamNames.find(l_sName) != m_mTransParamNames.end())																										   //if(l_sName.Cmp("TFN")!=0 || l_sName.Cmp("BFN") != 0|| l_sName.Cmp("TZN") != 0)																						///
		{
			l_asParameterNames.push_back(l_sName);
		}

	}

	std::vector<double> vdCurrentCombination;
	vdCurrentCombination.clear();
	vdCurrentCombination = m_fr->GetCombinationMatrix()[iteration];
	m_vCurrentSample.clear();
	for (int y = 0; y < l_asParameterNames.size(); y++)
	{
		if (m_mFuzzyParam2Position.find(l_asParameterNames[y]) != m_mFuzzyParam2Position.end())//if (l_asParameterNames[y] == m_fuzzyParams[y])
		{
			int pos = m_mFuzzyParam2Position[l_asParameterNames[y]];
			m_vCurrentSample.push_back(vdCurrentCombination[pos]);
			continue;
		}
	}
	return m_vCurrentSample;
}
void SP_DLG_FCPNSimulationResults::SetSimulationProgressText(long& p_nValue)
{
	if (m_lTotalSimRuns != 0)
	{
		m_pcSimulationProgressText->SetLabel(wxString::Format(wxT("%i %%"), 100 - ((p_nValue * 100) / m_lTotalSimRuns)));


	}


}

void SP_DLG_FCPNSimulationResults::SetSimulationProgressGauge(long p_nValue)
{
	if (m_lTotalSimRuns != 0) {
		int iProgress = 100 - ((p_nValue * 100) / m_lTotalSimRuns);
		if (iProgress>100) { iProgress = 100; }
		m_pcSimulationProgressGauge->SetValue(iProgress);
		//Update();

	}
}
void SP_DLG_FCPNSimulationResults::SetSimulationProgressGaugeRange(long p_nRangeValue)
{
	m_pcSimulationProgressGauge->SetRange(p_nRangeValue);

}
void* SP_DLG_FCPNSimulationResults::RunBasicSamplingFcpnThreadSim()
{
	m_fcpnThreadVector.clear();

	bool isIitialized = InitializeFuzzySetting();

	if (!m_fr || !isIitialized)
	{
		SP_LOGERROR(wxT("Simulation could not be started!"));
		return 0;
	}

	std::vector<double> vdTopLevelParams = GetCombinationVectorForTopLevel();


	/*clear the result fuzzy band*/
	m_vResultFBand.clear();

	/*inform the user to the  simulation running times*/
	long l_numofLevels = m_fr->GetNumFuzzyLevels();
	long l_numofSamples = m_fr->GetNumSamplePoints();
	long l_numFN = m_fr->GetNumFuzzyNum();

	/*infor the user about sim. runs*/

	long l_Runningtimes;

	if (m_fr->GetSamplingType() == fuzzification::SAMPLING_TYPE::BASIC)
	{
		l_Runningtimes = pow(l_numofSamples, l_numFN) * (l_numofLevels)+1;
	}
	else
	{
		l_Runningtimes= (l_numofSamples*l_numofLevels) + 1;
	}

	//give warning to the user
	const std::string sWarnMesag = "The total simulation number is: ";
	m_lTotalSimRuns = l_Runningtimes;
	m_lruncounter = 0;
	std::string s_warningMsg = sWarnMesag + std::to_string(l_Runningtimes);
	SP_LOGWARNING(s_warningMsg);
	wxMessageDialog* p_warningDialog = new wxMessageDialog(this, s_warningMsg, "High number of Simulation Runs", wxOK | wxCANCEL | wxICON_WARNING);

	if (p_warningDialog->ShowModal() == wxID_OK)
	{

		m_stopWatch.Start();
		m_pcStartButton->SetLabel(wxT("Abort Simulation"));
		wxIdleEvent t;
		SendIdleEvents(t);
		m_pcStartButton->SetBackgroundColour(*wxRED);

		Update();

		/*calculate alpha levels up to the number of levels default 11, 0->10*/
		m_fr->CalculateAlphaLevels();

		long lRemainingSimRunCoun = 0; long lRunCount = 0;
		for (int ialpha = 0; ialpha < m_fr->GetAlphaSet().size() - 1; ialpha++)
		{

			if (m_bIsAbort)
			{
				break;
			}
			/*do Combination for each alpha level*/
			m_fr->DoSamplePointsCombination(m_fr->GetAlphaSet()[ialpha]);

			/*number of simulation per alpha level*/
		   long m_lNumCombinedPoints ;

			if (m_fr->GetSamplingType() == fuzzification::SAMPLING_TYPE::BASIC)
			{
					m_lNumCombinedPoints = pow(m_fr->GetNumSamplePoints(), m_fr->GetNumFuzzyNum());
			}
			else
			{
					m_lNumCombinedPoints = m_fr->GetNumSamplePoints();
			}

			for (long literation = 0; literation < m_lNumCombinedPoints; literation++)
			{
				if (m_bIsAbort) {
					break;
				}


				//	bool m_bIsInitialized =
				InitializeSimulator(literation, m_fr->GetAlphaSet()[ialpha]);
				//if(m_bIsInitialized)
				m_fcpnThreadVector.push_back(new SP_DS_FcpnSimulThread(this, m_pcMainSimulator, m_fr->GetAlphaSet()[ialpha], m_vCurrentSample));


				if (m_fcpnThreadVector[lRunCount]->Create() != wxTHREAD_NO_ERROR)
				{
				}

				m_fcpnThreadVector[lRunCount]->SetPriority(WXTHREAD_MAX_PRIORITY);

				if (m_fcpnThreadVector[lRunCount]->Run())
				{

				}
				lRunCount++;
				//m_lruncounter = lRunCount;
				m_fcpnThreadVector[lRunCount - 1]->Wait();

				wxYieldIfNeeded();

				wxDELETE(m_fcpnThreadVector[lRunCount - 1]);

			}
		}

		//alpha=1
		bool bIsInitialized = InitializeSimulator(0, 1);
		if (bIsInitialized && !m_bIsAbort) {
			m_fcpnThreadVector.push_back(new SP_DS_FcpnSimulThread(this, m_pcMainSimulator, 1, m_vCurrentSample));
			if (m_fcpnThreadVector[lRunCount]->Create() != wxTHREAD_NO_ERROR)
			{
			}

			m_fcpnThreadVector[lRunCount]->SetPriority(WXTHREAD_MAX_PRIORITY);

			if (m_fcpnThreadVector[lRunCount]->Run())
			{

			}
			m_fcpnThreadVector[lRunCount]->Wait();
			//m_lruncounter = lRunCount+1;
			wxYieldIfNeeded();

			wxDELETE(m_fcpnThreadVector[lRunCount]);
		}
		if (m_bIsAbort)
		{
			m_pcSimulationProgressGauge->SetValue(0);
			m_pcSimulationProgressText->SetLabel(wxString::Format(wxT("%i %%"), 0));
			m_bIsAbort = false;
			m_vResultFBand.clear();
			m_pcMainSimulator->AbortSimulation();
			Update();
			m_stopWatch.Pause();
		}
	}

	return NULL;
}

void    SP_DLG_FCPNSimulationResults::OnSimulatorThreadEvent(SP_DS_ThreadEvent& event)
{

	switch (event.GetEventTask())
	{

	case SP_THREAD_EVENT_END_ONE_RUN://SP_THREAD_EVENT_END_SIMULATION:
	{
		long lremain;
		void* trace = event.GetUserData();
		m_lruncounter++;

		lremain = m_lTotalSimRuns - m_lruncounter;
		SetSimulationProgressText(lremain);
		SetSimulationProgressGauge(lremain);//

		TraceElement* temp = (TraceElement*)trace;
		TraceElement tresultTrace;
		tresultTrace.sample = temp->sample;
		tresultTrace.currentLevel = temp->currentLevel;
		tresultTrace.fuzzyTrace = temp->fuzzyTrace;
		m_vResultFBand.push_back(tresultTrace);
		SetSimulationStopWatch(m_stopWatch.Time());
		//}
		if (lremain == 0)
		{
			m_stopWatch.Pause();
			render_loop_on = false;
			activateRenderLoop(false);
			m_pcStartButton->SetLabel(wxT("Abort Processing"));
			SP_DS_FuzzyResult_Thread* fthreadRes = new SP_DS_FuzzyResult_Thread(this, m_vResultFBand, m_fr);
			fthreadRes->Create();
			m_info = new wxBusyInfo
			(
				wxBusyInfoFlags()
				.Parent(this)
				.Text("Finialiazing the processing, please wait.")
				.Foreground(*wxWHITE)
				.Background(*wxBLACK)
				.Transparency(4 * wxALPHA_OPAQUE / 5)
			);

			fthreadRes->Run();



		}

		break;
	}



	case SP_THREAD_EVENT_END_SIMULATION: {
		wxDELETE(m_info);
		void*	m_pcCompressedBand = event.GetUserData();
		SetCompressedBand((SP_Compressed_Fuzzy_Band*)m_pcCompressedBand);
		m_pcStartButton->SetLabel(wxT("Start Simulation"));
		m_pcStartButton->SetBackgroundColour(*wxGREEN);
		m_pcMainSimulator->AbortSimulation();
		//m_vResultFBand.clear();
		LoadResults();
		m_bIsAbort = false;
		
		m_fr->ClearData();
		break;
	}
	case 10: {
		render_loop_on = false;
		activateRenderLoop(false);
		m_stopWatch.Pause();
		m_pcStartButton->SetLabel(wxT("Start Simulation"));
		m_pcStartButton->SetBackgroundColour(*wxGREEN);
		m_pcSimulationProgressGauge->SetValue(0);
		m_bIsAbort = false;
		wxDELETE(m_info);
		//wxDELETE(m_fr);
		m_fr->ClearData();
		m_pcMainSimulator->AbortSimulation();
		RefreshExternalWindows();
		Update();

		break;

	}

	default:
		break;
	}

}


void   SP_DLG_FCPNSimulationResults::OnDlgCancel(wxCommandEvent& p_cEvent)
{
	for (auto itVthread = m_fcpnThreadVector.begin(); itVthread != m_fcpnThreadVector.end(); ++itVthread)
	{
		if (*itVthread)
		{//check thread states
			if ((*itVthread)->IsRunning())
			{
				return;
			}
		}
	}
	//to be sure, maybe the result simulation thread calculating the fuzzy bands and membership functions is working
	//button label gives us also an indication
	if (m_pcStartButton->GetLabel() != wxT("Start Simulation"))
	{

		return;
	}

	//end the dialog
	if (IsModal())
	{
		EndModal(wxID_CANCEL);
	}
	else
	{
		SetReturnCode(wxID_CANCEL);
		Show(FALSE);
		Destroy();
	}
}

wxString SP_DLG_FCPNSimulationResults::GetFuzzySettingAttribute(SP_DS_Metadata* p_pcView, const wxString& p_sAttributeName)
{
	CHECK_POINTER(p_pcView, return wxT(""));
	SP_DS_Attribute* l_pcAttribute = p_pcView->GetAttribute(p_sAttributeName);

	CHECK_POINTER(l_pcAttribute, return wxT(""));

	return l_pcAttribute->GetValueString();
}

 void  SP_DLG_FCPNSimulationResults::SaveFuzzySimulatorProperties()
{
	 long l_nAlphaLevels;
	 long l_nSamplingPoints;


	 if (m_lalphaLevels->GetValue().ToLong(&l_nAlphaLevels))
	 {
		 SP_DS_Metadata* l_pcSimProp = *(m_pcGraph->GetMetadataclass(wxT("FuzzySettings"))->GetElements()->begin());

		 SP_DS_Attribute* l_pcAttr = NULL;

		 if (l_pcSimProp != NULL) {
			 l_pcAttr = l_pcSimProp->GetAttribute(wxT("AlphaLevels"));
			 l_pcAttr->SetValueString(m_lalphaLevels->GetValue());
		 }

	 }
	 if (m_lSamplePoints->GetValue().ToLong(&l_nSamplingPoints))
	 {
		 SP_DS_Metadata* l_pcSimProp = *(m_pcGraph->GetMetadataclass(wxT("FuzzySettings"))->GetElements()->begin());

		 SP_DS_Attribute* l_pcAttr = NULL;

		 if (l_pcSimProp != NULL) {
			 l_pcAttr = l_pcSimProp->GetAttribute(wxT("SamplingPoints"));
			 l_pcAttr->SetValueString(m_lSamplePoints->GetValue());
		 }
	 }
	 long l_nselectedSimulation = m_pcSamplingchoices->GetSelection();
	 if (l_nselectedSimulation>=0)
	 {
		 wxString l_sAlgo;
		 l_sAlgo << l_nselectedSimulation;
		 SP_DS_Metadata* l_pcSimProp = *(m_pcGraph->GetMetadataclass(wxT("FuzzySettings"))->GetElements()->begin());

		 SP_DS_Attribute* l_pcAttr = NULL;

		 if (l_pcSimProp != NULL) {
			 l_pcAttr = l_pcSimProp->GetAttribute(wxT("SamplingAlgo"));
			 l_pcAttr->SetValueString(l_sAlgo);
		 }
	 }

}


 fuzzification::SAMPLING_TYPE SP_DLG_FCPNSimulationResults::GetSelectedSamplingMethod()
 {
	 if (!m_pcSamplingchoices) fuzzification::SAMPLING_TYPE::INVALID;

	 long l_nselectedSimulation = m_pcSamplingchoices->GetSelection();

	 if (l_nselectedSimulation == 0)
	 {
		 return fuzzification::SAMPLING_TYPE::BASIC;
	 }
	 else if (l_nselectedSimulation == 1)
	 {
		 return  fuzzification::SAMPLING_TYPE::REDUCED;
	 }
	 else if (l_nselectedSimulation == 2)
	 {
		 return   fuzzification::SAMPLING_TYPE::RANDOM_LHS;
	 }
	 else if (l_nselectedSimulation == 3)
	 {
		 return fuzzification::SAMPLING_TYPE::IMPROVED_LHS;
	 }
	 else if (l_nselectedSimulation == 4)
	 {
		 return fuzzification::SAMPLING_TYPE::OPTIMUM_LHS;
	 }
	 else if (l_nselectedSimulation == 5)
	 {
		 return fuzzification::SAMPLING_TYPE::GENETIC_LHS;

	 }
	 else if (l_nselectedSimulation == 6)
	 {
		 return fuzzification::SAMPLING_TYPE::MAXIMINI_LHS;
	 }
	 else
	 {
		 return fuzzification::SAMPLING_TYPE::INVALID;
	 }
 }


 void SP_DLG_FCPNSimulationResults::OnSamplingProperties(wxCommandEvent& p_cEvent)
 {

	 if (m_pcStartButton->GetLabel() != wxT("Start Simulation"))
	 {

		 return;
	 }
	 fuzzification::SAMPLING_TYPE l_typeSampling = GetSelectedSamplingMethod();

	 if (l_typeSampling ==fuzzification::SAMPLING_TYPE::BASIC ||l_typeSampling==fuzzification::SAMPLING_TYPE::REDUCED )//no proporties for basic and reduced samplings
	 {
		 return;
	 }


	 if (m_fr == nullptr)
	 {
		 InitializeFuzzySetting();
	}


	 SP_DLG_SamplingProperties* l_pcDlg;

	 l_pcDlg = new SP_DLG_SamplingProperties(this,m_fr, m_pcSamplingchoices->GetSelection());

	 l_pcDlg->ShowModal();

	 l_pcDlg->Destroy();

	 SetFocus();


	 SP_Core::Instance()->GetRootDocument()->Modify(true);

 }

 void SP_DLG_FCPNSimulationResults::OnSamplingAlgoChanged(wxCommandEvent& p_evt)
 {
	 if (m_fr == nullptr)
	 {
		 InitializeFuzzySetting();
	 }
	 else
	 {
		 m_fr->SetSamplingMethod(GetSelectedSamplingMethod());
	 }
 }

