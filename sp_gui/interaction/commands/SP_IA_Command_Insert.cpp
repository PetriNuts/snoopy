// SP_IA_Command_Insert.cpp: Implementierung der Klasse SP_IA_Command_Insert.
//
//////////////////////////////////////////////////////////////////////

#include "SP_IA_Command_Insert.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "snoopy.h"
#include "sp_gui/interaction/SP_IA_Manager.h"
#include "sp_gr/SP_GR_Edge.h"

#include <wx/spinctrl.h>
#include <wx/choicdlg.h>


BEGIN_EVENT_TABLE(SP_IA_Command_Insert, wxEvtHandler)

//EVT_BUTTON(SP_ID_BUTTON_SETNODE, SP_IA_Command_Insert::OnSetCurrentNode)

END_EVENT_TABLE()


SP_IA_Command_Insert::SP_IA_Command_Insert()
: SP_IA_Command(SP_IA_CMD_INSERT, wxT("Command: Insert <Default Name>"), true, true),
m_pcTargetDoc(0),
m_nPosX(DEFAULT_POS),
m_nPosY(DEFAULT_POS),
m_nOffsetX(DEFAULT_OFFSET),
m_nOffsetY(DEFAULT_OFFSET),
m_pcEdgeSourceGraphic(0),
m_cColor(wxNullColour),
m_pcColorCmd(0)
{ }


SP_IA_Command_Insert::SP_IA_Command_Insert(wxString p_sName, int p_nPosX, int p_nPosY,
																					 int p_nOffsetX, int p_nOffsetY, bool p_bHasConfig, bool p_bCanUndo)
: SP_IA_Command(SP_IA_CMD_INSERT, p_sName, p_bHasConfig, p_bCanUndo),
m_pcTargetDoc(0),
m_pcEdgeSourceGraphic(0),
m_cColor(wxNullColour),
m_pcColorCmd(0)
{
	SetPosition(p_nPosX, p_nPosY);
	SetOffset(p_nOffsetX, p_nOffsetY);
}


SP_IA_Command_Insert::~SP_IA_Command_Insert() {
/*
	if (m_pcColorCmd) {
		m_pcColorCmd->Undo();
		wxDELETE(m_pcColorCmd);
		m_pcColorCmd = 0;
	}*/
}


SP_IA_Command*
SP_IA_Command_Insert::Copy(bool p_bTryInit) {
	SP_IA_Command_Insert* l_pcResult = new SP_IA_Command_Insert(GetName(), m_nPosX, m_nPosY,
																						m_nOffsetX, m_nOffsetY, HasConfig(), CanUndo());
	l_pcResult->SetDescription(m_sDescription);
	l_pcResult->SetNetclassNames(m_sSourceNetclass, m_sTargetNetclass);
	l_pcResult->SetCopyStamp(true);
	l_pcResult->SetEdgeSource(GetEdgeSource(), false);
	l_pcResult->SetColor(m_cColor);
	l_pcResult->SetSearchAttributeName(m_sSearchAttributeName);
	if (p_bTryInit) {
		l_pcResult->SetTargetGraph(m_pcTarget);
		l_pcResult->SetTriggerEvent(m_pcTriggerEvent);
		l_pcResult->SetDataProcessors(m_pcDataProcessors);
	}
	return l_pcResult;
}


