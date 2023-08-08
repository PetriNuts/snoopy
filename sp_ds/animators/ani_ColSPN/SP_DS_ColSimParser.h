#ifndef SP_DS_COL_SIM_PARSER_H__
#define SP_DS_COL_SIM_PARSER_H__

#include <iostream>
#include <string>
#include <stdexcept>
#include <sstream>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>
#include <boost/fusion/include/at.hpp>
#include <cmath>
#include <map>

namespace qi = boost::spirit::qi;
namespace phx = boost::phoenix;
namespace ascii =  boost::spirit::ascii;


struct ExpressionEvaluator
{
	std::map<std::string, double> variablesMap;

	double evaluate(double left, char op, double right) const
	{
		switch (op)
		{
		case '+': return left + right;
		case '-': return left - right;
		case '*': return left * right;
		case '/': return left / right;
		case '^': return std::pow(left, right);
		case '<': return left < right ? 1.0 : 0.0;   // Less than
		case '>': return left > right ? 1.0 : 0.0;   // Greater than
		case '=': return left == right ? 1.0 : 0.0;  // Equal to
		default: throw std::runtime_error("Invalid operator");
		}
	}

	void setVariable(const std::string& name, double value)
	{
		variablesMap[name] = value;
	}

	double lookupVariable(const std::vector<char>& nameVec) const
	{
		std::string name(nameVec.begin(), nameVec.end());

		auto it = variablesMap.find(name);
		if (it != variablesMap.end())
		{
			return it->second;
		}
		throw std::runtime_error("Unknown constant or variable: " + name);
	}
};

template <typename Iterator>
struct ExpressionGrammar : qi::grammar<Iterator, double()>
{
	ExpressionGrammar( const ExpressionEvaluator& eval)
		: ExpressionGrammar::base_type(expression), evaluator(eval)
	{
		using namespace qi;

		// Add space skipping to all rules
		expression = skip(space)[comparison[_val = _1] >> *(('+' >> comparison[_val = phx::bind(&ExpressionEvaluator::evaluate, evaluator, _val, '+', _1)])
			| ('-' >> comparison[_val = phx::bind(&ExpressionEvaluator::evaluate, evaluator, _val, '-', _1)]))];
		comparison = skip(space)[term[_val = _1] >> *(('=' >> term[_val = phx::bind(&ExpressionEvaluator::evaluate, evaluator, _val, '=', _1)])
			| ('<' >> term[_val = phx::bind(&ExpressionEvaluator::evaluate, evaluator, _val, '<', _1)])
			| ('>' >> term[_val = phx::bind(&ExpressionEvaluator::evaluate, evaluator, _val, '>', _1)]))];
		term = skip(space)[power[_val = _1] >> *(('*' >> power[_val = phx::bind(&ExpressionEvaluator::evaluate, evaluator, _val, '*', _1)])
			| ('/' >> power[_val = phx::bind(&ExpressionEvaluator::evaluate, evaluator, _val, '/', _1)]))];
		power = skip(space)[factor[_val = _1] >> *(('^' >> factor[_val = phx::bind(&ExpressionEvaluator::evaluate, evaluator, _val, '^', _1)]))];
		factor = skip(space)[double_ | variable | ('(' >> expression >> ')')];
		variable = qi::lexeme[+qi::char_("a-zA-Z0-9_")][_val = phx::bind(&ExpressionEvaluator::lookupVariable, evaluator, qi::_1)];
		///variable = skip(space)[lexeme[alpha >> *(alnum | char_('_'))]][_val = phx::bind(&ExpressionEvaluator::lookUpVariable, evaluator, _1)];
		BOOST_SPIRIT_DEBUG_NODES((expression)(comparison)(term)(power)(factor)(variable))


		/**
		// Add space skipping to all rules
		expression = skip(space)[term[_val = _1] >> *(('+' >> term[_val = phx::bind(&ExpressionEvaluator::evaluate, evaluator, _val, '+', _1)])
			| ('-' >> term[_val = phx::bind(&ExpressionEvaluator::evaluate, evaluator, _val, '-', _1)]))];
		term = skip(space)[power[_val = _1] >> *(('*' >> power[_val = phx::bind(&ExpressionEvaluator::evaluate, evaluator, _val, '*', _1)])
			| ('/' >> power[_val = phx::bind(&ExpressionEvaluator::evaluate, evaluator, _val, '/', _1)]))];
		power = skip(space)[factor[_val = _1] >> *(('^' >> factor[_val = phx::bind(&ExpressionEvaluator::evaluate, evaluator, _val, '^', _1)]))];
		factor = skip(space)[double_ | ('(' >> expression >> ')')];

	 
		BOOST_SPIRIT_DEBUG_NODES((expression)(term)(power)(factor))
			*/

	}

	qi::rule<Iterator, double()> expression, comparison, term, power, factor;
	 qi::rule<Iterator, double()> variable;
	//qi::rule<Iterator, double(), ascii::space_type> variable;
	ExpressionEvaluator evaluator;
};
 

#endif // SP_DS_COL_SIM_PARSER_H__
