//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2010/08/01 10:20:00 $
// Short Description: generate twin nets class
//////////////////////////////////////////////////////////////////////

#include "sp_ds/extensions/Col_PN/Folding/SP_CPN_GenTwinNet.h"
#include <algorithm>
#include "sp_core/SP_Core.h"

#include <wx/tokenzr.h>
#include <wx/filefn.h>
#include <wx/filedlg.h>
#include <wx/file.h>
#include <wx/textfile.h>
#include <iostream>


#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/SP_DS_Metadataclass.h"
#include "sp_ds/SP_DS_Netclass.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"
#include "sp_ds/attributes/SP_DS_IdAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"

#include "sp_gui/dialogs/dia_CPN/SP_WDG_DeclarationTreectrl.h"
#include "sp_ds/extensions/SP_DS_Coarse.h"

#include "sp_gui/dialogs/dia_CPN/SP_DLG_TwinNetSetting.h"

#include "sp_gr/SP_GR_Edge.h"
#include "sp_gr/attributes/SP_GR_ColListAttribute.h"



SP_CPN_GenTwinNet::SP_CPN_GenTwinNet()
{
	m_sColorSetName = wxT("CS");
	m_sVariableName= wxT("x");

	m_sColors = wxT("a-b");

	m_sFusionPlaceName = wxT("Fusion");
	m_sScheduleTransitionName = wxT("Schedule");
	m_sCoarsePlaceName = wxT("Exchange");

	m_sParameterName = wxT("k");

	m_dOffValue = 100.0;
	
}

SP_CPN_GenTwinNet::~SP_CPN_GenTwinNet()
{	
}


bool SP_CPN_GenTwinNet::GenerateTwinNet(SP_DS_Graph* p_pcGraph,SP_GUI_Canvas* p_pcCanvas,SP_MDI_Doc* p_pcDoc, SP_MDI_View* p_pcView )
{

	m_pcGraph = p_pcGraph;
	if(!m_pcGraph)
		return false;
	m_pcCanvas = p_pcCanvas;
	if(!m_pcCanvas)
		return false;
	m_pcDoc = p_pcDoc;
	if(!m_pcDoc)
		return false;
	m_pcView = p_pcView;
	if(!m_pcView)
		return false;

	if(m_pcGraph->GetNodeclass(wxT("Place"))->GetElements()->size() ==  0 && m_pcGraph->GetNodeclass(wxT("Transition"))->GetElements()->size() ==  0)
	{
		SP_MESSAGEBOX(wxT("Please open or draw a Petri Net"), wxT("Generate twin nets"), wxOK | wxICON_ERROR);
		return false;
	}



	//////////////////////////////////////////////////////////////
	SP_ListNode::const_iterator l_Iter;	
	SP_DS_Nodeclass* l_pcNewNC = m_pcGraph->GetNodeclass(wxT("Place"));
	if( !l_pcNewNC)
		return false;

	const SP_ListNode* l_plElements0 = l_pcNewNC->GetElements();
	wxString l_sExistingPlaceName = wxT("####");
	for ( l_Iter = l_plElements0->begin(); l_Iter != l_plElements0->end(); ++l_Iter )
	{		
		SP_DS_Node* l_pcOldNode = *l_Iter;		

		if ( l_sExistingPlaceName == l_pcOldNode->GetAttribute(wxT("Name"))->GetValueString() )
		{
			SP_MESSAGEBOX(wxT("Two places with empty or identical names are not allowed!"), wxT("Notification"), wxOK | wxICON_ERROR );
			return false;
		}
		l_sExistingPlaceName = l_pcOldNode->GetAttribute(wxT("Name"))->GetValueString();
	}


	/////////////////////////////////////////////////////////////


	//Get settings
	SP_DLG_TwinNetSetting l_cDlg(NULL);
	if (l_cDlg.ShowModal() == wxID_OK)
	{			
		m_sColorSetName = l_cDlg.GetColorSetName();
		m_sType = l_cDlg.GetType();
		m_sColors = l_cDlg.GetColors();
		m_sVariableName = l_cDlg.GetVariableName();
		m_sFusionPlaceName = l_cDlg.GetFusionPlaceName();
		m_sScheduleTransitionName = l_cDlg.GetScheduleTransitionName();
		m_sCoarsePlaceName = l_cDlg.GetCoarsePlaceName();
		m_sNewPlaceName = l_cDlg.GetNewPlaceName();
		m_sNewTransitionName = l_cDlg.GetNewTransitionName();
		m_sParameterName = l_cDlg.GetParameterName();
		m_sParameterValue = l_cDlg.GetParameterValue();

		m_bGenerateSideConditions = l_cDlg.GetGenerateSideConditions();

		l_cDlg.Destroy();
	}
	else
	{
		l_cDlg.Destroy();
		return false;
	}	


	

	/////////////////////////////////////////////////////////////////////




	GetNetSize();

	if( !GenerateColorSets() )
		return false;

	if( !GenerateNewNet() )
		return false;

	return true;
}


