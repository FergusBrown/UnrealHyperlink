#include <iostream>
#include <regex>

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cout << "Provided " << argc - 1 << " arguments, expected 1 argument.";
        return EXIT_FAILURE;
    }

    std::string link{ argv[1] };
    std::regex project_pattern{ R"(^unreal:\/\/(\w+)\/.*)" };

    std::smatch match{};
    
    if (!std::regex_match(link, match, project_pattern))
    {
        std::cout << "Provided link is not in the format unreal://PROJECT_NAME/...";
        return EXIT_FAILURE;
    }

    std::string project_name{ match.str(1) };
    // Add link to the pipe name to avoid potential conflicts with other plugins
    // TODO
    std::cout << project_name;
}