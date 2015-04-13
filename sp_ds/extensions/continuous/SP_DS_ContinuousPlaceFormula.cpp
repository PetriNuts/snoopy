/*
 * SP_DS_ContinuousPlaceFormula.cpp
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 4.6.2010
 * Short Description:
 */
 //======================================================================================
#include "sp_gui/windows/SP_GUI_Mainframe.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_core/SP_Core.h"

#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/extensions/continuous/SP_DS_ContinuousPlaceFormula.h"

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include<boost/spirit/home/classic/core/parser.hpp>

/*
 * Begin IL formula grammar definition
 */
using namespace rfdag;


namespace clientContinuous
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
                ("MassAction", MA)
            ;
        }

    } numunop;

    struct numbinop_ : qi::symbols<char, TType>
    {
    	numbinop_()
        {
            add
                ("pow"   , POW)
                ("MichaelisMenten", MME)
                ("Inhibit",INHIBIT)
			    ("Read",READ)
			    ("Equal",EQUAL)
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
                ( "^" , POW)
            ;
        }

    } add;

    struct pow_ : qi::symbols<char, TType>
	{
    	pow_()
		{
			add
				( "^" , POW)
			;
		}

	} pow;

    struct UneryMinus_ : qi::symbols<char, TType>
        {
    	   UneryMinus_()
            {
                add
                    ("-"    , UMinus)
                ;
            }

        } UneryMinus;

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

    struct bracketop_ : qi::symbols<char, TType>
        {
    	  bracketop_()
            {
                add
                    ("("    ,BRACKET)
                ;
            }

        } bracketop;

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
		: qi::grammar<Iterator,RFDAG(), qi::locals<std::string>, ascii::space_type>
    {
    	  il_formula_grammar(std::ostream& err)
         // : il_formula_grammar::base_type(START, "START")
          : il_formula_grammar::base_type(expr, "expr")
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

            term 	= term1[_val = _1] >> *(mul > term1)[_val += construct<RFDAG>(_1, _2)]

            		;
            term1 = factor[_val = _1] >> *(pow > factor)[_val += construct<RFDAG>(_1, _2)]

                    ;
            factor 	= (bracketop > expr > lit(')'))[_val =construct<RFDAG>(_1,_2)]
            		| (numbinop > lit('(') > expr > lit(',') > expr > lit(')'))[_val = construct<RFDAG>(_1, _2, _3)]
            		| (numunop > lit('(') > expr > lit(')'))[_val = construct<RFDAG>(_1, _2)]
            		| placename[_val = _1]
            		| num[_val = _1]
            		| (UneryMinus >lit('(') > expr > lit(')'))[_val = construct<RFDAG>(_1,_2)]//-(exp)
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
            term1.name("term1");
            factor.name("factor");
            num.name("num");
            placename.name("placename");

            on_error<fail>
            (
            	expr
				 ,
				   err
                     <<val("Error! Expecting ")
                     <<_4                               // what failed?
                     <<val(" here: \"")
                     <<construct<std::string>(_3, _2)   // iterators to error-pos, end
                     <<val("\"")
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
          qi::rule<Iterator, RFDAG(), qi::locals<std::string>, ascii::space_type> term1;
          qi::rule<Iterator, RFDAG(), qi::locals<std::string>, ascii::space_type> num;
          qi::rule<Iterator, RFDAG(), qi::locals<std::string>, ascii::space_type> placename;

    };

}

/*
 * End IL formula grammar definition
 */

bool
SP_DS_ContinuousPlaceFormula::Initialise()
{
	LoadPlaces();
	LoadParameters();

	return true;
}

