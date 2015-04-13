//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: liu $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2010/04/23 12:20:00 $
//////////////////////////////////////////////////////////////////////
#ifndef __SP_ExportColExtPT2CPNTool_H__
#define __SP_ExportColExtPT2CPNTool_H__

#include "sp_core/SP_XmlWriter.h"
#include "sp_ds/extensions/Col_PN/Transform/SP_CPNToolXmlWriter.h"
#include "export/SP_ExportRoutine.h"

#include "sp_core/SP_Vector.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"


class SP_ExportColExtPT2CPNTool:  public SP_ExportRoutine, private SP_Writer
{

  public:
    SP_ExportColExtPT2CPNTool();
	~SP_ExportColExtPT2CPNTool();
    virtual bool AcceptsDoc( SP_MDI_Doc* );


    virtual bool Write( SP_MDI_Doc*, const wxString& );

    inline virtual wxString GetName()
      { return wxT("Export to CPN tool"); }

    inline virtual wxString GetDescr()
      { return wxT("Export an Colored Extended/Stochastic Petri Net into CPN tool Format"); }

    virtual wxString GetExtension()
      { return wxT("cpn"); }

    virtual wxString GetFileDescr()
      { return wxT("CP Net Files"); }

};

#endif // __SP_ExportColExtPT2CPNTool_H__


