//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: slehrack $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2007/07/12 09:35:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////


#include "SP_DS_StSimFAU.h"

#include <math.h>
#include <algorithm>

#include "sp_gui/windows/SP_GUI_Mainframe.h"
#include "sp_gui/dialogs/dia_SPN/SP_DLG_StSimulationResults.h"
#include "sp_gui/dialogs/SP_DLG_SimulationProperties.h"

#include "sp_gui/mdi/SP_MDI_Doc.h"

#include "sp_ds/SP_DS_Node.h"
#include "sp_core/SP_Core.h"

#include "sp_ds/attributes/SP_DS_BoolAttribute.h"
#include "sp_ds/attributes/SP_DS_DoubleAttribute.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"
#include "sp_ds/attributes/SP_DS_NumberAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"

#include <wx/ffile.h>
#include <dssd/auxi/auxi.h>

#define ZERO 1e-14
#define REALMAX 1.7e308
#define WINDOW
#define BIRTH_COMPRESS

SP_DS_StSimFAU::SP_DS_StSimFAU(SP_DS_Graph* p_pcGraph)
	:SP_DS_Simulation(p_pcGraph),
	m_nDelta(1e-11),
	m_nDeltaB(1e-11),
	m_nTolerance(1e-7),
	m_nEpsilon(1e-7),
	m_nMaxLambda(0),
	m_nCurrentResultPoint(0),
	m_ResetStates(false),
	m_cParser( 0),
	m_pImmediateTransitionList( 0), m_qpcSimTimeEventTimeline( 0),
	m_aanResults( 0),
	m_aanSingleResults( 0),
    m_aanResultTransitionFireCount(0),
    m_aanSingleTransitionFireCount(0)
{

	m_pImmediateTransitionList = new SP_DS_StSimImmediateTransitionList();
	m_qpcSimTimeEventTimeline = new SP_DS_StSimTimeEventTimeline();

	m_cParser = new SP_DS_StParser(p_pcGraph);

	m_pcApParser = new SP_DS_ApParser(p_pcGraph);
}

bool SP_DS_StSimFAU::AddToSimulationProperties(SP_DLG_SimulationProperties* p_pcDlg, wxSizer* p_pcSizer)
{
	wxSizer* l_pcRowSizer = NULL;

	l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );
	l_pcRowSizer->Add(new wxStaticText( p_pcDlg, -1, wxT("Max Lambda:") ), 1, wxALL | wxEXPAND, 5);
	m_pcMaxLambdaTextCtrl = new wxTextCtrl( p_pcDlg, -1, dssd::aux::toString(m_nMaxLambda), wxDefaultPosition, wxDefaultSize, 0 );
	l_pcRowSizer->Add(m_pcMaxLambdaTextCtrl, 0, wxALL, 5);
	p_pcSizer->Add(l_pcRowSizer, 1, wxEXPAND | wxALL, 5);

	l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );
	l_pcRowSizer->Add(new wxStaticText( p_pcDlg, -1, wxT("Delta:") ), 1, wxALL | wxEXPAND, 5);
	m_pcDeltaTextCtrl = new wxTextCtrl( p_pcDlg, -1, dssd::aux::toString(m_nDelta), wxDefaultPosition, wxDefaultSize, 0 );
	l_pcRowSizer->Add(m_pcDeltaTextCtrl, 0, wxALL, 5);
	p_pcSizer->Add(l_pcRowSizer, 1, wxEXPAND | wxALL, 5);

	l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );
	l_pcRowSizer->Add(new wxStaticText( p_pcDlg, -1, wxT("DeltaB:") ), 1, wxALL | wxEXPAND, 5);
	m_pcDeltaBTextCtrl = new wxTextCtrl( p_pcDlg, -1, dssd::aux::toString(m_nDeltaB), wxDefaultPosition, wxDefaultSize, 0 );
	l_pcRowSizer->Add(m_pcDeltaBTextCtrl, 0, wxALL, 5);
	p_pcSizer->Add(l_pcRowSizer, 1, wxEXPAND | wxALL, 5);

	l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );
	l_pcRowSizer->Add(new wxStaticText( p_pcDlg, -1, wxT("Tolerance:") ), 1, wxALL | wxEXPAND, 5);
	m_pcTolerenceTextCtrl = new wxTextCtrl( p_pcDlg, -1, dssd::aux::toString(m_nTolerance), wxDefaultPosition, wxDefaultSize, 0 );
	l_pcRowSizer->Add(m_pcTolerenceTextCtrl, 0, wxALL, 5);
	p_pcSizer->Add(l_pcRowSizer, 1, wxEXPAND | wxALL, 5);

	l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );
	l_pcRowSizer->Add(new wxStaticText( p_pcDlg, -1, wxT("Epsilon:") ), 1, wxALL | wxEXPAND, 5);
	m_pcEpsilonTextCtrl = new wxTextCtrl( p_pcDlg, -1, dssd::aux::toString(m_nEpsilon), wxDefaultPosition, wxDefaultSize, 0 );
	l_pcRowSizer->Add(m_pcEpsilonTextCtrl, 0, wxALL, 5);
	p_pcSizer->Add(l_pcRowSizer, 1, wxEXPAND | wxALL, 5);

	return true;
}

