//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: slehrack $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2007/10/03 09:35:00 $
// Short Description: 
//////////////////////////////////////

#ifndef __SP_DS_STSIMIMMEDIATETRANSITIONLIST_H__
#define __SP_DS_STSIMIMMEDIATETRANSITIONLIST_H__

#include "sp_utilities.h"

class MTRand;

class SP_DS_StSimImmediateTransitionList
{

  public :

    SP_DS_StSimImmediateTransitionList();
    virtual ~SP_DS_StSimImmediateTransitionList();
    
    void InsertImmediateTransition(long p_nTransitionNumber, double m_nWeight);

    void RemoveElem(long p_nTransNumber);

    void Clear();

    size_t GetElemCount();
    
    long ChooseTransitionByWeight();

    long ChooseTransitionByUniformDistribution();

  protected : 

  private :
	MTRand* m_pcMTRand;

    SP_MapLong2Double m_mTrans;

    double m_Sum;

};

#endif
