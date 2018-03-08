////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IREmitterTest.cpp (emitters_test)
//  Authors:  Umesh Madan, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "IREmitterTest.h"

// emitters
#include "EmitterException.h"
#include "EmitterTypes.h"
#include "IRBlockRegion.h"
#include "IREmitter.h"
#include "IRExecutionEngine.h"
#include "IRFunctionEmitter.h"
#include "IRHeaderWriter.h"
#include "IRModuleEmitter.h"

// testing
#include "testing.h"

// utilities
#include "Unused.h"

// stl
#include <iostream>
#include <memory>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

using namespace ell;
using namespace ell::emitters;

std::string g_outputBasePath = "";
void SetOutputPathBase(std::string path)
{
    g_outputBasePath = std::move(path);
}

std::string OutputPath(const char* pRelPath)
{
    return g_outputBasePath + pRelPath;
}

std::vector<llvm::Instruction*> RemoveTerminators(llvm::Function* pfn)
{
    std::vector<llvm::Instruction*> terms;
    auto& blocks = pfn->getBasicBlockList();
    for (auto pBlock = blocks.begin(); pBlock != blocks.end(); ++pBlock)
    {
        std::cout << "##BLOCK## ";
        std::cout << std::string(pBlock->getName()) << std::endl;
        auto& instructions = pBlock->getInstList();
        for (auto pInst = instructions.begin(); pInst != instructions.end(); ++pInst)
        {
            if (pInst->isTerminator())
            {
                terms.push_back(&(*pInst));
            }
        }
    }
    for (auto pInstr : terms)
    {
        pInstr->removeFromParent();
    }
    return terms;
}

void InsertTerminators(llvm::Function* pfn, std::vector<llvm::Instruction*>& terms)
{
    size_t i = 0;
    auto& blocks = pfn->getBasicBlockList();
    for (auto pBlock = blocks.begin(); pBlock != blocks.end(); ++pBlock)
    {
        std::cout << "##BLOCK## ";
        std::cout << std::string(pBlock->getName()) << std::endl;
        auto& instructions = pBlock->getInstList();
        instructions.push_back(terms[i++]);
    }
}

void TestIREmitter()
{
    llvm::LLVMContext context;
    IREmitter emitter(context);

    // Create a module
    auto module1 = emitter.CreateModule("Module1");
    emitter.DeclareFunction(module1.get(), "foobar");

    // Create another module
    auto module2 = emitter.CreateModule("Module1");
    emitter.DeclareFunction(module2.get(), "foobar");

    module1->dump();
    module2->dump();
}

void TestLLVMShiftRegister()
{
    auto module = MakeHostModuleEmitter("Shifter");
    module.DeclarePrintf();

    std::vector<double> data({ 1.1, 2.1, 3.1, 4.1, 5.1 });
    std::vector<double> newData1({ 1.2, 2.2 });
    std::vector<double> newData2({ 3.2, 4.2 });

    auto shiftFunction = module.BeginMainFunction();
    llvm::GlobalVariable* pRegister = module.GlobalArray("g_shiftRegister", data);
    llvm::Value* c1 = module.ConstantArray("c_1", newData1);
    llvm::Value* c2 = module.ConstantArray("c_2", newData2);

    shiftFunction.Print("Begin\n");
    shiftFunction.PrintForEach("%f\n", pRegister, data.size());
    shiftFunction.Print("Shift 1\n");
    shiftFunction.ShiftAndUpdate<double>(pRegister, data.size(), newData1.size(), c1);
    shiftFunction.PrintForEach("%f\n", pRegister, data.size());
    shiftFunction.Print("Shift 2\n");
    shiftFunction.ShiftAndUpdate<double>(pRegister, data.size(), newData2.size(), c2);
    shiftFunction.PrintForEach("%f\n", pRegister, data.size());
    shiftFunction.Return();
    module.EndFunction();

    module.DebugDump();
    module.WriteToFile("shift.bc");
    module.WriteToFile("shift.ll");
    module.WriteToFile("shift.asm");
}

