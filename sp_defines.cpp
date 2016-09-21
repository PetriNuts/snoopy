#include "sp_utilities.h"


const unsigned SP_INFINITY = std::numeric_limits<unsigned>::max();

const wxString SP_WILDCARD = wxT("<*>");

/**
 * names of elements of each netclass
 */
// SP_DS_SimpleGraph
const wxString SP_DS_SIMPLEGRAPH_CLASS = wxT("Simple Graph");

// SP_DS_BipartGraph
const wxString SP_DS_BIPARTGRAPH_CLASS = wxT("Bipart Graph");

// SP_DS_SimplePed
const wxString SP_DS_PN_CLASS = wxT("Petri Net");

// SP_DS_ExtPT
const wxString SP_DS_EXTPN_CLASS = wxT("Extended Petri Net");

// SP_DS_ContinuousPed
const wxString SP_DS_CONTINUOUSPED_CLASS = wxT("Continuous Petri Net");

const wxString SP_DS_COARSE_PLACE = wxT("Coarse Place");
const wxString SP_DS_COARSE_TRANSITION = wxT("Coarse Transition");
const wxString SP_DS_COARSE_PARAMETER = wxT("Coarse Parameter");


// SP_DS_EventSPN
const wxString SP_DS_SPN_CLASS = wxT("Stochastic Petri Net");

// SP_DS_TimePT
const wxString SP_DS_TIMEPN_CLASS = wxT("Time Petri Net");

// SP_DS_Metadataclass constants
const wxString SP_DS_META_CONSTANT = wxT("Constant Class");
const wxString SP_DS_META_FUNCTION = wxT("Function Class");

// SP_DS_Colored Petri Nets
const wxString SP_DS_COLEPN_CLASS = wxT("Colored Extended Petri Net");
const wxString SP_DS_CPN_MARKINGLIST = wxT("MarkingList");
const wxString SP_DS_CPN_COLORSETNAME = wxT("Colorset");
const wxString SP_DS_CPN_BASICCOLORSETCLASS = wxT("Basic Colorset Class");
const wxString SP_DS_CPN_STRUCTUREDCOLORSETCLASS = wxT("Structured Colorset Class");
const wxString SP_DS_CPN_ALIASCOLORSETCLASS = wxT("Alias Colorset Class");
const wxString SP_DS_CPN_CONSTANTCLASS = wxT("Constant Class");
const wxString SP_DS_CPN_VARIABLECLASS = wxT("Variable Class");
const wxString SP_DS_CPN_GUARDLIST = wxT("GuardList");
const wxString SP_DS_CPN_INSCRIPTION = wxT("ExpressionList");
const wxString SP_DS_CPN_FUNCTIONCLASS = wxT("Function Class");
const wxString SP_DS_CPN_RATEFUNCTIONLIST = wxT("FunctionList");
const wxString SP_DS_CPN_AUXILIARVARIABLESCLASS = wxT("Auxiliary Variables Class");

//Continuous - Stochastic Petri Net
const wxString SP_DS_HYBRIDPN_CLASS = wxT("Hybrid Petri Net");
//Extension
const wxString SP_DS_HYBRIDPN_EXT = wxT("hpn");

//SP_DS_ColPN
const wxString SP_DS_COLPN_CLASS = wxT("Colored Petri Net");

//SP_DS_ColSPN
const wxString SP_DS_COLSPN_CLASS = wxT("Colored Stochastic Petri Net");

//SP_DS_ColCPN
const wxString SP_DS_COLCPN_CLASS = wxT("Colored Continuous Petri Net");

//SP_DS_ColHPN
const wxString SP_DS_COLHPN_CLASS = wxT("Colored Hybrid Petri Net");

//SP_DS_ModuloNets
const wxString SP_DS_MODULOPN_CLASS = wxT("Modulo Petri Net");

// SP_DS_MusicPed
const wxString SP_DS_MUSICPN_CLASS = wxT("Music Petri Net");

// SP_DS_ReachabilityGraph
const wxString SP_DS_REACHABILITY_CLASS = wxT("Reachability Graph");

// SP_DS_FaultTree
const wxString SP_DS_FAULTTREE_CLASS = wxT("Fault Tree");

// SP_DS_ExtendedFaultTree
const wxString SP_DS_EXTFAULTTREE_CLASS = wxT("Extended Fault Tree");

// SP_DS_Freestyle
const wxString SP_DS_FREESTYLE_CLASS = wxT("Freestyle Net");

// SP_DS_MTBDD
const wxString SP_DS_MTBDD_CLASS = wxT("MTBDD");

// SP_DS_MTIDD
const wxString SP_DS_MTIDD_CLASS = wxT("MTIDD");

//Continuous place
const wxString SP_DS_CONTINUOUS_PLACE = wxT("Place, Continuous");

//Continuous Transition
const wxString SP_DS_CONTINUOUS_TRANS = wxT("Transition, Continuous");

//Discrete Place
const wxString SP_DS_DISCRETE_PLACE = wxT("Place");

//Discrete Transition
const wxString SP_DS_DISCRETE_TRANS = wxT("Transition");

//discrete param
const wxString SP_DS_PARAM = wxT("Parameter");

//Immediate Transition
const wxString SP_DS_IMMEDIATE_TRANS = wxT("Immediate Transition");

//deterministic Transition
const wxString SP_DS_DETERMINISTIC_TRANS = wxT("Deterministic Transition");

//scheduled transition
const wxString SP_DS_SCHEDULED_TRANS = wxT("Scheduled Transition");

//Stochastic Transition
const wxString SP_DS_STOCHASTIC_TRANS = wxT("Transition");

// Normal Edge
const wxString SP_DS_EDGE = wxT("Edge");

//Inhibitor Edge
const wxString SP_DS_INHIBITOR_EDGE = wxT("Inhibitor Edge");

//Read Edge
const wxString SP_DS_READ_EDGE = wxT("Read Edge");

//Equal
const wxString SP_DS_EQUAL_EDGE = wxT("Equal Edge");

//Reset Edge
const wxString SP_DS_RESET_EDGE = wxT("Reset Edge");

//Modifier Edge
const wxString SP_DS_MODIFIER_EDGE = wxT("Modifier Edge");
