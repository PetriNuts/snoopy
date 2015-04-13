//////////////////////////////////////////////////////////////////////
// $Author: alextov $
// $Version: 0.1 $
// $Date: 2005/06/23 $
// Short Description: writes SimplePed from Extended PT net
//////////////////////////////////////////////////////////////////////
#ifndef __SP_EXPORTEXTPT2PED_H__
#define __SP_EXPORTEXTPT2PED_H__

#include "export/SP_ExportRoutine.h"
#include "sp_core/SP_XmlWriter.h"

class SP_ExportExtPT2Ped:  public SP_ExportRoutine, private SP_XmlWriter
{
 private:
  bool m_changeColour;
  wxString m_edgeClass;
 protected:
  virtual bool WriteNetclass(SP_DS_Netclass* p_pcVal,
			     wxXmlNode* p_pcRoot);

  virtual bool WriteEdgeGraphic(SP_GR_Edge* p_pcVal,
				wxXmlNode* p_pcRoot);

  virtual bool WriteEdgeclass(SP_DS_Edgeclass* p_pcVal,
			      wxXmlNode* p_pcRoot);

  virtual bool WriteEdge(SP_DS_Edge* p_pcVal,
				  wxXmlNode* p_pcRoot);

  virtual bool WriteColourInformation(SP_Graphic* p_pcVal,
				      wxXmlNode* p_pcNode);

 public:

  virtual bool AcceptsDoc(SP_MDI_Doc*);
  virtual bool Write(SP_MDI_Doc*, const wxString&);

  inline virtual wxString GetName()
    { return wxT("Export to Petri Net");}
  inline virtual wxString GetDescr()
    { return wxT("Export to Petri Net Format");}
  virtual wxString GetExtension()
    { return wxT("pn");}
  virtual wxString GetFileDescr()
    { return wxT("Discrete Petri Net Format");}


};

#endif // __SP_EXPORTEXTPT2PED_H__