void TestEmitLLVM()
{
    auto module = MakeHostModuleEmitter("Looper");
    module.DeclarePrintf();

    llvm::StructType* structType = module.GetOrCreateStruct("ShiftRegister", { { "size", VariableType::Int32 }, { "value", VariableType::Double } });

    std::vector<double> data({ 3.3, 4.4, 5.5, 6.6, 7.7 });
    llvm::GlobalVariable* pData = module.ConstantArray("g_weights", data);
    llvm::GlobalVariable* pOutput = module.GlobalArray(VariableType::Double, "g_output", data.size());
    llvm::GlobalVariable* pTotal = module.Global(VariableType::Double, "g_total");
    llvm::GlobalVariable* pRegisters = module.GlobalArray("g_registers", structType, data.size());

    auto fnMain = module.BeginMainFunction();

    IRForLoopEmitter testLoop(fnMain);
    testLoop.Begin(data.size());
    testLoop.End();

    IRForLoopEmitter forLoop(fnMain);
    auto pBodyBlock = forLoop.Begin(data.size());
    {
        auto printBlock = fnMain.BlockAfter(pBodyBlock, "PrintBlock");
        fnMain.Branch(printBlock);
        fnMain.SetCurrentBlock(printBlock);

        auto i = forLoop.LoadIterationVariable();
        auto item = fnMain.ValueAt(pData, i);
        auto sum = fnMain.Operator(TypedOperator::addFloat, fnMain.Literal(0.3), item);
        fnMain.SetValueAt(pOutput, i, sum);
        fnMain.OperationAndUpdate(pTotal, TypedOperator::addFloat, sum);

        llvm::Value* pRegisterSum = fnMain.PointerOffset(pRegisters, i, fnMain.Literal(1));
        fnMain.Store(pRegisterSum, sum);

        IRIfEmitter ife(fnMain);
        ife.If(TypedComparison::lessThanFloat, item, fnMain.Literal(5.7));
        {
            fnMain.Print("First IF!\n");
        }
        ife.If(TypedComparison::equalsFloat, item, fnMain.Literal(6.6));
        {
            fnMain.Print("Second If!\n");
        }
        ife.Else();
        {
            fnMain.Print("Else\n");
        }
        ife.End();
        fnMain.Printf({ fnMain.Literal("%d, %f\n"), i, item });
    }
    forLoop.End();

    fnMain.SetValueAt(pOutput, fnMain.Literal(3), fnMain.Literal(10.0));
    fnMain.SetValueAt(pOutput, fnMain.Literal(4), fnMain.Literal(20.0));

    auto pOtherTotal = module.Global(VariableType::Double, "g_total");
    IRForLoopEmitter forLoop2(fnMain);
    forLoop2.Begin(data.size());
    {
        auto ival = forLoop2.LoadIterationVariable();
        auto v = fnMain.ValueAt(pOutput, ival);

        llvm::Value* pRegisterSum = fnMain.Load(fnMain.PointerOffset(pRegisters, ival, fnMain.Literal(1)));

        fnMain.OperationAndUpdate(pOtherTotal, TypedOperator::addFloat, v);
        fnMain.Printf({ fnMain.Literal("%f, %f\n"), v, pRegisterSum });
    }
    forLoop2.End();
    fnMain.Printf({ fnMain.Literal("Total = %f, OtherTotal= %f\n"), fnMain.Load(pTotal), fnMain.Load(pOtherTotal) });

    fnMain.Return();
    module.EndFunction();

    module.DebugDump();

    module.WriteToFile("loop.bc");
}