bool SP_CPN_GenTwinNet::GenerateColorSets()
{
	SP_DS_Metadataclass* l_pcMetadataclass;
	SP_DS_Metadata* l_pcNewMetadata;
	
	//Generate a color set
	l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_BASICCOLORSETCLASS);
	if(!l_pcMetadataclass)
		return false;
	l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	if(!l_pcNewMetadata)
		return false;	
	SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("ColorsetList")));
	if(!l_pcColList)
		return false;

	//for twin nets
	l_pcColList->Clear();
	l_pcColList->AppendEmptyRow();
	l_pcColList->SetCell(0,0,wxT("Dot") );
	l_pcColList->SetCell(0,1,wxT("dot"));
	l_pcColList->SetCell(0,2,wxT("dot"));


	l_pcColList->AppendEmptyRow();
	l_pcColList->SetCell(1,0,m_sColorSetName );
	l_pcColList->SetCell(1,1, m_sType );
	l_pcColList->SetCell(1,2, m_sColors );
/*
	//for two transitions
	l_pcColList->AppendEmptyRow();
	l_pcColList->SetCell(2,0,wxT("TS") );
	l_pcColList->SetCell(2,1,wxT("int"));
	l_pcColList->SetCell(2,2,wxT("1-2"));
*/
	//Declare variables
	l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_VARIABLECLASS);
	if(!l_pcMetadataclass)
		return false;
	l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	if(!l_pcNewMetadata)
		return false;
	l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("VariableList")));
	if(!l_pcColList)
		return false;

	l_pcColList->Clear();
	l_pcColList->AppendEmptyRow();
	l_pcColList->SetCell(0,0,m_sVariableName);
	l_pcColList->SetCell(0,1,m_sColorSetName);

/*
	l_pcColList->AppendEmptyRow();
	l_pcColList->SetCell(1,0,wxT("y"));
	l_pcColList->SetCell(1,1,wxT("TS"));
*/
	//update color set definition
	m_pcGraph->CreateDeclarationTree()->UpdateColorSetTree();

	return true;
}

