//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei $
// $Version: 0.0 $
// $Revision: 1.0 $
// $Date: 2010/01/28 12:20:00 $
//////////////////////////////////////////////////////////////////////
#ifndef __SP_ExportColExtPT2ColPN_H__
#define __SP_ExportColExtPT2ColPN_H__

#include "sp_core/SP_XmlWriter.h"
#include "export/SP_ExportRoutine.h"

class SP_ExportColExtPT2ColPN: public SP_XmlWriter, public SP_ExportRoutine
{
  private:
	bool m_changeColour;
	wxString m_edgeClass;

  protected:
	virtual bool WriteNetclass(SP_DS_Netclass* p_pcVal, wxXmlNode* p_pcRoot);

	virtual bool WriteEdgeGraphic(SP_GR_Edge* p_pcVal, wxXmlNode* p_pcRoot);

	virtual bool WriteEdgeclass(SP_DS_Edgeclass* p_pcVal, wxXmlNode* p_pcRoot);

	virtual bool WriteEdge(SP_DS_Edge* p_pcVal, wxXmlNode* p_pcRoot);

	virtual bool WriteColourInformation(SP_Graphic* p_pcVal, wxXmlNode* p_pcNode);

  public:
	SP_ExportColExtPT2ColPN() {}
	virtual ~SP_ExportColExtPT2ColPN() {}

    virtual bool AcceptsDoc( SP_MDI_Doc* );

    virtual bool Write( SP_MDI_Doc*, const wxString& );

    inline virtual wxString GetName()
      { return wxT("Export to Colored Petri Net"); }

    inline virtual wxString GetDescr()
      { return wxT("Export an Colored Extended Petri Net into Colored Petri Net Format"); }

    virtual wxString GetExtension()
      { return wxT("colpn"); }

    virtual wxString GetFileDescr()
      { return wxT("Colored Petri Net Files"); }

};

#endif // __SP_ExportColExtPT2ColPN_H__


