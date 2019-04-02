/*
 * SP_ExportHPN2SPN.h
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 04.02.2011
 * Short Description:
 */
//======================================================================================
#ifndef SP_EXPORTHPN2SPN_H_
#define SP_EXPORTHPN2SPN_H_

#include "sp_core/SP_XmlWriter.h"
#include "export/SP_ExportRoutine.h"
#include "export/SpConvertElement.h"

class SP_ExportHPN2SPN: public SP_XmlWriter, public SP_ExportRoutine
{
private:
	unsigned long m_nPlaceCount;
	unsigned long m_mTransitionCount;
	SP_MDI_Doc* m_doc;
	SP_DS_Graph* m_graph;
	wxString m_fileName;
	wxXmlNode* m_pcElem;
	SpConvertElement m_converter = SpConvertElement();

protected:

	bool WriteNodeclass(SP_DS_Nodeclass* p_pcVal, wxXmlNode* p_pcRoot);
	bool WritePlace(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot);
	bool WriteTransition(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot);

public:
	// must be implemented in successors
	virtual bool AcceptsDoc(SP_MDI_Doc*);

	virtual bool Write(SP_MDI_Doc*, const wxString&);
	inline virtual wxString GetName()
	{
		return wxT("Export to Stochastic Petri Net");
	}
	inline virtual wxString GetDescr()
	{
		return wxT("Export to Stochastic Petri Nets format");
	}
	virtual wxString GetExtension()
	{
		return wxT("spn");
	}
	virtual wxString GetFileDescr()
	{
		return wxT("Stochastic Petri Net Files");
	}
};

#endif /* SP_EXPORTHPN2SPN_H_ */