bool
SP_IA_Command_Insert::Do(SP_ListData* p_pcData) {

	//calculating the elements to insert...
	//in fact the result is just a list of prototypes, from which we derive the actual nodes to be inserted
	DoDataProcessing(p_pcData);

	//we found nothing means that something probably went wrong...
	if (!p_pcData || p_pcData->empty()) return false;

	SP_ListData::iterator it;
	if (!m_pcTargetDoc) m_pcTargetDoc = GetTargetGraph()->GetParentDoc();
	SP_GUI_Canvas *l_pcCanvas = dynamic_cast<SP_GUI_Canvas*>(m_pcTargetDoc->GetDiagram()->GetCanvas());
	if (!l_pcCanvas) return false;

	SP_DS_Node *l_pcNewNode = 0;
	SP_Graphic *l_pcNewNodeGraphic = 0;
	SP_DS_Edge *l_pcCurrentEdge = 0;
	m_pcEdgeSourceGraphic = GetEdgeSource();
	int edgecounter = 0;

	for (it = p_pcData->begin(); it != p_pcData->end(); ++it) {
		if ((*it)->GetElementType() == SP_ELEMENT_NODE) {
			SP_DS_Nodeclass *l_pcNC = GetTargetGraph()->GetNodeclass((*it)->GetClassName());
			if (l_pcNC) {
				//check if node with these attributes (incl. values) already exists
				l_pcNewNode = dynamic_cast<SP_DS_Node*>(FindNode(l_pcNC, (*it)->GetAttributes()));
				//...if not, create a new one
				if (!l_pcNewNode) {
					l_pcNewNode = l_pcNC->NewElement(l_pcCanvas->GetNetnumber());
					//transfer previously attached attributes (labled with wxT("#IA#VALUE#<name>"))
					//only attributes, this node type knows of, are transfered
					wxString l_sName;
					for (SP_ListAttribute::const_iterator itA = (*it)->GetAttributes()->begin();
																									itA != (*it)->GetAttributes()->end(); ++itA) {
						if (wxString((*itA)->GetName()).StartsWith(wxT("#IA#VALUE#"), &l_sName)) {
							SP_DS_Attribute *l_pcAttrib = l_pcNewNode->GetAttribute(l_sName);
							if (l_pcAttrib) l_pcAttrib->SetValueString((*itA)->GetValueString());
						}
					}
					SetPosition(m_nPosX + m_nOffsetX, m_nPosY + m_nOffsetY);
					l_pcNewNode->ShowOnCanvas(l_pcCanvas, false, m_nPosX, m_nPosY);
				}
				SP_ListGraphic l_lGraphics;
				l_pcNewNode->GetGraphicsInNet(&l_lGraphics, l_pcCanvas->GetNetnumber(), SP_ELEMENT_NODE);
				//if there are several graphics we just take the first in list
				//note: this may cause inconsistencies with logical elements,
				//			which should be taken care of when commands make use of such elements
				if (!l_lGraphics.empty()) l_pcNewNodeGraphic = l_lGraphics.front();

				//now we are able to connect the node, using the saved edge
				if (l_pcCurrentEdge && m_pcEdgeSourceGraphic && m_pcEdgeSourceGraphic->GetCanvas() == l_pcCanvas) {
					SP_DS_Edgeclass *l_pcEC = GetTargetGraph()->GetEdgeclass(l_pcCurrentEdge->GetClassName());
					if (l_pcEC && !FindEdge(l_pcEC, m_pcEdgeSourceGraphic->GetParent(), l_pcNewNode)
							&& GetTargetGraph()->GetNetclass()->EdgeRequirement(l_pcEC,
									m_pcEdgeSourceGraphic->GetParent(), l_pcNewNode)) {
						SP_DS_Edge *l_pcNewEdge = l_pcEC->NewElement(l_pcCanvas->GetNetnumber(),
																													m_pcEdgeSourceGraphic, l_pcNewNodeGraphic);
						if (l_pcNewEdge) {
							//transfer previously attached attributes (labled with wxT("#IA#VALUE#<name>"))
							//only attributes, this edge type knows of, are transfered
							wxString l_sName;
							for (SP_ListAttribute::const_iterator itA = (l_pcCurrentEdge)->GetAttributes()->begin();
																											itA != (l_pcCurrentEdge)->GetAttributes()->end(); ++itA) {
								if (wxString((*itA)->GetName()).StartsWith(wxT("#IA#VALUE#"), &l_sName)) {
									SP_DS_Attribute *l_pcAttrib = l_pcNewEdge->GetAttribute(l_sName);
									if (l_pcAttrib) {
										if (edgecounter == 1) {
											l_pcAttrib->SetValueString((*itA)->GetValueString());
										} else {
											l_pcAttrib->SetValueString(wxT("?"));
										}
									}
								}
							}
							if (m_pcEdgeSourceGraphic->GetParent() == l_pcNewNode) {
								//if source and target are identical,
								//we need to create an edge loop, to make the resulting edge visible
								SP_ListGraphic *l_lEdgeGraphics = l_pcNewEdge->GetGraphics();
								for (SP_ListGraphic::iterator itE = l_lEdgeGraphics->begin(); itE != l_lEdgeGraphics->end(); ++itE) {
									SP_Graphic* l_pcGr = *itE;
									SP_GR_Edge *l_pcEdgeGraphic = dynamic_cast<SP_GR_Edge*>(l_pcGr);
									wxList l_lPoints;
									double OffX = l_pcNewNodeGraphic->GetWidth() / 2 + 30;
									double OffY = l_pcNewNodeGraphic->GetHeight() / 2;
									double X = l_pcNewNodeGraphic->GetPosX();
									double Y = l_pcNewNodeGraphic->GetPosY();
									l_lPoints.Append((wxObject*)new wxRealPoint(X, Y));
									l_lPoints.Append((wxObject*)new wxRealPoint(X + OffX - 15, Y - OffY));
									l_lPoints.Append((wxObject*)new wxRealPoint(X + OffX, Y));
									l_lPoints.Append((wxObject*)new wxRealPoint(X + OffX - 15, Y + OffY));
									l_lPoints.Append((wxObject*)new wxRealPoint(X, Y));
									l_pcEdgeGraphic->SetControlPoints(&l_lPoints);

									//just cleaning up
									wxNode* l_pcLNode = l_lPoints.GetFirst();
									while (l_pcLNode) {
										wxRealPoint* pt = (wxRealPoint*)l_pcLNode->GetData();
										wxDELETE(pt);
										l_pcLNode = l_pcLNode->GetNext();
									}
								}
							}
							l_pcNewEdge->ShowOnCanvas(l_pcCanvas);
						}
					}
				}
			}
			edgecounter = 0;
		} else if ((*it)->GetElementType() == SP_ELEMENT_EDGE) {
			//just saving the edge
			//actual creation will take place later (when we know source and target)
			SP_Data* l_pcData = *it;
			l_pcCurrentEdge = dynamic_cast<SP_DS_Edge*>(l_pcData);
			edgecounter++;
		}
	}
	//now that we are done, we need to save edge source, and (PosX,PosY) for future node creations
	//because this object will be destroyed after command execution,
	//we need another persistent object for that. so we use the activated command prototype
	SP_IA_Manager *l_pcIAM = wxGetApp().GetIAManager();
	wxString l_sSource = l_pcIAM->GetFilenameFromGraph(GetSourceGraph());
	wxString l_sTarget = l_pcIAM->GetFilenameFromGraph(GetTargetGraph());
	SP_IA_Command_Insert *l_pcActCmd = dynamic_cast<SP_IA_Command_Insert*>(wxGetApp().GetIAManager()
																					->GetActiveCommand(l_sSource, l_sTarget, GetName()));
	l_pcActCmd->SetEdgeSource(l_pcNewNodeGraphic);
	l_pcActCmd->SetPosition(m_nPosX, m_nPosY);

	//refresh target view
	m_pcTargetDoc->Refresh();

	//elements of l_pcData can be deleted, since they should be just wxT("dummies")
	for (it = p_pcData->begin(); it != p_pcData->end(); ++it) {
		SP_IA_DataProcessor::ReleaseData(*it, true);
	}

	return true;
}


