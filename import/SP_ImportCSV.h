/*
 * SP_ImportCSV.h
 *
 *  Created on: April 26, 2012
 *      Author: crohr
 */

#ifndef SP_IMPORTCSV_H_
#define SP_IMPORTCSV_H_

#include "import/SP_ImportRoutine.h"
#include "sp_ds/SP_DS_Node.h"
#include "spsim/simulator.h"

class SP_ImportCSV: public SP_ImportRoutine, public SP_Error
{
private:
	SP_DS_Graph* m_pcGraph;
	wxString m_sClassname;
	wxString m_Separator;
	wxArrayString m_ArrayString;
	SP_MapString2ULong m_Place2Id;
	SP_MapString2ULong m_Trans2Id;
	SP_MapString2ULong m_Param2Id;
	SP_VectorLong m_Csv2Id;
	SP_Vector2DDouble m_ResultMatrix;
	unsigned long m_ColCount;
	unsigned long m_RowLength;
	double m_IntervalStart;
	double m_IntervalEnd;
	unsigned long m_OutputStepCount;

protected:

	void LoadPlaces();
	void LoadTransitions();
	void LoadParameters();

	bool ParseLine(const wxString& p_sValue);
	bool DoReadFile(const wxString& fileName);
	void Clear();

public:
	SP_ImportCSV();
	virtual ~SP_ImportCSV();

	virtual bool ReadFile(const wxString& fileName,SP_ImportRoutine* p_sIR=NULL);
	virtual bool ReadFile(const wxString& fileName, spsim::Simulator* simulator);
	inline virtual SP_DS_Graph* ImportToDoc(const wxString& fileName)//by george
	{return NULL;}

	inline virtual wxString GetName()
		{ return wxT("Import Simulation Results");}
	inline virtual wxString GetDescr()
		{ return wxT("Import simulation results from CSV file.");}
	virtual wxString GetExtension()
		{ return wxT("CSV files (*.csv)|*.csv");}


	void SetSeparator(const wxString& separator) { m_Separator = separator; }
	const SP_Vector2DDouble& GetResults() { return m_ResultMatrix; }
	double GetIntervalStart() { return m_IntervalStart; }
	double GetIntervalEnd() { return m_IntervalEnd; }
	unsigned long GetOutputStepCount() { return m_OutputStepCount-1; }
	const wxString& GetClassName() { return m_sClassname; }
};

#endif /* SP_IMPORTCSV2SPN_H_ */
