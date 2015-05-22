//////////////////////////////////////////////////////////////////////
// $Author: schroed $
// $Version: 0.1 $
// $Date: 2007/04/30 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#ifndef __SP_IMPORT_SBML_H__
#define __SP_IMPORT_SBML_H__

#include <sbml/SBMLTypes.h>
#include <wx/wx.h>

#include "import/SP_ImportRoutine.h"
#include "sp_core/SP_List.h"

#include "sp_ds/extensions/SP_DS_NetCreater.h"

class SP_ImportSBML: public SP_ImportRoutine,
	public SP_Error,
	public SP_DS_NetCreater

{

private:

protected:

public:
	wxString formulaToString(const ASTNode* p_Math);

	bool getSBMLSpeciesName(Species* p_Species, wxString& p_Id, wxString& p_Name);

	bool getSBMLEventName(Event* p_Event, wxString& p_Id, wxString& p_Name);

	bool getSBMLReactionName(Reaction* p_Reaction, wxString& p_Id, wxString& p_Name);

	bool getSBMLParameterName(Parameter* p_Parameter, wxString& p_Id, wxString& p_Name);

	bool getSBMLCompartmentName(Compartment* p_Compartment, wxString& p_Id, wxString& p_Name);

	const ASTNode* getSBMLFormula(ASTNode* p_sbmlFormula);

	SP_DS_Node* existInCompoundList(const wxString& searchName);

	bool existInReactionList(const wxString& newName);
	bool existInReactionFormula(const wxString& p_sName, const ASTNode* p_sbmlFormula);

	bool changeInReactionFormula(const wxString& p_sOld, const wxString& p_sNew, ASTNode* p_sbmlFormula);

	bool ValidateSBML(SBMLDocument* p_sbmlDoc);

	SP_DS_Graph* m_pcGraph = nullptr;
	Model* m_sbmlModel = nullptr;

	SP_ListNode g_CompoundList;
	SP_ListNode g_ReactionList;

	int yComRea;
	int xPara;
	int yPara;

	size_t numReverseReactions = 0;

};

#endif // __SP_IMPORT_SBML_H__
