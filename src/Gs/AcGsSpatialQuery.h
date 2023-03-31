#ifndef _ACGS_SPATIAL_QUERY_H_
#define _ACGS_SPATIAL_QUERY_H_

#include "AcGsViewImpl.h"
#include "si/SiSpatialIndex.h"
#include "si/BBox.h"
#include "Gs/SiVolume.h"

class AcGsSpatialQuery {
public:
  AcGsSpatialQuery();

  const OdSiShape& build(const AcGsViewImpl& view,
                         const AcGeExtents3d& sceneExtents,
                         int   numPoints, 
                         const AcGePoint2d* points,
                         double x = 0.5,
                         double y = 0.5);

  const OdSiShape& build(const AcGsViewImpl& view,
                         const AcGeExtents3d& sceneExtents,
                         int   numPoints = 0, 
                         const AcGsDCPoint* points = 0);

private:
  OdSi::BBox  box_shape_;

  OdSi::Volume volume_shape_;
};

#endif //_ACGS_SPATIAL_QUERY_H_