bool SP_CPN_GenTwinNet::GenerateNewNet()
{
	vector<SP_DS_Node*> l_vNewLogicalNodes;
	SP_ListNode::const_iterator l_Iter;	
	SP_DS_Nodeclass* l_pcNewNC = m_pcGraph->GetNodeclass(wxT("Place"));
	if( !l_pcNewNC)
		return false;

	wxString l_sExistingPlaceName = wxT("####");


	//Set arc expressions for the original net
	SP_ListEdge::const_iterator l_itEdges;
	SP_DS_Nodeclass* l_pcNewTransitionNC = m_pcGraph->GetNodeclass(wxT("Transition"));
	if( !l_pcNewTransitionNC)
		return false;

	int l_nCount = 0;

	if( m_bGenerateSideConditions )
	{
	    //Create a place for every transition	
		//Create a place for each transition, connected by a read arc
		for ( l_Iter = l_pcNewTransitionNC->GetElements()->begin(); l_Iter != l_pcNewTransitionNC->GetElements()->end(); ++l_Iter )
		{
				
				SP_DS_Node* l_pcOldNode = *(l_pcNewNC->GetElements()->begin());
				SP_DS_Node* l_pcNewPlaceNode = l_pcNewNC->NewElement(l_pcOldNode->GetNetnumber());
				l_pcNewPlaceNode->GetAttribute(wxT("Name"))->SetValueString( m_sNewPlaceName + wxString::Format(wxT("%d"),l_nCount++) );	
				//l_pcNewFusionNode->GetAttribute(wxT("Logic"))->SetValueString(wxT("1"));

				//Add color set and initial marking
				SP_DS_ColListAttribute * l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcNewPlaceNode->GetAttribute(SP_DS_CPN_MARKINGLIST) );
				if(!l_pcColList)
					return false;
				l_pcColList->Clear();
				SP_DS_TextAttribute* l_pcColorSetAttibute = dynamic_cast< SP_DS_TextAttribute* >(l_pcNewPlaceNode->GetAttribute(SP_DS_CPN_COLORSETNAME) );
				if(!l_pcColorSetAttibute)
					return false;
				l_pcColorSetAttibute->SetValue( m_sColorSetName );
				
				SP_Graphic* l_pcTransGr = *((*l_Iter)->GetGraphics()->begin());

				for(SP_ListGraphic::const_iterator l_itGr = l_pcNewPlaceNode->GetGraphics()->begin(); l_itGr != l_pcNewPlaceNode->GetGraphics()->end();l_itGr++)
				{				
					SP_Graphic* l_pcNewGr = *l_itGr;				
					l_pcNewGr->SetGraphicState((*(l_pcOldNode->GetGraphics()->begin()))->GetGraphicState());
					l_pcNewGr->SetPosX(l_pcTransGr->GetPosX()); 
					l_pcNewGr->SetPosY(l_pcTransGr->GetPosY()+ m_dOffValue);		// it should be reasonablely set.		
				}

					SP_ListGraphic::const_iterator l_itGrSource = l_pcNewPlaceNode->GetGraphics()->begin();
					
					SP_DS_Edgeclass* l_pcREC = m_pcGraph->GetEdgeclass(wxT("Read Edge"));
					SP_Graphic* l_pcPlaceGr = *(l_itGrSource);
					

					SP_DS_Edge* l_pcNewEdge3 = l_pcREC->NewElement( 1,l_pcPlaceGr,l_pcTransGr );
					l_pcREC->AddElement(l_pcNewEdge3);

					l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcNewEdge3->GetAttribute(SP_DS_CPN_INSCRIPTION) );
					if(!l_pcColList)
						return false;
					l_pcColList->SetCell(0, 1, m_sVariableName);	

					l_pcNewEdge3->SetSource( l_pcNewPlaceNode );
					l_pcNewEdge3->SetTarget( *l_Iter );
					l_pcNewEdge3->SetNodes(l_pcPlaceGr, l_pcTransGr, (*(l_pcNewEdge3->GetGraphics()->begin())));
					l_pcNewEdge3->Update(TRUE);
					l_pcNewEdge3->ShowOnCanvas( m_pcCanvas );		
		}

	}


	for ( l_Iter = l_pcNewTransitionNC->GetElements()->begin(); l_Iter != l_pcNewTransitionNC->GetElements()->end(); ++l_Iter )
	{
		for (l_itEdges = (*l_Iter)->GetTargetEdges()->begin(); l_itEdges != (*l_Iter)->GetTargetEdges()->end(); ++l_itEdges)
		{
			SP_DS_Edge* l_pcEdge = *l_itEdges;
			SP_DS_ColListAttribute * l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcEdge->GetAttribute(SP_DS_CPN_INSCRIPTION) );
			if(!l_pcColList)
				return false;
			l_pcColList->SetCell(0, 1, m_sVariableName);	
			l_pcEdge->Update(TRUE);
				
		}
		for (l_itEdges = (*l_Iter)->GetSourceEdges()->begin(); l_itEdges != (*l_Iter)->GetSourceEdges()->end(); ++l_itEdges)
		{
			SP_DS_Edge* l_pcEdge = *l_itEdges;
			SP_DS_ColListAttribute * l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcEdge->GetAttribute(SP_DS_CPN_INSCRIPTION) );
			if(!l_pcColList)
				return false;
			l_pcColList->SetCell(0, 1, m_sVariableName);	
			l_pcEdge->Update(TRUE);
		}
	}


	//Create new logical place nodes
	const SP_ListNode* l_plElements = l_pcNewNC->GetElements();

		
	vector<SP_DS_Node*> l_vNewTransitionNodes;
	l_nCount = 0;
	l_sExistingPlaceName = wxT("####");

	for ( l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter )
	{		
		SP_DS_Node* l_pcOldNode = *l_Iter;
		

		if ( l_sExistingPlaceName == l_pcOldNode->GetAttribute(wxT("Name"))->GetValueString() )
		{
			SP_MESSAGEBOX(wxT("Two places with empty or identical names are not allowed!"), wxT("Notification"), wxOK | wxICON_ERROR );
			return false;
		}
		l_sExistingPlaceName = l_pcOldNode->GetAttribute(wxT("Name"))->GetValueString();

		l_pcOldNode->GetAttribute(wxT("Logic"))->SetValueString(wxT("1"));

		//Add color set and initial marking
		SP_DS_ColListAttribute * l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcOldNode->GetAttribute(SP_DS_CPN_MARKINGLIST) );
		if(!l_pcColList)
			return false;
		l_pcColList->Clear();
		SP_DS_TextAttribute* l_pcColorSetAttibute = dynamic_cast< SP_DS_TextAttribute* >(l_pcOldNode->GetAttribute(SP_DS_CPN_COLORSETNAME) );
		if(!l_pcColorSetAttibute)
				return false;
		l_pcColorSetAttibute->SetValue( m_sColorSetName );
		//

		l_pcOldNode->Update(TRUE);

		//Create a new graphic for a logic node
		SP_Graphic* l_pcOldGr = *( l_pcOldNode->GetGraphics()->begin() );
		SP_Graphic* l_pcNewGr = l_pcOldNode->NewGraphicInSubnet(1);
		l_pcNewGr->SetPosX(l_pcOldGr->GetPosX());	
		l_pcNewGr->SetPosY( m_dOffValue + m_dNetMaxY);		// it should be reasonably set.		
		l_pcOldNode->Update(TRUE);
		l_pcOldNode->ShowOnCanvas( m_pcCanvas );

		m_lShapes.push_back(l_pcNewGr);



		//Create new transition nodes
		SP_DS_Nodeclass* l_pcNewTransitionNC = m_pcGraph->GetNodeclass(wxT("Transition"));
		SP_DS_Node* l_pcOldTransitionNode = *(l_pcNewTransitionNC->GetElements()->begin());
		SP_DS_Node* l_pcNewTransitionNode = l_pcNewTransitionNC->NewElement(l_pcOldTransitionNode->GetNetnumber());

		l_pcNewTransitionNode->GetAttribute(wxT("Name"))->SetValueString( m_sNewTransitionName + wxString::Format(wxT("%d"),l_nCount) );

		//Set rate functions for newly created transitions
		l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcNewTransitionNode->GetAttribute( wxT("FunctionList") ) );
		if(!l_pcColList)
			return false;
		l_pcColList->SetCell(0, 0, wxT("true"));	
		l_pcColList->SetCell(0, 1, wxT("MassAction(") + m_sParameterName + wxT(")") );	


		l_nCount++;

		SP_Graphic* l_pcLogicPlaceGr = *(++((*l_Iter)->GetGraphics()->begin())); //to get the second graphic

		bool l_bFirstGr = true;
		for(SP_ListGraphic::const_iterator l_itGr = l_pcOldTransitionNode->GetGraphics()->begin(); l_itGr != l_pcOldTransitionNode->GetGraphics()->end();l_itGr++)
		{
			SP_Graphic* l_pcOldGr = *l_itGr;
			SP_Graphic* l_pcNewGr = NULL;
			if(l_bFirstGr)
			{
				l_pcNewGr = *(l_pcNewTransitionNode->GetGraphics()->begin());
				l_bFirstGr = false;
			}
			else
			{
				l_pcNewGr = l_pcNewTransitionNode->NewGraphicInSubnet(l_pcOldGr->GetNetnumber());
			}
			l_pcNewGr->SetGraphicState(l_pcOldGr->GetGraphicState());
			l_pcNewGr->SetPosX(l_pcLogicPlaceGr->GetPosX()); 
			l_pcNewGr->SetPosY(l_pcLogicPlaceGr->GetPosY()+ m_dOffValue);		// it should be reasonablely set.		
		}
		l_pcNewTransitionNode->Update(TRUE);
		l_pcNewTransitionNode->ShowOnCanvas( m_pcCanvas );
		l_vNewTransitionNodes.push_back(l_pcNewTransitionNode);





		//Create edges
		SP_DS_Edgeclass* l_pcEC = m_pcGraph->GetEdgeclass(wxT("Edge"));
		SP_DS_Edge* l_pcOldEdge = *(l_pcEC->GetElements()->begin());

		//From place to transition
		SP_DS_Edge* l_pcNewEdge1 = l_pcOldEdge->Clone();					
		l_pcEC->AddElement(l_pcNewEdge1);

		l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcNewEdge1->GetAttribute(SP_DS_CPN_INSCRIPTION) );
		if(!l_pcColList)
			return false;
		l_pcColList->SetCell(0, 1, m_sVariableName);	
		
		//Use the second subnet graphic
		SP_ListGraphic::const_iterator l_itGrSource = ++((*l_Iter)->GetGraphics()->begin());
		SP_ListGraphic::const_iterator l_itGrTarget = l_pcNewTransitionNode->GetGraphics()->begin();
		SP_ListGraphic::const_iterator l_itGrEdge = l_pcNewEdge1->GetGraphics()->begin();

		while(l_itGrEdge != l_pcNewEdge1->GetGraphics()->end() 
				&& l_itGrSource !=  (*l_Iter)->GetGraphics()->end()
				&& l_itGrTarget != l_pcNewTransitionNode->GetGraphics()->end() )
		{
			l_pcNewEdge1->SetNodes((*l_itGrSource), (*l_itGrTarget), (*l_itGrEdge));
			l_itGrEdge++;
			l_itGrSource++;
			l_itGrTarget++;
		}	
				
		l_pcNewEdge1->Update(TRUE);
		l_pcNewEdge1->ShowOnCanvas( m_pcCanvas );

