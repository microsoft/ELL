// HingeLoss.cpp

#include "HingeLoss.h"


namespace loss_functions
{
    double HingeLoss::Evaluate(double prediction, double label) const
    {
        double scaled_margin = prediction * label;

        if(scaled_margin <= 1.0)
        {
            return 1.0-scaled_margin;
        }

        return 0.0;
    }

    double HingeLoss::GetDerivative(double prediction, double label) const
    {
        if(prediction * label <= 1.0)
        {
            return -label;
        }

        return 0.0;
    }
}
