#include "command_line_parser.hpp"

command_line_parser::command_line_parser(int argc, char ** argv) :
    argc { argc }, argv { argv }
{
}

auto command_line_parser::finished() -> bool const {
    return count >= argc;
}

command_line_parser::dummy_handle::dummy_handle(
    command_line_parser & parent,
    std::string val
) :
    parent { parent }, val { std::move(val) }
{
}

auto command_line_parser::dummy_handle::value() const -> const std::string & {
    return val;
}

command_line_parser::dummy_handle::~dummy_handle() {
    ++parent.count;
    parent.in_handle = false;
}

command_line_parser::handle::handle(
    command_line_parser & parent,
    std::string arg_name,
    std::string val
) :
    dummy_handle { parent, std::move(val) }, arg_name { std::move(arg_name) }
{
}

void command_line_parser::handle::next_arg() {
    ++parent.count;
    if (parent.count >= parent.argc)
        throw "missing argument for " + arg_name;
    val = parent.argv[parent.count];
}

auto command_line_parser::dummy() -> std::unique_ptr<dummy_handle> {
    if (in_handle || finished())
        throw std::logic_error { "error" };
    in_handle = true;
    return std::unique_ptr<dummy_handle> { new dummy_handle { *this, argv[count] } };
}

auto command_line_parser::enter(const std::string & arg)
    -> std::unique_ptr<command_line_parser::handle>
{
    if (in_handle || finished())
        throw std::logic_error { "error" };
    auto val = argv[count];
    if (val == arg) {
        in_handle = true;
        return std::unique_ptr<handle> { new handle { *this, arg, val } };
    }
    return std::unique_ptr<handle> { };
}
