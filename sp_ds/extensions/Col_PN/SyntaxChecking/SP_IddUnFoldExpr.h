//////////////////////////////////////////////////////////////////////
// $Author: George Assaf $
// $Version: 0.1 $
// $Date: 2019/12/21 $
// Short Description:Using dssd util for checking the syntax of colour expression
// of guards, arcs and places, and compute initial marking of places.
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_IDD_UNFOLD_EXPR_H__
#define __SP_DS_IDD_UNFOLD_EXPR_H__
#include "sp_core/tools/SP_NetUnfolder.h"
#include <dssd/andl/andl_builder.h>
#include <dssd/auxi/logger.h>
#include <dssd/colexpr/builder.h>
#include <dssd/colexpr/colexpr_parser.h>
#include <dssd/colexpr/environment.h>
#include <dssd/colexpr/eval.h>
#include <dssd/colexpr/modify.h>
#include <dssd/extern/alphanum.hpp>
#include <dssd/misc/net_evaluation.h>


#include "sp_ds/extensions/Col_SPN/SP_DS_ColPN_Unfolding.h"
#include "sp_utilities.h"
#include "dssd/auxi/auxi.h"
#include "dssd/auxi/timer.h"
#include "dssd/andl/andl_reader.h"
#include "dssd/andl/andl_writer.h"
#include "dssd/candl/candl_writer.h"
#include "dssd/unfolding/net_unfolding.icc"
#include "dssd/unfolding/gecode_representation.h"
#include "dssd/unfolding/idd_representation.h"

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
using  colexpr::evalColExpr;
using  colexpr::checkColExpr;
using dssd::andl::Place_ptr;
using dssd::andl::Transition_ptr;
using dssd::andl::Update_ptr;
using dssd::andl::Condition_ptr;
using dssd::andl::Net_ptr;
using dssd::andl::Constant_ptr;
using namespace dssd::unfolding;
using namespace dssd::ds;
using solution_space = idd_guard_representation;
 
using dssd::colexpr::evalColExpr;
 
class SP_IddUnFoldExpr {
public:
	
	SP_IddUnFoldExpr(SP_DS_Graph* p_pcGraph,wxString p_sColExpr,wxString p_sPname);//constructor

	bool LoadNetDefentions(dssd::andl::Net_ptr, dssd::andl::simple_net_builder&);//register the net

	bool EvalExpr(wxString &, wxString& p_scolorset);//evalute colour expression for comuting the marking

	int  UnfoldPlace1(std::string marking,dssd::andl::Place_ptr p,dssd::andl::simple_net_builder&);//Do the actual unfold for evaluating the colour expression

	bool CheckCoLourExpression(std::string p_sExp, std::string &p_sColorSet,wxString& p_sErrorPos);//Check the colour expression for Arcs 

	bool CheckGuardEXpression(std::string& p_sExp, std::string& p_sErrorPos);//Check Colour expression for guards
	
	placeLookUpTable GetPlaceLookupTable() { return m_lkt; }

	private:
		colExpr parseExpr(const std::string& expr_string);
		void    substituteColorFunctions(colExpr &expr);
		void    flatExpression(colExpr &expr, std::string colorset="");
	unsigned    createPlaces(solution_space &sol,
			    std::string name, dssd::andl::PlType type, bool fixed,
			    colExpr color, colEnv &env,
			    colExpr token, dssd::andl::simple_net_builder &netBuilder, placeLookUpTable &unfoldedPlaces,bool evalTokens);

		

private:
		colExprBuilder* colExprBuilder_;
		colExprParser* colExprParser_;
		dssd::colexpr::environment colDefinitions_;
		placeLookUpTable m_lkt;
		 

protected:
	SP_DS_Graph* m_pcGraph;
	wxString     m_sColExpr;
	wxString     m_sPname;
};


#endif // __SP_DS_IDD_UNFOLD_EXPR_H__