void
SP_DS_ContinuousPlaceFormula::LoadPlaces()
{


	clientContinuous::places.clear();
	m_Placename2Id.clear();
	unsigned l_nPos = 0;

	LoadPlacesOfType(SP_DS_CONTINUOUS_PLACE,l_nPos);
	LoadPlacesOfType(SP_DS_DISCRETE_PLACE,l_nPos);


}
void SP_DS_ContinuousPlaceFormula::LoadPlacesOfType(const wxString& p_sPlaceType, unsigned& p_nPos)
{

	SP_DS_Nodeclass* l_pcPlacesNode = m_pcGraph->GetNodeclass( p_sPlaceType );
	SP_ListNode::const_iterator l_itElem;

	//load Places of such type
	if(l_pcPlacesNode!=NULL)
	{
		m_Id2Placename.resize(p_nPos+l_pcPlacesNode->GetElements()->size());
		for(l_itElem = l_pcPlacesNode->GetElements()->begin();l_itElem != l_pcPlacesNode->GetElements()->end();
			++l_itElem)
		{
			wxString l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>((*l_itElem)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
			m_Placename2Id[l_sPlaceName] = p_nPos;
			m_Id2Placename[p_nPos] = l_sPlaceName;

			clientContinuous::places.add(l_sPlaceName.mb_str(), p_nPos);

			++p_nPos;
		}
	}
}
//for colored PN
void SP_DS_ContinuousPlaceFormula::ClearclientContinuousPlaces()
{
	clientContinuous::places.clear();
}

//for colored PN
void SP_DS_ContinuousPlaceFormula::AddclientContinuousPlaces(const wxCharBuffer p_sPlaceName, unsigned p_nPos)
{
	clientContinuous::places.add(p_sPlaceName, p_nPos);
}


void
SP_DS_ContinuousPlaceFormula::LoadParameters()
{
	SP_DS_Metadataclass* l_pcMC = m_pcGraph->GetMetadataclass( SP_DS_META_CONSTANT );

	clientContinuous::parameters.clear();
	m_Parametername2Id.clear();
	m_Parameter.clear();
	m_Id2Parametername.resize(l_pcMC->GetElements()->size());
	m_Parameter.resize(l_pcMC->GetElements()->size());
	unsigned l_nPos = 0;

	SP_ListMetadata::const_iterator l_itElem;
	for(l_itElem = l_pcMC->GetElements()->begin();
		l_itElem != l_pcMC->GetElements()->end();
		++l_itElem )
	{
		wxString l_sParameterName = dynamic_cast<SP_DS_NameAttribute*>((*l_itElem)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
		m_Parametername2Id[l_sParameterName] = l_nPos;
		m_Id2Parametername[l_nPos] = l_sParameterName;

		clientContinuous::parameters.add(l_sParameterName.mb_str(), l_nPos);

		SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >( (*l_itElem)->GetAttribute( wxT("ValueList") ) );
		double l_nDouble = l_pcColList->GetActiveCellValueDouble(1);
		m_Parameter[l_nPos]=l_nDouble;

		++l_nPos;
	}

}

bool
SP_DS_ContinuousPlaceFormula::Parse()
{

	std::ostringstream oss;

#if wxABI_VERSION > 30000
    typedef clientContinuous::il_formula_grammar<std::string::const_iterator> il_formula_grammar;
    il_formula_grammar l_Formula(oss); // Our grammar
    std::string l_FormulaString = m_sFormula;
    std::string::const_iterator l_Iter = l_FormulaString.begin();
    std::string::const_iterator l_End = l_FormulaString.end();
#else
    typedef clientContinuous::il_formula_grammar<wxString::const_iterator> il_formula_grammar;
    il_formula_grammar l_Formula(oss); // Our grammar

    wxString::const_iterator l_Iter = m_sFormula.begin();
    wxString::const_iterator l_End = m_sFormula.end();
#endif

    using boost::spirit::ascii::space;
	bool l_Return=phrase_parse(l_Iter, l_End, l_Formula, space, *m_Expression);

	if(l_Iter!=l_End)
		l_Return=false;

	if(l_Return)
	{

	   SetNames(*m_Expression);
	}
	else
	{
		wxString l_sMsg = wxT("Parsing formula: ");
		l_sMsg << m_sFormula;
		SP_LOGERROR( l_sMsg);
		wxString l_sErrDetails;

		//convert from std::string to wxString
		 const char* my_string=oss.str().c_str();
		 wxMBConvUTF8 *wxconv= new wxMBConvUTF8();
		 l_sErrDetails=wxString(wxconv->cMB2WC(my_string),wxConvUTF8);
		 delete wxconv;
		 // test if conversion works of not. In case it fails convert fromAscii
		 if(l_sErrDetails.length()==0)
			 l_sErrDetails=wxString(wxString::FromAscii(oss.str().c_str()));

		 //show the error message
		SP_LOGERROR( l_sErrDetails);
	}
	return l_Return;
}

void
SP_DS_ContinuousPlaceFormula::SetNames(RFDAG& p_Expression)
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
	}
}

double SP_DS_ContinuousPlaceFormula::Evaluate(const SP_VectorDouble *p_CurrentMarking)
{
	m_CurrentMarking = p_CurrentMarking;
	return Eval(*m_Expression);
}
SP_DS_ContinuousPlaceFormula::SP_DS_ContinuousPlaceFormula(const wxString& l_sFormula,const wxString & p_sPlaceType)
 :m_sFormula(l_sFormula),m_sPlaceType(p_sPlaceType)
{
   m_pcGraph=SP_Core::Instance()->GetRootDocument()->GetGraph();
   m_Expression=new RFDAG();

   //initialization
   Initialise();
}
SP_DS_ContinuousPlaceFormula::~SP_DS_ContinuousPlaceFormula()
{
   wxDELETE(m_Expression);
}


