/*
* SP_DLG_FHybridSimulationResults.cpp
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
	SAMPLING_CHOICES_ID2,
	SP_ID_BUTTON_SAMPLING_PROP

};
BEGIN_EVENT_TABLE(SP_DLG_FHybridSimulationResults, SP_DLG_HybridSimulationResults)
EVT_SIMTHREAD(SP_SIMULATION_THREAD_EVENT, SP_DLG_FHybridSimulationResults::OnSimulatorThreadEvent)
EVT_CHOICE(SAMPLING_CHOICES_ID2, SP_DLG_FHybridSimulationResults::OnSamplingAlgoChanged)
EVT_BUTTON(SP_ID_BUTTON_SAMPLING_PROP, SP_DLG_FHybridSimulationResults::OnSamplingProperties)
END_EVENT_TABLE()
SP_DLG_FHybridSimulationResults::SP_DLG_FHybridSimulationResults(SP_DS_Graph* p_pcGraph, wxWindow* p_pcParent, wxString p_sHelpText, wxString p_sTitle, long p_nStyle) :
	SP_DLG_HybridSimulationResults(p_pcGraph, p_pcParent, p_sHelpText, p_sTitle, p_nStyle)

{
	wxSizer* l_pcRowSizer = NULL;
	wxSizer* l_pcRowSizer1 = NULL;
	wxSizer* l_pcRowSizer2 = NULL;
	wxSizer* l_pcRowSizer3 = NULL;
	m_pcCompressedBand = new SP_Compressed_Fuzzy_Band();
	m_samplingStrategyselection = 0;
	wxString l_sAlphaLevels;
	wxString l_sSamplingPoints;

	m_bIsAbort = false;
	m_fr = nullptr;
	m_lTotalSimRuns = 0;
	m_samplingchoices = new wxChoice(m_pcPropertyWindowPropertySizer, SAMPLING_CHOICES_ID2, wxDefaultPosition, wxSize(100, -1));

	m_samplingchoices->Append(wxT("Basic"));
	m_samplingchoices->Append(wxT("Reduced"));
	m_samplingchoices->Append(wxT("Random LHS"));
	m_samplingchoices->Append(wxT("Improved LHS"));
	m_samplingchoices->Append(wxT("Optimum LHS"));
	m_samplingchoices->Append(wxT("Genetic LHS"));
	m_samplingchoices->Append(wxT("Maximin LHS"));

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
					m_samplingStrategyselection = l_pcAttr->GetValue();
				}
			}
		}

		//SP_DS_Attribute* l_pcAttrAlpha = l_pcSimProp->GetAttribute(wxT("AlphaLevels"));
		l_sAlphaLevels = GetFuzzySettingAttribute(*m_pcGraph->GetMetadataclass(wxT("FuzzySettings"))->GetElements()->begin(), wxT("AlphaLevels"));

		if (l_sAlphaLevels.IsEmpty())
		{
			l_sAlphaLevels = wxT("10");
		}

		l_sSamplingPoints = GetFuzzySettingAttribute(*m_pcGraph->GetMetadataclass(wxT("FuzzySettings"))->GetElements()->begin(), wxT("SamplingPoints"));

		if (l_sSamplingPoints.IsEmpty())
		{
			l_sSamplingPoints = wxT("10");
		}
	}


	m_samplingchoices->SetSelection(m_samplingStrategyselection);

	l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
	l_pcRowSizer->Add(new wxStaticText(m_pcPropertyWindowPropertySizer, -1, wxT("Fuzzy Setting:")), wxSizerFlags(1).Expand().Border(wxALL, 2));

	///////////////////////////////////////
	l_pcRowSizer1 = new wxBoxSizer(wxHORIZONTAL);
	l_pcRowSizer2 = new wxBoxSizer(wxHORIZONTAL);
	l_pcRowSizer1->Add(new wxStaticText(m_pcPropertyWindowPropertySizer, -1, wxT("alpha levels")), wxSizerFlags(1).Expand().Border(wxALL, 2));
	m_lalphaLevels = new  wxTextCtrl(m_pcPropertyWindowPropertySizer, -1, l_sAlphaLevels, wxDefaultPosition, wxDefaultSize, 0);
	l_pcRowSizer1->Add(m_lalphaLevels, wxSizerFlags(1).Expand().Border(wxALL, 2));
	/////////////////////
	l_pcRowSizer2->Add(new wxStaticText(m_pcPropertyWindowPropertySizer, -1, wxT("discretisation points")), wxSizerFlags(1).Expand().Border(wxALL, 2));

	m_lSamplePoints = new  wxTextCtrl(m_pcPropertyWindowPropertySizer, -1, l_sSamplingPoints, wxDefaultPosition, wxDefaultSize, 0);


	l_pcRowSizer2->Add(m_lSamplePoints, wxSizerFlags(1).Expand().Border(wxALL, 2));

	l_pcRowSizer3 = new wxBoxSizer(wxHORIZONTAL);
	l_pcRowSizer3->Add(new wxStaticText(m_pcPropertyWindowPropertySizer, -1, wxT("sampling strategy")), wxSizerFlags(1).Expand().Border(wxALL, 2));
	l_pcRowSizer3->Add(m_samplingchoices, 1, wxALL, 5);

	m_samplingchoices->SetSelection(m_samplingStrategyselection);

	wxButton* m_pcSamplingProp = new wxButton(m_pcPropertyWindowPropertySizer, SP_ID_BUTTON_SAMPLING_PROP, wxT("Properties"));
	l_pcRowSizer3->Add(m_pcSamplingProp, 0, wxALL
#if wxCHECK_VERSION(2,8,8)
		| wxRESERVE_SPACE_EVEN_IF_HIDDEN
#endif
		, 5);

	l_pcRowSizer3->Add(m_pcSamplingProp);


	m_pcPropertySizer->Add(l_pcRowSizer, wxSizerFlags(0).Expand().Border(wxALL, 2));
	m_pcPropertySizer->Add(l_pcRowSizer1, wxSizerFlags(0).Expand().Border(wxALL, 2));
	m_pcPropertySizer->Add(l_pcRowSizer2, wxSizerFlags(0).Expand().Border(wxALL, 2));
	m_pcPropertySizer->Add(l_pcRowSizer3, wxSizerFlags(0).Expand().Border(wxALL, 2));
	SP_DS_Nodeclass* l_pcNodeclass = m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_PLACE);
	m_lpCount = l_pcNodeclass->GetElements()->size();
	l_pcNodeclass = m_pcGraph->GetNodeclass(SP_DS_DISCRETE_PLACE);
	m_lpCount += l_pcNodeclass->GetElements()->size();
	m_binitialRun = false;
	m_lnFuzzyNum = 0;

	SetSizerAndFit(m_pcMainSizer);
	m_bIsSimulatorInitialized = false;
	Layout();
}
SP_DLG_FHybridSimulationResults::~SP_DLG_FHybridSimulationResults()
{
	SaveFuzzySimulatorProperties();
	wxDELETE(m_fr);
	m_pcGraph->GetParentDoc()->Modify(true);
}

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

	m_mFuzzyParam2Position.clear();

	std::vector<TriangularFN>  asTFNParams;

	SP_DS_Metadataclass* mc = m_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);
	if (mc)
	{
		//go through all groups and activate the current selective set
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
			if (l_sType == wxT("TFN") && (m_mTransParamNames.find(l_sName) != m_mTransParamNames.end()))
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
				//	m_fuzzyParams.push_back(l_sName);
					iPos++;
					continue;
				}
			}


			SP_DS_FunctionRegistryEntry l_FE = l_pcFR->lookUpFunction(l_sName);
			if (l_FE.IsOk() && (m_mTransParamNames.find(l_sName) != m_mTransParamNames.end()))
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

			fuzzification::SAMPLING_TYPE l_tSamplingType;

			l_tSamplingType = GetSelectedSamplingMethod();

			if (m_fr == nullptr)
			{
				m_fr = new FuzzyReasoning(m_lAlphaLevels, m_lnDiscPoints, m_paramMatrix, m_lnFuzzyNum, m_lpCount,l_tSamplingType);

			}
			else
			{
				m_fr->SetAlphaLevels(m_lAlphaLevels);
				m_fr->SetDiscretisingPoints(m_lnDiscPoints);
				m_fr->SetParamMatrix(m_paramMatrix);
				m_fr->SetNumberFuzzyParams(m_lnFuzzyNum);
				m_fr->SetPlaceNum(m_lpCount);
				m_fr->InitialiseCombinationMatrix();
			}
			return true;
		}
		else {
			SP_MESSAGEBOX(wxT("The chosen values of Alpha levels or Sample points must be larger than 0"));
			return false;
		}
	}
	else {
		SP_MESSAGEBOX(wxT("The chosen values of Alpha levels or Sample points must be numerical values"));
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


	unsigned long l_nPosition = 0;



	//Go through all the continuous transition nodes
	for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); l_itElem++)
	{
		wxString l_sName = dynamic_cast<SP_DS_NameAttribute*>((*l_itElem)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		//Get the transition rate function
		l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>((*l_itElem)->GetAttribute(wxT("FunctionList")));

		const wxString l_sTransitionFunction = l_pcColList->GetActiveCellValue(1);

		wxString m_sParamName = GetKParameter(l_sTransitionFunction);


		wxString l_sTobeTokenized=m_sParamName;
		wxStringTokenizer tokenize(l_sTobeTokenized,"(+-/%*) ");
		while(tokenize.HasMoreTokens())
		{
		 wxString l_sToken=tokenize.GetNextToken();
		 m_mTransParamNames[l_sToken] = l_sName;

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

		wxString l_sTobeTokenized=m_sParamName;
		wxStringTokenizer tokenize(l_sTobeTokenized,"(+-/%*) ");
		while(tokenize.HasMoreTokens())
		{
			wxString l_sToken=tokenize.GetNextToken();
			m_mTransParamNames[l_sToken] = l_sName;

		}
	}


}

wxString SP_DLG_FHybridSimulationResults::GetKParameter(const wxString& sfunc)
{
	wxString m_sKparam;
	wxString l_msfun = sfunc;
	l_msfun.Replace("MassAction", "");
	l_msfun.Replace("pow", "");
	l_msfun.Replace("(", "");
	l_msfun.Replace(")", "");
	m_sKparam = l_msfun;
	return m_sKparam;
}

void SP_DLG_FHybridSimulationResults::LoadParameters(unsigned long literation, double dAlpha)
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

	std::vector<double> vdCurrentParams = m_fr->GetCombinationMatrix()[literation];
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
	if (m_pcMainSimulator->GetSimulatorOptions()->GetOption("InBetweenVisualization") != NULL)
	{
		m_nRedraw = m_pcMainSimulator->GetSimulatorOptions()->GetOption("InBetweenVisualization")->GetValueBool();
	}

	if (m_pcMainSimulator->GetSimulatorOptions()->GetOption("Refreshrate") != NULL)
	{
		m_nRefreshRate = m_pcMainSimulator->GetSimulatorOptions()->GetOption("Refreshrate")->GetValuelong();
	}

	//SetSimulationProgressGaugeRange(100);

	return l_bIsInitialized;
}
void SP_DLG_FHybridSimulationResults::OnStartAbortSimulation(wxCommandEvent& p_cEvent)
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
			m_pcMainSimulator->AbortSimulation();
			m_pcStartButton->SetLabel(wxT("Start Simulation"));
			m_pcStartButton->SetBackgroundColour(*wxGREEN);
			Update();
		}
	}
	else {
		if (m_pcDirectExportRadioButton->GetValue())
			m_ExportType = SP_SIM_DIRECT_EXPORT;
		else
			m_ExportType = SP_SIM_NO_EXPORT;

		fuzzification::SAMPLING_TYPE l_typeSampling = GetSelectedSamplingMethod();

		if (l_typeSampling == fuzzification::SAMPLING_TYPE::REDUCED)
		{
				DoReducedSamplingFhpnSimulThread();
		}
		else
		{
					DoBasicSamplingFhpnSimulThread();
		}
	}
}


void* SP_DLG_FHybridSimulationResults::DoBasicSamplingFhpnSimulThread()
{
	/*Initialise the fuzzy settings*/
	bool bisInitialised = InitializeFuzzySetting();
	if (!bisInitialised)
	{
		m_pcStartButton->SetBackgroundColour(*wxGREEN);
		m_pcStartButton->SetLabel(wxT("Start Simulation"));
		return 0;
	}
	m_ResultFBand.clear();
	m_fhpnThreadVector.clear();
	//m_FuzzyBand.clear();
	/*inform the user to the  simulation running times*/
	long lnumofLevels = m_fr->GetNumFuzzyLevels();
	long lnumofSamples = m_fr->GetNumSamplePoints();
	long lnumFN = m_fr->GetNumFuzzyNum();
	long lRunningtimes;// = pow(lnumofSamples, lnumFN)* lnumofLevels + 1;

	if (m_fr->GetSamplingType() == fuzzification::SAMPLING_TYPE::BASIC)
	{
		lRunningtimes = pow(lnumofSamples, lnumFN) * (lnumofLevels)+1;
	}
	else
	{//one of LHS algos
		lRunningtimes = (lnumofSamples * lnumofLevels) + 1;
	}

	const std::string sWarnMesag = "The total number of Simulation Runs is : ";
	std::string strmsg = sWarnMesag + std::to_string(lRunningtimes);
	SP_LOGWARNING(strmsg);
	m_lTotalSimRuns = lRunningtimes;
	long lthreadCounter = 0;
	m_lruncounter = 0;

	wxMessageDialog* pc_msgDialog = new wxMessageDialog(this, strmsg, "High number of simulation runs", wxOK | wxCANCEL | wxICON_WARNING);

	if (pc_msgDialog->ShowModal() == wxID_OK)
	{
		//m_pcStartButton->SetBackgroundColour(*wxRED);
		m_pcStartButton->SetLabel(wxT("Abort Simulation"));
		m_pcStartButton->SetOwnBackgroundColour(*wxRED);
		//m_pcStartButton->Refresh();
		m_simStopWatch.Start();
		Update();
		m_fr->CalculateAlphaLevels();

		for (int iAlpha = 0; iAlpha < m_fr->GetAlphaSet().size() - 1; iAlpha++)
		{

			if (m_bIsAbort)
			{
				break;
			}
			m_fr->DoSamplePointsCombination(m_fr->GetAlphaSet()[iAlpha]);

			//long m_lNumCombinedPoints = pow(m_fr->GetNumSamplePoints(), m_fr->GetNumFuzzyNum());
			long m_lNumCombinedPoints;// = pow(m_fr->GetNumSamplePoints(), m_fr->GetNumFuzzyNum());

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
				if (m_bIsAbort)
				{
					break;
				}
				bool bIsInitialized = InitializeSimulator(literation, m_fr->GetAlphaSet()[iAlpha]);
				if (bIsInitialized)
				{
					if (m_bIsAbort)
					{
						break;
					}

					m_fhpnThreadVector.push_back(new SP_DS_FhpnSimulThread(this, m_pcMainSimulator, m_fr->GetAlphaSet()[iAlpha], m_vdCurrentSample));
					if (m_fhpnThreadVector[lthreadCounter]->Create() != wxTHREAD_NO_ERROR)
					{
					}

					m_fhpnThreadVector[lthreadCounter]->SetPriority(WXTHREAD_MAX_PRIORITY);

					if (m_fhpnThreadVector[lthreadCounter]->Run())
					{

					}


					m_fhpnThreadVector[lthreadCounter]->Wait();

					wxYieldIfNeeded();

					wxDELETE(m_fhpnThreadVector[lthreadCounter]);
					lthreadCounter++;
					m_pcMainSimulator->AbortSimulation();

				}



			}
		}
		bool bIsInitialized = InitializeSimulator(0, 1);
		if (bIsInitialized && !m_bIsAbort)
		{
			m_fhpnThreadVector.push_back(new SP_DS_FhpnSimulThread(this, m_pcMainSimulator, 1, m_vdCurrentSample));
			if (m_fhpnThreadVector[lthreadCounter]->Create() != wxTHREAD_NO_ERROR)
			{
			}

			m_fhpnThreadVector[lthreadCounter]->SetPriority(WXTHREAD_MAX_PRIORITY);

			if (m_fhpnThreadVector[lthreadCounter]->Run())
			{

			}


			m_fhpnThreadVector[lthreadCounter]->Wait();

			wxYieldIfNeeded();

			wxDELETE(m_fhpnThreadVector[lthreadCounter]);
			lthreadCounter++;
			m_pcMainSimulator->AbortSimulation();

		}


	}
	else {
		//the user has not confirm the simulation
		return NULL;
	}
	if (m_bIsAbort)
	{
		m_pcSimulationProgressGauge->SetValue(0);
		m_pcSimulationProgressText->SetLabel(wxString::Format(wxT("%i %%"), 0));
		m_pcStartButton->SetLabel(wxT("Start Simulation"));
		m_pcStartButton->SetBackgroundColour(*wxGREEN);
		m_bIsAbort = false;
		m_ResultFBand.clear();
		//m_vResultFBand.clear();
		m_simStopWatch.Pause();
		m_pcMainSimulator->AbortSimulation();
		Update();
	}
	return NULL;
}


