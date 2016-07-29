namespace emll
{
	namespace compiler
	{
		template<typename T>
		llvm::Value* IRCompiler::Emit(Variable& var)
		{
			if (var.IsScalar())
			{
				return EmitScalar<T>(var);
			}
			else if (var.IsVector())
			{
				return EmitVector<T>(var);
			}
			else
			{
				throw new CompilerException(CompilerError::variableTypeNotSupported);
			}
		}

		template<typename T>
		llvm::Value* IRCompiler::EmitScalar(Variable& var)
		{
			llvm::Value* pVal = nullptr;
			switch (var.Scope())
			{
			case VariableScope::Literal:
				pVal = EmitLiteral<T>(static_cast<LiteralVar<T>&>(var));
				_literals.Set(var.EmittedName(), pVal);
				break;
			case VariableScope::Local:
				if (var.IsVectorRef())
				{
					pVal = EmitRef<T>(static_cast<VectorElementVar<T>&>(var));
				}
				else if (var.HasInitValue())
				{
					pVal = EmitLocal<T>(static_cast<InitializedScalarVar<T>&>(var));
				}
				else
				{
					pVal = EmitLocal<T>(static_cast<ScalarVar<T>&>(var));
				}
				_locals.Set(var.EmittedName(), pVal);
				break;

			case VariableScope::Global:
				pVal = EmitGlobal<T>(static_cast<InitializedScalarVar<T>&>(var));
				break;

			default:
				throw new CompilerException(CompilerError::variableScopeNotSupported);
			}
			return pVal;
		}

		template<typename T>
		llvm::Value* IRCompiler::EmitVector(Variable& var)
		{
			llvm::Value* pVal = nullptr;
			switch (var.Scope())
			{
			case VariableScope::Literal:
				pVal = EmitLiteralVector<T>(static_cast<LiteralVarV<T>&>(var));
				_literals.Set(var.EmittedName(), pVal);
				break;
			case VariableScope::Global:
				if (var.HasInitValue())
				{
					pVal = EmitGlobalVector<T>(static_cast<InitializedVectorVar<T>&>(var));
				}
				else
				{
					pVal = EmitGlobalVector<T>(static_cast<VectorVar<T>&>(var));
				}
				_globals.Set(var.EmittedName(), pVal);
				break;
			default:
				throw new CompilerException(CompilerError::variableScopeNotSupported);
			}
			assert(pVal != nullptr);
			return pVal;
		}


		template<typename T>
		llvm::Value* IRCompiler::EmitLiteral(LiteralVar<T>& var)
		{
			llvm::Value* pVar = _fn.Literal(var.Data());
			return pVar;
		}

		template<typename T>
		llvm::Value* IRCompiler::EmitLocal(ScalarVar<T>& var)
		{
			return _fn.Var(var.Type(), var.EmittedName());
		}

		template<typename T>
		llvm::Value* IRCompiler::EmitLocal(InitializedScalarVar<T>& var)
		{
			llvm::Value* pVar = _fn.Var(var.Type(), var.EmittedName());
			_fn.Store(pVar, _fn.Literal(var.Data()));
			return pVar;
		}

		template<typename T>
		llvm::Value* IRCompiler::EmitRef(VectorElementVar<T>& var)
		{
			llvm::Value* pSrcVar = EnsureEmitted(var.Src());
			return _fn.PtrOffsetA(pSrcVar, _fn.Literal(var.Offset()), var.EmittedName());
		}

		template<typename T>
		llvm::Value* IRCompiler::EmitGlobal(InitializedScalarVar<T>& var)
		{
			llvm::Value* pVal = nullptr;
			if (var.IsMutable())
			{
				pVal = _module.Global(var.Type(), var.EmittedName());
				_fn.Store(pVal, _fn.Literal(var.Data()));
			}
			else
			{
				pVal = _module.Constant(var.Type(), var.EmittedName(), var.Data());
			}
			return pVal;
		}
		template<typename T>
		llvm::Value* IRCompiler::EmitLiteralVector(LiteralVarV<T>& var)
		{
			return _module.Constant(var.EmittedName(), var.Data());
		}

		template<typename T>
		llvm::Value* IRCompiler::EmitGlobalVector(VectorVar<T>& var)
		{
			return _module.Global(GetValueType<T>(), var.EmittedName(), var.Dimension());
		}

		template<typename T>
		llvm::Value* IRCompiler::EmitGlobalVector(InitializedVectorVar<T>& var)
		{
			return _module.Global(var.EmittedName(), var.Data());
		}


		template<typename T>
		void IRCompiler::ApplyComputed(ComputedVar<T>& var, llvm::Value* pDest)
		{
			assert(var.LastOp() != OperatorType::None);

			llvm::Value* pSrc = LoadVar(var.Src());
			T increment = var.IncrementBy();
			T multiplyBy = var.MultiplyBy();

			llvm::Value* pResult;
			if (increment == 0.0)
			{
				if (multiplyBy != 1.0)
				{
					pResult = _fn.Op(GetMultiplyForValueType<T>(), pSrc, _fn.Literal(multiplyBy));
					_fn.Store(pDest, pResult);
				}
				else
				{
					_fn.Store(pDest, pSrc);
				}
			}
			else
			{
				if (multiplyBy == 0.0)
				{
					_fn.Store(pDest, _fn.Literal(multiplyBy));
				}
				else if (multiplyBy == 1.0)
				{
					pResult = _fn.Op(GetAddForValueType<T>(), pSrc, _fn.Literal(increment));
					_fn.Store(pDest, pResult);
				}
				else if (var.LastOp() == OperatorType::Add)
				{
					pResult = _fn.Op(GetAddForValueType<T>(),
						_fn.Op(GetMultiplyForValueType<T>(), _fn.Literal(multiplyBy), pSrc),
						_fn.Literal(increment));
					_fn.Store(pDest, pResult);
				}
				else if (var.LastOp() == OperatorType::Multiply)
				{
					pResult = _fn.Op(GetMultiplyForValueType<T>(),
						_fn.Op(GetAddForValueType<T>(), _fn.Literal(increment), pSrc),
						_fn.Literal(multiplyBy));
					_fn.Store(pDest, pResult);
				}
			}
		}

		template<typename T>
		void IRCompiler::CompileConstant(const nodes::ConstantNode<T>& node)
		{
			auto output = node.GetOutputPorts()[0];
			auto values = node.GetValues();
			Variable* pVar = nullptr;
			if (output->Size() == 1)
			{
				pVar = Variables().AddVariable<LiteralVar<T>>(values[0]);
			}
			else
			{
				pVar = Variables().AddVariable<LiteralVarV<T>>(values);
			}
			SetVariableFor(output, pVar);
			EnsureEmitted(pVar);
		}

		template<typename T>
		void IRCompiler::CompileOutput(const model::OutputNode<T>& node)
		{
			// Output ports have exactly 1 input, output
			auto pInput = node.GetInputPorts()[0];
			llvm::Value* pOutputVar = EnsureEmitted(node.GetOutputPorts()[0]);
			for (size_t i = 0; i < pInput->Size(); ++i)
			{
				llvm::Value* pVal = LoadVar(pInput->GetOutputPortElement(i));
				_fn.SetValueAt(pOutputVar, _fn.Literal((int)i), pVal);
			}
		}

		template<typename T>
		void IRCompiler::CompileBinary(const nodes::BinaryOperationNode<T>& node)
		{
			auto pInput1 = node.GetInputPorts()[0];
			auto pInput2 = node.GetInputPorts()[1];
			if ((ModelEx::IsPureVector(*pInput1) && ModelEx::IsPureVector(*pInput2)) &&
				!ShouldUnrollLoops())
			{
				CompileBinaryLoop<T>(node);
			}
			else
			{
				CompileBinaryExpanded<T>(node);
			}
		}

		template<typename T>
		void IRCompiler::CompileBinaryLoop(const nodes::BinaryOperationNode<T>& node)
		{
			llvm::Value* pLVector = EnsureEmitted(node.GetInputPorts()[0]);
			llvm::Value* pRVector = EnsureEmitted(node.GetInputPorts()[1]);
			auto pOutput = node.GetOutputPorts()[0];
			llvm::Value* pResultVector = EnsureEmitted(pOutput);

			_fn.OpV(GetOperator<T>(node), pOutput->Size(), pLVector, pRVector,
				[&pResultVector, this](llvm::Value* i, llvm::Value* pValue) {
				_fn.SetValueAt(pResultVector, i, pValue);
			});
		}

		template<typename T>
		void IRCompiler::CompileBinaryExpanded(const nodes::BinaryOperationNode<T>& node)
		{
			auto pInput1 = node.GetInputPorts()[0];
			auto pInput2 = node.GetInputPorts()[1];
			auto pOutput = node.GetOutputPorts()[0];
			llvm::Value* pResult = EnsureEmitted(pOutput);
			Variable& resultVar = *(GetVariableFor(pOutput));
			for (size_t i = 0; i < pInput1->Size(); ++i)
			{
				llvm::Value* pLVal = LoadVar(pInput1->GetOutputPortElement(i));
				llvm::Value* pRVal = LoadVar(pInput2->GetOutputPortElement(i));
				llvm::Value* pOpResult = _fn.Op(GetOperator<T>(node), pLVal, pRVal);
				SetVar(resultVar, pResult, i, pOpResult);
			}
		}

		template<typename T>
		void IRCompiler::CompileDotProduct(const nodes::DotProductNode<T>& node)
		{
			auto pInput1 = node.GetInputPorts()[0];
			auto pInput2 = node.GetInputPorts()[1];
			if ((ModelEx::IsPureVector(*pInput1) && ModelEx::IsPureVector(*pInput2)) &&
				!ShouldUnrollLoops())
			{
				CompileDotProductLoop<T>(node);
			}
			else
			{
				CompileDotProductExpanded<T>(node);
			}
		}

		template<typename T>
		void IRCompiler::CompileDotProductLoop(const nodes::DotProductNode<T>& node)
		{
			llvm::Value* pLVector = EnsureEmitted(node.GetInputPorts()[0]);
			llvm::Value* pRVector = EnsureEmitted(node.GetInputPorts()[1]);
			auto pOutput = node.GetOutputPorts()[0];
			llvm::Value* pResult = EnsureEmitted(pOutput);
			_fn.DotProductF(pOutput->Size(), pLVector, pRVector, pResult);
		}

		template<typename T>
		void IRCompiler::CompileDotProductExpanded(const nodes::DotProductNode<T>& node)
		{
			auto pInput1 = node.GetInputPorts()[0];
			auto pInput2 = node.GetInputPorts()[1];
			auto pOutput = node.GetOutputPorts()[0];
			llvm::Value* pResult = EnsureEmitted(pOutput);
			Variable& resultVar = *(GetVariableFor(pOutput));

			_fn.Store(pResult, _fn.Literal(0.0));
			for (size_t i = 0; i < pInput1->Size(); ++i)
			{
				llvm::Value* pLVal = LoadVar(pInput1->GetOutputPortElement(i));
				llvm::Value* pRVal = LoadVar(pInput2->GetOutputPortElement(i));
				llvm::Value* pMultiplyResult = _fn.Op(GetMultiplyForValueType<T>(), pLVal, pRVal);
				_fn.OpAndUpdate(pResult, GetAddForValueType<T>(), pMultiplyResult);
			}
		}

		template<typename T>
		void IRCompiler::CompileSum(const nodes::SumNode<T>& node)
		{
			// SumNode has exactly 1 input and 1 output
			auto input = node.GetInputPorts()[0];
			if (ModelEx::IsPureVector(*input) && 
				!ShouldUnrollLoops())
			{
				CompileSumLoop<T>(node);
			}
			else
			{
				CompileSumExpanded<T>(node);
			}
		}

		template<typename T>
		void IRCompiler::CompileSumLoop(const nodes::SumNode<T>& node)
		{
			auto pInput = node.GetInputPorts()[0];
			auto pOutput = node.GetOutputPorts()[0];
			llvm::Value* pSrcVector = EnsureEmitted(pInput);
			llvm::Value* pResult = EnsureEmitted(pOutput);
			Variable& resultVar = *(GetVariableFor(pOutput));

			_fn.Store(pResult, _fn.Literal(0.0));
			auto forLoop = _fn.ForLoop();
			auto pBodyBlock = forLoop.Begin(pInput->Size());
			{
				auto i = forLoop.LoadIterationVar();
				llvm::Value* pValue = _fn.ValueAt(pSrcVector, i);
				_fn.OpAndUpdate(pResult, GetAddForValueType<T>(), pValue);
			}
			forLoop.End();
		}

		template<typename T>
		void IRCompiler::CompileSumExpanded(const nodes::SumNode<T>& node)
		{
			auto pInput = node.GetInputPorts()[0];
			auto pOutput = node.GetOutputPorts()[0];
			llvm::Value* pResult = EnsureEmitted(pOutput);
			Variable& resultVar = *(GetVariableFor(pOutput));

			_fn.Store(pResult, _fn.Literal(0.0));
			for (size_t i = 0; i < pInput->Size(); ++i)
			{
				llvm::Value* pValue = LoadVar(pInput->GetOutputPortElement(i));
				_fn.OpAndUpdate(pResult, GetAddForValueType<T>(), pValue);
			}
		}

		template<typename T>
		void IRCompiler::CompileAccumulator(const nodes::AccumulatorNode<T>& node)
		{
			// AccumulatorNode has exactly 1 input and 1 output
			// Accumulators are always long lived - either globals or heap. Currently, we use globals
			auto pInput = node.GetInputPorts()[0];
			auto pOutput = node.GetOutputPorts()[0];
			Variable* pVar = Variables().AddVectorVariable(VariableScope::Global, GetValueType<T>(), pOutput->Size());
			SetVariableFor(pOutput, pVar);

			if (ModelEx::IsPureVector(*pInput) && 
				!ShouldUnrollLoops())
			{
				CompileAccumulatorLoop<T>(node);
			}
			else
			{
				CompileAccumulatorExpanded<T>(node);
			}
		}

		template<typename T>
		void IRCompiler::CompileAccumulatorLoop(const nodes::AccumulatorNode<T>& node)
		{
			auto pInput = node.GetInputPorts()[0];
			auto pOutput = node.GetOutputPorts()[0];
			llvm::Value* pInputVector = EnsureEmitted(pInput);
			llvm::Value* pAccumulatorVector = EnsureEmitted(pOutput);

			_fn.OpV(GetAddForValueType<T>(), pOutput->Size(), pAccumulatorVector, pInputVector,
				[&pAccumulatorVector, this](llvm::Value* i, llvm::Value* pValue) {
				_fn.SetValueAt(pAccumulatorVector, i, pValue);
			});
		}

		template<typename T>
		void IRCompiler::CompileAccumulatorExpanded(const nodes::AccumulatorNode<T>& node)
		{
			auto pInput = node.GetInputPorts()[0];
			auto pOutput = node.GetOutputPorts()[0];
			llvm::Value* pAccumulatorVector = EnsureEmitted(pOutput);
			Variable& accumulatorVar = *(GetVariableFor(pOutput));

			for (size_t i = 0; i < pInput->Size(); ++i)
			{
				llvm::Value* pVal = LoadVar(pInput->GetOutputPortElement(i));
				_fn.OpAndUpdate(_fn.PtrOffset(pAccumulatorVector, _fn.Literal((int) i)), GetAddForValueType<T>(), pVal);
			}
		}

		template<typename T>
		void IRCompiler::CompileDelay(const nodes::DelayNode<T>& node)
		{
			auto pInput = node.GetInputPorts()[0];
			auto pOutput = node.GetOutputPorts()[0];
			size_t sampleSize = pOutput->Size();
			size_t windowSize = node.GetWindowSize();
			size_t bufferSize = sampleSize * windowSize;
			//
			// Delay nodes are always long lived - either globals or heap. Currently, we use globals
			// Each sample chunk is of size == sampleSize. The number of chunks we hold onto == windowSize
			// We need two buffers - one for the entire lot, one for the "last" chunk forwarded to the next operator
			// 
			Variable* pVarAllWindows = Variables().AddVariable<VectorVar<T>>(VariableScope::Global, bufferSize);
			llvm::Value* pAllWindows = EnsureEmitted(pVarAllWindows);

			Variable* pVarOutputBuffer = Variables().AddVariable<VectorVar<T>>(VariableScope::Global, sampleSize);
			SetVariableFor(pOutput, pVarOutputBuffer);
			llvm::Value* pOutputBuffer = EnsureEmitted(pVarOutputBuffer);
			//
			// We implement a delay not as a Shift Register
			//
			llvm::Value* pInputBuffer = EnsureEmitted(pInput);
			_fn.ShiftAndUpdate<T>(pAllWindows, bufferSize, sampleSize, pInputBuffer, pOutputBuffer);
		}
	}
}