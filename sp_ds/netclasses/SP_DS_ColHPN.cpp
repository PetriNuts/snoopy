//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: F. Liu $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2009/11/1 11:55:00 $
// Short Description: colored Stochastic Petri nets
//////////////////////////////////////////////////////////////////////

#include "sp_ds/netclasses/SP_DS_ColHPN.h"

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

#include "sp_gr/attributes/SP_GR_ContinuousMarkAttribute.h"

//Special for colored SPN
#include "sp_gui/dialogs/dia_ColSPN/SP_WDG_ColStMarkingList.h"
#include "sp_gui/dialogs/dia_ColSPN/SP_WDG_ColStFunctionList.h"
#include "sp_gui/dialogs/dia_CPN/SP_WDG_ArcInscription.h"
#include "sp_gui/dialogs/dia_CPN/SP_WDG_Guard.h"

#include "sp_ds/animators/ani_ColSPN/SP_DS_ColStCoarseTransAnimator.h"
#include "sp_ds/animators/ani_ColSPN/SP_DS_ColStAnimation.h"

#include "sp_ds/attributes/SP_DS_TypeAttribute.h"//by george, for constants harmonizing 

SP_DS_ColHPN::SP_DS_ColHPN()
:SP_DS_HybridPN( SP_DS_COLHPN_CLASS )
{
	m_sExtension = wxT("colhpn");
	m_sFilter = wxT("*.colhpn;*.colhybpn");

	//register node classes for editing their graphic properties
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Discrete Place"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Continuous Place"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Discrete Transition"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Continuous Transition"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), SP_DS_PARAM);

}


SP_DS_ColHPN::SP_DS_ColHPN( const wxString& p_pchName )
:SP_DS_HybridPN( p_pchName )
{
	m_sExtension = wxT("colhpn");
	m_sFilter = wxT("*.colhpn;*.colhybpn");

	//register node classes for editing their graphic properties
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Discrete Place"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Continuous Place"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Discrete Transition"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Continuous Transition"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), SP_DS_PARAM);
	wxGetApp().GetElementPrefs()->RegisterNodeclass( GetName(), wxT("Immediate Transition") );
	wxGetApp().GetElementPrefs()->RegisterNodeclass( GetName(), wxT("Deterministic Transition") );
	wxGetApp().GetElementPrefs()->RegisterNodeclass( GetName(), wxT("Scheduled Transition") );

}


