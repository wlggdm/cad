#ifndef _ACGS_DISPLAY_H_
#define _ACGS_DISPLAY_H_

#include "acgs.h"
#include "si/SiSpatialIndex.h"
#include "Int32Array.h"

class AcGsRender {
public:
  virtual ~AcGsRender() {}

  virtual void setColor(const uint32_t& color) {}

  virtual void setGsMarker(const OdGsMarker& marker) {};

  virtual void polyline(Adesk::Int32 count, const AcGePoint3d* vertexs) {}

  virtual void triangle(OdUInt32 count, const AcGePoint3d* vertexs) {}

  virtual void setVertex(OdUInt32 count, const AcGePoint3d* vertexs) {}

  virtual void polyline(OdUInt32 count, const OdInt32* index) {}

  virtual void triangle(OdUInt32 count, const OdInt32* index) {}

  virtual void shell(const OdInt32Array& polylines, const OdInt32Array& triangles) {}
};

class AcGsDisplayContext{
public:
  virtual ~AcGsDisplayContext() {}

  virtual AcGsRender* render() = 0;

  virtual const OdSiShape& queryShape(const AcGeExtents3d& ext) = 0;

  virtual AcGsView* view() = 0;

  virtual bool intersectView(const AcGeExtents3d& extent, bool& fade) = 0;

  virtual int viewId() = 0;

  virtual bool fastInteractivity() const = 0;
};

class AcGsRecorder {
public:
  virtual ~AcGsRecorder() {}

  virtual void display(AcGsDisplayContext&) = 0;

  virtual bool extents(OdGeExtents3d& extents) const = 0;
};

#endif //_ACGS_DISPLAY_CONTEXT_H_