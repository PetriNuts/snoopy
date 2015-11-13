/*
 * SP_DS_HybridPN.cpp
 *
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 5.7.2010
 * Short Description:
 */
 //======================================================================================

#include "sp_defines.h"
#include "snoopy.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_core/SP_GPR_Elements.h"
#include "sp_core/SP_GPR_Animation.h"
#include "sp_ds/netclasses/SP_DS_HybridPN.h"

#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/extensions/SP_DS_Coarse.h"

#include "sp_ds/SP_DS_Edgeclass.h"
#include "sp_ds/SP_DS_Nodeclass.h"
#include "sp_ds/SP_DS_Metadataclass.h"

// extended graphical representation
#include "sp_gr/shapes/SP_GR_ExtendedCircle.h"
#include "sp_gr/shapes/SP_GR_ExtendedRectangle.h"
#include "sp_gr/shapes/SP_GR_ExtendedEllipse.h"
#include "sp_gr/metadata/SP_GR_PlotSymbol.h"
#include "sp_gr/shapes/SP_GR_Rectangle.h"

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
#include "sp_gr/attributes/gra_SPN/SP_GR_StParameterListAttribute.h"

#include "sp_ds/attributes/SP_DS_BoolAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/attributes/SP_DS_NumberAttribute.h"
#include "sp_ds/attributes/SP_DS_DoubleAttribute.h"
#include "sp_ds/attributes/SP_DS_LogicAttribute.h"
#include "sp_ds/attributes/SP_DS_IdAttribute.h"
#include "sp_ds/attributes/SP_DS_MultiplicityAttribute.h"
#include "sp_ds/attributes/SP_DS_DoubleMultiplicityAttribute.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"

//by sl
#include "sp_ds/attributes/SP_DS_DoubleMarkingDependentMultiplicity.h"
#include "sp_ds/attributes/SP_DS_DoubleMarkingAttribute.h"
#include "sp_ds/attributes/SP_DS_MarkingAttribute.h"
#include "sp_ds/attributes/SP_DS_TypeAttribute.h"

#include "sp_gui/widgets/dialogs/SP_WDG_DialogText.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogBool.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogShowOnly.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogGraphic.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogExtendedGraphic.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogMultiline.h"
#include "sp_gui/widgets/dialogs/wid_dia_SPN/SP_WDG_StFunctionList.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogColList.h"

#include "sp_gr/edges/SP_GR_ArrowEdge.h"
#include "sp_gr/edges/SP_GR_ExtendedEdge.h"

#include "sp_ds/animators/ani_HybridPN/SP_DS_HybridAnimation.h"

SP_DS_HybridPN::SP_DS_HybridPN() :
SP_DS_ExtPT(SP_DS_HYBRIDPN_CLASS)
{
	m_sExtension = wxT("hpn");
	m_sFilter = wxT("*.hpn;*.sphybrid");

	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Discrete Place"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Continuous Place"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Discrete Transition"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Continuous Transition"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), SP_DS_PARAM);

}

SP_DS_HybridPN::SP_DS_HybridPN(const wxString& p_pchName) :
SP_DS_ExtPT(p_pchName)
{
	m_sExtension = wxT("hpn");
	m_sFilter = wxT("*.hpn;*.sphybrid");

	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Discrete Place"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Continuous Place"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Discrete Transition"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Continuous Transition"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), SP_DS_PARAM);

}

