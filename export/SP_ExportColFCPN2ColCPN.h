//////////////////////////////////////////////////////////////////////
// $Author: G. Assaf $
// $Version: 0.2 $
// $Date: 2020/3/14 $
//////////////////////////////////////////////////////////////////////

#ifndef __SP_ExportColFCPN2ColCPN_H__
#define __SP_ExportColFCPN2ColCPN_H__

#include "export/SP_ExportRoutine.h"
#include "sp_core/SP_XmlWriter.h"


class SP_ExportColFCPN2ColCPN : public SP_ExportRoutine, private SP_XmlWriter
{

private:
	long m_nTransitionNumber;
	wxXmlNode* m_pcTransitionNodeclass;

	long m_nPlaceNumber;
	wxXmlNode* m_pcPlaceNodeclass;

	wxString m_edgeClass;
	bool m_changeColour;
	SP_VectorDouble   GetFNConstants(wxString &val);
	wxString          GetCrispValue(wxString& param, wxString& type);

protected:

	SP_MDI_Doc* m_doc;
	SP_DS_Graph* m_graph;
	wxString m_fileName;

	virtual bool WriteNetclass(SP_DS_Netclass* p_pcVal, wxXmlNode* p_pcRoot);
	//virtual bool WriteNodeclass(SP_DS_Nodeclass* p_pcVal, wxXmlNode* p_pcRoot);
	//virtual bool WriteEdgeclass(SP_DS_Edgeclass* p_pcVal, wxXmlNode* p_pcRoot);

	//virtual bool WriteEdge(SP_DS_Edge* p_pcVal, wxXmlNode* p_pcRoot);

	//bool WritePlace(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot);
	//bool WriteTransition(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot);
	//bool WriteParameter(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot);


	//virtual bool WriteColourInformation(SP_Graphic* p_pcVal, wxXmlNode* p_pcNode);
	//virtual bool WriteEdgeGraphic(SP_GR_Edge* p_pcVal, wxXmlNode* p_pcRoot);
	virtual bool WriteMetadataclass(SP_DS_Metadataclass* p_pcVal, wxXmlNode* p_pcRoot);


public:

	virtual bool AcceptsDoc(SP_MDI_Doc*);
	virtual bool Write(SP_MDI_Doc*, const wxString&);

	inline virtual wxString GetName()
	{
		return wxT("Export to Colored Continuous Petri Net");
	}

	inline virtual wxString GetDescr()
	{
		return wxT("Export a Colored Fuzzy Continuous Petri Net into Colored Continuous Petri Net Format");
	}

	virtual wxString GetExtension()
	{
		return wxT("colcpn");
	}

	virtual wxString GetFileDescr()
	{
		return wxT("Colored Continuous Petri Net Files");
	}

public:





};

#endif // __SP_ExportColFCPN2ColCPN_H__

