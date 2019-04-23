
#include "SP_DLG_FspnSimResult.h"
#include "SP_DS_FspnThread.h"
#include <vector>
#include <wx/dcps.h>
#include <wx/printdlg.h>
#include <wx/filename.h>
#include <wx/textfile.h>
#include <wx/regex.h>
#include "export/SP_EPSDC.h"
#include "sp_gui/dialogs/dia_SPN/SP_DLG_StSimulationResults.h"
#include "sp_gui/dialogs/dia_ColSPN/SP_DLG_ColStMarkingOverview.h"
#include "sp_gui/dialogs/dia_ColSPN/SP_DLG_ColStFunctionOverview.h"
#include "sp_gui/dialogs/dia_SPN/SP_DLG_StDirectExportProperties.h"
#include "sp_gui/dialogs/dia_SPN/SP_DLG_StMarkingOverview.h"
#include "sp_gui/dialogs/dia_FPN/SP_DLG_FpnConstantDefinition.h"
#include "sp_ds/extensions/SP_DS_SimulatorThread.h"

//bysl
#include "sp_gui/dialogs/SP_DLG_NewConstantDefinition.h"
#include "sp_ds/attributes/SP_DS_MarkingDependentMultiplicity.h"
#include "sp_ds/attributes/SP_DS_MarkingAttribute.h"

#include "sp_gui/dialogs/dia_SPN/SP_DLG_StFunctionOverview.h"
#include "sp_gui/dialogs/dia_SPN/SP_DLG_StParameterOverview.h"
#include "sp_gui/dialogs/SP_DLG_MarkingOverview.h"
#include "sp_gui/dialogs/SP_DLG_ColListOverview.h"
#include "sp_gui/dialogs/SP_DLG_SimulationProperties.h"
#include "sp_ds/attributes/SP_DS_BoolAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_gui/windows/SP_GUI_Mainframe.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gr/SP_GR_Node.h"
#include "sp_core/SP_Core.h"
#include "sp_ds/extensions/ext_SPN/SP_DS_SimulatorThreadStochastic.h"
#include "sp_ds/extensions/SP_DS_FunctionEvaluator.h"
#include "sp_ds/extensions/SP_DS_FunctionRegistry.h"
#include "spsim/spsim.h"
#include "wx/busyinfo.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/attributes/SP_DS_TypeAttribute.h"
 
