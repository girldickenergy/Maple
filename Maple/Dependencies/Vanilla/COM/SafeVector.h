// minified version of https://stackoverflow.com/questions/4007496/late-binding-of-com-net-call-with-enum-value-arguments
#pragma once
#include <assert.h>
#include <stddef.h> // ptrdiff_t
#include "mscoree.h" // SAFEARRAY

typedef size_t   Size;
typedef Size     Index;

class SafeVector
{
private:
    SAFEARRAY* _descriptor;
    Index       _lowerBound;
    Index       _upperBound;

    Index lowerBound() const
    {
        if (_descriptor == 0) {
            return 0;
        }
        long result;
        SafeArrayGetLBound(_descriptor, 1, &result);
        return result;
    }

    Index upperBound() const
    {
        if (_descriptor == 0) {
            return 0;
        }
        long result;
        SafeArrayGetUBound(_descriptor, 1, &result);
        return result;
    }

public:
    SafeVector() : _descriptor(0), _lowerBound(0), _upperBound(0) {}

    explicit SafeVector(SAFEARRAY* descriptor) : _descriptor(descriptor), _lowerBound(0), _upperBound(0)
    {
        assert(_descriptor == 0 || ::SafeArrayGetDim(_descriptor) == 1);
        _lowerBound = lowerBound();
        _upperBound = upperBound();
    }

    ~SafeVector()
    {
        if (_descriptor != 0)
        {
            ::SafeArrayDestroy(_descriptor);
        }
    }

    SAFEARRAY* asSafeArray() const
    {
        return _descriptor;
    }

    Size count() const
    {
        return (_upperBound + 1) - _lowerBound;
    }

    Size elemSize() const
    {
        return (_descriptor == 0 ? 0 : ::SafeArrayGetElemsize(_descriptor));
    }

    void getElement(Index i, void* pResult) const
    {
        long internalIndex = i + _lowerBound;
        ::SafeArrayGetElement(_descriptor, &internalIndex, pResult);
    }

    void setElement(Index i, void* pData)
    {
        long internalIndex = i + _lowerBound;
        ::SafeArrayPutElement(_descriptor, &internalIndex, pData);
    }
};