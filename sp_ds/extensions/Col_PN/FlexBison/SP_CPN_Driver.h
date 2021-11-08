// $Id: driver.h 17 2007-08-19 18:51:39Z tb $ 	
/** \file driver.h Declaration of the SP_CPN::Driver class. */

#ifndef SP_CPN_DRIVER_H
#define SP_CPN_DRIVER_H

#include <string>
#include <vector>

#include "sp_core/SP_Core.h"

// forward declaration
class SP_CPN_Parse_Context;

/** The SP_CPN namespace is used to encapsulate the three parser classes
 * SP_CPN::Parser, SP_CPN::Scanner and SP_CPN::SP_CPN_Driver */
namespace SP_CPN {

/** The Driver class brings together all components. It creates an instance of
 * the Parser and Scanner classes and connects them. Then the input stream is
 * fed into the scanner object and the parser gets it's token
 * sequence. Furthermore the driver object is available in the grammar rules as
 * a parameter. Therefore the driver class contains a reference to the
 * structure into which the parsed data is saved. */
class SP_CPN_Driver
{
public:
    /// construct a new parser driver context
    SP_CPN_Driver(class SP_CPN_Parse_Context& calc);

    /// enable debug output in the flex scanner
    bool trace_scanning;

	wxString m_sError;

    /// enable debug output in the bison parser
    bool trace_parsing;

    /// stream name (file or input stream) used for error messages.
    std::string streamname;

    /** Invoke the scanner and parser for a stream.
     * @param in	input stream
     * @param sname	stream name for error messages
     * @return		true if successfully parsed
     */
    bool parse_stream(std::istream& in,
		      const std::string& sname = "stream input");

    /** Invoke the scanner and parser on an input string.
     * @param input	input string
     * @param sname	stream name for error messages
     * @return		true if successfully parsed
     */
    bool parse_string(const std::string& input,
		      const std::string& sname = "string stream");

    /** Invoke the scanner and parser on a file. Use parse_stream with a
     * std::ifstream if detection of file reading errors is required.
     * @param filename	input file name
     * @return		true if successfully parsed
     */
    bool parse_file(const std::string& filename);

    // To demonstrate pure handling of parse errors, instead of
    // simply dumping them on the standard error output, we will pass
    // them to the driver using the following two member functions.

    /** Error handling with associated line number. This can be modified to
     * output the error e.g. to a dialog box. */
    void error(const class location& l, const std::string& m);

    /** General error handling. This can be modified to output the error
     * e.g. to a dialog box. */
    void error(const std::string& m);

	wxString GetError(){return m_sError;}

    /** Pointer to the current lexer instance, this is used to connect the
     * parser to the scanner. It is used in the yylex macro. */
    class SP_CPN_Scanner* lexer;

    /** Reference to the calculator context filled during parsing of the
     * expressions. */
    class SP_CPN_Parse_Context& calc;
};

} // namespace SP_CPN

typedef std::shared_ptr<SP_CPN::SP_CPN_Driver> SP_CPN_Driver_ptr;

SP_CPN_Driver_ptr make_CPN_Driver(class SP_CPN_Parse_Context& calc);

#endif // SP_CPN_DRIVER_H
