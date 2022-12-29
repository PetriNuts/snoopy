//////////////////////////////////////////////////////////////////////
// $Author: r2 $
// $Version: 0.1 $
// $Date: 2007/01/26 $
// Short Description: PT nets with time extensions
//////////////////////////////////////////////////////////////////////
#include "sp_ds/netclasses/SP_DS_TimePT.h"

#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/SP_DS_Nodeclass.h"

#include "sp_ds/attributes/SP_DS_TextAttribute.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_gr/attributes/SP_GR_TextAttribute.h"
#include "sp_gr/attributes/SP_GR_ColListAttribute.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogText.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogColList.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"

SP_DS_TimePT::SP_DS_TimePT() :
	SP_DS_SimplePed(SP_DS_TIMEPN_CLASS)
{
	m_sExtension = wxT("tpn");
	m_sFilter = wxT("*.tpn;*.sptpt");
}

SP_DS_TimePT::SP_DS_TimePT(const wxString& p_pchName) :
	SP_DS_SimplePed(p_pchName)
{
	m_sExtension = wxT("sptpt");
	m_sFilter = wxT("*.tpn;*.sptpt");
}

SP_DS_Graph* SP_DS_TimePT::CreateGraph(SP_DS_Graph* p_graph,SP_MapString2Int p_mapAttributeValue )
{
	if (!SP_DS_SimplePed::CreateGraph(p_graph))
		return NULL;

	SP_DS_Attribute* l_pcAttr;
	SP_Graphic* l_pcGrAttr;
	SP_DS_Nodeclass* l_pcNS;

	//////////////////////////////////////////////////////////////////////////////
	l_pcNS = p_graph->GetNodeclass(wxT("Transition"));

	SP_DS_ColListAttribute* l_pcColList;
	unsigned int l_nNewRow;
	//Duration Attribute
	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_ColListAttribute( wxT("Duration"), SP_COLLIST_DOUBLE, 2 ));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogColList( wxT("Duration") ));

	l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcAttr );
	l_pcColList->SetColLabel( 0, wxT("Duration List"));
	l_pcColList->SetColLabel( 1, wxT("Duration"));
	l_nNewRow = l_pcColList->AppendEmptyRow();
	l_pcColList->SetCell(l_nNewRow, 0, wxT("Main"));
	l_pcColList->SetCell(l_nNewRow, 1, wxT("0"));

	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_ColListAttribute( l_pcAttr, wxT("<%>") ));
	l_pcGrAttr->SetOffsetX(0);
	l_pcGrAttr->SetOffsetY(20);
	l_pcGrAttr->SetShow(TRUE);
	l_pcAttr->SetGlobalShow();

	//EFT, LFT Attribute
	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_ColListAttribute( wxT("Interval"), SP_COLLIST_STRING, 3 ));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogColList( wxT("Interval") ));

	l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcAttr );
	l_pcColList->SetColLabel( 0, wxT("Interval List"));
	l_pcColList->SetColLabel( 1, wxT("EFT"));
	l_pcColList->SetColLabel( 2, wxT("LFT"));
	l_nNewRow = l_pcColList->AppendEmptyRow();
	l_pcColList->SetCell(l_nNewRow, 0, wxT("Main"));
	l_pcColList->SetCell(l_nNewRow, 1, wxT("?"));
	l_pcColList->SetCell(l_nNewRow, 2, wxT("?"));

	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_ColListAttribute( l_pcAttr, wxT("[%]") ));
	l_pcGrAttr->SetOffsetX(0);
	l_pcGrAttr->SetOffsetY(32);
	l_pcGrAttr->SetShow(TRUE);
	l_pcAttr->SetGlobalShow();

	return p_graph;
}

