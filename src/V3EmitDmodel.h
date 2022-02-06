
#include "config_build.h"
#include "verilatedos.h"

#include "V3Global.h"
#include "V3EmitCFunc.h"
#include "V3UniqueNames.h"

class EmitDModel final : public EmitCFunc {
  void emitDFile(AstModeModule* modp);
  void emitCHelperFile(AstModeModule* modp);
}
