/*
 * SP_GUI_ConnectToServer.cpp
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 08.03.2010
 * Short Description:
 ==============================================================================*/

#include "sp_core/base/SP_Error.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_core/SP_Core.h"

#include "spsa/spsa.h"
#include "sp_gui/dialogs/dia_SteeringGUI/SP_GUI_ConnectToServer.h"

#include "sp_gui/dialogs/dia_SteeringGUI/SP_GUI_SteeringOptions.h"
#include "sp_gui/dialogs/dia_SteeringGUI/SP_GUI_SteeringDialog.h"

#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/attributes/SP_DS_BoolAttribute.h"

#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/attributes/SP_DS_DoubleAttribute.h"
#include "sp_ds/attributes/SP_DS_NumberAttribute.h"

#include "sp_ds/attributes/SP_DS_DoubleMarkingAttribute.h"
#include "sp_ds/attributes/SP_DS_MarkingAttribute.h"

#include "sp_ds/extensions/Col_PN/SyntaxChecking/SP_CPN_SyntaxChecking.h"
#include "sp_ds/extensions/Col_SPN/SP_DS_ColPN_Unfolding.h"
#include "sp_ds/extensions/SP_DS_FunctionEvaluator.h"

#include "wx/busyinfo.h"

using namespace spsa;

enum
{
	SP_ID_BUTTON_CONNECT, SP_ID_BUTTON_PROPERTIES, SP_ID_BUTTON_CLOSE
};
BEGIN_EVENT_TABLE(SP_GUI_ConnectToServer,wxFrame)

EVT_BUTTON(SP_ID_BUTTON_CONNECT,SP_GUI_ConnectToServer::OnConnect)
EVT_BUTTON(SP_ID_BUTTON_PROPERTIES,SP_GUI_ConnectToServer::OnConnectionProperties)
EVT_BUTTON(SP_ID_BUTTON_CLOSE,SP_GUI_ConnectToServer::OnClose)
EVT_BUTTON(wxID_CLOSE, SP_GUI_ConnectToServer::OnClose)


END_EVENT_TABLE()

SP_GUI_ConnectToServer::SP_GUI_ConnectToServer(wxWindow* p_pcParent, const wxString& p_sTitle, long p_nStyle) :
wxFrame(p_pcParent, -1, p_sTitle, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE), m_sPort(wxT("3000")), m_sIP(wxT("Localhost")), m_pcGUIClient(NULL)
{

	//Main Sizer
	wxBoxSizer* l_pcSizer = new wxBoxSizer(wxVERTICAL);

	//Connection Status
	wxBoxSizer* l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);

	m_pcStatusTxtCtrl = new wxStaticText(this, -1, wxT("Disconnected"));
	l_pcRowSizer->Add(m_pcStatusTxtCtrl, 0, wxALL | wxEXPAND, 5);
	m_pcIndicatorGaugeCtrl = new wxGauge(this, 100, -1);
	l_pcRowSizer->Add(m_pcIndicatorGaugeCtrl, 1, wxALL | wxEXPAND || wxRESERVE_SPACE_EVEN_IF_HIDDEN, 5);
	l_pcSizer->Add(l_pcRowSizer, 0, wxEXPAND );

	//Buttons
	l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
	m_pcConnectButtonCtrl = new wxButton(this, SP_ID_BUTTON_CONNECT, wxT("Connect"));
	l_pcRowSizer->Add(m_pcConnectButtonCtrl, 1, wxEXPAND | wxALL, 5);
	m_pcPropertiesButtonCtrl = new wxButton(this, SP_ID_BUTTON_PROPERTIES, wxT("Show Details"));

	l_pcRowSizer->Add(m_pcPropertiesButtonCtrl, 1, wxEXPAND | wxALL, 5);
	m_pcCloseButtonCtrl = new wxButton(this, SP_ID_BUTTON_CLOSE, wxT("Close"));
	l_pcRowSizer->Add(m_pcCloseButtonCtrl, 1, wxEXPAND | wxALL, 5);
	l_pcSizer->Add(l_pcRowSizer, 0, wxEXPAND );

	//Details
	m_pcPropertiesSize = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("Connection Properties")), wxVERTICAL);
	l_pcSizer->Add(m_pcPropertiesSize, 1, wxEXPAND );

	//port
	l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
	m_pcPortStaticTxtCtrl = new wxStaticText(this, -1, wxT("Service"));
	l_pcRowSizer->Add(m_pcPortStaticTxtCtrl, 1, wxEXPAND | wxALL, 5);
	m_pcPortComboCtrl = new wxComboBox(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(100, -1), 0, NULL, wxCB_DROPDOWN);

	l_pcRowSizer->Add(m_pcPortComboCtrl, 1, wxEXPAND | wxALL, 5);
	m_pcPropertiesSize->Add(l_pcRowSizer, 1, wxEXPAND | wxALL, 5);

	//IP
	l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
	m_pcIPStaticTxtCtrl = new wxStaticText(this, -1, wxT("Server"));
	l_pcRowSizer->Add(m_pcIPStaticTxtCtrl, 1, wxEXPAND | wxALL, 5);
	m_pcIPComboCtrl = new wxComboBox(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(100, -1), 0, NULL, wxCB_DROPDOWN);

	l_pcRowSizer->Add(m_pcIPComboCtrl, 1, wxEXPAND | wxALL, 5);
	m_pcPropertiesSize->Add(l_pcRowSizer, 1, wxEXPAND | wxALL, 5);

	m_pcPropertiesSize->Show(false);

	ReadConfig();

	SetSizerAndFit(l_pcSizer);

	m_pcIndicatorGaugeCtrl->Show(false);

	Centre(wxBOTH);

	m_pcGUIClient = new spsa::SteeringClient(m_sIP, m_sPort);

}

SP_GUI_ConnectToServer::~SP_GUI_ConnectToServer()
{
	UpdateConfig();
}

