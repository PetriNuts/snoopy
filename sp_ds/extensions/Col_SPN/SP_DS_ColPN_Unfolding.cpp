//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2009/12/28 10:20:00 $
// Short Description: unfolding class
//////////////////////////////////////////////////////////////////////
//#define _CRTDBG_MAP_ALLOC
#include "sp_ds/extensions/Col_SPN/SP_DS_ColPN_Unfolding.h"
#include "sp_core/SP_Core.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/extensions/Col_PN/SyntaxChecking/SP_CPN_SyntaxChecking.h"

#include <algorithm>
#include <ctime>

#include "sp_ds/extensions/Col_PN/Preprocessing/SP_ColPN_ProcessGuard.h"
#include <wx/busyinfo.h> 
#include <wx/regex.h>

#include "sp_ds/extensions/Col_SPN/SP_DS_ColUnfoldFile.h"

#include "sp_core/tools/SP_NetUnfolder.h"

#include "sp_core/tools/alphanum.h"
// alphanumeric comparision of strings
bool alphanum_comparer::operator() (const wxString& left, const wxString& right) const
{
  std::string l(left.mbc_str());
  std::string r(right.mbc_str());

  return doj::alphanum_comp(l, r) < 0;
}

//#include "vld.h"

/*
//TODO: find a way to solve this warning
#ifdef __WXMSW__
#pragma warning( disable: C4800 )
#pragma warning( disable: C4200 )
#pragma warning( disable: C4345 )
#pragma warning( disable: C4251 )
#pragma warning( disable: C4146 )
#pragma warning( disable: C4251 )
#endif
*/

wxMutex m_InitGraphMutex;

SP_DS_ColPN_Unfolding::SP_DS_ColPN_Unfolding()
{			
	m_bFlagUnfoldingRunning = false;
	m_bFlagPause = false;
	m_bRemIsoPlaces = true;
	m_bRemIsoTranss = true;
}

SP_DS_ColPN_Unfolding::~SP_DS_ColPN_Unfolding()
{	
}

bool SP_DS_ColPN_Unfolding::Initialize()
{			
	m_nLastUpdateTime = 0;	
	m_bThreadSuccess = true;
	
	if( !m_ValueAssign.InitializeColorset(m_ColorSetClass) )
		return false;		

	return true;
}

bool SP_DS_ColPN_Unfolding::Unfolding()
{
	//open dialogue
	m_pcUnfoldDlg = new SP_DLG_ColStUnfolding(this,NULL);
	if (m_pcUnfoldDlg->ShowModal() == wxID_OK)
	{
		m_pcUnfoldDlg->Destroy();
	}
	else
	{
		m_pcUnfoldDlg->Destroy();
		wxString l_Msg = wxT("Error while unfolding.\n");
		l_Msg << wxT(__FILE__);
		SP_LOGERROR(l_Msg);
		return false;
	}		

	ReportUnfoldingStatistics();
	
	return true;	

}

void SP_DS_ColPN_Unfolding::ReportUnfoldingStatistics()
{
	//report unfolding statistics	
	long l_nTime = m_cUnfoldingStopWatch.Time();
	wxString l_sTime = wxString::Format(wxT("%.3f sec"), (float)l_nTime/1000);
	SP_LOGMESSAGE( wxT("\n The unfolding time is ") + l_sTime + wxT(".\n") );	

	//report the size of the flat net
	wxString l_sPlaceNumber = wxString::Format(wxT("%ld"),m_nPlaceCountStat);
	l_sPlaceNumber = FormatByComma(l_sPlaceNumber);
	wxString l_sTransNumber = wxString::Format(wxT("%ld"),m_nTransitionCountStat);
	l_sTransNumber = FormatByComma(l_sTransNumber);
	wxString l_sStandardArcNumber = wxString::Format(wxT("%ld"),m_nOrdinaryArcNum);
	l_sStandardArcNumber = FormatByComma(l_sStandardArcNumber);
	wxString l_sReadArcNumber = wxString::Format(wxT("%ld"),m_nReadArcNum);
	l_sReadArcNumber = FormatByComma(l_sReadArcNumber);
	wxString l_sInhibitorArcNumber = wxString::Format(wxT("%ld"),m_nInhibitorArcNum);
	l_sInhibitorArcNumber = FormatByComma(l_sInhibitorArcNumber);
	wxString l_sEqualArcNumber = wxString::Format(wxT("%ld"),m_nEqualArcNum);
	l_sEqualArcNumber = FormatByComma(l_sEqualArcNumber);
	wxString l_sResetArcNumber = wxString::Format(wxT("%ld"),m_nResetArcNum);
	l_sResetArcNumber = FormatByComma(l_sResetArcNumber);
	wxString l_sModifierArcNumber = wxString::Format(wxT("%ld"),m_nModifierArcNum);
	l_sModifierArcNumber = FormatByComma(l_sModifierArcNumber);

	SP_LOGMESSAGE( wxT("The size of the flat net: \n Place number: ") + l_sPlaceNumber +
		                      wxT("\n Transition number: ")   + l_sTransNumber +
		                      wxT("\n Standard arc number: ") + l_sStandardArcNumber + 
							  wxT("\n Read arc number: ") +     l_sReadArcNumber+ 
							  wxT("\n Inhibitor arc number: ") +l_sInhibitorArcNumber + 
							  wxT("\n Equal arc number: ") +	l_sEqualArcNumber + 
							  wxT("\n Reset arc number: ") +	l_sResetArcNumber + 
							  wxT("\n Modifier arc number: ") + l_sModifierArcNumber + 
							  wxT("\n"));
}


wxString SP_DS_ColPN_Unfolding::FormatByComma(wxString p_sNumber)
{
	//for example, 2000000 to 2,000,000
	wxString l_sPlaceNumber  = p_sNumber;
	vector<wxString> l_vPlacesegments;
	if(l_sPlaceNumber.Len()>3)
	{
		unsigned int l_nMo = l_sPlaceNumber.Len() % 3;
		unsigned int l_n3times = l_sPlaceNumber.Len() / 3;
		l_vPlacesegments.push_back(l_sPlaceNumber.Mid(0,l_nMo));
		for(unsigned i = 0; i < l_n3times; i++)
		{
			wxString l_sNew3str = l_sPlaceNumber.Mid(l_nMo+3*i,3);
			l_vPlacesegments.push_back(l_sNew3str);
		}
		l_sPlaceNumber = wxT("");
		for(unsigned i = 0; i < l_vPlacesegments.size(); i++)
		{
			l_sPlaceNumber = l_sPlaceNumber + wxT(",") + l_vPlacesegments[i] ; 
		}
		l_sPlaceNumber = l_sPlaceNumber.AfterFirst(wxT(','));
		if( l_sPlaceNumber.Mid(0,1) == wxT(",") )
		{
			l_sPlaceNumber = l_sPlaceNumber.AfterFirst(wxT(','));
		}
	}

	return  l_sPlaceNumber;
}

bool SP_DS_ColPN_Unfolding::Start()
{
	m_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	m_sNetClass = m_pcGraph->GetNetclass()->GetName();

	//Unfold a colored net
	m_bFlagUnfoldingRunning = true;
	m_bFlagPause = false;
	m_pcUnfoldDlg->SetUnfoldingProgressGauge(0);
	m_cUnfoldingStopWatch.Start(0);

	if(m_sUnfoldSource == wxT("Load"))
	{
		//Load unfolding information from file
		SP_DS_ColUnfoldFile l_cColUnfoldFile(this);
		if( ! l_cColUnfoldFile.DoRead(m_sLoadFilename))
			return false;
		//deal with unfolding results
		ProcessResult();
	}
	else if(m_bIddDsszUtilSolve)
	{
		SP_IddNetUnfolder unfolder;
		if(unfolder(m_pcGraph))
		{
			unfolder.FillInResults(this);

			//assign position to places
			AssignPlacePositions(m_mmUnfoldedContPlaces);
			AssignPlacePositions(m_mmUnfoldedDiscPlaces);   //order them in terms of hybrid nets

			//assign position to transitions
			AssignTransPositions(m_mmUnfoldedStochTransions); //order them in terms of hybrid nets
			AssignTransPositions(m_mmUnfoldedContTransions);
			AssignTransPositions(m_mmUnfoldedImmTransions);
			AssignTransPositions(m_mmUnfoldedDetTransions);
			AssignTransPositions(m_mmUnfoldedSchedTransions);

			m_nPlaceCountStat = m_msPlaceNames.size();
			m_nTransitionCountStat = m_msTransitionNames.size();
		}
		else
		{
			SP_LOGERROR(wxT(" while unfolding colored net using IDD (dssz_util) engine!"));
			return false;
		}
	}
	else if(m_bGecodeDsszUtilSolve)
	{
		SP_GecodeNetUnfolder unfolder;
		if(unfolder(m_pcGraph))
		{
			unfolder.FillInResults(this);

			//assign position to places
			AssignPlacePositions(m_mmUnfoldedContPlaces);
			AssignPlacePositions(m_mmUnfoldedDiscPlaces);   //order them in terms of hybrid nets

			//assign position to transitions
			AssignTransPositions(m_mmUnfoldedStochTransions); //order them in terms of hybrid nets
			AssignTransPositions(m_mmUnfoldedContTransions);
			AssignTransPositions(m_mmUnfoldedImmTransions);
			AssignTransPositions(m_mmUnfoldedDetTransions);
			AssignTransPositions(m_mmUnfoldedSchedTransions);

			m_nPlaceCountStat = m_msPlaceNames.size();
			m_nTransitionCountStat = m_msTransitionNames.size();
		}
		else
		{
			SP_LOGERROR(wxT(" while unfolding colored net using Gecode (dssz_util) engine!"));
			return false;
		}
	}
	else
	{
		//initialize
		if( ! Initialize() )
			return false;


		if( ! UnfoldPlaces() )
				return false;

		//for marking-dependent arcs testing	
		if (!UnfoldPlacesForMarkingDependent())
			return false;

		if( ! StartThreadUnfolding() )
			return false;

		//deal with unfolding results
		ProcessResult();
	}
	//SP_MESSAGEBOX(wxT("Paused"));


	m_bFlagUnfoldingRunning = false;
	m_bFlagPause = false;

	m_cUnfoldingStopWatch.Pause();
	m_pcUnfoldDlg->SetUnfoldingProgressGauge(100);
	m_pcUnfoldDlg->SetUnfoldingStopWatch(m_cUnfoldingStopWatch.Time());

	m_pcUnfoldDlg->GetStartButton()->SetBackgroundColour( *wxGREEN);
	m_pcUnfoldDlg->GetStartButton()->SetLabel(wxT("Start"));

	m_cUnfoldingStopWatch.Resume();	

	//write unfolding info to a file
	if(m_bWriteFlag)
	{
		SP_DS_ColUnfoldFile l_cColUnfoldFile(this);
		l_cColUnfoldFile.DoWrite(m_sSaveFilename);
	}
	
	m_cUnfoldingStopWatch.Pause();

	return true;
}


bool SP_DS_ColPN_Unfolding::GetTransitions()
{
	//m_vTransExprs.clear();
	SP_DS_Nodeclass* l_pcNodeclass;
	SP_ListNode::const_iterator l_itElem;

	l_pcNodeclass= m_pcGraph->GetNodeclass( SP_DS_CONTINUOUS_TRANS );
	if( l_pcNodeclass )
	{
		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{			
			SP_DS_Node* l_pcTransNode = *l_itElem;
			m_vTransitions.push_back(l_pcTransNode);			
		}
	}
	l_pcNodeclass= m_pcGraph->GetNodeclass( SP_DS_STOCHASTIC_TRANS );
	if( l_pcNodeclass )
	{
		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{			
			SP_DS_Node* l_pcTransNode = *l_itElem;
			m_vTransitions.push_back(l_pcTransNode);			
		}
	}
	
	l_pcNodeclass= m_pcGraph->GetNodeclass( wxT("Immediate Transition") );
	if( l_pcNodeclass )
	{
		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{			
			SP_DS_Node* l_pcTransNode = *l_itElem;
			m_vTransitions.push_back(l_pcTransNode);			
		}
	}
	
	l_pcNodeclass= m_pcGraph->GetNodeclass( wxT("Deterministic Transition") );
	if( l_pcNodeclass )
	{
		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{			
			SP_DS_Node* l_pcTransNode = *l_itElem;
			m_vTransitions.push_back(l_pcTransNode);			
		}
	}
	
	l_pcNodeclass= m_pcGraph->GetNodeclass( wxT("Scheduled Transition") );
	if( l_pcNodeclass )
	{
		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{			
			SP_DS_Node* l_pcTransNode = *l_itElem;
			m_vTransitions.push_back(l_pcTransNode);			
		}
	}

	return true;
}


bool SP_DS_ColPN_Unfolding::UnfoldPlaces()
{
	m_mssColPlaceName2ColorSetforOrdering.clear();
	m_mmUnfoldedContPlaces.clear();
	m_mmUnfoldedDiscPlaces.clear();

	//for discrete Petri nets
	if(!UnfoldPlaceNodeClass(SP_DS_DISCRETE_PLACE))
		return false;
	
	//for both colored continuous and hybrid PNs
	if(!UnfoldPlaceNodeClass(SP_DS_CONTINUOUS_PLACE))
		return false;	
	return true;
}

bool SP_DS_ColPN_Unfolding::UnfoldPlacesForMarkingDependent()
{
	m_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();

	if (!m_ValueAssign.InitializeColorset(m_ColorSetClass))
		return false;

	SP_DS_Nodeclass* l_pcNodeclass;
	SP_ListNode::const_iterator l_itElem;

	l_pcNodeclass = m_pcGraph->GetNodeclass(SP_DS_DISCRETE_PLACE);
	if (l_pcNodeclass)
	{
		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{
			SP_DS_Node* l_pcNode = *l_itElem;
			wxString l_sColPlaceName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

			//get the color set
			SP_DS_TextAttribute* l_pcCSAttibute = dynamic_cast<SP_DS_TextAttribute*>(l_pcNode->GetAttribute(SP_DS_CPN_COLORSETNAME));
			CHECK_POINTER(l_pcCSAttibute, return false);
			wxString l_sColorSetName = l_pcCSAttibute->GetValue();
			vector<wxString> l_ColorVector;
			SP_CPN_ColorSet* l_pcColorSet = m_ColorSetClass.LookupColorSet(l_sColorSetName);
			CHECK_POINTER(l_pcColorSet, return false);
			l_ColorVector = l_pcColorSet->GetStringValue();
			vector<wxString> l_vsUncoloredPlaces;
			for (unsigned int l_nPos = 0; l_nPos < l_ColorVector.size(); l_nPos++)
			{
				wxString l_sColor = l_ColorVector[l_nPos];
				ModifyName(l_sColor);
				wxString l_sUnColoredPlaceName = l_sColPlaceName + wxT("_") + l_sColor;
				l_vsUncoloredPlaces.push_back(l_sUnColoredPlaceName);
			}
			m_svColored2UnColoredPlaceNames[l_sColPlaceName] = l_vsUncoloredPlaces;
		}
	}
			
	l_pcNodeclass = m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_PLACE);
	if (l_pcNodeclass)
	{
		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{
			SP_DS_Node* l_pcNode = *l_itElem;
			wxString l_sColPlaceName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

			//get the color set
			SP_DS_TextAttribute* l_pcCSAttibute = dynamic_cast<SP_DS_TextAttribute*>(l_pcNode->GetAttribute(SP_DS_CPN_COLORSETNAME));
			CHECK_POINTER(l_pcCSAttibute, return false);
			wxString l_sColorSetName = l_pcCSAttibute->GetValue();
			vector<wxString> l_ColorVector;
			SP_CPN_ColorSet* l_pcColorSet = m_ColorSetClass.LookupColorSet(l_sColorSetName);
			CHECK_POINTER(l_pcColorSet, return false);
			l_ColorVector = l_pcColorSet->GetStringValue();
			vector<wxString> l_vsUncoloredPlaces;
			for (unsigned int l_nPos = 0; l_nPos < l_ColorVector.size(); l_nPos++)
			{
				wxString l_sColor = l_ColorVector[l_nPos];
				ModifyName(l_sColor);
				wxString l_sUnColoredPlaceName = l_sColPlaceName + wxT("_") + l_sColor;
				l_vsUncoloredPlaces.push_back(l_sUnColoredPlaceName);
			}
			m_svColored2UnColoredPlaceNames[l_sColPlaceName] = l_vsUncoloredPlaces;
		}
	}
	
	return true;
}

