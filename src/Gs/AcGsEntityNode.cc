#include "AcGsEntityNode.h"
#include "AcGiVectorizer.h"
#include "base/logging.h"

AcGsEntityNode::AcGsEntityNode() : AcGsNodeImpl(kEntity),
skiped_(true){
}

AcGsEntityNode::~AcGsEntityNode() {
  OdGiDrawablePtr drawable = object_id_.openObject().get();
  drawable->setGsNode(nullptr);
}

void AcGsEntityNode::display(AcGsDisplayContext& context) {
  const int view_id = context.viewId();
  DCHECK(recorders_.size() > view_id);

  AcGsRecorderPtr& recorder = recorders_[view_id];
  if (recorder)
    recorder->display(context);
}

void AcGsEntityNode::update(AcGiVectorizer& vectorizer) {
  OdGiDrawablePtr drawable = object_id_.openObject().get();
  AcGsEntityNode* node = reinterpret_cast<AcGsEntityNode*>(drawable->gsNode());

  AcGsEntityRecorder* recorder = vectorizer.internalDraw(drawable);

  node->setRecorder(vectorizer.viewId(), recorder);
}

void AcGsEntityNode::setExtents(const OdGeExtents3d& extents) {
  extents_ = extents;
}

bool AcGsEntityNode::extents(OdGeExtents3d& extents) const {
  if (!extents_.isValidExtents())
    return false;

  extents = extents_;
  return true;
}

void AcGsEntityNode::setRecorder(int view_id, AcGsRecorderImpl* recorder) {
  if (recorders_.size() <= view_id)
    recorders_.resize(view_id + 1);

  recorders_[view_id].reset(recorder);
}
