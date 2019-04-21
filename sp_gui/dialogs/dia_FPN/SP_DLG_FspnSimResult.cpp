
#include "SP_DLG_FspnSimResult.h"
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


#if !defined(__WXMSW__) && !defined(__WXPM__)
#include "sp_gui/widgets/sp_plot/plot_enl.xpm"
#include "sp_gui/widgets/sp_plot/plot_shr.xpm"
#include "sp_gui/widgets/sp_plot/plot_zin.xpm"
#include "sp_gui/widgets/sp_plot/plot_zot.xpm"
#endif
IMPLEMENT_CLASS(SP_DLG_FspnSimResult, SP_DLG_Simulation)
enum {
	SAMPLING_CHOICES_FSPN_ID2
};

SP_DLG_FspnSimResult::SP_DLG_FspnSimResult(SP_DS_Graph* p_pcGraph, wxWindow* p_pcParent, wxString p_sHelpText, const wxString& p_sTitle, long p_nStyle)
	:  SP_DLG_StSimulationResults(p_pcGraph, p_pcParent, p_sHelpText, p_sTitle, p_nStyle)

{
	 
	wxSizer* l_pcRowSizer = NULL;
	wxSizer* l_pcRowSizer1 = NULL;
	wxSizer* l_pcRowSizer2 = NULL;
	wxSizer* l_pcRowSizer3 = NULL;
	m_lSamplingStrategyselection = 0;
	m_bIsAbort = false;
	 
	m_pcSamplingchoices = new wxChoice(m_pcPropertyWindowPropertySizer, SAMPLING_CHOICES_FSPN_ID2, wxDefaultPosition, wxSize(100, -1));
	m_pcSamplingchoices->Append(wxT("Basic Sampling"));
	m_pcSamplingchoices->Append(wxT("Reduced Sampling"));
	m_pcSamplingchoices->SetSelection(m_lSamplingStrategyselection);
	l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
	l_pcRowSizer->Add(new wxStaticText(m_pcPropertyWindowPropertySizer, -1, wxT("Fuzzy Setting:")), wxSizerFlags(1).Expand().Border(wxALL, 2));
	///////////////////////////////////////
	l_pcRowSizer1 = new wxBoxSizer(wxHORIZONTAL);
	l_pcRowSizer2 = new wxBoxSizer(wxHORIZONTAL);
	l_pcRowSizer1->Add(new wxStaticText(m_pcPropertyWindowPropertySizer, -1, wxT("alpha levels")), wxSizerFlags(1).Expand().Border(wxALL, 2));
	m_alphaLevels1 = new  wxTextCtrl(m_pcPropertyWindowPropertySizer, -1, "10", wxDefaultPosition, wxDefaultSize, 0);
	l_pcRowSizer1->Add(m_alphaLevels1, wxSizerFlags(1).Expand().Border(wxALL, 2));
	/////////////////////
	l_pcRowSizer2->Add(new wxStaticText(m_pcPropertyWindowPropertySizer, -1, wxT("discretisation points")), wxSizerFlags(1).Expand().Border(wxALL, 2));

	m_pSamplePints = new  wxTextCtrl(m_pcPropertyWindowPropertySizer, -1, "10", wxDefaultPosition, wxDefaultSize, 0);


	l_pcRowSizer2->Add(m_pSamplePints, wxSizerFlags(1).Expand().Border(wxALL, 2));

	l_pcRowSizer3 = new wxBoxSizer(wxHORIZONTAL);
	l_pcRowSizer3->Add(new wxStaticText(m_pcPropertyWindowPropertySizer, -1, wxT("sampling strategy")), wxSizerFlags(1).Expand().Border(wxALL, 2));
	l_pcRowSizer3->Add(m_pcSamplingchoices, 1, wxALL, 5);

	m_pcPropertySizer->Add(l_pcRowSizer, wxSizerFlags(0).Expand().Border(wxALL, 2));
	m_pcPropertySizer->Add(l_pcRowSizer1, wxSizerFlags(0).Expand().Border(wxALL, 2));
	m_pcPropertySizer->Add(l_pcRowSizer2, wxSizerFlags(0).Expand().Border(wxALL, 2));
	m_pcPropertySizer->Add(l_pcRowSizer3, wxSizerFlags(0).Expand().Border(wxALL, 2));
	// = m_pcMainSimulator->GetPlaceCount();
	m_lnFuzzyNum = 0;
	m_initialRun = false;
	SP_DS_Nodeclass* l_pcNodeclass = m_pcGraph->GetNodeclass(SP_DS_DISCRETE_PLACE);
	m_lpCount = l_pcNodeclass->GetElements()->size();
	m_lTotalSimRuns = 0;// init total sim run for showing progress of total simulation
	m_pcCompressedBand = new SP_Compressed_Fuzzy_Band();
	//SP_DS_FspnThread* m_tt = new SP_DS_FspnThread(p_pcGraph, p_pcParent,wxT(""), p_sTitle,0);
	InitializeParamMatrix();
	SetSizerAndFit(m_pcMainSizer);


}

