// Vamco:
// Author: Dennis Basgier
// 20.01.2013
#ifndef _ID_DISTANCE_TUPEL_H
#define _ID_DISTANCE_TUPEL_H

/**
    This class stores the point id of a point in an other point set with the distance to this point. The 'from' point is not part of this class. This class is supposed to be used in a map or similar data as 'value' while the 'from' point serves as key.
 */
class IdDistanceTupel{
	
	public:
		IdDistanceTupel(int closestPointId, double distance);
		int getClosestPointId();
		double getDistance();
	private:
		int closestPointId;
		double distance;
};
#endif