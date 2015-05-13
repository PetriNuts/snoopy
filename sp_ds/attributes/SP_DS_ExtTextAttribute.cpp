//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: ak $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2006/09/02 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_ds/attributes/SP_DS_ExtTextAttribute.h"
#include "sp_core/SP_Core.h"
#include "sp_ds/SP_DS_Node.h"
#include "sp_ds/SP_DS_Nodeclass.h"

#include "snoopy.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"

SP_DS_ExtTextAttribute::SP_DS_ExtTextAttribute(const wxString& p_pchName,
                                         const wxString& p_pchVal)
:SP_DS_Attribute(p_pchName, SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_TEXT),
m_sValue(p_pchVal)
{
}

SP_DS_Attribute*
SP_DS_ExtTextAttribute::Clone(bool p_bCloneGr)
{
    SP_DS_ExtTextAttribute* l_pcAttr = new SP_DS_ExtTextAttribute(GetName(), GetValue());

    // just clone the graphic stuff, too
    return CloneBase(l_pcAttr, p_bCloneGr);
}

bool
SP_DS_ExtTextAttribute::SetValue(const wxString& p_pchVal)
{
    if (!CheckDuplicateNodes(p_pchVal)) {
		return FALSE;
	}else{
		wxString l_sTemp(p_pchVal);

		// special, if the param contains '*' as wildcard, we replace every occurance
		// of '*' with the content of the old member value
		l_sTemp.Replace(SP_WILDCARD, m_sValue);

		// hack to make empty lines (only newline) show up in the canvas
		// l_sTemp.Replace(wxT("\n\n"), wxT("\n \n"));

		m_sValue = l_sTemp;
	}

    return TRUE;
}

bool
SP_DS_ExtTextAttribute::CheckDuplicateNodes(const wxString& p_pchVal)
{
	wxString l_sTemp(p_pchVal);
	l_sTemp.Replace(SP_WILDCARD, m_sValue);
	if (!l_sTemp.IsEmpty()
		&& (l_sTemp !=  m_sValue)) {

		SP_DS_Node* l_pcParent = dynamic_cast<SP_DS_Node*>(GetParent());
		if (!l_pcParent)
			return FALSE;

		if (l_pcParent->GetClassName()== wxT("Basic Event") ||
			l_pcParent->GetClassName()== wxT("Undeveloped Event")) {

			wxString searchClass = l_pcParent->GetClassName();
			SP_ListNodeclass::const_iterator ncIt;
			SP_GM_Docmanager* l_pcDM = wxGetApp().GetDocmanager();
			wxDocument* p_pcDoc = l_pcDM->GetCurrentDocument();
			SP_MDI_Doc* l_pcDoc = dynamic_cast<SP_MDI_Doc*>(p_pcDoc);

			const SP_ListNodeclass* ncList = l_pcDoc->GetGraph()->GetNodeclasses();

			CHECK_POINTER(ncList, return FALSE);

			SP_DS_Nodeclass* nc;
			SP_ListNode::const_iterator nIt;
			const SP_ListNode* nList;

			for (ncIt = ncList->begin(); ncIt != ncList->end(); ncIt++)  {
				nc = (*ncIt);
				nList = nc->GetElements();
				if (nc->GetName()== searchClass )
					for (nIt = nList->begin(); nIt != nList->end(); nIt++)
						if (((*nIt) != l_pcParent && !l_pcParent->GetLogical())) {

							wxString name = (*nIt)->GetAttribute(wxT("Name"))->GetValueString();

							if (name == l_sTemp) {
								int answer = SP_MESSAGEBOX(wxT("There is an other ") + searchClass + wxT(" with the same name.")
								wxT(" Are you sure that the ") + searchClass + wxT(" get the same node?"), wxT("Notification"),
								wxYES_NO | wxICON_QUESTION);
								if (answer == wxNO){
									return FALSE;
								}else{
									if (!(*nIt)->GetLogical() ) {
										if (!l_pcParent->GetLogical()) {
											l_pcParent->GetAttribute(wxT("Logic"))->SetValueString(wxT("1"));
											l_pcParent->SetLogical(TRUE);
											l_pcParent->Update(TRUE);
											l_pcParent->SynchronizeAttributes();
											//l_pcParent->Update(TRUE);
										}

										(*nIt)->GetAttribute(wxT("Logic"))->SetValueString(wxT("1"));
										(*nIt)->SetLogical(TRUE);

										(*nIt)->Update(TRUE);
										(*nIt)->SynchronizeAttributes();

										wxString sId = l_pcParent->GetAttribute(wxT("ID"))->GetValueString();
										(*nIt)->GetAttribute(wxT("ID"))->SetValueString(sId);

										//(*nIt)->Update(TRUE);
									}else if(!l_pcParent->GetLogical()) {
										l_pcParent->GetAttribute(wxT("Logic"))->SetValueString(wxT("1"));
										l_pcParent->SetLogical(TRUE);
										l_pcParent->SynchronizeAttributes();
										l_pcParent->Update(TRUE);
										wxString sId = (*nIt)->GetAttribute(wxT("ID"))->GetValueString();
										l_pcParent->GetAttribute(wxT("ID"))->SetValueString(sId);
											//l_pcParent->Update(TRUE);
									}

								}
							}
						}
			}
		}
	}

	return TRUE;
}
