//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: dscheibl $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2005/04/05 13:28:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#include "sp_ds/netclasses/SP_DS_ContinuousPed.h"
#include "sp_ds/SP_DS_Edgeclass.h"
#include "sp_ds/SP_DS_Nodeclass.h"
#include "sp_ds/SP_DS_Metadataclass.h"

#include "sp_ds/attributes/SP_DS_BoolAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/attributes/SP_DS_NumberAttribute.h"
#include "sp_ds/attributes/SP_DS_DoubleAttribute.h"
#include "sp_ds/attributes/SP_DS_LogicAttribute.h"
#include "sp_ds/attributes/SP_DS_IdAttribute.h"
#include "sp_ds/attributes/SP_DS_MultiplicityAttribute.h"

// by sl
#include "sp_ds/attributes/SP_DS_DoubleMarkingDependentMultiplicity.h"
#include "sp_ds/attributes/SP_DS_DoubleMarkingAttribute.h"


#include "sp_ds/attributes/SP_DS_EquationAttribute.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"
#include "sp_ds/attributes/SP_DS_TypeAttribute.h"

#include "sp_gr/attributes/gra_SPN/SP_GR_StParameterListAttribute.h"
//for ColList
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"

#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/extensions/SP_DS_Coarse.h"
#include "sp_ds/extensions/SP_DS_ContinuousPedAnimation.h"

// extended graphical representation
#include "sp_gr/shapes/SP_GR_ExtendedCircle.h"
#include "sp_gr/shapes/SP_GR_ExtendedRectangle.h"
#include "sp_gr/shapes/SP_GR_ExtendedEllipse.h"
#include "sp_gr/metadata/SP_GR_PlotSymbol.h"
#include "sp_gr/shapes/SP_GR_Rectangle.h"

#include "sp_gr/edges/SP_GR_ArrowEdge.h"
#include "sp_gr/edges/SP_GR_ExtendedEdge.h"

#include "sp_gr/attributes/SP_GR_TextAttribute.h"
#include "sp_gr/attributes/SP_GR_NumberAttribute.h"
#include "sp_gr/attributes/SP_GR_DoubleAttribute.h"
#include "sp_gr/attributes/SP_GR_MultiplicityAttribute.h"
#include "sp_gr/attributes/SP_GR_DoubleMultiplicityAttribute.h"
#include "sp_gr/attributes/SP_GR_MarkAttribute.h"
#include "sp_gr/attributes/SP_GR_DoubleMarkAttribute.h"
#include "sp_gr/attributes/SP_GR_CommentAttribute.h"
#include "sp_gr/attributes/SP_GR_HiddenTextAttribute.h"
#include "sp_gr/attributes/SP_GR_ColListAttribute.h"
#include "sp_gr/attributes/SP_GR_MarkNumberAttribute.h"

#include "sp_gr/attributes/gra_SPN/SP_GR_StFunctionAttribute.h"

#include "sp_gui/widgets/dialogs/SP_WDG_DialogText.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogBool.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogShowOnly.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogGraphic.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogExtendedGraphic.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogMultiline.h"
#include "sp_gui/widgets/dialogs/wid_dia_SPN/SP_WDG_StFunctionList.h"

#include "sp_defines.h"
#include "snoopy.h"
#include "sp_core/SP_GPR_Elements.h"
#include "sp_core/SP_GPR_Animation.h"

SP_DS_ContinuousPed::SP_DS_ContinuousPed() :
	SP_DS_BipartGraph(SP_DS_CONTINUOUSPED_CLASS)
{
	m_sExtension = wxT("cpn");
	m_sFilter = wxT("*.cpn;*.spcontped");

	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Continuous Place"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Continuous Transition"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), SP_DS_PARAM);
}

SP_DS_ContinuousPed::SP_DS_ContinuousPed(const wxString& p_pchName) :
	SP_DS_BipartGraph(p_pchName)
{
	m_sExtension = wxT("cpn");
	m_sFilter = wxT("*.cpn;*.spcontped");

	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Continuous Place"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Continuous Transition"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), SP_DS_PARAM);
}

