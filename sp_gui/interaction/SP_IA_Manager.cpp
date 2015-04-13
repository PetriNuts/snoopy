//////////////////////////////////////////////////////////////////////
// $Source: SP_IA_Manager.cpp $
// $Author: MD $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2006/05/29 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#include "SP_IA_Manager.h"

#include "snoopy.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gui/mdi/SP_MDI_View.h"
#include "sp_core/base/SP_ElementClass.h"
#include "sp_ds/SP_DS_Graph.h"

//for command creation **************************************************************
#include "sp_gui/interaction/data_processors/SP_IA_DP_MapperByAttribute.h"
#include "sp_gui/interaction/data_processors/SP_IA_DP_FilterByClass.h"
#include "sp_gui/interaction/data_processors/SP_IA_DP_FilterByAttribute.h"
#include "sp_gui/interaction/data_processors/SP_IA_DP_ConverterValue2Quantity.h"
#include "sp_gui/interaction/data_processors/SP_IA_DP_ConverterQuantity2Value.h"
#include "sp_gui/interaction/data_processors/SP_IA_DP_SetterByClass.h"
#include "sp_gui/interaction/data_processors/SP_IA_DP_SetterByPosition.h"
#include "sp_gui/interaction/data_processors/SP_IA_DP_AttributeTranslator.h"
#include "sp_gui/interaction/data_processors/SP_IA_DP_ElementCreator.h"
#include "sp_gui/interaction/data_processors/SP_IA_DP_RGEdgeSourceSetter.h"
#include "sp_ds/attributes/SP_DS_ListAttribute.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"

#include "sp_gui/interaction/commands/SP_IA_Command_Colorize.h"
#include "sp_gui/interaction/commands/SP_IA_Command_Delete.h"
#include "sp_gui/interaction/commands/SP_IA_Command_SetAttribute.h"
#include "sp_gui/interaction/commands/SP_IA_Command_Insert.h"
//***********************************************************************************