// Generate the Then, Else blocks first, then combine then in an if,else
void TestIfElseComplex()
{
    auto module = MakeHostModuleEmitter("IfElse");
    module.DeclarePrintf();

    auto fn = module.BeginMainFunction();
    auto pMainBlock = fn.GetCurrentBlock();
    fn.Print("Begin IfThen\n");
    // We deliberately create the done block first, so that we have to move blocks around when we do if then
    auto pDoneBlock = fn.BeginBlock("DoneBlock");
    {
        fn.Print("Done IfThen\n");
        fn.Return();
    }
    auto pThenBlock = fn.BeginBlock("ThenBlock");
    {
        fn.Print("Then Called\n");
        fn.Branch(pDoneBlock); // Inject an unconditional jump here, so we can test if the jump gets reset by the IfThen below
    }
    auto pElseBlock = fn.BeginBlock("ElseBlock");
    {
        fn.Print("Else Called\n");
        fn.Branch(pDoneBlock); // Inject an unconditional jump here, so we can test if the jump gets reset by the IfThen below
    }
    auto pCondBlock = fn.BeginBlock("Condition");
    {
        fn.Print("Checking condition\n");
        IRIfEmitter ife(fn);
        ife.IfThenElse(TypedComparison::lessThanFloat, fn.Literal(10.0), fn.Literal(5.0), pThenBlock, pElseBlock);
    }
    auto pAfterIf = fn.GetCurrentBlock();
    {
        fn.Print("After If\n");
        fn.BlockAfter(pAfterIf, pDoneBlock);
        fn.Branch(pDoneBlock);
    }
    fn.SetCurrentBlock(pMainBlock);
    {
        fn.Branch(pCondBlock);
    }

    module.EndFunction();
    module.DebugDump();
    module.WriteToFile("ifelse.bc");
}

void TestIfElseBlockRegions(bool runJit)
{
    auto module = MakeHostModuleEmitter("IfElse");
    module.DeclarePrintf();

    auto fn = module.BeginMainFunction();
    auto pMainBlock = fn.GetCurrentBlock();
    fn.Print("Begin IfThen\n");
    IRBlockRegionList regions;
    IRBlockRegion* pRegion1;
    IRBlockRegion* pRegion2;

    llvm::BasicBlock* pBlock = fn.BeginBlock("Region1_1");
    {
        pRegion1 = regions.Add(pBlock);
        fn.Print("Region1_1\n");
    }
    pBlock = fn.BeginBlock("Region1_2", true);
    {
        fn.Print("Region1_2\n");
    }
    pBlock = fn.BeginBlock("Region1_3", true);
    {
        fn.Print("Region1_3\n");
    }
    pRegion1->SetEnd(pBlock);

    pBlock = fn.BeginBlock("Region2_1");
    {
        pRegion2 = regions.Add(pBlock);
        fn.Print("Region2_1\n");
    }
    pBlock = fn.BeginBlock("Region2_2", true);
    {
        fn.Print("Region2_2\n");
    }
    pBlock = fn.BeginBlock("Region2_3", true);
    {
        fn.Print("Region2_3\n");
    }
    pRegion2->SetEnd(pBlock);

    IRIfEmitter ife(fn);
    auto pCondBlock = ife.IfThenElse(TypedComparison::lessThanFloat, fn.Literal(10.0), fn.Literal(5.0), pRegion1, pRegion2);
    fn.Print("Done IfThen\n");
    fn.Return();

    fn.SetCurrentBlock(pMainBlock);
    {
        fn.Branch(pCondBlock);
    }

    module.EndFunction();
    if (runJit)
    {
        IRExecutionEngine iee(std::move(module));
        iee.RunMain();
    }
    else
    {
        module.DebugDump();
        module.WriteToFile("ifelseRegion.bc");
    }
}

