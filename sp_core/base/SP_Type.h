//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_TYPE_H__
#define __SP_TYPE_H__

// Element Types
enum SP_ELEMENT_TYPE
{
    SP_ELEMENT_NULL = 0,
    SP_ELEMENT_GRAPH = 1,
    SP_ELEMENT_NETCLASS = 2,
    SP_ELEMENT_NODECLASS = 4,
    SP_ELEMENT_EDGECLASS = 8,
    SP_ELEMENT_NODE = 16,
    SP_ELEMENT_EDGE = 32,
    SP_ELEMENT_ATTRIBUTE = 64,
    // different
    SP_ELEMENT_ANIMATOR = 128,
    SP_ELEMENT_NODEVECTOR = 256,
    // metadata
    SP_ELEMENT_METADATACLASS = 512,
    SP_ELEMENT_METADATA = 1024,
    // don't really take this as a type for concrete elements
    SP_ELEMENT_ALL = 4096
};

extern unsigned long g_nIdCounter;

class SP_IdCounter
{
private:
protected:
    unsigned long m_nId;
public:
    SP_IdCounter():m_nId(g_nIdCounter++) { }

    inline const unsigned long GetId() const { return m_nId; }
    inline void SetId(unsigned long p_nVal) { m_nId = p_nVal; if (p_nVal > g_nIdCounter) g_nIdCounter = p_nVal + 1; }
};

class SP_Type
{
private:
protected:
    SP_ELEMENT_TYPE m_eElementType;
public:
    SP_Type(SP_ELEMENT_TYPE p_eType):m_eElementType(p_eType) { }

    inline SP_ELEMENT_TYPE GetElementType() const { return m_eElementType; }
    //inline void SetElementType(SP_ELEMENT_TYPE p_eVal) { m_eElementType = p_eVal; }
};

#endif // __SP_TYPE_H__
