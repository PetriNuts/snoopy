/*
* SP_DLG_HybridSimulationResults.cpp
* $Author: G.Assaf
* $Version: 0.0 $
* $Revision: 0.0 $
* $Date:17.2.2019
* Short Description:
*/
//==============================================================================
#include <wx/wx.h>
#include <wx/statline.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/valgen.h>
#include <wx/gbsizer.h>
#include <math.h>
#include <wx/ffile.h>
#include <wx/busyinfo.h>
#include "sp_core/base/SP_Error.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/attributes/SP_DS_TypeAttribute.h"
#include "sp_gui/dialogs/dia_FPN/SP_DLG_FHybridSimultionResults.h"
#include "sp_gui/dialogs/dia_HybridPN/SP_DLG_HybridSimultionResults.h"

#include "sp_gui/dialogs/SP_DLG_MarkingOverview.h"
#include "sp_gui/dialogs/dia_SPN/SP_DLG_StFunctionOverview.h"
#include "sp_gui/dialogs/dia_SPN/SP_DLG_StParameterOverview.h"
#include "sp_gui/dialogs/SP_DLG_ColListOverview.h"
#include "sp_gui/dialogs/SP_DLG_SimulationProperties.h"

#include "sp_ds/attributes/SP_DS_NumberAttribute.h"

#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/SP_DS_Node.h"
#include "sp_gr/SP_GR_Node.h"
#include "sp_core/SP_Core.h"

#include "sp_ds/attributes/SP_DS_BoolAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"

#include "sp_ds/attributes/SP_DS_DoubleMarkingAttribute.h"
#include "sp_ds/attributes/SP_DS_DoubleMarkingDependentMultiplicity.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"
#include "sp_gui/dialogs/SP_DLG_NewConstantDefinition.h"
#include "sp_ds/attributes/SP_DS_MarkingAttribute.h"

#include "sp_ds/extensions/HybridPN/SP_DS_SimulatorThreadHybrid.h"

#include "spsim/spsim.h"
#include "spsim/helpers/property.h"
#include "spsim/helpers/simulationOptions.h"

IMPLEMENT_CLASS(SP_DLG_FHybridSimulationResults, SP_DLG_Simulation)
enum
{
	SAMPLING_CHOICES_ID2

};
SP_DLG_FHybridSimulationResults::SP_DLG_FHybridSimulationResults(SP_DS_Graph* p_pcGraph, wxWindow* p_pcParent, wxString p_sHelpText, wxString p_sTitle, long p_nStyle) :
	SP_DLG_HybridSimulationResults(p_pcGraph, p_pcParent, p_sHelpText, p_sTitle, p_nStyle)
 
{
	wxSizer* l_pcRowSizer = NULL;
	wxSizer* l_pcRowSizer1 = NULL;
	wxSizer* l_pcRowSizer2 = NULL;
	wxSizer* l_pcRowSizer3 = NULL;
	m_pcCompressedBand = new SP_Compressed_Fuzzy_Band();
	m_samplingStrategyselection = 0;
	m_bIsAbort = false;
	m_lTotalSimRuns = 0;
	m_samplingchoices = new wxChoice(m_pcPropertyWindowPropertySizer, SAMPLING_CHOICES_ID2, wxDefaultPosition, wxSize(100, -1));

	//	m_samplingchoices->Clear();
	m_samplingchoices->Append(wxT("Basic Sampling"));
	m_samplingchoices->Append(wxT("Reduced Sampling"));
	m_samplingchoices->SetSelection(m_samplingStrategyselection);

	l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
	l_pcRowSizer->Add(new wxStaticText(m_pcPropertyWindowPropertySizer, -1, wxT("Fuzzy Setting:")), wxSizerFlags(1).Expand().Border(wxALL, 2));

	///////////////////////////////////////
	l_pcRowSizer1 = new wxBoxSizer(wxHORIZONTAL);
	l_pcRowSizer2 = new wxBoxSizer(wxHORIZONTAL);
	l_pcRowSizer1->Add(new wxStaticText(m_pcPropertyWindowPropertySizer, -1, wxT("alpha levels")), wxSizerFlags(1).Expand().Border(wxALL, 2));
	m_lalphaLevels = new  wxTextCtrl(m_pcPropertyWindowPropertySizer, -1, "10", wxDefaultPosition, wxDefaultSize, 0);
	l_pcRowSizer1->Add(m_lalphaLevels, wxSizerFlags(1).Expand().Border(wxALL, 2));
	/////////////////////
	l_pcRowSizer2->Add(new wxStaticText(m_pcPropertyWindowPropertySizer, -1, wxT("discretisation points")), wxSizerFlags(1).Expand().Border(wxALL, 2));

	m_lSamplePoints = new  wxTextCtrl(m_pcPropertyWindowPropertySizer, -1, "10", wxDefaultPosition, wxDefaultSize, 0);


	l_pcRowSizer2->Add(m_lSamplePoints, wxSizerFlags(1).Expand().Border(wxALL, 2));

	l_pcRowSizer3 = new wxBoxSizer(wxHORIZONTAL);
	l_pcRowSizer3->Add(new wxStaticText(m_pcPropertyWindowPropertySizer, -1, wxT("sampling strategy")), wxSizerFlags(1).Expand().Border(wxALL, 2));
	l_pcRowSizer3->Add(m_samplingchoices, 1, wxALL, 5);

	m_samplingchoices->SetSelection(m_samplingStrategyselection);
	 
	m_pcPropertySizer->Add(l_pcRowSizer, wxSizerFlags(0).Expand().Border(wxALL, 2));
	m_pcPropertySizer->Add(l_pcRowSizer1, wxSizerFlags(0).Expand().Border(wxALL, 2));
	m_pcPropertySizer->Add(l_pcRowSizer2, wxSizerFlags(0).Expand().Border(wxALL, 2));
	m_pcPropertySizer->Add(l_pcRowSizer3, wxSizerFlags(0).Expand().Border(wxALL, 2));
	SP_DS_Nodeclass* l_pcNodeclass = m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_PLACE);
	m_lpCount = l_pcNodeclass->GetElements()->size();
	l_pcNodeclass= m_pcGraph->GetNodeclass(SP_DS_DISCRETE_PLACE);
	m_lpCount+= l_pcNodeclass->GetElements()->size();
	m_binitialRun = false;
	m_lnFuzzyNum = 0;
	//At the end call this function for alignment
	SetSizerAndFit(m_pcMainSizer);
	m_bIsSimulatorInitialized = false;
	Layout();
}
SP_DLG_FHybridSimulationResults::~SP_DLG_FHybridSimulationResults() {}

