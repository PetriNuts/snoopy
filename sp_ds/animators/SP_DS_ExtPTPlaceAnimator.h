//////////////////////////////////////////////////////////////////////
// $Author: alextov $
// $Version: 0.1 $
// $Date: 2005/06/29 $
// Short Description: place animator for Extended PT nets
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_EXTPTPLACEANIMATOR_H__
#define __SP_DS_EXTPTPLACEANIMATOR_H__

#include "sp_ds/animators/SP_DS_PlaceAnimator.h"

class SP_DS_ExtPTPlaceAnimator: public SP_DS_PlaceAnimator
{
 private:

 protected:
  virtual bool DecrementMark();
  bool TestMarking(const SP_ListEdge* p_edges,
		   long p_tokens);

  // Added by Erik Jongsma, changed to pass the transanimation pointer as well
  long DecrementedMarking(const SP_ListEdge* p_edges,
			  long p_tokens, SP_DS_TransAnimator* p_pcTrans);

  virtual unsigned int SumEdge(SP_DS_Edge* p_pcEdge);

  //by sl
  virtual bool MarkingEdge(SP_DS_Edge* p_pcEdge);

 public:
  SP_DS_ExtPTPlaceAnimator(const wxString& p_pchAttributeName,
			   SP_DS_Animation* p_pcAnim = NULL,
			   SP_DS_Node* p_pcParent = NULL);
  virtual ~SP_DS_ExtPTPlaceAnimator();

  virtual SP_DS_Animator* Clone(SP_Data* p_pcParent,
				SP_DS_Animation* p_pcAnim);

  virtual bool TestMarking();
  virtual bool ResolveConflicts();

  virtual bool BuildConflictList (list<SP_DS_TransAnimator*> &p_lConflictTransAnimators);
  virtual bool BuildAutoConflictList (list<SP_DS_TransAnimator*> &p_lConflictTransAnimators);
  virtual bool DecrementSimValue (SP_DS_TransAnimator* p_pcTrans);
  virtual bool DecrementSimValueAutoConcurrency (SP_DS_TransAnimator* p_pcTrans);

  //by sl
  bool OnRightClick();
  bool PreStep(unsigned int p_nSteps, SP_DIRECTION_T p_nDir);


};

#endif // __SP_DS_EXTPLACEANIMATOR_H__


