#include "sp_gui/dialogs/dia_FPN/SP_Compressed_Fuzzy_Band.h"


SP_Compressed_Fuzzy_Band::SP_Compressed_Fuzzy_Band():SP_TfnMembershipList(),
m_minCurve(NULL),
m_maxCurve(NULL),
m_alphaLevels(NULL)
{
 

}
SP_Compressed_Fuzzy_Band::SP_Compressed_Fuzzy_Band(Membership_List list, vector<vector<double>> min, vector<vector<double>> max, vector<double> levels) :
	SP_TfnMembershipList(list),
	m_minCurve(min),
	m_maxCurve(max),
	m_alphaLevels(levels)
{

}

SP_Compressed_Fuzzy_Band::~SP_Compressed_Fuzzy_Band()
{
//	SP_TfnMembershipList::~SP_TfnMembershipList();
	//m_minCurve.clear();
	//m_maxCurve.clear();
	//m_alphaLevels.clear();
}