void SP_GUI_ConnectToServer::OnConnect(wxCommandEvent& event)
{
	if (m_pcConnectButtonCtrl->GetLabel() == wxT("Connect"))
	{
		if (m_pcIPComboCtrl->GetValue().empty())
		{
			SP_MESSAGEBOX(wxT("The server name is empty. Cannot make a connection. Please enter/select a server name from show details."), wxT("Information"));
			return;
		}

		if (m_pcPortComboCtrl->GetValue().empty())
		{
			SP_MESSAGEBOX(wxT("The service name is empty. Cannot make a connection. Please enter/select a service name from show details."), wxT("Information"));
			return;
		}

		m_pcConnectButtonCtrl->SetLabel(wxT("Cancel"));

		//update the configurations
		UpdateConfig();

		DisableButtons();
		ConnectToServer();
		EnableButtons();
		m_pcConnectButtonCtrl->SetLabel(wxT("Connect"));

	}
	else
	{
		m_pcConnectButtonCtrl->SetLabel(wxT("Connect"));

		m_pcGUIClient->DisConnectFromServer();

		EnableButtons();
	}

	//Host name
	if (m_pcIPComboCtrl->FindString(m_pcIPComboCtrl->GetValue()) == -1)
	{
		m_pcIPComboCtrl->Insert(m_pcIPComboCtrl->GetValue(), 0);
		m_pcIPComboCtrl->SetSelection(0);
	}

	//Listening port
	if (m_pcPortComboCtrl->FindString(m_pcPortComboCtrl->GetValue()) == -1)
	{
		m_pcPortComboCtrl->Insert(m_pcPortComboCtrl->GetValue(), 0);
		m_pcPortComboCtrl->SetSelection(0);
	}
}
void SP_GUI_ConnectToServer::DisableButtons()
{
	m_pcCloseButtonCtrl->Enable(false);
	m_pcPropertiesButtonCtrl->Enable(false);
	m_pcIndicatorGaugeCtrl->Show(true);
	Fit();

}

void SP_GUI_ConnectToServer::EnableButtons()
{
	m_pcCloseButtonCtrl->Enable(true);
	m_pcPropertiesButtonCtrl->Enable(true);
	m_pcIndicatorGaugeCtrl->Show(false);
	Fit();
}
void SP_GUI_ConnectToServer::OnConnectionProperties(wxCommandEvent& event)
{
	if (m_pcPropertiesButtonCtrl->GetLabel() == wxT("Hide Details"))
	{
		m_pcPropertiesSize->Show(false);
		m_pcPropertiesButtonCtrl->SetLabel(wxT("Show Details"));
	}
	else
	{
		m_pcPropertiesSize->Show(true);
		m_pcPropertiesButtonCtrl->SetLabel(wxT("Hide Details"));
	}
	Fit();

}
void SP_GUI_ConnectToServer::OnClose(wxCommandEvent& event)
{
	/*if (IsModal() )
	{
		EndModal(wxID_CANCEL);
	}
	else
	{
		SetReturnCode(wxID_CANCEL);
		this->Show(false);
	}*/

	this->Close();
}
bool SP_GUI_ConnectToServer::ConnectToServer()
{
	VectorString l_sVector;

	int l_nIsInitialized;
	bool l_bShowTwoOptions = false;

	int l_nIsConnected = FAILURE;

	SP_VectorString l_asModelNames;
	wxString l_sName;

	Model* l_pcModel = NULL;

	//Get Port and IP
	m_sPort = m_pcPortComboCtrl->GetValue();
	m_sIP = m_pcIPComboCtrl->GetValue();

	if (SP_Core::Instance()->GetRootDocument() == NULL)
	{

		l_bShowTwoOptions = false;

	}
	else
	{
		m_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
		CHECK_POINTER(m_pcGraph, return false);

		l_sName = m_pcGraph->GetNetclass()->GetName();

		if ((l_sName == SP_DS_SPN_CLASS) || (l_sName == SP_DS_CONTINUOUSPED_CLASS) || (l_sName == SP_DS_COLSPN_CLASS) || (l_sName == SP_DS_COLCPN_CLASS) || (l_sName == SP_DS_HYBRIDPN_CLASS)
				|| (l_sName == SP_DS_COLHPN_CLASS))
		{
			l_bShowTwoOptions = true;
		}
		else
		{
			l_bShowTwoOptions = false;
		}
	}

	// try to connect to the server
	while (l_nIsConnected != SUCCESS && m_pcConnectButtonCtrl->GetLabel() == wxT("Cancel"))
	{
		m_pcIndicatorGaugeCtrl->Pulse();

		l_nIsConnected = m_pcGUIClient->ConnectToServer(m_sIP, m_sPort, false);

		if (l_nIsConnected == spsa::INVALIDVERSION)
		{
			wxString l_sMsg = wxT("The server and client communication have different versions.");

			SP_MESSAGEBOX(l_sMsg, wxT("Error"));

			return false;
		}

	}

	if (l_nIsConnected == SUCCESS)
	{
		//TODO: implement a new function to get the number of running models
		m_pcGUIClient->GetRuningModelNames(l_asModelNames);

		if (l_bShowTwoOptions == false && l_asModelNames.size() == 0)
		{
			SP_MESSAGEBOX(wxT("No models in the server, please open a model in Snoopy and try again"));

			m_pcGUIClient->DisConnectFromServer();

			return false;
		}

		//Hide this dialog
		//Show(false);

		SP_GUI_SteeringOptions* dlg = new SP_GUI_SteeringOptions(this, m_pcGUIClient, l_bShowTwoOptions, wxT("Steering Options"));

		//to record the initalization time
		wxStopWatch l_sw;

		if (dlg->ShowModal() == wxID_OK)
		{
			if (dlg->GetUserSelection() == 1)
			{
				l_nIsInitialized = m_pcGUIClient->Initialize(dlg->GetSelectedModel());
			}
			else
			{

				if ((l_sName == SP_DS_COLSPN_CLASS) || (l_sName == SP_DS_COLCPN_CLASS) || (l_sName == SP_DS_COLHPN_CLASS))
				{
					//load colored model
					l_pcModel = CreateColoredModel(dlg->GetModelName());

				}
				else
				{
					//load low level model
					l_pcModel = CreateModel(dlg->GetModelName());
				}

				CHECK_POINTER(l_pcModel, return false);

				//set the model net type
				l_pcModel->SetNetType(GetNetType(l_sName));

				//load views from Snoopy into the model
				LoadModelViews(l_pcModel);

				//load simulation properties from Snoopy
				LoadSimulationProperties(l_pcModel);

				wxBusyInfo wait(wxT("Sending the model to the server. Please wait, working..."));

				l_sw.Start();

				l_nIsInitialized = m_pcGUIClient->Initialize(l_pcModel);

				//output the initialization time in debug mode
				SP_LOGDEBUG(wxT("Steering initialization time=") + wxString::Format(wxT("%gs"), (double) (l_sw.Time()) / 1000));
			}

			if (l_nIsInitialized == spsa::SUCCESS)
			{
				SP_GUI_SteeringDialog* l_pcSteeringDlg = new SP_GUI_SteeringDialog(this, m_pcGUIClient, wxT("Steering GUI"));

				l_pcSteeringDlg->Show();

				//this->Show(false);
			}
			else
			{
				SP_MESSAGEBOX(wxT("The connection with the server can not be initialized"));

				if (l_nIsInitialized == spsa::INVALIDVERSION)
				{
					wxString l_sMesage = wxT("The server and the client have incompatible versions. The GUI client version is ");

					l_sMesage << m_pcGUIClient->GetVersion();

					SP_LOGERROR( l_sMesage);
				}

				//delete the model if it has been created
				if (l_pcModel != NULL)
				{
					wxDELETE(l_pcModel);
				}

				return false;
			}
		}
	}

	m_pcIndicatorGaugeCtrl->SetValue(0);

	return true;
}

