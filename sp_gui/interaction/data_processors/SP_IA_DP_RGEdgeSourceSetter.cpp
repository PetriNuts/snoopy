
#include "SP_IA_DP_RGEdgeSourceSetter.h"

#include "sp_gui/interaction/SP_IA_Manager.h"
#include "sp_gui/interaction/commands/SP_IA_Command_Insert.h"
#include "sp_ds/attributes/SP_DS_ListAttribute.h"
#include "sp_ds/attributes/SP_DS_MultiplicityAttribute.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "snoopy.h"

enum {
	SP_PROCESSING_PLACES,
	SP_PROCESSING_TRANSITIONS
};

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

SP_IA_DP_RGEdgeSourceSetter::SP_IA_DP_RGEdgeSourceSetter()
{ }

SP_IA_DP_RGEdgeSourceSetter::~SP_IA_DP_RGEdgeSourceSetter()
{ }


SP_IA_DataProcessor*
SP_IA_DP_RGEdgeSourceSetter::Copy() {

	SP_IA_DataProcessor *l_pcNewDP = new SP_IA_DP_RGEdgeSourceSetter();
	l_pcNewDP->SetCopyStamp(true);

	return l_pcNewDP;
}


SP_ListData*
SP_IA_DP_RGEdgeSourceSetter::DoProcessing(SP_ListData *p_pcData) {
	if (!p_pcData) return 0;

	SP_ListData::reverse_iterator rit;
	SP_MapString2Int::iterator it;
	SP_DS_ListAttribute *l_pcPlaces;
	m_mPlaces2Marking.clear();
	int step = SP_PROCESSING_PLACES, diff;
	wxString place;

	for (rit = p_pcData->rbegin(); rit != p_pcData->rend(); ++rit) {
		SP_Data* l_pcData = *rit;
		if (step == SP_PROCESSING_PLACES && wxString((l_pcData)->GetClassName()).Cmp(wxT("Node")) == 0) {
			l_pcPlaces = dynamic_cast<SP_DS_ListAttribute*>((l_pcData)->GetAttribute(wxT("#IA#VALUE#Places")));
			if (!l_pcPlaces) continue;
			for (int i = 0; i < l_pcPlaces->Size(); i++) {
				if (m_mPlaces2Marking.find(l_pcPlaces->GetValue(i)) == m_mPlaces2Marking.end()) {
					m_mPlaces2Marking[l_pcPlaces->GetValue(i)] = 1;
				} else {
					m_mPlaces2Marking[l_pcPlaces->GetValue(i)]++;
				}
			}
			step = SP_PROCESSING_TRANSITIONS;
		} else if (step == SP_PROCESSING_TRANSITIONS && wxString((l_pcData)->GetClassName()).Cmp(wxT("Transition")) == 0) {
			//take marks away from target nodes
			const SP_ListEdge *edges = dynamic_cast<SP_DS_Node*>(l_pcData)->GetSourceEdges();
			for (SP_ListEdge::const_iterator itE = edges->begin(); itE != edges->end(); ++itE) {
				place = (*itE)->GetTarget()->GetAttribute(wxT("Name"))->GetValueString();
				diff = dynamic_cast<SP_DS_MultiplicityAttribute*>((*itE)->GetAttribute(wxT("Multiplicity")))->GetValue();
				SP_LOGDEBUG(wxString::Format(wxT("IN FOR: %s, %d"),place.c_str(),diff));
				m_mPlaces2Marking[place] -= diff;
			}
			//put marks back to source nodes
			edges = dynamic_cast<SP_DS_Node*>(l_pcData)->GetTargetEdges();
			for (SP_ListEdge::const_iterator itE = edges->begin(); itE != edges->end(); ++itE) {
				place = (*itE)->GetSource()->GetAttribute(wxT("Name"))->GetValueString();
				diff = dynamic_cast<SP_DS_MultiplicityAttribute*>((*itE)->GetAttribute(wxT("Multiplicity")))->GetValue();
				SP_LOGDEBUG(wxString::Format(wxT("IN FOR: %s, %d"),place.c_str(),diff));
				if (m_mPlaces2Marking.find(place) == m_mPlaces2Marking.end()) {
					m_mPlaces2Marking[place] = diff;
				} else {
					m_mPlaces2Marking[place] += diff;
				}
			}
		} else {
			break;
		}
	}

	it = m_mPlaces2Marking.begin();
	wxString valueString = wxT("");
	valueString.Printf(wxT("(%d) %s"), (*it).second, (*it).first.c_str());
	it++;
	while (it != m_mPlaces2Marking.end()) {
		place.Printf(wxT("; (%d) %s"), (*it).second, (*it).first.c_str());
		valueString.Append(place);
		it++;
	}

	//getting original parent command for setting edge source
	SP_IA_Manager *l_pcIAM = wxGetApp().GetIAManager();
	wxString l_sSource = l_pcIAM->GetFilenameFromGraph(m_pcParentCmd->GetSourceGraph());
	wxString l_sTarget = l_pcIAM->GetFilenameFromGraph(m_pcParentCmd->GetTargetGraph());
	SP_IA_Command_Insert *l_pcActCmd = dynamic_cast<SP_IA_Command_Insert*>(wxGetApp().GetIAManager()
																					->GetActiveCommand(l_sSource, l_sTarget, m_pcParentCmd->GetName()));
	if (!l_pcActCmd) return p_pcData;

	SP_ListNodeclass::const_iterator itNC;
	SP_ListNode::const_iterator itN;
	l_pcPlaces = new SP_DS_ListAttribute(wxT("Places"), valueString, false);

	for (itNC = m_pcParentCmd->GetTargetGraph()->GetNodeclasses()->begin();
				itNC != m_pcParentCmd->GetTargetGraph()->GetNodeclasses()->end(); ++itNC) {
		for (itN = (*itNC)->GetElements()->begin(); itN != (*itNC)->GetElements()->end(); ++itN) {
			SP_DS_Attribute *attrib = (*itN)->GetAttribute(wxT("Places"));
			if (l_pcPlaces->Equals(attrib)) {
				//found node of previous marking, so set it as edge source
				SP_ListGraphic l_lGraphics;
				(*itN)->GetGraphicsInNet(&l_lGraphics, m_pcParentCmd->GetTargetGraph()->GetNetnumber(), SP_ELEMENT_NODE);
				if (!l_lGraphics.empty()) {
					//if there are several graphics we just take the first in list
					//note: this may cause inconsistencies with logical elements,
					//			which should be taken care of when commands make use of such elements
					l_pcActCmd->SetEdgeSource(l_lGraphics.front());
					wxDELETE(l_pcPlaces);
					return p_pcData;
				}
			}
		}
	}
	//searched node does not exist, so create it and set it as current
	SP_GUI_Canvas *l_pcCanvas = dynamic_cast<SP_GUI_Canvas*>(m_pcParentCmd->GetTargetGraph()->GetParentDoc()->GetDiagram()->GetCanvas());
	if (!l_pcCanvas) {
		l_pcActCmd->SetEdgeSource(0);
		wxDELETE(l_pcPlaces);
		return p_pcData;
	}
	SP_DS_Graph *l_pcTarget = m_pcParentCmd->GetTargetGraph();
	SP_DS_Nodeclass *l_pcNC = l_pcTarget->GetNodeclass(wxT("Node"));
	SP_DS_Node* l_pcNewNode = l_pcNC->NewElement(l_pcCanvas->GetNetnumber());
	SP_DS_Attribute *l_pcAttrib = l_pcNewNode->GetAttribute(wxT("Places"));
	if (l_pcAttrib) l_pcAttrib->SetValueString(valueString);
	l_pcNewNode->ShowOnCanvas(l_pcCanvas, false, l_pcActCmd->GetPosX() + 120, l_pcActCmd->GetPosY());

	SP_ListGraphic l_lGraphics;
	SP_Graphic *l_pcNewNodeGraphic = 0;
	l_pcNewNode->GetGraphicsInNet(&l_lGraphics, l_pcCanvas->GetNetnumber(), SP_ELEMENT_NODE);
	//if there are several graphics we just take the first in list
	//note: this may cause inconsistencies with logical elements,
	//			which should be taken care of when commands make use of such elements
	if (!l_lGraphics.empty()) l_pcNewNodeGraphic = l_lGraphics.front();
	l_pcActCmd->SetEdgeSource(l_pcNewNodeGraphic);

	wxDELETE(l_pcPlaces);
	return p_pcData;
}