SP_DS_FspnThread::SP_DS_FspnThread(SP_DS_Graph* p_pcGraph, wxWindow* p_pcParent, wxString p_sHelpText, const wxString& p_sTitle, long p_nStyle) :wxThread(wxTHREAD_JOINABLE),
SP_DLG_StSimulationResults(p_pcGraph, p_pcParent, p_sHelpText, p_sTitle, p_nStyle)
{
	
	
	spsim::Simulator* m_pcMainSimulatorth = nullptr;
	std::vector<double> v = { 1, 2, 3, 4 };
		std::vector<std::vector<double>> paramMap(0, std::vector<double>(0));
	m_fuzzyReasoning = FuzzyReasoning(0, 0, paramMap, 0, 0);
	m_bIsAbort1 = false;

	m_fuzzyBand=(0, std::vector<TraceElement>(0));
	 m_initialRun1=false;
	 
	 m_TransParamNames[wxT("RR")] = wxT("dd"); 
	  
	  m_mFuzzyParam2Position[wxT("t")] = 10;
	m_vdCurrentSample = v;
	m_paramMatrix1 = paramMap;
     m_lTotalSimRuns = 0;
    m_isReducedSampling = false;
   m_lCurrentProgress = 0;
 
  
}
SP_DS_FspnThread::~SP_DS_FspnThread()
{}
void  SP_DS_FspnThread::SendGuiEvent(const int& p_nEventTask, const long& l_nLongValue, void* p_pcUserData)
{
	SP_DS_ThreadEvent l_eGuiEvent(spEVT_THREAD, SP_SIMULATION_THREAD_EVENT);

	l_eGuiEvent.SetEventTask(p_nEventTask);
	l_eGuiEvent.SetUserData(p_pcUserData);
	long iProgress = 0;
	if (m_lTotalSimRuns != 0) {
		iProgress = 100 - ((l_nLongValue * 100) / m_lTotalSimRuns);
	}
	l_eGuiEvent.SetLongValue(iProgress);
	wxPostEvent(this->m_parent, l_eGuiEvent);
	//add a pending event to the main gui thread
	//this->m_parent->GetEventHandler()->AddPendingEvent(l_eGuiEvent);

}
void * SP_DS_FspnThread::Entry()
{
	m_fspnstopWatch.Start();
	if (m_isReducedSampling == false)
	{
		m_fuzzyBand.clear();

		/*inform the user to the  simulation running times*/
		long l_numofLevels = m_fuzzyReasoning.GetNumFuzzyLevels();
		long l_numofSamples = m_fuzzyReasoning.GetNumSamplePoints();
		long l_numFN = m_fuzzyReasoning.GetNumFuzzyNum();
		long l_nRunningTimes = pow(l_numofLevels, l_numFN) * (l_numofLevels)+1;// pow(numofSamples, lnumFN)* numofLevels + 1;

		m_lTotalSimRuns = l_nRunningTimes;
		spsim::StochasticSimulator* l_pcSimulator;
		long lRunCount = 1;

		l_pcSimulator = dynamic_cast<spsim::StochasticSimulator*> (m_pcMainSimulatorth);
		const std::string s_msgWarnMesag = "The total number of simulation runs is: ";
		spsim::Property* l_pcPropRunCounts = l_pcSimulator->GetSimulatorOptions()->GetOption(wxT("RunCount"));
		if (l_pcPropRunCounts != NULL)
		{
			lRunCount = l_pcPropRunCounts->GetValuelong();
		}
		std::string s_msg = s_msgWarnMesag + std::to_string(l_nRunningTimes) + " x " + std::to_string(lRunCount);
		SP_LOGWARNING(s_msg);

		wxMessageDialog* pc_msgDialog = new wxMessageDialog(this, s_msg, "High number of simulation Runs", wxOK | wxCANCEL | wxICON_WARNING);

		if (pc_msgDialog->ShowModal() == wxID_OK)
		{/*D Fspn simulation*/
			m_pcStartButton->SetBackgroundColour(*wxRED);
			/*calculate alpha levels, default levels=11 ,0->10 */
			m_fuzzyReasoning.CalculateAlphaLevels();

			long lRemainingSimRunCoun = 0; long lRunCount = 0;

			for (int iAlpha = 0; iAlpha < m_fuzzyReasoning.GetAlphaSet().size() - 1; iAlpha++)
			{
				if (m_bIsAbort1)
				{
					break;
				}
				/*do sample point combination for current alpha level*/
				m_fuzzyReasoning.DoSamplePointsCombination(m_fuzzyReasoning.GetAlphaSet()[iAlpha]);

				/*calculate number of simulation at current alpha levels*/
				long m_lNumCombinedPoints = pow(m_fuzzyReasoning.GetNumSamplePoints(), m_fuzzyReasoning.GetNumFuzzyNum());
				for (long lIteration = 0; lIteration < m_lNumCombinedPoints; lIteration++)
				{
					//m_clock = std::clock(); // get current time
					if (m_bIsAbort1)
					{
						SendGuiEvent(10, 0, NULL);
						break;
					}
					/*to store current simulation trace*/
					TraceElement structTraceElement;

					spsim::Matrix2DDouble resultMat;
					double alphaValue = m_fuzzyReasoning.GetAlphaSet()[iAlpha];

					/*do simulation on current sample combination*/
					resultMat = DOneSpnSimulation(lIteration, alphaValue);

					/*show the total progress*/
					lRunCount++;
					lRemainingSimRunCoun = m_lTotalSimRuns - lRunCount;

					long lcurrentTime = m_fspnstopWatch.Time();
					// case SP_THREAD_EVENT_END_SIMULATION:SP_THREAD_EVENT_END_ONE_RUN
					SendGuiEvent(SP_THREAD_EVENT_END_ONE_RUN, lRemainingSimRunCoun,(long*) lcurrentTime);

					/*store current sim trace with its level information eg., current sample,level*/
					structTraceElement.sample = m_vdCurrentSample;
					structTraceElement.currentLevel = m_fuzzyReasoning.GetAlphaSet()[iAlpha];
					structTraceElement.levelNum = iAlpha;
					structTraceElement.fuzzyTrace = resultMat;
					m_fuzzyBand.push_back(structTraceElement);

				}
			}
			TraceElement structTraceElement;
			spsim::Matrix2DDouble vvdResultMat;
			if (!m_bIsAbort1)
			{
				
				vvdResultMat = DOneSpnSimulation(0, 1); /*Do one Simulation for the top sample*/

				/*show the total progress*/
				lRunCount++;
				lRemainingSimRunCoun = m_lTotalSimRuns - lRunCount;
				long lcurrentTime = m_fspnstopWatch.Time();
				SendGuiEvent(SP_THREAD_EVENT_END_ONE_RUN, lRemainingSimRunCoun, (long*)lcurrentTime);


				/*add the trace of the heighest level to the band*/

				structTraceElement.sample = m_vdCurrentSample;
				structTraceElement.currentLevel = 1;
				structTraceElement.levelNum = m_fuzzyReasoning.GetNumFuzzyLevels();
				structTraceElement.fuzzyTrace = vvdResultMat;
				m_fuzzyBand.push_back(structTraceElement);
			}

			if (!m_bIsAbort1) {
				//m_pcSimulationProgressGauge->SetValue(100);
				//m_pcStartButton->SetLabel(wxT("Abort Processing"));
			//	m_pcStartButton->SetBackgroundColour(*wxRED);
				wxBusyInfo info(wxT("Finalizing the processing, please wait."), this);
				m_pcCompressedBand = m_fuzzyReasoning.CompressResults(m_fuzzyBand);// m_FuzzyBand);
				SetCompressedBand(m_pcCompressedBand);
				m_fuzzyBand.clear();
				if (!m_bIsAbort1)
				{
					SendGuiEvent(SP_THREAD_EVENT_END_SIMULATION, lRemainingSimRunCoun, m_pcCompressedBand);
					m_fspnstopWatch.Pause();
				}else{
					SendGuiEvent(10, 0, NULL);
					m_fspnstopWatch.Pause();
				}
				LoadResults();
			}
		 
			m_initialRun1 = false;
			m_bIsAbort1 = false;
			m_pcMainSimulator->Initialise(false);
			m_pcMainSimulator->AbortSimulation();
		}
		else 
		{
			SendGuiEvent(10, 0, NULL);
			return 0;
		}
 
	}
	else
	{
		/*inform the user to the  simulation running times*/
		long l_numofLevels = m_fuzzyReasoning.GetNumFuzzyLevels();
		long l_numofSamples = m_fuzzyReasoning.GetNumSamplePoints();
		long l_numFN = m_fuzzyReasoning.GetNumFuzzyNum();
		long l_nRunningtimes = pow(l_numofSamples, l_numFN) + (l_numofLevels) * 2 + 1;// pow(numofSamples, lnumFN)* numofLevels + 1;

		m_lTotalSimRuns = l_nRunningtimes;
		spsim::StochasticSimulator* l_pcSimulator;
		long lRunCount = 1;

		l_pcSimulator = dynamic_cast<spsim::StochasticSimulator*> (m_pcMainSimulatorth);
		const std::string s_msgWarnMesag = "The total number of simulation runs is: ";
		spsim::Property* l_pcPropRunCounts = l_pcSimulator->GetSimulatorOptions()->GetOption(wxT("RunCount"));
		if (l_pcPropRunCounts != NULL)
		{
			lRunCount = l_pcPropRunCounts->GetValuelong();
		}
		const std::string sWarnMesag = "The total number of simulation runs is : ";
		std::string s_WarningMsg = sWarnMesag + std::to_string(l_nRunningtimes) + " x " + std::to_string(lRunCount);
		SP_LOGWARNING(s_WarningMsg);
		wxMessageDialog* p_pcDialog = new wxMessageDialog(this, s_WarningMsg, "High number of simulation runs", wxOK | wxCANCEL | wxICON_WARNING);

		/*warning the user about number of simulation runs*/
		if (p_pcDialog->ShowModal() == wxID_OK)
		{
			/*caculate alpha levels, default 11, 0->10*/
			m_fuzzyReasoning.CalculateAlphaLevels();
			long lRemainingSimRunCoun = 0; long lRunCount = 0;
			for (int iAlpha = 0; iAlpha < m_fuzzyReasoning.GetAlphaSet().size() ; iAlpha++)
			{
				if (m_bIsAbort1)
				{
					break;
				}
				/* do simulation just for the first level*/
				if (iAlpha == 0)
				{
					/*do sample point combinations for very first level*/
					m_fuzzyReasoning.DoSamplePointsCombination(m_fuzzyReasoning.GetAlphaSet()[iAlpha]);

					/*calculate number of samples at the first level*/
					long m_lNumCombinedPoints = pow(m_fuzzyReasoning.GetNumSamplePoints(), m_fuzzyReasoning.GetNumFuzzyNum());

					for (long l_iteration = 0; l_iteration < m_lNumCombinedPoints; l_iteration++)
					{
						//m_clock = std::clock(); // get current time
						if (m_bIsAbort1)
						{
							break;
						}
						/*to store the simulation trace*/
						TraceElement structTraceElement;
						spsim::Matrix2DDouble vvdResultMat;
						double alphaValue = m_fuzzyReasoning.GetAlphaSet()[iAlpha];
						/*do simulation for current sample */
						vvdResultMat = DOneSpnSimulation(l_iteration, alphaValue);
						
						/*show the total progress*/
						lRunCount++;
						lRemainingSimRunCoun = m_lTotalSimRuns - lRunCount;
						long lcurrentTime = m_fspnstopWatch.Time();
						SendGuiEvent(SP_THREAD_EVENT_END_ONE_RUN, lRemainingSimRunCoun, (long*)lcurrentTime);

						/*store the simulation trace with its level information e.g, current sample and corresponding level*/
						structTraceElement.sample = m_vdCurrentSample;
						structTraceElement.currentLevel = m_fuzzyReasoning.GetAlphaSet()[iAlpha];
						structTraceElement.levelNum = iAlpha;
						structTraceElement.fuzzyTrace = vvdResultMat;
						/*add the trace to fuzzy band*/
						m_fuzzyBand.push_back(structTraceElement);

					}
				}
				else {
					//alpha>0
					m_fuzzyReasoning.DoSamplePointsCombination(m_fuzzyReasoning.GetAlphaSet()[iAlpha]);
					long m_lNumCombinedPoints = pow(m_fuzzyReasoning.GetNumSamplePoints(), m_fuzzyReasoning.GetNumFuzzyNum());
					TraceElement traceElement1;
					spsim::Matrix2DDouble vvdResultMat;
					double currentAlpha = m_fuzzyReasoning.GetAlphaSet()[iAlpha];

					/*do one run simulation for the first point of the current level*/
					vvdResultMat = DOneSpnSimulation(0, currentAlpha);

					/*show the total progress*/
					lRunCount++;
					lRemainingSimRunCoun = m_lTotalSimRuns - lRunCount;
					long lcurrentTime = m_fspnstopWatch.Time();
					SendGuiEvent(SP_THREAD_EVENT_END_ONE_RUN, lRemainingSimRunCoun, (long*)lcurrentTime);

					traceElement1.sample = GetCurentSamples(0);
					traceElement1.currentLevel = m_fuzzyReasoning.GetAlphaSet()[iAlpha];
					traceElement1.levelNum = iAlpha;
					traceElement1.fuzzyTrace = vvdResultMat;
					m_fuzzyBand.push_back(traceElement1);

					vector<double> low = GetCurentSamples(0);
					vector<double> up = GetCurentSamples(m_lNumCombinedPoints - 1);

					/*make use of the previous traces when the curent sample is in between low and up*/
					for (long literation = 1; literation < m_lNumCombinedPoints - 1; literation++)
					{
						int count = 0;
						if (m_bIsAbort1)
						{
							break;
						}

						for (int j = 0; j < m_fuzzyReasoning.GetNumFuzzyNum(); j++)
						{
							if (m_bIsAbort1) { break; }
							if (m_fuzzyBand[literation].sample[j] > low[j] && m_fuzzyBand[literation].sample[j] < up[j])
							{
								count++;
								if (count == m_fuzzyReasoning.GetNumFuzzyNum() && m_fuzzyReasoning.GetAlphaSet()[iAlpha] != 1)
								{

									TraceElement structTraceElement2 = m_fuzzyBand[literation];
									structTraceElement2.currentLevel = m_fuzzyReasoning.GetAlphaSet()[iAlpha];;
									structTraceElement2.levelNum = iAlpha;
									m_fuzzyBand.push_back(structTraceElement2);
								}

							}

						}
					}
					/*do simulation on the last sample at the current level*/
					TraceElement structTraceElementn;
					spsim::Matrix2DDouble vvdResultMat1;
					double currentAlpha1 = m_fuzzyReasoning.GetAlphaSet()[iAlpha];
					if (m_bIsAbort1) { break; }
					/*do one run on the last point of the current level*/
					vvdResultMat1 = DOneSpnSimulation(m_lNumCombinedPoints - 1, currentAlpha1);

					/*show the total progress*/
					lRunCount++;
					lRemainingSimRunCoun = m_lTotalSimRuns - lRunCount;
					  lcurrentTime = m_fspnstopWatch.Time();
					SendGuiEvent(SP_THREAD_EVENT_END_ONE_RUN, lRemainingSimRunCoun, (long*)lcurrentTime);
					
					structTraceElementn.sample = GetCurentSamples(m_lNumCombinedPoints - 1);
					structTraceElementn.currentLevel = currentAlpha1;
					structTraceElementn.levelNum = iAlpha;
					structTraceElementn.fuzzyTrace = vvdResultMat1;
					m_fuzzyBand.push_back(structTraceElementn);
				}
				if (m_bIsAbort1)
				{
					SendGuiEvent(10, 0, NULL);
					break;
				}
			}
			/*for alpha=1*
			*initialize the simulator with zero iteration and alpha =1*/
			TraceElement structTtraceElement1;
			spsim::Matrix2DDouble resultMat;
			double currentAlpha = m_fuzzyReasoning.GetAlphaSet()[1];
			if (!m_bIsAbort1) {

				resultMat = DOneSpnSimulation(0, 1);

				/*show the total progress*/
				lRunCount++;
				lRemainingSimRunCoun = m_lTotalSimRuns - lRunCount;
				long lcurrentTime = m_fspnstopWatch.Time();
				SendGuiEvent(SP_THREAD_EVENT_END_ONE_RUN, lRemainingSimRunCoun,(long*)lcurrentTime);

				structTtraceElement1.sample = m_vdCurrentSample;
				structTtraceElement1.currentLevel = m_fuzzyReasoning.GetAlphaSet()[1];
				structTtraceElement1.levelNum = 1;
				structTtraceElement1.fuzzyTrace = resultMat;
				m_fuzzyBand.push_back(structTtraceElement1);
			}
			if (!m_bIsAbort1)
			{

				wxBusyInfo info(wxT("Finalizing the processing, please wait."), this);
				m_pcCompressedBand = m_fuzzyReasoning.CompressResults(m_fuzzyBand);// m_FuzzyBand);
				SetCompressedBand(m_pcCompressedBand);
				m_fuzzyBand.clear();
				if (!m_bIsAbort1)
				{
					SendGuiEvent(SP_THREAD_EVENT_END_SIMULATION, lRemainingSimRunCoun, m_pcCompressedBand);
					m_fspnstopWatch.Pause();
				}
				else {
					SendGuiEvent(10, 0, NULL);
					m_fspnstopWatch.Pause();
				}
				m_fuzzyBand.clear();
				m_initialRun1 = false;
				m_bIsAbort1 = false;
			}


		}
		else { 
			SendGuiEvent(10, 0, NULL);
			return NULL; 
		}
	}


	return NULL;
}


