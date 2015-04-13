//////////////////////////////////////////////////////////////////////
// $Author: rr $
// $Version: 0.1 $
// $Date: 2008/01/07 $
// Short Description: place animator for Modulo Nets
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_MODNETPLACEANIMATOR_H__
#define __SP_DS_MODNETPLACEANIMATOR_H__

#include "sp_ds/animators/SP_DS_PlaceAnimator.h"

class SP_DS_ModNetPlaceAnimator: public SP_DS_PlaceAnimator
{
 private:
	 long m_nModToken;
 protected:
  virtual bool DecrementMark();
//  virtual bool TestMarking();
// bool TestMarking(const SP_ListEdge* p_edges,
//		   long p_tokens);

  long DecrementedMarking(const SP_ListEdge* p_edges,
			  long p_tokens);

  virtual unsigned int SumEdge(SP_DS_Edge* p_pcEdge);
 public:
  SP_DS_ModNetPlaceAnimator(const wxString& p_pchAttributeName,
			   SP_DS_Animation* p_pcAnim = NULL,
			   SP_DS_Node* p_pcParent = NULL);
  virtual ~SP_DS_ModNetPlaceAnimator();

  virtual SP_DS_Animator* Clone(SP_Data* p_pcParent,
				SP_DS_Animation* p_pcAnim);
};

#endif // __SP_DS_MODNETPLACEANIMATOR_H__


