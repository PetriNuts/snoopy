//////////////////////////////////////////////////////////////////////
// $Author: ms $
// $Version: 0.1 $
// $Date: 2007/07/12 $
////////////////////////////////////////////////////////////////////
#ifndef __SP_IMPORTAPNNH__
#define __SP_IMPORTAPNNH__

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

class SP_DS_Graph;
class SP_DS_Edge;
class SP_DS_Node;
class SP_Graphic;

class SP_ImportAPNN: public SP_ImportRoutine,
	public SP_Error
{
 private:

	enum SP_NET_TYPE{
		SIMPLE_PT,
		EXTENDED_PT,
		STOCHASTIC_PT
	};


	struct PlaceAPNNImp
	{
		PlaceAPNNImp(unsigned long, const wxString&, const wxString&, double ,double );
		unsigned long id;
		wxString m_name;
		wxString m_marking;
		SP_DS_Node* m_node;
		double m_x;
		double m_y;

	};

	struct ConstAPNNImp
	{
		ConstAPNNImp(unsigned long, const wxString&, const wxString&, double ,double );
		unsigned long id;
		wxString m_name;
		wxString m_value;
		SP_DS_Node* m_node;
		double m_x;
		double m_y;

	};

	struct TransitionAPNNImp
	{
		TransitionAPNNImp(unsigned long, const wxString&, const wxString&, double ,double);
		unsigned long id;
		wxString m_name;
		wxString m_function;
		SP_DS_Node* m_node;
		double m_x;
		double m_y;
	};

	struct ArcAPNNImp{
		ArcAPNNImp(const wxString&, const wxString&, const wxString&, const wxString&);
		wxString fromID;
		wxString toID;
		wxString weight;
		wxString type;

	};


	typedef map<wxString, PlaceAPNNImp*> PlaceIdentifierMap;
	typedef map<wxString, ConstAPNNImp*> ConstIdentifierMap;
	typedef map<wxString, TransitionAPNNImp*> TransitionIdentifierMap;
	typedef list<ArcAPNNImp*> ArcList;

	//file to be read



	wxString errorString;
	wxStringTokenizer* stz;
	wxString m_fileName;
	wxString netName;
	wxString currentToken;

	PlaceIdentifierMap m_placeIds;
	ConstIdentifierMap m_constIds;
	TransitionIdentifierMap m_transitionIds;
	ArcList m_arcs;

	void ClearPlaceIds();
	void ClearConstIds();
	void ClearTransitionIds();
	void ClearArcList();
	void ClearAll();

	bool Open(const wxString& p_sFile);
	bool Close();

	////////////////////////Parsing//////////////////////
	wxFFile m_fFile;

	unsigned long arcs;
	unsigned long places;
	unsigned long consts;
	unsigned long transitions;

	SP_NET_TYPE m_eNetType;
	bool parsePlace();

	bool parseConst();

    bool parseTransition();

	bool parseArc();

	bool parse();
	bool CheckValue();
	////////////////////////Graph Creation////////////////
	SP_MDI_View* m_pcView;
	SP_DS_Graph* m_pcGraph;
	SP_GUI_Canvas* m_pcCanvas;
	SP_MDI_Doc* m_pcMyDoc;

	double x;
	double y;

	//void CreateGraph();
	void CreateGraph(const wxString& p_sFile);
	void CreateEdge(SP_DS_Node* source, SP_DS_Node* target, const wxString& weight,const wxString& type);

	//SP_DS_Graph* CreateDocument();
	SP_DS_Graph* CreateDocument(const wxString& p_sFile);
	void CreatePlaces();
	void CreateConst();
	void CreateTransitions();
	void CreateArcs();
	void doLayout();


 protected:



 public:
	SP_ImportAPNN();
	virtual ~SP_ImportAPNN();

	virtual bool ReadFile(const wxString& fileName,SP_ImportRoutine* p_sIR=NULL);
	inline virtual SP_DS_Graph* ImportToDoc(const wxString& fileName)//by george
	{return NULL;}
	inline virtual wxString GetName()
		{ return wxT("Import APNN");}
	inline virtual wxString GetDescr()
		{ return wxT("Import a apnn.");}
	virtual wxString GetExtension()
		{ return wxT("APNN files (*.apnn)|*.apnn");}

};

#endif // __SP_IMPORTAPNNH__