bool SP_DS_FspnThread::InitializeSimulator(unsigned long lIteration, double dAlpha)
{
	//before simulation
	if (m_initialRun1 == false) {
		if (m_pcMainSimulator->IsSimulationRunning())
		{
			m_pcMainSimulator->AbortSimulation();
			return false;
		}

		LoadSimulatorData();

		if (m_pcDirectExportRadioButton->GetValue())
		{
			m_ExportType = SP_SIM_DIRECT_EXPORT;

			OpenExportFile();
		}
		else
			if (m_pcDirectSingleExportRadioButton->GetValue())
			{
				m_ExportType = SP_SIM_DIRECT_SINGLE_EXPORT;

				OpenExportFile();
			}
			else
				if (m_pcDirectSingleExactExportRadioButton->GetValue())
				{
					m_ExportType = SP_SIM_DIRECT_SINGLE_EXACT_EXPORT;

					OpenExportFile();
				}
				else
				{
					m_ExportType = SP_SIM_NO_EXPORT;
				}

		dynamic_cast<spsim::StochasticSimulator*>(m_pcMainSimulator)->SetExportType((spsim::ExportType) (m_ExportType));

		if (m_bComAuxVarSingleRun == true)
		{
			dynamic_cast<spsim::StochasticSimulator*>(m_pcMainSimulator)->SetExportType(spsim::DIRECT_SINGLE_EXPORT);
		}

		//	Update();

	}
	//////////////////////////////////////////////////////////////////////////
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
		SP_MESSAGEBOX(wxT("The interval end value must be non-negative real number > 0. The inputed value is invalid."), wxT("Error"), wxOK | wxICON_ERROR);
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

	if (m_initialRun1 == false)
		wxBusyInfo l_Info(wxT("Initialising, please wait...."), this);

	SetSimulationProgressGaugeRange(100);


	//Parameters
	LoadParameters(lIteration, dAlpha);

	//Places
	LoadPlaces();

	//Transitions
	LoadTransitions();

	//Load Connections
	LoadConnections();

	// load observers
	LoadObservers();
	//m_pcMainSimulator->ChangeParameterValue(0, 0.06);


	return m_pcMainSimulator->Initialise(true);
}



