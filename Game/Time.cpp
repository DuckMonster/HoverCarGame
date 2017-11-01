#include "stdafx.h"
#include "Time.h"

using namespace std;
using namespace std::chrono;

CTimePoint CTime::m_Start;

//--------------------------------------------------- TIME POINT

/**	Constructor
*******************************************************************************/
CTimePoint::CTimePoint( ) :
	m_TimePoint( clock::now( ) )
{
}

/**	Copy Constructor
*******************************************************************************/
CTimePoint::CTimePoint( const CTimePoint && other ) :
	m_TimePoint( other.m_TimePoint )
{
}

/**	Elapsed
*******************************************************************************/
float CTimePoint::Elapsed( )
{
	timepoint nowTime = clock::now( );
	return (float)(duration_cast<microseconds>(nowTime - m_TimePoint).count( ) * 1.0e-6);
}

/**	Operator -
*******************************************************************************/
float CTimePoint::operator-( const CTimePoint & other )
{
	return (float)(duration_cast<microseconds>(m_TimePoint - other.m_TimePoint).count( ) * 1.0e-6);
}

/**	Operator =
*******************************************************************************/
void CTimePoint::operator=( const CTimePoint & other )
{
	m_TimePoint = other.m_TimePoint;
}