SP_DLG_FspnSimResult::~SP_DLG_FspnSimResult() {}

void SP_DLG_FspnSimResult::OnCloseWindow(wxCloseEvent& p_cEvent)
{
	Destroy();
	//Destroy();  // you may also do:  event.Skip();
	// since the default event handler does call Destroy(), too
	p_cEvent.Skip();

}

void   SP_DLG_FspnSimResult::OnSimulatorThreadEvent(SP_DS_ThreadEvent& event)
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

 

std::vector<TriangularFN> SP_DLG_FspnSimResult::LoadParams()
{
	//////////////////////////////////////////////////

	wxString strSelection = GetCurrentSelectedSet();
	int iPos = 0;
	m_lnFuzzyNum = 0;
	std::vector<TriangularFN>  vasTFNParams;
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
				int iSelection = m_apcComboBoxes[4]->GetCurrentSelection();
				for (unsigned int iIter = iSelection; iIter < l_pcColList->GetRowCount(); ++iIter)
				{
					if (l_sMetadataType.Cmp(wxT("TFN")) == 0)
					{
						wxString m_sVset = l_pcColList->GetCell(iIter, 0);
						SP_VectorDouble constants = GetFNConstants(l_pcColList->GetCell(iIter, 1));
						double dConstA;
						double dContB;
						double dContC;
						dConstA = constants[0];
						dContB = constants[1];
						dContC = constants[2];
						if (dConstA &&  dContB && dContC) {
							TriangularFN ttf(dConstA, dContC, dContB);
							vasTFNParams.push_back(ttf);
							m_lnFuzzyNum++;
							m_mFuzzyParam2Position[l_sMetadataName] = iPos;
							m_fuzzyParams.push_back(l_sMetadataName);
							m_fuzzyParamPositions.push_back(iPos);
						}
					}
					else if (l_sMetadataType.Cmp(wxT("double")) == 0 || l_sMetadataType.Cmp(wxT("int")) == 0)
					{
						double main;
						l_pcColList->GetCell(iIter, 1).ToDouble(&main);
						if (main)
						{
							TriangularFN ttf(main, main, main);
							vasTFNParams.push_back(ttf);
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
	return vasTFNParams;
}


bool SP_DLG_FspnSimResult::InitializeFuzzySetting()
{
	m_lnFuzzyNum = 0;
	long m_lAlphaLevels;
	long m_lnDiscPoints;
	if ((m_alphaLevels1->GetValue().ToLong(&m_lAlphaLevels)) && (m_pSamplePints->GetValue().ToLong(&m_lnDiscPoints)))
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
		SP_MESSAGEBOX(wxT("The intered values of Alpha levels or Sample points must be numerical values"));
		return false;
	}

}



SP_VectorDouble   SP_DLG_FspnSimResult::GetFNConstants(const wxString &sval)
{
	SP_VectorDouble v_dConstants;
	std::string strVal = sval;
	wxString delimiter = ",";
	bool isOk = true;
	size_t sztpos = 0;
	wxString token;
	int numConst = 0;
	while ((sztpos = strVal.find(delimiter)) != std::string::npos) {
		numConst++;
		token = strVal.substr(0, sztpos);
		double value;
		if (token.ToDouble(&value)) { v_dConstants.push_back(value); }
		strVal.erase(0, sztpos + delimiter.length());

	}
	wxString strRes = strVal;
	double dConstD;

	if (strRes.ToDouble(&dConstD)) { v_dConstants.push_back(dConstD); }


	return v_dConstants;
}


void SP_DLG_FspnSimResult::InitializeParamMatrix()
{
	unsigned long num_lparam = 0;
	SP_DS_Metadataclass* l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);
	num_lparam = l_pcMetadataclass->GetElements()->size();
	m_paramMatrix.resize(num_lparam, std::vector<double>(3, 0));

}

void SP_DLG_FspnSimResult::ConvertTFNListToParamMatrix(std::vector<TriangularFN> asTFNParams)
{
	m_paramMatrix.clear();
	TFN_List::iterator it;

	SP_DS_Metadataclass* l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);

	long lrow = l_pcMetadataclass->GetElements()->size(); int inum = 0;
	for (it = asTFNParams.begin(); it != asTFNParams.end(); ++it)
	{
		std::vector<double> vdmyvector;

		if (inum < lrow)
		{

			vdmyvector.push_back(asTFNParams[inum].getLeft());
			vdmyvector.push_back(asTFNParams[inum].getMiddle());
			vdmyvector.push_back(asTFNParams[inum].getRight());
			inum++;
		}
		m_paramMatrix.push_back(vdmyvector);
	}
}

