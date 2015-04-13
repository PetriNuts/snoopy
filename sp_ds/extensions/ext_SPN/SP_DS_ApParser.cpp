/*
 * SP_DS_ApParser.cpp
 *
 *  Created on: Mar 24, 2010
 *      Author: crohr
 */
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>

#include "SP_DS_ApParser.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"

#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_core/SP_Core.h"

/*
 * Begin IL formula grammar definition
 */
using namespace rfdag;


namespace client
{
    namespace phoenix = boost::phoenix;
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;

    struct numunop_ : qi::symbols<char, TType>
    {
    	numunop_()
        {
            add
                ("sqr"   , SQR)
                ("sqrt"  , SQRT)
                ("sin"   , SIN)
                ("asin"  , ASIN)
                ("abs"   , ABS)
                ("cos"   , COS)
                ("acos"  , ACOS)
                ("tan"   , TAN)
                ("atan"  , ATAN)
                ("log"   , LOG)
                ("log10" , LOG10)
                ("ceil"  , CEIL)
                ("floor" , FLOOR)
            ;
        }

    } numunop;

    struct numbinop_ : qi::symbols<char, TType>
    {
    	numbinop_()
        {
            add
                ("pow"   , POW)
            ;
        }

    } numbinop;

    struct add_ : qi::symbols<char, TType>
    {
    	add_()
        {
            add
                ("+"    , ADD)
                ("-"    , MINUS)
            ;
        }

    } add;

    struct mul_ : qi::symbols<char, TType>
    {
    	mul_()
        {
            add
                ("*"    , MUL)
                ("/"    , DIV)
            ;
        }

    } mul;

    struct cmp_ : qi::symbols<char, TType>
    {
    	cmp_()
        {
            add
                ("="    , EQ)
                ("=="   , EQ)
                ("!="   , NEQ)
                ("<"    , LESS)
                (">"    , GREAT)
                ("<="   , LEQ)
                (">="   , GEQ)
            ;
        }

    } cmp;

    struct logicunop_ : qi::symbols<char, TType>
    {
    	logicunop_()
        {
            add
                ("!"    , NOT)
            ;
        }

    } logicunop;

    struct logicbinop_ : qi::symbols<char, TType>
    {
    	logicbinop_()
        {
            add
                ("&"    , AND)
                ("&&"    , AND)
                ("|"    , OR)
                ("||"    , OR)
                ("->"    , IF)
                ("<-"    , IFB)
                ("<->"    , IFF)
            ;
        }

    } logicbinop;

    struct places_ : qi::symbols<char, unsigned>{} places;

    struct parameters_ : qi::symbols<char, unsigned>{} parameters;

