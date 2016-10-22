////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DTWNode.tcc (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace emll
{
namespace nodes
{
    template <typename ValueType>
    DTWNode<ValueType>::DTWNode()
        : Node({ &_input }, { &_output }), _input(this, {}, inputPortName), _output(this, outputPortName, 1), _sampleDimension(0), _prototypeLength(0), _threshold(0)
    {
    }

    template <typename ValueType>
    DTWNode<ValueType>::DTWNode(const model::PortElements<ValueType>& input, const std::vector<std::vector<ValueType>>& prototype, double confidenceThreshold)
        : Node({ &_input }, { &_output }), _input(this, input, inputPortName), _output(this, outputPortName, 1), _prototype(prototype)
    {
        _sampleDimension = input.Size();
        _prototypeLength = prototype.size();
        _d.resize(_prototypeLength + 1);
        _s.resize(_prototypeLength + 1);

        std::fill(_d.begin() + 1, _d.end(), 99999.0);
        _d[0] = 0.0;

        // TODO: compute threshold from confidenceThreshold and variance of sample
        _prototypeVariance = 392.0529540761332; //this is the variance of the nextSlidePrototype
        _threshold = std::sqrt(-2 * std::log(confidenceThreshold)) * _prototypeVariance;
        Reset();
    }

    template <typename ValueType>
    DTWNode<ValueType>::DTWNode(const model::PortElements<ValueType>& input, const std::vector<std::vector<ValueType>>& prototype, double threshold, UseRawThreshold)
        : Node({ &_input }, { &_output }), _input(this, input, inputPortName), _output(this, outputPortName, 1), _prototype(prototype)
    {
        _sampleDimension = input.Size();
        _prototypeLength = prototype.size();
        _d.resize(_prototypeLength + 1);
        _s.resize(_prototypeLength + 1);

        _prototypeVariance = 392.0529540761332; //this is the variance of the nextSlidePrototype
        _threshold = threshold;
        Reset();
    }

    template <typename ValueType>
    void DTWNode<ValueType>::Reset() const
    {
        std::fill(_d.begin() + 1, _d.end(), 1e10);
        _d[0] = 0.0;
        std::fill(_s.begin(), _s.end(), 0);
        _currentTime = 0;
    }

    template <typename T>
    float distance(const std::vector<T>& a, const std::vector<T>& b)
    {
        int s = 0;
        for (int index = 0; index < a.size(); index++)
        {
            s += std::abs(a[index] - b[index]);
        }
        return s;
    }

    template <typename ValueType>
    void DTWNode<ValueType>::Compute() const
    {
        std::vector<ValueType> input = _input.GetValue();
        auto t = ++_currentTime;
        auto dLast = _d[0] = 0;
        auto sLast = _s[0] = t;

        ValueType bestDist = 0;
        int bestStart = 0;
        for (size_t index = 1; index < _prototypeLength + 1; ++index)
        {
            auto d_iMinus1 = _d[index - 1];
            auto dPrev_iMinus1 = dLast;
            auto dPrev_i = _d[index];
            auto s_iMinus1 = _s[index - 1];
            auto sPrev_iMinus1 = sLast;
            auto sPrev_i = _s[index];

            bestDist = d_iMinus1;
            bestStart = s_iMinus1;
            if (dPrev_i < bestDist)
            {
                bestDist = dPrev_i;
                bestStart = sPrev_i;
            }
            if (dPrev_iMinus1 < bestDist)
            {
                bestDist = dPrev_iMinus1;
                bestStart = sPrev_iMinus1;
            }
            bestDist += distance(_prototype[index - 1], input);
            
            _d[index] = bestDist;
            _s[index] = bestStart;
        }
        assert(bestDist == _d[_prototypeLength]);
        assert(bestStart == _s[_prototypeLength]);
        auto result = bestDist / _prototypeVariance;

        // Ensure best match is between 80% and 120% of prototype length
        auto timeDiff = _currentTime - bestStart;
        if (timeDiff < _prototypeLength * 0.8 || timeDiff > _prototypeLength * 1.2)
        {
            bestDist = 1e10;
        }

        _output.SetOutput({ static_cast<ValueType>(result) });
    };

    template <typename ValueType>
    void DTWNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newinput = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<DTWNode<ValueType>>(newinput, _prototype, _threshold, UseRawThreshold());
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    void DTWNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[inputPortName] << _input;
        archiver[outputPortName] << _output;
        // archiver["prototype"] << _prototype;
        throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
        archiver["threshold"] << _threshold;
    }

    template <typename ValueType>
    void DTWNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[inputPortName] >> _input;
        archiver[outputPortName] >> _output;
        // archiver["prototype"] >> _prototype;
        throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
        archiver["threshold"] >> _threshold;
    }
}
}