bool SP_DS_StSimFAU::OnSimulationPropertiesOk()
{
	double l_nValue = 0;

	if(m_pcMaxLambdaTextCtrl->GetValue().ToDouble(&l_nValue) && l_nValue >= 0)
	{
		m_nMaxLambda = l_nValue;
	}
	else
	{
		SP_MESSAGEBOX(wxT("Max Lambda is not a non negative number!"), wxT("Error"));
		return false;
	}

	if(m_pcDeltaTextCtrl->GetValue().ToDouble(&l_nValue) && l_nValue >= 0)
	{
		m_nDelta = l_nValue;
	}
	else
	{
		SP_MESSAGEBOX(wxT("Delta is not a non negative number!"), wxT("Error"));
		return false;
	}

	if(m_pcDeltaBTextCtrl->GetValue().ToDouble(&l_nValue) && l_nValue >= 0)
	{
		m_nDeltaB = l_nValue;
	}
	else
	{
		SP_MESSAGEBOX(wxT("DeltaB is not a non negative number!"), wxT("Error"));
		return false;
	}

	if(m_pcTolerenceTextCtrl->GetValue().ToDouble(&l_nValue) && l_nValue >= 0)
	{
		m_nTolerance = l_nValue;
	}
	else
	{
		SP_MESSAGEBOX(wxT("Tolerance is not a non negative number!"), wxT("Error"));
		return false;
	}

	if(m_pcEpsilonTextCtrl->GetValue().ToDouble(&l_nValue) && l_nValue >= 0)
	{
		m_nEpsilon = l_nValue;
	}
	else
	{
		SP_MESSAGEBOX(wxT("Epsilon is not a non negative number!"), wxT("Error"));
		return false;
	}

	return true;
}

void SP_DS_StSimFAU::SimulateRun()
{
	do
	{
		m_ResetStates = false;

		m_nLB = 0;
		m_nRB = 0;
		m_nLMin = 0;
		m_nRMax = 0;

		m_nCurrentTime = 0;
		m_nCurrentResultPoint = 0;

		m_nStateSpaceSize = 0;
		m_nMaxSize = 0;
		m_StateSpace.clear();
		m_StateSpace.resize(10000);
		m_mMapping.clear();
		m_aAvailable.clear();
		m_anW.clear();
		m_anCrtState.clear();

		long l_nIndex = NewNode(m_anInitialNetMarking);
		node& l_Initial = m_StateSpace[l_nIndex];
		l_Initial.prob = 1;
		l_Initial.ctmc = 1;
		if(m_nMaxLambda == 0.0)
		{
			m_nMaxLambda = l_Initial.sum;
		}

		GenerateResultPoint(m_nCurrentResultPoint++);

		if(m_nCurrentTime < m_nIntervalStart && m_nIntervalStart <= m_nIntervalEnd)
		{
			m_nCurrentTime = m_nIntervalStart;

			FoxGlynn_w(m_nMaxLambda*m_nIntervalStart, m_nEpsilon);

			l_Initial.acc = l_Initial.ctmc;
			l_Initial.prob = 0;
			l_Initial.ctmc = 0;

			Solution(m_nIntervalStart);

			UpdateSimulationDialog(m_nCurrentSimulationRunNumber);

			GenerateResultPoint(m_nCurrentResultPoint++);
		}


		while (!m_ResetStates && m_nCurrentTime < m_nIntervalEnd && IsSimulationRunning())
		{
			m_nCurrentTime += m_nIntervalSize;

			FoxGlynn_w(m_nMaxLambda*m_nIntervalSize, m_nEpsilon);

			for(unsigned long i = 0; i < m_nMaxSize; i++)
			{
				node& l_Node = m_StateSpace[i];
				l_Node.acc = l_Node.ctmc;
				l_Node.prob = 0;
				l_Node.ctmc = 0;
			}

			Solution(m_nIntervalSize);

			UpdateSimulationDialog(m_nCurrentSimulationRunNumber);

			GenerateResultPoint(m_nCurrentResultPoint++);
		}
	}
	while(m_ResetStates);

	CheckApFormulae();
}

double SP_DS_StSimFAU::Solution(double p_nStepTime)
{
	long k = 0;
	double l_nSumCoeff = 0;
	double l_nLambda = 0;
    double l_nExpectation = 0;
    double l_nVariance = 0;

#ifdef BIRTH_COMPRESS
    double l_nA = 0;
    double l_nB = 0;
#else
    double l_nA = 1;
    double l_nB = 0;
#endif

#ifdef WINDOW
    m_nLB = 0;
    m_nRB = 0;
#else
    m_nLB = 0;
    m_nRB = m_nRMax;
#endif

    m_anCrtState.assign(m_nRMax+1, 0.0);
	m_anCrtState[0] = 1;

	while(k < m_nRMax)
	{
		l_nLambda = Collect(k);

		if(l_nLambda > m_nMaxLambda)
		{
			wxString l_sMsg;
			l_sMsg << wxT("Max Lambda = ") << m_nMaxLambda << wxT(" is to small, found Lambda = ") << l_nLambda;
			SP_LOGMESSAGE( l_sMsg);
			//SP_MESSAGEBOX(l_sMsg, wxT("Error"));
			resetStates();
			m_nMaxLambda = l_nLambda;
			//AbortSimulation();
			break;
		}

		// Expectation and variance for the next iteration
		l_nExpectation += 1.0 / l_nLambda;
		l_nVariance += 1.0 / (l_nLambda * l_nLambda);

		//a is the selfloop probability of state i
		//b is the probability on the edge going from state i-1 to state i
		l_nB = 1.0 - l_nA;
		l_nA = 1.0 - l_nLambda / m_nMaxLambda;

#ifdef BIRTH_COMPRESS
		double l_nCoeff = BirthCoeffCompress(l_nA, l_nB, k);
#else
		double l_nCoeff = BirthProcessState(l_nA, l_nB, k);
#endif
		l_nSumCoeff += l_nCoeff;
		Propagate(l_nLambda, l_nCoeff, k);

		if (l_nExpectation > p_nStepTime && l_nCoeff < m_nTolerance)
		{
			wxString l_sMsg;
			l_sMsg << wxT("Break because enough mass was reached, with sum_coeff ") << l_nSumCoeff << wxT(" expected time = ")	<< l_nExpectation;
			//SP_LOGMESSAGE( l_sMsg);
			break;
		}
		++k;
	}
	return l_nLambda;
}

