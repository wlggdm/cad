#include "OdaCommon.h"
#include "AcGiVectorizer.h"

#include "base/auto_reset.h"
#include "GiExtAccum.h"

#include "AcGsTraitRecorder.h"
#include "AcGsTextRecorder.h"
#include "AcGsPolylineRecorder.h"
#include "AcGsTriangleRecorder.h"
#include "AcGsEntityRecorder.h"

void AcGiVectorizer::onSetVertexData() {
  if (generate_shell_)
     shell_recorder_ = new AcGsShellRecorder;

  shell_recorder_->addVertexs(vertexDataCount(), vertexDataList());

  if (generate_shell_)
     getDisplayObject()->addRecorder(shell_recorder_);
}

void AcGiVectorizer::polylineOut(OdInt32 nbPoints, 
  const OdGePoint3d* pVertexList) {
#if 0
  if (!poly_recorder_){
    poly_recorder_ = new AcGsPolylineRecorder;
    getDisplayObject()->addRecorder(poly_recorder_);
  }
  poly_recorder_->setVertexs(nbPoints, pVertexList);
#else
  AcGsPolylineRecorder* poly_recorder = new AcGsPolylineRecorder;
  poly_recorder->setVertexs(nbPoints, pVertexList);
  getDisplayObject()->addRecorder(poly_recorder);
#endif
}

void AcGiVectorizer::polylineOut(OdInt32 numPoints, const OdInt32* vertexIndexList) {
  shell_recorder_->polyline(numPoints, vertexIndexList);
}

void AcGiVectorizer::triangleOut(const OdInt32* p3Vertices, const OdGeVector3d* pNormal) {
  shell_recorder_->triangle(3, p3Vertices);
  /*
  AcGsTriangleRecorder* triangle_recorder = new AcGsTriangleRecorder;
  triangle_recorder->setVertexs(vertexDataList()[p3Vertices[0]], vertexDataList()[p3Vertices[1]], vertexDataList()[p3Vertices[2]]);
  getDisplayObject()->addRecorder(triangle_recorder);
  */
}

const OdGeVector3d* AcGiVectorizer::extrusion(
  const OdGePoint3d& start, 
  const OdGePoint3d& point, 
  const OdGePoint3d& end) {
  if (OdNonZero(effectiveTraits().thickness())){
    primitive_plane_.set(start, point, end);
    extrusion_ = primitive_plane_.normal();
    extrusion_ *= effectiveTraits().thickness();
    return &extrusion_;
  }
  return 0;
}

const OdGeVector3d* AcGiVectorizer::extrusion(const OdGeVector3d* normal){
  if (normal)
    return extrusion(*normal);
  return 0;
}

const OdGeVector3d* AcGiVectorizer::extrusion(const OdGeVector3d& normal){
  if (OdNonZero(effectiveTraits().thickness())){
    extrusion_ = normal;
    extrusion_ *= (1. / normal.length() * effectiveTraits().thickness());
    return &extrusion_;
  }
  return 0;
}

OdGeMatrix3d AcGiVectorizer::getModelToWorldTransform() const {
  return *model_transform_.top();
}

OdGeMatrix3d AcGiVectorizer::getWorldToModelTransform() const {
  return model_transform_.top()->inverse();
}

void AcGiVectorizer::pushModelTransform(const OdGeVector3d& normal) {
  OdGeMatrix3d xfm = OdGeMatrix3d::planeToWorld(normal);
  pushModelTransform(xfm);
}

void AcGiVectorizer::pushModelTransform(const OdGeMatrix3d& xfm) {
  const OdGeMatrix3d* top = model_transform_.top();
  model_transform_.push(*top * xfm);

  xform.setTransform(getModelToWorldTransform());
  model_view_transform_ = view_->viewingMatrix() * getModelToWorldTransform();
}

void AcGiVectorizer::popModelTransform() {
  model_transform_.pop();

  xform.setTransform(getModelToWorldTransform());
  model_view_transform_ = view_->viewingMatrix() * getModelToWorldTransform();
}

void AcGiVectorizer::circle(const OdGePoint3d& center, double radius, const OdGeVector3d& normal) {
  if (effectivelyVisible() && !regenAbort()){
    onTraitsModified();

    xform.circleProc(center, radius, normal, extrusion(normal));
  }
}

