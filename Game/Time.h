#pragma once
class CTimePoint
{
	typedef std::chrono::high_resolution_clock::time_point timepoint;
	typedef std::chrono::high_resolution_clock clock;

public:
	CTimePoint( );
	CTimePoint( const CTimePoint&& other );

	float Elapsed( );
	float operator-( const CTimePoint& other );
	void operator=( const CTimePoint& other );

private:
	timepoint	m_TimePoint;
};

class CTime
{
public:


public:
	static float TotalElapsed( ) { return m_Start.Elapsed( ); }
	static void Reset( ) { m_Start = CTimePoint( ); }

private:
	static CTimePoint	m_Start;
};