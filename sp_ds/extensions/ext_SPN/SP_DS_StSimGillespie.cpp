 //////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: slehrack $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2007/07/12 09:35:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////


#include "SP_DS_StSimGillespie.h"

#include <math.h>
#include <algorithm>

#include "sp_gui/windows/SP_GUI_Mainframe.h"
#include "sp_gui/dialogs/dia_SPN/SP_DLG_StSimulationResults.h"
#include "sp_gui/dialogs/SP_DLG_SimulationProperties.h"

#include "sp_gui/mdi/SP_MDI_Doc.h"

#include "sp_ds/SP_DS_Node.h"
#include "sp_core/SP_Core.h"
#include "sp_core/tools/MersenneTwister.h"

#include "sp_ds/attributes/SP_DS_BoolAttribute.h"
#include "sp_ds/attributes/SP_DS_DoubleAttribute.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"
#include "sp_ds/attributes/SP_DS_NumberAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"

#include <wx/ffile.h>

SP_DS_StSimGillespie::SP_DS_StSimGillespie(SP_DS_Graph* p_pcGraph)
	:SP_DS_Simulation(p_pcGraph),
	m_nCombinedHazardValue(0),
	m_cParser( 0),
	m_pImmediateTransitionList( 0), m_qpcSimTimeEventTimeline( 0),
	m_anCurrentMarking( 0), m_anLastMarking(0),
	m_anLastHazardValues(0),
	m_aanResults( 0),
	m_aanSingleResults( 0),
    m_anCurrentFireCount(0), m_aanResultTransitionFireCount(0),
    m_aanSingleTransitionFireCount(0),
    m_pcThreadCountComboBox(NULL),
    m_nThreadCount(1)
{

	m_pImmediateTransitionList = new SP_DS_StSimImmediateTransitionList();
	m_qpcSimTimeEventTimeline = new SP_DS_StSimTimeEventTimeline();

	m_cParser = new SP_DS_StParser(p_pcGraph);	

	
	
	m_pcApParser = new SP_DS_ApParser(m_pcGraph);
	
	

}

bool SP_DS_StSimGillespie::AddToSimulationProperties(SP_DLG_SimulationProperties* p_pcDlg, wxSizer* p_pcSizer)
{
	SP_DS_Simulation::AddToSimulationProperties(p_pcDlg, p_pcSizer);

	int l_nCPUCount = wxThread::GetCPUCount();

	wxSizer* l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );
	l_pcRowSizer->Add(new wxStaticText( p_pcDlg, -1, wxT("Thread count:") ), 1, wxALL | wxEXPAND, 5);
	m_pcThreadCountComboBox = new wxChoice( p_pcDlg, -1 );
	for(int i = 1; i <= l_nCPUCount; ++i)
	{
		m_pcThreadCountComboBox->Append(wxString() << i);
	}
	m_pcThreadCountComboBox->SetSelection(m_nThreadCount-1);
	l_pcRowSizer->Add(m_pcThreadCountComboBox, 0, wxALL, 5);
	p_pcSizer->Add(l_pcRowSizer, 1, wxEXPAND | wxALL, 5);

	return true;
}

bool SP_DS_StSimGillespie::OnSimulationPropertiesOk()
{
	SP_DS_Simulation::OnSimulationPropertiesOk();

	unsigned long l_nValue = 0;

	if(m_pcThreadCountComboBox->GetStringSelection().ToULong(&l_nValue) && l_nValue > 0)
	{
		m_nThreadCount = l_nValue;
	}
	else
	{
		SP_MESSAGEBOX(wxT("Thread count has to be a number greater than zero!"), wxT("Error"));
		return false;
	}

	return true;
}

void SP_DS_StSimGillespie::SimulateRun()
{
	m_nCurrentTime = 0;
	m_nLastTime = 0;
	double l_nOutputTime = 0;

	m_nCombinedHazardValue = 0;

	long l_nChosenTransition = 0;

	unsigned long l_nCurrentResultPoint = 0;

	m_anCurrentMarking = m_anInitialNetMarking;

	m_anCurrentFireCount.assign(m_nTransitionCount, 0);
	m_anHazardValues.assign(m_nTransitionCount, 0);

	std::fill(m_aApFormulaHappend.begin(), m_aApFormulaHappend.end(), false);

	if (m_eExportType == SP_SIM_DIRECT_SINGLE_EXACT_EXPORT)
	{
		SP_VectorLong::iterator l_itI = m_anInitialNetMarking.begin();
		SP_VectorDouble::iterator l_itL = m_anLastMarking.begin();
		while(l_itI != m_anInitialNetMarking.end())
		{
			*l_itL = *l_itI;
			++l_itI;
			++l_itL;
		}
		m_anLastHazardValues.assign(m_nTransitionCount, 0);
	}

	InitialiseTimeline();
	CalculateAllHazardValues();
	CheckImmediateTransitions();

	CheckTimeEventTimeLine(l_nOutputTime, l_nCurrentResultPoint);

	GenerateResultPoints(l_nOutputTime, l_nCurrentResultPoint);

	while ((m_nCurrentTime < m_nIntervalEnd) && IsSimulationRunning())
	{
		m_nCurrentTime += GenerateRandomVariableExpDistr(m_nCombinedHazardValue);

		CheckTimeEventTimeLine(l_nOutputTime, l_nCurrentResultPoint);

		GenerateResultPoints(l_nOutputTime, l_nCurrentResultPoint);

		if (m_nCurrentTime < m_nIntervalEnd)
		{
			l_nChosenTransition	= GenerateRandomVariableDiscrete(m_nCombinedHazardValue);

			FireTransition(l_nChosenTransition);
			CheckImmediateTransitions();

			UpdateSimulationDialog(m_nCurrentSimulationRunNumber);
		}
	}

	CheckApFormulae(NULL);

	l_nOutputTime = m_nIntervalEnd;
	GenerateResultPoints(l_nOutputTime, l_nCurrentResultPoint);
}

SP_DS_StAnimationRunElem*
SP_DS_StSimGillespie::GetNextAnimationRunElem()
{

	SP_DS_StAnimationRunElem* l_pcNewElem = new SP_DS_StAnimationRunElem();

	double l_nCombinedHazardValue = 0;

	CalculateAllHazardValues();

	if (m_pImmediateTransitionList->GetElemCount() > 0)
	{

		l_pcNewElem->timepoint = m_nCurrentTime;

		l_pcNewElem->type = SP_ST_ANIM_RUN_FIRE_TRANSITION;
		l_pcNewElem->transitionArrayPos	= m_pImmediateTransitionList->ChooseTransitionByWeight();

		wxString actionDescription = wxT("Fire transition : ");
		actionDescription << m_msTransitionNames[ l_pcNewElem->transitionArrayPos ];

		l_pcNewElem->actionDescription->push_back(actionDescription);

		return l_pcNewElem;

	}

	l_nCombinedHazardValue = SumHazardValues();

	m_nCurrentTime += GenerateRandomVariableExpDistr(l_nCombinedHazardValue);
//TODO
	SP_MultimapSimTimeEvent l_mTimeEvents = m_qpcSimTimeEventTimeline->GetTimeEvent(m_nCurrentTime);

	if ( l_mTimeEvents.empty() && l_nCombinedHazardValue == 0)
	{
		wxDELETE(l_pcNewElem);
		return NULL;
	}

	size_t l_nChoose = (l_mTimeEvents.size() > 1) ? SP_RandomLong(l_mTimeEvents.size()) : 0;
	size_t i = 0;
	SP_DS_StSimTimeEvent* l_pcTimeEvent = NULL;

	for(SP_MultimapSimTimeEvent::iterator l_Iter = l_mTimeEvents.begin();
			l_Iter != l_mTimeEvents.end(); ++l_Iter)
	{
		if(i == l_nChoose)
		{
			l_pcTimeEvent = l_Iter->second;
			break;
		}
		++i;
	}

	if (l_pcTimeEvent)
	{
		l_pcNewElem->transitionArrayPos = -1;

		m_nCurrentTime = m_qpcSimTimeEventTimeline->GetTimeEventTime(l_pcTimeEvent);

		ExecuteTimeEventForAnimation(l_pcTimeEvent, l_pcNewElem);

		l_pcNewElem->timepoint = m_nCurrentTime;

		if (l_pcNewElem->transitionArrayPos == -1)
		{
			wxDELETE(l_pcNewElem);
			return GetNextAnimationRunElem();
		}
	}
	else
	{
		l_pcNewElem->timepoint = m_nCurrentTime;

		l_pcNewElem->type = SP_ST_ANIM_RUN_FIRE_TRANSITION;
		l_pcNewElem->transitionArrayPos = GenerateRandomVariableDiscrete(l_nCombinedHazardValue);

		wxString l_sActionDescription = wxT("Fire transition : ");
		l_sActionDescription << m_msTransitionNames[ l_pcNewElem->transitionArrayPos ];

		l_pcNewElem->actionDescription->push_back(l_sActionDescription);
	}

	return l_pcNewElem;

}

void SP_DS_StSimGillespie::GenerateResultPoints(double& p_nOutputTime, unsigned long& p_nCurrentResultPoint)
{
	while ( (p_nOutputTime <= m_nCurrentTime ) &&
			(p_nOutputTime <= m_nIntervalEnd ))
	{
		if(m_nIntervalStart <= p_nOutputTime && p_nCurrentResultPoint < m_aanResults.size())
		{
			GenerateResultPoint(p_nCurrentResultPoint++);
		}
		p_nOutputTime += m_nIntervalSize;
	}
}

void SP_DS_StSimGillespie::GenerateResultPoint(long p_nCurrentResultPoint)
{
	SP_VectorDouble& l_aResult = m_aanResults[ p_nCurrentResultPoint ];
	SP_VectorDouble& l_aResultSD = m_aanResultsSD[ p_nCurrentResultPoint ];
	SP_VectorDouble::iterator l_itResult = l_aResult.begin();
	SP_VectorDouble::iterator l_itResultSD = l_aResultSD.begin();
	SP_VectorLong::iterator l_itMarking = m_anCurrentMarking.begin();
	for (; l_itResult != l_aResult.end(); ++l_itResult, ++l_itResultSD, ++l_itMarking)
	{
		double l_nMarking = *l_itMarking;
		*l_itResult += l_nMarking;
		*l_itResultSD += pow(l_nMarking, 2.0);
	}

	if (m_eExportType == SP_SIM_DIRECT_SINGLE_EXPORT || m_bComAuxVarSingleRun == true)
	{
		SP_VectorDouble& l_aSingleResult = m_aanSingleResults[ p_nCurrentResultPoint ];
		std::copy(m_anCurrentMarking.begin(), m_anCurrentMarking.end(), l_aSingleResult.begin());

		SP_VectorDouble& l_aSingleFireCount = m_aanSingleTransitionFireCount[ p_nCurrentResultPoint ];
		std::copy(m_anCurrentFireCount.begin(), m_anCurrentFireCount.end(), l_aSingleFireCount.begin());
	}

	SP_VectorDouble& l_aResultFireCount = m_aanResultTransitionFireCount[ p_nCurrentResultPoint ];
	SP_VectorDouble::iterator l_itResultFireCount = l_aResultFireCount.begin();
	SP_VectorLong::iterator l_itFireCount = m_anCurrentFireCount.begin();
	for (; l_itResultFireCount != l_aResultFireCount.end(); ++l_itResultFireCount, ++l_itFireCount)
	{
		*l_itResultFireCount += *l_itFireCount;
		*l_itFireCount = 0;
	}
}

void SP_DS_StSimGillespie::CheckTimeEventTimeLine(double& p_nOutputTime, unsigned long& p_nCurrentResultPoint)
{
	bool l_bGenerateResultPoints = (m_nCurrentTime == 0.0) ? false : true;

	SP_MultimapSimTimeEvent	l_pcTimeEvent = m_qpcSimTimeEventTimeline->GetTimeEvent(m_nCurrentTime);
	if(!l_pcTimeEvent.empty())
	{
		m_nCurrentTime = l_pcTimeEvent.begin()->first;
	}
	while(!l_pcTimeEvent.empty() && m_nCurrentTime <= m_nIntervalEnd
			&& IsSimulationRunning())
	{
		if(l_bGenerateResultPoints)
		{
			GenerateResultPoints(p_nOutputTime, p_nCurrentResultPoint);
		}

		list<SP_DS_StSimTimeEvent*> l_lConsuming;
		list<SP_DS_StSimTimeEvent*> l_lNonConsuming;

		for(SP_MultimapSimTimeEvent::const_iterator l_Iter = l_pcTimeEvent.begin();
				l_Iter != l_pcTimeEvent.end(); ++l_Iter)
		{
			m_qpcSimTimeEventTimeline->RemoveElem(l_Iter->second);
			if(IsTransitionEnabled(l_Iter->second->arrayPos))
			{
				if(m_apcSimTransitions[l_Iter->second->arrayPos]->nonConsuming)
				{
					l_lNonConsuming.push_back(l_Iter->second);
				}
				else
				{
					l_lConsuming.push_back(l_Iter->second);
				}
			}
			if(l_Iter->second->eventType == SP_ST_SIM_TIME_EVENT_DET_FIRING_PERIODIC)
			{
				if (l_Iter->second->period > 0 && l_Iter->second->intervalEnd >= m_nCurrentTime + l_Iter->second->period)
				{
					m_qpcSimTimeEventTimeline->InsertTimeEvent(	l_Iter->second, m_nCurrentTime + l_Iter->second->period);
				}
			}
		}
		//fire consuming transitions
		while(!l_lConsuming.empty())
		{
			size_t l_nSize = l_lConsuming.size();
			size_t l_nChoose = (l_nSize > 1) ? SP_RandomLong(l_nSize) : 0;
			size_t i = 0;
			for(list<SP_DS_StSimTimeEvent*>::iterator l_Iter = l_lConsuming.begin();
					l_Iter != l_lConsuming.end(); ++l_Iter)
			{
				if(i == l_nChoose)
				{
					if(IsTransitionEnabled((*l_Iter)->arrayPos))
					{
						FireTransition((*l_Iter)->arrayPos);
						CalculateHazardValue((*l_Iter)->arrayPos);
					}
					l_lConsuming.erase(l_Iter);
					break;
				}
				++i;
			}
		}
		//fire non consuming transitions
		for(list<SP_DS_StSimTimeEvent*>::iterator l_Iter = l_lNonConsuming.begin();
				l_Iter != l_lNonConsuming.end(); ++l_Iter)
		{
			FireTransition((*l_Iter)->arrayPos);
			CalculateHazardValue((*l_Iter)->arrayPos);
		}

		CheckImmediateTransitions();

		m_nCurrentTime += GenerateRandomVariableExpDistr(m_nCombinedHazardValue);
		l_pcTimeEvent = m_qpcSimTimeEventTimeline->GetTimeEvent(m_nCurrentTime);
		if(!l_pcTimeEvent.empty())
		{
			m_nCurrentTime = l_pcTimeEvent.begin()->first;
		}

		if (m_nCurrentTime < m_nIntervalEnd)
		{
			UpdateSimulationDialog(m_nCurrentSimulationRunNumber);
		}
	}
}