void SP_DS_StSimFAU::Propagate(double p_nLambda, double p_nCoeff, long p_nIteration)
{
	bool l_bCheckVanishing = false;
	const double l_nDelta = m_nDelta;
	//check normal states
	for(unsigned long i = 0; i < m_nMaxSize; i++)
	{
		node* l_Node = &m_StateSpace[i];

		if(l_Node->prob == 0)
		{
			continue;
		}
		if(l_Node->vanish)
		{
			l_bCheckVanishing = true;
			continue;
		}

		l_Node->ctmc += p_nCoeff * l_Node->prob;

		if(l_Node->absorb == STATE_ABSORB)
        {
        	continue;
        }

		const double l_nMult = l_Node->prob / p_nLambda;
        l_Node->prob = (p_nLambda - l_Node->sum) * l_nMult;

        SP_VectorDouble::const_iterator l_itRate = l_Node->rate.begin();
        SP_VectorLong::iterator l_itNext = l_Node->next.begin();
		for(unsigned long j = 0; j < m_nTransitionCount; ++j, ++l_itRate, ++l_itNext)
        {
			const double l_nPrb = (*l_itRate) * l_nMult; //l_Node->rate[j] * l_nMult;

			if(l_nPrb > l_nDelta)
			{
				if((*l_itNext) == -1)
				{
					state l_aSuccessor = l_Node->s;
					FireTransition(j, l_aSuccessor);
					long l_nIndex = NewNode(l_aSuccessor);
					l_Node = &m_StateSpace[i];
					l_itRate = l_Node->rate.begin()+j;
					l_itNext = l_Node->next.begin()+j;
					//l_Node->next[j] = l_nIndex;
					(*l_itNext) = l_nIndex;
				}
				node& l_NodeSuccessor = m_StateSpace[(*l_itNext)]; //l_Node->next[j]];
				l_NodeSuccessor.acc += l_nPrb;
				if(l_NodeSuccessor.vanish)
				{
					l_bCheckVanishing = true;
				}
			}
			else
			{
				//l_Node->next[j] = -1;
				(*l_itNext) = -1;
			}
		}
	}
	if(l_bCheckVanishing)
	{
		CheckVanishing(p_nIteration);
	}
}

void SP_DS_StSimFAU::CheckVanishing(long p_nIteration)
{
	bool l_bCheckVanishing = true;
	//check vanish states
	while(l_bCheckVanishing)
	{
		l_bCheckVanishing = false;
		Collect(p_nIteration);
		for(unsigned long i = 0; i < m_nMaxSize; i++)
		{
			node* l_Node = &m_StateSpace[i];
			if(!l_Node->vanish || l_Node->prob == 0)
			{
				continue;
			}
			for (unsigned long j = 0; j < m_nTransitionCount; ++j)
			{
				double l_nProb = l_Node->prob;
				l_nProb *= l_Node->rate[j];
				l_nProb /= l_Node->sum;
				if(l_nProb > m_nDelta
					&& m_apcSimTransitions[j]->hazardFunctionType == SP_ST_SIM_HAZARD_FUNCTION_IMMEDIATE_TRANSITION)
				{
					if(l_Node->next[j] == -1)
					{
						state l_aSuccessor = l_Node->s;
						FireTransition(j, l_aSuccessor);
						long l_nIndex = NewNode(l_aSuccessor);
						l_Node = &m_StateSpace[i];
						l_Node->next[j] = l_nIndex;
					}
					node& l_NodeSuccessor = m_StateSpace[l_Node->next[j]];
					l_NodeSuccessor.acc += l_nProb;
					if(l_NodeSuccessor.vanish)
					{
						l_bCheckVanishing = true;
					}
				}
				else
				{
					l_Node->next[j] = -1;
				}
			}
			l_Node->prob = 0;
		}
	}
}

double SP_DS_StSimFAU::Collect(long p_nIteration)
{
	double l_nReturn = 0;

	for(unsigned long i = 0; i < m_nMaxSize; i++)
	{
		node& l_Node = m_StateSpace[i];
		if(l_Node.prob == 0 && l_Node.acc == 0 && l_Node.ctmc == 0)
		{
			continue;
		}

		if(l_Node.absorb == STATE_NOT_CHECKED)
		{
			CheckNodeState(l_Node);
		}

		l_Node.prob += l_Node.acc;
		l_Node.acc = 0;


/*		if(l_Node.prob < m_nDelta 
			&& l_Node.ctmc == 0 
			&& p_nIteration > 1)
		{
			RemoveNode(i);
		}
		else 
*/		if(l_Node.sum > l_nReturn)
		{
			l_nReturn = l_Node.sum;
		}
	}

	return l_nReturn;
}

double SP_DS_StSimFAU::BirthProcessState(double p_nA, double p_nB, long p_nK)
{
	double l_nP = (p_nK ? 0 : 1);
    //the RB, LB variables keep the range of significant states in the birth process
    for(long j = m_nLB; j <= m_nRB; j++)
    {
        double l_nTmp = m_anCrtState[j] * p_nB;
        m_anCrtState[j] = l_nP;
        //below is equal, but should perform better than: l_nP = l_nTmp + m_anCrtState[j] * p_nA;
        l_nP = l_nTmp + l_nP * p_nA;

    }

	#ifdef WINDOW
	while ((l_nP > m_nDeltaB) && (m_nRB < m_nRMax))
	{
		m_nRB++;
		m_anCrtState[m_nRB] = l_nP;
		l_nP *= p_nA;
	}
	while (m_anCrtState[m_nLB] < m_nDeltaB)
	{
		m_nLB++;
	}
	#endif

	double l_nCoeff = 0;
	for(long j = m_nLMin; j <= m_nRB; j++)
	{
		l_nCoeff += m_anCrtState[j] * m_anW[j-m_nLMin];
    }

    // if coeff too small, just make it null
    if (l_nCoeff < m_nDelta)
    {
    	l_nCoeff = 0;
    }

	return l_nCoeff;
}