bool
SP_IA_Command_Insert::Undo() {

	if (m_cColor == wxNullColour) return true;

	SP_IA_Manager *l_pcIAM = wxGetApp().GetIAManager();
	wxString l_sSource = l_pcIAM->GetFilenameFromGraph(GetSourceGraph());
	wxString l_sTarget = l_pcIAM->GetFilenameFromGraph(GetTargetGraph());
	SP_IA_Command_Insert *l_pcActCmd = dynamic_cast<SP_IA_Command_Insert*>(wxGetApp().GetIAManager()
																					->GetActiveCommand(l_sSource, l_sTarget, GetName()));

	if (l_pcActCmd && l_pcActCmd->m_pcColorCmd) {
		l_pcActCmd->m_pcColorCmd->Undo();
		wxDELETE(l_pcActCmd->m_pcColorCmd);
		l_pcActCmd->m_pcColorCmd = 0;
	}
	return true;
}


SP_DS_Node*
SP_IA_Command_Insert::FindNode(SP_DS_Nodeclass *p_pcNC, const SP_ListAttribute *p_pcAttributes) {

	for (SP_ListNode::const_iterator it = p_pcNC->GetElements()->begin(); it != p_pcNC->GetElements()->end(); ++it) {
		bool l_bReturnNode = true;
		//check if current node has got all attributes contained in the given list...
		for (SP_ListAttribute::const_iterator itA = p_pcAttributes->begin(); itA != p_pcAttributes->end(); ++itA) {
			SP_DS_Attribute *l_pcAttrib;
			wxString l_sName;
			if (wxString((*itA)->GetName()).StartsWith(wxT("#IA#VALUE#"), &l_sName)) {
				//checking special #IA#-attributes
				l_pcAttrib = (*it)->GetAttribute(l_sName);
			} else {
				//actually there should be only #IA#VALUE#-attributes, so we never really get here
				l_pcAttrib = (*it)->GetAttribute((*itA)->GetName());
			}
			if (!(*itA)->Equals(l_pcAttrib, true, false, true)) {
				l_bReturnNode = false;
				break;
			}
		}
		//...if so, return that node.
		if (l_bReturnNode) return *it;
	}

	return 0;
}


