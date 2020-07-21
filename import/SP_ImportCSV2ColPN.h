//////////////////////////////////////////////////////////////////////
// $Author: fei liu $
// $Version: 0.1 $
// $Date: 2010/05/26 $
////////////////////////////////////////////////////////////////////
#ifndef __SP_ImportCSV2ColPN_H__
#define __SP_ImportCSV2ColPN_H__

#include <wx/wx.h>
#include <wx/colordlg.h>
#include <wx/string.h>
#include <wx/file.h>
#include <wx/log.h>
#include <wx/app.h>
#include <wx/textfile.h>
#include <wx/cmdline.h>
#include <wx/dir.h>
#include <wx/regex.h>
#include <wx/utils.h>
#include <wx/tokenzr.h>
#include <wx/ffile.h>


#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gui/mdi/SP_MDI_View.h"
#include "import/SP_ImportRoutine.h"
#include "sp_core/SP_Core.h"
#include "snoopy.h"

class SP_ImportCSV2ColPN: public SP_ImportRoutine,
	public SP_Error
{
 private:

	SP_DS_Graph* m_pcGraph;
	bool ParseColorset(wxString& p_sLine);
	bool ParseConstant(wxString& p_sLine);
	bool ParseVariable(wxString& p_sLine);
	bool ParseFunction(wxString& p_sLine);

	bool ParseDeclarations(wxTextFile& p_cFile);
	bool ParsePlacesTransitions(wxTextFile& p_cFile);
	bool ParseParameters(wxTextFile& p_cFile);

	 bool g_bError;
	 wxString g_sMessage;
	
	 bool Parse(wxTextFile& p_cFile);
	 
	wxString errorString;
	wxString m_fileName;
	wxString netName;
	wxString currentToken;
	
	wxFFile m_fFile;


 protected:



 public:
	SP_ImportCSV2ColPN();
	~SP_ImportCSV2ColPN();

	virtual bool ReadFile(const wxString& p_sFile,SP_ImportRoutine* p_sIR=NULL);

	inline virtual wxString GetName()
		{ return wxT("Import Color Petri Net Declarations from CSV");}
	inline virtual wxString GetDescr()
		{ return wxT("Import a csv.");}
	virtual wxString GetExtension()
		{ return wxT("CSV files (*.csv)|*.csv");}

};

#endif // __SP_ImportCSV2ColPN_H__
