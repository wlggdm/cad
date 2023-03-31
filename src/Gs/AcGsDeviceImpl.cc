#include "AcGsDeviceImpl.h"

#include "ui/compositor/context_provider.h"
#include "ui/gfx/geometry/size.h"

#include "AcGsViewImpl.h"

namespace {
  gfx::Size WindowSize(AcGsWindowingSystemID id) {
    RECT rect;
    ::GetClientRect(reinterpret_cast<HWND>(id), &rect);
    return gfx::Size(rect.right - rect.left, rect.bottom - rect.top);
  }
}

AcGsDeviceImpl::AcGsDeviceImpl() {
}

AcGsDeviceImpl::~AcGsDeviceImpl() {
  ContextProvider::GetInstance()->RemoveCompositor(reinterpret_cast<HWND>(window_id_));
}

void AcGsDeviceImpl::setWindowSystemID(AcGsWindowingSystemID id) {
  window_id_ = id;
}

AcGsWindowingSystemID AcGsDeviceImpl::windowSystemID() const {
  return window_id_;
}

void AcGsDeviceImpl::setGraphicsKernel(AcGsGraphicsKernel* graphics_kernel) {
  graphics_kernel_ = graphics_kernel;
}

AcGsGraphicsKernel & AcGsDeviceImpl::graphicsKernel(void) {
  return *graphics_kernel_;
}

void AcGsDeviceImpl::invalidate(void) {
  //::InvalidateRect(reinterpret_cast<HWND>(window_id_), nullptr, FALSE);
}

void AcGsDeviceImpl::invalidate(const AcGsDCRect &rect) {
  //RECT window_rect = {rect.m_min.x, rect.m_min.y, rect.m_max.x, rect.m_max.y};
  //::InvalidateRect(reinterpret_cast<HWND>(window_id_), &window_rect, FALSE);
}

void AcGsDeviceImpl::invalidate(AcGsModel::RenderType pane) {
  
}

void AcGsDeviceImpl::invalidate(const AcGsDCRect &rect, AcGsModel::RenderType pane) {
  
}

bool AcGsDeviceImpl::isValid(void) const {
  return true;
}

void AcGsDeviceImpl::update(AcGsDCRect *pUpdatedRect /*= nullptr*/) {
  gfx::Size size = WindowSize(window_id_);

  ContextProvider::GetInstance()->ResizeDisplay((HWND)window_id_, gfx::Size(size.width(), size.height()));

  for (auto& iter : views_)
    iter->update();
}

void AcGsDeviceImpl::update(AcGsDCRect* pUpdatedRect, GS::SyncBehavior sync) {
  
}

Atil::Image * AcGsDeviceImpl::createSnapshot(const AcGsDCPoint& offset, const AcGsDCRect& imageDim, GS::ImageDataFormat format, GS::ImageOrientation orientation) {
  return nullptr;
}

int AcGsDeviceImpl::getMaxDeviceWidth(void) {
  return 0;
}

int AcGsDeviceImpl::getMaxDeviceHeight(void) {
  return 0;
}

void AcGsDeviceImpl::setDesiredFrameRate(float frameRate) {
  
}

void AcGsDeviceImpl::pauseInteractiveRender(void) {
  
}

void AcGsDeviceImpl::resumeInteractiveRender(void) {
 
}

void AcGsDeviceImpl::postRenderProgressCheckingReqeust() {
  
}

AcGsDevice::EnableIRResult AcGsDeviceImpl::beginInteractiveRender(AcGsRenderProgressMonitor* pProgressMonitor) {
  return EnableIRResult::kSuccess;
}

void AcGsDeviceImpl::endInteractiveRender(void) {
  
}

bool AcGsDeviceImpl::isRendering(void) const {
  return true;
}

void AcGsDeviceImpl::setFontKerningDisplay(bool bDisplay) {
  
}

void AcGsDeviceImpl::onSize(int width, int height) {
  AcGsDCRect rect(0, width, height, 0);
  for (auto& iter : views_)
    iter->setViewport(rect);
}

void AcGsDeviceImpl::onRealizeForegroundPalette(void) {
  
}

void AcGsDeviceImpl::onRealizeBackgroundPalette(void) {
  
}

void AcGsDeviceImpl::onDisplayChange(int nBitsPerPixel, int nXRes, int nYRes) {
  
}

bool AcGsDeviceImpl::add(AcGsView * view) {
  gfx::Size size = WindowSize(window_id_);

  AcGsDCRect dc_rect(0, size.width(), size.height(), 0);

  auto iter = std::find(views_.begin(), views_.end(), view);
  if (iter == views_.end()) {
    views_.push_back(view);
    view->setViewport(dc_rect);

    static_cast<AcGsViewImpl*>(view)->setDevice(this);
  }

  invalidate();

  return true;
}

bool AcGsDeviceImpl::erase(AcGsView * view) {
  auto iter = std::find(views_.begin(), views_.end(), view);
  if (iter != views_.end())
    views_.erase(iter);

  invalidate();
  return true;
}

void AcGsDeviceImpl::eraseAll(void) {
  views_.clear();

  invalidate();
}

bool AcGsDeviceImpl::setBackgroundColor(AcGsColor color) {
  background_color_ = color;
  return true;
}

AcGsColor AcGsDeviceImpl::getBackgroundColor(void) {
  return background_color_;
}

void AcGsDeviceImpl::setLogicalPalette(const AcGsColor * palette, int nCount) {
  
}

void AcGsDeviceImpl::setPhysicalPalette(const AcGsColor * palette, int nCount) {
  
}

void AcGsDeviceImpl::getSnapShot(Atil::Image * pOutput, AcGsDCPoint const & offset) {
  
}

void AcGsDeviceImpl::setDeviceRenderer(RendererType type) {
  renderer_type_ = type;
}

AcGsDevice::RendererType AcGsDeviceImpl::getDeviceRenderer(void) {
  return renderer_type_;
}

void AcGsDeviceImpl::setRenderInterrupter(AcGsRenderInterrupter* pInterrupter) {
  
}

AcGsRenderInterrupter* AcGsDeviceImpl::getRenderInterrupter(void) const {
  return nullptr;
}

void AcGsDeviceImpl::setDisplayUpdateSuppressed(bool bSuppressed) {
  
}
