#include "WorkerQueue.h"
#include "socket/Server.h"
#include "logger/log.h"
#include "osrm/osrm.hpp"
#include "util/coordinate.hpp"
#include "osrm/json_container.hpp"
#include "util/json_renderer.hpp"


#include <assert.h>


/// <summary> Constructors a new work queue object. </summary>
/// <param name="numWorkers"> (Optional) number of workers, less than 1 to
///     auto-detect (may fail on esoteric platforms). </param>
WorkQueue::WorkQueue(EngineConfig& engineConfig, int numWorkers)
{
    if (numWorkers < 1)
    {
        numWorkers = std::thread::hardware_concurrency() + 1;
    }

    LOG_DEBUG("Starting %d workers.",numWorkers);

    while (numWorkers--)
    {
        m_workers.emplace_back(std::thread(&WorkQueue::doWork, this));
    }

    this->osrmEngine = new osrm::OSRM(engineConfig);
}

/// <summary> Will abort all pending jobs and run any in-progress jobs to
///     completion upon destruction. </summary>
WorkQueue::~WorkQueue()
{
    abort();
    delete osrmEngine;
}

/// <summary> Stops work queue and finishes jobs currently being executed.
///     Queued jobs that have not begun will be dropped. </summary>
void WorkQueue::abort()
{
    LOG_DEBUG("Aborting workers...");

    m_exit = true;
    m_finish_work = false;
    m_signal.notify_all();
    joinAll();

    {
        std::lock_guard<std::mutex> lg(m_mutex);
        m_work.clear();
    }
}

/// <summary> Stops new work from being submitted to this work queue.</summary>
void WorkQueue::stop()
{
    m_exit = true;
    m_finish_work = true;
    m_signal.notify_all();
}

/// <summary> Wait for completion of all submitted work. No more work will
///     be allowed to be submitted. </summary>
void WorkQueue::waitForCompletion()
{
    stop();
    joinAll();
    assert(m_work.empty());
}

/// <summary> Queues up a client to deal with</summary>
/// <exception cref="std::runtime_error"> Thrown if attempting to submit a job
///     to a work queue that is terminating. </exception>
void WorkQueue::submit(int client)
{
    LOG_DEBUG("Client %d pushed to queue.", client);

    if (m_exit)
    {
        throw std::runtime_error("Caught work submission to work queue that is desisting.");
    }

    {
        std::lock_guard<std::mutex> lg(m_mutex);
        m_work.emplace_back(client);
    }

    m_signal.notify_one();
}



void WorkQueue::doWork()
{
    std::unique_lock<std::mutex> ul(m_mutex);
    std::thread::id this_id = std::this_thread::get_id();
    while (!m_exit || (m_finish_work && !m_work.empty()))
    {
        if (!m_work.empty())
        {
            int client = m_work.front();
            m_work.pop_front();
            ul.unlock();
            LOG_DEBUG("Worker %d is going to work with client %d", this_id, client);
            handleClient(client);
            ul.lock();
        }
        else
        {
            LOG_DEBUG("Worker %d is going to sleep.", this_id);
            m_signal.wait(ul);
        }
    }
}

void WorkQueue::joinAll()
{
    for (auto& thread : m_workers)
    {
        thread.join();
    }
    m_workers.clear();
}

void WorkQueue::handleClient(int client)
{
    //get a request
    std::string request = Server::get_request(client);

    if (!request.empty())
    {
        RouteParameters route_parameters;

        ApiGrammarParser api_parser(&route_parameters);

        auto api_iterator = request.begin();
        boost::spirit::qi::parse(api_iterator, request.end(), api_parser);

        osrm::json::Object json_result;


        const int result_code = osrmEngine->RunQuery(route_parameters, json_result);

        std::vector<char> response;
        osrm::json::render(response, json_result);

        //send response
        Server::send_response(client,&*response.begin());
    }

    close(client);
}
