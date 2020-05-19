
/*
* SP_ExportHPN2CPN.cpp
* $Author: George Assaf
* $Version: 0.0 $
* $Revision: 0.0 $
* $Date: 28.01.2019
* Short Description:FSPN===>FCPN
*/
//======================================================================================
#ifndef SP_EXPORTFSPN2FCPN_H_
#define SP_EXPORTFSPN2FCPN_H_

#include "sp_core/SP_XmlWriter.h"
#include "export/SP_ExportRoutine.h"
#include "sp_gr/attributes/SP_GR_ColListAttribute.h"
#include "export/SpConvertElement.h"

class SP_ExportFSPN2FCPN : public SP_XmlWriter, public SP_ExportRoutine
{
private:
	unsigned long m_nPlaceCount;
	unsigned long m_mTransitionCount;
	SP_MDI_Doc* m_doc;
	SP_DS_Graph* m_graph;
	SP_DS_Metadataclass* m_pcMetaDataConstants;
	//wxString m_fileName;
	wxXmlNode* m_pcElem;
	SpConvertElement m_converter = SpConvertElement();

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
		return wxT("Export to Fuzzy Continuous Petri Net");
	}
	inline virtual wxString GetDescr()
	{
		return wxT("Export to Fuzzy Continuous Petri Nets format");
	}
	virtual wxString GetExtension()
	{
		return wxT("fcpn");
	}
	virtual wxString GetFileDescr()
	{
		return wxT("Fuzzy Continuous Petri Net Files");
	}
	virtual wxString GetKenitikParam(wxString& func);

	virtual bool IsParamFN(wxString& param);
 
};

#endif /* SP_EXPORTFSPN2FCPN_H_ */
