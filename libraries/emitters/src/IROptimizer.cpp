////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IROptimizer.cpp (emitters)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IROptimizer.h"

#include "LLVMInclude.h"

// llvm
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Scalar/GVN.h>

// stl
#include <memory>

//
// Notes
//

// clang optimization passes for -O3:
//
// > cjacobs$ llvm-as < /dev/null | opt -O3 -disable-output -debug-pass=Arguments
// Pass Arguments:  -tti -tbaa -scoped-noalias -assumption-cache-tracker -targetlibinfo -verify -simplifycfg -domtree -sroa -early-cse -lower-expect
// Pass Arguments:  -targetlibinfo -tti -tbaa -scoped-noalias -assumption-cache-tracker -profile-summary-info -forceattrs -inferattrs -ipsccp -globalopt -domtree -mem2reg -deadargelim -domtree -basicaa -aa -instcombine -simplifycfg -pgo-icall-prom -basiccg -globals-aa -prune-eh -inline -functionattrs -argpromotion -domtree -sroa -early-cse -speculative-execution -lazy-value-info -jump-threading -correlated-propagation -simplifycfg -domtree -basicaa -aa -instcombine -tailcallelim -simplifycfg -reassociate -domtree -loops -loop-simplify -lcssa -basicaa -aa -scalar-evolution -loop-rotate -licm -loop-unswitch -simplifycfg -domtree -basicaa -aa -instcombine -loops -loop-simplify -lcssa -scalar-evolution -indvars -loop-idiom -loop-deletion -loop-unroll -mldst-motion -aa -memdep -gvn -basicaa -aa -memdep -memcpyopt -sccp -domtree -demanded-bits -bdce -basicaa -aa -instcombine -lazy-value-info -jump-threading -correlated-propagation -domtree -basicaa -aa -memdep -dse -loops -loop-simplify -lcssa -aa -scalar-evolution -licm -adce -simplifycfg -domtree -basicaa -aa -instcombine -barrier -elim-avail-extern -basiccg -rpo-functionattrs -globals-aa -float2int -domtree -loops -loop-simplify -lcssa -basicaa -aa -scalar-evolution -loop-rotate -loop-accesses -branch-prob -lazy-block-freq -opt-remark-emitter -loop-distribute -loop-simplify -lcssa -branch-prob -block-freq -scalar-evolution -basicaa -aa -loop-accesses -demanded-bits -loop-vectorize -loop-simplify -scalar-evolution -aa -loop-accesses -loop-load-elim -basicaa -aa -instcombine -scalar-evolution -demanded-bits -slp-vectorizer -simplifycfg -domtree -basicaa -aa -instcombine -loops -loop-simplify -lcssa -scalar-evolution -loop-unroll -instcombine -loop-simplify -lcssa -scalar-evolution -licm -instsimplify -scalar-evolution -alignment-from-assumptions -strip-dead-prototypes -globaldce -constmerge -verify
// Pass Arguments:  -domtree

// extra higher-level optimizations:
//
// > cjacobs$ echo 'int;' | clang -xc -O3 - -o /dev/null -\#\#\#
// Apple LLVM version 8.1.0 (clang-802.0.38)
// Target: x86_64-apple-darwin16.4.0
// Thread model: posix
// InstalledDir: /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin
//  "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang" "-cc1" "-triple" "x86_64-apple-macosx10.12.0" "-Wdeprecated-objc-isa-usage" "-Werror=deprecated-objc-isa-usage" "-emit-obj" "-disable-free" "-disable-llvm-verifier" "-discard-value-names" "-main-file-name" "-" "-mrelocation-model" "pic" "-pic-level" "2" "-mthread-model" "posix" "-mdisable-fp-elim" "-masm-verbose" "-munwind-tables" "-target-cpu" "penryn" "-target-linker-version" "278.4" "-dwarf-column-info" "-debugger-tuning=lldb" "-resource-dir" "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/../lib/clang/8.1.0" "-isysroot" "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.12.sdk" "-O3" "-fdebug-compilation-dir" "/Users/cjacobs" "-ferror-limit" "19" "-fmessage-length" "141" "-stack-protector" "1" "-fblocks" "-fobjc-runtime=macosx-10.12.0" "-fencode-extended-block-signature" "-fmax-type-align=16" "-fdiagnostics-show-option" "-fcolor-diagnostics" "-vectorize-loops" "-vectorize-slp" "-o" "/var/folders/4j/dqtpjht13g100m9xxk5t74n80000gn/T/--593f11.o" "-x" "c" "-"
//  "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/ld" "-demangle" "-lto_library" "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib/libLTO.dylib" "-dynamic" "-arch" "x86_64" "-macosx_version_min" "10.12.0" "-syslibroot" "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.12.sdk" "-o" "/dev/null" "/var/folders/4j/dqtpjht13g100m9xxk5t74n80000gn/T/--593f11.o" "-lSystem" "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/../lib/clang/8.1.0/lib/darwin/libclang_rt.osx.a"