void AcGiVectorizer::circle(const OdGePoint3d& start, const OdGePoint3d& point, const OdGePoint3d& end) {
  if (effectivelyVisible() && !regenAbort()){
    onTraitsModified();

    xform.circleProc(start, point, end, extrusion(start, point, end));
  }
}

void AcGiVectorizer::circularArc(
  const OdGePoint3d& center,
  double radius,
  const OdGeVector3d& normal,
  const OdGeVector3d& startVector,
  double sweepAngle,
  OdGiArcType arcType /*= kOdGiArcSimple*/) {
  if (effectivelyVisible() && !regenAbort()) {
    onTraitsModified();

    xform.circularArcProc(
      center, radius, normal, startVector, sweepAngle, arcType, extrusion(normal));
  }
}

void AcGiVectorizer::circularArc(
  const OdGePoint3d& start,
  const OdGePoint3d& point,
  const OdGePoint3d& end,
  OdGiArcType arcType /*= kOdGiArcSimple*/) {
  if (effectivelyVisible() && !regenAbort()){
    onTraitsModified();

    xform.circularArcProc(
      start, point, end, arcType, extrusion(start, point, end));
  }
}

void AcGiVectorizer::polyline(
  OdInt32 numVertices,
  const OdGePoint3d* vertexList,
  const OdGeVector3d* pNormal /*= 0*/,
  OdGsMarker baseSubEntMarker/* = -1*/) {
  if (effectivelyVisible() && !regenAbort() && numVertices && vertexList) {
    onTraitsModified();

    xform.polylineProc(numVertices, vertexList, pNormal, extrusion(pNormal), baseSubEntMarker);
  }
}

void AcGiVectorizer::polygon(OdInt32 numVertices, const OdGePoint3d* vertexList) {
  if (effectivelyVisible() && !regenAbort() && numVertices && vertexList) {
    onTraitsModified();

    xform.polygonProc(numVertices, vertexList);
  }
}

void AcGiVectorizer::pline(const OdGiPolyline& gi_polyline, OdUInt32 fromIndex /*= 0*/, OdUInt32 numSegs /*= 0*/) {
  if (effectivelyVisible() && !regenAbort()) {
    onTraitsModified();

    xform.plineProc(gi_polyline, 0, fromIndex, numSegs);
  }
}

void AcGiVectorizer::mesh(
  OdInt32 rows,
  OdInt32 columns,
  const OdGePoint3d* vertexList,
  const OdGiEdgeData* pEdgeData /*= 0*/,
  const OdGiFaceData* pFaceData /*= 0*/,
  const OdGiVertexData* pVertexData /*= 0*/) {
  if (effectivelyVisible() && !regenAbort()) {
    onTraitsModified();

    xform.meshProc(rows, columns, vertexList, pEdgeData, pFaceData, pVertexData);
  }
}

void AcGiVectorizer::shell(
  OdInt32 numVertices,
  const OdGePoint3d* vertexList,
  OdInt32 faceListSize,
  const OdInt32* faceList,
  const OdGiEdgeData* pEdgeData /*= 0*/,
  const OdGiFaceData* pFaceData /*= 0*/,
  const OdGiVertexData* pVertexData /*= 0*/) {
  if (effectivelyVisible() && !regenAbort() && numVertices && vertexList && faceListSize > 1 && faceList) {
    onTraitsModified();

    xform.shellProc(
      numVertices, vertexList, faceListSize, faceList, pEdgeData, pFaceData, pVertexData);
  }
}

static void odgiCalculateTextBasis(OdGeVector3d& u, OdGeVector3d& v,
  const OdGeVector3d& normal,
  const OdGeVector3d& direction,
  double height,
  double width,
  double oblique,
  bool bMirrorX,
  bool bMirrorY){
  if (OdZero(height)){
    height = 1.1e-10;
    width = 1.;
  }
  else if (OdZero(width))
    width = 1.0;

  u = direction.normal();
  v = normal.crossProduct(direction);
  if (!v.isZeroLength()) // #7208
    v.normalize();
  v *= height;
  u *= (width * height);

  if (bMirrorX)
    u.negate();
  if (bMirrorY)
    v.negate();

  if (OdNonZero(oblique) && (Oda2PI - oblique <= OdaToRadian(85.0) || oblique <= OdaToRadian(85.0)))
    v += (u * tan(oblique) / width);
}


