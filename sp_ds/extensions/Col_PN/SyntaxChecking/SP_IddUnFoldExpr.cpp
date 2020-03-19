//////////////////////////////////////////////////////////////////////
// $Author: George Assaf $
// $Version: 0.1 $
// $Date: 2019/12/21 $
// Short Description (Implementation file):Using dssd util for checking the syntax of colour expression
// of guards, arcs and places, and compute initial marking of places.
//////////////////////////////////////////////////////////////////////
#include "SP_IddUnFoldExpr.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"

SP_IddUnFoldExpr::SP_IddUnFoldExpr(SP_DS_Graph* p_pcGraph, wxString p_sColExpr,wxString p_sPna)
{
	m_pcGraph = p_pcGraph;
	m_sColExpr = p_sColExpr;
	m_sPname = p_sPna;
	colExprBuilder_ = new colExprBuilder();
	colExprParser_=new colExprParser(colExprBuilder_);
	dssd::colexpr::environment colDefinitions_;
 
}
 

bool SP_IddUnFoldExpr::LoadNetDefentions(dssd::andl::Net_ptr net_, dssd::andl::simple_net_builder& nb)
{
	dssd::andl::net_checker check(net_, colDefinitions_);

	dssd::andl::simple_net_builder netBuilder_=nb;

	dssd::functions::FunctionRegistry fReg_ = {};
	dssd::functions::SIMFunctionBuilder *fBuilder_=new dssd::functions::SIMFunctionBuilder(fReg_);
	dssd::functions::BisonFunctionParser* fParser_=new dssd::functions::BisonFunctionParser(*fBuilder_);
	

	for (auto const & it_values : *net_->valuesets_)
	{
		netBuilder_.registerValueSet(it_values);
	 
	}

	for (Constant_ptr c : *net_->constants_)
	{
		if (c)
		{
			std::string value = netBuilder_.constantValue(c);
			 
 
			dssd::functions::Function *f =
				dssd::functions::parseFunctionString(*fParser_,value);
			dssd::functions::FunctionRegistryEntry *funRegEntry = fReg_.lookUpFunction(c->name_);
			if (!funRegEntry)
			{
				fReg_.registerFunction(c->name_, f);
				netBuilder_.addConstant(Constant_ptr(c->copy()));
			}
			else {
				// If exists overwrite definition
				// Set new value
				c->values_.clear();
				c->values_.push_back(funRegEntry->function().toString());
				netBuilder_.addConstant(Constant_ptr(c->copy()));
			}
		}
	}

	colDefinitions_.setfReg(fReg_);
	std::vector < std::string > varNames;
	for (auto cs : *net_->colorsets_)
	{
		if (!cs) continue;

		std::string name = cs->name_;
		/*special case ENUM*/
		
		dssd::ds::IntSet::enums_t enums;
		if (cs->type_ == dssd::andl::ColorsetType::ENUM_T)
		{
			//we expect just a list of identifier
			int i = 0;
			std::vector < std::string > idents;
			dssd::aux::tokenize(cs->definition_, idents, "{,} ");
			for (auto const & ident : idents)
			{
				if (colDefinitions_.fReg().lookUpFunction(ident))
				{
					wxString ss;
					ss << "cannot create enum type <" << name << ">! element <"
						<< ident << "> is already defined!";
					SP_LOGERROR(ss);
				}
				 
				enums.insert(std::make_pair(i, ident));
				fBuilder_->processEnum(i);
				const_cast<dssd::functions::FunctionRegistry &>(colDefinitions_.fReg()).registerFunction(ident, fBuilder_->getFunction());
				i++;
			}
		}
		std::string ss = "";
		ss = cs->definition_;
		colExprParser_->parse(cs->definition_);
		colExpr expr(colExprBuilder_->node());

		colExprBuilder_->reset();
	 
		auto res = evalColExpr<colexpr_descriptor_evaluator>(expr, colDefinitions_);
		//wxString mss;
		ss = res.toString();
		if (res.ed_.isIntSet())
		{

		 
			auto r = evalColExpr<colexpr_intset_evaluator>(expr, colDefinitions_);
			r.set_.setCyclic(cs->cyclic_);
			if (!enums.empty())
			{
				r.set_.setEnums(enums);
			}
			colDefinitions_.registerIntColorset(name, r.set_);
		
		}
		else if (res.ed_.isStringSet())
		{
			auto r = evalColExpr<colexpr_stringset_evaluator>(expr, colDefinitions_);
			r.set_.setCyclic(cs->cyclic_);
			colDefinitions_.registerStringColorset(name, r.set_);
		}
		else
		{
			 
			if (res.ed_.isSubSet())
			{
				// l_sMsg;
				///<<"subset name "<<name<<" "<< res.ed_.toString();
				//SP_LOGERROR(l_sMsg);
			}
			colDefinitions_.registerColorset(name, res.ed_, res.ed_.exprString_);
		}
	}


	for (auto v : *net_->variables_)
	{
		if (!v) continue;
		colDefinitions_.registerVariable(v->name_, v->colorset_);
	}

	for (auto cf : *net_->color_functions_)
	{
		if (!cf) continue;

		colDefinitions_.registerColorFunction(cf);
	}

	netBuilder_.setName(net_->name_);
	switch (net_->type_) {
	case dssd::andl::NetType::COL_PN_T: {
		netBuilder_.setType(dssd::andl::NetType::PN_T);
	}
										break;
	case dssd::andl::NetType::COL_XPN_T: {
		netBuilder_.setType(dssd::andl::NetType::XPN_T);
	}
										 break;
	case dssd::andl::NetType::COL_SPN_T: {
		netBuilder_.setType(dssd::andl::NetType::SPN_T);
	}
										 break;
	case dssd::andl::NetType::COL_GSPN_T: {
		netBuilder_.setType(dssd::andl::NetType::GSPN_T);
	}
										  break;
	case dssd::andl::NetType::COL_XSPN_T: {
		netBuilder_.setType(dssd::andl::NetType::XSPN_T);
	}
										  break;
	case dssd::andl::NetType::COL_CPN_T: {
		netBuilder_.setType(dssd::andl::NetType::CPN_T);
	}
										 break;
	case dssd::andl::NetType::COL_HPN_T: {
		netBuilder_.setType(dssd::andl::NetType::HPN_T);
	}
										 break;
	case dssd::andl::NetType::COL_TPN_T: {
		netBuilder_.setType(dssd::andl::NetType::TPN_T);
	}
										 break;
	default:
		//error
		break;
	}
	
	return true;
}

