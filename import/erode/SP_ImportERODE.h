/*
 * SP_ImportERODE.h
 *
 *  Created on: 30 Nov 2017
 *      Author: JCH
 */

#ifndef IMPORT_SP_IMPORTERODE_H_
#define IMPORT_SP_IMPORTERODE_H_

#include "import/SP_ImportRoutine.h"

class SP_ImportERODE : public SP_ImportRoutine {
	public:
		SP_ImportERODE() = default;
		virtual ~SP_ImportERODE() = default;

		virtual bool ReadFile(const wxString& fileName,SP_ImportRoutine* p_sIR=NULL);
		inline virtual SP_DS_Graph* ImportToDoc(const wxString& fileName)//by george
		{return NULL;}
		inline virtual wxString GetName()
			{ return wxT("Import ERODE");}
		inline virtual wxString GetDescr()
			{ return wxT("Import a ERODE.");}
		virtual wxString GetExtension()
			{ return wxT("ODE files (*.ode)|*.ode");}

	private:
//		SP_DS_Graph* m_pDsGraph;
//		SP_MDI_View* m_pMdiView;

		void doLayout(SP_MDI_Doc&);

};

#endif /* IMPORT_SP_IMPORTERODE_H_ */
