
/*
* SP_ExportHPN2CPN.cpp
* $Author: George Assaf
* $Version: 0.0 $
* $Revision: 0.0 $
* $Date: 28.01.2019
* Short Description:FCPN===>FHPN
*/
//======================================================================================
#ifndef SP_EXPORTFCPN2FHPN_H_
#define SP_EXPORTFCPN2FHPN_H_

#include "sp_core/SP_XmlWriter.h"
#include "export/SP_ExportRoutine.h"
#include "sp_gr/attributes/SP_GR_ColListAttribute.h"

class SP_ExportFCPN2FHPN : public SP_XmlWriter, public SP_ExportRoutine
{
private:
	unsigned long m_nPlaceCount;
	unsigned long m_mTransitionCount;
	SP_MDI_Doc* m_doc;
	SP_DS_Graph* m_graph;
	SP_DS_Metadataclass* m_pcMetaDataConstants;
	wxString m_fileName;
	wxXmlNode* m_pcElem;
protected:

	bool WriteNodeclass(SP_DS_Nodeclass* p_pcVal, wxXmlNode* p_pcRoot);
	bool WritePlace(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot);
	bool WriteTransition(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot);
	//bool WriteStochTransition(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot);
	bool WriteMetadata(SP_DS_Metadata *p_pcVal, wxXmlNode *p_pcRoot);

public:
	// must be implemented in successors
	virtual bool AcceptsDoc(SP_MDI_Doc*);

	virtual bool Write(SP_MDI_Doc*, const wxString&);
	inline virtual wxString GetName()
	{
		return wxT("Export to Fuzzy Hybrid Petri Net");
	}
	inline virtual wxString GetDescr()
	{
		return wxT("Export to Fuzzy Hybrid Petri Nets format");
	}
	virtual wxString GetExtension()
	{
		return wxT("fhpn");
	}
	virtual wxString GetFileDescr()
	{
		return wxT("Fuzzy Hybrid Petri Net Files");
	}
	//virtual wxString GetKenitikParam(wxString& func);

	//virtual bool IsParamFN(wxString& param);

};

#endif /* SP_EXPORTFCPN2FHPN_H_ */