void SP_DS_StSimGillespie::ExecuteTimeEventForAnimation(
		SP_DS_StSimTimeEvent* p_pcTimeEvent,
		SP_DS_StAnimationRunElem* p_pcRunElem)
{
	wxString actionDescription;
	long l_nArrayPos = p_pcTimeEvent->arrayPos;
	double l_nPeriod = p_pcTimeEvent->period;
	double l_nIntervallEnd = p_pcTimeEvent->intervalEnd;

	if(p_pcTimeEvent->eventType == SP_ST_SIM_TIME_EVENT_TIMED_FIRING_SINGLE
		|| p_pcTimeEvent->eventType == SP_ST_SIM_TIME_EVENT_DET_FIRING_SINGLE
		|| p_pcTimeEvent->eventType == SP_ST_SIM_TIME_EVENT_DET_FIRING_PERIODIC)
	{
		m_qpcSimTimeEventTimeline->RemoveElem(p_pcTimeEvent);

		if (IsTransitionEnabled(l_nArrayPos) )
		{
			FireTransition(l_nArrayPos);

			p_pcRunElem->type = SP_ST_ANIM_RUN_FIRE_TRANSITION;
			p_pcRunElem->transitionArrayPos = l_nArrayPos;

			actionDescription = wxT("Fire transition : ");
			actionDescription << m_msTransitionNames[ p_pcRunElem->transitionArrayPos ];

			p_pcRunElem->actionDescription->push_back(actionDescription);
		}

	}

	if(p_pcTimeEvent->eventType == SP_ST_SIM_TIME_EVENT_DET_FIRING_PERIODIC)
	{
		if (l_nPeriod > 0 && l_nIntervallEnd >= m_nCurrentTime + l_nPeriod)
		{
			m_qpcSimTimeEventTimeline->InsertTimeEvent(p_pcTimeEvent, m_nCurrentTime + l_nPeriod);
		}
	}
}

double SP_DS_StSimGillespie::GenerateRandomVariableExpDistr(double p_nLambda)
{
	if(p_nLambda == 0)
	{
		wxString l_Msg = wxT("dead state detected at t=");
		l_Msg << m_nCurrentTime;
		SP_LOGMESSAGE( l_Msg);
	}
	double l_nRandom = SP_RandomDouble();

	return -1 * log(l_nRandom) / p_nLambda;
}

long SP_DS_StSimGillespie::GenerateRandomVariableDiscrete(double p_nSum)
{
	while(true)
	{
		double l_nU = p_nSum * SP_RandomDouble();
		double l_nRunningSum = 0;
		SP_VectorDouble::const_iterator l_it;
		long l_nTransitionPos = 0;
		for(l_it = m_anHazardValues.begin(); l_it != m_anHazardValues.end(); ++l_it)
		{
			l_nRunningSum += *l_it;
			if (l_nRunningSum >= l_nU )
			{
				return l_nTransitionPos;
			}
			++l_nTransitionPos;
		}
	}
	return -1;
}

void SP_DS_StSimGillespie::FireTransition(long p_nTransNumber)
{
	if(p_nTransNumber < 0)
	{
		wxString l_sMsg = wxT("error, wrong transition number ");
		l_sMsg << p_nTransNumber;
		SP_LOGERROR( l_sMsg);
	}
	else
	{
		SP_DS_StSimTransition* l_pcSimTransition = m_apcSimTransitions[p_nTransNumber];

		CheckApFormulae(l_pcSimTransition);

		for(unsigned long i = 0; i < l_pcSimTransition->manipulatedPlacePos.size(); i++)
		{
			long l_nPlacePos = l_pcSimTransition->manipulatedPlacePos[i];
			if(l_pcSimTransition->resetMarking[i])
			{
				m_anCurrentMarking[ l_nPlacePos] = 0;
			}
			m_anCurrentMarking[l_nPlacePos] += l_pcSimTransition->deltaMarking[i];
		}
		if(m_eExportType == SP_SIM_DIRECT_SINGLE_EXACT_EXPORT)
		{
			DirectSingleExactExport();
		}

		m_anCurrentFireCount[p_nTransNumber] += 1;

		if(m_nLastTime != m_nCurrentTime)
		{
			m_nLastTime = m_nCurrentTime;
		}

		for(unsigned long i = 0; i < l_pcSimTransition->toUpdateTransitionPos.size(); ++i)
		{
			CalculateHazardValue(l_pcSimTransition->toUpdateTransitionPos[i]);
		}
	}
}

void SP_DS_StSimGillespie::CalculateHazardValue(long p_nTransitionNumber)
{
	SP_DS_StSimTransition* l_pcSimTransition = m_apcSimTransitions[ p_nTransitionNumber ];

	if ( !IsTransitionEnabled(p_nTransitionNumber) )
	{
		if(l_pcSimTransition->hazardFunctionType == SP_ST_SIM_HAZARD_FUNCTION_TIMED_TRANSITION)
		{
			m_qpcSimTimeEventTimeline->RemoveElem(l_pcSimTransition->timeEvent);
		}
		else if(l_pcSimTransition->hazardFunctionType == SP_ST_SIM_HAZARD_FUNCTION_IMMEDIATE_TRANSITION)
		{
			m_pImmediateTransitionList->RemoveElem(p_nTransitionNumber);
		}
		if(m_anHazardValues[p_nTransitionNumber] > 0)
		{
			m_nCombinedHazardValue -= m_anHazardValues[p_nTransitionNumber];
			m_anHazardValues[p_nTransitionNumber] = 0;
			if(m_nCombinedHazardValue < 0)
			{
				m_nCombinedHazardValue = 0;
			}
		}
		return;
	}
	if( l_pcSimTransition->hazardFunctionType == SP_ST_SIM_HAZARD_FUNCTION_BMA_CONSTANT_PARAMETER ||
		l_pcSimTransition->hazardFunctionType == SP_ST_SIM_HAZARD_FUNCTION_BMA_DYNAMIC_PARAMETER ||
		l_pcSimTransition->hazardFunctionType == SP_ST_SIM_HAZARD_FUNCTION_BIO_LEVEL_INTERPRETATION ||
		l_pcSimTransition->hazardFunctionType == SP_ST_SIM_HAZARD_FUNCTION_BIO_MICHAELIS_MENTEN ||
		l_pcSimTransition->hazardFunctionType == SP_ST_SIM_HAZARD_FUNCTION_DYNAMIC ||
		l_pcSimTransition->hazardFunctionType == SP_ST_SIM_HAZARD_FUNCTION_CONSTANT)
	{
		double l_nHazardValue = m_cParser->Evaluate(l_pcSimTransition->hazardTree,
													l_pcSimTransition);
		if(l_nHazardValue < 0)
		{
			l_nHazardValue = 0;
		}

		m_nCombinedHazardValue -= m_anHazardValues[p_nTransitionNumber];
		m_nCombinedHazardValue += l_nHazardValue;
		m_anHazardValues[p_nTransitionNumber] = l_nHazardValue;

		if(m_nCombinedHazardValue < 0)
		{
			m_nCombinedHazardValue = 0;
		}
	}
	else if(l_pcSimTransition->hazardFunctionType == SP_ST_SIM_HAZARD_FUNCTION_IMMEDIATE_TRANSITION)
	{
		double l_nHazardValue = m_cParser->Evaluate(l_pcSimTransition->hazardTree,
													l_pcSimTransition);
		if(l_nHazardValue < 0)
		{
			l_nHazardValue = 0;
		}
		m_pImmediateTransitionList->InsertImmediateTransition(p_nTransitionNumber, l_nHazardValue);
	}
	else if(l_pcSimTransition->hazardFunctionType == SP_ST_SIM_HAZARD_FUNCTION_TIMED_TRANSITION)
	{
		double l_nTime = m_cParser->Evaluate(l_pcSimTransition->hazardTree->left, l_pcSimTransition);
		l_pcSimTransition->timeEvent->fixedTime = l_nTime;
		m_qpcSimTimeEventTimeline->InsertTimeEvent(l_pcSimTransition->timeEvent,
							m_nCurrentTime + l_nTime);
	}
}

bool SP_DS_StSimGillespie::IsTransitionEnabled( long p_nTransitionNumber )
{
	SP_DS_StSimTransition* l_pcSimTransition = m_apcSimTransitions[p_nTransitionNumber];
	for (unsigned long i = 0; i < l_pcSimTransition->basedPlacePos.size(); i++)
	{
		long l_nPlacePos = l_pcSimTransition->basedPlacePos[i];
		if( m_anCurrentMarking[ l_nPlacePos ] < l_pcSimTransition->intervalLowerBound[i] ||
			m_anCurrentMarking[ l_nPlacePos ] >= l_pcSimTransition->intervalUpperBound[i])
		{
			return false;
		}
	}

	return true;
}

void SP_DS_StSimGillespie::CalculateAllHazardValues()
{

	for (unsigned long i = 0; i < m_nTransitionCount; i++)
	{
		CalculateHazardValue(i);
	}
}

double SP_DS_StSimGillespie::SumHazardValues()
{

	double l_nSum = .0;

	for (unsigned long i = 0; i < m_nTransitionCount; i++)
	{
		l_nSum += m_anHazardValues[i];
	}

	return l_nSum;

}

void SP_DS_StSimGillespie::StartSimulation()
{

	unsigned long i = 0;
	unsigned long j = 0;

	m_nCurrentSimulationRunNumber = 0;
	m_bFlagSimulationRunning = true;

	m_nCurrentIterationNumber = 0;

	m_nIterationCount = (m_nIntervalEnd / m_nIntervalSize) * m_nSimulationRunCount;

	m_pcSimulationResultsDlg->SetSimulationStopWatch(0);
	m_cSimulationStopWatch.Start(0);

	if (m_eExportType != SP_SIM_NO_EXPORT)
	{
		m_pcSimulationResultsDlg->OpenExportFile();
	}

	for (i = 0; i < m_nResultPointCount + 2; i++)
	{
		for (j = 0; j < m_nPlaceCount; j++)
		{
			m_aanResults[ i ][ j ] = 0;
			m_aanResultsSD[i][j] = 0;
		}
	}

	for (i = 0; i < m_nResultPointCount + 2; i++)
	{
		for (j = 0; j < m_nTransitionCount; j++)
		{
			m_aanResultTransitionFireCount[ i ][ j ] = 0;
		}
	}

	if(m_nThreadCount > 1)
	{
		m_nThreadFinished = 0;
		unsigned long l_Runs = 1;
		unsigned long l_FirstRun = 1;
		unsigned long l_Threads = m_nSimulationRunCount;
		if(m_nSimulationRunCount > m_nThreadCount)
		{
			l_Runs = m_nSimulationRunCount / m_nThreadCount;
			l_FirstRun = l_Runs + m_nSimulationRunCount % m_nThreadCount;
			l_Threads = m_nThreadCount;
		}

		vector<SP_DS_StSimGillespie::SimulationThread*> l_aTrheads(l_Threads);
		for(unsigned long i = 0; i < l_Threads; ++i)
		{
			SP_DS_StSimGillespie::SimulationThread* l_pcThread =
					new SP_DS_StSimGillespie::SimulationThread(i,
							this, m_pcGraph,
							m_nIntervalStart,
							m_nIntervalEnd,
							m_nIntervalSize,
							m_nResultPointCount,
							m_nPlaceCount,
							m_nTransitionCount,
							m_msTransitionFunctions,
							m_apcSimPlaces,
							m_apcSimTransitions,
							m_anInitialNetMarking,
							m_aanPreIncMatrix,
							(i == 0 ? l_FirstRun : l_Runs),
							m_aApFormulas);

			l_pcThread->Create();
			l_aTrheads[i] = l_pcThread;
		}
		for(unsigned long i = 0; i < l_Threads; ++i)
		{
			l_aTrheads[i]->Run();
		}
		SP_DS_StSimGillespie::SimulationThread* l_pcThread = l_aTrheads[0];
		while(m_nThreadFinished < l_Threads)
		{
			UpdateSimulationDialog(l_pcThread->GetCurrentSimulationRunCount()*l_Threads);
			wxSleep(1);
		}
		for(unsigned long i = 0; i < l_Threads; ++i)
		{
			l_aTrheads[i]->Wait();
		}
		for(unsigned long i = 0; i < l_Threads; ++i)
		{
			SP_DS_StSimGillespie::SimulationThread* l_pcThread = l_aTrheads[i];
			wxDELETE(l_pcThread);
		}
		m_nCurrentSimulationRunNumber = m_nSimulationRunCount;
	}
	else
	{
		while (m_nCurrentSimulationRunNumber < m_nSimulationRunCount)
		{
			
			if (m_eExportType == SP_SIM_DIRECT_SINGLE_EXPORT || m_bComAuxVarSingleRun == true)
			{
				for (i = 0; i < m_nResultPointCount + 2; i++)
				{
					for (j = 0; j < m_nPlaceCount; j++)
					{
						m_aanSingleResults[ i ][ j ] = 0;
					}

					for(j=0; j< m_nTransitionCount; j++)
					{
						m_aanSingleTransitionFireCount[i][j]=0;
					}
				}
			}
			else 
				if(m_eExportType == SP_SIM_DIRECT_SINGLE_EXACT_EXPORT)
			{
				m_pcSimulationResultsDlg->DirectSingleExactExportToCSV(m_nCurrentSimulationRunNumber,
																		-1,
																		m_anLastMarking, m_anLastHazardValues);
			}

			SimulateRun();

			if (m_eExportType == SP_SIM_DIRECT_SINGLE_EXPORT)
			{
				m_pcSimulationResultsDlg->ComputeEachSimulationAuxiliaryVars();

				m_pcSimulationResultsDlg->DirectSingleExportToCSV(m_nCurrentSimulationRunNumber);
			}
			else if(m_eExportType == SP_SIM_DIRECT_SINGLE_EXACT_EXPORT)
			{
				DirectSingleExactExport();
			}

			//to compute the auxiliary variables
			if( m_bComAuxVarSingleRun == true)
			{
				m_pcSimulationResultsDlg->ComputeAverageAuxiliaryVars(m_nCurrentSimulationRunNumber,m_nSimulationRunCount);
			}


			m_nCurrentSimulationRunNumber++;

		}
	}
	m_bFlagSimulationRunning = false;

	WriteResults();
	m_nCurrentSimulationRunNumber = 0;

	if (m_eExportType == SP_SIM_DIRECT_EXPORT)
	{
		m_pcSimulationResultsDlg->DirectExportToCSV();
	}

	if (m_eExportType != SP_SIM_NO_EXPORT)
	{
		m_pcSimulationResultsDlg->CloseExportFile();
	}

	m_cSimulationStopWatch.Pause();
	m_pcSimulationResultsDlg->SetSimulationProgressGauge(100);
	m_pcSimulationResultsDlg->SetSimulationStopWatch(m_cSimulationStopWatch.Time());

	m_pcSimulationResultsDlg->GetStartButton()->SetBackgroundColour( *wxGREEN);
	m_pcSimulationResultsDlg->GetStartButton()->SetLabel(wxT("Start simulation"));

}

