// SP_IA_Command_ChangeAttribute.cpp: Implementierung der Klasse SP_IA_Command_ChangeAttribute.
//
//////////////////////////////////////////////////////////////////////

#include "SP_IA_Command_SetAttribute.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "snoopy.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

SP_IA_Command_SetAttribute::SP_IA_Command_SetAttribute()
: SP_IA_Command(SP_IA_CMD_SETATTRIBUTE, wxT("Command: SetAttribute <Default Name>"), true, true),
m_pcTargetDoc(0),
m_pcAttribute(0)
{ }

SP_IA_Command_SetAttribute::SP_IA_Command_SetAttribute(const wxString& p_sName, SP_DS_Attribute *p_pcAttribute, bool p_bHasConfig, bool p_bCanUndo)
: SP_IA_Command(SP_IA_CMD_SETATTRIBUTE, p_sName, p_bHasConfig, p_bCanUndo),
m_pcTargetDoc(0),
m_pcAttribute(p_pcAttribute)
{ }

SP_IA_Command_SetAttribute::~SP_IA_Command_SetAttribute()
{ 
	if (m_pcAttribute) wxDELETE(m_pcAttribute);
}


SP_IA_Command*
SP_IA_Command_SetAttribute::Copy(bool p_bTryInit) {
	SP_DS_Attribute *l_pcClone = 0;
	if (m_pcAttribute) l_pcClone = m_pcAttribute->Clone();
	
	SP_IA_Command_SetAttribute* l_pcResult = new SP_IA_Command_SetAttribute(GetName(), l_pcClone, HasConfig(), CanUndo());
	l_pcResult->SetDescription(m_sDescription);
	l_pcResult->SetNetclassNames(m_sSourceNetclass, m_sTargetNetclass);
	l_pcResult->SetCopyStamp(true);
	if (p_bTryInit) {
		l_pcResult->SetTargetGraph(m_pcTarget);
		l_pcResult->SetTriggerEvent(m_pcTriggerEvent);
		l_pcResult->SetDataProcessors(m_pcDataProcessors);
	}
	return l_pcResult;
}


bool
SP_IA_Command_SetAttribute::Do(SP_ListData* p_pcData) {

	DoDataProcessing(p_pcData);

	//we found nothing means that something went wrong...
	//...or the connected nets do not really correspond
	if (!p_pcData) return false;
	if (p_pcData->size() == 0) return false;

	m_mSavedValues.clear();
	if (!m_pcTargetDoc) m_pcTargetDoc = GetTargetGraph()->GetParentDoc();

	SP_ListData::iterator itD;
	SP_ListAttribute::const_iterator itA;
	SP_DS_Attribute *l_pcDataAttrib;
	wxString l_sKey;
	bool l_bAttribSet;
	
	for (itD = p_pcData->begin(); itD != p_pcData->end(); ++itD) {
wxString name = (*itD)->GetAttribute(wxT("Name"))->GetValueString();
		l_bAttribSet = false;
		for (itA = (*itD)->GetAttributes()->begin(); itA != (*itD)->GetAttributes()->end(); ++itA) {
			if (wxString((*itA)->GetName()).StartsWith(wxT("#IA#VALUE#"), &l_sKey)) {
				//we set this attribute value either if there is no certain attribute specified...
				//...or if this attribute corresponds to the specified one
				if (!m_pcAttribute || (m_pcAttribute && l_sKey.Cmp(m_pcAttribute->GetName()) == 0)) {
					l_pcDataAttrib = (*itD)->GetAttribute(l_sKey);
					if (l_pcDataAttrib) {
						//save old attribute value for later undo
						m_mSavedValues[l_pcDataAttrib] = l_pcDataAttrib->GetValueString();
						//setting new value
						l_pcDataAttrib->SetValueString((*itA)->GetValueString());
						l_bAttribSet = true;
					}
					(*itD)->Update(true);
				} 
			}
		}
		//if there is a certain attribute to set, which hasn't been done yet...
		if (m_pcAttribute && !l_bAttribSet) {
			//...we do that here, using the default value given by m_pcAttribute
			l_pcDataAttrib = (*itD)->GetAttribute(m_pcAttribute->GetName());
			if (l_pcDataAttrib) {
				m_mSavedValues[l_pcDataAttrib] = l_pcDataAttrib->GetValueString();
				l_pcDataAttrib->SetValueString(m_pcAttribute->GetValueString());
			}
			(*itD)->Update(true);
		}
	}
	//refresh target view
	//here everything updates fine...
	m_pcTargetDoc->Refresh();

	for (itD = p_pcData->begin(); itD != p_pcData->end(); ++itD) {
		SP_IA_DataProcessor::ReleaseData(*itD, false, true);
	}

	return true;
}


bool
SP_IA_Command_SetAttribute::Undo() {

	map<SP_DS_Attribute*, wxString>::iterator it;
	
	for (it = m_mSavedValues.begin(); it != m_mSavedValues.end(); ++it) {
		((*it).first)->SetValueString((*it).second);
		((*it).first)->Update(true);
	}
	//refresh target view
	//BUG: doesn't update wxT("0") values here, but works above...???
	m_pcTargetDoc->Refresh();

	return true;
}


void
SP_IA_Command_SetAttribute::AddToConfigDialog(wxWindow *p_pcParent, wxSizer *p_pcSizer) {

	m_cbUndo = new wxCheckBox(p_pcParent, -1, wxT("Preserve original attribute values"));
	m_cbUndo->SetValue(CanUndo());
	p_pcSizer->Add(m_cbUndo, 0, wxLEFT | wxTOP, 10);

}


void 
SP_IA_Command_SetAttribute::OnConfigDialogOk() {
	m_canUndo = m_cbUndo->IsChecked();
}