SP_DS_Graph*
SP_DS_ContinuousPed::CreateGraph(SP_DS_Graph* p_pcGraph)
{

	if (!SP_DS_BipartGraph::CreateGraph(p_pcGraph))
		return NULL;

	SP_DS_Attribute* l_pcAttr;
	SP_DS_Coarse* l_pcCoarse;
	SP_GR_Node* l_pcGr;
	SP_Graphic* l_pcGrAttr;
	SP_DS_Nodeclass* l_pcNC;
     SP_DS_ColListAttribute* l_pcColList;
    unsigned int l_nNewRow;

	//////////////////////////////////////////////////////////////////////////////
    /*
     * Place
     */
	l_pcNC = p_pcGraph->RenameNodeclass(wxT("Circle"), SP_DS_CONTINUOUS_PLACE);
	l_pcNC->SetDisplayName(wxT("Continuous Place"));
	l_pcNC->SetShortcut(wxT("P"));
	l_pcAttr = l_pcNC->GetPrototype()->GetAttribute(wxT("Name"));//Name
	l_pcAttr->SetGlobalShow();
	l_pcAttr = l_pcNC->AddAttribute(new SP_DS_IdAttribute(wxT("ID")));//ID
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogShowOnly(wxT("General")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_NumberAttribute(l_pcAttr, wxT("_p%_")));
	l_pcGrAttr->SetOffsetX(20);
	l_pcGrAttr->SetShow(FALSE);
	l_pcAttr->SetGlobalShow();

	l_pcAttr = l_pcNC->AddAttribute(new SP_DS_DoubleMarkingAttribute(wxT("Marking"), 0.0));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General")));
	l_pcAttr->AddGraphic(new SP_GR_MarkNumberAttribute(l_pcAttr));

	l_pcAttr = l_pcNC->AddAttribute(new SP_DS_BoolAttribute(wxT("Fixed"), FALSE));//Fixed
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogBool(wxT("General")));

	l_pcAttr = l_pcNC->AddAttribute(new SP_DS_LogicAttribute(wxT("Logic"), wxT("Name")));//logic
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogBool(wxT("General")));

	l_pcAttr = l_pcNC->AddAttribute(new SP_DS_TextAttribute(wxT("Comment"), wxT("")));//Comment
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
	l_pcGrAttr->SetOffsetY(40);
	l_pcAttr->SetGlobalShow();

	int pwidth = wxGetApp().GetElementPrefs()->GetNodeWidth(GetName(), SP_DS_CONTINUOUS_PLACE);

	l_pcGr = new SP_GR_ExtendedCircle(l_pcNC->GetPrototype(), pwidth, SP_EXTENDED_TYPE_DEFAULT,3,wxColour(128,128,128));
	l_pcGr->SetDefaultPen(wxThePenList->FindOrCreatePen(wxColour(128, 128, 128), 3, wxSOLID ));
	l_pcGr->SetFixedSize(wxGetApp().GetElementPrefs()->GetNodeFixed(GetName(), l_pcNC->GetName()));
	l_pcNC->SetGraphic(l_pcGr);
	l_pcNC->RegisterGraphicWidget(new SP_WDG_DialogExtendedGraphic(wxT("Graphic")));

	/////////////////////////////////////////////////////////////////////////////////////////
    /*
     * Transition
     */
	l_pcNC = p_pcGraph->RenameNodeclass(wxT("Rectangle"), SP_DS_CONTINUOUS_TRANS);
	l_pcNC->SetDisplayName(wxT("Continuous Transition"));
	l_pcNC->SetShortcut(wxT("T"));

	l_pcAttr = l_pcNC->GetPrototype()->GetAttribute(wxT("Name"));//Name
	l_pcAttr->SetGlobalShow();
	l_pcAttr = l_pcNC->AddAttribute( new SP_DS_ColListAttribute( wxT("FunctionList"), SP_COLLIST_STRING, 2 ) );
	l_pcAttr->SetDisplayName(wxT("Function set"));
	l_pcAttr->RegisterDialogWidget( new SP_WDG_StFunctionList( wxT("Function") ) );
	l_pcGrAttr = l_pcAttr->AddGraphic( new SP_GR_StFunctionAttribute( l_pcAttr ) );
	l_pcGrAttr->SetOffsetY( 30 );
	l_pcGrAttr->SetShow( FALSE );
	l_pcAttr->SetGlobalShow();
	l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >( l_pcAttr );
	l_pcColList->SetColLabel( 0, wxT("Function set") );
	l_pcColList->SetColLabel( 1, wxT("Function") );
	l_nNewRow = l_pcColList->AppendEmptyRow();
	l_pcColList->SetCell(l_nNewRow, 0, wxT("Main"));
	l_pcColList->SetCell(l_nNewRow, 1, wxT("MassAction(1)"));
	l_pcGrAttr->SetOffsetY(30);
	l_pcGrAttr->SetShow(FALSE);
	l_pcAttr->SetGlobalShow();

	l_pcAttr = l_pcNC->AddAttribute(new SP_DS_IdAttribute(wxT("ID")));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogShowOnly(wxT("General")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_NumberAttribute(l_pcAttr, wxT("_t%_")));
	l_pcGrAttr->SetOffsetX(20);
	l_pcGrAttr->SetShow(FALSE);
	l_pcAttr->SetGlobalShow();

	l_pcAttr = l_pcNC->AddAttribute(new SP_DS_LogicAttribute(wxT("Logic"), wxT("Name")));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogBool(wxT("General")));

	l_pcAttr = l_pcNC->AddAttribute(new SP_DS_TextAttribute(wxT("Comment"), wxT("")));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
	l_pcGrAttr->SetOffsetX(40);
	l_pcAttr->SetGlobalShow();

	l_pcAttr = l_pcNC->AddAttribute(new SP_DS_BoolAttribute(wxT("Reversible"), FALSE));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogBool(wxT("Function")));

	int twidth = wxGetApp().GetElementPrefs()->GetNodeWidth(GetName(), SP_DS_CONTINUOUS_TRANS);
	int theight = wxGetApp().GetElementPrefs()->GetNodeHeight(GetName(), SP_DS_CONTINUOUS_TRANS);

	l_pcGr = new SP_GR_ExtendedRectangle(l_pcNC->GetPrototype(), twidth, theight,TRUE,SP_EXTENDED_TYPE_DEFAULT,3,wxColour(128,128,128));
	l_pcGr->SetDefaultPen(wxThePenList->FindOrCreatePen(wxColour(128, 128, 128), 3, wxSOLID ));
	l_pcGr->SetFixedSize(wxGetApp().GetElementPrefs()->GetNodeFixed(GetName(), l_pcNC->GetName()));
	l_pcNC->SetGraphic(l_pcGr);
	l_pcNC->RegisterGraphicWidget(new SP_WDG_DialogExtendedGraphic(wxT("Graphic")));


	///////////////////////////////////////////////////////////////////////////////
	l_pcNC = p_pcGraph->AddNodeclass(new SP_DS_Nodeclass(p_pcGraph, SP_DS_COARSE_PLACE));
	l_pcNC->SetDisplayName(wxT("Coarse Place"));
	l_pcNC->SetShortcut(wxT("Shift+P"));
	l_pcAttr = l_pcNC->AddAttribute(new SP_DS_NameAttribute(wxT("Name"), wxT("")));
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
	l_pcCoarse = new SP_DS_Coarse(wxT("Subnet"), SP_DS_CONTINUOUS_PLACE, p_pcGraph, FALSE);
	l_pcCoarse->SetLabelAttribute(wxT("Name"));
	l_pcNC->SetCoarse(l_pcCoarse);

	l_pcGr = new SP_GR_ExtendedCircle(l_pcNC->GetPrototype(), pwidth, SP_EXTENDED_TYPE_TWO, 3,wxColour(128,128,128));
	l_pcGr->SetDefaultPen(wxThePenList->FindOrCreatePen(wxColour(128, 128, 128), 3, wxSOLID ));
	l_pcGr->SetFixedSize(wxGetApp().GetElementPrefs()->GetNodeFixed(GetName(), SP_DS_CONTINUOUS_PLACE));
	l_pcNC->SetGraphic(l_pcGr);
	l_pcNC->RegisterGraphicWidget(new SP_WDG_DialogExtendedGraphic(wxT("Graphic")));

	//////////////////////////////////////////////////////////////////////////////
	l_pcNC = p_pcGraph->AddNodeclass(new SP_DS_Nodeclass(p_pcGraph, SP_DS_COARSE_TRANSITION));
	l_pcNC->SetDisplayName(wxT("Coarse Transition"));
	l_pcNC->SetShortcut(wxT("Shift+T"));
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
	l_pcCoarse = new SP_DS_Coarse(wxT("Subnet"), SP_DS_CONTINUOUS_TRANS, p_pcGraph, FALSE);
	l_pcCoarse->SetLabelAttribute(wxT("Name"));
	l_pcNC->SetCoarse(l_pcCoarse);

	l_pcGr = new SP_GR_ExtendedRectangle(l_pcNC->GetPrototype(), twidth, theight,TRUE,SP_EXTENDED_TYPE_TWO, 3,wxColour(128,128,128));
	l_pcGr->SetDefaultPen(wxThePenList->FindOrCreatePen(wxColour(128, 128, 128), 3, wxSOLID ));
	l_pcGr->SetFixedSize(wxGetApp().GetElementPrefs()->GetNodeFixed(GetName(), SP_DS_CONTINUOUS_TRANS));
	l_pcNC->SetGraphic(l_pcGr);
	l_pcNC->RegisterGraphicWidget(new SP_WDG_DialogExtendedGraphic(wxT("Graphic")));


	///////////////////////////////////////////////////////////////////////////////
	SP_DS_Edgeclass* l_pcEC = p_pcGraph->RenameEdgeclass(wxT("Edge"), SP_DS_EDGE);
	l_pcEC->SetDisplayName(wxT("Edge"));
	l_pcEC->SetShortcut(wxT("E"));
	l_pcAttr = l_pcEC->GetPrototype()->GetAttribute(wxT("Name"));
	l_pcAttr->SetGlobalShow();

	//special wish from mh: edges have no names, but this conflicts with the compatibility of the nets we have
	if (l_pcAttr->Remove())
	{
		wxDELETE(l_pcAttr);
	}

	l_pcEC->RegisterGraphicWidget(new SP_WDG_DialogGraphic(wxT("Graphic")));

	l_pcAttr = l_pcEC->AddAttribute(new SP_DS_DoubleMarkingDependentMultiplicity(wxT("Multiplicity"), wxT("1")));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_MultiplicityAttribute(l_pcAttr));
	l_pcAttr->SetGlobalShow();
	l_pcGrAttr->SetOffsetY(20);
	l_pcGrAttr->SetShow(true);


	l_pcAttr = l_pcEC->AddAttribute(new SP_DS_TextAttribute(wxT("Comment"), wxT("")));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
	l_pcGrAttr->SetOffsetX(40);
	l_pcAttr->SetGlobalShow();

	///////////////////////////////////////////////////////////////////////////////
	l_pcEC = p_pcGraph->AddEdgeclass(new SP_DS_Edgeclass(p_pcGraph, SP_DS_INHIBITOR_EDGE));
	l_pcEC->SetDisplayName(wxT("Inhibitor Edge"));
	l_pcEC->SetShortcut(wxT("I"));
	l_pcEC->SetGraphic(new SP_GR_ArrowEdge(l_pcEC->GetPrototype(), ARROW_HOLLOW_CIRCLE, ARROW_POSITION_END, 6.0));
	l_pcEC->RegisterGraphicWidget(new SP_WDG_DialogGraphic(wxT("Graphic")));

	l_pcAttr = l_pcEC->AddAttribute(new SP_DS_DoubleMarkingDependentMultiplicity(wxT("Multiplicity"), wxT("1")));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_MultiplicityAttribute(l_pcAttr));
	l_pcGrAttr->SetOffsetY(20);
	l_pcGrAttr->SetShow(true);
	l_pcAttr->SetGlobalShow();

	l_pcAttr = l_pcEC->AddAttribute(new SP_DS_TextAttribute(wxT("Comment"), wxT("")));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
	l_pcGrAttr->SetOffsetX(40);
	l_pcAttr->SetGlobalShow();

	///////////////////////////////////////////////////////////////////////////////
	l_pcEC = p_pcGraph->AddEdgeclass(new SP_DS_Edgeclass(p_pcGraph, SP_DS_READ_EDGE));
	l_pcEC->SetDisplayName(wxT("Test Edge"));
	l_pcEC->SetShortcut(wxT("R"));
	l_pcEC->SetGraphic(new SP_GR_ArrowEdge(l_pcEC->GetPrototype(), ARROW_FILLED_CIRCLE, ARROW_POSITION_END, 6.0));
	l_pcEC->RegisterGraphicWidget(new SP_WDG_DialogGraphic(wxT("Graphic")));

	l_pcAttr = l_pcEC->AddAttribute(new SP_DS_DoubleMarkingDependentMultiplicity(wxT("Multiplicity"), wxT("1")));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_MultiplicityAttribute(l_pcAttr));
	l_pcGrAttr->SetOffsetY(20);
	l_pcGrAttr->SetShow(true);
	l_pcAttr->SetGlobalShow();

	l_pcAttr = l_pcEC->AddAttribute(new SP_DS_TextAttribute(wxT("Comment"), wxT("")));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
	l_pcAttr->SetGlobalShow();
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
	l_pcGrAttr->SetOffsetX(40);


	///////////////////////////////////////////////////////////////////////////////
	l_pcEC = p_pcGraph->AddEdgeclass( new SP_DS_Edgeclass( p_pcGraph, SP_DS_MODIFIER_EDGE ) );
	l_pcEC->SetGraphic(new SP_GR_ExtendedEdge(l_pcEC->GetPrototype(),SP_EXTENDED_TYPE_DASHED_EDGE,1,
								SP_EXTENDED_ARROW_TYPE_NONE_EDGE,10,SP_EXTENDED_ARROW_TYPE_ARROW_EDGE,10) );
	l_pcEC->SetShortcut( wxT("M") );

	l_pcEC->RegisterGraphicWidget( new SP_WDG_DialogExtendedGraphic( wxT("Graphic") ) );

	l_pcAttr = l_pcEC->AddAttribute( new SP_DS_TextAttribute( wxT("Comment"), wxT("") ) );
	l_pcAttr->RegisterDialogWidget( new SP_WDG_DialogMultiline( wxT("General") ) );
	l_pcAttr->SetGlobalShow();

	l_pcGrAttr = l_pcAttr->AddGraphic( new SP_GR_TextAttribute( l_pcAttr ) );
	l_pcGrAttr->SetOffsetX( 40 );

	//////////////////////////////////////////////////////////////////////////////
	SP_DS_Metadataclass* l_pcMC = p_pcGraph->AddMetadataclass( new SP_DS_Metadataclass( p_pcGraph, wxT("Plot") ) );

	l_pcMC->SetShowInElementTree( false );

	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("Name") ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_IdAttribute( wxT("ID") ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("Comment"), wxT("") ) );

	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("Nodeclass"), wxT("Place") ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("ViewerType"), wxT("xyPlot") ) );

	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("Results"), wxT("Marking") ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_ColListAttribute( wxT("PlaceList"), SP_COLLIST_UNSIGNED_INTEGER, 1 ) );

	/*
	* Curve information:
	* 0: Curve ID
	* 1: place type (colored, uncolored...)
	* 2: State: (checked, unchecked)
	* 3: Curve color
	* 4: Line width
	* 5: line style
	*/

	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_ColListAttribute( wxT("CurveInfo"), SP_COLLIST_STRING,7) );

	//Is current view
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("IsCurrent"), wxT("0") ) );

	//xy plot
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("ViewTitle"), wxT("Untitled") ) );
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
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_NumberAttribute( wxT("SortCurvesBy"), 0) );

	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_NumberAttribute( wxT("HistogramFrequencyType"), 0) );

	//////////////////////////////////////////////////////////////////////////////
	l_pcMC = p_pcGraph->AddMetadataclass( new SP_DS_Metadataclass( p_pcGraph, wxT("Table") ) );

	l_pcMC->SetShowInElementTree( false );

	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("Name") ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_IdAttribute( wxT("ID") ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("Comment"), wxT("") ) );

	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("Nodeclass"), wxT("Place") ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("Results"), wxT("Marking") ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_ColListAttribute( wxT("PlaceList"), SP_COLLIST_UNSIGNED_INTEGER, 1 ) );

	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_BoolAttribute( wxT("SumOfNodes"), false ) );


	//////////////////////////////////////////////////////////////////////////////
	l_pcMC = p_pcGraph->AddMetadataclass( new SP_DS_Metadataclass( p_pcGraph, wxT("Simulation Properties") ) );

	l_pcMC->SetShowInElementTree( false );

	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_DoubleAttribute( wxT("interval start"), 0.0 ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_DoubleAttribute( wxT("interval end"), 100.0 ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_NumberAttribute( wxT("output step"), 100 ) );

	///simulator properties
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_NumberAttribute( wxT("simulator"), 0 ));
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_ColListAttribute( wxT("simulator properties"), SP_COLLIST_STRING, 2 ) );

	//////////////////////////////////////////////////////////////////////////////
	l_pcMC = p_pcGraph->AddMetadataclass( new SP_DS_Metadataclass( p_pcGraph, wxT("Simulation Results") ) );

	l_pcMC->SetShowInElementTree( false );

	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("marking") ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("marking square") ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("firing rate") ) );


	/*
	 * add new metadata class
	 * by sl
	 */
	/////////////////////////////////////////////////////////////////////////////////////
	l_pcMC = p_pcGraph->AddMetadataclass( new SP_DS_Metadataclass( p_pcGraph, SP_DS_META_CONSTANT ) );

	l_pcMC->SetDisplayName(wxT("Constants"));

	l_pcMC->SetShowInElementTree( false );
	l_pcMC->SetShowInDeclarationTreeOther( true );

	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_NameAttribute( wxT("Name") ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_IdAttribute( wxT("ID") ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("Group") ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TypeAttribute( wxT("Type"), wxT("int") ) );
	SP_DS_TypeAttribute* l_pcType = dynamic_cast< SP_DS_TypeAttribute* >( l_pcAttr );
	l_pcType->AddPossibleValue(wxT("double"));

	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_ColListAttribute( wxT("ValueList"), SP_COLLIST_STRING, 2 ,wxT("Group")) );
	l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >( l_pcAttr );
	l_pcColList->SetColLabel( 0, wxT("Value set") );
	l_pcColList->SetColLabel( 1, wxT("Value") );
	unsigned int l_nRow = l_pcColList->AppendEmptyRow();
	l_pcColList->SetCell(l_nRow, 0, wxT("Main"));
	l_pcColList->SetCell(l_nRow, 1, wxT(""));

	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("Comment") ) );

    /////////////////////////////////////////////////////////////////////////////////////
	l_pcMC = p_pcGraph->AddMetadataclass( new SP_DS_Metadataclass( p_pcGraph, SP_DS_META_FUNCTION ) );

	l_pcMC->SetDisplayName(wxT("Functions"));

	l_pcMC->SetShowInElementTree( false );
	l_pcMC->SetShowInDeclarationTreeOther( true );

	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_NameAttribute( wxT("Name") ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_IdAttribute( wxT("ID") ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("Parameter") ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_EquationAttribute( wxT("Body") ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TypeAttribute( wxT("Return"), wxT("int") ) );
	l_pcType = dynamic_cast< SP_DS_TypeAttribute* >( l_pcAttr );
	l_pcType->AddPossibleValue(wxT("double"));

	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("Comment") ) );

    /////////////////////////////////////////////////////////////////////////////////////



	p_pcGraph->SetAnimation(new SP_DS_ContinuousPedAnimation(1, 2, 50, 1.0));

	return p_pcGraph;
}
bool
SP_DS_ContinuousPed::CreateInitialElements(SP_DS_Graph* p_pcGraph)
{
	SP_DS_BipartGraph::CreateInitialElements(p_pcGraph);

	SP_DS_Metadataclass* l_pcMC;
	l_pcMC = p_pcGraph->GetMetadataclass(wxT("Plot"));
	SP_DS_Metadata* l_pcMainPlot = l_pcMC->NewElement( 1 );
	if(l_pcMainPlot)
	{
		l_pcMainPlot->GetAttribute( wxT("Name") )->SetValueString( wxT("Default View") );
		l_pcMainPlot->GetAttribute( wxT("Comment") )->SetValueString( wxT("Default View") );
		l_pcMainPlot->GetAttribute( wxT("IsCurrent") )->SetValueString( wxT("1") );
	}
	
	l_pcMC = p_pcGraph->GetMetadataclass(wxT("Simulation Properties"));
	l_pcMC->NewElement( 1 );

	l_pcMC = p_pcGraph->GetMetadataclass(wxT("Simulation Results"));
	l_pcMC->NewElement( 1 );

	return true;
}

