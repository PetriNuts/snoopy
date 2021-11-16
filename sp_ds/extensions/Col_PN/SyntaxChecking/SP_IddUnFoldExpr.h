//////////////////////////////////////////////////////////////////////
// $Author: George Assaf $
// $Version: 0.1 $
// $Date: 2019/12/21 $
// Short Description:Using dssd util for checking the syntax of colour expression
// of guards, arcs and places, and compute initial marking of places.
//TODO: functions of this class has to be implemented in dssd_util
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_IDD_UNFOLD_EXPR_H__
#define __SP_DS_IDD_UNFOLD_EXPR_H__
#include "sp_core/tools/SP_NetUnfolder.h"
#include "sp_ds/extensions/Col_SPN/SP_DS_ColPN_Unfolding.h"


using namespace dssd::unfolding;
using namespace dssd;
 
using  colexpr::colexpr_descriptor_evaluator;
using colexpr::colexpr_intset_evaluator;
using colexpr::colexpr_stringset_evaluator;
using colexpr::colexpr_node_evaluator;
using colexpr::colexpr_guard_evaluator;
using colexpr::colexpr_variable_collector;
using colexpr::colexpr_expression_finder;
using colexpr::colexpr_expression_collector;
 
using colDescriptor = dssd::colexpr::element_descriptor;


using solution_space = idd_guard_representation;
 


class SP_IddUnFoldExpr {
public:
	
	SP_IddUnFoldExpr(SP_DS_Graph* p_pcGraph,wxString p_sColExpr,wxString p_sPname);//constructor

	bool LoadNetDefentions(dssd::andl::Net_ptr, dssd::andl::simple_net_builder&);//register the net

	bool EvalExpr(wxString &, wxString& p_scolorset);//evalute colour expression for comuting the marking

	int  UnfoldPlace1(std::string marking,dssd::andl::Place_ptr p,dssd::andl::simple_net_builder&);//Do the actual unfold for evaluating the colour expression


	bool CheckCoLourExpression(const std::string& p_sExp,const std::string& p_sColorSet,const wxString& p_sErrorPos);


	bool CheckGuardEXpression(std::string p_sExp, std::string p_sErrorPos);//Check Colour expression for guards

	bool CheckTransRateFunction(SP_DS_Node*, wxString& p_sRateFun);
	
	placeLookUpTable GetPlaceLookupTable() { return m_lkt; }


	private:
		colExpr parseExpr(const std::string& expr_string);
		void    substituteColorFunctions(colExpr &expr,std::string s="");
		void    flatExpression(colExpr &expr, std::string colorset="");
	unsigned    createPlaces(solution_space &sol,
			    std::string name, dssd::andl::PlType type, bool fixed,
			    colExpr color, colEnv &env,
			    colExpr token, dssd::andl::simple_net_builder &netBuilder, placeLookUpTable &unfoldedPlaces,bool evalTokens);
	bool        ISValidIdientifer(std::string& p_SId, colEnv &env);
	 
 
	 
private:
		colExprBuilder* colExprBuilder_;
		colExprParser* colExprParser_;
		dssd::colexpr::environment colDefinitions_;
		placeLookUpTable m_lkt;
		std::vector<string> m_vRegisteredConstants;//for checking rate fun
		std::vector<string> m_vPlaceNames;//for checking rate function
		using str2str = std::map<std::string, std::string>;
		str2str places2Colorset_;
		dssd::andl::simple_net_builder netBuilder_;

protected:
	SP_DS_Graph* m_pcGraph;
	wxString     m_sColExpr;
	wxString     m_sPname;
};


#endif // __SP_DS_IDD_UNFOLD_EXPR_H__
