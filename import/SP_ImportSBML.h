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
	bool m_HighlightReversibleReactions;
	bool m_CreateReverseReactions;
	bool m_CreateBoundaryConditions;
	bool m_NormalizeStoichiometries;

	SP_DS_Graph* m_pcGraph = nullptr;
	Model* m_sbmlModel = nullptr;

	SP_MapString2Node m_Species;
	SP_MapString2Node m_Reactions;

	int yComRea;
	int xPara;
	int yPara;

	size_t numReverseReactions = 0;
	size_t numBoundaryConditions = 0;


	bool NormalizeStoichiometries(const std::map<SP_DS_Edge*, double>& s);

	wxString formulaToString(const ASTNode* p_Math);

	template<typename T>
	bool getSBMLName(T* p_Elem, wxString& p_Id, wxString& p_Name)
	{
		CHECK_POINTER(p_Elem, return false);
		bool ret = false;
		// is ID or Name set
		wxString l_Name;
		if (p_Elem->isSetId())
		{
			p_Id = p_Elem->getId();
			ret = true;
		}
		if (p_Elem->isSetName())
		{
			p_Name = p_Elem->getName();
			ret = true;
		}
		return ret;
	}

	const ASTNode* getSBMLFormula(ASTNode* p_sbmlFormula);

	SP_DS_Node* getSpeciesNode(const wxString& searchName);
	SP_DS_Node* getReactionNode(const wxString& searchName);

	bool existInReactionFormula(const wxString& p_sName, const ASTNode* p_sbmlFormula);
	bool changeInReactionFormula(const wxString& p_sOld, const wxString& p_sNew, ASTNode* p_sbmlFormula);

	bool ValidateSBML(SBMLDocument* p_sbmlDoc);

public:

};

#endif // __SP_IMPORT_SBML_H__