bool
SP_IA_Command_Insert::FindEdge(SP_DS_Edgeclass *p_pcEC, SP_Data *p_pcSource, SP_Data *p_pcTarget) {
	if (!p_pcEC || !p_pcSource || !p_pcTarget) return false;

	const SP_ListEdge *l_pcEL = p_pcEC->GetElements();
	SP_ListEdge::const_iterator it;

	for (it = l_pcEL->begin(); it != l_pcEL->end(); ++it) {
		if ((*it)->GetSource() == p_pcSource && (*it)->GetTarget() == p_pcTarget) {
			return true;
		}
	}

	return false;
}


void
SP_IA_Command_Insert::AddToConfigDialog(wxWindow *p_pcParent, wxSizer *p_pcSizer) {

	wxBoxSizer *helpSizer = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer *helpSizer2 = new wxBoxSizer(wxVERTICAL);
	helpSizer2->Add(new wxStaticText(p_pcParent, -1, wxT("Starting Position X:")), 0, wxALL, 4);
	helpSizer2->Add(new wxStaticText(p_pcParent, -1, wxT("Starting Position Y:")), 0, wxALL, 4);
	helpSizer2->Add(new wxStaticText(p_pcParent, -1, wxT("Offset X:")), 0, wxALL, 4);
	helpSizer2->Add(new wxStaticText(p_pcParent, -1, wxT("Offset Y:")), 0, wxALL, 4);
	helpSizer->Add(helpSizer2);

	helpSizer2 = new wxBoxSizer(wxVERTICAL);
	m_scPosX = new wxSpinCtrl(p_pcParent, SP_ID_SPINCTRL_POS_X, wxString::Format(wxT("%d"), m_nPosX),
						wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, INT_MAX, m_nPosX);
	helpSizer2->Add(m_scPosX, 0, wxLEFT, 5);
	m_scPosY = new wxSpinCtrl(p_pcParent, SP_ID_SPINCTRL_POS_Y, wxString::Format(wxT("%d"), m_nPosY),
						wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, INT_MAX, m_nPosY);
	helpSizer2->Add(m_scPosY, 0, wxLEFT, 5);
	m_scOffsetX = new wxSpinCtrl(p_pcParent, SP_ID_SPINCTRL_OFF_X, wxString::Format(wxT("%d"), m_nOffsetX),
						wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, -OFFSET_MAX, OFFSET_MAX, m_nOffsetX);
	helpSizer2->Add(m_scOffsetX, 0, wxLEFT, 5);
	m_scOffsetY = new wxSpinCtrl(p_pcParent, SP_ID_SPINCTRL_OFF_Y, wxString::Format(wxT("%d"), m_nOffsetY),
						wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, -OFFSET_MAX, OFFSET_MAX, m_nOffsetY);
	helpSizer2->Add(m_scOffsetY, 0, wxLEFT, 5);
	helpSizer->Add(helpSizer2);

	p_pcSizer->Add(helpSizer, 0, wxALL, 10);

	p_pcSizer->Add(new wxButton(p_pcParent, SP_ID_BUTTON_SETNODE, wxT("Set Current Edge Source...")), 0, wxALL | wxEXPAND, 10);

	//p_pcParent->PushEventHandler(this);

}