void TestLogical()
{
    auto module = MakeHostModuleEmitter("Logical");
    module.DeclarePrintf();

    auto fn = module.BeginFunction("TestLogical", VariableType::Void, { VariableType::Int32, VariableType::Int32, VariableType::Int32 });
    auto args = fn.Arguments().begin();
    llvm::Argument& val1 = *args++;
    llvm::Argument& val2 = *args++;
    llvm::Argument& val3 = *args++;

    auto pResult = fn.LogicalAnd(fn.Comparison(TypedComparison::equals, &val1, &val1), fn.Comparison(TypedComparison::equals, &val2, &val2));
    fn.Printf("And TRUE: %d\n", { pResult });

    pResult = fn.LogicalAnd(fn.Comparison(TypedComparison::equals, &val1, &val1), fn.Comparison(TypedComparison::equals, &val2, &val3));
    fn.Printf("And FALSE %d\n", { pResult });

    pResult = fn.LogicalAnd(fn.Comparison(TypedComparison::equals, &val1, &val3), fn.Comparison(TypedComparison::equals, &val2, &val3));
    fn.Printf("And FALSE %d\n", { pResult });

    pResult = fn.LogicalOr(fn.Comparison(TypedComparison::equals, &val1, &val1), fn.Comparison(TypedComparison::equals, &val2, &val3));
    fn.Printf("OR True %d\n", { pResult });

    pResult = fn.LogicalOr(fn.Comparison(TypedComparison::equals, &val2, &val3), fn.Comparison(TypedComparison::equals, &val1, &val1));
    fn.Printf("OR True %d\n", { pResult });

    pResult = fn.LogicalOr(fn.Comparison(TypedComparison::equals, &val2, &val3), fn.Comparison(TypedComparison::equals, &val1, &val2));
    fn.Printf("OR False %d\n", { pResult });

    pResult = fn.LogicalNot(fn.Comparison(TypedComparison::equals, &val1, &val1));
    fn.Printf("NOT True %d\n", { pResult });

    pResult = fn.LogicalNot(fn.Comparison(TypedComparison::equals, &val1, &val2));
    fn.Printf("NOT False %d\n", { pResult });

    fn.Return();
    module.EndFunction();

    fn.Verify();

    auto fnMain = module.BeginMainFunction();
    // We do this to prevent LLVM from doing constant folding.. so we can debug/see what is happening.
    fnMain.Call("TestLogical", { fnMain.Literal(5), fnMain.Literal(10), fnMain.Literal(15) });
    fnMain.Return();
    module.DebugDump();

    IRExecutionEngine jit(std::move(module));
    try
    {
        jit.RunMain();
    }
    catch (...)
    {
    }
}

void TestForLoop(bool runJit)
{
    auto module = MakeHostModuleEmitter("ForLoop");
    module.DeclarePrintf();

    auto add = GetOperator<double>(BinaryOperationType::add);
    auto varType = GetVariableType<double>();
    auto fn = module.BeginFunction("TestForLoop", VariableType::Void, VariableTypeList{});

    auto sum = fn.Variable(varType);

    fn.Print("Begin ForLoop\n");
    const int numIter = 10;
    IRForLoopEmitter forLoop(fn);
    forLoop.Begin(numIter);
    {
        auto i = forLoop.LoadIterationVariable();
        fn.Printf({ fn.Literal("i: %f\n"), i });
        fn.Store(sum, fn.Operator(add, fn.Load(sum), i));
    }
    forLoop.End();

    fn.Return();
    module.EndFunction();

    auto fnMain = module.BeginMainFunction();
    fnMain.Call("TestForLoop");
    fnMain.Return();

    if (runJit)
    {
        IRExecutionEngine jit(std::move(module));
        jit.RunMain();
    }
    else
    {
        module.DebugDump();
    }
}

void TestWhileLoop()
{
    auto module = MakeHostModuleEmitter("WhileLoop");
    module.DeclarePrintf();

    auto int8Type = GetVariableType<char>();
    auto int32Type = GetVariableType<int32_t>();

    auto fn = module.BeginMainFunction();
    {
        auto conditionVar = fn.Variable(int8Type, "cond");
        fn.Print("Begin while loop\n");
        auto i = fn.Variable(int32Type);
        fn.Store(i, fn.Literal<int>(5));
        IRWhileLoopEmitter whileLoop(fn);
        fn.Store(conditionVar, fn.TrueBit());
        whileLoop.Begin(conditionVar);
        {
            fn.Printf("i: %d\n", { fn.Load(i) });

            // i++
            fn.OperationAndUpdate(i, TypedOperator::add, fn.Literal<int>(1)); // i++

            // update conditionVar (i != 10)
            fn.Store(conditionVar, fn.Comparison(TypedComparison::notEquals, fn.Load(i), fn.Literal<int>(10)));
        }
        whileLoop.End();

        fn.Printf("Done with while loop: i = %d\n", { fn.Load(i) });
        fn.Return();
    }

    IRExecutionEngine jit(std::move(module));
    jit.RunMain();
}