// The above, reformatted:
//
// Pass Arguments:  -tti -tbaa -scoped-noalias -assumption-cache-tracker -targetlibinfo -verify -simplifycfg -domtree -sroa -early-cse -lower-expect
//
// Pass Arguments:  -targetlibinfo -tti -tbaa -scoped-noalias -assumption-cache-tracker -profile-summary-info -forceattrs -inferattrs -ipsccp -globalopt -domtree
// -mem2reg -deadargelim -domtree -basicaa -aa -instcombine -simplifycfg -pgo-icall-prom -basiccg -globals-aa -prune-eh -inline -functionattrs -argpromotion -domtree
// -sroa -early-cse -speculative-execution -lazy-value-info -jump-threading -correlated-propagation -simplifycfg -domtree -basicaa -aa -instcombine -tailcallelim
// -simplifycfg -reassociate -domtree -loops -loop-simplify -lcssa -basicaa -aa -scalar-evolution -loop-rotate -licm -loop-unswitch -simplifycfg -domtree -basicaa -aa
// -instcombine -loops -loop-simplify -lcssa -scalar-evolution -indvars -loop-idiom -loop-deletion -loop-unroll -mldst-motion -aa -memdep -gvn -basicaa -aa -memdep -memcpyopt
// -sccp -domtree -demanded-bits -bdce -basicaa -aa -instcombine -lazy-value-info -jump-threading -correlated-propagation -domtree -basicaa -aa -memdep -dse -loops -loop-simplify
// -lcssa -aa -scalar-evolution -licm -adce -simplifycfg -domtree -basicaa -aa -instcombine -barrier -elim-avail-extern -basiccg -rpo-functionattrs -globals-aa -float2int -domtree
// -loops -loop-simplify -lcssa -basicaa -aa -scalar-evolution -loop-rotate -loop-accesses -branch-prob -lazy-block-freq -opt-remark-emitter -loop-distribute -loop-simplify -lcssa
// -branch-prob -block-freq -scalar-evolution -basicaa -aa -loop-accesses -demanded-bits -loop-vectorize -loop-simplify -scalar-evolution -aa -loop-accesses -loop-load-elim -basicaa
// -aa -instcombine -scalar-evolution -demanded-bits -slp-vectorizer -simplifycfg -domtree -basicaa -aa -instcombine -loops -loop-simplify -lcssa -scalar-evolution -loop-unroll
// -instcombine -loop-simplify -lcssa -scalar-evolution -licm -instsimplify -scalar-evolution -alignment-from-assumptions -strip-dead-prototypes -globaldce -constmerge -verify
//
// Pass Arguments:  -domtree


// TODO: make a string->pass map and a function that takes the above string and generates the passes
// TODO: use PassManagerBuilder
// http://llvm.org/docs/doxygen/html/classllvm_1_1PassManagerBuilder.html

namespace ell
{
namespace emitters
{
    using namespace llvm;

    IRFunctionOptimizer::IRFunctionOptimizer(llvm::Module* pModule)
    {
        assert(pModule != nullptr);
        _pPasses = std::make_unique<legacy::FunctionPassManager>(pModule);
    }

