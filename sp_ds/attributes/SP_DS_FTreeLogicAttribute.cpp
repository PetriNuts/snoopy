//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: akurth $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2007/01/30 10:35:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_ds/attributes/SP_DS_LogicAttribute.h"
#include "sp_ds/attributes/SP_DS_FTreeLogicAttribute.h"
#include "sp_core/SP_Core.h"
#include "sp_ds/SP_DS_Node.h"
#include "sp_ds/SP_DS_Nodeclass.h"
#include "snoopy.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"

SP_DS_FTreeLogicAttribute::SP_DS_FTreeLogicAttribute(const wxString& p_pchName,
                                           const wxString& p_pchRef,
                                           bool p_bVal)
:SP_DS_LogicAttribute(p_pchName, p_pchRef, p_bVal)
{

}

SP_DS_Attribute*
SP_DS_FTreeLogicAttribute::Clone(bool p_bCloneGr)
{
    SP_DS_FTreeLogicAttribute* l_pcAttr = new SP_DS_FTreeLogicAttribute(GetName(), GetReferenceAttribute(), GetValue());

    // just clone the graphic stuff, too
    return CloneBase(l_pcAttr, p_bCloneGr);
}

// reimplement
bool
SP_DS_FTreeLogicAttribute::SetValue(bool p_bVal)
{
#if 0
    if (m_bValue == p_bVal)
    {
        m_bOneShot = FALSE;
        return TRUE;
    }
#endif

	if (!checkStatusFTree(p_bVal))
		return false;
    m_bValue = p_bVal;

    return OnChangeState();
}


//////////////////////////////////////////////////////////////////////

bool
SP_DS_FTreeLogicAttribute::checkStatusFTree(bool p_bVal)
{
    SP_DS_Node* l_pcParent = dynamic_cast<SP_DS_Node*>(GetParent());

	if (l_pcParent->GetClassName()== wxT("Basic Event") ||
		l_pcParent->GetClassName()== wxT("Undeveloped Event")) {


		SP_ListEdge::const_iterator eIt;
		const SP_ListEdge* edgesSrc;
		edgesSrc = (dynamic_cast<SP_DS_Node*>(l_pcParent))->GetSourceEdges();

		if ((l_pcParent->GetLogical() != p_bVal) && (edgesSrc->size() > 1)) {
			SP_MESSAGEBOX(wxT("  independent basic/undeveloped event: there is only one exit edge. Please cancel superfluous edges!"),
			wxT("Error"), wxOK | wxICON_ERROR);
			return FALSE;

		} else {
			wxString name = l_pcParent->GetAttribute(wxT("Name"))->GetValueString();
			if (!name.IsEmpty()) {
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
					if (nc->GetName()== searchClass ) {
						for (nIt = nList->begin(); nIt != nList->end(); nIt++) {
							wxString tname = (*nIt)->GetAttribute(wxT("Name"))->GetValueString();
							if (name == tname && (*nIt)->GetLogical() && !l_pcParent->GetLogical()) {
								int answer = SP_MESSAGEBOX(wxT("There is an other ") + searchClass + wxT(" with the same name.")
								wxT(" Are you sure that these ") + searchClass + wxT("s are similar?"), wxT("Identical Nodes?"),
								wxYES_NO | wxICON_QUESTION);
								if (answer == wxNO)
									return FALSE;

							}//endif
						}//endfor
					}//endif
				}//endfor
			}//endif
		}//endif
	}//endif

	return TRUE;
}

