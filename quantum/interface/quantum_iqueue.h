/*
** Copyright 2018 Bloomberg Finance L.P.
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/
#ifndef BLOOMBERG_QUANTUM_IQUEUE_H
#define BLOOMBERG_QUANTUM_IQUEUE_H

#include <quantum/quantum_spinlock.h>
#include <quantum/interface/quantum_iterminate.h>
#include <quantum/interface/quantum_itask.h>
#include <quantum/interface/quantum_iqueue_statistics.h>
#include <quantum/quantum_allocator.h>

namespace Bloomberg {
namespace quantum {

//==============================================================================================
//                                  interface IQueue
//==============================================================================================
/// @interface IQueue
/// @brief Interface to a task queue. For internal use only.
struct IQueue : public ITerminate
{
    //Typedefs and enum definitions
    using Ptr = std::shared_ptr<IQueue>;
    enum class QueueType : int { Coro, IO, All };
    enum class QueueId : int { Any = -1, Same = -2, All = -3 };
    
    //Interface methods
    virtual void pinToCore(int coreId) = 0;
    
    virtual void run() = 0;
    
    virtual void enqueue(ITask::Ptr task) = 0;
    
    virtual bool tryEnqueue(ITask::Ptr task) = 0;
    
    virtual ITask::Ptr dequeue(std::atomic_bool& hint) = 0;
    
    virtual ITask::Ptr tryDequeue(std::atomic_bool& hint) = 0;
    
    virtual size_t size() const = 0;
    
    virtual bool empty() const = 0;
    
    virtual IQueueStatistics& stats() = 0;
    
    virtual SpinLock& getLock() = 0;
    
    virtual void signalEmptyCondition(bool value) = 0;
    
    virtual bool isIdle() const = 0;
};

using IQueuePtr = IQueue::Ptr;

#ifndef __QUANTUM_USE_DEFAULT_ALLOCATOR
    #ifdef __QUANTUM_ALLOCATE_POOL_FROM_HEAP
        using QueueListAllocator = HeapAllocator<ITask::Ptr>;
        using IoQueueListAllocator = HeapAllocator<ITask::Ptr>;
    #else
        using QueueListAllocator = StackAllocator<ITask::Ptr, __QUANTUM_QUEUE_LIST_ALLOC_SIZE>;
        using IoQueueListAllocator = StackAllocator<ITask::Ptr, __QUANTUM_IO_QUEUE_LIST_ALLOC_SIZE>;
    #endif
#else
    using QueueListAllocator = StlAllocator<ITask::Ptr>;
    using IoQueueListAllocator = StlAllocator<ITask::Ptr>;
#endif

}}

#endif //BLOOMBERG_QUANTUM_IQUEUE_H
