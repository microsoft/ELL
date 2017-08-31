CONSTRUCTABLE_WITH_NUMPY(FloatTensor)
CONSTRUCTABLE_WITH_NUMPY(DoubleTensor)

// Additional C++ code to make Api more natural for Python callers
namespace ell
{
namespace math
{
    %extend TensorShape
    {  
        TensorShape(size_t rows, size_t columns, size_t channels) 
        {
            return new ell::math::TensorShape{rows, columns, channels};
        }
    };
}
}