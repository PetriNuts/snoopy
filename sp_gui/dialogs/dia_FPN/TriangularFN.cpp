
#include "TriangularFN.h"
#include <iomanip>
#include<math.h>
using namespace std;
//#<include"bits/stdc++.h>"
#include "wx/wx.h"

TriangularFN::TriangularFN(double dLef, double dRight, double dMiddle) :FuzzyBasics(dLef, dRight)
{

	m_dMiddlePoint = dMiddle;
	m_type = Triangular;
}

AlphaCutSet TriangularFN::getAlphaCutSet(double dAlphaLevel, int iSample)
{
	AlphaCutSet vdset;
	double dstart;
	double dend;
	if (dAlphaLevel > 0.0 && dAlphaLevel < 1.0)
	{
		dstart = m_dPointleft + dAlphaLevel * (m_dMiddlePoint - m_dPointleft);
		dend = m_dPointright - dAlphaLevel * (m_dPointright - m_dMiddlePoint);

	}
	if (dAlphaLevel == 1.0)
	{
		dstart = m_dMiddlePoint;
		dend = m_dMiddlePoint;
		vdset.push_back(dstart);
		return vdset;
	}
	if (dAlphaLevel == 0.0)
	{
		dstart = m_dPointleft;
		dend = m_dPointright;
		

	}
	double dstepSize = (dend - dstart) / (iSample - 1);

	for (double m = dstart; m <= dend; m = m + dstepSize)
	{
		vdset.push_back(m);
	}
	if(vdset.size()!= iSample)
	{
		vdset.push_back(dend);
	}

	return vdset;
}


double TriangularFN::getMembershipValue(double dVal)
{
	double dResult = 0.0;
	if (dVal < m_dPointleft)
		return 0;
	else if (dVal < m_dMiddlePoint)
	{
		dResult = (dVal - m_dPointleft) / (m_dMiddlePoint - m_dPointleft);
	}

	else if (dVal == m_dMiddlePoint)
		dResult = 1.0;
	else if (dVal < m_dPointright)
	{
		dResult = (m_dPointright - dVal) / (m_dMiddlePoint - dVal);
	}
	else
		dResult = 0.0;
	return dResult;

}
