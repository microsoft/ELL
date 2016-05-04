////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SortingTreeLearner.tcc (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace trainers
{    
    template <typename LossFunctionType>
    SortingTreeLearner<LossFunctionType>::SortingTreeLearner(LossFunctionType lossFunction) : _lossFunction(lossFunction)
    {}

    template <typename LossFunctionType>
    template <typename ExampleIteratorType>
    predictors::DecisionTree SortingTreeLearner<LossFunctionType>::Train(ExampleIteratorType exampleIterator)
    {

        LoadData(exampleIterator);

        predictors::DecisionTree tree;

        _dataset.Sort([](const dataset::SupervisedExample<dataset::DoubleDataVector>& example) {return example.GetDataVector()[6];});

        std::cout << _dataset << std::endl;

        // TODO - replace the below with a real tree learning algo
        auto& root = tree.SplitRoot(predictors::DecisionTree::SplitRule(0, 0.0), -1.0, 1.0);
        root.GetNegativeChild().Split(predictors::DecisionTree::SplitRule(1, 1.0), -2.0, 2.0);
        root.GetPositiveChild().Split(predictors::DecisionTree::SplitRule(2, 2.0), -4.0, 4.0);

        Cleanup();

        return tree;
    }

    template<typename LossFunctionType>
    template <typename ExampleIteratorType>
    void SortingTreeLearner<LossFunctionType>::LoadData(ExampleIteratorType exampleIterator)
    {
        // create DenseRowDataset: TODO this code breaks encapsulation
        while (exampleIterator.IsValid())
        {
            const auto& example = exampleIterator.Get();
            auto denseDataVector = std::make_unique<dataset::DoubleDataVector>(example.GetDataVector().ToArray());
            auto denseSupervisedExample = dataset::SupervisedExample<dataset::DoubleDataVector>(std::move(denseDataVector), example.GetLabel(), example.GetWeight());
            _dataset.AddExample(std::move(denseSupervisedExample));
            exampleIterator.Next();
        }
    }

    template<typename LossFunctionType>
    void SortingTreeLearner<LossFunctionType>::Cleanup()
    {
        _dataset.Reset();
        while (!_queue.empty())
        {
            _queue.pop();
        }
    }
}
