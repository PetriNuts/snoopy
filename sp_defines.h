//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DEFINES_H__
#define __SP_DEFINES_H__

#include <algorithm>
#include <limits>
#include <string>
#include <sstream>

#include <wx/defs.h>
#include <wx/datetime.h>
#include <wx/log.h>
#include <wx/colour.h>

#include "dssz/auxi/sh_ptr.h"

#include "dssz/functions/comp.h"

#include "sp_core/SP_List.h"
#include "sp_core/SP_Map.h"
#include "sp_core/SP_Set.h"
#include "sp_core/SP_Vector.h"
#include "sp_core/SP_Matrix.h"
#include "sp_core/SP_Deque.h"

#include "sp_revision.h"
//------------------------------------------------------------------------
//typedefs

typedef pair<long, long> SP_PairLong;
typedef pair<double, double> SP_PairDouble;
typedef pair<wxString, wxString> SP_PairString;
typedef pair<long, wxString> SP_PairLongString;
typedef pair<wxColour, wxColour> SP_PairColour;

typedef SP_Matrix<SP_PairLong> SP_MatrixPairLong;
typedef SP_Matrix<SP_PairDouble> SP_MatrixPairDouble;

extern const unsigned SP_INFINITY;
extern const double SP_PI;
extern const wxString SP_WILDCARD;

//------------------------------------------------------------------------

#define SP_Find(Container, Value) std::find(std::begin(Container),std::end(Container),(Value))

void SP_LOGDEBUG(const wxString&);

