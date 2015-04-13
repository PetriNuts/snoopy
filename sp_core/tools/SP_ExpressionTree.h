/*
 * SP_ExpressionTree.h
 *
 *  Created on: 03.05.2012
 *      Author: Steffen Laarz
 *
 *
 *  Datenstruktur zum speichern von arithmetischen Ausdruecken
 */

#ifndef SP_EXPRESSIONTREE_H_
#define SP_EXPRESSIONTREE_H_

#include <iostream>
#include <string>
#include <vector>

namespace exprTr
{
    ///////////////////////////////////////////////////////////////////////////
    //  Our ExpressionTree representation
    ///////////////////////////////////////////////////////////////////////////

	enum TType
	{
    	     DEFAULT = -1,
		DOUBLEVAL,
		IDENT,
		PLACE,
		PARAMETER,
		ADD,
		MUL,
		DIV,
		MINUS,
		MOD,
		POW
	};

	struct ExpressionTree;

	typedef std::vector<ExpressionTree> children_t;

	struct ExpressionTree
	{
		children_t children;
    	TType token;
    	double value;
    	std::string placename;
    	unsigned placeid;

    	ExpressionTree():
		token(DEFAULT),
		value(0),
		placename(""),
		placeid(-1)
		{}

    	ExpressionTree(const ExpressionTree& p_Copy):
		children(p_Copy.children),
		token(p_Copy.token),
		value(p_Copy.value),
		placename(p_Copy.placename),
		placeid(p_Copy.placeid)
		{}

    	ExpressionTree& operator=(const ExpressionTree& p_RHS)
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

		~ExpressionTree()
		{
			children.clear();
		}

		explicit ExpressionTree(TType p_Type, const ExpressionTree& p_Child1):
		token(p_Type),
		value(0),
		placename(""),
		placeid(-1)
		{
			children.push_back(p_Child1);
		}

		explicit ExpressionTree(TType p_Type, const ExpressionTree& p_Child1, const ExpressionTree& p_Child2):
		token(p_Type),
		value(0),
		placename(""),
		placeid(-1)
		{
			children.push_back(p_Child1);
			children.push_back(p_Child2);
		}

		explicit ExpressionTree(TType p_Type, const ExpressionTree& p_Child1, const ExpressionTree& p_Child2, const ExpressionTree& p_Child3):
		token(p_Type),
		value(0),
		placename(""),
		placeid(-1)
		{
			children.push_back(p_Child1);
			children.push_back(p_Child2);
			children.push_back(p_Child3);
		}

		explicit ExpressionTree(double p_Value):
		token(DOUBLEVAL),
		value(p_Value),
		placename(""),
		placeid(-1)
		{}

		explicit ExpressionTree(TType p_Type, unsigned p_Placeid):
		token(p_Type),
		value(0),
		placename(""),
		placeid(p_Placeid)
		{}

		explicit ExpressionTree(TType p_Type, std::string p_Placename):
		token(IDENT),
		value(0),
		placename(p_Placename),
		placeid(-1)
		{}

		explicit ExpressionTree(std::string p_Placename):
		token(IDENT),
		value(0),
		placename(p_Placename),
		placeid(-1)
		{}
/*
		explicit ExpressionTree(const char* p_Placename):
		token(IDENT),
		value(0),
		placename(p_Placename),
		placeid(-1)
		{}
*/

		ExpressionTree& operator+=(const ExpressionTree& p_RHS)
		{
			ExpressionTree l_LHS = *this;
			*this = p_RHS;
			children.insert(children.begin(), l_LHS);

			return *this;
		}

		std::string toString() const;

	};

	typedef std::vector<ExpressionTree> vectorExpressionTree;
}



#endif /* SP_EXPRESSIONTREE_H_ */