/*		//Add control points
		l_itGrEdge = l_pcNewEdge1->GetGraphics()->begin();
		SP_GR_Edge* l_pcGREdge = dynamic_cast<SP_GR_Edge*>(*l_itGrEdge);
		wxList* l_pcList = dynamic_cast<wxLineShape*>(l_pcGREdge->GetPrimitive())->GetLineControlPoints();
		l_pcList->Append((wxObject*)new wxRealPoint(l_pcGREdge->GetSource()->GetPosX()-20,(l_pcGREdge->GetSource()->GetPosY()+l_pcGREdge->GetTarget()->GetPosY())/2));				
		if(l_pcGREdge)
		{
			l_pcGREdge->SetControlPoints(l_pcList);
		}
		l_pcNewEdge1->Update(TRUE);
		l_pcNewEdge1->ShowOnCanvas( m_pcCanvas );
		//end of add control points
*/	

		m_lEdges.push_back(*(l_pcNewEdge1->GetGraphics()->begin()));


		//From transition to place
		SP_DS_Edge* l_pcNewEdge2 = l_pcOldEdge->Clone();					
		l_pcEC->AddElement(l_pcNewEdge2);

		l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcNewEdge2->GetAttribute(SP_DS_CPN_INSCRIPTION) );
		if(!l_pcColList)
			return false;
		l_pcColList->SetCell(0, 1, wxT("+") + m_sVariableName);	

		l_pcNewEdge2->SetSource(l_pcNewTransitionNode);
		l_pcNewEdge2->SetTarget( *l_Iter );
								
		SP_ListGraphic::const_iterator l_itGrSource2 = l_pcNewTransitionNode->GetGraphics()->begin();
		SP_ListGraphic::const_iterator l_itGrTarget2 = ++( (*l_Iter)->GetGraphics()->begin() );
		SP_ListGraphic::const_iterator l_itGrEdge2 = l_pcNewEdge2->GetGraphics()->begin();

		while(l_itGrEdge2 != l_pcNewEdge2->GetGraphics()->end() 
				&& l_itGrSource2 !=  l_pcNewTransitionNode->GetGraphics()->end()
				&& l_itGrTarget2 != (*l_Iter)->GetGraphics()->end() )
		{
			l_pcNewEdge2->SetNodes((*l_itGrSource2), (*l_itGrTarget2), (*l_itGrEdge2));
			l_itGrEdge2++;
			l_itGrSource2++;
			l_itGrTarget2++;
		}
		l_pcNewEdge2->Update(TRUE);
		l_pcNewEdge2->ShowOnCanvas( m_pcCanvas );

