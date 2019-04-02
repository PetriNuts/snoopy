//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: slehrack $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2007/06/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#include "sp_ds/netclasses/SP_DS_EventSPN.h"

#include "sp_ds/SP_DS_Edgeclass.h"
#include "sp_ds/SP_DS_Nodeclass.h"
#include "sp_ds/SP_DS_Metadataclass.h"
#include "sp_ds/animators/ani_SPN/SP_DS_StPlaceAnimator.h"
#include "sp_ds/animators/ani_SPN/SP_DS_StTransAnimator.h"

//by sl
#include "sp_ds/animators/SP_DS_ExtPTPlaceAnimator.h"
#include "sp_ds/animators/SP_DS_ExtPTTransAnimator.h"
#include "sp_ds/attributes/SP_DS_TypeAttribute.h"
#include "sp_ds/attributes/SP_DS_MarkingAttribute.h"
#include "sp_ds/attributes/SP_DS_MarkingDependentMultiplicity.h"


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
#include "sp_ds/extensions/ext_SPN/SP_DS_StAnimation.h"

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

#include "sp_gui/widgets/dialogs/SP_WDG_DialogUnsigned.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogBool.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogShowOnly.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogGraphic.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogMultiline.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogExtendedGraphic.h"

#include "sp_gui/widgets/dialogs/wid_dia_SPN/SP_WDG_StFunctionList.h"
#include "sp_gui/widgets/dialogs/wid_dia_SPN/SP_WDG_StLookupTable.h"
#include "sp_gui/widgets/dialogs/wid_dia_SPN/SP_WDG_StLookupTableName.h"

#include "sp_gui/widgets/dialogs/SP_WDG_DialogColList.h" //Added By Liu on 6 Mar. 2009
#include "sp_gr/attributes/SP_GR_ColListAttribute.h" //Added By Liu on 6 Mar. 2009


#include "sp_utilities.h"

#include "snoopy.h"
#include "sp_core/SP_GPR_Elements.h"
#include "sp_core/SP_GPR_Animation.h"

#include "sp_gui/windows/SP_GUI_Mainframe.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_core/SP_Core.h"

SP_DS_EventSPN::SP_DS_EventSPN()
:SP_DS_ExtPT( SP_DS_SPN_CLASS )
{
	m_sExtension = wxT("spn");
	m_sFilter = wxT("*.spn;*.spstochpn");

	//register node classes for editing their graphic properties

	wxGetApp().GetElementPrefs()->RegisterNodeclass( GetName(), wxT("Place") );
	wxGetApp().GetElementPrefs()->RegisterNodeclass( GetName(), wxT("Transition") );
	wxGetApp().GetElementPrefs()->RegisterNodeclass( GetName(), wxT("Immediate Transition") );
	wxGetApp().GetElementPrefs()->RegisterNodeclass( GetName(), wxT("Deterministic Transition") );
	wxGetApp().GetElementPrefs()->RegisterNodeclass( GetName(), wxT("Scheduled Transition") );
	wxGetApp().GetElementPrefs()->RegisterNodeclass( GetName(), wxT("Parameter") );
	wxGetApp().GetElementPrefs()->RegisterNodeclass( GetName(), wxT("LookupTable") );

}