bool SP_DS_ColPN_Unfolding::UnfoldPlaceNodeClass(wxString p_sPlaceNodeClass)
{
	SP_DS_Nodeclass* l_pcNodeclass;
	l_pcNodeclass= m_pcGraph->GetNodeclass( p_sPlaceNodeClass );
	SP_ListNode::const_iterator l_itElem;	

	if( l_pcNodeclass )
	{
		for(l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{
			SP_DS_Node* l_pcNode = *l_itElem;	
			wxString l_sColPlaceName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
	
	//get the color set
			SP_DS_TextAttribute* l_pcCSAttibute = dynamic_cast< SP_DS_TextAttribute* >(l_pcNode->GetAttribute(SP_DS_CPN_COLORSETNAME) );
			CHECK_POINTER(l_pcCSAttibute, return false);
			wxString l_sColorSetName = l_pcCSAttibute->GetValue();	
			vector<wxString> l_ColorVector;		
			SP_CPN_ColorSet* l_pcColorSet = m_ColorSetClass.LookupColorSet(l_sColorSetName);
			CHECK_POINTER(l_pcColorSet, return false);				
			l_ColorVector = l_pcColorSet->GetStringValue(); 

			m_mssColPlaceName2ColorSetforOrdering[l_sColPlaceName] = l_sColorSetName;

	//Get the initial markings	
			SP_CPN_SyntaxChecking l_cSyntaxChecking;
			if( !l_cSyntaxChecking.Initialize())
				return false;	
	
			map<wxString, vector<SP_CPN_TokenNum> > l_mColorToMarkingMap;
			if( ! l_cSyntaxChecking.ComputeInitialMarking(l_pcNode,l_mColorToMarkingMap,false) )
				return false;
			SP_CPN_UnfoldedPlace l_mSingleUnPlace;
			
			map<wxString, vector<SP_CPN_TokenNum> >::iterator itMap;
			for(itMap = l_mColorToMarkingMap.begin(); itMap != l_mColorToMarkingMap.end(); itMap++)
			{				
				SP_CPN_UnfoldedPlaceInfo l_structDetails;
				l_structDetails.m_sNodeType = p_sPlaceNodeClass;
				l_structDetails.m_sColorSet = l_sColorSetName;

				wxString l_sColor = itMap->first;
				if( p_sPlaceNodeClass == SP_DS_CONTINUOUS_PLACE )
				{
					SP_VectorDouble l_adMarking;
					for(unsigned k = 0; k < itMap->second.size(); k++)
					{
						double l_dMark;
						l_dMark = itMap->second[k].m_DoubleMultiplicity;
						l_adMarking.push_back( l_dMark );					
					}	
					l_structDetails.m_adNetMarkings = l_adMarking;
				}
				else
				{
					SP_VectorLong l_anMarking;
					for(unsigned k = 0; k < itMap->second.size(); k++)
					{
						long l_nMark;
						l_nMark = itMap->second[k].m_intMultiplicity;
						l_anMarking.push_back( l_nMark );					
					}		
					l_structDetails.m_anNetMarkings = l_anMarking;
				}
				l_mSingleUnPlace[l_sColor] = l_structDetails;
			}

			if(  p_sPlaceNodeClass == SP_DS_CONTINUOUS_PLACE && m_mmUnfoldedContPlaces.find(l_sColPlaceName) == m_mmUnfoldedContPlaces.end())
			{
				m_mmUnfoldedContPlaces[l_sColPlaceName] = l_mSingleUnPlace;
			}
			else if(  p_sPlaceNodeClass == SP_DS_DISCRETE_PLACE && m_mmUnfoldedDiscPlaces.find(l_sColPlaceName) == m_mmUnfoldedDiscPlaces.end())
			{
				m_mmUnfoldedDiscPlaces[l_sColPlaceName] = l_mSingleUnPlace;
			}
			else
			{
				SP_MESSAGEBOX(wxT("Error: Every place need a unique name for unfolding!\n\nSee log (Alt+3) for more details!"), wxT("Error"));
				return false;
			}			
		}
	}

	return true;
}



////Multi-threads-unfolding

bool SP_DS_ColPN_Unfolding::StartThreadUnfolding()
{
	if( !GetTransitions() )  //put all transtions into m_vTransitions
		return false;

	if(m_nThreadCount <= 0)
		return false;
	
	m_nThreadFinished = 0;
	unsigned long l_Runs = 1;
	unsigned long l_FirstRun = 1;

	unsigned long l_nTransNum = m_vTransitions.size();
	unsigned long l_Threads = l_nTransNum;	

	if(l_nTransNum > m_nThreadCount)
	{
		l_Runs = l_nTransNum / m_nThreadCount;
		l_FirstRun = l_Runs + l_nTransNum % m_nThreadCount;
		l_Threads = m_nThreadCount;
	}

	m_nTranNumInFirstThread = l_FirstRun;

	m_nTranNumPerThread = l_FirstRun;

	vector<SP_DS_ColPN_Unfolding::UnfoldThread*> l_aTrheads;

	if(l_nTransNum <= m_nThreadCount)
	{
		for(unsigned i = 0; i < l_nTransNum; i++)
		{
			vector<SP_DS_Node*> l_vpcNodes;
			l_vpcNodes.push_back(m_vTransitions[i]);

			SP_DS_ColPN_Unfolding::UnfoldThread* l_pcThread = new SP_DS_ColPN_Unfolding::UnfoldThread(i,this,l_vpcNodes,m_pcGraph, m_bCSPSolve);
			l_pcThread->Create();
			l_aTrheads.push_back(l_pcThread);
		}
	}	
	else
	{
		unsigned long l_Runs = l_nTransNum / m_nThreadCount;
		for(unsigned i = 0; i < m_nThreadCount; i++)
		{
			unsigned long l_nLowPos;
			unsigned long l_nUpPos;
			if(i == 0)
			{
				l_nLowPos = 0;
				l_nUpPos = l_FirstRun-1;
			}
			else
			{
				l_nLowPos = l_FirstRun + (i-1)*l_Runs;
				l_nUpPos  = l_FirstRun + i*l_Runs-1;

			}

			vector<SP_DS_Node*> l_vpcNodes;
			for(unsigned int k = l_nLowPos; k <= l_nUpPos; k++ )
			{				
				l_vpcNodes.push_back(m_vTransitions[k]);
			}

			SP_DS_ColPN_Unfolding::UnfoldThread* l_pcThread =	new SP_DS_ColPN_Unfolding::UnfoldThread(i,this,l_vpcNodes,m_pcGraph,m_bCSPSolve);
			l_pcThread->Create();
			l_aTrheads.push_back(l_pcThread);
		}
		
	}
	
	for(unsigned long i = 0; i < l_aTrheads.size(); ++i)
	{
		l_aTrheads[i]->Run();
	}
	SP_DS_ColPN_Unfolding::UnfoldThread* l_pcThread = l_aTrheads[0];
	while(m_nThreadFinished < l_aTrheads.size())
	{
		UpdateUnfoldDlg(l_pcThread->GetCurrentFinishedNumber());
		wxSleep(1);
	}

	//begin to pause
	m_cUnfoldingStopWatch.Pause();
	while(m_bFlagPause)
	{		
		for(unsigned long i = 0; i < l_aTrheads.size(); ++i)
		{
			l_aTrheads[i]->Sleep(1);
		}		
	}
	m_cUnfoldingStopWatch.Resume();
	//end pause


	for(unsigned long i = 0; i < l_aTrheads.size(); ++i)
	{
		l_aTrheads[i]->Wait();
	}
	for(unsigned long i = 0; i < l_aTrheads.size(); ++i)
	{
		SP_DS_ColPN_Unfolding::UnfoldThread* l_pcThread = l_aTrheads[i];
		wxDELETE(l_pcThread);
	}	

	if( ! m_bThreadSuccess)   //if any thread is false
		return false;

	return true;
}


void SP_DS_ColPN_Unfolding::ComputeBindings(SP_DS_Node* p_pcTransNode)
{
	bool l_bCSPSucceed;
	vector<wxString> l_vsBindings;
	SP_DS_ColPN_Unfolding::UnfoldThread l_cUnfold;
	if( ! l_cUnfold.ComputeBindings(p_pcTransNode,l_bCSPSucceed, l_vsBindings) )
	{
		SP_LOGERROR( wxT("Unfolding error. \n") );		
	}
	else
	{
		if(l_bCSPSucceed)
		{
			SP_LOGMESSAGE( wxT("The constraint satisfaction approach is used.\n") );	
		}
		SP_LOGMESSAGE( wxT("There are ")+ wxString::Format(wxT("%d"),l_vsBindings.size())+ wxT(" legal bindings, which are as follows:\n") );	
		wxString l_sBindings;
		int l_nCount = 1;
		for(unsigned int i = 0; i < l_vsBindings.size(); i++)
		{
			l_sBindings << wxString::Format(wxT("Binding %d: "),l_nCount);
			l_sBindings << l_vsBindings[i] + wxT("\n");		
			if( l_nCount%100 == 0 )
			{
				SP_LOGMESSAGE( wxT("\n") + l_sBindings + wxT("\n") );
				l_sBindings.clear();
			}			
			l_nCount++;
		}
		SP_LOGMESSAGE( wxT("\n") + l_sBindings + wxT("\n") );		
	}
}

bool SP_DS_ColPN_Unfolding::ProcessResult()
{
	ClearPlaces();
	ClearTranss();
	
	//find isolated places
	FindIsolatedPlaces(m_mmUnfoldedStochTransions);
	FindIsolatedPlaces(m_mmUnfoldedContTransions);
	FindIsolatedPlaces(m_mmUnfoldedImmTransions);
	FindIsolatedPlaces(m_mmUnfoldedDetTransions);
	FindIsolatedPlaces(m_mmUnfoldedSchedTransions);

	//assign position to places
	AssignPlacePositionsNew(m_mmUnfoldedContPlaces);
	AssignPlacePositionsNew(m_mmUnfoldedDiscPlaces);   //order them in terms of hybrid nets	

	//assign position to transitions	
	AssignTransPositions(m_mmUnfoldedStochTransions); //order them in terms of hybrid nets	
	AssignTransPositions(m_mmUnfoldedContTransions);
	AssignTransPositions(m_mmUnfoldedImmTransions);
	AssignTransPositions(m_mmUnfoldedDetTransions);
	AssignTransPositions(m_mmUnfoldedSchedTransions);

	m_nPlaceCountStat = m_msPlaceNames.size();
	m_nTransitionCountStat = m_msTransitionNames.size();


	return true;
}


void SP_DS_ColPN_Unfolding::FindIsolatedPlaces(SP_CPN_UnfoldedTransitions & p_mmUnfoldedTransions)
{
	SP_CPN_UnfoldedTransitions::iterator itMap1;
	for(itMap1 = p_mmUnfoldedTransions.begin(); itMap1 != p_mmUnfoldedTransions.end(); itMap1++)
	{
		SP_CPN_UnfoldedTransition::iterator itMap2; 
		for(itMap2 = itMap1->second.begin(); itMap2 != itMap1->second.end(); itMap2++ )
		{
			for(unsigned int i = 0; i < itMap2->second.m_vInputArcs.size(); i++ )
			{
				wxString l_sColPlName = itMap2->second.m_vInputArcs[i].m_sColPlaceName;
				wxString l_sColor     = itMap2->second.m_vInputArcs[i].m_sColor;

				SP_CPN_UnfoldedPlaces::iterator itPLMap1;
				itPLMap1 = m_mmUnfoldedDiscPlaces.find(l_sColPlName);
				if( itPLMap1 != m_mmUnfoldedDiscPlaces.end() )
				{
					SP_CPN_UnfoldedPlace::iterator itPLMap2;
					itPLMap2 = itPLMap1->second.find( l_sColor );
					if( itPLMap2 != itPLMap1->second.end() )
					{
						itPLMap2->second.m_bIsolated = false;
					}
				}
				itPLMap1 = m_mmUnfoldedContPlaces.find(l_sColPlName);
				if( itPLMap1 != m_mmUnfoldedContPlaces.end() )
				{
					SP_CPN_UnfoldedPlace::iterator itPLMap2;
					itPLMap2 = itPLMap1->second.find( l_sColor );
					if( itPLMap2 != itPLMap1->second.end() )
					{
						itPLMap2->second.m_bIsolated = false;
					}
				}
			}

			for(unsigned int i = 0; i < itMap2->second.m_vOutputArcs.size(); i++ )
			{
				wxString l_sColPlName = itMap2->second.m_vOutputArcs[i].m_sColPlaceName;
				wxString l_sColor     = itMap2->second.m_vOutputArcs[i].m_sColor;

				SP_CPN_UnfoldedPlaces::iterator itPLMap1;
				itPLMap1 = m_mmUnfoldedDiscPlaces.find(l_sColPlName);
				if( itPLMap1 != m_mmUnfoldedDiscPlaces.end() )
				{
					SP_CPN_UnfoldedPlace::iterator itPLMap2;
					itPLMap2 = itPLMap1->second.find( l_sColor );
					if( itPLMap2 != itPLMap1->second.end() )
					{
						itPLMap2->second.m_bIsolated = false;
					}
				}
				itPLMap1 = m_mmUnfoldedContPlaces.find(l_sColPlName);
				if( itPLMap1 != m_mmUnfoldedContPlaces.end() )
				{
					SP_CPN_UnfoldedPlace::iterator itPLMap2;
					itPLMap2 = itPLMap1->second.find( l_sColor );
					if( itPLMap2 != itPLMap1->second.end() )
					{
						itPLMap2->second.m_bIsolated = false;
					}
				}
			}
		}
	}

}

void SP_DS_ColPN_Unfolding::ClearPlaces()
{
	m_msPlaceNames.clear();
	m_mnPlaceArrayPos.clear();
	m_msPlaceNamesById.clear();
	m_anNetMarkings.clear();
	m_anColCPNNetMarkings.clear();
	m_vsPlaceNodeType.clear();
	m_msColoredPlaceNames.clear();
	m_msColPlName2ColSet.clear();
	m_nsPlaceCountById.clear();
}

void SP_DS_ColPN_Unfolding::AssignPlacePositionsNew(SP_CPN_UnfoldedPlaces& p_mmUnfoldedPlaces )
{
	if( p_mmUnfoldedPlaces.size() == 0 )
		return;

	map< wxString,wxString >::iterator itMap;
	for(itMap = m_mssColPlaceName2ColorSetforOrdering.begin(); itMap != m_mssColPlaceName2ColorSetforOrdering.end();itMap++)
	{
		wxString l_sColPlName = itMap->first;
		wxString l_sColorSetName = itMap->second;
		SP_CPN_ColorSet* l_pcColorSet = m_ColorSetClass.LookupColorSet(l_sColorSetName);
		vector<wxString> l_vColorVector = l_pcColorSet->GetStringValue(); 

		bool l_bIsoColPlace = true;
		unsigned long l_nLow = m_msPlaceNames.size();
		unsigned long l_nUp;

		SP_CPN_UnfoldedPlaces::iterator itMap1;
		if( p_mmUnfoldedPlaces.find(l_sColPlName) != p_mmUnfoldedPlaces.end() )
		{
			itMap1 = p_mmUnfoldedPlaces.find(l_sColPlName);

			for(unsigned int l_nPos = 0; l_nPos < l_vColorVector.size(); l_nPos++ )
			{
				wxString l_sColor = l_vColorVector[l_nPos];
				SP_CPN_UnfoldedPlace::iterator itMap2; 
				itMap2 = itMap1->second.find(l_sColor);
				if( ! itMap2->second.m_bIsolated )
				{
					wxString l_sNodeType = itMap2->second.m_sNodeType;					
					m_msColorsOfPlace.push_back( l_sColor );
					ModifyName(l_sColor);
					wxString l_sUnPlName = l_sColPlName+ wxT("_") + l_sColor;
					long l_nId = m_msPlaceNames.size();
					m_msPlaceNames.push_back( l_sUnPlName );
					m_mnPlaceArrayPos[l_sUnPlName] = l_nId;
					wxString l_sId = wxString::Format(wxT("%ld"), l_nId);
					m_msPlaceNamesById[l_sId] = l_sUnPlName;
					if(l_sNodeType == SP_DS_DISCRETE_PLACE)
						m_anNetMarkings.push_back(itMap2->second.m_anNetMarkings);
					if(l_sNodeType == SP_DS_CONTINUOUS_PLACE)
						m_anColCPNNetMarkings.push_back(itMap2->second.m_adNetMarkings);

					m_vsPlaceNodeType.push_back( l_sNodeType );

					l_bIsoColPlace = false;
				}	
			}
			if( ! l_bIsoColPlace)
			{
				
				m_msColoredPlaceNames.push_back(l_sColPlName);
				m_msColPlName2ColSet[l_sColPlName] = l_sColorSetName;	

				l_nUp = m_msPlaceNames.size();
				SP_CPN_CountInterval l_PlInterval;
				l_PlInterval.m_nLow = l_nLow;
				l_PlInterval.m_nUp  = l_nUp-1;
				m_nsPlaceCountById[m_msColoredPlaceNames.size()-1] = l_PlInterval;
			}
		}
	
	}

	m_nPlaceCount = m_msPlaceNames.size();			
}

void SP_DS_ColPN_Unfolding::AssignPlacePositions(SP_CPN_UnfoldedPlaces& p_mmUnfoldedPlaces )
{		
	SP_CPN_UnfoldedPlaces::iterator itMap1;
	for(itMap1 = p_mmUnfoldedPlaces.begin(); itMap1 != p_mmUnfoldedPlaces.end(); itMap1++)
	{
		wxString l_sColPlName = itMap1->first;
		bool l_bIsoColPlace = true;
		wxString l_sColorSetName;
		unsigned long l_nLow = m_msPlaceNames.size();
		SP_CPN_UnfoldedPlace::iterator itMap2; 
		for(itMap2 = itMap1->second.begin(); itMap2 != itMap1->second.end(); itMap2++ )
		{		
			wxString l_sColor = itMap2->first;
			if( ! itMap2->second.m_bIsolated )
			{
				wxString l_sNodeType = itMap2->second.m_sNodeType;
				l_sColorSetName = itMap2->second.m_sColorSet;
				m_msColorsOfPlace.push_back( l_sColor );
				ModifyName(l_sColor);
				wxString l_sUnPlName = l_sColPlName + wxT("_") + l_sColor;
				long l_nId = m_msPlaceNames.size();
				m_msPlaceNames.push_back( l_sUnPlName ); //need further procession
				m_mnPlaceArrayPos[l_sUnPlName] = l_nId;
				wxString l_sId = wxString::Format(wxT("%ld"), l_nId);
				m_msPlaceNamesById[l_sId] = l_sUnPlName;
				if(l_sNodeType == SP_DS_DISCRETE_PLACE)
					m_anNetMarkings.push_back(itMap2->second.m_anNetMarkings);
				if(l_sNodeType == SP_DS_CONTINUOUS_PLACE)
					m_anColCPNNetMarkings.push_back(itMap2->second.m_adNetMarkings);

				m_vsPlaceNodeType.push_back( l_sNodeType );

				l_bIsoColPlace = false;
			}			
		}
		if( ! l_bIsoColPlace)
		{
			SP_CPN_CountInterval l_PlInterval;
			l_PlInterval.m_nLow = l_nLow;
			l_PlInterval.m_nUp  = m_msPlaceNames.size()-1;
			m_nsPlaceCountById[m_msColoredPlaceNames.size()] = l_PlInterval;

			m_msColoredPlaceNames.push_back(l_sColPlName);
			m_msColPlName2ColSet[l_sColPlName] = l_sColorSetName;	
		}
	}

	m_nPlaceCount = m_msPlaceNames.size();
}


void SP_DS_ColPN_Unfolding::ClearTranss()
{
	m_msTransitionNames.clear();
	m_mnTransitionArrayPos.clear();	
	m_anNetFunctions.clear();
	m_vsTransNodeType.clear();
	m_msColoredTransitionNames.clear();	
	m_nsTransitionCountById.clear();
	//m_abNonConsumingMode.clear();

	m_nOrdinaryArcNum = 0;
	m_nReadArcNum = 0;
	m_nInhibitorArcNum = 0;
	m_nResetArcNum = 0;
	m_nEqualArcNum = 0;
	m_nModifierArcNum = 0;
	
}

void SP_DS_ColPN_Unfolding::AssignTransPositions(SP_CPN_UnfoldedTransitions & p_mmUnfoldedTransions)
{
	SP_CPN_UnfoldedTransitions::iterator itMap1;
	for(itMap1 = p_mmUnfoldedTransions.begin(); itMap1 != p_mmUnfoldedTransions.end(); itMap1++)
	{
		wxString l_sColTRName = itMap1->first;
		bool l_bIsoColTrans = true;		
		unsigned long l_nLow = m_msTransitionNames.size();
		unsigned long l_nUp;

		SP_CPN_UnfoldedTransition::iterator itMap2; 
		for(itMap2 = itMap1->second.begin(); itMap2 != itMap1->second.end(); itMap2++ )
		{			
			if( itMap2->second.m_vInputArcs.size() + itMap2->second.m_vOutputArcs.size() != 0 ) //not isolated transition instance
			{	
				l_bIsoColTrans = false;
				wxString l_sUnTransName = l_sColTRName + wxT("_") + itMap2->first;
				long l_nTransPos = m_msTransitionNames.size();
				wxString l_sTransPos = wxString::Format(wxT("%ld"), l_nTransPos);
				m_msTransitionNames.push_back( l_sUnTransName );
				m_vsTransNodeType.push_back(itMap2->second.m_sType);
				m_anNetFunctions.push_back( itMap2->second.m_anNetFunctions );
				m_mnTransitionArrayPos[l_sUnTransName] = l_nTransPos;
				m_msTransitionNamesById[l_sTransPos] = l_sUnTransName;	
				m_msAnimationTransitionNamesById[l_sColTRName + wxT("_") + itMap2->second.m_sAnimTransInstName] = l_sTransPos;


				//deal with arcs
				//if(m_sNetClass == SP_DS_COLSPN_CLASS)
				{
					//SetStochInputArcDeltaVector(m_msTransitionNames.size()-1,itMap2->second.m_vInputArcs);
					//SetStochOutputArcDeltaVector(m_msTransitionNames.size()-1,itMap2->second.m_vOutputArcs);
				}
				//if(m_sNetClass == SP_DS_COLCPN_CLASS)
				{
					SetContInputArcDeltaVector(l_nTransPos, itMap2->second.m_vInputArcs);
					SetContOutputArcDeltaVector(l_nTransPos, itMap2->second.m_vOutputArcs);
				}
			}			
		}

		if( ! l_bIsoColTrans)
		{			
			m_msColoredTransitionNames.push_back(l_sColTRName);
			l_nUp = m_msTransitionNames.size();
			SP_CPN_CountInterval l_Interval;
			l_Interval.m_nLow = l_nLow;
			l_Interval.m_nUp  = l_nUp-1;
			m_nsTransitionCountById[m_msColoredTransitionNames.size()-1] = l_Interval;
		}
	}
}

/*
void SP_DS_ColPN_Unfolding::SetStochInputArcDeltaVector(long p_nTransArrayPos, vector<SP_CPN_UnfoldedArcInfo>& m_vArcs)
{
	SP_MapLong2Long l_mMultEdge;
	SP_MapLong2Long l_mMultReadEdge;
	SP_MapLong2Long l_mMultInhibEdge;
	
	SP_MapString2Long   l_mnPreplaceName2Pos;
	SP_MapString2Long   l_mnPreplaceName2PosTemp;

	if( p_nTransArrayPos >= (long)( m_abNonConsumingMode.size() ) )
	{
		m_abNonConsumingMode.push_back(true);							
	}

	for(unsigned i = 0; i < m_vArcs.size(); i++)
	{
		SP_CPN_UnfoldedArcInfo& l_ArcInfo = m_vArcs[i];
		wxString l_sColor = l_ArcInfo.m_sColor;
		ModifyPlaceName(l_sColor);
		wxString l_sUnPlaceName = l_ArcInfo.m_sColPlaceName + wxT("_") + l_sColor;
		long l_nPlaceArrayPos = m_mnPlaceArrayPos[ l_sUnPlaceName ];
		long l_nMultiplicity = l_ArcInfo.m_nMultiplicity;

		l_mnPreplaceName2PosTemp[l_sUnPlaceName] = l_nPlaceArrayPos;
		l_mnPreplaceName2Pos[l_sUnPlaceName] = l_nPlaceArrayPos;   //used for mass action functions

		if (l_ArcInfo.m_sArcType == wxT("Edge"))
		{
			m_nOrdinaryArcNum++;
			l_mMultEdge[l_nPlaceArrayPos] += l_nMultiplicity;
			m_abNonConsumingMode[p_nTransArrayPos] = false;			
		}					
		else if(l_ArcInfo.m_sArcType == wxT("Read Edge"))
		{
			m_nReadArcNum++;
			l_mMultReadEdge[l_nPlaceArrayPos] = l_nMultiplicity;
			l_mnPreplaceName2Pos[l_sUnPlaceName] = l_nPlaceArrayPos;
		}
		else if(l_ArcInfo.m_sArcType == wxT("Inhibitor Edge"))
		{
			m_nInhibitorArcNum++;
			l_mMultInhibEdge[l_nPlaceArrayPos] = l_nMultiplicity;						
		}
		else if (l_ArcInfo.m_sArcType == wxT("Equal Edge"))
		{
			m_nEqualArcNum++;
			l_mMultReadEdge[l_nPlaceArrayPos] = l_nMultiplicity;
			l_mMultInhibEdge[l_nPlaceArrayPos] = l_nMultiplicity+1;						
		}
		else if (l_ArcInfo.m_sArcType == wxT("Reset Edge"))
		{
			m_nResetArcNum++;
			//resize the matrix when its size is less than the index
			if( p_nTransArrayPos >= (long)( m_aanResetIncMatrix->GetNumRows() ) )
			{
				m_aanResetIncMatrix->SetNumRows(p_nTransArrayPos+1);
			}
			m_aanResetIncMatrix->SetValue(p_nTransArrayPos, l_nPlaceArrayPos, true);
		}
		else if (l_ArcInfo.m_sArcType == wxT("Modifier Edge"))
		{
			m_nModifierArcNum++;
			//because the edge has no impact on firing of the transition						
		}

		//for APNN
		SP_CPN_ContinuousArc l_StructArc;
		l_StructArc.m_nPlaceID = l_nPlaceArrayPos;
		l_StructArc.m_nTranPos = p_nTransArrayPos;
		l_StructArc.m_sMultiplicity = wxString::Format(wxT("%d"),l_nMultiplicity);
		l_StructArc.m_sArcClass = l_ArcInfo.m_sArcType;
		m_vPreTransitionArc.push_back(l_StructArc);	
		//end
	}

	m_mmPrePlaceArrayPosOfTransitionNode[p_nTransArrayPos] = l_mnPreplaceName2Pos;

	for(unsigned i = 0; i < m_vArcs.size(); i++)
	{
		SP_CPN_UnfoldedArcInfo& l_ArcInfo = m_vArcs[i];
		wxString l_sColor = l_ArcInfo.m_sColor;
		ModifyPlaceName(l_sColor);
		wxString l_sUnPlaceName = l_ArcInfo.m_sColPlaceName + wxT("_") + l_sColor;
		long l_nPlaceArrayPos = l_mnPreplaceName2PosTemp[ l_sUnPlaceName ];

		long l_nMultEdge = l_mMultEdge[l_nPlaceArrayPos];
		long l_nMultRead = l_mMultReadEdge[l_nPlaceArrayPos];
		long l_nMultInhib = l_mMultInhibEdge[l_nPlaceArrayPos];

		SP_PairDouble l_Interval(0,LONG_MAX);
		l_Interval.first = l_nMultRead > l_nMultEdge ? l_nMultRead : l_nMultEdge;
		l_Interval.second = l_nMultInhib > 0 ? l_nMultInhib : LONG_MAX;

		//resize the matrix when its size is less than the index
		if( p_nTransArrayPos >= (long)( m_aanPreIncMatrix->GetNumRows()) )
		{
			m_aanPreIncMatrix->SetNumRows(p_nTransArrayPos+1);
		}
		m_aanPreIncMatrix->SetValue(p_nTransArrayPos, l_nPlaceArrayPos, l_Interval);

		if(m_aanResetIncMatrix->GetValue(p_nTransArrayPos, l_nPlaceArrayPos))
		{
			//resize the matrix when its size is less than the index
			if( p_nTransArrayPos >= (long)( m_aanDeltaIncMatrix->GetNumRows() ))
			{
				m_aanDeltaIncMatrix->SetNumRows(p_nTransArrayPos+1);
			}
			m_aanDeltaIncMatrix->SetValue(p_nTransArrayPos, l_nPlaceArrayPos, 0);
		}
		else
		{
			//resize the matrix when its size is less than the index
			if( p_nTransArrayPos >= (long)( m_aanDeltaIncMatrix->GetNumRows()) )
			{
				m_aanDeltaIncMatrix->SetNumRows(p_nTransArrayPos+1);
			}
			m_aanDeltaIncMatrix->SetValue(p_nTransArrayPos, l_nPlaceArrayPos, -1*l_nMultEdge);
		}
	}
}


void SP_DS_ColPN_Unfolding::SetStochOutputArcDeltaVector(long p_nTransArrayPos, vector<SP_CPN_UnfoldedArcInfo>& m_vArcs)
{
	for(unsigned i = 0; i < m_vArcs.size(); i++)
	{
		m_nOrdinaryArcNum++;
		SP_CPN_UnfoldedArcInfo& l_ArcInfo = m_vArcs[i];
		wxString l_sColor = l_ArcInfo.m_sColor;
		ModifyPlaceName(l_sColor);
		wxString l_sUnPlaceName = l_ArcInfo.m_sColPlaceName + wxT("_") + l_sColor;
		long l_nPlaceArrayPos = m_mnPlaceArrayPos[ l_sUnPlaceName ];
		long l_nMultiplicity = l_ArcInfo.m_nMultiplicity;	

		if( p_nTransArrayPos >= (long)( m_aanDeltaIncMatrix->GetNumRows() ) )
		{
			m_aanDeltaIncMatrix->SetNumRows(p_nTransArrayPos+1);
		}
		m_aanDeltaIncMatrix->SetValue(p_nTransArrayPos, l_nPlaceArrayPos,m_aanDeltaIncMatrix->GetValue(p_nTransArrayPos, l_nPlaceArrayPos) + l_nMultiplicity);

		//for APNN
		SP_CPN_ContinuousArc l_StructArc;
		l_StructArc.m_nPlaceID = l_nPlaceArrayPos;
		l_StructArc.m_nTranPos = p_nTransArrayPos;
		l_StructArc.m_sMultiplicity = wxString::Format(wxT("%d"),l_nMultiplicity);
		l_StructArc.m_sArcClass = l_ArcInfo.m_sArcType;
		m_vPostTransitionArc.push_back(l_StructArc);	
		//end
	}
}
*/


void SP_DS_ColPN_Unfolding::SetContInputArcDeltaVector(long p_nTransArrayPos, vector<SP_CPN_UnfoldedArcInfo>& m_vArcs)
{
	SP_MapString2Long  l_mnPreplaceName2Pos;
	for(unsigned i = 0; i < m_vArcs.size(); i++)
	{
		SP_CPN_UnfoldedArcInfo& l_ArcInfo = m_vArcs[i];
		wxString l_sColor = l_ArcInfo.m_sColor;
		ModifyName(l_sColor);
		wxString l_sUnPlaceName = l_ArcInfo.m_sColPlaceName + wxT("_") + l_sColor;
		long l_nPlaceArrayPos = m_mnPlaceArrayPos[ l_sUnPlaceName ];
		//SP_MESSAGEBOX(l_sUnPlaceName + wxString::Format(wxT(" l_nPlaceArrayPos(%ld)"), p_nTransArrayPos, i, l_nPlaceArrayPos));
		wxString l_sPlaceType = m_vsPlaceNodeType[l_nPlaceArrayPos];
		//SP_MESSAGEBOX(wxString::Format(wxT("trans(%ld) arc(%d)"), p_nTransArrayPos, i));

		wxString l_sMultiplicity;		
		if (l_sPlaceType == wxT("Place"))
			//l_sMultiplicity = wxString::Format(wxT("%ld"), l_ArcInfo.m_nMultiplicity);
			l_sMultiplicity = l_ArcInfo.m_sMultiplicity;
		else
			//l_sMultiplicity = wxString::Format(wxT("%g"), l_ArcInfo.m_dMultiplicity);
			l_sMultiplicity = l_ArcInfo.m_sMultiplicity;

		SP_CPN_ContinuousArc l_structContinuousArc;
		l_structContinuousArc.m_nTranPos = p_nTransArrayPos;
		l_structContinuousArc.m_sArcClass = l_ArcInfo.m_sArcType;
		l_structContinuousArc.m_sMultiplicity = l_sMultiplicity;
		l_structContinuousArc.m_nPlaceID = l_nPlaceArrayPos;

		m_vArcs_Place2Transition.push_back( l_structContinuousArc );

		if (l_ArcInfo.m_sArcType == SP_DS_EDGE)
		{
			m_nOrdinaryArcNum++;
			//m_mPrePlaceToArcsOfTransitionNode[p_nTransArrayPos][l_sUnPlaceName] = l_sMultiplicity;
			l_mnPreplaceName2Pos[l_sUnPlaceName] = l_nPlaceArrayPos;			
			//m_mvPlaceID_PostTransitionID[l_nPlaceArrayPos].push_back( l_structContinuousArc );			
		}					
		else if(l_ArcInfo.m_sArcType == SP_DS_READ_EDGE)
		{
			m_nReadArcNum++;
			//m_mPrePlaceToArcsOfTransitionNode[p_nTransArrayPos][l_sUnPlaceName] = l_sMultiplicity;
			l_mnPreplaceName2Pos[l_sUnPlaceName] = l_nPlaceArrayPos;			
			//m_mvPlaceID_PostTransitionID[l_nPlaceArrayPos].push_back( l_structContinuousArc );					
		}
		else if(l_ArcInfo.m_sArcType == SP_DS_INHIBITOR_EDGE )
		{			
			m_nInhibitorArcNum++;
			//m_mvPlaceID_PostTransitionID[l_nPlaceArrayPos].push_back( l_structContinuousArc );			
		}	
		else if (l_ArcInfo.m_sArcType == SP_DS_EQUAL_EDGE )
		{			
			m_nEqualArcNum++;
		}	
		else if (l_ArcInfo.m_sArcType == SP_DS_RESET_EDGE )
		{			
			m_nResetArcNum++;
		}	
		else if (l_ArcInfo.m_sArcType == SP_DS_MODIFIER_EDGE )
		{			
			m_nModifierArcNum++;
		}				
	}

	m_mmPrePlaceArrayPosOfTransitionNode[p_nTransArrayPos] = l_mnPreplaceName2Pos;
						
}

void SP_DS_ColPN_Unfolding::SetContOutputArcDeltaVector(long p_nTransArrayPos, vector<SP_CPN_UnfoldedArcInfo>& m_vArcs)
{
	SP_MapString2Long  l_mnPreplaceName2Pos;

	for(unsigned i = 0; i < m_vArcs.size(); i++)
	{
		m_nOrdinaryArcNum++;
		SP_CPN_UnfoldedArcInfo& l_ArcInfo = m_vArcs[i];
		wxString l_sColor = l_ArcInfo.m_sColor;
		ModifyName(l_sColor);
		wxString l_sUnPlaceName = l_ArcInfo.m_sColPlaceName + wxT("_") + l_sColor;
		long l_nPlaceArrayPos = m_mnPlaceArrayPos[ l_sUnPlaceName ];
		wxString l_sPlaceType = m_vsPlaceNodeType[l_nPlaceArrayPos];

		wxString l_sMultiplicity;		
		if(l_sPlaceType == wxT("Place"))
			//l_sMultiplicity << l_ArcInfo.m_nMultiplicity;
			l_sMultiplicity = l_ArcInfo.m_sMultiplicity;
		else
			//l_sMultiplicity << l_ArcInfo.m_dMultiplicity;
			l_sMultiplicity = l_ArcInfo.m_sMultiplicity;

		SP_CPN_ContinuousArc l_structContinuousArc;
		l_structContinuousArc.m_nTranPos = p_nTransArrayPos;
		l_structContinuousArc.m_sArcClass = l_ArcInfo.m_sArcType;
		l_structContinuousArc.m_sMultiplicity = l_sMultiplicity;
		l_structContinuousArc.m_nPlaceID = l_nPlaceArrayPos;

		//m_mPostPlace2ArcsOfTransitionNode[p_nTransArrayPos][l_sUnPlaceName] = l_sMultiplicity;
		//m_mvPlaceID_PreTransitionID[l_nPlaceArrayPos].push_back( l_structContinuousArc );	
		m_vArcs_Transition2Place.push_back( l_structContinuousArc );

	}
}

vector<double>* SP_DS_ColPN_Unfolding::GetCurCPNMarking()
{
	if( !(m_sNetClass == SP_DS_COLCPN_CLASS)  )
		return NULL;

	m_adInitialNetMarking.resize(m_anColCPNNetMarkings.size());

	SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >((*(m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_PLACE)->GetElements()->begin()))->GetAttribute(wxT("MarkingList")));
	unsigned l_nCurrentColumn =	l_pcColList->GetActiveColumn();
	
	for( unsigned i = 0; i < m_anColCPNNetMarkings.size(); i++)
	{
		m_adInitialNetMarking[i] = m_anColCPNNetMarkings[i][l_nCurrentColumn];
	}

	return &m_adInitialNetMarking;
}

