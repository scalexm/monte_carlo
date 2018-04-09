#ifndef COMMAND_LINE_PARSER_HPP
#define COMMAND_LINE_PARSER_HPP

#include <string>
#include <memory>

class command_line_parser {
    private:
        int count = 0, argc;
        char ** argv;
        bool in_handle = false;
    
    public:
        command_line_parser(int, char **);
        auto finished() -> bool const;

        class dummy_handle {
            friend command_line_parser;
            protected:
                command_line_parser & parent;
            private:
                std::string val;
            
                dummy_handle(command_line_parser &, std::string);
                dummy_handle(const dummy_handle &) = delete;
                dummy_handle(dummy_handle &&) = delete;

            public:
                auto value() const -> const std::string &;
                ~dummy_handle();
        };

        class handle : public dummy_handle {
            friend command_line_parser;
            private:
                std::string arg_name;
            
                handle(command_line_parser &, std::string, std::string);

            public:
                void next_arg();
        };

        auto enter(const std::string &) -> std::unique_ptr<handle>;
        auto dummy() -> std::unique_ptr<dummy_handle>;
};

#endif