static const OdGiTextStyle* odgiPrepareTextStyle(const OdGiTextStyle* pStyle, OdGiTextStyle& res){
  res = *pStyle;
  res.setBackward(false);
  res.setObliquingAngle(0.);
  res.setTextSize(1.);
  res.setUpsideDown(false);
  res.setXScale(1.);
  return &res;
}

void AcGiVectorizer::text(
  const OdGePoint3d& position,
  const OdGeVector3d& normal,
  const OdGeVector3d& direction,
  double height,
  double width,
  double oblique,
  const OdString& msg) {
  if (effectivelyVisible() && !regenAbort()){
    onTraitsModified();
    OdGeVector3d u, v;
    context()->getDefaultTextStyle(m_textStyle);
    odgiCalculateTextBasis(u, v, normal, direction, height, width, oblique, m_textStyle.isBackward(), m_textStyle.isUpsideDown());
    m_textStyle.setTextSize(1.0); // Text size is took into account in u, v vectors
    onTextProcessing(position, u, v);
    xform.textProc(
      position, u, v, msg, msg.getLength(), true, &m_textStyle, extrusion(normal));
  }
}

void AcGiVectorizer::text(
  const OdGePoint3d& position,
  const OdGeVector3d& normal,
  const OdGeVector3d& direction,
  const OdChar* msg,
  OdInt32 length,
  bool raw,
  const OdGiTextStyle* pTextStyle) {
  if (effectivelyVisible() && !regenAbort() && length > 0 && msg) {
    onTraitsModified();

    OdGeVector3d u, v;
    odgiCalculateTextBasis(u, v, normal, direction,
      pTextStyle->textSize(), pTextStyle->xScale(),
      pTextStyle->obliquingAngle(),
      pTextStyle->isBackward(),
      pTextStyle->isUpsideDown());

    ::odgiPrepareTextStyle(pTextStyle, m_textStyle);

    //if (!pTextStyle->isShxFont()) {
      OdGePoint3d extMin, extMax;
      OdGiExtAccum::textExtents(drawContext(), m_textStyle, msg, (int)length, raw ? kOdGiRawText : 0, extMin, extMax);

      OdGeVector3d uVector = u * (extMax.x - extMin.x);
      OdGeVector3d vVector = v * (extMax.y - extMin.y);
      OdGePoint3d minPos = position + (u * extMin.x + v * extMin.y);
      OdGeVector3d vv = (!pTextStyle->isVertical()) ? vVector : uVector;
      vv.transformBy(getModelToWorldTransform()); // AMark : #7215

      std::unique_ptr<AcGsTextRecorder> text_recorder(new AcGsTextRecorder());
      text_recorder->setHeightVector(vv);
      text_recorder->setShxFont(pTextStyle->isShxFont());

      OdGePoint3d polyl[2] = {
        minPos,
        minPos + vVector + uVector,
      };

      pushDisplayObject(text_recorder->deGenerate1());
      polyline(2, polyl);
      popDisplayObject();

      if (!pTextStyle->isShxFont()) {
        OdGiFillType fille_type = kOdGiFillAlways;
        OdGiFillType fillMode = subEntityTraits().fillType();
        if (fillMode != fille_type) {
          subEntityTraits().setFillType(fille_type);
          onTraitsModified();
        }

        pushDisplayObject(text_recorder->deGenerate5());

        OdGePoint3d polyg[4] = {
          minPos,
          minPos + vVector,
          minPos + vVector + uVector,
          minPos + uVector
        };

        generate_wires_ = false;
        polygon(4, polyg);
        generate_wires_ = true;
        popDisplayObject();

        if (fillMode != fille_type) {
          subEntityTraits().setFillType(fillMode);
          onTraitsModified();
        }
      }
      
      //文字是每个文字作为shell生成的，这里将他们合并到一起
      shell_recorder_ = text_recorder->generate();
      generate_shell_ = false;
      //pushDisplayObject(text_recorder->generate());
      xform.textProc(position, u, v, msg, length, raw, &m_textStyle, extrusion(normal));
      //popDisplayObject();
      generate_shell_ = true;
      shell_recorder_ = nullptr;
      getDisplayObject()->addRecorder(text_recorder.release());
    /*}
    else {
      onTextProcessing(position, u, v);

      xform.textProc(position, u, v, msg, length, raw, &m_textStyle, extrusion(normal));
    }
    */
  }
}

