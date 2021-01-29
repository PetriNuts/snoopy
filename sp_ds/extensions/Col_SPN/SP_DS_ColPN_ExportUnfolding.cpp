//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2009/12/28 10:20:00 $
// Short Description: colored transition animator class
//////////////////////////////////////////////////////////////////////

#include "sp_ds/extensions/Col_SPN/SP_DS_ColPN_ExportUnfolding.h"

#include "sp_gui/widgets/dialogs/wid_dia_SPN/SP_WDG_StFunctionList.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogColList.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_gr/attributes/SP_GR_ColListAttribute.h" 
#include "sp_gr/SP_GR_Edge.h"
#include "sp_ds/attributes/SP_DS_DoubleMultiplicityAttribute.h"
#include "sp_ds/extensions/SP_DS_Coarse.h"
#include "sp_ds/attributes/SP_DS_MarkingAttribute.h"
#include "sp_ds/attributes/SP_DS_DoubleMarkingAttribute.h"

SP_DS_ColPN_ExportUnfolding::SP_DS_ColPN_ExportUnfolding()
{	
	
}

SP_DS_ColPN_ExportUnfolding::~SP_DS_ColPN_ExportUnfolding()
{

}


bool SP_DS_ColPN_ExportUnfolding::ProcessResult()
{
	m_nOrdinaryArcNum = 0;
	m_nReadArcNum = 0;
	m_nInhibitorArcNum = 0;
	m_nResetArcNum = 0;
	m_nEqualArcNum = 0;
	m_nModifierArcNum = 0;

	Initialize();

	if(!CreateDuplicatePlaceInstances())
		return false;

	if(!CreateDuplicateTransInstances())
		return false;

	RemoveIsolatedNoed( &m_mNewDiscretePlaceNodes );
	RemoveIsolatedNoed( &m_mNewContinuousPlaceNodes );
	RemoveIsolatedNoed( &m_mNewContinuousTransitionNodes );
	RemoveIsolatedNoed( &m_mNewDiscreteTransitionNodes );
	RemoveIsolatedNoed( &m_mNewImmediateTransitionNodes );
	RemoveIsolatedNoed( &m_mNewDeterministicTransitionNodes );
	RemoveIsolatedNoed( &m_mNewScheduledTransitionNodes );


	m_nPlaceCountStat = m_mNewDiscretePlaceNodes.size() + m_mNewContinuousPlaceNodes.size();
	m_nTransitionCountStat = m_mNewContinuousTransitionNodes.size() + m_mNewDiscreteTransitionNodes.size() + m_mNewImmediateTransitionNodes.size() +
		                     m_mNewDeterministicTransitionNodes.size() + m_mNewScheduledTransitionNodes.size();

	return true;
}


double SP_DS_ColPN_ExportUnfolding::GetNetSize()
{
	double l_NetMaxX=0;
	double l_NetMinX=0;

	const SP_ListNode* l_plElements;
	SP_ListNode::const_iterator l_Iter;	
	SP_DS_Nodeclass* l_pcNodeclass;

	l_pcNodeclass= m_pcGraph->GetNodeclass( SP_DS_DISCRETE_PLACE );
	if(l_pcNodeclass)
	{
		l_plElements= l_pcNodeclass->GetElements();
		for ( l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter )
		{		
			for(SP_ListGraphic::const_iterator l_itGr = (*l_Iter)->GetGraphics()->begin();
					l_itGr != (*l_Iter)->GetGraphics()->end();l_itGr++)
			{
				if((*l_itGr)->GetPosX()>l_NetMaxX)
					l_NetMaxX=(*l_itGr)->GetPosX();
				if((*l_itGr)->GetPosX()<l_NetMinX)
					l_NetMinX=(*l_itGr)->GetPosX();			
			}			
		}
	}

	l_pcNodeclass= m_pcGraph->GetNodeclass( SP_DS_CONTINUOUS_PLACE );
	if(l_pcNodeclass)
	{
		l_plElements= l_pcNodeclass->GetElements();
		for ( l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter )
		{		
			for(SP_ListGraphic::const_iterator l_itGr = (*l_Iter)->GetGraphics()->begin();
					l_itGr != (*l_Iter)->GetGraphics()->end();l_itGr++)
			{
				if((*l_itGr)->GetPosX()>l_NetMaxX)
					l_NetMaxX=(*l_itGr)->GetPosX();
				if((*l_itGr)->GetPosX()<l_NetMinX)
					l_NetMinX=(*l_itGr)->GetPosX();			
			}			
		}
	}
	
	return l_NetMaxX-l_NetMinX;
}





