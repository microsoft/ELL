////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SignPredictor.h (predictors)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <utilities/include/IArchivable.h>

#include <memory>

namespace ell
{
namespace predictors
{
    /// <summary> Wraps a predictor, typically an IPredictor(double), and exposes an IPredictor(bool)
    /// whose predictions are the sign bit of the original predictions. </summary>
    ///
    /// <typeparam name="PredictorType"> The internal predictor type. </typeparam>
    template <typename PredictorType>
    class SignPredictor : IPredictor<bool>
        , public utilities::IArchivable
    {
    public:
        /// <summary> Type of the data vector expected by this predictor type. </summary>
        using DataVectorType = typename PredictorType::DataVectorType;

        SignPredictor() = default;

        /// <summary> Constructs an instance of SignPredictor from an internal predictor. </summary>
        ///
        /// <param name="predictor"> The internal predictor. </param>
        SignPredictor(PredictorType predictor);

        SignPredictor(SignPredictor&&) = default;

        SignPredictor(const SignPredictor&) = default;

        /// <summary> Gets a reference to the internal predictor. </summary>
        ///
        /// <returns> A reference to the internal predictor. </returns>
        PredictorType& GetPredictor();

        /// <summary> Gets a const reference to the internal predictor. </summary>
        ///
        /// <returns> A const reference to the internal predictor. </returns>
        const PredictorType& GetPredictor() const;

        /// <summary> Returns the output of the predictor for a given example. </summary>
        ///
        /// <param name="example"> The data vector. </param>
        ///
        /// <returns> The prediction. </returns>
        bool Predict(const DataVectorType& dataVector) const;

    protected:
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        PredictorType _predictor;
    };

    /// <summary> Makes a SignPredictor. </summary>
    ///
    /// <typeparam name="PredictorType"> Internal predictor type. </typeparam>
    /// <param name="predictor"> The internal predictor. </param>
    ///
    /// <returns> A SignPredictor </returns>
    template <typename PredictorType>
    SignPredictor<PredictorType> MakeSignPredictor(PredictorType predictor);
} // namespace predictors
} // namespace ell

#pragma region implementation

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
    SignPredictor<PredictorType>::SignPredictor(PredictorType predictor) :
        _predictor(std::move(predictor))
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
} // namespace predictors
} // namespace ell

#pragma endregion implementation