#if 0
// HOWTO:
// Call setupRecognizer(0.2) first.
// Then, call String className = recognize(sample);
// "NONE" will be returned if nothing is detected.
// sample should be a float array, in the same order you put the training data in the tool.

struct DTWInfo {
    int dim;                        // The dimension of the signal.
    int prototypeSize;              // The length of the prototype.
    float* prototype;               // The data for the prototype.

    int* s;                         // Matching array: start points.
    float* d;                       // Matching array: distances.
    int t;                          // Current time in samples.
    float variance;                 // The variance.

    float bestMatchEndsAtTDistance; // The distance of the best match that ends at t.
    int bestMatchEndsAtTStart;      // The start of the best match that ends at t.
};

float DTWDistanceFunction(int dim, float* a, float* b) {
    int s = 0;
    for(int i = 0; i < dim; i++) {
        s += abs(a[i] - b[i]);
    }
    return s;
}

void DTWInit(struct DTWInfo* DTW, int dim, float* prototype, int prototypeSize, float variance) {
    DTW->dim = dim;
    DTW->prototypeSize = prototypeSize;
    DTW->prototype = prototype;
    DTW->d = new float[prototypeSize + 1];
    DTW->s = new int[prototypeSize + 1];
    DTW->t = 0;
    DTW->variance = variance;
    DTWReset(DTW);
}

void DTWReset(struct DTWInfo* DTW) {
    for(int i = 0; i <= DTW->prototypeSize; i++) {
        DTW->d[i] = 1e10;
        DTW->s[i] = 0;
    }
    DTW->d[0] = 0;
}

void DTWFeed(struct DTWInfo* DTW, float* sample) {
    float* d = DTW->d;
    int* s = DTW->s;
    DTW->t += 1;
    d[0] = 0;
    s[0] = DTW->t;
    float dp = d[0];
    int sp = s[0];
    for(int i = 1; i <= DTW->prototypeSize; i++) {
        float dist = DTWDistanceFunction(DTW->dim, DTW->prototype + (i - 1) * DTW->dim, sample);
        float d_i_minus_1 = d[i - 1]; int s_i_minus_1 = s[i - 1];
        float d_i_p = d[i]; int s_i_p = s[i];
        float d_i_p_minus_1 = dp; int s_i_p_minus_1 = sp;
        dp = d[i];
        sp = s[i];
        if(d_i_minus_1 <= d_i_p && d_i_minus_1 <= d_i_p_minus_1) {
            d[i] = dist + d_i_minus_1;
            s[i] = s_i_minus_1;
        } else if(d_i_p <= d_i_minus_1 && d_i_p <= d_i_p_minus_1) {
            d[i] = dist + d_i_p;
            s[i] = s_i_p;
        } else {
            d[i] = dist + d_i_p_minus_1;
            s[i] = s_i_p_minus_1;
        }
    }
    DTW->bestMatchEndsAtTDistance = d[DTW->prototypeSize] / DTW->variance;
    DTW->bestMatchEndsAtTStart = s[DTW->prototypeSize];
    if(DTW->t - DTW->bestMatchEndsAtTStart > DTW->prototypeSize * 0.8 && DTW->t - DTW->bestMatchEndsAtTStart < DTW->prototypeSize * 1.2) {
    } else DTW->bestMatchEndsAtTDistance = 1e10;
}