void SP_DS_ColPN_ExportUnfolding::Initialize()
{
	m_mOldPlaceNodes.clear();	
	m_mOldTransitionNodes.clear();	

	m_mNewDiscretePlaceNodes.clear();
	m_mNewContinuousPlaceNodes.clear();
	m_mNewDiscreteTransitionNodes.clear();
	m_mNewContinuousTransitionNodes.clear();
	m_mNewImmediateTransitionNodes.clear();
	m_mNewDeterministicTransitionNodes.clear();
	m_mNewScheduledTransitionNodes.clear();

	SP_DS_Nodeclass* l_pcNodeclass;
	const SP_ListNode* l_plElements;
	SP_ListNode::const_iterator l_Iter;
	
	//store the old places
	l_pcNodeclass= m_pcGraph->GetNodeclass( SP_DS_DISCRETE_PLACE);		
	if(l_pcNodeclass)
	{
		l_plElements= l_pcNodeclass->GetElements();
		for ( l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter )
		{			
			wxString l_sNodeName;
			l_sNodeName = dynamic_cast<SP_DS_NameAttribute*>((*l_Iter)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();			
			m_mOldPlaceNodes[l_sNodeName] = (*l_Iter);			
		}	
	}

	l_pcNodeclass= m_pcGraph->GetNodeclass( SP_DS_CONTINUOUS_PLACE);		
	if(l_pcNodeclass)
	{
		l_plElements= l_pcNodeclass->GetElements();
		for ( l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter )
		{
			wxString l_sNodeName;
			l_sNodeName = dynamic_cast<SP_DS_NameAttribute*>((*l_Iter)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();			
			m_mOldPlaceNodes[l_sNodeName] = (*l_Iter);			
		}	
	}


	//store the old transitions
	GetOldTransitons(SP_DS_STOCHASTIC_TRANS);
	GetOldTransitons(SP_DS_CONTINUOUS_TRANS);
	GetOldTransitons(wxT("Immediate Transition"));
	GetOldTransitons(wxT("Deterministic Transition"));
	GetOldTransitons(wxT("Scheduled Transition"));	

	m_nNetSize = GetNetSize();

}

void SP_DS_ColPN_ExportUnfolding::GetOldTransitons(wxString p_sNodeClass)
{
	SP_DS_Nodeclass* l_pcNodeclass;
	const SP_ListNode* l_plElements;
	SP_ListNode::const_iterator l_Iter;
	SP_ListEdge::const_iterator l_itEdges;

	l_pcNodeclass= m_pcGraph->GetNodeclass(p_sNodeClass);
	if(l_pcNodeclass)
	{
		l_plElements= l_pcNodeclass->GetElements();

		for ( l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter )
		{
			wxString l_sNodeName;
			l_sNodeName = dynamic_cast<SP_DS_NameAttribute*>((*l_Iter)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();			
			m_mOldTransitionNodes[l_sNodeName] = (*l_Iter);	

			for (l_itEdges = (*l_Iter)->GetTargetEdges()->begin(); l_itEdges != (*l_Iter)->GetTargetEdges()->end(); ++l_itEdges)
			{
				if ((*l_itEdges)->GetSource())
				{					
					SP_DS_Node* l_pcPlaceNode = dynamic_cast<SP_DS_Node*>((*l_itEdges)->GetSource());					
					SP_DS_Edge* l_pcEdge = *l_itEdges;	
					wxString l_sPlaceNodeName = dynamic_cast<SP_DS_NameAttribute*>(l_pcPlaceNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
					wxString l_sEdge = l_sPlaceNodeName + wxT("+") + l_sNodeName + wxT("+") + l_pcEdge->GetClassName();   //source + target as an edge name
					m_mOldEdges[l_sEdge] = l_pcEdge;					
				}
			}
			for (l_itEdges = (*l_Iter)->GetSourceEdges()->begin(); l_itEdges != (*l_Iter)->GetSourceEdges()->end(); ++l_itEdges)
			{
				if ((*l_itEdges)->GetTarget())
				{					
					SP_DS_Node* l_pcPlaceNode = dynamic_cast<SP_DS_Node*>((*l_itEdges)->GetTarget());					
					SP_DS_Edge* l_pcEdge = *l_itEdges;	
					wxString l_sPlaceNodeName = dynamic_cast<SP_DS_NameAttribute*>(l_pcPlaceNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
					wxString l_sEdge = l_sNodeName + wxT("+") + l_sPlaceNodeName + wxT("+") + l_pcEdge->GetClassName();
					m_mOldEdges[l_sEdge] = l_pcEdge;						
				}
			}
		}	
	}
}



bool SP_DS_ColPN_ExportUnfolding::CreateDuplicatePlaceInstances()
{
	if( ! CreateDuplicatePlaceNodeType(m_mmUnfoldedDiscPlaces,SP_DS_DISCRETE_PLACE,m_mNewDiscretePlaceNodes) )
		return false;

	if( ! CreateDuplicatePlaceNodeType(m_mmUnfoldedContPlaces,SP_DS_CONTINUOUS_PLACE,m_mNewContinuousPlaceNodes) )
		return false;

	return true;
}



bool SP_DS_ColPN_ExportUnfolding::CreateDuplicatePlaceNodeType( SP_CPN_UnfoldedPlaces& p_mmUnfoldedPlaces, 
															    wxString p_sNodeClass, map<wxString, SP_DS_Node*>& p_mNewPlaceNodes)
{
	SP_CPN_UnfoldedPlaces::iterator itPlaceMap; 
	for(itPlaceMap = p_mmUnfoldedPlaces.begin(); itPlaceMap != p_mmUnfoldedPlaces.end(); itPlaceMap++)
	{
		wxString l_sOldPlaceName = itPlaceMap->first;
		SP_DS_Node* l_pcOldNode = m_mOldPlaceNodes.find(l_sOldPlaceName)->second;

		int l_nInstCount = 0;
		SP_CPN_UnfoldedPlace::iterator itUnfoldedPlaceMap;
		for(itUnfoldedPlaceMap = itPlaceMap->second.begin(); itUnfoldedPlaceMap != itPlaceMap->second.end(); itUnfoldedPlaceMap++)
		{
			//compose the unfolded name
			wxString l_sColor = itUnfoldedPlaceMap->first;
			wxString l_sUnfoldedPlaceName;
			l_sUnfoldedPlaceName = l_sOldPlaceName + wxT("_")+ l_sColor;			
			ModifyName(l_sUnfoldedPlaceName);

			//create an unfolding node
			SP_DS_Nodeclass* l_pcNewNC = m_pcGraph->GetNodeclass( p_sNodeClass );
			SP_DS_Node* l_pcNewNode = l_pcOldNode->Clone();
			l_pcNewNC->AddElement(l_pcNewNode);

			l_pcNewNode->GetAttribute(wxT("Name"))->SetValueString( l_sUnfoldedPlaceName );

			SP_ListGraphic::const_iterator l_itOldGr = l_pcOldNode->GetGraphics()->begin();
			SP_ListGraphic::const_iterator l_itNewGr = l_pcNewNode->GetGraphics()->begin();
			for(; l_itOldGr != l_pcOldNode->GetGraphics()->end()
					&& l_itNewGr != l_pcNewNode->GetGraphics()->end()
				; ++l_itOldGr, ++l_itNewGr)
			{
				SP_Graphic* l_pcOldGr = *l_itOldGr;
				SP_Graphic* l_pcNewGr = *l_itNewGr;

				//l_pcNewGr->SetPosX(l_pcOldGr->GetPosX()+l_nInstCount*(m_nNetSize+50.0)); // it should be reasonablely set.
				//l_pcNewGr->SetPosY(l_pcOldGr->GetPosY());
				//bugfix:by george
				l_pcNewGr->SetPosX(350.0 ); // it should be reasonablely set.
				l_pcNewGr->SetPosY(250.0);

				m_mGraphic2PlaceNode[l_pcNewGr] = l_pcNewNode;
			}
			l_nInstCount++;

			//assign initial marking

			//set marking for colepn and colpn
			if( m_sNetClass ==  SP_DS_COLPN_CLASS || m_sNetClass ==  SP_DS_COLEPN_CLASS  ||
				 m_sNetClass ==  SP_DS_COLSPN_CLASS || m_sNetClass ==  SP_DS_COLCPN_CLASS || m_sNetClass ==  SP_DS_COLHPN_CLASS)
			{
				wxString l_sMarking = wxT("");					
				if( p_sNodeClass == SP_DS_DISCRETE_PLACE && itUnfoldedPlaceMap->second.m_anNetMarkings.size() > 0 )
				{			
					long l_nMarking = 0;
					l_nMarking = itUnfoldedPlaceMap->second.m_anNetMarkings[0];
					l_sMarking << l_nMarking;
				}	
				if( p_sNodeClass == SP_DS_CONTINUOUS_PLACE && itUnfoldedPlaceMap->second.m_adNetMarkings.size() > 0 )
				{			
					double l_dMarking = 0;
					l_dMarking = itUnfoldedPlaceMap->second.m_adNetMarkings[0];
					l_sMarking << l_dMarking;
				}	
				SP_ListAttribute::const_iterator l_Iter;
				const SP_ListAttribute* l_plAttributes = l_pcNewNode->GetAttributes();

				for (l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter)
				{		
					if (( *l_Iter )->GetName() == wxT("Marking"))
					{						
						SP_DS_Attribute* l_pcAttribute = *l_Iter;
						if(p_sNodeClass == SP_DS_DISCRETE_PLACE)
						{
							SP_DS_MarkingAttribute* l_pcAttr = dynamic_cast< SP_DS_MarkingAttribute* >(l_pcAttribute);
							l_pcAttr->SetValueString(l_sMarking, false);
						}
						else
						{
							SP_DS_DoubleMarkingAttribute* l_pcAttr = dynamic_cast< SP_DS_DoubleMarkingAttribute* >(l_pcAttribute);
							l_pcAttr->SetValueString(l_sMarking, false);
						}
						break;
					}
				}
			}

			//Set marking for colocpn and colspn
/*			if( m_sNetClass ==  SP_DS_COLSPN_CLASS || m_sNetClass ==  SP_DS_COLCPN_CLASS || m_sNetClass ==  SP_DS_COLHPN_CLASS)
			{
				//collect marking sets and get the marking attribute pointer
				SP_ListAttribute::const_iterator l_Iter;
				const SP_ListAttribute* l_plAttributes = l_pcNewNode->GetAttributes();
				vector<wxString> l_sMarkingSetVector;
				SP_DS_Attribute *l_pcMarkingAttr = NULL;
				for (l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter)
				{		
					if (( *l_Iter )->GetName() == SP_DS_CPN_MARKINGLIST ) 
					{
						SP_DS_Attribute* l_pcAttribute = *l_Iter;
						SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcAttribute);	
						for(unsigned i = 1; i < l_pcColList->GetColCount(); i++)
						{
							wxString l_sColLabel = l_pcColList->GetColLabel(i);
							l_sColLabel = l_sColLabel.BeforeFirst(wxT(':'));
							l_sMarkingSetVector.push_back( l_sColLabel );
							i++;
						}
					}
					if (( *l_Iter )->GetName() == wxT("Marking") )
					{
						l_pcMarkingAttr = (*l_Iter);				
					}
				}

				//put the marking (long or double) in a double vector
				vector<double> l_dvMarking;
				vector<long> l_nvMarking;
				wxString l_sMarking = wxT("");
				if( p_sNodeClass == SP_DS_DISCRETE_PLACE )
				{
					for(unsigned int i= 0; i < itUnfoldedPlaceMap->second.m_anNetMarkings.size(); i++ )
					{	
						l_nvMarking = itUnfoldedPlaceMap->second.m_anNetMarkings;
						l_sMarking = wxString::Format(wxT("%d"),l_nvMarking[0]);
					}	
				}
				else
				{
					for(unsigned int i= 0; i < itUnfoldedPlaceMap->second.m_adNetMarkings.size(); i++ )
					{					
						l_dvMarking = itUnfoldedPlaceMap->second.m_adNetMarkings;
						l_sMarking = wxString::Format(wxT("%f"),l_dvMarking[0]);
					}
				}
				//assign the marking number in the marking attribute								
				l_pcMarkingAttr->SetValueString(l_sMarking);					

				//Assign the marking sets to the collist
				SP_DS_Attribute* l_pcAttr = l_pcNewNode->GetAttribute(wxT("MarkingList"));
				l_pcAttr->SetName(wxT("OldMarkingList"));
				//it is tricky here, we may not do it in other ways
				SP_DS_ColListAttribute* l_pcColList = new SP_DS_ColListAttribute( wxT("MarkingList"), SP_COLLIST_UNSIGNED_INTEGER, 2 );

				l_pcColList->SetColLabel( 0, wxT("Marking set") );
				l_pcColList->SetColLabel( 1, wxT("Marking") );	

				unsigned int l_nMarkingSetNum;
				if( p_sNodeClass == SP_DS_DISCRETE_PLACE )
					l_nMarkingSetNum = l_sMarkingSetVector.size()>=l_nvMarking.size()? l_nvMarking.size(): l_sMarkingSetVector.size();
				else
					l_nMarkingSetNum = l_sMarkingSetVector.size()>=l_dvMarking.size()? l_dvMarking.size(): l_sMarkingSetVector.size();

				for(unsigned k = 0; k < l_nMarkingSetNum; k++)
				{
					unsigned int l_nNewRow = l_pcColList->AppendEmptyRow();
					l_pcColList->SetCell(l_nNewRow, 0, l_sMarkingSetVector[k]);
					if( p_sNodeClass == SP_DS_DISCRETE_PLACE )
						l_pcColList->SetCell(l_nNewRow, 1, wxString::Format(wxT("%d"),l_nvMarking[k]));
					else
						l_pcColList->SetCell(l_nNewRow, 1, wxString::Format(wxT("%f"),l_dvMarking[k]));
				}					
				l_pcNewNode->AddAttribute(l_pcColList);	
			}
*/
			p_mNewPlaceNodes[ l_sUnfoldedPlaceName ] = l_pcNewNode;		
		}
	}		

	return true;
}



bool SP_DS_ColPN_ExportUnfolding::CreateDuplicateTransInstances()
{
	if( ! CreateDuplicateTransNodeType(m_mmUnfoldedStochTransions,SP_DS_STOCHASTIC_TRANS,m_mNewDiscreteTransitionNodes) )
		return false;

	if( ! CreateDuplicateTransNodeType(m_mmUnfoldedContTransions,SP_DS_CONTINUOUS_TRANS,m_mNewContinuousTransitionNodes) )
		return false;

	if( ! CreateDuplicateTransNodeType(m_mmUnfoldedImmTransions,wxT("Immediate Transition"),m_mNewImmediateTransitionNodes) )
		return false;

	if( ! CreateDuplicateTransNodeType(m_mmUnfoldedDetTransions,wxT("Deterministic Transition"),m_mNewDeterministicTransitionNodes) )
		return false;

	if( ! CreateDuplicateTransNodeType(m_mmUnfoldedSchedTransions,wxT("Scheduled Transition"),m_mNewScheduledTransitionNodes) )
		return false;

	return true; 		    
}



bool SP_DS_ColPN_ExportUnfolding::CreateDuplicateTransNodeType( SP_CPN_UnfoldedTransitions& p_mmTransitions, 
		                               wxString p_sNodeClass, map<wxString, SP_DS_Node*>& p_mNewTransNodes)
{
	SP_CPN_UnfoldedTransitions::iterator itTransMap; 
	for(itTransMap = p_mmTransitions.begin(); itTransMap != p_mmTransitions.end(); itTransMap++)
	{		
		wxString l_sOldTransName = itTransMap->first;
		
		SP_DS_Node* l_pcOldNode = m_mOldTransitionNodes.find(l_sOldTransName)->second;

		int l_nInstCount = 0;
		SP_CPN_UnfoldedTransition::iterator itUnfoldedTransMap;
		for(itUnfoldedTransMap = itTransMap->second.begin(); itUnfoldedTransMap != itTransMap->second.end(); itUnfoldedTransMap++)
		{
			//get the new name
			wxString l_sInstance = itUnfoldedTransMap->first;
			wxString l_sNewTransName = l_sOldTransName + wxT("_")+ l_sInstance;
			ModifyName(l_sNewTransName);

			//create a new node
			SP_DS_Nodeclass* l_pcNewNC = m_pcGraph->GetNodeclass( p_sNodeClass );
			SP_DS_Node* l_pcNewNode = l_pcOldNode->Clone();
			l_pcNewNC->AddElement(l_pcNewNode);

			l_pcNewNode->GetAttribute(wxT("Name"))->SetValueString( l_sNewTransName );

			SP_ListGraphic::const_iterator l_itOldGr = l_pcOldNode->GetGraphics()->begin();
			SP_ListGraphic::const_iterator l_itNewGr = l_pcNewNode->GetGraphics()->begin();
			for(; l_itOldGr != l_pcOldNode->GetGraphics()->end()
					&& l_itNewGr != l_pcNewNode->GetGraphics()->end()
				; ++l_itOldGr, ++l_itNewGr)
			{
				SP_Graphic* l_pcOldGr = *l_itOldGr;
				SP_Graphic* l_pcNewGr = *l_itNewGr;

				//commented by george, here causes crash, the big unfolded net will cause tresspassing the canvas
				///l_pcNewGr->SetPosX(l_pcOldGr->GetPosX()+l_nInstCount*(m_nNetSize+50.0)); // it should be reasonablely set.
				//l_pcNewGr->SetPosY(l_pcOldGr->GetPosY());
				l_pcNewGr->SetPosX(300);//george
				l_pcNewGr->SetPosX( 200);//george

				m_mGraphic2TransNode[l_pcNewGr] = l_pcNewNode;
			}
			l_nInstCount++;

			//add rate functions
			if( m_sNetClass ==  SP_DS_COLSPN_CLASS || m_sNetClass ==  SP_DS_COLCPN_CLASS || m_sNetClass ==  SP_DS_COLHPN_CLASS)
			{
				//get the function sets
				SP_ListAttribute::const_iterator l_Iter;
				const SP_ListAttribute* l_plAttributes = l_pcNewNode->GetAttributes();
				vector<wxString> l_sFunctionSetVector;
				for (l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter)
				{		
					if (( *l_Iter )->GetName() == wxT("FunctionList") || 
						 ( *l_Iter )->GetName() == wxT("DelayList") ||
						 ( *l_Iter )->GetName() == wxT("PeriodicList") )						 
					{
						SP_DS_Attribute* l_pcAttribute = *l_Iter;
						SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcAttribute);					
			
						for(unsigned i = 1; i < l_pcColList->GetColCount(); i++)
						{
							wxString l_sFuncSet = l_pcColList->GetColLabel(i);
							l_sFuncSet = l_sFuncSet.BeforeFirst(wxT(':'));
							l_sFunctionSetVector.push_back(l_sFuncSet);
						}
						break;
					}					
				}

				//get rate functions
				vector<wxString> l_vsOriginalRateFunctions;
				l_vsOriginalRateFunctions = itUnfoldedTransMap->second.m_anNetFunctions;
				vector<wxString> l_vsRateFunctions;
				for(unsigned int i = 0; i < l_vsOriginalRateFunctions.size(); i++)
				{
					wxString l_sRate = l_vsOriginalRateFunctions[i];
					/*
					if(p_sNodeClass == wxT("Immediate Transition") || (p_sNodeClass == wxT("Deterministic Transition")))
					{
						l_sRate = l_sRate.AfterFirst(wxT('('));
						l_sRate = l_sRate.BeforeLast(wxT(')'));
						l_vsRateFunctions.push_back(l_sRate);
					}
					*/
					if(p_sNodeClass == wxT("Scheduled Transition"))
					{
						l_sRate = l_sRate.AfterFirst(wxT('('));
						l_sRate = l_sRate.BeforeLast(wxT(')'));
						wxString l_sBegin = l_sRate.BeforeFirst(wxT(','));
						l_sRate = l_sRate.AfterFirst(wxT(','));
						wxString l_sRepetition = l_sRate.BeforeFirst(wxT(','));
						wxString l_sEnd = l_sRate.AfterFirst(wxT(','));
						l_vsRateFunctions.push_back(l_sBegin);
						l_vsRateFunctions.push_back(l_sRepetition);
						l_vsRateFunctions.push_back(l_sEnd);
					}
					else
					{
						l_vsRateFunctions.push_back(l_sRate);
					}
				}

				unsigned int l_nCount;
				if(p_sNodeClass == wxT("Scheduled Transition"))
					l_nCount = l_sFunctionSetVector.size() >= l_vsRateFunctions.size() ? l_vsRateFunctions.size()/3:l_sFunctionSetVector.size()/3;
				else
					l_nCount = l_sFunctionSetVector.size() >= l_vsRateFunctions.size() ? l_vsRateFunctions.size():l_sFunctionSetVector.size();

				//write rate functions
				SP_DS_ColListAttribute* l_pcColList;
				if ( p_sNodeClass == SP_DS_STOCHASTIC_TRANS || p_sNodeClass == SP_DS_CONTINUOUS_TRANS || p_sNodeClass == wxT("Immediate Transition") )
				{
					SP_DS_Attribute* l_pcAttr = l_pcNewNode->GetAttribute(wxT("FunctionList"));
					l_pcAttr->RegisterDialogWidget( new SP_WDG_StFunctionList( wxT("Function") ) );
					l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcAttr);					
					l_pcColList->Clear();	
					l_pcColList->RemoveAllColumns();
					l_pcColList->AppendEmptyColum();
					l_pcColList->AppendEmptyColum();

					if(p_sNodeClass == wxT("Immediate Transition"))
					{
						l_pcColList->SetColLabel( 0, wxT("Weight set") );
						l_pcColList->SetColLabel( 1, wxT("Weight") );
					}					
					else
					{
						l_pcColList->SetColLabel( 0, wxT("Function set") );
						l_pcColList->SetColLabel( 1, wxT("Function") );
					}
				}
				else if(p_sNodeClass == wxT("Deterministic Transition"))
				{
					SP_DS_Attribute* l_pcAttr = l_pcNewNode->GetAttribute(wxT("DelayList"));
					l_pcAttr->RegisterDialogWidget( new SP_WDG_DialogColList( wxT("Delay") ) );
					l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcAttr);
					l_pcColList->Clear();	
					l_pcColList->RemoveAllColumns();
					l_pcColList->AppendEmptyColum();
					l_pcColList->AppendEmptyColum();

					l_pcColList->SetColLabel( 0, wxT("Delay set") );
					l_pcColList->SetColLabel( 1, wxT("Delay") );
				}
				else if(p_sNodeClass == wxT("Scheduled Transition"))
				{
					SP_DS_Attribute* l_pcAttr = l_pcNewNode->GetAttribute(wxT("PeriodicList"));
					l_pcAttr->RegisterDialogWidget( new SP_WDG_DialogColList( wxT("Periodic") ) );
					l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcAttr);
					l_pcColList->Clear();	
					l_pcColList->RemoveAllColumns();
					l_pcColList->AppendEmptyColum();
					l_pcColList->AppendEmptyColum();
					l_pcColList->AppendEmptyColum();
					l_pcColList->AppendEmptyColum();

					l_pcColList->SetColLabel( 0, wxT("Periodic List"));
					l_pcColList->SetColLabel( 1, wxT("Begin"));
					l_pcColList->SetColLabel( 2, wxT("Repetition"));
					l_pcColList->SetColLabel( 3, wxT("End"));
				}

				for(unsigned j = 0; j < l_nCount; j++)
				{
					unsigned int l_nNewRow = l_pcColList->AppendEmptyRow();
					wxString l_sRowLabel = l_sFunctionSetVector[j];						
					l_pcColList->SetCell(l_nNewRow, 0, l_sRowLabel);
					if(p_sNodeClass == wxT("Scheduled Transition"))
					{
						l_pcColList->SetCell(l_nNewRow, 1, l_vsRateFunctions[3*j]);	
						l_pcColList->SetCell(l_nNewRow, 2, l_vsRateFunctions[3*j+1]);	
						l_pcColList->SetCell(l_nNewRow, 3, l_vsRateFunctions[3*j+2]);	
						j++;
						j++;
					}
					else
					{										
						l_pcColList->SetCell(l_nNewRow, 1, l_vsRateFunctions[j]);	
					}										
				}
			}

			//edges
			for(unsigned int i=0; i < itUnfoldedTransMap->second.m_vInputArcs.size(); i++)
			{
				if( ! CreateDuplicateEdge(l_pcOldNode, l_pcNewNode, itUnfoldedTransMap->second.m_vInputArcs[i],true) )
					return false;
			}
			for(unsigned int i=0; i < itUnfoldedTransMap->second.m_vOutputArcs.size(); i++)
			{
				if( ! CreateDuplicateEdge(l_pcOldNode, l_pcNewNode, itUnfoldedTransMap->second.m_vOutputArcs[i], false) )
					return false;
			}

			//add a new node
			p_mNewTransNodes[l_sNewTransName] = l_pcNewNode;
		}
	}

	return true;
}


bool SP_DS_ColPN_ExportUnfolding::CreateDuplicateEdge(SP_DS_Node* p_pcTransOldNode, SP_DS_Node* p_pcTransNewNode, SP_CPN_UnfoldedArcInfo& p_UnfoldedArcInfo, bool p_bInput)
{
	SP_DS_Node* l_newTransitionNode = p_pcTransNewNode;					
	SP_DS_Node* l_OldTransNode = p_pcTransOldNode;

	wxString l_sEdgeClassName = p_UnfoldedArcInfo.m_sArcType;

	wxString l_sColPlaceName = p_UnfoldedArcInfo.m_sColPlaceName;
	wxString l_sUnfoldedPlaceName = l_sColPlaceName + wxT("_") + p_UnfoldedArcInfo.m_sColor;
	ModifyName( l_sUnfoldedPlaceName );

	SP_DS_Node* l_OldPlaceNode = m_mOldPlaceNodes[l_sColPlaceName];
	SP_DS_Node* l_newPlaceNode;
	if( m_mNewDiscretePlaceNodes.find(l_sUnfoldedPlaceName) != m_mNewDiscretePlaceNodes.end() )
	{
		l_newPlaceNode = m_mNewDiscretePlaceNodes.find(l_sUnfoldedPlaceName)->second;
	}
	else
	{
		l_newPlaceNode = m_mNewContinuousPlaceNodes.find(l_sUnfoldedPlaceName)->second;
	}

	wxString l_sColTransName = dynamic_cast<SP_DS_NameAttribute*>(l_OldTransNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

	wxString l_sEdge;
	if(p_bInput)
		l_sEdge = l_sColPlaceName + wxT("+") + l_sColTransName;
	else
		l_sEdge = l_sColTransName + wxT("+") + l_sColPlaceName;

	l_sEdge = l_sEdge + wxT("+") + l_sEdgeClassName;

	SP_DS_Edge* l_pcOldEdge = m_mOldEdges[l_sEdge];

	//Map graphics of old and new places	
	map<SP_Graphic*,SP_Graphic*> l_mOld2NewPlaceGraphics;
	SP_ListGraphic::const_iterator l_itGrNewPlace = l_newPlaceNode->GetGraphics()->begin();
	SP_ListGraphic::const_iterator l_itGrOldPlace = l_OldPlaceNode->GetGraphics()->begin();

	while(l_itGrOldPlace != l_OldPlaceNode->GetGraphics()->end()  && l_itGrNewPlace != l_newPlaceNode->GetGraphics()->end() )
	{
		l_mOld2NewPlaceGraphics[*l_itGrOldPlace] = *l_itGrNewPlace;
		l_itGrNewPlace++;
		l_itGrOldPlace++;
	}

	//Map graphics of old and new transitions
	map<SP_Graphic*,SP_Graphic*> l_mOld2NewTransGraphics;
	SP_ListGraphic::const_iterator l_itGrNewTrans = l_newTransitionNode->GetGraphics()->begin();
	SP_ListGraphic::const_iterator l_itGrOldTrans = l_OldTransNode->GetGraphics()->begin();

	while(l_itGrOldTrans != l_OldTransNode->GetGraphics()->end()  && l_itGrNewTrans != l_newTransitionNode->GetGraphics()->end() )
	{
		l_mOld2NewTransGraphics[*l_itGrOldTrans] = *l_itGrNewTrans;
		l_itGrNewTrans++;
		l_itGrOldTrans++;
	}

	//set graphics
	SP_DS_Edgeclass* l_pcNewNC; 					
	l_pcNewNC=l_pcOldEdge->GetEdgeclass();	
	SP_DS_Edge* l_pcNewEdge = l_pcOldEdge->Clone(); 					
	l_pcNewNC->AddElement(l_pcNewEdge);								

	if(p_bInput)
	{
		l_pcNewEdge->SetNodes(l_newPlaceNode, l_newTransitionNode);
	}
	else
	{
		l_pcNewEdge->SetNodes(l_newTransitionNode, l_newPlaceNode);
	}
	SP_ListGraphic::const_iterator l_itGrOldEdge = l_pcOldEdge->GetGraphics()->begin();
	SP_ListGraphic::const_iterator l_itGrNewEdge = l_pcNewEdge->GetGraphics()->begin();

	for(; l_itGrOldEdge != l_pcOldEdge->GetGraphics()->end()
			&& l_itGrNewEdge != l_pcNewEdge->GetGraphics()->end()
		; ++l_itGrOldEdge, ++l_itGrNewEdge)
	{
		SP_GR_Edge* l_pcOldGraphic = dynamic_cast<SP_GR_Edge*>(*l_itGrOldEdge);
		SP_GR_Edge* l_pcNewGraphic = dynamic_cast<SP_GR_Edge*>(*l_itGrNewEdge);
		SP_Graphic* l_sSource = NULL;
		SP_Graphic* l_sTarget = NULL;
		if(p_bInput)
		{
			l_sSource = l_mOld2NewPlaceGraphics[l_pcOldGraphic->GetSource()];
			l_sTarget = l_mOld2NewTransGraphics[l_pcOldGraphic->GetTarget()];
			m_mGraphic2PlaceNode.erase(l_sSource);
			m_mGraphic2TransNode.erase(l_sTarget);
		}
		else
		{
			l_sTarget = l_mOld2NewPlaceGraphics[l_pcOldGraphic->GetTarget()];
			l_sSource = l_mOld2NewTransGraphics[l_pcOldGraphic->GetSource()];
			m_mGraphic2PlaceNode.erase(l_sTarget);
			m_mGraphic2TransNode.erase(l_sSource);
		}
		if(!l_sSource && l_pcOldGraphic->GetSource()->GetGraphicState() == SP_STATE_COARSECOARSE)
			l_sSource = l_pcOldGraphic->GetSource();
		if(!l_sTarget && l_pcOldGraphic->GetTarget()->GetGraphicState() == SP_STATE_COARSECOARSE)
			l_sTarget = l_pcOldGraphic->GetTarget();
		l_pcNewGraphic->SetSource(l_sSource);
		l_pcNewGraphic->SetTarget(l_sTarget);
		wxList* l_pcList = dynamic_cast<wxLineShape*>(l_pcOldGraphic->GetPrimitive())->GetLineControlPoints();
		l_pcNewGraphic->SetControlPoints(l_pcList);
	}

	//assign multiplicity
	if( !(p_UnfoldedArcInfo.m_sArcType == wxT("Modifier Edge") || p_UnfoldedArcInfo.m_sArcType == wxT("Reset Edge")) )
	{
		SP_DS_Attribute* l_pcAttr = l_pcNewEdge->GetAttribute(wxT("Multiplicity"));	
		wxString l_sMultiplicity;
		if(p_UnfoldedArcInfo.m_sDiscContType == wxT("Place"))
		{
			//l_sMultiplicity = wxString::Format(wxT("%ld"),p_UnfoldedArcInfo.m_nMultiplicity);
			l_sMultiplicity = p_UnfoldedArcInfo.m_sMultiplicity;
		}
		else
		{
			//l_sMultiplicity = wxString::Format(wxT("%f"),p_UnfoldedArcInfo.m_dMultiplicity);
			l_sMultiplicity = p_UnfoldedArcInfo.m_sMultiplicity;
		}	

		//if( m_sNetClass !=  SP_DS_COLHPN_CLASS )
		{
			
			l_pcAttr->SetValueString( l_sMultiplicity );
			l_pcAttr->SetShow(TRUE);//by george, to show the multiplicity 
			l_pcAttr->Update(TRUE);//by george
		}
		//else
		{		
		//	dynamic_cast< SP_DS_DoubleMultiplicityAttribute* >(l_pcAttr)->SetValueString(	l_sMultiplicity );		
		}
	}

	//add a new edge
	m_lNewEdges.push_back(l_pcNewEdge);

	wxString l_sNetClassName = l_pcNewEdge->GetClassName();
	if( l_sNetClassName == SP_DS_EDGE )
	{
		m_nOrdinaryArcNum++;
	}
	else if( l_sNetClassName == SP_DS_READ_EDGE )
	{
		m_nReadArcNum++;
	}
	else if( l_sNetClassName == SP_DS_INHIBITOR_EDGE )
	{
		m_nInhibitorArcNum++;
	}
	else if( l_sNetClassName == SP_DS_EQUAL_EDGE )
	{
		m_nEqualArcNum++;
	}
	else if( l_sNetClassName == SP_DS_RESET_EDGE )
	{
		m_nResetArcNum++;
	}
	else if( l_sNetClassName == SP_DS_MODIFIER_EDGE )
	{			
		m_nModifierArcNum++;
	}	


	return true;
}










void SP_DS_ColPN_ExportUnfolding::RemoveIsolatedNoed( map<wxString, SP_DS_Node*>* p_pmNodes )
{
	map<wxString, SP_DS_Node*> l_mRemovedNodes;
	map<wxString, SP_DS_Node*>::iterator itMap;
	
	for(itMap = p_pmNodes->begin(); itMap != p_pmNodes->end(); itMap++)
	{
		SP_DS_Node* l_pcNode = itMap->second;

	//look for isolated nodes
		if (l_pcNode->GetSourceEdges()->size() == 0 && l_pcNode->GetTargetEdges()->size() == 0)
		{
			l_mRemovedNodes[itMap->first] = itMap->second;
		}	
	}

	for(itMap = l_mRemovedNodes.begin(); itMap != l_mRemovedNodes.end(); itMap++)
	{
		p_pmNodes->erase( itMap->first );
		itMap->second->GetNodeclass()->RemoveElement(itMap->second);
		wxDELETE(itMap->second);
	}	

	return;
}


void SP_DS_ColPN_ExportUnfolding::Reset()
{
	SP_DS_Nodeclass* l_pcNodeclass;
	
	l_pcNodeclass= m_pcGraph->GetNodeclass(wxT("Place"));
	map<wxString, SP_DS_Node*>::iterator itMap;
	for( itMap = m_mNewDiscretePlaceNodes.begin(); itMap != m_mNewDiscretePlaceNodes.end(); itMap++ )	
	{
		SP_DS_Node *l_pcNode = itMap->second;
		l_pcNodeclass->RemoveElement(l_pcNode);
		wxDELETE(l_pcNode);
	}

	l_pcNodeclass= m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_PLACE);	
	for( itMap = m_mNewContinuousPlaceNodes.begin(); itMap != m_mNewContinuousPlaceNodes.end(); itMap++ )	
	{
		SP_DS_Node *l_pcNode = itMap->second;
		l_pcNodeclass->RemoveElement(l_pcNode);
		wxDELETE(l_pcNode);
	}

	l_pcNodeclass= m_pcGraph->GetNodeclass(wxT("Transition"));

	for( itMap = m_mNewDiscreteTransitionNodes.begin(); itMap != m_mNewDiscreteTransitionNodes.end(); itMap++ )	
	{
		SP_DS_Node *l_pcNode = itMap->second;
		l_pcNodeclass->RemoveElement(l_pcNode);
		wxDELETE(l_pcNode);
	}

	l_pcNodeclass= m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_TRANS);

	for( itMap = m_mNewContinuousTransitionNodes.begin(); itMap != m_mNewContinuousTransitionNodes.end(); itMap++ )	
	{
		SP_DS_Node *l_pcNode = itMap->second;
		l_pcNodeclass->RemoveElement(l_pcNode);
		wxDELETE(l_pcNode);
	}

	l_pcNodeclass= m_pcGraph->GetNodeclass(wxT("Immediate Transition"));

	for( itMap = m_mNewImmediateTransitionNodes.begin(); itMap != m_mNewImmediateTransitionNodes.end(); itMap++ )	
	{
		SP_DS_Node *l_pcNode = itMap->second;
		l_pcNodeclass->RemoveElement(l_pcNode);
		wxDELETE(l_pcNode);
	}

	l_pcNodeclass= m_pcGraph->GetNodeclass(wxT("Deterministic Transition"));

	for( itMap = m_mNewDeterministicTransitionNodes.begin(); itMap != m_mNewDeterministicTransitionNodes.end(); itMap++ )	
	{
		SP_DS_Node *l_pcNode = itMap->second;
		l_pcNodeclass->RemoveElement(l_pcNode);
		wxDELETE(l_pcNode);
	}

	l_pcNodeclass= m_pcGraph->GetNodeclass(wxT("Scheduled Transition"));

	for( itMap = m_mNewScheduledTransitionNodes.begin(); itMap != m_mNewScheduledTransitionNodes.end(); itMap++ )	
	{
		SP_DS_Node *l_pcNode = itMap->second;
		l_pcNodeclass->RemoveElement(l_pcNode);
		wxDELETE(l_pcNode);
	}

	SP_DS_Edgeclass* l_pcEdgeclass;	
	for(SP_ListEdge::const_iterator l_it = m_lNewEdges.begin();	l_it != m_lNewEdges.end();l_it++)
	{
		SP_DS_Edge *l_pcEdge = *l_it;

		if( l_pcEdge->GetClassName() == wxT("Edge"))
			l_pcEdgeclass= m_pcGraph->GetEdgeclass(wxT("Edge"));
		if( l_pcEdge->GetClassName() == wxT("Read Edge"))
			l_pcEdgeclass= m_pcGraph->GetEdgeclass(wxT("Read Edge"));
		if( l_pcEdge->GetClassName() == wxT("Inhibitor Edge"))
			l_pcEdgeclass= m_pcGraph->GetEdgeclass(wxT("Inhibitor Edge"));
		if( l_pcEdge->GetClassName() == wxT("Equal Edge"))
			l_pcEdgeclass= m_pcGraph->GetEdgeclass(wxT("Equal Edge"));
		if( l_pcEdge->GetClassName() == wxT("Reset Edge"))
			l_pcEdgeclass= m_pcGraph->GetEdgeclass(wxT("Reset Edge"));
		if( l_pcEdge->GetClassName() == wxT("Modifier Edge"))
			l_pcEdgeclass= m_pcGraph->GetEdgeclass(wxT("Modifier Edge"));

		l_pcEdgeclass->RemoveElement(l_pcEdge);
		wxDELETE(l_pcEdge);
	}

	m_pcGraph->GetParentDoc()->UpdateAllViews();
	
}
