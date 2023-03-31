#include "OdaCommon.h"
#include "AcGiVectorizer.h"
#include "AcGsLayerNode.h"

#include "GiNonEntityTraits.h"
#include "DbLayerTableRecord.h"
#include <atlstr.h>

#include "ColorMapping.h"
#include "DbSymUtl.h"

#define SWAPBYTES(a) ODRGB(ODGETBLUE(a), ODGETGREEN(a), ODGETRED(a))

void AcGiVectorizer::updateLayerTraits(OdGiSubEntityTraitsData& traits) const {
  OdDbStub* layer_id = traits.layer();
  if (!layer_id)
    return;

  AcGsViewImpl* view_impl = static_cast<AcGsViewImpl*>(view_);
  effect_layer_data_ = *view_impl->layerNode(layer_id);

  int flags = 0;
  SETBIT(flags, OdGiSubEntityTraitsData::kLayerFrozen, GETBIT(effect_layer_data_.flags(), OdGiLayerTraits::kFrozen));
  SETBIT(flags, OdGiSubEntityTraitsData::kLayerOff,GETBIT(effect_layer_data_.flags(), OdGiLayerTraits::kOff));
  traits.setFlags(flags);
}

void AcGiVectorizer::affectTraits(const OdGiSubEntityTraitsData* from, OdGiSubEntityTraitsData& to) const {
  updateLayerTraits(to);

  if (from && GETBIT(effect_layer_data_.flags(), OdGiLayerTraits::kByBlock)) {
    to.setLayer(from->layer());
    updateLayerTraits(to);
  }

  if (to.trueColor().isByBlock())
    to.setTrueColor(from ? from->trueColor() : OdCmEntityColor(OdCmEntityColor::kForeground));

  if (to.lineWeight() == OdDb::kLnWtByBlock)
    to.setLineWeight(from ? from->lineWeight() : OdDb::kLnWtByLwDefault);

  if (to.plotStyleNameType() == OdDb::kPlotStyleNameByBlock){
    if (from)
      to.setPlotStyleName(from->plotStyleNameType(), from->plotStyleNameId());
    else
      to.setPlotStyleName(OdDb::kPlotStyleNameIsDictDefault, 0);
  }

  if (to.transparency().isByBlock())
      to.setTransparency(from ? from->transparency() : OdCmTransparency(OdUInt8(0xFF)));

  // "BYLAYER" ITEMS
  const OdGiLayerTraitsData& layerTraits = effect_layer_data_;
  const bool bXrefOverride = false; // GETBIT(m_flags, kXrefOverride);
  if (to.trueColor().isByLayer() || bXrefOverride)
    to.setTrueColor(layerTraits.trueColor());

  if ((to.lineWeight() == OdDb::kLnWtByLayer) || bXrefOverride){
    to.setLineWeight(layerTraits.lineweight());
    if (to.lineWeight() == OdDb::kLnWtByLayer)
      to.setLineWeight(OdDb::kLnWtByLwDefault);
  }

  /*
  if (to.lineWeight() == OdDb::kLnWtByLwDefault)
  {
    to.setLineWeight(m_pContext->defaultLineWeight());
  }
  */
  if ((to.plotStyleNameType() == OdDb::kPlotStyleNameByLayer) || bXrefOverride)
    to.setPlotStyleName(layerTraits.plotStyleNameType(), layerTraits.plotStyleNameId());

  /*
  if (!to.isLayerFrozen() && !layerVisible(m_effectiveLayerId))
  {
    //layerTraits.setFlags(m_effectiveLayerTraitsData.flags() | OdGiLayerTraits::kFrozen);
    to.setFlags(to.flags() | OdGiSubEntityTraitsData::kLayerFrozen);
  }
  */
  /*
  if (GETBIT(m_materialAttributes, OdGiMaterialTraits::kByLayer))
  {
    to.setMaterial(layerTraits.materialId());
  }
  */
  if (to.transparency().isByLayer() || bXrefOverride)
    to.setTransparency(layerTraits.transparency());
}

void AcGiVectorizer::setColor(OdUInt16 color) {
  if (entity_trait_data_.trueColor().isByColor() || 
    color != entity_trait_data_.trueColor().colorIndex()) {
    setEntityTraitChanged(kColorChanged, true);
    entity_trait_data_.setColor(color);
  }
}

void AcGiVectorizer::setTrueColor(const OdCmEntityColor& color) {
  if (color != entity_trait_data_.trueColor()){
    setEntityTraitChanged(kColorChanged, true);
    entity_trait_data_.setTrueColor(color);
  }
}

void AcGiVectorizer::setLayer(OdDbStub* layerId) {
  if (layerId != entity_trait_data_.layer()){
    setEntityTraitChanged(kLayerChanged, true);
    entity_trait_data_.setLayer(layerId);
  }
}