void SP_DLG_FHybridSimulationResults::InitializeParamMatrix()
{
	unsigned long num_lparam = 0;
	SP_DS_Metadataclass* l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);
	num_lparam = l_pcMetadataclass->GetElements()->size();
	m_paramMatrix.resize(num_lparam, std::vector<double>(3, 0));

}

std::vector<TriangularFN> SP_DLG_FHybridSimulationResults::LoadParams()
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
				int iSelection = m_apcComboBoxes[0]->GetCurrentSelection();
				for (unsigned int i = iSelection; i < l_pcColList->GetRowCount(); ++i)
				{
					if (l_sMetadataType.Cmp(wxT("TFN")) == 0)
					{
						wxString m_sVset = l_pcColList->GetCell(i, 0);
						SP_VectorDouble vdconstants = GetFNConstants(l_pcColList->GetCell(i, 1));
						double dConstA;
						double dConstB;
						double dConstC;
						dConstA = vdconstants[0];
						dConstB = vdconstants[1];
						dConstC = vdconstants[2];
						if (dConstA&& dConstA &&dConstC) {
							TriangularFN ttf(dConstA, dConstC, dConstB);
							asTFNParams.push_back(ttf);
							m_mFuzzyParam2Position[l_sMetadataName] = iPos;
							m_lnFuzzyNum++;
						}
					}
					else if (l_sMetadataType.Cmp(wxT("double")) == 0 || l_sMetadataType.Cmp(wxT("int")) == 0)
					{
						double dmain;
						l_pcColList->GetCell(i, 1).ToDouble(&dmain);
						if (dmain)
						{
							TriangularFN ttf(dmain, dmain, dmain);
							asTFNParams.push_back(ttf);
						}

					}

					else
					{

					}
				}
			}
		}
	}
	return asTFNParams;
}

SP_VectorDouble   SP_DLG_FHybridSimulationResults::GetFNConstants(const wxString &val)
{
	SP_VectorDouble v_dConstants;
	std::string strVal = val;
	wxString delimiter = ",";
	bool isOk = true;
	size_t stpos = 0;
	wxString token;
	int numConst = 0;
	while ((stpos = strVal.find(delimiter)) != std::string::npos) {
		numConst++;
		token = strVal.substr(0, stpos);
		double dvalue;
		if (token.ToDouble(&dvalue)) { v_dConstants.push_back(dvalue); }
		strVal.erase(0, stpos + delimiter.length());

	}
	wxString strDConst = strVal;
	double dval;

	if (strDConst.ToDouble(&dval)) { v_dConstants.push_back(dval); }


	return v_dConstants;
}

void SP_DLG_FHybridSimulationResults::ConvertTFNListToParamMatrix(std::vector<TriangularFN> asTFNParams)
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

bool SP_DLG_FHybridSimulationResults::InitializeFuzzySetting()
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
		SP_MESSAGEBOX(wxT("The intered values of Alpha levels or Sample points must be numerical values"));
		return false;
	}


}



