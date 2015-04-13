//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: slehrack $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2007/06/04 16:35:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_COLLISTATTRIBUTE_H__
#define __SP_DS_COLLISTATTRIBUTE_H__

#include "sp_ds/SP_DS_Attribute.h"

typedef Sh_ptr<unsigned int> UIntPtr;

enum SP_COLLIST_TYPE{
	SP_COLLIST_BOOL,
	SP_COLLIST_INTEGER,
	SP_COLLIST_UNSIGNED_INTEGER,
	SP_COLLIST_DOUBLE,
	SP_COLLIST_CHAR,
	SP_COLLIST_STRING
};

class SP_DS_ColListAttribute: public SP_DS_Attribute
{
  private:
	unsigned int m_nRowCount;
	unsigned int m_nColCount;
	SP_COLLIST_TYPE m_eColListType;
	/**
	 * holds the current active list number,
	 * because of the shared_ptr every object that is created via Clone()
	 * shares the same value
	 */
	UIntPtr m_nActiveList;
	UIntPtr m_nActiveColumn;
	/** 
	 * name of the attribute responsible for grouping of elements
	 */
	wxString m_sGrouping;

	SP_MapUInt2DString m_mmValues;
    SP_MapString2UInt m_mColHead;

  protected:
	/**
	 *
	 * @param p_nCol
	 * @return
	 */
	inline bool CheckColIndex(unsigned int p_nCol) { return p_nCol < GetColCount(); }
	/**
	 *
	 * @param p_nRow
	 * @return
	 */
	inline bool CheckRowIndex(unsigned int p_nRow) { return p_nRow < GetRowCount(); }

  public:

    SP_DS_ColListAttribute( const wxString& p_pchName, SP_COLLIST_TYPE p_eColType, unsigned int p_nColCount, const wxString& p_sGrouping = wxT(""), UIntPtr p_nActiveList = UIntPtr(NULL),UIntPtr p_nActiveColumn = UIntPtr(NULL) );
    virtual ~SP_DS_ColListAttribute();
    /**
     *
     * @param p_bCloneGr
     * @return
     */
    virtual SP_DS_Attribute* Clone( bool p_bCloneGr = TRUE );
    /**
     *
     * @return
     */
    SP_COLLIST_TYPE GetColListType() { return m_eColListType; }
    /**
     *
     * @param p_nColNumber
     * @param p_sColLabel
     * @return
     */
	void RemoveExistingIndex(unsigned int p_nColNumber);
	wxString GetColLabel(unsigned int p_nColNumber);
    bool SetColLabel( unsigned int p_nColNumber, const wxString& p_sColLabel );
    /**
     *
     * @return
     */
    inline unsigned int GetRowCount() { return m_nRowCount; }
    /**
     *
     * @return
     */
    inline unsigned int GetColCount() { return m_nColCount; }
    /**
     *
     * @param p_nRowNumber
     * @return
     */
    bool RemoveRow( unsigned int p_nRowNumber );
    /*
     * remove remaining rows, starting from one row
     */
    bool RemoveRemainingRows(unsigned int p_nStartRowNumber);
    /**
     *
     * @param p_nRowNumber
     * @param p_nColNumber
     * @return
     */
    bool RemoveCell( unsigned int p_nRowNumber, unsigned int p_nColNumber );
    /**
     *
     * @param p_nRowNumber
     * @param p_sColKey
     * @return
     */
    bool RemoveCell( unsigned int p_nRowNumber, const wxString& p_sColKey );
    /**
     *
     * @param p_nRowNumber
     * @param p_nColNumber
     * @param p_nVal
     * @return
     */
    bool SetCell( unsigned int p_nRowNumber, unsigned int p_nColNumber, const wxString& p_nVal );
    /**
     *
     * @param p_nRowNumber
     * @param p_sColKey
     * @param p_nVal
     * @return
     */
    bool SetCell( unsigned int p_nRowNumber, const wxString& p_sColKey, const wxString& p_nVal );
    /**
     *
     * @param p_nRowNumber
     * @param p_nColNumber
     * @return
     */
    const wxString GetCell( unsigned int p_nRowNumber, unsigned int p_nColNumber );
    /**
     *
     * @param p_nRowNumber
     * @param p_sColKey
     * @return
     */
    const wxString GetCell( unsigned int p_nRowNumber, const wxString& p_sColKey );

    /**
     * @return index of appended row
     */
    unsigned int AppendEmptyRow();
	unsigned int AppendEmptyColum();
    /**
     *
     * @param p_sColKey
     * @return
     */
    unsigned int GetColNumberOfColLabel( const wxString& p_sColKey );
    /**
     *
     */
	void RemoveAllColumns();
    void Clear();
    /**
     *
     * @param p_nColNumber
     * @param p_sAttributeValue
     * @param p_nItemNumber
     * @return
     */
    bool RemoveRowByAttribute( unsigned int p_nColNumber, const wxString& p_sAttributeValue, unsigned int p_nItemNumber = -1 );
    /**
     *
     * @return
     */
    virtual wxString GetValueString();
    /**
     *
     * @param p_pchVal
     * @return
     */
    virtual bool SetValueString( const wxString& p_pchVal, bool p_bCheckValue = true );
    /**
     *
     * @param p_nActiveList
     */
    void SetActiveList(unsigned int p_nActiveList);
    /**
     *
     * @return
     */
    unsigned int GetActiveList();
    /*
     * Get active value using only column
     */
    const wxString GetActiveCellValue(unsigned int p_nColNumber );
    /*
     *return the current active value in double
     */
    long double GetActiveCellValueDouble(unsigned int p_nColNumber );
    /*
     * Set the active Cell Value (Double)
     */
    void SetActiveCellValueDouble(unsigned int p_nColNumber,long double p_nVal );


	void SetActiveColumn(unsigned int p_nActiveColumn);
	unsigned int GetActiveColumn();

	/**
	 *  return UIntPtr
	 */
	UIntPtr GetActiveListPtr() { return m_nActiveList; }
	UIntPtr GetActiveColumnPtr() { return m_nActiveColumn; }

	/*
	 * Updates the shared pointers m_nActiveList and m_nActiveColumn
	 * according to p_sAttribute
	 *
	 * @param p_sAttribute
	 */
	void UpdateActiveListColumnPtr();

	virtual bool CopyValueFrom(SP_DS_Attribute* p_pcAttribute);

};


#endif // __SP_DS_COLLISTATTRIBUTE_H__