void AcGiVectorizer::setLineType(OdDbStub* lineTypeId) {
  if (lineTypeId != entity_trait_data_.lineType()){
    setEntityTraitChanged(kLineTypeChanged, true);
    entity_trait_data_.setLineType(lineTypeId);
  }
}

void AcGiVectorizer::setSelectionMarker(OdGsMarker selectionMarker) {
  /*
  if (GETBIT(m_flags, kAttributesSegment))
    return;
  if (nSelectionMarker != m_nSelectionMarker)
    selectionMarkerOnChange(nSelectionMarker);
  else
    m_nPrevSelectionMarker = nSelectionMarker;
  */
}

void AcGiVectorizer::setFillType(OdGiFillType fillType) {
  if (fillType != entity_trait_data_.fillType()){
    setEntityTraitChanged(kFillTypeChanged, true);
    entity_trait_data_.setFillType(fillType);
  }
}

//void setFillPlane(const OdGeVector3d* pNormal = 0) override;
void AcGiVectorizer::setLineWeight(OdDb::LineWeight lineWeight) {
  if (lineWeight != entity_trait_data_.lineWeight()){
    setEntityTraitChanged(kLineWeightChanged, true);
    entity_trait_data_.setLineWeight(lineWeight);
  }
}

void AcGiVectorizer::setLineTypeScale(double lineTypeScale /*= 1.0*/) {
  if (lineTypeScale != entity_trait_data_.lineTypeScale()){
    setEntityTraitChanged(kLineTypeScaleChanged, true);
    entity_trait_data_.setLineTypeScale(lineTypeScale);
  }
}

void AcGiVectorizer::setThickness(double thickness) {
  if (thickness != entity_trait_data_.thickness()){
    setEntityTraitChanged(kThicknessChanged, true);
    entity_trait_data_.setThickness(thickness);
  }
}

void AcGiVectorizer::setPlotStyleName(OdDb::PlotStyleNameType plotStyleNameType, OdDbStub* plotStyleNameId /*= 0*/) {
  if (plotStyleNameType != entity_trait_data_.plotStyleNameType()
    || plotStyleNameId != entity_trait_data_.plotStyleNameId()){
    setEntityTraitChanged(kPlotStyleChanged, true);
    entity_trait_data_.setPlotStyleName(plotStyleNameType, plotStyleNameId);
  }
}

void AcGiVectorizer::setMaterial(OdDbStub* materialId) {
  if (materialId != entity_trait_data_.material()){
    setEntityTraitChanged(kMaterialChanged, true);
    entity_trait_data_.setMaterial(materialId);
  }
}

void AcGiVectorizer::setMapper(const OdGiMapper *pMapper) {
  /*
  if (((pMapper == NULL || entity_trait_data_.mapper() == NULL) && pMapper != entity_trait_data_.mapper()) ||
    (pMapper != NULL && entity_trait_data_.mapper() != NULL &&
      *pMapper != *entity_trait_data_.mapper())){
    SetEntityTraitChanged(kMapperChanged, true);
    if (pMapper)
      entity_trait_data_.setMapper(m_pCurMaterialMapper);
    else
      entity_trait_data_.setMapper(NULL);
  }
  */
}

void AcGiVectorizer::setVisualStyle(const OdDbStub* visualStyleId) {
  if (visualStyleId != entity_trait_data_.visualStyle()){
    setEntityTraitChanged(kVisualStyleChanged, true);
    entity_trait_data_.setVisualStyle(const_cast<OdDbStub*>(visualStyleId));
  }
}

void AcGiVectorizer::setTransparency(const OdCmTransparency &transparency) {
  if (transparency != entity_trait_data_.transparency()){
    setEntityTraitChanged(kTransparencyChanged, true);
    entity_trait_data_.setTransparency(transparency);
  }
}

void AcGiVectorizer::setDrawFlags(OdUInt32 drawFlags) {
  if (drawFlags != entity_trait_data_.drawFlags()){
    if ((drawFlags^entity_trait_data_.drawFlags())&~kInternalDrawFlags)
      setEntityTraitChanged(kDrawFlagsChanged, true);
    entity_trait_data_.setDrawFlags(drawFlags);
  }
}

void AcGiVectorizer::setSelectionGeom(bool bSelectionFlag) {
  if (bSelectionFlag != entity_trait_data_.selectionGeom()){
    setEntityTraitChanged(kSelectionGeomChanged, true);
    entity_trait_data_.setSelectionGeom(bSelectionFlag);
  }
}

