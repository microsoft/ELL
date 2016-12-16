////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     PredictorOutputAdapter.tcc (predictors)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace predictors
{
    template <typename PredictorType>
    SignPredictor<PredictorType> MakeSignPredictor(PredictorType predictor)
    {
        return SignPredictor<PredictorType>(std::move(predictor));
    }

    template <typename PredictorType>
    SignPredictor<PredictorType>::SignPredictor(PredictorType predictor)
        : _predictor(std::move(predictor))
    {
    }

    template <typename PredictorType>
    PredictorType& SignPredictor<PredictorType>::GetPredictor()
    {
        return _predictor;
    }

    template <typename PredictorType>
    const PredictorType& SignPredictor<PredictorType>::GetPredictor() const
    {
        return _predictor;
    }

    template <typename PredictorType>
    bool SignPredictor<PredictorType>::Predict(const DataVectorType& dataVector) const
    {
        auto prediction = _predictor.Predict(dataVector);
        return prediction > 0;
    }

    template <typename PredictorType>
    void SignPredictor<PredictorType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        archiver["predictor"] << _predictor;
    }

    template <typename PredictorType>
    void SignPredictor<PredictorType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        archiver["predictor"] >> _predictor;
    }
}
}
