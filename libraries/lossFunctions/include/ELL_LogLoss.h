class ELL_LogLoss
{
    public:
        static double Evaluate(double prediciton, double label);
        static double GetDerivative(double prediction, double label);
};

