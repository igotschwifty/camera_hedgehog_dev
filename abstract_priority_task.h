//
// Created by soc2 on 6/16/23.
//

#ifndef ABSTRACTPRIORITYTASK_H
#define ABSTRACTPRIORITYTASK_H

#include <unordered_set>
#include <hedgehog/hedgehog.h>
#include <pthread.h>
#include <sched.h>

/*
struct ComparePriorityTask {
    template<class TaskOutput, class ... TaskInputs>
    bool operator() (hh::AbstractPriorityTask<TaskOutput, class ... TaskInputs> const& task1, AbstractPriorityTask const& task2) {
        return task1.GetPriority() > task2.GetPriority();
    }
};
*/



namespace hh {
    enum class Policy {
        SchedFIFO = SCHED_FIFO,
        SchedRR = SCHED_RR,
        SchedOther = SCHED_OTHER,
    };

    template<size_t Separator, class ...TaskInputs>
    class AbstractPriorityTask : public AbstractTask<Separator,TaskInputs ...> {

    public:
        // don't think we need setters unlesss there's a reason to use it?
        AbstractPriorityTask( Policy policy, const struct sched_param *param) :
                AbstractTask<Separator, TaskInputs...>(){
            SetSchedule(policy, param);
        }
        explicit AbstractPriorityTask(std::string_view const &name, Policy policy, const struct sched_param *param):
                AbstractTask<Separator, TaskInputs...>(name) {
            SetSchedule(policy, param);
        }

        explicit AbstractPriorityTask(std::basic_string<char> name, size_t numberThreads, Policy policy, const struct sched_param *param) :
                AbstractTask<Separator, TaskInputs...>(name, numberThreads){
            SetSchedule(policy, param);
        }

        explicit AbstractPriorityTask(std::basic_string<char> name, size_t numberThreads, Policy policy, const struct sched_param *param, bool const automaticStart = false):
                AbstractTask<Separator, TaskInputs...>(name, numberThreads, automaticStart){
            SetSchedule(policy, param);
        }


        explicit AbstractPriorityTask(std::shared_ptr<core::CoreTask<1, TaskInputs...>> taskCore,  Policy policy, const struct sched_param *param) :
                AbstractTask<Separator, TaskInputs...>(taskCore) {
            SetSchedule(policy, param);
        }

        explicit AbstractPriorityTask(AbstractTask<Separator, TaskInputs ...> *rhs) :
                AbstractTask<Separator, TaskInputs...>(*rhs) {
            SetSchedule(rhs->policy_, rhs->param_);
        }

// note to self - check if need to do something about the virtual destructor in parent class
        ~AbstractPriorityTask() = default;

        Policy GetPolicy() { return policy_; }

        const struct sched_param* GetPriority() { return param_; }

        //policy + prioirty levek

        void SetSchedule(Policy policy, const struct sched_param *param) {
            param_ = param;
            policy_ = policy;
        }
        // testing

        virtual std::shared_ptr<AbstractTask<Separator, TaskInputs...>> copy() { return nullptr; }

        virtual void initialize() {
            int int_policy = (int) policy_;
            int thread_check = pthread_setschedparam(pthread_self(),int_policy,param_);

            //testing
            if (thread_check) {
                std::cerr << "Failed to set thread scheduling: " << std::strerror(errno) <<std::endl;
            }
            /* testing
            struct sched_param param;
            int policy = 0;
            sched_getparam(0, &param);
            std::cout << "Priority of this process: " << param.sched_priority << std::endl;

            pthread_getschedparam(pthread_self(), &policy, &param);

            std::cout << "Priority of the thread: " <<
                      param.sched_priority << " Current policy:" << policy <<std::endl;
            */

            initialize_prio_task();
        }

        virtual void initialize_prio_task() {

        }

    private:
        Policy policy_;
        const struct sched_param* param_;
    };


}



#endif


