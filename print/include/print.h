// print.h

#pragma once

#include "printable.h"

namespace print
{
	/// A class that adds printing capabilities to a Mapping
	///
	class PrintableConstant : public Constant, public printable
	{
		/// Prints a human-friendly description of the Mapping to a stream
		///
		virtual void print(ostream& os, int indentation, int enumeration);
	};

	/// A class that adds printing capabilities to a Mapping
	///
	class PrintableScale : public Scale, public printable
	{
		/// Prints a human-friendly description of the Mapping to a stream
		///
		virtual void print(ostream& os, int indentation, int enumeration);
	};

	/// A class that adds printing capabilities to a Mapping
	///
	class PrintableShift : public Shift, public printable
	{
		/// Prints a human-friendly description of the Mapping to a stream
		///
		virtual void print(ostream& os, int indentation, int enumeration);
	};

	/// A class that adds printing capabilities to a Mapping
	///
	class PrintableSum : public Sum, public printable
	{
		/// Prints a human-friendly description of the Mapping to a stream
		///
		virtual void print(ostream& os, int indentation, int enumeration);
	};

	/// A class that adds printing capabilities to a Mapping
	///
	class PrintableDecisionTreePath : public DecisionTreePath, public printable
	{
		/// Prints a human-friendly description of the Mapping to a stream
		///
		virtual void print(ostream& os, int indentation, int enumeration);
	};

	/// A class that adds printing capabilities to a Mapping
	///
	class PrintableRow : public Row, public printable
	{
		/// Prints a human-friendly description of the Mapping to a stream
		///
		virtual void print(ostream& os, int indentation, int enumeration);
	};

	/// A class that adds printing capabilities to a Mapping
	///
	class PrintableColumn : public Column, public printable
	{
		/// Prints a human-friendly description of the Mapping to a stream
		///
		virtual void print(ostream& os, int indentation, int enumeration);
	};
}