void SP_DS_FspnThread::LoadParameters(unsigned long lIteration, double dAlpha)
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
		if (m_TransParamNames.find(l_sName) != m_TransParamNames.end())																					   //if(l_sName.Cmp("TFN")!=0 || l_sName.Cmp("BFN") != 0|| l_sName.Cmp("TZN") != 0)																						///
		{

			l_asParameterNames.push_back(l_sName);
		}
		std::vector<double> c = m_fuzzyReasoning.GetCombinationMatrix()[lIteration];
		double l_nValue = c[0];
		wxString ss;
		ss << l_nValue;


	}
	/*when alpha is exactly equals 1 then we need only one combination*/
	if (dAlpha == 1)
	{
		std::vector<double>  params = GetCombinationVectorForTopLevel();
		for (int iIt = 0; iIt < l_asParameterNames.size(); iIt++)
		{

			l_anParameterValue.push_back(params[iIt]);

			m_msParameterName2Value[l_asParameterNames[iIt]] = params[iIt];

			wxString sParam;
			sParam << params[iIt];
			m_pcGraph->GetFunctionRegistry()->registerFunction(l_asParameterNames[iIt], sParam);


		}

		m_pcMainSimulator->SetParameterNames(l_asParameterNames);
		m_pcMainSimulator->SetParameterValues(l_anParameterValue);

		return;
	}

	std::vector<double> vdCurrentParams = m_fuzzyReasoning.GetCombinationMatrix()[lIteration];
	m_vdCurrentSample.clear();
	for (int iCount = 0; iCount < l_asParameterNames.size(); iCount++)
	{
		if (m_mFuzzyParam2Position.find(l_asParameterNames[iCount]) != m_mFuzzyParam2Position.end())//if (l_asParameterNames[y] == m_fuzzyParams[y])
		{
			int pos = m_mFuzzyParam2Position[l_asParameterNames[iCount]];
			l_anParameterValue.push_back(vdCurrentParams[pos]);
			m_msParameterName2Value[l_asParameterNames[iCount]] = vdCurrentParams[pos];
			wxString sparam;
			sparam << vdCurrentParams[pos];
			m_pcGraph->GetFunctionRegistry()->registerFunction(l_asParameterNames[iCount], sparam);
			m_vdCurrentSample.push_back(vdCurrentParams[pos]);
			continue;
		}

		l_anParameterValue.push_back(vdCurrentParams[iCount]);

		m_msParameterName2Value[l_asParameterNames[iCount]] = vdCurrentParams[iCount];

		wxString sParam;
		sParam << vdCurrentParams[iCount];
		m_pcGraph->GetFunctionRegistry()->registerFunction(l_asParameterNames[iCount], sParam);


	}

	m_pcMainSimulator->SetParameterNames(l_asParameterNames);
	m_pcMainSimulator->SetParameterValues(l_anParameterValue);

}

