/*
 * SP_DS_ApParser.h
 *
 *  Created on: Mar 24, 2010
 *      Author: crohr
 */

#ifndef SP_DS_ILPARSER_H_
#define SP_DS_ILPARSER_H_

#include "sp_core/tools/rfdag.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/extensions/Col_SPN/SP_DS_ColPN_Unfolding.h"

//using namespace rfdag;

class SP_DS_ApParser
{
public:
	SP_DS_ApParser(SP_DS_Graph* p_pcGraph);
	

	virtual ~SP_DS_ApParser();

	virtual bool Initialise();

	void SetUnfoldedNet(SP_DS_ColPN_Unfolding* p_pcUnfolding)
	{
		m_pcUnfoldedNet = p_pcUnfolding;
	}

	virtual bool Parse(const wxString& p_ToParse);

	virtual rfdag::vectorRFDAG GetExpressions() { return m_Expressions; }

	virtual wxString GetFormulaString() { return m_FormulaString; }
protected:

	rfdag::vectorRFDAG m_Expressions;
	wxString m_FormulaString;
	SP_DS_Graph* m_pcGraph;
	SP_MapString2Long m_Placename2Id;
	SP_VectorStdString m_Id2Placename;
	SP_MapString2Long m_Parametername2Id;
	SP_VectorString m_Id2Parametername;

	SP_DS_ColPN_Unfolding* m_pcUnfoldedNet;

private:
	void LoadPlaces();
	void LoadUnfoldedPlaces();
	void LoadParameters();

	void SetNames(rfdag::RFDAG& p_Expression);

};

#endif /* SP_DS_ILPARSER_H_ */