SP_IA_Manager::SP_IA_Manager() {

	m_lCommandHistory.clear();
	m_lEnabledConnections.clear();

	SP_IA_Event *event;
	SP_ListData l_lEventElements;

	//****************************************************************************
	//petri net to reachability graph
	//****************************************************************************
	l_lEventElements.clear();
	l_lEventElements.push_back(new SP_DS_Node(new SP_DS_Nodeclass(0, wxT("Place"))));
	event = new SP_IA_Event(&l_lEventElements, SP_IA_EVT_SINGLE, true);

	SP_IA_Command *l_lCommand = new SP_IA_Command_Colorize(wxT("PN->RG: Place to containing state"));
	l_lCommand->SetTriggerEvent(event);
	l_lCommand->SetDescription(wxT("Colorizing all states in the reachability graph that contain the selected place"));
	l_lCommand->SetNetclassNames(SP_DS_PN_CLASS, SP_DS_REACHABILITY_CLASS);
	l_lCommand->AddDataProcessor(new SP_IA_DP_MapperByAttribute(new SP_DS_ListAttribute(wxT("Places"), wxT(""), false), SP_IA_CONTAINS, new SP_DS_TextAttribute(wxT("Name"))));

	RegisterCommand(l_lCommand);

	//****************************************************************************
	l_lEventElements.clear();
	l_lEventElements.push_back(new SP_DS_Node(new SP_DS_Nodeclass(0, wxT("Place"))));
	event = new SP_IA_Event(&l_lEventElements, SP_IA_EVT_MAKRO, true);

	l_lCommand = new SP_IA_Command_Colorize(wxT("PN->RG: Marked places to containing state"), *wxCYAN, true);
	l_lCommand->SetTriggerEvent(event);
	l_lCommand->SetDescription(wxT("Colorizing all states in the reachability graph that contain all of the selected places, also obeying their marking"));
	l_lCommand->SetNetclassNames(SP_DS_PN_CLASS, SP_DS_REACHABILITY_CLASS);
	l_lCommand->AddDataProcessor(new SP_IA_DP_ConverterValue2Quantity(new SP_DS_NumberAttribute(wxT("Marking"))));
	l_lCommand->AddDataProcessor(new SP_IA_DP_MapperByAttribute(new SP_DS_ListAttribute(wxT("Places"), wxT(""), false), SP_IA_CONTAINS_CMPL, new SP_DS_TextAttribute(wxT("Name"))));

	RegisterCommand(l_lCommand);

	//****************************************************************************
	l_lEventElements.clear();
	//adding a dummy-element for holding the source graph
	l_lEventElements.push_back(new SP_DS_Node(new SP_DS_Nodeclass(0, wxT(""))));
	event = new SP_IA_Event(&l_lEventElements, SP_IA_EVT_GENERAL, true);

	l_lCommand = new SP_IA_Command_Colorize(wxT("PN->RG: Current state"), *wxGREEN, true);
	l_lCommand->SetTriggerEvent(event);
	l_lCommand->SetDescription(wxT("Colorizing the state which is defined by the current marking of the petri net"));
	l_lCommand->SetNetclassNames(SP_DS_PN_CLASS, SP_DS_REACHABILITY_CLASS);
	l_lCommand->AddDataProcessor(new SP_IA_DP_SetterByClass(new SP_DS_Nodeclass(0, wxT("Place"))));
	l_lCommand->AddDataProcessor(new SP_IA_DP_FilterByAttribute(SP_IA_IS_GREATER_THAN, new SP_DS_NumberAttribute(wxT("Marking"), 0)));
	l_lCommand->AddDataProcessor(new SP_IA_DP_ConverterValue2Quantity(new SP_DS_NumberAttribute(wxT("Marking"))));
	l_lCommand->AddDataProcessor(new SP_IA_DP_MapperByAttribute(new SP_DS_ListAttribute(wxT("Places"), wxT(""), false), SP_IA_EQUALS, new SP_DS_TextAttribute(wxT("Name"))));

	RegisterCommand(l_lCommand);

	//****************************************************************************
	l_lEventElements.clear();
	l_lEventElements.push_back(new SP_DS_Node(new SP_DS_Nodeclass(0, wxT("Transition"))));
	event = new SP_IA_Event(&l_lEventElements, SP_IA_EVT_SINGLE, true);

	l_lCommand = new SP_IA_Command_Colorize(wxT("PN->RG: Transition to Edge"), *wxGREEN, true);
	l_lCommand->SetTriggerEvent(event);
	l_lCommand->SetDescription(wxT("Colorizing all edges in the reachability graph that correspond to the selected transition"));
	l_lCommand->SetNetclassNames(SP_DS_PN_CLASS, SP_DS_REACHABILITY_CLASS);
	l_lCommand->AddDataProcessor(new SP_IA_DP_MapperByAttribute(new SP_DS_TextAttribute(wxT("Transition")), SP_IA_EQUALS, new SP_DS_TextAttribute(wxT("Name"))));

	RegisterCommand(l_lCommand);

	//****************************************************************************
	l_lEventElements.clear();
	l_lEventElements.push_back(new SP_DS_Node(new SP_DS_Nodeclass(0, wxT("Transition"))));
	event = new SP_IA_Event(&l_lEventElements, SP_IA_EVT_PEDANIMPRESTEP_FWD, true);

	l_lCommand = new SP_IA_Command_Colorize(wxT("ANIMPRESTEP_FWD PN->RG: Firing transition"), *wxGREEN);
	l_lCommand->SetTriggerEvent(event);
	l_lCommand->SetDescription(wxT("Colorizing the resulting state of the made animation forward-step"));
	l_lCommand->SetNetclassNames(SP_DS_PN_CLASS, SP_DS_REACHABILITY_CLASS);
	SP_IA_DataProcessor *l_DP = new SP_IA_DP_SetterByClass(new SP_DS_Nodeclass(0, wxT("Place")), false, false);
	l_DP->SetProcessSelected(true);
	l_lCommand->AddDataProcessor(l_DP);
	l_lCommand->AddDataProcessor(new SP_IA_DP_FilterByAttribute(SP_IA_IS_GREATER_THAN, new SP_DS_NumberAttribute(wxT("Marking"), 0)));
	l_lCommand->AddDataProcessor(new SP_IA_DP_ConverterValue2Quantity(new SP_DS_NumberAttribute(wxT("Marking"))));
	l_lCommand->AddDataProcessor(new SP_IA_DP_MapperByAttribute(new SP_DS_ListAttribute(wxT("Places"), wxT(""), false), SP_IA_EQUALS, new SP_DS_TextAttribute(wxT("Name"))));
	l_lCommand->AddDataProcessor(new SP_IA_DP_SetterByPosition(SP_IA_NEXT));
	//the following command processor sets the current result as mapping basis
	l_lCommand->AddDataProcessor(new SP_IA_DP_FilterByAttribute(SP_IA_MAP_SELECTED));
	l_lCommand->AddDataProcessor(new SP_IA_DP_FilterByClass(new SP_DS_Nodeclass(0, wxT("Transition"))), false);
	l_lCommand->AddDataProcessor(new SP_IA_DP_MapperByAttribute(new SP_DS_TextAttribute(wxT("Transition")), SP_IA_EQUALS, new SP_DS_TextAttribute(wxT("Name")), true));

	RegisterCommand(l_lCommand);

	//****************************************************************************/
	l_lEventElements.clear();
	l_lEventElements.push_back(new SP_DS_Node(new SP_DS_Nodeclass(0, wxT("Transition"))));
	event = new SP_IA_Event(&l_lEventElements, SP_IA_EVT_SINGLE, true);

	l_lCommand = new SP_IA_Command_Colorize(wxT("PN->RG: Transition to enabling state"), *wxGREEN);
	l_lCommand->SetTriggerEvent(event);
	l_lCommand->SetDescription(wxT("Colorizing all rg-nodes (states) where the selected transition is enabled"));
	l_lCommand->SetNetclassNames(SP_DS_PN_CLASS, SP_DS_REACHABILITY_CLASS);
	l_lCommand->AddDataProcessor(new SP_IA_DP_MapperByAttribute(new SP_DS_TextAttribute(wxT("Transition")), SP_IA_EQUALS, new SP_DS_TextAttribute(wxT("Name"))));
	l_lCommand->AddDataProcessor(new SP_IA_DP_SetterByPosition(SP_IA_PREV));

	RegisterCommand(l_lCommand);

	//****************************************************************************
	l_lEventElements.clear();
	//adding a dummy-element for holding the source graph
	l_lEventElements.push_back(new SP_DS_Node(new SP_DS_Nodeclass(0, wxT(""))));
	event = new SP_IA_Event(&l_lEventElements, SP_IA_EVT_GENERAL, true);

	l_lCommand = new SP_IA_Command_Insert(wxT("PN->RG: Create current state"));
	l_lCommand->SetTriggerEvent(event);
	l_lCommand->SetDescription(wxT("Creating a new state for the current marking"));
	l_lCommand->SetNetclassNames(SP_DS_PN_CLASS, SP_DS_REACHABILITY_CLASS);
	l_lCommand->AddDataProcessor(new SP_IA_DP_SetterByClass(new SP_DS_Nodeclass(0, wxT("Place"))));
	l_lCommand->AddDataProcessor(new SP_IA_DP_FilterByAttribute(SP_IA_IS_GREATER_THAN, new SP_DS_NumberAttribute(wxT("Marking"), 0)));
	l_lCommand->AddDataProcessor(new SP_IA_DP_ConverterValue2Quantity(new SP_DS_NumberAttribute(wxT("Marking"))));
	l_lCommand->AddDataProcessor(new SP_IA_DP_AttributeTranslator(new SP_DS_ListAttribute(wxT("Places"), wxT(""), false), SP_IA_EQUALS, new SP_DS_TextAttribute(wxT("Name"))));
	l_lCommand->AddDataProcessor(new SP_IA_DP_ElementCreator(new SP_DS_Nodeclass(0, wxT("Node"))));

	//would be nicer in a dialog to choose...
	dynamic_cast<SP_IA_Command_Insert*>(l_lCommand)->SetSearchAttributeName(wxT("Places"));

	RegisterCommand(l_lCommand);

	//****************************************************************************
	l_lEventElements.clear();
	l_lEventElements.push_back(new SP_DS_Node(new SP_DS_Nodeclass(0, wxT("Transition"))));
	event = new SP_IA_Event(&l_lEventElements, SP_IA_EVT_PEDANIMSTEP_FWD, true);

	l_lCommand = new SP_IA_Command_Colorize(wxT("ANIMSTEP_FWD PN->RG: Resulting state"), *wxGREEN);
	l_lCommand->SetTriggerEvent(event);
	l_lCommand->SetDescription(wxT("Colorizing the resulting state of the made animation forward-step"));
	l_lCommand->SetNetclassNames(SP_DS_PN_CLASS, SP_DS_REACHABILITY_CLASS);
	l_lCommand->AddDataProcessor(new SP_IA_DP_SetterByClass(new SP_DS_Nodeclass(0, wxT("Place"))));
	l_lCommand->AddDataProcessor(new SP_IA_DP_FilterByAttribute(SP_IA_IS_GREATER_THAN, new SP_DS_NumberAttribute(wxT("Marking"), 0)));
	l_lCommand->AddDataProcessor(new SP_IA_DP_ConverterValue2Quantity(new SP_DS_NumberAttribute(wxT("Marking"))));
	l_lCommand->AddDataProcessor(new SP_IA_DP_MapperByAttribute(new SP_DS_ListAttribute(wxT("Places"), wxT(""), false), SP_IA_EQUALS, new SP_DS_TextAttribute(wxT("Name"))));

	RegisterCommand(l_lCommand);

	//****************************************************************************/
	l_lEventElements.clear();
	l_lEventElements.push_back(new SP_DS_Node(new SP_DS_Nodeclass(0, wxT("Transition"))));
	event = new SP_IA_Event(&l_lEventElements, SP_IA_EVT_PEDANIMSTEP_BWD, true);

	l_lCommand = new SP_IA_Command_Colorize(wxT("ANIMSTEP_BWD PN->RG: Resulting state"), *wxGREEN);
	l_lCommand->SetTriggerEvent(event);
	l_lCommand->SetDescription(wxT("Colorizing the resulting state of the made animation backward-step"));
	l_lCommand->SetNetclassNames(SP_DS_PN_CLASS, SP_DS_REACHABILITY_CLASS);
	l_lCommand->AddDataProcessor(new SP_IA_DP_SetterByClass(new SP_DS_Nodeclass(0, wxT("Place"))));
	l_lCommand->AddDataProcessor(new SP_IA_DP_FilterByAttribute(SP_IA_IS_GREATER_THAN, new SP_DS_NumberAttribute(wxT("Marking"), 0)));
	l_lCommand->AddDataProcessor(new SP_IA_DP_ConverterValue2Quantity(new SP_DS_NumberAttribute(wxT("Marking"))));
	l_lCommand->AddDataProcessor(new SP_IA_DP_MapperByAttribute(new SP_DS_ListAttribute(wxT("Places"), wxT(""), false), SP_IA_EQUALS, new SP_DS_TextAttribute(wxT("Name"))));

	RegisterCommand(l_lCommand);

	//****************************************************************************/
	l_lEventElements.clear();
	l_lEventElements.push_back(new SP_DS_Node(new SP_DS_Nodeclass(0, wxT("Transition"))));
	event = new SP_IA_Event(&l_lEventElements, SP_IA_EVT_PEDANIMSTEP_FWD, true);

	l_lCommand = new SP_IA_Command_Insert(wxT("ANIMSTEP_FWD PN->RG: Create resulting state"), 50, 50);
	l_lCommand->SetTriggerEvent(event);
	l_lCommand->SetDescription(wxT("creating a node for the resulting state of the made animation step\n\n")
															wxT("Note: This command can be used for semi-automatic construction of reachability graphs.\n")
															wxT("Single stepping must be enabled for an accurate result."));
	l_lCommand->SetNetclassNames(SP_DS_PN_CLASS, SP_DS_REACHABILITY_CLASS);
	l_DP = new SP_IA_DP_SetterByClass(new SP_DS_Nodeclass(0, wxT("Place")), false, false);
	l_DP->SetProcessSelected(true);
	l_lCommand->AddDataProcessor(l_DP);
	l_lCommand->AddDataProcessor(new SP_IA_DP_FilterByAttribute(SP_IA_IS_GREATER_THAN, new SP_DS_NumberAttribute(wxT("Marking"), 0)));
	l_lCommand->AddDataProcessor(new SP_IA_DP_ConverterValue2Quantity(new SP_DS_NumberAttribute(wxT("Marking"))));
	l_lCommand->AddDataProcessor(new SP_IA_DP_AttributeTranslator(new SP_DS_ListAttribute(wxT("Places"), wxT(""), false), SP_IA_EQUALS, new SP_DS_TextAttribute(wxT("Name"))));
	l_lCommand->AddDataProcessor(new SP_IA_DP_ElementCreator(new SP_DS_Nodeclass(0, wxT("Node"))));
	l_lCommand->AddDataProcessor(new SP_IA_DP_RGEdgeSourceSetter());
	l_lCommand->AddDataProcessor(new SP_IA_DP_FilterByClass(new SP_DS_Nodeclass(0, wxT("Transition")), SP_IA_SELECTED), false);
	l_DP = new SP_IA_DP_AttributeTranslator(new SP_DS_TextAttribute(wxT("Transition")), SP_IA_EQUALS, new SP_DS_TextAttribute(wxT("Name")));
	l_DP->SetProcessSelected(true);
	l_lCommand->AddDataProcessor(l_DP, false);
	l_lCommand->AddDataProcessor(new SP_IA_DP_ElementCreator(0, new SP_DS_Edgeclass(0, wxT("Edge"))));

	//would be nicer in a dialog to choose...
	dynamic_cast<SP_IA_Command_Insert*>(l_lCommand)->SetSearchAttributeName(wxT("Places"));
	dynamic_cast<SP_IA_Command_Insert*>(l_lCommand)->SetColor(*wxGREEN);

	RegisterCommand(l_lCommand);

	//****************************************************************************/
	//reachability graph to petri net
	//****************************************************************************
	l_lEventElements.clear();
	l_lEventElements.push_back(new SP_DS_Node(new SP_DS_Nodeclass(0, wxT("Node"))));
	event = new SP_IA_Event(&l_lEventElements, SP_IA_EVT_SINGLE, true);

	l_lCommand = new SP_IA_Command_Colorize(wxT("RG->PN: State to places"), *wxGREEN);
	l_lCommand->SetTriggerEvent(event);
	l_lCommand->SetDescription(wxT("Colorizing all places of the petri net that are contained in the selected state"));
	l_lCommand->SetNetclassNames(SP_DS_REACHABILITY_CLASS, SP_DS_PN_CLASS);
	l_lCommand->AddDataProcessor(new SP_IA_DP_MapperByAttribute(new SP_DS_TextAttribute(wxT("Name")), SP_IA_IS_CONTAINED, new SP_DS_ListAttribute(wxT("Places"), wxT(""), false)));
	l_lCommand->AddDataProcessor(new SP_IA_DP_FilterByClass(new SP_ElementClass(SP_ELEMENT_NODECLASS, wxT("Place"))));

	RegisterCommand(l_lCommand);

	//****************************************************************************
	l_lEventElements.clear();
	l_lEventElements.push_back(new SP_DS_Node(new SP_DS_Nodeclass(0, wxT("Node"))));
	event = new SP_IA_Event(&l_lEventElements, SP_IA_EVT_SINGLE, true);

	l_lCommand = new SP_IA_Command_Colorize(wxT("RG->PN: State to enabled transitions"), *wxGREEN);
	l_lCommand->SetTriggerEvent(event);
	l_lCommand->SetDescription(wxT("Colorizing all transitions of the petri net that are enabled in the selected state"));
	l_lCommand->SetNetclassNames(SP_DS_REACHABILITY_CLASS, SP_DS_PN_CLASS);
	l_lCommand->AddDataProcessor(new SP_IA_DP_SetterByPosition(SP_IA_NEXT));
	l_lCommand->AddDataProcessor(new SP_IA_DP_MapperByAttribute(new SP_DS_TextAttribute(wxT("Name")), SP_IA_EQUALS, new SP_DS_TextAttribute(wxT("Transition"))));
	l_lCommand->AddDataProcessor(new SP_IA_DP_FilterByClass(new SP_ElementClass(SP_ELEMENT_NODECLASS, wxT("Transition"))));

	RegisterCommand(l_lCommand);

	//****************************************************************************
	l_lEventElements.clear();
	l_lEventElements.push_back(new SP_DS_Edge(new SP_DS_Edgeclass(0, wxT("Edge"))));
	event = new SP_IA_Event(&l_lEventElements, SP_IA_EVT_SINGLE, true);

	l_lCommand = new SP_IA_Command_Colorize(wxT("RG->PN: Edge to transition"), *wxGREEN);
	l_lCommand->SetTriggerEvent(event);
	l_lCommand->SetDescription(wxT("Colorizing the transition in the petri net that corresponds to the selected edge"));
	l_lCommand->SetNetclassNames(SP_DS_REACHABILITY_CLASS, SP_DS_PN_CLASS);
	l_lCommand->AddDataProcessor(new SP_IA_DP_MapperByAttribute(new SP_DS_TextAttribute(wxT("Name")), SP_IA_EQUALS, new SP_DS_TextAttribute(wxT("Transition"))));
	l_lCommand->AddDataProcessor(new SP_IA_DP_FilterByClass(new SP_ElementClass(SP_ELEMENT_NODECLASS, wxT("Transition"))));

	RegisterCommand(l_lCommand);

	//****************************************************************************
	l_lEventElements.clear();
	l_lEventElements.push_back(new SP_DS_Node(new SP_DS_Nodeclass(0, wxT("Node"))));
	event = new SP_IA_Event(&l_lEventElements, SP_IA_EVT_SINGLE, true);

	l_lCommand = new SP_IA_Command_SetAttribute(wxT("RG->PN: State to marking of places"), new SP_DS_NumberAttribute(wxT("Marking"), 0), true, false);
	l_lCommand->SetTriggerEvent(event);
	l_lCommand->SetDescription(wxT("Setting petri net marking according to the selected state of the reachability graph"));
	l_lCommand->SetNetclassNames(SP_DS_REACHABILITY_CLASS, SP_DS_PN_CLASS);
	l_lCommand->AddDataProcessor(new SP_IA_DP_MapperByAttribute(new SP_DS_TextAttribute(wxT("Name")), SP_IA_IS_CONTAINED, new SP_DS_ListAttribute(wxT("Places"), wxT(""), false)));
	l_lCommand->AddDataProcessor(new SP_IA_DP_FilterByClass(new SP_ElementClass(SP_ELEMENT_NODECLASS, wxT("Place"))));
	l_lCommand->AddDataProcessor(new SP_IA_DP_ConverterQuantity2Value(new SP_DS_NumberAttribute(wxT("Marking"))));
	l_lCommand->AddDataProcessor(new SP_IA_DP_SetterByClass(new SP_DS_Nodeclass(0, wxT("Place")), true, false));

	RegisterCommand(l_lCommand);

	//****************************************************************************
	//SAMPLE COMMANDS
	//****************************************************************************
	l_lEventElements.clear();
	l_lEventElements.push_back(new SP_DS_Node(new SP_DS_Nodeclass(0, wxT("Place"))));
	event = new SP_IA_Event(&l_lEventElements, SP_IA_EVT_SINGLE, true);

	l_lCommand = new SP_IA_Command_Colorize(wxT("SAMPLE PN->PN: Ascending Edge"), *wxBLUE);
	l_lCommand->SetTriggerEvent(event);
	l_lCommand->SetDescription(wxT("colorizes ascending edge"));
	l_lCommand->SetNetclassNames(SP_DS_PN_CLASS, SP_DS_PN_CLASS);
	l_lCommand->AddDataProcessor(new SP_IA_DP_SetterByPosition(SP_IA_NEXT));

	RegisterCommand(l_lCommand);

	//****************************************************************************/
	l_lEventElements.clear();
	l_lEventElements.push_back(new SP_DS_Node(new SP_DS_Nodeclass(0, wxT("Place"))));
	event = new SP_IA_Event(&l_lEventElements, SP_IA_EVT_SINGLE, true);

	l_lCommand = new SP_IA_Command_Delete(wxT("SAMPLE PN->RG: Delete State"));
	l_lCommand->SetTriggerEvent(event);
	l_lCommand->SetDescription(wxT("deletes the state corresponding to the marked place."));
	l_lCommand->SetNetclassNames(SP_DS_PN_CLASS, SP_DS_REACHABILITY_CLASS);
	l_lCommand->AddDataProcessor(new SP_IA_DP_MapperByAttribute(new SP_DS_ListAttribute(wxT("Places"), wxT(""), false), SP_IA_CONTAINS, new SP_DS_TextAttribute(wxT("Name"))));

	RegisterCommand(l_lCommand);
}

