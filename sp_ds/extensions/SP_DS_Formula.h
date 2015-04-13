/*
 * SP_DS_Formula.h
 *
 *  Created on: Mar 24, 2010
 *      Author: crohr
 */

#ifndef SP_DS_FORMULA_H_
#define SP_DS_FORMULA_H_

#include "sp_core/tools/rfdag.h"
#include "sp_utilities.h"
#include "sp_ds/SP_DS_Graph.h"

class SP_DS_Formula {
public:
	SP_DS_Formula(SP_DS_Graph* p_pcGraph, const rfdag::RFDAG& p_ExpressionTree);
	virtual ~SP_DS_Formula();

	virtual bool Initialise() = 0;

	virtual double Evaluate(const SP_VectorLong* p_Marking) = 0;

	rfdag::RFDAG GetExpressionTree() { return m_Expression; }

protected:
	SP_DS_Graph* m_pcGraph;
	rfdag::RFDAG m_Expression;

private:
/*
	SP_DS_Formula();
	SP_DS_Formula(const SP_DS_Formula&);
	virtual SP_DS_Formula& operator=(const SP_DS_Formula&);
*/
};



#endif /* SP_DS_FORMULA_H_ */