    void IRFunctionOptimizer::AddStandardPasses()
    {
        AddMemoryToRegisterPass();
        AddInstructionCombiner();
        AddReassociation();
        AddCommonSubexpressionEliminator();
        AddControlFlowSimplification();
        AddDeadCodeEliminator();
        AddInstructionCombiner();
        AddVectorizationPasses();
    }

    // void IRFunctionOptimizer::AddPassByName(const std::string& pass)
    // {
// -aa
// -adce
// -alignment-from-assumptions
// -argpromotion
// -assumption-cache-tracker
// -barrier
// -basicaa
// -basiccg
// -bdce
// -block-freq
// -branch-prob
// -constmerge
// -correlated-propagation
// -deadargelim
// -demanded-bits
// -domtree
// -dse
// -early-cse
// -elim-avail-extern
// -float2int
// -forceattrs
// -functionattrs
// -globaldce
// -globalopt
// -globals-aa
// -gvn
// -indvars
// -inferattrs
// -inline
// -instcombine
// -instsimplify
// -ipsccp
// -jump-threading
// -lazy-block-freq
// -lazy-value-info
// -lcssa
// -licm
// -loop-accesses
// -loop-deletion
// -loop-distribute
// -loop-idiom
// -loop-load-elim
// -loop-rotate
// -loop-simplify
// -loop-unroll
// -loop-unswitch
// -loop-vectorize
// -loops
// -mem2reg
// -memcpyopt
// -memdep
// -mldst-motion
// -opt-remark-emitter
// -pgo-icall-prom
// -profile-summary-info
// -prune-eh
// -reassociate
// -rpo-functionattrs
// -scalar-evolution
// -sccp
// -scoped-noalias
// -simplifycfg
// -slp-vectorizer
// -speculative-execution
// -sroa
// -strip-dead-prototypes
// -tailcallelim
// -targetlibinfo
// -tbaa
// -tti
// -verify
    // }

    void IRFunctionOptimizer::AddInstructionCombiner()
    {
        _pPasses->add(llvm::createInstructionCombiningPass());
    }

    void IRFunctionOptimizer::AddDeadCodeEliminator()
    {
        _pPasses->add(llvm::createAggressiveDCEPass());
    }

    void IRFunctionOptimizer::AddReassociation()
    {
        _pPasses->add(llvm::createReassociatePass());
    }

    void IRFunctionOptimizer::AddMemoryToRegisterPass()
    {
        _pPasses->add(llvm::createPromoteMemoryToRegisterPass());
    }

    void IRFunctionOptimizer::AddCommonSubexpressionEliminator()
    {
        _pPasses->add(llvm::createGVNPass());
    }

    void IRFunctionOptimizer::AddControlFlowSimplification()
    {
        _pPasses->add(llvm::createCFGSimplificationPass());
        _pPasses->add(llvm::createLoopSimplifyPass());
        _pPasses->add(llvm::createLoopStrengthReducePass());
        _pPasses->add(llvm::createLoopIdiomPass());
    }

    void IRFunctionOptimizer::AddVectorizationPasses()
    {
        _pPasses->add(llvm::createBBVectorizePass());
        _pPasses->add(llvm::createLoopVectorizePass());
        _pPasses->add(llvm::createSLPVectorizerPass());
        // bool llvm::vectorizeBasicBlock(Pass * P, BasicBlock & BB, const VectorizeConfig& C = VectorizeConfig())
    }

    void IRFunctionOptimizer::Run(llvm::Function* pFunction)
    {
        assert(pFunction != nullptr);
        if (!_initialized)
        {
            _pPasses->doInitialization();
            _initialized = true;
        }
        _pPasses->run(*pFunction);
        _pPasses->doFinalization();
    }

    //
    // IRModuleOptimizer
    //

    void IRModuleOptimizer::AddStandardPasses()
    {
    }

    void IRModuleOptimizer::Run(llvm::Module* pModule)
    {
        assert(pModule != nullptr);
        _passes.run(*pModule, _analysis);
    }
}
}