bool
SP_DS_ContinuousPed::MetadataRequirement( SP_DS_Metadata* p_pcMetadata )
{
    if(!SP_DS_BipartGraph::MetadataRequirement( p_pcMetadata ))
    {
    	return false;
    }

    wxString l_sName = p_pcMetadata->GetClassName();
	if ((l_sName == wxT("Plot") || l_sName == wxT("Table") || l_sName == wxT("Simulation Properties") || l_sName == wxT("Simulation Results") )
		&& !p_pcMetadata->GetMetadataclass()->GetElements()->empty())
	{
		return false;
	}

	return true;
}


bool SP_DS_ContinuousPed::NodeRequirement(SP_DS_Node* p_pcNode)
{
	return SP_DS_BipartGraph::NodeRequirement(p_pcNode);
}

bool SP_DS_ContinuousPed::EdgeRequirement(SP_DS_Edgeclass* p_pcClass, SP_Data* p_pcSrc, SP_Data* p_pcTrg)
{
	if (!SP_DS_BipartGraph::EdgeRequirement(p_pcClass, p_pcSrc, p_pcTrg))
		return FALSE;

	if (p_pcSrc->GetElementType() != SP_ELEMENT_NODE || p_pcTrg->GetElementType() != SP_ELEMENT_NODE)
		return FALSE;

	if (p_pcSrc->GetClassName() == wxT("Comment"))
		return FALSE;
	if (p_pcTrg->GetClassName() == wxT("Comment"))
		return FALSE;

	SP_DS_Node* l_pcSrc = dynamic_cast<SP_DS_Node*> (p_pcSrc);
	SP_DS_Node* l_pcTrg = dynamic_cast<SP_DS_Node*> (p_pcTrg);
	if (l_pcSrc->GetNodeclass() == l_pcTrg->GetNodeclass())
		return FALSE;

	// Test and Inhibitor Edge only from Place to Transition
	if ((p_pcClass->GetName() == SP_DS_INHIBITOR_EDGE) || (p_pcClass->GetName() == SP_DS_READ_EDGE))
	{
		if (l_pcSrc->GetNodeclass()->GetName() == SP_DS_CONTINUOUS_TRANS)
			return FALSE;
		if (l_pcTrg->GetNodeclass()->GetName() == SP_DS_CONTINUOUS_PLACE)
			return FALSE;
		if (l_pcSrc->GetNodeclass()->GetName() == SP_DS_PARAM)
			return FALSE;
		if (l_pcTrg->GetNodeclass()->GetName() == SP_DS_PARAM)
			return FALSE;
	}

	// Egde only from Place to Transition or Transition to Place
	if ( ((l_pcSrc->GetNodeclass()->GetName() == SP_DS_PARAM) ||
		(l_pcTrg->GetNodeclass()->GetName() == SP_DS_PARAM)))
	{
		return FALSE;
	}

	if (l_pcSrc->GetCoarse() && l_pcTrg->GetCoarse())
		return FALSE;
	if (l_pcSrc->GetCoarse())
		return (l_pcSrc->GetCoarse()->GetInnerClass() != l_pcTrg->GetNodeclass()->GetName());

	if (l_pcTrg->GetCoarse())
		return (l_pcTrg->GetCoarse()->GetInnerClass() != l_pcSrc->GetNodeclass()->GetName());

	SP_ListEdge::const_iterator eIt;
	const SP_ListEdge* edges;
	edges = (dynamic_cast<SP_DS_Node*> (p_pcSrc))->GetSourceEdges();

	//check if there are another arc of the same type between these nodes
	for (eIt = edges->begin(); eIt != edges->end(); ++eIt)
	{
		SP_LOGDEBUG(wxString::Format(wxT("%p->%p (%p), %s"), (*eIt)->GetSource(), (*eIt)->GetTarget(), p_pcTrg, (*eIt)->GetClassName().c_str()));

		if ((*eIt)->GetTarget() == p_pcTrg && (*eIt)->GetClassName() == p_pcClass->GetName())
		{
			SP_MESSAGEBOX(wxString::Format(wxT("Only one edge of the type \"%s\" ")
			wxT("is allowed between these nodes"), (*eIt)->GetClassName().c_str()), wxT("Error"), wxOK | wxICON_ERROR);
			return false;
		}
	}

	//Modifier arc
	if(p_pcClass->GetName()==SP_DS_MODIFIER_EDGE)
	{
		if(p_pcSrc->GetClassName().Contains(wxT("Transition")))
		  return false;
	}
	return TRUE;
}

