#ifndef _ACGS_ENTITY_NODE_H_
#define _ACGS_ENTITY_NODE_H_

#include "AcGsNodeImpl.h"
#include "base/containers/linked_list.h"

#include "oda/Include/si/SiSpatialIndex.h"
#include "AcGsEntityRecorder.h"

class AcGsEntityNode : public AcGsNodeImpl, 
                       public AcGsDisplay,
                       public OdSiEntity,
                       public base::LinkNode<AcGsEntityNode>{
public:
  AcGsEntityNode();
  ~AcGsEntityNode() override;

  void display(AcGsDisplayContext& context) override;
  void update(AcGiVectorizer& vectorizer) override;

  void setExtents(const OdGeExtents3d& extents);
  bool extents(OdGeExtents3d& extents) const override;

  void markToSkip(bool skip) { skiped_ = skip; }
  bool markedToSkip() const { return skiped_;  }

  void setRecorder(int view_id, AcGsRecorderImpl* recorder);
private:
  OdGeExtents3d extents_;

  std::vector<AcGsRecorderPtr> recorders_;

  bool skiped_;
};


#endif //_ACGS_ENTITY_NODE_H_