SP_IA_Manager::~SP_IA_Manager()
{
  list<SP_IA_Command*>::iterator CLit;
  SP_IA_Command* l_pcCmd;
	//at this point command history should already be empty
	//if not, we clean up now
	if (!m_lCommandHistory.empty()) {
		for (CLit = m_lCommandHistory.begin(); CLit != m_lCommandHistory.end(); ++CLit) {
			l_pcCmd = *CLit;
			wxDELETE(l_pcCmd);
		}
	}

	//clean up registered commands
	map<wxString, list<SP_IA_Command*>*>::iterator it;
  list<SP_IA_Command*>* l_pcCmdList;

	for (it = m_mRegisteredCommands.begin(); it != m_mRegisteredCommands.end(); ++it) {
    l_pcCmdList = (*it).second;
		for (CLit = l_pcCmdList->begin(); CLit != l_pcCmdList->end(); ++CLit) {
			l_pcCmd = *CLit;
			wxDELETE(l_pcCmd);
		}
		wxDELETE(l_pcCmdList);
  }

}



bool
SP_IA_Manager::ActivateCommand(const wxString& p_sSource, const wxString& p_sTarget, const wxString& p_sCmdName) {
	SP_DS_Graph *p_pcSource = GetGraphFromFilename(p_sSource);
	SP_DS_Graph *p_pcTarget = GetGraphFromFilename(p_sTarget);
	wxString l_sSource = p_pcSource->GetNetclass()->GetName();
	wxString l_sTarget = p_pcTarget->GetNetclass()->GetName();

	//getting command prototype and initializing it
	SP_IA_Command* l_pcCmd = GetRegisteredCommand(l_sSource, l_sTarget, p_sCmdName);
	if (l_pcCmd) l_pcCmd = l_pcCmd->InitializedCopy(p_pcSource, p_pcTarget);

	//if everything is okay,...
	if (!l_pcCmd) return false;
	//...we retrieve a reference to already active commands for the same trigger event...
	list<SP_IA_Command*> *l_pcCmdsRef = FindActiveCommands(l_pcCmd->GetTriggerEvent());
	list<SP_IA_Command*>::iterator itCL;

	//... and add this command...
	for (itCL = l_pcCmdsRef->begin(); itCL != l_pcCmdsRef->end(); ++itCL) {
		if (l_pcCmd->GetTargetGraph() == (*itCL)->GetTargetGraph()) {
			SP_LOGDEBUG(wxT("ERROR in ActivateCommand(...): There is already one command for this target triggered by the same event."));
			wxDELETE(l_pcCmd);
			return false;
		} else if (l_pcCmd->Equals(*itCL)) {
			SP_LOGDEBUG(wxT("ERROR in ActivateCommand(...): This command is already activated."));
			wxDELETE(l_pcCmd);
			return false;
		}
	}
	l_pcCmdsRef->push_back(l_pcCmd);

	return true;
}

