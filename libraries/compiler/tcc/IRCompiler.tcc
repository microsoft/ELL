namespace emll
{
	namespace compiler
	{
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
		void IRCompiler::Compile(const nodes::ConstantNode<T>& node)
		{
			auto output = node.GetOutputPorts()[0];
			const std::vector<T>& values = node.output.GetOutput();
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
		void IRCompiler::Compile(const nodes::BinaryOperationNode<T>& node)
		{
			auto input1 = node.GetInputPorts()[0];
			auto input2 = node.GetInputPorts()[1];
			if (ModelEx::IsPureVector(*input1) && ModelEx::IsPureVector(*input2))
			{
				CompileLoop<T>(node);
			}
			else
			{
				CompileExpanded<T>(node);
			}
		}

		template<typename T>
		void IRCompiler::CompileLoop(const nodes::BinaryOperationNode<T>& node)
		{
			auto pVar1 = EnsureEmitted(node.GetInputPorts()[0]);
			auto pVar2 = EnsureEmitted(node.GetInputPorts()[1]);
			auto pOutput = node.GetOutputPorts()[0];
			auto pResult = EnsureEmitted(pOutput);

			_fn.OpV(GetOperator<T>(node), pOutput->Size(), pVar1, pVar2,
				[&pResult, this](llvm::Value* i, llvm::Value* pValue) {
				_fn.SetValueAt(pResult, i, pValue);
			});
		}

		template<typename T>
		void IRCompiler::CompileExpanded(const nodes::BinaryOperationNode<T>& node)
		{
			auto input1 = node.GetInputPorts()[0];
			auto input2 = node.GetInputPorts()[1];
			llvm::Value* pResult = EnsureEmitted(node.GetOutputPorts()[0]);
			for (size_t i = 0; i < input1->Size(); ++i)
			{
				llvm::Value* pVal1 = LoadVar(input1->GetOutputPortElement(i));
				llvm::Value* pVal2 = LoadVar(input2->GetOutputPortElement(i));
				llvm::Value* pOpResult = _fn.Op(GetOperator<T>(node), pVal1, pVal2);
				_fn.SetValueAt(pResult, _fn.Literal((int)i), pOpResult);
			}
		}
	}
}