SP_DS_EventSPN::SP_DS_EventSPN( const wxString& p_pchName )
:SP_DS_ExtPT( p_pchName )
{
	m_sExtension = wxT("spn");
	m_sFilter = wxT("*.spn;*.spstochpn");

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
SP_DS_EventSPN::CreateGraph( SP_DS_Graph* p_pcGraph )
{

	if ( ! SP_DS_ExtPT::CreateGraph( p_pcGraph ) )
	{
		return NULL;
	}

	SP_DS_Attribute* l_pcAttr;

	SP_GR_Node* l_pcGr;
	SP_Graphic* l_pcGrAttr;

	SP_DS_Nodeclass* l_pcNC;

	SP_DS_ColListAttribute* l_pcColList;
	unsigned int l_nNewRow;

	//////////////////////////////////////////////////////////////////////////////
	l_pcNC = p_pcGraph->GetNodeclass( wxT("Place") );

	// animator
	l_pcNC->AddAnimator( new SP_DS_StPlaceAnimator( wxT("Marking") ) );

	//////////////////////////////////////////////////////////////////////////////
	l_pcNC = p_pcGraph->GetNodeclass( wxT("Transition") );

	// special EventSPN components

	l_pcAttr = l_pcNC->AddAttribute( new SP_DS_ColListAttribute( wxT("FunctionList"), SP_COLLIST_STRING, 2 ) );
	l_pcAttr->SetDisplayName(wxT("Function set"));
	l_pcAttr->RegisterDialogWidget( new SP_WDG_StFunctionList( wxT("Function") ) );
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_ColListAttribute( l_pcAttr, wxT("%") ));
	l_pcGrAttr->SetOffsetX(45);
	l_pcGrAttr->SetOffsetY(-25);
	l_pcGrAttr->SetShow( FALSE );
	l_pcAttr->SetGlobalShow();

	l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >( l_pcAttr );
	l_pcColList->SetColLabel( 0, wxT("Function set") );
	l_pcColList->SetColLabel( 1, wxT("Function") );
	l_nNewRow = l_pcColList->AppendEmptyRow();
	l_pcColList->SetCell(l_nNewRow, 0, wxT("Main"));
	l_pcColList->SetCell(l_nNewRow, 1, wxT("MassAction(1)"));

	//////////////////////////////////////////////////////////////////////////////
	//share IdCounter with "Transition"
	l_pcNC = p_pcGraph->AddNodeclass( new SP_DS_Nodeclass( p_pcGraph,
															wxT("Immediate Transition"),
															p_pcGraph->GetNodeclass(wxT("Transition"))->GetIdCountPtr() ) );
	l_pcNC->SetAbbreviation(wxT("IT"));
	l_pcNC->SetShortcut( wxT("Shift+I") );
	l_pcAttr = l_pcNC->AddAttribute(new SP_DS_NameAttribute(wxT("Name"), wxT("")));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General"), 1));
	l_pcAttr->SetGlobalShow();
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
	l_pcGrAttr->SetOffsetX(25);
	l_pcGrAttr->SetOffsetY(20);

	l_pcAttr = l_pcNC->AddAttribute(new SP_DS_IdAttribute(wxT("ID")));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogShowOnly(wxT("General")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_NumberAttribute(l_pcAttr, wxT("_it%_")));
	l_pcGrAttr->SetOffsetX(25);
	l_pcGrAttr->SetOffsetY(20);
	l_pcGrAttr->SetShow(FALSE);
	l_pcAttr->SetGlobalShow();



	l_pcAttr = l_pcNC->AddAttribute(new SP_DS_LogicAttribute(wxT("Logic"), wxT("Name")));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogBool(wxT("General")));

	l_pcAttr = l_pcNC->AddAttribute(new SP_DS_TextAttribute(wxT("Comment"), wxT("")));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
	l_pcAttr->SetGlobalShow();
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
	l_pcGrAttr->SetOffsetX(25);
	l_pcGrAttr->SetOffsetX(40);

	l_pcAttr = l_pcNC->AddAttribute( new SP_DS_ColListAttribute( wxT("FunctionList"), SP_COLLIST_STRING, 2 ) );
	l_pcAttr->SetDisplayName(wxT("Weight set"));
	l_pcAttr->RegisterDialogWidget( new SP_WDG_StFunctionList( wxT("Weight") ) );
	l_pcGrAttr = l_pcAttr->AddGraphic( new SP_GR_ColListAttribute( l_pcAttr ) );
	l_pcGrAttr->SetOffsetX(25);
	l_pcGrAttr->SetOffsetY(30);
	l_pcGrAttr->SetShow(FALSE);
	l_pcAttr->SetGlobalShow();

	l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >( l_pcAttr );
	l_pcColList->SetColLabel( 0, wxT("Weight set") );
	l_pcColList->SetColLabel( 1, wxT("Weight") );
	l_nNewRow = l_pcColList->AppendEmptyRow();
	l_pcColList->SetCell(l_nNewRow, 0, wxT("Main"));
	l_pcColList->SetCell(l_nNewRow, 1, wxT("1"));


	int twidth = wxGetApp().GetElementPrefs()->GetNodeWidth(GetName(), wxT("Immediate Transition"));
	int theight = wxGetApp().GetElementPrefs()->GetNodeHeight(GetName(), wxT("Immediate Transition"));

	l_pcGr = new SP_GR_Rectangle(l_pcNC->GetPrototype(), twidth, theight);
	l_pcGr->SetHeight(20.0);
	l_pcGr->SetWidth(10.0);
	l_pcGr->SetDefaultBrush(wxTheBrushList->FindOrCreateBrush(wxColour(0, 0, 0)));
    l_pcGr->SetFixedSize(wxGetApp().GetElementPrefs()->GetNodeFixed(GetName(), l_pcNC->GetName()));
	l_pcNC->SetGraphic(l_pcGr);
	l_pcNC->RegisterGraphicWidget(new SP_WDG_DialogGraphic(wxT("Graphic")));

	// animator
	l_pcNC->AddAnimator(new SP_DS_ExtPTTransAnimator());


	//////////////////////////////////////////////////////////////////////////////
	//share IdCounter with "Transition"
	l_pcNC = p_pcGraph->AddNodeclass( new SP_DS_Nodeclass( p_pcGraph,
															wxT("Deterministic Transition"),
															p_pcGraph->GetNodeclass(wxT("Transition"))->GetIdCountPtr() ) );
	l_pcNC->SetAbbreviation(wxT("DT"));
	l_pcNC->SetShortcut( wxT("Shift+D") );
	l_pcAttr = l_pcNC->AddAttribute(new SP_DS_NameAttribute(wxT("Name"), wxT("")));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General"), 1));
	l_pcAttr->SetGlobalShow();
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
	l_pcGrAttr->SetOffsetX(25);
	l_pcGrAttr->SetOffsetY(20);

	l_pcAttr = l_pcNC->AddAttribute(new SP_DS_IdAttribute(wxT("ID")));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogShowOnly(wxT("General")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_NumberAttribute(l_pcAttr, wxT("_dt%_")));
	l_pcGrAttr->SetOffsetX(25);
	l_pcGrAttr->SetOffsetY(20);
	l_pcGrAttr->SetShow(FALSE);
	l_pcAttr->SetGlobalShow();




	l_pcAttr = l_pcNC->AddAttribute(new SP_DS_LogicAttribute(wxT("Logic"), wxT("Name")));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogBool(wxT("General")));

	l_pcAttr = l_pcNC->AddAttribute(new SP_DS_TextAttribute(wxT("Comment"), wxT("")));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
	l_pcAttr->SetGlobalShow();
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
	l_pcGrAttr->SetOffsetX(40);

	//Delay Attribute
	l_pcAttr = l_pcNC->AddAttribute(new SP_DS_ColListAttribute( wxT("DelayList"), SP_COLLIST_DOUBLE, 2 ));
	l_pcAttr->SetDisplayName(wxT("Delay set"));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogColList( wxT("Delay")));
	l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcAttr );
	l_pcColList->SetColLabel( 0, wxT("Delay set"));
	l_pcColList->SetColLabel( 1, wxT("Delay"));
	l_nNewRow = l_pcColList->AppendEmptyRow();
	l_pcColList->SetCell(l_nNewRow, 0, wxT("Main"));
	l_pcColList->SetCell(l_nNewRow, 1, wxT("1"));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_ColListAttribute( l_pcAttr, wxT("<%>") ));
	l_pcGrAttr->SetOffsetX(0);
	l_pcGrAttr->SetOffsetY(40);
	l_pcGrAttr->SetShow(TRUE);
	l_pcAttr->SetGlobalShow();

	//////////////////////////////////////////////////////////////////////////////////////

	twidth = wxGetApp().GetElementPrefs()->GetNodeWidth(GetName(), wxT("Deterministic Transition"));
	theight = wxGetApp().GetElementPrefs()->GetNodeHeight(GetName(), wxT("Deterministic Transition"));

	l_pcGr = new SP_GR_Rectangle(l_pcNC->GetPrototype(), twidth, theight);
	l_pcGr->SetDefaultBrush(wxTheBrushList->FindOrCreateBrush(wxColour(0, 0, 0)));
    l_pcGr->SetFixedSize(wxGetApp().GetElementPrefs()->GetNodeFixed(GetName(), l_pcNC->GetName()));
	l_pcNC->SetGraphic(l_pcGr);
	l_pcNC->RegisterGraphicWidget(new SP_WDG_DialogGraphic(wxT("Graphic")));

	// animator
	l_pcNC->AddAnimator(new SP_DS_ExtPTTransAnimator());


	//////////////////////////////////////////////////////////////////////////////
	//share IdCounter with "Transition"
	l_pcNC = p_pcGraph->AddNodeclass( new SP_DS_Nodeclass( p_pcGraph,
															wxT("Scheduled Transition"),
															p_pcGraph->GetNodeclass(wxT("Transition"))->GetIdCountPtr() ) );
	l_pcNC->SetAbbreviation(wxT("ST"));
	l_pcNC->SetShortcut( wxT("Shift+S") );
	l_pcAttr = l_pcNC->AddAttribute(new SP_DS_NameAttribute(wxT("Name"), wxT("")));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General"), 1));
	l_pcAttr->SetGlobalShow();
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
	l_pcGrAttr->SetOffsetX(25);
	l_pcGrAttr->SetOffsetY(20);

	l_pcAttr = l_pcNC->AddAttribute(new SP_DS_IdAttribute(wxT("ID")));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogShowOnly(wxT("General")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_NumberAttribute(l_pcAttr, wxT("_st%_")));
	l_pcGrAttr->SetOffsetX(25);
	l_pcGrAttr->SetOffsetY(20);
	l_pcGrAttr->SetShow(FALSE);
	l_pcAttr->SetGlobalShow();


	l_pcAttr = l_pcNC->AddAttribute(new SP_DS_LogicAttribute(wxT("Logic"), wxT("Name")));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogBool(wxT("General")));

	l_pcAttr = l_pcNC->AddAttribute(new SP_DS_TextAttribute(wxT("Comment"), wxT("")));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
	l_pcAttr->SetGlobalShow();
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
	l_pcGrAttr->SetOffsetX(40);


	//Periodic Attribute
	l_pcAttr = l_pcNC->AddAttribute(new SP_DS_ColListAttribute( wxT("PeriodicList"), SP_COLLIST_STRING, 4 ));
	l_pcAttr->SetDisplayName(wxT("Schedule set"));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogColList( wxT("Periodic") ));
	l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcAttr );
	l_pcColList->SetColLabel( 0, wxT("Periodic List"));
	l_pcColList->SetColLabel( 1, wxT("Begin"));
	l_pcColList->SetColLabel( 2, wxT("Repetition"));
	l_pcColList->SetColLabel( 3, wxT("End"));
	l_nNewRow = l_pcColList->AppendEmptyRow();
	l_pcColList->SetCell(l_nNewRow, 0, wxT("Main"));
	l_pcColList->SetCell(l_nNewRow, 1, wxT("_SimStart"));
	l_pcColList->SetCell(l_nNewRow, 2, wxT("1"));
	l_pcColList->SetCell(l_nNewRow, 3, wxT("_SimEnd"));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_ColListAttribute( l_pcAttr, wxT("[%]") ));
	l_pcGrAttr->SetOffsetX(0);
	l_pcGrAttr->SetOffsetY(40);
	l_pcGrAttr->SetShow(TRUE);
	l_pcAttr->SetGlobalShow();


	twidth = wxGetApp().GetElementPrefs()->GetNodeWidth(GetName(), wxT("Scheduled Transition"));
	theight = wxGetApp().GetElementPrefs()->GetNodeHeight(GetName(), wxT("Scheduled Transition"));

	l_pcGr = new SP_GR_Rectangle(l_pcNC->GetPrototype(), twidth, theight);
	l_pcGr->SetDefaultBrush(wxTheBrushList->FindOrCreateBrush(wxColour(125, 125, 125)));
    l_pcGr->SetFixedSize(wxGetApp().GetElementPrefs()->GetNodeFixed(GetName(), l_pcNC->GetName()));
	l_pcNC->SetGraphic(l_pcGr);
	l_pcNC->RegisterGraphicWidget(new SP_WDG_DialogGraphic(wxT("Graphic")));

	// animator
	l_pcNC->AddAnimator(new SP_DS_ExtPTTransAnimator());


	//////////////////////////////////////////////////////////////////////////////
	l_pcNC = p_pcGraph->AddNodeclass( new SP_DS_Nodeclass( p_pcGraph, wxT("LookupTable") ) );
	l_pcNC->SetAbbreviation(wxT("L"));
	l_pcNC->SetShortcut( wxT("L") );

	l_pcAttr = l_pcNC->AddAttribute(new SP_DS_NameAttribute( wxT("Name") ) );
	l_pcAttr->RegisterDialogWidget(new SP_WDG_StLookupTableName( wxT("General"), 1 ) );
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

	l_pcAttr = l_pcNC->AddAttribute( new SP_DS_DoubleAttribute( wxT("DefaultValue"), 0 ) );

	l_pcAttr = l_pcNC->AddAttribute( new SP_DS_ColListAttribute( wxT("MapList"), SP_COLLIST_DOUBLE, 2 ) );
	l_pcAttr->RegisterDialogWidget( new SP_WDG_StLookupTable( wxT("General") ) );

	l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >( l_pcAttr );

	l_pcColList->SetColLabel( 0, wxT("x") );
	l_pcColList->SetColLabel( 1, wxT("F( x )") );

	int paheight = wxGetApp().GetElementPrefs()->GetNodeHeight( GetName(), wxT("LookupTable") );
	int pawidth = wxGetApp().GetElementPrefs()->GetNodeWidth(GetName(), wxT("LookupTable"));
	if(pawidth == DEFAULT_WIDTH) { pawidth = 2*pawidth; }
   	l_pcGr = new SP_GR_ExtendedEllipse(l_pcNC->GetPrototype(),pawidth, paheight,TRUE,SP_EXTENDED_TYPE_DEFAULT,3,wxColour(128, 128, 128));
   	l_pcGr->SetFixedSize(wxGetApp().GetElementPrefs()->GetNodeFixed(GetName(), l_pcNC->GetName()));
	l_pcNC->SetGraphic( l_pcGr );
	l_pcNC->RegisterGraphicWidget( new SP_WDG_DialogGraphic( wxT("Graphic") ) );



	//////////////////////////////////////////////////////////////////////////////
	SP_DS_Edgeclass* l_pcEC = p_pcGraph->AddEdgeclass( new SP_DS_Edgeclass( p_pcGraph, wxT("Modifier Edge") ) );
	l_pcEC->SetDisplayName(wxT("Modifier Arc"));
	l_pcEC->SetAbbreviation(wxT("MA"));
	l_pcEC->SetShortcut( wxT("M") );
	l_pcEC->SetGraphic(new SP_GR_ExtendedEdge(l_pcEC->GetPrototype(),SP_EXTENDED_TYPE_DASHED_EDGE,1,
								SP_EXTENDED_ARROW_TYPE_NONE_EDGE,10,SP_EXTENDED_ARROW_TYPE_ARROW_EDGE,10) );

	l_pcEC->RegisterGraphicWidget( new SP_WDG_DialogGraphic( wxT("Graphic") ) );

	l_pcAttr = l_pcEC->AddAttribute( new SP_DS_TextAttribute( wxT("Comment"), wxT("") ) );
	l_pcAttr->RegisterDialogWidget( new SP_WDG_DialogMultiline( wxT("General") ) );
	l_pcAttr->SetGlobalShow();

	l_pcGrAttr = l_pcAttr->AddGraphic( new SP_GR_TextAttribute( l_pcAttr ) );
	l_pcGrAttr->SetOffsetX( 40 );

	//////////////////////////////////////////////////////////////////////////////
	SP_DS_Metadataclass* l_pcMC;
	l_pcMC = p_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);

	l_pcAttr = l_pcMC->GetPrototype()->GetAttribute(wxT("Type"));
	SP_DS_TypeAttribute* l_pcType = dynamic_cast< SP_DS_TypeAttribute* >( l_pcAttr );
	l_pcType->AddPossibleValue(wxT("double"));
	//l_pcType->AddPossibleValue(wxT("TFN1"));
	//l_pcType->AddPossibleValue(wxT("BFN1"));


 	//////////////////////////////////////////////////////////////////////////////
	l_pcMC = p_pcGraph->GetMetadataclass(SP_DS_META_FUNCTION);
	l_pcAttr = l_pcMC->GetPrototype()->GetAttribute(wxT("Return"));
	l_pcType = dynamic_cast< SP_DS_TypeAttribute* >( l_pcAttr );
	l_pcType->AddPossibleValue(wxT("double"));


	//////////////////////////////////////////////////////////////////////////////
	l_pcMC = p_pcGraph->AddMetadataclass( new SP_DS_Metadataclass( p_pcGraph, wxT("Plot") ) );

	l_pcMC->SetShowInElementTree( false );

	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("Name") ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_IdAttribute( wxT("ID") ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("Comment"), wxT("") ) );

	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("Nodeclass"), wxT("Place") ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("ViewerType"), wxT("xyPlot") ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("Results"), wxT("Marking") ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_BoolAttribute( wxT("NodeColour"), false ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("RegEx"), wxT("") ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_BoolAttribute( wxT("RegExInvert"), false ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_ColListAttribute( wxT("PlaceList"), SP_COLLIST_UNSIGNED_INTEGER, 1 ) );
	
    /*
     * Curve information:
	* 0: Curve ID
	* 1: place type (colored, uncolored...)
	* 2: State: (checked, unchecked)
	* 3: Curve color
	* 4: Line width
	* 5: line style
	* 6: name
	*/
	
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_ColListAttribute( wxT("CurveInfo"), SP_COLLIST_STRING,7) );

	//Is current view
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("IsCurrent"), wxT("0") ) );

	//xy plot
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("ViewTitle"), wxT("Untitled") ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("ChartWidth"), wxT("600") ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("ChartHeight"), wxT("600") ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("WindowWidth"), wxT("600") ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("WindowHeight"), wxT("600") ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("XAxisTitle"), wxT("Time") ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("YAxisTitle"), wxT("") ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_NumberAttribute( wxT("DefaultLineWidth"), 2 ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_NumberAttribute( wxT("DefaultLineStyle"), wxSOLID) );

	//legend properties
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_BoolAttribute( wxT("ShowLegend"), true ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_NumberAttribute( wxT("LegendHorizontalPosition"), wxRIGHT ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_NumberAttribute( wxT("LegendVerticalPosition"), wxCENTER ) );

	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_BoolAttribute( wxT("ShowLines"), true ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_BoolAttribute( wxT("ShowSymbols"), false ) );

	//for hstogram plot
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_NumberAttribute( wxT("BarWidth"), 20 ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_DoubleAttribute( wxT("IntervalWidth"), 0 ) );

	//X axis variable type
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("XAxisVariable"), wxT("Time") ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("XAxisVariableName"), wxT("Simulation Time") ) );

	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_BoolAttribute( wxT("SumOfNodes"), false ) );

	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_BoolAttribute( wxT("FixedXAdjustment"), false ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_BoolAttribute( wxT("FixedYAdjustment"), false ) );

	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_DoubleAttribute( wxT("X_Axis_Max"), 1.0 ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_DoubleAttribute( wxT("X_Axis_Min"), 0.0 ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_DoubleAttribute( wxT("Y_Axis_Max"), 1.0 ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_DoubleAttribute( wxT("Y_Axis_Min"), 0.0 ) );

	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_DoubleAttribute( wxT("X_ZOOM"), 1.0 ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_DoubleAttribute( wxT("Y_ZOOM"), 1.0 ) );
	
	//sorting
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_NumberAttribute( wxT("SortCurvesBy"), 0 ) );

	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_NumberAttribute( wxT("HistogramFrequencyType"), 0) );

	//////////////////////////////////////////////////////////////////////////////
	l_pcMC = p_pcGraph->AddMetadataclass( new SP_DS_Metadataclass( p_pcGraph, wxT("Simulation Properties") ) );

	l_pcMC->SetShowInElementTree( false );

	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_DoubleAttribute( wxT("interval start"), 0.0 ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_DoubleAttribute( wxT("interval end"), 100.0 ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_NumberAttribute( wxT("output step"), 100 ) );

	//simulator properties
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_NumberAttribute( wxT("simulator"), 0 ));
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_ColListAttribute( wxT("simulator properties"), SP_COLLIST_STRING, 2 ) );


	//////////////////////////////////////////////////////////////////////////////
	l_pcMC = p_pcGraph->AddMetadataclass( new SP_DS_Metadataclass( p_pcGraph, wxT("Simulation Results") ) );

	l_pcMC->SetShowInElementTree( false );

	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("marking") ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("marking square") ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("firing rate") ) );


	//////////////////////////////////////////////////////////////////////////////


	int	refresh = wxGetApp().GetAnimationPrefs()->GetRefresh(GetName());
	int duration = wxGetApp().GetAnimationPrefs()->GetDuration(GetName());

	SP_ANIM_STEP_T stepping = SP_ANIM_STEP_SINGLE;

	p_pcGraph->SetAnimation( new SP_DS_StAnimation(refresh, duration, stepping));

	return p_pcGraph;

}