void SP_DLG_FspnSimResult::OnStartAbortSimulation(wxCommandEvent& p_cEvent)
{
	//SP_DS_FspnThread* m_tt;
	wxString state = m_pcStartButton->GetLabel();
	if (state==wxT("Abort Simulation") || state == wxT("Abort Processing"))
	{
		wxString s_warningMsg = "You are going to halt the simulation, please confirm!";
		wxMessageDialog* pwarningSimStopDialog = new wxMessageDialog(this, s_warningMsg, "Warning", wxOK | wxCANCEL | wxICON_WARNING);
		if (pwarningSimStopDialog->ShowModal() == wxID_OK)
		{
			m_bIsAbort = true;
			m_fr.SetAbort(true);
			if (m_FspnSimulationThread != nullptr)
			{
				m_FspnSimulationThread->SetAbortSim(true);
				 
			}
		}
		else {
			//the user cancel aborting 
			return;

		}
	}
	else if (!m_bIsAbort)
	{
		int selctedStrategy = m_pcSamplingchoices->GetSelection();
		if (selctedStrategy == 1)

		{
		
			m_FspnSimulationThread = new  SP_DS_FspnThread(m_pcGraph, this, wxT(""), this->GetTitle(), 0);

			spsim::StochasticSimulator*	l_pcSimulator = dynamic_cast<spsim::StochasticSimulator*> (m_pcMainSimulator);
			m_FspnSimulationThread->SetParentWin(this);
			m_FspnSimulationThread->SetCurrentGraph(m_pcGraph);
			m_FspnSimulationThread->SetMainSimulator(l_pcSimulator);
			bool bisInitialised = InitializeFuzzySetting();
			m_FspnSimulationThread->SetReducedSampling(true);
			m_FspnSimulationThread->SetAbortSim(false);
			m_FspnSimulationThread->SetReasoning(m_fr);
			m_FspnSimulationThread->SetTransitionNames(m_mTransParamNames);
			m_FspnSimulationThread->SetParam2Pos(m_mFuzzyParam2Position);
			m_FspnSimulationThread->SetCurrentSample(m_vdCurrentSample);
			m_FspnSimulationThread->SetParamMatrix(m_paramMatrix);
			m_pcStartButton->SetLabel(wxT("Abort Simulation"));
			m_pcStartButton->SetBackgroundColour(*wxRED);
		 
			m_FspnSimulationThread->Run();
			m_FspnSimulationThread->SetPriority(WXTHREAD_MAX_PRIORITY);


		}
		else {
			 
		 
			m_FspnSimulationThread = new  SP_DS_FspnThread(m_pcGraph, this, wxT(""), this->GetTitle(), 0);
			 
			 spsim::StochasticSimulator*	l_pcSimulator = dynamic_cast<spsim::StochasticSimulator*> (m_pcMainSimulator);
			
			
			 m_FspnSimulationThread->SetParentWin(this);
			 m_FspnSimulationThread->SetCurrentGraph(m_pcGraph);
			 m_FspnSimulationThread->SetMainSimulator(l_pcSimulator);
			bool bisInitialised = InitializeFuzzySetting();
			m_FspnSimulationThread->SetReducedSampling(false);
			m_FspnSimulationThread->SetAbortSim(false);
			m_FspnSimulationThread->SetReasoning(m_fr);
			m_FspnSimulationThread->SetTransitionNames(m_mTransParamNames);
			m_FspnSimulationThread->SetParam2Pos(m_mFuzzyParam2Position);
			m_FspnSimulationThread->SetCurrentSample(m_vdCurrentSample);
			m_FspnSimulationThread->SetParamMatrix(m_paramMatrix);
			m_pcStartButton->SetLabel(wxT("Abort Simulation"));
			m_pcStartButton->SetBackgroundColour(*wxRED);
			m_FspnSimulationThread->Run();
			m_FspnSimulationThread->SetPriority(WXTHREAD_MAX_PRIORITY);
			 
		}
	}
	 
}


