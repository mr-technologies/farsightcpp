// std
#include <cstdio>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <iostream>
#include <iterator>

// json
#include <nlohmann/json.hpp>

// IFF SDK
#include <iffwrapper.hpp>

#ifdef __aarch64__
#pragma message("Make sure that configuration file uses YV12 output format instead of default NV12")
#endif


namespace iff = iffwrapper;

int main()
{
    std::ifstream cfg_file("farsightcpp.json");
    const std::string config_str = { std::istreambuf_iterator<char>(cfg_file), std::istreambuf_iterator<char>() };

    const auto config = nlohmann::json::parse(config_str, nullptr, true, true);
    const auto it_chains = config.find("chains");
    if(it_chains == config.end())
    {
        std::cerr << "Invalid configuration provided: missing `chains` section\n";
        return EXIT_FAILURE;
    }
    if(!it_chains->is_array())
    {
        std::cerr << "Invalid configuration provided: section `chains` must be an array\n";
        return EXIT_FAILURE;
    }
    if(it_chains->empty())
    {
        std::cerr << "Invalid configuration provided: section `chains` must not be empty\n";
        return EXIT_FAILURE;
    }
    const auto it_iff = config.find("IFF");
    if(it_iff == config.end())
    {
        std::cerr << "Invalid configuration provided: missing `IFF` section\n";
        return EXIT_FAILURE;
    }

    iff::initialize_engine(it_iff.value().dump());

    std::vector<std::shared_ptr<iff::chain>> chains;
    for(const auto& chain_config : it_chains.value())
    {
        auto chain = std::make_shared<iff::chain>(chain_config.dump(), [](const std::string& element_id, int error_code)
        {
            std::cerr << "Chain element `" << element_id << "` reported an error: " << error_code;
        });
        chains.push_back(chain);
    }

    iff::log(iff::log_level::info, "farsight", "Press Enter to terminate the program");
    std::getchar();

    chains.clear();

    iff::finalize_engine();

    return EXIT_SUCCESS;
}
