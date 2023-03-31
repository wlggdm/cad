#ifndef _ACGS_RECORDER_IMPL_H_
#define _ACGS_RECORDER_IMPL_H_

#include "AcGsDisplay.h"
#include "GeExtents3d.h"
#include <vector>

class AcGsRecorderImpl : public AcGsRecorder {
public:
  AcGsRecorderImpl();
  ~AcGsRecorderImpl() override;

  void display(AcGsDisplayContext&) override;

  bool extents(OdGeExtents3d& extents) const override;

  virtual bool hasExtent() const { return true; }

  void addRecorder(AcGsRecorderImpl* recorder);

public:
  std::vector<std::unique_ptr<AcGsRecorderImpl>> recorders_;

  OdGeExtents3d extent_;

  AcGsRecorderImpl* parent;
};

using AcGsRecorderPtr = std::unique_ptr<AcGsRecorderImpl>;

#endif //_ACGS_RECORDER_IMPL_H_