SP_DS_Graph*
SP_DS_ColHPN::CreateGraph( SP_DS_Graph* p_pcGraph, SP_MapString2Int p_mapAttribute2Value )
{
	if ( ! SP_DS_HybridPN::CreateGraph( p_pcGraph ) ) 
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

	//===============================================================================================================
   /*
    * Discrete Place
    */

	l_pcNC = p_pcGraph->GetNodeclass( SP_DS_DISCRETE_PLACE );

	for(auto itMap=p_mapAttribute2Value.begin();itMap!= p_mapAttribute2Value.end();++itMap)
		{
			wxString l_sNetClass = (itMap)->first.BeforeFirst('|');
			wxString l_sGraphicAttribute= (itMap)->first.AfterLast(wxChar('|'));
			wxString l_sNodeType = (itMap)->first.AfterFirst(wxChar('|'));
			l_sNodeType = l_sNodeType.BeforeFirst(wxT('|'));

			//SP_LOGMESSAGE(l_sNodeType);

			if(l_sNetClass.IsSameAs(SP_DS_COLHPN_CLASS) && l_sNodeType.IsSameAs(wxT("Discrete Place")))
			 {
			   if(!l_pcNC) break;

			   SP_ListGraphic* pc_ListAtt= l_pcNC->GetPrototype()->GetAttribute(wxT("Name"))->GetGraphics();

			  if(!pc_ListAtt) break;

				for(auto it=pc_ListAtt->begin();it!=pc_ListAtt->end();++it)
				{
					if(l_sGraphicAttribute.IsSameAs(wxT("NameAttPosX")))
					{
						(*it)->SetOffsetX((itMap)->second);


					}
					else if(l_sGraphicAttribute.IsSameAs(wxT("NameAttPosY")))
					{
						(*it)->SetOffsetY((itMap)->second);

					}

				}
				}
		}

	////////////////////////////////////////////

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

	//////////////////////////////////////////////////////////////////////////////
	// Colorset name
	l_pcAttr = l_pcNC->AddAttribute(new SP_DS_TextAttribute(SP_DS_CPN_COLORSETNAME, wxT("Dot")));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogChoice(wxT("Markings")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
	l_pcGrAttr->SetTextColor( wxGetApp().GetElementPrefs()->GetColorSetColor(GetName(), l_pcNC->GetName()));
	l_pcGrAttr->SetOffsetY(-20);
	l_pcGrAttr->SetShow(TRUE);
	l_pcGrAttr->SetBrush(wxTheBrushList->FindOrCreateBrush(*wxBLUE));
	l_pcAttr->SetGlobalShow();	


	//==============================================================================================================
  	/*
     * Continuous Place
     */

	l_pcNC = p_pcGraph->GetNodeclass( SP_DS_CONTINUOUS_PLACE );


	for(auto itMap=p_mapAttribute2Value.begin();itMap!= p_mapAttribute2Value.end();++itMap)
		{
			wxString l_sNetClass = (itMap)->first.BeforeFirst('|');
			wxString l_sGraphicAttribute= (itMap)->first.AfterLast(wxChar('|'));
			wxString l_sNodeType = (itMap)->first.AfterFirst(wxChar('|'));
			l_sNodeType = l_sNodeType.BeforeFirst(wxT('|'));

			//SP_LOGMESSAGE(l_sNodeType);

			if(l_sNetClass.IsSameAs(SP_DS_COLHPN_CLASS) && l_sNodeType.IsSameAs(wxT("Continuous Place")))
			 {
			   if(!l_pcNC) break;

			   SP_ListGraphic* pc_ListAtt= l_pcNC->GetPrototype()->GetAttribute(wxT("Name"))->GetGraphics();

			  if(!pc_ListAtt) break;

				for(auto it=pc_ListAtt->begin();it!=pc_ListAtt->end();++it)
				{
					if(l_sGraphicAttribute.IsSameAs(wxT("NameAttPosX")))
					{
						(*it)->SetOffsetX((itMap)->second);

					}
					else if(l_sGraphicAttribute.IsSameAs(wxT("NameAttPosY")))
					{
						(*it)->SetOffsetY((itMap)->second);

					}

				}
				}
		}


	////////////

	l_pcAttr = l_pcNC->GetPrototype()->GetAttribute( wxT("Marking") );
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogShowOnly(wxT("Markings")));
	

	l_pcAttr = l_pcNC->AddAttribute( new SP_DS_ColListAttribute( SP_DS_CPN_MARKINGLIST, SP_COLLIST_DOUBLE, 3 ) );
	l_pcAttr->SetDisplayName(wxT("Marking set"));
	l_pcAttr->RegisterDialogWidget( new SP_WDG_ColStMarkingList( wxT("Markings") ) );
	l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >( l_pcAttr );
	l_pcColList->SetColLabel( 0, wxT("Main: Color(s)") );
	l_pcColList->SetColLabel( 1, wxT("Main: Marking") );
	l_pcColList->SetColLabel( 2, wxT("Product Color"));//by george
	l_pcColList->Clear();	
	l_nNewRow = l_pcColList->AppendEmptyRow();
	l_pcColList->SetCell(l_nNewRow, 0, wxT("all()"));
	l_pcColList->SetCell(l_nNewRow, 1, wxT("0"));
	l_pcColList->SetCell(l_nNewRow, 2, wxT("0"));//by george

	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_ColListAttribute( l_pcAttr, wxT("%") ));
	l_pcGrAttr->SetTextColor( wxGetApp().GetElementPrefs()->GetMarkingColor(GetName(), l_pcNC->GetDisplayName()));
	l_pcGrAttr->SetOffsetX(45);
	l_pcGrAttr->SetOffsetY(-25);  
	l_pcGrAttr->SetShow(TRUE);
	l_pcAttr->SetGlobalShow();

	//////////////////////////////////////////////////////////////////////////////
	// Colorset name
	l_pcAttr = l_pcNC->AddAttribute(new SP_DS_TextAttribute(SP_DS_CPN_COLORSETNAME, wxT("Dot")));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogChoice(wxT("Markings")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
	l_pcGrAttr->SetTextColor( wxGetApp().GetElementPrefs()->GetColorSetColor(GetName(), l_pcNC->GetName()));
	l_pcGrAttr->SetOffsetY(-20);
	l_pcGrAttr->SetShow(TRUE);
	l_pcGrAttr->SetBrush(wxTheBrushList->FindOrCreateBrush(*wxBLUE));
	l_pcAttr->SetGlobalShow();	


	//==============================================================================================================
  	/*
  	 * Transition
     */

	l_pcNC = p_pcGraph->GetNodeclass( wxT("Transition") );


	//////////////////////////////
	for(auto itMap=p_mapAttribute2Value.begin();itMap!= p_mapAttribute2Value.end();++itMap)
		{
			wxString l_sNetClass = (itMap)->first.BeforeFirst('|');
			wxString l_sGraphicAttribute= (itMap)->first.AfterLast(wxChar('|'));
			wxString l_sNodeType = (itMap)->first.AfterFirst(wxChar('|'));
			l_sNodeType = l_sNodeType.BeforeFirst(wxT('|'));

			//SP_LOGMESSAGE(l_sNodeType);

			if(l_sNetClass.IsSameAs(SP_DS_COLHPN_CLASS) && l_sNodeType.IsSameAs(wxT("Discrete Transition")))
			 {
			   if(!l_pcNC) break;

			   SP_ListGraphic* pc_ListAtt= l_pcNC->GetPrototype()->GetAttribute(wxT("Name"))->GetGraphics();

			  if(!pc_ListAtt) break;

				for(auto it=pc_ListAtt->begin();it!=pc_ListAtt->end();++it)
				{
					if(l_sGraphicAttribute.IsSameAs(wxT("NameAttPosX")))
					{
						(*it)->SetOffsetX((itMap)->second);


					}
					else if(l_sGraphicAttribute.IsSameAs(wxT("NameAttPosY")))
					{
						(*it)->SetOffsetY((itMap)->second);

					}

				}
				}
		}



	/////////////////////////////////////////////

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
	l_pcGrAttr->SetTextColor( wxGetApp().GetElementPrefs()->GetGuardColor(GetName(), l_pcNC->GetName()));
	l_pcGrAttr->SetOffsetX(45);
	l_pcGrAttr->SetOffsetY(-25);
	l_pcGrAttr->SetShow(TRUE);
	l_pcAttr->SetGlobalShow();


	//==========================================================================================================
  	/*
  	 * Continuous Transition
  	 */

	l_pcNC = p_pcGraph->GetNodeclass( SP_DS_CONTINUOUS_TRANS );	

	for(auto itMap=p_mapAttribute2Value.begin();itMap!= p_mapAttribute2Value.end();++itMap)
		{
			wxString l_sNetClass = (itMap)->first.BeforeFirst('|');
			wxString l_sGraphicAttribute= (itMap)->first.AfterLast(wxChar('|'));
			wxString l_sNodeType = (itMap)->first.AfterFirst(wxChar('|'));
			l_sNodeType = l_sNodeType.BeforeFirst(wxT('|'));

			//SP_LOGMESSAGE(l_sNodeType);

			if(l_sNetClass.IsSameAs(SP_DS_COLHPN_CLASS) && l_sNodeType.IsSameAs(wxT("Continuous Transition")))
			 {
			   if(!l_pcNC) break;

			   SP_ListGraphic* pc_ListAtt= l_pcNC->GetPrototype()->GetAttribute(wxT("Name"))->GetGraphics();

			  if(!pc_ListAtt) break;

				for(auto it=pc_ListAtt->begin();it!=pc_ListAtt->end();++it)
				{
					if(l_sGraphicAttribute.IsSameAs(wxT("NameAttPosX")))
					{
						(*it)->SetOffsetX((itMap)->second);


					}
					else if(l_sGraphicAttribute.IsSameAs(wxT("NameAttPosY")))
					{
						(*it)->SetOffsetY((itMap)->second);


					}

				}
				}
		}
	/////////////////////////////

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
	l_pcGrAttr->SetTextColor( wxGetApp().GetElementPrefs()->GetGuardColor(GetName(), l_pcNC->GetName()));
	l_pcGrAttr->SetOffsetX(45);
	l_pcGrAttr->SetOffsetY(-25);
	l_pcGrAttr->SetShow(TRUE);
	l_pcAttr->SetGlobalShow();


	//===========================================================================================================
  	/*
  	 *Immediate Transition
  	 */
	
	l_pcNC = p_pcGraph->GetNodeclass( wxT("Immediate Transition") );
	

	for(auto itMap=p_mapAttribute2Value.begin();itMap!= p_mapAttribute2Value.end();++itMap)
		{
			wxString l_sNetClass = (itMap)->first.BeforeFirst('|');
			wxString l_sGraphicAttribute= (itMap)->first.AfterLast(wxChar('|'));
			wxString l_sNodeType = (itMap)->first.AfterFirst(wxChar('|'));
			l_sNodeType = l_sNodeType.BeforeFirst(wxT('|'));

			//SP_LOGMESSAGE(l_sNodeType);

			if(l_sNetClass.IsSameAs(SP_DS_COLHPN_CLASS) && l_sNodeType.IsSameAs(wxT("Immediate Transition")))
			 {
			   if(!l_pcNC) break;

			   SP_ListGraphic* pc_ListAtt= l_pcNC->GetPrototype()->GetAttribute(wxT("Name"))->GetGraphics();

			  if(!pc_ListAtt) break;

				for(auto it=pc_ListAtt->begin();it!=pc_ListAtt->end();++it)
				{
					if(l_sGraphicAttribute.IsSameAs(wxT("NameAttPosX")))
					{
						(*it)->SetOffsetX((itMap)->second);

					}
					else if(l_sGraphicAttribute.IsSameAs(wxT("NameAttPosY")))
					{
						(*it)->SetOffsetY((itMap)->second);

					}

				}
				}
		}
	/////////////////////////////
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
	l_pcGrAttr->SetTextColor( wxGetApp().GetElementPrefs()->GetGuardColor(GetName(), l_pcNC->GetName()));
	l_pcGrAttr->SetOffsetX(45);
	l_pcGrAttr->SetOffsetY(-25);
	l_pcGrAttr->SetShow(TRUE);
	l_pcAttr->SetGlobalShow();


	 //==================================================================================================================
  	/*
  	 *Deterministic Transition
  	 */
	
	l_pcNC = p_pcGraph->GetNodeclass( wxT("Deterministic Transition") );



	for(auto itMap=p_mapAttribute2Value.begin();itMap!= p_mapAttribute2Value.end();++itMap)
		{
			wxString l_sNetClass = (itMap)->first.BeforeFirst('|');
			wxString l_sGraphicAttribute= (itMap)->first.AfterLast(wxChar('|'));
			wxString l_sNodeType = (itMap)->first.AfterFirst(wxChar('|'));
			l_sNodeType = l_sNodeType.BeforeFirst(wxT('|'));

			//SP_LOGMESSAGE(l_sNodeType);

			if(l_sNetClass.IsSameAs(SP_DS_COLHPN_CLASS) && l_sNodeType.IsSameAs(wxT("Deterministic Transition")))
			 {
			   if(!l_pcNC) break;

			   SP_ListGraphic* pc_ListAtt= l_pcNC->GetPrototype()->GetAttribute(wxT("Name"))->GetGraphics();

			  if(!pc_ListAtt) break;

				for(auto it=pc_ListAtt->begin();it!=pc_ListAtt->end();++it)
				{
					if(l_sGraphicAttribute.IsSameAs(wxT("NameAttPosX")))
					{
						(*it)->SetOffsetX((itMap)->second);


					}
					else if(l_sGraphicAttribute.IsSameAs(wxT("NameAttPosY")))
					{
						(*it)->SetOffsetY((itMap)->second);


					}

				}
				}
		}
	/////////////////////////////

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
	l_pcGrAttr->SetTextColor( wxGetApp().GetElementPrefs()->GetGuardColor(GetName(), l_pcNC->GetName()));
	l_pcGrAttr->SetOffsetX(45);
	l_pcGrAttr->SetOffsetY(-25);
	l_pcGrAttr->SetShow(TRUE);
	l_pcAttr->SetGlobalShow();


	 //==================================================================================================================
  	 /*
  	  *Scheduled Transition
  	  */

	l_pcNC = p_pcGraph->GetNodeclass( wxT("Scheduled Transition") );
	

	for(auto itMap=p_mapAttribute2Value.begin();itMap!= p_mapAttribute2Value.end();++itMap)
		{
			wxString l_sNetClass = (itMap)->first.BeforeFirst('|');
			wxString l_sGraphicAttribute= (itMap)->first.AfterLast(wxChar('|'));
			wxString l_sNodeType = (itMap)->first.AfterFirst(wxChar('|'));
			l_sNodeType = l_sNodeType.BeforeFirst(wxT('|'));

			//SP_LOGMESSAGE(l_sNodeType);

			if(l_sNetClass.IsSameAs(SP_DS_COLHPN_CLASS) && l_sNodeType.IsSameAs(wxT("Scheduled Transition")))
			 {
			   if(!l_pcNC) break;

			   SP_ListGraphic* pc_ListAtt= l_pcNC->GetPrototype()->GetAttribute(wxT("Name"))->GetGraphics();

			  if(!pc_ListAtt) break;

				for(auto it=pc_ListAtt->begin();it!=pc_ListAtt->end();++it)
				{
					if(l_sGraphicAttribute.IsSameAs(wxT("NameAttPosX")))
					{
						(*it)->SetOffsetX((itMap)->second);


					}
					else if(l_sGraphicAttribute.IsSameAs(wxT("NameAttPosY")))
					{
						(*it)->SetOffsetY((itMap)->second);


					}

				}
				}
		}
	/////////////////////////////

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
	l_pcGrAttr->SetTextColor( wxGetApp().GetElementPrefs()->GetGuardColor(GetName(), l_pcNC->GetName()));
	l_pcGrAttr->SetOffsetX(45);
	l_pcGrAttr->SetOffsetY(-25);
	l_pcGrAttr->SetShow(TRUE);
	l_pcAttr->SetGlobalShow();

	
	//////////////////////////////////////////////////////////////////////////////
	//for constants harmonizing, we cannot delete param node, because wee need them to load the constants value after that we can remove them
	l_pcNC = p_pcGraph->AddNodeclass( new SP_DS_Nodeclass( p_pcGraph, wxT("Parameter") ) );
	//l_pcNC->SetShortcut( wxT("K") );disable the shortcut for the parameter nodes
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
	l_pcNC->SetShowInElementTree(false);//by george, do not display the parameter nodes in the elements tree of colhpn net class
	//////////////////////////////////////////////////////////////////////////////
	//for constants harmonizing, we cannot delete coars param node, because wee need them to load the constants value after that we can remove them
	l_pcNC = p_pcGraph->AddNodeclass(new SP_DS_Nodeclass(p_pcGraph, wxT("Coarse Parameter") ));
	//l_pcNC->SetShortcut(wxT("Shift+K"));//disable the shortcut for the parameter nodes
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
	l_pcNC->SetShowInElementTree(false);//by george, do not display the parameter nodes in the elements tree of colhpn net class
	
	//////////////////////////////////////////////////////////////////////////////
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
	//if(l_pcAttr->Remove()) wxDELETE(l_pcAttr);//commented by  george for constants harmonizing

	l_pcAttr = l_pcMC->GetPrototype()->GetAttribute(wxT("Group"));
	//if(l_pcAttr->Remove()) wxDELETE(l_pcAttr);//commented by  george for constants harmonizing

	l_pcAttr = l_pcMC->GetPrototype()->GetAttribute(wxT("Type"));
	//if(l_pcAttr->Remove()) wxDELETE(l_pcAttr);//commented by  george for constants harmonizing



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

	//by george, observer types for colored HPN
	l_pcMC = p_pcGraph->GetMetadataclass(SP_DS_META_OBSERVER);

	l_pcAttr = l_pcMC->AddAttribute(new SP_DS_TypeAttribute(wxT("ColPNType"), wxT("Col|Place")));
	l_pcType->AddPossibleValue(wxT("Col|Transition"));
	l_pcType->AddPossibleValue(wxT("Place instance"));
	l_pcType->AddPossibleValue(wxT("Transition instance"));
	l_pcType->AddPossibleValue(wxT("Mixed"));

	/////////////////////////////////////////////////////////////////////////////


	return p_pcGraph;

}

bool
SP_DS_ColHPN::CreateInitialElements(SP_DS_Graph* p_pcGraph)
{
	SP_DS_HybridPN::CreateInitialElements(p_pcGraph);

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
SP_DS_ColHPN::NodeRequirement( SP_DS_Node* p_pcNode )
{
    return SP_DS_HybridPN::NodeRequirement( p_pcNode );
}

bool
SP_DS_ColHPN::EdgeRequirement( SP_DS_Edgeclass* p_pcClass, SP_Data* p_pcSrc, SP_Data* p_pcTrg )
{
	return SP_DS_HybridPN::EdgeRequirement(p_pcClass, p_pcSrc, p_pcTrg);
}

bool
SP_DS_ColHPN::MetadataRequirement( SP_DS_Metadata* p_pcMetadata )
{
    if(!SP_DS_HybridPN::MetadataRequirement( p_pcMetadata ))
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
