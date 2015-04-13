//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: cr $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2009/05/27 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_SIMULATION_H__
#define __SP_DS_SIMULATION_H__

#include "sp_ds/SP_DS_Graph.h"
#include "sp_gui/dialogs/dia_SPN/SP_DLG_StSimulationResults.h"
#include "sp_gui/dialogs/SP_DLG_Simulation.h"
#include "sp_ds/extensions/Col_SPN/SP_DS_ColPN_Unfolding.h"

class SP_DLG_SimulationProperties;

class SP_DS_Simulation : public wxEvtHandler
{
private:
    DECLARE_EVENT_TABLE()

protected:
	SP_DS_Graph* m_pcGraph;
	SP_DLG_Simulation* m_pcSimulationResultsDlg;

	bool m_bFlagSimulationRunning;
    double m_nIntervalStart;
    double m_nIntervalEnd;
    double m_nIntervalSize;

	unsigned long m_nResultPointCount;
    unsigned long m_nSimulationRunCount;
    unsigned long m_nGeneratedResultPointsCount;
    wxTextCtrl* m_pcSimRunCountTextCtrl;
    unsigned long m_nCurrentSimulationRunNumber;

	unsigned long m_nCurrentIterationNumber;
	unsigned long m_nIterationCount;

	unsigned long m_nRounds;
	long m_nLastUpdateTime;

    double m_nCurrentTime;
    double m_nLastTime;

    unsigned long m_nPlaceCount;
    unsigned long m_nTransitionCount;

	SP_MapString2Long m_mnPlaceArrayPos;
    SP_VectorString m_msPlaceNames;
    SP_MapString2String m_msPlaceNamesById;
    vector<SP_DS_Node*> m_mpcPlaceNodes;

    SP_MapString2Long m_mnTransitionArrayPos;
    SP_VectorString m_msTransitionNames;
    SP_MapString2String m_msTransitionNamesById;
    vector<SP_DS_Node*> m_mpcTransitionNodes;

	wxStopWatch m_cSimulationStopWatch;

	SP_ExportType m_eExportType;

	bool m_bComAuxVarSingleRun;

	//only for colored stochastic petri nets

	SP_DS_ColPN_Unfolding* m_pcUnfoldedNet;
	map<wxString, int>*   m_pmColActiveList;

	vector<SP_VectorString> m_anNetFunctions;
	vector<SP_VectorLong> m_anNetMarkings;

	SP_MatrixDouble* m_aanDeltaIncMatrix;
	SP_MatrixBool* m_aanResetIncMatrix;
	SP_MatrixPairDouble* m_aanPreIncMatrix;
	SP_VectorBool m_abNonConsumingMode;

	map< long, SP_MapString2Long > m_mmPrePlaceArrayPosOfTransitionNode;
	

	map<wxString, SP_CPN_CountInterval> m_nsTransitionCountByNodeClass;

public:
	SP_DS_Simulation(SP_DS_Graph* p_pcGraph);
	virtual ~SP_DS_Simulation();

//	virtual bool AddToSimulation(SP_DLG_Simulation* p_pcDlg, wxSizer* p_pcSizer);
	virtual bool AddToSimulationProperties(SP_DLG_SimulationProperties* p_pcDlg, wxSizer* p_pcSizer);
	virtual bool OnSimulationPropertiesOk();

	virtual bool Initialise() = 0;
	virtual void  LoadPlaces() = 0;
    virtual void LoadTransitions() = 0;

    virtual void StartSimulation() = 0;
    virtual void AbortSimulation() { m_bFlagSimulationRunning = false; }
    virtual bool IsSimulationRunning() { return m_bFlagSimulationRunning; }

    virtual void SetSimulationResultsDlg( SP_DLG_Simulation* p_pcDlg ) { m_pcSimulationResultsDlg = p_pcDlg; }

    virtual const SP_Vector2DDouble& GetResultMatrix() = 0;
    virtual const SP_Vector2DDouble& GetResultSDMatrix() = 0;
    virtual const SP_Vector2DDouble& GetSingleResultMatrix() = 0;
    virtual const SP_Vector2DDouble& GetResultFireCount() = 0;
	virtual const SP_Vector2DDouble& GetSingleResultFireCount() = 0;

	unsigned long GetResultPointCount() { return m_nResultPointCount; }
	virtual long GetCurrentSimulationRunCount(){return m_nCurrentSimulationRunNumber;}
    long GetPlaceCount() { return m_nPlaceCount; }
    long GetTransitionCount() { return m_nTransitionCount; }

    double GetSimulationCurrentTime(){return m_nCurrentTime;}

    void SetSimulationRunCount(unsigned long p_nSimRunCount){ m_nSimulationRunCount = p_nSimRunCount; }
    long GetSimulationRunCount() { return m_nSimulationRunCount; }

