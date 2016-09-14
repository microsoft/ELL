////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SGDIncrementalTrainer_wrap.h (interfaces)
//  Authors:  Ofer Dekel, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef SWIG
// Just enough of the SGDIncrementalTrainer class to allow SWIG to wrap it

namespace emll
{
namespace trainers
{
    /// <summary> Parameters for the stochastic gradient descent trainer. </summary>
    struct SGDIncrementalTrainerParameters
    {
        double regularization = 1.0;
    };

    template <typename LossFunctionType>
    class SGDIncrementalTrainer
    {
    public:
        SGDIncrementalTrainer(uint64_t dim, const LossFunctionType& lossFunction, const SGDIncrementalTrainerParameters& parameters);
        virtual void Update(dataset::GenericRowDataset::Iterator& exampleIterator) override;
        virtual const std::shared_ptr<const predictors::LinearPredictor> GetPredictor() const override;
    };
}
}
#else
#endif