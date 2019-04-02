#ifndef _SP_COMPRESSED_FUZZY_BAND_H_
#define _SP_COMPRESSED_FUZZY_BAND_H_

#include "sp_gui/dialogs/dia_FPN/SP_TfnMembershipList.h"
#include <vector>

class SP_Compressed_Fuzzy_Band : public SP_TfnMembershipList
{
private:
	std::vector<vector<double>> m_minCurve;
	std::vector<vector<double>> m_maxCurve;
	std::vector<double> m_alphaLevels;

public:
	SP_Compressed_Fuzzy_Band();
	SP_Compressed_Fuzzy_Band( Membership_List list,vector<vector<double>> min, vector<vector<double>> max, vector<double> levels);
	virtual ~SP_Compressed_Fuzzy_Band();

	virtual void SetMinimumCurve(vector<vector<double>>& min) { m_minCurve = min; }
	virtual vector<vector<double>>& GetMinimumCurve() { return m_minCurve; }

	virtual void SetMaximumCurve(vector<vector<double>>& max) { m_maxCurve = max; }
	virtual vector<vector<double>> GetMaximumCurve() { return m_maxCurve; }

	virtual void SetAlphaLevels(vector<double>& levels) { m_alphaLevels = levels; }
	virtual vector<double> GetAlphaLevels() { return m_alphaLevels; }

	SP_Compressed_Fuzzy_Band operator=(SP_Compressed_Fuzzy_Band b)
	{
		SP_Compressed_Fuzzy_Band copy = SP_Compressed_Fuzzy_Band();
		copy.SetMinimumCurve(b.GetMinimumCurve());
		copy.SetMaximumCurve(b.GetMaximumCurve());
		copy.SetMembershipList(b.GetMembershipList());
		copy.SetAlphaLevels(b.GetAlphaLevels());
		return copy;
	}
};
#endif //_SP_COMPRESSED_FUZZY_BAND_H_