void* SP_DS_FspnThread::DoFSPwithNormalSampling()
{
	/*reset the fuzzy setting and the result band*/
	//bool bisInitialised = InitializeFuzzySetting();
	//if (!bisInitialised)
	//{
	//	return 0;
	//}
	m_fuzzyBand.clear();

	/*inform the user to the  simulation running times*/
	long l_numofLevels = m_fuzzyReasoning.GetNumFuzzyLevels();
	long l_numofSamples = m_fuzzyReasoning.GetNumSamplePoints();
	long l_numFN = m_fuzzyReasoning.GetNumFuzzyNum();
	long l_nRunningTimes = pow(l_numofLevels, l_numFN) * (l_numofLevels)+1;// pow(numofSamples, lnumFN)* numofLevels + 1;

	m_lTotalSimRuns = l_nRunningTimes;
	/*in case of no fuzzy numbers*/
	/*
	if (l_numFN == 0)
	{
	const std::string s_msgWarnMsg = "You are trying to run standard SPN ! ";

	SP_LOGWARNING(s_msgWarnMsg);

	spsim::Matrix2DDouble n_mResultMat = DoNormalFSPN();

	TraceElement structTraceElement;
	structTraceElement.sample = m_vdCurrentSample;
	structTraceElement.currentLevel = 0;
	structTraceElement.levelNum = 0;
	structTraceElement.fuzzyTrace = n_mResultMat;
	m_ResultFBand.push_back(structTraceElement);
	m_pcCompressedBand = m_fr.CompressResults(m_ResultFBand);
	m_ResultFBand.clear();
	LoadResults();
	return 0;

	}
	*/
	spsim::StochasticSimulator* l_pcSimulator;
	long lRunCount = 1;

	l_pcSimulator = dynamic_cast<spsim::StochasticSimulator*> (m_pcMainSimulator);
	const std::string s_msgWarnMesag = "The total number of simulation runs is: ";
	spsim::Property* l_pcPropRunCounts = l_pcSimulator->GetSimulatorOptions()->GetOption(wxT("RunCount"));
	if (l_pcPropRunCounts != NULL)
	{
		lRunCount = l_pcPropRunCounts->GetValuelong();
	}
	std::string s_msg = s_msgWarnMesag + std::to_string(l_nRunningTimes) + " x " + std::to_string(lRunCount);
	SP_LOGWARNING(s_msg);

	wxMessageDialog* pc_msgDialog = new wxMessageDialog(this, s_msg, "High number of simulation Runs", wxOK | wxCANCEL | wxICON_WARNING);

	if (pc_msgDialog->ShowModal() == wxID_OK)
	{/*D Fspn simulation*/

	 /*calculate alpha levels, default levels=11 ,0->10 */
		m_fuzzyReasoning.CalculateAlphaLevels();
		//thSimThreadWorker = std::thread([&]() {
		long lRemainingSimRunCoun = 0; long lRunCount = 0;
		for (int iAlpha = 0; iAlpha < m_fuzzyReasoning.GetAlphaSet().size() - 1; iAlpha++)
		{
			if (m_bIsAbort1)
			{
				break;
			}
			/*do sample point combination for current alpha level*/
			m_fuzzyReasoning.DoSamplePointsCombination(m_fuzzyReasoning.GetAlphaSet()[iAlpha]);

			/*calculate number of simulation at current alpha levels*/
			long m_lNumCombinedPoints = pow(m_fuzzyReasoning.GetNumSamplePoints(), m_fuzzyReasoning.GetNumFuzzyNum());

			for (long lIteration = 0; lIteration < m_lNumCombinedPoints; lIteration++)
			{
				//m_clock = std::clock(); // get current time
				if (m_bIsAbort1)
				{
					break;
				}
				/*to store current simulation trace*/
				TraceElement structTraceElement;

				spsim::Matrix2DDouble resultMat;
				double alphaValue = m_fuzzyReasoning.GetAlphaSet()[iAlpha];

				/*do simulation on current sample combination*/
				resultMat = DOneSpnSimulation(lIteration, alphaValue);

				/*show the total progress*/
				lRunCount++;
				lRemainingSimRunCoun = m_lTotalSimRuns - lRunCount;
				//SetSimulationProgressText(lRemainingSimRunCoun);
				SetSimulationProgressGauge(lRemainingSimRunCoun);
				/*store current sim trace with its level information eg., current sample,level*/
				structTraceElement.sample = m_vdCurrentSample;
				structTraceElement.currentLevel = m_fuzzyReasoning.GetAlphaSet()[iAlpha];
				structTraceElement.levelNum = iAlpha;
				structTraceElement.fuzzyTrace = resultMat;
				m_fuzzyBand.push_back(structTraceElement);

			}
		}
		/*do simulation on the heighest level alpha=1*/
		TraceElement structTraceElement;
		spsim::Matrix2DDouble vvdResultMat;
		if (!m_bIsAbort1)
		{
			vvdResultMat = DOneSpnSimulation(0, 1); /*Do one Simulation for the top sample*/

													/*show the total progress*/
			lRunCount++;
			lRemainingSimRunCoun = m_lTotalSimRuns - lRunCount;
			//SetSimulationProgressText(lRemainingSimRunCoun);
			SetSimulationProgressGauge(lRemainingSimRunCoun);
			/*add the trace of the heighest level to the band*/

			structTraceElement.sample = m_vdCurrentSample;
			structTraceElement.currentLevel = 1;
			structTraceElement.levelNum = m_fuzzyReasoning.GetNumFuzzyLevels();
			structTraceElement.fuzzyTrace = vvdResultMat;
			m_fuzzyBand.push_back(structTraceElement);
		}
		//double duration = (std::clock() - m_clock) / (double)CLOCKS_PER_SEC;
		//m_pcSimulationStopWatch->SetLabel(wxString::Format(wxT("%.3f s"), duration));
		//wxWindowDisabler disableAll;
		if (!m_bIsAbort1) {
			m_pcSimulationProgressGauge->SetValue(100);
			m_pcStartButton->SetLabel(wxT("Abort Processing"));
			m_pcStartButton->SetBackgroundColour(*wxRED);
			wxBusyInfo info(wxT("Finalizing the processing, please wait."), this);
			m_pcCompressedBand = m_fuzzyReasoning.CompressResults(m_fuzzyBand);// m_FuzzyBand);
		}
		if (m_bIsAbort1)
		{
			//InitProgress();
			m_pcSimulationProgressGauge->SetValue(0);
			m_pcSimulationStopWatch->SetLabel(wxString::Format(wxT("%.3f s"), 0.0));
		}


		m_pcStartButton->SetBackgroundColour(*wxGREEN);
		m_pcStartButton->SetLabel(wxT("Start Simulation"));
		//m_lnFuzzyNum = 0;
		m_initialRun1 = false;
		m_bIsAbort1 = false;
		m_pcMainSimulator->Initialise(false);
		m_pcMainSimulator->AbortSimulation();


	}
	else {
		m_pcStartButton->SetBackgroundColour(*wxGREEN);
		m_pcStartButton->SetLabel(wxT("Start Simulation"));
		return 0;

	}

	return 0;
}


