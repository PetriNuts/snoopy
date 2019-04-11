/*
*@Author=George Assaf
*Data structure for storing Membership functions of places over time
*/
#include "SP_TfnMembershipList.h"

SP_TfnMembershipList::SP_TfnMembershipList()
{
	m_tfnList.clear();
	m_lplaceNum = 0;
}
SP_TfnMembershipList::SP_TfnMembershipList(Membership_List list)
{
	m_tfnList = list;
	m_lplaceNum = list.size();
}
SP_TfnMembershipList::~SP_TfnMembershipList()
{
	m_tfnList.clear();
	m_lplaceNum = 0;
}
void SP_TfnMembershipList::AddToTFNList(const long& place, const Time2Membership& membership)
{
	m_tfnList[place] = membership;
}
Time2Membership SP_TfnMembershipList::GetTimeMembershipofPlace(long& dataCol)
{
	Time2Membership membership;
	if (m_tfnList.find(dataCol) != m_tfnList.end())
	{
		  membership=m_tfnList[dataCol];
		  return membership;
	}
	return membership;

}

vector<vector<double>> SP_TfnMembershipList::GetMembershipAtTimePoint(const long& timepoint,const long& dataCol)
{
	std::vector<vector<double>> membership;

	membership= m_tfnList[dataCol][timepoint];
	return membership;
}
/*Store Timed membership function */
bool SP_TfnMembershipList::StoreTFNMembership(const long dataColumn, const long timePoint, vector<vector<double>> membership)
{
	
		if (m_tfnList.find(dataColumn) != m_tfnList.end())
		{
			
			m_tfnList[dataColumn].insert(std::pair<long, vector<vector<double>>>(timePoint, membership));
				
			
			return true;
		}
    	else {
		map<long, vector<vector<double>>> mem_map;
		mem_map[timePoint] = membership;
		m_tfnList[dataColumn]= mem_map;
		m_lplaceNum++;
		return true;
         	}
	 
	return false;
	 
}