void AcGiVectorizer::xline(const OdGePoint3d& firstPoint, const OdGePoint3d& secondPoint) {
  if (effectivelyVisible() && !regenAbort()){
    onTraitsModified();


  }
}

void AcGiVectorizer::ray(const OdGePoint3d& basePoint, const OdGePoint3d& throughPoint) {
  if (effectivelyVisible() && !regenAbort()){
    onTraitsModified();

  }
}

void AcGiVectorizer::nurbs(const OdGeNurbCurve3d& nurbs) {
  if (effectivelyVisible() && !regenAbort()) {
    onTraitsModified();

    xform.nurbsProc(nurbs);
  }
}

void AcGiVectorizer::ellipArc(
  const OdGeEllipArc3d& ellipArc,
  const OdGePoint3d* endPointsOverrides /*= 0*/,
  OdGiArcType arcType /*= kOdGiArcSimple*/) {
  if (effectivelyVisible() && !regenAbort()){
    onTraitsModified();

    xform.ellipArcProc(ellipArc, endPointsOverrides, arcType);
  }
}

void AcGiVectorizer::pushClipBoundary(OdGiClipBoundary* pBoundary) {

}

void AcGiVectorizer::popClipBoundary() {

}

void AcGiVectorizer::worldLine(const OdGePoint3d points[2]) {
  if (effectivelyVisible() && !regenAbort()) {
    onTraitsModified();

    OdGeMatrix3d mat = getWorldToModelTransform();
    OdGePoint3d pt[2];
    pt[0] = mat * points[0];
    pt[1] = mat * points[1];

    polyline(2, pt);

  }
}

void AcGiVectorizer::image(
  const OdGiImageBGRA32& img,
  const OdGePoint3d& origin,
  const OdGeVector3d& uVec,
  const OdGeVector3d& vVec,
  OdGiRasterImage::TransparencyMode trpMode /*= OdGiRasterImage::kTransparency8Bit*/) {

}

void AcGiVectorizer::edge(const OdGiEdge2dArray& edges) {

}

void AcGiVectorizer::pushClipBoundary(OdGiClipBoundary* pBoundary, OdGiAbstractClipBoundary* pClipInfo) {

}

void AcGiVectorizer::polypoint(
  OdInt32 numPoints,
  const OdGePoint3d* vertexList,
  const OdCmEntityColor* pColors,
  const OdCmTransparency* pTransparency,
  const OdGeVector3d* pNormals /*= NULL*/,
  const OdGsMarker* pSubEntMarkers /*= NULL*/,
  OdInt32 nPointSize /*= 0*/) {
  if (effectivelyVisible() && !regenAbort() && numPoints && vertexList){
    onTraitsModified();

    if (pNormals && OdNonZero(effectiveTraits().thickness())){
      OdGeVector3dArray extrusions;
      extrusions.resize((OdUInt32)numPoints);
      OdGeVector3d *pExtrusion = extrusions.asArrayPtr();
      double thickness = effectiveTraits().thickness();
      for (OdInt32 nPoint = 0; nPoint < numPoints; nPoint++)
        pExtrusion[nPoint] = pNormals[nPoint].normal() * thickness;
      xform.polypointProc(numPoints, vertexList, pColors, pTransparency, pNormals, pExtrusion, pSubEntMarkers, nPointSize);
    }
    else
      xform.polypointProc(numPoints, vertexList, pColors, pTransparency, pNormals, NULL, pSubEntMarkers, nPointSize);
  }
}

void AcGiVectorizer::rowOfDots(OdInt32 numPoints,
  const OdGePoint3d& startPoint,
  const OdGeVector3d& dirToNextPoint) {
  xform.rowOfDotsProc(numPoints, startPoint, dirToNextPoint);
}