vector<double>*	SP_DS_ColPN_Unfolding::GetCurHybPNMarking()
{
	/*if( !(m_sNetClass == SP_DS_COLHPN_CLASS)  )
		return NULL;*/

	m_adHybInitialNetMarking.clear();	

	if( m_anColCPNNetMarkings.size()>0 )
	{
		SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >((*(m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_PLACE)->GetElements()->begin()))->GetAttribute(wxT("MarkingList")));
		unsigned l_nCurrentColumn =	l_pcColList->GetActiveColumn();
		
		for( unsigned i = 0; i < m_anColCPNNetMarkings.size(); i++)
		{
			m_adHybInitialNetMarking.push_back( m_anColCPNNetMarkings[i][l_nCurrentColumn] );
		}
	}
	if( m_anNetMarkings.size()>0 )
	{
		SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >((*(m_pcGraph->GetNodeclass(wxT("Place"))->GetElements()->begin()))->GetAttribute(wxT("MarkingList")));
		unsigned l_nCurrentColumn =	l_pcColList->GetActiveColumn();

		for( unsigned i = 0; i < m_anNetMarkings.size(); i++)
		{
			m_adHybInitialNetMarking.push_back( m_anNetMarkings[i][l_nCurrentColumn] );
		}
	}

	return &m_adHybInitialNetMarking;
}


