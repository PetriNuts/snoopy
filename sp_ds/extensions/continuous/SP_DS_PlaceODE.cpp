/*
 * SP_DS_PlaceODE.cpp
 * $Author: M.Herajy
 * $Version: 1.0 $
 * $Revision: 0.0 $
 * $Date: 2.6.2010
 * Short Description:
 */
 //======================================================================================
#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/extensions/continuous/SP_DS_ContinuousPlaceFormula.h"
#include "sp_ds/extensions/continuous/SP_DS_PlaceODE.h"

SP_DS_PlaceODE::SP_DS_PlaceODE(SP_DS_Graph* p_pcGraph):m_pcGraph(p_pcGraph)
{

}

SP_DS_PlaceODE::~SP_DS_PlaceODE()
{

}

bool SP_DS_PlaceODE::LoadPlaces()
{
    m_nPlaceCount=0;

	//Load continuous places
    if(!LoadPlacesOfType(SP_DS_CONTINUOUS_PLACE))
	{
	  return false;
	}

 return true;
}

bool SP_DS_PlaceODE::LoadPlacesOfType(const wxString& p_sPlacesType)
{
    SP_DS_Nodeclass* l_pcNodeclass;
    SP_ListNode::const_iterator l_itElem;

    //Get Place Type
    l_pcNodeclass= m_pcGraph->GetNodeclass(p_sPlacesType);
    CHECK_POINTER(l_pcNodeclass,return false);


    long l_nPos = m_nPlaceCount;
    //Increase the places count
	m_nPlaceCount+= l_pcNodeclass->GetElements()->size();

	m_apcPlaceNodes.resize(m_nPlaceCount);
	asPlaceNames.resize(m_nPlaceCount);


	for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
	{

		m_mPlacesPosByID[GetNodeID((*l_itElem))]=l_nPos;

		m_apcPlaceNodes[l_nPos]=(*l_itElem);

		//Place Names
		wxString l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>(m_apcPlaceNodes[l_nPos]->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
		asPlaceNames[l_nPos]=l_sPlaceName;
		l_nPos++;
	}
 return true;
}

bool SP_DS_PlaceODE::LoadTransitions()
{
		//Continuous Transition
		SP_DS_Nodeclass* l_pcNodeContinuousTrans;
		l_pcNodeContinuousTrans= m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_TRANS);
		CHECK_POINTER(l_pcNodeContinuousTrans,return false);

		//Count the net's transitions
		m_nTransCount =l_pcNodeContinuousTrans->GetElements()->size();

		m_anTransPosition.resize(m_nTransCount);
		m_asTransRateFunction.resize(m_nTransCount);
		m_anTransitionsType.resize(m_nTransCount);

		SP_ListNode::const_iterator l_itElem;
		wxString l_sTransitionName;

		SP_DS_ColListAttribute* l_pcColList;

		long l_nPos = 0;

		//Continuous Transition
		for (l_itElem = l_pcNodeContinuousTrans->GetElements()->begin(); l_itElem != l_pcNodeContinuousTrans->GetElements()->end(); ++l_itElem)
		{
			m_anTransPosition[l_nPos]=l_nPos;

			//Removes the pattern from the trnasition's rate function
			l_pcColList	= dynamic_cast< SP_DS_ColListAttribute* >((*l_itElem)->GetAttribute(wxT("FunctionList")) );
			CHECK_POINTER(l_pcColList,return false);

			wxString l_TransRatFunction= l_pcColList->GetActiveCellValue( 1);
			SP_DS_ContinuousPlaceFormula l_continuousParser(l_TransRatFunction);

			m_asTransRateFunction[l_nPos]=wxT("(")
										  + l_continuousParser.ReplacePatternInTransitionFormula((*l_itElem))
                                          +wxT(")");


			m_mTransitionsPosByID[l_nPos]=GetNodeID((*l_itElem));

			m_anTransitionsType[l_nPos]=SP_CONTINUOUS_TRANS;
			l_nPos++;
		}


 return true;
}