double SP_DS_ContinuousPlaceFormula::EvaluateA(RFDAG* p_Expression)
{
	return EvalA(*p_Expression);
}

double SP_DS_ContinuousPlaceFormula::EvalA(const RFDAG& p_Expression)
{
	double l_nReturn;
		switch(p_Expression.token)
		{
		case rfdag::DOUBLE:
			l_nReturn = p_Expression.value;
			break;
		case rfdag::IDENT:
		case rfdag::PLACE:
			l_nReturn =m_pcCurrentMarking[p_Expression.placeid];
			break;
		case rfdag::PARAMETER:
			l_nReturn = m_Parameter[p_Expression.placeid];
			break;
		case rfdag::ADD:
			l_nReturn = EvalA(p_Expression.children[0]) + EvalA(p_Expression.children[1]);
			break;
		case rfdag::MUL:
			l_nReturn = EvalA(p_Expression.children[0]) * EvalA(p_Expression.children[1]);
			break;
		case rfdag::DIV:
			l_nReturn = EvalA(p_Expression.children[0]) / EvalA(p_Expression.children[1]);
			break;
		case rfdag::MINUS:
			l_nReturn = EvalA(p_Expression.children[0]) - EvalA(p_Expression.children[1]);
			break;
		case rfdag::UMinus:
				l_nReturn = - EvalA(p_Expression.children[0]);
				break;
		case rfdag::POW:
			l_nReturn = pow(EvalA(p_Expression.children[0]), EvalA(p_Expression.children[1]));
			break;
		case rfdag::SQR:
			l_nReturn = pow(EvalA(p_Expression.children[0]), 2.0);
			break;
		case rfdag::SQRT:
			l_nReturn = sqrt(EvalA(p_Expression.children[0]));
			break;
		case rfdag::SIN:
			l_nReturn = sin(EvalA(p_Expression.children[0]));
			break;
		case rfdag::ASIN:
			l_nReturn = asin(EvalA(p_Expression.children[0]));
			break;
		case rfdag::ABS:
			l_nReturn = fabs(EvalA(p_Expression.children[0]));
			break;
		case rfdag::CEIL:
			l_nReturn = ceil(EvalA(p_Expression.children[0]));
			break;
		case rfdag::FLOOR:
			l_nReturn = floor(EvalA(p_Expression.children[0]));
			break;
		case rfdag::COS:
			l_nReturn = cos(EvalA(p_Expression.children[0]));
			break;
		case rfdag::ACOS:
			l_nReturn = acos(EvalA(p_Expression.children[0]));
			break;
		case rfdag::TAN:
			l_nReturn = tan(EvalA(p_Expression.children[0]));
			break;
		case rfdag::ATAN:
			l_nReturn = atan(EvalA(p_Expression.children[0]));
			break;
		case rfdag::LOG:
			l_nReturn = log(EvalA(p_Expression.children[0]));
			break;
		case rfdag::LOG10:
			l_nReturn = log10(EvalA(p_Expression.children[0]));
			break;
		case rfdag::EQ:
			l_nReturn = EvalA(p_Expression.children[0]) == EvalA(p_Expression.children[1]);
			break;
		case rfdag::NEQ:
			l_nReturn = EvalA(p_Expression.children[0]) != EvalA(p_Expression.children[1]);
			break;
		case rfdag::LEQ:
			l_nReturn = EvalA(p_Expression.children[0]) <= EvalA(p_Expression.children[1]);
			break;
		case rfdag::GEQ:
			l_nReturn = EvalA(p_Expression.children[0]) >= EvalA(p_Expression.children[1]);
			break;
		case rfdag::LESS:
			l_nReturn = EvalA(p_Expression.children[0]) < EvalA(p_Expression.children[1]);
			break;
		case rfdag::GREAT:
			l_nReturn = EvalA(p_Expression.children[0]) > EvalA(p_Expression.children[1]);
			break;
		case rfdag::NOT:
			l_nReturn = !EvalA(p_Expression.children[0]);
			break;
		case rfdag::AND:
			l_nReturn = EvalA(p_Expression.children[0]) && EvalA(p_Expression.children[1]);
			break;
		case rfdag::OR:
			l_nReturn = EvalA(p_Expression.children[0]) || EvalA(p_Expression.children[1]);
			break;
		case rfdag::XOR:
			l_nReturn = (EvalA(p_Expression.children[0]) && !EvalA(p_Expression.children[1]))
					|| (!EvalA(p_Expression.children[0]) && EvalA(p_Expression.children[1]));
			break;
		case rfdag::IF:
			l_nReturn = !EvalA(p_Expression.children[0]) || EvalA(p_Expression.children[1]);
			break;
		case rfdag::IFB:
			l_nReturn = EvalA(p_Expression.children[0]) || !EvalA(p_Expression.children[1]);
			break;
		case rfdag::IFF:
			l_nReturn = (EvalA(p_Expression.children[0]) && EvalA(p_Expression.children[1]))
					|| (!EvalA(p_Expression.children[0]) && !EvalA(p_Expression.children[1]));
			break;
		case rfdag::BRACKET:
				l_nReturn = EvalA(p_Expression.children[0]);
				break;
		case rfdag::INHIBIT://             weight                   Marking
				l_nReturn = EvalA(p_Expression.children[0])>EvalA(p_Expression.children[1])? 1.0:0.0;
				break;
		case rfdag::READ:
				l_nReturn = EvalA(p_Expression.children[0])>EvalA(p_Expression.children[1])? 0.0:1.0;
				break;
	    case rfdag::EQUAL:
	    	    //We need to find a way to make a continuous comparison
				l_nReturn =  EvalA(p_Expression.children[0])==EvalA(p_Expression.children[1])? 1.0:0.0;
				break;

		default:
			SP_LOGERROR( wxT("Error while evaluating ") + wxString(p_Expression.toString().c_str(), wxConvUTF8));
			l_nReturn = 0;
			break;
		}
		return l_nReturn;

}
 double
