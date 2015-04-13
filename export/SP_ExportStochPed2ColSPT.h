//////////////////////////////////////////////////////////////////////
// $Author: fei liu $
// $Version: 0.1 $
// $Date: 2009/11/9 $
//////////////////////////////////////////////////////////////////////
#ifndef __SP_ExportStochPed2ColSPT_H__
#define __SP_ExportStochPed2ColSPT_H__

#include "sp_core/SP_XmlWriter.h"
#include "export/SP_ExportRoutine.h"

class SP_ExportStochPed2ColSPT: public SP_XmlWriter, public SP_ExportRoutine
{
  private:
	  SP_DS_Graph* m_pcGraph;

  protected:

    virtual bool WriteNetclass( SP_DS_Netclass* p_pcVal, wxXmlNode* p_pcRoot);
    virtual bool WriteNodeclass( SP_DS_Nodeclass* p_pcVal, wxXmlNode* p_pcRoot);
	virtual bool WriteEdgeclass(SP_DS_Edgeclass* p_pcVal, wxXmlNode* p_pcRoot);
	
    bool WriteExtPlace( SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot);
    bool WriteExtTransition( SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot);
	bool WriteExtEdge( SP_DS_Edge* p_pcVal, wxXmlNode* p_pcRoot);
	
	bool WriteImmediateTransition(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot );
	bool WriteDerterministicTransition(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot );
	bool WriteScheduledTransition(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot );


  public:

    virtual bool AcceptsDoc( SP_MDI_Doc* );


    virtual bool Write( SP_MDI_Doc*, const wxString& );

    inline virtual wxString GetName()
      { return wxT("Export to Colored Stochastic Petri Net"); }

    inline virtual wxString GetDescr()
      { return wxT("Export an Stochastic Petri Net into Colored Stochastic Petri Net Format"); }

    virtual wxString GetExtension()
      { return wxT("colspn"); }

    virtual wxString GetFileDescr()
      { return SP_DS_COLSPN_CLASS; }

};

#endif // __SP_ExportStochPed2ColSPT__