void TestMetadata()
{
    auto module = MakeHostModuleEmitter("Metadata");

    // Function-level metadata
    auto fn = module.BeginFunction("TestMetadata", VariableType::Void);
    fn.InsertMetadata("hello.fn");
    fn.InsertMetadata("hello.fn.content", "test content");
    fn.Return();
    module.EndFunction();
    fn.Verify();

    // Module-level metadata
    module.InsertMetadata("hello.world");
    module.InsertMetadata("hello.world.content", { "12345" });
    module.InsertMetadata("hello.world.content", { "67890" });

    auto fnMain = module.BeginMainFunction();
    fnMain.Call("TestMetadata");
    fnMain.Return();
    module.DebugDump();

    // Missing metadata
    testing::ProcessTest("Testing missing module metadata check", testing::IsEqual(module.HasMetadata("does.not.exist"), false));
    testing::ProcessTest("Testing missing function metadata check", testing::IsEqual(module.HasFunctionMetadata("TestMetadata", "fn.does.not.exist"), false));

    // Empty metadata
    auto actualModuleMetadata = module.GetMetadata("hello.world");
    std::vector<std::string> flattenedModuleMetadata;
    for (auto m : actualModuleMetadata)
    {
        flattenedModuleMetadata.push_back(m[0]);
    }
    std::vector<std::string> expected{ "" };
    testing::ProcessTest("Testing empty module metadata check", testing::IsEqual(module.HasMetadata("hello.world"), true));
    testing::ProcessTest("Testing empty module metadata get", testing::IsEqual(flattenedModuleMetadata, expected));
    std::vector<std::string> actualFunctionMetadata = module.GetFunctionMetadata("TestMetadata", "hello.fn");
    testing::ProcessTest("Testing empty function metadata check", testing::IsEqual(module.HasFunctionMetadata("TestMetadata", "hello.fn"), true));
    testing::ProcessTest("Testing empty function metadata get", testing::IsEqual(actualFunctionMetadata, expected));

    // Non-empty metadata
    actualModuleMetadata = module.GetMetadata("hello.world.content");
    flattenedModuleMetadata.clear();
    for (auto m : actualModuleMetadata)
    {
        flattenedModuleMetadata.push_back(m[0]);
    }
    expected.clear();
    expected.push_back("12345");
    expected.push_back("67890");
    testing::ProcessTest("Testing non-empty module metadata check", testing::IsEqual(module.HasMetadata("hello.world.content"), true));
    testing::ProcessTest("Testing non-empty module metadata get", testing::IsEqual(flattenedModuleMetadata, expected));
    actualFunctionMetadata = module.GetFunctionMetadata("TestMetadata", "hello.fn.content");
    expected.clear();
    expected.push_back("test content");
    testing::ProcessTest("Testing non-empty function metadata check", testing::IsEqual(module.HasFunctionMetadata("TestMetadata", "hello.fn.content"), true));
    testing::ProcessTest("Testing non-empty function metadata get", testing::IsEqual(actualFunctionMetadata, expected));

    // Just for fun - metadata should have no effect
    IRExecutionEngine jit(std::move(module));
    jit.RunMain();
}