    ///////////////////////////////////////////////////////////////////////////
    //  Our IL formula grammar definition
    ///////////////////////////////////////////////////////////////////////////
    template <typename Iterator>
    struct il_formula_grammar
		: qi::grammar<Iterator, vectorRFDAG(), qi::locals<std::string>, ascii::space_type>
    {
    	  il_formula_grammar()
          : il_formula_grammar::base_type(START, "START")
//          : il_formula_grammar::base_type(formula, "formula")
        {
    		using qi::lexeme;
            using qi::lit;
            using qi::on_error;
            using qi::fail;
            using qi::double_;
            using qi::alpha;
            using qi::alnum;
            using ascii::char_;
            using ascii::string;
            using namespace qi::labels;

            using phoenix::construct;
            using phoenix::val;
			
			START   = formula % ';'
					;

            formula = unary[_val = _1] >> *(logicbinop > unary)[_val += construct<RFDAG>(_1, _2)]
					;

            unary   = (logicunop > ap)[_val = construct<RFDAG>(_1, _2)]
                    | ap[_val = _1]
            		;

            ap 		= (lit('(') > formula > lit(')'))[_val = _1]
            		| (expr >> cmp >> expr)[_val = construct<RFDAG>(_2, _1, _3)]
            		;

            expr 	= term[_val = _1] >> *(add > term)[_val += construct<RFDAG>(_1, _2)]
            		;

            term 	= factor[_val = _1] >> *(mul > factor)[_val += construct<RFDAG>(_1, _2)]
            		;

            factor 	= (lit('(') > expr > lit(')'))[_val = _1]
            		| (numbinop > lit('(') > expr > lit(',') > expr > lit(')'))[_val = construct<RFDAG>(_1, _2, _3)]
            		| (numunop > lit('(') > expr > lit(')'))[_val = construct<RFDAG>(_1, _2)]
            		| placename[_val = _1]
            		| num[_val = _1]
            		;

            num 	= double_[_val = construct<RFDAG>(_1)]
            		;

            placename = lexeme[places >> !(alnum | '_')][_val = construct<RFDAG>(PLACE, _1)]
            		| lexeme[parameters >> !(alnum | '_')][_val = construct<RFDAG>(PARAMETER, _1)]
            		;

            START.name("START");
			formula.name("formula");
            ap.name("ap");
            expr.name("expr");
            term.name("term");
            factor.name("factor");
            num.name("num");
            placename.name("placename");

            on_error<fail>
            (
                START
				, std::cout
                    << val("Error! Expecting ")
                    << _4                               // what failed?
                    << val(" here: \"")
                    << construct<std::string>(_3, _2)   // iterators to error-pos, end
                    << val("\"")
                    << std::endl
            );
        }

		  qi::rule<Iterator, vectorRFDAG(), qi::locals<std::string>, ascii::space_type> START;
          qi::rule<Iterator, RFDAG(), qi::locals<std::string>, ascii::space_type> formula;
          qi::rule<Iterator, RFDAG(), qi::locals<std::string>, ascii::space_type> unary;
          qi::rule<Iterator, RFDAG(), qi::locals<std::string>, ascii::space_type> ap;
          qi::rule<Iterator, RFDAG(), qi::locals<std::string>, ascii::space_type> expr;
          qi::rule<Iterator, RFDAG(), qi::locals<std::string>, ascii::space_type> term;
          qi::rule<Iterator, RFDAG(), qi::locals<std::string>, ascii::space_type> factor;
          qi::rule<Iterator, RFDAG(), qi::locals<std::string>, ascii::space_type> num;
          qi::rule<Iterator, RFDAG(), qi::locals<std::string>, ascii::space_type> placename;

    };

}

/*
 * End IL formula grammar definition
 */

SP_DS_ApParser::SP_DS_ApParser(SP_DS_Graph* p_pcGraph):
m_pcGraph(p_pcGraph)
{
	//Initialise();
}


SP_DS_ApParser::~SP_DS_ApParser()
{

}

bool
SP_DS_ApParser::Initialise()
{	

	if( m_pcGraph->GetNetclass()->GetName() == SP_DS_COLSPN_CLASS )
	{
		LoadUnfoldedPlaces();
	}
	else
	{
		LoadPlaces();
	}
	
	LoadParameters();

	return true;
}

