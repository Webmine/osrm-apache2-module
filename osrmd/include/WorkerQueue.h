#ifndef WORKERQUEUE_H_INCLUDED
#define WORKERQUEUE_H_INCLUDED

#include <deque>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include "osrm/osrm.hpp"
#include "osrm/engine_config.hpp"
#include "osrm/route_parameters.hpp"
#include "server/api_grammar.hpp"

//template<typename Iterator, class HandlerT> struct APIGrammar;

/// <summary> A typical thread worker queue that can execute arbitrary jobs.
/// </summary>
///
class WorkQueue
{

public:
    WorkQueue(osrm::engine::EngineConfig& engineConfig, int numWorkers = -1);
    ~WorkQueue();
    void abort();
    void stop();
    void waitForCompletion();
    void submit(int client);

private:
    std::deque<int> m_work;
    std::mutex m_mutex;
    std::condition_variable m_signal;
    std::atomic<bool> m_exit{ false };
    std::atomic<bool> m_finish_work{ true };
    std::vector<std::thread> m_workers;
    osrm::OSRM* osrmEngine;
    using ApiGrammarParser = osrm::server::APIGrammar<std::string::iterator, osrm::engine::RouteParameters>;

    void doWork();
    void joinAll();
    void handleClient(int client);

    void operator=(const WorkQueue&) = delete;
    WorkQueue(const WorkQueue&) = delete;
};

#endif // WORKERQUEUE_H_INCLUDED