void* SP_DLG_FHybridSimulationResults::DoReducedSamplingFhpnSimulThread()
{
	bool isInitialised = InitializeFuzzySetting();
	if (!isInitialised)
	{
		m_pcStartButton->SetBackgroundColour(*wxGREEN);
		m_pcStartButton->SetLabel(wxT("Start Simulation"));
		return 0;
	}
	m_fhpnThreadVector.clear();
	m_ResultFBand.clear();
	long lnumofLevels = m_fr->GetNumFuzzyLevels();
	long lnumofSamples = m_fr->GetNumSamplePoints();
	long lnumFN = m_fr->GetNumFuzzyNum();

	long lRunningtimes = pow(lnumofSamples, lnumFN) + (lnumofLevels - 1) * 2 + 1;//pow(numofSamples, numFN)* numofLevels+1;
	const std::string sWarnMesag = "The total number of simulation runs is: ";
	std::string strMsg = sWarnMesag + std::to_string(lRunningtimes);
	SP_LOGWARNING(strMsg);
	m_lTotalSimRuns = lRunningtimes;
	long lthreadCounter = 0;
	m_lruncounter = 0;
	wxMessageDialog* pc_msgDlg = new wxMessageDialog(this, strMsg, "High number of simulation runs", wxOK | wxCANCEL | wxICON_WARNING);

	if (pc_msgDlg->ShowModal() == wxID_OK)
	{
		m_pcStartButton->SetLabel(wxT("Abort Simulation"));
		m_pcStartButton->SetOwnBackgroundColour(wxColour(255, 0, 0));
		//m_pcStartButton->Refresh();
		m_simStopWatch.Start();
		Update();
		m_fr->CalculateAlphaLevels();

		for (int ialpha = 0; ialpha < m_fr->GetAlphaSet().size() - 1; ialpha++)
		{
			//	m_clock = std::clock(); // get current time
			if (m_bIsAbort)
			{
				break;
			}
			if (ialpha == 0)
			{
				m_fr->DoSamplePointsCombination(m_fr->GetAlphaSet()[ialpha]);

				long m_lNumCombinedPoints = pow(m_fr->GetNumSamplePoints(), m_fr->GetNumFuzzyNum());

				for (long literation = 0; literation < m_lNumCombinedPoints; literation++)
				{
					if (m_bIsAbort)
					{
						break;
					}
					bool bIsInitialized = InitializeSimulator(literation, m_fr->GetAlphaSet()[ialpha]);
					if (bIsInitialized)
					{
						if (m_bIsAbort)
						{
							break;
						}

						m_fhpnThreadVector.push_back(new SP_DS_FhpnSimulThread(this, m_pcMainSimulator, m_fr->GetAlphaSet()[ialpha], m_vdCurrentSample));
						if (m_fhpnThreadVector[lthreadCounter]->Create() != wxTHREAD_NO_ERROR)
						{
						}

						m_fhpnThreadVector[lthreadCounter]->SetPriority(WXTHREAD_MAX_PRIORITY);

						if (m_fhpnThreadVector[lthreadCounter]->Run())
						{

						}


						m_fhpnThreadVector[lthreadCounter]->Wait();

						wxYieldIfNeeded();

						wxDELETE(m_fhpnThreadVector[lthreadCounter]);
						lthreadCounter++;
						m_pcMainSimulator->AbortSimulation();
					}
				}
			}
			else {
				//alpha>0
				m_fr->DoSamplePointsCombination(m_fr->GetAlphaSet()[ialpha]);
				long m_lNumCombinedPoints = pow(m_fr->GetNumSamplePoints(), m_fr->GetNumFuzzyNum());
				double dcurrentLevel = m_fr->GetAlphaSet()[ialpha];
				bool bIsInitialized = InitializeSimulator(0, m_fr->GetAlphaSet()[ialpha]);
				if (bIsInitialized && !m_bIsAbort)
				{
					m_fhpnThreadVector.push_back(new SP_DS_FhpnSimulThread(this, m_pcMainSimulator, m_fr->GetAlphaSet()[ialpha], m_vdCurrentSample));
					if (m_fhpnThreadVector[lthreadCounter]->Create() != wxTHREAD_NO_ERROR)
					{
					}
					m_fhpnThreadVector[lthreadCounter]->SetPriority(WXTHREAD_MAX_PRIORITY);

					if (m_fhpnThreadVector[lthreadCounter]->Run())
					{

					}
					m_fhpnThreadVector[lthreadCounter]->Wait();

					wxYieldIfNeeded();
					wxDELETE(m_fhpnThreadVector[lthreadCounter]);
					lthreadCounter++;
					m_pcMainSimulator->AbortSimulation();
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

					for (int j = 0; j < m_fr->GetNumFuzzyNum(); j++)
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
							if (iCount == m_fr->GetNumFuzzyNum() && m_fr->GetAlphaSet()[ialpha] != 1)
							{

								TraceElement structTtraceElement = m_ResultFBand[literation];
								structTtraceElement.currentLevel = m_fr->GetAlphaSet()[ialpha];;
								structTtraceElement.levelNum = ialpha;
								m_ResultFBand.push_back(structTtraceElement);
							}

						}

					}
				}
				/*Do Simulation at the last point of the level above zero*/
				bIsInitialized = InitializeSimulator(m_lNumCombinedPoints - 1, dcurrentLevel);
				if (bIsInitialized && !m_bIsAbort)
				{
					m_fhpnThreadVector.push_back(new SP_DS_FhpnSimulThread(this, m_pcMainSimulator, m_fr->GetAlphaSet()[ialpha], m_vdCurrentSample));
					if (m_fhpnThreadVector[lthreadCounter]->Create() != wxTHREAD_NO_ERROR)
					{
					}
					m_fhpnThreadVector[lthreadCounter]->SetPriority(WXTHREAD_MAX_PRIORITY);

					if (m_fhpnThreadVector[lthreadCounter]->Run())
					{

					}
					m_fhpnThreadVector[lthreadCounter]->Wait();

					wxYieldIfNeeded();
					wxDELETE(m_fhpnThreadVector[lthreadCounter]);
					lthreadCounter++;
					m_pcMainSimulator->AbortSimulation();
				}
				//else { return NULL; }
			}
		}

		bool bIsInitialized = InitializeSimulator(0, 1);
		if (bIsInitialized && !m_bIsAbort)
		{
			m_fhpnThreadVector.push_back(new SP_DS_FhpnSimulThread(this, m_pcMainSimulator, 1, m_vdCurrentSample));
			if (m_fhpnThreadVector[lthreadCounter]->Create() != wxTHREAD_NO_ERROR)
			{
			}
			m_fhpnThreadVector[lthreadCounter]->SetPriority(WXTHREAD_MAX_PRIORITY);

			if (m_fhpnThreadVector[lthreadCounter]->Run())
			{

			}
			m_fhpnThreadVector[lthreadCounter]->Wait();

			wxYieldIfNeeded();
			wxDELETE(m_fhpnThreadVector[lthreadCounter]);
			lthreadCounter++;
			m_pcMainSimulator->AbortSimulation();
		}


	}
	else {
		//the user did not confirm
		return NULL;
	}
	if (m_bIsAbort)
	{
		m_pcSimulationProgressGauge->SetValue(0);
		m_pcSimulationProgressText->SetLabel(wxString::Format(wxT("%i %%"), 0));
		m_pcStartButton->SetLabel(wxT("Start Simulation"));
		m_pcStartButton->SetBackgroundColour(*wxGREEN);
		m_bIsAbort = false;
		m_ResultFBand.clear();
		m_simStopWatch.Pause();
		m_pcMainSimulator->AbortSimulation();
		Update();
	}
	return NULL;
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

