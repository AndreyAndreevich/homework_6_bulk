#define BOOST_TEST_LOG_LEVEL test_suite
#define BOOST_TEST_MODULE test_main
#include <boost/test/unit_test.hpp>
#include <boost/mpl/assert.hpp>

#include "Handler.h"
#include "Writers.h"

using Commands = std::vector<std::string>;

std::string name(const std::weak_ptr<std::time_t>& time) {
    std::stringbuf out_buffer;
    std::ostream out_stream(&out_buffer);
    out_stream << "bulk" << *time.lock() << ".log";
    return out_buffer.str();
}

BOOST_AUTO_TEST_SUITE(test_parser)

    BOOST_AUTO_TEST_CASE(start_parser)
    {
        char* argv[2];
        argv[1] = "55";
        BOOST_CHECK_THROW(start_parsing(0,argv),std::exception);
        BOOST_CHECK_THROW(start_parsing(1,argv),std::exception);
        BOOST_CHECK_EQUAL(start_parsing(2,argv),55);
        BOOST_CHECK_EQUAL(start_parsing(10,argv),55);
        argv[1] = "asd";
        BOOST_CHECK_THROW(start_parsing(2,argv),std::exception);
        argv[1] = "0";
        BOOST_CHECK_THROW(start_parsing(2,argv),std::exception);
        argv[1] = "asd4";
        BOOST_CHECK_THROW(start_parsing(2,argv),std::exception);
        argv[1] = "4fd";
        BOOST_CHECK_EQUAL(start_parsing(10,argv),4);
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(block_parser)
    {
        auto parser = std::make_unique<BlockParser>();
        BOOST_CHECK_EQUAL(parser->parsing("cmd"),BlockParser::Command);
        BOOST_CHECK_EQUAL(parser->parsing(""),BlockParser::Command);
        BOOST_CHECK_EQUAL(parser->parsing(" "),BlockParser::Command);
        BOOST_CHECK_EQUAL(parser->parsing("{"),BlockParser::StartBlock);
        BOOST_CHECK_EQUAL(parser->parsing("cmd"),BlockParser::Command);
        BOOST_CHECK_EQUAL(parser->parsing("{"),BlockParser::Empty);
        BOOST_CHECK_EQUAL(parser->parsing("cmd"),BlockParser::Command);
        BOOST_CHECK_EQUAL(parser->parsing("}"),BlockParser::Empty);
        BOOST_CHECK_EQUAL(parser->parsing("}"),BlockParser::CancelBlock);
        BOOST_CHECK_EQUAL(parser->parsing("}"),BlockParser::Command);

        BOOST_CHECK_EQUAL(parser->parsing("{cmd"),BlockParser::Command);
        BOOST_CHECK_EQUAL(parser->parsing("cmd{"),BlockParser::Command);
        BOOST_CHECK_EQUAL(parser->parsing("{cmd}"),BlockParser::Command);
        BOOST_CHECK_EQUAL(parser->parsing("{}"),BlockParser::Command);
        BOOST_CHECK_EQUAL(parser->parsing("}{"),BlockParser::Command);
    }

BOOST_AUTO_TEST_SUITE_END()

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

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
        auto time = std::make_shared<std::time_t>();
        FileWriter writer(time);
        auto commands = std::make_shared<Commands>(Commands{"cmd1"});
        BOOST_CHECK_NO_THROW(writer.update(commands));
        BOOST_CHECK_NO_THROW(writer.print());
        std::ifstream file{name(time)};
        std::stringstream string_stream;
        string_stream << file.rdbuf();
        BOOST_CHECK_EQUAL(string_stream.str(),"bulk: cmd1");
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(current_time)
    {
        auto time = std::make_shared<std::time_t>();
        FileWriter writer(time);
        auto commands = std::make_shared<Commands>(Commands{"cmd1"});
        writer.update(commands);
        writer.print();
        BOOST_CHECK_EQUAL(std::time(nullptr),*time);
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
        auto time = std::make_shared<std::time_t>();
        FileWriter writer(time);
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
        auto time = std::make_shared<std::time_t>();

        auto handler = std::make_shared<Handler>();
        auto consoleWriter = std::shared_ptr<Observer>(new ConsoleWriter(out_stream));
        auto fileWriter = std::shared_ptr<Observer>(new FileWriter(time));
        consoleWriter->subscribe(handler);
        fileWriter->subscribe(handler);
        handler->setN(2);
        handler->addCommand("cmd1");
        handler->addCommand("cmd2");

        std::ifstream file{name(time)};
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
        auto time = std::make_shared<std::time_t>();

        auto handler = std::make_shared<Handler>();
        auto consoleWriter = std::shared_ptr<Observer>(new ConsoleWriter(out_stream));
        auto fileWriter = std::shared_ptr<Observer>(new FileWriter(time));
        consoleWriter->subscribe(handler);
        fileWriter->subscribe(handler);

        handler->setN(5);
        handler->addCommand("cmd1");
        handler->addCommand("cmd2");
        handler->addCommand("{");

        std::ifstream file{name(time)};
        std::stringstream string_stream;
        string_stream << file.rdbuf();
        file.close();

        BOOST_CHECK_EQUAL(out_buffer.str(),"bulk: cmd1, cmd2\n");
        BOOST_CHECK_EQUAL(string_stream.str(),"bulk: cmd1, cmd2");

        out_buffer.str("");
        string_stream.str("");

        handler->addCommand("cmd3");
        handler->addCommand("cmd4");
        handler->addCommand("}");

        file.open(name(time));
        string_stream << file.rdbuf();
        file.close();

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
        auto time = std::make_shared<std::time_t>();

        auto handler = std::make_shared<Handler>();
        auto consoleWriter = std::shared_ptr<Observer>(new ConsoleWriter(out_stream));
        auto fileWriter = std::shared_ptr<Observer>(new FileWriter(time));
        consoleWriter->subscribe(handler);
        fileWriter->subscribe(handler);

        handler->setN(3);
        handler->addCommand("cmd1");
        handler->addCommand("cmd2");
        handler->addCommand("cmd3");

        std::ifstream file{name(time)};
        std::stringstream string_stream;
        string_stream << file.rdbuf();
        file.close();

        BOOST_CHECK_EQUAL(out_buffer.str(),"bulk: cmd1, cmd2, cmd3\n");
        BOOST_CHECK_EQUAL(string_stream.str(),"bulk: cmd1, cmd2, cmd3");

        out_buffer.str("");
        string_stream.str("");

        handler->addCommand("cmd4");
        handler->addCommand("cmd5");
        handler->stop();

        file.open(name(time));
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
        auto time = std::make_shared<std::time_t>();

        auto handler = std::make_shared<Handler>();
        auto consoleWriter = std::shared_ptr<Observer>(new ConsoleWriter(out_stream));
        auto fileWriter = std::shared_ptr<Observer>(new FileWriter(time));
        consoleWriter->subscribe(handler);
        fileWriter->subscribe(handler);
        
        handler->setN(3);
        handler->addCommand("cmd1");
        handler->addCommand("cmd2");
        handler->addCommand("cmd3");

        std::ifstream file{name(time)};
        std::stringstream string_stream;
        string_stream << file.rdbuf();
        file.close();

        BOOST_CHECK_EQUAL(out_buffer.str(),"bulk: cmd1, cmd2, cmd3\n");
        BOOST_CHECK_EQUAL(string_stream.str(),"bulk: cmd1, cmd2, cmd3");

        out_buffer.str("");
        string_stream.str("");

        handler->addCommand("{");
        handler->addCommand("cmd4");
        handler->addCommand("cmd5");
        handler->addCommand("cmd6");
        handler->addCommand("cmd7");
        handler->addCommand("}");

        file.open(name(time));
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
        auto time = std::make_shared<std::time_t>();

        auto handler = std::make_shared<Handler>();
        auto consoleWriter = std::shared_ptr<Observer>(new ConsoleWriter(out_stream));
        auto fileWriter = std::shared_ptr<Observer>(new FileWriter(time));
        consoleWriter->subscribe(handler);
        fileWriter->subscribe(handler);

        handler->setN(1);
        handler->addCommand("{");
        handler->addCommand("cmd1");
        handler->addCommand("cmd2");
        handler->addCommand("{");
        handler->addCommand("cmd3");
        handler->addCommand("cmd4");
        handler->addCommand("}");
        handler->addCommand("cmd5");
        handler->addCommand("cmd6");
        handler->addCommand("}");

        std::ifstream file{name(time)};
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
        auto time = std::make_shared<std::time_t>();

        auto handler = std::make_shared<Handler>();
        auto consoleWriter = std::shared_ptr<Observer>(new ConsoleWriter(out_stream));
        auto fileWriter = std::shared_ptr<Observer>(new FileWriter(time));
        consoleWriter->subscribe(handler);
        fileWriter->subscribe(handler);

        handler->setN(4);
        handler->addCommand("cmd1");
        handler->addCommand("cmd2");
        handler->addCommand("cmd3");
        handler->addCommand("{");
        handler->addCommand("cmd4");
        handler->addCommand("cmd5");
        handler->addCommand("cmd6");
        handler->addCommand("cmd7");
        handler->stop();

        std::ifstream file{name(time)};
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
        auto time = std::make_shared<std::time_t>();

        auto handler = std::make_shared<Handler>();
        {
            auto fileWriter = std::shared_ptr<Observer>(new FileWriter(time));
            fileWriter->subscribe(handler);
        }
        auto consoleWriter = std::shared_ptr<Observer>(new ConsoleWriter(out_stream));
        consoleWriter->subscribe(handler);
        
        handler->setN(1);
        handler->addCommand("cmd1");
        handler->stop();

        std::fstream file{name(time)};
        std::stringstream string_stream;
        string_stream << file.rdbuf();
        file.close();

        BOOST_CHECK_EQUAL(out_buffer.str(),"bulk: cmd1\n");
        BOOST_CHECK_EQUAL(string_stream.str(),"");
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(large_string)
    {
        Handler handler;
        handler.setN(4);
        BOOST_CHECK_THROW(handler.addCommand("123456789012345678901234567890123456789012345678901"),
                            std::exception);
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(error_set_N)
    {
        Handler handler;
        BOOST_CHECK_THROW(handler.addCommand("cmd1"),std::exception);
        handler.setN(5);
        handler.addCommand("cmd1");
        handler.addCommand("cmd2");
        handler.addCommand("cmd3");
        handler.addCommand("cmd4");
        BOOST_CHECK_THROW(handler.setN(3),std::exception);
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(empty_string)
    {
        std::stringbuf out_buffer;
        std::ostream out_stream(&out_buffer);
        auto time = std::make_shared<std::time_t>();

        auto handler = std::make_shared<Handler>();
        auto consoleWriter = std::shared_ptr<Observer>(new ConsoleWriter(out_stream));
        auto fileWriter = std::shared_ptr<Observer>(new FileWriter(time));
        consoleWriter->subscribe(handler);
        fileWriter->subscribe(handler);
        
        handler->setN(2);
        handler->addCommand("");
        handler->addCommand("");
        handler->stop();

        std::fstream file;
        file.open(name(time));
        std::stringstream string_stream;
        string_stream << file.rdbuf();
        file.close();

        BOOST_CHECK_EQUAL(out_buffer.str(),"bulk: , \n");
        BOOST_CHECK_EQUAL(out_buffer.str(),"bulk: , \n"); 
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(start_empty_block)
    {
        std::stringbuf out_buffer;
        std::ostream out_stream(&out_buffer);
        auto time = std::make_shared<std::time_t>();

        auto handler = std::make_shared<Handler>();
        auto consoleWriter = std::shared_ptr<Observer>(new ConsoleWriter(out_stream));
        auto fileWriter = std::shared_ptr<Observer>(new FileWriter(time));
        consoleWriter->subscribe(handler);
        fileWriter->subscribe(handler);
        
        handler->setN(2);
        handler->addCommand("{");
        handler->addCommand("}");
        handler->stop();

        std::fstream file;
        file.open(name(time));
        std::stringstream string_stream;
        string_stream << file.rdbuf();
        file.close();

        BOOST_CHECK(out_buffer.str().empty());
        BOOST_CHECK(out_buffer.str().empty()); 
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(empty_block)
    {
        std::stringbuf out_buffer;
        std::ostream out_stream(&out_buffer);

        auto handler = std::make_shared<Handler>();
        auto consoleWriter = std::shared_ptr<Observer>(new ConsoleWriter(out_stream));
        consoleWriter->subscribe(handler);
        
        handler->setN(2);
        handler->addCommand("cmd1");
        handler->addCommand("{");
        handler->addCommand("}");
        handler->addCommand("cmd2");
        handler->stop();

        BOOST_CHECK_EQUAL(out_buffer.str(),"bulk: cmd1\nbulk: cmd2\n");
    }

BOOST_AUTO_TEST_SUITE_END()