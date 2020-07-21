//////////////////////////////////////////////////////////////////////
// $Author: ms $
// $Version: 0.1 $
// $Date: 2007/07/12 $
////////////////////////////////////////////////////////////////////
#ifndef __SP_IMPORTPED_H__
#define __SP_IMPORTPED_H__

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
#include "import/SP_ImportRoutine.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"

class SP_ImportPED: public SP_ImportRoutine,
	public SP_Error
{
 private:

	 long g_nIndex;
	 bool g_bError;
	 wxString g_sMessage;
	 map<long, SP_DS_Node*> m_mPlaces;
	 map<long, SP_DS_Node*> m_mTransitions;
	 map<long, SP_DS_Node*> m_mCoarse;

	 map<long, SP_Graphic*> m_mPlacesGr;
	 map<long, SP_Graphic*> m_mTransitionsGr;
	 map<long, SP_Graphic*> m_mCoarseGr;

	 bool Parse(wxTextFile& p_cFile);
	 bool ParseComment(wxTextFile& p_cFile);
	 bool ParsePlace(wxTextFile& p_cFile, wxString& p_sId);
	 bool ParseTrans(wxTextFile& p_cFile, wxString& p_sId);
	 bool ParseCoarse(wxTextFile& p_cFile, wxString& p_sId);
	 bool ParseArc(wxTextFile& p_cFile);

	 long ToLong(const wxString& p_sVal);
	//file to be read



	wxString errorString;
	wxStringTokenizer* stz;
	wxString m_fileName;
	wxString netName;
	wxString currentToken;



	bool Open(const wxString& p_sFile);
	bool Close();

	////////////////////////Parsing//////////////////////
	wxFFile m_fFile;

	////////////////////////Graph Creation////////////////
	SP_MDI_View* m_pcView;
	SP_DS_Graph* m_pcGraph;
	SP_DS_Graph* m_pcNewGraph;
	SP_GUI_Canvas* m_pcCanvas;
	SP_MDI_Doc* m_pcMyDoc;

	void CreateGraph(const wxString& p_sFile);
	SP_DS_Graph* CreateDocument(const wxString& p_sFile);


 protected:



 public:
	SP_ImportPED();
	virtual ~SP_ImportPED();

	virtual bool ReadFile(const wxString& fileName,SP_ImportRoutine* p_sIR=NULL);

	inline virtual wxString GetName()
		{ return wxT("Import PED");}
	inline virtual wxString GetDescr()
		{ return wxT("Import a ped.");}
	virtual wxString GetExtension()
		{ return wxT("PED files (*.ped)|*.ped");}

};

#endif // __SP_IMPORTPED_H__
