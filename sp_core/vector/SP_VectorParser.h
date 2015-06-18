//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: kw $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Vector Reader, reads vectors of transitions
// 		or places
//////////////////////////////////////////////////////////////////////
#ifndef __SP_VECTORPARSER_H__
#define __SP_VECTORPARSER_H__

#include "sp_core/base/SP_Error.h"
#include "sp_core/vector/SP_VectorScanner.h"

enum SP_VECTOR_TYPE
{
	SP_TRANSITION_TYPE = 201,
	SP_PLACE_TYPE = 211,
	SP_NO_NODE_VECTOR = 212,
};

class SP_VectorParser: public SP_Error
{
private:
	SP_VectorScanner& m_pcScanner;
	wxString m_sVecSetInfo;
protected:
public:
    SP_VectorParser(SP_VectorScanner& p_pcScanner);
    ~SP_VectorParser();

	// searches for the next occurrence of "transition" or "place"
	// and returns this information
	// sets the parser to the next following "=" as initialisation
	// to read the vectors
	SP_VECTOR_TYPE ReadVectorTypeInformation();

	//virtual bool ReadVectorData();
	bool ReadNodeData(long& p_nNodeNumber, wxString& p_sNodeName, long& p_nVal);
	bool ReadVectorInformation(wxString& p_sName, wxString& p_sText);
	bool ReadVectorNumber(long &p_nNumber);
	bool ReadNodeName(wxString& p_sName);
	wxString GetVectorSetInformation();
};

#endif // __SP_VECTORPARSER_H__