void
SP_DS_ApParser::LoadPlaces()
{
	SP_DS_Nodeclass* l_pcNodeclass = m_pcGraph->GetNodeclass( wxT("Place") );

	client::places.clear();
	m_Id2Placename.resize(l_pcNodeclass->GetElements()->size());
	m_Placename2Id.clear();

	unsigned l_nPos = 0;
	
	SP_ListNode::const_iterator l_itElem;
	for(l_itElem = l_pcNodeclass->GetElements()->begin();
		l_itElem != l_pcNodeclass->GetElements()->end();
		++l_itElem)
	{
		wxString l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>((*l_itElem)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
		m_Placename2Id[l_sPlaceName] = l_nPos;
		m_Id2Placename[l_nPos] = l_sPlaceName;

		client::places.add(l_sPlaceName.mb_str(), l_nPos);

		++l_nPos;
	}
}

void
SP_DS_ApParser::LoadParameters()
{

	SP_DS_Metadataclass* mc = m_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);
	SP_ListMetadata::const_iterator it;

	client::parameters.clear();
	m_Parametername2Id.clear();
	m_Id2Parametername.resize(mc->GetElements()->size());
	//m_Parameter.resize(mc->GetElements()->size());
	unsigned l_nPos = 0;

	for (it = mc->GetElements()->begin(); it != mc->GetElements()->end(); ++it) {

		SP_DS_Metadata* l_pcConstant = *it;
		wxString l_sName = dynamic_cast<SP_DS_NameAttribute*>(l_pcConstant->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		m_Parametername2Id[l_sName] = l_nPos;
		m_Id2Parametername[l_nPos] = l_sName;

		client::parameters.add(l_sName.mb_str(), l_nPos);

		++l_nPos;
	}


/*
	SP_DS_Nodeclass* l_pcNodeclass = m_pcGraph->GetNodeclass( wxT("Parameter") );

	client::parameters.clear();
	m_Parametername2Id.clear();
	m_Id2Parametername.resize(l_pcNodeclass->GetElements()->size());
	unsigned l_nPos = 0;

	SP_ListNode::const_iterator l_itElem;
	for(l_itElem = l_pcNodeclass->GetElements()->begin();
		l_itElem != l_pcNodeclass->GetElements()->end();
		++l_itElem )
	{
		wxString l_sParameterName = dynamic_cast<SP_DS_NameAttribute*>((*l_itElem)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
		m_Parametername2Id[l_sParameterName] = l_nPos;
		m_Id2Parametername[l_nPos] = l_sParameterName;

		client::parameters.add(l_sParameterName.mb_str(), l_nPos);

		SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >( (*l_itElem)->GetAttribute( wxT("ParameterList") ) );
		double l_nDouble = 0;
		l_pcColList->GetActiveCellValue( 1).ToDouble( &l_nDouble );

		++l_nPos;
	}
*/
}

bool
SP_DS_ApParser::Parse(const wxString& p_ToParse)
{
	m_Expressions.clear();

	m_FormulaString = p_ToParse;

#if wxABI_VERSION > 30000
    typedef client::il_formula_grammar<std::string::const_iterator> il_formula_grammar;
    il_formula_grammar l_Formula; // Our grammar
    std::string l_FormulaString = m_FormulaString;
    std::string::const_iterator l_Iter = l_FormulaString.begin();
    std::string::const_iterator l_End = l_FormulaString.end();
#else
    typedef client::il_formula_grammar<wxString::const_iterator> il_formula_grammar;
    il_formula_grammar l_Formula; // Our grammar

    wxString::const_iterator l_Iter = m_FormulaString.begin();
    wxString::const_iterator l_End = m_FormulaString.end();
#endif
	
    using boost::spirit::ascii::space;
	bool l_Return = phrase_parse(l_Iter, l_End, l_Formula, space, m_Expressions);

	if(l_Return)
	{
		for(size_t i = 0; i < m_Expressions.size(); ++i)
		{	
			SetNames(m_Expressions[i]);
		}
	}
	else
	{
		wxString l_sMsg = wxT("Parsing formula: ");
		l_sMsg << m_FormulaString;
		SP_LOGERROR( l_sMsg);
	}
	return l_Return;
}

void
SP_DS_ApParser::SetNames(RFDAG& p_Expression)
{
	switch(p_Expression.token)
	{
	case IDENT:
	case PLACE:
		if(p_Expression.placename.empty())
		{
			p_Expression.placename = m_Id2Placename.at(p_Expression.placeid).mb_str();
		}
		break;
	case PARAMETER:
		if(p_Expression.placename.empty())
		{
			p_Expression.placename = m_Id2Parametername.at(p_Expression.placeid).mb_str();
		}
		break;
	default:
		children_t::iterator it = p_Expression.children.begin();
		for(; it != p_Expression.children.end(); ++it)
		{
			SetNames(*it);
		}
	    break;
	}
}

void SP_DS_ApParser::LoadUnfoldedPlaces()
{
	m_Placename2Id = *( m_pcUnfoldedNet->GetPlaceArrayPos() );
	m_Id2Placename = *( m_pcUnfoldedNet->GetPlaceNames() );
	
	SP_VectorString l_msColoredPlaceNames =  *(m_pcUnfoldedNet->GetColoredPlaceNames());	
	
	for(unsigned i=0; i < l_msColoredPlaceNames.size(); i++)
	{
		m_Id2Placename.push_back( l_msColoredPlaceNames[i] );
		m_Placename2Id[l_msColoredPlaceNames[i]] = m_Id2Placename.size()-1;
	}

	client::places.clear();
	for(unsigned l_nPos = 0; l_nPos < m_Id2Placename.size(); l_nPos++)
	{
		client::places.add(m_Id2Placename[l_nPos].mb_str(), l_nPos);
	}	

}
