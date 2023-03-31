#ifndef _ACGS_TRIANGLE_RECORDER_H_
#define _ACGS_TRIANGLE_RECORDER_H_

#include "AcGsRecorderImpl.h"

class AcGsTriangleRecorder : public AcGsRecorderImpl {
public:
  AcGsTriangleRecorder();
  ~AcGsTriangleRecorder() override;

  void display(AcGsDisplayContext&) override;

  void setVertexs(OdUInt32 count, const OdGePoint3d* vertexs);
  void setVertexs(const OdGePoint3d&, const OdGePoint3d&, const OdGePoint3d&);

private:
  OdGePoint3dArray points_;
};

#endif //_ACGS_TRIANGLE_RECORDER_H_