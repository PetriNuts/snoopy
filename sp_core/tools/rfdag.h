/*
 * rfdag.hpp
 *
 *  Created on: Mar 19, 2010
 *      Author: crohr
 */

#ifndef RFDAG_HPP_
#define RFDAG_HPP_

#include <iostream>
#include <string>
#include <vector>

namespace rfdag
{
    ///////////////////////////////////////////////////////////////////////////
    //  Our rfdag representation
    ///////////////////////////////////////////////////////////////////////////

	enum TType
	{
    	DEFAULT = -1,
		DOUBLE,
		IDENT,
		PLACE,
		PARAMETER,
		ADD,
		MUL,
		MA,
		MME,
		BRACKET,
		DIV,
		MINUS,
		UMinus,
		BIOMASS,
		BIOLEVEL,
		IMMEDIATE,
		POW,
		SQR,
		SQRT,
		SIN,
		ASIN,
		ABS,
		CEIL,
		FLOOR,
		COS,
		ACOS,
		TAN,
		ATAN,
		LOG,
		LOG10,
		EQ,
		NEQ,
		LEQ,
		GEQ,
		LESS,
		GREAT,
		NOT,
		AND,
		OR,
		XOR,
		IF,
		IFB,
		IFF,
		TIMED,
		DTFSINGLE,
		DTFPERIODIC,
		MIN,
		MAX,
		INHIBIT,
		READ,
		EQUAL
	};

	struct RFDAG;

	typedef std::vector<RFDAG> children_t;

	struct RFDAG
	{
		children_t children;
    	TType token;
    	double value;
    	std::string placename;
    	unsigned placeid;

		RFDAG():
		token(DEFAULT),
		value(0),
		placename(""),
		placeid(-1)
		{}

		RFDAG(const RFDAG& p_Copy):
		children(p_Copy.children),
		token(p_Copy.token),
		value(p_Copy.value),
		placename(p_Copy.placename),
		placeid(p_Copy.placeid)
		{}

		RFDAG& operator=(const RFDAG& p_RHS)
		{
			if(this != &p_RHS)
			{
				this->children = p_RHS.children;
				this->token = p_RHS.token;
				this->value = p_RHS.value;
				this->placename = p_RHS.placename;
				this->placeid = p_RHS.placeid;
			}
			return *this;
		}

		~RFDAG()
		{
			children.clear();
		}
/*
		explicit RFDAG(TType p_Type):
		token(p_Type),
		value(0),
		placename(""),
		placeid(-1)
		{}
*/
		explicit RFDAG(TType p_Type, const RFDAG& p_Child1):
		token(p_Type),
		value(0),
		placename(""),
		placeid(-1)
		{
			children.push_back(p_Child1);
		}

		explicit RFDAG(TType p_Type, const RFDAG& p_Child1, const RFDAG& p_Child2):
		token(p_Type),
		value(0),
		placename(""),
		placeid(-1)
		{
			children.push_back(p_Child1);
			children.push_back(p_Child2);
		}

		explicit RFDAG(TType p_Type, const RFDAG& p_Child1, const RFDAG& p_Child2, const RFDAG& p_Child3):
		token(p_Type),
		value(0),
		placename(""),
		placeid(-1)
		{
			children.push_back(p_Child1);
			children.push_back(p_Child2);
			children.push_back(p_Child3);
		}

		explicit RFDAG(double p_Value):
		token(DOUBLE),
		value(p_Value),
		placename(""),
		placeid(-1)
		{}

		explicit RFDAG(TType p_Type, unsigned p_Placeid):
		token(p_Type),
		value(0),
		placename(""),
		placeid(p_Placeid)
		{}

		explicit RFDAG(std::string p_Placename):
		token(IDENT),
		value(0),
		placename(p_Placename),
		placeid(-1)
		{}

		explicit RFDAG(const char* p_Placename):
		token(IDENT),
		value(0),
		placename(p_Placename),
		placeid(-1)
		{}


		RFDAG& operator+=(const RFDAG& p_RHS)
		{
			RFDAG l_LHS = *this;
			*this = p_RHS;
			children.insert(children.begin(), l_LHS);

			return *this;
		}

		std::string toString() const;

	};

	typedef std::vector<RFDAG> vectorRFDAG;
}


#endif /* RFDAG_HPP_ */
