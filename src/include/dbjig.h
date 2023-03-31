#ifndef   _DBJIG_H_
#define   _DBJIG_H_ 1

#include "adesk.h"
#include "acgs.h"

class AcEdImpJig;

class AcEdJig{
public:
    enum UserInputControls {
        kGovernedByOrthoMode            = 0x000001,
        kNullResponseAccepted           = 0x000002,
        kDontEchoCancelForCtrlC         = 0x000004,
        kDontUpdateLastPoint            = 0x000008,
        kNoDwgLimitsChecking            = 0x000010,
        kNoZeroResponseAccepted         = 0x000020,
        kNoNegativeResponseAccepted     = 0x000040,
        kAccept3dCoordinates            = 0x000080,
        kAcceptMouseUpAsPoint           = 0x000100,
        kAnyBlankTerminatesInput        = 0x000200,
        kInitialBlankTerminatesInput    = 0x000400,
        kAcceptOtherInputString         = 0x000800,
        kGovernedByUCSDetect            = 0x001000,
        kNoZDirectionOrtho              = 0x002000,
        kImpliedFaceForUCSChange        = 0x004000,
        kUseBasePointElevation          = 0x008000,

        kDisableDirectDistanceInput     = 0x010000,
    };

    enum DragStatus {
        kModeless       = -17,
        kNoChange       = -6,
        kCancel         = -4,
        kOther          = -3,
        kNull           = -1,
        kNormal         = 0,
        kKW1,
        kKW2,
        kKW3,
        kKW4,
        kKW5,
        kKW6,
        kKW7,
        kKW8,
        kKW9
    };

    enum CursorType {
        kNoSpecialCursor = -1,           // No Special Cursor Specified
        kCrosshair = 0,                  // Full Screen Cross Hair. 
        kRectCursor,                     // Rectangular cursor. 
        kRubberBand,                     // Rubber band line. 
        kNotRotated,                     // NotRotated Type. 
        kTargetBox,                      // Target Box Type. 
        kRotatedCrosshair,               // Rotated Crosshair w/ rubber band. 
        kCrosshairNoRotate,              // Crosshairs forced non-rotated. 
        kInvisible,                      // Invisible cursor. 
        kEntitySelect,                   // Entity selection target cursor. 
        kParallelogram,                  // Parallelogram cursor. 
        kEntitySelectNoPersp,            // Pickbox, suppressed in persp. 
        kPkfirstOrGrips,                 // Auto-select cursor. 
        kCrosshairDashed                 // 15 dashed style crosshair cursor

    };
    AcEdJig();
    virtual ~AcEdJig();

    DragStatus drag();
    virtual DragStatus sampler(); 
    virtual Adesk::Boolean update();
    AcDbObjectId append();

    const ACHAR* keywordList();
    void         setKeywordList(const ACHAR*);

    const ACHAR* dispPrompt();
    void         setDispPrompt(const ACHAR*, ...);

    DragStatus acquireString(ACHAR (&str)[2049]);
    DragStatus acquireAngle(double &ang);
    DragStatus acquireAngle(double &ang, const AcGePoint3d& basePnt);
    DragStatus acquireDist(double &dist);
    DragStatus acquireDist(double &dist, const AcGePoint3d& basePnt);
    DragStatus acquirePoint(AcGePoint3d&);
    DragStatus acquirePoint(AcGePoint3d&, const AcGePoint3d&basePnt);

    AcEdJig::CursorType specialCursorType();
    void                setSpecialCursorType(CursorType);

    AcEdJig::UserInputControls userInputControls();
    void                       setUserInputControls(AcEdJig::UserInputControls);

    virtual AcDbEntity* entity() const;

private:
    AcEdImpJig*  mpImpJig;
};

#endif