#ifndef   _ACED_CONTEXT_H_
#define   _ACED_CONTEXT_H_

class AcEdImpJig;
class AcEdJig;

class AcEdContext {
public:
  AcEdContext();
  ~AcEdContext();

  static AcEdImpJig* JigHost();
  static void        SetJigHost(AcEdImpJig* jig);

private:
  AcEdImpJig* jig_host_;
};

class AcEdContextSwitch {
public:
  AcEdContextSwitch();
  virtual ~AcEdContextSwitch();

  void SwitchToUI();

  void SwitchToSelf();

  virtual void Run() = 0;

private:
  void* context_;
};

#endif //_ACED_CONTEXT_H_