//unfold marking expression
bool SP_IddUnFoldExpr::EvalExpr( wxString& markingExp,wxString& p_scolorset)
{
	dssd::andl::Net_ptr m_ColoredNet;
	dssd::andl::Net_ptr m_UnfoldedNet;
	SP_ColoredNetBuilder builder(true);
	dssd::andl::simple_net_builder builder1;
	bool l_bRes = true;
	if (builder(m_pcGraph))
	{
		m_ColoredNet = builder.GetNet();
		 
		try
		{
			dssd::functions::FunctionRegistry fReg;
			dssd::unfolding::net_unfolder<dssd::unfolding::idd_guard_representation> unfolder(m_ColoredNet, fReg);
			using solution_space = idd_guard_representation;

			 
			bool b=LoadNetDefentions(m_ColoredNet, builder1);//register the net
		   
			//===================================
			size_t places;
			for (auto p : *(m_ColoredNet->places_))
			{
				if (!p) continue;

				if (p->name_ == m_sPname)
				{
					places += UnfoldPlace1(markingExp.ToStdString(),p, builder1);
				}

			}
			//=========================================
		
		}
		catch (const std::exception &exc)
		{
			wxString l_sMsg = exc.what();
			SP_LOGERROR(l_sMsg);
			l_bRes = false;
		}
		 
	}
	 
	return l_bRes;
}

 
colExpr SP_IddUnFoldExpr::parseExpr(const std::string& expr_string)
{
	colExprParser_->parse(expr_string);
	colExpr expr(colExprBuilder_->node());
	colExprBuilder_->reset();
	return expr;
}

