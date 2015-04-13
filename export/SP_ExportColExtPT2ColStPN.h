//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei $
// $Version: 0.0 $
// $Revision: 1.0 $
// $Date: 2010/01/28 12:20:00 $
//////////////////////////////////////////////////////////////////////
#ifndef __SP_ExportColExtPT2ColStPN_H__
#define __SP_ExportColExtPT2ColStPN_H__

#include "sp_core/SP_XmlWriter.h"
#include "export/SP_ExportRoutine.h"

class SP_ExportColExtPT2ColStPN: public SP_XmlWriter, public SP_ExportRoutine
{
  private:

  protected:

    virtual bool WriteNetclass( SP_DS_Netclass* p_pcVal, wxXmlNode* p_pcRoot);
    virtual bool WriteNodeclass( SP_DS_Nodeclass* p_pcVal, wxXmlNode* p_pcRoot);

    bool WriteStochPlace( SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot);
    bool WriteStochTransition( SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot);

  public:

    virtual bool AcceptsDoc( SP_MDI_Doc* );


    virtual bool Write( SP_MDI_Doc*, const wxString& );

    inline virtual wxString GetName()
      { return wxT("Export to Colored Stochastic Petri Net"); }

    inline virtual wxString GetDescr()
      { return wxT("Export an Colored Extended Petri Net into Colored Stochastic Petri Net Format"); }

    virtual wxString GetExtension()
      { return wxT("colspn"); }

    virtual wxString GetFileDescr()
      { return wxT("Colored Stochastic Petri Net Files"); }

};

#endif // __SP_ExportColExtPT2ColStPN_H__