    void SetIntervalStart( double p_nTimePoint ) { m_nIntervalStart = p_nTimePoint; }
    double GetIntervalStart() { return m_nIntervalStart;}

    void SetIntervalEnd( double p_nTimePoint ) { m_nIntervalEnd = p_nTimePoint; }
    double GetIntervalEnd() { return m_nIntervalEnd;}

    void SetResultPointCount(unsigned long p_nResultPointCount )
	{
		m_nResultPointCount = p_nResultPointCount;
    	if(m_nIntervalStart < m_nIntervalEnd)
    	{
    		m_nIntervalSize = ( m_nIntervalEnd - m_nIntervalStart ) / ( p_nResultPointCount - 1 );
    	}
    	else
    	{
    		m_nIntervalSize = ( m_nIntervalEnd ) / ( p_nResultPointCount - 1 );
    	}
    }
	double GetIntervalSize() { return m_nIntervalSize; }

    void UpdateSimulationDialog(unsigned long p_nSimulationRun);

	// colored stochatic Petri Nets
	// set unfolding functions

	virtual void SetMemoryFree() = 0;

	void SetUnfoldedNet(SP_DS_ColPN_Unfolding* p_pcUnfolding)
	{
		m_pcUnfoldedNet = p_pcUnfolding;
	}

	void SetUnFoldingInformation();


	void SetActiveList(map<wxString, int>*  p_pmColActiveList) 
	{ 
		m_pmColActiveList = p_pmColActiveList; 
	}


	SP_MapString2Long* GetPlaceArrayPos() { return &m_mnPlaceArrayPos; }

    SP_VectorString* GetPlaceNames() { return &m_msPlaceNames; }

    SP_MapString2String* GetPlaceNamesById() { return &m_msPlaceNamesById; }

    SP_MapString2Long* GetTransitionArrayPos() { return &m_mnTransitionArrayPos; }

    SP_VectorString* GetTransitionNames() { return &m_msTransitionNames; }

    SP_MapString2String* GetTransitionNamesById() { return &m_msTransitionNamesById; }

	
	void SetPlaceArrayPos(SP_MapString2Long &p_mnPlaceArrayPos)
	{
		m_mnPlaceArrayPos = p_mnPlaceArrayPos;
	}
	

	void SetPlaceNames(SP_VectorString &p_msPlaceNames)
	{
		m_msPlaceNames = p_msPlaceNames;
	}

	void SetPlaceNamesById(SP_MapString2String &p_msPlaceNamesById)
	{
		m_msPlaceNamesById = p_msPlaceNamesById;
	}

	void SetNetMarkings(vector<SP_VectorLong> &p_anNetMarkings)
	{
		m_anNetMarkings = p_anNetMarkings;
	}

	
	
	void SetTransitionNames(SP_VectorString &p_msTransitionNames)
	{
		m_msTransitionNames = p_msTransitionNames;
	}

	void SetTransitionArrayPos(SP_MapString2Long &p_mnTransitionArrayPos)
	{
		m_mnTransitionArrayPos = p_mnTransitionArrayPos;
	}

	void SetTransitionNamesById(SP_MapString2String &p_msTransitionNamesById)
	{
		m_msTransitionNamesById = p_msTransitionNamesById;
	}

	void SetNetFunctions(vector<SP_VectorString> &p_anNetFunctions)
	{
		m_anNetFunctions = p_anNetFunctions;
	}


	void SetPrePlaceArrayPosOfTransitionNode(map< long, SP_MapString2Long > &p_mmPrePlaceArrayPosOfTransitionNode)
	{
		m_mmPrePlaceArrayPosOfTransitionNode = p_mmPrePlaceArrayPosOfTransitionNode;
	}	
	
	const map< long, SP_MapString2Long >&
	GetPrePlaceArrayPosOfTransitionNode()
	{
		return m_mmPrePlaceArrayPosOfTransitionNode;
	}

	void SetTransitionCountByNodeClass(map<wxString, SP_CPN_CountInterval> &p_nsTransitionCountByNodeClass)
	{
		m_nsTransitionCountByNodeClass = p_nsTransitionCountByNodeClass;
	}
	

	void SetDeltaIncMatrix(SP_MatrixDouble* &p_aanDeltaIncMatrix)
	{
		m_aanDeltaIncMatrix = p_aanDeltaIncMatrix;
	}

	void SetResetIncMatrix(SP_MatrixBool* &p_aanResetIncMatrix)
	{
		m_aanResetIncMatrix = p_aanResetIncMatrix;
	}

	void SetPreIncMatrix(SP_MatrixPairDouble* &p_aanPreIncMatrix)
	{
		m_aanPreIncMatrix = p_aanPreIncMatrix;
	}

	unsigned long GetGeneratedResultPointCount(){return m_nGeneratedResultPointsCount;}

};

#endif //__SP_DS_SIMULATION_H__