bool
SP_IA_Manager::DeactivateCommand(const wxString& p_sSource, const wxString& p_sTarget, const wxString& p_sCmdName) {
SP_LOGDEBUG(wxT("entered DeactivateCommand()...*") + p_sCmdName + wxT("*"));
	bool l_bResult = false; //indicates whether we deactivated a command or not
	list<SP_IA_Command*>* l_pcCL;
	list<SP_IA_Command*>::iterator itCL;
	map<SP_IA_Event*, list<SP_IA_Command*>*>::iterator it;
	SP_DS_Graph *p_pcSource = GetGraphFromFilename(p_sSource);
	SP_DS_Graph *p_pcTarget = GetGraphFromFilename(p_sTarget);

	for (it = m_mActiveCommands.begin(); it != m_mActiveCommands.end(); ) {
		//current event is connected to p_pcSource...
		if ((*it).first->GetSourceGraph() == p_pcSource) {
			l_pcCL = (*it).second;
			if (p_pcTarget) {
				for (itCL = l_pcCL->begin(); itCL != l_pcCL->end(); ) {
					if (p_pcTarget == (*itCL)->GetTargetGraph() &&
							(p_sCmdName.Cmp((*itCL)->GetName()) == 0 || p_sCmdName.Cmp(wxT("")) == 0)) {
						SP_IA_Command* cmd = *itCL;
						l_pcCL->erase(itCL);
						wxDELETE(cmd);
						l_bResult = true;
						//was there a particular command to deactivate?
						if (!p_sCmdName.IsNull()) {
							//yes...we're done.
							return l_bResult;
						} else {
							//no...move on to the next event.
							break;
						}
					} else {
						//target graph or command name doesn't match
						//...so moving on to the next command
						++itCL;
					}
				}
			}
			//saving current event iterator for possible later deletion
			map<SP_IA_Event*, list<SP_IA_Command*>*>::iterator it2 = it;
			//setting iterator onto next event
			++it;
			//if we erased the only command for this event...
			//or we are here to delete all commands for this event (e.g. net was closed)...
			//then delete the entire entry
			if (!p_pcTarget || l_pcCL->empty()) {
				SP_IA_Event *evt = (*it2).first;
				m_mActiveCommands.erase(it2);
				if (!l_pcCL->empty()) {
					for (list<SP_IA_Command*>::iterator CLit = l_pcCL->begin(); CLit != l_pcCL->end(); ++CLit) {
						SP_IA_Command *l_pcCmd = *CLit;
						wxDELETE(l_pcCmd);
					}
				}
				wxDELETE(l_pcCL);
				l_pcCL = 0;
				wxDELETE(evt);
			}
		} else {
			//source graph doesn't match
			//...so moving on to the next event
			++it;
		}
	}

	return l_bResult;
}

