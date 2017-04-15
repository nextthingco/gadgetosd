/*
 * Gadget.cpp
 *
 *  Created on: Apr 13, 2017
 *      Author: howie
 */

#include "Gadget.h"
#include "ProjectFactory.h"
#include "Project.h"
#include "Container.h"
#include "Helpers.h"
#include "OptionPrinter.hpp"
#include <boost/program_options.hpp>
#include <boost/log/trivial.hpp>
#include "boost/filesystem.hpp"

#include <iostream>
namespace po = boost::program_options;
const std::string GADGET_CONFIG_FILE = "config.yaml";
Gadget::Gadget() {
}

/**
 * loads a project from a config file
 * @return
 */
std::shared_ptr<Project>  Gadget::loadProject() {
	std::string configFile;
	try {
		configFile = Helpers::findFileHereOrInParent(GADGET_CONFIG_FILE);
	} catch(std::exception & e) {
		BOOST_LOG_TRIVIAL(fatal) << "Could not find the configuration file " << GADGET_CONFIG_FILE;
		throw;
	}
	return ProjectFactory::fromYaml(configFile);

}
namespace
{
  const size_t ERROR_IN_COMMAND_LINE = 1;
  const size_t SUCCESS = 0;
  const size_t ERROR_UNHANDLED_EXCEPTION = 2;

} // namespace
//note to self. check out http://stackoverflow.com/questions/15541498/how-to-implement-subcommands-using-boost-program-options

size_t parseCommandLine(int argc, char **argv) {
    std::string appName = boost::filesystem::basename(argv[0]);

    int command = 0;

    /** Define and parse the program options
     */
    namespace po = boost::program_options;
    po::options_description desc("Options");
    desc.add_options()
      ("help,h", "Print this help message")
      ("version,v", "Show version information")
      ("debug,X","Show detailed messages")
      ("<command>", po::value<int>(&command)->required(), "One of the choices below");

    po::positional_options_description positionalOptions;
    positionalOptions.add("<command>", 1);

    po::variables_map vm;
    try
    {
      po::store(po::command_line_parser(argc, argv).options(desc)
                  .positional(positionalOptions).run(),
                vm); // throws on error

      /** --help option
       */
      if ( vm.count("help")  )
      {
        std::cout << "Gadget help" << std::endl << std::endl;
        rad::OptionPrinter::printStandardAppDesc(appName,
                                                 std::cout,
                                                 desc,
                                                 &positionalOptions);
        return SUCCESS;
      }
      po::notify(vm); // throws on error, so do after help in case
                      // there are any problems
    }
    catch(boost::program_options::required_option& e)
    {
      rad::OptionPrinter::formatRequiredOptionError(e);
      std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
      rad::OptionPrinter::printStandardAppDesc(appName,
                                               std::cout,
                                               desc,
                                               &positionalOptions);
      return ERROR_IN_COMMAND_LINE;
    }
    catch(boost::program_options::error& e)
    {
      std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
      rad::OptionPrinter::printStandardAppDesc(appName,
                                               std::cout,
                                               desc,
                                               &positionalOptions);
      return ERROR_IN_COMMAND_LINE;
    }
    return SUCCESS;
}

int main(int argc, char **argv)
{
	Gadget gadget;
	parseCommandLine(argc,argv);
	std::shared_ptr<Project> project;
	try {
		project = gadget.loadProject();
	} catch(std::exception & e) {
		exit(-1);
	}


	auto container = project->getContainers()[0];
	container->build();
}
