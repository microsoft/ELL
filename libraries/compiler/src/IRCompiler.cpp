#include "IRCompiler.h"

namespace emll
{
	namespace compiler
	{
		IRCompiler::IRCompiler()
			: _module(_emitter, "EMLL")
		{
		}
	}
}
