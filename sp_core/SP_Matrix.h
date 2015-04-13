//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: cr $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2009/03/09 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_MATRIX_H__
#define __SP_MATRIX_H__

#include "sp_core/SP_Map.h"

template <typename T> class SP_Matrix
{
private:

	typedef map<size_t, T> SP_MatrixColType;
	typedef map<size_t, SP_MatrixColType > SP_MatrixType;

	size_t m_nNumRows;
	size_t m_nNumCols;
	T m_DefaultValue;
	SP_MatrixType m_mMatrix;

	//no default c'tor
	SP_Matrix(){}

protected:

public:
	/**
	 * c'tor
	 *
	 * @param p_nNumRows
	 * @param p_nNumCols
	 * @param p_DefaultValue
	 */
	SP_Matrix(size_t p_nNumRows, size_t p_nNumCols, const T& p_DefaultValue):
		m_nNumRows(p_nNumRows), m_nNumCols(p_nNumCols), m_DefaultValue(p_DefaultValue) {}
	/**
	 * copy c'tor
	 * @param p_Copy
	 * @return
	 */
	SP_Matrix(const SP_Matrix& p_Copy):
		m_nNumRows(p_Copy.GetNumRows()), m_nNumCols(p_Copy.GetNumCols()),
		m_DefaultValue(p_Copy.GetDefaultValue()), m_mMatrix(p_Copy.GetData()) {}
	/**
	 *
	 * @param
	 * @return
	 */
	SP_Matrix& operator= (const SP_Matrix& p_Matrix)
	{
		if( m_nNumRows == p_Matrix.GetNumRows() &&
			m_nNumCols == p_Matrix.GetNumCols() &&
			m_DefaultValue == p_Matrix.GetDefaultValue())
		{
			m_mMatrix = p_Matrix.GetData();
		}
		return *this;
	}
	/**
	 * d'tor
	 */
	virtual ~SP_Matrix() {}
	/**
	 *
	 * @return matrix
	 */
	const SP_MatrixType& GetData() const { return m_mMatrix; }
	/**
	 *
	 * @return number of rows
	 */
	size_t GetNumRows() const { return m_nNumRows; }
	/**
	 *
	 * @return number of cols
	 */
	size_t GetNumCols() const { return m_nNumCols; }
	/**
	 *
	 * @param number of rows
	 */
	void SetNumRows(size_t p_nNumRows)
	{
		if(p_nNumRows < m_nNumRows)
		{
			typename SP_MatrixType::iterator l_itRow = m_mMatrix.upper_bound(p_nNumRows-1);
			if(l_itRow != m_mMatrix.end())
			{
				m_mMatrix.erase(l_itRow,m_mMatrix.end());
			}
		}
		m_nNumRows = p_nNumRows;
	}
	/**
	 *
	 * @param number of cols
	 */
	void SetNumCols(size_t p_nNumCols)
	{
		if(p_nNumCols < m_nNumCols)
		{
			typename SP_MatrixType::iterator l_itRow = m_mMatrix.begin();
			for(l_itRow; l_itRow != m_mMatrix.end(); l_itRow++)
			{
				typename SP_MatrixColType::iterator l_itCol = (*l_itRow).second.upper_bound(p_nNumCols-1);
				if(l_itCol != (*l_itRow).second.end())
				{
					(*l_itRow).second.erase(l_itCol,(*l_itRow).second.end());
				}
			}

		}
		m_nNumCols = p_nNumCols;
	}
	/**
	 *
	 * @return default value
	 */
	const T& GetDefaultValue() const { return m_DefaultValue; }
	/**
	 *
	 * @param p_nNumRows
	 * @param p_nNumCols
	 * @return
	 */
	const T& GetValue(size_t p_nRow, size_t p_nCol) const
	{
		typename SP_MatrixType::const_iterator l_itRow = m_mMatrix.find(p_nRow);
		if(l_itRow != m_mMatrix.end())
		{
			typename SP_MatrixColType::const_iterator l_itCol = (*l_itRow).second.find(p_nCol);
			if(l_itCol != (*l_itRow).second.end())
			{
				return (*l_itCol).second;
			}
		}
		return m_DefaultValue;
	}
	/**
	 *
	 * @param p_nNumRows
	 * @param p_nNumCols
	 * @param p_Value
	 * @return
	 */
	bool SetValue(size_t p_nRow, size_t p_nCol, const T& p_Value)
	{
		if(p_nRow < m_nNumRows && p_nCol < m_nNumCols)
		{
			if(m_DefaultValue == p_Value)
			{
				typename SP_MatrixType::iterator l_itRow = m_mMatrix.find(p_nRow);
				if(l_itRow != m_mMatrix.end())
				{
					typename SP_MatrixColType::iterator l_itCol = (*l_itRow).second.find(p_nCol);
					if(l_itCol != (*l_itRow).second.end())
					{
						(*l_itRow).second.erase(l_itCol);
					}
					if((*l_itRow).second.empty())
					{
						m_mMatrix.erase(l_itRow);
					}
				}
			}
			else
			{
				m_mMatrix[p_nRow][p_nCol] = p_Value;
			}
			return true;
		}
		return false;
	}

	void Clear() { m_mMatrix.clear(); }

};

typedef SP_Matrix<double> SP_MatrixDouble;
typedef SP_Matrix<long> SP_MatrixLong;
typedef SP_Matrix<unsigned long> SP_MatrixULong;
typedef SP_Matrix<bool> SP_MatrixBool;

#endif // __SP_MATRIX_H__
