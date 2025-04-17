//DLL
#include <math.h>
#include <stdlib.h>
#include "DLL_Radar_Project.h"


RadarResult  CalcRadarPoint(int angle, int distance, int maxDistance,int canvasWidth, int canvasHeight, double scale)
{
    RadarResult resultDraw;
    //distance check range
    if (distance > maxDistance) 
		distance = maxDistance;
    if (distance < 0) 
		distance = 0;

    double radians = angle * 3.14 / 180.0; // convert angle deg to rad
    double scaledDistance = distance * scale; // scale the distance for display on the canvas

	 // calculate the center of the radar
    int centerX = canvasWidth / 2;
    int centerY = canvasHeight;

	// Calculate final point for the radar line
    resultDraw.x = centerX + (int)(scaledDistance * cos(radians));
    resultDraw.y = centerY - (int)(scaledDistance * sin(radians));
	
	// Calculate RGB color based on the scaled distance
    resultDraw.r = (int)(255 * (1 - scaledDistance / maxDistance)); // closer = redder
    resultDraw.g = (int)(255 * (scaledDistance / maxDistance));  // farther = greener
    resultDraw.b = 0;

    return resultDraw;
}

StatsData CalcStats(const double* values, int size) // Calculates the minimum, maximum, and average values from an array of doubles
{
	double sum = 0,val;
    StatsData resultCalc = {0}; // initialize result struct with zeroes

    if (values == NULL || size <= 0) // if input is invalid
        return resultCalc;
	// initialize min and max with the first value in the array
    resultCalc.min = values[0];
    resultCalc.max = values[0];
    

    for (int i = 0; i < size; ++i) // find min, max, and sum
    {
        val = values[i];
        if (val < resultCalc.min)
			resultCalc.min = val;
        if (val > resultCalc.max)
			resultCalc.max = val;
        sum += val;
    }

    resultCalc.avg = sum / size; // calc average
    return resultCalc;
}