double SP_DS_StSimFAU::BirthCoeffCompress(double p_nA, double p_nB, long p_nK)
{
    //computing the coefficients by solving a birth process in an on the fly manner.
    //a is the selfloop probability of state i
    //b is the probability on the edge going from state i-1 to state i

    double l_nCoeff = 0.0;
    if ((p_nK+m_nRB) >= m_nRMax) m_nRB = m_nRMax - p_nK;
    //the m_nRB, LB variables keep the range of significant states in the birth process
    for (int j = m_nLB; j <= m_nRB; j++) {
        if (j == 0)
            m_anCrtState[j] = m_anCrtState[j] * p_nB;
        else
            m_anCrtState[j] = m_anCrtState[j] * p_nB + m_anCrtState[j-1] * p_nA;

        if (p_nK+j >= m_nLMin) {
            l_nCoeff += m_anCrtState[j] * m_anW[p_nK+j-m_nLMin];
        }

        #ifdef WINDOW
        if (j == m_nRB && m_anCrtState[m_nRB] > m_nDeltaB && (p_nK+m_nRB) < m_nRMax) {
            m_nRB++;
        }
        #endif
    }
    #ifdef WINDOW
    while (m_anCrtState[m_nLB] < m_nDeltaB && (p_nK+m_nLB) < m_nRMax) m_nLB++;
    #endif



    // if coeff too small, just make it null
    if (l_nCoeff<m_nDelta) l_nCoeff=0;


    return l_nCoeff;
}

long SP_DS_StSimFAU::NewNode(const state& p_aState)
{
	mapType::iterator l_it = m_mMapping.find(p_aState);
	if(l_it != m_mMapping.end())
	{
		return l_it->second;
	}

	long l_nIndex = 0;
	++m_nStateSpaceSize;

	if(!m_aAvailable.empty())
	{
		l_nIndex = *(m_aAvailable.begin());
		m_aAvailable.erase(m_aAvailable.begin());
	}
	else
	{
		if(m_nMaxSize == m_StateSpace.size())
		{
			long l_nNewSize = m_nMaxSize * 1.25;
			m_StateSpace.resize(l_nNewSize);
		}
		l_nIndex = m_nMaxSize++;
	}

	node& l_Node = m_StateSpace[l_nIndex];
	l_Node.s = p_aState;
	l_Node.vanish = false;
	l_Node.absorb = STATE_NOT_CHECKED;
	l_Node.prob = 0.0;
	l_Node.acc = 0.0;
	l_Node.ctmc = 0.0;
	l_Node.rate.assign(m_nTransitionCount, 0.0);
	l_Node.next.assign(m_nTransitionCount, -1);

	ComputeRate(l_Node);

	CheckNodeState(l_Node);

	m_mMapping.insert(make_pair(p_aState, l_nIndex));

	return l_nIndex;
}

void SP_DS_StSimFAU::RemoveNode(long p_nNode)
{
	node& l_Node = m_StateSpace[p_nNode];
	state& l_State = l_Node.s;
	m_mMapping.erase(l_State);

	l_Node.vanish = false;
	l_Node.absorb = STATE_NOT_CHECKED;
	l_Node.prob = 0;
	l_Node.acc = 0;
	l_Node.ctmc = 0;
	l_Node.sum = 0;
	l_Node.next.clear();
	l_Node.rate.clear();
	l_Node.s.clear();

	m_aAvailable.insert(p_nNode);
	--m_nStateSpaceSize;
}

void SP_DS_StSimFAU::resetStates()
{
	for(unsigned long k = 0; k < m_nMaxSize; k++)
	{
		node& s = m_StateSpace[k];
		s.prob = 0.0;
		s.ctmc = 0.0;
		s.acc = 0.0;
		s.absorb = STATE_NOT_CHECKED;
	}
	for(unsigned i = 0; i < m_aanResults.size(); ++i)
	{
		for(unsigned k = 0; k < m_aanResults[i].size(); ++k)
		{
			m_aanResults[i][k] = 0.0;
			m_aanResultsSD[i][k] = 0.0;
		}
		for(unsigned k = 0; k < m_aanResultTransitionFireCount[i].size(); ++k)
		{
			m_aanResultTransitionFireCount[i][k] = 0.0;
		}
	}
	m_nCurrentResultPoint = 0;
	m_ResetStates = true;
}

int SP_DS_StSimFAU::FoxGlynn_Left(double l, double epsilon)
{
    int k, L;
    int m = (int) l;
    //find L
    double bl = (1+1/l)*exp(1/(8*l));
    if (l<25) {
        L = 0;
    }
    else {
        k=4;
        while (epsilon/2.0<((bl*exp(-(k*k)/2.0))/(k*sqrt(2.0*M_PI)))) k++;
        L = (int)floor(m-k*sqrt(l)-3.0/2.0);
    }
    return L;
}


int SP_DS_StSimFAU::FoxGlynn_Right(double l, double epsilon)
{
    int k, R;
    int m = (int) l;
    //find R
    double al = (1+1/l)*exp(1.0/16)*sqrt(2.0);

    if (l<400) {
        //l=400;
    }
    k=4;
    double dkl = exp(-(2.0/9.0)*(k*sqrt(2*l)+3.0/2.0));
    dkl = 1.0/(1-dkl);
    while (epsilon/2.0<((al*dkl*exp(-(k*k)/2.0))/(k*sqrt(2.0*M_PI)))) {
        k=k+1;
        dkl=exp(-(2.0/9.0)*(k*sqrt(2*l)+3.0/2.0));
        dkl=1.0/(1-dkl);
    }
    R=(int)ceil(m+k*sqrt(2.0*l)+3/2);

    return R;
}


void SP_DS_StSimFAU::FoxGlynn_w(double l, double epsilon)
{
    int j, s, t;
    int m = (int)l;
    double wm, W;

    m_nLMin = FoxGlynn_Left(l, epsilon);
    m_nRMax = FoxGlynn_Right(l, epsilon);

    int L = m_nLMin;
    int R = m_nRMax;


    //compute elements of m_anW
    //m_anW = (double *)malloc(sizeof(double) * (R-L+1));
    m_anW.assign(R-L+1, 0.0);

    wm=(REALMAX/1.e10)/(R-L);
    j=m;

    m_anW[m-L] = wm;
    while (j>L) {
        m_anW[j-1-L]=(j/l)*m_anW[j-L];
        j=j-1;
    }
    j=m;
    while (j<R) {
        m_anW[j+1-L]=(l/(j+1))*m_anW[j-L];
        j=j+1;
    }

    // compute W
    W=0;
    s=L;
    t=R;
    while (s<t) {
        if (m_anW[s-L]<=m_anW[t-L]) {
            W=W+m_anW[s-L];
            s=s+1;
        }
        else {
            W=W+m_anW[t-L];
            t=t-1;
        }
    }
    W=W+m_anW[s-L];

    for (s = 0; s < R-L+1; s++) {
        m_anW[s] /= W;
    }

}

