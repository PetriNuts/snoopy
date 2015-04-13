//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: md $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: XML Reader, reads graph structures
//////////////////////////////////////////////////////////////////////
#ifndef __SP_IA_XMLREADER_H__
#define __SP_IA_XMLREADER_H__

#include <wx/xml/xml.h>
#include "sp_core/SP_Reader.h"
#include "sp_gui/interaction/SP_IA_Manager.h"

class SP_IA_XMLReader: public SP_Reader
{
private:
		//first: netclass, second: filename
		SP_PairString m_pcNet1, m_pcNet2;
		wxArrayString m_asCommands1, m_asCommands2;

protected:
    void Init();

    bool CheckRoot(wxXmlNode* p_pcVal);
    bool ReadConnectedNets(wxXmlNode* p_pcNode);
	bool ReadNet(wxXmlNode* p_pcNode);
	bool ReadConnection(wxXmlNode* p_pcNode);

public:
    SP_IA_XMLReader();
    virtual ~SP_IA_XMLReader();

	bool Read(SP_DS_Graph* p_pcGraph, const wxString& p_sFile) { return false; }
    bool Read(const wxString& p_sFile);
	const SP_PairString& GetNet(bool p_bNet1);
	wxArrayString GetCommandNames(bool p_bNet1);
};

#endif // __SP_IA_XMLREADER_H__

