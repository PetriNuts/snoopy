//////////////////////////////////////////////////////////////////////
// $Author: CR $
// $Version: 0.1 $
// $Date: 2014/07/14 $
// Short Description: export to ssc
//////////////////////////////////////////////////////////////////////
#ifndef __SP_ExportColPN2SSC__
#define __SP_ExportColPN2SSC__

#include "export/SP_ExportPT.h"
#include "sp_ds/extensions/Col_SPN/SP_DS_ColPN_Unfolding.h"

class SP_ExportColPN2SSC: public SP_ExportPT
{
 protected:
  bool WritePlaces(std::shared_ptr<SP_DS_ColPN_Unfolding> p_Unfolding);
  bool WritePlaceClass(std::shared_ptr<SP_DS_ColPN_Unfolding> p_Unfolding, const SP_CPN_UnfoldedPlaces& p_pmmUnfoldedPlaces);
  bool WriteTransitions(std::shared_ptr<SP_DS_ColPN_Unfolding> p_Unfolding);
  bool WriteTransitionClass(std::shared_ptr<SP_DS_ColPN_Unfolding> p_Unfolding, const SP_CPN_UnfoldedTransitions& p_mmUnfoldedTransions);
  bool WriteParameters(std::shared_ptr<SP_DS_ColPN_Unfolding> p_Unfolding);
  bool WriteArcs(std::shared_ptr<SP_DS_ColPN_Unfolding> p_Unfolding, const vector<SP_CPN_UnfoldedArcInfo>& p_vArcs, wxString& p_sConditions, wxString& p_sUpdates, bool p_bInput);
  virtual bool DoWrite();

private:
	wxString m_sNetClass;

 public:
  SP_ExportColPN2SSC();
  virtual ~SP_ExportColPN2SSC();

  virtual bool AcceptsDoc(SP_MDI_Doc*);
	
  inline virtual wxString GetName()
    { return wxT("Export to SSC");}
  inline virtual wxString GetDescr()
    { return wxT("Export a Colored Petri Net in SSC Format");}
  virtual wxString GetExtension()
    { return wxT("rxn");}
  virtual wxString GetFileDescr()
    { return wxT("SSC Files");}

  public :

    
    
    

};

#endif // __SP_ExportColPN2SSC__