void AcGiVectorizer::setShadowFlags(ShadowFlags shadowFlags) {
  if (shadowFlags != entity_trait_data_.shadowFlags()){
    setEntityTraitChanged(kShadowFlagsChanged, true);
    entity_trait_data_.setShadowFlags(shadowFlags);
  }
}

void AcGiVectorizer::setSectionable(bool bSectionableFlag) {
  if (bSectionableFlag != entity_trait_data_.sectionable()){
    setEntityTraitChanged(kSectionableChanged, true);
    entity_trait_data_.setSectionable(bSectionableFlag);
  }
}

void AcGiVectorizer::setSelectionFlags(SelectionFlags selectionFlags) {
  if (selectionFlags != entity_trait_data_.selectionFlags()){
    setEntityTraitChanged(kSelectionFlagsChanged, true);
    entity_trait_data_.setSelectionFlags(selectionFlags);
  }
}

void AcGiVectorizer::setSecondaryTrueColor(const OdCmEntityColor& color) {
  if (color != entity_trait_data_.secondaryTrueColor()){
    setEntityTraitChanged(kSecColorChanged, true);
    entity_trait_data_.setSecondaryTrueColor(color);
  }
}

const OdGiDgLinetypeModifiers* AcGiVectorizer::lineStyleModifiers() const {
  /*
  if (((pLSMod == NULL || entity_trait_data_.lineStyleModifiers() == NULL) && pLSMod != entity_trait_data_.lineStyleModifiers()) ||
    (pLSMod != NULL && entity_trait_data_.lineStyleModifiers() != NULL &&
      *pLSMod != *entity_trait_data_.lineStyleModifiers()))
  {
    SetEntityTraitChanged(kLSModifiersChanged, true);
    if (pLSMod)
    {
      *m_pCurLSModifiers = *pLSMod;
      entity_trait_data_.setLineStyleModifiers(m_pCurLSModifiers);
    }
    else
      entity_trait_data_.setLineStyleModifiers(NULL);
  }
  */

  return nullptr;
}

void AcGiVectorizer::setFill(const OdGiFill* pFill) {
  if (pFill != entity_trait_data_.fill()){
    setEntityTraitChanged(kFillChanged, true);
    entity_trait_data_.setFill(pFill);
  }
}

OdUInt16 AcGiVectorizer::color() const {
  return entity_trait_data_.color();
}

OdCmEntityColor AcGiVectorizer::trueColor() const {
  return entity_trait_data_.trueColor();
}

OdDbStub* AcGiVectorizer::layer() const {
  return entity_trait_data_.layer();
}

OdDbStub* AcGiVectorizer::lineType() const {
  return entity_trait_data_.lineType();
}

OdGiFillType AcGiVectorizer::fillType() const {
  return entity_trait_data_.fillType();
}

//bool fillPlane(OdGeVector3d& normal) override;
OdDb::LineWeight AcGiVectorizer::lineWeight() const {
  return entity_trait_data_.lineWeight();
}

double AcGiVectorizer::lineTypeScale() const {
  return entity_trait_data_.lineTypeScale();
}

double AcGiVectorizer::thickness() const {
  return entity_trait_data_.thickness();
}

OdDb::PlotStyleNameType AcGiVectorizer::plotStyleNameType() const {
  return entity_trait_data_.plotStyleNameType();
}

OdDbStub* AcGiVectorizer::plotStyleNameId() const {
  return entity_trait_data_.plotStyleNameId();
}

OdDbStub* AcGiVectorizer::material() const {
  return entity_trait_data_.material();
}

const OdGiMapper* AcGiVectorizer::mapper() const {
  return entity_trait_data_.mapper();
}

OdDbStub* AcGiVectorizer::visualStyle() const {
  return entity_trait_data_.visualStyle();
}

OdCmTransparency AcGiVectorizer::transparency() const {
  return entity_trait_data_.transparency();
}

OdUInt32 AcGiVectorizer::drawFlags() const {
  return entity_trait_data_.drawFlags();
}

bool AcGiVectorizer::selectionGeom() const {
  return entity_trait_data_.selectionGeom();
}

OdGiSubEntityTraits::ShadowFlags AcGiVectorizer::shadowFlags() const {
  return entity_trait_data_.shadowFlags();
}

bool AcGiVectorizer::sectionable() const {
  return entity_trait_data_.sectionable();
}

OdGiSubEntityTraits::SelectionFlags AcGiVectorizer::selectionFlags() const {
  return entity_trait_data_.selectionFlags();
}

OdCmEntityColor AcGiVectorizer::secondaryTrueColor() const {
  return entity_trait_data_.secondaryTrueColor();
}

void AcGiVectorizer::setLineStyleModifiers(const OdGiDgLinetypeModifiers* pLSMod) {

}

const OdGiFill* AcGiVectorizer::fill() const {
  return entity_trait_data_.fill();
}