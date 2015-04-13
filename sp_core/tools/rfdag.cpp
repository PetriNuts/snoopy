/*
 * rfdag.cpp
 *
 *  Created on: Mar 20, 2010
 *      Author: crohr
 */
#include "rfdag.h"
#include "sstream"

std::string
rfdag::RFDAG::toString() const
{
	std::string l_Return;
	std::stringstream l_Stream;
	//l_Stream << "(" << token << ")";
	switch(token)
	{
		case DOUBLE:
			l_Stream << value;
			break;
		case IDENT:
		case PLACE:
		case PARAMETER:
			l_Stream << placename;
			break;
		case ADD:
			l_Stream << "(" << children.at(0).toString() << "+" << children.at(1).toString() << ")";
			break;
		case MUL:
			l_Stream << "(" << children.at(0).toString() << "*" << children.at(1).toString() << ")";
			break;
		case DIV:
			l_Stream << "(" << children.at(0).toString() << "/" << children.at(1).toString() << ")";
			break;
		case MINUS:
			l_Stream << "(" << children.at(0).toString() << "-" << children.at(1).toString() << ")";
			break;
		case MA:
		case BIOMASS:
			l_Stream << "MassAction(" << children.at(0).toString() << ")";
			break;
		case BIOLEVEL:
			l_Stream << "LevelInterpretation(" << children.at(0).toString() << "," << children.at(1).toString() << ")";
			break;
		case IMMEDIATE:
			l_Stream << "ImmediateFiring(" << children.at(0).toString() << ")";
			break;
		case POW:
			l_Stream << "pow(" << children.at(0).toString() << "," << children.at(1).toString() << ")";
			break;
		case MIN:
			l_Stream << "min(" << children.at(0).toString() << "," << children.at(1).toString() << ")";
			break;
		case MAX:
			l_Stream << "max(" << children.at(0).toString() << "," << children.at(1).toString() << ")";
			break;
		case SQR:
			l_Stream << "sqr(" << children.at(0).toString() << ")";
			break;
		case SQRT:
			l_Stream << "sqrt(" << children.at(0).toString() << ")";
			break;
		case SIN:
			l_Stream << "sin(" << children.at(0).toString() << ")";
			break;
		case ASIN:
			l_Stream << "asin(" << children.at(0).toString() << ")";
			break;
		case ABS:
			l_Stream << "abs(" << children.at(0).toString() << ")";
			break;
		case CEIL:
			l_Stream << "ceil(" << children.at(0).toString() << ")";
			break;
		case FLOOR:
			l_Stream << "floor(" << children.at(0).toString() << ")";
			break;
		case COS:
			l_Stream << "cos(" << children.at(0).toString() << ")";
			break;
		case ACOS:
			l_Stream << "acos(" << children.at(0).toString() << ")";
			break;
		case TAN:
			l_Stream << "tan(" << children.at(0).toString() << ")";
			break;
		case ATAN:
			l_Stream << "atan(" << children.at(0).toString() << ")";
			break;
		case LOG:
			l_Stream << "log(" << children.at(0).toString() << ")";
			break;
		case LOG10:
			l_Stream << "log10(" << children.at(0).toString() << ")";
			break;
		case EQ:
			l_Stream << "(" << children.at(0).toString() << "=" << children.at(1).toString() << ")";
			break;
		case NEQ:
			l_Stream << "(" << children.at(0).toString() << "!=" << children.at(1).toString() << ")";
			break;
		case LEQ:
			l_Stream << "(" << children.at(0).toString() << "<=" << children.at(1).toString() << ")";
			break;
		case GEQ:
			l_Stream << "(" << children.at(0).toString() << ">=" << children.at(1).toString() << ")";
			break;
		case LESS:
			l_Stream << "(" << children.at(0).toString() << "<" << children.at(1).toString() << ")";
			break;
		case GREAT:
			l_Stream << "(" << children.at(0).toString() << ">" << children.at(1).toString() << ")";
			break;
		case NOT:
			l_Stream << "!" << children.at(0).toString();
			break;
		case AND:
			l_Stream << "(" << children.at(0).toString() << "&" << children.at(1).toString() << ")";
			break;
		case OR:
			l_Stream << "(" << children.at(0).toString() << "|" << children.at(1).toString() << ")";
			break;
		case XOR:
			break;
		case IF:
			l_Stream << "(" << children.at(0).toString() << "->" << children.at(1).toString() << ")";
			break;
		case IFB:
			l_Stream << "(" << children.at(0).toString() << "<-" << children.at(1).toString() << ")";
			break;
		case IFF:
			l_Stream << "(" << children.at(0).toString() << "<->" << children.at(1).toString() << ")";
			break;
		default:
			l_Stream << "error";
			for(children_t::const_iterator it = children.begin();
					it != children.end();
					++it)
			{
				l_Stream << it->toString();
			}
	}
	l_Return = l_Stream.str();
	return l_Return;
}