//------------------------------------------------------------------------
#define CHECK_POINTER(A, RET) \
    if ((A) == NULL) \
    { \
        SetLastError(wxString::Format(wxT("pointer error in %s(%d): %s is NULL, we %s"), wxT(__FILE__), __LINE__, wxT(#A), wxT(#RET))); \
        SP_LOGDEBUG(GetLastError()); \
        RET; \
    }

//------------------------------------------------------------------------
#define CHECK_BOOL(A, RET) \
    if ((A) != TRUE) \
    { \
        SetLastError(wxString::Format(wxT("boolean error in %s(%d):  %s is FALSE, we %s"), wxT(__FILE__), __LINE__, wxT(#A), wxT(#RET))); \
        SP_LOGDEBUG(GetLastError()); \
        RET; \
    }

//------------------------------------------------------------------------
//proccess pending events and if A != true then RET
#define CHECK_EVENTS(A, RET) \
    while ( wxTheApp->Pending()) \
	{ \
		wxTheApp->Dispatch(); \
	} \
	if ((A) == FALSE) \
    { \
		SP_LOGDEBUG(wxString::Format(wxT("proccess event in %s(%d):  %s is FALSE, we %s"), wxT(__FILE__), __LINE__, wxT(#A), wxT(#RET))); \
        RET; \
    }

#define SP_NUMBER_TO_STR(N,F) wxString::Format(F,N)

//------------------------------------------------------------------------
#define CHECK_EQUAL(A, B, RET) if ((A) != (B)) RET;
//------------------------------------------------------------------------

// GUI IDs
enum
{
    // MENU IDS
	SP_MENU_ITEM_TREE_SELECT_ALL = 12000 + 1,
    SP_MENU_ITEM_TREE_EDIT_PROPERTIES,
    SP_MENU_ITEM_EDIT_SHOW,
    SP_MENU_ITEM_SQUEEZE,
	SP_MENU_ITEM_LAYOUT,
    SP_MENU_ITEM_DELETE_ALL,
    SP_MENU_ITEM_CHECKFORUPDATE,
    SP_MENU_ITEM_SORTNODES,
    SP_MENU_ITEM_CHECKDUPLICATENODES,
    SP_MENU_ITEM_NETINFORMATIONS,
	SP_MENU_ITEM_DUPLICATE,
	SP_MENU_ITEM_MERGE,
	SP_MENU_ITEM_COARSE,
	SP_MENU_ITEM_FLATTEN,

    // hierarchy navigation
    SP_MENU_ITEM_GO_UP,
    SP_MENU_ITEM_GO_FIRST_CHILD,
    SP_MENU_ITEM_GO_NEXT_SIBLING,
    SP_MENU_ITEM_GO_PREV_SIBLING,

	//visualization
	SP_MENU_ITEM_IA_MENU,
	SP_MENU_ITEM_IA_SUBMENU,
	SP_MENU_ITEM_IA_SUBMENUITEM,
	//this allows ten connected nets for one source net
	//which should be more than enough
	SP_MENU_ITEM_IA_SUBMENUITEM_LAST = SP_MENU_ITEM_IA_SUBMENUITEM + 9,
	SP_MENU_ITEM_IA_TRIGGERGENERAL,
	SP_MENU_ITEM_IA_TRIGGERSINGLE,
	SP_MENU_ITEM_IA_TRIGGERMAKRO,
	SP_MENU_ITEM_IA_UNDOLASTACTION,
	SP_MENU_ITEM_IA_CONFIGURE,
	SP_MENU_ITEM_IA_REGCMDLIST,

	// Convert to Menu
	SP_MENU_CONVERT_ELEMENT_MENU,
	//metadata
	SP_MENU_ITEM_MD_GENERAL,

    //export
	SP_MENU_ITEM_OPEN_AS,
    SP_MENU_ITEM_SAVE_SIGN,
    SP_MENU_ITEM_EXPORT_MGR,

    // highlighten marking edges ---- by sl
    SP_MENU_ITEM_HIGHMARKING,


    // anim
    SP_MENU_ITEM_TOGGLEANIM,
    SP_ID_BUTTON_STEPANIM,
    SP_MENU_ITEM_ANIMOPTIONS,

    //simulation
    SP_MENU_ITEM_TOGGLESIMULATION,
	// Amr Direct Method
	SP_MENU_ITEM_TOGGLEDIRSIMULATION,

    //Steering
    SP_MENU_ITEM_TOGGLESTEERING,

    // TOGGLE IDs in one continuous row!
    SP_MENU_ITEM_TOGGLE_DEV_ID,
    SP_MENU_ITEM_TOGGLE_FMEN_ID,
    SP_MENU_ITEM_TOGGLE_HIERARCHY_ID,
    SP_MENU_ITEM_TOGGLE_LOG_ID,
    // !TOGGLE IDs

    SP_MENU_ITEM_TOGGLE_EXP_LOG_ID,

    SP_MENU_ITEM_ELEMENTS_FIRST,
    SP_MENU_ITEM_ELEMENTS_LAST = SP_MENU_ITEM_ELEMENTS_FIRST + 100,


    SP_EDIT_PROPERTIES_ID,
    SP_EDIT_TRANSFORM_ID,
    SP_EDIT_SELECT_ALL_ID,
    SP_EDIT_COPY_IN_NEW_NET,
    SP_EDIT_HIDE_ID,
    SP_EDIT_HIDEALL_ID,
    SP_EDIT_UNHIDE_ID,
    SP_EDIT_UNHIDEALL_ID,
    SP_TRANSFORM_RADIO_ID,
	SP_ID_DELETE,

    // TOOL IDS
    // don't put anything else between!
    SP_TOOL_SELECT_ID,
    SP_TOOL_MOVE_ID,
    SP_TOOL_EDIT_ID,
    // !TOOL IDs

    // WINDOW IDS USED FOR EVENTS
    SP_GUI_SASHWINDOW_LEFT_ID,
    SP_WDG_NOTEBOOK_ID,
    SP_WDG_TREEBOOK_ID,
    SP_ID_ANIMTIMER,

    // TREE CONTROL ID
    SP_ID_GRAPHTREE_CTRL_ID,
    SP_ID_COARSETREE_CTRL_ID,
	SP_ID_COLORSETTREE_CTRL_ID,

    // animation buttons
	SP_ID_BUTTON_ANIM_PLAYBWD,
    SP_ID_BUTTON_ANIM_STEPBWD,
    SP_ID_BUTTON_ANIM_RESET,
    SP_ID_BUTTON_ANIM_STEPFWD,
    SP_ID_BUTTON_ANIM_PLAYFWD,
	//music ped animation
	SP_ID_TOGGLEMUTE,
	SP_ID_CHOICE_SOUND_SETS,
	SP_ID_SLIDER,
	SP_ID_BUTTON_MODIFY_SOUND_SETS,

    // search id
    SP_MENU_ITEM_SEARCHID,
    SP_ID_RADIOBOX_SELECT,
    SP_ID_BUTTON_FIND,
    SP_ID_BUTTON_NEXT,
    SP_ID_BUTTON_PREV,
    SP_ID_BUTTON_ALL,

	// invariant analysis
	SP_MENU_ITEM_LOAD_INVARIANT_FILE,
	SP_ID_BUTTON_FIRST_VEC,
	SP_ID_BUTTON_NEXT_VEC,
	SP_ID_BUTTON_PREV_VEC,
	SP_ID_LISTBOX_VEC,
	SP_ID_CHANGED_VEC,
	SP_ID_BUTTON_COLOUR,
	SP_ID_CHECKBOX_GRADIENT_VEC,
    SP_ID_BUTTON_LAST_VEC,

	//Partition,SMART export
	SP_ID_CHECKBOX_Part_1,
	SP_ID_CHECKBOX_Part_2,
	SP_ID_CHECKBOX_Part_3,
	SP_ID_CHECKBOX_Part_4,
	SP_ID_BUTTON_NEW_PARTITION,
	SP_ID_BUTTON_DELETE_PARTITION,
	SP_ID_BUTTON_ELEMENTTO_PARTITION,
	SP_ID_BUTTON_ELEMENTFROM_PARTITION,
	SP_ID_LISTBOX_PARTITIONLIST,
	SP_ID_BUTTON_PARTITION_LOAD,
	SP_ID_BUTTON_PARTITION_SAVE,
	SP_ID_BUTTON_CTL_LOAD,
	SP_ID_BUTTON_CTL_SAVE,
	SP_ID_BUTTON_CTL_CLEAR,
	SP_ID_BUTTON_CTL_CHECK,
	SP_ID_LISTBOX_STEADYSTATES,
	SP_ID_BUTTON_STEADYSTATE_ADD,
	SP_ID_BUTTON_STEADYSTATE_REMOVE,
	SP_ID_BUTTON_STEADYSTATE_CLEAR,
	SP_ID_BUTTON_STEADYSTATE_CHECK,
	SP_ID_BUTTON_STEADYSTATE_LOAD,
	SP_ID_BUTTON_STEADYSTATE_SAVE,
	SP_ID_LISTBOX_STEADYSTATE,
	SP_ID_LISTBOX_TRANSIENTE,
	SP_ID_BUTTON_TRANSIENTE_ADD,
	SP_ID_BUTTON_TRANSIENTE_REMOVE,
	SP_ID_BUTTON_TRANSIENTE_CLEAR,
	SP_ID_BUTTON_TRANSIENTE_CHECK,
	SP_ID_BUTTON_TRANSIENTE_LOAD,
	SP_ID_BUTTON_TRANSIENTE_SAVE,

	//import
	SP_MENU_ITEM_IMPORT,

	//Automatic folding
	SP_MENU_ITEM_FOLDING,
	SP_MENU_ITEM_GENERATE_MASTER_NET,
	SP_MENU_ITEM_GENERATE_TWIN_NET,
	SP_MENU_ITEM_COLORIZE_ANY_NET,

	//simplification of fault trees
	SP_MENU_ITEM_REDUCTION_RULES,
	SP_MENU_ITEM_LOAD_DNF_FILE,
	SP_ID_RADIOBOX_FTREE_ANALYSIS,
	SP_MENU_ITEM_DEMORGAN_TRANSFORMER,

	// id's for preference categories
	SP_ID_PREF_CANVAS,
	SP_ID_PREF_ANIMATION,
	SP_ID_PREF_ELEMENTS,
	SP_ID_PREF_FONTS,

	// id's for preference controls
    SP_ID_RADIOBOX_GRID_STYLE,
    SP_ID_RADIOBOX_TRANSSHAPE,
    SP_ID_SPINCTRL_TRANSWIDTH,
    SP_ID_SPINCTRL_TRANSHEIGHT,
    SP_ID_BUTTON_FONT,
	SP_ID_BUTTON_FONT_END = SP_ID_BUTTON_FONT + 10,
	SP_ID_BUTTON_SOUND,
	SP_ID_BUTTON_BROWSE,
	SP_ID_BUTTON_BROWSEFromFile,

    // let this always be the last entry!
    SP_ID_LAST_ID
};

enum SP_ANIM_STEP_T {
		//don't change order
		SP_ANIM_STEP_MAXIMUM = 1,
		SP_ANIM_STEP_INTERMEDIATE,
		SP_ANIM_STEP_SINGLE
};

enum SP_FT_ANALYSIS_TYPE {
		SP_FT_QUALITATIVE,
		SP_FT_QUANTITATIVE
};

enum SP_FT_SYSTEM_TYPE {
		SP_FT_SYSTEM_REPAIRABLE,
		SP_FT_SYSTEM_IRREPARABLE
};

enum SP_DIRECTION_T {
	FORWARD,
	BACKWARD
};

enum SP_ORDERING_MODE
{
    SP_PLAIN,
    SP_REVERSE,
    SP_RANDOM,
	SP_NOACK1,
	SP_NOACK2,
	SP_FROMFILE
};

/**
 * names of elements of each netclass
 */
// SP_DS_SimpleGraph
extern const wxString SP_DS_SIMPLEGRAPH_CLASS;

// SP_DS_BipartGraph
extern const wxString SP_DS_BIPARTGRAPH_CLASS;

// SP_DS_SimplePed
extern const wxString SP_DS_PN_CLASS;

// SP_DS_ExtPT
extern const wxString SP_DS_EXTPN_CLASS;

// SP_DS_ContinuousPed
extern const wxString SP_DS_CONTINUOUSPED_CLASS;

extern const wxString SP_DS_COARSE_PLACE;
extern const wxString SP_DS_COARSE_TRANSITION;
extern const wxString SP_DS_COARSE_PARAMETER;


// SP_DS_EventSPN
extern const wxString SP_DS_SPN_CLASS;

// SP_DS_TimePT
extern const wxString SP_DS_TIMEPN_CLASS;

// SP_DS_Metadataclass constants
extern const wxString SP_DS_META_CONSTANT;
extern const wxString SP_DS_META_FUNCTION;

// SP_DS_Colored Petri Nets
extern const wxString SP_DS_COLEPN_CLASS;
extern const wxString SP_DS_CPN_MARKINGLIST;
extern const wxString SP_DS_CPN_COLORSETNAME;
extern const wxString SP_DS_CPN_BASICCOLORSETCLASS;
extern const wxString SP_DS_CPN_STRUCTUREDCOLORSETCLASS;
extern const wxString SP_DS_CPN_ALIASCOLORSETCLASS;
extern const wxString SP_DS_CPN_CONSTANTCLASS;
extern const wxString SP_DS_CPN_VARIABLECLASS;
extern const wxString SP_DS_CPN_GUARDLIST;
extern const wxString SP_DS_CPN_INSCRIPTION;
extern const wxString SP_DS_CPN_FUNCTIONCLASS;
extern const wxString SP_DS_CPN_RATEFUNCTIONLIST;
extern const wxString SP_DS_CPN_AUXILIARVARIABLESCLASS;

//Continuous - Stochastic Petri Net
extern const wxString SP_DS_HYBRIDPN_CLASS;
//Extension
extern const wxString SP_DS_HYBRIDPN_EXT;

//SP_DS_ColPN
extern const wxString SP_DS_COLPN_CLASS;

//SP_DS_ColSPN
extern const wxString SP_DS_COLSPN_CLASS;

//SP_DS_ColCPN
extern const wxString SP_DS_COLCPN_CLASS;

//SP_DS_ColHPN
extern const wxString SP_DS_COLHPN_CLASS;

//SP_DS_ModuloNets
extern const wxString SP_DS_MODULOPN_CLASS;

// SP_DS_MusicPed
extern const wxString SP_DS_MUSICPN_CLASS;

// SP_DS_ReachabilityGraph
extern const wxString SP_DS_REACHABILITY_CLASS;

// SP_DS_FaultTree
extern const wxString SP_DS_FAULTTREE_CLASS;

// SP_DS_ExtendedFaultTree
extern const wxString SP_DS_EXTFAULTTREE_CLASS;

// SP_DS_Freestyle
extern const wxString SP_DS_FREESTYLE_CLASS;

// SP_DS_MTBDD
extern const wxString SP_DS_MTBDD_CLASS;

// SP_DS_MTIDD
extern const wxString SP_DS_MTIDD_CLASS;

//Continuous place
extern const wxString SP_DS_CONTINUOUS_PLACE;

//Continuous Transition
extern const wxString SP_DS_CONTINUOUS_TRANS;

//Discrete Place
extern const wxString SP_DS_DISCRETE_PLACE;

//Discrete Transition
extern const wxString SP_DS_DISCRETE_TRANS;

//discrete param
extern const wxString SP_DS_PARAM;

//Immediate Transition
extern const wxString SP_DS_IMMEDIATE_TRANS;

//deterministic Transition
extern const wxString SP_DS_DETERMINISTIC_TRANS;

//scheduled transition
extern const wxString SP_DS_SCHEDULED_TRANS;

//Stochastic Transition
extern const wxString SP_DS_STOCHASTIC_TRANS;

// Normal Edge
extern const wxString SP_DS_EDGE;

//Inhibitor Edge
extern const wxString SP_DS_INHIBITOR_EDGE;

//Read Edge
extern const wxString SP_DS_READ_EDGE;

//Equal
extern const wxString SP_DS_EQUAL_EDGE;

//Reset Edge
extern const wxString SP_DS_RESET_EDGE;

//Modifier Edge
extern const wxString SP_DS_MODIFIER_EDGE;

#endif // __SP_DEFINES_H__