void SP_IddUnFoldExpr::substituteColorFunctions(colExpr &expr)
{
	dssd::colexpr::col_expr_function_instantiator fi(colDefinitions_);
	dssd::misc::modifier_complete_tree<dssd::colexpr::col_expr_function_instantiator> modifier(fi);
	modifier(expr);
	//lsBefore= "expression after function substitution: " + expr.toString();
	//SP_LOGMESSAGE(lsBefore);

}


void SP_IddUnFoldExpr::flatExpression(colExpr &expr, std::string colorset)
{
	std::vector<std::string> varNames;
	dssd::colexpr::DefaultExpressionCreator expr_creator(colDefinitions_, varNames, "", colorset);
	dssd::colexpr::col_expr_variable_substituter es(expr_creator);
 
	dssd::misc::modifier_complete_tree<dssd::colexpr::col_expr_variable_substituter> modifier(es);
	modifier(expr);//if i uncommented this, the memory is not acciable(pointers), e.g, [ISNeighbour(x,y,a,b)]1`(x,y,a,b), the problem here is with the tupel is two elements not 4!, 
	 wxString after;
	 after<< "expression after flattening: " << expr.toString();
}
int  SP_IddUnFoldExpr::UnfoldPlace1(std::string markingExp,dssd::andl::Place_ptr p, dssd::andl::simple_net_builder& netBuilder_)
{
	
	placeLookUpTable l_lkt;
	if (!p) return 0; // should not happen

	std::string name = p->name_;
	std::string colorset = p->colorset_;
	std::string marking = markingExp;// p->marking_;//"[(x=MID&y=MID)]100`all";
	
	//the marking; we need an evaluator, which extracts the subcolorset whose related place carries a specified number of tokens
	wxString msg;
	msg << "place " << name << " with colorset " << colorset << " marking " << marking;
	//SP_LOGERROR(msg);
	colExpr markingExpr = parseExpr(marking);
	substituteColorFunctions(markingExpr);
	flatExpression(markingExpr, colorset);
	wxString ms1;
	ms1 << "marking Expr: " << markingExpr.toString();
	//SP_LOGERROR(ms1);
	

	exprVec expressions;
	checkColExpr<colexpr_expression_collector>(markingExpr, colDefinitions_, expressions);

	colExprVec guards;

	std::set<std::string> varNames;

	auto desc = colDefinitions_.lookUpName(colorset);
	colExpr implicitGuard;
	colExpr implicitGuardNegate;
	if (desc.isSubSet() && desc.guard_ != "")
	{
		implicitGuard = parseExpr(desc.guard_);
		//guards.push_back(implicitGuard);
		wxString ms2;
	ms2<< "adding implicit guard " << implicitGuard.toString();
	//SP_LOGERROR(ms2);
		implicitGuardNegate = parseExpr("!(" + desc.guard_ + ")");
		guards.push_back(implicitGuardNegate);
		auto tmpVarNames = collectVarNames(implicitGuard, colDefinitions_);
		varNames.insert(std::begin(tmpVarNames), std::end(tmpVarNames));
	}

	int places = 0;
	//colexpr remaining = true
	for (auto &expr : expressions)
	{
		//TODO here we must assure that the value does not contains variables which are not
		//defined by the guard or the color e.g. [x<3]x`x OK; [x<3]y`x OK; would create several place instances
		//of the same color with different initial markings
		//SP_LOGMESSAGE("**Color:" + expr.color_.toString());
		auto tmpVarNames = collectVarNames(expr, colDefinitions_);
		
		colExprVec guard;
		if (!implicitGuard.empty())
		{
			guard.push_back(implicitGuard);
			auto tmpVarNames2 = collectVarNames(implicitGuard, colDefinitions_);
			tmpVarNames.insert(std::begin(tmpVarNames2), std::end(tmpVarNames2));
		}

		varNames.insert(std::begin(tmpVarNames), std::end(tmpVarNames));

		//TODO: we need to handle tuples
		/*
		if(tmpVarNames.empty())
		{
		std::vector<std::string> auxVarNames;
		dssd::colexpr::DefaultExpressionCreator expr_create(colDefinitions_, auxVarNames, "", "");
		auto dummy_expression = expr_create(colorset);
		expr.guard_ = parseExpr(dummy_expression + " == (" + expr.color_.toString() + ")");
		expr.color_ = parseExpr(dummy_expression);
		flatExpression(expr.guard_, colorset);
		flatExpression(expr.color_, colorset);
		logger(aux::logDEBUG) << "generated guard: " << expr.guard_.toString();

		tmpVarNames = collectVarNames(expr, colDefinitions_);
		}
		*/

		if (!expr.guard_.empty())
		{
			guards.push_back(expr.guard_);
			guard.push_back(expr.guard_);
		}

		auto tmpVarNames_ = dssd::aux::toVector(tmpVarNames);
		solution_space sol(guard, colDefinitions_, tmpVarNames_);

		places += createPlaces(sol, name, p->type_, p->fixed_, expr.color_, colDefinitions_,
			expr.value_, netBuilder_, l_lkt, true);
	}

	if (places == 0)
	{
		wxString ms3;
		ms3<< "the marking expression does not define any valid places !";
		//SP_LOGERROR(ms3);
	}
	
	// << "check whether all guards define disjoint solutions sets!";
	
	auto varNames_ = dssd::aux::toVector(varNames);
	solution_space sol(guards, colDefinitions_, varNames_);
	auto sol_ = sol.next();
	if (guards.size() > 1 && sol_)
	{
		//throw exception, non disjoint guards
		wxString ms5;
		ms5<< "throw exception, non disjoint guards";
		SP_LOGERROR(ms5);
	}
	/**comented for phase variation
	//create  the remaining places !guard_1 && !guard_2 ... and !guard_n
	//SP_LOGERROR ( "create  the remaining places with 0 tokens");
	if (!varNames_.empty())
	{
		solution_space remaining(guards, colDefinitions_, varNames_, true);
		if (!remaining.empty())
		{
			dssd::colexpr::DefaultExpressionCreator expr_create(colDefinitions_, varNames_, "", "");
			auto dummy_expression = expr_create(colorset);
			colExpr dummy = parseExpr(dummy_expression);
			colExpr zero = parseExpr("0");
			places += createPlaces(remaining, name, p->type_, p->fixed_, dummy, colDefinitions_, zero,
				netBuilder_, l_lkt,true);
		}
	}
	else
	{ //there are no constraint which could be inverted
		std::vector<std::string> auxVarNames;
		dssd::colexpr::DefaultExpressionCreator expr_create(colDefinitions_, auxVarNames, "", "");
		auto dummy_expression = expr_create(colorset);
		if (!dummy_expression.empty())
		{
			colExpr dummy = parseExpr(dummy_expression);
			auxVarNames = dssd::aux::toVector(collectVarNames(dummy, colDefinitions_));
			colExprVec guard;
			solution_space remaining(guard, colDefinitions_, auxVarNames);
			colExpr zero = parseExpr("0");
			places += createPlaces(remaining, name, p->type_, p->fixed_, dummy, colDefinitions_, zero,
				netBuilder_, l_lkt,true);
		}
		colDefinitions_.removeAuxilaryVariables();
	}

 */
	m_lkt = l_lkt;

	 
	return places;
}

