#ifndef LOOPEVENT_H
#define LOOPEVENT_H

#include "src/noncopyable.h"

#include <functional>

namespace hhl
{
namespace net
{

class EventLoop : noncopyable
{
    public:
    typedef std::function<void()> Functor;

    //TODO
    EventLoop();
    ~EventLoop();





};

} //namespace net
} //namespace hhl



#endif // !LOOPEVENT_H