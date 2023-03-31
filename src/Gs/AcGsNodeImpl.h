#ifndef _ACGS_NODE_IMPL_H_
#define _ACGS_NODE_IMPL_H_

#include "acgs.h"

class AcGsNodeImpl : public AcGsNode {
public:
  enum NodeType {
    kEntity,
    kContainer,
    kLayer,
  };

  AcGsNodeImpl(NodeType type);
  virtual ~AcGsNodeImpl() {}

  AcGiDrawable * drawable(void) const override;
  void setDrawable(AcGiDrawable*);

  NodeType type() const;
  
protected:
  AcDbObjectId  object_id_;

  NodeType      type_;
};

class AcGsDisplayContext;
class AcGiVectorizer;

class AcGsDisplay {
public:
  virtual void display(AcGsDisplayContext& context) {}
  virtual void update(AcGiVectorizer& vectorizer) {}
};

#endif //_ACGS_NODE_IMPL_H_