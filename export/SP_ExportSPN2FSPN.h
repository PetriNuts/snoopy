/*
* SP_ExportSPN2FSPN.h
* $Author: George Assaf
* $Version: 0.0 $
* $Revision: 0.0 $
* $Date: 25.01.2019
* Short Description:
*/
//======================================================================================
#ifndef SP_EXPORTSPN2FSPN_H_
#define SP_EXPORTSPN2FSPN_H_

#include "sp_core/SP_XmlWriter.h"
#include "export/SP_ExportRoutine.h"

class SP_ExportSPN2FSPN: public SP_XmlWriter, public SP_ExportRoutine
{
private:
	unsigned long m_nPlaceCount;
	unsigned long m_mTransitionCount;
	SP_MDI_Doc* m_doc;
	SP_DS_Graph* m_graph;
	wxString m_fileName;
	wxXmlNode* m_pcElem;
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
		return wxT("Export to Fuzzy Stochastic Petri Net");
	}
	inline virtual wxString GetDescr()
	{
		return wxT("Export to Fuzzy Stochastic Petri Nets format");
	}
	virtual wxString GetExtension()
	{
		return wxT("fspn");
	}
	virtual wxString GetFileDescr()
	{
		return wxT("Fuzzy Stochastic Petri Net Files");
	}
};

#endif /* SP_EXPORTSPN2FSPN_H_*/
