#define BOOST_TEST_LOG_LEVEL test_suite
#define BOOST_TEST_MODULE test_main
#include <boost/test/unit_test.hpp>
#include <boost/mpl/assert.hpp>

#include "bulk.h"
#include "writers.h"

using Commands = std::vector<std::string>;

BOOST_AUTO_TEST_SUITE(test_writers)

    BOOST_AUTO_TEST_CASE(print_console)
    {
        std::stringbuf out_buffer;
        std::ostream out_stream(&out_buffer);
        ConsoleWriter writer(out_stream);
        auto commands = std::make_shared<Commands>(Commands{"cmd1", "cmd2"});
        BOOST_CHECK_NO_THROW(writer.update(commands));
        BOOST_CHECK_NO_THROW(writer.print());
        BOOST_CHECK_EQUAL(out_buffer.str(),"bulk: cmd1, cmd2\n");
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(print_file)
    {
        std::string name{"test_file.txt"};
        FileWriter writer(name);
        auto commands = std::make_shared<Commands>(Commands{"cmd1", "cmd2"});
        BOOST_CHECK_NO_THROW(writer.update(commands));
        BOOST_CHECK_NO_THROW(writer.print());
        std::ifstream file{name};
        std::stringstream string_stream;
        string_stream << file.rdbuf();
        BOOST_CHECK_EQUAL(string_stream.str(),"bulk: cmd1, cmd2");
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(delete_commands_console)
    {
        std::stringbuf out_buffer;
        std::ostream out_stream(&out_buffer);
        ConsoleWriter writer(out_stream);
        {
            auto commands = std::make_shared<Commands>(Commands{"cmd1", "cmd2"});
            writer.update(commands);
        }
        BOOST_CHECK_THROW(writer.print(),std::exception);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(delete_commands_file)
    {
        FileWriter writer("test_file.txt");
        std::shared_ptr<Commands> commands;
        BOOST_CHECK_THROW(writer.update(commands),std::exception);
        BOOST_CHECK_THROW(writer.print(),std::exception);
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

        BOOST_CHECK_EQUAL(out_buffer.str(),"bulk: cmd1, cmd2\n");
        BOOST_CHECK_EQUAL(string_stream.str(),"bulk: cmd1, cmd2");
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(print_blocks)
    {
        std::stringbuf out_buffer;
        std::ostream out_stream(&out_buffer);
        std::string name{"test_file.txt"};

        Handler handler;
        ConsoleWriter consoleWriter(out_stream, &handler);
        FileWriter fileWriter(name, &handler);
        handler.setN(5);
        handler.addCommand("cmd1");
        handler.addCommand("cmd2");
        handler.addCommand("{");
        handler.addCommand("cmd3");
        handler.addCommand("cmd4");
        handler.addCommand("}");

        std::ifstream file{name};
        std::stringstream string_stream;
        string_stream << file.rdbuf();

        BOOST_CHECK_EQUAL(out_buffer.str(),"bulk: cmd3, cmd4\n");
        BOOST_CHECK_EQUAL(string_stream.str(),"bulk: cmd3, cmd4");
    }

BOOST_AUTO_TEST_SUITE_END()

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE(test_tusk)

    BOOST_AUTO_TEST_CASE(example_1)
    {
        std::stringbuf out_buffer;
        std::ostream out_stream(&out_buffer);
        std::string name{"test_file.txt"};

        Handler handler;
        ConsoleWriter consoleWriter(out_stream, &handler);
        FileWriter fileWriter(name, &handler);
        handler.setN(3);
        handler.addCommand("cmd1");
        handler.addCommand("cmd2");
        handler.addCommand("cmd3");

        std::ifstream file{name};
        std::stringstream string_stream;
        string_stream << file.rdbuf();
        file.close();

        BOOST_CHECK_EQUAL(out_buffer.str(),"bulk: cmd1, cmd2, cmd3\n");
        BOOST_CHECK_EQUAL(string_stream.str(),"bulk: cmd1, cmd2, cmd3");

        out_buffer.str("");
        string_stream.str("");

        handler.addCommand("cmd4");
        handler.addCommand("cmd5");
        handler.stop();

        file.open(name);
        string_stream << file.rdbuf();
        file.close();

        BOOST_CHECK_EQUAL(out_buffer.str(),"bulk: cmd4, cmd5\n");
        BOOST_CHECK_EQUAL(string_stream.str(),"bulk: cmd4, cmd5");
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(example_2)
    {
        std::stringbuf out_buffer;
        std::ostream out_stream(&out_buffer);
        std::string name{"test_file.txt"};

        Handler handler;
        ConsoleWriter consoleWriter(out_stream, &handler);
        FileWriter fileWriter(name, &handler);
        handler.setN(3);
        handler.addCommand("cmd1");
        handler.addCommand("cmd2");
        handler.addCommand("cmd3");

        std::ifstream file{name};
        std::stringstream string_stream;
        string_stream << file.rdbuf();
        file.close();

        BOOST_CHECK_EQUAL(out_buffer.str(),"bulk: cmd1, cmd2, cmd3\n");
        BOOST_CHECK_EQUAL(string_stream.str(),"bulk: cmd1, cmd2, cmd3");

        out_buffer.str("");
        string_stream.str("");

        handler.addCommand("{");
        handler.addCommand("cmd4");
        handler.addCommand("cmd5");
        handler.addCommand("cmd6");
        handler.addCommand("cmd7");
        handler.addCommand("}");

        file.open(name);
        string_stream << file.rdbuf();
        file.close();

        BOOST_CHECK_EQUAL(out_buffer.str(),"bulk: cmd4, cmd5, cmd6, cmd7\n");
        BOOST_CHECK_EQUAL(string_stream.str(),"bulk: cmd4, cmd5, cmd6, cmd7");
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(example_3)
    {
        std::stringbuf out_buffer;
        std::ostream out_stream(&out_buffer);
        std::string name{"test_file.txt"};

        Handler handler;
        ConsoleWriter consoleWriter(out_stream, &handler);
        FileWriter fileWriter(name, &handler);
        handler.setN(1);
        handler.addCommand("{");
        handler.addCommand("cmd1");
        handler.addCommand("cmd2");
        handler.addCommand("{");
        handler.addCommand("cmd3");
        handler.addCommand("cmd4");
        handler.addCommand("}");
        handler.addCommand("cmd5");
        handler.addCommand("cmd6");
        handler.addCommand("}");

        std::ifstream file{name};
        std::stringstream string_stream;
        string_stream << file.rdbuf();
        file.close();

        BOOST_CHECK_EQUAL(out_buffer.str(),"bulk: cmd1, cmd2, cmd3, cmd4, cmd5, cmd6\n");
        BOOST_CHECK_EQUAL(string_stream.str(),"bulk: cmd1, cmd2, cmd3, cmd4, cmd5, cmd6");
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(example_4)
    {
        std::stringbuf out_buffer;
        std::ostream out_stream(&out_buffer);
        std::string name{"test_file.txt"};

        Handler handler;
        ConsoleWriter consoleWriter(out_stream, &handler);
        FileWriter fileWriter(name, &handler);
        handler.setN(3);
        handler.addCommand("cmd1");
        handler.addCommand("cmd2");
        handler.addCommand("cmd3");
        handler.addCommand("{");
        handler.addCommand("cmd4");
        handler.addCommand("cmd5");
        handler.addCommand("cmd6");
        handler.addCommand("cmd7");
        handler.stop();

        std::ifstream file{name};
        std::stringstream string_stream;
        string_stream << file.rdbuf();
        file.close();

        BOOST_CHECK_EQUAL(out_buffer.str(),"bulk: cmd1, cmd2, cmd3\n");
        BOOST_CHECK_EQUAL(string_stream.str(),"bulk: cmd1, cmd2, cmd3");
    }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(extendeds_tests)

    BOOST_AUTO_TEST_CASE(delete_writer)
    {
        std::stringbuf out_buffer;
        std::ostream out_stream(&out_buffer);
        std::string name{"test_file.txt"};
        std::fstream file{name};
        file.open(name);
        file.close();

        Handler handler;
        {
            FileWriter fileWriter(name, &handler);
        }
        ConsoleWriter consoleWriter(out_stream, &handler);
        
        handler.setN(1);
        handler.addCommand("cmd1");
        handler.stop();

        std::stringstream string_stream;
        string_stream << file.rdbuf();
        file.close();

        BOOST_CHECK_EQUAL(out_buffer.str(),"bulk: cmd1\n");
        BOOST_CHECK_EQUAL(string_stream.str(),"");
    }

BOOST_AUTO_TEST_SUITE_END()