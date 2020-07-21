/*
 * SP_ImportERODE.cpp
 *
 *  Created on: 30 Nov 2017
 *      Author: JCH
 */

//#include <wx/wx.h>

#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/SP_DS_Nodeclass.h"
#include "sp_ds/SP_DS_Node.h"
#include "sp_core/SP_Core.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gui/mdi/SP_MDI_View.h"
#include "sp_gui/management/SP_GM_Docmanager.h"
#include "sp_gui/management/SP_GM_DocTemplate.h"
#include "sp_gui/dialogs/SP_DLG_LayoutProperties.h"

#include "snoopy.h"

#include "import/erode/ErodeNet.h"
#include "import/erode/ErodeParser.h"
//#include "import/erode/IDocBuilder.h"
#include "import/erode/DocBuilder.h"

#include "import/erode/SP_ImportERODE.h"

/**
 *
 */
bool SP_ImportERODE::ReadFile(const wxString& fileName,SP_ImportRoutine* p_sIR) {

	sp::import::erode::ErodeParser erodeParser;

	std::string strFileName = fileName.ToStdString();

	erodeParser.parse(strFileName);

	sp::import::erode::DocBuilder docBuilder;

	SP_MDI_Doc& mdiDoc = docBuilder
		.appendConstants(erodeParser.getNet().m_vConstants)
//		.appendFunctions(erodeParser.getNet().m_vConstants)
		.appendPlaces(erodeParser.getNet().m_vPlaces)
		.appendTransitions(erodeParser.getNet().m_vTransitions)
		.build();


	// Set layout
	doLayout(mdiDoc);
	//


	return true;
}
/**
 *
 */
void SP_ImportERODE::doLayout(SP_MDI_Doc& mdiDoc) {

	SP_DLG_LayoutProperties *pDlgLayout = new SP_DLG_LayoutProperties(NULL, &mdiDoc);
	pDlgLayout->ShowModal();
	pDlgLayout->Destroy();
}