bool AcGiVectorizer::effectivelyVisible() const {
  return !effect_trait_data_.isLayerFrozen() && !effect_trait_data_.isLayerOff();
}

bool AcGiVectorizer::needDraw(OdUInt32 drawable_flag) {
  const OdGiSubEntityTraitsData& traits = effectiveTraits();

  if (GETBIT(drawable_flag, OdGiDrawable::kDrawableIsInvisible))
    return false;

  if (traits.isLayerFrozen())
    return false;

  if (traits.isLayerOff())
    return false;

  return true;
}

void AcGiVectorizer::setEffectiveTraits(const OdGiSubEntityTraitsData& traits,
  const OdGeVector3d* fillNormal /*= NULL*/) {
  OdGiSubEntityTraits& subEntTraits = subEntityTraits();
  subEntTraits.setTrueColor(traits.trueColor());
  subEntTraits.setFillType(traits.fillType());
  subEntTraits.setLayer(traits.layer());
  subEntTraits.setLineType(traits.lineType());
  subEntTraits.setLineTypeScale(traits.lineTypeScale());
  subEntTraits.setLineWeight(traits.lineWeight());
  subEntTraits.setPlotStyleName(traits.plotStyleNameType(), traits.plotStyleNameId());
  subEntTraits.setMaterial(traits.material());
  subEntTraits.setThickness(traits.thickness());
  subEntTraits.setMapper(traits.mapper());
  subEntTraits.setVisualStyle(traits.visualStyle());
  subEntTraits.setTransparency(traits.transparency());
  subEntTraits.setDrawFlags(traits.drawFlags());
  subEntTraits.setSelectionGeom(traits.selectionGeom());
  subEntTraits.setShadowFlags(traits.shadowFlags());
  subEntTraits.setSectionable(traits.sectionable());
  subEntTraits.setSelectionFlags(traits.selectionFlags());
  subEntTraits.setFillPlane(fillNormal);
  subEntTraits.setSecondaryTrueColor(traits.secondaryTrueColor());
  subEntTraits.setLineStyleModifiers(traits.lineStyleModifiers());
  subEntTraits.setFill(traits.fill());
  effectiveTraits();
}

const OdGiSubEntityTraitsData& AcGiVectorizer::effectiveTraits() const {
  static const int kSimpleFlagsChanged = kDrawFlagsChanged | kSelectionGeomChanged |
    kShadowFlagsChanged | kSectionableChanged | kSelectionFlagsChanged;

  if (isEntityTraitChanged()){
    if (isEntityTraitChanged(kAllChanged & ~kSimpleFlagsChanged)) {
      effect_trait_data_ = entity_trait_data_;
      affectTraits(block_trait_data_, effect_trait_data_);
    }
    else {
      if (isEntityTraitChanged(kDrawFlagsChanged))
        effect_trait_data_.setDrawFlags(entity_trait_data_.drawFlags());
      if (isEntityTraitChanged(kSelectionGeomChanged))
        effect_trait_data_.setSelectionGeom(entity_trait_data_.selectionGeom());
      if (isEntityTraitChanged(kShadowFlagsChanged))
        effect_trait_data_.setShadowFlags(entity_trait_data_.shadowFlags());
      if (isEntityTraitChanged(kSectionableChanged))
        effect_trait_data_.setSectionable(entity_trait_data_.sectionable());
      if (isEntityTraitChanged(kSelectionFlagsChanged))
        effect_trait_data_.setSelectionFlags(entity_trait_data_.selectionFlags());
    }

    clearEntityTraitChanged();
  }
  
  return effect_trait_data_;
}

void AcGiVectorizer::onTraitsModified() {
  AcGsTraitRecorder* trait_recorder = new AcGsTraitRecorder;
  trait_recorder->setColor(effectiveTraits().trueColor());
  trait_recorder->setDrawFlag(effectiveTraits().drawFlags());
  getDisplayObject()->addRecorder(trait_recorder);

  if (isEntityTraitChanged(kColorChanged)) {
    AcGsTraitRecorder* trait_recorder = new AcGsTraitRecorder;
    trait_recorder->setColor(effectiveTraits().trueColor());
    getDisplayObject()->addRecorder(trait_recorder);
  }
}