bool
SP_IA_Manager::RegisterCommand(SP_IA_Command *p_pcCmd) {

	if (p_pcCmd->GetSourceNetclassName().IsNull()
			|| p_pcCmd->GetTargetNetclassName().IsNull()) {
		SP_LOGDEBUG(wxT("ERROR in RegisterCommand(...): couldn't register command, ")
									wxT("because either source or target netclass were not specified."));
		return false;
	}

	list<SP_IA_Command*>* l_pcCL;
	wxString l_sKey = wxString::Format(wxT("%s|%s"), p_pcCmd->GetSourceNetclassName().c_str(),
																							p_pcCmd->GetTargetNetclassName().c_str());
	map<wxString, list<SP_IA_Command*>*>::iterator itRC = m_mRegisteredCommands.find(l_sKey);

	if (itRC == m_mRegisteredCommands.end()) {
		l_pcCL = new list<SP_IA_Command*>();
	} else {
		l_pcCL = (*itRC).second;
	}
	//TODO?:	check for uniqueness of inserted commands
	//				or leave responsibility to the programmer
	l_pcCL->push_back(p_pcCmd);
	m_mRegisteredCommands[l_sKey] = l_pcCL;

	return true;
}

SP_IA_Command*
SP_IA_Manager::GetRegisteredCommand(const wxString& p_sSource, const wxString& p_sTarget, const wxString& p_sCmdName)
{
	wxString l_sKey = wxString::Format(wxT("%s|%s"), p_sSource.c_str(), p_sTarget.c_str());
	map<wxString, list<SP_IA_Command*>*>::iterator itRC = m_mRegisteredCommands.find(l_sKey);

	if (itRC == m_mRegisteredCommands.end()) {
		//while working with the wxT("configuration dialog") this should never happen.
		SP_LOGDEBUG(wxT("ERROR in GetRegisteredCommand(...): No command registered for this pair of net classes."));
		return 0;
	}

	list<SP_IA_Command*>* l_pcCL;
	list<SP_IA_Command*>::iterator itCL;
	l_pcCL = (*itRC).second;
	//searching for command prototype
	for (itCL = l_pcCL->begin(); itCL != l_pcCL->end(); ++itCL) {
		if (p_sCmdName.Cmp((*itCL)->GetName()) == 0) {
			return *itCL;
		}
	}
	return 0;
}

