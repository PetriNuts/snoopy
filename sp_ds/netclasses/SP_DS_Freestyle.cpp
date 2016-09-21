//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: Ckruege2 $
// $Version: 0.0 $
// $Date: 2009/06/17
// Short Description:
//////////////////////////////////////////////////////////////////////

#include "sp_ds/netclasses/SP_DS_Freestyle.h"
#include "sp_ds/SP_DS_Edgeclass.h"
#include "sp_ds/SP_DS_Nodeclass.h"
#include "sp_ds/SP_DS_Graph.h"

#include "sp_ds/attributes/SP_DS_BoolAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"

#include "sp_gr/attributes/SP_GR_CommentAttribute.h"
#include "sp_gr/attributes/SP_GR_TextAttribute.h"

#include "sp_core/SP_GPR_Elements.h"

//simple graphik
#include "sp_gr/shapes/SP_GR_Actor.h"

// extended graphical representation
#include "sp_gr/shapes/SP_GR_ExtendedCircle.h"
#include "sp_gr/shapes/SP_GR_ExtendedRectangle.h"
#include "sp_gr/shapes/SP_GR_ExtendedEllipse.h"
#include "sp_gr/shapes/SP_GR_ExtendedRhombus.h"
#include "sp_gr/shapes/SP_GR_ExtendedParallelogram.h"
#include "sp_gr/shapes/SP_GR_ExtendedPaPLoop.h"
#include "sp_gr/shapes/SP_GR_ExtendedState.h"
#include "sp_gr/shapes/SP_GR_ExtendedAction.h"

#include "sp_gr/edges/SP_GR_ArrowEdge.h"
#include "sp_gr/edges/SP_GR_ExtendedEdge.h"

#include "sp_utilities.h"

#include "snoopy.h"

#include "sp_gui/widgets/dialogs/SP_WDG_DialogText.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogGraphic.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogMultiline.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogExtendedGraphic.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogBool.h"

SP_DS_Freestyle::SP_DS_Freestyle():SP_DS_SimpleGraph(SP_DS_FREESTYLE_CLASS)
{
    m_sExtension = wxT("spfreen");
	m_sFilter = wxT("*.") + m_sExtension;
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Circle"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Square"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Rectangle"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("State"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Activity"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Ellipse"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Rhombus"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Parallelogram"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Loop"));
}

SP_DS_Freestyle::SP_DS_Freestyle(const wxString& p_pchName):SP_DS_SimpleGraph(p_pchName)
{
    m_sExtension = wxT("spfreen");
	m_sFilter = wxT("*.") + m_sExtension;
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Circle"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Square"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Rectangle"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("State"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Activity"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Ellipse"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Rhombus"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Parallelogram"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Loop"));
}