void SP_DS_StSimFAU::CheckApFormulae()
{
	for(size_t i = 0; i < m_nMaxSize; ++i)
	{
		const node& l_Node = m_StateSpace[i];

		if(l_Node.vanish || l_Node.ctmc == 0)
		{
			continue;
		}

		for(size_t i = 0; i < m_aApFormulas.size(); ++i)
		{
			double l_nResult = m_aApFormulas[i].Evaluate(&l_Node.s);
			if(l_nResult > 0)// && !m_aApFormulaHappend[i])
			{
				//m_aApFormulaHappend[i] = true;
				m_aApResults[i] += l_Node.ctmc;
				m_aApResultsCount[i] += 1;
			}
		}
	}
}

void SP_DS_StSimFAU::CheckNodeState(node& p_Node)
{
	if(p_Node.vanish || (m_nIntervalStart >= m_nCurrentTime && m_nIntervalStart > 0))
	{
		return;
	}

	//TODO: only 1 SpFormula possible
	for(unsigned int i = 0; i < m_aApFormulas.size(); ++i)
	{
		double l_Result = m_aApFormulas[i].Evaluate(&(p_Node.s));
		if(l_Result == 0)
		{
			p_Node.absorb = STATE_NOT_ABSORB;
		}
		else
		{
			p_Node.absorb = STATE_ABSORB;
		}
	}
}

void SP_DS_StSimFAU::GenerateResultPoint(long p_nCurrentResultPoint)
{
	double l_nSumCTMC = 0;
	double l_nMinCTMC = m_nMaxLambda;
	double l_nMaxCTMC = 0;
	double l_nSumProb = 0;
	double l_nMinProb = m_nMaxLambda;
	double l_nMaxProb = 0;
	for(unsigned long k = 0; k < m_nMaxSize; k++)
	{
		double l_nCTMC = m_StateSpace[k].ctmc;
		if(l_nCTMC > 0.0)
		{
			l_nSumCTMC += l_nCTMC;
			if(l_nCTMC != 0 && l_nMinCTMC > l_nCTMC) l_nMinCTMC = l_nCTMC;
			if(l_nCTMC > l_nMaxCTMC) l_nMaxCTMC = l_nCTMC;

			double l_nProb = m_StateSpace[k].prob;
			l_nSumProb += l_nProb;
			if(l_nProb != 0 && l_nMinProb > l_nProb) l_nMinProb = l_nProb;
			if(l_nProb > l_nMaxProb) l_nMaxProb = l_nProb;

			for (unsigned long i = 0; i < m_nPlaceCount; i++)
			{
				long l_nMarking = m_StateSpace[k].s[i];
				m_aanResults[ p_nCurrentResultPoint ][ i ] += l_nCTMC * l_nMarking;
			}
		}
	}
	wxString l_sMsg;
	l_sMsg << wxT("Time: ") << m_nCurrentTime << wxT(" States: ") << m_nStateSpaceSize << wxT(" max States: ") << m_nMaxSize << wxT("\n");
	l_sMsg << wxT(" sum CTMC: ") << l_nSumCTMC << wxT(" min CTMC: ") << l_nMinCTMC << wxT(" max CTMC: ") << l_nMaxCTMC << wxT(" mean CTMC: ") << (l_nSumCTMC / m_nStateSpaceSize) << wxT("\n");
	l_sMsg << wxT(" sum Prob: ") << l_nSumProb << wxT(" min Prob: ") << l_nMinProb << wxT(" max Prob: ") << l_nMaxProb << wxT(" mean Prob: ") << (l_nSumProb / m_nStateSpaceSize);
//	SP_LOGMESSAGE( l_sMsg);

}
/*
void SP_DS_StSimFAU::ExecuteTimeEvent(SP_DS_StSimTimeEvent* p_pcTimeEvent)
{

	long l_nArrayPos = p_pcTimeEvent->arrayPos;
	double l_nPeriod = p_pcTimeEvent->period;
	double l_nIntervallStart = p_pcTimeEvent->intervalStart;
	double l_nIntervallEnd = p_pcTimeEvent->intervalEnd;
	SP_DS_StSimTransition* l_pcSimTransition = m_apcSimTransitions[ l_nArrayPos ];
	if(p_pcTimeEvent->eventType == SP_ST_SIM_TIME_EVENT_TIMED_FIRING_SINGLE)
	{
		if (l_pcSimTransition->timelineElemPointer)
		{
			m_qpcSimTimeEventTimeline->RemoveElem(l_pcSimTransition->timelineElemPointer);

			l_pcSimTransition->timelineElemPointer	= 0;
		}

		if (IsTransitionEnabled(l_nArrayPos) )
		{
			FireTransition(l_nArrayPos);
			CalculateHazardValue(l_nArrayPos);

			CheckImmediateTransitions();
		}
	}
	else if(p_pcTimeEvent->eventType == SP_ST_SIM_TIME_EVENT_DET_FIRING_SINGLE)
	{
		if (l_pcSimTransition->timelineElemPointer)
		{
			m_qpcSimTimeEventTimeline->RemoveElem(l_pcSimTransition->timelineElemPointer);

			l_pcSimTransition->timelineElemPointer	= 0;
		}

		if (IsTransitionEnabled(l_nArrayPos) )
		{
			FireTransition(l_nArrayPos);
			CheckImmediateTransitions();
		}
	}
	else if(p_pcTimeEvent->eventType == SP_ST_SIM_TIME_EVENT_DET_FIRING_PERIODIC)
	{
		if (l_pcSimTransition->timelineElemPointer)
		{
			m_qpcSimTimeEventTimeline->RemoveElem(l_pcSimTransition->timelineElemPointer);

			l_pcSimTransition->timelineElemPointer	= 0;
		}

		if (IsTransitionEnabled(l_nArrayPos) )
		{
			FireTransition(l_nArrayPos);
			CheckImmediateTransitions();
		}

		if (l_nPeriod > 0 && l_nIntervallEnd >= m_nCurrentTime + l_nPeriod)
		{
			l_pcSimTransition->timelineElemPointer
					= m_qpcSimTimeEventTimeline->InsertTimeEvent(
							CreateSimTimeEventForDTF_Transition_PeriodInterval(
									l_nArrayPos, l_nPeriod,
									l_nIntervallStart, l_nIntervallEnd),
							m_nCurrentTime + l_nPeriod);
		}

		if ( !m_qpcSimTimeEventTimeline->m_pcCurrentTimelineElem)
		{
			m_qpcSimTimeEventTimeline->m_pcCurrentTimelineElem
					= l_pcSimTransition->timelineElemPointer;
		}
	}

}
*/
void SP_DS_StSimFAU::FireTransition(long p_nTransNumber, state& p_aState)
{
	SP_DS_StSimTransition* l_pcSimTransition = m_apcSimTransitions[p_nTransNumber];
	for(unsigned long k = 0; k < l_pcSimTransition->manipulatedPlacePos.size(); k++)
	{
		long l_nPlacePos = l_pcSimTransition->manipulatedPlacePos[k];
		if(l_pcSimTransition->resetMarking[k])
		{
			p_aState[ l_nPlacePos] = 0;
		}
		p_aState[ l_nPlacePos ] += l_pcSimTransition->deltaMarking[k];
	}
}