bool
SP_DS_EventSPN::CreateInitialElements(SP_DS_Graph* p_pcGraph)
{
	SP_DS_ExtPT::CreateInitialElements(p_pcGraph);

	SP_DS_Metadataclass* l_pcMC;

	l_pcMC = p_pcGraph->GetMetadataclass(wxT("Simulation Properties"));
	l_pcMC->NewElement( 1 );

	l_pcMC = p_pcGraph->GetMetadataclass(wxT("Simulation Results"));
	l_pcMC->NewElement( 1 );

	return true;
}

bool
SP_DS_EventSPN::NodeRequirement( SP_DS_Node* p_pcNode )
{
    return SP_DS_ExtPT::NodeRequirement( p_pcNode );
}

bool
SP_DS_EventSPN::EdgeRequirement( SP_DS_Edgeclass* p_pcClass, SP_Data* p_pcSrc, SP_Data* p_pcTrg )
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


	return true;
}

bool
SP_DS_EventSPN::MetadataRequirement( SP_DS_Metadata* p_pcMetadata )
{
    if(!SP_DS_ExtPT::MetadataRequirement( p_pcMetadata ))
    {
    	return false;
    }

    wxString l_sName = p_pcMetadata->GetClassName();
	if ((l_sName == wxT("Simulation Properties") || l_sName == wxT("Simulation Results") )
		&& !p_pcMetadata->GetMetadataclass()->GetElements()->empty())
	{
		return false;
	}

	return true;
}


