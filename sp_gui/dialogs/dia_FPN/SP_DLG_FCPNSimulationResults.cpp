/*
* SP_DLG_CPNSimulationResults.cpp
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
//by sl
#include "sp_ds/attributes/SP_DS_DoubleMarkingAttribute.h"
#include "sp_ds/attributes/SP_DS_DoubleMarkingDependentMultiplicity.h"

#include "sp_gui/dialogs/dia_SPN/SP_DLG_StDirectExportProperties.h"
#include "sp_gui/dialogs/dia_ContinuousPN/SP_DLG_CSVExport.h"
#include "sp_gui/dialogs/dia_FPN/SP_DLG_FCPNSimulationResults.h"

#include "sp_ds/extensions/SP_DS_ThreadEvent.h"
//New simulator header file
#include "spsim/spsim.h"
#if !defined(__WXMSW__) && !defined(__WXPM__)
#include "sp_gui/widgets/sp_plot/plot_enl.xpm"
#include "sp_gui/widgets/sp_plot/plot_shr.xpm"
#include "sp_gui/widgets/sp_plot/plot_zin.xpm"
#include "sp_gui/widgets/sp_plot/plot_zot.xpm"
#endif
IMPLEMENT_CLASS(SP_DLG_FCPNSimulationResults, SP_DLG_Simulation)

enum
{
	SAMPLING_CHOICES_ID2,
	TOTAL_TIMER
};

SP_DLG_FCPNSimulationResults::SP_DLG_FCPNSimulationResults(SP_DS_Graph* p_pcGraph, wxWindow* p_pcParent, wxString p_sHelpText, wxString p_sTitle, long p_nStyle) :
	SP_DLG_CPNSimulationResults(p_pcGraph, p_pcParent, p_sHelpText, p_sTitle, p_nStyle)//, m_bIsSimulatorInitialized(false), m_nRedraw(true), m_nSimulationSemantics(0)
{

	m_pcCompressedBand = new SP_Compressed_Fuzzy_Band();

	wxSizer* l_pcRowSizer = NULL;
	wxSizer* l_pcRowSizer1 = NULL;
	wxSizer* l_pcRowSizer2 = NULL;
	wxSizer* l_pcRowSizer3 = NULL;

	m_bIsAbort = false;
	m_lSamplingStrategyselection = 0;
	m_lTotalSimRuns = 0;
	m_pcSamplingchoices = new wxChoice(m_pcPropertyWindowPropertySizer, SAMPLING_CHOICES_ID2, wxDefaultPosition, wxSize(100, -1));

	m_pcSamplingchoices->Append(wxT("Basic Sampling"));
	m_pcSamplingchoices->Append(wxT("Reduced Sampling"));
	m_pcSamplingchoices->SetSelection(m_lSamplingStrategyselection);
	l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
	l_pcRowSizer->Add(new wxStaticText(m_pcPropertyWindowPropertySizer, -1, wxT("Fuzzy Settings:")), wxSizerFlags(1).Expand().Border(wxALL, 2));
	///////////////////////////////////////
	l_pcRowSizer1 = new wxBoxSizer(wxHORIZONTAL);
	l_pcRowSizer2 = new wxBoxSizer(wxHORIZONTAL);
	l_pcRowSizer3 = new wxBoxSizer(wxHORIZONTAL);
	l_pcRowSizer1->Add(new wxStaticText(m_pcPropertyWindowPropertySizer, -1, wxT("alpha levels")), wxSizerFlags(1).Expand().Border(wxALL, 2));
	m_lalphaLevels = new  wxTextCtrl(m_pcPropertyWindowPropertySizer, -1, "10", wxDefaultPosition, wxDefaultSize, 0);
	l_pcRowSizer1->Add(m_lalphaLevels, wxSizerFlags(1).Expand().Border(wxALL, 2));
	/////////////////////
	l_pcRowSizer2->Add(new wxStaticText(m_pcPropertyWindowPropertySizer, -1, wxT("discretisation points")), wxSizerFlags(1).Expand().Border(wxALL, 2));

	m_lSamplePoints = new  wxTextCtrl(m_pcPropertyWindowPropertySizer, -1, "10", wxDefaultPosition, wxDefaultSize, 0);


	l_pcRowSizer2->Add(m_lSamplePoints, wxSizerFlags(1).Expand().Border(wxALL, 2));

	//m_samplingchoices.push_back(new wxChoice(m_pcPropertyWindowPropertySizer, SAMPLING_CHOICES_ID2, wxDefaultPosition, wxSize(100, -1)));
	l_pcRowSizer3->Add(new wxStaticText(m_pcPropertyWindowPropertySizer, -1, wxT("sampling strategy")), wxSizerFlags(1).Expand().Border(wxALL, 2));
	l_pcRowSizer3->Add(m_pcSamplingchoices, 1, wxALL, 5);

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
}

SP_DLG_FCPNSimulationResults::~SP_DLG_FCPNSimulationResults() {}

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
	if ((m_lalphaLevels->GetValue().ToLong(&m_lAlphaLevels)) && (m_lSamplePoints->GetValue().ToLong(&m_lnDiscPoints)))
	{
		if (m_lAlphaLevels > 0 && m_lnDiscPoints > 0)
		{

			LoadUsedParams();
			ConvertTFNListToParamMatrix(LoadParams());
			m_fr = FuzzyReasoning(m_lAlphaLevels, m_lnDiscPoints, m_paramMatrix, m_lnFuzzyNum, m_lpCount);
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
	int iPos = 0; m_lnFuzzyNum = 0;
	std::vector<TriangularFN>  asTFNParams;
	SP_DS_Metadataclass* l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);
	if (l_pcMetadataclass)
	{
		SP_ListMetadata::const_iterator l_itElem;
		for (l_itElem = l_pcMetadataclass->GetElements()->begin();
			l_itElem != l_pcMetadataclass->GetElements()->end(); ++l_itElem)
		{
			SP_DS_Metadata* l_pcMetadata = *l_itElem;
			wxString l_sMetadataName = dynamic_cast<SP_DS_NameAttribute*>(l_pcMetadata->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
			wxString l_sMetadataGroup = dynamic_cast<SP_DS_TextAttribute*>(l_pcMetadata->GetAttribute(wxT("Group")))->GetValue();
			SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcMetadata->GetAttribute(wxT("ValueList")));
			wxString l_sMetadataValue;
			wxString l_sMetadataType = dynamic_cast<SP_DS_TypeAttribute*>(l_pcMetadata->GetAttribute(wxT("Type")))->GetValue();

			if (m_mTransParamNames.find(l_sMetadataName) != m_mTransParamNames.end())
			{
				int x = m_apcComboBoxes[1]->GetCurrentSelection();
				for (unsigned int i = x; i < l_pcColList->GetRowCount(); ++i)
				{
					if (l_sMetadataType.Cmp(wxT("TFN")) == 0)
					{
						wxString m_sVset = l_pcColList->GetCell(i, 0);
						SP_VectorDouble constants = GetFNConstants(l_pcColList->GetCell(i, 1));
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
							m_mFuzzyParam2Position[l_sMetadataName] = iPos;
							m_fuzzyParams.push_back(l_sMetadataName);
							m_fuzzyParamPositions.push_back(iPos);
						}
					}
					else if (l_sMetadataType.Cmp(wxT("double")) == 0 || l_sMetadataType.Cmp(wxT("int")) == 0)
					{
						double main;
						l_pcColList->GetCell(i, 1).ToDouble(&main);
						if (main)
						{
							TriangularFN ttf(main, main, main);
							asTFNParams.push_back(ttf);
						}

					}

					else
					{

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

	SP_VectorString l_asParameterNames;
	SP_VectorDouble l_anParameterValue;

	m_msParameterName2Value.clear();

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
	c = m_fr.GetCombinationMatrix()[lIteration];
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
	if (m_initialRun)
	{
		wxString s_warningMsg = "You are going to halt the simulation, please confirm!";
		wxMessageDialog* pwarningSimStopDialog = new wxMessageDialog(this, s_warningMsg, "Warning", wxOK | wxCANCEL | wxICON_WARNING);
		if (pwarningSimStopDialog->ShowModal() == wxID_OK)
		{
			m_bIsAbort = true;
			m_fr.SetAbort(true);
		}
		else {
			return;

		}
	}

	if (m_pcMainSimulator->IsSimulationRunning() && m_bIsAbort)
	{
		//TDOD: rewrite this part
		m_pcMainSimulator->AbortSimulation();
		// SetSimulationProgressGauge(100);
		m_pcStartButton->SetLabel(wxT("Start Simulation"));
		m_pcStartButton->SetBackgroundColour(*wxGREEN);

		//Stop the stopwatch
		m_cSimulationStopWatch.Pause();

		Update();

		return;
	}

	//Export Type
	if (m_pcDirectExportRadioButton->GetValue())
		m_ExportType = SP_SIM_DIRECT_EXPORT;
	else
		m_ExportType = SP_SIM_NO_EXPORT;

	//  SetSimulationProgressGauge(0);
	m_pcStartButton->SetLabel(wxT("Abort Simulation"));
	m_pcStartButton->SetBackgroundColour(*wxRED);
	Update();

	if (m_initialRun == false)
	{


		long selectedSimulation = m_pcSamplingchoices->GetSelection();
		if (selectedSimulation == 1)
		{
			DoFcpnSimulation();
		}
		else
		{


			DoNormalFcpnSimulation();
			m_pcTimer->Stop();
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
	else
	{
		if (m_bIsAbort)
		{
			SP_LOGERROR(wxT("The simulation can not be initialized"));
		}
	}



	return;
}


void* SP_DLG_FCPNSimulationResults::DoNormalFcpnSimulation()
{
	/*Initialize the fuzzy setting e.g, alphalevels vector and combination matrix*/
	bool bisInitialized = InitializeFuzzySetting();

	if (!bisInitialized)

	{
		return 0;
	}
	/*clear the result fuzzy band*/
	m_vResultFBand.clear();

	/*inform the user to the  simulation running times*/
	long l_numofLevels = m_fr.GetNumFuzzyLevels();
	long l_numofSamples = m_fr.GetNumSamplePoints();
	long l_numFN = m_fr.GetNumFuzzyNum();

	/*infor the user about sim. runs*/
	long l_Runningtimes = pow(l_numofSamples, l_numFN) * (l_numofLevels)+1;//pow(numofSamples, numFN)* numofLevels+1;
	const std::string sWarnMesag = "The total simulation number is: ";
	m_lTotalSimRuns = l_Runningtimes;
	std::string s_warningMsg = sWarnMesag + std::to_string(l_Runningtimes);
	SP_LOGWARNING(s_warningMsg);
	wxMessageDialog* p_warningDialog = new wxMessageDialog(this, s_warningMsg, "High number of Simulation Runs", wxOK | wxCANCEL | wxICON_WARNING);


	if (l_numFN == 0)
	{
		/* numFN=0 means normal CPN simulation*/

		m_fr.CalculateAlphaLevels();

		m_fr.DoSamplePointsCombination(m_fr.GetAlphaSet()[0]);

		bool m_bIsInitialized = InitializeSimulator(0, 0);
		m_initialRun = true;
		if (m_bIsInitialized == true)
		{
			SetSimulationProgressGauge(0);
			m_pcStartButton->SetLabel(wxT("Abort Simulation"));
			m_pcStartButton->SetBackgroundColour(*wxRED);

			DoStartSimulation();

			spsim::Matrix2DDouble vvDresultMat;

			vvDresultMat = m_pcMainSimulator->GetResultMatrix();

			m_pcStartButton->SetLabel(wxT("Start Simulation"));
			m_pcStartButton->SetBackgroundColour(*wxGREEN);
			m_lnFuzzyNum = 0;
			m_initialRun = false;
			m_pcTimer->Stop();

			LoadResults();
			m_lSimTim = 0;
			return 0;

		}


	}
	std::thread     m_thSimThread; bool bisFinished = false;

	/* fuzzy simulation- warning the user to the number of simulations*/
	if (p_warningDialog->ShowModal() == wxID_OK)
	{
		m_pcStartButton->SetLabel(wxT("Abort Simulation"));
		m_pcStartButton->SetBackgroundColour(*wxRED);
		/*calculate alpha levels up to the number of levels default 11, 0->10*/
		m_fr.CalculateAlphaLevels();
		m_thSimThread = std::thread([&]() {
			long lRemainingSimRunCoun = 0; long lRunCount = 0;
			for (int ialpha = 0; ialpha < m_fr.GetAlphaSet().size() ; ialpha++)
			{
				m_clock = std::clock(); // get current time
				if (m_bIsAbort)
				{
					break;
				}
				/*do Combination for each alpha level*/
				m_fr.DoSamplePointsCombination(m_fr.GetAlphaSet()[ialpha]);

				/*number of simulation per alpha level*/
				long m_lNumCombinedPoints = pow(m_fr.GetNumSamplePoints(), m_fr.GetNumFuzzyNum());

				for (long literation = 0; literation < m_lNumCombinedPoints; literation++)
				{
					if (m_bIsAbort) {
						break;
					}
					TraceElement structTraceElement;
					bool m_bIsInitialized = InitializeSimulator(literation, m_fr.GetAlphaSet()[ialpha]);
					m_initialRun = true;
					if (m_bIsInitialized == true)
					{
						//SetSimulationProgressGauge(0);



						DoStartSimulation();
						lRunCount++;
						lRemainingSimRunCoun = m_lTotalSimRuns - lRunCount;
						SetSimulationProgressText(lRemainingSimRunCoun);
						SetSimulationProgressGauge(lRemainingSimRunCoun);//

						spsim::Matrix2DDouble vvDresultMat;

						vvDresultMat = m_pcMainSimulator->GetResultMatrix();

						structTraceElement.sample = m_vCurrentSample;
						structTraceElement.currentLevel = m_fr.GetAlphaSet()[ialpha];
						structTraceElement.levelNum = ialpha;
						structTraceElement.fuzzyTrace = vvDresultMat;
						m_vResultFBand.push_back(structTraceElement);


					}
				}
			}
			//alpha=1 one simulation trace
			bool m_bIsInitialized = InitializeSimulator(0, 1);
			m_initialRun = true;
			if (m_bIsInitialized == true && !m_bIsAbort)
			{

				//SetSimulationProgressGauge(0);//

				DoStartSimulation();
				lRunCount++;
				lRemainingSimRunCoun = m_lTotalSimRuns - lRunCount;
				SetSimulationProgressText(lRemainingSimRunCoun);
				SetSimulationProgressGauge(lRemainingSimRunCoun);//
				spsim::Matrix2DDouble vvdresultMat;


				vvdresultMat = m_pcMainSimulator->GetResultMatrix();


				m_fr.DoSamplePointsCombination(1);

				vector<double> vdlow = GetCurentSamples(0);


				TraceElement structTraceElement;
				structTraceElement.fuzzyTrace = vvdresultMat;
				structTraceElement.sample = vdlow;
				structTraceElement.currentLevel = 1;
				structTraceElement.levelNum = m_fr.GetNumFuzzyLevels();
				m_vResultFBand.push_back(structTraceElement);
			}
			bisFinished = true;

			double duration = (std::clock() - m_clock) / (double)CLOCKS_PER_SEC;
			m_pcSimulationStopWatch->SetLabel(wxString::Format(wxT("%.3f s"), duration));
			if (!m_bIsAbort && bisFinished)
			{
				m_pcStartButton->SetLabel(wxT("Abort Processing"));
				m_pcStartButton->SetBackgroundColour(*wxRED);
				wxBusyInfo info(wxT("Finalizing the processing, please wait."), this);
				m_pcCompressedBand = m_fr.CompressResults(m_vResultFBand);
				m_vResultFBand.clear();
				if (!m_bIsAbort)
				{
					LoadResults();
				}
				m_lSimTim = 0;
				m_initialRun = false;

			}
			if (m_bIsAbort)
			{
				InitProgress();
				m_pcSimulationProgressGauge->SetValue(0);
				m_pcSimulationStopWatch->SetLabel(wxString::Format(wxT("%.3f s"), 0.0));
				//SetSimulationProgressGauge(0);
			}

			m_bIsAbort = false;
			m_initialRun = false;
			m_pcMainSimulator->AbortSimulation();

			m_pcStartButton->SetLabel(wxT("Start Simulation"));
			m_pcStartButton->SetBackgroundColour(*wxGREEN);
		});

		//m_thSimThread.detach();

		if (m_thSimThread.joinable())
		{
			//	m_thSimThread.join();
			m_thSimThread.detach();
		}
	}

	else
	{
		m_pcStartButton->SetBackgroundColour(*wxGREEN);
		return 0;
	}



	m_lnFuzzyNum = 0;
	m_initialRun = false;

	m_bIsAbort = false;
	return 0;

}

