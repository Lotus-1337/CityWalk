#pragma once

namespace FTimers
{

	/**
	* @param Seconds time converted to milliseconds ( multiplied by 10 ^ 3 )
	* @returns Seconds -> Milliseconds
	*/
	inline double MilliSeconds(const double& Seconds)
	{
		return Seconds * 1e3;
	}

	/**
	* @param Seconds time converted to microseconds ( multiplied by 10 ^ 6 )
	* @returns Seconds -> Microseconds
	*/
	inline double MicroSeconds(const double& Seconds)
	{
		return Seconds * 1e6;
	}


};