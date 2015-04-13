//////////////////////////////////////////////////////////////////////
// $Author:  Liu $
// $Version: 0.2 $
// $Date: 2010/1/27 $
//////////////////////////////////////////////////////////////////////

#ifndef __SP_ExportColStPN2ColExPN_H__
#define __SP_ExportColStPN2ColExPN_H__

#include "export/SP_ExportRoutine.h"
#include "sp_core/SP_XmlWriter.h"

class SP_ExportColStPN2ColExPN:  public SP_ExportRoutine, private SP_XmlWriter
{

 private:
	 long m_nTransitionNumber;
	 wxXmlNode* m_pcTransitionNodeclass;
	 wxXmlNode* m_pcEdgeclass;

	 bool m_changeColour;
     wxString m_edgeClass;

 protected:

  SP_MDI_Doc* m_doc;
  SP_DS_Graph* m_graph;
  wxString m_fileName;
  
  virtual bool WriteNetclass(SP_DS_Netclass* p_pcVal, wxXmlNode* p_pcRoot);
  virtual bool WriteNodeclass(SP_DS_Nodeclass* p_pcVal, wxXmlNode* p_pcRoot);
  virtual bool WriteEdgeclass(SP_DS_Edgeclass* p_pcVal, wxXmlNode* p_pcRoot);  


 // virtual bool WriteColourInformation(SP_Graphic* p_pcVal,wxXmlNode* p_pcNode);

 

  bool WritePlace( SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot);
  bool WriteTransition( SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot);
  
 public:

  virtual bool AcceptsDoc(SP_MDI_Doc*);
  virtual bool Write(SP_MDI_Doc*, const wxString&);

  inline virtual wxString GetName()
    { return wxT("Export to Colored Extended Petri Net");}
  inline virtual wxString GetDescr()
    { return wxT("Export a Colored Stochastic Petri Net into Colored Extended PT Net Format");}
  virtual wxString GetExtension()
    { return wxT("colxpn");}
  virtual wxString GetFileDescr()
    { return wxT("Colored Extended PT Net Files");}

  public :

     
     
    

};

#endif // __SP_ExportColStPN2ColExPN_H__