void SP_DLG_FHybridSimulationResults::LoadUsedParams()
{
	m_mTransParamNames.clear();
	SP_DS_Nodeclass* l_pcNodeclass;
	l_pcNodeclass = m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_TRANS);
	SP_ListNode::const_iterator l_itElem;

	SP_DS_ColListAttribute* l_pcColList;
	unsigned long m_lTransCount = l_pcNodeclass->GetElements()->size();
	//l_pcNodeclass = m_pcGraph->GetNodeclass(SP_DS_STOCHASTIC_TRANS);

	//m_lTransCount += l_pcNodeclass->GetElements()->size();

	//m_pcMainSimulator->SetTransitionCount(m_lTransCount);

	unsigned long l_nPosition = 0;

	//clear old transitions
	//m_pcMainSimulator->ClearTransitions();

	//SP_DS_FunctionRegistry* l_pcFR = m_pcGraph->GetFunctionRegistry();

	//Go through all the continuous transition nodes
	for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); l_itElem++)
	{
		wxString l_sName = dynamic_cast<SP_DS_NameAttribute*>((*l_itElem)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		//Get the transition rate function
		l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>((*l_itElem)->GetAttribute(wxT("FunctionList")));

		const wxString l_sTransitionFunction = l_pcColList->GetActiveCellValue(1);

		wxString m_sParamName = GetKParameter(l_sTransitionFunction);
		if (m_sParamName != wxT("1"))
		{
			m_mTransParamNames[m_sParamName] = l_sName;
		}
	}

	//scan Stoch Transitions
	l_pcNodeclass = m_pcGraph->GetNodeclass(SP_DS_STOCHASTIC_TRANS);

	for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); l_itElem++)
	{
		wxString l_sName = dynamic_cast<SP_DS_NameAttribute*>((*l_itElem)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		//Get the transition rate function
		l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>((*l_itElem)->GetAttribute(wxT("FunctionList")));

		const wxString l_sTransitionFunction = l_pcColList->GetActiveCellValue(1);

		wxString m_sParamName = GetKParameter(l_sTransitionFunction);
		if (m_sParamName != wxT("1"))
		{
			m_mTransParamNames[m_sParamName] = l_sName;
		}
	}


}

wxString SP_DLG_FHybridSimulationResults::GetKParameter(const wxString& sfunc)
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

void SP_DLG_FHybridSimulationResults::LoadParameters(unsigned long literation, double dAlpha)
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
		std::vector<double>  vdparams = GetCombinationVectorForTopLevel();
		for (int y = 0; y < l_asParameterNames.size(); y++)
		{

			l_anParameterValue.push_back(vdparams[y]);

			m_msParameterName2Value[l_asParameterNames[y]] = vdparams[y];

			wxString ss;
			ss << vdparams[y];
			m_pcGraph->GetFunctionRegistry()->registerFunction(l_asParameterNames[y], ss);


		}

		m_pcMainSimulator->SetParameterNames(l_asParameterNames);
		m_pcMainSimulator->SetParameterValues(l_anParameterValue);

		return;
	}

	std::vector<double> vdCurrentParams = m_fr.GetCombinationMatrix()[literation];
	m_vdCurrentSample.clear();
	for (int iCount = 0; iCount < l_asParameterNames.size(); iCount++)
	{

		l_anParameterValue.push_back(vdCurrentParams[iCount]);

		m_msParameterName2Value[l_asParameterNames[iCount]] = vdCurrentParams[iCount];
		m_vdCurrentSample.push_back(vdCurrentParams[iCount]);
		wxString sParam;
		sParam << vdCurrentParams[iCount];
		m_pcGraph->GetFunctionRegistry()->registerFunction(l_asParameterNames[iCount], sParam);


	}

	m_pcMainSimulator->SetParameterNames(l_asParameterNames);
	m_pcMainSimulator->SetParameterValues(l_anParameterValue);

}

std::vector<double> SP_DLG_FHybridSimulationResults::GetCombinationVectorForTopLevel()
{
	std::vector<double> vdparam_top(m_paramMatrix.size(), 0);
	for (int iCount = 0; iCount < m_paramMatrix.size(); iCount++)
		vdparam_top[iCount] = m_paramMatrix[iCount][1];

	return vdparam_top;


}
bool SP_DLG_FHybridSimulationResults::InitializeSimulator(unsigned long lIteration, double dAlpha)
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
		if (m_binitialRun == false)
		{
			wxBusyInfo l_Info(wxT("Loading data, please wait...."), this);
		}
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
	}

	wxBusyInfo* l_pcInfo;
	if (m_binitialRun == false)
	{
	
		l_pcInfo = new wxBusyInfo(wxT("Initializing, please wait...."), this);
	wxDELETE(l_pcInfo);
    }
	 
	bool  l_bIsInitialized = m_pcMainSimulator->Initialise(true);
	 
	//Get some properties from the user
	if (m_pcMainSimulator->GetSimulatorOptions()->GetOption(wxT("InBetweenVisualization")) != NULL)
	{
		m_nRedraw = m_pcMainSimulator->GetSimulatorOptions()->GetOption(wxT("InBetweenVisualization"))->GetValueBool();
	}

	if (m_pcMainSimulator->GetSimulatorOptions()->GetOption(wxT("Refreshrate")) != NULL)
	{
		m_nRefreshRate = m_pcMainSimulator->GetSimulatorOptions()->GetOption(wxT("Refreshrate"))->GetValuelong();
	}

	//SetSimulationProgressGaugeRange(100);

	return l_bIsInitialized;
}

