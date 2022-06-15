#include <fstream>
#include <iostream>
#include <map>
#include <queue>
#include <string>
#include <utility>
#include <vector>

#include <boost/program_options.hpp>

#include "ir.hpp"
#include "pipeline.hpp"
#include "frontend.hpp"
#include "backend.hpp"

namespace po = boost::program_options;

// TODO: create map like this to select frontends and backends
// https://stackoverflow.com/a/24124037

/*
 * Construct a new frontend from a code or file extension
 *
 * code		A string to use to determine which frontend to use
 * isExt	Whether the string is a file extension (true) or a -x code (false)
 * Returns a pointer to a new instance of an IFrontend (which must be deleted)
 * or nullptr if one could not be selected.
 */
IFrontend *selectFrontend(std::string code, bool isExt) {
	if (isExt) {
		if (code == "bf") {
			return new BrainfuckFrontend();
		}
	} else {
		if (code == "brainfuck" || code == "bf") {
			return new BrainfuckFrontend();
		}
	}

	return nullptr;
}

/*
 * Construct a new backend from an architecture string.
 *
 * arch		The name of the architecture to target.
 * Returns a pointer to a new instance of an IBackend (which must be deleted)
 * or nullptr if one could not be selected.
 */
IBackend *selectBackend(std::string arch) {
	return nullptr;
}

int main(int argc, char **argv) {
	// NOTE: boost::program_options has severe limitations.
	// NOTE: only --arch, --output, and input are guaranteed to be functional
	// NOTE: this library will be replaced with another in the future
	po::options_description generalOpts("General Options");
	generalOpts.add_options()
		("arch", po::value<std::string>(), "The target architecture to generate code for.")
		(",f", po::value<std::vector<std::string>>(), "Set flags. Prefix a flag with no- to disable it")
		("help,h", "Show this help message. Combine with -x or --arch to see help for a specific frontend or backend")
		("output,o", po::value<std::string>(), "Place primary output in the specified file")
		(",S", "Stop after the first stage of compilation, and output IR")
		("verbose,v", "Show verbose output")
		("version", "Print version string")
		(",W", po::value<std::vector<std::string>>(), "Enable or disable warnings.")
		(",x", po::value<std::string>(), "Select the language")
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

	// Check peripheral options (options that do not trigger the main function of the program)
	if (vm.count("help")) {
		if (vm.count("x")) {
			std::string language = vm["x"].as<std::string>();
			IFrontend *frontend = selectFrontend(language, false);

			if (frontend) {
				std::cout << frontend->helpStr() << std::endl;
			} else {
				std::cerr << "No frontend found for language " << language << std::endl;
			}
		} else if (vm.count("arch")) {
			std::string arch = vm["arch"].as<std::string>();
			IBackend *backend = selectBackend(arch);

			if (backend) {
				std::cout << backend->helpStr() << std::endl;
			} else {
				std::cerr << "No backend found for architecture " << arch << std::endl;
			}
		} else {
			std::cout << "A Brainfuck Compiler." << std::endl;
			std::cout << "Usage: " << argv[0] << " FILE [options]" << std::endl;
			std::cout << visibleOpts << std::endl;
		}

		return 0;
	} else if (vm.count("version")) {
		std::cout << NAME " " VERSION << std::endl;
		return 0;
	}

	// Check options are okay
	if (!vm.count("input")) {
		std::cerr << "An input file must be specified. Run '" << argv[0] << " --help' for usage." << std::endl;
		return 1;
	}

	// Options are okay

	std::string srcFile = vm["input"].as<std::string>();
	IFrontend *frontend;
	IBackend *backend;

	// Select front end
	if (!vm.count("x")) {
		// Try to infer language from file extension
		std::size_t extIndex = srcFile.find_last_of('.');
		std::string fileExt = srcFile.substr(extIndex+1);

		frontend = selectFrontend(fileExt, true);

		if (!frontend) {
			std::cerr << "Could not infer language from file extension ." << fileExt << std::endl;
			return 1;
		}
	} else {
		// due to limitations of boost::program_options, this does not work
		std::string language = vm["x"].as<std::string>();
		frontend = selectFrontend(language, false);

		if (!frontend) {
			std::cerr << "Unknown language " << language << std::endl;
			return 1;
		}
	}

	// Select back end
	if (!vm.count("arch")) {
		// Target same architecture as this device
	} else {

	}

	// Apply options to frontend and backend
	/*{
		std::vector<std::string> flags = vm["f"].as<std::vector<std::string>>();
		std::vector<std::string> warnings = vm["W"].as<std::vector<std::string>>();

		frontend->applyOptions('f', flags);
		frontend->applyOptions('W', warnings);

		backend->applyOptions('f', flags);
		backend->applyOptions('W', warnings);
	}

	if (vm.count("verbose")) {
		frontend->setVerbosity(true);
		backend->setVerbosity(true);
	}*/

	/*
	 * Now its time to compile
	 *
	 * 1. call the parser
	 * 2. optimize the IR
	 * 3. call the code generator
	 */

	std::vector<std::uint8_t> ir;
	try {
		ir = frontend->parse(srcFile);
	} catch (IR::InvalidInstructionException &e) {
		std::cerr << e.what() << std::endl;
		return -1;
	}

	std::ofstream file;
	file.open("bin/out.ir", std::ios::out | std::ios::trunc | std::ios::binary);

	for (const std::uint8_t &v : ir) {
		file.put(v);
	}

	// optimize(&ir);

	//ir = backend->compile(ir);

	delete frontend;
	delete backend;
}