Model* SP_GUI_ConnectToServer::CreateModel(const wxString& p_sModelName)
{

	Model* l_pcModel;
	VectorString l_asPlaceNames;
	VectorString l_asTransitionNames;

	VectorString l_asTransitionFunctions;

	SP_VectorDouble l_anInitialMarking;

	VectorPlaceType l_aPlaceTypes;
	VectorTransitionType l_aTransitionTypes;

	SP_VectorString l_asParameterNames;
	SP_VectorDouble l_anParameterValues;

	const unsigned int MAX_PLACES = 2;
	const unsigned int MAX_TRANSITIONS = 5;
	const unsigned int MAX_ARCS = 6;

	wxString l_asPlaces[] =
	{ SP_DS_CONTINUOUS_PLACE, SP_DS_DISCRETE_PLACE };
	PlaceType l_aPlaceType[] =
	{ PLACE_TYPE_CONTINUOUS, PLACE_TYPE_DISCRETE };

	wxString l_asTransitions[] =
	{ SP_DS_STOCHASTIC_TRANS, SP_DS_CONTINUOUS_TRANS, SP_DS_IMMEDIATE_TRANS, SP_DS_DETERMINISTIC_TRANS, SP_DS_SCHEDULED_TRANS };
	TransitionType l_aTransitionType[] =
	{ TRANSITION_TYPE_STOCHASTIC, TRANSITION_TYPE_CONTINUOUS, TRANSITION_TYPE_IMMEDIATE, TRANSITION_TYPE_DETERMINISTIC, TRANSITION_TYPE_SCHEDULED };

	wxString l_asArcs[] =
	{ SP_DS_EDGE, SP_DS_INHIBITOR_EDGE, SP_DS_READ_EDGE, SP_DS_EQUAL_EDGE, SP_DS_RESET_EDGE, SP_DS_MODIFIER_EDGE };

	l_asPlaceNames.clear();
	l_anInitialMarking.clear();

	//Load place names
	for (unsigned int l_nPlace = 0; l_nPlace < MAX_PLACES; l_nPlace++)
	{
		unsigned long l_nNodeCount = LoadNodeNames(l_asPlaces[l_nPlace], l_asPlaceNames);

		loadInitialMarking(l_asPlaces[l_nPlace], l_anInitialMarking);

		for (unsigned int l_nType = 0; l_nType < l_nNodeCount; l_nType++)
		{
			l_aPlaceTypes.push_back(l_aPlaceType[l_nPlace]);
		}
	}

	//Load transition names
	for (unsigned int l_nTrans = 0; l_nTrans < MAX_TRANSITIONS; l_nTrans++)
	{
		unsigned long l_nNodeCount = LoadNodeNames(l_asTransitions[l_nTrans], l_asTransitionNames);

		for (unsigned int l_nType = 0; l_nType < l_nNodeCount; l_nType++)
		{
			l_aTransitionTypes.push_back(l_aTransitionType[l_nTrans]);
		}

		//Transitition's function
		LoadTransitionFunctions(l_asTransitions[l_nTrans], l_nTrans, l_asTransitionFunctions);
	}

	l_pcModel = new Model(NULL, p_sModelName, l_asTransitionNames.size(), l_asPlaceNames.size());

	//Load Parameters
	l_asParameterNames.clear();
	l_anParameterValues.clear();

	//for low level nets they use constants
	LoadConstants(l_asParameterNames, l_anParameterValues);

	l_pcModel->SetPlaceNames(l_asPlaceNames);
	l_pcModel->SetTransitionNames(l_asTransitionNames);

	for (unsigned int l_nArc = 0; l_nArc < MAX_ARCS; l_nArc++)
	{
		LoadConnection(l_asArcs[l_nArc], l_nArc, l_pcModel);
	}

	//Set the place Types
	l_pcModel->SetPlaceTypes(l_aPlaceTypes);

	//Set initial marking
	l_pcModel->SetInitialMarking(l_anInitialMarking);

	//Set the transition types
	l_pcModel->SetTransitionTypes(l_aTransitionTypes);

	//Set transition Functions
	l_pcModel->SetTransitionFunctions(l_asTransitionFunctions);

	//set parameters to the model
	l_pcModel->SetParameterNames(l_asParameterNames);
	l_pcModel->SetInitialParameterValues(l_anParameterValues);

	return l_pcModel;
}

unsigned long SP_GUI_ConnectToServer::LoadNodeNames(const wxString& p_sNodeName, SP_VectorString& p_asNames)
{
	const SP_ListNode* l_pcNodes;
	SP_ListNode::const_iterator l_itNode;

	unsigned long l_nNodeCount = 0;

	if (m_pcGraph->GetNodeclass(p_sNodeName) == NULL)
	{
		return 0;
	}

	l_pcNodes = m_pcGraph->GetNodeclass(p_sNodeName)->GetElements();

	for (l_itNode = l_pcNodes->begin(); l_itNode != l_pcNodes->end(); l_itNode++)
	{
		SP_DS_NameAttribute* l_pcNameAttribute = dynamic_cast<SP_DS_NameAttribute*>((*l_itNode)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME));
		p_asNames.push_back(l_pcNameAttribute->GetValue());

		l_nNodeCount++;
	}

	return l_nNodeCount;
}

void SP_GUI_ConnectToServer::loadInitialMarking(const wxString& p_sNodeName, SP_VectorDouble& p_anInitialMarking)
{
	const SP_ListNode* l_pcNodes;
	SP_ListNode::const_iterator l_itNode;

	if (m_pcGraph->GetNodeclass(p_sNodeName) == NULL)
	{
		return;
	}

	l_pcNodes = m_pcGraph->GetNodeclass(p_sNodeName)->GetElements();

	for (l_itNode = l_pcNodes->begin(); l_itNode != l_pcNodes->end(); l_itNode++)
	{
		//Current Marking
		SP_DS_Attribute* l_pcAttribute = (*l_itNode)->GetAttribute(wxT("Marking"));

		CHECK_POINTER(l_pcAttribute, return);

		double l_nValue = 0;

		if (p_sNodeName == SP_DS_CONTINUOUS_PLACE)
		{
			SP_DS_DoubleMarkingAttribute* l_pcMarkAttr = dynamic_cast<SP_DS_DoubleMarkingAttribute*>(l_pcAttribute);
			l_nValue = l_pcMarkAttr->GetValue(true);
		}
		else
		{
			SP_DS_MarkingAttribute* l_pcMarkAttr = dynamic_cast<SP_DS_MarkingAttribute*>(l_pcAttribute);
			l_nValue = l_pcMarkAttr->GetValue(true);
		}

		p_anInitialMarking.push_back(l_nValue);
	}
}

