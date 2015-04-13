/*
 * SP_ExpressionTree.cpp
 *
 *  Created on: 03.05.2012
 *      Author: Steffen Laarz
 */


#include "SP_ExpressionTree.h"
#include "sstream"

namespace exprTr
{

std::string
ExpressionTree::toString() const
{
	std::string l_Return;
	std::stringstream l_Stream;
	//l_Stream << "(" << token << ")";
	switch(token)
	{
		case DOUBLEVAL:
			l_Stream << value;
			break;
		case IDENT:
			l_Stream << placename;
			break;
		case PLACE:
			l_Stream << placename;
			break;
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
		case MOD:
			l_Stream << "(" << children.at(0).toString() << "%" << children.at(1).toString() << ")";
			break;
		case POW:
			l_Stream << "(" << children.at(0).toString() << "^" << children.at(1).toString() << ")";
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

}
