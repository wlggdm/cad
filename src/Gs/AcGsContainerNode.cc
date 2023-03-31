#include "AcGsContainerNode.h"
#include "AcGsViewImpl.h"
#include "AcGiVectorizer.h"

AcGsContainerNode::AcGsContainerNode() : AcGsNodeImpl(kContainer){
#ifndef VP_DEPENDENT
  spatial_index_ = OdSiSpatialIndex::createObject(OdSiSpatialIndex::kSiNoFlags, 0, 30, 20);
#endif
}

AcGsContainerNode::~AcGsContainerNode() {
  base::LinkNode<AcGsEntityNode>* entity = entity_list_.head();
  base::LinkNode<AcGsEntityNode>* next = nullptr;

  for (; entity != entity_list_.end(); entity = next) {
    next = entity->next();
    delete entity->value();
  }
}

void AcGsContainerNode::add(AcGiDrawable* drawable) {
  if (drawable == nullptr)
    return;

  AcGsEntityNode* node = reinterpret_cast<AcGsEntityNode*>(drawable->gsNode());
  if (node == nullptr) {
    node = new AcGsEntityNode();
    node->setDrawable(drawable);
    drawable->setGsNode(reinterpret_cast<OdGsCache*>(node));

    OdGeExtents3d extent;
    if (drawable->getGeomExtents(extent) == OdResult::eOk)
       node->setExtents(extent);
  }

  entity_list_.Append(node);

#ifndef VP_DEPENDENT
  spatial_index_->insert(static_cast<OdSiEntity*>(node));
#endif
}

void AcGsContainerNode::remove(AcGsEntityNode* node) {
  if (node == nullptr)
    return;

  node->RemoveFromList();

#ifdef VP_DEPENDENT
  for (auto& iter : spatial_indexs_) {
    if (!iter.isNull())
      iter->remove(node);
  }
#else
  spatial_index_->remove(node);
#endif

  delete node;
}

//void OdGsContainerNode::display(OdGsDisplayContext& ctx)
void AcGsContainerNode::display(AcGsDisplayContext& context) {
  //clear display mark
  for (auto node = entity_list_.head(); node != entity_list_.end(); node = node->next())
    node->value()->markToSkip(true);

  AcGsViewImpl* view = static_cast<AcGsViewImpl*>(context.view());

  AcGeExtents3d extent;
#ifdef VP_DEPENDENT
  const OdSiSpatialIndex* spatial_index = spatialIndex(context.viewId());
#else
  const OdSiSpatialIndex* spatial_index = spatial_index_.get();
#endif
  spatial_index->extents(extent);
  const OdSiShape& shape = context.queryShape(extent);
  //set display mark
  spatial_index->query(shape, *this);
  
  //if 2d need draw order, 3d query and draw, not need draw order
  AcGsEntityNode* entity = nullptr;
  for(auto node = entity_list_.head(); node != entity_list_.end(); node = node->next()) {
    entity = node->value();
    if (!entity->markedToSkip())
      entity->display(context);
  }
}

#include <ppl.h>
#include "StaticRxObject.h"

struct VectorizerUpdate{
  VectorizerUpdate(std::vector<AcGsEntityNode*>& vec, AcGiVectorizer& view, int b, int e) : 
    entitys(vec), begin(b), end(e){
    vectorizer.setGsView(view.gsView());
    vectorizer.setContext(view.context());
    vectorizer.setDrawContext(&vectorizer);
    vectorizer.setViewId(0);
  }

  void operator()()const {
    for (int i = begin; i < end; ++i)
      entitys[i]->update(vectorizer);
  }

  mutable OdStaticRxObject<AcGiVectorizer> vectorizer;
  std::vector<AcGsEntityNode*>& entitys;
  int begin;
  int end;
};

//这里要考虑是图层修改或是实体修改
void AcGsContainerNode::update(AcGiVectorizer& vectorizer) {
#ifdef VP_DEPENDENT
  OdUInt32 vp = vectorizer.viewId();
  if (spatial_indexs_.size() <= vp){
    spatial_indexs_.resize(vp + 1);
    spatial_indexs_[vp] = OdSiSpatialIndex::createObject(OdSiSpatialIndex::kSiNoFlags, 0, 30, 20);
  }
  OdSiSpatialIndexPtr spatial_index = spatial_indexs_[vp];
#endif

#if 1
  std::vector<AcGsEntityNode*> entitys;
  //collect all entity need update, some entity may out of virtual boundary
  for (auto node = entity_list_.head(); node != entity_list_.end(); node = node->next())
    entitys.push_back(node->value());


#if 0
  VectorizerUpdate update1(entitys, vectorizer, 0, entitys.size() / 4);
  VectorizerUpdate update2(entitys, vectorizer, entitys.size() / 4, entitys.size() / 2);
  VectorizerUpdate update3(entitys, vectorizer, entitys.size() / 2, entitys.size() * 3 / 4);
  VectorizerUpdate update4(entitys, vectorizer, entitys.size() * 3 / 4, entitys.size());
  Concurrency::parallel_invoke([&update1]{ update1();}, [&update2]{update2();}, [&update3]{update3();}, [&update4]{update4();});
#else
  VectorizerUpdate update1(entitys, vectorizer, 0, entitys.size());
  update1();
#endif

#ifdef VP_DEPENDENT
  for (auto node = entity_list_.head(); node != entity_list_.end(); node = node->next())
    spatial_index->insert(node->value());
#endif
#else
  
  AcGsEntityNode* entity = nullptr;
  for (auto node = entity_list_.head(); node != entity_list_.end(); node = node->next()) {
    entity = node->value();
    entity->update(vectorizer);

#ifdef VP_DEPENDENT
    spatial_index->insert(entity);
#endif
  }
#endif
}

void AcGsContainerNode::visit(OdSiEntity* entity, bool completelyInside) {
  AcGsEntityNode* entity_node = static_cast<AcGsEntityNode*>(entity);
  entity_node->markToSkip(false);
}

const OdSiSpatialIndex* AcGsContainerNode::spatialIndex(OdUInt32 vp_id) const {
#ifdef VP_DEPENDENT
  return spatial_indexs_[vp_id];
#else
  return spatial_index_.get();
#endif
  
}
