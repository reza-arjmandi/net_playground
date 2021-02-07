#pragma once

template<class BufferType>
class IWork {

public:

    virtual ~IWork() = default;
    virtual BufferType operator()(BufferType&& buffer) = 0;
};