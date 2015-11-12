// compile.h

#pragma once

#include "compilable.h"

namespace compile
{
	/// A class that adds compileing capabilities to a Mapping
	///
	class CompilableConstant : public Constant, public compilable
	{
		/// compiles a human-friendly description of the Mapping to a stream
		///
		virtual void compile(ostream& os, int indentation, int enumeration);
	};

	/// A class that adds compileing capabilities to a Mapping
	///
	class CompilableSum : public Sum, public compilable
	{
		/// compiles a human-friendly description of the Mapping to a stream
		///
		virtual void compile(ostream& os, int indentation, int enumeration);
	};

	/// A class that adds compileing capabilities to a Mapping
	///
	class CompilableDecisionTreePath : public DecisionTreePath, public compilable
	{
		/// compiles a human-friendly description of the Mapping to a stream
		///
		virtual void compile(ostream& os, int indentation, int enumeration);
	};

	/// A class that adds compileing capabilities to a Mapping
	///
	class CompilableRow : public Row, public compilable
	{
		/// compiles a human-friendly description of the Mapping to a stream
		///
		virtual void compile(ostream& os, int indentation, int enumeration);
	};

	/// A class that adds compileing capabilities to a Mapping
	///
	class CompilableColumn : public Column, public compilable
	{
		/// compiles a human-friendly description of the Mapping to a stream
		///
		virtual void compile(ostream& os, int indentation, int enumeration);
	};
}