// $Id: driver.cc 39 2008-08-03 10:07:15Z tb $
/** \file driver.cc Implementation of the SP_CPN::Driver class. */

#include <fstream>
#include <sstream>

#include "SP_CPN_Driver.h"
#include "SP_CPN_Scanner.h"



# include "position.hh"

namespace SP_CPN {

SP_CPN_Driver::SP_CPN_Driver(class SP_CPN_Parse_Context& _calc)
    : trace_scanning(false),
      trace_parsing(false),
      calc(_calc)
{
}

bool SP_CPN_Driver::parse_stream(std::istream& in, const std::string& sname)
{
    streamname = sname;

    SP_CPN_Scanner scanner(&in);
    scanner.set_debug(trace_scanning);
    this->lexer = &scanner;

    SP_CPN_Parser parser(*this);
    //parser.set_debug_level(trace_parsing);  /*10.08.2011*/
    return (parser.parse() == 0);
}

bool SP_CPN_Driver::parse_file(const std::string &filename)
{
    std::ifstream in(filename.c_str());
    if (!in.good()) return false;
    return parse_stream(in, filename);
}

bool SP_CPN_Driver::parse_string(const std::string &input, const std::string& sname)
{
    std::istringstream iss(input);
    return parse_stream(iss, sname);
}

void SP_CPN_Driver::error(const class location& l,
		   const std::string& m)
{
	
    //std::cerr << l << ": " << m << std::endl;

	wxString l_sError;

	position last = l.end - 1;
	l_sError << wxT("Line ") << l.begin.line << wxT(" Column ") << l.begin.column;

	if (l.begin.line != last.line)
	   l_sError << wxT('-') << wxT("Line ") << last.line  << wxT(" Column ") << last.column;
	else if (l.begin.column != last.column)
	   l_sError << wxT('-') << wxT(" Column ") << last.column;

	l_sError << wxT(": ") << wxString( m.c_str(), wxConvUTF8);

	m_sError = wxT("");
	m_sError = l_sError;

	//SP_LOGERROR(l_sError);
	
}

void SP_CPN_Driver::error(const std::string& m)
{
    std::cerr << m << std::endl;
}

} // namespace SP_CPN

SP_CPN_Driver_ptr
make_CPN_Driver(class SP_CPN_Parse_Context& calc)
{
	return SP_CPN_Driver_ptr(new SP_CPN::SP_CPN_Driver(calc));
}
