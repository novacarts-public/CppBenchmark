/*!
    \file macros.h
    \brief Macros definitions
    \author Ivan Shynkarenka
    \date 16.07.2015
    \copyright MIT License
*/

#ifndef CPPBENCHMARK_MACROS_H
#define CPPBENCHMARK_MACROS_H

#include "launcher_console.h"

/*!
    \namespace CppBenchmark
    \brief CppBenchmark project definitions
*/
namespace CppBenchmark {

//! @cond
namespace Internals {

class BenchmarkRegistrator
{
public:
    explicit BenchmarkRegistrator(std::shared_ptr<Benchmark> benchmark)
    { LauncherConsole::GetInstance().AddBenchmark(benchmark); }
};

} // namespace Internals
//! @endcond

} // namespace CppBenchmark

//! @cond
#define BENCHMARK_INTERNAL_UNIQUE_NAME_LINE2(name, line) name##line
#define BENCHMARK_INTERNAL_UNIQUE_NAME_LINE(name, line) BENCHMARK_INTERNAL_UNIQUE_NAME_LINE2(name, line)
#define BENCHMARK_INTERNAL_UNIQUE_NAME(name) BENCHMARK_INTERNAL_UNIQUE_NAME_LINE(name, __LINE__)
//! @endcond

//! Benchmark main entry point macro
/*!
    Main entry point definition for all benchmarks. Place this macro in some .cpp file to provide a main() function
    with registered LauncherConsole with \a argc & \a argv arguments parsing.
*/
#define BENCHMARK_MAIN()\
int main(int argc, char** argv)\
{\
    CppBenchmark::LauncherConsole::GetInstance().Initialize(argc, argv);\
    CppBenchmark::LauncherConsole::GetInstance().Launch();\
    CppBenchmark::LauncherConsole::GetInstance().Report();\
    return 0;\
}

//! Benchmark register macro
/*!
    Register a new benchmark with a given \a name and settings. Next to the definition you should provide a benchmark
    code.

    Example:
    \code{.cpp}
    // This benchmark will call MyTest() function in 1000000 iterations
    BENCHMARK("MyTestBenchmark", 1000000)
    {
        MyTest();
    }
    \endcode
*/
#define BENCHMARK(name, ...)\
namespace CppBenchmark {\
    class BENCHMARK_INTERNAL_UNIQUE_NAME(__benchmark__) : public Benchmark\
    {\
    public:\
        using Benchmark::Benchmark;\
    protected:\
        void Run(Context& context) override;\
    };\
    Internals::BenchmarkRegistrator BENCHMARK_INTERNAL_UNIQUE_NAME(benchmark_registrator)(std::make_shared<BENCHMARK_INTERNAL_UNIQUE_NAME(__benchmark__)>(name, Benchmark::TSettings(__VA_ARGS__)));\
}\
void CppBenchmark::BENCHMARK_INTERNAL_UNIQUE_NAME(__benchmark__)::Run(CppBenchmark::Context& context)


//! Benchmark with fixture register macro
/*!
    Register a new benchmark with a given \a fixture, \a name and settings. Next to the definition you should provide
    a benchmark code. Benchmark fixture is a user class that will be constructed before benchmarking and destructed
    after. In benchmark code you can access to public and protected fields & methods of the fixture.

    Example:
    \code{.cpp}
    class VectorFixture
    {
    protected:
        std::vector<int> container;
    };

    // This benchmark will insert random value into std::vector<int> 1000000 times
    BENCHMARK_FIXTURE(VectorFixture, "VectorPushBackBenchmark", 1000000)
    {
        container.push_back(rand());
    }
    \endcode
*/
#define BENCHMARK_FIXTURE(fixture, name, ...)\
namespace CppBenchmark {\
    class BENCHMARK_INTERNAL_UNIQUE_NAME(__benchmark__) : public Benchmark, public fixture\
    {\
    public:\
        using Benchmark::Benchmark;\
    protected:\
        void Run(Context& context) override;\
    };\
    Internals::BenchmarkRegistrator BENCHMARK_INTERNAL_UNIQUE_NAME(benchmark_registrator)(std::make_shared<BENCHMARK_INTERNAL_UNIQUE_NAME(__benchmark__)>(name, Benchmark::TSettings(__VA_ARGS__)));\
}\
void CppBenchmark::BENCHMARK_INTERNAL_UNIQUE_NAME(__benchmark__)::Run(Context& context)

