//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: slehrack $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2007/10/01 12:20:00 $
//////////////////////////////////////////////////////////////////////
#ifndef __SP_EXPORTPED2STOCHPED_H__
#define __SP_EXPORTPED2STOCHPED_H__

#include "sp_core/SP_XmlWriter.h"
#include "export/SP_ExportRoutine.h"

class SP_ExportPed2StochPed: public SP_XmlWriter, public SP_ExportRoutine
{
  private:

  protected:

    virtual bool WriteNetclass( SP_DS_Netclass* p_pcVal, wxXmlNode* p_pcRoot);
    virtual bool WriteNodeclass( SP_DS_Nodeclass* p_pcVal, wxXmlNode* p_pcRoot);

    bool WriteStochTransition( SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot);

  public:

    virtual bool AcceptsDoc( SP_MDI_Doc* );


    virtual bool Write( SP_MDI_Doc*, const wxString& );

    inline virtual wxString GetName()
      { return wxT("Export to Stochastic Petri Net"); }

    inline virtual wxString GetDescr()
      { return wxT("Export a Discrete Petri Net into Stochastic Petri Net Format"); }

    virtual wxString GetExtension()
      { return wxT("spn"); }

    virtual wxString GetFileDescr()
      { return wxT("Stochastic Petri Net Files"); }

};

#endif // __SP_EXPORTPED2STOCHPED_H__


