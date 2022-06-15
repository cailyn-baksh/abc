#ifndef _BACKEND_HPP_
#define _BACKEND_HPP_

#include <string>
#include <vector>

class IBackend {
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

	virtual void compile(std::vector<std::uint8_t> &ir) = 0;

	virtual ~IBackend() {}
};

/* Backends */


#endif  // _BACKEND_HPP_
