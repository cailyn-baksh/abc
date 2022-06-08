#ifndef _FRONTEND_HPP_
#define _FRONTEND_HPP_

#include <queue>
#include <string>
#include <vector>

#include "ir.hpp"

/*
 * A compiler front-end.
 */
class IFrontend {
public:
	/*
	 * Apply options specified on the command line to the frontend.
	 *
	 * option	The character code of the option. Unrecognised options are
	 *			ignored.
	 * values	The values of the options. These values should either be flags
	 *			(in the form "name") or settings (in the form "name=value").
	 *			Flags may be prefixed with "no-" to disable the flag.
	 *			Unrecognised values are ignored.
	 */
	virtual void applyOptions(char option, std::vector<std::string> &values) = 0;

	/*
	 * Use the front-end to parse a file into a program.
	 * This function is outward-facing. This means it "takes control" of the
	 * program, and thus may write directly to output streams and terminate the
	 * program.
	 */
	virtual std::vector<std::uint8_t> parse(std::string &file) = 0;

	/*
	 * Return a help string. This should document all user-facing features
	 * of the frontend.
	 */
	virtual std::string helpStr() = 0;

	/*
	 * Enable/disable verbose output. If enabled, parse should describe what
	 * its doing in stdout.
	 *
	 * verbosity	The new verbosity.
	 */
	virtual void setVerbosity(bool verbosity) = 0;

	virtual ~IFrontend() {}
};

/* Frontend */
class BrainfuckFrontend : public IFrontend {
public:
	void applyOptions(char option, std::vector<std::string> &values);

	std::vector<std::uint8_t> parse(std::string &file);

	std::string helpStr();

	void setVerbosity(bool verbosity);
};

#endif  // _FRONTEND_HPP_

