//
//  RenderTupel.h
//  
//
//  Created by Toobee on 12.06.12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#ifndef _RENDER_TUPEL_H
#define _RENDER_TUPEL_H

#include "Liver.h"
#include "RelativeDistance.h"
#include "vtkTable.h"
#include <vtkTransformPolyDataFilter.h>
#include <vtkTransform.h>

class RenderTupel {
    public:
        RenderTupel(Liver*, Liver*, RelativeDistance*, RelativeDistance *);
        ~RenderTupel();
        Liver * getExpertLiver();
        Liver * getSegmentationLiver();
		Liver * getSegmentationLiverPCA2D();
		Liver * getSegmentationLiverPCA3D();
        RelativeDistance * getDistanceExpert2Seg();
        RelativeDistance * getDistanceSeg2Expert();
		double labelPosition[3];
		void movePCA3D(int pcaScale);
		void movePCA2D(int pcaScale);
		double pcaX;
		double pcaY;
		double pcaZ;
		
    private:
        Liver * expert;
        Liver * segmentation;
		Liver * segmentationPCA2D;
		Liver * segmentationPCA3D;
        RelativeDistance * disExpertToSeg;
        RelativeDistance * disSegToExpert;
        
};

#endif
