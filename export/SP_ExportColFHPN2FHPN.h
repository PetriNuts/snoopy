//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: George Assaf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2020/03/15 12:20:00 $
//////////////////////////////////////////////////////////////////////
#ifndef __SP_ExportColFHPN2HybridPN_H__
#define __SP_ExportColFHPN2HybridPN_H__

#include "sp_core/SP_XmlWriter.h"
#include "export/SP_ExportRoutine.h"
#include "sp_core/SP_Vector.h"

class SP_ExportColFHPN2FHPN : public SP_XmlWriter, public SP_ExportRoutine
{

private:
	SP_DS_Graph* m_pcExportGraph;

	map<wxString, SP_DS_Node*> m_mNewPlaceNodes;
	map<wxString, SP_DS_Node*> m_mNewTransitionNodes;

	map<wxString, SP_DS_Node*> m_mNewContinuousPlaceNodes;
	map<wxString, SP_DS_Node*> m_mNewContinuousTransitionNodes;

	map<wxString, SP_DS_Node*> m_mNewImmediateTransitionNodes;
	map<wxString, SP_DS_Node*> m_mNewDeterministicTransitionNodes;
	map<wxString, SP_DS_Node*> m_mNewScheduledTransitionNodes;

	SP_ListEdge m_lNewEdges;

protected:

	virtual bool WriteNetclass(SP_DS_Netclass* p_pcVal, wxXmlNode* p_pcRoot);
	virtual bool WriteNodeclass(SP_DS_Nodeclass* p_pcVal, wxXmlNode* p_pcRoot);
	virtual bool WriteEdgeclass(SP_DS_Edgeclass* p_pcVal, wxXmlNode* p_pcRoot);
	virtual bool WriteEdge(SP_DS_Edge* p_pcVal, wxXmlNode* p_pcRoot);
	virtual bool WriteMetadataclass(SP_DS_Metadataclass* p_pcVal, wxXmlNode* p_pcRoot);

	bool WriteUnfoldedPlace(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot);
	bool WriteUnfoldedTransition(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot);

public:
	SP_ExportColFHPN2FHPN();
	~SP_ExportColFHPN2FHPN();
	virtual bool AcceptsDoc(SP_MDI_Doc*);


	virtual bool Write(SP_MDI_Doc*, const wxString&);

	inline virtual wxString GetName()
	{
		return wxT("Export to Fuzzy Hybrid Petri Net");
	}

	inline virtual wxString GetDescr()
	{
		return wxT("Export a  Colored Fuzzy Hybrid Petri Net into Fuzzy Hybrid Petri Net Format");
	}

	virtual wxString GetExtension()
	{
		return wxT("fhpn");
	}

	virtual wxString GetFileDescr()
	{
		return wxT("Fuzzy Hybrid PT Net Files");
	}

};

#endif // __SP_ExportColFHPN2HybridPN_H__