/*		//Add control points
		l_itGrEdge = l_pcNewEdge2->GetGraphics()->begin();
		l_pcGREdge = dynamic_cast<SP_GR_Edge*>(*l_itGrEdge);
		l_pcList = dynamic_cast<wxLineShape*>(l_pcGREdge->GetPrimitive())->GetLineControlPoints();
		l_pcList->Append((wxObject*)new wxRealPoint(l_pcGREdge->GetSource()->GetPosX()+20,(l_pcGREdge->GetSource()->GetPosY()+l_pcGREdge->GetTarget()->GetPosY())/2));				
		if(l_pcGREdge)
		{
			l_pcGREdge->SetControlPoints(l_pcList);
		}
		l_pcNewEdge2->Update(TRUE);
		l_pcNewEdge2->ShowOnCanvas( m_pcCanvas );
		//end of add control points
*/
		m_lEdges.push_back(*(l_pcNewEdge2->GetGraphics()->begin()));

		
	
	}


	//Create a fusion node	
	wxString l_sID;
	SP_DS_Node* l_pcNewFusionNode;
	
	for(unsigned i = 0; i < l_vNewTransitionNodes.size(); i++)
	{
		//Create a new place node
		if( i == 0 )
		{
			SP_DS_Node* l_pcOldNode = *(l_pcNewNC->GetElements()->begin());
			l_pcNewFusionNode = l_pcNewNC->NewElement(l_pcOldNode->GetNetnumber());
			l_pcNewFusionNode->GetAttribute(wxT("Name"))->SetValueString( m_sFusionPlaceName );	
			l_pcNewFusionNode->GetAttribute(wxT("Logic"))->SetValueString(wxT("1"));

			//Add color set and initial marking
			SP_DS_ColListAttribute * l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcNewFusionNode->GetAttribute(SP_DS_CPN_MARKINGLIST) );
			if(!l_pcColList)
				return false;
			l_pcColList->Clear();
			SP_DS_TextAttribute* l_pcColorSetAttibute = dynamic_cast< SP_DS_TextAttribute* >(l_pcNewFusionNode->GetAttribute(SP_DS_CPN_COLORSETNAME) );
			if(!l_pcColorSetAttibute)
				return false;
			l_pcColorSetAttibute->SetValue( wxT("Dot") );
			
			SP_Graphic* l_pcTransGr = *(l_vNewTransitionNodes[i]->GetGraphics()->begin());

			for(SP_ListGraphic::const_iterator l_itGr = l_pcNewFusionNode->GetGraphics()->begin(); l_itGr != l_pcNewFusionNode->GetGraphics()->end();l_itGr++)
			{				
				SP_Graphic* l_pcNewGr = *l_itGr;				
				l_pcNewGr->SetGraphicState((*(l_pcOldNode->GetGraphics()->begin()))->GetGraphicState());
				l_pcNewGr->SetPosX(l_pcTransGr->GetPosX()); 
				l_pcNewGr->SetPosY(l_pcTransGr->GetPosY()+ m_dOffValue);		// it should be reasonablely set.		
			}

				SP_ListGraphic::const_iterator l_itGrSource = l_pcNewFusionNode->GetGraphics()->begin();
				
				SP_DS_Edgeclass* l_pcREC = m_pcGraph->GetEdgeclass(wxT("Read Edge"));
				SP_Graphic* l_pcPlaceGr = *(l_itGrSource);

				m_lShapes.push_back(l_pcPlaceGr);

				SP_DS_Edge* l_pcNewEdge3 = l_pcREC->NewElement( 1,l_pcPlaceGr,l_pcTransGr );
				l_pcREC->AddElement(l_pcNewEdge3);

				l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcNewEdge3->GetAttribute(SP_DS_CPN_INSCRIPTION) );
				if(!l_pcColList)
					return false;
				l_pcColList->SetCell(0, 1, wxT("dot"));	

				l_pcNewEdge3->SetSource( l_pcNewFusionNode );
				l_pcNewEdge3->SetTarget( l_vNewTransitionNodes[i] );
				l_pcNewEdge3->SetNodes(l_pcPlaceGr, l_pcTransGr, (*(l_pcNewEdge3->GetGraphics()->begin())));
				l_pcNewEdge3->Update(TRUE);
				l_pcNewEdge3->ShowOnCanvas( m_pcCanvas );

				m_lEdges.push_back(*(l_pcNewEdge3->GetGraphics()->begin()));
		}
		else
		{
			//Add a logic graphics
			//SP_Graphic* l_pcOldGr = *( l_pcNewFusionNode->GetGraphics()->begin() );
			SP_Graphic* l_pcTransGr = *(l_vNewTransitionNodes[i]->GetGraphics()->begin());

			SP_Graphic* l_pcNewGr = l_pcNewFusionNode->NewGraphicInSubnet(1);
			l_pcNewGr->SetPosX(l_pcTransGr->GetPosX());	
			l_pcNewGr->SetPosY(l_pcTransGr->GetPosY() + m_dOffValue);		// it should be reasonablely set.		
			l_pcNewFusionNode->Update(TRUE);
			l_pcNewFusionNode->ShowOnCanvas( m_pcCanvas );

			m_lShapes.push_back(l_pcNewGr);

			//Add an edge
			SP_ListGraphic::const_iterator l_itGrSource = l_pcNewFusionNode->GetGraphics()->begin();
			SP_DS_Edgeclass* l_pcREC = m_pcGraph->GetEdgeclass(wxT("Read Edge"));
			SP_DS_Edge* l_pcOldEdge = *(l_pcREC->GetElements()->begin());
			SP_DS_Edge* l_pcNewEdge2 = l_pcOldEdge->Clone();					
			l_pcREC->AddElement(l_pcNewEdge2);	

			SP_DS_ColListAttribute * l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcNewEdge2->GetAttribute(SP_DS_CPN_INSCRIPTION) );
			if(!l_pcColList)
				return false;
			l_pcColList->SetCell(0, 1, wxT("dot"));	

			l_pcNewEdge2->SetNodes(l_pcNewGr, l_pcTransGr, (*(l_pcNewEdge2->GetGraphics()->begin())));
			l_pcNewEdge2->Update(TRUE);
			l_pcNewEdge2->ShowOnCanvas( m_pcCanvas );

			m_lEdges.push_back(*(l_pcNewEdge2->GetGraphics()->begin()));

		}	

		//show them in the canvas
		l_pcNewFusionNode->Update(TRUE);
		l_pcNewFusionNode->ShowOnCanvas( m_pcCanvas );
	}


	//Generate scheduled transition	
	SP_Graphic* l_pcNewGr = l_pcNewFusionNode->NewGraphicInSubnet(1);
	//l_pcNewGr->SetPosX(  150.0 );	
	//l_pcNewGr->SetPosY( m_dNetMaxY + m_dOffValue);		// it should be reasonablely set.		

	l_pcNewGr->SetPosX(  300 );	
	l_pcNewGr->SetPosY(  75 );

	l_pcNewFusionNode->Update(TRUE);
	l_pcNewFusionNode->ShowOnCanvas( m_pcCanvas );


	SP_DS_Nodeclass* l_pcScheduledNC = m_pcGraph->GetNodeclass(wxT("Scheduled Transition"));
	
	SP_DS_Node* l_pcTransition = l_pcScheduledNC->NewElement(1);

	l_pcTransition->GetAttribute(wxT("Name"))->SetValueString( m_sScheduleTransitionName );	

	SP_Graphic* l_pcNewTransGr =  *(l_pcTransition->GetGraphics()->begin());
	//l_pcNewTransGr->SetPosX(  150.0 );	//m_dNetMaxX
	//l_pcNewTransGr->SetPosY( m_dNetMaxY + m_dOffValue + 100.0 );

	l_pcNewTransGr->SetPosX(  150 );	
	l_pcNewTransGr->SetPosY(  75 );

	//
	SP_DS_ColListAttribute * l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcTransition->GetAttribute( wxT("PeriodicList")) );
	SP_Graphic* l_pcColListGr = *(l_pcColList->GetGraphics()->begin());	
	l_pcColListGr->SetOffsetX(0);
	l_pcColListGr->SetOffsetY(-40);
	l_pcColListGr->SetShow(TRUE);

	l_pcTransition->Update(TRUE);
	l_pcTransition->ShowOnCanvas( m_pcCanvas );

	//Create edges
	SP_DS_Edgeclass* l_pcEC = m_pcGraph->GetEdgeclass(wxT("Edge"));
	SP_DS_Edge* l_pcOldEdge = *(l_pcEC->GetElements()->begin());

	SP_DS_Edge* l_pcNewEdge = l_pcOldEdge->Clone();					
	l_pcEC->AddElement(l_pcNewEdge);

	l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcNewEdge->GetAttribute(SP_DS_CPN_INSCRIPTION) );
	if(!l_pcColList)
		return false;
	l_pcColList->SetCell(0, 1, wxT("dot"));	

	l_pcNewEdge->SetNodes(l_pcNewTransGr, l_pcNewGr, (*(l_pcNewEdge->GetGraphics()->begin())));
	l_pcNewEdge->Update(TRUE);
	l_pcNewEdge->ShowOnCanvas( m_pcCanvas );


	


	//Create course transition
	//SP_ListEdge::const_iterator l_itEdges;

	for(unsigned i = 0; i < l_vNewTransitionNodes.size(); i++)
	{
		SP_Graphic* l_pcTransGr =  *(l_vNewTransitionNodes[i]->GetGraphics()->begin());
		m_lShapes.push_back( l_pcTransGr );
	}
	
	OnCoarse(&m_lShapes, &m_lEdges);


	/////Create a parameter 
	SP_DS_Nodeclass* l_pcParameterNC = m_pcGraph->GetNodeclass(wxT("Parameter"));
	SP_DS_Node* l_pcParameter = l_pcParameterNC->NewElement(1);

	l_pcParameter->GetAttribute(wxT("Name"))->SetValueString( m_sParameterName );	

	//set position
	SP_Graphic* l_pcNewParaGr =  *(l_pcParameter->GetGraphics()->begin());
	l_pcNewParaGr->SetPosX(  600 );	//m_dNetMaxX
	l_pcNewParaGr->SetPosY( 75 );

	//assign value
	l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcParameter->GetAttribute(wxT("ParameterList")) );
	if(!l_pcColList)
		return false;
	l_pcColList->SetCell(0, 1, m_sParameterValue );	

	l_pcParameter->Update(TRUE);
	l_pcParameter->ShowOnCanvas( m_pcCanvas );

	

			

	return true;
}





