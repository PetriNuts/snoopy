//////////////////////////////////////////////////////////////////////
// $Author: F. Liu $
// $Version: 0.1 $
// $Date: 2011/05/24 $
// Short Description: export to apnn
//////////////////////////////////////////////////////////////////////
#ifndef __SP_ExportColPN2APNN__
#define __SP_ExportColPN2APNN__

#include "export/SP_ExportPT.h"
#include "sp_ds/extensions/Col_SPN/SP_DS_ColPN_Unfolding.h"

class SP_ExportColPN2APNN: public SP_ExportPT
{
 protected:
  bool WritePlaces();
  bool WriteTransitions();	
  bool WriteParameters();
  bool WriteArcs();	
  virtual bool DoWrite();

private:
	wxString m_sNetClass;
	SP_VectorString m_msPlaceNames;
	SP_VectorString m_msTransitionNames;
	SP_VectorLong m_anInitialNetMarking;
	SP_VectorDouble m_adInitialNetMarking;
	SP_VectorString m_msTransitionFunctions;

	vector<SP_CPN_ContinuousArc> m_vPreTransitionArc ;
	vector<SP_CPN_ContinuousArc> m_vPostTransitionArc ;


	bool Initialize();


 public:
  SP_ExportColPN2APNN();
  virtual ~SP_ExportColPN2APNN();

  virtual bool AcceptsDoc(SP_MDI_Doc*);
	
  inline virtual wxString GetName()
    { return wxT("Export to APNN ");}
  inline virtual wxString GetDescr()
    { return wxT("Export a Colored Petri Net in APNN Format");}
  virtual wxString GetExtension()
    { return wxT("apnn");}
  virtual wxString GetFileDescr()
    { return wxT("APNN Files");}

  public :

    
    
    

};

#endif // __SP_ExportColPN2APNN__

