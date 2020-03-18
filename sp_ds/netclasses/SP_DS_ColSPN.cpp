//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: F. Liu $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2009/11/1 11:55:00 $
// Short Description: colored Stochastic Petri nets
//////////////////////////////////////////////////////////////////////

#include "sp_ds/netclasses/SP_DS_ColSPN.h"

#include "sp_ds/SP_DS_Edgeclass.h"
#include "sp_ds/SP_DS_Nodeclass.h"
#include "sp_ds/SP_DS_Metadataclass.h"

#include "sp_ds/animators/ani_ColSPN/SP_DS_ColStPlaceAnimator.h"
#include "sp_ds/animators/ani_ColSPN/SP_DS_ColStTransAnimator.h"


#include "sp_ds/attributes/SP_DS_BoolAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/attributes/SP_DS_NumberAttribute.h"
#include "sp_ds/attributes/SP_DS_DoubleAttribute.h"
#include "sp_ds/attributes/SP_DS_LogicAttribute.h"
#include "sp_ds/attributes/SP_DS_IdAttribute.h"
#include "sp_ds/attributes/SP_DS_MultiplicityAttribute.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"

#include "sp_ds/attributes/SP_DS_ColListAttribute.h"

#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/extensions/SP_DS_Coarse.h"


#include "sp_gr/shapes/SP_GR_Ellipse.h"
#include "sp_gr/shapes/SP_GR_Circle.h"
#include "sp_gr/shapes/SP_GR_Rectangle.h"
#include "sp_gr/shapes/SP_GR_ExtendedEllipse.h"

#include "sp_gr/edges/SP_GR_ArrowEdge.h"
#include "sp_gr/edges/SP_GR_ExtendedEdge.h"

#include "sp_gr/attributes/SP_GR_TextAttribute.h"
#include "sp_gr/attributes/SP_GR_NumberAttribute.h"
#include "sp_gr/attributes/SP_GR_MultiplicityAttribute.h"
#include "sp_gr/attributes/SP_GR_MarkAttribute.h"
#include "sp_gr/attributes/SP_GR_CommentAttribute.h"
#include "sp_gr/attributes/SP_GR_HiddenTextAttribute.h"

#include "sp_gr/attributes/gra_SPN/SP_GR_StFunctionAttribute.h"
#include "sp_gr/attributes/gra_SPN/SP_GR_StParameterListAttribute.h"

#include "sp_gui/widgets/dialogs/SP_WDG_DialogChoice.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogUnsigned.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogBool.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogShowOnly.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogGraphic.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogMultiline.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogExtendedGraphic.h"

#include "sp_gui/widgets/dialogs/wid_dia_SPN/SP_WDG_StParameterList.h"

#include "sp_gui/widgets/dialogs/wid_dia_SPN/SP_WDG_StLookupTable.h"
#include "sp_gui/widgets/dialogs/wid_dia_SPN/SP_WDG_StLookupTableName.h"

#include "sp_gui/widgets/dialogs/wid_dia_SPN/SP_WDG_StDummyMarking.h"

#include "sp_gui/widgets/dialogs/SP_WDG_DialogColList.h" 
#include "sp_gr/attributes/SP_GR_ColListAttribute.h" 

#include "sp_utilities.h"

#include "snoopy.h"
#include "sp_core/SP_GPR_Elements.h"
#include "sp_core/SP_GPR_Animation.h"

#include "sp_gui/windows/SP_GUI_Mainframe.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_core/SP_Core.h"

//Special for colored SPN
#include "sp_gui/dialogs/dia_ColSPN/SP_WDG_ColStMarkingList.h"
#include "sp_gui/dialogs/dia_ColSPN/SP_WDG_ColStFunctionList.h"
#include "sp_gui/dialogs/dia_CPN/SP_WDG_ArcInscription.h"
#include "sp_gui/dialogs/dia_CPN/SP_WDG_Guard.h"

#include "sp_ds/animators/ani_ColSPN/SP_DS_ColStCoarseTransAnimator.h"
#include "sp_ds/animators/ani_ColSPN/SP_DS_ColStAnimation.h"
#include "sp_ds/attributes/SP_DS_TypeAttribute.h"//by george, for constants harmonizing 

SP_DS_ColSPN::SP_DS_ColSPN()
:SP_DS_EventSPN( SP_DS_COLSPN_CLASS )
{
	m_sExtension = wxT("colspn");
	m_sFilter = wxT("*.colspn;*.colstochpn");

	//register node classes for editing their graphic properties

	wxGetApp().GetElementPrefs()->RegisterNodeclass( GetName(), wxT("Place") );
	wxGetApp().GetElementPrefs()->RegisterNodeclass( GetName(), wxT("Transition") );
	wxGetApp().GetElementPrefs()->RegisterNodeclass( GetName(), wxT("Immediate Transition") );
	wxGetApp().GetElementPrefs()->RegisterNodeclass( GetName(), wxT("Deterministic Transition") );
	wxGetApp().GetElementPrefs()->RegisterNodeclass( GetName(), wxT("Scheduled Transition") );
	wxGetApp().GetElementPrefs()->RegisterNodeclass( GetName(), wxT("Parameter") );
	wxGetApp().GetElementPrefs()->RegisterNodeclass( GetName(), wxT("LookupTable") );

}


SP_DS_ColSPN::SP_DS_ColSPN( const wxString& p_pchName )
:SP_DS_EventSPN( p_pchName )
{
	m_sExtension = wxT("colspn");
	m_sFilter = wxT("*.colspn;*.colstochpn");

	//register node classes for editing their graphic properties
	wxGetApp().GetElementPrefs()->RegisterNodeclass( GetName(), wxT("Place") );
	wxGetApp().GetElementPrefs()->RegisterNodeclass( GetName(), wxT("Transition") );
	wxGetApp().GetElementPrefs()->RegisterNodeclass( GetName(), wxT("Immediate Transition") );
	wxGetApp().GetElementPrefs()->RegisterNodeclass( GetName(), wxT("Deterministic Transition") );
	wxGetApp().GetElementPrefs()->RegisterNodeclass( GetName(), wxT("Scheduled Transition") );
	wxGetApp().GetElementPrefs()->RegisterNodeclass( GetName(), wxT("Parameter") );
	wxGetApp().GetElementPrefs()->RegisterNodeclass( GetName(), wxT("LookupTable") );

}