void SP_CPN_GenTwinNet::OnCoarse(SP_ListGraphic* p_plShapes, SP_ListGraphic* p_plEdges)
{
	// coarse means to take all selected nodes and test their edges,
	// connecting them. only coarse edges, if both ends are selected, too.

	SP_ListGraphic::iterator l_Iter;
	double l_nMinX = 10000, l_nMaxX = -1, l_nMinY = 10000, l_nMaxY = -1;
	double l_nX1, l_nX2, l_nY1, l_nY2;
	
	for (l_Iter = p_plShapes->begin(); l_Iter != p_plShapes->end(); ++l_Iter)
	{
		(*l_Iter)->Select(TRUE);
		(*l_Iter)->GetBoundingBox(&l_nX1, &l_nY1, &l_nX2, &l_nY2);
		l_nMinX = wxMin(l_nMinX, l_nX1);
		l_nMinY = wxMin(l_nMinY, l_nY1);
		l_nMaxX = wxMax(l_nMaxX, l_nX2);
		l_nMaxY = wxMax(l_nMaxY, l_nY2);
	}
	// add edges, that have both ends selected to the parameter

	// show selection
	for (l_Iter = p_plEdges->begin(); l_Iter != p_plEdges->end(); ++l_Iter)
	{
		p_plShapes->push_back((*l_Iter));
		(*l_Iter)->Select(TRUE);
	}

	m_pcView->Refresh();

	DoCoarse(p_plShapes, l_nMinX + ((l_nMaxX - l_nMinX) / 2.0), l_nMinY
			+ ((l_nMaxY - l_nMinY) / 2.0),wxT("Coarse Place"));
}

