//////////////////////////////////////////////////////////////////////
// $Author: ms $
// $Version: 0.1 $
// $Date: 2007/07/12 $
////////////////////////////////////////////////////////////////////
#ifndef __SP_ImportTINA_H__
#define __SP_ImportTINA_H__

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

class SP_ImportTINA: public SP_ImportRoutine,
	public SP_Error
{
 private:

	 long g_nIndex;
	 bool g_bError;
	 wxString g_sMessage;
	 map<wxString, SP_DS_Node*> m_mPlaces;
	 map<wxString, SP_DS_Node*> m_mTransitions;
	 map<wxString, SP_DS_Node*> m_mCoarse;

	 map<wxString, SP_Graphic*> m_mPlacesGr;
	 map<wxString, SP_Graphic*> m_mTransitionsGr;
	 map<wxString, SP_Graphic*> m_mCoarseGr;

	 bool Parse(wxTextFile& p_cFile);
	 bool ParseComment(wxString& p_sLine);
	 bool ParsePlace(wxString& p_sLine);
	 bool ParseTrans(wxString& p_sLine);
	 bool ParseArc(wxString& p_sLine);

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
	SP_ImportTINA();
	~SP_ImportTINA();

	virtual bool ReadFile(const wxString& p_sFile,SP_ImportRoutine* p_sIR=NULL);
	inline virtual SP_DS_Graph* ImportToDoc(const wxString& fileName)//by george
	{return NULL;}
	inline virtual wxString GetName()
		{ return wxT("Import TINA");}
	inline virtual wxString GetDescr()
		{ return wxT("Import a ndr.");}
	virtual wxString GetExtension()
		{ return wxT("TINA files (*.ndr)|*.ndr");}

};

#endif // __SP_ImportTINA_H__