SP_IA_Command*
SP_IA_Manager::GetActiveCommand(const wxString& p_sSource, const wxString& p_sTarget, const wxString& p_sCmdName) {
	list<SP_IA_Command*>* l_pcCL;
	list<SP_IA_Command*>::iterator itCL;
	map<SP_IA_Event*, list<SP_IA_Command*>*>::iterator it;
	SP_DS_Graph *p_pcSource = GetGraphFromFilename(p_sSource);
	SP_DS_Graph *p_pcTarget = GetGraphFromFilename(p_sTarget);

	for (it = m_mActiveCommands.begin(); it != m_mActiveCommands.end(); ++it) {
		//current event is connected to p_pcSource...
		if ((*it).first->GetSourceGraph() == p_pcSource) {
			l_pcCL = (*it).second;
			for (itCL = l_pcCL->begin(); itCL != l_pcCL->end(); ++itCL) {
				//...and current command operates on p_pcTarget
				if ((*itCL)->GetTargetGraph() == p_pcTarget
						&& p_sCmdName.Cmp((*itCL)->GetName()) == 0) {
					return *itCL;
				}
			}
		}
	}
	return 0;

}

wxArrayString
SP_IA_Manager::GetRegisteredCommandNames(const wxString& p_sSource, const wxString& p_sTarget) {
	wxArrayString l_asResult;
	l_asResult.Clear();
	wxString l_sKey = wxString();
	list<SP_IA_Command*>* l_pcCL;
	map<wxString, list<SP_IA_Command*>*>::iterator itRC;

	if (!p_sSource.IsNull() && !p_sTarget.IsNull()) {
		l_sKey = wxString::Format(wxT("%s|%s"), p_sSource.c_str(), p_sTarget.c_str());
	}

	if (!l_sKey.IsNull()) {
		//if we need the registered commands for a particular net combination, we search for it...
		itRC = m_mRegisteredCommands.find(l_sKey);
	} else {
		//...if not, we just start at the beginning
		itRC = m_mRegisteredCommands.begin();
	}

	while (itRC != m_mRegisteredCommands.end()) {
		l_pcCL = (*itRC).second;
		for (list<SP_IA_Command*>::iterator it = l_pcCL->begin(); it != l_pcCL->end(); ++it) {
			l_asResult.Add((*it)->GetName());
		}

		//if we searched for particular commands we're done.
		if (!l_sKey.IsNull()) {
			return l_asResult;
		}
		//otherwise we go on to the next
		itRC++;
	}

	return l_asResult;
}

wxArrayString
SP_IA_Manager::GetActiveCommandNames(const wxString& p_sSource, const wxString& p_sTarget) {
	wxArrayString l_asResult;
	l_asResult.Clear();
	SP_DS_Graph *p_pcSource = GetGraphFromFilename(p_sSource);
	SP_DS_Graph *p_pcTarget = GetGraphFromFilename(p_sTarget);
	list<SP_IA_Command*>* l_pcCL;
	list<SP_IA_Command*>::iterator itCL;
	map<SP_IA_Event*, list<SP_IA_Command*>*>::iterator it;

	for (it = m_mActiveCommands.begin(); it != m_mActiveCommands.end(); ++it) {
		//current event is connected to p_pcSource...
		if ((*it).first->GetSourceGraph() == p_pcSource) {
			l_pcCL = (*it).second;
			for (itCL = l_pcCL->begin(); itCL != l_pcCL->end(); ++itCL) {
				//...and current command operates on p_pcTarget
				if ((*itCL)->GetTargetGraph() == p_pcTarget) {
					l_asResult.Add((*itCL)->GetName());
				}
			}
		}
	}
	return l_asResult;
}


bool
SP_IA_Manager::ExecuteMatchingCommands(SP_IA_Event *p_pcEvent) {
//SP_LOGDEBUG(wxT("entered ExecuteMatchingCommands..."));
	SP_ListData::iterator itS;
	bool result = false, cmd_result;

	if (p_pcEvent->GetTriggerType() == SP_IA_EVT_SINGLE && p_pcEvent->GetSourceElements()->size() > 1) {
		//Splitting multiple commands...creating a new event for every contained single command
		for (itS = p_pcEvent->GetSourceElements()->begin(); itS != p_pcEvent->GetSourceElements()->end(); ++itS) {
			SP_IA_Event *l_pcEvt = new SP_IA_Event(*itS, p_pcEvent->GetTriggerType());
			result |= ExecuteMatchingCommands(l_pcEvt);
			wxDELETE(l_pcEvt);
		}
	} else {
		//Searching for commands...
		list<SP_IA_Command*> l_pcCmds;
		FindActiveCommands(p_pcEvent, &l_pcCmds);
		if (l_pcCmds.empty()) return false;

		list<SP_IA_Command*>::iterator itCL;
		for (itCL = l_pcCmds.begin(); itCL != l_pcCmds.end(); ++itCL) {
			//need to make an initialized copy in order to be able to undo several executions...
			//...of the same command (multiple command execution)
			SP_IA_Command *l_pcCmd = (*itCL)->Copy(true);
			//executing command and saving it for later undo...
			SP_ListData l_lSource;
			l_lSource = *(p_pcEvent->GetSourceElements());
			cmd_result = l_pcCmd->Do(&l_lSource);
			if (cmd_result) {
				//SP_LOGDEBUG(wxString::Format(wxT("EXECUTING COMMAND: %s"), l_pcCmd->GetName()));
				m_lCommandHistory.push_front(l_pcCmd);
			} else {
				wxDELETE(l_pcCmd);
			}
			result |= cmd_result;
		}
	}
	//returns true if at least one command was successfully executed
	return result;

}


