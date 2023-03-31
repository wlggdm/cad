#ifndef _ACGS_ENTITY_RECORDER_H_
#define _ACGS_ENTITY_RECORDER_H_

#include "AcGsRecorderImpl.h"

class AcGsEntityRecorder : public AcGsRecorderImpl {
public:
  AcGsEntityRecorder(bool flag = false);
  ~AcGsEntityRecorder() override;

  void display(AcGsDisplayContext&) override;

private:
  bool flag_;
};

using AcGsEntityRecorderPtr = std::unique_ptr<AcGsEntityRecorder>;

#endif //_ACGS_ENTITY_RECORDER_H_