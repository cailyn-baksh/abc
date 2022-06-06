#include <iostream>
#include <queue>
#include <string>
#include <vector>

#include <boost/program_options.hpp>

#include "ir.hpp"
#include "frontend.hpp"

#define NAME "abc"
#define VERSION "v0.0.2"

namespace po = boost::program_options;

int main(int argc, char **argv) {
	po::options_description generalOpts("General Options");
	generalOpts.add_options()
		("arch", po::value<std::string>(), "The target architecture to generate code for.")
		(",f", po::value<std::vector<std::string>>(), "Options to be passed to the code generator")
		("help,h", "Show this help message")
		("output,o", po::value<std::string>(), "Place primary output in the specified file")
		(",S", "Stop after the first stage of compilation, and output IR")
		("verbose,v", "Show verbose output")
		("version", "Print version string")
		("x", po::value<std::string>(), "Select the language")
		;

	po::positional_options_description positional;
	positional.add("input", 1);

	po::options_description hidden("Hidden Options");
	hidden.add_options()
		("input", po::value<std::string>(), "input file")
		;

	po::options_description options;
	options.add(generalOpts).add(hidden);

	po::options_description visibleOpts;
	visibleOpts.add(generalOpts);

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).options(options).positional(positional).run(), vm);
	po::notify(vm);

	if (vm.count("help")) {
		std::cout << "A Brainfuck Compiler." << std::endl;
		std::cout << "Usage: " << argv[0] << " FILE [options]" << std::endl;
		std::cout << visibleOpts << std::endl;
		return 0;
	} else if (vm.count("version")) {
		std::cout << NAME << " " << VERSION << std::endl;
		return 0;
	}

	if (!vm.count("input")) {
		std::cerr << "An input file must be specified. Run '" << argv[0] << " --help' for usage." << std::endl;
		return 1;
	}

	std::string srcFile = vm["input"].as<std::string>();
	IFrontend *frontend;
	//IBackend generator;

	// Select front end
	if (!vm.count("x")) {
		// Try to infer language from file extension
		std::size_t extIndex = srcFile.find_last_of('.');
		std::string fileExt = srcFile.substr(extIndex+1);

		if (fileExt == "bf") {
			frontend = new BrainfuckFrontend();
		} else {
			std::cerr << "Unrecognized file extension ." << fileExt << std::endl;
			return 1;
		}
	} else {
		std::string language = vm["x"].as<std::string>();

		if (language == "bf" || language == "brainfuck") {
			frontend = new BrainfuckFrontend();
		} else {
			std::cerr << "Unknown language " << language << std::endl;
			return 1;
		}
	}

	if (!vm.count("arch")) {
		// Target same architecture as this device
	}

	/*
	 * Now its time to compile
	 *
	 * 1. call the parser
	 * 2. optimize the IR
	 * 3. call the code generator
	 */

	IR::Program prog = frontend->parse(srcFile);

	// optimize()

	delete frontend;
}