std::vector<double> SP_DLG_FHybridSimulationResults::GetCurentSamples(int iteration)
{

	SP_DS_Metadataclass* mc = m_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);
	SP_ListMetadata::const_iterator it;

	SP_VectorStdString l_asParameterNames;
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
	vdcurrentParams = m_fr->GetCombinationMatrix()[iteration];
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

void    SP_DLG_FHybridSimulationResults::OnSimulatorThreadEvent(SP_DS_ThreadEvent& event)
{
	switch (event.GetEventTask())
	{

	case SP_THREAD_EVENT_END_ONE_RUN://SP_THREAD_EVENT_END_SIMULATION:
	{
		long lremain;
		void* trace = event.GetUserData();
		m_lruncounter++;
		//if (m_lTotalSimRuns >= m_lruncounter) {
		lremain = m_lTotalSimRuns - m_lruncounter;
		SetSimulationProgressText(lremain);
		SetSimulationProgressGauge(lremain);//

		TraceElement* temp = (TraceElement*)trace;
		TraceElement tresultTrace;
		tresultTrace.sample = temp->sample;
		tresultTrace.currentLevel = temp->currentLevel;
		tresultTrace.fuzzyTrace = temp->fuzzyTrace;

		m_ResultFBand.push_back(tresultTrace);
		SetSimulationStopWatch(m_simStopWatch.Time());
		//}
		if (lremain == 0)
		{
			m_simStopWatch.Pause();
			m_pcStartButton->SetLabel(wxT("Abort Processing"));
			SP_DS_FuzzyResult_Thread* fthreadRes = new SP_DS_FuzzyResult_Thread(this, m_ResultFBand, m_fr);
			fthreadRes->Create();
			m_info = new wxBusyInfo
			(
				wxBusyInfoFlags()
				.Parent(this)
				//.Title("<b>Finialiazing the processing</b>")
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
		m_ResultFBand.clear();
		LoadResults();
		m_bIsAbort = false;
		//wxDELETE(m_fr);
		m_fr->ClearData();
		break;
	}
	case 10: {

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

void SP_DLG_FHybridSimulationResults::LoadTransitions()
{
	//Stochastic Transitions
	SP_DS_Nodeclass* l_pcNodeclass;
	l_pcNodeclass = m_pcGraph->GetNodeclass(SP_DS_STOCHASTIC_TRANS);
	CHECK_POINTER(l_pcNodeclass, return);

	//Continuous Transition
	SP_DS_Nodeclass* l_pcNodeContinuousTrans;
	l_pcNodeContinuousTrans = m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_TRANS);
	CHECK_POINTER(l_pcNodeContinuousTrans, return);

	//Immediate Transitions
	SP_DS_Nodeclass* l_pcNodeclassforImmediateTrans;
	l_pcNodeclassforImmediateTrans = m_pcGraph->GetNodeclass(SP_DS_IMMEDIATE_TRANS);
	CHECK_POINTER(l_pcNodeclassforImmediateTrans, return);

	//Timed Transition
	SP_DS_Nodeclass* l_pcNodeclassforDeterminTrans;
	l_pcNodeclassforDeterminTrans = m_pcGraph->GetNodeclass(SP_DS_DETERMINISTIC_TRANS);
	CHECK_POINTER(l_pcNodeclassforDeterminTrans, return);

	//Scheduled Transition
	SP_DS_Nodeclass* l_pcNodeclassforScheduedTrans;
	l_pcNodeclassforScheduedTrans = m_pcGraph->GetNodeclass(SP_DS_SCHEDULED_TRANS);
	CHECK_POINTER(l_pcNodeclassforScheduedTrans, return);

	SP_DS_ColListAttribute* l_pcColList = NULL;

	SP_ListNode::const_iterator l_itElem;

	m_pcMainSimulator->ClearTransitions();

	m_asTransitionNames.clear();

	m_mTransitionName2Position.clear();

	unsigned long l_nTransitionPosition = 0;

	//if the user wants to run the drawn net as continuous nets, then stochastic transitions need to be added as continuous
	spsim::TransitionType l_nTransitionType = m_sSimulatorType == wxT("Continuous") ? spsim::TRANSITION_TYPE_CONTINUOUS : spsim::TRANSITION_TYPE_STOCHASTIC;

	SP_DS_FunctionRegistry* l_pcFR = m_pcGraph->GetFunctionRegistry();

	//Stochastic Transition
	for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
	{
		SP_DS_Node* l_pcNode = *l_itElem;
		wxString l_sTransitionName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("FunctionList")));

		wxString l_sTransitionFunction = l_pcColList->GetActiveCellValue(1);

		/**********/
		/*
		string l_str = l_sTransitionFunction.ToStdString();
		typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
		boost::char_separator<char> sep("(+-/%*) ");
		tokenizer tokens(l_str, sep);

		for (tokenizer::iterator beg = tokens.begin(); beg != tokens.end(); ++beg)
		{
			auto pos = m_msParameterName2Value.find(*beg);
			wxString result = *beg;
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
		  wxString l_sToken=tokenize.GetNextToken();
		  auto pos = m_msParameterName2Value.find(l_sToken);
		  if (pos != m_msParameterName2Value.end())
		  	{
		  		double l_dValue = m_msParameterName2Value[l_sToken];
		  		wxString l_sVal;
		  	   l_sVal << l_dValue;
		  	   l_sTransitionFunction.Replace(l_sToken, l_sVal);

		  	}
		}
		/**************/
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

		m_asTransitionNames.push_back(l_sTransitionName);

		m_mTransitionName2Position[l_sTransitionName] = l_nTransitionPosition++;

		m_pcMainSimulator->AddTransition(l_sTransitionName, l_sExpanded, l_nTransitionType);
	}

	//if the user wants to run the drawn net as stochastic nets, then continuous transitions need to be added as stochastic
	l_nTransitionType = m_sSimulatorType == wxT("Stochastic") ? spsim::TRANSITION_TYPE_STOCHASTIC : spsim::TRANSITION_TYPE_CONTINUOUS;

	for (l_itElem = l_pcNodeContinuousTrans->GetElements()->begin(); l_itElem != l_pcNodeContinuousTrans->GetElements()->end(); ++l_itElem)
	{
		SP_DS_Node* l_pcNode = *l_itElem;

		wxString l_sTransitionName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("FunctionList")));

		wxString l_sTransitionFunction = l_pcColList->GetActiveCellValue(1);
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

		m_asTransitionNames.push_back(l_sTransitionName);

		m_mTransitionName2Position[l_sTransitionName] = l_nTransitionPosition++;

		m_pcMainSimulator->AddTransition(l_sTransitionName, l_sExpanded, l_nTransitionType);
	}

	//Immediate Transition
	for (l_itElem = l_pcNodeclassforImmediateTrans->GetElements()->begin(); l_itElem != l_pcNodeclassforImmediateTrans->GetElements()->end(); ++l_itElem)
	{
		SP_DS_Node* l_pcNode = *l_itElem;

		wxString l_sTransitionName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("FunctionList")));
		wxString l_sTransitionFunction = l_pcColList->GetActiveCellValue(1);
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

		m_asTransitionNames.push_back(l_sTransitionName);

		m_mTransitionName2Position[l_sTransitionName] = l_nTransitionPosition++;

		m_pcMainSimulator->AddTransition(l_sTransitionName, l_sExpanded, spsim::TRANSITION_TYPE_IMMEDIATE);
	}

	//Timed Transition
	for (l_itElem = l_pcNodeclassforDeterminTrans->GetElements()->begin(); l_itElem != l_pcNodeclassforDeterminTrans->GetElements()->end(); ++l_itElem)
	{
		SP_DS_Node* l_pcNode = *l_itElem;

		wxString l_sTransitionName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("DelayList")));

		wxString l_delayvalue = l_pcColList->GetActiveCellValue(1);
		SP_FunctionPtr l_pcFunction = l_pcFR->parseFunctionString(l_delayvalue);
		wxString l_sExpanded;
		if (l_pcFunction)
		{
			SP_FunctionPtr l_pcExpanded = l_pcFR->substituteFunctions(l_pcFunction);
			l_sExpanded = l_pcExpanded->toString();
		}
		else
		{
			l_sExpanded = l_delayvalue;
		}

		m_asTransitionNames.push_back(l_sTransitionName);

		m_mTransitionName2Position[l_sTransitionName] = l_nTransitionPosition++;

		m_pcMainSimulator->AddTransition(l_sTransitionName, l_sExpanded, spsim::TRANSITION_TYPE_DETERMINISTIC);
	}

	//Scheduled transition
	for (l_itElem = l_pcNodeclassforScheduedTrans->GetElements()->begin(); l_itElem != l_pcNodeclassforScheduedTrans->GetElements()->end(); ++l_itElem)
	{
		SP_DS_Node* l_pcNode = *l_itElem;

		wxString l_sTransitionName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("PeriodicList")));
		wxString l_Begin = l_pcColList->GetActiveCellValue(1);
		wxString l_Repetition = l_pcColList->GetActiveCellValue(2); //FixedTimedFiring_Single( . ) not realized
		wxString l_End = l_pcColList->GetActiveCellValue(3); //FixedTimedFiring_Periodic( _SimStart, . , _SimEnd )
		wxString l_sTransitionFunction = wxT("FixedTimedFiring_Periodic(") + l_Begin + wxT(",") + l_Repetition + wxT(",") + l_End + wxT(")");
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

		m_asTransitionNames.push_back(l_sTransitionName);

		m_mTransitionName2Position[l_sTransitionName] = l_nTransitionPosition++;

		m_pcMainSimulator->AddTransition(l_sTransitionName, l_sExpanded, spsim::TRANSITION_TYPE_SCHEDULED);
	}
}


