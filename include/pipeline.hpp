#ifndef _PIPELINE_HPP_
#define _PIPELINE_HPP_

#include <forward_list>
#include <functional>
#include <initializer_list>
#include <map>
#include <stdexcept>
#include <string>

/*
 * The pipeline is created at the start of compilation, by installing different
 * components together to create a complete pipeline that flows from source
 * code to executable code. The different components comprising the pipeline
 * run synchronous to each other -- that is, they all are executing at the same
 * time. Each component feeds data into the next component as soon as it can.
 *
 * At the start of the pipeline is the 'inlet', which takes an input file and
 * outputs IR, which will flow through the rest of the pipeline
 *
 * At the end of the pipeline is the 'outlet', which turns the IR into the
 * output code.
 *
 * At the intermediate phases of the pipeline there can be many different
 * components, or none at all. Since all of these components both take in and
 * yield IR, they can be assembled in any order.
 */

template <typename T>
class PipelineComponentFactory {
private:
	std::map<std::string, std::function<T*()>> registeredComponents;

	PipelineComponentFactory() {}

public:
	PipelineComponentFactory(PipelineComponentFactory<T> const&) = delete;
	void operator=(PipelineComponentFactory<T> const) = delete;

	/*
	 * Register a new component.
	 *
	 * func		A lambda which returns a pointer to a subclass of T.
	 * names	The names associated with this component. These must be unique.
	 * Always returns 0.
	 * This function may throw std::invalid_argument if a name has already been
	 * registered.
	 */
	int registerComponent(std::function<T*()> func, std::initializer_list<std::string> names) {
		for (std::string &name : names) {
			if (!registeredComponents.contains(name)) {
				registeredComponents[name] = func;
			} else {
				throw std::invalid_argument(std::string("Component ") + name + " already registered");
			}
		}

		return 0;
	}

	/*
	 * Get an instance of the T subclass associated with name
	 */
	T *get(std::string name) {
		return registeredComponents.at(name)();
	}

	static PipelineComponentFactory<T> &getInstance() {
		static PipelineComponentFactory<T> instance;
		return instance;
	}
};

/*
 * An inlet into a pipeline
 */
class IInlet {

};

typedef PipelineComponentFactory<IInlet> InletFactory;

/*
 * A pipe somewhere in the middle of a pipeline
 */
class IPipe {

};

typedef PipelineComponentFactory<IPipe> PipeFactory;

/*
 * An outlet from a pipeline
 */
class IOutlet {

};

typedef PipelineComponentFactory<IOutlet> OutletFactory;

/*
 */
class Pipeline {
private:
	IInlet *inlet;
	std::forward_list<IPipe> pipes;
	IOutlet *outlet;

public:
};

#endif  // _PIPELINE_HPP_