vector<long>* SP_DS_ColPN_Unfolding::GetCurColStMarking()
{
	if( m_sNetClass == SP_DS_COLCPN_CLASS
		|| m_sNetClass == SP_DS_COLHPN_CLASS )
		return NULL;

	m_anInitialNetMarking.resize(m_anNetMarkings.size());

	SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >((*(m_pcGraph->GetNodeclass(wxT("Place"))->GetElements()->begin()))->GetAttribute(wxT("MarkingList")));
	unsigned l_nCurrentColumn =	l_pcColList->GetActiveColumn();

	for( unsigned i = 0; i < m_anNetMarkings.size(); i++)
	{
		m_anInitialNetMarking[i] = m_anNetMarkings[i][l_nCurrentColumn];
	}

	return &m_anInitialNetMarking;
}



SP_VectorString* SP_DS_ColPN_Unfolding::GetCurRateFunction()
{

	if( !(m_sNetClass == SP_DS_COLSPN_CLASS||
		m_sNetClass == SP_DS_COLCPN_CLASS||
		m_sNetClass == SP_DS_COLHPN_CLASS) )
		return NULL;

	unsigned int l_nContTrCurrentColumn;
	unsigned int l_nStochTrCurrentColumn;
	unsigned int l_nImmTrCurrentColumn;
	unsigned int l_nDetTrCurrentColumn;
	unsigned int l_nSchedTrCurrentColumn;

	SP_DS_Nodeclass* l_pcNodeclass;
	SP_DS_ColListAttribute* l_pcColList;

	l_pcNodeclass= m_pcGraph->GetNodeclass( SP_DS_CONTINUOUS_TRANS );
	if(l_pcNodeclass)
	{
		if(l_pcNodeclass->GetElements()->size() > 0)
		{
			l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >((*(l_pcNodeclass->GetElements()->begin()))->GetAttribute(wxT("FunctionList")));
			l_nContTrCurrentColumn = l_pcColList->GetActiveColumn();
		}
	}

	l_pcNodeclass= m_pcGraph->GetNodeclass( wxT("Transition") );
	if(l_pcNodeclass)
	{		
		if(l_pcNodeclass->GetElements()->size() > 0)
		{
			l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >((*(l_pcNodeclass->GetElements()->begin()))->GetAttribute(wxT("FunctionList")));
			l_nStochTrCurrentColumn = l_pcColList->GetActiveColumn();
		}
	}

	l_pcNodeclass= m_pcGraph->GetNodeclass(wxT("Immediate Transition"));
	if(l_pcNodeclass)
	{		
		if(l_pcNodeclass->GetElements()->size() > 0)
		{
			l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >((*(l_pcNodeclass->GetElements()->begin()))->GetAttribute(wxT("FunctionList")));
			l_nImmTrCurrentColumn = l_pcColList->GetActiveColumn();
		}
	}

	l_pcNodeclass= m_pcGraph->GetNodeclass(wxT("Deterministic Transition"));
	if(l_pcNodeclass)
	{
		if(l_pcNodeclass->GetElements()->size() > 0)
		{
			l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >((*(l_pcNodeclass->GetElements()->begin()))->GetAttribute(wxT("DelayList")));
			l_nDetTrCurrentColumn =	l_pcColList->GetActiveColumn();
		}
	}

	l_pcNodeclass= m_pcGraph->GetNodeclass(wxT("Scheduled Transition"));
	if(l_pcNodeclass)
	{		
		if(l_pcNodeclass->GetElements()->size() > 0)
		{
			l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >((*(l_pcNodeclass->GetElements()->begin()))->GetAttribute(wxT("PeriodicList")));
			l_nSchedTrCurrentColumn =	l_pcColList->GetActiveColumn();
		}
	}

	//initilize
	m_msTransitionFunctions.assign(m_anNetFunctions.size(), wxT("0"));

	for(unsigned int i = 0; i < m_anNetFunctions.size(); i++ )
	{
		unsigned int l_nCurrentColumn;
		if( m_vsTransNodeType[i] == SP_DS_CONTINUOUS_TRANS )
			l_nCurrentColumn = l_nContTrCurrentColumn;
		if( m_vsTransNodeType[i] == wxT("Transition") )
			l_nCurrentColumn = l_nStochTrCurrentColumn;
		if( m_vsTransNodeType[i] == wxT("Immediate Transition") )
			l_nCurrentColumn = l_nImmTrCurrentColumn;
		if( m_vsTransNodeType[i] == wxT("Deterministic Transition") )
			l_nCurrentColumn = l_nDetTrCurrentColumn;
		if( m_vsTransNodeType[i] == wxT("Scheduled Transition") )
			l_nCurrentColumn = l_nSchedTrCurrentColumn;

		m_msTransitionFunctions[i] = m_anNetFunctions[i][l_nCurrentColumn];
	}
	
    return &m_msTransitionFunctions;
}









void SP_DS_ColPN_Unfolding::ThreadFinished()
{
	wxCriticalSectionLocker locker(m_critsect);
	++m_nThreadFinished;
}

void SP_DS_ColPN_Unfolding::ModifyName(wxString &p_sPlaceName)
{
	wxString l_sNameRegEx = wxT("[^0-9a-zA-Z_:.]");
	wxRegEx l_cRegEx;
	if(l_cRegEx.Compile(l_sNameRegEx))
	{
		l_cRegEx.Replace(&p_sPlaceName, wxT("_"));
	}
}


void SP_DS_ColPN_Unfolding::GetTransInstances(SP_CPN_UnfoldedTransition & p_mUnfoldedOneTransInfo, SP_CPN_UnfoldedTransition& p_msstUnfoldedTransInfo)
{
		SP_CPN_UnfoldedTransition::iterator itMap2;
		for( itMap2 = p_mUnfoldedOneTransInfo.begin(); itMap2 != p_mUnfoldedOneTransInfo.end(); itMap2++)
		{
			SP_CPN_UnfoldedTransInfo l_stUnfoldedTransInfo;
			wxString l_sBinding = wxString(itMap2->first.c_str());			
			l_stUnfoldedTransInfo.m_sAnimTransInstName = wxString(itMap2->second.m_sAnimTransInstName.c_str());
			l_stUnfoldedTransInfo.m_sType = wxString(itMap2->second.m_sType.c_str());

			vector<wxString> l_vFunctions;
			for(unsigned int k = 0; k < itMap2->second.m_anNetFunctions.size(); k++)
			{
				l_vFunctions.push_back(wxString(itMap2->second.m_anNetFunctions[k].c_str()));
			}
			l_stUnfoldedTransInfo.m_anNetFunctions = l_vFunctions;

			vector<SP_CPN_UnfoldedArcInfo> l_vInputArcs;	
			for(unsigned int k = 0; k < itMap2->second.m_vInputArcs.size(); k++)
			{
				SP_CPN_UnfoldedArcInfo l_stUnfoldedArcInfo;
				l_stUnfoldedArcInfo.m_dMultiplicity = itMap2->second.m_vInputArcs[k].m_dMultiplicity;
				l_stUnfoldedArcInfo.m_nMultiplicity = itMap2->second.m_vInputArcs[k].m_nMultiplicity;
				l_stUnfoldedArcInfo.m_sMultiplicity = itMap2->second.m_vInputArcs[k].m_sMultiplicity;
				l_stUnfoldedArcInfo.m_sArcType = wxString(itMap2->second.m_vInputArcs[k].m_sArcType.c_str());
				l_stUnfoldedArcInfo.m_sColor = wxString(itMap2->second.m_vInputArcs[k].m_sColor.c_str());
				l_stUnfoldedArcInfo.m_sColPlaceName = wxString(itMap2->second.m_vInputArcs[k].m_sColPlaceName.c_str());
				l_stUnfoldedArcInfo.m_sDiscContType = wxString(itMap2->second.m_vInputArcs[k].m_sDiscContType.c_str());
				l_vInputArcs.push_back(l_stUnfoldedArcInfo);
			}
			l_stUnfoldedTransInfo.m_vInputArcs = l_vInputArcs;
			
			vector<SP_CPN_UnfoldedArcInfo> l_vOutputArcs;
			for(unsigned int k = 0; k < itMap2->second.m_vOutputArcs.size(); k++)
			{
				SP_CPN_UnfoldedArcInfo l_stUnfoldedArcInfo;
				l_stUnfoldedArcInfo.m_dMultiplicity = itMap2->second.m_vOutputArcs[k].m_dMultiplicity;
				l_stUnfoldedArcInfo.m_nMultiplicity = itMap2->second.m_vOutputArcs[k].m_nMultiplicity;
				l_stUnfoldedArcInfo.m_sMultiplicity = itMap2->second.m_vOutputArcs[k].m_sMultiplicity;
				l_stUnfoldedArcInfo.m_sArcType = wxString(itMap2->second.m_vOutputArcs[k].m_sArcType.c_str());
				l_stUnfoldedArcInfo.m_sColor = wxString(itMap2->second.m_vOutputArcs[k].m_sColor.c_str());
				l_stUnfoldedArcInfo.m_sColPlaceName = wxString(itMap2->second.m_vOutputArcs[k].m_sColPlaceName.c_str());
				l_stUnfoldedArcInfo.m_sDiscContType = wxString(itMap2->second.m_vOutputArcs[k].m_sDiscContType.c_str());
				l_vOutputArcs.push_back(l_stUnfoldedArcInfo);
			}
			l_stUnfoldedTransInfo.m_vOutputArcs = l_vOutputArcs;

			p_msstUnfoldedTransInfo[l_sBinding]=l_stUnfoldedTransInfo;
		}

}

void SP_DS_ColPN_Unfolding::ThreadAddStochResult(SP_CPN_UnfoldedTransitions& p_mmUnfoldedStochTransions)
{
	wxCriticalSectionLocker locker(m_critsect);

	SP_CPN_UnfoldedTransitions::iterator itMap1;
	for( itMap1 = p_mmUnfoldedStochTransions.begin(); itMap1 != p_mmUnfoldedStochTransions.end(); itMap1++)
	{
		wxString l_sColTransName = wxString(itMap1->first.c_str());		
		SP_CPN_UnfoldedTransition l_msstUnfoldedTransInfo;

		GetTransInstances(itMap1->second, l_msstUnfoldedTransInfo);

		/*
		SP_CPN_UnfoldedTransition::iterator itMap2;
		for( itMap2 = itMap1->second.begin(); itMap2 != itMap1->second.end(); itMap2++)
		{
			SP_CPN_UnfoldedTransInfo l_stUnfoldedTransInfo;
			wxString l_sBinding = wxString(itMap2->first.c_str());			
			l_stUnfoldedTransInfo.m_sAnimTransInstName = wxString(itMap2->second.m_sAnimTransInstName.c_str());
			l_stUnfoldedTransInfo.m_sType = wxString(itMap2->second.m_sType.c_str());

			vector<wxString> l_vFunctions;
			for(unsigned int k = 0; k < itMap2->second.m_anNetFunctions.size(); k++)
			{
				l_vFunctions.push_back(wxString(itMap2->second.m_anNetFunctions[k].c_str()));
			}
			l_stUnfoldedTransInfo.m_anNetFunctions = l_vFunctions;

			vector<SP_CPN_UnfoldedArcInfo> l_vInputArcs;	
			for(unsigned int k = 0; k < itMap2->second.m_vInputArcs.size(); k++)
			{
				SP_CPN_UnfoldedArcInfo l_stUnfoldedArcInfo;
				l_stUnfoldedArcInfo.m_dMultiplicity = itMap2->second.m_vInputArcs[k].m_dMultiplicity;
				l_stUnfoldedArcInfo.m_nMultiplicity = itMap2->second.m_vInputArcs[k].m_nMultiplicity;
				l_stUnfoldedArcInfo.m_sArcType = wxString(itMap2->second.m_vInputArcs[k].m_sArcType.c_str());
				l_stUnfoldedArcInfo.m_sColor = wxString(itMap2->second.m_vInputArcs[k].m_sColor.c_str());
				l_stUnfoldedArcInfo.m_sColPlaceName = wxString(itMap2->second.m_vInputArcs[k].m_sColPlaceName.c_str());
				l_stUnfoldedArcInfo.m_sDiscContType = wxString(itMap2->second.m_vInputArcs[k].m_sDiscContType.c_str());
				l_vInputArcs.push_back(l_stUnfoldedArcInfo);
			}
			l_stUnfoldedTransInfo.m_vInputArcs = l_vInputArcs;
			
			vector<SP_CPN_UnfoldedArcInfo> l_vOutputArcs;
			for(unsigned int k = 0; k < itMap2->second.m_vOutputArcs.size(); k++)
			{
				SP_CPN_UnfoldedArcInfo l_stUnfoldedArcInfo;
				l_stUnfoldedArcInfo.m_dMultiplicity = itMap2->second.m_vOutputArcs[k].m_dMultiplicity;
				l_stUnfoldedArcInfo.m_nMultiplicity = itMap2->second.m_vOutputArcs[k].m_nMultiplicity;
				l_stUnfoldedArcInfo.m_sArcType = wxString(itMap2->second.m_vOutputArcs[k].m_sArcType.c_str());
				l_stUnfoldedArcInfo.m_sColor = wxString(itMap2->second.m_vOutputArcs[k].m_sColor.c_str());
				l_stUnfoldedArcInfo.m_sColPlaceName = wxString(itMap2->second.m_vOutputArcs[k].m_sColPlaceName.c_str());
				l_stUnfoldedArcInfo.m_sDiscContType = wxString(itMap2->second.m_vOutputArcs[k].m_sDiscContType.c_str());
				l_vOutputArcs.push_back(l_stUnfoldedArcInfo);
			}
			l_stUnfoldedTransInfo.m_vOutputArcs = l_vOutputArcs;

			l_msstUnfoldedTransInfo[l_sBinding]=l_stUnfoldedTransInfo;
		}
		*/

		m_mmUnfoldedStochTransions[l_sColTransName] = l_msstUnfoldedTransInfo;
	}	
}

void SP_DS_ColPN_Unfolding::ThreadAddContResult(SP_CPN_UnfoldedTransitions& p_mmUnfoldedContTransions)
{
	wxCriticalSectionLocker locker(m_critsect);

	
	SP_CPN_UnfoldedTransitions::iterator itMap1;
	for( itMap1 = p_mmUnfoldedContTransions.begin(); itMap1 != p_mmUnfoldedContTransions.end(); itMap1++)
	{
		wxString l_sColTransName = wxString(itMap1->first.c_str());		
		SP_CPN_UnfoldedTransition l_msstUnfoldedTransInfo;

		GetTransInstances(itMap1->second, l_msstUnfoldedTransInfo);

		m_mmUnfoldedContTransions[l_sColTransName] = l_msstUnfoldedTransInfo;
	}	

}
void SP_DS_ColPN_Unfolding::ThreadAddImmResult(SP_CPN_UnfoldedTransitions& p_mmUnfoldedImmTransions)
{
	wxCriticalSectionLocker locker(m_critsect);

	SP_CPN_UnfoldedTransitions::iterator itMap1;
	for( itMap1 = p_mmUnfoldedImmTransions.begin(); itMap1 != p_mmUnfoldedImmTransions.end(); itMap1++)
	{
		wxString l_sColTransName = wxString(itMap1->first.c_str());		
		SP_CPN_UnfoldedTransition l_msstUnfoldedTransInfo;
		
		GetTransInstances(itMap1->second, l_msstUnfoldedTransInfo);

		m_mmUnfoldedImmTransions[l_sColTransName] = l_msstUnfoldedTransInfo;
	}	

	
}
void SP_DS_ColPN_Unfolding::ThreadAddDetResult(SP_CPN_UnfoldedTransitions& p_mmUnfoldedDetTransions)
{
	wxCriticalSectionLocker locker(m_critsect);

	SP_CPN_UnfoldedTransitions::iterator itMap1;
	for( itMap1 = p_mmUnfoldedDetTransions.begin(); itMap1 != p_mmUnfoldedDetTransions.end(); itMap1++)
	{
		wxString l_sColTransName = wxString(itMap1->first.c_str());		
		SP_CPN_UnfoldedTransition l_msstUnfoldedTransInfo;
		
		GetTransInstances(itMap1->second, l_msstUnfoldedTransInfo);

		m_mmUnfoldedDetTransions[l_sColTransName] = l_msstUnfoldedTransInfo;
	}	
	
}
void SP_DS_ColPN_Unfolding::ThreadAddSchedResult(SP_CPN_UnfoldedTransitions& p_mmUnfoldedSchedTransions)
{
	wxCriticalSectionLocker locker(m_critsect);

	SP_CPN_UnfoldedTransitions::iterator itMap1;
	for( itMap1 = p_mmUnfoldedSchedTransions.begin(); itMap1 != p_mmUnfoldedSchedTransions.end(); itMap1++)
	{
		wxString l_sColTransName = wxString(itMap1->first.c_str());		
		SP_CPN_UnfoldedTransition l_msstUnfoldedTransInfo;

		GetTransInstances(itMap1->second, l_msstUnfoldedTransInfo);

		m_mmUnfoldedSchedTransions[l_sColTransName] = l_msstUnfoldedTransInfo;
	}	

	
}

