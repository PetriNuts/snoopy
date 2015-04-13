//////////////////////////////////////////////////////////////////////
// $Author:  slehrack&Liu $
// $Version: 0.2 $
// $Date: 2009/3/13 $
//////////////////////////////////////////////////////////////////////

#ifndef __SP_EXPORTSTOCHPED2EXTPT_H__
#define __SP_EXPORTSTOCHPED2EXTPT_H__

#include "export/SP_ExportRoutine.h"
#include "sp_core/SP_XmlWriter.h"

class SP_ExportStochPed2ExtPT:  public SP_ExportRoutine, private SP_XmlWriter
{

 private:
	 long m_nTransitionNumber;
	 unsigned m_nTran;
	 unsigned m_nImmediateTran;
	 unsigned m_nDeterministicTran;
	 unsigned m_nScheduledTran;
	 wxXmlNode* m_pcTransitionNodeclass;

 protected:

  SP_MDI_Doc* m_doc;
  SP_DS_Graph* m_graph;
  wxString m_fileName;
  
  virtual bool WriteNetclass(SP_DS_Netclass* p_pcVal, wxXmlNode* p_pcRoot);
  virtual bool WriteNodeclass(SP_DS_Nodeclass* p_pcVal, wxXmlNode* p_pcRoot);
  virtual bool WriteEdgeclass(SP_DS_Edgeclass* p_pcVal, wxXmlNode* p_pcRoot);
  //Added by Laarz 16.10.2012
  virtual bool WriteMetadataclass(SP_DS_Metadataclass* p_pcVal, wxXmlNode* p_pcRoot);
 
  //Changed by Laarz 16.10.2012
  //bool WritePlace( SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot);
  bool WriteTransition( SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot);
  //Added by Laarz 16.10.2012
  bool WriteMetadata(SP_DS_Metadata* p_pcVal, wxXmlNode* p_pcRoot);

 public:

  virtual bool AcceptsDoc(SP_MDI_Doc*);
  virtual bool Write(SP_MDI_Doc*, const wxString&);

  inline virtual wxString GetName()
    { return wxT("Export to Extended Petri Net");}
  inline virtual wxString GetDescr()
    { return wxT("Export a Stochastic Petri Net into Extended PT Net Format");}
  virtual wxString GetExtension()
    { return wxT("xpn");}
  virtual wxString GetFileDescr()
    { return wxT("Extended PT Net Files");}

  public :

     
     
    

};

#endif // __SP_EXPORTSTOCHPED2EXTPT_H__