SP_DS_Graph* SP_DS_HybridPN::CreateGraph(SP_DS_Graph* p_pcGraph)
{

	SP_DS_Attribute* l_pcAttr;
	SP_GR_Node* l_pcGr;
	SP_Graphic* l_pcGrAttr;
	SP_DS_Nodeclass* l_pcNC;
	SP_DS_Edgeclass* l_pcEC;
	SP_DS_ColListAttribute* l_pcColList;
    unsigned int l_nNewRow;
	if (!SP_DS_ExtPT::CreateGraph(p_pcGraph))
	  return NULL;

   //===============================================================================================================
   /*
    * Discrete Place
    */
	l_pcNC = p_pcGraph->GetNodeclass(SP_DS_DISCRETE_PLACE );
	l_pcNC->SetDisplayName(wxT("Discrete Place"));


  	//==============================================================================================================
  	/*
     * Continuous Place
     */
  	 l_pcNC = p_pcGraph->AddNodeclass(new SP_DS_Nodeclass(p_pcGraph, SP_DS_CONTINUOUS_PLACE,p_pcGraph->GetNodeclass(SP_DS_DISCRETE_PLACE)->GetIdCountPtr()));
  	 l_pcNC->SetShortcut(wxT("Shift+P"));
  	 l_pcNC->SetDisplayName(wxT("Continuous Place"));

  	 l_pcAttr = l_pcNC->AddAttribute(new SP_DS_BoolAttribute(wxT("Fixed"), FALSE));//Fixed
  	 l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogBool(wxT("General"), 1));

  	 l_pcAttr = l_pcNC->AddAttribute(new SP_DS_NameAttribute(wxT("Name")));
  	 l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General")));
  	 l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
  	 l_pcGrAttr->SetOffsetY(20);
  	 l_pcGrAttr->SetShow(TRUE);
  	 l_pcAttr->SetGlobalShow();

  	 l_pcAttr = l_pcNC->AddAttribute(new SP_DS_IdAttribute(wxT("ID")));
  	 l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogShowOnly(wxT("General")));
  	 l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_NumberAttribute(l_pcAttr, wxT("_pa%_")));
  	 l_pcGrAttr->SetOffsetX(20);
  	 l_pcGrAttr->SetShow(FALSE);
  	 l_pcAttr->SetGlobalShow();


  	l_pcAttr = l_pcNC->AddAttribute(new SP_DS_DoubleMarkingAttribute(wxT("Marking"), 0.0));
  	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General")));
  	l_pcAttr->AddGraphic(new SP_GR_MarkNumberAttribute(l_pcAttr));



  	 int pwidth = wxGetApp().GetElementPrefs()->GetNodeWidth(GetName(), SP_DS_CONTINUOUS_PLACE);
  	 l_pcGr = new SP_GR_ExtendedCircle(l_pcNC->GetPrototype(), pwidth, SP_EXTENDED_TYPE_DEFAULT,3,wxColour(128,128,128));
  	 l_pcGr->SetDefaultPen(wxThePenList->FindOrCreatePen(wxColour(128, 128, 128), 3 ));
  	 l_pcGr->SetFixedSize(wxGetApp().GetElementPrefs()->GetNodeFixed(GetName(), l_pcNC->GetName()));
  	 l_pcNC->SetGraphic( l_pcGr );
  	 l_pcNC->RegisterGraphicWidget( new SP_WDG_DialogExtendedGraphic( wxT("Graphic") ) );

  	 l_pcAttr = l_pcNC->AddAttribute(new SP_DS_LogicAttribute(wxT("Logic"), wxT("Name")));
  	 l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogBool(wxT("General")));

  	 l_pcAttr = l_pcNC->AddAttribute(new SP_DS_TextAttribute(wxT("Comment"), wxT("")));
  	 l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
  	 l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
  	 l_pcGrAttr->SetOffsetY(40);
  	 l_pcAttr->SetGlobalShow();


  	//==============================================================================================================
  	/*
  	 * Transition
     */
  	l_pcNC = p_pcGraph->GetNodeclass( SP_DS_STOCHASTIC_TRANS );
  	l_pcNC->SetDisplayName(wxT("Stochastic Transition"));

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

  	//Reorder net elements
  	l_pcNC=p_pcGraph->RemoveNodeclass(l_pcNC);
  	p_pcGraph->AddNodeclass(l_pcNC);
    //==========================================================================================================
  	/*
  	 * Continuous Transition
  	 */
  	 l_pcNC = p_pcGraph->AddNodeclass( new SP_DS_Nodeclass( p_pcGraph,
  	  																SP_DS_CONTINUOUS_TRANS,
  	  																p_pcGraph->GetNodeclass(SP_DS_STOCHASTIC_TRANS)->GetIdCountPtr() ) );
  	l_pcNC->SetDisplayName(wxT("Continuous Transition"));

  	 l_pcNC->SetShortcut( wxT("Shift+T") );
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
  	 l_pcAttr->SetDisplayName(wxT("Function set"));
  	 l_pcAttr->RegisterDialogWidget( new SP_WDG_StFunctionList( wxT("Functions") ) );
  	 l_pcGrAttr = l_pcAttr->AddGraphic( new SP_GR_ColListAttribute( l_pcAttr ) );
  	 l_pcGrAttr->SetOffsetX(25);
  	 l_pcGrAttr->SetOffsetY(30);
  	 l_pcGrAttr->SetShow(FALSE);
  	 l_pcAttr->SetGlobalShow();
  	 l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >( l_pcAttr );
  	 l_pcColList->SetColLabel( 0, wxT("Function set") );
  	 l_pcColList->SetColLabel( 1, wxT("Function") );
  	 l_nNewRow = l_pcColList->AppendEmptyRow();
  	 l_pcColList->SetCell(l_nNewRow, 0, wxT("Main"));
  	 l_pcColList->SetCell(l_nNewRow, 1, wxT("MassAction(1)"));
  	 int twidth = wxGetApp().GetElementPrefs()->GetNodeWidth(GetName(),SP_DS_CONTINUOUS_TRANS);
  	 int theight = wxGetApp().GetElementPrefs()->GetNodeHeight(GetName(), SP_DS_CONTINUOUS_TRANS);
  	 l_pcGr = new SP_GR_ExtendedRectangle(l_pcNC->GetPrototype(), twidth, theight,TRUE,SP_EXTENDED_TYPE_DEFAULT,3,wxColour(128,128,128));
  	 l_pcGr->SetDefaultPen(wxThePenList->FindOrCreatePen(wxColour(128, 128, 128), 3 ));
  	 l_pcGr->SetFixedSize(wxGetApp().GetElementPrefs()->GetNodeFixed(GetName(), l_pcNC->GetName()));
  	 l_pcNC->SetGraphic(l_pcGr);
  	 l_pcNC->RegisterGraphicWidget(new SP_WDG_DialogExtendedGraphic(wxT("Graphic")));

  	//===========================================================================================================
  	/*
  	 *Immediate Transition
  	 */
  	 l_pcNC = p_pcGraph->AddNodeclass( new SP_DS_Nodeclass( p_pcGraph,
  			                                                    SP_DS_IMMEDIATE_TRANS,
  																p_pcGraph->GetNodeclass(SP_DS_STOCHASTIC_TRANS)->GetIdCountPtr() ) );
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


  	 twidth = wxGetApp().GetElementPrefs()->GetNodeWidth(GetName(), SP_DS_IMMEDIATE_TRANS);
  	 theight = wxGetApp().GetElementPrefs()->GetNodeHeight(GetName(), SP_DS_IMMEDIATE_TRANS);

  	 l_pcGr = new SP_GR_Rectangle(l_pcNC->GetPrototype(), twidth, theight);
  	 l_pcGr->SetHeight(20.0);
  	 l_pcGr->SetWidth(10.0);
  	 l_pcGr->SetDefaultBrush(wxTheBrushList->FindOrCreateBrush(wxColour(0, 0, 0)));
  	 l_pcGr->SetFixedSize(wxGetApp().GetElementPrefs()->GetNodeFixed(GetName(), l_pcNC->GetName()));
  	 l_pcNC->SetGraphic(l_pcGr);
  	 l_pcNC->RegisterGraphicWidget(new SP_WDG_DialogGraphic(wxT("Graphic")));
  		// animator
  		//l_pcNC->AddAnimator(new SP_DS_TransAnimator());

    //==================================================================================================================
  	/*
  	 *Deterministic Transition
  	 */
  	 l_pcNC = p_pcGraph->AddNodeclass( new SP_DS_Nodeclass( p_pcGraph,
  			                                                    SP_DS_DETERMINISTIC_TRANS,
  																p_pcGraph->GetNodeclass(SP_DS_STOCHASTIC_TRANS)->GetIdCountPtr() ) );

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


  	 twidth = wxGetApp().GetElementPrefs()->GetNodeWidth(GetName(), SP_DS_DETERMINISTIC_TRANS);
  	 theight = wxGetApp().GetElementPrefs()->GetNodeHeight(GetName(), SP_DS_DETERMINISTIC_TRANS);

  	 l_pcGr = new SP_GR_Rectangle(l_pcNC->GetPrototype(), twidth, theight);
  	 l_pcGr->SetDefaultBrush(wxTheBrushList->FindOrCreateBrush(wxColour(0, 0, 0)));
  	 l_pcGr->SetFixedSize(wxGetApp().GetElementPrefs()->GetNodeFixed(GetName(), l_pcNC->GetName()));
  	 l_pcNC->SetGraphic(l_pcGr);
  	 l_pcNC->RegisterGraphicWidget(new SP_WDG_DialogGraphic(wxT("Graphic")));
  	 // animator
     //l_pcNC->AddAnimator(new SP_DS_TransAnimator());
  	 //==================================================================================================================
  	 /*
  	  *Scheduled Transition
  	  */
  	 l_pcNC = p_pcGraph->AddNodeclass( new SP_DS_Nodeclass( p_pcGraph,
  			                                                    SP_DS_SCHEDULED_TRANS,
  																p_pcGraph->GetNodeclass(SP_DS_STOCHASTIC_TRANS)->GetIdCountPtr() ) );

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


  	l_pcAttr = l_pcNC->AddAttribute(new SP_DS_LogicAttribute(wxT("Logic"), wxT("Name")));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogBool(wxT("General")));

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


  	 twidth = wxGetApp().GetElementPrefs()->GetNodeWidth(GetName(), SP_DS_SCHEDULED_TRANS);
  	 theight = wxGetApp().GetElementPrefs()->GetNodeHeight(GetName(), SP_DS_SCHEDULED_TRANS);

  	 l_pcGr = new SP_GR_Rectangle(l_pcNC->GetPrototype(), twidth, theight);
  	 l_pcGr->SetDefaultBrush(wxTheBrushList->FindOrCreateBrush(wxColour(125, 125, 125)));
  	 l_pcGr->SetFixedSize(wxGetApp().GetElementPrefs()->GetNodeFixed(GetName(), l_pcNC->GetName()));
  	 l_pcNC->SetGraphic(l_pcGr);
  	 l_pcNC->RegisterGraphicWidget(new SP_WDG_DialogGraphic(wxT("Graphic")));
  	 // animator
  	 //l_pcNC->AddAnimator(new SP_DS_TransAnimator());


     //===================================================================================================================
  	 /*
  	  *Modify other inherited nodes's Attributes
  	  */
  	 //Coarse place
  	 l_pcNC=p_pcGraph->GetNodeclass(wxT("Coarse Place"));
  	 l_pcNC->SetShortcut(wxT("Alt+P"));

  	 //insert Coarse Place at the end of net classes
  	 l_pcNC=p_pcGraph->RemoveNodeclass(l_pcNC);
  	 p_pcGraph->AddNodeclass(l_pcNC);

  	 //Coarse transition
  	 l_pcNC=p_pcGraph->GetNodeclass(wxT("Coarse Transition"));
  	 l_pcNC->SetShortcut(wxT("Alt+T"));
  	 //insert Coarse Transition at the end of net classes
  	 l_pcNC=p_pcGraph->RemoveNodeclass(l_pcNC);
	 p_pcGraph->AddNodeclass(l_pcNC);
	 //==================================================================================================================


	 /*
	 * Edges Multiplicity
	 */
	 l_pcEC=p_pcGraph->GetEdgeclass(wxT("Edge"));

  	 //remove old multiplicity
  	 l_pcAttr = l_pcEC->GetPrototype()->GetAttribute(wxT("Multiplicity"));
  	 if(l_pcAttr->Remove()) wxDELETE(l_pcAttr);

  	 l_pcAttr=l_pcEC->AddAttribute(new SP_DS_DoubleMarkingDependentMultiplicity(wxT("Multiplicity"),wxT("1")));
  	 l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General")));
  	 l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_MultiplicityAttribute(l_pcAttr));
     l_pcGrAttr->SetOffsetX(20);
  	 l_pcGrAttr->SetShow(TRUE);
  	 l_pcAttr->SetGlobalShow();



  	 //Inhibitor arc Multiplicity
  	 l_pcEC=p_pcGraph->GetEdgeclass(SP_DS_INHIBITOR_EDGE);

  	 //remove old multiplicity
  	 l_pcAttr = l_pcEC->GetPrototype()->GetAttribute(wxT("Multiplicity"));
  	 if(l_pcAttr->Remove()) wxDELETE(l_pcAttr);

	  l_pcAttr=l_pcEC->AddAttribute(new SP_DS_DoubleMarkingDependentMultiplicity(wxT("Multiplicity"),wxT("1")));
	  l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General")));
	  l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_MultiplicityAttribute(l_pcAttr));
	  l_pcGrAttr->SetOffsetX(20);
	  l_pcGrAttr->SetShow(TRUE);
	  l_pcAttr->SetGlobalShow();

  	 //Read arc Multiplicity

  	 l_pcEC=p_pcGraph->GetEdgeclass(SP_DS_READ_EDGE);

  	 //remove old multiplicity
  	 l_pcAttr = l_pcEC->GetPrototype()->GetAttribute(wxT("Multiplicity"));
  	 if(l_pcAttr->Remove()) wxDELETE(l_pcAttr);

  	 l_pcAttr=l_pcEC->AddAttribute(new SP_DS_DoubleMarkingDependentMultiplicity(wxT("Multiplicity"),wxT("1")));
  	 l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General")));
  	 l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_MultiplicityAttribute(l_pcAttr));
  	 l_pcGrAttr->SetOffsetX(20);
  	 l_pcGrAttr->SetShow(TRUE);
  	 l_pcAttr->SetGlobalShow();

  	 //Equal Multiplicity

  	 l_pcEC=p_pcGraph->GetEdgeclass(SP_DS_EQUAL_EDGE);

  	 //remove old multiplicity
  	 l_pcAttr = l_pcEC->GetPrototype()->GetAttribute(wxT("Multiplicity"));
  	 if(l_pcAttr->Remove()) wxDELETE(l_pcAttr);

  	  l_pcAttr=l_pcEC->AddAttribute(new SP_DS_DoubleMarkingDependentMultiplicity(wxT("Multiplicity"),wxT("1")));
  	  l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General")));
  	  l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_MultiplicityAttribute(l_pcAttr));
  	  l_pcGrAttr->SetOffsetX(20);
  	  l_pcGrAttr->SetShow(TRUE);
  	  l_pcAttr->SetGlobalShow();



  	  // modifier arc

  	  l_pcEC = p_pcGraph->AddEdgeclass( new SP_DS_Edgeclass( p_pcGraph, wxT("Modifier Edge") ) );
	  l_pcEC->SetGraphic(new SP_GR_ExtendedEdge(l_pcEC->GetPrototype(),SP_EXTENDED_TYPE_DASHED_EDGE,1,
									SP_EXTENDED_ARROW_TYPE_NONE_EDGE,10,SP_EXTENDED_ARROW_TYPE_ARROW_EDGE,10) );
	  l_pcEC->SetShortcut( wxT("M") );

	  l_pcEC->RegisterGraphicWidget( new SP_WDG_DialogExtendedGraphic( wxT("Graphic") ) );

	  l_pcAttr = l_pcEC->AddAttribute( new SP_DS_TextAttribute( wxT("Comment"), wxT("") ) );
	  l_pcAttr->RegisterDialogWidget( new SP_WDG_DialogMultiline( wxT("General") ) );
	  l_pcAttr->SetGlobalShow();

	  l_pcGrAttr = l_pcAttr->AddGraphic( new SP_GR_TextAttribute( l_pcAttr ) );
	  l_pcGrAttr->SetOffsetX( 40 );


	  /*
  	   * Tables and Plots
  	   */
	  SP_DS_Metadataclass* l_pcMC;

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
    l_pcAttr = l_pcMC->AddAttribute( new SP_DS_NumberAttribute( wxT("ODE Solver"), 2 ));
    l_pcAttr = l_pcMC->AddAttribute( new SP_DS_ColListAttribute( wxT("simulator properties"), SP_COLLIST_STRING, 2 ) );

 	//////////////////////////////////////////////////////////////////////////////
 	l_pcMC = p_pcGraph->AddMetadataclass( new SP_DS_Metadataclass( p_pcGraph, wxT("Simulation Results") ) );

 	l_pcMC->SetShowInElementTree( false );

 	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("marking") ) );
 	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("marking square") ) );
 	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("firing rate") ) );


 	//////////////////////////////////////////////////////////////////////////////
	l_pcMC = p_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);
	l_pcAttr = l_pcMC->GetPrototype()->GetAttribute(wxT("Type"));
	SP_DS_TypeAttribute* l_pcType = dynamic_cast< SP_DS_TypeAttribute* >( l_pcAttr );
	l_pcType->AddPossibleValue(wxT("double"));

 	//////////////////////////////////////////////////////////////////////////////
	l_pcMC = p_pcGraph->GetMetadataclass(SP_DS_META_FUNCTION);
	l_pcAttr = l_pcMC->GetPrototype()->GetAttribute(wxT("Return"));
	l_pcType = dynamic_cast< SP_DS_TypeAttribute* >( l_pcAttr );
	l_pcType->AddPossibleValue(wxT("double"));





  	 //Set Animator
  	 p_pcGraph->SetAnimation(new SP_DS_HybridAnimation(1, 2, 50, 1.0));

  	 return p_pcGraph;
}