void SP_DS_ColPN_Unfolding::SetThreadSuccess(bool p_bThreadSuccess)
{
	wxCriticalSectionLocker locker(m_critsect);
	m_bThreadSuccess = p_bThreadSuccess;
}

void SP_DS_ColPN_Unfolding::UpdateUnfoldDlg(unsigned long p_nFinishedNumber)
{
	bool l_bUpdate = false;
	long l_nUpdateTime = m_cUnfoldingStopWatch.Time();
	if((m_nLastUpdateTime + 5000) < l_nUpdateTime
		|| m_nLastUpdateTime > l_nUpdateTime)
	{
		l_bUpdate = true;
		m_nLastUpdateTime = l_nUpdateTime;
	}

	unsigned long l_nIndicator = p_nFinishedNumber/m_nTranNumInFirstThread * 100;
	if(p_nFinishedNumber>0)
		l_bUpdate = true;

	if(l_bUpdate)
	{
		m_pcUnfoldDlg->SetUnfoldingProgressGauge(l_nIndicator);
		m_pcUnfoldDlg->SetUnfoldingStopWatch(l_nUpdateTime);
		m_pcUnfoldDlg->Update();

		//CHECK_EVENTS(IsUnfoldingRunning(), return);

	}
}



bool SP_DS_ColPN_Unfolding::WriteToFile()
{
	if (!m_file.Open(m_sSaveFilename.c_str(), wxT("wt")) )
	{
		return false;
	}	

	// close file handle and check if this was ok
	CHECK_BOOL((m_file.Close()), return false);

	return true;
}



bool SP_DS_ColPN_Unfolding::SumPlaceInstances(map<wxString, wxString> *p_pmPlaceToColorSet, wxString &p_sRateFunExpr, SP_CPN_ColorSetClass* p_pcColorSetClass, bool p_bCheck)
{
	SP_DS_ColPN_Unfolding::UnfoldThread l_cUnfold;
	if( !l_cUnfold.SumPlaceInstances(p_pmPlaceToColorSet, p_sRateFunExpr, p_pcColorSetClass, p_bCheck) )
		return false;

	return true;
}


///////////////////////////////////////////////////////////////////////////////////
//thread unfolding
///////////////////////////////////////////////////////////////////////////////////



#if 1


bool SP_DS_ColPN_Unfolding::UnfoldThread::Unfold()
{	
	if( ! UnfoldTransitons() )
		return false;
	
	return true;
}

