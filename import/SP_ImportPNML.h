//////////////////////////////////////////////////////////////////////
// $Author: cr $
// $Version: 0.1 $
// $Date: 2011/10/27 $
////////////////////////////////////////////////////////////////////
#ifndef __SP_IMPORTPNML_H__
#define __SP_IMPORTPNML_H__

#include "import/SP_ImportANDL.h"

class SP_DS_Graph;
class SP_DS_Edge;
class SP_DS_Node;
class SP_Graphic;


class SP_ImportPNML: public SP_ImportANDL
{

 public:
	SP_ImportPNML();
	virtual ~SP_ImportPNML();

	virtual bool ReadFile(const wxString& fileName);

	inline virtual wxString GetName()
		{ return wxT("Import PNML");}
	inline virtual wxString GetDescr()
		{ return wxT("Import a pnml.");}
	virtual wxString GetExtension()
		{ return wxT("PNML files (*.pnml)|*.pnml");}

};

#endif // __SP_IMPORTPNML_H__