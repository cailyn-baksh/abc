#include <iostream>
#include <queue>
#include <string>
#include <vector>

#include <boost/program_options.hpp>

#include "parser.hpp"

#define NAME "abc"
#define VERSION "v0.0.1"

namespace po = boost::program_options;

int main(int argc, char **argv) {
	po::options_description generalOpts("General Options");
	generalOpts.add_options()
		("arch", po::value<std::string>(), "The target architecture to generate code for.")
		(",f", po::value<std::vector<std::string>>(), "Options to be passed to the code generator")
		("help,h", "Show this help message")
		("output,o", po::value<std::string>(), "Place primary output in the specified file")
		("verbose,v", "Show verbose output")
		("version", "Print version string")
		;

	po::positional_options_description positional;
	positional.add("input", 1);

	po::options_description hidden("Hidden Options");
	hidden.add_options()
		("input", po::value<std::string>(), "input file");
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

	std::queue<Tokens> tokens = parse(vm["input"].as<std::string>());

	std::cout << "Press enter to view next token. Type q to quit." << std::endl;
	while (!tokens.empty()) {
		Tokens t = tokens.front();
		tokens.pop();

		switch (t) {
			case Tokens::INC:
				std::cout << "INC";
				break;
			case Tokens::DEC:
				std::cout << "DEC";
				break;
			case Tokens::NEXT:
				std::cout << "NEXT";
				break;
			case Tokens::PREV:
				std::cout << "PREV";
				break;
			case Tokens::BEGINLOOP:
				std::cout << "BEGINLOOP";
				break;
			case Tokens::ENDLOOP:
				std::cout << "ENDLOOP";
				break;
			case Tokens::OUT:
				std::cout << "OUT";
				break;
			case Tokens::IN:
				std::cout << "IN";
				break;
		}
		if (std::cin.get() == 'q') return 0;
	}
}