bool SP_CPN_GenTwinNet::DoCoarse(SP_ListGraphic* p_plShapes, double p_nX,
		double p_nY, const wxString& p_sCoarseNode)
{
	if (!p_plShapes || p_plShapes->size() == 0 || !m_pcDoc)
		return FALSE;

	
	SP_DS_Graph* l_pcGraph = m_pcDoc->GetGraph();
	SP_ListGraphic::iterator l_Iter;
	SP_Graphic* l_pcCoarseGr;
	wxString l_sNodeClass = p_sCoarseNode;

	l_sNodeClass = wxT("Coarse Place");

	if (!l_pcGraph->GetNodeclass(l_sNodeClass))
		return FALSE;

	SP_DS_Node* l_pcNode = l_pcGraph->GetNodeclass(l_sNodeClass)->NewElement(1);
	l_pcNode->GetAttribute(wxT("Name"))->SetValueString( m_sCoarsePlaceName );	
	if (!l_pcNode || !l_pcNode->GetCoarse())
		return FALSE;

	//l_pcNode->ShowOnCanvas(m_pcCanvas, TRUE, p_nX, p_nY);

	l_pcNode->ShowOnCanvas(m_pcCanvas, TRUE, 450, 75);

	l_pcCoarseGr = (*l_pcNode->GetGraphics()->begin());

	if(!m_pcView->CheckCoarseConsistency(p_plShapes, l_pcNode))
	{
		l_pcNode->RemoveAllGraphics(true);
		l_pcNode->Remove();
		wxDELETE(l_pcNode);
		return false;
	}

	m_pcView->SelectAll(FALSE);
	for (l_Iter = p_plShapes->begin(); l_Iter != p_plShapes->end(); ++l_Iter)
	{
		(*l_Iter)->Coarse(l_pcNode->GetCoarse()->GetNetnumber(), l_pcCoarseGr, p_plShapes);
	}

	if (m_pcCanvas)
	{
		m_pcCanvas->MoveShape(l_pcCoarseGr->GetPrimitive(), 0, 0);
	}

	SP_Core::Instance()->RemoveQueuedElements();
	if (m_pcDoc)
	{
		m_pcDoc->GetGraph()->ResetCoarseTree();
		m_pcDoc->Modify(true);
	}
	m_pcView->Refresh();

	return TRUE;
}



void SP_CPN_GenTwinNet::GetNetSize()
{	

	const SP_ListNode* l_plElements;
	SP_ListNode::const_iterator l_Iter;	
	SP_DS_Nodeclass* l_pcNodeclass;
	l_pcNodeclass= m_pcGraph->GetNodeclass(wxT("Place"));
	l_plElements= m_pcGraph->GetNodeclass(wxT("Place"))->GetElements();
	

	for ( l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter )
	{		
		for(SP_ListGraphic::const_iterator l_itGr = (*l_Iter)->GetGraphics()->begin();
				l_itGr != (*l_Iter)->GetGraphics()->end();l_itGr++)
		{
			if((*l_itGr)->GetPosX()>m_dNetMaxX)
				m_dNetMaxX=(*l_itGr)->GetPosX();

			if((*l_itGr)->GetPosY() > m_dNetMaxY)
				m_dNetMaxY=(*l_itGr)->GetPosY();			
		}			
	}
	
}