double SP_DS_StSimFAU::CalculateHazardValue(long p_nTransitionNumber, const state& p_aState)
{
	double l_nHazardValue = 0;
	SP_DS_StSimTransition* l_pcSimTransition = m_apcSimTransitions[ p_nTransitionNumber ];

	if ( !IsTransitionEnabled(p_nTransitionNumber, p_aState) )
	{

	}
	else if( l_pcSimTransition->hazardFunctionType == SP_ST_SIM_HAZARD_FUNCTION_BMA_CONSTANT_PARAMETER ||
		l_pcSimTransition->hazardFunctionType == SP_ST_SIM_HAZARD_FUNCTION_BMA_DYNAMIC_PARAMETER ||
		l_pcSimTransition->hazardFunctionType == SP_ST_SIM_HAZARD_FUNCTION_BIO_LEVEL_INTERPRETATION ||
		l_pcSimTransition->hazardFunctionType == SP_ST_SIM_HAZARD_FUNCTION_BIO_MICHAELIS_MENTEN ||
		l_pcSimTransition->hazardFunctionType == SP_ST_SIM_HAZARD_FUNCTION_DYNAMIC ||
		l_pcSimTransition->hazardFunctionType == SP_ST_SIM_HAZARD_FUNCTION_CONSTANT)
	{

		l_nHazardValue = m_cParser->Evaluate(l_pcSimTransition->hazardTree,
												l_pcSimTransition);
		if(l_nHazardValue < 0)
		{
			l_nHazardValue = 0;
		}
	}
	else if(l_pcSimTransition->hazardFunctionType == SP_ST_SIM_HAZARD_FUNCTION_IMMEDIATE_TRANSITION)
	{
		l_nHazardValue = m_cParser->Evaluate(l_pcSimTransition->hazardTree,
												l_pcSimTransition);
		if(l_nHazardValue < 0)
		{
			l_nHazardValue = 0;
		}
	}
	else if(l_pcSimTransition->hazardFunctionType == SP_ST_SIM_HAZARD_FUNCTION_TIMED_TRANSITION)
	{

	}
	return l_nHazardValue;
}

bool SP_DS_StSimFAU::IsTransitionEnabled( long p_nTransitionNumber, const state& p_aState)
{
	SP_DS_StSimTransition* l_pcSimTransition = m_apcSimTransitions[p_nTransitionNumber];
	for (unsigned long i = 0; i < l_pcSimTransition->basedPlacePos.size(); i++)
	{
		long l_nPlacePos = l_pcSimTransition->basedPlacePos[i];
		if( p_aState[ l_nPlacePos ] < l_pcSimTransition->intervalLowerBound[i] ||
			p_aState[ l_nPlacePos ] >= l_pcSimTransition->intervalUpperBound[i])
		{
			return false;
		}
	}

	return true;
}

void SP_DS_StSimFAU::ComputeRate(node& p_Node)
{
	m_cParser->SetCurrentMarking(&(p_Node.s));
	double l_nSum = 0;
	double l_nSumImm = 0;
	for (unsigned long i = 0; i < m_nTransitionCount; ++i)
	{
		double l_nHazard = CalculateHazardValue(i, p_Node.s);
		if(l_nHazard > 0
			&& m_apcSimTransitions[i]->hazardFunctionType == SP_ST_SIM_HAZARD_FUNCTION_IMMEDIATE_TRANSITION)
		{
			p_Node.vanish = true;
			l_nSumImm += l_nHazard;
		}
		else
		{
			l_nSum += l_nHazard;
		}
		p_Node.rate[i] = l_nHazard;
	}
	if(p_Node.vanish)
	{
		p_Node.sum = l_nSumImm;
	}
	else
	{
		p_Node.sum = l_nSum;
	}
}

void SP_DS_StSimFAU::StartSimulation()
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
		}
	}

	SimulateRun();

	m_bFlagSimulationRunning = false;

	WriteResults();
	m_pcSimulationResultsDlg->LoadResults();
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