list<SP_IA_Command*>*
SP_IA_Manager::FindActiveCommands(SP_IA_Event* p_pcEvent, list<SP_IA_Command*> *p_pcCmdList) {
	map<SP_IA_Event*, list<SP_IA_Command*>*>::iterator itAC;
	list<SP_IA_Command*>::iterator itC;

	for (itAC = m_mActiveCommands.begin(); itAC != m_mActiveCommands.end(); ++itAC) {
		if (p_pcEvent->Equals((*itAC).first)) {
			if (p_pcCmdList) {
				//we collect only commands of enabled connections
				for (itC = (*itAC).second->begin(); itC != (*itAC).second->end(); ++itC) {
					if (IAModeEnabled((*itC)->GetSourceGraph(), (*itC)->GetTargetGraph())) {
						p_pcCmdList->push_back(*itC);
					}
				}
				return 0;
			} else {
				//we return all commands as a reference to their list
				return (*itAC).second;
			}
		}
	}
	//if we get here we couldn't find the event, so we create an entry for it and return its reference
	list<SP_IA_Command*> *l_pcRef = new list<SP_IA_Command*>();
	m_mActiveCommands[new SP_IA_Event(p_pcEvent->GetSourceElements(),	p_pcEvent->GetTriggerType())] = l_pcRef;
	return l_pcRef;
}


wxArrayString
SP_IA_Manager::GetConnectedNets(const wxString& p_sSource) {
//SP_LOGDEBUG(wxT("entered GetConnectedNets()..."));

	if (m_mConnectedNets.find(p_sSource) != m_mConnectedNets.end())	return m_mConnectedNets[p_sSource];

	return wxArrayString();
}


bool
SP_IA_Manager::ConnectNet(const wxString& p_sSource, const wxString& p_sTarget, bool p_bAutoEnable) {
//SP_LOGDEBUG(wxT("entered ConnectNet()..."));

	map<wxString, wxArrayString>::iterator it = m_mConnectedNets.find(p_sSource);
	if (it != m_mConnectedNets.end()) {
		//source has already connected nets
		if (((*it).second).Index(p_sTarget) != wxNOT_FOUND) {
			SP_LOGDEBUG(wxT("ERROR in ConnectNet(...): This net is already connected."));
			return false;
		} else {
			((*it).second).Add(p_sTarget);
		}
	} else {
		//first connection to source
		wxArrayString l_Array;
		l_Array.Empty();
		l_Array.Add(p_sTarget);
		m_mConnectedNets[p_sSource] = l_Array;
	}
	EnableConnection(true, p_sSource, p_sTarget);

	return true;
}


bool
SP_IA_Manager::DisconnectNet(const wxString& p_sSource, const wxString& p_sTarget) {
SP_LOGDEBUG(wxT("entered double disconnect"));
	map<wxString, wxArrayString>::iterator it = m_mConnectedNets.find(p_sSource);
	//source has no connected nets
	if (it == m_mConnectedNets.end()) return false;

	if (((*it).second).Index(p_sTarget) != wxNOT_FOUND)
	{
		((*it).second).Remove(p_sTarget);

		//also remove the m_lEnabledConnections entry (if existing) and...
		EnableConnection(false, p_sSource, p_sTarget);
		//...update the enabled connections menu and...
		SP_MDI_Doc* l_pcDoc = m_mFilename2Graph[p_sSource]->GetParentDoc();
		if(l_pcDoc)
		{
			SP_MDI_Diagram* l_pcDiag = l_pcDoc->GetDiagram();
			if(l_pcDiag)
			{
				SP_GUI_Canvas* l_pcCanvas = dynamic_cast<SP_GUI_Canvas*>(l_pcDiag->GetCanvas());
				if(l_pcCanvas)
				{
					l_pcCanvas->GetView()->UpdateIASubMenu();
				}
			}
		}

		UndoLastSubmit(m_mFilename2Graph[p_sTarget]);
		//...deactivate all active commands for this pair of nets
		DeactivateCommand(p_sSource, p_sTarget);
	}
//SP_LOGDEBUG(wxT("left double disconnect"));

	return true;
}


bool
SP_IA_Manager::DisconnectNet(const wxString& p_sNet, bool p_bFileClosed) {
	bool l_bResult = false;
//if (p_bFileClosed) SP_LOGDEBUG(wxT("entered single disconnect (file closed)") else SP_LOGDEBUG("entered single disconnect"));
//SP_LOGDEBUG(p_sNet);

	//remove p_sNet as source net
	if (m_mConnectedNets.erase(p_sNet) > 0)
	{
		l_bResult = true;
		//also delete the m_lEnabledConnections entry and...
		EnableConnection(false, p_sNet);
		//...deactivate all active commands for this source net
		DeactivateCommand(p_sNet);
	}

	if (p_bFileClosed)
	{
		//need to undo made changes in this net
		//otherwise it will be tried by the next command when this net is already closed
		//UndoLastSubmit(GetGraphFromFilename(p_sNet));

		map<wxString, wxArrayString>::iterator it;
		//remove p_sNet as target net for all other source nets
		for (it = m_mConnectedNets.begin(); it != m_mConnectedNets.end(); ++it)
		{
			l_bResult |= DisconnectNet((*it).first, p_sNet);
		}
		//now check if any sources have no targets anymore.
		//if so, remove them, too.
		it = m_mConnectedNets.begin();
		while (it != m_mConnectedNets.end())
		{
			if (((*it).second).IsEmpty())
			{
				map<wxString, wxArrayString>::iterator it2 = it;
				++it;
				m_mConnectedNets.erase(it2);
			}
			else
			{
				++it;
			}
		}
	}
//SP_LOGDEBUG(wxT("left single disconnect"));

	return l_bResult;
}


