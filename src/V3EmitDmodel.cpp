// -*- mode: C++; c-file-style: "cc-mode" -*-
//*************************************************************************
// DESCRIPTION: Verilator: Emit D for model entry point class and helping C files
//

#include "V3EmitDmodel.h"


void emitDPionterDecl (const AstVar* nodep){
    
  if (nodep->isQuad()) {
    puts("ulong* ");
  } else if (nodep->widthMin() <= 8) {
    puts("ubyte* ");
  } else if (nodep->widthMin() <= 16) {
    puts("ushort* ");
  } else if (nodep->isWide()) {
    // puts("void* ");
  } else {
    puts("uint* ");
  }

  puts("_esdl__ptr_");
  puts(nodep->nameProtect());
  puts("\n");
}

void emitDFunction (const AstVar* nodep){
  puts("\nref ");
  if (nodep->isQuad()) {
    puts("ulong ");
  } else if (nodep->widthMin() <= 8) {
    puts("ubyte ");
  } else if (nodep->widthMin() <= 16) {
    puts("ushort ");
  } else if (nodep->isWide()) {
    // puts("void* ");
  } else {
    puts("uint ");
  }
  puts(nodep->nameProtect());
  puts("(){\n");
  puts("return *(top._esdl__ptr_"+nodep->nameProtect()+");\n");
  puts("}\n");
}

void EmitDModel::emitDFile(AstModeModule* modp){
  UASSERT(!m_ofp, "Output file should not be open");

  const string filename = v3Global.opt.makeDir() + "/" + topClassName() + "_esdl.d";
  newCFile(filename, /* slow: */ false, /* source: */ false);
  m_ofp = new V3OutDFile(filename);
  puts("//DESCRIPTION: Dlang code to link D classes and functions with the C++ classes\n");

  puts("extern(C++) {\n");
    
  puts("class " + topClassName() + "{\n");
  puts("//Symbol table, currently unimplemented, using void pointer\n");
  puts("void* vlSymsp;\n");
  puts("\n//PORTS \n");
  for (const AstNode* nodep = modp->stmtsp(); nodep; nodep = nodep->nextp()) {
    if (const AstVar* const varp = VN_CAST(nodep, Var)) {
      if (varp->isPrimaryIO()) { 
	emitDPionterDecl(varp);
      }
    }
  }
  puts("\n// CELLS\n //Currently unimplemented, using void pointers \n");
  for (AstNode* nodep = modp->stmtsp(); nodep; nodep = nodep->nextp()) {
    if (const AstCell* const cellp = VN_CAST(nodep, Cell)) {
      puts("void* const " + cellp->nameProtect() + ";\n");
    }
  }
  puts("\n// Root instance pointer, currently unimplemented, using void pointers \n");
  puts("void* rootp;\n");
  //end of topmost class
  puts("}\n");

  // function declarations
  // external constructor for the top class
  puts(topClassName() + " _esdl__constructor_" + topClassName() + "();");
  puts("void _esdl__evaluate(" + topClassName() + " obj);");
  puts("void _esdl__final(" + topClassName() + " obj);");

  //end extern
  puts("}");


  //wrapper class definition
  puts("class D" + topClassName() + "{\n");
  //pointer to C++ class
  puts(topClassName() + " top;");
  //constructor function
  puts("this (){\n");
  puts("top = new " + topClassName() "();\n");
  puts("}\n");
  puts("\n//Functions for Ports \n");
  for (const AstNode* nodep = modp->stmtsp(); nodep; nodep = nodep->nextp()) {
    if (const AstVar* const varp = VN_CAST(nodep, Var)) {
      if (varp->isPrimaryIO()) { 
	emitDFunction(varp);
      }
    }
  }
  //eval function
  puts("void eval() {\n");
  puts("_esdl__evaluate(top);\n");
  puts("}\n");
  //final function, named it finish
  puts("void finish() {\n");
  puts("_esdl__final(top);\n");
  puts("}\n");
  //class end
  puts("}\n"); 
  VL_DO_CLEAR(delete m_ofp, m_ofp = nullptr);
}
void EmitDModel::emitCHelperFile(AstModeModule* modp){
  UASSERT(!m_ofp, "Output file should not be open");
  const string filename = v3Global.opt.makeDir() + "/" + topClassName() + "_esdl.cpp";
  newCFile(filename, /* slow: */ false, /* source: */ false);
  m_ofp = new V3OutCFile(filename);
  puts("#include \"" + topClassName() + ".h\"\n\n");
  puts(topClassName() + "* _esdl__constructor_" + topClassName() + "(){\n");
  puts("return new " + topClassName() + "();\n");
  puts("}\n");
  puts("\n");
  puts("void _esdl__evaluate (" + topClassName() "* obj){\n");
  puts("obj->eval();\n");
  puts("}\n");
  puts("\n");
  puts("void _esdl__final (" + topClassName() "* obj){\n");
  puts("obj->final();\n");
  puts("}\n");
  puts("\n");
  VL_DO_CLEAR(delete m_ofp, m_ofp = nullptr);
}