void SP_DLG_FHybridSimulationResults::OnStartAbortSimulation(wxCommandEvent& p_cEvent)
{

	wxString strBtnComm = m_pcStartButton->GetLabel();
	if (strBtnComm == wxT("Start Simulation"))
	{
		m_bIsAbort = false;
	}
	else 
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
	if (m_pcMainSimulator->IsSimulationRunning())
	{
		m_pcMainSimulator->AbortSimulation();
		return;
	}

	//Export Type
	if (m_pcDirectExportRadioButton->GetValue())
		m_ExportType = SP_SIM_DIRECT_EXPORT;
	else
		m_ExportType = SP_SIM_NO_EXPORT;

	if (m_binitialRun==false)
	{
		 

		//SetSimulationProgressGauge(0);

		m_pcStartButton->SetLabel(wxT("Abort Simulation"));
		m_pcStartButton->SetBackgroundColour(*wxRED);

		//Update();
		long selectedSimulation = m_samplingchoices->GetSelection();
		if (selectedSimulation == 1)
		{
			DoReducedFhpn();
		}
		else
		{
			DoFHpnSimulation();
		}
 
		
	}
	else
	{
		//SP_LOGERROR(wxT("The simulation can not be initialized"));
	}

	return;
}

void* SP_DLG_FHybridSimulationResults::DoFHpnSimulation()
{
	/*Initialise the fuzzy settings*/
	bool bisInitialised=InitializeFuzzySetting();
	if (!bisInitialised)
	{
		m_pcStartButton->SetBackgroundColour(*wxGREEN);
		m_pcStartButton->SetLabel(wxT("Start Simulation"));
		return 0;
	}
	m_ResultFBand.clear();
	//m_FuzzyBand.clear();
	/*inform the user to the  simulation running times*/
	long lnumofLevels = m_fr.GetNumFuzzyLevels();
	long lnumofSamples = m_fr.GetNumSamplePoints();
	long lnumFN = m_fr.GetNumFuzzyNum();
	long lRunningtimes = pow(lnumofSamples, lnumFN)* lnumofLevels+1;
	
	if (lnumFN == 0)
	{
		//run Traditional HPN
		const std::string sWarnMsg = "you are trying to run Standard HPN !";
		SP_LOGWARNING(sWarnMsg);
		RunNormalHPN();
		return 0;

	}

	const std::string sWarnMesag = "The total number of Simulation Runs is : ";
	std::string strmsg = sWarnMesag + std::to_string(lRunningtimes);
	SP_LOGWARNING(strmsg);
	m_lTotalSimRuns = lRunningtimes;
	wxMessageDialog* pc_msgDialog = new wxMessageDialog(this, strmsg, "High number of simulation runs", wxOK | wxCANCEL | wxICON_WARNING);
	std::thread     thSimWorkerThread;
	if (pc_msgDialog->ShowModal() == wxID_OK)
	{

		//long currentLevel = 1;

		m_fr.CalculateAlphaLevels();
	 
		thSimWorkerThread = std::thread([&]() {
			long lRemainingSimRunCoun = 0; long lRunCount = 0;
			for (int iAlpha = 0; iAlpha < m_fr.GetAlphaSet().size() ; iAlpha++)
			{
				m_clock = std::clock(); // get current time
				if (m_bIsAbort)
				{
					break;
				}
				m_fr.DoSamplePointsCombination(m_fr.GetAlphaSet()[iAlpha]);

				long m_lNumCombinedPoints = pow(m_fr.GetNumSamplePoints(), m_fr.GetNumFuzzyNum());

				for (long literation = 0; literation < m_lNumCombinedPoints; literation++)
				{
					if (m_bIsAbort)
					{
						break;
					}
					DoOneHPNSimulation(literation, m_fr.GetAlphaSet()[iAlpha]);
					lRunCount++;
					lRemainingSimRunCoun = m_lTotalSimRuns - lRunCount;
					SetSimulationProgressText(lRemainingSimRunCoun);
					SetSimulationProgressGauge(lRemainingSimRunCoun);
				}

			}
			double duration = (std::clock() - m_clock)/(double)CLOCKS_PER_SEC;
			m_pcSimulationStopWatch->SetLabel(wxString::Format(wxT("%.3f s"), duration));
			//alpha=1
			if (!m_bIsAbort)
			{
				DoOneHPNSimulation(0, 1);
				lRunCount++;
				lRemainingSimRunCoun = m_lTotalSimRuns - lRunCount;
				SetSimulationProgressText(lRemainingSimRunCoun);
				SetSimulationProgressGauge(lRemainingSimRunCoun);
				m_pcStartButton->SetLabel(wxT("Abort Processing"));
				m_pcStartButton->SetBackgroundColour(*wxRED);
				wxBusyInfo info(wxT("Finalizing the processing, please wait."), this);
				m_pcCompressedBand = m_fr.CompressResults(m_ResultFBand);//( m_FuzzyBand)
				//m_FuzzyBand.clear();
				m_ResultFBand.clear();
			}
			if (m_bIsAbort)
			{
				InitProgress();
				m_pcSimulationProgressGauge->SetValue(0);
				m_pcSimulationStopWatch->SetLabel(wxString::Format(wxT("%.3f s"), 0));
			}
			else {
				m_pcSimulationProgressGauge->SetValue(100);
			}
			m_lnFuzzyNum = 0;
			m_binitialRun = false;
			m_pcTimer->Stop();
			//SetSimulationProgressGauge(GetSimulatorProgress());
			SetSimulationStopWatch(1);
			m_pcStartButton->SetLabel(wxT("Start Simulation"));
			m_pcStartButton->SetBackgroundColour(*wxGREEN);
			if (!m_bIsAbort)
			{LoadResults(); }
		m_bIsSimulatorInitialized = false;
		m_bIsAbort = false;

		});
		thSimWorkerThread.detach();

		if (thSimWorkerThread.joinable())
		{
			thSimWorkerThread.join();
		}
	}
	else
	{
		m_pcStartButton->SetBackgroundColour(*wxGREEN);
		m_pcStartButton->SetLabel(wxT("Start Simulation"));
		return 0;

	}
	 
	m_lnFuzzyNum = 0;
	m_binitialRun = false;
	m_pcTimer->Stop();
	//SetSimulationProgressGauge(GetSimulatorProgress());
	SetSimulationStopWatch(1);

	//LoadResults();
	m_bIsSimulatorInitialized = false;
	return 0;
}
void* SP_DLG_FHybridSimulationResults::DoReducedFhpn()
{
	bool isInitialised=InitializeFuzzySetting();
	if (!isInitialised)
	{
		m_pcStartButton->SetBackgroundColour(*wxGREEN);
		m_pcStartButton->SetLabel(wxT("Start Simulation"));
		return 0;
	}
	m_ResultFBand.clear();
	long lnumofLevels = m_fr.GetNumFuzzyLevels();
	long lnumofSamples = m_fr.GetNumSamplePoints();
	long lnumFN = m_fr.GetNumFuzzyNum();

	long lRunningtimes = pow(lnumofSamples, lnumFN) + (lnumofLevels - 1) * 2 + 1;//pow(numofSamples, numFN)* numofLevels+1;
	const std::string sWarnMesag = "The total number of simulation runs is: ";
	std::string strMsg = sWarnMesag + std::to_string(lRunningtimes);
	SP_LOGWARNING(strMsg);
	wxMessageDialog* pc_msgDlg = new wxMessageDialog(this, strMsg, "High number of simulation runs", wxOK | wxCANCEL | wxICON_WARNING);
	std::thread thSimWorkerThread; m_lTotalSimRuns = lRunningtimes;
	if (pc_msgDlg->ShowModal() == wxID_OK)
	{

		m_fr.CalculateAlphaLevels();
	 
		thSimWorkerThread = std::thread([&]() {
			long lRemainingSimRunCoun = 0; long lRunCount = 0;
			for (int ialpha = 0; ialpha < m_fr.GetAlphaSet().size() ; ialpha++)
			{ 
				m_clock = std::clock(); // get current time
				if (m_bIsAbort)
				{
					break;
				}
				if (ialpha == 0)
				{
					m_fr.DoSamplePointsCombination(m_fr.GetAlphaSet()[ialpha]);

					long m_lNumCombinedPoints = pow(m_fr.GetNumSamplePoints(), m_fr.GetNumFuzzyNum());

					for (long literation = 0; literation < m_lNumCombinedPoints; literation++)
					{
						if (m_bIsAbort)
						{
							break;
						}
						double dalphaVal = m_fr.GetAlphaSet()[ialpha];
						DoOneHPNSimulation(literation, dalphaVal);
						lRunCount++;
						lRemainingSimRunCoun = m_lTotalSimRuns - lRunCount;
						SetSimulationProgressText(lRemainingSimRunCoun);
						SetSimulationProgressGauge(lRemainingSimRunCoun);
					}
				}
				else {//alpha>0

					/*Do simulation on the first point of the level above zero*/
					m_fr.DoSamplePointsCombination(m_fr.GetAlphaSet()[ialpha]);
					long m_lNumCombinedPoints = pow(m_fr.GetNumSamplePoints(), m_fr.GetNumFuzzyNum());
					double dcurrentLevel = m_fr.GetAlphaSet()[ialpha];
					if (!m_bIsAbort)
					{
						DoOneHPNSimulation(0, dcurrentLevel);
						lRunCount++;
						lRemainingSimRunCoun = m_lTotalSimRuns - lRunCount;
						SetSimulationProgressText(lRemainingSimRunCoun);
						SetSimulationProgressGauge(lRemainingSimRunCoun);
					}

					//Get first and last sample points to check if the samples of level zero belong to this interval
					vector<double> vdlow = GetCurentSamples(0);
					vector<double> vdup = GetCurentSamples(m_lNumCombinedPoints - 1);

					for (long literation = 1; literation < m_lNumCombinedPoints - 1; literation++)
					{
						int iCount = 0;
						if (m_bIsAbort)
						{
							break;
						}

						for (int j = 0; j < m_fr.GetNumFuzzyNum(); j++)
						{
							if (m_bIsAbort)
							{
								break;
							}

							if (m_ResultFBand[literation].sample[j] > vdlow[j] && m_ResultFBand[literation].sample[j] < vdup[j])
							{
								if (m_bIsAbort)
								{
									break;
								}
								iCount++;
								if (iCount == m_fr.GetNumFuzzyNum() && m_fr.GetAlphaSet()[ialpha] != 1)
								{

									TraceElement structTtraceElement = m_ResultFBand[literation];
									structTtraceElement.currentLevel = m_fr.GetAlphaSet()[ialpha];;
									structTtraceElement.levelNum = ialpha;
									m_ResultFBand.push_back(structTtraceElement);
								}

							}

						}
					}
					/*Do Simulation at the last point of the level above zero*/
					if (!m_bIsAbort)
					{
					 
					DoOneHPNSimulation(m_lNumCombinedPoints - 1, dcurrentLevel);
					lRunCount++;
					lRemainingSimRunCoun = m_lTotalSimRuns - lRunCount;
					SetSimulationProgressText(lRemainingSimRunCoun);
					SetSimulationProgressGauge(lRemainingSimRunCoun);
				     }


				}
			}
			double duration = (std::clock() - m_clock) / (double)CLOCKS_PER_SEC;
			m_pcSimulationStopWatch->SetLabel(wxString::Format(wxT("%.3f s"), duration));
			if (!m_bIsAbort) {
				// now alphe excatly equal to 1
				DoOneHPNSimulation(0, 1);
				lRunCount++;
				lRemainingSimRunCoun = m_lTotalSimRuns - lRunCount;
				SetSimulationProgressText(lRemainingSimRunCoun);
				m_pcSimulationProgressGauge->SetValue(100);
				m_pcStartButton->SetBackgroundColour(*wxRED);
				m_pcStartButton->SetLabel(wxT("Abort Processing"));
				wxBusyInfo info(wxT("Finalizing the processing, please wait."), this);
				m_pcCompressedBand = m_fr.CompressResults(m_ResultFBand);
				m_ResultFBand.clear();
				if (!m_bIsAbort)
				{
					m_pcSimulationProgressGauge->SetValue(100);
					LoadResults();
				}
				else { 
					InitProgress();
					m_pcSimulationProgressGauge->SetValue(0);
					m_pcSimulationStopWatch->SetLabel(wxString::Format(wxT("%.3f s"), 0.0));
				}
			}
			else {
				InitProgress();
				m_pcSimulationProgressGauge->SetValue(0);
			}
			m_pcStartButton->SetBackgroundColour(*wxGREEN);
			m_pcStartButton->SetLabel(wxT("Start Simulation"));
			m_lnFuzzyNum = 0;
			m_binitialRun = false;
			m_bIsAbort = false;
		});

		thSimWorkerThread.detach();

		if (thSimWorkerThread.joinable())
		{
			thSimWorkerThread.join();
		}
	}
	else {
		m_pcStartButton->SetBackgroundColour(*wxGREEN);
		m_pcStartButton->SetLabel(wxT("Start Simulation"));
		return 0;
	}

	return 0;

}

