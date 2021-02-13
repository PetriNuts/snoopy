
/*/////////////////////////////////
*$Header file: $
*$Author: George Assaf $
*$Date: 2018$
*@Description: manuplating Fuzzy numbers e.g., obtain alpha cuts
*/
///////////////////////////////////
#ifndef SP_DLG_FUZZYBASICS_H_
#define SP_DLG_FUZZYBASICS_H_
#include <vector>
typedef std::vector<double>  VectorDouble;

typedef std::vector< VectorDouble >  DoubleMatrix;
typedef  std::vector<double> AlphaCutSet;
enum Ftype {
	TrapeZoidal,
	Triangular,
	GBell
};
struct TimedTFN {
	long l_timePoint;
	DoubleMatrix m_nTFNMemberShip;

};


class FuzzyBasics
{
protected:
	double m_dPointleft, m_dPointright;//interval of the number[a,d]
	Ftype  m_type;

public:
	FuzzyBasics() { };
	FuzzyBasics(double dPleft1, double dPright1) :m_dPointleft(dPleft1), m_dPointright(dPright1)
	{

	}
	virtual ~FuzzyBasics() = default;
	virtual void setInterval(double dleft, double dright)
	{
		m_dPointleft = dleft;
		m_dPointright = dright;
	}
	bool isPointinInterval(double p)
	{
		if ((p >= m_dPointleft) && (p <= m_dPointright))
			return true; else return false;
	}
	virtual double getLeft() { return m_dPointleft; }
	virtual double getRight() { return m_dPointright; }
	virtual void   setMiddle(double pl = 0, double pr = 0) = 0;
	virtual void setType(Ftype f) { m_type = f; }
	Ftype getType(void)const { return m_type; }
	virtual double getMembershipValue(double x) = 0;
	virtual AlphaCutSet getAlphaCutSet(double dAlpha, int iSample) = 0;

};
#endif /* SP_DLG_FUZZYBASICS_H_*/
