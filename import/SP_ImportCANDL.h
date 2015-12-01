//////////////////////////////////////////////////////////////////////
// $Author: cr $
// $Version: 0.1 $
// $Date: 2011/12/14 $
////////////////////////////////////////////////////////////////////
#ifndef __SP_IMPORTCANDLH__
#define __SP_IMPORTCANDLH__

#include <wx/wx.h>
#include <wx/colordlg.h>
#include "sp_core/SP_List.h"
#include "sp_core/SP_Map.h"

#include "sp_core/base/SP_Error.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "import/SP_ImportRoutine.h"
#include <wx/ffile.h>
#include <wx/tokenzr.h>
#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gui/management/SP_GM_Docmanager.h"
#include "sp_gui/management/SP_GM_DocTemplate.h"

#include "dssz/misc/net.h"

class SP_DS_Graph;
class SP_DS_Edge;
class SP_DS_Node;
class SP_Graphic;


class SP_ImportCANDL: public SP_ImportRoutine,
	public SP_Error
{
public:

	struct Weights
	{
		SP_DS_Node* place_;
		SP_DS_Node* trans_;
		wxString vpt_; // PT weight met
		wxString vtp_; // TP weight met
		wxString vi_; // for inhibitor arcs
		wxString vr_; // for read arcs
		wxString ve_; // for equal arcs
		wxString reset_; // for reset arcs
		wxString modifier_; // for modifier arcs
		Weights():
			place_(NULL),
			trans_(NULL),
			vpt_(wxEmptyString),
			vtp_(wxEmptyString),
			vi_(wxEmptyString),
			vr_(wxEmptyString),
			ve_(wxEmptyString),
			reset_(wxEmptyString),
			modifier_(wxEmptyString)
		{}
	};

private:

	map<std::string, SP_DS_Node*> lookupPlaces;
	map<std::string, SP_DS_Node*> lookupParameters;
	map<std::string, SP_DS_Metadata*> lookupConstants;
	map<std::string, SP_DS_Metadata*> lookupColorsets;
	map<std::string, SP_DS_Metadata*> lookupVariables;
	map<std::string, SP_DS_Metadata*> lookupFunctions;
	map<std::string, SP_DS_Node*> lookupTransitions;
	map<pair<std::string, std::string>, Weights > arcs;

	void ClearAll();

	//file to be read
	wxString m_fileName;
	wxString netName;

	////////////////////////Parsing//////////////////////
	wxFFile m_fFile;

	dsszmc::andl::NetType m_eNetType;

	////////////////////////Graph Creation////////////////
	SP_MDI_View* m_pcView;
	SP_DS_Graph* m_pcGraph;
	SP_GUI_Canvas* m_pcCanvas;
	SP_MDI_Doc* m_pcMyDoc;

	double x;
	double y;

	//void CreateGraph();
	void CreateGraph(const wxString& p_sFile, const dsszmc::andl::Net& p_Net);
	void CreateEdge(SP_DS_Node* source, SP_DS_Node* target, const wxString& weight,const wxString& type);

	//SP_DS_Graph* CreateDocument();
	SP_DS_Graph* CreateDocument(const wxString& p_sFile, dsszmc::andl::NetType p_eType);
	bool CreatePlaces(const dsszmc::andl::Places& p_Places);
	bool CreateConst(const dsszmc::andl::Constants& p_Constants, const dsszmc::andl::Valuesets& p_Valuesets);
	bool CreateColorsets(const dsszmc::andl::Colorsets& p_Colorsets);
	bool CreateVariables(const dsszmc::andl::Variables& p_Variables);
	bool CreateFunctions(const dsszmc::andl::ColorFunctions& p_Functions);
	bool CreateObservers(const dsszmc::andl::Observers& p_Observers);
	bool CreateTransitions(const dsszmc::andl::Transitions& p_Transitions);
	void CreateArcs();
	void doLayout();


 protected:



 public:
	SP_ImportCANDL();
	virtual ~SP_ImportCANDL();

	virtual bool ReadFile(const wxString& fileName);

	inline virtual wxString GetName()
		{ return wxT("Import CANDL");}
	inline virtual wxString GetDescr()
		{ return wxT("Import a candl.");}
	virtual wxString GetExtension()
		{ return wxT("CANDL files (*.candl)|*.candl");}

};

#endif // __SP_IMPORTCANDLH__
