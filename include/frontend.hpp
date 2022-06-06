#ifndef _FRONTEND_HPP_
#define _FRONTEND_HPP_

#include <queue>
#include <string>

#include "ir.hpp"

class IFrontend {
public:
	virtual IR::Program parse(std::string file) = 0;

	virtual ~IFrontend() {}
};

/* Frontend */
class BrainfuckFrontend : public IFrontend {
public:
	IR::Program parse(std::string file);
};

#endif  // _FRONTEND_HPP_

