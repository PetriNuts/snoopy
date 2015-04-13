//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: MD $
// $Version:  $
// $Revision:  $
// $Date:  $
// Short Description: writes a configuration to an xml-file
//////////////////////////////////////////////////////////////////////
#ifndef __SP_IA_XMLWRITER_H__
#define __SP_IA_XMLWRITER_H__

#include <wx/xml/xml.h>
#include "sp_core/SP_Writer.h"
#include "sp_gui/interaction/SP_IA_Manager.h"

class SP_IA_XMLWriter: public SP_Writer
{
private:
	SP_IA_Manager *m_pcMgr;
	SP_PairString m_pcNets;

protected:
    void Init();
    bool WriteNets(wxXmlNode* p_pcRoot);
    bool WriteConnection(wxXmlNode* p_pcRoot, const wxString& p_sSource);

public:
    SP_IA_XMLWriter();
    virtual ~SP_IA_XMLWriter();

	bool Write(SP_DS_Graph* p_pcGraph, const wxString& p_sFile) { return false; }
    bool Write(const SP_PairString& p_pcNets, const wxString& p_sFile);
};

#endif // __SP_IA_XMLWRITER_H__