void TestHeader()
{
    auto module = MakeHostModuleEmitter("Predictor");

    auto int32Type = ell::emitters::VariableType::Int32;
    emitters::NamedVariableTypeList namedFields = { { "rows", int32Type }, { "columns", int32Type }, { "channels", int32Type } };
    auto shapeType = module.GetOrCreateStruct("Shape", namedFields);
    // test that this casues the type to show up in the module header.
    module.IncludeTypeInHeader(shapeType->getName());

    const emitters::NamedVariableTypeList parameters = { { "index", emitters::GetVariableType<int>() } };
    auto function = module.BeginFunction("Test_GetInputShape", shapeType, parameters);
    // test that this causes the function to show up in the module header
    function.IncludeInHeader();
    auto& emitter = module.GetIREmitter();
    auto& irBuilder = emitter.GetIRBuilder();
    llvm::AllocaInst* shapeVar = function.Variable(shapeType, "shape");
    auto rowsPtr = irBuilder.CreateInBoundsGEP(shapeType, shapeVar, { function.Literal(0), function.Literal(0) });
    auto columnsPtr = irBuilder.CreateInBoundsGEP(shapeType, shapeVar, { function.Literal(0), function.Literal(1) });
    auto channelsPtr = irBuilder.CreateInBoundsGEP(shapeType, shapeVar, { function.Literal(0), function.Literal(2) });
    function.Store(rowsPtr, function.Literal(224));
    function.Store(columnsPtr, function.Literal(224));
    function.Store(channelsPtr, function.Literal(3));
    function.Return(function.ValueAt(shapeVar));
    module.EndFunction();

    std::ostringstream out;
    ell::emitters::WriteModuleHeader(out, module);

    std::string result = out.str();
    auto structPos = result.find("typedef struct Shape");
    auto funcPos = result.find("Shape Test_GetInputShape(int32_t");
    testing::ProcessTest("Testing header generation",
                         structPos != std::string::npos && funcPos != std::string::npos);
}

std::string EmitStruct(const char* moduleName)
{
    auto module = MakeHostModuleEmitter(moduleName);
    const char* TensorShapeName = "TensorShape";
    auto int32Type = ell::emitters::VariableType::Int32;
    emitters::NamedVariableTypeList namedFields = { { "rows", int32Type }, { "columns", int32Type }, { "channels", int32Type } };
    auto shapeType = module.GetOrCreateStruct(TensorShapeName, namedFields);
    module.IncludeTypeInHeader(shapeType->getName());

    const emitters::NamedVariableTypeList parameters = { { "index", emitters::GetVariableType<int>() } };
    auto function = module.BeginFunction("Dummy", shapeType, parameters);
    function.IncludeInHeader();
    module.EndFunction();

    std::ostringstream out;
    ell::emitters::WriteModuleHeader(out, module);
    return out.str();
}

void TestTwoEmitsInOneSession()
{
    auto emit1 = EmitStruct("Mod1");
    auto emit2 = EmitStruct("Mod2");
    std::cout << emit1 << std::endl;
    std::cout << emit2 << std::endl;
    auto badpos1 = emit1.find("TensorShape.");
    auto badpos2 = emit2.find("TensorShape.");
    testing::ProcessTest("Testing two uses of module emitter",
                         badpos1 == std::string::npos && badpos2 == std::string::npos);
}

void TestStruct()
{
    auto module = MakeHostModuleEmitter("StructTest");
    auto& context = module.GetLLVMContext();
    auto int32Type = llvm::Type::getInt32Ty(context);
    auto int8PtrType = llvm::Type::getInt8PtrTy(context);
    auto doubleType = llvm::Type::getDoubleTy(context);

    llvm::StructType* structType = module.GetOrCreateStruct("MytStruct", { { "intField", int32Type }, { "ptrField", int8PtrType }, { "doubleField", doubleType } });

    module.DeclarePrintf();

    auto function = module.BeginMainFunction();
    {
        auto structVar = function.Variable(structType, "s");
        function.Store(function.GetStructFieldPointer(structVar, 0), function.Literal<int>(1));
        function.Store(function.GetStructFieldPointer(structVar, 1), function.Literal("Hello"));
        function.Store(function.GetStructFieldPointer(structVar, 2), function.Literal<double>(3.14));
        function.Return();
    }
    module.EndFunction();

    module.WriteToFile("testStruct.ll");
    module.WriteToFile("testStruct.h");
}