SP_DS_Graph*
SP_IA_Manager::GetGraphFromFilename(const wxString& p_sFName) {
//SP_LOGDEBUG(wxT("entered GetGraphFromFilename()..."));

	SP_DS_Graph *l_pcResult = m_mFilename2Graph[p_sFName];
	if (l_pcResult) return l_pcResult;

	wxNode *l_pcNode = wxGetApp().GetDocmanager()->GetDocuments().GetFirst();
	SP_MDI_Doc *l_pcDoc;

	while (l_pcNode) {
		l_pcDoc = dynamic_cast<SP_MDI_Doc*>(l_pcNode->GetData());
		if (p_sFName.Cmp(l_pcDoc->GetFilename()) == 0) {
			m_mFilename2Graph[p_sFName] = l_pcDoc->GetGraph();
			return m_mFilename2Graph[p_sFName];
		}
		l_pcNode = l_pcNode->GetNext();
	}

	return 0;
}


bool
SP_IA_Manager::SubmitEvent(SP_IA_Event *p_pcEvent) {
//SP_LOGDEBUG(wxT("entered SubmitEvent..."));
	//return if event was triggered w/out any selection
	if (p_pcEvent->GetSourceElements()->empty()) {
		wxDELETE(p_pcEvent);
		return false;
	}

	UndoLastSubmit();

	bool result = ExecuteMatchingCommands(p_pcEvent);

	if (result)
		SP_LOGDEBUG(wxT("Successfully executed one or more command(s)."));
	else
		SP_LOGDEBUG(wxT("Executed no command."));
	//destroy the submitted event, since it's not needed anymore
	wxDELETE(p_pcEvent);

	return result;
}


void
SP_IA_Manager::UndoLastSubmit(SP_DS_Graph *p_pcGraph, bool p_bForceUndo) {
//SP_LOGDEBUG(wxT("entered UndoLastSubmit()..."));
	//undoing executed commands of last event
	//p_pcGraph may indicate a particular target where undo should be done
	list<SP_IA_Command*>::iterator it = m_lCommandHistory.begin();
	while (it != m_lCommandHistory.end()) {
		if (!p_pcGraph || (*it)->GetTargetGraph() == p_pcGraph) {
			if ((*it)->CanUndo() || p_bForceUndo) (*it)->Undo();
			SP_IA_Command *cmd = *it;
			it = m_lCommandHistory.erase(it);
			wxDELETE(cmd);
		} else {
			it++;
		}
	}
}


bool
SP_IA_Manager::IAModeEnabled(const wxString& p_sSource, const wxString& p_sTarget) {
//SP_LOGDEBUG(wxT("entered IAModeEnabled(wxString)..."));
	if (p_sSource.Cmp(wxT("")) == 0 && p_sTarget.Cmp(wxT("")) == 0) {
		//asking whether there are any enabled connections
		return !m_lEnabledConnections.empty();
	} else if (p_sTarget.Cmp(wxT("")) == 0) {
		//asking whether there are any enabled connections to source
		for (SP_ListString::iterator it = m_lEnabledConnections.begin(); it != m_lEnabledConnections.end(); ++it) {
			wxString& l_rsHelp = *it;
			if (l_rsHelp.StartsWith(p_sSource + wxT("|"))) return true;
		}
	} else {
		//asking whether there is a particular connection enabled
		return SP_Find(m_lEnabledConnections, (p_sSource + wxT("|") + p_sTarget)) != m_lEnabledConnections.end();
	}

	return false;
}


bool
SP_IA_Manager::IAModeEnabled(SP_DS_Graph *p_pcSource, SP_DS_Graph *p_pcTarget) {
//SP_LOGDEBUG(wxT("entered IAModeEnabled(SP_DS_Graph)..."));

	return IAModeEnabled(GetFilenameFromGraph(p_pcSource), GetFilenameFromGraph(p_pcTarget));
}


void
SP_IA_Manager::EnableConnection(bool p_bEnable,const wxString& p_sSource, const wxString& p_sTarget) {
//SP_LOGDEBUG(wxString::Format(wxT("ENABLE CONNECTION: %s - %s: %d"), p_sSource, p_sTarget, p_bEnable));
	//if there are no active connections, we have nothing to do here
	if (m_mConnectedNets.empty()) return;

	if (p_bEnable) {
		map<wxString, wxArrayString>::iterator it;
		for (it = m_mConnectedNets.begin(); it != m_mConnectedNets.end(); ++it) {
			//if we search for a certain source but haven't found it here, we go on.
			if (p_sSource.Cmp(wxT("")) != 0 && p_sSource.Cmp((*it).first) != 0) continue;
			for (unsigned int i = 0; i < ((*it).second).GetCount(); i++) {
				//either we enable any connection found...
				if ((p_sSource.Cmp(wxT("")) == 0 && p_sTarget.Cmp(wxT("")) == 0)
					//...or every connection to source...
					|| (p_sSource.Cmp((*it).first) == 0 && (p_sTarget.Cmp(wxT("")) == 0
					//...or the source-target connection
					|| p_sTarget.Cmp(((*it).second)[i]) == 0)))
				{
					m_lEnabledConnections.push_back((*it).first + wxT("|") + ((*it).second)[i]);
					//if there was a particular target, we're done.
					if (p_sTarget.Cmp(wxT("")) != 0) return;
				}
			}
			//if we get here and there was a particular source, we're done.
			if (p_sSource.Cmp(wxT("")) != 0) return;
		}
		//remove elements that were accidentally inserted twice
		m_lEnabledConnections.unique();
	} else {
		if (p_sSource.Cmp(wxT("")) == 0 && p_sTarget.Cmp(wxT("")) == 0) {
			//disabling all connections
			m_lEnabledConnections.clear();
		} else {
			//disabling all connections of source and/or target
			SP_ListString::iterator it = m_lEnabledConnections.begin();
			wxString l_sKey = p_sSource + wxT("|") + p_sTarget;
			while (it != m_lEnabledConnections.end()) {
				wxString& l_rsBuffer = *it;
				if (l_rsBuffer.Contains(l_sKey)) {
					it = m_lEnabledConnections.erase(it);
				} else {
					++it;
				}
			}
		}
		//TODO?: undoing last commands, if all connections disabled
	}
}


wxString
SP_IA_Manager::GetFilenameFromGraph(SP_DS_Graph *p_pcGraph) {
//SP_LOGDEBUG(wxT("entered GetFilenameFromGraph()..."));

	if (!p_pcGraph) return wxString();

	map<wxString, SP_DS_Graph*>::iterator it;
	for (it = m_mFilename2Graph.begin(); it != m_mFilename2Graph.end(); ++it) {
		if ((*it).second == p_pcGraph) return (*it).first;
	}

	return wxString();
}
