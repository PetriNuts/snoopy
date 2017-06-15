//////////////////////////////////////////////////////////////////////
// $Author: cr $
// $Version: 0.1 $
// $Date: 2011/10/27 $
////////////////////////////////////////////////////////////////////
#ifndef __SP_IMPORTANDLH__
#define __SP_IMPORTANDLH__

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

#include "dssd/misc/net.h"

class SP_DS_Graph;
class SP_DS_Edge;
class SP_DS_Node;
class SP_Graphic;


class SP_ImportANDL: public SP_ImportRoutine,
	public SP_Error
{
protected:

	struct Weights
	{
		SP_DS_Node* place_;
		SP_DS_Node* trans_;
		wxString vpt_; // PT weight met
		wxString vtp_; // TP weight met
		wxString vi_; // for inhibitor arcs
		wxString vr_; // for read arcs
		wxString ve_; // for equal arcs
		bool reset_; // for reset arcs
		bool modifier_; // for modifier arcs
		Weights():
			place_(NULL),
			trans_(NULL),
			vpt_(wxEmptyString),
			vtp_(wxEmptyString),
			vi_(wxEmptyString),
			vr_(wxEmptyString),
			ve_(wxEmptyString),
			reset_(false), modifier_(false)
		{}
	};

	map<std::string, SP_DS_Node*> lookupPlaces;
	map<std::string, SP_DS_Node*> lookupParameters;
	map<std::string, SP_DS_Node*> lookupTransitions;
	map<pair<std::string, std::string>, Weights > arcs;

	void ClearAll();

	//file to be read
	wxString m_fileName;
	wxString netName;

	dssd::andl::NetType m_eNetType;

	////////////////////////Graph Creation////////////////
	SP_MDI_View* m_pcView;
	SP_DS_Graph* m_pcGraph;
	SP_GUI_Canvas* m_pcCanvas;
	SP_MDI_Doc* m_pcMyDoc;

	double x;
	double y;

	//void CreateGraph();
	bool CreateGraph(const wxString& p_sFile, const dssd::andl::Net& p_Net);
	bool CreateEdge(SP_DS_Node* source, SP_DS_Node* target, const wxString& weight,const wxString& type);

	//SP_DS_Graph* CreateDocument();
	SP_DS_Graph* CreateDocument(const wxString& p_sFile, dssd::andl::NetType p_eType);
	bool CreateFunc(const dssd::andl::Functions& p_Functions);
	bool CreatePlaces(const dssd::andl::Places& p_Places);
	bool CreateConst(const dssd::andl::Constants& p_Constants, const dssd::andl::Valuesets& p_Valuesets);
	bool CreateObservers(const dssd::andl::Observers& p_Observers);
	bool CreateTransitions(const dssd::andl::Transitions& p_Transitions);
	bool CreateArcs();
	void doLayout();


 public:
	SP_ImportANDL();
	virtual ~SP_ImportANDL();

	virtual bool ReadFile(const wxString& fileName);

	inline virtual wxString GetName()
		{ return wxT("Import ANDL");}
	inline virtual wxString GetDescr()
		{ return wxT("Import a andl.");}
	virtual wxString GetExtension()
		{ return wxT("ANDL files (*.andl)|*.andl");}

};

#endif // __SP_IMPORTANDLH__