void   SP_DLG_FHybridSimulationResults::OnDlgCancel(wxCommandEvent& p_cEvent)
{
	for (auto itVthread = m_fhpnThreadVector.begin(); itVthread != m_fhpnThreadVector.end(); ++itVthread)
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

wxString SP_DLG_FHybridSimulationResults::GetFuzzySettingAttribute(SP_DS_Metadata* p_pcView, const wxString& p_sAttributeName)
{
	CHECK_POINTER(p_pcView, return wxT(""));
	SP_DS_Attribute* l_pcAttribute = p_pcView->GetAttribute(p_sAttributeName);

	CHECK_POINTER(l_pcAttribute, return wxT(""));

	return l_pcAttribute->GetValueString();
}

void  SP_DLG_FHybridSimulationResults::SaveFuzzySimulatorProperties()
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
	long l_nselectedSimulation = m_samplingchoices->GetSelection();
	if (l_nselectedSimulation >= 0)
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

void SP_DLG_FHybridSimulationResults::OnSamplingProperties(wxCommandEvent& p_cEvent)
{

	if (m_pcStartButton->GetLabel() != wxT("Start Simulation"))
	{

		return;
	}
	fuzzification::SAMPLING_TYPE l_typeSampling = GetSelectedSamplingMethod();

	if (l_typeSampling == fuzzification::SAMPLING_TYPE::BASIC || l_typeSampling == fuzzification::SAMPLING_TYPE::REDUCED)//no proporties for basic and reduced samplings
	{
		return;
	}


	if (m_fr == nullptr)
	{
		InitializeFuzzySetting();
	}


	SP_DLG_SamplingProperties* l_pcDlg;

	l_pcDlg = new SP_DLG_SamplingProperties(this, m_fr, m_samplingchoices->GetSelection());

	l_pcDlg->ShowModal();

	l_pcDlg->Destroy();

	SetFocus();


	SP_Core::Instance()->GetRootDocument()->Modify(true);

}

fuzzification::SAMPLING_TYPE SP_DLG_FHybridSimulationResults::GetSelectedSamplingMethod()
{
	if (!m_samplingchoices) fuzzification::SAMPLING_TYPE::INVALID;

	long l_nselectedSimulation = m_samplingchoices->GetSelection();

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

void SP_DLG_FHybridSimulationResults::OnSamplingAlgoChanged(wxCommandEvent& p_evt)
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
