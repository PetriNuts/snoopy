/*
* SP_ExportFHPN2HPN.h
* $Author: George Assaf
* $Version: 0.0 $
* $Revision: 0.0 $
* $Date: 28.01.2019
* Short Description: export FHPN ===> HPN
*/
//======================================================================================
#ifndef SP_EXPORTFHPN2HPN_H_
#define SP_EXPORTFHPN2HPN_H_

#include "sp_core/SP_XmlWriter.h"
#include "export/SP_ExportRoutine.h"

class SP_ExportFHPN2HPN : public SP_XmlWriter, public SP_ExportRoutine
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
	bool WriteMetadata(SP_DS_Metadata *p_pcVal, wxXmlNode *p_pcRoot);


public:
	// must be implemented in successors
	virtual bool AcceptsDoc(SP_MDI_Doc*);
	virtual SP_VectorDouble   GetFNConstants(wxString &val);
	virtual  wxString  GetCrispValue(wxString& param, wxString& type);
	virtual bool Write(SP_MDI_Doc*, const wxString&);

	inline virtual wxString GetName()
	{
		return wxT("Export to Hybrid Petri Net");
	}
	inline virtual wxString GetDescr()
	{
		return wxT("Export to Hybrid Petri Nets format");
	}
	virtual wxString GetExtension()
	{
		return wxT("hpn");
	}
	virtual wxString GetFileDescr()
	{
		return wxT("Hybrid Petri Net Files");
	}


};

#endif /* SP_EXPORTFHPN2HPN_H_ */