void SP_DLG_FspnSimResult::OnSimulatorSelect(wxCommandEvent& p_cEvent)
{
	wxDELETE(m_pcMainSimulator);

	m_pcMainSimulator = CreateSimulator(m_pcSimulatorComboBox->GetSelection());
	//l_pcSimulator = dynamic_cast<spsim::StochasticSimulator*>(m_pcMainSimulator);

}




void SP_DLG_FspnSimResult::OnModifyConstantSets(wxCommandEvent& p_cEvent)
{

	if (m_pcGraph->GetNetclass()->GetName().Contains("Fuzzy")) {
		SP_DLG_FpnConstantDefinition* l_pcDlg = new SP_DLG_FpnConstantDefinition(NULL);
		if (l_pcDlg->ShowModal() == wxID_OK)
		{
			LoadSets();
		}

		l_pcDlg->Destroy();
	}
	else {

		SP_DLG_NewConstantDefinition* l_pcDlg = new SP_DLG_NewConstantDefinition(NULL);
		if (l_pcDlg->ShowModal() == wxID_OK)
		{
			LoadSets();
		}

		l_pcDlg->Destroy();

	}
}

std::vector<double> SP_DLG_FspnSimResult:: GetCombinationVectorForTopLevel()
{
	std::vector<double> param_top(m_paramMatrix.size(), 0);
	for (int i = 0; i < m_paramMatrix.size(); i++)
		param_top[i] = m_paramMatrix[i][1];

	return param_top;


}
spsim::Matrix2DDouble SP_DLG_FspnSimResult::DoNormalFSPN()
{
	//m_nFuzzyResultBand.clear();
	m_fr.CalculateAlphaLevels();
	m_fr.DoSamplePointsCombination(m_fr.GetAlphaSet()[0]);

	//bool m_bIsInitialized = InitializeSimulator(0, m_fr.GetAlphaSet()[0]);
	spsim::Matrix2DDouble vvdresultMat;
	m_initialRun = true;
	if ( true)
	{

		SetSimulationProgressGauge(0);
		SetSimulationStopWatch(0);
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
			//single thread
			for (long i = 0; i < l_nRunCount; i++)
			{
				l_pcSimulator->SimulateSingleRun();
			}

			vvdresultMat = l_pcSimulator->GetResultMatrix();

			long	l_nColCount1 = l_pcSimulator->GetPlaceCount();

			for (unsigned int l_nRow = 0; l_nRow < vvdresultMat.size(); l_nRow++)
				for (unsigned int l_nCol = 0; l_nCol < l_nColCount1; l_nCol++)
				{
					vvdresultMat[l_nRow][l_nCol] /= l_nRunCount;
				}
		}
		else
		{
			//Multithreading
			l_pcSimulator->RunSimulation();
			l_pcSimulator->AbortSimulation();

			if (!l_pcSimulator->IsSimulationRunning())
			{
				vvdresultMat = l_pcSimulator->GetResultMatrix();
				long	l_nColCount = l_pcSimulator->GetPlaceCount();
				for (unsigned int l_nRow = 0; l_nRow < vvdresultMat.size(); l_nRow++)
					for (unsigned int l_nCol = 0; l_nCol < l_nColCount; l_nCol++)
					{
						vvdresultMat[l_nRow][l_nCol] /= l_nRunCount;
					}
			}


		}

	}

	m_pcStartButton->SetLabel(wxT("Start Simulation"));
	m_pcStartButton->SetBackgroundColour(*wxGREEN);
	m_lnFuzzyNum = 0;
	m_initialRun = false;
	m_pcTimer->Stop();
	SetSimulationProgressGauge(GetSimulatorProgress());
	SetSimulationStopWatch(m_cSimulationStopWatch.Time());
	m_pcMainSimulator->Initialise(false);
	m_pcMainSimulator->AbortSimulation();
	return vvdresultMat;

}
void SP_DLG_FspnSimResult::DirectExportToCSV()
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
	std::vector<wxString> vsPlaces;


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
			vsPlaces.push_back(l_sName);

		}

	}
	////////////////////////////
	for (int iIter = 0; iIter < mPlaces2PosMap.size(); iIter++)
	{
		wxString l_sName = vsPlaces[iIter];
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
		l_sOutput << dssd::aux::toString(m_pcMainSimulator->GetOutputSampleSize() * l_nRow);
		l_sCurrentRow = wxT("");
		for (int iCount = 0; iCount < mPlaces2PosMap.size(); iCount++)
		{
			wxString l_sName = vsPlaces[iCount];
			wxString l_sPos = mPlaces2PosMap[l_sName];
			long l_nPos;
			if (!l_sPos.ToLong(&l_nPos))
				return;
			double l_dResult = 0;
			//l_dResult = l_aanPLResults[l_nRow][l_nPos];
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

void SP_DLG_FspnSimResult::LoadUsedParams()
{

	SP_DS_Nodeclass* l_pcNodeclass;
	l_pcNodeclass = m_pcGraph->GetNodeclass(SP_DS_STOCHASTIC_TRANS);
	SP_ListNode::const_iterator l_itElem;

	SP_DS_ColListAttribute* l_pcColList;

	//	m_pcMainSimulator->SetTransitionCount(l_pcNodeclass->GetElements()->size());

	unsigned long l_nPosition = 0;

	//clear old transitions
	//m_pcMainSimulator->ClearTransitions();

	//SP_DS_FunctionRegistry* l_pcFR = m_pcGraph->GetFunctionRegistry();

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

wxString SP_DLG_FspnSimResult::GetKParameter(const wxString& sfunc)
{
	wxString m_sKparam;
	wxString sdelimiter = "(";
	size_t stpos1, stpos2 = 0;
	stpos1 = sfunc.find(sdelimiter);
	sdelimiter = ")";
	stpos2 = sfunc.find(sdelimiter);
	stpos2 = stpos2 - stpos1 - 1;
	m_sKparam = sfunc.substr(stpos1 + 1, stpos2);
	return m_sKparam;
}


void SP_DLG_FspnSimResult::SetSimulationProgressText(long p_nValue)
{
	 
		m_pcSimulationProgressText->SetLabel(wxString::Format(wxT("%i %%"), p_nValue));
		//m_pcTotalSimulationProgressGauge->SetValue(100 - ((p_nValue * 100) / m_lTotalSimRuns));
		m_pcSimulationProgressText->Refresh();
		Update();
		RefreshExternalWindows();
	 


}

void SP_DLG_FspnSimResult::SetSimulationProgressGaugeRange(long p_nRangeValue)
{
	m_pcSimulationProgressGauge->SetRange(p_nRangeValue);

}