std::vector<double> SP_DS_FspnThread::GetCombinationVectorForTopLevel()
{
	std::vector<double> param_top(m_paramMatrix1.size(), 0);
	for (int i = 0; i < m_paramMatrix1.size(); i++)
		param_top[i] = m_paramMatrix1[i][1];

	return param_top;


}


std::vector<double>   SP_DS_FspnThread::GetCurentSamples(int iteration)
{

	SP_DS_Metadataclass* mc = m_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);
	SP_ListMetadata::const_iterator it;

	SP_VectorString l_asParameterNames;
	//	SP_VectorDouble l_anParameterValue;

	//	m_msParameterName2Value.clear();

	for (it = mc->GetElements()->begin(); it != mc->GetElements()->end(); ++it)
	{
		SP_DS_Metadata* l_pcConstant = *it;
		wxString l_sName = dynamic_cast<SP_DS_NameAttribute*>(l_pcConstant->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
		wxString l_sType = dynamic_cast<SP_DS_TypeAttribute*>(l_pcConstant->GetAttribute(wxT("Type")))->GetValue();///Added by G.A
		if (m_TransParamNames.find(l_sName) != m_TransParamNames.end())																										   //if(l_sName.Cmp("TFN")!=0 || l_sName.Cmp("BFN") != 0|| l_sName.Cmp("TZN") != 0)																						///
		{
			l_asParameterNames.push_back(l_sName);
		}

	}

	std::vector<double>  vdCurrentConstant;
	vdCurrentConstant.clear();
	vdCurrentConstant = m_fuzzyReasoning.GetCombinationMatrix()[iteration];
	m_vdCurrentSample.clear();
	for (int y = 0; y < l_asParameterNames.size(); y++)
	{
		if (m_mFuzzyParam2Position.find(l_asParameterNames[y]) != m_mFuzzyParam2Position.end())//if (l_asParameterNames[y] == m_fuzzyParams[y])
		{
			int ipos = m_mFuzzyParam2Position[l_asParameterNames[y]];
			m_vdCurrentSample.push_back(vdCurrentConstant[ipos]);
			continue;
		}
	}
	return m_vdCurrentSample;
}