void SP_GUI_ConnectToServer::LoadParameters(SP_VectorString& p_asParameterNames, SP_VectorDouble& p_anParametervalues)
{
	SP_DS_Attribute* l_pcParamAttribute;
	const SP_ListNode* l_pcNodes;
	SP_ListNode::const_iterator l_itNode;

	if (m_pcGraph->GetNodeclass(SP_DS_PARAM) == NULL)
	{
		return;
	}

	l_pcNodes = m_pcGraph->GetNodeclass(SP_DS_PARAM)->GetElements();

	for (l_itNode = l_pcNodes->begin(); l_itNode != l_pcNodes->end(); l_itNode++)
	{
		//Parameter names
		SP_DS_NameAttribute* l_pcNameAttribute = dynamic_cast<SP_DS_NameAttribute*>((*l_itNode)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME));
		p_asParameterNames.push_back(l_pcNameAttribute->GetValue());

		//Current Marking
		l_pcParamAttribute = (*l_itNode)->GetAttribute(wxT("ParameterList"));

		CHECK_POINTER(l_pcParamAttribute, return);

		double l_nParam = (dynamic_cast<SP_DS_ColListAttribute*>(l_pcParamAttribute))->GetActiveCellValueDouble(1);

		p_anParametervalues.push_back(l_nParam);
	}

}

void SP_GUI_ConnectToServer::LoadConstants(SP_VectorString& p_asConstantNames, SP_VectorDouble& p_anConstantValues)
{
	SP_DS_Metadataclass* l_pcMC;
	SP_ListMetadata::const_iterator l_itConst;

	if ((l_pcMC = m_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT)) == NULL)
	{
		return;
	}

	m_msParamName2Values.clear();

	//load values
	for (l_itConst = l_pcMC->GetElements()->begin(); l_itConst != l_pcMC->GetElements()->end(); ++l_itConst)
	{
		SP_DS_Metadata* l_pcConstant = *l_itConst;
		wxString l_sName = dynamic_cast<SP_DS_NameAttribute*>(l_pcConstant->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		p_asConstantNames.push_back(l_sName);

		SP_DS_FunctionRegistry* l_pcFR = m_pcGraph->GetFunctionRegistry();

		SP_DS_FunctionRegistryEntry l_FE = l_pcFR->lookUpFunction(l_sName);
		if(l_FE.IsOk())
		{
			double l_nValue = 0;
			SP_FunctionPtr l_Function = l_FE.getFunction();
			if (l_Function->isValue())
			{
				l_nValue = l_Function->getValue();
			}
			else
			{
				//evaluate string
				wxString l_sType = l_pcConstant->GetAttribute(wxT("Type"))->GetValueString();
				if(l_sType == wxT("int"))
				{
					l_nValue = SP_DS_FunctionEvaluatorLong{l_pcFR, l_Function}();
				}
				else if(l_sType == wxT("double"))
				{
					l_nValue = SP_DS_FunctionEvaluatorDouble{l_pcFR, l_Function}();
				}
			}
			//****************************************************************************************
			p_anConstantValues.push_back(l_nValue);

			m_msParamName2Values[l_sName]=l_nValue;
		}
	}
}

void SP_GUI_ConnectToServer::LoadTransitionFunctions(const wxString& l_pcTransitionName, const unsigned int& p_nAttributeIndex, SP_VectorString& p_asFunctions)
{
	const SP_ListNode* l_pcNodes;
	SP_ListNode::const_iterator l_itNode;

	SP_DS_Attribute* l_pcRateAttribute;

	wxString l_sFunction;

	wxString l_sAttributeName[] =
	{ wxT("FunctionList"), wxT("FunctionList"), wxT("WeightList"), wxT("DelayList"), wxT("PeriodicList") };

	if (m_pcGraph->GetNodeclass(l_pcTransitionName) == NULL)
	{
		return;
	}

	l_pcNodes = m_pcGraph->GetNodeclass(l_pcTransitionName)->GetElements();
	for (l_itNode = l_pcNodes->begin(); l_itNode != l_pcNodes->end(); l_itNode++)
	{
		l_pcRateAttribute = (*l_itNode)->GetAttribute(l_sAttributeName[p_nAttributeIndex]);

		CHECK_POINTER(l_pcRateAttribute, return);

		if (l_sAttributeName[p_nAttributeIndex] == wxT("PeriodicList"))
		{
			SP_DS_ColListAttribute* l_pcColList = (dynamic_cast<SP_DS_ColListAttribute*>(l_pcRateAttribute));

			wxString l_Begin = l_pcColList->GetActiveCellValue(1);
			wxString l_Repetition = l_pcColList->GetActiveCellValue(2); //FixedTimedFiring_Single( . ) not realized
			wxString l_End = l_pcColList->GetActiveCellValue(3); //FixedTimedFiring_Periodic( _SimStart, . , _SimEnd )
			l_sFunction = wxT("FixedTimedFiring_Periodic(") + l_Begin + wxT(",") + l_Repetition + wxT(",") + l_End + wxT(")");
		}
		else
		{
			l_sFunction = (dynamic_cast<SP_DS_ColListAttribute*>(l_pcRateAttribute))->GetActiveCellValue(1);
		}

		p_asFunctions.push_back(l_sFunction);

	}
}

