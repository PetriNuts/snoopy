////////////////
//
//$Author:G.Assaf
//fuzzy number defenition and its operations
//
//
//
////////////////
#ifndef SP_TriangularFN_H_
#define SP_TriangularFN_H_
#include "sp_gui/dialogs/dia_FPN/FuzzyBasics.h"
#include <math.h>

class TriangularFN : public FuzzyBasics
{
private:
	double m_dMiddlePoint;
public:
	TriangularFN() {}
	TriangularFN(double  dLeft, double dRight, double dMiddle);
	virtual ~TriangularFN() {};
	virtual void setMiddle(double pldLeft, double dRight)
	{
		m_dMiddlePoint = pldLeft;
	}
	virtual AlphaCutSet  getAlphaCutSet(double dalpha, int isample);
	virtual double getMembershipValue(double x);
	virtual double getMiddle() { return m_dMiddlePoint; }
};
#endif /* SP_TriangularFN_H_*/