float classPrototype1_samples[] = { -1.478520393371582, -2.1905629634857178, 12.653594017028809, -1.5795615911483765, -1.9825688600540161, 12.944308280944824, -1.669622778892517, -1.905685305595398, 13.858282089233398, -1.7169421911239624, -2.3402647972106934, 14.710609436035156, -2.116196870803833, -2.4582860469818115, 15.292011260986328, -2.355292797088623, -2.514773368835449, 16.435489654541016, -2.3453755378723145, -3.3132898807525635, 16.24806022644043, -2.1890299320220947, -4.076504707336426, 15.220948219299316, -1.7911244630813599, -4.894933223724365, 12.880328178405762, -1.3189077377319336, -5.14675235748291, 9.79120922088623, -0.7668161988258362, -5.02614164352417, 6.89097785949707, -0.3520662784576416, -4.3262410163879395, 4.216436386108398, 0.049032069742679596, -3.673246145248413, 1.7166551351547241, 0.4192008376121521, -3.1881649494171143, -0.1820717304944992, 0.7587016820907593, -2.2737557888031006, -2.3927698135375977, 0.8166244029998779, -0.5444239974021912, -4.591752529144287, 0.39829742908477783, 1.026826024055481, -7.701811790466309, -0.4309975504875183, 2.014106035232544, -9.994332313537598, -1.1065213680267334, 3.5540099143981934, -11.630337715148926, -1.6825555562973022, 5.3747429847717285, -13.578455924987793, -1.9926517009735107, 7.111701488494873, -14.365182876586914, -2.413794755935669, 6.875368118286133, -16.06263542175293, -2.508897542953491, 6.962903022766113, -18.693397521972656, -2.419985055923462, 6.492448806762695, -19.117576599121094, -2.286170244216919, 6.137541770935059, -17.710750579833984, -2.0689120292663574, 5.961248397827148, -17.31321144104004, -1.9414849281311035, 5.59035587310791, -16.615877151489258, -1.5107510089874268, 4.393791198730469, -15.776649475097656, -1.1101502180099487, 3.631516933441162, -14.74056625366211, -0.8625031113624573, 2.711045742034912, -13.14953327178955, -0.36934277415275574, 0.6873451471328735, -9.903190612792969, 0.17662441730499268, -0.7611079812049866, -6.830111503601074, 0.8628125190734863, -1.2383979558944702, -3.754117488861084, 0.9643272161483765, -2.0702743530273438, -0.037545096129179, 1.1344794034957886, -2.2818586826324463, 1.3756287097930908, 0.7208653092384338, -2.7972896099090576, 4.098924160003662, 0.4389396011829376, -2.7945480346679688, 5.816989898681641, 0.19442889094352722, -2.5286829471588135, 7.713456153869629, -0.039724987000226974, -1.7121461629867554, 8.177274703979492, -0.40596386790275574, -1.9758797883987427, 9.747163772583008, -0.5614347457885742, -2.1075141429901123, 10.684499740600586, -0.7834624648094177, -2.1738250255584717, 10.82553482055664, -1.0397993326187134, -2.0482707023620605, 12.192214012145996, -1.1435582637786865, -2.3009347915649414, 13.235583305358887, -1.3382675647735596, -2.2615256309509277, 13.312945365905762, -1.6138858795166016, -1.7415132522583008, 14.310065269470215, -1.6642853021621704, -1.9691425561904907, 14.43862533569336, -1.4077874422073364, -1.8146731853485107, 13.947094917297363, -1.328006625175476, -1.8988226652145386, 13.386384010314941, -1.3145757913589478, -1.7010194063186646, 12.503358840942383, -1.2732443809509277, -1.5617626905441284, 11.87225341796875 };
int classPrototype1_dim = 3;
int classPrototype1_length = 51;
float classPrototype1_variance = 105.06251114475319;
struct DTWInfo classPrototype1_DTW;
float classPrototype2_samples[] = { 0.6901801824569702, 3.841435194015503, -13.320639610290527, 0.5944736003875732, 4.103882789611816, -13.837116241455078, 0.26253435015678406, 4.6120476722717285, -15.257369041442871, 0.12531445920467377, 4.571020603179932, -15.524421691894531, -0.13660001754760742, 4.631524085998535, -15.998251914978027, -0.619969367980957, 4.802264213562012, -16.383901596069336, -1.2368435859680176, 5.248495578765869, -16.11681365966797, -1.1423057317733765, 3.8122928142547607, -15.079034805297852, -1.0226199626922607, 2.2924115657806396, -12.750395774841309, -0.7284725308418274, 0.5822920799255371, -8.105111122131348, -0.7463936805725098, -1.2284189462661743, -4.349471569061279, -0.959234356880188, -2.5364787578582764, -1.9603853225708008, -1.0949347019195557, -4.78711462020874, 2.2490718364715576, -1.2680988311767578, -4.6747846603393555, 3.8717377185821533, -1.8590167760849, -3.1274871826171875, 6.105289459228516, -3.0430638790130615, -2.5814521312713623, 7.981269836425781, -3.941037178039551, -1.6267807483673096, 11.519072532653809, -3.807985544204712, -2.1152238845825195, 14.149234771728516, -3.7703046798706055, -1.9509299993515015, 15.0963134765625, -3.471930980682373, -1.534948706626892, 16.31439208984375, -3.056572437286377, -2.3377795219421387, 15.190712928771973, -3.1298725605010986, -1.8577009439468384, 17.114049911499023, -3.1082401275634766, -2.202996253967285, 17.99336051940918, -2.986555337905884, -2.067692279815674, 19.017982482910156, -2.7833335399627686, -2.371936082839966, 18.535470962524414, -2.642882823944092, -2.5504701137542725, 16.906843185424805, -2.3843648433685303, -3.2296810150146484, 15.581653594970703, -2.32369065284729, -3.794437885284424, 14.492452621459961, -1.3968321084976196, -4.671698570251465, 11.737346649169922, -0.7500773668289185, -4.882292747497559, 8.637837409973145, 0.011957775801420212, -4.437305927276611, 6.252891540527344, 0.8766199946403503, -3.6816048622131348, 3.97591495513916, 1.6824501752853394, -2.9167068004608154, 1.7972434759140015, 2.3063161373138428, -2.281745433807373, 0.1588795781135559, 2.7139742374420166, -1.3684309720993042, -1.1715236902236938, 3.0286450386047363, -0.5998460650444031, -3.1696722507476807, 3.0845234394073486, 0.20139847695827484, -3.7636117935180664, 2.7677040100097656, 1.032002568244934, -4.534811973571777, 2.5058798789978027, 1.36373770236969, -6.068145275115967, 2.2814300060272217, 1.4736223220825195, -8.317057609558105, 1.9133367538452148, 2.120187759399414, -9.406639099121094, 1.604649543762207, 2.9209349155426025, -10.171515464782715, 1.1254132986068726, 3.6312873363494873, -11.220516204833984, 1.06033194065094, 4.130187511444092, -11.907069206237793, -0.09132120013237, 4.624052047729492, -13.190241813659668, -0.5957978963851929, 5.40481424331665, -14.470355033874512, -0.5790176391601562, 4.719405174255371, -13.84998893737793, 0.12609782814979553, 3.641794204711914, -12.65711498260498 };
int classPrototype2_dim = 3;
int classPrototype2_length = 48;
float classPrototype2_variance = 89.67294051836421;
struct DTWInfo classPrototype2_DTW;

