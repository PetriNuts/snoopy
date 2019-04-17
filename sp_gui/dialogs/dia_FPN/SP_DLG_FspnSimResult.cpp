
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
	: SP_DLG_StSimulationResults(p_pcGraph, p_pcParent, p_sHelpText, p_sTitle, p_nStyle)
	
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
	m_lpCount =  l_pcNodeclass->GetElements()->size();
	m_lTotalSimRuns = 0;// init total sim run for showing progress of total simulation
	m_pcCompressedBand = new SP_Compressed_Fuzzy_Band();
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
	m_paramMatrix.resize(num_lparam, std::vector<double>(3,0));
 
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
	if (!m_bIsAbort)
	{
		int selctedStrategy = m_pcSamplingchoices->GetSelection();
		if (selctedStrategy == 1)

		{
			DoFspnSimulation();
		}
		else {
			m_cSimulationStopWatch.Start(0);
			DoFSPwithNormalSampling();
			m_cSimulationStopWatch.Pause();
		}
	}
}


void SP_DLG_FspnSimResult::OnSimulatorSelect(wxCommandEvent& p_cEvent)
{
	wxDELETE(m_pcMainSimulator);

	m_pcMainSimulator = CreateSimulator(m_pcSimulatorComboBox->GetSelection());
	//l_pcSimulator = dynamic_cast<spsim::StochasticSimulator*>(m_pcMainSimulator);
	
}