bool SP_DS_PlaceODE::LoadConnections()
{
     //Resize pre and post transitions' data structure of the places
      m_a2DPlacePostTransitions.resize(m_nPlaceCount);
      m_a2DPlacePreTransitions.resize(m_nPlaceCount);

	  // fill in the connection Matrices
       for(unsigned long l_nPlacePos=0;l_nPlacePos<m_apcPlaceNodes.size();l_nPlacePos++)
     	{
     	  //Get the place pre and post transitions
    	  if(!FillInPreConnection(m_apcPlaceNodes[l_nPlacePos]) || !FillInPostConnection(m_apcPlaceNodes[l_nPlacePos]) )
    	  {
    		 return false;
    	  }

     	}

  return true;
}

bool SP_DS_PlaceODE::FillInPreConnection(SP_DS_Node* p_pcPlaceNode)
{
      CHECK_POINTER(p_pcPlaceNode,return false);

      //GetNodeID
      long l_nPlaceID=GetNodeID(p_pcPlaceNode);
      long l_nPlacePos=m_mPlacesPosByID[l_nPlaceID];

      SP_ListEdge::const_iterator l_itEdge;

      for (l_itEdge = p_pcPlaceNode->GetTargetEdges()->begin(); l_itEdge!= p_pcPlaceNode->GetTargetEdges()->end(); ++l_itEdge)
      {

    	  SP_DS_Node* l_pcTransNode=dynamic_cast<SP_DS_Node*>((*l_itEdge)->GetSource());

    	  CHECK_POINTER(l_pcTransNode,return false);

    	  wxString l_sTransType=l_pcTransNode->GetClassName();

    	  //we record only connections between places and continuous or stochastic transitions.
    	  if(!(l_sTransType==SP_DS_CONTINUOUS_TRANS || l_sTransType==SP_DS_STOCHASTIC_TRANS))
    		  continue;

    	  long l_nTransID=GetNodeID(l_pcTransNode);
    	  long l_nTransPos=m_mTransitionsPosByID[l_nTransID];

    	  //Set the pre connection type
    	  PlaceTransitions l_Trans;

    	  //Connection type
    	  l_Trans.ConType=GetConnectionType((*l_itEdge));

    	  //Transition's position
    	  l_Trans.nTransPos=l_nTransPos;

    	  SP_DS_Attribute* l_pcMultAttr=GetEdgeAttribute((*l_itEdge));

    	  if (l_Trans.ConType == SP_MODIFIER_ARC) continue;//george

		  CHECK_POINTER(l_pcMultAttr,return false);

		  wxString l_sMult=l_pcMultAttr->GetValueString();

		  //Connection weight
		  l_Trans.sConWeight=l_sMult;

		 //Add the transition's information to the place's pre transitions
		  m_a2DPlacePreTransitions[l_nPlacePos].push_back(l_Trans);
      }

 return true;
}
bool SP_DS_PlaceODE::FillInPostConnection(SP_DS_Node* p_pcPlaceNode)
{
	CHECK_POINTER(p_pcPlaceNode,return false);

	      //GetNodeID
	      long l_nPlaceID=GetNodeID(p_pcPlaceNode);
	      long l_nPlacePos=m_mPlacesPosByID[l_nPlaceID];

	      SP_ListEdge::const_iterator l_itEdge;

	      for (l_itEdge = p_pcPlaceNode->GetSourceEdges()->begin(); l_itEdge!= p_pcPlaceNode->GetSourceEdges()->end(); ++l_itEdge)
	      {

	    	  SP_DS_Node* l_pcTransNode=dynamic_cast<SP_DS_Node*>((*l_itEdge)->GetTarget());

	    	  CHECK_POINTER(l_pcTransNode,return false);

	    	  wxString l_sTransType=l_pcTransNode->GetClassName();

	    	  //we record only connections between places and continuous or stochastic transitions.
			  if(!(l_sTransType==SP_DS_CONTINUOUS_TRANS || l_sTransType==SP_DS_STOCHASTIC_TRANS))
				  continue;

	    	  long l_nTransID=GetNodeID(l_pcTransNode);
	    	  long l_nTransPos=m_mTransitionsPosByID[l_nTransID];

			  //Set the post connection type
			  PlaceTransitions l_Trans;

			  //Connection type
			  l_Trans.ConType=GetConnectionType((*l_itEdge));

			  //Transition's position
			  l_Trans.nTransPos=l_nTransPos;

			  SP_DS_Attribute* l_pcMultAttr=GetEdgeAttribute((*l_itEdge));

			  if (l_Trans.ConType == SP_MODIFIER_ARC)   continue;//george

			  CHECK_POINTER(l_pcMultAttr,return false);

			  wxString l_sMult=l_pcMultAttr->GetValueString();

			  //Connection weight
			  l_Trans.sConWeight=l_sMult;

			 //Add the transition's information to the place's post transitions
			  m_a2DPlacePostTransitions[l_nPlacePos].push_back(l_Trans);
	      }

	return true;
}

