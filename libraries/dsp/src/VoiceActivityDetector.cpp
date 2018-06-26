////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     VoiceActivityDetector.cpp (dsp)
//  Authors:  Chris Lovett, Ivan Tashev
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "VoiceActivityDetector.h"
#include <algorithm>
#include <cassert>

namespace ell
{
namespace dsp
{
    class ActivityTracker
    {
        double _lastLevel;
        double _lastTime;
        int _signal;

    public:
        double _tauUp;
        double _tauDown;
        double _largeInput;
        double _gainAtt;
        double _thresholdUp;
        double _thresholdDown;
        double _levelThreshold;

        ActivityTracker(
            double tauUp,
            double tauDown,
            double largeInput,
            double gainAtt,
            double thresholdUp,
            double thresholdDown,
            double levelThreshold)
            : _tauUp(tauUp), _tauDown(tauDown), _largeInput(largeInput), _gainAtt(gainAtt), _thresholdUp(thresholdUp), _thresholdDown(thresholdDown), _levelThreshold(levelThreshold)
        {
            reset();
        }

        void reset()
        {
            _lastLevel = 0.1;
            _lastTime = 0;
            _signal = 0;
        }

        /// <summary> compute the next signal state given input time and power levels </summary>
        int classify(double time, double level)
        {
            double timeDelta = time - this->_lastTime;
            double levelDelta = level - this->_lastLevel;

            if (level < this->_lastLevel)
            {
                this->_lastLevel = this->_lastLevel + timeDelta / this->_tauDown * levelDelta;
                if (this->_lastLevel < level)
                {
                    this->_lastLevel = level;
                }
            }
            else if (level > this->_largeInput * this->_lastLevel)
            {
                this->_lastLevel = this->_lastLevel + this->_gainAtt * timeDelta / this->_tauUp * levelDelta;
                if (this->_lastLevel > level)
                {
                    this->_lastLevel = level;
                }
            }
            else
            {
                this->_lastLevel = this->_lastLevel + timeDelta / this->_tauUp * levelDelta;
                if (this->_lastLevel > level)
                {
                    this->_lastLevel = level;
                }
            }

            if ((level > this->_thresholdUp * this->_lastLevel) && (level > this->_levelThreshold))
            {
                this->_signal = 1;
            }
            if (level < this->_thresholdDown * this->_lastLevel)
            {
                this->_signal = 0;
            }

            this->_lastTime = time;
            return this->_signal;
        };
    };

    /// <summary> internal use only - this class builds a message weight lookup table for given sampleRate and windowSize </summary>
    class CMessageWeights
    {
        const int _maxFreq = 4000;
        std::vector<double> _weights;

    public:
        CMessageWeights(double sampleRate, double windowSize)
        {
            this->generate(sampleRate, windowSize);
        }

        double genWeight(double freq)
        {
            static int freqMap[41] = {
                60, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100, 1200, 1300, 1400, 1500, 1600, 1700, 1800, 1900, 2000, 2100, 2200, 2300, 2400, 2500, 2600, 2700, 2800, 2900, 3000, 3100, 3200, 3300, 3400, 3500, 3600, 3700, 3800, 3900, 4000
            };
            static double msgWeights[41] = {
                -54.65, -41.71, -25.17, -16.64, -11.29, -7.55, -4.75, -2.66, -1.19, -0.32, 0.03, 0.03, -0.17, -0.44, -0.71, -0.94, -1.12, -1.24, -1.32, -1.36, -1.38, -1.39, -1.41, -1.44, -1.50, -1.60, -1.76, -1.97, -2.26, -2.62, -3.09, -3.66, -4.35, -5.18, -6.18, -7.36, -8.75, -10.36, -12.12, -13.72, -14.43
            };

            size_t tableSize = sizeof(freqMap) / sizeof(int);
            size_t f = 0;
            for (size_t i = 0; i < tableSize; ++i)
            {
                if (freq > freqMap[i])
                {
                    f = i + 1;
                }
            }
            double cmessw = 0;
            tableSize = sizeof(msgWeights) / sizeof(double);
            if (f >= tableSize)
            {
                cmessw = msgWeights[f - 1];
            }
            else if (f > 0)
            {
                cmessw = interpolate(freq, freqMap[f - 1], freqMap[f], msgWeights[f - 1], msgWeights[f]);
            }
            return cmessw;
        }