bool SP_DS_StSimGillespie::Initialise()
{
	if( m_pcGraph->GetNetclass()->GetName() !=  SP_DS_COLSPN_CLASS)
	{
		SetMemoryFree();
	}

	


	if(m_pcSimulationResultsDlg)
	{
		m_eExportType = m_pcSimulationResultsDlg->GetExportType();

		m_bComAuxVarSingleRun = m_pcSimulationResultsDlg->GetComAuxVarSingleRun();

		if( m_pcGraph->GetNetclass()->GetName() == SP_DS_COLSPN_CLASS )
		{
			m_pcApParser->SetUnfoldedNet(m_pcUnfoldedNet);
		}

		m_pcApParser->Initialise();

		wxString l_sApFormulae = m_pcSimulationResultsDlg->GetApFormulae();
		
		if(!l_sApFormulae.IsEmpty())
		{
			if(m_pcApParser->Parse(l_sApFormulae))
			{
				rfdag::vectorRFDAG l_Expressions = m_pcApParser->GetExpressions();
				for(size_t i = 0; i < l_Expressions.size(); ++i)
				{
					if(m_pcGraph->GetNetclass()->GetName() == SP_DS_COLSPN_CLASS)
					{						
						m_aApFormulas.push_back(SP_DS_ApFormula(m_pcUnfoldedNet,m_pcGraph, l_Expressions[i]));
					}
					else
					{
						m_aApFormulas.push_back(SP_DS_ApFormula(m_pcGraph, l_Expressions[i]));
					}
					m_aApFormulaHappend.push_back(false);
					m_aApResults.push_back(0);
					m_sApFormulas.push_back(wxString(l_Expressions[i].toString().c_str(), wxConvUTF8));
				}
			}
		}
	}

	m_nCurrentTime = 0;
	m_nLastTime = 0;

	//deal with color stochastic Petri Nets
	if( m_pcGraph->GetNetclass()->GetName() ==  SP_DS_COLSPN_CLASS)
	{
		LoadData();
	}
	else
	{
		LoadPlaces();
		LoadTransitions();
	}

	m_anCurrentMarking.assign(m_nPlaceCount, 0);
	m_anCurrentFireCount.assign(m_nTransitionCount, 0);

	m_anHazardValues.assign(m_nTransitionCount, 0);

	if (m_nResultPointCount > 0)
	{
		m_aanResults.assign(m_nResultPointCount+2, SP_VectorDouble(m_nPlaceCount, 0));
		m_aanResultsSD.assign(m_nResultPointCount+2, SP_VectorDouble(m_nPlaceCount, 0));
		m_aanResultTransitionFireCount.assign(m_nResultPointCount+2, SP_VectorDouble(m_nTransitionCount, 0));
	}

	if (m_eExportType == SP_SIM_DIRECT_SINGLE_EXPORT || m_bComAuxVarSingleRun == true)
	{
		m_aanSingleResults.assign(m_nResultPointCount+2, SP_VectorDouble(m_nPlaceCount, 0));
		m_aanSingleTransitionFireCount.assign(m_nResultPointCount+2, SP_VectorDouble(m_nTransitionCount, 0));
	}
	else if (m_eExportType == SP_SIM_DIRECT_SINGLE_EXACT_EXPORT)
	{
		m_anLastMarking.assign(m_nPlaceCount, 0);
		m_anLastHazardValues.assign(m_nTransitionCount, 0);
	}

	m_cParser->SetCurrentMarking(&m_anCurrentMarking);

	m_cParser->SetSimTime( &m_nCurrentTime);

	m_cParser->SetSimIntervalStart(m_nIntervalStart);
	m_cParser->SetSimIntervalEnd(m_nIntervalEnd);

	m_cParser->SetPlaceArrayPos(&m_mnPlaceArrayPos);
	m_cParser->SetTransitionArrayPos(&m_mnTransitionArrayPos);
	m_cParser->SetPreIncMatrix(m_aanPreIncMatrix);

	m_cParser->LoadParameter();

	if( m_pcGraph->GetNetclass()->GetName() != SP_DS_COLSPN_CLASS )
	{		
		m_cParser->LoadPrePlaces();
	}
	else
	{
		m_cParser->SetPrePlaceArrayPosOfTransitionNode(m_mmPrePlaceArrayPosOfTransitionNode);		
	}


	if ( InitSimTransitions() && InitSimPlaces())
	{

		for (unsigned long i = 0; i < m_nTransitionCount; i++)
		{
			SetToUpdateTransitionPosOfTransitions(i);
		}

		InitialiseTimeline();

#ifdef __WXDEBUG__
		//Debug_ShowDeltaMatrix();
		//Debug_ShowSimTransitions();
		//Debug_ShowSimPlaces();


		//    Debug_ShowSimTimeEventQueue( m_qpcSimTimeEventTimeline.m_pcFirstTimelineElem );

		//    m_qpcSimTimeEventTimeline.SwitchElements( m_qpcSimTimeEventTimeline.m_pcFirstTimelineElem,
		//      m_qpcSimTimeEventTimeline.m_pcFirstTimelineElem->next->next->next->next );

		//    Debug_ShowSimTimeEventQueue( m_qpcSimTimeEventTimeline.m_pcFirstTimelineElem );
#endif
		return true;

	}
	else
	{

		/*
		 Debug_ShowDeltaMatrix();

		 Debug_ShowSimTransitions();
		 Debug_ShowSimPlaces();

		 Debug_ShowSimTimeEventQueue();
		 */
		return false;

	}

}