SP_DS_Graph*
SP_DS_ColSPN::CreateGraph( SP_DS_Graph* p_pcGraph )
{

	if ( ! SP_DS_EventSPN::CreateGraph( p_pcGraph ) ) 
	{
		return NULL;
	}

	SP_GR_Node* l_pcGr;
    SP_DS_Coarse* l_pcCoarse;
	SP_DS_Attribute* l_pcAttr;
	SP_Graphic* l_pcGrAttr;
	SP_DS_Nodeclass* l_pcNC;
	SP_DS_ColListAttribute* l_pcColList;
	unsigned int l_nNewRow;

	//////////////////////////////////////////////////////////////////////////////
	l_pcNC = p_pcGraph->GetNodeclass( wxT("Place") );

	// animator
	l_pcNC->AddAnimator( new SP_DS_ColStPlaceAnimator( wxT("Marking") ) );


	l_pcAttr = l_pcNC->GetPrototype()->GetAttribute( wxT("Marking") );
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogShowOnly(wxT("Markings")));

	// special EventSPN components
	l_pcAttr = l_pcNC->AddAttribute( new SP_DS_ColListAttribute( SP_DS_CPN_MARKINGLIST, SP_COLLIST_UNSIGNED_INTEGER, 3 ) );
	l_pcAttr->SetDisplayName(wxT("Marking set"));
	//Colors and marking list
	l_pcAttr->RegisterDialogWidget( new SP_WDG_ColStMarkingList( wxT("Markings") ) );
	l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >( l_pcAttr );

	l_pcColList->SetColLabel( 0, wxT("Main: Color(s)") );
	l_pcColList->SetColLabel( 1, wxT("Main: Marking") );
	l_pcColList->SetColLabel( 2, wxT("Product Color"));//by george
	l_pcColList->Clear();	
	l_nNewRow = l_pcColList->AppendEmptyRow();
	l_pcColList->SetCell(l_nNewRow, 0, wxT("all()"));
	l_pcColList->SetCell(l_nNewRow, 1, wxT("0"));
	l_pcColList->SetCell(l_nNewRow, 2, wxT("()"));//by george
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_ColListAttribute( l_pcAttr, wxT("%") ));
	l_pcGrAttr->SetTextColor( wxGetApp().GetElementPrefs()->GetMarkingColor(GetName(), l_pcNC->GetDisplayName()));	
	l_pcGrAttr->SetOffsetX(45);
	l_pcGrAttr->SetOffsetY(-25);  
	l_pcGrAttr->SetShow(TRUE);
	l_pcAttr->SetGlobalShow();
    


	// Colorset name
	l_pcAttr = l_pcNC->AddAttribute(new SP_DS_TextAttribute(SP_DS_CPN_COLORSETNAME, wxT("Dot")));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogChoice(wxT("Markings")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
	l_pcGrAttr->SetTextColor( wxGetApp().GetElementPrefs()->GetColorSetColor(GetName(), l_pcNC->GetDisplayName()));
	l_pcGrAttr->SetOffsetY(-20);
	l_pcGrAttr->SetShow(TRUE);
	l_pcGrAttr->SetBrush(wxTheBrushList->FindOrCreateBrush(*wxBLUE));
	l_pcAttr->SetGlobalShow();	

	//////////////////////////////////////////////////////////////////////////////
	l_pcNC = p_pcGraph->GetNodeclass( wxT("Transition") );
	//////////////////////////////////////////////////////////////////////////////
	l_pcAttr = l_pcNC->GetPrototype()->GetAttribute( wxT("FunctionList") );	
	l_pcAttr->RegisterDialogWidget( new SP_WDG_ColStFunctionList( wxT("Functions") ) );
	l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >( l_pcAttr );

	l_pcColList->SetColLabel( 0, wxT("Predicate") );
	l_pcColList->SetColLabel( 1, wxT("Main: Function") );
	l_pcColList->Clear();	
	l_nNewRow = l_pcColList->AppendEmptyRow();
	l_pcColList->SetCell(l_nNewRow, 0, wxT("true"));
	l_pcColList->SetCell(l_nNewRow, 1, wxT("MassAction(1)"));


	//////////////////////////////////////////////////////////////////////////////
	// special ColoredSPN components
	l_pcAttr = l_pcNC->AddAttribute( new SP_DS_ColListAttribute( SP_DS_CPN_GUARDLIST, SP_COLLIST_UNSIGNED_INTEGER, 2 ) );
	l_pcAttr->RegisterDialogWidget( new SP_WDG_Guard( wxT("Guards") ) );

	l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >( l_pcAttr );
	l_pcColList->SetColLabel( 0, wxT("Guard set") );
	l_pcColList->SetColLabel( 1, wxT("Guard") );
	l_nNewRow = l_pcColList->AppendEmptyRow();
	l_pcColList->SetCell(l_nNewRow, 0, wxT("Main"));
	l_pcColList->SetCell(l_nNewRow, 1, wxT(""));

	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_ColListAttribute( l_pcAttr, wxT("[%]") ));
	l_pcGrAttr->SetTextColor( wxGetApp().GetElementPrefs()->GetGuardColor(GetName(), l_pcNC->GetDisplayName()));
	l_pcGrAttr->SetOffsetX(45);
	l_pcGrAttr->SetOffsetY(-25);
	l_pcGrAttr->SetShow(TRUE);
	l_pcAttr->SetGlobalShow();	

	//////////////////////////////////////////////////////////////////////////////
	l_pcNC->AddAnimator(new SP_DS_ColStTransAnimator());	


	//////////////////////////////////////////////////////////////////////////////
	l_pcNC = p_pcGraph->GetNodeclass( wxT("Immediate Transition") );
	//////////////////////////////////////////////////////////////////////////////
	l_pcAttr = l_pcNC->GetPrototype()->GetAttribute( wxT("FunctionList") );	
	l_pcAttr->RegisterDialogWidget( new SP_WDG_ColStFunctionList( wxT("Weight") ) );

	l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >( l_pcAttr );

	l_pcColList->SetColLabel( 0, wxT("Predicate") );
	l_pcColList->SetColLabel( 1, wxT("Main: Weight") );
	l_pcColList->Clear();	
	l_nNewRow = l_pcColList->AppendEmptyRow();
	l_pcColList->SetCell(l_nNewRow, 0, wxT("true"));
	l_pcColList->SetCell(l_nNewRow, 1, wxT("1"));

	//////////////////////////////////////////////////////////////////////////////
	// special ColoredSPN components
	l_pcAttr = l_pcNC->AddAttribute( new SP_DS_ColListAttribute( SP_DS_CPN_GUARDLIST, SP_COLLIST_UNSIGNED_INTEGER, 2 ) );
	l_pcAttr->RegisterDialogWidget( new SP_WDG_Guard( wxT("Guards") ) );

	l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >( l_pcAttr );
	l_pcColList->SetColLabel( 0, wxT("Guard set") );
	l_pcColList->SetColLabel( 1, wxT("Guard") );
	l_nNewRow = l_pcColList->AppendEmptyRow();
	l_pcColList->SetCell(l_nNewRow, 0, wxT("Main"));
	l_pcColList->SetCell(l_nNewRow, 1, wxT(""));

	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_ColListAttribute( l_pcAttr, wxT("[%]") ));
	l_pcGrAttr->SetTextColor( wxGetApp().GetElementPrefs()->GetGuardColor(GetName(), l_pcNC->GetDisplayName()));
	l_pcGrAttr->SetOffsetX(45);
	l_pcGrAttr->SetOffsetY(-25);
	l_pcGrAttr->SetShow(TRUE);
	l_pcAttr->SetGlobalShow();

	//////////////////////////////////////////////////////////////////////////////
	l_pcNC->AddAnimator(new SP_DS_ColStTransAnimator());
	//////////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////////
	l_pcNC = p_pcGraph->GetNodeclass( wxT("Deterministic Transition") );
	//////////////////////////////////////////////////////////////////////////////
	l_pcAttr = l_pcNC->GetPrototype()->GetAttribute( wxT("DelayList") );
	l_pcAttr->RegisterDialogWidget( new SP_WDG_DialogColList( wxT("Delay") ) );

	l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >( l_pcAttr );

	l_pcColList->SetColLabel( 0, wxT("Predicate") );
	l_pcColList->SetColLabel( 1, wxT("Main: Delay") );
	l_pcColList->Clear();	
	l_nNewRow = l_pcColList->AppendEmptyRow();
	l_pcColList->SetCell(l_nNewRow, 0, wxT("true"));
	l_pcColList->SetCell(l_nNewRow, 1, wxT("0"));

	//////////////////////////////////////////////////////////////////////////////
	// special ColoredSPN components
	l_pcAttr = l_pcNC->AddAttribute( new SP_DS_ColListAttribute( SP_DS_CPN_GUARDLIST, SP_COLLIST_UNSIGNED_INTEGER, 2 ) );
	l_pcAttr->RegisterDialogWidget( new SP_WDG_Guard( wxT("Guards") ) );

	l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >( l_pcAttr );
	l_pcColList->SetColLabel( 0, wxT("Guard set") );
	l_pcColList->SetColLabel( 1, wxT("Guard") );
	l_nNewRow = l_pcColList->AppendEmptyRow();
	l_pcColList->SetCell(l_nNewRow, 0, wxT("Main"));
	l_pcColList->SetCell(l_nNewRow, 1, wxT(""));

	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_ColListAttribute( l_pcAttr, wxT("[%]") ));
	l_pcGrAttr->SetTextColor( wxGetApp().GetElementPrefs()->GetGuardColor(GetName(), l_pcNC->GetDisplayName()));
	l_pcGrAttr->SetOffsetX(45);
	l_pcGrAttr->SetOffsetY(-25);
	l_pcGrAttr->SetShow(TRUE);
	l_pcAttr->SetGlobalShow();

	//////////////////////////////////////////////////////////////////////////////
	l_pcNC->AddAnimator(new SP_DS_ColStTransAnimator());
	//////////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////////
	l_pcNC = p_pcGraph->GetNodeclass( wxT("Scheduled Transition") );
	//////////////////////////////////////////////////////////////////////////////
	l_pcAttr = l_pcNC->GetPrototype()->GetAttribute( wxT("PeriodicList") );
	l_pcAttr->RegisterDialogWidget( new SP_WDG_DialogColList( wxT("Periodic") ) );

	l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >( l_pcAttr );

	l_pcColList->SetColLabel( 0, wxT("Predicate"));
	l_pcColList->SetColLabel( 1, wxT("Main: Begin"));
	l_pcColList->SetColLabel( 2, wxT("Main: Repetition"));
	l_pcColList->SetColLabel( 3, wxT("Main: End"));
	l_pcColList->Clear();	
	l_nNewRow = l_pcColList->AppendEmptyRow();
	l_pcColList->SetCell(l_nNewRow, 0, wxT("true"));
	l_pcColList->SetCell(l_nNewRow, 1, wxT("_SimStart"));
	l_pcColList->SetCell(l_nNewRow, 2, wxT("1"));
	l_pcColList->SetCell(l_nNewRow, 3, wxT("_SimEnd"));

	//////////////////////////////////////////////////////////////////////////////
	// special ColoredSPN components
	l_pcAttr = l_pcNC->AddAttribute( new SP_DS_ColListAttribute( SP_DS_CPN_GUARDLIST, SP_COLLIST_UNSIGNED_INTEGER, 2 ) );
	l_pcAttr->RegisterDialogWidget( new SP_WDG_Guard( wxT("Guards") ) );	
	l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >( l_pcAttr );
	l_pcColList->SetColLabel( 0, wxT("Guard set") );
	l_pcColList->SetColLabel( 1, wxT("Guard") );
	l_nNewRow = l_pcColList->AppendEmptyRow();
	l_pcColList->SetCell(l_nNewRow, 0, wxT("Main"));
	l_pcColList->SetCell(l_nNewRow, 1, wxT(""));

	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_ColListAttribute( l_pcAttr, wxT("[%]") ));
	l_pcGrAttr->SetTextColor( wxGetApp().GetElementPrefs()->GetGuardColor(GetName(), l_pcNC->GetDisplayName()));
	l_pcGrAttr->SetOffsetX(45);
	l_pcGrAttr->SetOffsetY(-25);
	l_pcGrAttr->SetShow(TRUE);
	l_pcAttr->SetGlobalShow();

	//////////////////////////////////////////////////////////////////////////////
	l_pcNC->AddAnimator(new SP_DS_ColStTransAnimator());
	//////////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////////
	l_pcNC = p_pcGraph->GetNodeclass( wxT("Coarse Transition") );
	l_pcNC->AddAnimator(new SP_DS_ColStCoarseTransAnimator());

	//////////////////////////////////////////////////////////////////////////////
 
	//////////////////////////////////////////////////////////////////////////////
	//we cannot remove param nodes, because we still need them for loading old constants
	l_pcNC = p_pcGraph->AddNodeclass( new SP_DS_Nodeclass( p_pcGraph, wxT("Parameter") ) );
	//l_pcNC->SetShortcut( wxT("K") ); by georg, to disable the shortcut of parameter nodes
	l_pcAttr = l_pcNC->AddAttribute(new SP_DS_NameAttribute( wxT("Name") ) );
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText( wxT("General") ) );
	l_pcGrAttr = l_pcAttr->AddGraphic( new SP_GR_TextAttribute( l_pcAttr ) );
	l_pcGrAttr->SetOffsetY( 20 );
	l_pcGrAttr->SetShow( TRUE );
	l_pcAttr->SetGlobalShow() ;

	l_pcAttr = l_pcNC->AddAttribute( new SP_DS_IdAttribute( wxT("ID") ) );
	l_pcAttr->RegisterDialogWidget( new SP_WDG_DialogShowOnly( wxT("General") ) );
	l_pcGrAttr = l_pcAttr->AddGraphic( new SP_GR_NumberAttribute( l_pcAttr, wxT("_pa%_") ) );
	l_pcGrAttr->SetOffsetX( 20 );
	l_pcGrAttr->SetShow( FALSE );
	l_pcAttr->SetGlobalShow();

    l_pcAttr = l_pcNC->AddAttribute(new SP_DS_TextAttribute(wxT("Comment"), wxT("")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
    l_pcAttr->SetGlobalShow();
    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
    l_pcGrAttr->SetOffsetX(40);

    l_pcAttr = l_pcNC->AddAttribute( new SP_DS_ColListAttribute( wxT("ParameterList"), SP_COLLIST_DOUBLE, 2 ) );
	l_pcAttr->SetDisplayName(wxT("Parameter set"));
	l_pcAttr->RegisterDialogWidget( new SP_WDG_StParameterList( wxT("Parameters") ) );

	l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >( l_pcAttr );
	l_pcColList->SetColLabel( 0, wxT("Parameter set") );
	l_pcColList->SetColLabel( 1, wxT("Parameter") );
	l_nNewRow = l_pcColList->AppendEmptyRow();
	l_pcColList->SetCell(l_nNewRow, 0, wxT("Main"));
	l_pcColList->SetCell(l_nNewRow, 1, wxT("1"));

	l_pcAttr->AddGraphic(new SP_GR_StParameterListAttribute(l_pcAttr));

	int paheight = wxGetApp().GetElementPrefs()->GetNodeHeight( GetName(), wxT("Parameter") );
	int pawidth = wxGetApp().GetElementPrefs()->GetNodeWidth(GetName(), wxT("Parameter"));
	if(pawidth == DEFAULT_WIDTH) { pawidth = 2*pawidth; }
   	l_pcGr = new SP_GR_ExtendedEllipse(l_pcNC->GetPrototype(),pawidth, paheight,TRUE,SP_EXTENDED_TYPE_DEFAULT,3,wxColour(128, 128, 128));
	l_pcGr->SetDefaultPen(wxThePenList->FindOrCreatePen(wxColour(128, 128, 128), 3 ));
   	l_pcGr->SetFixedSize(wxGetApp().GetElementPrefs()->GetNodeFixed(GetName(), l_pcNC->GetName()));
	l_pcNC->SetGraphic( l_pcGr );
	l_pcNC->RegisterGraphicWidget( new SP_WDG_DialogGraphic( wxT("Graphic") ) );
	l_pcNC->SetShowInElementTree(false);//by george, do not display the parameter nodes in the elements tree of colspn net class
	//////////////////////////////////////////////////////////////////////////////
	l_pcNC = p_pcGraph->AddNodeclass(new SP_DS_Nodeclass(p_pcGraph, wxT("Coarse Parameter") ));
	//l_pcNC->SetShortcut(wxT("Shift+A"));//disable short cut 
	l_pcAttr = l_pcNC->AddAttribute(new SP_DS_NameAttribute(wxT("Name")));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
	l_pcGrAttr->SetOffsetY(20);
	l_pcGrAttr->SetShow(TRUE);
	l_pcAttr->SetGlobalShow();

	l_pcAttr = l_pcNC->AddAttribute(new SP_DS_TextAttribute(wxT("Comment"), wxT("")));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
	l_pcAttr->SetGlobalShow();
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
	l_pcGrAttr->SetOffsetX(40);
	l_pcGrAttr->SetShow(TRUE);


	// subnet property for this class
	l_pcCoarse = new SP_DS_Coarse(wxT("Subnet"), wxT("Parameter"), p_pcGraph, FALSE);
	l_pcCoarse->SetLabelAttribute(wxT("Name"));
	l_pcNC->SetCoarse(l_pcCoarse);

   	l_pcGr = new SP_GR_ExtendedEllipse(l_pcNC->GetPrototype(),pawidth, paheight,TRUE,SP_EXTENDED_TYPE_TWO,3,wxColour(128, 128, 128));
	l_pcGr->SetDefaultPen(wxThePenList->FindOrCreatePen(wxColour(128, 128, 128), 3 ));
   	l_pcGr->SetFixedSize(wxGetApp().GetElementPrefs()->GetNodeFixed(GetName(), wxT("Parameter")));
	l_pcNC->SetGraphic(l_pcGr);
	l_pcNC->RegisterGraphicWidget(new SP_WDG_DialogGraphic(wxT("Graphic")));
	l_pcNC->SetShowInElementTree(false);//by george, do not display the coarse parameter nodes in the elements tree of colspn net class


	//Edges
	//////////////////////////////////////////////////////////////////////////////
	SP_DS_Edgeclass* l_pcEC;
	l_pcEC = p_pcGraph->GetEdgeclass( wxT("Edge") );

	l_pcAttr = l_pcEC->GetPrototype()->GetAttribute( wxT("Multiplicity") );

	//remove expression attribute from pn
	//////////////////////////////////////////////////////////////////////////////////////////
	l_pcAttr->RemoveAllGraphics(true);
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_HiddenTextAttribute(l_pcAttr));
	l_pcGrAttr->SetOffsetX(20);
	l_pcGrAttr->SetShow(false);
	l_pcAttr->RegisterDialogWidget( new SP_WDG_StDummyMarking( wxT("Multiplicities") ) );
	//////////////////////////////////////////////////////////////////////////////////////////

	l_pcAttr = l_pcEC->AddAttribute( new SP_DS_ColListAttribute( SP_DS_CPN_INSCRIPTION, SP_COLLIST_STRING, 2 ) );
	l_pcAttr->RegisterDialogWidget( new SP_WDG_ArcInscription( wxT("Expression") ) );

	l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >( l_pcAttr );
	l_pcColList->SetColLabel( 0, wxT("Expression set") );
	l_pcColList->SetColLabel( 1, wxT("Expression") );
	l_nNewRow = l_pcColList->AppendEmptyRow();
	l_pcColList->SetCell(l_nNewRow, 0, wxT("Main"));
	l_pcColList->SetCell(l_nNewRow, 1, wxT("dot"));

	

	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_ColListAttribute( l_pcAttr, wxT("%") ));
	l_pcGrAttr->SetTextColor( wxGetApp().GetElementPrefs()->GetExpressionColor(GetName()));
	l_pcGrAttr->SetOffsetX(25);
	l_pcGrAttr->SetOffsetY(0);
	l_pcGrAttr->SetShow( TRUE );
	l_pcAttr->SetGlobalShow();

	//////////////////////////////////////////////////////////////////////////////
	
	l_pcEC = p_pcGraph->GetEdgeclass( wxT("Read Edge") );

	l_pcAttr = l_pcEC->GetPrototype()->GetAttribute( wxT("Multiplicity") );

	//remove expression attribute from pn
	//////////////////////////////////////////////////////////////////////////////////////////
	l_pcAttr->RemoveAllGraphics(true);
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_HiddenTextAttribute(l_pcAttr));
	l_pcGrAttr->SetOffsetX(20);
	l_pcGrAttr->SetShow(false);
	l_pcAttr->RegisterDialogWidget( new SP_WDG_StDummyMarking( wxT("Multiplicities") ) );
	//////////////////////////////////////////////////////////////////////////////////////////

	l_pcAttr = l_pcEC->AddAttribute( new SP_DS_ColListAttribute( SP_DS_CPN_INSCRIPTION, SP_COLLIST_STRING, 2 ) );
	l_pcAttr->RegisterDialogWidget( new SP_WDG_ArcInscription( wxT("Expression") ) );

	l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >( l_pcAttr );
	l_pcColList->SetColLabel( 0, wxT("Expression set") );
	l_pcColList->SetColLabel( 1, wxT("Expression") );
	l_nNewRow = l_pcColList->AppendEmptyRow();
	l_pcColList->SetCell(l_nNewRow, 0, wxT("Main"));
	l_pcColList->SetCell(l_nNewRow, 1, wxT("dot"));

	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_ColListAttribute( l_pcAttr, wxT("%") ));
	l_pcGrAttr->SetTextColor( wxGetApp().GetElementPrefs()->GetExpressionColor(GetName()));
	l_pcGrAttr->SetOffsetX(25);
	l_pcGrAttr->SetOffsetY(0);
	l_pcGrAttr->SetShow(TRUE);
	l_pcAttr->SetGlobalShow();

	//////////////////////////////////////////////////////////////////////////////
	
	l_pcEC = p_pcGraph->GetEdgeclass( wxT("Reset Edge") );

	l_pcAttr = l_pcEC->AddAttribute( new SP_DS_ColListAttribute( SP_DS_CPN_INSCRIPTION, SP_COLLIST_STRING, 2 ) );
	l_pcAttr->RegisterDialogWidget( new SP_WDG_ArcInscription( wxT("Expression") ) );

	l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >( l_pcAttr );
	l_pcColList->SetColLabel( 0, wxT("Expression set") );
	l_pcColList->SetColLabel( 1, wxT("Expression") );
	l_nNewRow = l_pcColList->AppendEmptyRow();
	l_pcColList->SetCell(l_nNewRow, 0, wxT("Main"));
	l_pcColList->SetCell(l_nNewRow, 1, wxT("dot"));

	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_ColListAttribute( l_pcAttr, wxT("%") ));
	l_pcGrAttr->SetTextColor( wxGetApp().GetElementPrefs()->GetExpressionColor(GetName()));
	l_pcGrAttr->SetOffsetX(25);
	l_pcGrAttr->SetOffsetY(0);
	l_pcGrAttr->SetShow(TRUE);
	l_pcAttr->SetGlobalShow();

	//////////////////////////////////////////////////////////////////////////////
	
	l_pcEC = p_pcGraph->GetEdgeclass( wxT("Equal Edge") );

	l_pcAttr = l_pcEC->GetPrototype()->GetAttribute( wxT("Multiplicity") );

	//remove expression attribute from pn
	//////////////////////////////////////////////////////////////////////////////////////////
	l_pcAttr->RemoveAllGraphics(true);
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_HiddenTextAttribute(l_pcAttr));
	l_pcGrAttr->SetOffsetX(20);
	l_pcGrAttr->SetShow(false);
	l_pcAttr->RegisterDialogWidget( new SP_WDG_StDummyMarking( wxT("Multiplicities") ) );
	//////////////////////////////////////////////////////////////////////////////////////////

	l_pcAttr = l_pcEC->AddAttribute( new SP_DS_ColListAttribute( SP_DS_CPN_INSCRIPTION, SP_COLLIST_STRING, 2 ) );
	l_pcAttr->RegisterDialogWidget( new SP_WDG_ArcInscription( wxT("Expression") ) );

	l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >( l_pcAttr );
	l_pcColList->SetColLabel( 0, wxT("Expression set") );
	l_pcColList->SetColLabel( 1, wxT("Expression") );
	l_nNewRow = l_pcColList->AppendEmptyRow();
	l_pcColList->SetCell(l_nNewRow, 0, wxT("Main"));
	l_pcColList->SetCell(l_nNewRow, 1, wxT("dot"));

	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_ColListAttribute( l_pcAttr, wxT("%") ));
	l_pcGrAttr->SetTextColor( wxGetApp().GetElementPrefs()->GetExpressionColor(GetName()));
	l_pcGrAttr->SetOffsetX(25);
	l_pcGrAttr->SetOffsetY(0);
	l_pcGrAttr->SetShow(TRUE);
	l_pcAttr->SetGlobalShow();
	//////////////////////////////////////////////////////////////////////////////
	
	l_pcEC = p_pcGraph->GetEdgeclass( wxT("Inhibitor Edge") );

	l_pcAttr = l_pcEC->GetPrototype()->GetAttribute( wxT("Multiplicity") );

	//remove expression attribute from pn
	//////////////////////////////////////////////////////////////////////////////////////////
	l_pcAttr->RemoveAllGraphics(true);
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_HiddenTextAttribute(l_pcAttr));
	l_pcGrAttr->SetOffsetX(20);
	l_pcGrAttr->SetShow(false);
	l_pcAttr->RegisterDialogWidget( new SP_WDG_StDummyMarking( wxT("Multiplicities") ) );
	//////////////////////////////////////////////////////////////////////////////////////////

	l_pcAttr = l_pcEC->AddAttribute( new SP_DS_ColListAttribute( SP_DS_CPN_INSCRIPTION, SP_COLLIST_STRING, 2 ) );
	l_pcAttr->RegisterDialogWidget( new SP_WDG_ArcInscription( wxT("Expression") ) );//Inscriptions

	l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >( l_pcAttr );
	l_pcColList->SetColLabel( 0, wxT("Expression set") );
	l_pcColList->SetColLabel( 1, wxT("Expression") );
	l_nNewRow = l_pcColList->AppendEmptyRow();
	l_pcColList->SetCell(l_nNewRow, 0, wxT("Main"));
	l_pcColList->SetCell(l_nNewRow, 1, wxT("dot"));

	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_ColListAttribute( l_pcAttr, wxT("%") ));
	l_pcGrAttr->SetTextColor( wxGetApp().GetElementPrefs()->GetExpressionColor(GetName()));
	l_pcGrAttr->SetOffsetX(25);
	l_pcGrAttr->SetOffsetY(0);
	l_pcGrAttr->SetShow(TRUE);
	l_pcAttr->SetGlobalShow();
	//////////////////////////////////////////////////////////////////////////////

	l_pcEC = p_pcGraph->GetEdgeclass( wxT("Modifier Edge") );

	l_pcAttr = l_pcEC->AddAttribute( new SP_DS_ColListAttribute( SP_DS_CPN_INSCRIPTION, SP_COLLIST_STRING, 2 ) );
	l_pcAttr->RegisterDialogWidget( new SP_WDG_ArcInscription( wxT("Expression") ) );//Inscriptions

	l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >( l_pcAttr );
	l_pcColList->SetColLabel( 0, wxT("Expression set") );
	l_pcColList->SetColLabel( 1, wxT("Expression") );
	l_nNewRow = l_pcColList->AppendEmptyRow();
	l_pcColList->SetCell(l_nNewRow, 0, wxT("Main"));
	l_pcColList->SetCell(l_nNewRow, 1, wxT("dot"));

	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_ColListAttribute( l_pcAttr, wxT("%") ));
	l_pcGrAttr->SetTextColor( wxGetApp().GetElementPrefs()->GetExpressionColor(GetName()));
	l_pcGrAttr->SetOffsetX(25);
	l_pcGrAttr->SetOffsetY(0);
	l_pcGrAttr->SetShow(TRUE);
	l_pcAttr->SetGlobalShow();

	//////////////////////////////////////////////////////////////////////////////


	SP_DS_Metadataclass* l_pcMC;

	l_pcMC = p_pcGraph->GetMetadataclass(wxT("Plot"));
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("OutputType"), wxT("Colored") ) );

	//////////////////////////////////////////////////////////////////////////////
    //basic color set
	l_pcMC  = p_pcGraph->AddMetadataclass( new SP_DS_Metadataclass( p_pcGraph, SP_DS_CPN_BASICCOLORSETCLASS ) );

	l_pcMC->SetDisplayName(wxT("Simple Color Sets"));

	l_pcMC->SetShowInElementTree( false );
	l_pcMC->SetShowInDeclarationTreeColorSet( true );

	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("Name") ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_IdAttribute( wxT("ID") ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("Comment"), wxT("") ) );
	
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_ColListAttribute( wxT("ColorsetList"), SP_COLLIST_UNSIGNED_INTEGER, 5 ) );


	/////////////////////////////////////////////////////////////////////////////


	l_pcMC = p_pcGraph->AddMetadataclass( new SP_DS_Metadataclass( p_pcGraph, SP_DS_CPN_STRUCTUREDCOLORSETCLASS ) );

	l_pcMC->SetDisplayName(wxT("Compound Color Sets"));

	l_pcMC->SetShowInElementTree( false );
	l_pcMC->SetShowInDeclarationTreeColorSet( true );

	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("Name") ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_IdAttribute( wxT("ID") ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("Comment"), wxT("") ) );
	
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_ColListAttribute( wxT("StructuredColorsetList"), SP_COLLIST_UNSIGNED_INTEGER, 6 ) );

	
	/////////////////////////////////////////////////////////////////////////////


	l_pcMC = p_pcGraph->AddMetadataclass( new SP_DS_Metadataclass( p_pcGraph, SP_DS_CPN_ALIASCOLORSETCLASS ) );

	l_pcMC->SetDisplayName(wxT("Alias Color Sets"));

	l_pcMC->SetShowInElementTree( false );
	l_pcMC->SetShowInDeclarationTreeColorSet( true );

	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("Name") ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_IdAttribute( wxT("ID") ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("Comment"), wxT("") ) );
	
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_ColListAttribute( wxT("AliasColorsetList"), SP_COLLIST_UNSIGNED_INTEGER, 3 ) );


	/////////////////////////////////////////////////////////////////////////////
	/***********george constants harmonizing************/
	l_pcMC = p_pcGraph->AddMetadataclass(new SP_DS_Metadataclass(p_pcGraph, SP_DS_CPN_CONSTANT_HARMONIZING));

	l_pcMC->SetDisplayName(wxT("Harmonized Constant"));

	l_pcMC->SetShowInElementTree(false);
	//l_pcMC->SetShowInDeclarationTreeColorSet(true);

	 

	l_pcAttr = l_pcMC->AddAttribute(new SP_DS_IdAttribute(wxT("ID")));

	l_pcAttr = l_pcMC->AddAttribute(new SP_DS_TextAttribute(wxT("Group")));
	//l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General")));
	//l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr, wxT("%:")));
	//l_pcGrAttr->SetShow(false);

	l_pcAttr = l_pcMC->AddAttribute(new SP_DS_TypeAttribute(wxT("Type"), wxT("int")));
	l_pcAttr = l_pcMC->GetPrototype()->GetAttribute(wxT("Type"));
	SP_DS_TypeAttribute* l_pcType = dynamic_cast< SP_DS_TypeAttribute* >(l_pcAttr);
	l_pcType->AddPossibleValue(wxT("double"));
	
	//l_pcAttr = l_pcMC->GetPrototype()->GetAttribute(wxT("Type"));

	//l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General")));
	//l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
	//l_pcGrAttr->SetShow(false);

	l_pcAttr = l_pcMC->AddAttribute(new SP_DS_NameAttribute(wxT("Name")));
	//l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General")));
	//l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
	//l_pcGrAttr->SetShow(false);

	l_pcAttr = l_pcMC->AddAttribute(new SP_DS_ColListAttribute(wxT("ValueList"), SP_COLLIST_STRING, 2, wxT("Group")));
	//l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogColList(wxT("Values")));
	//l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_ColListAttribute(l_pcAttr, wxT("=  [ % ];")));
	//l_pcGrAttr->SetShow(false);
	SP_DS_ColListAttribute* l_pcColList1 = dynamic_cast< SP_DS_ColListAttribute* >(l_pcAttr);
	l_pcColList1->SetColLabel(0, wxT("Value set"));
	l_pcColList1->SetColLabel(1, wxT("Value"));
	 
	unsigned int l_nNewRow1 = l_pcColList1->AppendEmptyRow();
	l_pcColList1->SetCell(l_nNewRow1, 0, wxT("Main"));
	l_pcColList1->SetCell(l_nNewRow1, 1, wxT(""));
 
	l_pcAttr = l_pcMC->AddAttribute(new SP_DS_TextAttribute(wxT("Comment")));
	//l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
	//l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr, wxT("//%")));
	//l_pcGrAttr->SetShow(false);

	/**********************/

	//l_pcMC = p_pcGraph->AddMetadataclass( new SP_DS_Metadataclass( p_pcGraph, SP_DS_CPN_CONSTANTCLASS ) );
    //l_pcMC->SetDisplayName(wxT("Constants"));

	/*
	 * remove attributes from the pn
	 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	 */
	l_pcMC = p_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);

	l_pcMC->GetPrototype()->RemoveAllGraphics(true);

	l_pcAttr = l_pcMC->GetPrototype()->GetAttribute(wxT("ValueList"));
	//if(l_pcAttr->Remove()) wxDELETE(l_pcAttr); temp commented by george

	l_pcAttr = l_pcMC->GetPrototype()->GetAttribute(wxT("Group"));
	//if(l_pcAttr->Remove()) wxDELETE(l_pcAttr);temp commented by george

	l_pcAttr = l_pcMC->GetPrototype()->GetAttribute(wxT("Type"));
