/*
* SP_ExportHPN2CPN.h
* $Author: George Assaf
* $Version: 0.0 $
* $Revision: 0.0 $
* $Date: 26.1.2019
* Short Description: export FCPN ==> CPN
*/
//======================================================================================
#ifndef SP_EXPORTFCPN2CPN_H_
#define SP_EXPORTFCPN2CPN_H_

#include "sp_core/SP_XmlWriter.h"
#include "export/SP_ExportRoutine.h"

class SP_ExportFCPN2CPN : public SP_XmlWriter, public SP_ExportRoutine
{
private:
	unsigned long m_nPlaceCount;
	unsigned long m_mTransitionCount;
	SP_DS_Metadataclass* m_pcMetaDataConstants;

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

	virtual bool Write(SP_MDI_Doc*, const wxString&);
	inline virtual wxString GetName()
	{
		return wxT("Export to Continuous Petri Net");
	}
	inline virtual wxString GetDescr()
	{
		return wxT("Export to Continuous Petri Nets format");
	}
	virtual wxString GetExtension()
	{
		return wxT("cpn");
	}
	virtual wxString GetFileDescr()
	{
		return wxT("Continuous Petri Net Files");
	}

 virtual SP_VectorDouble   GetFNConstants(wxString &val);
 virtual  wxString  GetCrispValue(wxString& param, wxString& type);
 
};

#endif /* SP_EXPORTFCPN2CPN_H_ */
