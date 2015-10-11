//////////////////////////////////////////////////////////////////////
// $Author: crohr $
// $Version: 0.1 $
// $Date: 2015/10/08 $
// Short Description: dialog to merge nodes
//////////////////////////////////////////////////////////////////////
#include "sp_gui/dialogs/SP_DLG_MergeNodes.h"

#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gui/mdi/SP_MDI_View.h"
#include "sp_core/SP_Core.h"

#include "sp_ds/attributes/SP_DS_NameAttribute.h"

IMPLEMENT_CLASS(SP_DLG_MergeNodes, SP_DLG_DuplicateNodes)

BEGIN_EVENT_TABLE(SP_DLG_MergeNodes, SP_DLG_DuplicateNodes)
END_EVENT_TABLE()

SP_DLG_MergeNodes::SP_DLG_MergeNodes(SP_DS_Graph *p_graph, unsigned int p_netNumber, SP_ListGraphic* p_graphics, wxWindow *p_parent, const wxString& p_title) :
		SP_DLG_DuplicateNodes(p_graph, p_netNumber, p_graphics, p_parent, p_title)
{
	if (m_choice)
	{
		m_text->SetLabelText(wxT("min # of graphics "));
		LoadData(m_choice->GetStringSelection(), m_number->GetValue());
	}
}

SP_DLG_MergeNodes::~SP_DLG_MergeNodes()
{
}

void SP_DLG_MergeNodes::LoadData(const wxString& p_sNodeClassName, int p_nNrArcs)
{
	//Load data
	Clear();
	m_pcListCtrl->InsertColumn( 0, p_sNodeClassName );
	m_pcListCtrl->InsertColumn( 1, wxT("# of graphics") );

	int l_nPos = 0;
	long l_nIndex = 0;

	SP_DS_Nodeclass* l_pcNodeclass = m_graph->GetNodeclassByDisplayedName( p_sNodeClassName );
	if(l_pcNodeclass)
	{
		for( SP_DS_Node* l_pcNode : *l_pcNodeclass->GetElements())
		{
			wxString l_sElementName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
			SP_ListGraphic l_lGraphics;
			l_pcNode->GetGraphicsInNet(&l_lGraphics, m_netNumber, SP_ELEMENT_NODE);

			if (l_lGraphics.size() >= p_nNrArcs)
			{
				wxListItem l_cItem;
				l_cItem.SetBackgroundColour(*wxWHITE);
				l_cItem.m_col = 0;
				l_cItem.m_mask = wxLIST_MASK_TEXT;
				l_cItem.m_itemId = l_nPos++;
				l_cItem.m_text = l_sElementName;
				SP_ItemData* data = new SP_ItemData(l_lGraphics.front(), l_lGraphics.size(), l_sElementName);
				l_cItem.SetData(data);
				l_nIndex = m_pcListCtrl->InsertItem(l_cItem);
				m_pcListCtrl->SetItem(l_nIndex, 1, wxString::Format(wxT("%lu"), l_lGraphics.size()));
			}

		}
		m_pcListCtrl->SortItems(SP_CompareFunction, 1);
	}
}