//	if(l_pcAttr->Remove()) wxDELETE(l_pcAttr);temp commented by george



	/*
	 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	 */

	l_pcMC->SetShowInElementTree( false );
	l_pcMC->SetShowInDeclarationTreeOther( true );

	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("Name") ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_IdAttribute( wxT("ID") ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("Comment"), wxT("") ) );
	
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_ColListAttribute( wxT("ConstantList"), SP_COLLIST_UNSIGNED_INTEGER, 4+1 ) );//+1 added for group attribute (constants harmonizing) G.Assaf


	/////////////////////////////////////////////////////////////////////////////


	l_pcMC = p_pcGraph->AddMetadataclass( new SP_DS_Metadataclass( p_pcGraph, SP_DS_CPN_VARIABLECLASS ) );

	l_pcMC->SetDisplayName(wxT("Variables"));

	l_pcMC->SetShowInElementTree( false );
	l_pcMC->SetShowInDeclarationTreeOther( true );

	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("Name") ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_IdAttribute( wxT("ID") ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("Comment"), wxT("") ) );
	
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_ColListAttribute( wxT("VariableList"), SP_COLLIST_UNSIGNED_INTEGER, 3 ) );


	/////////////////////////////////////////////////////////////////////////////


	l_pcMC = p_pcGraph->GetMetadataclass(SP_DS_CPN_FUNCTIONCLASS);

	l_pcMC->SetDisplayName(wxT("Functions"));

	l_pcMC->SetShowInElementTree( false );
	l_pcMC->SetShowInDeclarationTreeOther( true );

	/*
	 * remove attributes from the pn
	 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	 */
	l_pcMC->GetPrototype()->RemoveAllGraphics(true);

	l_pcAttr = l_pcMC->GetPrototype()->GetAttribute(wxT("Parameter"));
	if(l_pcAttr->Remove()) wxDELETE(l_pcAttr);

	l_pcAttr = l_pcMC->GetPrototype()->GetAttribute(wxT("Body"));
	if(l_pcAttr->Remove()) wxDELETE(l_pcAttr);

	l_pcAttr = l_pcMC->GetPrototype()->GetAttribute(wxT("Return"));
	if(l_pcAttr->Remove()) wxDELETE(l_pcAttr);

	/*
	 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	 */

	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("Name") ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_IdAttribute( wxT("ID") ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("Comment"), wxT("") ) );
	
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_ColListAttribute( wxT("FunctionList"), SP_COLLIST_UNSIGNED_INTEGER, 6 ) );


	/////////////////////////////////////////////////////////////////////////////

	//for user defining anxiliary variables
	l_pcMC = p_pcGraph->AddMetadataclass( new SP_DS_Metadataclass( p_pcGraph, SP_DS_CPN_AUXILIARVARIABLESCLASS ) );
	l_pcMC->SetDisplayName(wxT("Auxiliary variables"));

	l_pcMC->SetShowInElementTree( false );
	l_pcMC->SetShowInDeclarationTreeColorSet( false );
	l_pcMC->SetShowInDeclarationTreeOther( false );

	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("Name") ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_IdAttribute( wxT("ID") ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("Comment"), wxT("") ) );
	
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_ColListAttribute( wxT("AuxiliaryVariableList"), SP_COLLIST_UNSIGNED_INTEGER, 5 ) );


	/////////////////////////////////////////////////////////////////////////////

	/*
	 * remove observers from the net class
	 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	 */
	l_pcMC = p_pcGraph->GetMetadataclass(SP_DS_META_OBSERVER);
	p_pcGraph->RemoveMetadataclass(l_pcMC);
	wxDELETE(l_pcMC);

	/////////////////////////////////////////////////////////////////////////////


	int	refresh = wxGetApp().GetAnimationPrefs()->GetRefresh(GetName());
	int duration = wxGetApp().GetAnimationPrefs()->GetDuration(GetName());

	SP_ANIM_STEP_T stepping = SP_ANIM_STEP_SINGLE;

	p_pcGraph->SetAnimation( new SP_DS_ColStAnimation(refresh, duration, stepping));

	return p_pcGraph;

}