void* SP_DLG_FCPNSimulationResults::DoFcpnSimulation()
{//with Reduced Sampling strategy

 /*initializing the fuzzy setting e.g, alpha levels and combination matrix*/

	bool bisInitialized = InitializeFuzzySetting();

	if (!bisInitialized)

	{
		return 0;
	}
	/*clear the previous result band if it was filled out*/
	m_vResultFBand.clear();

	/*inform the user to the  simulation running times*/
	long l_numofLevels = m_fr.GetNumFuzzyLevels();
	long l_numofSamples = m_fr.GetNumSamplePoints();
	long l_numFN = m_fr.GetNumFuzzyNum();

	/*calculate how many times the simulation should be run*/
	long l_nRunningtimes = pow(l_numofSamples, l_numFN) + (l_numofLevels - 1) * 2 + 1;//pow(numofSamples, numFN)* numofLevels+1;
	m_lTotalSimRuns = l_nRunningtimes;
	const std::string sWarnMesag = "The total simulation number is: ";

	std::string s_warningMsg = sWarnMesag + std::to_string(l_nRunningtimes);

	SP_LOGWARNING(s_warningMsg);// add the message to log window 

	wxMessageDialog* p_messageDialog = new wxMessageDialog(this, s_warningMsg, "High number of Simulation Runs", wxOK | wxCANCEL | wxICON_WARNING);

	if (l_numFN == 0)
	{
		/*Normal Simulation without fuzzy numbers*/
		m_fr.CalculateAlphaLevels();

		m_fr.DoSamplePointsCombination(m_fr.GetAlphaSet()[0]);

		bool m_bIsInitialized = InitializeSimulator(0, 0);
		m_initialRun = true;
		if (m_bIsInitialized == true)
		{
			TraceElement structTraceElement;

			DoStartSimulation();

			spsim::Matrix2DDouble vvdResultMat;
			vvdResultMat = m_pcMainSimulator->GetResultMatrix();
			structTraceElement.sample = m_vCurrentSample;
			structTraceElement.currentLevel = 1;
			structTraceElement.levelNum = 1;
			structTraceElement.fuzzyTrace = vvdResultMat;
			m_vResultFBand.push_back(structTraceElement);

			m_lnFuzzyNum = 0;
			m_initialRun = false;
			m_pcTimer->Stop();
			SetSimulationStopWatch(1);
			m_pcCompressedBand = m_fr.CompressResults(m_vResultFBand);
			m_vResultFBand.clear();
			LoadResults();
			m_lSimTim = 0;
			return 0;

		}


	}

	std::thread thSimThread;

	/*Notify the user about the number of simulations*/
	if (p_messageDialog->ShowModal() == wxID_OK)
	{
		m_pcStartButton->SetLabel(wxT("Abort Simulation"));
		m_pcStartButton->SetBackgroundColour(*wxRED);

		/*calculate alpha levels between 0 and 1 debending on the number of levels, default=11,0->10*/
		m_fr.CalculateAlphaLevels();

		thSimThread = std::thread([&]() {
			long lRemainingSimRunCoun = 0; long lRunCount = 0;
			for (int ialpha = 0; ialpha < m_fr.GetAlphaSet().size() ; ialpha++)
			{
				m_clock = std::clock();
				if (m_bIsAbort)
				{
					break;
				}
				/*do simulation on all sample points of the very firt level*/
				if (ialpha == 0 && !m_bIsAbort)
				{
					/*do sample point combination*/
					m_fr.DoSamplePointsCombination(m_fr.GetAlphaSet()[ialpha]);

					/*calculate the number of simulation in the current level*/
					long m_lNumCombinedPoints = pow(m_fr.GetNumSamplePoints(), m_fr.GetNumFuzzyNum());

					for (long literation = 0; literation < m_lNumCombinedPoints; literation++)
					{
						TraceElement structtraceElement;
						bool m_bIsInitialized = InitializeSimulator(literation, m_fr.GetAlphaSet()[ialpha]);
						m_initialRun = true;
						if (m_bIsInitialized == true)
						{

							//SetSimulationProgressGauge(0);//

							DoStartSimulation();

							lRunCount++;
							lRemainingSimRunCoun = m_lTotalSimRuns - lRunCount;
							SetSimulationProgressText(lRemainingSimRunCoun);
							SetSimulationProgressGauge(lRemainingSimRunCoun);
							m_pcTimer->Stop();

							spsim::Matrix2DDouble vvdResultMat;
							vvdResultMat = m_pcMainSimulator->GetResultMatrix();


							/*store the following info:1.the result trace, 2.current level, 3.the current sample point/combination */
							structtraceElement.sample = m_vCurrentSample;
							structtraceElement.currentLevel = m_fr.GetAlphaSet()[ialpha];
							structtraceElement.levelNum = ialpha;
							structtraceElement.fuzzyTrace = vvdResultMat;
							m_vResultFBand.push_back(structtraceElement);

						}
					}
				}
				else
				{
					/*when alpha larger than zero, we just need to check the previous traces with the current sample combination*/

					/*do sample point combination */
					m_fr.DoSamplePointsCombination(m_fr.GetAlphaSet()[ialpha]);
					long m_lNumCombinedPoints = pow(m_fr.GetNumSamplePoints(), m_fr.GetNumFuzzyNum());

					/*do simulation on the first and last sample combination and check those in between*/
					TraceElement structTraceElement1;
					bool m_bIsInitialized = InitializeSimulator(0, m_fr.GetAlphaSet()[ialpha]);
					m_initialRun = true;
					if (m_bIsInitialized == true && !m_bIsAbort)
					{
						//SetSimulationProgressGauge(0);//

						DoStartSimulation();
						lRunCount++;
						lRemainingSimRunCoun = m_lTotalSimRuns - lRunCount;
						SetSimulationProgressText(lRemainingSimRunCoun);
						SetSimulationProgressGauge(lRemainingSimRunCoun);
						m_pcTimer->Stop();

						spsim::Matrix2DDouble vvdResultMat;


						vvdResultMat = m_pcMainSimulator->GetResultMatrix();
						structTraceElement1.sample = GetCurentSamples(0);
						structTraceElement1.currentLevel = m_fr.GetAlphaSet()[ialpha];
						structTraceElement1.levelNum = ialpha;
						structTraceElement1.fuzzyTrace = vvdResultMat;
						m_vResultFBand.push_back(structTraceElement1);

					}

					/*obtain the first and the last sampl combination and
					compare those which lay in the previous level*/
					vector<double> vdLow = GetCurentSamples(0);
					vector<double> vdUp = GetCurentSamples(m_lNumCombinedPoints - 1);
					TraceElement traceElement;
					for (long literation = 1; literation < m_lNumCombinedPoints - 1; literation++)
					{
						if (m_bIsAbort)
						{
							break;
						}
						int icount = 0;

						for (int j = 0; j < m_fr.GetNumFuzzyNum(); j++)
						{

							if (m_vResultFBand[literation].sample[j] > vdLow[j] && m_vResultFBand[literation].sample[j] < vdLow[j])
							{
								icount++;
								if (icount == m_fr.GetNumFuzzyNum() && m_fr.GetAlphaSet()[ialpha] != 1)
								{

									TraceElement structTraceElement = m_vResultFBand[literation];
									structTraceElement.currentLevel = m_fr.GetAlphaSet()[ialpha];;
									structTraceElement.levelNum = ialpha;
									m_vResultFBand.push_back(structTraceElement);
								}

							}

						}
					}

					/*do simulation on the last time point of the current level*/
					TraceElement structTraceElementn;
					bool m_bIsInitialized1 = InitializeSimulator(m_lNumCombinedPoints - 1, m_fr.GetAlphaSet()[ialpha]);
					m_initialRun = true;
					if (m_bIsInitialized1 == true && !m_bIsAbort)
					{
						//SetSimulationProgressGauge(0);//


						DoStartSimulation();
						lRunCount++;
						lRemainingSimRunCoun = m_lTotalSimRuns - lRunCount;
						SetSimulationProgressText(lRemainingSimRunCoun);
						SetSimulationProgressGauge(lRemainingSimRunCoun);
						m_pcTimer->Stop();

						spsim::Matrix2DDouble vvdResultMat;

						vvdResultMat = m_pcMainSimulator->GetResultMatrix();

						structTraceElementn.sample = GetCurentSamples(m_lNumCombinedPoints - 1);
						structTraceElementn.currentLevel = m_fr.GetAlphaSet()[ialpha];
						structTraceElementn.levelNum = ialpha;
						structTraceElementn.fuzzyTrace = vvdResultMat;
						m_vResultFBand.push_back(structTraceElementn);

					}

				}
			}

			/*do simulation on the very last level alpha=1*/
			bool m_bIsInitialized = InitializeSimulator(0, 1);
			m_initialRun = true;
			if (m_bIsInitialized == true && !m_bIsAbort)
			{

				//SetSimulationProgressGauge(0);


				DoStartSimulation();
				lRunCount++;
				lRemainingSimRunCoun = m_lTotalSimRuns - lRunCount;
				SetSimulationProgressText(lRemainingSimRunCoun);
				SetSimulationProgressGauge(lRemainingSimRunCoun);
				m_pcTimer->Stop();
				spsim::Matrix2DDouble vvdResultMat;


				vvdResultMat = m_pcMainSimulator->GetResultMatrix();


				m_fr.DoSamplePointsCombination(1);

				vector<double> vdLow = GetCurentSamples(0);
				TraceElement structTraceElement;
				structTraceElement.fuzzyTrace = vvdResultMat;
				structTraceElement.sample = vdLow;
				structTraceElement.currentLevel = 1;
				structTraceElement.levelNum = m_fr.GetNumFuzzyLevels();
				m_vResultFBand.push_back(structTraceElement);

			}
			double duration = (std::clock() - m_clock) / (double)CLOCKS_PER_SEC;
			m_pcSimulationStopWatch->SetLabel(wxString::Format(wxT("%.3f s"), duration));
			if (!m_bIsAbort)
			{
				m_pcStartButton->SetLabel(wxT("Abort Processing"));
				m_pcStartButton->SetBackgroundColour(*wxRED);
				wxBusyInfo info(wxT("Finalizing the processing, please wait."), this);
				m_pcCompressedBand = m_fr.CompressResults(m_vResultFBand);
				m_vResultFBand.clear();
				if (!m_bIsAbort)
				{
					LoadResults();
				}
				m_lSimTim = 0;
				m_initialRun = false;

			}
			if (m_bIsAbort)
			{
				InitProgress();
				//SetSimulationProgressGauge(0);
				m_pcSimulationStopWatch->SetLabel(wxString::Format(wxT("%.3f s"), 0.0));
				m_pcSimulationProgressGauge->SetValue(0);
			}
			else {
				SetSimulationProgressGauge(100);
				SetSimulationProgressGauge(lRemainingSimRunCoun);
			}
			m_bIsAbort = false;
			m_initialRun = false;
			m_pcMainSimulator->AbortSimulation();

			m_pcStartButton->SetLabel(wxT("Start Simulation"));
			m_pcStartButton->SetBackgroundColour(*wxGREEN);
		});
		thSimThread.detach();

		if (thSimThread.joinable())
		{
			thSimThread.join();
		}
	}
	else
	{

		m_pcStartButton->SetLabel(wxT("Start Simulation"));
		m_pcStartButton->SetBackgroundColour(*wxGREEN);
		return 0;

	}

	m_lnFuzzyNum = 0;
	m_lSimTim = 0;
	m_initialRun = false;
	m_bIsAbort = false;

	return 0;
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
			wxBusyInfo l_Info(wxT("Loading data, please wait...."), this);
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
	if (m_pcMainSimulator->GetSimulatorOptions()->GetOption(wxT("InBetweenVisualization")) != NULL)
	{
		m_nRedraw = m_pcMainSimulator->GetSimulatorOptions()->GetOption(wxT("InBetweenVisualization"))->GetValueBool();
	}

	if (m_pcMainSimulator->GetSimulatorOptions()->GetOption(wxT("Refreshrate")) != NULL)
	{
		m_nRefreshRate = m_pcMainSimulator->GetSimulatorOptions()->GetOption(wxT("Refreshrate"))->GetValuelong();
	}

	//SetSimulationProgressGaugeRange(l_nLong0);//


	return l_bIsInitialized;
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
		m_mTransParamNames[m_sParamName] = l_sName;


	}

}


