#ifndef _ACGS_TEXT_RECORDER_H_
#define _ACGS_TEXT_RECORDER_H_

#include "AcGsRecorderImpl.h"
#include "AcGsShellRecorder.h"

class AcGsTextRecorder : public AcGsRecorderImpl {
public:
  AcGsTextRecorder();
  ~AcGsTextRecorder() override;

  void display(AcGsDisplayContext&) override;

  void setHeightVector(const AcGeVector3d&);

  void setShxFont(bool);

  bool extents(OdGeExtents3d& extents) const override;

  AcGsRecorderImpl*  deGenerate1() { return &degenerate1_; }
  AcGsRecorderImpl*  deGenerate5() { return &degenerate5_; }
  AcGsShellRecorder* generate() { return &generate_; }

private:
  AcGeVector3d       vheight_;
  AcGsRecorderImpl   degenerate1_;
  AcGsRecorderImpl   degenerate5_;
  AcGsShellRecorder  generate_;

  bool               shx_font_;
};

#endif //_ACGS_TEXT_RECORDER_H_