        /// <summary> generates a lookup table of size windowSize </summary>
        void generate(double sampleRate, double windowSize)
        {
            this->_weights.resize(static_cast<int>(windowSize));
            double div = sampleRate / this->_maxFreq;
            double freq_step = sampleRate / windowSize / div;
            for (int i = 0; i < windowSize; i++)
            {
                double w = this->genWeight(i * freq_step);
                if (w != 0)
                {
                    w = pow(10, w / 20);
                    this->_weights[i] = w * w;
                }
            }
        }

        /// <summary> lookup the weight for given bin number out of windowSize bins </summary>
        double getWeight(int bin)
        {
            assert(bin < this->_weights.size());
            return this->_weights[bin];
        }

        /// <summary> for x in the range [x1, x2], interpolate the corresponding value of y in the range [y1, y2]  </summary>
        double interpolate(double x, double x1, double x2, double y1, double y2)
        {
            const double epsilon = 1e-6;
            double diff = std::abs(x2 - x1);
            if (diff < epsilon)
            {
                return (y1 + y2) / 2.0;
            }
            double proportion = (x - x1) / (x2 - x1);
            return y1 + (y2 - y1) * proportion;
        }

        std::vector<double> getWeights()
        {
            return _weights;
        }
    };

    struct VoiceActivityDetectorImpl
    {
        CMessageWeights _cmw;
        ActivityTracker _tracker;
        double _frameDuration;
        double _sampleRate;
        double _windowSize;
        long long _time; 

        VoiceActivityDetectorImpl(
            double sampleRate,
            double windowSize,
            double frameDuration,
            double tauUp,
            double tauDown,
            double largeInput,
            double gainAtt,
            double thresholdUp,
            double thresholdDown,
            double levelThreshold)
            : _cmw(sampleRate, windowSize), _tracker(tauUp, tauDown, largeInput, gainAtt, thresholdUp, thresholdDown, levelThreshold)
        {
            this->_sampleRate = sampleRate;
            this->_windowSize = windowSize;
            this->_frameDuration = frameDuration;
            this->_time = 0;
        }
    };

    VoiceActivityDetector::VoiceActivityDetector()
    {
        // cannot use =default because VoiceActivityDetectorImpl is not defined to external callers.
    }

    VoiceActivityDetector::VoiceActivityDetector(
        double sampleRate, 
        double windowSize,
        double frameDuration,
        double tauUp,
        double tauDown,
        double largeInput,
        double gainAtt,
        double thresholdUp,
        double thresholdDown,
        double levelThreshold)
        : _impl(std::make_unique<VoiceActivityDetectorImpl>(sampleRate, windowSize, frameDuration, tauUp, tauDown, largeInput, gainAtt, thresholdUp, thresholdDown, levelThreshold))
    {
        reset();
    }

    VoiceActivityDetector::~VoiceActivityDetector()
    {
        // This is needed here because we are deleting a private VoiceActivityDetectorImpl object which
        // external callers don't know about.
        // The default destructor generates compile error: "can't delete an incomplete type".
    }

    void VoiceActivityDetector::reset()
    {
        _impl->_tracker.reset();
    }

    double VoiceActivityDetector::getSampleRate() const
    {
        return _impl->_sampleRate;
    }

    double VoiceActivityDetector::getWindowSize() const
    {
        return _impl->_windowSize;
    }
    
    double VoiceActivityDetector::getFrameDuration() const
    {
        return _impl->_frameDuration;
    }

    double VoiceActivityDetector::getTauUp() const
    {
        return _impl->_tracker._tauUp;
    }

    double VoiceActivityDetector::getTauDown() const
    {
        return _impl->_tracker._tauDown;
    }

    double VoiceActivityDetector::getLargeInput() const
    {
        return _impl->_tracker._largeInput;
    }

    double VoiceActivityDetector::getGainAtt() const
    {
        return _impl->_tracker._gainAtt;
    }
    