SP_DS_ContinuousPlaceFormula::Eval(const RFDAG& p_Expression)
{
	double l_nReturn;
	switch(p_Expression.token)
	{
	case rfdag::DOUBLE:
		l_nReturn = p_Expression.value;
		break;
	case rfdag::IDENT:
	case rfdag::PLACE:
		l_nReturn = (*m_CurrentMarking)[p_Expression.placeid];
		break;
	case rfdag::PARAMETER:
		l_nReturn = m_Parameter[p_Expression.placeid];
		break;
	case rfdag::ADD:
		l_nReturn = Eval(p_Expression.children[0]) + Eval(p_Expression.children[1]);
		break;
	case rfdag::MUL:
		l_nReturn = Eval(p_Expression.children[0]) * Eval(p_Expression.children[1]);
		break;
	case rfdag::DIV:
		l_nReturn = Eval(p_Expression.children[0]) / Eval(p_Expression.children[1]);
		break;
	case rfdag::MINUS:
		l_nReturn = Eval(p_Expression.children[0]) - Eval(p_Expression.children[1]);
		break;
	case rfdag::UMinus:
			l_nReturn = - Eval(p_Expression.children[0]);
			break;
	case rfdag::POW:
		l_nReturn = pow(Eval(p_Expression.children[0]), Eval(p_Expression.children[1]));
		break;
	case rfdag::SQR:
		l_nReturn = pow(Eval(p_Expression.children[0]), 2.0);
		break;
	case rfdag::SQRT:
		l_nReturn = sqrt(Eval(p_Expression.children[0]));
		break;
	case rfdag::SIN:
		l_nReturn = sin(Eval(p_Expression.children[0]));
		break;
	case rfdag::ASIN:
		l_nReturn = asin(Eval(p_Expression.children[0]));
		break;
	case rfdag::ABS:
		l_nReturn = fabs(Eval(p_Expression.children[0]));
		break;
	case rfdag::CEIL:
		l_nReturn = ceil(Eval(p_Expression.children[0]));
		break;
	case rfdag::FLOOR:
		l_nReturn = floor(Eval(p_Expression.children[0]));
		break;
	case rfdag::COS:
		l_nReturn = cos(Eval(p_Expression.children[0]));
		break;
	case rfdag::ACOS:
		l_nReturn = acos(Eval(p_Expression.children[0]));
		break;
	case rfdag::TAN:
		l_nReturn = tan(Eval(p_Expression.children[0]));
		break;
	case rfdag::ATAN:
		l_nReturn = atan(Eval(p_Expression.children[0]));
		break;
	case rfdag::LOG:
		l_nReturn = log(Eval(p_Expression.children[0]));
		break;
	case rfdag::LOG10:
		l_nReturn = log10(Eval(p_Expression.children[0]));
		break;
	case rfdag::EQ:
		l_nReturn = Eval(p_Expression.children[0]) == Eval(p_Expression.children[1]);
		break;
	case rfdag::NEQ:
		l_nReturn = Eval(p_Expression.children[0]) != Eval(p_Expression.children[1]);
		break;
	case rfdag::LEQ:
		l_nReturn = Eval(p_Expression.children[0]) <= Eval(p_Expression.children[1]);
		break;
	case rfdag::GEQ:
		l_nReturn = Eval(p_Expression.children[0]) >= Eval(p_Expression.children[1]);
		break;
	case rfdag::LESS:
		l_nReturn = Eval(p_Expression.children[0]) < Eval(p_Expression.children[1]);
		break;
	case rfdag::GREAT:
		l_nReturn = Eval(p_Expression.children[0]) > Eval(p_Expression.children[1]);
		break;
	case rfdag::NOT:
		l_nReturn = !Eval(p_Expression.children[0]);
		break;
	case rfdag::AND:
		l_nReturn = Eval(p_Expression.children[0]) && Eval(p_Expression.children[1]);
		break;
	case rfdag::OR:
		l_nReturn = Eval(p_Expression.children[0]) || Eval(p_Expression.children[1]);
		break;
	case rfdag::XOR:
		l_nReturn = (Eval(p_Expression.children[0]) && !Eval(p_Expression.children[1]))
				|| (!Eval(p_Expression.children[0]) && Eval(p_Expression.children[1]));
		break;
	case rfdag::IF:
		l_nReturn = !Eval(p_Expression.children[0]) || Eval(p_Expression.children[1]);
		break;
	case rfdag::IFB:
		l_nReturn = Eval(p_Expression.children[0]) || !Eval(p_Expression.children[1]);
		break;
	case rfdag::IFF:
		l_nReturn = (Eval(p_Expression.children[0]) && Eval(p_Expression.children[1]))
				|| (!Eval(p_Expression.children[0]) && !Eval(p_Expression.children[1]));
		break;
	case rfdag::BRACKET:
			l_nReturn = Eval(p_Expression.children[0]);
			break;
	case rfdag::INHIBIT://             weight                   Marking
			l_nReturn = Eval(p_Expression.children[0])>Eval(p_Expression.children[1])? 1.0:0.0;
			break;
	case rfdag::READ:
			l_nReturn = Eval(p_Expression.children[0])>Eval(p_Expression.children[1])? 0.0:1.0;
			break;
    case rfdag::EQUAL:
    	    //We need to find a way to make a continuous comparison
			l_nReturn =  Eval(p_Expression.children[0])==Eval(p_Expression.children[1])? 1.0:0.0;
			break;

	default:
		SP_LOGERROR( wxT("Error while evaluating ") + wxString(p_Expression.toString().c_str(), wxConvUTF8));
		l_nReturn = 0;
		break;
	}
	return l_nReturn;
}


 wxString SP_DS_ContinuousPlaceFormula::EliminatePattern(const RFDAG& p_Expression,const bool& p_bUseModelicaNaming)
 {
   wxString l_nReturn;
   wxString l_sPrePlacesProduct=wxT("");
   unsigned long l_nPrePlace;
	 	switch(p_Expression.token)
	 	{
	 	case rfdag::DOUBLE:
	 		l_nReturn = wxString::Format(wxT("%g"),p_Expression.value);
	 		break;
	 	case rfdag::IDENT:
	 	case rfdag::PLACE:
	 		l_nReturn = p_bUseModelicaNaming==false? m_Id2Placename[p_Expression.placeid] :(m_Id2Placename[p_Expression.placeid]+wxT(".t")) ;
	 		break;
	 	case rfdag::PARAMETER:
	 		l_nReturn = m_Id2Parametername[p_Expression.placeid];
	 		break;
	 	case rfdag::ADD:
	 		l_nReturn =EliminatePattern(p_Expression.children[0],p_bUseModelicaNaming) +wxT("+")+ EliminatePattern(p_Expression.children[1],p_bUseModelicaNaming);
	 		break;
	 	case rfdag::MUL:
	 		l_nReturn = EliminatePattern(p_Expression.children[0],p_bUseModelicaNaming) +wxT("*")+ EliminatePattern(p_Expression.children[1],p_bUseModelicaNaming);
	 		break;
	 	case rfdag::DIV:
	 		l_nReturn = EliminatePattern(p_Expression.children[0],p_bUseModelicaNaming) +wxT("/")+ EliminatePattern(p_Expression.children[1],p_bUseModelicaNaming);
	 		break;
	 	case rfdag::MINUS:
	 		l_nReturn = EliminatePattern(p_Expression.children[0],p_bUseModelicaNaming) +wxT("-")+ EliminatePattern(p_Expression.children[1],p_bUseModelicaNaming);
	 		break;
	 	case rfdag::UMinus:
	 		l_nReturn = wxT("-(")+EliminatePattern(p_Expression.children[0],p_bUseModelicaNaming)+wxT(")");
	 		break;
	 	case rfdag::POW:
	 		l_nReturn =EliminatePattern(p_Expression.children[0],p_bUseModelicaNaming)+ wxT("^") + EliminatePattern(p_Expression.children[1],p_bUseModelicaNaming);
	 		break;
	 	case rfdag::SQR:
	 		l_nReturn = wxT("sqr(")+ EliminatePattern(p_Expression.children[0],p_bUseModelicaNaming) + wxT(")");
	 		break;
	 	case rfdag::SQRT:
	 		l_nReturn = wxT("sqr(")+EliminatePattern(p_Expression.children[0],p_bUseModelicaNaming) + wxT(")");
	 		break;
	 	case rfdag::MA:
	 		//Here the transformation of MassAction will take place
	 		if(m_vsTransitionPrePlaces.size()>0)
	 		{
	 			l_sPrePlacesProduct=m_mPlaceTransitionMultiplicity[m_vsTransitionPrePlaces[0]]==wxT("1") ? m_vsTransitionPrePlaces[0]:

	 			                         m_vsTransitionPrePlaces[0]+
	 			                                                  wxT("^")+
	 			                                                           m_mPlaceTransitionMultiplicity[m_vsTransitionPrePlaces[0]];

	 		for(l_nPrePlace=1;l_nPrePlace<m_vsTransitionPrePlaces.size();l_nPrePlace++)
	 			l_sPrePlacesProduct+=m_mPlaceTransitionMultiplicity[m_vsTransitionPrePlaces[l_nPrePlace]]==wxT("1") ? wxT("*")+m_vsTransitionPrePlaces[l_nPrePlace]:

	 				 			             wxT("*")+
	 				 			                         m_vsTransitionPrePlaces[l_nPrePlace]+
	 				 			                                                  wxT("^")+
	 				 			                                                           m_mPlaceTransitionMultiplicity[m_vsTransitionPrePlaces[l_nPrePlace]];

	 		l_nReturn+=EliminatePattern(p_Expression.children[0],p_bUseModelicaNaming)+wxT("*")+l_sPrePlacesProduct;
	 		}
	 		else
	 			l_nReturn+=EliminatePattern(p_Expression.children[0],p_bUseModelicaNaming);
	 		break;
	 	case rfdag::MME:
	 		//Here the transformation of Miachaels Mentan Equation will take place
	 		if(m_vsTransitionPrePlaces.size()>0)
	 		{
	 			 			l_sPrePlacesProduct=m_mPlaceTransitionMultiplicity[m_vsTransitionPrePlaces[0]]==wxT("1") ? m_vsTransitionPrePlaces[0]:

	 			 			                         m_vsTransitionPrePlaces[0]+
	 			 			                                                  wxT("^")+
	 			 			                                                           m_mPlaceTransitionMultiplicity[m_vsTransitionPrePlaces[0]];

	 			 		for(l_nPrePlace=1;l_nPrePlace<m_vsTransitionPrePlaces.size();l_nPrePlace++)
	 			 			l_sPrePlacesProduct+=m_mPlaceTransitionMultiplicity[m_vsTransitionPrePlaces[l_nPrePlace]]==wxT("1") ? wxT("*")+m_vsTransitionPrePlaces[l_nPrePlace]:

	 			 				 			              wxT("*")+
	 			 				 			                         m_vsTransitionPrePlaces[l_nPrePlace]+
	 			 				 			                                                  wxT("^")+
	 			 				 			                                                           m_mPlaceTransitionMultiplicity[m_vsTransitionPrePlaces[l_nPrePlace]];

	 		 l_nReturn+=wxT("(")+EliminatePattern(p_Expression.children[0],p_bUseModelicaNaming)+wxT("*")+l_sPrePlacesProduct+wxT(")")+wxT("/")+wxT("(")+l_sPrePlacesProduct+wxT("+")+EliminatePattern(p_Expression.children[1],p_bUseModelicaNaming)+wxT(")");
	 		}
	 		else
	 			l_nReturn+=EliminatePattern(p_Expression.children[0],p_bUseModelicaNaming)+wxT("/")+EliminatePattern(p_Expression.children[1],p_bUseModelicaNaming);
	 		break;
	 	case rfdag::SIN:
	 		l_nReturn = wxT("sin(")+EliminatePattern(p_Expression.children[0],p_bUseModelicaNaming)+wxT(")");
	 		break;
	 	case rfdag::ASIN:
	 		l_nReturn = wxT("asin(")+EliminatePattern(p_Expression.children[0],p_bUseModelicaNaming)+wxT(")");
	 		break;
	 	case rfdag::ABS:
	 		l_nReturn = wxT("fabs(")+EliminatePattern(p_Expression.children[0],p_bUseModelicaNaming)+wxT(")");
	 		break;
	 	case rfdag::CEIL:
	 		l_nReturn = wxT("ceil(")+EliminatePattern(p_Expression.children[0],p_bUseModelicaNaming)+wxT(")");
	 		break;
	 	case rfdag::FLOOR:
	 		l_nReturn = wxT("floor(")+EliminatePattern(p_Expression.children[0],p_bUseModelicaNaming)+wxT(")");
	 		break;
	 	case rfdag::COS:
	 		l_nReturn = wxT("cos(")+EliminatePattern(p_Expression.children[0],p_bUseModelicaNaming)+wxT(")");
	 		break;
	 	case rfdag::ACOS:
	 		l_nReturn = wxT("acos(")+EliminatePattern(p_Expression.children[0],p_bUseModelicaNaming)+wxT(")");
	 		break;
	 	case rfdag::TAN:
	 		l_nReturn = wxT("tan(")+EliminatePattern(p_Expression.children[0],p_bUseModelicaNaming)+wxT(")");
	 		break;
	 	case rfdag::ATAN:
	 		l_nReturn = wxT("atan(")+EliminatePattern(p_Expression.children[0],p_bUseModelicaNaming)+wxT(")");
	 		break;
	 	case rfdag::LOG:
	 		l_nReturn = wxT("log(")+EliminatePattern(p_Expression.children[0],p_bUseModelicaNaming)+wxT(")");
	 		break;
	 	case rfdag::LOG10:
	 		l_nReturn = wxT("log10(")+EliminatePattern(p_Expression.children[0],p_bUseModelicaNaming)+wxT(")");
	 		break;
	 	case rfdag::BRACKET:
	 		 l_nReturn = wxT("(")+EliminatePattern(p_Expression.children[0],p_bUseModelicaNaming)+wxT(")");
	 		  break;
	 	/*case rfdag::EQ:
	 		l_nReturn = EliminatePattern(p_Expression.children[0]) == EliminatePattern(p_Expression.children[1]);
	 		break;
	 	case rfdag::NEQ:
	 		l_nReturn = EliminatePattern(p_Expression.children[0]) != EliminatePattern(p_Expression.children[1]);
	 		break;
	 	case rfdag::LEQ:
	 		l_nReturn = EliminatePattern(p_Expression.children[0]) <= EliminatePattern(p_Expression.children[1]);
	 		break;
	 	case rfdag::GEQ:
	 		l_nReturn = EliminatePattern(p_Expression.children[0]) >= EliminatePattern(p_Expression.children[1]);
	 		break;
	 	case rfdag::LESS:
	 		l_nReturn = EliminatePattern(p_Expression.children[0]) < EliminatePattern(p_Expression.children[1]);
	 		break;
	 	case rfdag::GREAT:
	 		l_nReturn = EliminatePattern(p_Expression.children[0]) > EliminatePattern(p_Expression.children[1]);
	 		break;
	 	case rfdag::NOT:
	 		l_nReturn = !EliminatePattern(p_Expression.children[0]);
	 		break;
	 	case rfdag::AND:
	 		l_nReturn = EliminatePattern(p_Expression.children[0]) && EliminatePattern(p_Expression.children[1]);
	 		break;
	 	case rfdag::OR:
	 		l_nReturn = EliminatePattern(p_Expression.children[0]) || EliminatePattern(p_Expression.children[1]);
	 		break;
	 	case rfdag::XOR:
	 		l_nReturn = (EliminatePattern(p_Expression.children[0]) && !EliminatePattern(p_Expression.children[1]))
	 				|| (!EliminatePattern(p_Expression.children[0]) && EliminatePattern(p_Expression.children[1]));
	 		break;
	 	case rfdag::IF:
	 		l_nReturn = !EliminatePattern(p_Expression.children[0]) || EliminatePattern(p_Expression.children[1]);
	 		break;
	 	case rfdag::IFB:
	 		l_nReturn = EliminatePattern(p_Expression.children[0]) || !EliminatePattern(p_Expression.children[1]);
	 		break;
	 	case rfdag::IFF:
	 		l_nReturn = (EliminatePattern(p_Expression.children[0]) && EliminatePattern(p_Expression.children[1]))
	 				|| (!EliminatePattern(p_Expression.children[0]) && !EliminatePattern(p_Expression.children[1]));
	 		break;*/
	 	default:
	 		SP_LOGERROR( wxT("Transforming the formula") + wxString(p_Expression.toString().c_str(), wxConvUTF8));
	 		l_nReturn = wxT("0");
	 		break;
	 	}
	 	return l_nReturn;
 }

 wxString SP_DS_ContinuousPlaceFormula::ReplacePatternInTransitionFormula(SP_DS_Node * p_pcTransNode,const bool& p_bUseModelicaNaming)
 {


	 //Get the transition's Pre Places

	 m_vsTransitionPrePlaces.clear();
	 SP_ListEdge::const_iterator l_itEdge;
	 for (l_itEdge = p_pcTransNode->GetTargetEdges()->begin(); l_itEdge!= p_pcTransNode->GetTargetEdges()->end(); l_itEdge++)
	 	{
	 	   SP_DS_Node* l_pcPlace= dynamic_cast<SP_DS_Node*>((*l_itEdge)->GetSource());
	 	    wxString l_sEdgeName=(*l_itEdge)->GetClassName();
	 		if((l_sEdgeName==SP_DS_EDGE||l_sEdgeName==SP_DS_READ_EDGE) && l_pcPlace)
	 		 {
	 			wxString l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>((l_pcPlace)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
	 			l_sPlaceName=p_bUseModelicaNaming==false? l_sPlaceName: l_sPlaceName+wxT(".t");
	 			m_vsTransitionPrePlaces.push_back(l_sPlaceName) ;

	 			//Store the multiplicity of the edge connecting this place with the current transition
	 			SP_DS_Attribute* l_pcMultiplicity=((*l_itEdge)->GetAttribute(wxT("Multiplicity")));

	 			if(l_pcMultiplicity)
	 				m_mPlaceTransitionMultiplicity[l_sPlaceName]=l_pcMultiplicity->GetValueString();
	 			else
	 			 //for edges that does not contain equation attribute like inhibitors
	 			 m_mPlaceTransitionMultiplicity[l_sPlaceName]=wxT("1");
	 		 }
	 	}

	 if(Parse())
	 	  return (EliminatePattern(*m_Expression,p_bUseModelicaNaming));

	 else //error during parsing
	 {
		 SP_LOGERROR( wxT("Error in the transition's Formula: ") +  dynamic_cast<SP_DS_NameAttribute*>((p_pcTransNode)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue());
		 return(wxT("0"));
	 }
 }


 bool
SP_DS_ContinuousPlaceFormula::ParseforColCPN()
{

	std::ostringstream oss;

#if wxABI_VERSION > 30000
    typedef clientContinuous::il_formula_grammar<std::string::const_iterator> il_formula_grammar;
    il_formula_grammar l_Formula(oss); // Our grammar
    std::string l_FormulaString = m_sFormula;
    std::string::const_iterator l_Iter = l_FormulaString.begin();
    std::string::const_iterator l_End = l_FormulaString.end();
#else
    typedef clientContinuous::il_formula_grammar<wxString::const_iterator> il_formula_grammar;
    il_formula_grammar l_Formula(oss); // Our grammar

    wxString::const_iterator l_Iter = m_sFormula.begin();
    wxString::const_iterator l_End = m_sFormula.end();
#endif

    using boost::spirit::ascii::space;
	bool l_Return=phrase_parse(l_Iter, l_End, l_Formula, space, *m_Expression);

	if(l_Iter!=l_End)
		l_Return=false;

	if(l_Return)
	{

	   //SetNames(m_Expression);
	}
	else
	{
		wxString l_sMsg = wxT("Parsing formula: ");
		l_sMsg << m_sFormula;
		SP_LOGERROR( l_sMsg);
		wxString l_sErrDetails;

		//convert from std::string to wxString
		 const char* my_string=oss.str().c_str();
		 wxMBConvUTF8 *wxconv= new wxMBConvUTF8();
		 l_sErrDetails=wxString(wxconv->cMB2WC(my_string),wxConvUTF8);
		 delete wxconv;
		 // test if conversion works of not. In case it fails convert fromAscii
		 if(l_sErrDetails.length()==0)
			 l_sErrDetails=wxString(wxString::FromAscii(oss.str().c_str()));

		 //show the error message
		SP_LOGERROR( l_sErrDetails);
	}

	return l_Return;
 }

 