spsim::Matrix2DDouble  SP_DS_FspnThread::DOneSpnSimulation(unsigned long iteration, double alpha)
{
	//double* aa = &alpha;
	//SendGuiEvent(10, iteration, aa);

	bool m_bIsInitialized = InitializeSimulator(iteration, m_fuzzyReasoning.GetAlphaSet()[alpha]);
	//bool m_bIsInitialized = true;
	m_initialRun1 = true;
	spsim::Matrix2DDouble vvdResultMat;
	if (m_bIsInitialized == true)
	{

		//SetSimulationProgressGauge(0);
		m_pcStartButton->SetLabel(wxT("Abort Simulation"));
		m_pcStartButton->SetBackgroundColour(*wxRED);

		spsim::StochasticSimulator* l_pcSimulator;

		//CHECK_POINTER(m_pcMainSimulator, return NULL);

		//transform this simulator into hybrid simulator
		l_pcSimulator = dynamic_cast<spsim::StochasticSimulator*> (m_pcMainSimulator);
		l_pcSimulator->Initialise(true);
		l_pcSimulator->SetSimulationRunningState(true);

		long l_nRunCount;
		long l_nThreadCount;
		spsim::Property* l_pcPropRunCounts = l_pcSimulator->GetSimulatorOptions()->GetOption(wxT("RunCount"));
		spsim::Property* l_pcPropThreadCount = l_pcSimulator->GetSimulatorOptions()->GetOption(wxT("ThreadCount"));
		if (l_pcPropThreadCount != NULL)
		{
			l_nThreadCount = l_pcPropThreadCount->GetValuelong();
		}
		if (l_pcPropRunCounts != NULL)
		{
			l_nRunCount = l_pcPropRunCounts->GetValuelong();
		}
		if (l_nRunCount >= 1 && l_nThreadCount == 1)
		{
			for (long lrun = 0; lrun < l_nRunCount; lrun++)
			{
				if (m_bIsAbort1)
				{
					SendGuiEvent(10, 0, NULL);
					break;
				}
				// run single simulation
				l_pcSimulator->SimulateSingleRun();
			}
		}
		else
		{

			if (l_pcSimulator->GetSimulatorAlgorithm() != wxT("FAU"))
			{
				//Multithreading
				l_pcSimulator->RunSimulation();
				l_pcSimulator->AbortSimulation();
			}
			else
			{//Fau
				l_pcSimulator->SimulateSingleRun();
				l_pcSimulator->AbortSimulation();
			}
		}

		//Averaging the results according to choice of multithreading
		if (l_nRunCount > 1 && l_nThreadCount == 1)
		{
			//Get the result  Matrix and average it to run counts

			vvdResultMat = l_pcSimulator->GetResultMatrix();
			long	l_nColCount = l_pcSimulator->GetPlaceCount();
			for (unsigned int l_nRow = 0; l_nRow < vvdResultMat.size(); l_nRow++)
			{
				if (m_bIsAbort1) {
					SendGuiEvent(10, 0, NULL); break;
				}
				for (unsigned int l_nCol = 0; l_nCol < l_nColCount; l_nCol++)
				{
					if (m_bIsAbort1) { SendGuiEvent(10, 0, NULL); break; }
					vvdResultMat[l_nRow][l_nCol] /= l_nRunCount;
				}
			}
		}
		else if (l_nRunCount == 1 && l_nThreadCount == 1) {
			vvdResultMat = l_pcSimulator->GetResultMatrix();
		}
		else
		{
			wxString salg = l_pcSimulator->GetSimulatorAlgorithm();
			if (l_pcSimulator->GetSimulatorAlgorithm() != wxT("FAU"))
			{
				//Multithreading
				if (!l_pcSimulator->IsSimulationRunning())
				{
					vvdResultMat = l_pcSimulator->GetResultMatrix();
					long	l_nColCount = l_pcSimulator->GetPlaceCount();
					for (unsigned int l_nRow = 0; l_nRow < vvdResultMat.size(); l_nRow++)
					{
						if (m_bIsAbort1)
						{
							SendGuiEvent(10, 0, NULL);
							break;
						}
						for (unsigned int l_nCol = 0; l_nCol < l_nColCount; l_nCol++)
						{
							if (m_bIsAbort1) { SendGuiEvent(10, 0, NULL); break; }
							vvdResultMat[l_nRow][l_nCol] /= l_nRunCount;
						}
					}
				}
			}
			else
			{
				//FAU

				//Do no averging
			}

		}



		//m_pcStartButton->SetBackgroundColour(*wxGREEN);
		//	m_pcTimer->Stop();
		//double d = GetSimulatorProgress();

		//SetSimulationProgressGauge(GetSimulatorProgress());
		//float l_nSimulationCurrentTime = l_pcSimulator->GetCurrentTime() > l_pcSimulator->GetOutputEndPoint() ? l_pcSimulator->GetOutputEndPoint() : l_pcSimulator->GetCurrentTime();
		//	m_lSimTim += l_nSimulationCurrentTime;
		//	float l_nSimulationCurrentTime = l_pcSimulator->GetCurrentTime();
		//SetSimulationStopWatch(l_nSimulationCurrentTime);

		//Update();



	}

	return vvdResultMat;
}


void  SP_DS_FspnThread::InitProgress()
{

	m_pcSimulationProgressText->SetLabel(wxString::Format(wxT("%i %%"), 0));

}
void  SP_DS_FspnThread::SetSimulationProgressGauge(long p_nValue)
{


	m_pcSimulationProgressGauge->SetValue(p_nValue);

	Update();

}