    double VoiceActivityDetector::getThresholdUp() const
    {
        return _impl->_tracker._thresholdUp;
    }

    double VoiceActivityDetector::getThresholdDown() const
    {
        return _impl->_tracker._thresholdDown;
    }

    double VoiceActivityDetector::getLevelThreshold() const
    {
        return _impl->_tracker._levelThreshold;
    }

    template <typename ValueType>
    int VoiceActivityDetector::process(const std::vector<ValueType>& data)
    {
        if (data.size() != static_cast<size_t>(_impl->_windowSize))
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "data length should match windowSize");
        }
        double level = 0;
        for (int i = 0; i < _impl->_windowSize; i++)
        {
            level += data[i] * _impl->_cmw.getWeight(i);
        }
        level = level / _impl->_windowSize;
        double t = _impl->_time++ * _impl->_frameDuration;
        int signal = _impl->_tracker.classify(t, level);
        return signal;
    }

    std::vector<double> VoiceActivityDetector::getWeights() const
    {
        return _impl->_cmw.getWeights();
    }

    bool VoiceActivityDetector::equals(const VoiceActivityDetector& other) const
    {
        if (_impl.get() == nullptr)
        {
            return other._impl.get() == nullptr;
        }
        if (other._impl.get() == nullptr)
        {
            return false;
        }
        return _impl->_windowSize == other._impl->_windowSize &&
            _impl->_sampleRate == other._impl->_sampleRate &&
            _impl->_frameDuration == other._impl->_frameDuration &&
            _impl->_tracker._tauUp == other._impl->_tracker._tauUp &&
            _impl->_tracker._tauDown == other._impl->_tracker._tauDown &&
            _impl->_tracker._largeInput == other._impl->_tracker._largeInput &&
            _impl->_tracker._gainAtt == other._impl->_tracker._gainAtt &&
            _impl->_tracker._thresholdUp == other._impl->_tracker._thresholdUp &&
            _impl->_tracker._thresholdDown == other._impl->_tracker._thresholdDown &&
            _impl->_tracker._levelThreshold == other._impl->_tracker._levelThreshold;

    }

    void VoiceActivityDetector::WriteToArchive(utilities::Archiver& archiver) const
    {
        archiver["windowSize"] << _impl->_windowSize;
        archiver["sampleRate"] << _impl->_sampleRate;
        archiver["frameDuration"] << _impl->_frameDuration;
        archiver["tauUp"] << _impl->_tracker._tauUp;
        archiver["tauDown"] << _impl->_tracker._tauDown;
        archiver["largeInput"] << _impl->_tracker._largeInput;
        archiver["gainAtt"] << _impl->_tracker._gainAtt;
        archiver["thresholdUp"] << _impl->_tracker._thresholdUp;
        archiver["thresholdDown"] << _impl->_tracker._thresholdDown;
        archiver["levelThreshold"] << _impl->_tracker._levelThreshold;
    }

    void VoiceActivityDetector::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        double windowSize;
        double sampleRate;
        double frameDuration;
        double tauUp;
        double tauDown;
        double largeInput;
        double gainAtt;
        double thresholdUp;
        double thresholdDown;
        double levelThreshold;
        archiver["windowSize"] >> windowSize;
        archiver["sampleRate"] >> sampleRate;
        archiver["frameDuration"] >> frameDuration;
        archiver["tauUp"] >> tauUp;
        archiver["tauDown"] >> tauDown;
        archiver["largeInput"] >> largeInput;
        archiver["gainAtt"] >> gainAtt;
        archiver["thresholdUp"] >> thresholdUp;
        archiver["thresholdDown"] >> thresholdDown;
        archiver["levelThreshold"] >> levelThreshold;
        _impl = std::make_unique<VoiceActivityDetectorImpl>(sampleRate, windowSize, frameDuration, tauUp, tauDown, largeInput, gainAtt, thresholdUp, thresholdDown, levelThreshold);
    }

    //
    // Explicit instantiations
    //
    template int VoiceActivityDetector::process<float>(const std::vector<float>&);
    template int VoiceActivityDetector::process<double>(const std::vector<double>&);
}
}
