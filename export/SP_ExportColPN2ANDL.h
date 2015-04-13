//////////////////////////////////////////////////////////////////////
// $Author: CR $
// $Version: 0.1 $
// $Date: 2013/05/21 $
// Short Description: export to andl
//////////////////////////////////////////////////////////////////////
#ifndef __SP_ExportColPN2ANDL__
#define __SP_ExportColPN2ANDL__

#include "export/SP_ExportPT.h"
#include "sp_ds/extensions/Col_SPN/SP_DS_ColPN_Unfolding.h"

class SP_ExportColPN2ANDL: public SP_ExportPT
{
 protected:
  bool WritePlaces(Sh_ptr<SP_DS_ColPN_Unfolding> p_Unfolding);
  bool WritePlaceClass(Sh_ptr<SP_DS_ColPN_Unfolding> p_Unfolding, const SP_CPN_UnfoldedPlaces& p_pmmUnfoldedPlaces);
  bool WriteTransitions(Sh_ptr<SP_DS_ColPN_Unfolding> p_Unfolding);
  bool WriteTransitionClass(Sh_ptr<SP_DS_ColPN_Unfolding> p_Unfolding, const SP_CPN_UnfoldedTransitions& p_mmUnfoldedTransions);
  bool WriteParameters(Sh_ptr<SP_DS_ColPN_Unfolding> p_Unfolding);
  bool WriteArcs(Sh_ptr<SP_DS_ColPN_Unfolding> p_Unfolding, const vector<SP_CPN_UnfoldedArcInfo>& p_vArcs, wxString& p_sConditions, wxString& p_sUpdates, bool p_bInput);
  virtual bool DoWrite();

private:
	wxString m_sNetClass;

 public:
  SP_ExportColPN2ANDL();
  virtual ~SP_ExportColPN2ANDL();

  virtual bool AcceptsDoc(SP_MDI_Doc*);
	
  inline virtual wxString GetName()
    { return wxT("Export to ANDL (Charlie & Marcie)");}
  inline virtual wxString GetDescr()
    { return wxT("Export a Colored Petri Net in ANDL Format");}
  virtual wxString GetExtension()
    { return wxT("andl");}
  virtual wxString GetFileDescr()
    { return wxT("ANDL Files");}

  public :

    
    
    

};

#endif // __SP_ExportColPN2ANDL__

