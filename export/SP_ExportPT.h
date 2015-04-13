//////////////////////////////////////////////////////////////////////
// $Author: alextov $
// $Version: 0.2 $
// $Date: 2005/05/22 $
// Short Description: abstract export routine and help structures/funct
//                    for simple petri nets
//////////////////////////////////////////////////////////////////////
#ifndef __SP_EXPORT_PT__
#define __SP_EXPORT_PT__

#include "sp_core/SP_List.h"
#include "sp_core/SP_Map.h"
#include "export/SP_ExportRoutine.h"
#include "sp_core/base/SP_Error.h"

#include <wx/ffile.h>

class SP_DS_Node;
class SP_DS_Edge;
class SP_DS_Metadata;
class SP_DS_Graph;

struct SP_Place
{
  SP_Place(long, const wxString&, const wxString&, unsigned, SP_DS_Node*);
  long m_id;
  wxString m_name;
  wxString m_value;
  unsigned m_marking;
  SP_DS_Node* m_node;
};

struct SP_ContPlace
{
  SP_ContPlace(long, const wxString&, const wxString&, double, bool, SP_DS_Node*);
  long m_id;
  wxString m_name;
  wxString m_value;
  double m_marking;
  bool m_fixed;
  SP_DS_Node* m_node;
};

struct SP_Transition
{
  SP_Transition(long, const wxString&, const wxString&, SP_DS_Node*);
  long m_id;
  wxString m_name;
  wxString m_function;
  SP_DS_Node* m_node;
};

struct SP_ContTransition
{
  SP_ContTransition(long, const wxString&, const wxString&, bool, SP_DS_Node*);
  long m_id;
  wxString m_name;
  wxString m_function;
  bool m_reversible;
  SP_DS_Node* m_node;
};

struct SP_Parameter
{
  SP_Parameter(long, const wxString&, double, SP_DS_Node*);
  long m_id;
  wxString m_name;
  double m_marking;
  SP_DS_Node* m_node;
};

struct SP_DiscrArc
{
  SP_DiscrArc(long, unsigned, const wxString&, SP_DS_Edge*);
  long m_id;
  unsigned m_mult;
  wxString m_value;
  SP_DS_Edge* m_edge;
};

struct SP_ContArc
{
  SP_ContArc(long, double, const wxString&, SP_DS_Edge*);
  long m_id;
  double m_mult;
  wxString m_value;
  SP_DS_Edge* m_edge;
};

//by sl
struct SP_Constant
{
  SP_Constant(long, const wxString&, const wxString&, const wxString&, const wxString&, double, SP_DS_Metadata*);
  long m_id;
  wxString m_name;
  wxString m_type;
  wxString m_group;
  wxString m_value;
  double m_marking;
  SP_DS_Metadata* m_node;
};




typedef map<long, list<SP_DiscrArc*>*> ArcMap;
typedef map<long, list<SP_ContArc*>*> ContArcMap;

typedef map<wxString, SP_Place*> PlaceMap;
typedef map<long, SP_Place*> PlaceIdMap;

typedef map<wxString, SP_ContPlace*> ContPlaceMap;
typedef map<long, SP_ContPlace*> ContPlaceIdMap;

typedef map<long, SP_Transition*> TransitionIdMap;
typedef map<wxString, SP_Transition*> TransitionMap;

typedef map<wxString, SP_ContTransition*> ContTransitionMap;
typedef map<long, SP_ContTransition*> ContTransitionIdMap;

typedef map<wxString, SP_Parameter*> ParameterMap;
typedef map<long, SP_Parameter*> ParameterIdMap;

//by sl
typedef map<wxString, SP_Constant*> ConstantMap;
typedef map<long, SP_Constant*> ConstantIdMap;

class SP_ExportPT : public SP_ExportRoutine, public SP_Error
{
 protected:
  SP_MDI_Doc* m_doc;
  SP_DS_Graph* m_graph;
  wxString m_fileName;
  wxFFile m_file;

  PlaceIdMap m_placeIdMap;
  PlaceMap m_placeNameMap;

  ContPlaceIdMap m_contPlaceIdMap;
  ContPlaceMap m_contPlaceNameMap;

  TransitionIdMap m_trIdMap;
  TransitionMap m_trNameMap;

  ContTransitionIdMap m_contTrIdMap;
  ContTransitionMap m_contTrNameMap;

  ParameterIdMap m_parameterIdMap;
  ParameterMap m_parameterNameMap;

  ArcMap m_prePlMap;
  ArcMap m_postPlMap;

  ArcMap m_preTrMap;
  ArcMap m_postTrMap;

  ContArcMap m_preContPlMap;
  ContArcMap m_postContPlMap;

  ContArcMap m_preContTrMap;
  ContArcMap m_postContTrMap;

  ConstantIdMap m_constantIdMap;
  ConstantMap m_constantNameMap;


  bool InitNodes();

  void InitArcMaps();

  // clears all the maps using the following Clear* functions
  void ClearMaps();

  wxString GetNetName(const wxString&);

  bool WriteDeclarations();
  bool InitConstants();


  /**	\brief	replaces all non-alphanum chars with "_"
	\param	string to normalize
	\return	wxString
  */
  wxString NormalizeString (const wxString& p_sS);

  // must be implemented in successors
  virtual bool DoWrite() = 0;

 public:
  SP_ExportPT();
  virtual ~SP_ExportPT();

  virtual bool AcceptsDoc(SP_MDI_Doc*);


  virtual bool Write(SP_MDI_Doc*, const wxString&);
  virtual wxString GetName() = 0;
  virtual wxString GetDescr() = 0;
  virtual wxString GetExtension() = 0;
  virtual wxString GetFileDescr() = 0;

};
#endif // __SP_EXPORT_PT__
