//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: kw $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Vector Reader, reads vectors of transitions
// 		or places
//////////////////////////////////////////////////////////////////////
#ifndef __SP_VECTORWRITER_H__
#define __SP_VECTORWRITER_H__

#include "sp_core/base/SP_Error.h"
#include "sp_core/vector/SP_VectorWriter.h"
#include "sp_ds/SP_DS_NodeVectorSystem.h"
#include "sp_ds/SP_DS_NodeVector.h"
#include "sp_ds/SP_DS_Visitor.h"

#include <wx/ffile.h>


class SP_VectorWriter: public SP_Error, public SP_DS_Visitor
{
private:
//	SP_VectorParser *m_pcParser;
//	SP_VectorScanner *m_pcScanner;
//	SP_DS_Graph *m_pcGraph;
//	SP_MapInt2Int *m_pcMapNodeNum2VecNum;
  //fstream m_fFile;
  wxFFile m_fFile;
  bool m_bOccurrencesWritten;
  SP_DS_NodeVectorSystem* m_pcNvs;
  SP_ListNode::const_iterator m_nodeIt;


protected:
public:
    SP_VectorWriter();
    virtual ~SP_VectorWriter();

    virtual bool Write(SP_DS_NodeVectorSystem *p_pcNvs, const wxString& p_sFile);

	  bool Visit(SP_DS_NodeVector * p_pcNodeVector);

};

#endif // __SP_VECTORWRITER_H__
