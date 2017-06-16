//////////////////////////////////////////////////////////////////////
// $Author: cr $
// $Version: 0.1 $
// $Date: 2011/10/27 $
////////////////////////////////////////////////////////////////////
#ifndef __SP_IMPORTPNML2COLPN_H__
#define __SP_IMPORTPNML2COLPN_H__

#include "import/SP_ImportCANDL.h"

class SP_DS_Graph;
class SP_DS_Edge;
class SP_DS_Node;
class SP_Graphic;


class SP_ImportPNML2ColPN: public SP_ImportCANDL
{

 public:
	SP_ImportPNML2ColPN();
	virtual ~SP_ImportPNML2ColPN();

	virtual bool ReadFile(const wxString& fileName);

	inline virtual wxString GetName()
		{ return wxT("Import High-Level Petri Net PNML");}
	inline virtual wxString GetDescr()
		{ return wxT("Import a High-Level Petri Net from PNML.");}
	virtual wxString GetExtension()
		{ return wxT("PNML files (*.pnml)|*.pnml");}

};

#endif // __SP_IMPORTPNML2COLPN_H__