void TestDuplicateStructs()
{
    auto module = MakeHostModuleEmitter("DuplicateStructTest");
    auto& context = module.GetLLVMContext();
    auto int32Type = llvm::Type::getInt32Ty(context);
    auto int8PtrType = llvm::Type::getInt8PtrTy(context);
    auto doubleType = llvm::Type::getDoubleTy(context);

    // These should be fine --- the second GetOrCreateStruct call should return the existing type
    llvm::StructType* struct1TypeA = module.GetOrCreateStruct("MyStruct1", { { "intField", int32Type }, { "ptrField", int8PtrType }, { "doubleField", doubleType } });
    llvm::StructType* struct1TypeB = module.GetOrCreateStruct("MyStruct1", { { "intField", int32Type }, { "ptrField", int8PtrType }, { "doubleField", doubleType } });
    testing::ProcessTest("Testing double-declaration of equivalent structs", struct1TypeA == struct1TypeB);

    bool gotException = false;
    try
    {
        llvm::StructType* struct2TypeA = module.GetOrCreateStruct("MyStruct2", { { "intField", int32Type }, { "ptrField", int8PtrType }, { "doubleField", doubleType } });
        llvm::StructType* struct2TypeB = module.GetOrCreateStruct("MyStruct2", { { "intField", int8PtrType }, { "ptrField", int8PtrType }, { "doubleField", doubleType } });
        UNUSED(struct2TypeA);
        UNUSED(struct2TypeB);
    }
    catch (EmitterException&)
    {
        gotException = true;
    }

    testing::ProcessTest("Testing double-declaration of non-equivalent structs", gotException);
}

void TestScopedIf()
{
    auto module = MakeHostModuleEmitter("If");
    module.DeclarePrintf();

    auto fn = module.BeginMainFunction();
    auto cmp = fn.Comparison(TypedComparison::lessThanFloat, fn.Literal(10.0), fn.Literal(15.0));
    fn.If(cmp, [](IRFunctionEmitter& fn) {
        fn.Print("TrueBlock\n");
    });
    fn.Return();
    module.EndFunction();
    IRExecutionEngine iee(std::move(module));
    iee.RunMain();
}

void TestScopedIfElse()
{
    auto module = MakeHostModuleEmitter("IfElse");
    module.DeclarePrintf();

    auto fn = module.BeginMainFunction();
    fn.For(10, [](IRFunctionEmitter& fn, llvm::Value* index) {
        fn.Printf("index: ", { index });
        auto cmp = fn.Comparison(TypedComparison::lessThan, index, fn.Literal<int>(3));
        fn.If(cmp, [](IRFunctionEmitter& fn) {
              fn.Print("< 3\n");
          }).Else([](IRFunctionEmitter& fn) {
            fn.Print("not < 3\n");
        });
    });
    fn.Return();
    module.EndFunction();
    IRExecutionEngine iee(std::move(module));
    iee.RunMain();
}

void TestScopedIfElse2()
{
    auto module = MakeHostModuleEmitter("IfElse2");
    module.DeclarePrintf();

    auto fn = module.BeginMainFunction();
    fn.For(10, [](IRFunctionEmitter& fn, llvm::Value* index) {
        fn.Printf("Index: ", { index });
        auto cmp1 = fn.Comparison(TypedComparison::lessThan, index, fn.Literal<int>(3));
        auto cmp2 = fn.Comparison(TypedComparison::greaterThan, index, fn.Literal<int>(6));
        fn.If(cmp1, [](IRFunctionEmitter& fn) {
              fn.Print("< 3\n");
          }).ElseIf(cmp2, [](IRFunctionEmitter& fn) {
            fn.Print("> 6\n");
          }).Else([](IRFunctionEmitter& fn) {
            fn.Print("neither < 3 or > 6\n");
        });
    });
    fn.Return();
    module.EndFunction();
    IRExecutionEngine iee(std::move(module));
    iee.RunMain();
}