void SP_DS_StSimGillespie::LoadPlaces()
{
	SP_DS_Nodeclass* l_pcNodeclass;
	l_pcNodeclass= m_pcGraph->GetNodeclass(wxT("Place"));

	m_nPlaceCount = l_pcNodeclass->GetElements()->size();

	m_anInitialNetMarking.resize(m_nPlaceCount, 0);
	m_mpcPlaceNodes.resize(m_nPlaceCount);
	m_msPlaceNames.resize(m_nPlaceCount);

	SP_ListNode::const_iterator l_itElem;
	wxString l_sPlaceName;

	SP_DS_ColListAttribute* l_pcColList;

	long l_nPos = 0;
	long l_nLong = 0;
	bool l_bUniqueNames = true;

	for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
	{

		m_mpcPlaceNodes[l_nPos] = (*l_itElem);

		l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>(m_mpcPlaceNodes[ l_nPos ]->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		if(m_mnPlaceArrayPos.find(l_sPlaceName) != m_mnPlaceArrayPos.end())
		{
			wxString l_sMessage = wxT("place ") + l_sPlaceName + wxT(" is not unique!");
			SP_LOGWARNING( l_sMessage);
			l_bUniqueNames = false;
		}
		m_mnPlaceArrayPos[ l_sPlaceName ] = l_nPos;
		m_msPlaceNames[l_nPos] = l_sPlaceName;
		m_msPlaceNamesById[ wxString::Format( wxT("%ld"), m_mpcPlaceNodes[ l_nPos ]->GetId() ) ] = l_sPlaceName;

		l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(m_mpcPlaceNodes[ l_nPos ]->GetAttribute(wxT("MarkingList")) );

		l_pcColList->GetCell( l_pcColList->GetActiveList(), 1).ToLong( &l_nLong);

		m_anInitialNetMarking[ l_nPos ] = l_nLong;

		l_nPos++;
	}
	if(!l_bUniqueNames)
	{
		SP_MESSAGEBOX(wxT("Warning: Every place need a unique name for stochastic simulation!\n\nSee log (Alt+3) for more details!"), wxT("Warning"));
	}
}

void SP_DS_StSimGillespie::LoadTransitions()
{
	SP_DS_Nodeclass* l_pcNodeclass;
	l_pcNodeclass= m_pcGraph->GetNodeclass(wxT("Transition"));
	long l_nTran = l_pcNodeclass->GetElements()->size();

    SP_DS_Nodeclass* l_pcNodeclassforImmediateTrans;
	l_pcNodeclassforImmediateTrans = m_pcGraph->GetNodeclass(wxT("Immediate Transition"));
	long l_nImmTran = l_pcNodeclassforImmediateTrans->GetElements()->size();

	SP_DS_Nodeclass* l_pcNodeclassforDeterminTrans;
	l_pcNodeclassforDeterminTrans= m_pcGraph->GetNodeclass(wxT("Deterministic Transition"));
	long l_nDeterTran = l_pcNodeclassforDeterminTrans->GetElements()->size();

	SP_DS_Nodeclass* l_pcNodeclassforScheduedTrans;
	l_pcNodeclassforScheduedTrans= m_pcGraph->GetNodeclass(wxT("Scheduled Transition"));
	long l_nSchedTran = l_pcNodeclassforScheduedTrans->GetElements()->size();

	m_nTransitionCount = l_nTran+l_nImmTran+l_nDeterTran+l_nSchedTran;

	m_mpcTransitionNodes.resize(m_nTransitionCount);
	m_msTransitionNames.resize(m_nTransitionCount);
	m_msTransitionFunctions.resize(m_nTransitionCount);

	m_aanDeltaIncMatrix = new SP_MatrixDouble(m_nTransitionCount, m_nPlaceCount, 0);
	m_aanResetIncMatrix = new SP_MatrixBool(m_nTransitionCount, m_nPlaceCount, false);
	m_aanPreIncMatrix = new SP_MatrixPairDouble(m_nTransitionCount, m_nPlaceCount, SP_PairDouble(0,LONG_MAX));
	m_abNonConsumingMode.assign(m_nTransitionCount, true);

	SP_ListNode::const_iterator l_itElem;
	wxString l_sTransitionName;

	SP_DS_ColListAttribute* l_pcColList;

	long l_nPos = 0;

	for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
	{
		SP_DS_Node* l_pcNode = *l_itElem;
		m_mpcTransitionNodes[l_nPos] = l_pcNode;

		SetDeltaVector(l_pcNode, l_nPos);

		l_sTransitionName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		m_mnTransitionArrayPos[ l_sTransitionName ] = l_nPos;
		m_msTransitionNames[l_nPos] = l_sTransitionName;
		m_msTransitionNamesById[ wxString::Format( wxT("%ld"), l_pcNode->GetId() ) ] = l_sTransitionName;

		l_pcColList	= dynamic_cast< SP_DS_ColListAttribute* >(l_pcNode->GetAttribute(wxT("FunctionList")) );

		m_msTransitionFunctions[l_nPos] = l_pcColList->GetActiveCellValue( 1);

		l_nPos++;
	}

	for (l_itElem = l_pcNodeclassforImmediateTrans->GetElements()->begin(); l_itElem != l_pcNodeclassforImmediateTrans->GetElements()->end(); ++l_itElem)
	{
		SP_DS_Node* l_pcNode = *l_itElem;
		m_mpcTransitionNodes[l_nPos] = l_pcNode;

		SetDeltaVector(l_pcNode, l_nPos);

		l_sTransitionName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		m_mnTransitionArrayPos[ l_sTransitionName ] = l_nPos;
		m_msTransitionNames[l_nPos] = l_sTransitionName;
		m_msTransitionNamesById[ wxString::Format( wxT("%ld"), l_pcNode->GetId() ) ] = l_sTransitionName;

		l_pcColList	= dynamic_cast< SP_DS_ColListAttribute* >(l_pcNode->GetAttribute(wxT("FunctionList")) );

		m_msTransitionFunctions[l_nPos] = wxT("ImmediateFiring(") + l_pcColList->GetActiveCellValue( 1) + wxT(")");

		l_nPos++;
	}

    for (l_itElem = l_pcNodeclassforDeterminTrans->GetElements()->begin(); l_itElem != l_pcNodeclassforDeterminTrans->GetElements()->end(); ++l_itElem)
	{
		SP_DS_Node* l_pcNode = *l_itElem;
		m_mpcTransitionNodes[l_nPos] = l_pcNode;

		SetDeltaVector(l_pcNode, l_nPos);

		l_sTransitionName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		m_mnTransitionArrayPos[ l_sTransitionName ] = l_nPos;
		m_msTransitionNames[l_nPos] = l_sTransitionName;
		m_msTransitionNamesById[ wxString::Format( wxT("%ld"), l_pcNode->GetId() ) ] = l_sTransitionName;

		l_pcColList	= dynamic_cast< SP_DS_ColListAttribute* >(l_pcNode->GetAttribute(wxT("DelayList")) );

		wxString l_delayvalue=l_pcColList->GetActiveCellValue( 1);
		m_msTransitionFunctions[l_nPos] = wxT("TimedFiring(")+ l_delayvalue +wxT(")");//Get the delay value

		l_nPos++;
	}

    for (l_itElem = l_pcNodeclassforScheduedTrans->GetElements()->begin(); l_itElem != l_pcNodeclassforScheduedTrans->GetElements()->end(); ++l_itElem)
	{
		SP_DS_Node* l_pcNode = *l_itElem;
		m_mpcTransitionNodes[l_nPos] = l_pcNode;

		SetDeltaVector(l_pcNode, l_nPos);

		l_sTransitionName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		m_mnTransitionArrayPos[ l_sTransitionName ] = l_nPos;
		m_msTransitionNames[l_nPos] = l_sTransitionName;
		m_msTransitionNamesById[ wxString::Format( wxT("%ld"), l_pcNode->GetId() ) ] = l_sTransitionName;

		l_pcColList	= dynamic_cast< SP_DS_ColListAttribute* >(l_pcNode->GetAttribute(wxT("PeriodicList")) );

		wxString l_Begin=l_pcColList->GetActiveCellValue( 1);
		wxString l_Repetition=l_pcColList->GetActiveCellValue( 2);  //FixedTimedFiring_Single( . ) not realized
		wxString l_End=l_pcColList->GetActiveCellValue( 3); //FixedTimedFiring_Periodic( _SimStart, . , _SimEnd )
		m_msTransitionFunctions[l_nPos] = wxT("FixedTimedFiring_Periodic(")+ l_Begin +wxT(",")+ l_Repetition+wxT(",")+l_End+wxT(")");

		l_nPos++;
	}
}

bool SP_DS_StSimGillespie::InitSimPlaces()
{
	m_apcSimPlaces.resize(m_nPlaceCount);
	SP_DS_StSimPlace* l_pcSimPlace;
	bool l_bOk = true;
	for (unsigned long i = 0; i < m_nPlaceCount; i++)
	{
		l_pcSimPlace = CreateSimPlace(i);
		m_apcSimPlaces[ i ] = l_pcSimPlace;
		if (l_pcSimPlace == NULL)
		{
			l_bOk = false;
		}
		else
		{
			SetToUpdateTransitionPosOfPlaces(i);
		}
	}
	return l_bOk;
}

bool SP_DS_StSimGillespie::InitSimTransitions()
{
	m_apcSimTransitions.resize(m_nTransitionCount);
	SP_DS_StSimTransition* l_pcSimTransition;
	bool l_bOk = true;
	for (unsigned long i = 0; i < m_nTransitionCount; i++)
	{
		l_pcSimTransition = CreateSimTransition(i);
		m_apcSimTransitions[ i ] = l_pcSimTransition;
		if (l_pcSimTransition == NULL)
		{
			l_bOk = false;
		}
	}
	return l_bOk;
}

SP_DS_StSimPlace* SP_DS_StSimGillespie::CreateSimPlace(long p_nPlaceArrayPos)
{
	SP_DS_StSimPlace* l_pcSimPlace = new SP_DS_StSimPlace();
	l_pcSimPlace->arrayIndex = p_nPlaceArrayPos;

	return l_pcSimPlace;
}

SP_DS_StSimTransition* SP_DS_StSimGillespie::CreateSimTransition(long p_nTransitionArrayPos)
{
	SP_DS_StSimTransition* l_pcSimTransition= NULL;

	double l_nTimePoint = 0;
	double l_nTimePeriod = 0;

	SP_DS_StParserNode* l_pcRoot = 0;
	SP_DS_StSimHazardFunctionType l_peHazardFunctionType;

	double l_nIntervalStart;
	double l_nIntervalEnd;

	if (m_cParser->ParseFormulaFunction(m_msTransitionFunctions[ p_nTransitionArrayPos ],
										p_nTransitionArrayPos,
										l_pcRoot,
										&l_peHazardFunctionType) )
	{

		l_pcSimTransition = new SP_DS_StSimTransition();

		l_pcSimTransition->arrayIndex = p_nTransitionArrayPos;
		l_pcSimTransition->timeEvent->arrayPos = p_nTransitionArrayPos;
		l_pcSimTransition->basedPlacePos = m_cParser->GetBasedPlaces();
		l_pcSimTransition->nonConsuming = m_abNonConsumingMode[p_nTransitionArrayPos];
		//based places
		l_pcSimTransition->intervalLowerBound.reserve(l_pcSimTransition->basedPlacePos.size());
		l_pcSimTransition->intervalUpperBound.reserve(l_pcSimTransition->basedPlacePos.size());
		for(unsigned long i = 0; i < l_pcSimTransition->basedPlacePos.size(); i++)
		{
			long l_nPlacePos = l_pcSimTransition->basedPlacePos[i];
			SP_PairDouble l_nInterval = m_aanPreIncMatrix->GetValue(p_nTransitionArrayPos, l_nPlacePos);
			l_pcSimTransition->intervalLowerBound.push_back(l_nInterval.first);
			l_pcSimTransition->intervalUpperBound.push_back(l_nInterval.second);
		}

		// manipulated places
		for (unsigned long i = 0; i < m_nPlaceCount; i++)
		{
			long l_nDelta = m_aanDeltaIncMatrix->GetValue(p_nTransitionArrayPos, i);
			bool l_bReset = m_aanResetIncMatrix->GetValue(p_nTransitionArrayPos, i);
			if (l_nDelta != 0 || l_bReset)
			{
				l_pcSimTransition->manipulatedPlacePos.push_back(i);
				l_pcSimTransition->deltaMarking.push_back(l_nDelta);
				l_pcSimTransition->resetMarking.push_back(l_bReset);
			}
		}

		l_pcSimTransition->hazardTree = l_pcRoot;

		l_pcSimTransition->hazardFunctionType = l_peHazardFunctionType;

		if( l_peHazardFunctionType == SP_ST_SIM_HAZARD_FUNCTION_BMA_CONSTANT_PARAMETER ||
			l_peHazardFunctionType == SP_ST_SIM_HAZARD_FUNCTION_BMA_DYNAMIC_PARAMETER ||
			l_peHazardFunctionType == SP_ST_SIM_HAZARD_FUNCTION_BIO_LEVEL_INTERPRETATION ||
			l_peHazardFunctionType == SP_ST_SIM_HAZARD_FUNCTION_BIO_MICHAELIS_MENTEN ||
			l_peHazardFunctionType == SP_ST_SIM_HAZARD_FUNCTION_CONSTANT ||
			l_peHazardFunctionType == SP_ST_SIM_HAZARD_FUNCTION_DYNAMIC)
		{
		}
		else if( l_peHazardFunctionType == SP_ST_SIM_HAZARD_FUNCTION_DTF_SINGLE)
		{
			l_pcSimTransition->timeEvent->eventType = SP_ST_SIM_TIME_EVENT_DET_FIRING_SINGLE;

			l_nTimePoint = m_cParser->Evaluate(l_pcRoot->left, l_pcSimTransition);
			l_pcSimTransition->timeEvent->fixedTime = l_nTimePoint;
		}
		else if( l_peHazardFunctionType == SP_ST_SIM_HAZARD_FUNCTION_DTF_PERIODIC_WITHIN_INTERVAL)
		{
			l_pcSimTransition->timeEvent->eventType = SP_ST_SIM_TIME_EVENT_DET_FIRING_PERIODIC;

			l_nIntervalStart = m_cParser->Evaluate(l_pcRoot->left, l_pcSimTransition);

			if (l_nIntervalStart < 0)
			{
				l_nIntervalStart = 0;
			}

			l_nIntervalEnd = m_cParser->Evaluate(l_pcRoot->right, l_pcSimTransition);

			l_nTimePeriod = m_cParser->Evaluate(l_pcRoot->middle, l_pcSimTransition);

			if ( (l_nIntervalStart >= m_nCurrentTime ) )
			{
				l_pcSimTransition->timeEvent->intervalStart = l_nIntervalStart;
				l_pcSimTransition->timeEvent->intervalEnd = l_nIntervalEnd;
				l_pcSimTransition->timeEvent->period = l_nTimePeriod;
			}
		}
		else if( l_peHazardFunctionType == SP_ST_SIM_HAZARD_FUNCTION_TIMED_TRANSITION)
		{
			l_pcSimTransition->timeEvent->eventType = SP_ST_SIM_TIME_EVENT_TIMED_FIRING_SINGLE;

			l_nTimePoint = m_cParser->Evaluate(l_pcRoot->left, l_pcSimTransition);
			l_pcSimTransition->timeEvent->fixedTime = l_nTimePoint;
		}
		else if( l_peHazardFunctionType == SP_ST_SIM_HAZARD_FUNCTION_IMMEDIATE_TRANSITION)
		{
		}
	}
	else
	{

		wxString l_sErrorText =
				wxT("There are some problems in a function of a transition.  \n\n");

		l_sErrorText += wxT("Transition: ");
		l_sErrorText += m_msTransitionNames[ p_nTransitionArrayPos ];
		l_sErrorText += wxT("  \n");

		l_sErrorText += wxT("Function: ");
		l_sErrorText += m_msTransitionFunctions[ p_nTransitionArrayPos ];
		l_sErrorText += wxT("  \n\n-------------\n\n");

		ShowParseError(l_sErrorText);
		return NULL;

	}
	return l_pcSimTransition;
}

void SP_DS_StSimGillespie::ShowParseError(const wxString& p_sNodeType)
{

	wxString l_sVariables = wxT("");

	wxArrayString* l_plMissingVariableList = 0;
	l_plMissingVariableList = m_cParser->GetMissingVariableList();

	l_sVariables = p_sNodeType;
	//  l_sVariables += wxT("Occured problems:\n\n");

	for (unsigned long i = 0; i < l_plMissingVariableList->Count(); i++)
	{

		l_sVariables += wxT("Unknown component: ");
		l_sVariables += ( *l_plMissingVariableList )[ i ];
		l_sVariables += wxT("\n");

	}

	l_sVariables += wxT("\n\n");

	SP_MESSAGEBOX(l_sVariables, wxT("Error"), wxOK | wxICON_ERROR);

}

void SP_DS_StSimGillespie::SetDeltaVector(SP_DS_Node* p_pcTransNode, long p_nTransArrayPos)
{
	SP_MapLong2Long l_mMultEdge;
	SP_MapLong2Long l_mMultReadEdge;
	SP_MapLong2Long l_mMultInhibEdge;

	for (SP_ListEdge::const_iterator l_itEdge = p_pcTransNode->GetTargetEdges()->begin(); l_itEdge != p_pcTransNode->GetTargetEdges()->end(); ++l_itEdge)
	{
		SP_DS_Edge* l_pcEdge = *l_itEdge;
		SP_DS_Node* l_pcSourceNode = dynamic_cast< SP_DS_Node* >(l_pcEdge->GetSource() );
		wxString l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>(l_pcSourceNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
		long l_nPlaceArrayPos = m_mnPlaceArrayPos[ l_sPlaceName ];

		wxString l_sMultiplicity = wxEmptyString;
		long l_nMultiplicity = 0;
		if (l_pcEdge->GetAttribute( wxT("Multiplicity") ))
		{
			l_sMultiplicity = l_pcEdge->GetAttribute( wxT("Multiplicity") )->GetValueString();
			l_sMultiplicity.ToLong( &l_nMultiplicity);
		}
		if (l_pcEdge->GetClassName() == wxT("Edge"))
		{
			l_mMultEdge[l_nPlaceArrayPos] += l_nMultiplicity;
			m_abNonConsumingMode[p_nTransArrayPos] = false;
		}
		else if (l_pcEdge->GetClassName() == wxT("Read Edge"))
		{
			l_mMultReadEdge[l_nPlaceArrayPos] = l_nMultiplicity;
		}
		else if (l_pcEdge->GetClassName() == wxT("Inhibitor Edge"))
		{
			l_mMultInhibEdge[l_nPlaceArrayPos] = l_nMultiplicity;
		}
		else if (l_pcEdge->GetClassName() == wxT("Equal Edge"))
		{
			l_mMultReadEdge[l_nPlaceArrayPos] = l_nMultiplicity;
			l_mMultInhibEdge[l_nPlaceArrayPos] = l_nMultiplicity+1;
		}
		else if (l_pcEdge->GetClassName() == wxT("Reset Edge"))
		{
			m_aanResetIncMatrix->SetValue(p_nTransArrayPos, l_nPlaceArrayPos, true);
		}
		else if (l_pcEdge->GetClassName() == wxT("Modifier Edge"))
		{
			//because the edge has no impact on firing of the transition
		}
	}
	for (SP_ListEdge::const_iterator l_itEdge = p_pcTransNode->GetTargetEdges()->begin(); l_itEdge != p_pcTransNode->GetTargetEdges()->end(); ++l_itEdge)
	{
		SP_DS_Edge* l_pcEdge = *l_itEdge;
		SP_DS_Node* l_pcSourceNode = dynamic_cast< SP_DS_Node* >(l_pcEdge->GetSource() );
		wxString l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>(l_pcSourceNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
		long l_nPlaceArrayPos = m_mnPlaceArrayPos[ l_sPlaceName ];
		long l_nMultEdge = l_mMultEdge[l_nPlaceArrayPos];
		long l_nMultRead = l_mMultReadEdge[l_nPlaceArrayPos];
		long l_nMultInhib = l_mMultInhibEdge[l_nPlaceArrayPos];

		SP_PairDouble l_Interval(0,LONG_MAX);
		l_Interval.first = l_nMultRead > l_nMultEdge ? l_nMultRead : l_nMultEdge;
		l_Interval.second = l_nMultInhib > 0 ? l_nMultInhib : LONG_MAX;
		m_aanPreIncMatrix->SetValue(p_nTransArrayPos, l_nPlaceArrayPos, l_Interval);

		if(m_aanResetIncMatrix->GetValue(p_nTransArrayPos, l_nPlaceArrayPos))
		{
			m_aanDeltaIncMatrix->SetValue(p_nTransArrayPos, l_nPlaceArrayPos, 0);
		}
		else
		{
			m_aanDeltaIncMatrix->SetValue(p_nTransArrayPos, l_nPlaceArrayPos, -1*l_nMultEdge);
		}
	}
	for (SP_ListEdge::const_iterator l_itEdge = p_pcTransNode->GetSourceEdges()->begin(); l_itEdge != p_pcTransNode->GetSourceEdges()->end(); ++l_itEdge)
	{
		SP_DS_Edge* l_pcEdge = *l_itEdge;
		SP_DS_Node* l_pcTargetNode = dynamic_cast< SP_DS_Node* >(l_pcEdge->GetTarget() );

		wxString l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>(l_pcTargetNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
		long l_nPlaceArrayPos = m_mnPlaceArrayPos[ l_sPlaceName ];

		wxString l_sMultiplicity = l_pcEdge->GetAttribute( wxT("Multiplicity") )->GetValueString();
		long l_nMultiplicity = 0;
		l_sMultiplicity.ToLong( &l_nMultiplicity);

		m_aanDeltaIncMatrix->SetValue(p_nTransArrayPos, l_nPlaceArrayPos,
				m_aanDeltaIncMatrix->GetValue(p_nTransArrayPos, l_nPlaceArrayPos) + l_nMultiplicity);
	}
}

void SP_DS_StSimGillespie::Debug_ShowSimTransitions()
{

	SP_DS_StSimTransition* l_pcSimTransition;

	wxString l_sTrans = wxT("");
	wxString l_sAll = wxT("");

	wxString l_sMani = wxT("");

	unsigned long j = 0;

	for (unsigned long i = 0; i < m_nTransitionCount; i++)
	{

		l_pcSimTransition = m_apcSimTransitions[ i ];
		j = 0;

		l_sTrans += wxString::Format(wxT("%d"), i);
		l_sTrans += wxT(": ----  transition: ");
		l_sTrans += m_msTransitionNames[ i ];
		l_sTrans += wxT("  -----       \n\n");

		l_sMani = wxT("");

		while (j < l_pcSimTransition->manipulatedPlacePos.size())
		{

			l_sMani += wxT("to manipulated place: ");
			l_sMani
					+= m_msPlaceNames[ l_pcSimTransition->manipulatedPlacePos[ j ] ];
			l_sMani += wxT("        \n");

			j++;

		}

		l_sTrans += l_sMani;
		l_sTrans += wxT("\n");

		l_sMani = wxT("");
		j = 0;

		while (j < l_pcSimTransition->toUpdateTransitionPos.size())
		{

			l_sMani += wxT("to update transition: ");
			l_sMani
					+= m_msTransitionNames[ l_pcSimTransition->toUpdateTransitionPos[ j ] ];
			l_sMani += wxT("        \n");

			j++;

		}

		l_sTrans += l_sMani;
		l_sTrans += wxT("\n");

		l_sMani = wxT("");
		j = 0;

		while (j < l_pcSimTransition->basedPlacePos.size())
		{

			l_sMani += wxT("based place: ");
			l_sMani += m_msPlaceNames[ l_pcSimTransition->basedPlacePos[ j ] ];
			l_sMani += wxT("        \n");

			j++;

		}

		l_sTrans += l_sMani;
		l_sTrans += wxT("\n\n");

		l_sTrans += wxT("Function-Type: ");
		l_sTrans += GetHazardFunctionTypeAsString(l_pcSimTransition->hazardFunctionType);
		l_sTrans += wxT("\n\n");

		l_sTrans += wxT("Function: ");
		l_sTrans += m_msTransitionFunctions[ i ];
		l_sTrans += wxT("\n\n");
		l_sTrans += wxT("parsed Function: ");
		l_sTrans += m_cParser->GetFormulaString(l_pcSimTransition->hazardTree);
		l_sTrans += wxT("\n\n");


	}

	l_sAll += l_sTrans;

	SP_LOGMESSAGE( l_sAll );
	//SP_MESSAGEBOX( l_sAll, wxT("Debug - SimTransitions"), wxOK | wxICON_INFORMATION );

}

void SP_DS_StSimGillespie::Debug_ShowSimPlaces()
{

	SP_DS_StSimPlace* l_pcSimPlace;

	wxString l_sAll = wxT("");
	wxString l_sTrans = wxT("");

	wxString l_sPlace = wxT("");
	wxString l_sEvents = wxT("");

	unsigned long j = 0;

	for (unsigned long i = 0; i < m_nPlaceCount; i++)
	{

		l_pcSimPlace = m_apcSimPlaces[ i ];
		j = 0;

		l_sPlace += wxString::Format(wxT("%d"), i);
		l_sPlace += wxT(": ----  place: ");
		l_sPlace += m_msPlaceNames[ i ];
		l_sPlace += wxT("  -----                  \n\n");

		while (j < l_pcSimPlace->toUpdateTransitionPos.size())
		{

			l_sTrans += wxT("transition to update: ");
			l_sTrans
					+= m_msTransitionNames[ l_pcSimPlace->toUpdateTransitionPos[ j ] ];
			l_sTrans += wxT("\n");

			j++;

		}

		l_sPlace += l_sTrans;
		l_sPlace += wxT("\n\n");

		l_sEvents = wxT("");
		j = 0;

		l_sPlace += l_sEvents;
		l_sPlace += wxT("\n\n");

		l_sTrans = wxT("");

	}

	l_sAll += l_sPlace;

	SP_LOGMESSAGE( l_sAll );
	//SP_MESSAGEBOX(l_sAll, wxT("Debug"), wxOK | wxICON_INFORMATION);

}

void SP_DS_StSimGillespie::Debug_ShowDeltaMatrix()
{

	wxString l_sAll = wxT("Debug: PreIncMatrix\n");
	wxString l_sTrans = wxT("");

	for (unsigned long i = 0; i < m_nTransitionCount; i++)
	{

		l_sTrans += wxT("  ");
		l_sTrans += m_msTransitionNames[ i ];
		l_sTrans += wxT(":  ");

		for(unsigned long j = 0; j < m_nPlaceCount; j++)
		{

			l_sTrans += wxT("  ");
			l_sTrans += m_msPlaceNames[ j ];

			l_sTrans += wxT(":  ");
			SP_PairDouble l_Interval =  m_aanPreIncMatrix->GetValue(i, j);
			l_sTrans << l_Interval.first << wxT(",") << l_Interval.second;
			//l_sTrans += wxString::Format(wxT("%d"), m_aanPreIncMatrix->GetValue(i, j).first);

		}

		l_sTrans += wxT("\n");

	}

	l_sAll += l_sTrans;

	SP_LOGMESSAGE( l_sAll );

	l_sAll = wxT("Debug: Delta matrix\n");
	l_sTrans = wxT("");

	for (unsigned long i = 0; i < m_nTransitionCount; i++)
	{

		l_sTrans += wxT("  ");
		l_sTrans += m_msTransitionNames[ i ];
		l_sTrans += wxT(":  ");

		for (unsigned long j = 0; j < m_nPlaceCount; j++)
		{

			l_sTrans += wxT("  ");
			l_sTrans += m_msPlaceNames[ j ];

			l_sTrans += wxT(":  ");
			l_sTrans << m_aanDeltaIncMatrix->GetValue(i, j);
			//l_sTrans += wxString::Format(wxT("%d"), m_aanDeltaIncMatrix->GetValue(i, j));

		}

		l_sTrans += wxT("\n");

	}

	l_sAll += l_sTrans;

	SP_LOGMESSAGE( l_sAll );

}


void SP_DS_StSimGillespie::DirectSingleExactExport()
{
	if(m_nLastTime != m_nCurrentTime)
	{
		if(m_nIntervalStart <= m_nLastTime)
		{
			m_pcSimulationResultsDlg->DirectSingleExactExportToCSV(m_nCurrentSimulationRunNumber,
																m_nLastTime,
																m_anLastMarking,
																m_anLastHazardValues);
		}
		else if(m_nIntervalStart <= m_nCurrentTime)
		{
			m_pcSimulationResultsDlg->DirectSingleExactExportToCSV(m_nCurrentSimulationRunNumber,
																m_nIntervalStart,
																m_anLastMarking,
																m_anLastHazardValues);
		}
	}

	std::copy(m_anCurrentMarking.begin(), m_anCurrentMarking.end(), m_anLastMarking.begin());

	std::copy(m_anHazardValues.begin(), m_anHazardValues.end(), m_anLastHazardValues.begin());
}

void SP_DS_StSimGillespie::WriteResults()
{
	SP_Vector2DDouble::iterator l_itResults = m_aanResults.begin();
	SP_Vector2DDouble::iterator l_itResultsSD = m_aanResultsSD.begin();
	SP_Vector2DDouble::iterator l_itResultFireCount = m_aanResultTransitionFireCount.begin();
	for(; l_itResults != m_aanResults.end(); ++l_itResults, ++l_itResultsSD, ++l_itResultFireCount)
	{
		SP_VectorDouble::iterator l_itR = l_itResults->begin();
		SP_VectorDouble::iterator l_itRSD = l_itResultsSD->begin();
		for(; l_itR != l_itResults->end(); ++l_itR, ++l_itRSD)
		{
			*l_itRSD = sqrt((*l_itRSD - pow(*l_itR, 2.0)/m_nCurrentSimulationRunNumber)/(m_nCurrentSimulationRunNumber-1));
			*l_itR /= m_nCurrentSimulationRunNumber;
		}

		SP_VectorDouble::iterator l_itRFC = l_itResultFireCount->begin();
		for(; l_itRFC != l_itResultFireCount->end(); ++l_itRFC)
		{
			*l_itRFC /= m_nCurrentSimulationRunNumber;
		}
	}
	m_pcSimulationResultsDlg->LoadResults();

	for(size_t i = 0; i < m_aApFormulas.size(); ++i)
	{
		double l_nValue = m_aApResults[i];
		double l_nMean = l_nValue / m_nCurrentSimulationRunNumber;
		double l_nVariance = (l_nValue - pow(l_nValue, 2.0)/m_nCurrentSimulationRunNumber)/(m_nCurrentSimulationRunNumber-1);
		wxString l_sOutput = wxT("Result of formula ");
		l_sOutput << wxString(m_aApFormulas[i].GetExpressionTree().toString().c_str(), wxConvUTF8);
		l_sOutput << wxT(" at interval [") << m_nIntervalStart << wxT(", ") << m_nIntervalEnd << wxT("]");
		SP_LOGMESSAGE( l_sOutput);
		l_sOutput = wxT("runs: ");
		l_sOutput << m_nCurrentSimulationRunNumber;
		long l_nTime = m_cSimulationStopWatch.Time();
		long l_nTimeSec = l_nTime/1000;
		long l_nTimeMin = l_nTimeSec/60;
		long l_nTimeHour = l_nTimeMin/60;
		l_sOutput << wxT("; runtime: ") << l_nTimeHour << wxT("h ") << (l_nTimeMin - l_nTimeHour*60) << wxT("min ") << (l_nTimeSec - l_nTimeMin*60) << wxT("sec");
		l_sOutput << wxT("; threads: ") << m_nThreadCount;
		SP_LOGMESSAGE( l_sOutput);
		l_sOutput = wxT("Prop: ");
		l_sOutput << l_nMean;
		l_sOutput << wxT("; S^2: ") << l_nVariance;
		SP_LOGMESSAGE( l_sOutput);
		double l_nConfInterval = 1.96 * sqrt(l_nVariance/m_nCurrentSimulationRunNumber);
		l_sOutput = wxT("Confidence interval (beta = 0.95): ");
		l_sOutput << wxT("+- ") << l_nConfInterval;
		l_sOutput << wxT(" [ ") << (l_nMean - l_nConfInterval) << wxT(", ");
		l_sOutput << (l_nMean + l_nConfInterval) << wxT(" ]");
		SP_LOGMESSAGE( l_sOutput);
	}

}

void SP_DS_StSimGillespie::SetToUpdateTransitionPosOfPlaces(long p_nPlaceNumber)
{
	SP_SetLong l_snTransitionArrayPos;
	SP_SetLong::iterator l_iter;

	unsigned long j = 0;

	for (unsigned long i = 0; i < m_nTransitionCount; i++)
	{
		j = 0;

		while (j < m_apcSimTransitions[ i ]->basedPlacePos.size())
		{
			if (m_apcSimTransitions[ i ]->basedPlacePos[ j ] == p_nPlaceNumber)
			{
				l_snTransitionArrayPos.insert(i);
			}
			j++;
		}
	}

	m_apcSimPlaces[ p_nPlaceNumber ]->toUpdateTransitionPos.reserve(l_snTransitionArrayPos.size());
	for (l_iter = l_snTransitionArrayPos.begin(); l_iter != l_snTransitionArrayPos.end(); l_iter++)
	{
		m_apcSimPlaces[ p_nPlaceNumber ]->toUpdateTransitionPos.push_back( *l_iter );
	}

}

void SP_DS_StSimGillespie::SetToUpdateTransitionPosOfTransitions(long p_nTransNumber)
{
	SP_DS_StSimTransition* l_pcTrans = m_apcSimTransitions[ p_nTransNumber ];
	SP_SetLong l_snTransitionArrayPos;

	for(unsigned long i = 0; i < l_pcTrans->manipulatedPlacePos.size(); i++)
	{
		SP_DS_StSimPlace* l_pcPlace = m_apcSimPlaces[l_pcTrans->manipulatedPlacePos[i]];
		for(unsigned long j = 0; j < l_pcPlace->toUpdateTransitionPos.size(); j++)
		{
			l_snTransitionArrayPos.insert(l_pcPlace->toUpdateTransitionPos[j]);
		}
	}
	SP_SetLong::iterator l_iter;
	l_pcTrans->toUpdateTransitionPos.reserve(l_snTransitionArrayPos.size());
	for (l_iter = l_snTransitionArrayPos.begin(); l_iter != l_snTransitionArrayPos.end(); l_iter++)
	{
		l_pcTrans->toUpdateTransitionPos.push_back( *l_iter );
	}
}

wxString SP_DS_StSimGillespie::GetHazardFunctionTypeAsString(
		SP_DS_StSimHazardFunctionType p_eType)
{

	if(p_eType == SP_ST_SIM_HAZARD_FUNCTION_DYNAMIC)

		return wxT("SP_ST_SIM_HAZARD_FUNCTION_DYNAMIC");

	if(p_eType ==  SP_ST_SIM_HAZARD_FUNCTION_CONSTANT)

		return wxT("SP_ST_SIM_HAZARD_FUNCTION_CONSTANT");

	if(p_eType == SP_ST_SIM_HAZARD_FUNCTION_BMA_CONSTANT_PARAMETER)

		return wxT("SP_ST_SIM_HAZARD_FUNCTION_BMA_CONSTANT_PARAMETER");

	if(p_eType ==  SP_ST_SIM_HAZARD_FUNCTION_BMA_DYNAMIC_PARAMETER)

		return wxT("SP_ST_SIM_HAZARD_FUNCTION_BMA_DYNAMIC_PARAMETER");

	if(p_eType ==  SP_ST_SIM_HAZARD_FUNCTION_BIO_LEVEL_INTERPRETATION)

		return wxT("SP_ST_SIM_HAZARD_FUNCTION_BIO_LEVEL_INTERPRETATION");

	if(p_eType ==  SP_ST_SIM_HAZARD_FUNCTION_BIO_MICHAELIS_MENTEN)

		return wxT("SP_ST_SIM_HAZARD_FUNCTION_BIO_MICHAELIS_MENTEN");

	if(p_eType ==  SP_ST_SIM_HAZARD_FUNCTION_DTF_SINGLE)

		return wxT("SP_ST_SIM_HAZARD_FUNCTION_DTF_SINGLE");

	if(p_eType ==  SP_ST_SIM_HAZARD_FUNCTION_DTF_PERIODIC_WITHIN_INTERVAL)

		return wxT("SP_ST_SIM_HAZARD_FUNCTION_DTF_PERIODIC_WITHIN_INTERVAL");

	if(p_eType ==  SP_ST_SIM_HAZARD_FUNCTION_IMMEDIATE_TRANSITION)

		return wxT("SP_ST_SIM_HAZARD_FUNCTION_IMMEDIATE_TRANSITION");

	if(p_eType ==  SP_ST_SIM_HAZARD_FUNCTION_TIMED_TRANSITION)

		return wxT("SP_ST_SIM_HAZARD_FUNCTION_TIMED_TRANSITION");


	return wxT("unknown");

}

void SP_DS_StSimGillespie::CheckImmediateTransitions()
{

	while (m_pImmediateTransitionList->GetElemCount() > 0 && IsSimulationRunning())
	{
		UpdateSimulationDialog(m_nCurrentSimulationRunNumber);
		long l_nTransNumber = m_pImmediateTransitionList->ChooseTransitionByWeight();
		FireTransition(l_nTransNumber);
		CalculateHazardValue(l_nTransNumber);
	}

}

void SP_DS_StSimGillespie::InitialiseTimeline()
{
	SP_DS_StSimTransition* l_pcSimTransition;

	m_qpcSimTimeEventTimeline->Clear();

	for (unsigned long i = 0; i < m_nTransitionCount; i++)
	{
		l_pcSimTransition = m_apcSimTransitions[ i ];

		if(l_pcSimTransition->hazardFunctionType == SP_ST_SIM_HAZARD_FUNCTION_DTF_SINGLE)
		{
			double l_nTimePoint = l_pcSimTransition->timeEvent->fixedTime;

			if(l_nTimePoint >= m_nCurrentTime)
			{
				m_qpcSimTimeEventTimeline->InsertTimeEvent(l_pcSimTransition->timeEvent, l_nTimePoint);
			}

		}
		else if(l_pcSimTransition->hazardFunctionType == SP_ST_SIM_HAZARD_FUNCTION_DTF_PERIODIC_WITHIN_INTERVAL)
		{
			double l_nIntervalStart = l_pcSimTransition->timeEvent->intervalStart;

			if(l_nIntervalStart >= m_nCurrentTime)
			{
				m_qpcSimTimeEventTimeline->InsertTimeEvent(l_pcSimTransition->timeEvent, l_nIntervalStart);
			}
		}
	}
}

void
SP_DS_StSimGillespie::CheckApFormulae(SP_DS_StSimTransition* p_pcSimTrans)
{
	if(p_pcSimTrans != NULL)
	{
		if(p_pcSimTrans->hazardFunctionType == SP_ST_SIM_HAZARD_FUNCTION_IMMEDIATE_TRANSITION)
		{
			return;
		}
		if(p_pcSimTrans->hazardFunctionType == SP_ST_SIM_HAZARD_FUNCTION_TIMED_TRANSITION)
		{
			SP_DS_StSimTimeEvent* l_pcTimeEvent = p_pcSimTrans->timeEvent;
			if(l_pcTimeEvent->fixedTime == 0)
			{
				return;
			}
		}
		if((p_pcSimTrans->hazardFunctionType == SP_ST_SIM_HAZARD_FUNCTION_DTF_PERIODIC_WITHIN_INTERVAL
			|| p_pcSimTrans->hazardFunctionType == SP_ST_SIM_HAZARD_FUNCTION_DTF_SINGLE)
				&& m_nCurrentTime == 0)
		{
			return;
		}
	}
	if((m_nLastTime <= m_nIntervalStart && m_nIntervalStart <= m_nCurrentTime)
		|| (m_nIntervalStart <= m_nLastTime && m_nCurrentTime <= m_nIntervalEnd)
		|| (m_nLastTime <= m_nIntervalEnd && m_nIntervalEnd <= m_nCurrentTime))
	{
		for(size_t i = 0; i < m_aApFormulas.size(); ++i)
		{
			if(!m_aApFormulaHappend[i])
			{
				double l_nResult = m_aApFormulas[i].Evaluate(&m_anCurrentMarking);
				if(l_nResult > 0)
				{
					m_aApFormulaHappend[i] = true;
					m_aApResults[i] += l_nResult;
				}
			}
		}
	}
}

void SP_DS_StSimGillespie::SetMemoryFree()
{
/*
	 MEMORYSTATUS MemState;
	 MemState.dwLength = sizeof( MEMORYSTATUS );
	 GlobalMemoryStatus( &MemState );

	 SP_LOGDEBUG(wxT("Memorystatus before: ") + wxString::Format( wxT("%d"), MemState.dwMemoryLoad ));
*/
	m_mnPlaceArrayPos.clear();
	m_msPlaceNames.clear();
	m_msPlaceNamesById.clear();

	m_mpcPlaceNodes.clear();

	m_mnTransitionArrayPos.clear();
	m_msTransitionNames.clear();
	m_msTransitionNamesById.clear();

	m_msTransitionFunctions.clear();
	m_mpcTransitionNodes.clear();

	m_anCurrentFireCount.clear();
	m_anCurrentMarking.clear();
	m_anLastMarking.clear();
	m_anHazardValues.clear();
	m_anLastHazardValues.clear();

	m_aanResults.clear();
	m_aanResultsSD.clear();
	m_aanSingleResults.clear();

	m_aApFormulaHappend.clear();
	m_aApFormulas.clear();
	m_aApResults.clear();
	m_sApFormulas.clear();

	m_aanResultTransitionFireCount.clear();
	m_aanSingleTransitionFireCount.clear();
/*
	if( m_pcGraph->GetNetclass()->GetName() !=  SP_DS_COLSPN_CLASS)
	{
		wxDELETE(m_aanDeltaIncMatrix);
		wxDELETE(m_aanPreIncMatrix);
		wxDELETE(m_aanResetIncMatrix);
	}
*/

	if(m_aanDeltaIncMatrix)
		wxDELETE(m_aanDeltaIncMatrix);
	if(m_aanPreIncMatrix)
		wxDELETE(m_aanPreIncMatrix);
	if(m_aanResetIncMatrix)
		wxDELETE(m_aanResetIncMatrix);


	SetMemoryFreeTransitions();
	SetMemoryFreePlaces();

/*
	 MEMORYSTATUS MemState1;
	 MemState1.dwLength = sizeof( MEMORYSTATUS );
	 GlobalMemoryStatus( &MemState1 );

	 SP_LOGDEBUG(wxT("Memorystatus after: ") + wxString::Format( wxT("%d"), MemState1.dwMemoryLoad));
*/
}

void SP_DS_StSimGillespie::SetMemoryFreeTransitions()
{
	for (unsigned long i = 0; i < m_apcSimTransitions.size(); i++)
	{
		SP_DS_StSimTransition* l_pcSimTrans = m_apcSimTransitions[ i ];
		wxDELETE(l_pcSimTrans);
	}
	m_apcSimTransitions.clear();
}

void SP_DS_StSimGillespie::SetMemoryFreePlaces()
{
	for (unsigned long i = 0; i < m_apcSimPlaces.size(); i++)
	{
		SP_DS_StSimPlace* l_pcSimPlace = m_apcSimPlaces[ i ];
		wxDELETE(l_pcSimPlace);
	}
	m_apcSimPlaces.clear();
}

SP_DS_StSimGillespie::~SP_DS_StSimGillespie()
{

	SetMemoryFree();

	wxDELETE(m_pImmediateTransitionList);
	wxDELETE(m_qpcSimTimeEventTimeline);

	wxDELETE(m_cParser);

	
	wxDELETE(m_pcApParser);
}



void SP_DS_StSimGillespie::LoadData()
{
	m_nPlaceCount = m_mnPlaceArrayPos.size();
	m_nTransitionCount = m_mnTransitionArrayPos.size();

	//load current marking
	m_anInitialNetMarking.clear();
	m_anInitialNetMarking  = *(m_pcUnfoldedNet->GetCurColStMarking());

	//load current rate functions
	m_msTransitionFunctions.clear();
	m_msTransitionFunctions  = *(m_pcUnfoldedNet->GetCurRateFunction());


/*
	
	m_anInitialNetMarking.resize(m_nPlaceCount);

	SP_DS_ColListAttribute* l_pcColList;
	unsigned l_nCurrentColumn;

	SP_DS_Nodeclass* l_pcNodeclass;
	l_pcNodeclass= m_pcGraph->GetNodeclass(SP_DS_DISCRETE_PLACE);
	if(l_pcNodeclass->GetElements()->size() > 0)
	{
		l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >((*(l_pcNodeclass->GetElements()->begin()))->GetAttribute(wxT("MarkingList")));
		l_nCurrentColumn =	l_pcColList->GetActiveColumn();

		for( unsigned i = 0; i < m_anNetMarkings.size(); i++)
		{
			m_anInitialNetMarking[i] = m_anNetMarkings[i][l_nCurrentColumn];
		}
	}

	//load rate functions
	m_nTransitionCount = m_mnTransitionArrayPos.size();
	m_msTransitionFunctions.resize(m_nTransitionCount);

	for( unsigned i = 0; i < m_nTransitionCount; i++)
	{
		m_msTransitionFunctions[i] = wxT("0");
	}

	l_pcNodeclass= m_pcGraph->GetNodeclass(wxT("Transition"));
	if(l_pcNodeclass->GetElements()->size() > 0)
	{

		l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >((*(l_pcNodeclass->GetElements()->begin()))->GetAttribute(wxT("FunctionList")));
		l_nCurrentColumn =	l_pcColList->GetActiveColumn();

		

		if(m_nsTransitionCountByNodeClass.find(wxT("Transition")) != m_nsTransitionCountByNodeClass.end())
		{
			for(unsigned int i = m_nsTransitionCountByNodeClass.find(wxT("Transition"))->second.m_nLow ; i <= m_nsTransitionCountByNodeClass.find(wxT("Transition"))->second.m_nUp; i++ )
			{
				m_msTransitionFunctions[i] = m_anNetFunctions[i][l_nCurrentColumn];
			}
		}
	}


	l_pcNodeclass= m_pcGraph->GetNodeclass(wxT("Immediate Transition"));
	if(l_pcNodeclass->GetElements()->size() > 0)
	{
		l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >((*(l_pcNodeclass->GetElements()->begin()))->GetAttribute(wxT("FunctionList")));
		l_nCurrentColumn =	l_pcColList->GetActiveColumn();

		if(m_nsTransitionCountByNodeClass.find(wxT("Immediate Transition")) != m_nsTransitionCountByNodeClass.end())
		{
			for(unsigned int i = m_nsTransitionCountByNodeClass.find(wxT("Immediate Transition"))->second.m_nLow ; i <= m_nsTransitionCountByNodeClass.find(wxT("Immediate Transition"))->second.m_nUp; i++ )
			{
				m_msTransitionFunctions[i] = m_anNetFunctions[i][l_nCurrentColumn];
			}
		}
	}

	l_pcNodeclass= m_pcGraph->GetNodeclass(wxT("Deterministic Transition"));
	if(l_pcNodeclass->GetElements()->size() > 0)
	{
		l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >((*(l_pcNodeclass->GetElements()->begin()))->GetAttribute(wxT("DelayList")));
		l_nCurrentColumn =	l_pcColList->GetActiveColumn();

		if(m_nsTransitionCountByNodeClass.find(wxT("Deterministic Transition")) != m_nsTransitionCountByNodeClass.end())
		{
			for(unsigned int i = m_nsTransitionCountByNodeClass.find(wxT("Deterministic Transition"))->second.m_nLow ; i <= m_nsTransitionCountByNodeClass.find(wxT("Deterministic Transition"))->second.m_nUp; i++ )
			{
				m_msTransitionFunctions[i] = m_anNetFunctions[i][l_nCurrentColumn];
			}
		}
	}

	l_pcNodeclass= m_pcGraph->GetNodeclass(wxT("Scheduled Transition"));
	if(l_pcNodeclass->GetElements()->size() > 0)
	{
		l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >((*(l_pcNodeclass->GetElements()->begin()))->GetAttribute(wxT("PeriodicList")));
		l_nCurrentColumn =	l_pcColList->GetActiveColumn();

		if(m_nsTransitionCountByNodeClass.find(wxT("Scheduled Transition")) != m_nsTransitionCountByNodeClass.end())
		{
			for(unsigned int i = m_nsTransitionCountByNodeClass.find(wxT("Scheduled Transition"))->second.m_nLow ; i <= m_nsTransitionCountByNodeClass.find(wxT("Scheduled Transition"))->second.m_nUp; i++ )
			{
				m_msTransitionFunctions[i] = m_anNetFunctions[i][l_nCurrentColumn];
			}
		}
	}

*/

}


/**
 * Simulation Thread Implementation
 */
#if 1

void SP_DS_StSimGillespie::ThreadFinished()
{
	wxCriticalSectionLocker locker(m_critsect);
	++m_nThreadFinished;
}

void SP_DS_StSimGillespie::ThreadAddResult(const SP_Vector2DDouble& p_Result)
{
	wxCriticalSectionLocker locker(m_critsect);

	AddToMatrix(m_aanResults, p_Result);
}

void SP_DS_StSimGillespie::ThreadAddResultSD(const SP_Vector2DDouble& p_Result)
{
	wxCriticalSectionLocker locker(m_critsect);

	AddToMatrix(m_aanResultsSD, p_Result);
}

void SP_DS_StSimGillespie::ThreadAddFireCount(const SP_Vector2DDouble& p_FireCount)
{
	wxCriticalSectionLocker locker(m_critsect);

	AddToMatrix(m_aanResultTransitionFireCount, p_FireCount);
}

void SP_DS_StSimGillespie::AddToMatrix(SP_Vector2DDouble& p_LHS, const SP_Vector2DDouble& p_RHS)
{
	if(p_LHS.size() == p_RHS.size())
	{
		SP_Vector2DDouble::iterator l_itLHS = p_LHS.begin();
		SP_Vector2DDouble::const_iterator l_itRHS = p_RHS.begin();

		while(l_itLHS != p_LHS.end())
		{
			AddToVector(*l_itLHS, *l_itRHS);
			++l_itLHS;
			++l_itRHS;
		}
	}
}

void SP_DS_StSimGillespie::AddToVector(SP_VectorDouble& p_LHS, const SP_VectorDouble& p_RHS)
{
	if(p_LHS.size() == p_RHS.size())
	{
		SP_VectorDouble::iterator l_itLHS = p_LHS.begin();
		SP_VectorDouble::const_iterator l_itRHS = p_RHS.begin();
		while(l_itLHS != p_LHS.end())
		{
			*l_itLHS += *l_itRHS;
			++l_itLHS;
			++l_itRHS;
		}
	}

}

void SP_DS_StSimGillespie::ThreadAddApResult(const SP_VectorDouble& p_Result)
{
	wxCriticalSectionLocker locker(m_critsect);

	AddToVector(m_aApResults, p_Result);
}


SP_DS_StSimGillespie::SimulationThread::SimulationThread(unsigned long p_nThreadId,
								SP_DS_StSimGillespie* p_pcSimulation,
								SP_DS_Graph* p_pcGraph,
								double p_nIntervalStart,
								double p_nIntervalEnd,
								double p_nIntervalSize,
								unsigned long p_nResultPointCount,
								unsigned long p_nPlaceCount,
								unsigned long p_nTransitionCount,
								SP_VectorString p_msTransitionFunctions,
								SP_VectorSimPlace p_apcSimPlaces,
						    	SP_VectorSimTransition p_apcSimTransitions,
						    	SP_VectorLong p_anInitialNetMarking,
						    	SP_MatrixPairDouble* p_aanPreIncMatrix,
						    	unsigned long p_nSimulationRunCount,
						    	SP_VectorApFormula p_aApFormulae):
wxThread(wxTHREAD_JOINABLE),
m_nThreadId(p_nThreadId),
m_pcSimulation(p_pcSimulation),
m_pcGraph(p_pcGraph),
m_nIntervalStart(p_nIntervalStart),
m_nIntervalEnd(p_nIntervalEnd),
m_nIntervalSize(p_nIntervalSize),
m_nResultPointCount(p_nResultPointCount),
m_nPlaceCount(p_nPlaceCount),
m_nTransitionCount(p_nTransitionCount),
m_msTransitionFunctions(p_msTransitionFunctions),
m_apcSimPlaces(p_apcSimPlaces),
m_apcSimTransitions(p_apcSimTransitions),
m_anInitialNetMarking(p_anInitialNetMarking),
m_aanPreIncMatrix(p_aanPreIncMatrix),
m_nSimulationRunCount(p_nSimulationRunCount),
m_nCurrentSimulationRunCount(0),
m_aApFormulas(p_aApFormulae)
{
	m_pcMTRand = new MTRand(rand());
	m_pImmediateTransitionList = new SP_DS_StSimImmediateTransitionList();
	m_qpcSimTimeEventTimeline = new SP_DS_StSimTimeEventTimeline();

	m_cParser = new SP_DS_StParser(m_pcGraph);

	m_cParser->SetCurrentMarking(&m_anCurrentMarking);
	m_cParser->SetSimTime(&m_nCurrentTime);
	m_cParser->SetSimIntervalStart(m_nIntervalStart);
	m_cParser->SetSimIntervalEnd(m_nIntervalEnd);
	m_cParser->SetPreIncMatrix(m_aanPreIncMatrix);

	if( m_pcGraph->GetNetclass()->GetName() == SP_DS_COLSPN_CLASS )
	{
		m_cParser->SetPlaceArrayPos(m_pcSimulation->GetPlaceArrayPos());
		m_cParser->SetPrePlaceArrayPosOfTransitionNode(m_pcSimulation->GetPrePlaceArrayPosOfTransitionNode());		
	}

	for(SP_VectorSimTransition::iterator l_it = m_apcSimTransitions.begin();
		l_it != m_apcSimTransitions.end();
		++l_it)
	{
		SP_DS_StSimTransition* l_pcSimTrans = new SP_DS_StSimTransition(**l_it);

		SP_DS_StParserNode* l_pcRoot = 0;
		SP_DS_StSimHazardFunctionType l_peHazardFunctionType;

		m_cParser->ParseFormulaFunction(m_msTransitionFunctions[l_pcSimTrans->arrayIndex],
										l_pcSimTrans->arrayIndex,
										l_pcRoot,
										&l_peHazardFunctionType);
		l_pcSimTrans->hazardTree = l_pcRoot;
		*l_it = l_pcSimTrans;
	}

	m_aApFormulaHappend.assign(m_aApFormulas.size(), false);
	m_aApResults.assign(m_aApFormulas.size(), 0);
}

SP_DS_StSimGillespie::SimulationThread::~SimulationThread()
{
	wxDELETE(m_pcMTRand);
	wxDELETE(m_pImmediateTransitionList);
	wxDELETE(m_qpcSimTimeEventTimeline);
	wxDELETE(m_cParser);

	for(SP_VectorSimTransition::iterator l_it = m_apcSimTransitions.begin();
		l_it != m_apcSimTransitions.end();
		++l_it)
	{
		SP_DS_StSimTransition* l_pcSimTrans = *l_it;
		wxDELETE(l_pcSimTrans);
	}

}

void* SP_DS_StSimGillespie::SimulationThread::Entry()
{
	if (m_nResultPointCount > 0)
	{
		m_aanResults.assign(m_nResultPointCount+2, SP_VectorDouble(m_nPlaceCount, 0));
		m_aanResultsSD.assign(m_nResultPointCount+2, SP_VectorDouble(m_nPlaceCount, 0));
		m_aanResultTransitionFireCount.assign(m_nResultPointCount+2, SP_VectorDouble(m_nTransitionCount, 0));
	}
	m_nCurrentSimulationRunCount = 0;
	while(m_nCurrentSimulationRunCount < m_nSimulationRunCount
			&& m_pcSimulation->IsSimulationRunning())
	{
		SimulateRun();
		++m_nCurrentSimulationRunCount;
	}

	//write result
	m_pcSimulation->ThreadAddResult(m_aanResults);
	m_pcSimulation->ThreadAddResultSD(m_aanResultsSD);
	m_pcSimulation->ThreadAddFireCount(m_aanResultTransitionFireCount);
	m_pcSimulation->ThreadAddApResult(m_aApResults);

	m_pcSimulation->ThreadFinished();

	return NULL;
}

void SP_DS_StSimGillespie::SimulationThread::SimulateRun()
{
	m_nCurrentTime = 0.0;
	m_nLastTime = 0.0;
	double l_nOutputTime = 0;

	m_nCombinedHazardValue = 0;

	long l_nChosenTransition = 0;

	unsigned long l_nCurrentResultPoint = 0;

	m_anCurrentMarking = m_anInitialNetMarking;
	m_anCurrentFireCount.assign(m_nTransitionCount, 0);
	m_anHazardValues.assign(m_nTransitionCount, 0);

	std::fill(m_aApFormulaHappend.begin(), m_aApFormulaHappend.end(), false);

	InitialiseTimeline();
	CalculateAllHazardValues();
	CheckImmediateTransitions();

	CheckTimeEventTimeLine(l_nOutputTime, l_nCurrentResultPoint);

	GenerateResultPoints(l_nOutputTime, l_nCurrentResultPoint);

	while ((m_nCurrentTime < m_nIntervalEnd) && m_pcSimulation->IsSimulationRunning())
	{
		m_nCurrentTime += GenerateRandomVariableExpDistr(m_nCombinedHazardValue);

		CheckTimeEventTimeLine(l_nOutputTime, l_nCurrentResultPoint);

		GenerateResultPoints(l_nOutputTime, l_nCurrentResultPoint);

		if (m_nCurrentTime < m_nIntervalEnd)
		{
			l_nChosenTransition	= GenerateRandomVariableDiscrete(m_nCombinedHazardValue);

			FireTransition(l_nChosenTransition);
			CheckImmediateTransitions();
		}
	}

	CheckApFormulae(NULL);

	l_nOutputTime = m_nIntervalEnd;
	GenerateResultPoints(l_nOutputTime, l_nCurrentResultPoint);
}

double SP_DS_StSimGillespie::SimulationThread::GenerateRandomVariableExpDistr(double p_nLambda)
{
	if(m_nThreadId == 0 && p_nLambda == 0)
	{
		wxString l_Msg = wxT("dead state detected at t=");
		l_Msg << m_nCurrentTime;
		wxMutexGuiEnter();
		SP_LOGMESSAGE( l_Msg);
		wxMutexGuiLeave();
	}
	double l_nRandom = m_pcMTRand->rand();

	return -1 * log(l_nRandom) / p_nLambda;
}

long SP_DS_StSimGillespie::SimulationThread::GenerateRandomVariableDiscrete(double p_nSum)
{
	while(true)
	{
		double l_nU = p_nSum * m_pcMTRand->rand();
		double l_nRunningSum = 0;
		SP_VectorDouble::const_iterator l_it;
		long l_nTransitionPos = 0;
		for(l_it = m_anHazardValues.begin(); l_it != m_anHazardValues.end(); ++l_it)
		{
			l_nRunningSum += *l_it;
			if (l_nRunningSum >= l_nU  && *l_it > 0)
			{
				return l_nTransitionPos;
			}
			++l_nTransitionPos;
		}
	}
	return -1;
}

void SP_DS_StSimGillespie::SimulationThread::FireTransition(long p_nTransNumber)
{
	if(p_nTransNumber < 0)
	{
		wxString l_sMsg = wxT("error, wrong transition number ");
		l_sMsg << p_nTransNumber;
		wxMutexGuiEnter();
		SP_LOGERROR( l_sMsg);
		wxMutexGuiLeave();
	}
	else
	{
		SP_DS_StSimTransition* l_pcSimTransition = m_apcSimTransitions[p_nTransNumber];

		CheckApFormulae(l_pcSimTransition);

		for(unsigned long i = 0; i < l_pcSimTransition->manipulatedPlacePos.size(); i++)
		{
			long l_nPlacePos = l_pcSimTransition->manipulatedPlacePos[i];
			if(l_pcSimTransition->resetMarking[i])
			{
				m_anCurrentMarking[l_nPlacePos] = 0;
			}
			m_anCurrentMarking[l_nPlacePos] += l_pcSimTransition->deltaMarking[i];
		}

		m_anCurrentFireCount[p_nTransNumber] += 1;

		if(m_nLastTime != m_nCurrentTime)
		{
			m_nLastTime = m_nCurrentTime;
		}

		for(unsigned long i = 0; i < l_pcSimTransition->toUpdateTransitionPos.size(); ++i)
		{
			CalculateHazardValue(l_pcSimTransition->toUpdateTransitionPos[i]);
		}
	}
}

void SP_DS_StSimGillespie::SimulationThread::CalculateHazardValue(long p_nTransitionNumber)
{
	SP_DS_StSimTransition* l_pcSimTransition = m_apcSimTransitions[ p_nTransitionNumber ];

	if ( !IsTransitionEnabled(p_nTransitionNumber) )
	{
		if(l_pcSimTransition->hazardFunctionType == SP_ST_SIM_HAZARD_FUNCTION_TIMED_TRANSITION)
		{
			m_qpcSimTimeEventTimeline->RemoveElem(l_pcSimTransition->timeEvent);
		}
		else if(l_pcSimTransition->hazardFunctionType == SP_ST_SIM_HAZARD_FUNCTION_IMMEDIATE_TRANSITION)
		{
			m_pImmediateTransitionList->RemoveElem(p_nTransitionNumber);
		}
		if(m_anHazardValues[p_nTransitionNumber] > 0)
		{
			m_nCombinedHazardValue -= m_anHazardValues[p_nTransitionNumber];
			m_anHazardValues[p_nTransitionNumber] = 0;
			if(m_nCombinedHazardValue < 0)
			{
				m_nCombinedHazardValue = 0;
			}
		}
		return;
	}
	if( l_pcSimTransition->hazardFunctionType == SP_ST_SIM_HAZARD_FUNCTION_BMA_CONSTANT_PARAMETER ||
		l_pcSimTransition->hazardFunctionType == SP_ST_SIM_HAZARD_FUNCTION_BMA_DYNAMIC_PARAMETER ||
		l_pcSimTransition->hazardFunctionType == SP_ST_SIM_HAZARD_FUNCTION_BIO_LEVEL_INTERPRETATION ||
		l_pcSimTransition->hazardFunctionType == SP_ST_SIM_HAZARD_FUNCTION_BIO_MICHAELIS_MENTEN ||
		l_pcSimTransition->hazardFunctionType == SP_ST_SIM_HAZARD_FUNCTION_DYNAMIC ||
		l_pcSimTransition->hazardFunctionType == SP_ST_SIM_HAZARD_FUNCTION_CONSTANT)
	{
		double l_nHazardValue = m_cParser->Evaluate(l_pcSimTransition->hazardTree,
													l_pcSimTransition);
		if(l_nHazardValue < 0)
		{
			l_nHazardValue = 0;
		}

		m_nCombinedHazardValue -= m_anHazardValues[p_nTransitionNumber];
		m_nCombinedHazardValue += l_nHazardValue;
		m_anHazardValues[p_nTransitionNumber] = l_nHazardValue;

		if(m_nCombinedHazardValue < 0)
		{
			m_nCombinedHazardValue = 0;
		}
	}
	else if(l_pcSimTransition->hazardFunctionType == SP_ST_SIM_HAZARD_FUNCTION_IMMEDIATE_TRANSITION)
	{
		double l_nHazardValue = m_cParser->Evaluate(l_pcSimTransition->hazardTree,
													l_pcSimTransition);
		if(l_nHazardValue < 0)
		{
			l_nHazardValue = 0;
		}
		m_pImmediateTransitionList->InsertImmediateTransition(p_nTransitionNumber, l_nHazardValue);
	}
	else if(l_pcSimTransition->hazardFunctionType == SP_ST_SIM_HAZARD_FUNCTION_TIMED_TRANSITION)
	{
		double l_nTime = m_cParser->Evaluate(l_pcSimTransition->hazardTree->left, l_pcSimTransition);
		l_pcSimTransition->timeEvent->fixedTime = l_nTime;
		m_qpcSimTimeEventTimeline->InsertTimeEvent(l_pcSimTransition->timeEvent,
							m_nCurrentTime + l_nTime);
	}
}

bool SP_DS_StSimGillespie::SimulationThread::IsTransitionEnabled( long p_nTransitionNumber )
{
	SP_DS_StSimTransition* l_pcSimTransition = m_apcSimTransitions[p_nTransitionNumber];
	for (unsigned long i = 0; i < l_pcSimTransition->basedPlacePos.size(); i++)
	{
		long l_nPlacePos = l_pcSimTransition->basedPlacePos[i];
		if( m_anCurrentMarking[ l_nPlacePos ] < l_pcSimTransition->intervalLowerBound[i] ||
			m_anCurrentMarking[ l_nPlacePos ] >= l_pcSimTransition->intervalUpperBound[i])
		{
			return false;
		}
	}

	return true;
}

void SP_DS_StSimGillespie::SimulationThread::CalculateAllHazardValues()
{

	for (unsigned long i = 0; i < m_nTransitionCount; i++)
	{
		CalculateHazardValue(i);
	}
}

void SP_DS_StSimGillespie::SimulationThread::CheckImmediateTransitions()
{

	while (m_pImmediateTransitionList->GetElemCount() > 0
			&& m_pcSimulation->IsSimulationRunning())
	{
		long l_nTransNumber = m_pImmediateTransitionList->ChooseTransitionByWeight();
		FireTransition(l_nTransNumber);
		CalculateHazardValue(l_nTransNumber);
	}

}

void SP_DS_StSimGillespie::SimulationThread::GenerateResultPoints(double& p_nOutputTime, unsigned long& p_nCurrentResultPoint)
{
	while ( (p_nOutputTime <= m_nCurrentTime ) &&
			(p_nOutputTime <= m_nIntervalEnd ))
	{
		if(m_nIntervalStart <= p_nOutputTime && p_nCurrentResultPoint < m_aanResults.size())
		{
			GenerateResultPoint(p_nCurrentResultPoint++);
		}
		p_nOutputTime += m_nIntervalSize;
	}
}

void SP_DS_StSimGillespie::SimulationThread::GenerateResultPoint(long p_nCurrentResultPoint)
{
	SP_VectorDouble& l_aResult = m_aanResults[ p_nCurrentResultPoint ];
	SP_VectorDouble& l_aResultSD = m_aanResultsSD[ p_nCurrentResultPoint ];
	SP_VectorDouble::iterator l_itResult = l_aResult.begin();
	SP_VectorDouble::iterator l_itResultSD = l_aResultSD.begin();
	SP_VectorLong::iterator l_itMarking = m_anCurrentMarking.begin();
	for (; l_itResult != l_aResult.end(); ++l_itResult, ++l_itResultSD, ++l_itMarking)
	{
		double l_nMarking = *l_itMarking;
		*l_itResult += l_nMarking;
		*l_itResultSD += pow(l_nMarking, 2.0);
	}

	SP_VectorDouble& l_aResultFireCount = m_aanResultTransitionFireCount[ p_nCurrentResultPoint ];
	SP_VectorDouble::iterator l_itResultFireCount = l_aResultFireCount.begin();
	SP_VectorLong::iterator l_itFireCount = m_anCurrentFireCount.begin();
	for (; l_itResultFireCount != l_aResultFireCount.end(); ++l_itResultFireCount, ++l_itFireCount)
	{
		*l_itResultFireCount += *l_itFireCount;
		*l_itFireCount = 0;
	}
}

void SP_DS_StSimGillespie::SimulationThread::InitialiseTimeline()
{
	SP_DS_StSimTransition* l_pcSimTransition;

	m_qpcSimTimeEventTimeline->Clear();

	for (unsigned long i = 0; i < m_nTransitionCount; i++)
	{
		l_pcSimTransition = m_apcSimTransitions[ i ];

		if(l_pcSimTransition->hazardFunctionType == SP_ST_SIM_HAZARD_FUNCTION_DTF_SINGLE)
		{
			double l_nTimePoint = l_pcSimTransition->timeEvent->fixedTime;

			if(l_nTimePoint >= m_nCurrentTime)
			{
				m_qpcSimTimeEventTimeline->InsertTimeEvent(l_pcSimTransition->timeEvent, l_nTimePoint);
			}

		}
		else if(l_pcSimTransition->hazardFunctionType == SP_ST_SIM_HAZARD_FUNCTION_DTF_PERIODIC_WITHIN_INTERVAL)
		{
			double l_nIntervalStart = l_pcSimTransition->timeEvent->intervalStart;

			if(l_nIntervalStart >= m_nCurrentTime)
			{
				m_qpcSimTimeEventTimeline->InsertTimeEvent(l_pcSimTransition->timeEvent, l_nIntervalStart);
			}
		}
	}
}

void SP_DS_StSimGillespie::SimulationThread::CheckTimeEventTimeLine(double& p_nOutputTime, unsigned long& p_nCurrentResultPoint)
{
	bool l_bGenerateResultPoints = (m_nCurrentTime == 0.0) ? false : true;

	SP_MultimapSimTimeEvent	l_pcTimeEvent = m_qpcSimTimeEventTimeline->GetTimeEvent(m_nCurrentTime);
	if(!l_pcTimeEvent.empty())
	{
		m_nCurrentTime = l_pcTimeEvent.begin()->first;
	}
	while(!l_pcTimeEvent.empty() && m_nCurrentTime <= m_nIntervalEnd
			&& m_pcSimulation->IsSimulationRunning())
	{
		if(l_bGenerateResultPoints)
		{
			GenerateResultPoints(p_nOutputTime, p_nCurrentResultPoint);
		}

		list<SP_DS_StSimTimeEvent*> l_lConsuming;
		list<SP_DS_StSimTimeEvent*> l_lNonConsuming;

		for(SP_MultimapSimTimeEvent::const_iterator l_Iter = l_pcTimeEvent.begin();
				l_Iter != l_pcTimeEvent.end(); ++l_Iter)
		{
			m_qpcSimTimeEventTimeline->RemoveElem(l_Iter->second);
			if(IsTransitionEnabled(l_Iter->second->arrayPos))
			{
				if(m_apcSimTransitions[l_Iter->second->arrayPos]->nonConsuming)
				{
					l_lNonConsuming.push_back(l_Iter->second);
				}
				else
				{
					l_lConsuming.push_back(l_Iter->second);
				}
			}
			if(l_Iter->second->eventType == SP_ST_SIM_TIME_EVENT_DET_FIRING_PERIODIC)
			{
				if (l_Iter->second->period > 0 && l_Iter->second->intervalEnd >= m_nCurrentTime + l_Iter->second->period)
				{
					m_qpcSimTimeEventTimeline->InsertTimeEvent(	l_Iter->second, m_nCurrentTime + l_Iter->second->period);
				}
			}
		}
		//fire consuming transitions
		while(!l_lConsuming.empty())
		{
			size_t l_nSize = l_lConsuming.size();
			size_t l_nChoose = (l_nSize > 1) ? SP_RandomLong(l_nSize) : 0;
			size_t i = 0;
			for(list<SP_DS_StSimTimeEvent*>::iterator l_Iter = l_lConsuming.begin();
					l_Iter != l_lConsuming.end(); ++l_Iter)
			{
				if(i == l_nChoose)
				{
					if(IsTransitionEnabled((*l_Iter)->arrayPos))
					{
						FireTransition((*l_Iter)->arrayPos);
						CalculateHazardValue((*l_Iter)->arrayPos);
					}
					l_lConsuming.erase(l_Iter);
					break;
				}
				++i;
			}
		}
		//fire non consuming transitions
		for(list<SP_DS_StSimTimeEvent*>::iterator l_Iter = l_lNonConsuming.begin();
				l_Iter != l_lNonConsuming.end(); ++l_Iter)
		{
			FireTransition((*l_Iter)->arrayPos);
			CalculateHazardValue((*l_Iter)->arrayPos);
		}

		CheckImmediateTransitions();

		m_nCurrentTime += GenerateRandomVariableExpDistr(m_nCombinedHazardValue);
		l_pcTimeEvent = m_qpcSimTimeEventTimeline->GetTimeEvent(m_nCurrentTime);
		if(!l_pcTimeEvent.empty())
		{
			m_nCurrentTime = l_pcTimeEvent.begin()->first;
		}
	}
}

unsigned long
SP_DS_StSimGillespie::SimulationThread::GetCurrentSimulationRunCount()
{
	return m_nCurrentSimulationRunCount;
}

void
SP_DS_StSimGillespie::SimulationThread::CheckApFormulae(SP_DS_StSimTransition* p_pcSimTrans)
{
	if(p_pcSimTrans != NULL)
	{
		if(p_pcSimTrans->hazardFunctionType == SP_ST_SIM_HAZARD_FUNCTION_IMMEDIATE_TRANSITION)
		{
			return;
		}
		if(p_pcSimTrans->hazardFunctionType == SP_ST_SIM_HAZARD_FUNCTION_TIMED_TRANSITION)
		{
			SP_DS_StSimTimeEvent* l_pcTimeEvent = p_pcSimTrans->timeEvent;
			if(l_pcTimeEvent->fixedTime == 0)
			{
				return;
			}
		}
		if((p_pcSimTrans->hazardFunctionType == SP_ST_SIM_HAZARD_FUNCTION_DTF_PERIODIC_WITHIN_INTERVAL
			|| p_pcSimTrans->hazardFunctionType == SP_ST_SIM_HAZARD_FUNCTION_DTF_SINGLE)
				&& m_nCurrentTime == 0)
		{
			return;
		}
	}
	if((m_nLastTime <= m_nIntervalStart && m_nIntervalStart <= m_nCurrentTime)
		|| (m_nIntervalStart <= m_nLastTime && m_nCurrentTime <= m_nIntervalEnd)
		|| (m_nLastTime <= m_nIntervalEnd && m_nIntervalEnd <= m_nCurrentTime))
	{
		for(size_t i = 0; i < m_aApFormulas.size(); ++i)
		{
			if(!m_aApFormulaHappend[i])
			{
				double l_nResult = m_aApFormulas[i].Evaluate(&m_anCurrentMarking);
				if(l_nResult > 0)
				{
					m_aApFormulaHappend[i] = true;
					m_aApResults[i] += l_nResult;
				}
			}
		}
	}
}

#endif