void SP_GUI_ConnectToServer::LoadConnection(const wxString& p_sArcName, unsigned int& p_nIndex, Model* p_pcModel)
{
	const SP_ListEdge* l_pcEdgeList;
	SP_ListEdge::const_iterator l_itEdge;

	wxString l_sSourceNodeName, l_sDestNodeName;

	SP_DS_Node* l_pcSourceNode, *l_pcTargetNode;
	;

	SP_DS_NameAttribute* l_pcNameAttribute;

	ConnectionType l_aConnectionType[] =
	{ STANDARD_ARC, INHIBITOR_ARC, READ_ARC, EQUAL_ARC, RESET_ARC, MODIFIER_ARC, STANDARD_ARC, READ_ARC, INHIBITOR_ARC };

	if (m_pcGraph->GetEdgeclass(p_sArcName) == NULL)
	{
		return;
	}

	l_pcEdgeList = m_pcGraph->GetEdgeclass(p_sArcName)->GetElements();

	for (l_itEdge = l_pcEdgeList->begin(); l_itEdge != l_pcEdgeList->end(); l_itEdge++)
	{
		//Get source Node Name
		l_pcSourceNode = dynamic_cast<SP_DS_Node*>((*l_itEdge)->GetSource());

		CHECK_POINTER(l_pcSourceNode, return);

		l_pcNameAttribute = dynamic_cast<SP_DS_NameAttribute*>(l_pcSourceNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME));

		l_sSourceNodeName = l_pcNameAttribute->GetValue();

		//Get target Node Name
		l_pcTargetNode = dynamic_cast<SP_DS_Node*>((*l_itEdge)->GetTarget());

		CHECK_POINTER(l_pcTargetNode, return);

		l_pcNameAttribute = dynamic_cast<SP_DS_NameAttribute*>(l_pcTargetNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME));

		l_sDestNodeName = l_pcNameAttribute->GetValue();

		if (l_pcSourceNode->GetClassName().Contains(wxT("Transition")) == true)
		{
			p_pcModel->SetTransitionPostConnection(l_sSourceNodeName, l_sDestNodeName, GetEdgeWeight((*l_itEdge)));
		}
		else
		{
			p_pcModel->SetTransitionPreConnection(l_sDestNodeName, l_sSourceNodeName, l_aConnectionType[p_nIndex], GetEdgeWeight((*l_itEdge)));

		}
	}
}

double SP_GUI_ConnectToServer::GetEdgeWeight(SP_DS_Edge* p_pcEdge)
{
	double l_nWeight = 0;

	SP_MapString2Double::iterator l_itConst;

	//Get the Edge weight
	SP_DS_Attribute* l_pcAtt = p_pcEdge->GetAttribute(wxT("Multiplicity"));

	if (l_pcAtt == NULL) //modifier arcs do not have weight
	{
		return 0;
	}

	wxString l_sWeight = l_pcAtt->GetValueString();

	if (!l_sWeight.ToDouble(&l_nWeight))
	{
		if ((l_itConst = m_msParamName2Values.find(l_sWeight)) == m_msParamName2Values.end())
		{
			SP_LOGWARNING( wxT("Marking dependent arc weights are not supported in the steering mode."));

			return 0;
		}

		l_nWeight = (*l_itConst).second;
	}

	return l_nWeight;
}

void SP_GUI_ConnectToServer::UpdateConfig()
{
	wxConfig config(SP_APP_SHORT_NAME, SP_APP_VENDOR);

	int l_nHostNameCount = m_pcIPComboCtrl->GetCount();

	int l_nSelectedHostName = m_pcIPComboCtrl->GetSelection();

	//We save only the last 10 server names.
	l_nHostNameCount = l_nHostNameCount > 10 ? 10 : l_nHostNameCount;

	//Write Server names Count
	config.Write(wxT("HostNamesCount"), l_nHostNameCount);

	config.Flush();

	config.Write(wxT("SelectedHostName"), l_nSelectedHostName);

	config.Flush();

	for (int i = 0; i < l_nHostNameCount; i++)
	{
		config.Write(wxT("HostServers") + wxString::Format(wxT("%d"), i), m_pcIPComboCtrl->GetString(i));
		config.Flush();
	}

	//Listening Port
	int l_nPortsCount = m_pcPortComboCtrl->GetCount();

	int l_nSelectedPort = m_pcPortComboCtrl->GetSelection();

	//We save only the last 10 server names.
	l_nPortsCount = l_nPortsCount > 10 ? 10 : l_nPortsCount;

	//Write Server names Count
	config.Write(wxT("HostPortCount"), l_nPortsCount);

	config.Flush();

	config.Write(wxT("SelectedHostPort"), l_nSelectedPort);

	config.Flush();

	for (int i = 0; i < l_nPortsCount; i++)
	{
		config.Write(wxT("HostPort") + wxString::Format(wxT("%d"), i), m_pcPortComboCtrl->GetString(i));
		config.Flush();
	}

}

void SP_GUI_ConnectToServer::ReadConfig()
{
	wxConfig config(SP_APP_SHORT_NAME, SP_APP_VENDOR);

	wxString l_sServerName;
	int l_nHostNameCount = m_pcIPComboCtrl->GetCount();
	int l_nSelectedHostName = m_pcIPComboCtrl->GetSelection();

	//Host Name
	m_pcIPComboCtrl->Clear();

	if (config.Read(wxT("HostNamesCount"), &l_nHostNameCount))
	{
		if (config.Read(wxT("SelectedHostName"), &l_nSelectedHostName))
		{

		}
		for (int i = 0; i < l_nHostNameCount; i++)

			if (config.Read(wxT("HostServers") + wxString::Format(wxT("%d"), i), &l_sServerName))
			{
				//something wrong happen, we do not add the server name
				if (l_sServerName == wxT(""))
				{
					l_nSelectedHostName = 0;
				}
				else
				{
					m_pcIPComboCtrl->Append(l_sServerName);
				}
			}
	}
	else
	{
		m_pcIPComboCtrl->Append(wxT("localhost"));
	}

	// Listening port
	wxString l_sPort;
	int l_nSelectedPort = m_pcPortComboCtrl->GetSelection();
	int l_nPortsCount = m_pcPortComboCtrl->GetCount();

	m_pcPortComboCtrl->Clear();

	if (config.Read(wxT("HostPortCount"), &l_nPortsCount))
	{
		if (config.Read(wxT("SelectedHostPort"), &l_nSelectedPort))
		{

		}

		for (int i = 0; i < l_nPortsCount; i++)
			if (config.Read(wxT("HostPort") + wxString::Format(wxT("%d"), i), &l_sPort))
			{
				if (l_sPort == wxT(""))
				{
					l_nSelectedPort = 0;
				}
				else
				{
					m_pcPortComboCtrl->Append(l_sPort);
				}
			}
	}
	else
	{
		m_pcPortComboCtrl->Append(wxT("3000"));
	}

	if (m_pcPortComboCtrl->GetCount() == 0)
	{
		m_pcPortComboCtrl->Append(wxT("3000"));

		m_pcIPComboCtrl->Append(wxT("localhost"));
	}

	m_pcPortComboCtrl->SetSelection(l_nSelectedPort);
	m_pcIPComboCtrl->SetSelection(l_nSelectedHostName);

}

