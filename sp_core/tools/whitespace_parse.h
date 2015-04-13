/*
 * whitespace_parse.h
 *
 *  Created on: Aug 03, 2011
 *      Author: crohr
 */

#ifndef WHITESPACE_PARSER_H_
#define WHITESPACE_PARSER_H_

#include <wx/string.h>

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_function.hpp>
#include <boost/spirit/include/phoenix_container.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/std_pair.hpp>
#include <boost/spirit/include/support_utree.hpp>
#include <boost/type_traits.hpp>

	namespace phoenix = boost::phoenix;
	namespace qi = boost::spirit::qi;
	namespace ascii = boost::spirit::ascii;

	///////////////////////////////////////////////////////////////////////////////
    //  whitespace grammar definition
    ///////////////////////////////////////////////////////////////////////////////
    template <typename Iterator>
    struct comment : qi::grammar<Iterator>
    {
        comment() : comment::base_type(start)
        {
            using boost::spirit::ascii::char_;
            using boost::spirit::qi::eol;

            start =
                    "/*" >> *(char_ - "*/") >> "*/"     // C-style comments
                |   "//" >> *(char_ - eol) >> eol       // C++ style comment
                ;
        }

        qi::rule<Iterator> start;
    };

    template <typename Iterator>
    struct white_space : qi::grammar<Iterator>
    {
        white_space() : white_space::base_type(start)
        {
            using boost::spirit::ascii::space;

            start =
                    space                               // tab/space/cr/lf
                |   comment_
                ;
        }

        qi::rule<Iterator> start;
        comment<Iterator> comment_;
    };


namespace snoopy
{
    ///////////////////////////////////////////////////////////////////////////
	// simple utree printing facility prints the utree in a single line
	///////////////////////////////////////////////////////////////////////////

	std::ostream& print(std::ostream& out, boost::spirit::utree const& val);
	std::ostream& println(std::ostream& out, boost::spirit::utree const& val);

	// simple_print visitor
	struct simple_print
	{
		typedef void result_type;

		std::ostream& out;
		simple_print(std::ostream& out) : out(out) {}

        void operator()(boost::spirit::utree::invalid_type) const
        {
            out << "<invalid>";
        }

        void operator()(boost::spirit::utree::nil_type) const
        {
            out << "<nil>";
        }

        template <typename T>
        void operator()(T val) const
        {
            out << val;
        }

        void operator()(bool b) const
        {
            out << (b ? "true" : "false");
        }

        void operator()(boost::spirit::binary_range_type const& b) const
        {
            boost::io::ios_all_saver saver(out);
            out << "#";
            out.width(2);
            out.fill('0');

            typedef boost::spirit::binary_range_type::const_iterator iterator;
            for (iterator i = b.begin(); i != b.end(); ++i)
                out << std::hex << int((unsigned char)*i);
            out << "#";
        }

        void operator()(boost::spirit::utf8_string_range_type const& str) const
        {
            typedef boost::spirit::utf8_string_range_type::const_iterator iterator;
            iterator i = str.begin();
//            out << '"';
            for (; i != str.end(); ++i)
                out << *i;
//            out << "\" ";
        }

        void operator()(boost::spirit::utf8_symbol_range_type const& str) const
        {
            typedef boost::spirit::utf8_symbol_range_type::const_iterator iterator;
            iterator i = str.begin();
            for (; i != str.end(); ++i)
                out << *i;
//           out << ' ';
        }

        template <typename Iterator>
        void operator()(boost::iterator_range<Iterator> const& range) const
        {
            typedef typename boost::iterator_range<Iterator>::const_iterator iterator;
//            (*this)('(');
            for (iterator i = range.begin(); i != range.end(); ++i)
            {
                boost::spirit::utree::visit(*i, *this);
            }
//            (*this)(')');
        }

        void operator()(boost::spirit::any_ptr const&) const
        {
            return (*this)("<pointer>");
        }

        void operator()(boost::spirit::function_base const&) const
        {
            return (*this)("<function>");
        }

	};

	inline std::ostream& print(std::ostream& out, boost::spirit::utree const& val)
	{
		boost::spirit::utree::visit(val, snoopy::simple_print(out));
		return out;
	}

	inline std::ostream& println(std::ostream& out, boost::spirit::utree const& val)
	{
		snoopy::print(out, val) << std::endl;
		return out;
	}

}
    inline wxString utree2wxString(const boost::spirit::utree& tree)
    {
    	std::stringstream s;
    	snoopy::print(s, tree);
    	return wxString(s.str().c_str(), wxConvUTF8).Trim();
    }


#endif