ConnectionType SP_DS_PlaceODE::GetConnectionType(SP_DS_Edge* p_pcEdgeNode)
{
  wxString l_sArcType=p_pcEdgeNode->GetClassName();

          //Return the appreciate type based on the arc type
          if(l_sArcType==SP_DS_EDGE)
          {
        	  return SP_STANDARD_ARC;
          }
          else if(l_sArcType==SP_DS_READ_EDGE)
          {
        	  return SP_READ_ARC;
          }
          else if(l_sArcType==SP_DS_INHIBITOR_EDGE)
		  {
		    return SP_INHIBITOR_ARC;
		  }
          else if(l_sArcType ==SP_DS_EQUAL_EDGE)
		  {
			return SP_EQUAL_ARC;
		  }
          else if(l_sArcType ==SP_DS_MODIFIER_EDGE)
		  {
			return SP_MODIFIER_ARC;
		  }
          else
          {
        	return SP_OTHER_CONNECTION;
          }
}
//Check if is it a continuous or other edge type
SP_DS_Attribute* SP_DS_PlaceODE::GetEdgeAttribute(SP_DS_Edge* p_pcEdge)
{
   //Get the Edge weight
   SP_DS_Attribute* l_pcAtt=p_pcEdge->GetAttribute(wxT("Multiplicity"));

   return l_pcAtt;
}

long SP_DS_PlaceODE::GetNodeID(SP_DS_Node* p_pcNode)
{
	    unsigned long l_nTransID=0;
	    wxString l_sTransitionID =p_pcNode->GetAttribute(wxT("ID"))->GetValueString();
	    l_sTransitionID.ToULong(&l_nTransID);

	 return l_nTransID;
}
bool SP_DS_PlaceODE::Initialize()
{
	m_anTransitionsType.clear();
	m_anTransPosition.clear();
	m_asTransRateFunction.clear();

	m_a2DPlacePreTransitions.clear();
	m_a2DPlacePostTransitions.clear();

	asPlaceNames.clear();

	//Load Places
	if(!LoadPlaces())
	{
	  return false;
	}

	//Load transitions
	if(!LoadTransitions())
	{
	  return false;
	}

	//Load Connections
	if(!LoadConnections())
	{
	  return false;
	}

	if(!AddExtendedArcsEffects())
	{
	  return false;
	}

	//No error
	return true;
}

