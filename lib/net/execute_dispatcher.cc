#include "net/execute_dispatcher.hpp"
#include "net/co.hpp"
#include "net/execute_context.hpp"
namespace net
{

void execute_thread_dispatcher_t::dispatch()
{
    while (!co_wait_for_resume.empty())
    {
        auto exec = co_wait_for_resume.front();
        co_wait_for_resume.pop();
        auto fn = std::get<std::function<void()>>(exec);
        auto executor = std::get<execute_context_t *>(exec);
        if (cancel_contexts.find(executor) != cancel_contexts.end())
        {
            continue;
        }
        if (fn)
            executor->co->resume_with(std::move(fn));
        else
            executor->co->resume();
    }
    cancel_contexts.clear();
}

void execute_thread_dispatcher_t::add(execute_context_t *econtext, std::function<void()> func)
{
    co_wait_for_resume.emplace(econtext, std::move(func));
}

void execute_thread_dispatcher_t::cancel(execute_context_t *econtext) { cancel_contexts.insert(econtext); }
} // namespace net
