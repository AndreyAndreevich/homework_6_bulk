#define BOOST_TEST_LOG_LEVEL test_suite
#define BOOST_TEST_MODULE test_main
#include <boost/test/unit_test.hpp>
#include <boost/mpl/assert.hpp>

#include "bulk.h"

BOOST_AUTO_TEST_SUITE(test_writers)

    BOOST_AUTO_TEST_CASE(print_console)
    {
        std::stringbuf out_buffer;
        std::ostream out_stream(&out_buffer);
        ConsoleWriter writer(out_stream);
        std::vector<std::string> commands = {"cmd1", "cmd2"};
        writer.update(commands);
        writer.print();
        BOOST_CHECK_EQUAL(out_buffer.str(),"bulk: cmd1, cmd2");
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(print_file)
    {
        std::string name{"test_file.txt"};
        FileWriter writer(name);
        std::vector<std::string> commands = {"cmd1", "cmd2"};
        writer.update(commands);
        writer.print();
        std::ifstream file{name};
        std::stringstream string_stream;
        string_stream << file.rdbuf();
        BOOST_CHECK_EQUAL(string_stream.str(),"bulk: cmd1, cmd2");
    }

BOOST_AUTO_TEST_SUITE_END()

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE(test_handler)

    BOOST_AUTO_TEST_CASE(print_commands)
    {
        std::stringbuf out_buffer;
        std::ostream out_stream(&out_buffer);
        std::string name{"test_file.txt"};

        Handler handler;
        ConsoleWriter consoleWriter(out_stream, &handler);
        FileWriter fileWriter(name, &handler);
        handler.setN(2);
        handler.addCommand("cmd1");
        handler.addCommand("cmd2");

        std::ifstream file{name};
        std::stringstream string_stream;
        string_stream << file.rdbuf();

        BOOST_CHECK_EQUAL(out_buffer.str(),"bulk: cmd1, cmd2");
        BOOST_CHECK_EQUAL(string_stream.str(),"bulk: cmd1, cmd2");
    }

////////////////////////////////////////////////////////////////////////////////////////////////
/*
    BOOST_AUTO_TEST_CASE(print_blocks)
    {
        std::stringbuf out_buffer;
        std::ostream out_stream(&out_buffer);
        std::string name{"test_file.txt"};

        Handler handler;
        handler.addWriter(new ConsoleWriter(out_stream));
        handler.addWriter(new FileWriter(name));
        handler.setN(2);
        handler.addCommand("cmd1");
        handler.addCommand("cmd2");

        std::ifstream file{name};
        std::stringstream string_stream;
        string_stream << file.rdbuf();

        BOOST_CHECK_EQUAL(out_buffer.str(),"bulk: cmd1, cmd2");
        BOOST_CHECK_EQUAL(string_stream.str(),"bulk: cmd1, cmd2");
    }
*/
////////////////////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE_END()