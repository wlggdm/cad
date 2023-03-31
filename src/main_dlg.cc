#include "main_dlg.h"

#include <atltypes.h>
#include <atlstr.h>
#include <atldlgs.h>

#include "oda_app.h"

#include "DbViewportTable.h"
#include "DbViewportTableRecord.h"
#include "DbBlockTableRecord.h"
#include "DbEntity.h"
#include "GeExtents2d.h"

#include "ui/compositor/context_provider.h"
#include "ui/gfx/geometry/size.h"
#include "ui/gfx/geometry/rect.h"

#include "AcGsManagerImpl.h"
#include "AcGsViewImpl.h"
#include "AcGsModelImpl.h"
#include "AcGsDeviceImpl.h"
#include "AcGsContainerNode.h"

#include "AcEdContext.h"

extern OdaApp oda_app;


namespace {
  const wchar_t kFilePath[] = L"G:\\photo\\dwg\\example_2.dwg";

  const int kScale = 20;
}
CMainDlg::CMainDlg() {
  model = nullptr;
  device = nullptr;
  view = nullptr;
  graphic_kernel = nullptr;

  gsmanager = new AcGsManagerImpl;
  graphic_kernel = AcGsManager::acquireGraphicsKernel();

  down = false;

  AcEdContext::SetJigHost(this);
}

LRESULT CMainDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
  CenterWindow();
  
  oda_app.SetHWND(m_hWnd);

  return TRUE;
}


void CMainDlg::OnPaint(HDC hdc) {
  CPaintDC dc(m_hWnd);

  if (device)
    device->update();
}

void  CMainDlg::OnDestroy() {
}

LRESULT CMainDlg::OnOK(WORD, WORD wID, HWND, BOOL&) {
  EndDialog(wID);
  return 0L;
}

LRESULT CMainDlg::OnCancel(WORD, WORD wID, HWND, BOOL&) {
  db_->setMultiThreadedMode(OdDb::kSTMode);
  EndDialog(wID);
  return 0L;
}

void CMainDlg::OnSize(UINT wparam, CSize size) {
  if (device)
    device->onSize(size.cx, size.cy);
}

LRESULT CMainDlg::OnEraseBackground(HDC hdc) {
  return 1L;
}

void CMainDlg::OnLButtonDouble(UINT wParam, CPoint point) {
  if (!db_.isNull())
    return;

  CFileDialog open_file(TRUE, L".dwg");
  if (open_file.DoModal() != IDOK)
    return;

  db_ = oda_app.readFile(OdString(open_file.m_szFileName), false, false);
  //db_->setMultiThreadedMode(OdDb::kMTRendering);
  //OdDbBaseDatabasePE *pDbPE = OdGsDbRootLinkage::getDbBaseDatabasePE(db_.get());
  //if (pDbPE)
  //  pDbPE->setMultiThreadedRender(db_.get(), true);

  OdDbViewportTablePtr view_table = db_->getViewportTableId().safeOpenObject();
  OdDbViewportTableRecordPtr view_record = view_table->getActiveViewportId().safeOpenObject();
  view = gsmanager->createAutoCADViewport(*graphic_kernel, view_record.get());
  device = gsmanager->createAutoCADDevice(*graphic_kernel, m_hWnd);
  model = gsmanager->createAutoCADModel(*graphic_kernel);

  AcGsModelImpl* model_impl = static_cast<AcGsModelImpl*>(model);
  db_->addReactor(model_impl);

  OdDbBlockTableRecordPtr model_drawable = db_->getModelSpaceId().safeOpenObject();
  view->add(model_drawable.get(), model);

  device->add(view);

  OdDbObjectId layer_id = db_->getLayerTableId();
  AcGsViewImpl* view_impl = static_cast<AcGsViewImpl*>(view);
  view_impl->updateLayer(layer_id);

  CRect rect;
  GetClientRect(rect);

  gicontext.setDatabase(db_);
  vectorizer.setContext(&gicontext);

  vectorizer.setDrawContext(&vectorizer);
  vectorizer.setGsView(view);
  vectorizer.setViewId(0);

  AcGsContainerNode* container = reinterpret_cast<AcGsContainerNode*>(model_drawable->gsNode());

  OdDbObjectIteratorPtr entity_iter = model_drawable->newIterator();
  for (entity_iter->start(); !entity_iter->done(); entity_iter->step())
    container->add(entity_iter->entity());

  container->update(vectorizer);

  PostMessage(WM_PAINT);
}

//这个函数将来封闭到ImpJig里， database采用api获取当前db
AcDbObjectId CMainDlg::append(OdDbEntity* entity) {
  if (entity == nullptr)
    return AcDbObjectId::kNull;

  OdDbBlockTableRecordPtr model_drawable = db_->getModelSpaceId().safeOpenObject(OdDb::kForWrite);
  entity->setDatabaseDefaults(db_);
  AcDbObjectId id = model_drawable->appendOdDbEntity(entity);
  entity->release();

  return id;
}

void dolly(AcGsView* pView, int x, int y) {
  OdGeVector3d vec(x, y, 0.0);
  vec.transformBy((pView->screenMatrix() * pView->projectionMatrix()).inverse());
  pView->dolly(vec);
}

LRESULT CMainDlg::OnMouseWheel(UINT wParam, short delta, CPoint point) {
  if (view) {
    ScreenToClient(&point);
    OdGePoint3d pos(view->position());
    pos.transformBy(view->worldToDeviceMatrix());

    int vx, vy;
    vx = (int)OdRound(pos.x);
    vy = (int)OdRound(pos.y);
    vx = point.x - vx;
    vy = point.y - vy;
    dolly(view, vx, vy);

    view->zoom(delta > 0 ? 1. / .8 : .8);
    dolly(view, -vx, -vy);

    PostMessage(WM_PAINT);
  }
  return 0L;
}

#define ACTION_DRAG 1
void CMainDlg::OnLeftButtonDown(UINT wParam, CPoint point) {
#if ACTION_DRAG
  if (view) {
    view_position = view->position();

    base_position.set(point.x, point.y, 0);
    base_position.transformBy(view->worldToDeviceMatrix().inverse());
    base_position = base_position - view_position.asVector();

    down = true;

    view->beginFastInteractivity();
  }
#else
  base_position.set(point.x, point.y, 0);
  base_position.transformBy(view->worldToDeviceMatrix().inverse());
  pickPoint(base_position);
#endif
}

void CMainDlg::OnMouseMove(UINT wParam, CPoint point) {
  MSG msg;
  while (::PeekMessage(&msg, NULL, WM_MOUSEMOVE, WM_MOUSEMOVE, PM_REMOVE)) {
  }

  ::GetCursorPos(&point);
  ::ScreenToClient(m_hWnd, &point);

#if ACTION_DRAG == 0
  base_position.set(point.x, point.y, 0);
  base_position.transformBy(view->worldToDeviceMatrix().inverse());
  trackPoint(base_position);
#else
  if (view && down) {
    OdGePoint3d value(point.x, point.y, 0);
    value.transformBy(view->worldToDeviceMatrix().inverse());
    value = value - view_position.asVector();

    OdGeVector3d delta = base_position - value;
    base_position = value;
    view->dolly(delta);
    view_position = view->position();

    PostMessage(WM_PAINT);
  }
#endif
}


void CMainDlg::OnLeftButtonUp(UINT wParam, CPoint point) {
  if (view && down) {
    down = false;

    view->endFastInteractivity();
    PostMessage(WM_PAINT);
  }
}

void CMainDlg::OnRButtonUp(UINT wParam, CPoint point) {
  SwitchToSelf();
}