wxString SP_DLG_FCPNSimulationResults::GetKParameter(const wxString& func)
{
	wxString sKparam;
	wxString sdelimiter = "(";
	size_t stpos1, stpos2 = 0;
	stpos1 = func.find(sdelimiter);
	sdelimiter = ")";
	stpos2 = func.find(sdelimiter);
	stpos2 = stpos2 - stpos1 - 1;
	sKparam = func.substr(stpos1 + 1, stpos2);
	return sKparam;
}
void SP_DLG_FCPNSimulationResults::DoStartSimulation()
{

	double l_nCurrentTime = 0;

	m_nLastUpdateTime = 0;

	//m_cSimulationStopWatch.Start(0);

	//SetSimulationCurrentTime(m_pcMainSimulator->GetCurrentTime());

	//SetSimulationStopWatch(0);

	Update();

	while (l_nCurrentTime <= m_pcMainSimulator->GetOutputEndPoint() && m_pcMainSimulator->IsSimulationRunning())
	{
		//SP_LOGERROR( wxT("Simulating"));

		l_nCurrentTime = m_pcMainSimulator->RunSimulationToNextOutputPoint();

		//UpdateSimulationDialog(m_pcMainSimulator->GetGeneratedResultPointsCount());//
	}

	//Stop the stopwatch
	//m_cSimulationStopWatch.Pause();

	//SetSimulationProgressGauge(m_pcMainSimulator->GetOutputPointsCount());//

	//SetSimulationStopWatch(m_cSimulationStopWatch.Time());
	/*update simulation time*/
	//m_lSimTim += m_pcMainSimulator->GetCurrentTime();
	//m_pcMainSimulator->AbortSimulation();
	//SetSimulationStopWatch(m_lSimTim);

	float l_nSimulationCurrentTime = m_pcMainSimulator->GetCurrentTime() > m_pcMainSimulator->GetOutputEndPoint() ? m_pcMainSimulator->GetOutputEndPoint() : m_pcMainSimulator->GetCurrentTime();
	m_lSimTim += l_nSimulationCurrentTime;
	//SetSimulationStopWatch(m_lSimTim);
	//SetSimulationCurrentTime(l_nSimulationCurrentTime);

	//Load colored or auxiliary variables
	//LoadColAuxResults();//
	//Load the final result
	//LoadResults();
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
	std::vector<wxString> vsplaces;


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
			vsplaces.push_back(l_sName);

		}

	}
	////////////////////////////

	for (int iIt = 0; iIt < mPlaces2PosMap.size(); iIt++)
	{
		wxString l_sName = vsplaces[iIt];


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
	for (unsigned long l_nRow = 0; l_nRow < m_pcMainSimulator->GetGeneratedResultPointsCount(); l_nRow++)
	{
		l_sOutput = wxT("");
		l_sOutput << dssd::aux::toString(m_pcMainSimulator->GetOutputSampleSize() * l_nRow);
		l_sCurrentRow = wxT("");
		for (int iIt = 0; iIt < mPlaces2PosMap.size(); iIt++)
		{
			wxString l_sName = vsplaces[iIt];
			wxString l_sPos = mPlaces2PosMap[l_sName];
			long l_nPos;
			if (!l_sPos.ToLong(&l_nPos))
				return;
			double l_dResult = 0;
			//l_dResult = l_aanPLResults[l_nRow][l_nPos];
			for (int iIter = 0; iIter < m_nFuzzyResultBand.size(); iIter++)
			{
				SP_Vector2DDouble currentMat = m_nFuzzyResultBand[iIter];
				l_dResult = currentMat[l_nRow][l_nPos];
				//write 
				if (m_bReplaceValue == true && l_dResult <= m_nReplacedVaue)
				{
					l_dResult = 0;

				}
				l_sCurrentRow << l_sSpacer;
				l_sCurrentRow << wxString::Format(wxT("%.16g"), l_dResult);
			}

		}
		l_sOutput << l_sCurrentRow;
		*m_pcExport << l_sOutput;// << wxT("\n");
		*m_pcExport << wxT("\n");
	}


}