void* SP_DLG_FHybridSimulationResults::RunNormalHPN()
{
	//Doing combination without fuzzy numbers, means considring the same values of parameters
	m_fr.CalculateAlphaLevels();
	m_fr.DoSamplePointsCombination(m_fr.GetAlphaSet()[0]);

	bool m_bIsInitialized = InitializeSimulator(0, 0);
	m_binitialRun = true;

	if (m_binitialRun == true)
	{
		//SetSimulationProgressGauge(0);
		m_pcStartButton->SetLabel(wxT("Abort Simulation"));
		m_pcStartButton->SetBackgroundColour(*wxRED);

		spsim::HybridSimulator*		l_pcSimulator = dynamic_cast<spsim::HybridSimulator*> (m_pcMainSimulator);
		
		l_pcSimulator->Initialise(true);
		
		l_pcSimulator->SetSimulationRunningState(true);
		
		//check threading with run count
		long l_nRunCount;
		long l_nThreadCount;
		spsim::Property* l_pcPropRunCounts = l_pcSimulator->GetSimulatorOptions()->GetOption(wxT("RunCount"));
		spsim::Property* l_pcPropThreadCount = l_pcSimulator->GetSimulatorOptions()->GetOption(wxT("ThreadCount"));
		spsim::Matrix2DDouble vvdResultMat;

		if (l_pcPropThreadCount != NULL)
		{
			l_nThreadCount = l_pcPropThreadCount->GetValuelong();
		}
		if (l_pcPropRunCounts != NULL)
		{
			l_nRunCount = l_pcPropRunCounts->GetValuelong();
		}

		if (l_nThreadCount == 1)
		{
			//single thread

			for (int iRun = 0; iRun < l_nRunCount; iRun++)
			{
				l_pcSimulator->SimulateSingleRun();
				if (!l_pcSimulator->IsSimulationRunning()) {
					break;
				}
			}
			vvdResultMat = l_pcSimulator->GetResultMatrix();

			if (l_nRunCount > 1)
			{
				//if number of runs is greater than one, then do averging
				
				long	l_nColCount = l_pcSimulator->GetPlaceCount();
				for (unsigned int l_nRow = 0; l_nRow < vvdResultMat.size(); l_nRow++)
					for (unsigned int l_nCol = 0; l_nCol < l_nColCount; l_nCol++)
					{
						vvdResultMat[l_nRow][l_nCol] /= l_nRunCount;
					}
			}
		}
		else {
			//multi threading
			l_pcSimulator->RunSimulation();

			// do averaging
			vvdResultMat = l_pcSimulator->GetResultMatrix();
			long	l_nColCount = l_pcSimulator->GetPlaceCount();
			for (unsigned int l_nRow = 0; l_nRow < vvdResultMat.size(); l_nRow++)
				for (unsigned int l_nCol = 0; l_nCol < l_nColCount; l_nCol++)
				{
					vvdResultMat[l_nRow][l_nCol] /= l_nRunCount;
				}
		}

		l_pcSimulator->AbortSimulation();
	//	m_nFuzzyResultBand.push_back(resultMat);
	}
	m_pcStartButton->SetLabel(wxT("Start Simulation"));
	m_pcStartButton->SetBackgroundColour(*wxGREEN);
	//m_lnFuzzyNum = 0;
	m_binitialRun = false;
	m_pcTimer->Stop();
	SetSimulationProgressGauge(GetSimulatorProgress());
	SetSimulationStopWatch(1);

	LoadResults();

	m_bIsSimulatorInitialized = false;
	return 0;
}

