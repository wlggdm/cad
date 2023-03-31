#ifndef _ACGS_POLYLINE_RECORDER_H_
#define _ACGS_POLYLINE_RECORDER_H_

#include "AcGsRecorderImpl.h"
#include "Int32Array.h"

class AcGsPolylineRecorder : public AcGsRecorderImpl {
public:
  AcGsPolylineRecorder();
  ~AcGsPolylineRecorder() override;

  void display(AcGsDisplayContext&) override;

  void setVertexs(OdUInt32 count, const OdGePoint3d* vertexList);

private:
  OdGePoint3dArray  vertexs_;

  OdInt32Array      index_;
};

#endif //_ACGS_POLYLINE_RECORDER_H_