unsigned  SP_IddUnFoldExpr::createPlaces(solution_space &sol,
	std::string name, dssd::andl::PlType type, bool fixed,
	colExpr color, colEnv &env,
	colExpr token, dssd::andl::simple_net_builder& netBuilder, placeLookUpTable &unfoldedPlaces, bool evalTokens)
{
	unsigned places = 0;
	auto varNamesColor = collectVarNames(color, env);

	wxString ms1;
	ms1<< "place " << name << " --> " << color.toString();
	//SP_LOGERROR(ms1);
	//TODO: check whether these sets are consistent
	//int nrVars = varNamesColor.size();
	while (auto s = sol.next())
	{
		for (auto const & it_name : varNamesColor)
		{
			//TODO optimize this
			if (env.isIntVariable(it_name))
			{
				env.setIntVarValue(it_name, s->getVariable(it_name).val());
			}
			else
			{
				env.setStringVarValue(it_name, s->getVariable(it_name).val());
			}
		}

		auto res_color = evalColExpr<colexpr_node_evaluator>(color, env);
		std::string unfoldedName = name + res_color.node_.linearise();
		std::string unfoldedToken;
		if (token.empty())
		{
			unfoldedToken = "1";
		}
		else
		{
			dssd::colexpr::stringBuilderWithColorResolution sB(env);
			dssd::misc::visitor_complete_tree<
				dssd::colexpr::stringBuilderWithColorResolution> visitor(sB);
			bool tmp = env.getEvalFunctions();
			env.setEvalFunctions(evalTokens);
			visitor(evalColExpr<colexpr_node_evaluator>(token, env).node_);
			unfoldedToken = sB.sstream.str();
			env.setEvalFunctions(tmp);
		}
		Place_ptr up = std::make_shared<dssd::andl::Place>(
			type, unfoldedName, unfoldedToken, "");
		up->fixed_ = fixed;
		//set colored name length as prefix length
		up->prefix_ = name.size();
		auto res_insert = unfoldedPlaces.insert(	std::make_pair(unfoldedName,UnfoldedPlace{ 0, up }));
		wxString logs;
		 
		if (res_insert.second)
		{
			wxString l_s;
			l_s<< unfoldedName << " = " << unfoldedToken;
			//SP_LOGMESSAGE(l_s);
			places++;
		}
		else
		{
			//break;	//place already unfolded
		}

	}
	return places;

}


