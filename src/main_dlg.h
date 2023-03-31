#include <atlbase.h>
#include <atlapp.h>
#include <atlcrack.h>
#include <atltypes.h>
#include "resource.h"

#include "DbDatabase.h"
#include "StaticRxObject.h"
#include "GiContextForDbDatabase.h"

#include "AcGiVectorizer.h"
#include "AcEdImpJig.h"

class AcGsManager;

class CMainDlg : public CDialogImpl<CMainDlg>, public AcEdImpJig{
public:
  enum {
    IDD = IDD_MAINFRAME
  };

  CMainDlg();

  BEGIN_MSG_MAP_EX(CMainDlg)
    MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
    MSG_WM_DESTROY(OnDestroy)
    COMMAND_ID_HANDLER(IDOK, OnOK)
    COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
    MSG_WM_PAINT(OnPaint)
    MSG_WM_SIZE(OnSize)
    MSG_WM_ERASEBKGND(OnEraseBackground)
    MSG_WM_MOUSEWHEEL(OnMouseWheel)
    MSG_WM_LBUTTONDOWN(OnLeftButtonDown)
    MSG_WM_MOUSEMOVE(OnMouseMove)
    MSG_WM_LBUTTONUP(OnLeftButtonUp)
    MSG_WM_RBUTTONUP(OnRButtonUp)
    MSG_WM_LBUTTONDBLCLK(OnLButtonDouble)
  END_MSG_MAP()

  LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
  void    OnDestroy();
  void    OnPaint(HDC hdc);

  LRESULT OnOK(WORD, WORD, HWND, BOOL&);
  LRESULT OnCancel(WORD, WORD, HWND, BOOL&);
  void    OnSize(UINT wparam, CSize size);
  LRESULT OnMouseWheel(UINT wParam, short, CPoint point);
  void    OnLeftButtonDown(UINT wParam, CPoint point);
  void    OnMouseMove(UINT wParam, CPoint point);
  void    OnLeftButtonUp(UINT wParam, CPoint point);
  void    OnRButtonUp(UINT wParam, CPoint point);
  LRESULT OnEraseBackground(HDC hdc);
  void    OnLButtonDouble(UINT wParam, CPoint point);

  virtual AcDbObjectId append(OdDbEntity*) override;

private:
  OdDbDatabasePtr db_;

  AcGsView* view;
  AcGsDevice* device;
  AcGsModel* model;
  AcGsManager* gsmanager;
  AcGsGraphicsKernel* graphic_kernel;

  OdStaticRxObject<AcGiVectorizer> vectorizer;
  OdStaticRxObject<OdGiContextForDbDatabase> gicontext;

  OdGePoint3d  view_position;
  OdGePoint3d  base_position;

  bool down;
};