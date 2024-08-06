// std
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

// json
#include <nlohmann/json.hpp>

// IFF SDK
#include <iffwrapper.hpp>
namespace iff = iffwrapper;

#ifdef __aarch64__
#pragma message("Make sure that configuration file uses YV12 output format instead of default NV12")
#endif


constexpr char CONFIG_FILENAME[] = "farsightcpp.json";

int main()
{
    nlohmann::json config;
    try
    {
        config = nlohmann::json::parse(std::ifstream(CONFIG_FILENAME), nullptr, true, true);
    }
    catch(const std::exception& e)
    {
        std::cerr << "Invalid configuration provided: " << e.what() << "\n";
        return EXIT_FAILURE;
    }
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

    iff::initialize_engine(it_iff->dump());

    std::vector<std::shared_ptr<iff::chain>> chains;
    for(const auto& chain_config : *it_chains)
    {
        auto chain = std::make_shared<iff::chain>(chain_config.dump(),
                                                  [](const std::string& element_name, int error_code)
                                                  {
                                                      std::ostringstream message;
                                                      message << "Chain element `" << element_name << "` reported an error: " << error_code;
                                                      iff::log(iff::log_level::error, "farsightcpp", message.str());
                                                  });
        chains.emplace_back(std::move(chain));
    }

    iff::log(iff::log_level::info, "farsightcpp", "Press Enter to terminate the program");
    std::getchar();

    chains.clear();

    iff::finalize_engine();

    return EXIT_SUCCESS;
}