bool SP_IddUnFoldExpr::CheckGuardEXpression(std::string& p_sExp,std::string& p_sErrorPos)
{
	

	//first step:register the net definitions
	dssd::andl::Net_ptr m_ColoredNet;
	dssd::andl::Net_ptr m_UnfoldedNet;
	SP_ColoredNetBuilder builder;
	dssd::andl::simple_net_builder builder1;
	bool l_bRes = true;
	if (builder(m_pcGraph))
	{
		m_ColoredNet = builder.GetNet();

		try
		{
			 
			bool lb = LoadNetDefentions(m_ColoredNet, builder1);

		}
		catch (const std::exception &exc)
		{
			 
			SP_LOGERROR(p_sErrorPos);
			l_bRes = false;
		}
	}
	try{
	colExprVec	guards;
	colExpr expr = parseExpr(p_sExp);
	substituteColorFunctions(expr);
	flatExpression(expr);
	guards.push_back(expr);
	auto tmpVarNames = collectVarNames(expr, colDefinitions_);
	std::set<string> variables;
	variables.insert(std::begin(tmpVarNames), std::end(tmpVarNames));
	auto varNames = dssd::aux::toVector(variables);
	
	solution_space sol(guards, colDefinitions_, varNames);
	//
	auto r = evalColExpr<colexpr_descriptor_evaluator>(expr, colDefinitions_);

	wxString l_s;
	if (!r.isGuard())
	{
	 
	}
	}catch(dssd::exc::Exc& e){
		wxString l_sCaught = e.what();
		l_sCaught.Replace("\n", "; ");
		SP_LOGERROR_(l_sCaught +p_sErrorPos);
		l_bRes = false;
	}
	return l_bRes;
}
bool SP_IddUnFoldExpr::CheckCoLourExpression(std::string p_sExp,std::string& p_sColorSet, wxString& p_sErrorPos)
{
	//p_sExp = "[(x=50&y=50&c=A)]1`((x,y),c)";
	bool l_bRes = true;
	wxString l_sError;
	 

	//first step: is to check the syntax
	dssd::colexpr::builder*   l_pcBuilder = new dssd::colexpr::builder();//dssd syntax
	dssd::colexpr::parser* l_pcdssdParser = new dssd::colexpr::parser(l_pcBuilder);//dssd syntax

	try {
		l_pcdssdParser->parse(p_sExp);

	}
	catch (dssd::exc::ParseExc e) {
		SP_LOGERROR(p_sErrorPos);
		l_bRes = false;
	}


	//second step:register the net definitions
	dssd::andl::Net_ptr m_ColoredNet;
	dssd::andl::Net_ptr m_UnfoldedNet;
	SP_ColoredNetBuilder builder;
	dssd::andl::simple_net_builder builder1;
    
	if (builder(m_pcGraph))
	{
		m_ColoredNet = builder.GetNet();

		try
		{
			
			bool lb=LoadNetDefentions(m_ColoredNet, builder1);

		}
		catch (const std::exception &exc)
		{
			wxString l_sMsg = exc.what();
			SP_LOGERROR(l_sMsg);
			//SP_LOGERROR(p_sErrorPos);
			l_bRes = false;
		}
	}
    
	//third step:collect variables from the arcs inscription, verify that those vars are registered and match the color set.
	if (l_bRes)
	{
		std::set<std::string> variables;
		auto colorsetElementDescriptor = colDefinitions_.lookUpName(p_sColorSet);
		std::string colorsetGuard = colorsetElementDescriptor.guard_;
		wxString desc;
		desc<< p_sColorSet << " --> " << colorsetElementDescriptor.toString();
		colExpr expr = parseExpr(p_sExp);
		substituteColorFunctions(expr);

		if (p_sExp.find("all") != std::string::npos)
		{
			flatExpression(expr, p_sColorSet);
		}
		flatExpression(expr, p_sColorSet);
		//////////////////////////////////
		colExpr markingExpr = parseExpr(p_sExp);


		/*
		* BUGFIX: all
		*/
		auto tmpVarNames = collectVarNames(markingExpr, colDefinitions_);
		auto res = colDefinitions_.lookUpColorset(p_sColorSet);
		std::map<std::string, std::string> mapVarsNames;
		if (res.guard_.size() > 0) {
			colExpr expr = parseExpr(res.guard_);
			auto tmpVarNames1 = collectVarNames(expr, colDefinitions_);
			tmpVarNames.insert(tmpVarNames1.begin(), tmpVarNames1.end());
			for (std::string strName : tmpVarNames) {
				std::string strColor = colDefinitions_.getVarColorsetName(strName);
				mapVarsNames[strColor] = strName;
				logger(aux::logDEBUG1) << "color: " << strColor << ":" << strName;
			}
		}
		logger(aux::logDEBUG1) << "marking Exp: " << markingExpr;
		// if(mapVarsNames.size() > 0) {
		dssd::colexpr::DefaultExpressionCreator expr_creator(colDefinitions_, mapVarsNames, "", p_sColorSet);
		// dssd::colexpr::DefaultExpressionCreator expr_creator(colDefinitions_, dssd::aux::toVector(tmpVarNames), "", colorset);
		dssd::colexpr::col_expr_variable_substituter es(expr_creator);
		dssd::misc::modifier_complete_tree<dssd::colexpr::col_expr_variable_substituter> modifierExp(es);
		modifierExp(markingExpr);


		///////////////////////////////////
		variables = collectVarNames(expr, colDefinitions_, variables);
		exprVec arcExpressions;
		checkColExpr<colexpr_expression_collector>(expr, colDefinitions_, arcExpressions);
		for (auto& exp : arcExpressions)
		{
			if (!colorsetGuard.empty())
			{
				std::string newGuard = colorsetGuard;
				if (!exp.guard_.empty())
				{
					newGuard = newGuard + " & " + exp.guard_.toString();
				}
				 
				exp.guard_ = parseExpr(newGuard);
			}
			 
				auto res = evalColExpr<colexpr_descriptor_evaluator>(exp.color_, colDefinitions_);
				wxString l_sMs;
				l_sMs  << " --> " << res.ed_.toString();
				if (!elementOf(res.ed_, colorsetElementDescriptor, colDefinitions_))
				{
					SP_LOGERROR(p_sErrorPos);
					l_bRes = false;
					break;
					 
				}
			 
		 
		}
	}		 
	
	return l_bRes;
}


