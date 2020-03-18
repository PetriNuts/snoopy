//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: George Assaf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2020/03/15 12:20:00 $
//////////////////////////////////////////////////////////////////////
#ifndef __SP_ExportColSPN2FSPN_H__
#define __SP_ExportColSPN2FSPN_H__

#include "sp_core/SP_XmlWriter.h"
#include "export/SP_ExportRoutine.h"
#include "sp_core/SP_Vector.h"

#include "export/SP_ExportColExtPT2ExtPT.h"

class SP_ExportColFSPN2FSPN : public SP_XmlWriter, public SP_ExportRoutine
{

private:

	SP_DS_Graph* m_pcExportGraph;

	map<wxString, SP_DS_Node*> m_mNewPlaceNodes;
	map<wxString, SP_DS_Node*> m_mNewTransitionNodes;

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
	SP_ExportColFSPN2FSPN();
	~SP_ExportColFSPN2FSPN();
	virtual bool AcceptsDoc(SP_MDI_Doc*);


	virtual bool Write(SP_MDI_Doc*, const wxString&);

	inline virtual wxString GetName()
	{
		return wxT("Export to fuzzy Stochastic Petri Net");
	}

	inline virtual wxString GetDescr()
	{
		return wxT("Export a Colored fuzzy Stochastic Petri Net into fuzzy Stochastic Petri Net Format");
	}

	virtual wxString GetExtension()
	{
		return wxT("fspn");
	}

	virtual wxString GetFileDescr()
	{
		return wxT("fuzzy Stochastic PT Net Files");
	}

};

#endif // __SP_ExportColSPN2FSPN_H__