void SP_DLG_FCPNSimulationResults::
OnExportToCSV()
{
	wxString l_sFilename = m_sExportFilename;
	bool     l_bCompressExact = false;

	SP_DLG_CSVExport* l_pcDlg = new SP_DLG_CSVExport(SP_ST_SIM_EXPORT_CSV_EDIT_DLG_EXPLICIT, this,
		&l_sFilename, &m_nExportSpacer,
		&l_bCompressExact, m_bReplaceValue,
		m_nReplacedVaue);

	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		wxString l_sBackupFilename = m_sExportFilename;
		m_sExportFilename = l_sFilename;
		m_nReplacedVaue = l_pcDlg->GetReplacedValues();
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

	SP_VectorString l_asParameterNames;

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
	vdCurrentCombination = m_fr.GetCombinationMatrix()[iteration];
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
void SP_DLG_FCPNSimulationResults::InitProgress()
{


	m_pcSimulationProgressText->SetLabel(wxString::Format(wxT("%i %%"), 0));
}
void SP_DLG_FCPNSimulationResults::SetSimulationProgressGauge(long p_nValue)
{
	if (m_lTotalSimRuns != 0) {
		int iProgress = 100 - ((p_nValue * 100) / m_lTotalSimRuns);
		m_pcSimulationProgressGauge->SetValue(iProgress);
		Update();

	}
}
void SP_DLG_FCPNSimulationResults::SetSimulationProgressGaugeRange(long p_nRangeValue)
{
	m_pcSimulationProgressGauge->SetRange(p_nRangeValue);

}

void    SP_DLG_FCPNSimulationResults::OnSimulatorThreadEvent(SP_DS_ThreadEvent& event)
{
	long lprogress = 0;
	switch (event.GetEventTask())
	{
	case SP_THREAD_EVENT_END_ONE_RUN://SP_THREAD_EVENT_END_SIMULATION:
	{
		lprogress = event.GetLongValue();
		SetSimulationProgressGauge((long)lprogress);
		long progress = (long)lprogress;
		void* ltime = event.GetUserData();
		SetSimulationStopWatch((long)ltime);
		SetSimulationProgressText(progress);
		if (progress == 100)
		{

			m_pcStartButton->SetLabel(wxT("Abort Processing"));
		}
		LoadResults();
		break;
	}


	case SP_THREAD_EVENT_END_SIMULATION: {
		void*	m_pcCompressedBand = event.GetUserData();
		SetCompressedBand((SP_Compressed_Fuzzy_Band*)m_pcCompressedBand);
		m_pcStartButton->SetLabel(wxT("Start Simulation"));
		m_pcStartButton->SetBackgroundColour(*wxGREEN);
		break;
	}
	case 10: {

		RefreshExternalWindows();
		m_pcStartButton->SetLabel(wxT("Start Simulation"));
		m_pcStartButton->SetBackgroundColour(*wxGREEN);
		SetSimulationProgressGauge(0);
		m_bIsAbort = false;

		break;

	}

	default:
		break;
	}

}