bool SP_DS_ColPN_Unfolding::UnfoldThread::UnfoldTransitons()
{
	m_InitGraphMutex.Lock();
	m_sNetClass = wxString(m_pcGraph->GetNetclass()->GetName().c_str());
	m_InitGraphMutex.Unlock();

	for(unsigned int i=0; i < m_vTransitions.size(); i++)
	{
		m_pcTransNode = m_vTransitions[i];		 
		m_InitGraphMutex.Lock();
		m_sTransNodeClass = wxString(m_pcTransNode->GetNodeclass()->GetName().c_str());
		m_sColoredTransName = wxString(dynamic_cast<SP_DS_NameAttribute*>(m_pcTransNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue().c_str());
		m_InitGraphMutex.Unlock();

		if(!UnfoldOneTransition())
				return false;	

		m_nCurrentFinished++;
	}	

	return true;
}

bool SP_DS_ColPN_Unfolding::UnfoldThread::UnfoldTransNodeClass(wxString p_sTransNodeClass)
{
/*	SP_DS_Nodeclass* l_pcNodeclass;
	SP_ListNode::const_iterator l_itElem;
	l_pcNodeclass= m_pcGraph->GetNodeclass( p_sTransNodeClass );
	if( l_pcNodeclass )
	{
		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{			
			SP_DS_Node* l_pcTransNode = *l_itElem;

			wxString l_sTransName = dynamic_cast<SP_DS_NameAttribute*>(l_pcTransNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

			if(!UnfoldOneTransition(l_pcTransNode))
				return false;			
		}	
	}
*/
	return true;
}


bool SP_DS_ColPN_Unfolding::UnfoldThread::UnfoldOneTransition()
{
	//clear 
	m_mTransInstances.clear();		

	m_bCSPFlag = false;

	if(!InitializeParsedExprs())
		return false;

	if( ! BuildParserTree() )
		return false;

	CollectVariables();

	//constant
	if(m_vVar2ColorSet.size() == 0)    
	{
		if( ! ProcessConstantTransition() )
			return false;		
	}
	else
	{
		//check if the guard has been computed
		if( m_vvExisBindMatrix.find(m_ExpressionVector[0].m_sExpression) != m_vvExisBindMatrix.end() )
		{
			m_vvCSPAssigns = m_vvExisBindMatrix.find(m_ExpressionVector[0].m_sExpression)->second;
			m_bCSPFlag = true;
			if( ! ProcessTransitionByCSP() )
				return false;
		}
		else
		{
			//add constraint solover 
			//test if the color set of each variable is integer and successive, e.g. int from 1-100
			bool l_bIntSuccessive = IntCSSuccessiveTest();
			bool l_bCSPSucceed = false;
			if( m_vGuardVar2ColorSet.size() > 0 && l_bIntSuccessive && m_bCSPSolve)
			{
				l_bCSPSucceed = CSPSolve();
			}

			if(l_bCSPSucceed )
			{
				//begin to use CSP results
				m_bCSPFlag = true;
				if( ! ProcessTransitionByCSP() )
					return false;
				m_vvExisBindMatrix[m_ExpressionVector[0].m_sExpression] = m_vvCSPAssigns;
			}	
			else
			{
				//old unfolding way	//at least one variable	
				m_bCSPFlag = false;
				if( ! ProcessVariableTransition() )
					return false;
			}
		}
	}

	m_ExpressionVector.clear();
	
	//get instances
	if( m_sTransNodeClass.IsSameAs(SP_DS_CONTINUOUS_TRANS)	)
		m_mmUnfoldedContTransions[m_sColoredTransName] = m_mTransInstances;

	if( m_sTransNodeClass.IsSameAs(SP_DS_STOCHASTIC_TRANS)	)
		m_mmUnfoldedStochTransions[m_sColoredTransName] = m_mTransInstances;

	if( m_sTransNodeClass.IsSameAs(wxT("Immediate Transition") ))
		m_mmUnfoldedImmTransions[m_sColoredTransName] = m_mTransInstances;

	if( m_sTransNodeClass.IsSameAs(wxT("Deterministic Transition") ))
		m_mmUnfoldedDetTransions[m_sColoredTransName] = m_mTransInstances;

	if( m_sTransNodeClass.IsSameAs(wxT("Scheduled Transition") ) )
		m_mmUnfoldedSchedTransions[m_sColoredTransName] = m_mTransInstances;	

	return true;
}



bool SP_DS_ColPN_Unfolding::UnfoldThread::InitializeParsedExprs()
{
	m_ExpressionVector.clear();

	wxMutexLocker locker(m_InitGraphMutex);

	// the first element will always be the guard
	if(!CollectGuard())
	{
		return false;
	}

	if(!CollectPreArcExprs())
	{
		return false;
	}

	if(!CollectPostArcExprs())
	{
		return false;
	}
	
	if( m_sNetClass == SP_DS_COLSPN_CLASS||
		m_sNetClass == SP_DS_COLCPN_CLASS||
		m_sNetClass == SP_DS_COLHPN_CLASS)
	{
		if(!CollectRateFunction())
		{
			return false;
		}
	}

	return true;
}


bool SP_DS_ColPN_Unfolding::UnfoldThread::CollectPreArcExprs()
{
	SP_ListEdge::const_iterator l_itEdges;	
	SP_DS_ColListAttribute* l_pcColList;
	SP_DS_TextAttribute* l_pcCSAttibute;

	m_mPrePlaceName2ColorSetName.clear();

	//push back the preplace and edges
	for (l_itEdges = m_pcTransNode->GetTargetEdges()->begin(); l_itEdges != m_pcTransNode->GetTargetEdges()->end(); ++l_itEdges)
	{		
		if ((*l_itEdges)->GetSource())
		{
			SP_CPN_Expression l_Expression;
			
			//m_InitGraphMutex.Lock();
			SP_DS_Node* l_pcPlaceNode = dynamic_cast<SP_DS_Node*>((*l_itEdges)->GetSource());			
			l_pcCSAttibute = dynamic_cast< SP_DS_TextAttribute* >(l_pcPlaceNode->GetAttribute(SP_DS_CPN_COLORSETNAME) );
			if(!l_pcCSAttibute)
				return false;
			wxString l_sPlaceName = wxString(dynamic_cast<SP_DS_NameAttribute*>(l_pcPlaceNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue().c_str());			
			l_Expression.m_sColorSetName = wxString(l_pcCSAttibute->GetValue().c_str());
			l_Expression.m_sPlaceNodeClass = wxString(l_pcPlaceNode->GetNodeclass()->GetName().c_str());	
			l_Expression.m_sEdgeClass =wxString((*l_itEdges)->GetClassName().c_str());
			//m_InitGraphMutex.Unlock();

			

			l_Expression.m_pcPlaceNode = l_pcPlaceNode;
			l_Expression.m_sColPlaceName = l_sPlaceName;

			//SP_DS_Node* l_pcTransNode = dynamic_cast<SP_DS_Node*>((*l_itEdges)->GetTarget());
			l_Expression.m_eExprType = CPN_INPUT_EXPR;
			
			l_Expression.m_pcTransNode = m_pcTransNode;
			l_Expression.m_pcEdge = *l_itEdges;
			
			l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >((*l_itEdges)->GetAttribute(SP_DS_CPN_INSCRIPTION) );
			if( ! l_pcColList)
				return false;

			l_Expression.m_sExpression = wxString(l_pcColList->GetCell(0, 1).c_str());

			if(l_Expression.m_sExpression == wxT(""))
			{
				wxString l_sError;					
				l_sError = wxT("Arc exprssion should not be empty");				
				//SP_LOGERROR(l_sError);
				return false;
			}							
									
			l_Expression.m_sErrorPosition  = l_sPlaceName + wxT("->") + m_sColoredTransName;

			m_ExpressionVector.push_back(l_Expression);

			// for sumPlaceInstances function					
			m_mPrePlaceName2ColorSetName[l_sPlaceName] = l_Expression.m_sColorSetName;			
		}	
	}
	
	return true;
}

bool SP_DS_ColPN_Unfolding::UnfoldThread::CollectPostArcExprs()
{
	SP_ListEdge::const_iterator l_itEdges;	
	SP_DS_ColListAttribute* l_pcColList;
	SP_DS_TextAttribute* l_pcCSAttibute;

	//push back the postplace and edges
	for (l_itEdges = m_pcTransNode->GetSourceEdges()->begin(); l_itEdges != m_pcTransNode->GetSourceEdges()->end(); ++l_itEdges)
	{		
		if ((*l_itEdges)->GetTarget())
		{	
			SP_CPN_Expression l_Expression;
			
			//m_InitGraphMutex.Lock();
			SP_DS_Node* l_pcPlaceNode = dynamic_cast<SP_DS_Node*>((*l_itEdges)->GetTarget());			
			l_pcCSAttibute = dynamic_cast< SP_DS_TextAttribute* >(l_pcPlaceNode->GetAttribute(SP_DS_CPN_COLORSETNAME) );
			if(!l_pcCSAttibute)
				return false;
			l_Expression.m_sColorSetName = wxString(l_pcCSAttibute->GetValue().c_str());
			wxString l_sPlaceName = wxString(dynamic_cast<SP_DS_NameAttribute*>(l_pcPlaceNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue().c_str());
			l_Expression.m_sPlaceNodeClass = wxString(l_pcPlaceNode->GetNodeclass()->GetName().c_str());
			//m_InitGraphMutex.Unlock();

			l_Expression.m_pcPlaceNode = l_pcPlaceNode;
			l_Expression.m_sColPlaceName = l_sPlaceName;

			l_Expression.m_sEdgeClass = wxString((*l_itEdges)->GetClassName().c_str());

			//SP_DS_Node* l_pcTransNode = dynamic_cast<SP_DS_Node*>((*l_itEdges)->GetSource());
			l_Expression.m_eExprType = CPN_OUTPUT_EXPR;
			
			l_Expression.m_pcTransNode = m_pcTransNode;
			l_Expression.m_pcEdge = *l_itEdges;
			
			l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >((*l_itEdges)->GetAttribute(SP_DS_CPN_INSCRIPTION) );
			if( ! l_pcColList)
				return false;

			l_Expression.m_sExpression = wxString(l_pcColList->GetCell(0, 1).c_str());		
			if(l_Expression.m_sExpression == wxT(""))
			{
				wxString l_sError;					
				l_sError = wxT("Arc exprssion should not be empty");				
				//SP_LOGERROR(l_sError);
				return false;
			}							
				
			
			l_Expression.m_sErrorPosition  = m_sColoredTransName + wxT("->") + l_sPlaceName;


			m_ExpressionVector.push_back(l_Expression);
		}	
	}

	return true;
}
bool SP_DS_ColPN_Unfolding::UnfoldThread::CollectGuard()
{
	SP_CPN_Expression l_Expression;	
	SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(m_pcTransNode->GetAttribute(SP_DS_CPN_GUARDLIST) );
	if(!l_pcColList)
		return false;

	l_Expression.m_eExprType = CPN_GUARD_EXPR;	

	l_Expression.m_sExpression = wxString(l_pcColList->GetCell(0, 1).c_str());

	if(l_Expression.m_sExpression == wxT(""))
	{
		l_Expression.m_sExpression = wxT("true");  //if there is no guard, we set it to true
	}
	
	l_Expression.m_sErrorPosition  = m_sColoredTransName;
	
	m_ExpressionVector.push_back(l_Expression);

	return true;
}

bool SP_DS_ColPN_Unfolding::UnfoldThread::CollectRateFunction()
{
	if(!CollectStochContRateFunction())
		return false;
	if(!CollectImmRateFunction())
		return false;
	if(!CollectDetRateFunction())
		return false;
	if(!CollectScheduleRateFunction())
		return false;
	return true;
}

bool SP_DS_ColPN_Unfolding::UnfoldThread::CollectStochContRateFunction()
{
	//push back the predicates and rate functions
	
	if( m_sTransNodeClass == SP_DS_CONTINUOUS_TRANS  ||
		m_sTransNodeClass == SP_DS_STOCHASTIC_TRANS  
		//m_sTransNodeClass == SP_DS_IMMEDIATE_TRANS  
	  )	
	{			
			SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(m_pcTransNode->GetAttribute(wxT("FunctionList")) );
			m_nTransRateColumn = l_pcColList->GetColCount()-1;
			for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
			{				
				wxString l_sPredicate = wxString(l_pcColList->GetCell(i, 0).c_str());
				if( l_sPredicate  == wxT("") )
					continue; //omit the empty line

				vector<wxString> l_RateFunctionVector;
				vector<map<wxString, SP_ColPN_RateFunction> > l_RateFunctionTransformation;				
				for (unsigned int j = 1; j < l_pcColList->GetColCount(); j++)
				{
					wxString l_sFormula = wxString(l_pcColList->GetCell(i, j).c_str());
					//transform rate functions
					map<wxString, SP_ColPN_RateFunction> l_mColPlace2Color;
					SP_ColPN_ProcessRateFunction l_cProcessRateFunc;
					//m_InitGraphMutex.Lock();
					if( ! l_cProcessRateFunc.ProceedRateFunction(l_sFormula,m_pcTransNode,&l_mColPlace2Color) )
					{
						//m_InitGraphMutex.Unlock();
						return false;
					}
					//m_InitGraphMutex.Unlock();
					l_RateFunctionTransformation.push_back( l_mColPlace2Color );

					map<wxString, SP_ColPN_RateFunction>::iterator itMap;
					for(itMap = l_mColPlace2Color.begin(); itMap != l_mColPlace2Color.end(); itMap++)
					{
						SP_CPN_Expression l_Expression;
						l_Expression.m_eExprType = CPN_RATEFUNCTION_EXPR;
						//if( m_sTransNodeClass == wxT("Immediate Transition"))
						//{
							//l_Expression.m_sExpression = wxT("ImmediateFiring(") + itMap->second.m_sFormula + wxT(")");
						//}
						//else
						//{
							l_Expression.m_sExpression = itMap->second.m_sFormula;
						//}						
						l_Expression.m_sColorSetName = m_mPrePlaceName2ColorSetName[itMap->second.m_sColPlace];		
						l_Expression.m_sPredicate = l_sPredicate;
						l_Expression.m_nColumn = j-1;
						m_ExpressionVector.push_back(l_Expression);
					}

					//deal with SumPlaceInstances function
					if( ! SumPlaceInstances(&m_mPrePlaceName2ColorSetName, l_sFormula, &m_ColorSetClass) )
						return false;

					l_RateFunctionVector.push_back( l_sFormula );
				}

				if( l_sPredicate  != wxT("") )
				{
					SP_CPN_Expression l_Expression;
					l_Expression.m_eExprType = CPN_PREDICATE_EXPR;					
					l_Expression.m_sExpression = l_sPredicate;
					l_Expression.m_RateFunctionVector = l_RateFunctionVector;
					l_Expression.m_RateFunctionTransformation = l_RateFunctionTransformation;					
					l_Expression.m_sErrorPosition  = m_sColoredTransName;

					m_ExpressionVector.push_back(l_Expression);
				}
			}
		
		}

	return true;
}


bool SP_DS_ColPN_Unfolding::UnfoldThread::CollectImmRateFunction()
{
	if(m_sTransNodeClass == wxT("Immediate Transition"))
		{
			SP_DS_ColListAttribute*  l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(m_pcTransNode->GetAttribute(wxT("FunctionList")) );			
			m_nTransRateColumn = l_pcColList->GetColCount()-1;			
			for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
			{				
				SP_CPN_Expression l_Expression;
				wxString l_sPredicate = wxString(l_pcColList->GetCell(i, 0).c_str());
				if( l_sPredicate  == wxT("") )
					continue;

				vector<wxString> l_RateFunctionVector;

				for (unsigned int j = 1; j < l_pcColList->GetColCount(); j++)
				{
					
					wxString l_sDelay = wxString(l_pcColList->GetCell(i, j).c_str());					
					//l_sDelay = wxT("ImmediateFiring(")+ l_sDelay +wxT(")");
					l_RateFunctionVector.push_back(l_sDelay);
				}

				if( l_sPredicate  != wxT("") )
				{
					l_Expression.m_eExprType = CPN_PREDICATE_EXPR;
					l_Expression.m_sExpression = l_sPredicate;
					l_Expression.m_RateFunctionVector = l_RateFunctionVector;
					
					l_Expression.m_sErrorPosition  = m_sColoredTransName;

					m_ExpressionVector.push_back(l_Expression);
				}
			}
		}

	return true;
}

bool SP_DS_ColPN_Unfolding::UnfoldThread::CollectDetRateFunction()
{
	if(m_sTransNodeClass == wxT("Deterministic Transition"))
		{
			SP_DS_ColListAttribute*  l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(m_pcTransNode->GetAttribute(wxT("DelayList")) );			
			m_nTransRateColumn = l_pcColList->GetColCount()-1;
			//srand((unsigned)time(0));
			for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
			{				
				SP_CPN_Expression l_Expression;
				wxString l_sPredicate = wxString(l_pcColList->GetCell(i, 0).c_str());
				if( l_sPredicate  == wxT("") )
					continue;

				vector<wxString> l_RateFunctionVector;

				for (unsigned int j = 1; j < l_pcColList->GetColCount(); j++)
				{
					//add codes to deal with random
					wxString l_sDelay = wxString(l_pcColList->GetCell(i, j).c_str());
					//l_sDelay = HandleRandom(l_sDelay);
					//l_sDelay = wxT("TimedFiring(")+ l_sDelay +wxT(")");
					l_RateFunctionVector.push_back(l_sDelay);
				}

				if( l_sPredicate  != wxT("") )
				{
					l_Expression.m_eExprType = CPN_PREDICATE_EXPR;
					l_Expression.m_sExpression = l_sPredicate;
					l_Expression.m_RateFunctionVector = l_RateFunctionVector;
					
					l_Expression.m_sErrorPosition  = m_sColoredTransName;

					m_ExpressionVector.push_back(l_Expression);
				}
			}
		}

	return true;
}

bool SP_DS_ColPN_Unfolding::UnfoldThread::CollectScheduleRateFunction()
{
		if (m_sTransNodeClass == wxT("Scheduled Transition"))
		{
			SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(m_pcTransNode->GetAttribute(wxT("PeriodicList")) );
			m_nTransRateColumn = (l_pcColList->GetColCount()-1)/3;

			for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
			{
				SP_CPN_Expression l_Expression;
				wxString l_sPredicate = wxString(l_pcColList->GetCell(i, 0).c_str());
				if( l_sPredicate  == wxT("") )
					continue;

				vector<wxString> l_RateFunctionVector;

				for (unsigned int j = 1; j < l_pcColList->GetColCount(); j++)
				{
					wxString l_Begin= wxString(l_pcColList->GetCell(i, j).c_str());
					wxString l_Repetition=wxString(l_pcColList->GetCell(i, ++j).c_str());  
					wxString l_End= wxString(l_pcColList->GetCell(i, ++j).c_str());

					wxString l_sDelay = wxT("FixedTimedFiring_Periodic(")+ l_Begin +wxT(",")+ l_Repetition+wxT(",")+l_End+wxT(")");
					
					l_RateFunctionVector.push_back(l_sDelay);
				}
				if( l_sPredicate  != wxT("") )
				{
					l_Expression.m_eExprType = CPN_PREDICATE_EXPR;
					l_Expression.m_sExpression = l_sPredicate;
					l_Expression.m_RateFunctionVector = l_RateFunctionVector;

					
					l_Expression.m_sErrorPosition  = m_sColoredTransName;

					m_ExpressionVector.push_back(l_Expression);
				}
			}
		}
	
	return true;
}

bool SP_DS_ColPN_Unfolding::UnfoldThread::BuildParserTree()
{
	vector<SP_CPN_Expression>::iterator itVector;

	for(itVector = m_ExpressionVector.begin(); itVector != m_ExpressionVector.end(); itVector++)
	{		
		//Prepare parsing
		SP_CPN_Parse_Context_ptr l_pParseContext = make_CPN_Parse_Context();
		SP_CPN_Driver_ptr l_pDriver      = make_CPN_Driver((*l_pParseContext));

		////Set the ColorSetClass and colorset name for the parsing context		
		l_pParseContext->SetColorSetClass(&m_ColorSetClass);
		l_pParseContext->SetFunctionFlag(false); 
		//l_pParseContext->SetNetClassName(m_sNetClass); 
		l_pParseContext->SetPlaceType(itVector->m_sPlaceNodeClass);

		l_pParseContext->SetErrorPosition(itVector->m_sExpression +wxT("|")+ itVector->m_sErrorPosition);

		if(itVector->m_eExprType == CPN_INPUT_EXPR || itVector->m_eExprType ==	CPN_OUTPUT_EXPR)	
		{
			l_pParseContext->SetColorSetName(itVector->m_sColorSetName);
			l_pParseContext->SetColored2UnColoredPlaceNames(m_pcColPNUnfolding->GetsvColored2UnColoredPlaceNames());
		}
		else if( itVector->m_eExprType == CPN_RATEFUNCTION_EXPR  )
		{
			l_pParseContext->SetColorSetName(itVector->m_sColorSetName);
			l_pParseContext->SetPlaceType(wxT("Place"));
		}
		else
		{
			l_pParseContext->SetColorSetName(wxT(""));		
			l_pParseContext->SetPlaceType(wxT("Place"));
		}
	
		
		std::string l_ParseString;
		l_ParseString = string( itVector->m_sExpression.mb_str());
		bool l_bEval = l_pDriver->parse_string(l_ParseString, "input");	

		if(!l_bEval)
		{
			wxString l_sParseError  = m_pDriver->GetError();	
			wxString l_sError = wxT(" in the expression: ") + itVector->m_sExpression +wxT("|")+ itVector->m_sErrorPosition;
			l_sError = l_sParseError + l_sError;			
			//SP_LOGERROR(l_sError);			
			return false;
		}

		// call check function		
		if( !l_pParseContext->CheckExpression())
			return false;
		
		vector<SP_CPN_Var_Color>  l_vVariable2Color;
		l_vVariable2Color = l_pParseContext->GetVariables();
		
		itVector->m_pParseContext = l_pParseContext;
		itVector->m_pDriver = l_pDriver;
		itVector->m_vVariable2Color = l_vVariable2Color;		
	}

	return true;
}


//better to move it to check function.
bool SP_DS_ColPN_Unfolding::UnfoldThread::CheckVariableIsLegal()
{
	map<wxString, bool> l_mLegalVariables;
	map<wxString, bool> l_mVariablesInRates;
	for(unsigned i = 0; i < m_ExpressionVector.size(); i++)
	{
		for(unsigned k = 0; k < m_ExpressionVector[i].m_vVariable2Color.size(); k++)
		{
			if(m_ExpressionVector[i].m_eExprType == CPN_INPUT_EXPR || 
			   m_ExpressionVector[i].m_eExprType ==	CPN_OUTPUT_EXPR ||
			   m_ExpressionVector[i].m_eExprType ==	CPN_GUARD_EXPR )
			{
				l_mLegalVariables[m_ExpressionVector[i].m_vVariable2Color[k].m_sVariable] = true;
			}
			else
			{
				l_mVariablesInRates[m_ExpressionVector[i].m_vVariable2Color[k].m_sVariable] = true;
			}
		}
	}

	map<wxString, bool>::iterator itMap;
	for(itMap = l_mVariablesInRates.begin(); itMap != l_mVariablesInRates.end(); itMap++)
	{
		if(l_mLegalVariables.find(itMap->first) == l_mLegalVariables.end())
		{
			//wxMutexGuiEnter();
			//SP_LOGERROR(wxT("The variables used in rate functions are not legal."));			
			//wxMutexGuiLeave();			
			return false;
		}
	}

	return true;
}


void SP_DS_ColPN_Unfolding::UnfoldThread::CollectVariables()
{
	//search all variables and put them in a vector
	//order variables according to the expression with the max number variables
	m_mBindVar2Val.clear();
	m_vVar2ColorSet.clear();

	m_vGuardVar2ColorSet.clear();
	m_vGuardVar2ColorSet = m_ExpressionVector[0].m_vVariable2Color;

	unsigned int l_nMaxNumber = 0;
	unsigned int l_nMaxIndex  = 0;
    if(m_ExpressionVector.size() > 0)
	{
		l_nMaxNumber = m_ExpressionVector[0].m_vVariable2Color.size();
	}

	for(unsigned int i= 0; i < m_ExpressionVector.size(); i++ )
	{
		if(m_ExpressionVector[i].m_eExprType == CPN_INPUT_EXPR || 
			   m_ExpressionVector[i].m_eExprType ==	CPN_OUTPUT_EXPR ||
			   m_ExpressionVector[i].m_eExprType ==	CPN_GUARD_EXPR )
		{
				if(m_ExpressionVector[i].m_vVariable2Color.size() >= l_nMaxNumber)
				{
					l_nMaxNumber = m_ExpressionVector[i].m_vVariable2Color.size();
					l_nMaxIndex = i;
				}
		}
	}

	if(m_ExpressionVector.size() > 0)
	{
		for(unsigned j =0; j < m_ExpressionVector[l_nMaxIndex].m_vVariable2Color.size(); j++)
		{
			SP_CPN_Var_Color l_str_Var_Col = m_ExpressionVector[l_nMaxIndex].m_vVariable2Color[j];
			if(m_mBindVar2Val.find(l_str_Var_Col.m_sVariable) == m_mBindVar2Val.end() )
			{				
				m_mBindVar2Val[l_str_Var_Col.m_sVariable] = wxT("");				
				SP_CPN_Var_Color l_stVarCol;
				l_stVarCol.m_sVariable = l_str_Var_Col.m_sVariable;
				l_stVarCol.m_sColor = l_str_Var_Col.m_sColor;
				m_vVar2ColorSet.push_back(l_stVarCol);
			}
		}
	}

	for(unsigned int i= 0; i < m_ExpressionVector.size(); i++ )
	{
		if(l_nMaxIndex == i )
			continue;

		for(unsigned j =0; j < m_ExpressionVector[i].m_vVariable2Color.size(); j++)
		{
			SP_CPN_Var_Color l_str_Var_Col = m_ExpressionVector[i].m_vVariable2Color[j];
			if(m_mBindVar2Val.find(l_str_Var_Col.m_sVariable) == m_mBindVar2Val.end() )
			{				
				m_mBindVar2Val[l_str_Var_Col.m_sVariable] = wxT("");				
				SP_CPN_Var_Color l_stVarCol;
				l_stVarCol.m_sVariable = l_str_Var_Col.m_sVariable;
				l_stVarCol.m_sColor = l_str_Var_Col.m_sColor;
				m_vVar2ColorSet.push_back(l_stVarCol);					
/*
				wxString l_sMsg = wxT("CollectVariables:\n");
				l_sMsg << wxT("l_sVariable: ") << l_str_Var_Col.m_sVariable << wxT("\n");
				l_sMsg << wxT("l_sColorset: ") << l_str_Var_Col.m_sColor << wxT("\n");
				SP_LOGMESSAGE( l_sMsg);
*/
			}
		}
	}
}


bool SP_DS_ColPN_Unfolding::UnfoldThread::IntCSSuccessiveTest()
{	
	//test integer cs and successive
	for(unsigned i = 0; i < m_vGuardVar2ColorSet.size(); i++)
	{
		wxString l_sVarName = m_vGuardVar2ColorSet[i].m_sVariable;
		wxString l_sVarCS   = m_vGuardVar2ColorSet[i].m_sColor;

		SP_CPN_ColorSet* l_pcColorSet = m_ColorSetClass.LookupColorSet(l_sVarCS);

		if(l_pcColorSet->GetDataType() != CPN_INTEGER)
			return false;

		if( ! l_pcColorSet->GetIntSuccessiveFlag() )
			return false;
	}

	return true;
}



//to deal with the # function in rate function definition
bool SP_DS_ColPN_Unfolding::UnfoldThread::SumPlaceInstances(map<wxString, wxString> *p_pmPlaceToColorSet, wxString &p_sRateFunExpr, SP_CPN_ColorSetClass* p_pcColorSetClass, bool p_bCheck)
{
	//handle the function with the syntax: #(preplace)
	wxString l_sNewRatF = p_sRateFunExpr;

	if( l_sNewRatF.Find( wxT("#"))!= wxNOT_FOUND )
	{
		int l_nEndIndex = 1 + l_sNewRatF.Find( wxT("#"));
		int l_nLength = l_sNewRatF.Len();
		if(l_nLength == l_nEndIndex +1 )
		{
			return false;
		}
		wxString l_sSubExpr = l_sNewRatF.Mid(l_nEndIndex);
		wxString l_sColorPlaceName = l_sSubExpr.AfterFirst(wxT('('));
		l_sColorPlaceName = l_sColorPlaceName.BeforeFirst(wxT(')'));

		l_sColorPlaceName.Replace(wxT(" "),wxT(""));
		l_sColorPlaceName.Replace(wxT("\t"),wxT(""));
		l_sColorPlaceName.Replace(wxT("\n"),wxT(""));

		if( l_sColorPlaceName == wxT(""))
		{
			wxString l_sError = wxT("Rate function error: the parameter of the # function.");			
			//SP_LOGERROR(l_sError);
			return false;
		}
		wxString l_sBeforeFunction = l_sNewRatF.Mid(0,l_sNewRatF.Find( wxT("#")));
		wxString l_sAfterFunction = l_sSubExpr.AfterFirst(wxT(')'));
		if( p_pmPlaceToColorSet->find( l_sColorPlaceName ) == p_pmPlaceToColorSet->end())
		{
			wxString l_sError = wxT("Rate function error: the parameter of the # function.");				
			//SP_LOGERROR(l_sError);
			return false;
		}	

		if(p_bCheck)
		{
			p_sRateFunExpr = l_sBeforeFunction + l_sColorPlaceName + l_sAfterFunction;
			return true;
		}

		wxString l_sColorSet = p_pmPlaceToColorSet->find( l_sColorPlaceName )->second;

		if( ! p_pcColorSetClass->LookupColorSet(l_sColorSet) )
			return false;

		wxString l_sRep=wxT("");
		vector<wxString> l_vColors =  p_pcColorSetClass->LookupColorSet(l_sColorSet)->GetStringValue();
		for( unsigned i = 0; i < l_vColors.size(); i++ )
		{
			if(i==0)
				l_sRep = l_sColorPlaceName + wxT("_")+ l_vColors[i];
			else
				l_sRep = l_sRep + wxT("+") + l_sColorPlaceName + wxT("_")+ l_vColors[i];;
		}

		l_sRep = l_sBeforeFunction + wxT("(") + l_sRep+ wxT(")") + l_sAfterFunction;

		p_sRateFunExpr = l_sRep;

	}
	return true;

}




bool SP_DS_ColPN_Unfolding::UnfoldThread::ProcessConstantTransition()
{
	m_sOneBinding = wxT("constant");						//set it as constant
	m_sAnimationTransitionInstanceName = wxT("constant");	//set it as constant

	if(! Parsing() )
		return false;

	if( EvaluateGuard() )
	{	
		if( ! ProcessTransition() ) 
			return false;	
	}

	return true;
}


bool SP_DS_ColPN_Unfolding::UnfoldThread::Parsing()
{
	vector<SP_CPN_Expression>::iterator itVector;
	for(itVector = m_ExpressionVector.begin(); itVector != m_ExpressionVector.end(); itVector++)
	{
		if(m_bCSPFlag && itVector->m_eExprType == CPN_GUARD_EXPR)
		{
			continue;
		}

		itVector->m_ParsedInfo.clear();

		vector<SP_CPN_EvaluatedSingleValue> l_EvaluateResultArray;

		if( ! itVector->m_pParseContext->EvaluateExpression( l_EvaluateResultArray) )
			return false;
		
		if( ! (itVector->m_eExprType == CPN_GUARD_EXPR || itVector->m_eExprType == CPN_PREDICATE_EXPR) )
		{
			SP_CPN_ColorSet* l_pcColorSet;
			if(itVector->m_sExpression.Find(wxT("val(")) != wxNOT_FOUND)
			{
				wxString l_sValfunccolorset = itVector->m_pParseContext->GetHighLevelNodeColorSet();
				l_pcColorSet = m_ColorSetClass.LookupColorSet(l_sValfunccolorset);
			}
			else
			{
				l_pcColorSet = m_ColorSetClass.LookupColorSet(itVector->m_sColorSetName);
			}
			if(!l_pcColorSet)
				return false;

			vector<SP_CPN_EvaluatedSingleValue> l_NewEvaluateResultArray;			
			for(unsigned i = 0; i < l_EvaluateResultArray.size(); i++)
			{				
				if( l_EvaluateResultArray[i].m_Predicate == true && l_pcColorSet->IsColor(l_EvaluateResultArray[i].m_ColorValue)	)
				{
					l_NewEvaluateResultArray.push_back( l_EvaluateResultArray[i] );  //ignore those bindings that are beyond the range
				}
			}
			itVector->m_ParsedInfo = l_NewEvaluateResultArray;			
		}
		else
		{
			itVector->m_ParsedInfo = l_EvaluateResultArray; 
		}	
	}

	return true;
}

bool SP_DS_ColPN_Unfolding::UnfoldThread::EvaluateGuard()
{
	vector<SP_CPN_Expression>::iterator itVector;
	SP_CPN_Expression l_pExpression;

	if(m_bCSPFlag)
		return true;

	for(itVector = m_ExpressionVector.begin(); itVector != m_ExpressionVector.end(); itVector++)
	{		
		if ( itVector->m_eExprType == CPN_GUARD_EXPR ) 
		{
			if( itVector->m_ParsedInfo.begin()->m_ColorValue == wxT("true") )	
				return true;	
			else
				return false;
		}
	}
	
	return true;
}

bool SP_DS_ColPN_Unfolding::UnfoldThread::ProcessVariableTransition()
{
	if( ! DynamicBinding( m_vVar2ColorSet.size()-1 ) )
			return false;

	return true;
}



bool SP_DS_ColPN_Unfolding::UnfoldThread::DynamicBinding(int p_nVarPos)
{
	if(p_nVarPos < 0)
	{
		//process
		if( ! DynamicParse() )
			return false;

		return true;
	}

	//find the colors of a variable
	wxString l_sColorSet = m_vVar2ColorSet[p_nVarPos].m_sColor;
	wxString l_sVariable = m_vVar2ColorSet[p_nVarPos].m_sVariable;
/*
//	if(l_sColorSet.IsEmpty())
	{
		wxString l_sMsg = m_sColoredTransName + wxT("\n");
		l_sMsg << wxT("p_nVarPos: ") << p_nVarPos << wxT("(") << m_vVar2ColorSet.size() << wxT(")") << wxT("\n");
		l_sMsg << wxT("l_sVariable: ") << l_sVariable << wxT("\n");
		l_sMsg << wxT("l_sColorset: ") << l_sColorSet << wxT("\n");
		SP_LOGMESSAGE(l_sMsg);
	}
*/
	SP_CPN_ColorSet* l_pcColorSet = m_ColorSetClass.LookupColorSet(l_sColorSet);		
	if( ! l_pcColorSet )
		return false;
	vector<wxString> l_ColorVector = l_pcColorSet->GetStringValue();

	//do the loop
	for (unsigned int i=0;i<l_ColorVector.size();i++)
	{
		m_mBindVar2Val[l_sVariable] = l_ColorVector[i] ;

		if( ! DynamicBinding(p_nVarPos-1) )
			return false;
	}	

	return true;
}


bool SP_DS_ColPN_Unfolding::UnfoldThread::DynamicParse()
{		
	//If there is a variable in an expression	
	map<wxString,wxString>::iterator itMap;		

	//Assign values for variables
	m_sAnimationTransitionInstanceName = wxT("");		

	for(itMap = m_mBindVar2Val.begin(); itMap != m_mBindVar2Val.end(); itMap++)
	{
		if( ! m_ColorSetClass.SetVariableValue(itMap->first, itMap->second) )	
			return false;

		if( itMap == m_mBindVar2Val.begin() )
		{
			m_sAnimationTransitionInstanceName = itMap->second;
		}
		else		
		{
			m_sAnimationTransitionInstanceName = m_sAnimationTransitionInstanceName + wxT("_") + itMap->second ;		
		}
	}

	//Here order the variables for display
	m_sOneBinding = wxT("");		
	for(unsigned i = 0; i < m_vVar2ColorSet.size(); i++)
	{
		wxString l_sValue = m_mBindVar2Val[m_vVar2ColorSet[i].m_sVariable];
		m_sOneBinding = m_sOneBinding + wxT("_") + l_sValue;
	}
	m_sOneBinding = m_sOneBinding.AfterFirst(wxT('_'));
	

	//parsing the expressions
	if( !Parsing() ) 
		return false;								
	
	//begin to write unfolding information
	if( EvaluateGuard() )
	{		
		if( !ProcessTransition() ) 
			return false;
	}
	
	return true;
}


bool SP_DS_ColPN_Unfolding::UnfoldThread::ProcessTransition()
{
	m_scOneTransInfo.m_anNetFunctions.clear();
	m_scOneTransInfo.m_vInputArcs.clear();
	m_scOneTransInfo.m_vOutputArcs.clear();	

	//Assign rate function for unfolded transition	
	bool l_bFoundRateFunc = false;

	for(unsigned k = 0; k < m_ExpressionVector.size(); k++)	
	{	
		if ( m_ExpressionVector[k].m_eExprType == CPN_PREDICATE_EXPR && m_ExpressionVector[k].m_ParsedInfo.begin()->m_ColorValue == wxT("true")) 
		{
			if( m_sTransNodeClass == SP_DS_STOCHASTIC_TRANS || m_sTransNodeClass == SP_DS_CONTINUOUS_TRANS)
			{
				ProcessRateFunction(&(m_ExpressionVector[k]), m_scOneTransInfo.m_anNetFunctions);
			}
			else
			{
				m_scOneTransInfo.m_anNetFunctions = m_ExpressionVector[k].m_RateFunctionVector;
			}
			//Get the colored place name and its current color
			map<wxString, wxString> l_mPlaceToColor;
			CollectPlace2ColorMapforATransition( l_mPlaceToColor);

			//Get an unfolded name for a colored place in a rate function
			vector<wxString> l_sUnfoldedRateFunctionVector;
			GenerateUnfoledRateFunction(l_mPlaceToColor, m_scOneTransInfo.m_anNetFunctions, l_sUnfoldedRateFunctionVector);
			for(unsigned i = 0; i < l_sUnfoldedRateFunctionVector.size(); i++)
			{
				m_scOneTransInfo.m_anNetFunctions[i] = l_sUnfoldedRateFunctionVector[i];				
			}

			l_bFoundRateFunc = true;
			break;
		}
	}	
	//If there is no predicate that is true, then we assign 0 to it.
	if( m_sNetClass == SP_DS_COLSPN_CLASS||
		m_sNetClass == SP_DS_COLCPN_CLASS||
		m_sNetClass == SP_DS_COLHPN_CLASS)
	{
		if( !l_bFoundRateFunc )
		{
			m_scOneTransInfo.m_anNetFunctions.resize(m_nTransRateColumn,wxT("0"));		
		}
	}

	//DEAL WITH ARCS FOR A TRANSITION
	ProcessArcs();

	m_scOneTransInfo.m_sType = m_sTransNodeClass;	
	m_scOneTransInfo.m_sAnimTransInstName = m_sAnimationTransitionInstanceName;

	m_mTransInstances[m_sOneBinding] = m_scOneTransInfo;

	return true;
}

bool SP_DS_ColPN_Unfolding::UnfoldThread::ProcessRateFunction(SP_CPN_Expression* p_eExpression, SP_VectorString& p_sNetFunctions, bool p_bExport)
{
	vector<SP_CPN_Expression>::iterator itVector;		
	for(itVector = m_ExpressionVector.begin(); itVector != m_ExpressionVector.end(); itVector++)
	{	
		SP_CPN_Expression l_eExpression;
		l_eExpression = *itVector;
		if( l_eExpression.m_sPredicate == p_eExpression->m_sExpression)
		{
			unsigned int l_nColumn = l_eExpression.m_nColumn;
			vector<SP_CPN_EvaluatedSingleValue>::iterator itEvalVector;
			itEvalVector = l_eExpression.m_ParsedInfo.begin();
			wxString l_sColorValue = itEvalVector->m_ColorValue;

			map<wxString, SP_ColPN_RateFunction>::iterator itMap;
			for(itMap = p_eExpression->m_RateFunctionTransformation[l_nColumn].begin(); itMap != p_eExpression->m_RateFunctionTransformation[l_nColumn].end(); itMap++)
			{
				if( itMap->second.m_sFormula == l_eExpression.m_sExpression)
				{
					itMap->second.m_sValue = l_sColorValue;
				}
			}
		}
	}

	p_sNetFunctions.resize(p_eExpression->m_RateFunctionVector.size());
	for(unsigned i = 0; i < p_eExpression->m_RateFunctionVector.size(); i++)
	{
		wxString l_sRateFunction = p_eExpression->m_RateFunctionVector[i];

		SP_ColPN_ProcessRateFunction l_cProcessRateFunction;
		l_cProcessRateFunction.ReplaceRateFunction(l_sRateFunction,&(p_eExpression->m_RateFunctionTransformation[i]),p_bExport);
		p_sNetFunctions[i] = l_sRateFunction;
	}

	return true;
}


void SP_DS_ColPN_Unfolding::UnfoldThread::CollectPlace2ColorMapforATransition( map<wxString, wxString> &p_mPlace2ColorMap )
{
		//map<wxString, wxString> l_mPlaceToColor;
	    for(unsigned i =0; i < m_ExpressionVector.size(); i++)
		{		
			if ( m_ExpressionVector[i].m_eExprType == CPN_INPUT_EXPR) 
			{			
				//SP_DS_Node* l_pcPlaceNode = m_ExpressionVector[i].m_pcPlaceNode;

				wxString l_sPlaceName = m_ExpressionVector[i].m_sColPlaceName;

				wxString l_sColor = wxT("");

				vector<SP_CPN_EvaluatedSingleValue>	l_ParsedInfo = m_ExpressionVector[i].m_ParsedInfo;

				for(unsigned j = 0; j < l_ParsedInfo.size(); j++)
				{
					SP_CPN_EvaluatedSingleValue l_EvalInfo = l_ParsedInfo[j];
					if( m_ExpressionVector[i].m_sPlaceNodeClass == wxT("Place") && l_EvalInfo.m_Multiplicity > 0 )
					{
						l_sColor = l_EvalInfo.m_ColorValue;
						break;
					}

					if( m_ExpressionVector[i].m_sPlaceNodeClass == SP_DS_CONTINUOUS_PLACE && l_EvalInfo.m_DoubleMultiplicity > 0 )
					{
						l_sColor = l_EvalInfo.m_ColorValue;
						break;
					}
				}		

				p_mPlace2ColorMap[l_sPlaceName] = l_sColor;
			}
		}
}

void SP_DS_ColPN_Unfolding::UnfoldThread::GenerateUnfoledRateFunction(map<wxString, wxString> p_mPlaceToColor, vector<wxString> p_sColoredRateFunctionVector, vector<wxString> &p_sUnfoldedRateFunctionVector)
{
	map<wxString, wxString> l_mReplaceString;
	int l_nCount = 0;		

	for( unsigned i = 0; i < p_sColoredRateFunctionVector.size(); i++ )
	{
		wxString l_sColoredRateFunction = p_sColoredRateFunctionVector[i];			

		l_sColoredRateFunction.Replace(wxT(" "),wxT(""));
		l_sColoredRateFunction.Replace(wxT("\t"),wxT(""));
		l_sColoredRateFunction.Replace(wxT("\n"),wxT(""));				
		
		wxRegEx l_cUnfoldRateFunction;
		map<wxString, wxString>::iterator itMap;
		for(itMap = p_mPlaceToColor.begin(); itMap != p_mPlaceToColor.end(); itMap++ )
		{
			wxString l_sColoredPlace = itMap->first;
			wxString l_sSeparator = wxT("[\\(\\)\\+\\*\\/\\^\\=\\<\\>\\!\\%\\&\\|\\,\\-]");
			if(l_cUnfoldRateFunction.Compile(wxT("(^|") + l_sSeparator+ wxT(")(")
												+l_sColoredPlace+
												wxT(")($|") + l_sSeparator+ wxT(")")
											)
				)
			{
				while(l_cUnfoldRateFunction.Matches(l_sColoredRateFunction))
				{
					wxString l_sReplaceSymbol = wxT("##")+ wxString::Format(wxT("%d"),l_nCount++)+ wxT("##");
					l_cUnfoldRateFunction.Replace(&l_sColoredRateFunction,
												wxT("\\1")+l_sReplaceSymbol+wxT("\\3")
												,1);
					wxString l_sUnfoldedPlace = l_sColoredPlace + wxT("_") + itMap->second ;
					ModifyPlaceName(l_sUnfoldedPlace);
					l_mReplaceString[l_sReplaceSymbol] = l_sUnfoldedPlace;
				}
			}
		}

		map<wxString, wxString>::iterator itRepMap;
		for(itRepMap = l_mReplaceString.begin(); itRepMap != l_mReplaceString.end(); itRepMap++)
		{
			if( l_sColoredRateFunction.Find(itRepMap->first) != wxNOT_FOUND )
			{
				l_sColoredRateFunction.Replace(itRepMap->first,itRepMap->second);
			}
		}

		wxString l_sSepString ;
		while ( l_sColoredRateFunction.length() > 300 ) 
		{
			wxString l_sBeforeFirst = l_sColoredRateFunction.BeforeFirst(wxT(')'));

			if(l_sBeforeFirst == l_sColoredRateFunction)
			{
				l_sSepString = l_sSepString + l_sBeforeFirst;
			}
			else
			{
				l_sSepString = l_sSepString + l_sBeforeFirst + wxT(")\n");
			}
			
			l_sColoredRateFunction = l_sColoredRateFunction.AfterFirst(wxT(')'));

		}
		l_sSepString = l_sSepString+l_sColoredRateFunction;

		l_sColoredRateFunction = l_sSepString;
		
		p_sUnfoldedRateFunctionVector.push_back(l_sColoredRateFunction);		
	}
}

void SP_DS_ColPN_Unfolding::UnfoldThread::ProcessArcs()
{
	vector<SP_CPN_Expression>::iterator itVector;		
	for(itVector = m_ExpressionVector.begin(); itVector != m_ExpressionVector.end(); itVector++)
	{
		if( itVector->m_eExprType == CPN_INPUT_EXPR || itVector->m_eExprType == CPN_OUTPUT_EXPR)
		{			
			wxString l_sColPlaceName = itVector->m_sColPlaceName;		
			wxString l_sPlaceNodeClass = itVector->m_sPlaceNodeClass;			
		
			vector<SP_CPN_EvaluatedSingleValue>::iterator itEvalVector;
			for(itEvalVector = itVector->m_ParsedInfo.begin(); itEvalVector != itVector->m_ParsedInfo.end(); itEvalVector++ )
			{
				if(itEvalVector->m_Predicate == true)
				{
					wxString	l_sColor		=	itEvalVector->m_ColorValue;					
					wxString    l_sArcType      =   itVector->m_sEdgeClass;

					SP_CPN_UnfoldedArcInfo l_scOneArcInfo;
					l_scOneArcInfo.m_sColPlaceName = l_sColPlaceName;
					l_scOneArcInfo.m_sColor        = l_sColor;
					l_scOneArcInfo.m_sArcType      = l_sArcType;
					l_scOneArcInfo.m_sDiscContType = l_sPlaceNodeClass;
				
					if(l_sPlaceNodeClass == SP_DS_DISCRETE_PLACE)
					{
						if (!itEvalVector->m_bPlaceFlag && itEvalVector->m_Multiplicity == 0)
						{
							// multiplicity == 0 -> no arc
							continue;
						}
						else
						{
							l_scOneArcInfo.m_nMultiplicity = itEvalVector->m_Multiplicity;
							if (itEvalVector->m_bPlaceFlag)
								l_scOneArcInfo.m_sMultiplicity = itEvalVector->m_stringMultiplicity;
							else
								l_scOneArcInfo.m_sMultiplicity = wxString::Format(wxT("%d"), itEvalVector->m_Multiplicity);
						}
					}
					else
					{
						if (!itEvalVector->m_bPlaceFlag && itEvalVector->m_DoubleMultiplicity == 0.0)
						{
							// multiplicity == 0 -> no arc
							continue;
						}
						else
						{
							l_scOneArcInfo.m_dMultiplicity = itEvalVector->m_DoubleMultiplicity;
							if (itEvalVector->m_bPlaceFlag)
								l_scOneArcInfo.m_sMultiplicity = itEvalVector->m_stringMultiplicity;
							else
								l_scOneArcInfo.m_sMultiplicity = wxString::Format(wxT("%f"), itEvalVector->m_DoubleMultiplicity);
						}
					}

					if( itVector->m_eExprType == CPN_INPUT_EXPR )
					{
						m_scOneTransInfo.m_vInputArcs.push_back(l_scOneArcInfo);
					}
					else
					{
						m_scOneTransInfo.m_vOutputArcs.push_back(l_scOneArcInfo);
					}
				}				
			}		
		}
	}
	
}

void SP_DS_ColPN_Unfolding::UnfoldThread::ModifyPlaceName(wxString &p_sPlaceName)
{
	p_sPlaceName.Replace(wxT("("),wxT("_"));
	p_sPlaceName.Replace(wxT(")"),wxT("_"));
	p_sPlaceName.Replace(wxT(","),wxT("_"));
}


bool SP_DS_ColPN_Unfolding::UnfoldThread::CSPSolve()
{
	SP_CPN_Parse_Context_ptr l_pcGDParsContext = m_ExpressionVector[0].m_pParseContext;

	SP_DS_ColCSPSovler *l_pcCSPSolver = new SP_DS_ColCSPSovler();

	//initialize variables
	l_pcCSPSolver->InitializeVarArrary( m_vGuardVar2ColorSet.size() );

	for(unsigned i = 0; i < m_vGuardVar2ColorSet.size(); i++)
	{
		wxString l_sVarName = m_vGuardVar2ColorSet[i].m_sVariable;
		wxString l_sVarCS = m_vGuardVar2ColorSet[i].m_sColor;
		SP_CPN_ColorSet* l_pcColorSet = m_ColorSetClass.LookupColorSet( l_sVarCS );
		wxString l_sLow = l_pcColorSet->GetStringValue()[0];
		wxString l_sHigh = l_pcColorSet->GetStringValue()[l_pcColorSet->GetStringValue().size()-1];
		long l_nLow, l_nHigh;
		l_sLow.ToLong(&l_nLow);
		l_sHigh.ToLong(&l_nHigh);

		l_pcCSPSolver->InitializeVariable(l_sVarName, i, l_nLow, l_nHigh);
	}

	//get constraints
	vector< vector<wxString> > l_vvsConstraints;
	if( ! l_pcGDParsContext->GetConstraints(*l_pcCSPSolver ) )
		return false;

	l_pcCSPSolver->SetBranch();

	//solve it	
	Gecode::DFS<SP_DS_ColCSPSovler> xy(l_pcCSPSolver);
	wxDELETE( l_pcCSPSolver );
	
	m_vvCSPAssigns.clear();
	while (SP_DS_ColCSPSovler *ss = xy.next())
	{
		//ss->print();
	  vector<unsigned int> l_vOneRowVals;
	  IntVarArray* l_ArrayVariables = ss->GetArrayVariables();
	  for(int k = 0; k < l_ArrayVariables->size(); k++ )
	  {
		  for(Gecode::IntVarValues i((*l_ArrayVariables)[k]); i(); ++i)
		  {
			  l_vOneRowVals.push_back( i.val() );
			  break;
		  }
	  }
	  m_vvCSPAssigns.push_back(l_vOneRowVals);

		delete ss;
	}



	return true;
}

bool SP_DS_ColPN_Unfolding::UnfoldThread::Binding(vector<SP_CPN_Var_Color>& p_vNonCSPVars, vector<vector<wxString> > & p_vvNonCSPBindingRes)
{
	//find non-binding variables
	for(unsigned i = 0; i < m_vVar2ColorSet.size(); i++)
	{
		wxString l_sVarName = m_vVar2ColorSet[i].m_sVariable;
		bool l_bGFound = false;

		for(unsigned j = 0; j < m_vGuardVar2ColorSet.size(); j++)
		{
			wxString l_sGuardVarName = m_vGuardVar2ColorSet[j].m_sVariable;
			if(l_sVarName == l_sGuardVarName)
			{
				l_bGFound = true;
				break;
			}
		}
		if(!l_bGFound)
		{
			p_vNonCSPVars.push_back(m_vVar2ColorSet[i]);
		}
	}

	if(p_vNonCSPVars.size() == 0)
		return true;


	//initialize val vector
	unsigned int l_nNumColumns = 1;
	for(unsigned i = 0; i < p_vNonCSPVars.size(); i++)
	{
		wxString l_sColorSet = p_vNonCSPVars[i].m_sColor;
		SP_CPN_ColorSet* l_pcColorSet = m_ColorSetClass.LookupColorSet(l_sColorSet);		
		if( ! l_pcColorSet )
			return false;
		l_nNumColumns = l_nNumColumns * l_pcColorSet->GetStringValue().size();
	}

	p_vvNonCSPBindingRes.resize(l_nNumColumns);
	for(unsigned i=0; i< l_nNumColumns; i++)
	{
		p_vvNonCSPBindingRes[i].resize(p_vNonCSPVars.size());
	}

	//write values
	for(unsigned i = 0; i < p_vNonCSPVars.size(); i++)
	{
		wxString l_sColorSet = p_vNonCSPVars[i].m_sColor;
		SP_CPN_ColorSet* l_pcColorSet = m_ColorSetClass.LookupColorSet(l_sColorSet);		
		if( ! l_pcColorSet )
			return false;

		unsigned int l_nAssingNumCols = 1;
		for(unsigned j = i+1; j < p_vNonCSPVars.size(); j++)
		{
			wxString l_sColorSet2 = p_vNonCSPVars[j].m_sColor;
			SP_CPN_ColorSet* l_pcColorSet2 = m_ColorSetClass.LookupColorSet(l_sColorSet2);		
			if( ! l_pcColorSet2 )
				return false;
			l_nAssingNumCols = l_nAssingNumCols * l_pcColorSet2->GetStringValue().size();
		}

		vector<wxString> l_vColors = l_pcColorSet->GetStringValue();
		unsigned int l_nCurPos = 0;
		for(unsigned int n = 0; n < p_vvNonCSPBindingRes.size()/(l_vColors.size()*l_nAssingNumCols); n++ )
		{
			for(unsigned int k = 0; k < l_vColors.size(); k++)
			{
				for(unsigned m = 0; m < l_nAssingNumCols; m++)
				{
					p_vvNonCSPBindingRes[l_nCurPos][i] = l_vColors[k];
					l_nCurPos++;
				}
			}	
		}
	}	

	return true;
}


bool SP_DS_ColPN_Unfolding::UnfoldThread::ProcessTransitionByCSP()
{
	vector<SP_CPN_Var_Color> l_vNonCSPVars;
	vector<vector<wxString> > l_vvNonCSPBindingRes;
	if( ! Binding(l_vNonCSPVars, l_vvNonCSPBindingRes) )
		return false;

	//assign
	map<wxString, wxString> l_mBindVar2Val;
	for(unsigned i1 = 0; i1< m_vvCSPAssigns.size(); i1++)
	{		
		for(unsigned k1 = 0; k1 < m_vGuardVar2ColorSet.size(); k1++)
		{
			if( ! m_ColorSetClass.SetVariableValue( m_vGuardVar2ColorSet[k1].m_sVariable, m_vvCSPAssigns[i1][k1]) )	
				return false;
			l_mBindVar2Val[m_vGuardVar2ColorSet[k1].m_sVariable] = wxString::Format(wxT("%d"),m_vvCSPAssigns[i1][k1]);
		}

		if(l_vvNonCSPBindingRes.size() == 0)
		{
			if( ! ProcessOneBinding( l_mBindVar2Val) )
				return false;	
		}

		for(unsigned i2 = 0; i2< l_vvNonCSPBindingRes.size(); i2++)
		{
			for(unsigned k2 = 0; k2 < l_vNonCSPVars.size(); k2++)
			{
				if( ! m_ColorSetClass.SetVariableValue( l_vNonCSPVars[k2].m_sVariable, l_vvNonCSPBindingRes[i2][k2]) )	
					return false;
				l_mBindVar2Val[l_vNonCSPVars[k2].m_sVariable] = l_vvNonCSPBindingRes[i2][k2];
			}

			//
			if( ! ProcessOneBinding(l_mBindVar2Val) )
				return false;			
		}
	}

	return true;
}


bool SP_DS_ColPN_Unfolding::UnfoldThread::ProcessOneBinding(map<wxString,wxString>& p_mBindVar2Val)
{

			//naming
			map<wxString,wxString>::iterator itMap;

			m_sAnimationTransitionInstanceName = wxT("");
			for(itMap = p_mBindVar2Val.begin(); itMap != p_mBindVar2Val.end(); itMap++)
			{
				if( itMap == p_mBindVar2Val.begin() )
					m_sAnimationTransitionInstanceName = itMap->second;
				else		
					m_sAnimationTransitionInstanceName = m_sAnimationTransitionInstanceName + wxT("_") + itMap->second ;		
			}

			//Here order the variables for display
			m_sOneBinding = wxT("");		
			for(unsigned i = 0; i < m_vVar2ColorSet.size(); i++)
			{
				wxString l_sValue = p_mBindVar2Val[m_vVar2ColorSet[i].m_sVariable];
				m_sOneBinding = m_sOneBinding + wxT("_") + l_sValue;
			}
			m_sOneBinding = m_sOneBinding.AfterFirst(wxT('_'));
			

			//parsing the expressions
			if( !Parsing() ) 
				return false;								
			
			//begin to write unfolding information
			if( EvaluateGuard() )
			{		
				if( !ProcessTransition() ) 
					return false;
			}

			return true;
}





SP_DS_ColPN_Unfolding::UnfoldThread::UnfoldThread(	unsigned long p_nThreadId, 
										SP_DS_ColPN_Unfolding* p_pcColPNUnfolding, 	
										vector<SP_DS_Node*>    p_vTransitions,											
										SP_DS_Graph* p_pcGraph,
										bool p_bCSPSolve):
wxThread(wxTHREAD_JOINABLE),
m_nThreadId(p_nThreadId),
m_pcColPNUnfolding(p_pcColPNUnfolding),
m_vTransitions(p_vTransitions),
m_pcGraph(p_pcGraph),
m_bCSPSolve(p_bCSPSolve)
{		
	m_nCurrentFinished = 0;
}

SP_DS_ColPN_Unfolding::UnfoldThread::~UnfoldThread()
{
}

void* SP_DS_ColPN_Unfolding::UnfoldThread::Entry()
{	
   if(!InitializeDeclaration())
   {
	   m_pcColPNUnfolding->SetThreadSuccess(false);
	   m_pcColPNUnfolding->ThreadFinished();
	   return NULL;
   }

	bool l_bSuccess = Unfold();

	if(!l_bSuccess)
	{
		m_pcColPNUnfolding->SetThreadSuccess(false);
	}
	else
	{
		m_pcColPNUnfolding->ThreadAddContResult(m_mmUnfoldedContTransions);	
		m_pcColPNUnfolding->ThreadAddStochResult(m_mmUnfoldedStochTransions);	
		m_pcColPNUnfolding->ThreadAddImmResult(m_mmUnfoldedImmTransions);	
		m_pcColPNUnfolding->ThreadAddDetResult(m_mmUnfoldedDetTransions);	
		m_pcColPNUnfolding->ThreadAddSchedResult(m_mmUnfoldedSchedTransions);
	}

	m_pcColPNUnfolding->ThreadFinished();

	return NULL;
}

bool SP_DS_ColPN_Unfolding::UnfoldThread::InitializeDeclaration()
{
	wxMutexLocker locker(m_InitGraphMutex);
	//m_ValueAssign.SetAllDeclarations(m_svDeclarations);
	if( !m_ValueAssign.InitializeColorset(m_ColorSetClass) )
	{
		return false;
	}
	return true;
}


bool SP_DS_ColPN_Unfolding::UnfoldThread::ComputeBindings(SP_DS_Node* p_pcTransNode, bool& p_bCSPSucceed, vector<wxString>& p_vsBindings)
{
	//Initialize it
	if( !m_ValueAssign.InitializeColorset(m_ColorSetClass) )
			return false;

	m_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	m_sNetClass = wxString(m_pcGraph->GetNetclass()->GetName().c_str());	
	m_pcTransNode = p_pcTransNode;		
	m_sTransNodeClass = wxString(m_pcTransNode->GetNodeclass()->GetName().c_str());
	m_sColoredTransName = wxString(dynamic_cast<SP_DS_NameAttribute*>(m_pcTransNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue().c_str());		

	if(!InitializeParsedExprs())
		return false;

	if( ! BuildParserTree() )
		return false;

	CollectVariables();

	p_bCSPSucceed = false;

	//constant
	if(m_vVar2ColorSet.size() == 0)    
	{
		return true;
	}
	else
	{
		//add constraint solover 
		//test if the color set of each variable is integer and successive, e.g. int from 1-100
		bool l_bIntSuccessive = IntCSSuccessiveTest();
		bool l_bCSPSucceed = false;
		if( m_vGuardVar2ColorSet.size() > 0 && l_bIntSuccessive)
		{
			l_bCSPSucceed = CSPSolve();
			p_bCSPSucceed = l_bCSPSucceed;
		}
		if(l_bCSPSucceed )
		{			
			vector<SP_CPN_Var_Color> l_vNonCSPVars;
			vector<vector<wxString> > l_vvNonCSPBindingRes;
			if( ! Binding(l_vNonCSPVars, l_vvNonCSPBindingRes) )
				return false;

			//assign
			map<wxString, wxString> l_mBindVar2Val;
			for(unsigned i1 = 0; i1< m_vvCSPAssigns.size(); i1++)
			{		
				wxString l_sBinding1;
				for(unsigned k1 = 0; k1 < m_vGuardVar2ColorSet.size(); k1++)
				{					
					l_sBinding1 << m_vGuardVar2ColorSet[k1].m_sVariable +wxT("=")+ wxString::Format(wxT("%d"),m_vvCSPAssigns[i1][k1]) + wxT(";");
				}

				if( l_vvNonCSPBindingRes.size() == 0 )
				{					
					p_vsBindings.push_back(l_sBinding1);
				}
				for(unsigned i2 = 0; i2< l_vvNonCSPBindingRes.size(); i2++)
				{
					wxString l_sBinding2;
					for(unsigned k2 = 0; k2 < l_vNonCSPVars.size(); k2++)
					{	
						l_sBinding2 << l_vNonCSPVars[k2].m_sVariable +wxT("=")+ l_vvNonCSPBindingRes[i2][k2] + wxT(";");
					}
					
					wxString l_sBinding = l_sBinding1 + l_sBinding2;					
					p_vsBindings.push_back(l_sBinding);
				}				
			}			
		}	
		else
		{
			vector<vector<wxString> > l_CompleteBinding;
			SP_MapString2UInt l_IndexMap;	
			if( ! Binding(l_IndexMap,l_CompleteBinding) )	
				return false;
			SP_MapString2UInt::iterator itSUMap;	
			for( unsigned i = 0; i < l_CompleteBinding.size(); i++)
			{		
				wxString l_sBinding;
				for(itSUMap = l_IndexMap.begin(); itSUMap != l_IndexMap.end(); itSUMap++)
				{
					if( ! m_ColorSetClass.SetVariableValue(itSUMap->first, l_CompleteBinding[i][itSUMap->second]) )	
						return false;
					l_sBinding << itSUMap->first +wxT("=")+ l_CompleteBinding[i][itSUMap->second] + wxT(";");					
				}		
				if( !Parsing() ) 
				{
					return false;	
				}
				if( EvaluateGuard() )
				{
					p_vsBindings.push_back(l_sBinding);
				}
			}	
		}		
	}

	return true;
}


bool SP_DS_ColPN_Unfolding::UnfoldThread::Binding(SP_MapString2UInt &p_IndexMap, vector<vector<wxString> > & p_AllBindingVector)
{
	SP_MapString2UInt* l_pIndexMap; //Store the index of ordered variables
	l_pIndexMap = &p_IndexMap;
	SP_MapString2UInt l_IndexMap;		//the variable name -> the index in the vector
	unsigned l_nVariableNum ;
	vector<wxString> l_OneBinding;
	vector<SP_CPN_Var_Color> l_vVarColor;
	vector<vector<wxString> > l_PartialBindingOld;  // that is already bound
	vector<vector<wxString> > l_PartialBindingNew;  // new binding
	vector<vector<wxString> >* l_pCompleteBinding;  // store the final binding results
	l_pCompleteBinding = &p_AllBindingVector;		// as the final output if the binding is successful
	unsigned l_nVarPos = 0;

	for(unsigned int i= 0; i < m_ExpressionVector.size(); i++ )
	{		
		for(unsigned j =0; j < m_ExpressionVector[i].m_vVariable2Color.size(); j++)
		{
			SP_CPN_Var_Color l_str_Var_Col = m_ExpressionVector[i].m_vVariable2Color[j];
			if(l_IndexMap.find(l_str_Var_Col.m_sVariable) == l_IndexMap.end() )
			{				
				l_IndexMap[l_str_Var_Col.m_sVariable] = l_nVarPos;			//Initialized as 0				
				SP_CPN_Var_Color l_stVarCol;
				l_stVarCol.m_sVariable = l_str_Var_Col.m_sVariable;
				l_stVarCol.m_sColor = l_str_Var_Col.m_sColor;
				l_vVarColor.push_back(l_stVarCol);				
				l_nVarPos++;
			}
		}
	}	

	//Get the number of variables
	l_nVariableNum = l_IndexMap.size();
	//Initialized as empty to store each binding
	l_OneBinding.assign(l_nVariableNum, wxT(""));  // Assign the vector with the number of variables
	*l_pIndexMap = l_IndexMap; //as the final output if the binding is successful
	map<wxString,unsigned> l_mBoundVariables;

	//In the followings, the variable will be bound to color in the colorset
	vector<vector<wxString> > l_InternalBindingNew;
	vector<vector<wxString> > l_InternalBindingOld;

	SP_MapString2String::iterator itSSMap;
	for(unsigned l_nVC = 0; l_nVC < l_vVarColor.size(); l_nVC++)	
	{		
		l_mBoundVariables[l_vVarColor[l_nVC].m_sVariable] = l_IndexMap.find(l_vVarColor[l_nVC].m_sVariable)->second;
		vector<wxString> l_ColorVector;
		wxString l_sColorSet = l_vVarColor[l_nVC].m_sColor;

		SP_CPN_ColorSet* l_pcColorSet = m_ColorSetClass.LookupColorSet(l_sColorSet);		
		if( ! l_pcColorSet )
			return false;
		l_ColorVector = l_pcColorSet->GetStringValue();
	
		//get all partial bindings for one variable
		vector<wxString>::iterator itVector;
		for(itVector = l_ColorVector.begin(); itVector != l_ColorVector.end(); itVector++)
		{			
			l_OneBinding[l_IndexMap.find(l_vVarColor[l_nVC].m_sVariable)->second] = *itVector;
			l_InternalBindingNew.push_back(l_OneBinding);			
			l_OneBinding.assign(l_nVariableNum, wxT("")); // reset it to "" for each position of the vector 
		}
		
		//for the first time
		if(l_InternalBindingOld.size() == 0 )
		{
			l_PartialBindingNew = l_InternalBindingNew;
		}

		for(unsigned i = 0; i < l_InternalBindingOld.size(); i++)
		{
			for(unsigned j = 0; j < l_InternalBindingNew.size(); j++)
			{				
				for(unsigned k = 0; k < l_vVarColor.size() ; k++)
				{
					wxString l_sOld = l_InternalBindingOld[i][k];
					wxString l_sNew = l_InternalBindingNew[j][k];						
					l_OneBinding[k] = l_sOld + l_sNew;
				}
				l_PartialBindingNew.push_back(l_OneBinding);
				l_OneBinding.assign(l_nVariableNum, wxT(""));
			}
		}

		l_InternalBindingOld.clear();
		l_InternalBindingOld = l_PartialBindingNew;
		l_InternalBindingNew.clear();
		l_PartialBindingNew.clear();		
	}

	(*l_pCompleteBinding) = l_InternalBindingOld;

	return true;
}

#endif