Model* SP_GUI_ConnectToServer::CreateColoredModel(const wxString& p_sModelName)
{
	spsa::ColoredModel* l_pcModel = NULL;
	SP_CPN_SyntaxChecking l_cSyntaxChecking;
	SP_DS_ColPN_Unfolding* l_pcUnfoldedNet;
	SP_VectorString l_asParameterNames;
	SP_VectorDouble l_anParameterValues;
	VectorTransitionType l_aTransitionType;
	VectorPlaceType l_aPlaceType;

	if (!l_cSyntaxChecking.SyntaxChecking())
	{
		return NULL;
	}

	//unfold
	l_pcUnfoldedNet = new SP_DS_ColPN_Unfolding;

	if (!l_pcUnfoldedNet->Unfolding())
	{
		wxDELETE(l_pcUnfoldedNet);

		return NULL;
	}

	unsigned long l_nColoredPlaceCount = l_pcUnfoldedNet->GetColoredPlaceNames()->size();

	unsigned long l_nColoredTransitionCount = l_pcUnfoldedNet->GetColoredTransitionNames()->size();

	l_pcModel = new ColoredModel(NULL, p_sModelName, l_pcUnfoldedNet->GetTransitionNames()->size(), l_pcUnfoldedNet->GetPlaceNames()->size(), l_nColoredPlaceCount, l_nColoredTransitionCount);

	//uncolored information

	//set places information
	l_pcModel->SetPlaceNames(*l_pcUnfoldedNet->GetPlaceNames());
	l_pcModel->SetInitialMarking(*l_pcUnfoldedNet->GetCurHybPNMarking());

	//continuous place count: workaround
	unsigned int l_nContinuousPlaceCount = l_pcUnfoldedNet->GetContinuousPlaceCount();
	unsigned int l_nPlaceCount = l_pcUnfoldedNet->GetPlaceNames()->size();

	l_aPlaceType.assign(l_nContinuousPlaceCount, spsa::PLACE_TYPE_CONTINUOUS);

	l_aPlaceType.resize(l_nPlaceCount, spsa::PLACE_TYPE_DISCRETE);

	l_pcModel->SetPlaceTypes(l_aPlaceType);

	//set transition information
	l_pcModel->SetTransitionNames(*l_pcUnfoldedNet->GetTransitionNames());
	l_pcModel->SetTransitionFunctions(*l_pcUnfoldedNet->GetCurRateFunction());

	//load transition connection
	vector<SP_CPN_ContinuousArc>* l_pcPreConnection;
	vector<SP_CPN_ContinuousArc>* l_pcPostConnection;
	vector<SP_CPN_ContinuousArc>::const_iterator l_itConnection;

	//Load transitions' pre place
	l_pcPreConnection = l_pcUnfoldedNet->GetArcs_Place2Transition();
	for (l_itConnection = l_pcPreConnection->begin(); l_itConnection != l_pcPreConnection->end(); l_itConnection++)
	{
		double l_nWeight = 0;

		l_itConnection->m_sMultiplicity.ToDouble(&l_nWeight);

		l_pcModel->SetTransitionPreConnection(l_itConnection->m_nTranPos, l_itConnection->m_nPlaceID, GetConnectionType(l_itConnection->m_sArcClass), l_nWeight);
	}

	//Load transitions' post place
	l_pcPostConnection = l_pcUnfoldedNet->GetArcs_Transition2Place();
	for (l_itConnection = l_pcPostConnection->begin(); l_itConnection != l_pcPostConnection->end(); l_itConnection++)
	{
		double l_nWeight = 0;

		l_itConnection->m_sMultiplicity.ToDouble(&l_nWeight);

		l_pcModel->SetTransitionPostConnection(l_itConnection->m_nTranPos, l_itConnection->m_nPlaceID, l_nWeight);
	}

	//transition types
	SP_VectorString* l_pcTransitionType = l_pcUnfoldedNet->GetTransNodeType();

	l_aTransitionType.assign(l_pcUnfoldedNet->GetTransitionNames()->size(), TRANSITION_TYPE_CONTINUOUS);

	spsa::TransitionType l_nTransitionType;

	for (unsigned long l_nTransPos = 0; l_nTransPos < l_aTransitionType.size(); l_nTransPos++)
	{
		wxString l_sTransType = (*l_pcTransitionType)[l_nTransPos];

		if (l_sTransType == SP_DS_STOCHASTIC_TRANS)
		{
			l_nTransitionType = spsa::TRANSITION_TYPE_STOCHASTIC;
		}
		else if (l_sTransType == SP_DS_CONTINUOUS_TRANS)
		{
			l_nTransitionType = spsa::TRANSITION_TYPE_CONTINUOUS;
		}
		else if (l_sTransType == SP_DS_IMMEDIATE_TRANS)
		{
			l_nTransitionType = spsa::TRANSITION_TYPE_IMMEDIATE;
		}
		else if (l_sTransType == SP_DS_DETERMINISTIC_TRANS)
		{
			l_nTransitionType = spsa::TRANSITION_TYPE_DETERMINISTIC;
		}
		else if (l_sTransType == SP_DS_SCHEDULED_TRANS)
		{
			l_nTransitionType = spsa::TRANSITION_TYPE_SCHEDULED;
		}

		l_aTransitionType[l_nTransPos] = l_nTransitionType;
	}

	l_pcModel->SetTransitionTypes(l_aTransitionType);

	//Load Parameters
	l_asParameterNames.clear();
	l_anParameterValues.clear();

	LoadParameters(l_asParameterNames, l_anParameterValues);

	l_pcModel->SetParameterNames(l_asParameterNames);
	l_pcModel->SetInitialParameterValues(l_anParameterValues);

	//colored information
	//places
	l_pcModel->SetColoredPlaceCount(l_nColoredPlaceCount);

	l_pcModel->SetColoredPlaceNames(*l_pcUnfoldedNet->GetColoredPlaceNames());

	//set the relationship between colored and uncolored places
	for (unsigned long l_nPos = 0; l_nPos < l_nColoredPlaceCount; l_nPos++)
	{
		SP_CPN_CountInterval l_Interval = l_pcUnfoldedNet->GetPlaceCountById(l_nPos);

		l_pcModel->SetColorPlaceRange(l_nPos, l_Interval.m_nLow, l_Interval.m_nUp);
	}

	//transitions
	l_pcModel->SetColoredTransitionCount(l_nColoredTransitionCount);
	l_pcModel->SetColoredTransitionNames(*l_pcUnfoldedNet->GetColoredTransitionNames());

	//set the relationship between colored and uncolored transitions
	for (unsigned long l_nPos = 0; l_nPos < l_nColoredTransitionCount; l_nPos++)
	{
		SP_CPN_CountInterval l_Interval = l_pcUnfoldedNet->GetTransitionCountById(l_nPos);

		l_pcModel->SetColorTransitionRange(l_nPos, l_Interval.m_nLow, l_Interval.m_nUp);
	}

	wxDELETE(l_pcUnfoldedNet);

	return l_pcModel;
}
spsa::ConnectionType SP_GUI_ConnectToServer::GetConnectionType(const wxString& p_sConnectionType)
{
	if (p_sConnectionType == SP_DS_EDGE)
	{
		return spsa::STANDARD_ARC;
	}
	else if (p_sConnectionType == SP_DS_READ_EDGE)
	{
		return spsa::READ_ARC;
	}
	else if (p_sConnectionType == SP_DS_INHIBITOR_EDGE)
	{
		return spsa::INHIBITOR_ARC;
	}
	else if (p_sConnectionType == SP_DS_MODIFIER_EDGE)
	{
		return spsa::MODIFIER_ARC;
	}
	else if (p_sConnectionType == SP_DS_RESET_EDGE)
	{
		return spsa::RESET_ARC;
	}
	else if (p_sConnectionType == SP_DS_EQUAL_EDGE)
	{
		return spsa::EQUAL_ARC;
	}
	else
	{
		SP_LOGERROR( wxT("Unknown connection type,") + p_sConnectionType);
		return spsa::OTHER_CONNECTION;
	}
}