bool SP_DS_HybridPN::NodeRequirement(SP_DS_Node* p_pcNode)
{
	return SP_DS_ExtPT::NodeRequirement( p_pcNode );
}

bool SP_DS_HybridPN::EdgeRequirement( SP_DS_Edgeclass* p_pcClass, SP_Data* p_pcSrc, SP_Data* p_pcTrg )
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

	//No connection between any two Transitions
    if(p_pcSrc->GetClassName().Contains(wxT("Transition")) && p_pcTrg->GetClassName().Contains(wxT("Transition")))
		 return FALSE;

    //No connection between any two places
     if(p_pcSrc->GetClassName().Contains(wxT("Place")) && p_pcTrg->GetClassName().Contains(wxT("Place")))
    	return FALSE;

     //No connection between a discrete place and a continuous transition using a standard arc
    if(!p_pcClass->GetName().IsEmpty())
    {
      if( (p_pcSrc->GetClassName()==SP_DS_DISCRETE_PLACE || p_pcSrc->GetClassName()==SP_DS_CONTINUOUS_TRANS) &&
          (p_pcTrg->GetClassName()==SP_DS_CONTINUOUS_TRANS || p_pcTrg->GetClassName()==SP_DS_DISCRETE_PLACE) &&
    	  p_pcClass->GetName()==SP_DS_EDGE)

    	  return FALSE;
    }

    //no connection between continuous place and any transition using an equal arc
    if(p_pcClass->GetName()==SP_DS_EQUAL_EDGE && p_pcSrc->GetClassName().Contains(wxT("Continuous")))
    {
    	return false;
    }

    //continuous transitions can not use a reset arc
	if(p_pcClass->GetName()==SP_DS_RESET_EDGE && p_pcTrg->GetClassName().Contains(wxT("Continuous")))
	{
		return false;
	}

    //Modifier arc
    if(p_pcClass->GetName()==SP_DS_MODIFIER_EDGE)
    {
    	if(p_pcSrc->GetClassName().Contains(wxT("Transition")))
    	  return false;
    }

	return true;
}

bool SP_DS_HybridPN::CreateInitialElements(SP_DS_Graph* p_pcGraph)
{
	//create main table and plot

	SP_DS_ExtPT::CreateInitialElements(p_pcGraph);

	SP_DS_Metadataclass* l_pcMC;

	l_pcMC = p_pcGraph->GetMetadataclass(wxT("Simulation Properties"));
	l_pcMC->NewElement( 1 );

	l_pcMC = p_pcGraph->GetMetadataclass(wxT("Simulation Results"));
	l_pcMC->NewElement( 1 );

	return true;
}

bool
SP_DS_HybridPN::MetadataRequirement(SP_DS_Metadata* p_pcMetadata)
{
	if(!SP_DS_ExtPT::MetadataRequirement(p_pcMetadata))
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