void AcGiVectorizer::generateShellFaces(OdInt32 faceListSize,
  const OdInt32* faceList,
  const OdGiEdgeData* pEdgeData /*= 0*/,
  const OdGiFaceData* pFaceData /*= 0*/) {
  OdGiGeometrySimplifier::generateShellFaces(faceListSize, faceList, pEdgeData, pFaceData);
  //direct3d 模式下，如果三角形太小不画，故生成线框
  if (generate_wires_)
     generateShellWires(faceListSize, faceList, pEdgeData, pFaceData);
}

void AcGiVectorizer::draw(const OdGiDrawable* drawable) {
  if (regenAbort())
    return;
  
  AcGsEntityRecorder* display_object = internalDraw(drawable, 0);
  if (display_object) {
      getDisplayObject()->addRecorder(display_object);
  }
}

class OdGiBlockTraitSaveState {
public:
  OdGiBlockTraitSaveState(const OdGiSubEntityTraitsData*& pByBlock, OdGiSubEntityTraitsData& entityTraitsData, OdGiSubEntityTraitsData& effectiveEntityTraitsData) :
    m_pByBlock(pByBlock), m_pPrevByBlock(pByBlock), m_byBlockTraits(entityTraitsData), 
    m_prevEntityTraitsData(entityTraitsData), m_entityTraitsData(entityTraitsData),
    m_effectiveEntityTraitsData(effectiveEntityTraitsData), m_prevEffectiveEntityTraitsData(effectiveEntityTraitsData){
    m_pByBlock = &m_byBlockTraits;
  }

  ~OdGiBlockTraitSaveState() {
    m_pByBlock = m_pPrevByBlock;
    m_entityTraitsData = m_prevEntityTraitsData;

    m_effectiveEntityTraitsData = m_prevEffectiveEntityTraitsData;
  }

public:
  const OdGiSubEntityTraitsData*& m_pByBlock;
  const OdGiSubEntityTraitsData*  m_pPrevByBlock;
  OdGiSubEntityTraitsData         m_byBlockTraits;
  OdGiSubEntityTraitsData         m_prevEntityTraitsData;
  OdGiSubEntityTraitsData&        m_entityTraitsData;
  OdGiSubEntityTraitsData&        m_effectiveEntityTraitsData;
  OdGiSubEntityTraitsData         m_prevEffectiveEntityTraitsData;
};

OdUInt32 AcGiVectorizer::setAttributes(const OdGiDrawable* pDrawable) {
  entity_trait_data_ = OdGiSubEntityTraitsData(); // default
  setEntityTraitChanged();

  return pDrawable->setAttributes(&subEntityTraits());
}

AcGsEntityRecorder* AcGiVectorizer::internalDraw(const OdGiDrawable* drawable, OdUInt32 drawable_flags) {
  //1. generate attribute
  //reset sub entity traits data first
  base::AutoReset<OdUInt32> auto_set(&drawable_attribute_, drawable_attribute_);

  AcGsEntityRecorder* display_object = nullptr;
  
  if (GETBIT(drawable_attribute_, OdGiDrawable::kDrawableIsCompoundObject)) {
    OdGiBlockTraitSaveState entity_trait_autoset(block_trait_data_, entity_trait_data_, effect_trait_data_);
    affectTraits(entity_trait_autoset.m_pPrevByBlock, entity_trait_autoset.m_byBlockTraits);

    drawable_attribute_ = setAttributes(drawable);
    if (needDraw(drawable_attribute_)) {
      pushDisplayObject(new AcGsEntityRecorder);

      bool ret = drawable->worldDraw(this);
      if (!ret)
        drawable->viewportDraw(this);

      display_object = static_cast<AcGsEntityRecorder*>(popDisplayObject());
    }

    setEntityTraitChanged();
  }
  else {
    drawable_attribute_ = setAttributes(drawable);

    if (needDraw(drawable_attribute_)) {
      pushDisplayObject(new AcGsEntityRecorder(GETBIT(drawable_attribute_, OdGiDrawable::kDrawableIsCompoundObject)));

      bool ret = drawable->worldDraw(this);
      if (!ret)
        drawable->viewportDraw(this);

      display_object = static_cast<AcGsEntityRecorder*>(popDisplayObject());
    }
  }

  return display_object;
}