bool SP_DLG_FspnSimResult::InitializeSimulator(unsigned long lIteration,double dAlpha)
{
	//before simulation
	if (m_initialRun == false) {
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

		Update();

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

	if (m_initialRun == false)
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


void SP_DLG_FspnSimResult::LoadParameters(unsigned long lIteration,double dAlpha)
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
		if (m_mTransParamNames.find(l_sName) != m_mTransParamNames.end())																					   //if(l_sName.Cmp("TFN")!=0 || l_sName.Cmp("BFN") != 0|| l_sName.Cmp("TZN") != 0)																						///
		{
		
		l_asParameterNames.push_back(l_sName);
	    }
		std::vector<double> c = m_fr.GetCombinationMatrix() [lIteration];
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

	std::vector<double> vdCurrentParams = m_fr.GetCombinationMatrix()[lIteration];
	m_vdCurrentSample.clear();
	for (int iCount= 0; iCount < l_asParameterNames.size(); iCount++)
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
void* SP_DLG_FspnSimResult::DoFSPwithNormalSampling()
{
	/*reset the fuzzy setting and the result band*/
	bool bisInitialised =InitializeFuzzySetting();
	if (!bisInitialised)
	{
		return 0;
	}
	m_ResultFBand.clear();

	/*inform the user to the  simulation running times*/
	long l_numofLevels = m_fr.GetNumFuzzyLevels();
	long l_numofSamples = m_fr.GetNumSamplePoints();
	long l_numFN = m_fr.GetNumFuzzyNum();
	long l_nRunningTimes = pow(l_numofLevels, l_numFN) * (l_numofLevels) + 1;// pow(numofSamples, lnumFN)* numofLevels + 1;

	m_lTotalSimRuns = l_nRunningTimes;
	/*in case of no fuzzy numbers*/
	if (l_numFN == 0)
	{
		const std::string s_msgWarnMsg = "You are trying to run standard SPN ! ";

		SP_LOGWARNING(s_msgWarnMsg);

		spsim::Matrix2DDouble n_mResultMat=DoNormalFSPN();

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

	spsim::StochasticSimulator* l_pcSimulator;
	long lRunCount = 1;

	l_pcSimulator = dynamic_cast<spsim::StochasticSimulator*> (m_pcMainSimulator);
	const std::string s_msgWarnMesag = "The total number of simulation runs is: ";
	spsim::Property* l_pcPropRunCounts = l_pcSimulator->GetSimulatorOptions()->GetOption(wxT("RunCount"));
	if (l_pcPropRunCounts != NULL)
	{
		lRunCount = l_pcPropRunCounts->GetValuelong();
	}
	std::string s_msg = s_msgWarnMesag + std::to_string(l_nRunningTimes)+" x "+ std::to_string(lRunCount);
	SP_LOGWARNING(s_msg);

	wxMessageDialog* pc_msgDialog = new wxMessageDialog(this, s_msg, "High number of simulation Runs", wxOK | wxCANCEL | wxICON_WARNING);
	std::thread thSimThreadWorker;
	if (pc_msgDialog->ShowModal() == wxID_OK)
	{/*D Fspn simulation*/

		/*calculate alpha levels, default levels=11 ,0->10 */
		m_fr.CalculateAlphaLevels();
		thSimThreadWorker = std::thread([&]() {
			long lRemainingSimRunCoun = 0; long lRunCount = 0;
			for (int iAlpha = 0; iAlpha < m_fr.GetAlphaSet().size() - 1; iAlpha++)
			{
				if (m_bIsAbort)
				{
					break;
				}
				/*do sample point combination for current alpha level*/
				m_fr.DoSamplePointsCombination(m_fr.GetAlphaSet()[iAlpha]);

				/*calculate number of simulation at current alpha levels*/
				long m_lNumCombinedPoints = pow(m_fr.GetNumSamplePoints(), m_fr.GetNumFuzzyNum());

				for (long lIteration = 0; lIteration < m_lNumCombinedPoints; lIteration++)
				{
					m_clock = std::clock(); // get current time
					if(m_bIsAbort)
					{
						break;
					}
					/*to store current simulation trace*/
					TraceElement structTraceElement;

					spsim::Matrix2DDouble resultMat;
					double alphaValue = m_fr.GetAlphaSet()[iAlpha];

					/*do simulation on current sample combination*/
					resultMat = DOneSpnSimulation(lIteration, alphaValue);

					/*show the total progress*/
					lRunCount++;
					lRemainingSimRunCoun = m_lTotalSimRuns - lRunCount;
					SetSimulationProgressText(lRemainingSimRunCoun);
					SetSimulationProgressGauge(lRemainingSimRunCoun);
					/*store current sim trace with its level information eg., current sample,level*/
					structTraceElement.sample = m_vdCurrentSample;
					structTraceElement.currentLevel = m_fr.GetAlphaSet()[iAlpha];
					structTraceElement.levelNum = iAlpha;
					structTraceElement.fuzzyTrace = resultMat;
					m_ResultFBand.push_back(structTraceElement);

				}
			}
			/*do simulation on the heighest level alpha=1*/
			TraceElement structTraceElement;
			spsim::Matrix2DDouble vvdResultMat;
			if (!m_bIsAbort)
			{
				vvdResultMat = DOneSpnSimulation(0, 1); /*Do one Simulation for the top sample*/
				
				/*show the total progress*/
				lRunCount++;
				lRemainingSimRunCoun = m_lTotalSimRuns - lRunCount;
				SetSimulationProgressText(lRemainingSimRunCoun);
				SetSimulationProgressGauge(lRemainingSimRunCoun);
				/*add the trace of the heighest level to the band*/

				structTraceElement.sample = m_vdCurrentSample;
				structTraceElement.currentLevel = 1;
				structTraceElement.levelNum = m_fr.GetNumFuzzyLevels();
				structTraceElement.fuzzyTrace = vvdResultMat;
				m_ResultFBand.push_back(structTraceElement);
			}
			double duration = (std::clock() - m_clock)/(double)CLOCKS_PER_SEC;
			m_pcSimulationStopWatch->SetLabel(wxString::Format(wxT("%.3f s"), duration));
			//wxWindowDisabler disableAll;
			if (!m_bIsAbort) {
				m_pcSimulationProgressGauge->SetValue(100);
				m_pcStartButton->SetLabel(wxT("Abort Processing"));
				m_pcStartButton->SetBackgroundColour(*wxRED);
				wxBusyInfo info(wxT("Finalizing the processing, please wait."), this);
				m_pcCompressedBand = m_fr.CompressResults(m_ResultFBand);// m_FuzzyBand);
			}
			if (m_bIsAbort)
			{
				InitProgress();
				m_pcSimulationProgressGauge->SetValue(0);
				m_pcSimulationStopWatch->SetLabel(wxString::Format(wxT("%.3f s"), 0.0));
			}
			else {
				SetSimulationProgressGauge(100);
			}

			m_pcStartButton->SetBackgroundColour(*wxGREEN);
			m_pcStartButton->SetLabel(wxT("Start Simulation"));
			m_lnFuzzyNum = 0;
		 	m_initialRun = false;
			m_bIsAbort = false;
			m_pcMainSimulator->Initialise(false);
			m_pcMainSimulator->AbortSimulation();

			 
		});
		thSimThreadWorker.detach();

		if (thSimThreadWorker.joinable())
		{
			thSimThreadWorker.join();
		}
	}
	else {
		m_pcStartButton->SetBackgroundColour(*wxGREEN);
		m_pcStartButton->SetLabel(wxT("Start Simulation"));
		return 0;

	}

	return 0;
}

void* SP_DLG_FspnSimResult::DoFspnSimulation()
{
	/*Initialise fuzzy settings*/
	bool bisInitialised = InitializeFuzzySetting();
	if (!bisInitialised)
	{
		return 0;
	}
	m_ResultFBand.clear();
	 
	/*inform the user to the  simulation running times*/
	long l_numofLevels = m_fr.GetNumFuzzyLevels();
	long l_numofSamples = m_fr.GetNumSamplePoints();
	long l_numFN = m_fr.GetNumFuzzyNum();
	long l_nRunningtimes = pow(l_numofSamples, l_numFN) + (l_numofLevels) * 2 + 1;// pow(numofSamples, lnumFN)* numofLevels + 1;

	m_lTotalSimRuns = l_nRunningtimes;
	if (l_numFN == 0)
	{
		
		const std::string sWarnMsg = "You are trying to run Normal SPN ! ";
		 
		SP_LOGWARNING(sWarnMsg);

		 
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

	spsim::StochasticSimulator* l_pcSimulator;
	long lRunCount = 1;

	l_pcSimulator = dynamic_cast<spsim::StochasticSimulator*> (m_pcMainSimulator);
	 
	spsim::Property* l_pcPropRunCounts = l_pcSimulator->GetSimulatorOptions()->GetOption(wxT("RunCount"));
	if (l_pcPropRunCounts != NULL)
	{
		lRunCount = l_pcPropRunCounts->GetValuelong();
	}
	const std::string sWarnMesag = "The total number of simulation runs is : ";
	std::string s_WarningMsg = sWarnMesag + std::to_string(l_nRunningtimes)+ " x "+ std::to_string( lRunCount);
	SP_LOGWARNING(s_WarningMsg);
	wxMessageDialog* p_pcDialog = new wxMessageDialog(this, s_WarningMsg, "High number of simulation runs", wxOK| wxCANCEL | wxICON_WARNING);
	std::thread thSimThreadWorker;
	/*warning the user about number of simulation runs*/
	if (p_pcDialog->ShowModal() == wxID_OK)
	{
	
	    /*caculate alpha levels, default 11, 0->10*/
		m_fr.CalculateAlphaLevels();
		thSimThreadWorker=std::thread([&](){
		long lRemainingSimRunCoun = 0; long lRunCount = 0;
		for (int iAlpha = 0; iAlpha < m_fr.GetAlphaSet().size() - 1; iAlpha++)
		{    
			if (m_bIsAbort)
			{
				break;
			}
			/* do simulation just for the first level*/
			if (iAlpha == 0)
			{
				/*do sample point combinations for very first level*/
			m_fr.DoSamplePointsCombination(m_fr.GetAlphaSet()[iAlpha]);

			/*calculate number of samples at the first level*/
			long m_lNumCombinedPoints = pow(m_fr.GetNumSamplePoints(), m_fr.GetNumFuzzyNum());

			for (long l_iteration = 0; l_iteration < m_lNumCombinedPoints; l_iteration++)
			{
				m_clock = std::clock(); // get current time
				if (m_bIsAbort)
				{
					break;
				}
				/*to store the simulation trace*/
				TraceElement structTraceElement;
				spsim::Matrix2DDouble vvdResultMat;
				double alphaValue = m_fr.GetAlphaSet()[iAlpha];
				/*do simulation for current sample */
				vvdResultMat = DOneSpnSimulation(l_iteration, alphaValue);

				/*show the total progress*/
				lRunCount++;
				lRemainingSimRunCoun = m_lTotalSimRuns - lRunCount;
				SetSimulationProgressText(lRemainingSimRunCoun);
				SetSimulationProgressGauge(lRemainingSimRunCoun);

		        /*store the simulation trace with its level information e.g, current sample and corresponding level*/
				structTraceElement.sample = m_vdCurrentSample;
				structTraceElement.currentLevel = m_fr.GetAlphaSet()[iAlpha];
				structTraceElement.levelNum = iAlpha;
				structTraceElement.fuzzyTrace = vvdResultMat;
				/*add the trace to fuzzy band*/
				m_ResultFBand.push_back(structTraceElement);

			}
		 }
			else {
			//alpha>0
				m_fr.DoSamplePointsCombination(m_fr.GetAlphaSet()[iAlpha]);
				long m_lNumCombinedPoints = pow(m_fr.GetNumSamplePoints(), m_fr.GetNumFuzzyNum());
				TraceElement traceElement1;
				spsim::Matrix2DDouble vvdResultMat;
				double currentAlpha = m_fr.GetAlphaSet()[iAlpha];

				/*do one run simulation for the first point of the current level*/
				vvdResultMat = DOneSpnSimulation(0, currentAlpha);
	            
				/*show the total progress*/
				lRunCount++;
				lRemainingSimRunCoun = m_lTotalSimRuns - lRunCount;
				SetSimulationProgressText(lRemainingSimRunCoun);
				SetSimulationProgressGauge(lRemainingSimRunCoun);

				traceElement1.sample = GetCurentSamples(0);
				traceElement1.currentLevel = m_fr.GetAlphaSet()[iAlpha];
				traceElement1.levelNum = iAlpha;
				traceElement1.fuzzyTrace = vvdResultMat;
				m_ResultFBand.push_back(traceElement1);

				vector<double> low = GetCurentSamples(0);
				vector<double> up = GetCurentSamples(m_lNumCombinedPoints - 1);

				 /*make use of the previous traces when the curent sample is in between low and up*/
				for (long literation = 1; literation < m_lNumCombinedPoints - 1; literation++)
				{
					int count = 0;
					if (m_bIsAbort)
					{
						break;
					}

					for (int j = 0; j < m_fr.GetNumFuzzyNum(); j++)
					{
						if (m_bIsAbort) { break; }
						if (m_ResultFBand[literation].sample[j] > low[j] && m_ResultFBand[literation].sample[j] < up[j])
						{
							count++;
							if (count == m_fr.GetNumFuzzyNum() && m_fr.GetAlphaSet()[iAlpha] != 1)
							{

								TraceElement structTraceElement2 = m_ResultFBand[literation];
								structTraceElement2.currentLevel = m_fr.GetAlphaSet()[iAlpha];;
								structTraceElement2.levelNum = iAlpha;
								m_ResultFBand.push_back(structTraceElement2);
							}

						}

					}
				}

				/*do simulation on the last sample at the current level*/
				TraceElement structTraceElementn;
				spsim::Matrix2DDouble vvdResultMat1;
				double currentAlpha1 = m_fr.GetAlphaSet()[iAlpha];
				if (m_bIsAbort) { break; }
				/*do one run on the last point of the current level*/
				vvdResultMat1 = DOneSpnSimulation(m_lNumCombinedPoints - 1, currentAlpha1);
                
				/*show the total progress*/
				lRunCount++;
				lRemainingSimRunCoun = m_lTotalSimRuns - lRunCount;
				SetSimulationProgressText(lRemainingSimRunCoun);
				SetSimulationProgressGauge(lRemainingSimRunCoun);

				structTraceElementn.sample = GetCurentSamples(m_lNumCombinedPoints - 1);
				structTraceElementn.currentLevel = currentAlpha1;
				structTraceElementn.levelNum = iAlpha;
				structTraceElementn.fuzzyTrace = vvdResultMat1;
				m_ResultFBand.push_back(structTraceElementn);

			
			}

			// When fininshing the combination points of each level except the top level, calculate min and max values with their corresponding alpha level
		 
		}

		/*for alpha=1*
		*initialize the simulator with zero iteration and alpha =1*/
		TraceElement structTtraceElement1;
		spsim::Matrix2DDouble resultMat;
		double currentAlpha = m_fr.GetAlphaSet()[1];
		if (!m_bIsAbort) {

			resultMat = DOneSpnSimulation(0, 1);

			/*show the total progress*/
			lRunCount++;
			lRemainingSimRunCoun = m_lTotalSimRuns - lRunCount;
			SetSimulationProgressText(lRemainingSimRunCoun);
			SetSimulationProgressGauge(lRemainingSimRunCoun);

			structTtraceElement1.sample = m_vdCurrentSample;
			structTtraceElement1.currentLevel = m_fr.GetAlphaSet()[1];
			structTtraceElement1.levelNum = 1;
			structTtraceElement1.fuzzyTrace = resultMat;
			m_ResultFBand.push_back(structTtraceElement1);
		}
		double duration = (std::clock() - m_clock / (double)CLOCKS_PER_SEC);
		m_pcSimulationStopWatch->SetLabel(wxString::Format(wxT("%.3f s"), duration));
		if (!m_bIsAbort)
		{
			lRemainingSimRunCoun = 0;
			SetSimulationProgressText(lRemainingSimRunCoun);
			SetSimulationProgressGauge(lRemainingSimRunCoun);
			m_pcStartButton->SetLabel(wxT("Abort Processing"));
			m_pcStartButton->SetBackgroundColour(*wxRED);
			wxBusyInfo info(wxT("Finalizing the processing, please wait."), this);
			m_pcCompressedBand = m_fr.CompressResults(m_ResultFBand);// m_FuzzyBand);

			m_ResultFBand.clear();

		}
		m_lnFuzzyNum = 0;
		m_initialRun = false;
		m_pcTimer->Stop();
		//SetSimulationProgressGauge(GetSimulatorProgress());
		SetSimulationStopWatch(1);
		if (!m_bIsAbort)
		{
			LoadResults();
		}
		m_pcMainSimulator->Initialise(false);
		m_pcMainSimulator->AbortSimulation();
		if (m_bIsAbort)
		{
			InitProgress();
			m_pcSimulationProgressGauge->SetValue(0);
			m_pcSimulationStopWatch->SetLabel(wxString::Format(wxT("%.3f s"), 0));
			m_pcStartButton->SetLabel(wxT("Start Simulation"));
			m_pcStartButton->SetBackgroundColour(*wxGREEN);
		}
		else {
			lRemainingSimRunCoun = 0;
			SetSimulationProgressText(lRemainingSimRunCoun);
			SetSimulationProgressGauge(lRemainingSimRunCoun);
			m_pcStartButton->SetLabel(wxT("Start Simulation"));
			m_pcStartButton->SetBackgroundColour(*wxGREEN);
		}
		m_bIsAbort = false;
		m_initialRun = false;
		m_pcMainSimulator->AbortSimulation();
		//Update();
		});
		thSimThreadWorker.detach();

		if (thSimThreadWorker.joinable())
		{
			thSimThreadWorker.join();
		}
	}
	else {
		m_pcStartButton->SetBackgroundColour(*wxGREEN);
		m_pcStartButton->SetLabel(wxT("Start Simulation"));
		return 0;
	}
	m_bIsAbort = false;
	m_initialRun = false;
	m_pcMainSimulator->AbortSimulation();
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

std::vector<double> SP_DLG_FspnSimResult::GetCombinationVectorForTopLevel()
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

	bool m_bIsInitialized = InitializeSimulator(0, m_fr.GetAlphaSet()[0]);
	spsim::Matrix2DDouble vvdresultMat;
	m_initialRun = true;
	if (m_bIsInitialized == true)
	{

		SetSimulationProgressGauge(0);
		SetSimulationStopWatch( 0);
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
std::vector<double>  SP_DLG_FspnSimResult::GetCurentSamples(int iteration)
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
		if (m_mTransParamNames.find(l_sName) != m_mTransParamNames.end())																										   //if(l_sName.Cmp("TFN")!=0 || l_sName.Cmp("BFN") != 0|| l_sName.Cmp("TZN") != 0)																						///
		{
			l_asParameterNames.push_back(l_sName);
		}

	}

	std::vector<double>  vdCurrentConstant;
	vdCurrentConstant.clear();
	vdCurrentConstant = m_fr.GetCombinationMatrix()[iteration];
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
spsim::Matrix2DDouble SP_DLG_FspnSimResult::DOneSpnSimulation(unsigned long iteration, double alpha)
{
	bool m_bIsInitialized = InitializeSimulator(iteration, m_fr.GetAlphaSet()[alpha]);
	m_initialRun = true;
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
				if (m_bIsAbort)
				{
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
		
		/*Averaging the results according to choice of multithreading*/
		if (l_nRunCount > 1 && l_nThreadCount == 1)
		{
			//Get the result  Matrix and average it to run counts

			vvdResultMat = l_pcSimulator->GetResultMatrix();
			long	l_nColCount = l_pcSimulator->GetPlaceCount();
			for (unsigned int l_nRow = 0; l_nRow < vvdResultMat.size(); l_nRow++)
			{
				if (m_bIsAbort) { break; }
				for (unsigned int l_nCol = 0; l_nCol < l_nColCount; l_nCol++)
				{
					if (m_bIsAbort) break;
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
						if (m_bIsAbort)
						{ break; }
						for (unsigned int l_nCol = 0; l_nCol < l_nColCount; l_nCol++)
						{
							if (m_bIsAbort) { break; }
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
		float l_nSimulationCurrentTime = l_pcSimulator->GetCurrentTime();
		SetSimulationStopWatch(l_nSimulationCurrentTime);

		Update();

		 
	}

	return vvdResultMat;
}

void SP_DLG_FspnSimResult::SetSimulationProgressText(long& p_nValue)
{
	if (m_lTotalSimRuns != 0)
	{
		m_pcSimulationProgressText->SetLabel(wxString::Format(wxT("%i %%"), 100 - ((p_nValue * 100) / m_lTotalSimRuns)));
		//m_pcTotalSimulationProgressGauge->SetValue(100 - ((p_nValue * 100) / m_lTotalSimRuns));

	}


}
void SP_DLG_FspnSimResult::InitProgress()
{

	m_pcSimulationProgressText->SetLabel(wxString::Format(wxT("%i %%"),0));

}
 void SP_DLG_FspnSimResult::SetSimulationProgressGauge(long p_nValue)
{

	 if (m_lTotalSimRuns != 0) {
		 int iProgress = 100 - ((p_nValue * 100) / m_lTotalSimRuns);
		 m_pcSimulationProgressGauge->SetValue(iProgress);

		 Update();
	 }
}
void SP_DLG_FspnSimResult:: SetSimulationProgressGaugeRange(long p_nRangeValue)
{
	m_pcSimulationProgressGauge->SetRange(p_nRangeValue);

}