bool SP_DS_StSimFAU::Initialise()
{
	if( m_pcGraph->GetNetclass()->GetName() !=  SP_DS_COLSPN_CLASS)
	{
		SetMemoryFree();
	}


	if(m_pcSimulationResultsDlg)
	{
		m_eExportType = m_pcSimulationResultsDlg->GetExportType();

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
					m_aApResultsCount.push_back(0);
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

	if (m_nResultPointCount > 0)
	{
		m_aanResults.assign(m_nResultPointCount+2, SP_VectorDouble(m_nPlaceCount, 0));
		m_aanResultsSD.assign(m_nResultPointCount+2, SP_VectorDouble(m_nPlaceCount, 0));
		m_aanResultTransitionFireCount.assign(m_nResultPointCount+2, SP_VectorDouble(m_nTransitionCount, 0));
	}

	m_cParser->SetCurrentMarking(&m_anInitialNetMarking);

	m_cParser->SetSimTime( &m_nCurrentTime);

	m_cParser->SetSimIntervalStart(m_nIntervalStart);
	m_cParser->SetSimIntervalEnd(m_nIntervalEnd);

	m_cParser->SetPlaceArrayPos(&m_mnPlaceArrayPos);
	m_cParser->SetTransitionArrayPos(&m_mnTransitionArrayPos);
	m_cParser->SetPreIncMatrix(m_aanPreIncMatrix);


	m_cParser->LoadParameter();

	if( m_pcGraph->GetNetclass()->GetName()!= SP_DS_COLSPN_CLASS )
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

		/*
		 //    Debug_ShowDeltaMatrix();*/

		//	  Debug_ShowSimTransitions();
		//    Debug_ShowSimPlaces();


		//    Debug_ShowSimTimeEventQueue( m_qpcSimTimeEventTimeline.m_pcFirstTimelineElem );

		//    m_qpcSimTimeEventTimeline.SwitchElements( m_qpcSimTimeEventTimeline.m_pcFirstTimelineElem,
		//      m_qpcSimTimeEventTimeline.m_pcFirstTimelineElem->next->next->next->next );

		//    Debug_ShowSimTimeEventQueue( m_qpcSimTimeEventTimeline.m_pcFirstTimelineElem );

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

void SP_DS_StSimFAU::LoadPlaces()
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

void SP_DS_StSimFAU::LoadTransitions()
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

bool SP_DS_StSimFAU::InitSimPlaces()
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

bool SP_DS_StSimFAU::InitSimTransitions()
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

SP_DS_StSimPlace* SP_DS_StSimFAU::CreateSimPlace(long p_nPlaceArrayPos)
{
	SP_DS_StSimPlace* l_pcSimPlace = new SP_DS_StSimPlace();
	l_pcSimPlace->arrayIndex = p_nPlaceArrayPos;

	return l_pcSimPlace;
}

SP_DS_StSimTransition* SP_DS_StSimFAU::CreateSimTransition(long p_nTransitionArrayPos)
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
		//based places
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

void SP_DS_StSimFAU::ShowParseError(const wxString& p_sNodeType)
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

void SP_DS_StSimFAU::SetDeltaVector(SP_DS_Node* p_pcTransNode, long p_nTransArrayPos)
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

void SP_DS_StSimFAU::Debug_ShowSimTransitions()
{

	SP_DS_StSimTransition* l_pcSimTransition;

	wxString l_sTrans = wxT("");
	wxString l_sAll = wxT("");

	wxString l_sMani = wxT("");
	wxString l_sEvents = wxT("");

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

		while (l_pcSimTransition->manipulatedPlacePos[ j ] > -1)
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

		l_sTrans += l_sEvents;
		l_sTrans += wxT("\n\n");

		l_sTrans += wxT("Function-Type: ");
		l_sTrans
				+= GetHazardFunctionTypeAsString(l_pcSimTransition->hazardFunctionType);
		l_sTrans += wxT("\n\n");

		l_sTrans += wxT("Function: ");
		l_sTrans += m_msTransitionFunctions[ i ];
		l_sTrans += wxT("\n\n");

	}

	l_sAll += l_sTrans;

	SP_LOGDEBUG( l_sAll );
	//  SP_MESSAGEBOX( l_sAll, wxT("Debug - SimTransitions"), wxOK | wxICON_INFORMATION );

}

void SP_DS_StSimFAU::Debug_ShowSimPlaces()
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

	SP_MESSAGEBOX(l_sAll, wxT("Debug"), wxOK | wxICON_INFORMATION);

}

void SP_DS_StSimFAU::Debug_ShowDeltaMatrix()
{

	wxString l_sAll = wxT("");
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
			l_sTrans += wxString::Format(wxT("%d"), m_aanPreIncMatrix->GetValue(i, j).first);

		}

		l_sTrans += wxT("\n");

	}

	l_sAll += l_sTrans;

	SP_MESSAGEBOX(l_sAll, wxT("Debug: PreIncMatrix"), wxOK | wxICON_INFORMATION);

	l_sAll = wxT("");
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
			l_sTrans += wxString::Format(wxT("%d"), m_aanDeltaIncMatrix->GetValue(i, j));

		}

		l_sTrans += wxT("\n");

	}

	l_sAll += l_sTrans;

	SP_MESSAGEBOX(l_sAll, wxT("Debug: Delta matrix"), wxOK | wxICON_INFORMATION);

}

void SP_DS_StSimFAU::SetToUpdateTransitionPosOfPlaces(long p_nPlaceNumber)
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

	for (l_iter = l_snTransitionArrayPos.begin(); l_iter != l_snTransitionArrayPos.end(); l_iter++)
	{
		m_apcSimPlaces[ p_nPlaceNumber ]->toUpdateTransitionPos.push_back( *l_iter );
	}

}

void SP_DS_StSimFAU::SetToUpdateTransitionPosOfTransitions(long p_nTransNumber)
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
	for (l_iter = l_snTransitionArrayPos.begin(); l_iter != l_snTransitionArrayPos.end(); l_iter++)
	{
		l_pcTrans->toUpdateTransitionPos.push_back( *l_iter );
	}
}