bool SP_DS_PlaceODE::AddExtendedArcsEffects()
{

	for(unsigned long l_nPlacePos=0;l_nPlacePos<m_nPlaceCount;l_nPlacePos++)
	{
		for(unsigned long l_nPostTrans=0;l_nPostTrans<m_a2DPlacePostTransitions[l_nPlacePos].size();l_nPostTrans++)
		{
		  if(IsExtendedArc(m_a2DPlacePostTransitions[l_nPlacePos][l_nPostTrans].ConType))
           {
             wxString l_sExtendedFunc=ConstructExtendedFunction(l_nPostTrans,l_nPlacePos);

             if(l_sExtendedFunc!=wxT(""))
             {
            	 long l_nTransPos=m_a2DPlacePostTransitions[l_nPlacePos][l_nPostTrans].nTransPos;
            	 m_asTransRateFunction[l_nTransPos]+=wxT("*")+l_sExtendedFunc;
             }
           }
		}
	}


 return true;
}
bool SP_DS_PlaceODE::IsExtendedArc(const ConnectionType& p_nConType)
{
	if(p_nConType==SP_READ_ARC || p_nConType==SP_EQUAL_ARC || p_nConType==SP_INHIBITOR_ARC)
	{
	 return true;
	}
	else
	{
	 return false;
	}
}
wxString SP_DS_PlaceODE::ConstructExtendedFunction(const unsigned long& p_nTransPos,const unsigned long& p_nPlacePos)
{
	ConnectionType l_nType=m_a2DPlacePostTransitions[p_nPlacePos][p_nTransPos].ConType;
	wxString l_sReturn=wxT("");

	/*
	 * The constructed function will be in the form:
	 * Read(w,PlaceName)
	 * Where w is the connection weight between place and transitions,
	 * Read can be replaced by any function: Inhibit, Equal
	 */
	 wxString l_sRatio=m_a2DPlacePostTransitions[p_nPlacePos][p_nTransPos].sConWeight+wxT(",")+ asPlaceNames[p_nPlacePos];

	switch(l_nType)
	{
	case SP_READ_ARC:
		l_sReturn=wxT("Read(")+l_sRatio+wxT(")");
		break;
	case SP_INHIBITOR_ARC:
		 l_sReturn=wxT("Inhibit(")+l_sRatio+wxT(")");
		break;
	case SP_EQUAL_ARC:
		 l_sReturn=wxT("Equal(")+l_sRatio+wxT(")");
		break;
	default:
		l_sReturn=wxT("");
	}
  return l_sReturn;
}
wxString SP_DS_PlaceODE::ConstructODE(const long& p_nPlacePos)
{
   wxString l_sReturnEq=wxT("");

	   //Pre Equations
	   for(unsigned long l_nPreTrans=0;l_nPreTrans<m_a2DPlacePreTransitions[p_nPlacePos].size();l_nPreTrans++)
	   {
		   PlaceTransitions l_TransInfo=m_a2DPlacePreTransitions[p_nPlacePos][l_nPreTrans];

		   unsigned long l_nTransPos=l_TransInfo.nTransPos;

		   if(m_anTransitionsType[l_nTransPos]==SP_CONTINUOUS_TRANS && l_TransInfo.ConType==SP_STANDARD_ARC)
		   {
			   /*
				* Ignore 1 as a multiplicity
				*/
			   if(l_TransInfo.sConWeight!=wxT("1"))
			   {
				   l_sReturnEq+= wxT("+") + l_TransInfo.sConWeight+
								 wxT("*") + m_asTransRateFunction[l_nTransPos];
			   }
			   else
			   {
			     l_sReturnEq+= wxT("+") + m_asTransRateFunction[l_nTransPos];
			   }
		   }
	   }

	   //Post Equation
	   for(unsigned long l_nPostTrans=0;l_nPostTrans<m_a2DPlacePostTransitions[p_nPlacePos].size();l_nPostTrans++)
	   {
		   PlaceTransitions l_TransInfo=m_a2DPlacePostTransitions[p_nPlacePos][l_nPostTrans];

		   unsigned long l_nTransPos=l_TransInfo.nTransPos;

		   if(m_anTransitionsType[l_nTransPos]==SP_CONTINUOUS_TRANS && l_TransInfo.ConType==SP_STANDARD_ARC)
		   {

			   /*
				* Ignore 1 as a multiplicity
				*/
			   if(l_TransInfo.sConWeight!=wxT("1"))
			   {
				   l_sReturnEq+= wxT("-") + l_TransInfo.sConWeight+
								 wxT("*") + m_asTransRateFunction[l_nTransPos];
			   }
			   else
			   {
				   l_sReturnEq+= wxT("-") +m_asTransRateFunction[l_nTransPos];
			   }

		   }
	   }

	   if (l_sReturnEq.IsEmpty())
		   l_sReturnEq = wxT("0");

	   if(l_sReturnEq[0]== wxT('+'))
		   l_sReturnEq=l_sReturnEq.AfterFirst(wxT('+'));


  return l_sReturnEq;
}