void SP_DLG_FHybridSimulationResults::DirectExportToCSV()
{

	if (!m_pcExport)
		return;

	*m_pcExport << wxT("Time");

	//long lnumofLevels = m_fr.GetNumFuzzyLevels();
	//long lnumofSamples = m_fr.GetNumSamplePoints();
	//long lnumFN = m_fr.GetNumFuzzyNum();

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
	////////////////////////////
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
		l_sOutput << dssd::aux::toString(m_pcMainSimulator->GetOutputSampleSize() * l_nRow);
		l_sCurrentRow = wxT("");
		for (int j = 0; j < mPlaces2PosMap.size(); j++)
		{
			wxString l_sName = vstrPlaces[j];
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

void* SP_DLG_FHybridSimulationResults::DoOneHPNSimulation(long lIter, double dAlpha)
{
	bool m_bIsInitialized = InitializeSimulator(lIter, dAlpha);
	m_binitialRun = true;
	TraceElement traceElement;
	if (m_bIsInitialized == true)
	{

		//SetSimulationProgressGauge(0);
	//	m_pcStartButton->SetLabel(wxT("Abort Simulation"));
	//	m_pcStartButton->SetBackgroundColour(*wxRED);
		m_pcMainSimulator->Initialise(true);
		m_pcMainSimulator->SetSimulationRunningState(true);
		//DoStartSimulation();
		spsim::HybridSimulator*		l_pcSimulator = dynamic_cast<spsim::HybridSimulator*> (m_pcMainSimulator);
		l_pcSimulator->Initialise(true);
		l_pcSimulator->SetSimulationRunningState(true);
		long l_nRunCount;
		long l_nThreadCount;
		spsim::Property* l_pcPropRunCounts = l_pcSimulator->GetSimulatorOptions()->GetOption(wxT("RunCount"));
		spsim::Property* l_pcPropThreadCount = l_pcSimulator->GetSimulatorOptions()->GetOption(wxT("ThreadCount"));
		spsim::Matrix2DDouble vvdResultMat;
		if (l_pcPropThreadCount != NULL)
		{
			l_nThreadCount = l_pcPropThreadCount->GetValuelong();
		}
		if (l_pcPropRunCounts != NULL)
		{
			l_nRunCount = l_pcPropRunCounts->GetValuelong();
		}


		if (l_nThreadCount == 1)
		{
			//single thread

			for (int iRun = 0; iRun < l_nRunCount; iRun++)
			{
				if (m_bIsAbort)
				{
					break;
				}
				l_pcSimulator->SimulateSingleRun();
				if (!l_pcSimulator->IsSimulationRunning()) {
					break;
				}
			}

			if (l_nRunCount > 1) {
				vvdResultMat = l_pcSimulator->GetResultMatrix();
				long	l_nColCount = l_pcSimulator->GetPlaceCount();
				for (unsigned int l_nRow = 0; l_nRow < vvdResultMat.size(); l_nRow++){
					if (m_bIsAbort)
					{
						break;
					}
					for (unsigned int l_nCol = 0; l_nCol < l_nColCount; l_nCol++)
					{
						if (m_bIsAbort)
						{
							break;
						}
						vvdResultMat[l_nRow][l_nCol] /= l_nRunCount;
					}
			}
			}
			else {
				// no averaging when number of runs is no greater than one run
				vvdResultMat = l_pcSimulator->GetResultMatrix();
			}
		}
		else {

			//multithreading

			l_pcSimulator->RunSimulation();

			// do averaging
			vvdResultMat = l_pcSimulator->GetResultMatrix();
			long	l_nColCount = l_pcSimulator->GetPlaceCount();
			for (unsigned int l_nRow = 0; l_nRow < vvdResultMat.size(); l_nRow++)
			{
				if (m_bIsAbort)
				{
					break;
				}
				for (unsigned int l_nCol = 0; l_nCol < l_nColCount; l_nCol++)
				{
					if (m_bIsAbort)
					{
						break;
					}
					vvdResultMat[l_nRow][l_nCol] /= l_nRunCount;
				}
		}


		}


		l_pcSimulator->AbortSimulation();
		l_pcSimulator->Initialise(false);
		m_pcMainSimulator->Initialise(true);
		m_pcMainSimulator->SetSimulationRunningState(false);
		l_pcSimulator->SetSimulationRunningState(false);
		//m_pcStartButton->SetLabel(wxT("Start Simulation"));
	//	m_pcStartButton->SetBackgroundColour(*wxGREEN);
		m_pcTimer->Stop();
	//	SetSimulationProgressGauge(GetSimulatorProgress());
		SetSimulationStopWatch(1);
		//m_nFuzzyResultBand.push_back(resultMat);
		traceElement.sample = m_vdCurrentSample;
		traceElement.currentLevel = m_fr.GetAlphaSet()[dAlpha];
		traceElement.levelNum = dAlpha;
		traceElement.fuzzyTrace = vvdResultMat;
		m_ResultFBand.push_back(traceElement);

	}
	return 0;
}

std::vector<double> SP_DLG_FHybridSimulationResults::GetCurentSamples(int iteration)
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

	std::vector<double> vdcurrentParams;
	vdcurrentParams.clear();
	vdcurrentParams = m_fr.GetCombinationMatrix()[iteration];
	m_vdCurrentSample.clear();
	for (int y = 0; y < l_asParameterNames.size(); y++)
	{
		if (m_mFuzzyParam2Position.find(l_asParameterNames[y]) != m_mFuzzyParam2Position.end())//if (l_asParameterNames[y] == m_fuzzyParams[y])
		{
			int pos = m_mFuzzyParam2Position[l_asParameterNames[y]];
			m_vdCurrentSample.push_back(vdcurrentParams[pos]);
			continue;
		}
	}
	return m_vdCurrentSample;
}
void SP_DLG_FHybridSimulationResults::SetSimulationProgressText(long& p_nValue)
{
	if (m_lTotalSimRuns != 0)
	{
		m_pcSimulationProgressText->SetLabel(wxString::Format(wxT("%i %%"), 100 - ((p_nValue * 100) / m_lTotalSimRuns)));


	}


}
void SP_DLG_FHybridSimulationResults::InitProgress()
{


	m_pcSimulationProgressText->SetLabel(wxString::Format(wxT("%i %%"), 0));
}
void SP_DLG_FHybridSimulationResults::SetSimulationProgressGauge(long p_nValue)
{

	if (m_lTotalSimRuns != 0) {
		int iProgress = 100 - ((p_nValue * 100) / m_lTotalSimRuns);
		m_pcSimulationProgressGauge->SetValue(iProgress);

		Update();
	}
}
void SP_DLG_FHybridSimulationResults::SetSimulationProgressGaugeRange(long p_nRangeValue)
{
	m_pcSimulationProgressGauge->SetRange(p_nRangeValue);

}