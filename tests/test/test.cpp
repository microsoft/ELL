//// test.cpp
//

//#include "layers.h"
//#include "MutableDecisionTree.h"
//#include "QpLayoutGenerator.h"
#include "CommandLineParser.h"
//#include <vector>
//#include <memory>
//#include <iostream>
//#include <fstream>
//#include <sstream>
//#include <string>
//
//using namespace decision_tree;
//using namespace layers;
//using namespace tree_layout;
//using std::make_shared;
//using std::ifstream;
//using std::ofstream;
//using std::cerr;
//using std::endl;
//using std::string;
//using std::stringstream;

using namespace utilities;

//
//
//// A struct of parameters
//struct app_params
//{
//    int numIter = 0;
//    double thresh = 0.0;
//};
//
//// A subclass of your parameter struct that knows how to Add its members to the commandline parser
//struct parsed_params : app_params, utilities::ParsedArgSet
//{
//    parsed_params(utilities::CommandLineParser& parser)
//    {
//        AddArgs(parser);
//    }
//
//    virtual void AddArgs(utilities::CommandLineParser& parser)
//    {
//        parser.AddDocumentationString("---- Iteration-related parameters ----"); // optional documentation string that gets printed out when you use the --help option
//        parser.AddOption(numIter, "numIter", "i", "Number of iterations", 1);
//        parser.AddOption(thresh, "thresh", "t", "Threshold", 0.01);
//    }
//};
//

// Test of ParsedArgSet
/// A struct that holds command line parameters for loading maps
///
struct TestArguments
{
	string outputDataFile = "";
	bool outputDataFileHasWeights = false;
};

/// A version of DataSaveArguments that adds its members to the command line parser
///
struct ParsedTestArguments : public TestArguments, public ParsedArgSet
{
	/// Ctor
	///
	ParsedTestArguments(CommandLineParser& parser) : ParsedArgSet(parser)
	{
		AddArgs(parser);
	}

	/// Adds the arguments to the command line parser
	///
	virtual void AddArgs(CommandLineParser& parser)
	{
		parser.AddOption(
			outputDataFile,
			"outputDataFile",
			"odf",
			"Path to the output data file",
			"");

		parser.AddOption(
			outputDataFileHasWeights,
			"outputDataFileHasWeights",
			"odfhw",
			"Indicates whether the output data file format specifies a weight per example",
			false);
	}

	virtual bool PostProcess(CommandLineParser& parser)
	{
		std::cout << "Got ParsedTestArguments PostProcess callback" << std::endl;
		std::cout << "outputDataFile = " << outputDataFile << std::endl;
		std::cout << "outputDataFileHasWeights = " << outputDataFileHasWeights << std::endl;
		return true;
	}
};

bool postParseCallback(utilities::CommandLineParser& parser)
{
    std::cout << "Standalone callback called" << std::endl;

    // return parser.HasOption("a");

    return true;
}

int main(int argc, char* argv[])
{
    CommandLineParser cmdline(argc, argv);

//    // Add plain variables to the parser
    bool print_help;
    string filepath;
    cmdline.AddDocumentationString("---- General app parameters ----");
    cmdline.AddOption(filepath, "filepath", "f", "Output filepath", "");
    cmdline.AddOption(print_help, "help", "", "Print help and exit", "");

	// add parsed arg set
	ParsedTestArguments testArgs(cmdline);

//    // Here is a convenient way to use a struct of parameters:
//    parsed_params Params(cmdline);
//
//    if (print_help)
//    {
//        cmdline.PrintUsage(cout);
//        return 0;
//    }
//

    cmdline.AddPostParseCallback(postParseCallback);

    // Now actually parse the arguments and set the corresponding parameter values
    cmdline.ParseArgs();

	
	
	//
//    cout << "numIter: " << Params.numIter << endl;
//    cout << "thresh: " << Params.thresh << endl;
//
//    if (filepath == "")
//    {
//        cout << "Error: filepath not set!" << endl;
//        throw std::runtime_error("filepath not set");
//    }
//
//    cout << "filepath: " << filepath << endl;
//
//
//
//    // open file
//    ofstream fs(filepath);
//
//    // check that it opened
//    if (fs.bad())
//    {
//        cerr << "error writing to file " << filepath << endl;
//        return 1;
//    }
//
//    try
//    {
//        MutableDecisionTree tree;
//        tree.SplitLeaf(0, 0, -11, 11, 13);
//        tree.SplitLeaf(1, 0, -21, 21, 23);
//        tree.SplitLeaf(2, 0, -31, 31, 33);
//        tree.SplitLeaf(3, 0, -41, 41, 43);
//        tree.SplitLeaf(4, 0, -41, 41, 43);
//        tree.SplitLeaf(5, 0, -41, 41, 43);
//        tree.SplitLeaf(6, 0, -41, 41, 43);
//
//        DecisionTreePath path(tree.ChildrenBegin(), tree.ChildrenEnd(), 0);
//        //Scale sc(tree.vertex_begin(), tree.vertex_end());
//
//        QpLayoutGenerator g;
//        auto l = g.generate(path.Children());
//
//        auto col = make_shared<Column>();
//
//        auto L1 = make_shared<Row>();
//        L1->PushBack(make_shared<layers::Constant>(.1));
//        L1->PushBack(make_shared<layers::Constant>(.2));
//        L1->PushBack(make_shared<layers::Constant>(.3));
//        L1->PushBack(make_shared<layers::Constant>(.4));
//        col->PushBack(L1);
//
//        Io::Write(fs, col);
//    }
//    catch (runtime_error e)
//    {
//        cerr << "runtime error: " << e.what() << std::endl;
//    }
//
    return 0;
}

//
//#include "deserializer.h"
//void layers::Deserialize(JsonSerializer& serializer, std::shared_ptr<Mapping>& up)
//{
//    layers::DefaultDeserialize(serializer, up);
//}
//
//
//