void
SP_IA_Command_Insert::OnConfigDialogOk() {

	SetPosition(m_scPosX->GetValue(), m_scPosY->GetValue());
	SetOffset(m_scOffsetX->GetValue(), m_scOffsetY->GetValue());
}


void
SP_IA_Command_Insert::SetPosition(int p_nX, int p_nY) {
	if (p_nX < 0) m_nPosX = DEFAULT_POS; else m_nPosX = p_nX;
	if (p_nY < 0) m_nPosY = DEFAULT_POS; else m_nPosY = p_nY;
}


void
SP_IA_Command_Insert::SetOffset(int p_nX, int p_nY) {
	if (p_nX < -OFFSET_MAX || p_nX > OFFSET_MAX) m_nOffsetX = DEFAULT_OFFSET; else m_nOffsetX = p_nX;
	if (p_nY < -OFFSET_MAX || p_nY > OFFSET_MAX) m_nOffsetY = DEFAULT_OFFSET; else m_nOffsetY = p_nY;
}


void
SP_IA_Command_Insert::OnSetCurrentNode(wxCommandEvent& p_cEvent) {

	if (!GetTargetGraph()) return;

	wxArrayString NodeListArray;
	map<int, SP_DS_Node*> Index2Node;
	SP_ListNodeclass::const_iterator itNC;
	SP_ListNode::const_iterator itN;
	int index = 0;

	for (itNC = GetTargetGraph()->GetNodeclasses()->begin(); itNC != GetTargetGraph()->GetNodeclasses()->end(); ++itNC) {
		for (itN = (*itNC)->GetElements()->begin(); itN != (*itNC)->GetElements()->end(); ++itN) {
			SP_DS_Attribute *attrib = (*itN)->GetAttribute(m_sSearchAttributeName);
			if (attrib && !wxString(attrib->GetValueString()).IsNull()) {
				NodeListArray.Add(attrib->GetValueString());
			} else {
				NodeListArray.Add(wxT("No attribute value found."));
			}
			Index2Node[index] = *itN;
			index++;
		}
	}

	if (NodeListArray.GetCount() == 0) return;

	index =	wxGetSingleChoiceIndex(wxT("Select one node as current edge source..."), wxT(""), NodeListArray);
	if (index == -1) return;

	SP_DS_Node *choice = Index2Node[index];

	SP_ListGraphic l_lGraphics;
	choice->GetGraphicsInNet(&l_lGraphics, GetTargetGraph()->GetNetnumber(), SP_ELEMENT_NODE);
	//if there are several graphics we just take the first in list
	//note: this may cause inconsistencies with logical elements,
	//			which should be taken care of when commands make use of such elements
	if (!l_lGraphics.empty()) SetEdgeSource(l_lGraphics.front());
}


void
SP_IA_Command_Insert::SetEdgeSource(SP_Graphic *p_pcESG, bool p_bDoColoring)  {

	m_pcEdgeSourceGraphic = p_pcESG;

	if (!m_pcEdgeSourceGraphic || !p_bDoColoring || m_cColor == wxNullColour) return;

	//color current edge source
	if (m_pcColorCmd) {
		m_pcColorCmd->Undo();
		wxDELETE(m_pcColorCmd);
		m_pcColorCmd = 0;
	}
	m_pcColorCmd = new SP_CMD_ColorizeGraphic(m_pcEdgeSourceGraphic, m_cColor, m_cColor);
	m_pcColorCmd->Do();

}


SP_Graphic*
SP_IA_Command_Insert::GetEdgeSource() {

	SP_IA_Manager *l_pcIAM = wxGetApp().GetIAManager();
	wxString l_sSource = l_pcIAM->GetFilenameFromGraph(GetSourceGraph());
	wxString l_sTarget = l_pcIAM->GetFilenameFromGraph(GetTargetGraph());
	SP_IA_Command_Insert *l_pcActCmd = dynamic_cast<SP_IA_Command_Insert*>(wxGetApp().GetIAManager()
																					->GetActiveCommand(l_sSource, l_sTarget, GetName()));
	if (!l_pcActCmd) return 0;
	if (l_pcActCmd == this) return m_pcEdgeSourceGraphic;
	return l_pcActCmd->GetEdgeSource();
}