bool
SP_DS_ColSPN::CreateInitialElements(SP_DS_Graph* p_pcGraph)
{
	SP_DS_EventSPN::CreateInitialElements(p_pcGraph);

	SP_DS_Metadataclass* l_pcMC = (SP_DS_Metadataclass*) 0;

	l_pcMC = p_pcGraph->GetMetadataclass(SP_DS_CPN_BASICCOLORSETCLASS);
	SP_DS_Metadata* l_pcColorset = l_pcMC->NewElement( 1 );
	if(l_pcColorset)
	{
		//l_pcColorset->GetAttribute( wxT("Name") )->SetValueString( wxT("NewColorset") );
		SP_DS_ColListAttribute* l_pcColAttr =
				dynamic_cast<SP_DS_ColListAttribute*>(l_pcColorset->GetAttribute(wxT("ColorsetList")));
		l_pcColAttr->AppendEmptyRow();
		l_pcColAttr->SetCell(0,0,wxT("Dot"));
		l_pcColAttr->SetCell(0,1,wxT("dot"));
		l_pcColAttr->SetCell(0,2,wxT("dot"));
	}

	l_pcMC = p_pcGraph->GetMetadataclass(SP_DS_CPN_STRUCTUREDCOLORSETCLASS);
	SP_DS_Metadata* l_pcStructuredColorset = l_pcMC->NewElement( 1 );
	if(l_pcStructuredColorset)
	{
		//l_pcStructuredColorset->GetAttribute( wxT("Name") )->SetValueString( wxT("NewColorset") );
	}

	l_pcMC = p_pcGraph->GetMetadataclass(SP_DS_CPN_ALIASCOLORSETCLASS);
	SP_DS_Metadata* l_pcAliasColorset = l_pcMC->NewElement( 1 );
	if(l_pcAliasColorset)
	{
		//l_pcAliasColorset->GetAttribute( wxT("Name") )->SetValueString( wxT("NewColorset") );
	}

	l_pcMC = p_pcGraph->GetMetadataclass(SP_DS_CPN_CONSTANTCLASS);
	SP_DS_Metadata* l_pcConstant = l_pcMC->NewElement( 1 );
	if(l_pcConstant)
	{
		//l_pcConstant->GetAttribute( wxT("Name") )->SetValueString( wxT("NewConstant") );
	}

	l_pcMC = p_pcGraph->GetMetadataclass(SP_DS_CPN_VARIABLECLASS);
	SP_DS_Metadata* l_pcVariable = l_pcMC->NewElement( 1 );
	if(l_pcVariable)
	{
		//l_pcVariable->GetAttribute( wxT("Name") )->SetValueString( wxT("NewVariable") );
	}

	l_pcMC = p_pcGraph->GetMetadataclass(SP_DS_CPN_FUNCTIONCLASS);
	SP_DS_Metadata* l_pcFunction = l_pcMC->NewElement( 1 );
	if(l_pcFunction)
	{
		//l_pcFunction->GetAttribute( wxT("Name") )->SetValueString( wxT("NewFunction") );
	}

	l_pcMC = p_pcGraph->GetMetadataclass(SP_DS_CPN_AUXILIARVARIABLESCLASS);
	SP_DS_Metadata* l_pcAuxiVar = l_pcMC->NewElement( 1 );
	if(l_pcAuxiVar)
	{
		//l_pcAuxiVar->GetAttribute( wxT("Name") )->SetValueString( wxT("NewAuxiliaryVariable") );
	}

	return true;
}