bool SP_CPN_GenTwinNet::GenerateAnyNet(SP_DS_Graph* p_pcGraph,SP_MDI_View* p_pcView)
{

	m_pcGraph = p_pcGraph;
	if(!m_pcGraph)
		return false;

	GetSelectedNodes( p_pcView);

	if(m_vPlaceNodes.size() ==  0 && m_vTransNodes.size() ==  0)
	{
		SP_MESSAGEBOX(wxT("Please select a subnet"), wxT("Colorizing a subnet"), wxOK | wxICON_ERROR);
		return false;
	}	

	//Get settings
	SP_DLG_TwinNetSetting l_cDlg(NULL,wxT("Colorizing a subnet"));
	if (l_cDlg.ShowModal() == wxID_OK)
	{			
		m_sColorSetName = l_cDlg.GetColorSetName();
		m_sType = l_cDlg.GetType();
		m_sColors = l_cDlg.GetColors();
		m_sVariableName = l_cDlg.GetVariableName();
		
		l_cDlg.Destroy();
	}
	else
	{
		l_cDlg.Destroy();
		return false;
	}	
	

	/////////////////////////////////////////////////////////////////////


	if( !GenerateColorSets() )
		return false;

	if( !GenerateAnyColorNet() )
		return false;

	return true;
}

bool SP_CPN_GenTwinNet::GenerateAnyColorNet()
{
	for( unsigned i = 0; i < m_vPlaceNodes.size(); i++ )
	{
		SP_DS_Node* l_pcPlaceNode = m_vPlaceNodes[i];
		SP_DS_TextAttribute* l_pcNameAttibute;
		l_pcNameAttibute = dynamic_cast< SP_DS_TextAttribute* >(l_pcPlaceNode->GetAttribute(SP_DS_CPN_COLORSETNAME) );
		if(!l_pcNameAttibute)
			return false;
		l_pcNameAttibute->SetValue(m_sColorSetName);
		l_pcPlaceNode->Update(TRUE);
	}

	for( unsigned i = 0; i < m_vEdges.size(); i++ )
	{
		SP_DS_Edge* l_pcEdge = m_vEdges[i];
		SP_DS_ColListAttribute * l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcEdge->GetAttribute(SP_DS_CPN_INSCRIPTION) );
		if(!l_pcColList)
			return false;
		l_pcColList->SetCell(0, 1, m_sVariableName);	
		l_pcEdge->Update(TRUE);
	}

	return true;
}

bool SP_CPN_GenTwinNet::GetSelectedNodes(SP_MDI_View* p_pcView)
{
	wxList l_lShapes;
	p_pcView->FindSelectedShapes(l_lShapes);

	wxNode* l_pcGraphicNode = l_lShapes.GetFirst();
	while (l_pcGraphicNode)
	{
		SP_Graphic* l_pcGraphic = SP_Core::Instance()->ResolveExtern(dynamic_cast<wxShape*>(l_pcGraphicNode->GetData()));
		if (l_pcGraphic && l_pcGraphic->GetGraphicType() == SP_GRAPHIC_NODE)
		{		
			wxString l_sNodeClassNoName = l_pcGraphic->GetParent()->GetClassName();
			if( l_sNodeClassNoName == SP_DS_DISCRETE_PLACE  || l_sNodeClassNoName == SP_DS_CONTINUOUS_PLACE )
			{
				SP_DS_Node* l_pcShapeNode = dynamic_cast<SP_DS_Node*>(l_pcGraphic->GetParent());
				if(l_pcShapeNode)
					m_vPlaceNodes.push_back(l_pcShapeNode);
			}
			if( l_sNodeClassNoName == SP_DS_STOCHASTIC_TRANS  
				|| l_sNodeClassNoName == SP_DS_IMMEDIATE_TRANS 
				|| l_sNodeClassNoName == SP_DS_DETERMINISTIC_TRANS
				|| l_sNodeClassNoName == SP_DS_SCHEDULED_TRANS
				|| l_sNodeClassNoName == SP_DS_CONTINUOUS_TRANS)
			{
				SP_DS_Node* l_pcShapeNode = dynamic_cast<SP_DS_Node*>(l_pcGraphic->GetParent());
				if(l_pcShapeNode)
					m_vTransNodes.push_back(l_pcShapeNode);
			}
		}
		if (l_pcGraphic && l_pcGraphic->GetGraphicType() == SP_GRAPHIC_EDGE)
		{
			SP_DS_Edge* l_pcShapeEdge = dynamic_cast<SP_DS_Edge*>(l_pcGraphic->GetParent());
			if(l_pcShapeEdge)
				m_vEdges.push_back(l_pcShapeEdge);
		}

		l_pcGraphicNode = l_pcGraphicNode->GetNext();
	}

	return true;
}
