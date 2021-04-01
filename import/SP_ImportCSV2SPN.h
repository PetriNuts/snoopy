/*
 * SP_ImportCSV2SPN.h
 *
 *  Created on: May 26, 2010
 *      Author: crohr
 */

#ifndef SP_IMPORTCSV2SPN_H_
#define SP_IMPORTCSV2SPN_H_

#include "import/SP_ImportRoutine.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"

typedef std::map<wxString, SP_DS_Attribute*> SP_MapName2Attribute;
typedef std::map<wxString, SP_DS_ColListAttribute*> SP_MapName2ColList;
typedef std::map<wxString, SP_Vector2DString> SP_MapImportCSV;

class SP_ImportCSV2SPN: public SP_ImportRoutine, public SP_Error
{
private:
	SP_DS_Graph* m_pcGraph;
	wxString m_sClassname;
	SP_MapName2Attribute m_mPlaces;
	SP_MapName2ColList m_mTransitions;
	SP_MapName2ColList m_mParameters;

	SP_MapImportCSV m_mImportValues;

protected:

	void LoadPlaces();
	void LoadTransitions();
	void LoadParameters();

	void ParseLine(const wxString& p_sValue);
	void CheckImportValues();
	void SetImportValues();

public:
	SP_ImportCSV2SPN();
	virtual ~SP_ImportCSV2SPN();

	virtual bool ReadFile(const wxString& fileName,SP_ImportRoutine* p_sIR=NULL);

	inline virtual SP_DS_Graph* ImportToDoc(const wxString& fileName)//by george
	{return NULL;}

	inline virtual wxString GetName()
		{ return wxT("Import CSV to stochastic Petri Net");}
	inline virtual wxString GetDescr()
		{ return wxT("Import a CSV to stochastic Petri Net.");}
	virtual wxString GetExtension()
		{ return wxT("CSV files (*.csv)|*.csv");}
};

#endif /* SP_IMPORTCSV2SPN_H_ */