SP_DS_Graph* SP_DS_Freestyle::CreateGraph(SP_DS_Graph* p_pcGraph)
{
    if (!SP_DS_SimpleGraph::CreateGraph(p_pcGraph))
        return NULL;
	
	SP_DS_Nodeclass* l_pcNS;
	SP_DS_Attribute* l_pcAttr;
    SP_GR_Node* l_pcGr;
    SP_Graphic* l_pcGrAttr;
	SP_DS_Edgeclass* l_pcEC;
 	
	///////////////////////////////////////////////////////////////////////////
	l_pcNS = p_pcGraph->RenameNodeclass(wxT("Node"), wxT("Circle"));
	l_pcNS->SetShortcut(wxT("C"));
    //name
	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_NameAttribute(wxT("Name"), wxT("")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General")));
    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
	l_pcGrAttr->SetOffsetX(25);
    l_pcGrAttr->SetOffsetY(20);
    l_pcGrAttr->SetShow(TRUE);
	//comment
	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_TextAttribute(wxT("Comment"), wxT("")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));

	//graphik
	int l_nHeight = wxGetApp().GetElementPrefs()->GetNodeHeight( GetName(), wxT("Circle") );
	l_pcGr = new SP_GR_ExtendedCircle(l_pcNS->GetPrototype(), l_nHeight, SP_EXTENDED_TYPE_DEFAULT, 1);
	l_pcGr->SetFixedSize(wxGetApp().GetElementPrefs()->GetNodeFixed(GetName(), l_pcNS->GetName()));
	l_pcNS->SetGraphic(l_pcGr);
	l_pcNS->RegisterGraphicWidget(new SP_WDG_DialogExtendedGraphic(wxT("Graphic")));
 
	//////////////////////////////////////////////////////////////////////////////
	l_pcNS = p_pcGraph->AddNodeclass(new SP_DS_Nodeclass(p_pcGraph, wxT("Square")));
	l_pcNS->SetShortcut(wxT("Q"));
	//name
	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_NameAttribute(wxT("Name"), wxT("")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General")));
    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
	l_pcGrAttr->SetOffsetX(25);
    l_pcGrAttr->SetOffsetY(20);
    l_pcGrAttr->SetShow(TRUE);
	//comment
	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_TextAttribute(wxT("Comment"), wxT("")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));

	//graphik
	l_nHeight = wxGetApp().GetElementPrefs()->GetNodeHeight( GetName(), wxT("Square") );
	int l_nWidth = wxGetApp().GetElementPrefs()->GetNodeWidth( GetName(), wxT("Square") );
	l_pcGr = new SP_GR_ExtendedRectangle(l_pcNS->GetPrototype(),l_nWidth , l_nHeight,TRUE, SP_EXTENDED_TYPE_DEFAULT,1);
	l_pcGr->SetFixedSize(wxGetApp().GetElementPrefs()->GetNodeFixed(GetName(), l_pcNS->GetName()));
	l_pcNS->SetGraphic(l_pcGr);
	l_pcNS->RegisterGraphicWidget(new SP_WDG_DialogExtendedGraphic(wxT("Graphic")));
	
	//////////////////////////////////////////////////////////////////////////////
	l_pcNS = p_pcGraph->AddNodeclass(new SP_DS_Nodeclass(p_pcGraph, wxT("Rectangle")));
	l_pcNS->SetShortcut(wxT("R"));
	//name
	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_NameAttribute(wxT("Name"), wxT("")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General")));
    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
	l_pcGrAttr->SetOffsetX(25);
    l_pcGrAttr->SetOffsetY(20);
    l_pcGrAttr->SetShow(TRUE);
	//comment
	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_TextAttribute(wxT("Comment"), wxT("")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));

	//graphik
	l_nHeight = wxGetApp().GetElementPrefs()->GetNodeHeight( GetName(), wxT("Rectangle") );
	l_nWidth = wxGetApp().GetElementPrefs()->GetNodeWidth( GetName(), wxT("Rectangle") );
	if(l_nWidth == DEFAULT_WIDTH) { l_nWidth=30; }
	l_pcGr = new SP_GR_ExtendedRectangle(l_pcNS->GetPrototype(),l_nWidth , l_nHeight,FALSE, SP_EXTENDED_TYPE_DEFAULT,1);
	l_pcGr->SetFixedSize(wxGetApp().GetElementPrefs()->GetNodeFixed(GetName(), l_pcNS->GetName()));
	l_pcNS->SetGraphic(l_pcGr);
	l_pcNS->RegisterGraphicWidget(new SP_WDG_DialogExtendedGraphic(wxT("Graphic")));
	//////////////////////////////////////////////////////////////////////////////////////////
	l_pcNS = p_pcGraph->AddNodeclass(new SP_DS_Nodeclass(p_pcGraph, wxT("State")));
	l_pcNS->SetShortcut(wxT("T"));
	//name
	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_NameAttribute(wxT("Name"), wxT("")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General")));
    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
	l_pcGrAttr->SetOffsetX(25);
    l_pcGrAttr->SetOffsetY(20);
    l_pcGrAttr->SetShow(TRUE);
	//comment
	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_TextAttribute(wxT("Comment"), wxT("")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));

	//graphik
	l_nHeight = wxGetApp().GetElementPrefs()->GetNodeHeight( GetName(), wxT("State") );
	l_nWidth = wxGetApp().GetElementPrefs()->GetNodeWidth( GetName(), wxT("State") );
	if(l_nWidth == DEFAULT_WIDTH) { l_nWidth=40; }
	if(l_nHeight == DEFAULT_WIDTH) { l_nHeight=30; }
	l_pcGr = new SP_GR_ExtendedState(l_pcNS->GetPrototype(),l_nWidth , l_nHeight, SP_EXTENDED_TYPE_DEFAULT,1);
	l_pcGr->SetFixedSize(wxGetApp().GetElementPrefs()->GetNodeFixed(GetName(), l_pcNS->GetName()));
	l_pcNS->SetGraphic(l_pcGr);
	l_pcNS->RegisterGraphicWidget(new SP_WDG_DialogExtendedGraphic(wxT("Graphic")));
	//////////////////////////////////////////////////////////////////////////////////////////
	l_pcNS = p_pcGraph->AddNodeclass(new SP_DS_Nodeclass(p_pcGraph, wxT("Activity")));
	l_pcNS->SetShortcut(wxT("A"));
	//name
	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_NameAttribute(wxT("Name"), wxT("")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General")));
    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
	l_pcGrAttr->SetOffsetX(25);
    l_pcGrAttr->SetOffsetY(20);
    l_pcGrAttr->SetShow(TRUE);
	//comment
	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_TextAttribute(wxT("Comment"), wxT("")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));

	//graphik
	l_nHeight = wxGetApp().GetElementPrefs()->GetNodeHeight( GetName(), wxT("Activity") );
	l_nWidth = wxGetApp().GetElementPrefs()->GetNodeWidth( GetName(), wxT("Activity") );
	if(l_nWidth == DEFAULT_WIDTH) { l_nWidth=40; }
	if(l_nHeight == DEFAULT_WIDTH) { l_nHeight=30; }
	l_pcGr = new SP_GR_ExtendedAction(l_pcNS->GetPrototype(),l_nWidth , l_nHeight, SP_EXTENDED_TYPE_DEFAULT,1);
	l_pcGr->SetFixedSize(wxGetApp().GetElementPrefs()->GetNodeFixed(GetName(), l_pcNS->GetName()));
	l_pcNS->SetGraphic(l_pcGr);
	l_pcNS->RegisterGraphicWidget(new SP_WDG_DialogExtendedGraphic(wxT("Graphic")));

	//////////////////////////////////////////////////////////////////////////////
	l_pcNS = p_pcGraph->AddNodeclass(new SP_DS_Nodeclass(p_pcGraph, wxT("Ellipse")));
	l_pcNS->SetShortcut(wxT("I"));
	//name
	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_NameAttribute(wxT("Name"), wxT("")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General")));
    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
	l_pcGrAttr->SetOffsetX(25);
    l_pcGrAttr->SetOffsetY(20);
    l_pcGrAttr->SetShow(TRUE);
	//comment
	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_TextAttribute(wxT("Comment"), wxT("")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));

	//graphik
	l_nWidth = wxGetApp().GetElementPrefs()->GetNodeWidth( GetName(), wxT("Ellipse") );
	l_nHeight = wxGetApp().GetElementPrefs()->GetNodeHeight( GetName(), wxT("Ellipse") );
	if(l_nWidth == DEFAULT_WIDTH) { l_nWidth=40; }
	if(l_nHeight == DEFAULT_WIDTH) { l_nHeight=30; }
	l_pcGr = new SP_GR_ExtendedEllipse(l_pcNS->GetPrototype(), l_nWidth,l_nHeight,FALSE, SP_EXTENDED_TYPE_DEFAULT,1 );
	l_pcGr->SetFixedSize(wxGetApp().GetElementPrefs()->GetNodeFixed(GetName(), l_pcNS->GetName()));
	l_pcNS->SetGraphic(l_pcGr);
	l_pcNS->RegisterGraphicWidget(new SP_WDG_DialogExtendedGraphic(wxT("Graphic")));



	//////////////////////////////////////////////////////////////////////////////
	l_pcNS = p_pcGraph->AddNodeclass(new SP_DS_Nodeclass(p_pcGraph, wxT("Rhombus")));
    l_pcNS->SetShortcut(wxT("H"));
	//name
	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_NameAttribute(wxT("Name"), wxT("")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General")));
    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
	l_pcGrAttr->SetOffsetX(25);
    l_pcGrAttr->SetOffsetY(20);
    l_pcGrAttr->SetShow(TRUE);
	//comment
	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_TextAttribute(wxT("Comment"), wxT("")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));

	//graphik
	l_nWidth = wxGetApp().GetElementPrefs()->GetNodeWidth( GetName(), wxT("Rhombus") );
	l_nHeight = wxGetApp().GetElementPrefs()->GetNodeHeight( GetName(), wxT("Rhombus") );
	if(l_nWidth == DEFAULT_WIDTH) { l_nWidth=40; }
	if(l_nHeight == DEFAULT_WIDTH) { l_nHeight=30; }
	l_pcGr = new SP_GR_ExtendedRhombus(l_pcNS->GetPrototype(), l_nWidth,l_nHeight, SP_EXTENDED_TYPE_DEFAULT,1);
	l_pcGr->SetFixedSize(wxGetApp().GetElementPrefs()->GetNodeFixed(GetName(), l_pcNS->GetName()));
	l_pcNS->SetGraphic(l_pcGr);
	l_pcNS->RegisterGraphicWidget(new SP_WDG_DialogExtendedGraphic(wxT("Graphic")));

	//////////////////////////////////////////////////////////////////////////////
	l_pcNS = p_pcGraph->AddNodeclass(new SP_DS_Nodeclass(p_pcGraph, wxT("Parallelogram")));
    l_pcNS->SetShortcut(wxT("P"));
	//name
	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_NameAttribute(wxT("Name"), wxT("")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General")));
    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
	l_pcGrAttr->SetOffsetX(25);
    l_pcGrAttr->SetOffsetY(20);
    l_pcGrAttr->SetShow(TRUE);
	//comment
	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_TextAttribute(wxT("Comment"), wxT("")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));

	//graphik
	l_nWidth = wxGetApp().GetElementPrefs()->GetNodeWidth( GetName(), wxT("Parallelogram") );
	l_nHeight = wxGetApp().GetElementPrefs()->GetNodeHeight( GetName(), wxT("Parallelogram") );
	if(l_nWidth == DEFAULT_WIDTH) { l_nWidth=30; }
	if(l_nHeight == DEFAULT_WIDTH) { l_nHeight=30; }
	l_pcGr = new SP_GR_ExtendedParallelogram(l_pcNS->GetPrototype(), l_nWidth,l_nHeight, SP_EXTENDED_TYPE_DEFAULT,1);
	l_pcGr->SetFixedSize(wxGetApp().GetElementPrefs()->GetNodeFixed(GetName(), l_pcNS->GetName()));
	l_pcNS->SetGraphic(l_pcGr);
	l_pcNS->RegisterGraphicWidget(new SP_WDG_DialogExtendedGraphic(wxT("Graphic")));

	//////////////////////////////////////////////////////////////////////////////
	l_pcNS = p_pcGraph->AddNodeclass(new SP_DS_Nodeclass(p_pcGraph, wxT("Loop")));
    l_pcNS->SetShortcut(wxT("L"));
	//name
	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_NameAttribute(wxT("Name"), wxT("")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General")));
    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
	l_pcGrAttr->SetOffsetX(25);
    l_pcGrAttr->SetOffsetY(20);
    l_pcGrAttr->SetShow(TRUE);
	//comment
	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_TextAttribute(wxT("Content"), wxT("")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
	//rotate
	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_BoolAttribute(wxT("Rotate"),FALSE));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogBool(wxT("General")));
	//graphik
	l_nWidth = wxGetApp().GetElementPrefs()->GetNodeWidth( GetName(), wxT("Loop") );
	l_nHeight = wxGetApp().GetElementPrefs()->GetNodeHeight( GetName(), wxT("Loop") );
	if(l_nWidth == DEFAULT_WIDTH) { l_nWidth=30; }
	if(l_nHeight == DEFAULT_WIDTH) { l_nHeight=30; }
	l_pcGr = new SP_GR_ExtendedPaPLoop(l_pcNS->GetPrototype(), l_nWidth,l_nHeight, SP_EXTENDED_TYPE_DEFAULT,1);
	l_pcGr->SetFixedSize(wxGetApp().GetElementPrefs()->GetNodeFixed(GetName(), l_pcNS->GetName()));
	l_pcNS->SetGraphic(l_pcGr);
	l_pcNS->RegisterGraphicWidget(new SP_WDG_DialogExtendedGraphic(wxT("Graphic")));

	//////////////////////////////////////////////////////////////////////////////
	l_pcNS = p_pcGraph->AddNodeclass(new SP_DS_Nodeclass(p_pcGraph, wxT("Actor")));
    l_pcNS->SetShortcut(wxT("G"));
	//name
	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_NameAttribute(wxT("Name"), wxT("Actor")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General")));
    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
	l_pcGrAttr->SetOffsetX(25);
    l_pcGrAttr->SetOffsetY(20);
    l_pcGrAttr->SetShow(TRUE);
	//comment
	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_TextAttribute(wxT("Content"), wxT("")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
	//graphic
	l_pcGr = new SP_GR_Actor(l_pcNS->GetPrototype());
	l_pcGr->SetFixedSize(wxGetApp().GetElementPrefs()->GetNodeFixed(GetName(), l_pcNS->GetName()));
	l_pcNS->SetGraphic(l_pcGr);
	l_pcNS->RegisterGraphicWidget(new SP_WDG_DialogGraphic(wxT("Graphic")));

	//////////////////////////////////////////////////////////////////////////////
	l_pcNS = p_pcGraph->AddNodeclass(new SP_DS_Nodeclass(p_pcGraph, wxT("And")));
    l_pcNS->SetShortcut(wxT("U"));
	//name
	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_NameAttribute(wxT("Name"), wxT("")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General")));
    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
	l_pcGrAttr->SetOffsetX(25);
    l_pcGrAttr->SetOffsetY(20);
    l_pcGrAttr->SetShow(TRUE);
	//comment
	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_TextAttribute(wxT("Content"), wxT("")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));

	//graphic
	l_pcGr = new SP_GR_ExtendedRectangle(l_pcNS->GetPrototype(),2 , 30,FALSE, SP_EXTENDED_TYPE_DEFAULT,7);
	l_pcGr->SetFixedSize(wxGetApp().GetElementPrefs()->GetNodeFixed(GetName(), l_pcNS->GetName()));
	l_pcNS->SetGraphic(l_pcGr);
	l_pcNS->RegisterGraphicWidget(new SP_WDG_DialogExtendedGraphic(wxT("Graphic")));

	
	///////////////////////////////////////////////////////////////////////////////
	l_pcEC = p_pcGraph->RenameEdgeclass(wxT("Edge"), wxT("Edge"));
	SP_EXTENDED_TYPE_EDGE l_nType    = wxGetApp().GetElementPrefs()->GetLineEdgeDesignType(GetName() );
	SP_EXTENDED_ARROW_TYPE_EDGE l_nATypeR  = wxGetApp().GetElementPrefs()->GetArrowEdgeTypeR(GetName() );
	SP_EXTENDED_ARROW_TYPE_EDGE l_nATypeL  = wxGetApp().GetElementPrefs()->GetArrowEdgeTypeL(GetName());
	size_t l_nTSize=1; size_t l_nATSizeL=7; size_t l_nATSizeR=7;
	l_pcEC->SetGraphic( new SP_GR_ExtendedEdge( l_pcEC->GetPrototype(),l_nType, l_nTSize ,l_nATypeR,l_nATSizeL,l_nATypeL,l_nATSizeR) );
	l_pcEC->SetShortcut(wxT("E"));
	
	l_pcAttr = l_pcEC->AddAttribute(new SP_DS_NameAttribute(wxT("Name")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General")));
    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
    l_pcGrAttr->SetShow(TRUE);
	//comment
    l_pcAttr = l_pcEC->AddAttribute(new SP_DS_TextAttribute(wxT("Comment"),wxT("")));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
	l_pcGrAttr  = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
	//spline
	bool l_bSpline = wxGetApp().GetElementPrefs()->GetEdgeSpline(GetName());
		
	l_pcAttr = l_pcEC->AddAttribute(new SP_DS_BoolAttribute(wxT("Spline"),l_bSpline));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogBool(wxT("General")));
	l_pcAttr->SetGlobalShow();
	l_pcEC->RegisterGraphicWidget(new SP_WDG_DialogExtendedGraphic(wxT("Graphic")));

    return p_pcGraph;
}

bool SP_DS_Freestyle::NodeRequirement(SP_DS_Node* p_pcNode)
{
	return TRUE;
}

bool SP_DS_Freestyle::EdgeRequirement(SP_DS_Edgeclass* p_pcClass, SP_Data* p_pcSrc, SP_Data* p_pcTrg)
{
	if (!SP_DS_SimpleGraph::EdgeRequirement(p_pcClass, p_pcSrc, p_pcTrg))
		return FALSE;

	if ( p_pcSrc ==  p_pcTrg )
	{
		if(!p_pcClass->GetStartShape()->GetCanvas()->GetControlPoints())
		{
			SP_MESSAGEBOX(wxT("You need more Control Points in the Edge,\n when you will take the same Source and Target"),wxT("Error"), wxOK | wxICON_ERROR);
			return FALSE;
		}
	}
	return(TRUE);
}
