//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: kw $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Vector Reader, reads vectors of transitions
// 		or places
//////////////////////////////////////////////////////////////////////
#ifndef __SP_VECTORREADER_H__
#define __SP_VECTORREADER_H__

#include "sp_core/base/SP_Error.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_core/vector/SP_VectorScanner.h"
#include "sp_core/vector/SP_VectorParser.h"
#include "sp_core/vector/SP_VectorReader.h"
#include "sp_ds/SP_DS_NodeVectorSystem.h"



class SP_VectorReader: public SP_Error
{
private:
	SP_DS_Graph *m_pcGraph;

protected:
public:
    SP_VectorReader(SP_DS_Graph* p_pcGraph);
    virtual ~SP_VectorReader();

    virtual bool Read(SP_DS_NodeVectorSystem *p_pcNvms, const wxString& p_sFile);


};

#endif // __SP_VECTORREADER_H__
