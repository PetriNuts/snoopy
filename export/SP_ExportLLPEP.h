//////////////////////////////////////////////////////////////////////
// $Author: alextov $
// $Version: 0.1 $
// $Date: 2005/10/06 $
// Short Description: export to PEP low level format
//////////////////////////////////////////////////////////////////////
#ifndef __SP_EXPORTLLPEP__
#define __SP_EXPORTLLPEP__

#include "export/SP_ExportPT.h"

typedef map<long, unsigned> IdNrMap;

class SP_ExportLLPEP: public SP_ExportPT
{
 protected:
  // maps to store assigned node numbers
  IdNrMap m_placeIdNrMap;
  IdNrMap m_trIdNrMap;

  bool WritePlaces();
  bool WriteTransitions();	
  bool WriteArcs();	
  virtual bool DoWrite();

 public:
  SP_ExportLLPEP();
  virtual ~SP_ExportLLPEP();

  virtual bool AcceptsDoc(SP_MDI_Doc*);
	
  inline virtual wxString GetName()
    { return wxT("Export to PEP (low level, without layout)");}
  inline virtual wxString GetDescr()
    { return wxT("Export a Petri Net in low level PEP Format");}
  virtual wxString GetExtension()
    { return wxT("ll_net");}
  virtual wxString GetFileDescr()
    { return wxT("PEP low level Files");}


  public :

    
    
    

};

#endif // __SP_EXPORTLLPEP__