//! Benchmark threads register macro
/*!
    Register a new threads benchmark with a given \a name and settings. Next to the definition you should provide
    a benchmark code that will be executed in multi-thread environment. You can use \a settings parameter to give
    threads count to want to measure with.

    Example:
    \code{.cpp}
    // This benchmark will output random value in std::cout 1000000 times in 4 concurrent threads environment
    BENCHMARK_THREADS("ThreadsConsoleBenchmark", 1000000, 4)
    {
        std::count << rand();
    }
    \endcode
*/
#define BENCHMARK_THREADS(name, ...)\
namespace CppBenchmark {\
    class BENCHMARK_INTERNAL_UNIQUE_NAME(__benchmark__) : public BenchmarkThreads\
    {\
    public:\
        using BenchmarkThreads::BenchmarkThreads;\
    protected:\
        void RunThread(ContextThread& context) override;\
    };\
    Internals::BenchmarkRegistrator BENCHMARK_INTERNAL_UNIQUE_NAME(benchmark_registrator)(std::make_shared<BENCHMARK_INTERNAL_UNIQUE_NAME(__benchmark__)>(name, BenchmarkThreads::TSettings(__VA_ARGS__)));\
}\
void CppBenchmark::BENCHMARK_INTERNAL_UNIQUE_NAME(__benchmark__)::RunThread(CppBenchmark::ContextThread& context)

//! Benchmark threads with fixture register macro
/*!
    Register a new threads benchmark with a given \a fixture, \a name and settings. Next to the definition you should
    provide a benchmark code that will be executed in multi-thread environment. Benchmark fixture is a user class
    that will be constructed before benchmarking and destructed after. In benchmark code you can access to public
    and protected fields & methods of the fixture. You can use \a settings parameter to give threads count to want
    to measure with.

    Example:
    \code{.cpp}
    class AtomicFixture
    {
    protected:
        std::atomic<int> counter;
    };

    // This benchmark will increment atomic counter 1000000 times in 4 concurrent threads environment
    BENCHMARK_THREADS_FIXTURE(AtomicFixture, "ThreadsAtomicIncrementBenchmark", 1000000, 4)
    {
        counter++;
    }
    \endcode
*/
#define BENCHMARK_THREADS_FIXTURE(fixture, name, ...)\
namespace CppBenchmark {\
    class BENCHMARK_INTERNAL_UNIQUE_NAME(__benchmark__) : public BenchmarkThreads, public fixture\
    {\
    public:\
        using BenchmarkThreads::BenchmarkThreads;\
    protected:\
        void RunThread(ContextThread& context) override;\
    };\
    Internals::BenchmarkRegistrator BENCHMARK_INTERNAL_UNIQUE_NAME(benchmark_registrator)(std::make_shared<BENCHMARK_INTERNAL_UNIQUE_NAME(__benchmark__)>(name, BenchmarkThreads::TSettings(__VA_ARGS__)));\
}\
void CppBenchmark::BENCHMARK_INTERNAL_UNIQUE_NAME(__benchmark__)::RunThread(ContextThread& context)


//! Benchmark class register macro
/*!
    Register a new benchmark based on a child class of a \a type with a given \a name and \a settings. You should
    inherit \a type from Benchmark, BenchmarkThreads or BenchmarkPC and implement all necessary benchmark methods.

    Example:
    \code{.cpp}
    class VectorBenchmark
    {
    protected:
        std::vector<int> container;

        void Initialize(Context& context) { container.reserve(1000); }
        void Run(Context& context) override { container.push_back(rand()); }
        void Cleanup(Context& context) override { container.clear(); }
    };

    // This benchmark will measure VectorBenchmark with 1000000 iterations
    BENCHMARK_CLASS(VectorBenchmark, "VectorPushBackBenchmark", 1000000)
    \endcode
*/
#define BENCHMARK_CLASS(type, name, ...)\
namespace CppBenchmark { Internals::BenchmarkRegistrator BENCHMARK_INTERNAL_UNIQUE_NAME(benchmark_registrator)(std::make_shared<type>(name, type::TSettings(__VA_ARGS__))); }

#endif // CPPBENCHMARK_MACROS_H