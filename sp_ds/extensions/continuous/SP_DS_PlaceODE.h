/*
 * SP_DS_PlaceODE.h
 * $Author: M.Herajy
 * $Version: 1.0 $
 * $Revision: 0.0 $
 * $Date: 2.6.2010
 * Short Description:
 */
 //======================================================================================

#ifndef SP_DS_PLACE_ODE_H_
#define SP_DS_PLACE_ODE_H_

/*
 * Transition types. In dynamic hybrid simulation, transitions can change their
 * type during the simulation. We include only continuous simulation during the construction
 * of the place ODE.
 */
enum TransType
{
 SP_STOCHASTIC_TRANS,
 SP_CONTINUOUS_TRANS,
 SP_DETERMINISTIC_TRANS,
 SP_SCHEDULED_TRANS,
 SP_IMMEDIATE_TRANS,
 SP_OTHER_TRANS
};
typedef vector<TransType> SP_VectorTransType;

enum ConnectionType
{
 SP_NO_CONNECTION,
 SP_STANDARD_ARC,
 SP_READ_ARC,
 SP_EQUAL_ARC,
 SP_INHIBITOR_ARC,
 SP_MODIFIER_ARC,
 SP_OTHER_CONNECTION
};

typedef vector<vector<ConnectionType> > SP_Vector2DConnType;
/*
 * The pre and post transitions on one place
 */
struct PlaceTransitions
{
    unsigned long nTransPos;
    ConnectionType ConType;
    wxString sConWeight;
};
typedef vector<vector<PlaceTransitions> > SP_VectorPlaceTrans;

class SP_DS_PlaceODE: public SP_Error
{
 protected:
	      //Number of places
	      unsigned long m_nPlaceCount;

	      //Number of transitions
	      unsigned long m_nTransCount;

	      //Pointer to the main  graph
	      SP_DS_Graph* m_pcGraph;
          //Transition types: Stochastic, continuous,...
	      SP_VectorTransType m_anTransitionsType;

	      //Transitions index
	      SP_VectorLong m_anTransPosition;

	      //Transitions rate function
	      SP_VectorString m_asTransRateFunction;

	      // Places pre and post transitions
	      SP_VectorPlaceTrans m_a2DPlacePreTransitions;
	      SP_VectorPlaceTrans m_a2DPlacePostTransitions;

	      //Getting transitions and places' position using their IDs
	      SP_MapLong2Long m_mTransitionsPosByID;
	      SP_MapLong2Long m_mPlacesPosByID;

	      vector<SP_DS_Node*> m_apcPlaceNodes;
	      SP_VectorStdString asPlaceNames;


  protected:
	        // Initialization functions

	        //Load the net places
	        virtual bool LoadPlaces();

	        virtual bool LoadPlacesOfType(const wxString& p_sPlaceType);

	        //Load the net Transitions
	        virtual bool LoadTransitions();

	        //Load the connection between places and Transitions
	        virtual bool LoadConnections();

	        virtual bool FillInPreConnection(SP_DS_Node* p_pcPlaceNode);
	        virtual bool FillInPostConnection(SP_DS_Node* p_pcPlaceNode);

	        virtual ConnectionType GetConnectionType(SP_DS_Edge* p_pcEdgeNode);

	        virtual long GetNodeID(SP_DS_Node* p_pcNode);

	        virtual SP_DS_Attribute* GetEdgeAttribute(SP_DS_Edge* p_pcEdge);

	        //Constructing  ODE functions
	        virtual bool AddExtendedArcsEffects();
	        virtual wxString ConstructExtendedFunction(const unsigned long& p_nTransPos,const unsigned long& p_nPlacePos);
	        virtual bool IsExtendedArc(const ConnectionType& p_nConType);
  public:
	       virtual bool Initialize();

	       /*
	        * main function. call it each time one Place's ODE need to be constructed
	        */
	       virtual wxString ConstructODE(const long& p_nPlacePos);

	       SP_DS_PlaceODE(SP_DS_Graph* p_pcGraph);
	      virtual ~SP_DS_PlaceODE();
};

#endif /* SP_DS_PLACE_ODE_H_ */
