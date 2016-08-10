////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Compiler.tcc (compiler)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace emll
{
	namespace compiler
	{
		template<typename T>
		void Compiler::CompileConstant(const nodes::ConstantNode<T>& node)
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
			EnsureVarEmitted(pVar);
		}
	}
}