wxString SP_DS_StSimFAU::GetHazardFunctionTypeAsString(
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

void SP_DS_StSimFAU::InitialiseTimeline()
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

void SP_DS_StSimFAU::WriteResults()
{
	for(size_t i = 0; i < m_aApFormulas.size(); ++i)
	{
		double l_nValue = m_aApResults[i];
		wxString l_sOutput = wxT("Result of formula ");
		l_sOutput << wxString(m_aApFormulas[i].GetExpressionTree().toString().c_str(), wxConvUTF8);
		l_sOutput << wxT(" at interval [") << m_nIntervalStart << wxT(", ") << m_nIntervalEnd << wxT("]");
		SP_LOGMESSAGE( l_sOutput);
		long l_nTime = m_cSimulationStopWatch.Time();
		long l_nTimeSec = l_nTime/1000;
		long l_nTimeMin = l_nTimeSec/60;
		long l_nTimeHour = l_nTimeMin/60;
		l_sOutput = wxT("runtime: ");
		l_sOutput << l_nTimeHour << wxT("h ") << (l_nTimeMin - l_nTimeHour*60) << wxT("min ") << (l_nTimeSec - l_nTimeMin*60) << wxT("sec");
		SP_LOGMESSAGE( l_sOutput);
		l_sOutput = wxT("Prop: ");
		l_sOutput << l_nValue;
		l_sOutput << wxT(" states: ") << m_aApResultsCount[i];
		SP_LOGMESSAGE( l_sOutput);
	}

}
void SP_DS_StSimFAU::SetMemoryFree()
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

	m_anInitialNetMarking.clear();

	m_aanResults.clear();
	m_aanSingleResults.clear();

	m_aApFormulaHappend.clear();
	m_aApFormulas.clear();
	m_aApResults.clear();
	m_aApResultsCount.clear();
	m_sApFormulas.clear();

	m_aanResultTransitionFireCount.clear();
	m_aanSingleTransitionFireCount.clear();


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

void SP_DS_StSimFAU::SetMemoryFreeTransitions()
{
	for (unsigned long i = 0; i < m_apcSimTransitions.size(); i++)
	{
		SP_DS_StSimTransition* l_pcSimTrans = m_apcSimTransitions[ i ];
		wxDELETE(l_pcSimTrans);
	}
	m_apcSimTransitions.clear();
}

void SP_DS_StSimFAU::SetMemoryFreePlaces()
{
	for (unsigned long i = 0; i < m_apcSimPlaces.size(); i++)
	{
		SP_DS_StSimPlace* l_pcSimPlace = m_apcSimPlaces[ i ];
		wxDELETE(l_pcSimPlace);
	}
	m_apcSimPlaces.clear();
}

SP_DS_StSimFAU::~SP_DS_StSimFAU()
{

	SetMemoryFree();

	wxDELETE(m_pImmediateTransitionList);
	wxDELETE(m_qpcSimTimeEventTimeline);

	wxDELETE(m_cParser);

	wxDELETE(m_pcApParser);
}

void SP_DS_StSimFAU::LoadData()
{
/*
	m_nPlaceCount = m_mnPlaceArrayPos.size();
	m_anInitialNetMarking.resize(m_nPlaceCount);

	SP_DS_Nodeclass* l_pcNodeclass;
	l_pcNodeclass= m_pcGraph->GetNodeclass(SP_DS_DISCRETE_PLACE);
	SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >((*(l_pcNodeclass->GetElements()->begin()))->GetAttribute(wxT("MarkingList")));
	unsigned l_nCurrentColumn =	l_pcColList->GetActiveColumn();

	for( unsigned i = 0; i < m_anNetMarkings.size(); i++)
	{
		m_anInitialNetMarking[i] = m_anNetMarkings[i][l_nCurrentColumn];
	}
*/

	//load current marking
	m_anInitialNetMarking.clear();
	m_anInitialNetMarking  = *(m_pcUnfoldedNet->GetCurColStMarking());

	//load current rate functions
	m_msTransitionFunctions.clear();
	m_msTransitionFunctions  = *(m_pcUnfoldedNet->GetCurRateFunction());

/*
	m_nTransitionCount = m_mnTransitionArrayPos.size();
	m_msTransitionFunctions.resize(m_nTransitionCount);	
	for( unsigned i = 0; i < m_nTransitionCount; i++)
	{
		m_msTransitionFunctions[i] = wxT("0");
	}

	l_pcNodeclass= m_pcGraph->GetNodeclass(wxT("Transition"));
	l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >((*(l_pcNodeclass->GetElements()->begin()))->GetAttribute(wxT("FunctionList")));
	l_nCurrentColumn =	l_pcColList->GetActiveColumn();

	

	if(m_nsTransitionCountByNodeClass.find(wxT("Transition")) != m_nsTransitionCountByNodeClass.end())
	{
		for(unsigned int i = m_nsTransitionCountByNodeClass.find(wxT("Transition"))->second.m_nLow ; i <= m_nsTransitionCountByNodeClass.find(wxT("Transition"))->second.m_nUp; i++ )
		{
			m_msTransitionFunctions[i] = m_anNetFunctions[i][l_nCurrentColumn];
		}
	}


	l_pcNodeclass= m_pcGraph->GetNodeclass(wxT("Immediate Transition"));
	if(l_pcNodeclass->GetElements()->size() > 0)
	{
		l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >((*(l_pcNodeclass->GetElements()->begin()))->GetAttribute(wxT("FunctionList")));
		l_nCurrentColumn =	l_pcColList->GetActiveColumn();

		if(m_nsTransitionCountByNodeClass.find(wxT("Immediate Transition")) != m_nsTransitionCountByNodeClass.end())
		{
			for(unsigned i = m_nsTransitionCountByNodeClass.find(wxT("Immediate Transition"))->second.m_nLow ; i <= m_nsTransitionCountByNodeClass.find(wxT("Immediate Transition"))->second.m_nUp; i++ )
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
			for(unsigned i = m_nsTransitionCountByNodeClass.find(wxT("Deterministic Transition"))->second.m_nLow ; i <= m_nsTransitionCountByNodeClass.find(wxT("Deterministic Transition"))->second.m_nUp; i++ )
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
			for(unsigned i = m_nsTransitionCountByNodeClass.find(wxT("Scheduled Transition"))->second.m_nLow ; i <= m_nsTransitionCountByNodeClass.find(wxT("Scheduled Transition"))->second.m_nUp; i++ )
			{
				m_msTransitionFunctions[i] = m_anNetFunctions[i][l_nCurrentColumn];
			}
		}
	}

*/



}