bool SP_GUI_ConnectToServer::LoadModelViews(spsa::Model* p_pcModel)
{
	SP_DS_Metadataclass* l_pcMetadataclass;
	const SP_ListMetadata* l_pcViewList;
	SP_ListMetadata::const_iterator l_itElem;

	unsigned int l_nPos;

	CHECK_POINTER(p_pcModel, return false);
	CHECK_POINTER(m_pcGraph, return false);

	//get the views which are saved in Snoopy file
	l_pcMetadataclass = m_pcGraph->GetMetadataclass(wxT("Plot"));

	CHECK_POINTER(l_pcMetadataclass, return false);

	l_pcViewList = l_pcMetadataclass->GetElements();

	CHECK_POINTER(l_pcViewList, return false);

	l_nPos = 0;

	//clear the veiw list
	p_pcModel->GetViews()->clear();

	for (l_itElem = l_pcViewList->begin(); l_itElem != l_pcViewList->end(); ++l_itElem)
	{
		//create a new view
		spsa::ModelView* l_pcView = LoadView((*l_itElem));

		CHECK_POINTER(l_pcView, return false);

		//add this view to the model
		p_pcModel->GetViews()->push_back(l_pcView);

		//check if this is the current view
		wxString l_sCurrent = GetViewAttributeValue((*l_itElem), wxT("IsCurrent"));

		if (l_sCurrent.IsSameAs(wxT("1")))
		{
			p_pcModel->SetActiveView(l_nPos);
		}

		l_nPos++;
	}

	return true;
}

wxString SP_GUI_ConnectToServer::GetViewAttributeValue(SP_DS_Metadata* p_pcView, const wxString& p_sAttributeName)
{
	CHECK_POINTER(p_pcView, return wxT(""));
	SP_DS_Attribute* l_pcAttribute = p_pcView->GetAttribute(p_sAttributeName);

	CHECK_POINTER(l_pcAttribute, return wxT(""));

	return l_pcAttribute->GetValueString();
}

spsa::ModelView* SP_GUI_ConnectToServer::LoadView(SP_DS_Metadata* p_pcMetadata)
{
	spsa::ModelView* l_pcView;

	CHECK_POINTER(p_pcMetadata, return NULL);

	wxString l_sName = p_pcMetadata->GetAttribute(wxT("Name"))->GetValueString();

	l_pcView = new spsa::ModelView(l_sName);

	CHECK_POINTER(l_pcView, return NULL);

	SP_DS_Attribute* l_pcAttribute = p_pcMetadata->GetAttribute(wxT("CurveInfo"));

	CHECK_POINTER(l_pcAttribute, return NULL);

	SP_DS_ColListAttribute* l_pcCurveInfoList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcAttribute);

	//load curve information
	for (unsigned int l_nRow = 0; l_nRow < l_pcCurveInfoList->GetRowCount(); l_nRow++)
	{
		LoadCurveInfo(l_nRow, l_pcView, p_pcMetadata, l_pcCurveInfoList);
	}

	//load the view attributes
	LoadViewAttributes(l_pcView, p_pcMetadata);

	return l_pcView;
}

