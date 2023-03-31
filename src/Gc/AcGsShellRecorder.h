#ifndef _ACGS_SHELL_RECORDER_H_
#define _ACGS_SHELL_RECORDER_H_

#include "AcGsRecorderImpl.h"

#include "Int32Array.h"


class AcGsShellRecorder : public AcGsRecorderImpl {
public:
  AcGsShellRecorder();
  ~AcGsShellRecorder() override;

  void display(AcGsDisplayContext&) override;

  void addVertexs(OdUInt32 count, const OdGePoint3d* vertexs);

  void polyline(OdUInt32 count, const OdInt32* index);

  void triangle(OdUInt32 count, const OdInt32* index);

private:
  OdGePoint3dArray points_;

  OdInt32Array     triangles_;

  OdInt32Array     polylines_;

  OdInt32          index_;
};

#endif //_ACGS_SHELL_RECORDER_H_