void setupRecognizer(float confidenceThreshold) {
    float threshold = sqrt(-2 * log(confidenceThreshold));
    DTWInit(&classPrototype1_DTW, 3, classPrototype1_samples, classPrototype1_length, classPrototype1_variance * threshold);
    DTWInit(&classPrototype2_DTW, 3, classPrototype2_samples, classPrototype2_length, classPrototype2_variance * threshold);
}

void resetRecognizer() {
    DTWReset(&classPrototype1_DTW);
    DTWReset(&classPrototype2_DTW);
}

String recognize(float* sample) {
    DTWFeed(&classPrototype1_DTW, sample);
    DTWFeed(&classPrototype2_DTW, sample);
    String minClass = "NONE";
    float minClassScore = 1e10;
    if(classPrototype1_DTW.bestMatchEndsAtTDistance < 1 && minClassScore > classPrototype1_DTW.bestMatchEndsAtTDistance) {
        minClass = "Left";
        minClassScore = classPrototype1_DTW.bestMatchEndsAtTDistance;
    }
    if(classPrototype2_DTW.bestMatchEndsAtTDistance < 1 && minClassScore > classPrototype2_DTW.bestMatchEndsAtTDistance) {
        minClass = "Right";
        minClassScore = classPrototype2_DTW.bestMatchEndsAtTDistance;
    }
    if(minClassScore < 1) {
        resetRecognizer();
    }
    return minClass;
}
#endif