void SP_GUI_ConnectToServer::LoadViewAttributes(spsa::ModelView* p_pcView, SP_DS_Metadata* p_pcMetadata)
{
	spsa::Attribute* l_pcViewAttribute;

	//copy old setting
	l_pcViewAttribute = p_pcView->GetAttribute(wxT("VIEWER_TYPE"));
	CHECK_POINTER(l_pcViewAttribute, return);

	wxString l_sViewerType = GetViewAttributeValue(p_pcMetadata, wxT("ViewerType"));

	if (l_sViewerType == wxT("Tabular"))
	{
		(dynamic_cast<AttributeUint*>(l_pcViewAttribute))->SetValue(0);
	}
	else
		if (l_sViewerType == wxT("xyPlot"))
		{
			(dynamic_cast<AttributeUint*>(l_pcViewAttribute))->SetValue(1);
		}
		else
			if (l_sViewerType == wxT("Histogram"))
			{
				(dynamic_cast<AttributeUint*>(l_pcViewAttribute))->SetValue(2);
			}
			else
			{
				(dynamic_cast<AttributeUint*>(l_pcViewAttribute))->SetValue(2);
			}

	//get current view matrix type
	wxString l_sElementType = GetViewAttributeValue(p_pcMetadata, wxT("Nodeclass"));

	l_pcViewAttribute = p_pcView->GetAttribute(wxT("VIEW_MATRIX_TYPE"));
	CHECK_POINTER(l_pcViewAttribute, return);

	if (l_sElementType == wxT("Place"))
	{
		(dynamic_cast<AttributeUint*>(l_pcViewAttribute))->SetValue(0);
	}
	else
	{
		(dynamic_cast<AttributeUint*>(l_pcViewAttribute))->SetValue(1);
	}

	//view title
	wxString l_sTitle = GetViewAttributeValue(p_pcMetadata, wxT("ViewTitle"));

	l_pcViewAttribute = p_pcView->GetAttribute(wxT("VIEW_TITLE"));
	CHECK_POINTER(l_pcViewAttribute, return);

	l_pcViewAttribute->SetValueString(l_sTitle);

	//x-axis title
	wxString l_sXAxisTitle = GetViewAttributeValue(p_pcMetadata, wxT("XAxisTitle"));

	l_pcViewAttribute = p_pcView->GetAttribute(wxT("X_AXIS_LABEL"));
	CHECK_POINTER(l_pcViewAttribute, return);

	l_pcViewAttribute->SetValueString(l_sXAxisTitle);

	//y-axis title
	wxString l_sYAxisTitle = GetViewAttributeValue(p_pcMetadata, wxT("YAxisTitle"));

	l_pcViewAttribute = p_pcView->GetAttribute(wxT("Y_AXIS_LABEL"));
	CHECK_POINTER(l_pcViewAttribute, return);

	l_pcViewAttribute->SetValueString(l_sYAxisTitle);

}
void SP_GUI_ConnectToServer::LoadCurveInfo(const unsigned int &p_nCurvePos, spsa::ModelView* p_pcView, SP_DS_Metadata* p_pcMetadata, SP_DS_ColListAttribute* p_pcCurveInfoList)
{
	wxString l_sName;
	wxString l_sOutType;
	unsigned long l_nPosition = 0;
	spsa::RM_ROW_TYPE l_nCurveType;
	long l_nShow;

	wxString l_sColorName;

	long l_nLineWidth;

	long l_nLineStyle;

	//position
	wxString l_sPos = p_pcCurveInfoList->GetCell(p_nCurvePos, 0);
	l_sPos.ToULong(&l_nPosition);

	//curve type (colored/uncolored)
	l_sOutType = p_pcCurveInfoList->GetCell(p_nCurvePos, 1);

	//show
	wxString l_sShow = p_pcCurveInfoList->GetCell(p_nCurvePos, 2);
	l_sShow.ToLong(&l_nShow);

	//curve color
	l_sColorName = p_pcCurveInfoList->GetCell(p_nCurvePos, 3);

	//view element type (place/transition)
	wxString l_sElementType = GetViewAttributeValue(p_pcMetadata, wxT("Nodeclass"));

	wxString l_sLineWidth = p_pcCurveInfoList->GetCell(p_nCurvePos, 4);

	l_sLineWidth.ToLong(&l_nLineWidth);

	wxString l_sLineStyle = p_pcCurveInfoList->GetCell(p_nCurvePos, 5);

	l_sLineStyle.ToLong(&l_nLineStyle);

	//map curve type
	if (l_sOutType == wxT("Unfolded") && l_sElementType.IsSameAs(wxT("Place")))
	{
		l_nCurveType = spsa::RM_ROW_TYPE_UNCOLORED_PLACE;
	}
	else
		if (l_sOutType == wxT("Unfolded") && l_sElementType.IsSameAs(wxT("Transition")))
		{
			l_nCurveType = spsa::RM_ROW_TYPE_UNCOLORED_TRANSITION;
		}
		else
			if (l_sOutType == wxT("Colored") && l_sElementType.IsSameAs(wxT("Place")))
			{
				l_nCurveType = spsa::RM_ROW_TYPE_COLORED_PLACE;
			}
			else
				if (l_sOutType == wxT("Colored") && l_sElementType.IsSameAs(wxT("Transition")))
				{
					l_nCurveType = spsa::RM_ROW_TYPE_COLORED_TRANSITION;
				}

	//Curve Name
	l_sName = p_pcCurveInfoList->GetCell(p_nCurvePos, 6);

	//add this curve to the curve list
	p_pcView->AddNewCurve(l_nPosition, l_sName, l_nLineWidth, l_nLineStyle, l_nCurveType, l_sColorName, l_nShow);
}
//load simulation properties
void SP_GUI_ConnectToServer::LoadSimulationProperties(spsa::Model* p_pcModel)
{
	SP_DS_Metadataclass* l_pcMetadataclass;
	SP_DS_Metadata* l_pcMetadata;

	CHECK_POINTER(p_pcModel, return);
	CHECK_POINTER(m_pcGraph, return);

	//get the views which are saved in Snoopy file
	l_pcMetadataclass = m_pcGraph->GetMetadataclass(wxT("Simulation Properties"));

	CHECK_POINTER(l_pcMetadataclass, return);

	//get the first element
	l_pcMetadata = (*l_pcMetadataclass->GetElements()->begin());

	CHECK_POINTER(l_pcMetadata, return);

	//get interval start
	SP_DS_Attribute* l_pcAttribute = l_pcMetadata->GetAttribute(wxT("interval start"));
	CHECK_POINTER(l_pcAttribute, return);

	double l_nIntervalStart = dynamic_cast<SP_DS_DoubleAttribute*>(l_pcAttribute)->GetValue();

	p_pcModel->SetOutputStartPoint(l_nIntervalStart);

	//interval end
	l_pcAttribute = l_pcMetadata->GetAttribute(wxT("interval end"));
	CHECK_POINTER(l_pcAttribute, return);

	double l_nIntervalEnd = dynamic_cast<SP_DS_DoubleAttribute*>(l_pcAttribute)->GetValue();

	p_pcModel->SetOutputEndPoint(l_nIntervalEnd);

	//step size
	l_pcAttribute = l_pcMetadata->GetAttribute(wxT("output step"));
	CHECK_POINTER(l_pcAttribute, return);

	long l_nNumberOfPoints = dynamic_cast<SP_DS_NumberAttribute*>(l_pcAttribute)->GetValue();

	double l_nOutputSampleSize = (l_nIntervalEnd - l_nIntervalStart) / l_nNumberOfPoints;

	p_pcModel->SetOutputSamplingSize(l_nOutputSampleSize);
}

spsa::NET_TYPE SP_GUI_ConnectToServer::GetNetType(const wxString& p_sNetExt)
{
	//stochatic
	if (p_sNetExt == SP_DS_SPN_CLASS || p_sNetExt == SP_DS_COLSPN_CLASS)
	{
		return spsa::NET_TYPE_STOCHASTIC;
	}
	else
		if (p_sNetExt == SP_DS_CONTINUOUSPED_CLASS || p_sNetExt == SP_DS_COLCPN_CLASS)
		{
			return spsa::NET_TYPE_CONTINUOUS;
		}
		else
			if (p_sNetExt == SP_DS_HYBRIDPN_CLASS || p_sNetExt == SP_DS_COLHPN_CLASS)
			{
				return spsa::NET_TYPE_HYBRID;
			}
			else
			{
				return spsa::NET_TYPE_UNKNOWN;
			}
}

