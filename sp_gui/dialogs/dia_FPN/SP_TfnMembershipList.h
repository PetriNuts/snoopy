
#ifndef _SP_TFN_LIST_H_
#define _SP_TFN_LIST_H_

#include <vector>
#include <map>
using namespace std;

 
typedef std::map<long, vector<vector<double>>> Time2Membership;
typedef std::map<long, Time2Membership> Membership_List;

class SP_TfnMembershipList {
	protected:
		long m_lplaceNum;
		Membership_List m_tfnList;
		 
   public:
	   SP_TfnMembershipList();
	   SP_TfnMembershipList(Membership_List list);
	   virtual ~SP_TfnMembershipList();
	   virtual void SetPlaceNum( long& num) { m_lplaceNum = num; }
	   virtual long GetPlacenum() { return m_lplaceNum; }
	   virtual void SetMembershipList(Membership_List list) { m_tfnList = list; }
	   virtual Membership_List GetMembershipList() { return m_tfnList; }

	   virtual void AddToTFNList(const long& place, const Time2Membership& membership);
	   virtual Time2Membership GetTimeMembershipofPlace(long& dataCol);
	   virtual vector<vector<double>> GetMembershipAtTimePoint(const long& timepointconst,const long& dataCol);
	   virtual bool StoreTFNMembership(const long dataColumn, const long timePoint, vector<vector<double>> membership);

};

#endif //_SP_TFN_LIST_H_