bool SP_IddUnFoldExpr::CheckTransRateFunction(SP_DS_Node* p_pcTr, wxString& p_sRateFun)
{

	//first step: is to check the syntax
	dssd::colexpr::builder*   l_pcBuilder = new dssd::colexpr::builder();//dssd syntax
	dssd::colexpr::parser* l_pcdssdParser = new dssd::colexpr::parser(l_pcBuilder);//dssd syntax
	bool l_bRes = true;
  
	//second step:register the net definitions
	dssd::andl::Net_ptr m_ColoredNet;
	dssd::andl::Net_ptr m_UnfoldedNet;
	SP_ColoredNetBuilder builder;
	dssd::andl::simple_net_builder builder1;

	if (builder(m_pcGraph))
	{
		m_ColoredNet = builder.GetNet();

		try
		{

			bool lb = LoadNetDefentions(m_ColoredNet, builder1);

		}
		catch (const std::exception &exc)
		{
			wxString l_sMsg = exc.what();
			SP_LOGERROR(l_sMsg);
			//SP_LOGERROR(p_sErrorPos);
			l_bRes = false;

		}
	}

	wxString l_sTransName = dynamic_cast<SP_DS_NameAttribute*>(p_pcTr->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
	dssd::andl::Transition_ptr  t_u;
	for (auto t : *m_ColoredNet->transitions_)
	{
		if (!t) continue;
		
		if (t->name_ == l_sTransName)
		{
			t_u = t;
			break;
		}
		 
	}
	std::string func = p_sRateFun.ToStdString();
	func.erase(std::remove_if(func.begin(), func.end(),
		std::bind(std::isspace<char>, std::placeholders::_1, std::locale::classic())), func.end());

	std::stringstream f;

	colParser parser;
	unsigned guards_found = 0;
	unsigned guards_fulfilled = 0;
	std::vector < std::string > functions;
	{
		std::size_t last_pos = 0;
		std::size_t pos = func.find("++");
		while (pos != std::string::npos)
		{
			functions.push_back(func.substr(last_pos, pos - last_pos));
			last_pos = pos + 2;
			pos = func.find("++", pos + 1);
		}
		functions.push_back(func.substr(last_pos));
	}

	for (auto const& f_i : functions)
	{
		std::stringstream expr_stream;
		bool mode = false;
		unsigned int pos_found = 0;
		bool ignore = false;
		std::string ident;
		for (unsigned pos = 0; pos < f_i.length(); ++pos)
		{
			auto c = f_i[pos];
			if (c == '[')
			{
				pos_found = pos;
				if (!mode)
				{
					mode = true;
				}
				else
				{
					l_bRes = false;
					//throw dssd::exc::Exc(_FLINE_, "syntax error in rate function");
					SP_LOGERROR("syntax error in rate function");
					return l_bRes;
				}
			}
			else if (c == ']')
			{
				if (mode)
				{
					mode = false;
					colExpr colorExpr;
					//must be a guard
					if (pos_found == 0)
					{
						std::string tmp = "[";
						tmp += expr_stream.str();
						tmp += "]";
						colorExpr = parser(tmp);
						substituteColorFunctions(colorExpr);
						flatExpression(colorExpr, "");
						auto r = evalColExpr<colexpr_descriptor_evaluator>(colorExpr, colDefinitions_);

						if (!r.isGuard())
						{
							l_bRes = false;
							//throw dssd::exc::Exc(_FLINE_, "expecting guard in rate function definition\n");
							SP_LOGERROR("expecting guard in rate function definition\n");
							return l_bRes;
						}

						guards_found++;
						logger(aux::logDEBUG) << "guard found:" << colorExpr;
						auto b = evalColExpr<colexpr_guard_evaluator>(colorExpr, colDefinitions_);
						if (b.resValue_)
						{
							guards_fulfilled++;
						}
						else
						{
							ignore = true;
						}

					}
					else if (!ignore)
					{
						//SP_LOGMESSAGE(expr_stream.str());

						colorExpr = parser(expr_stream.str());
						substituteColorFunctions(colorExpr);
						flatExpression(colorExpr, "");
						auto res = evalColExpr<colexpr_node_evaluator>(colorExpr, colDefinitions_);
						if (!ident.empty())
						{
							f << ident << res.node_.linearise();
							ident.clear();
						}
						else
						{
							f << res.node_.toString();
						}
					}

				}
				expr_stream.str("");
			}
			else
			{
				if (mode)
				{
					expr_stream << c;
				}
				else if (!ignore)
				{
					if (aux::isextalnum(c) &&
						(!ident.empty() || !std::isdigit(c)))
					{
						ident += c;
						if ((pos + 1 == f_i.length()) ||
							(!aux::isextalnum(f_i[pos + 1]) && f_i[pos + 1] != '['))
						{
							//check if ident is pre-place, if so build sum

							unsigned count = 0;
							std::set<std::string> used;
							std::string sum;
							if (t_u->conditions_ && !t_u->conditions_->empty()) {
								for (auto& c : *(t_u->conditions_)) {
									if (!c) continue;
									const std::string& place = c->place_;
									//auto it = unfoldedPlaces.find(place);
									/**
									if (ident == place.substr(0, it->second.unfoldedPlace_->prefix_))
									{
										if (used.find(place) == used.end())
										{
											used.insert(place);
											if (count > 0)
											{
												sum += '+';
											}
											sum += place;
											++count;
										}
									}
									*/
								}
							}
							if (t_u->updates_ && !t_u->updates_->empty()) {
								for (auto& u : *(t_u->updates_)) {
									if (!u || u->type_ == andl::UpdateType::INCREASE_T) continue;
									const std::string& place = u->place_;
									//auto it = unfoldedPlaces.find(place);
									/**
									if (ident == place.substr(0, it->second.unfoldedPlace_->prefix_))
									{
										if (used.find(place) == used.end())
										{
											used.insert(place);
											if (count > 0)
											{
												sum += '+';
											}
											sum += place;
											++count;
										}
									}
									*/
								}
							}
							if (count == 0)
							{
								f << ident;
							}
							else if (count == 1)
							{
								f << sum;
							}
							else
							{
								f << '(' << sum << ')';
							}
							ident.clear();
						}
					}
					else
					{
						f << c;
					}
				}
			}
		}
	}
	if (guards_fulfilled > 1 || (guards_found < functions.size() - 1)
		|| (guards_fulfilled == 1 && guards_found < functions.size() - 1))
	{
		l_bRes = false;
		SP_LOGERROR(t_u->name_ + " rate function expression does not define a unique rate function\n");
		return l_bRes;
		//throw dssd::exc::Exc(_FLINE_, t_u->name_ + " rate function expression does not define a unique rate function\n");
		
		
	}
	t_u->function_ = f.str();
	return l_bRes;
}