bool
SP_DS_ColSPN::NodeRequirement( SP_DS_Node* p_pcNode )
{
    return SP_DS_EventSPN::NodeRequirement( p_pcNode );
}

bool
SP_DS_ColSPN::EdgeRequirement( SP_DS_Edgeclass* p_pcClass, SP_Data* p_pcSrc, SP_Data* p_pcTrg )
{

	SP_LOGDEBUG(wxString::Format(wxT("EdgeRequirement %p->%p %s"), p_pcSrc, p_pcTrg, p_pcClass->GetName().c_str()));
	if (!SP_DS_ExtPT::EdgeRequirement(p_pcClass, p_pcSrc, p_pcTrg))
		return false;

    if (p_pcSrc->GetClassName()== wxT("Parameter") ||
    	p_pcSrc->GetClassName()== wxT("Coarse Parameter"))
        return FALSE;
    if (p_pcTrg->GetClassName()== wxT("Parameter") ||
    	p_pcTrg->GetClassName()== wxT("Coarse Parameter"))
        return FALSE;

	// Added by Liu on 22 Mar. 2009
	if(((p_pcSrc->GetClassName() == wxT("Transition")) ||
		(p_pcSrc->GetClassName() == wxT("Immediate Transition")) ||
		(p_pcSrc->GetClassName() == wxT("Deterministic Transition")) ||
		(p_pcSrc->GetClassName() == wxT("Scheduled Transition"))) &&
		((p_pcTrg->GetClassName() == wxT("Transition")) ||
		(p_pcTrg->GetClassName() == wxT("Immediate Transition")) ||
		(p_pcTrg->GetClassName() == wxT("Deterministic Transition")) ||
		(p_pcTrg->GetClassName() == wxT("Scheduled Transition"))))
		return FALSE;

	if (p_pcClass->GetName() == wxT("Modifier Edge"))
	{
		if (!p_pcSrc->GetClassName().Contains(wxT("Place")) &&
			!p_pcTrg->GetClassName().Contains(wxT("Transition")))
		{
			SP_MESSAGEBOX(wxString(p_pcClass->GetName()) + wxT(" must connect places and transitions"), wxT("Error"), wxOK | wxICON_ERROR);

			return false;
		}
	}
/*
	const SP_ListEdge* l_lpEdges = ( dynamic_cast< SP_DS_Node* >( p_pcSrc ) )->GetSourceEdges();
	SP_ListEdge::const_iterator l_pIt;
	for ( l_pIt = l_lpEdges->begin(); l_pIt != l_lpEdges->end(); ++l_pIt )
	{
		if( (*l_pIt)->GetTarget() == p_pcTrg
			&& (p_pcClass->GetName() == wxT("Modifier Edge")
				|| (*l_pIt)->GetClassName() == wxT("Modifier Edge")))
		{
			wxString l_sMsg = wxT("The nodes ");
			l_sMsg << dynamic_cast<SP_DS_NameAttribute*>(p_pcSrc->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
			l_sMsg << wxT(" and ");
			l_sMsg << dynamic_cast<SP_DS_NameAttribute*>(p_pcTrg->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
			l_sMsg << wxT(" are already connected via a ");
			l_sMsg << (*l_pIt)->GetClassName();
			l_sMsg << wxT(".");
			SP_MESSAGEBOX( l_sMsg, wxT("Error"),
						  wxOK | wxICON_ERROR );
			return false;
		}
	}
*/

	return true;}

bool
SP_DS_ColSPN::MetadataRequirement( SP_DS_Metadata* p_pcMetadata )
{
    if(!SP_DS_EventSPN::MetadataRequirement( p_pcMetadata ))
    {
    	return false;
    }
	wxString l_sName1 = p_pcMetadata->GetClassName();//george for constnt harmonizing
	if (l_sName1 == wxT("Constant Class"))//george for constnt harmonizing
	{
		return true;
    }

    wxString l_sName = p_pcMetadata->GetClassName();
	if ((l_sName == SP_DS_CPN_BASICCOLORSETCLASS
			|| l_sName == SP_DS_CPN_STRUCTUREDCOLORSETCLASS
			|| l_sName == SP_DS_CPN_ALIASCOLORSETCLASS
			|| l_sName == SP_DS_CPN_CONSTANTCLASS
			|| l_sName == SP_DS_CPN_VARIABLECLASS
			|| l_sName == SP_DS_CPN_FUNCTIONCLASS
			|| l_sName == SP_DS_CPN_AUXILIARVARIABLESCLASS )
		&& !p_pcMetadata->GetMetadataclass()->GetElements()->empty())
	{
		return false;
	}

	return true;
}
