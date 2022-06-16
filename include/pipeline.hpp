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
	 * Always returns 0, so that it can be used to initialize a static variable
	 * This function may throw std::invalid_argument if a name has already been
	 * registered.
	 */
	int registerComponent(std::function<T*()> func, std::initializer_list<std::string> names) {
		for (std::string &name : names) {
			if (!registeredComponents.contains(name)) {
				registeredComponents[name] = func;
			} else {
				throw std::invalid_argument(std::string("Pipeline component ") + name + " is already registered");
			}
		}

		return 0;
	}

	/*
	 * Get an instance of the T subclass associated with name
	 */
	T *get(std::string name) {
		try {
			return registeredComponents.at(name)();
		} catch (std::out_of_range &ex) {
			// throw a more descriptive error
			throw std::out_of_range(std::string("Pipeline component ") + name + " is not registered");
		}
	}

	static PipelineComponentFactory<T> &getInstance() {
		static PipelineComponentFactory<T> instance;
		return instance;
	}
};

/*
 * Connects two pipes
 */
class Coupling {
public:
	/*
	 * The source end of the coupling, i.e. where it connects to the next pipe.
	 * This object is used to read from the previous component in the pipeline.
	 */
	class Source {
	private:
		Source();
	};

	/*
	 * The drain end of the coupling, i.e. where it connects to the previous
	 * pipe. This object is used to write to the next component in the pipeline
	 */
	class Drain {
	private:
		Drain();
	};

	friend class Coupling::Source;
	friend class Coupling::Drain;
private:

public:
	Coupling();
};

/*
 * An inlet into a pipeline
 */
class IInlet {
public:
	/*
	 * Inject product into the pipeline.
	 *
	 * file		The name of the file to read from.
	 * drain	The object to write output into, to be passed to the next
	 *			pipeline component.
	 */
	virtual void inject(std::string &file, Coupling::Drain &drain) = 0;

	virtual ~IInlet() {}
};

typedef PipelineComponentFactory<IInlet> InletFactory;

/*
 * A pipe somewhere in the middle of a pipeline
 */
class IPipe {
public:
	/*
	 * Pump product through this stage of the pipeline
	 *
	 * source	The source to get product from
	 * drain	The drain to pump product to.
	 */
	virtual void pump(Coupling::Source &source, Coupling::Drain &drain);

	virtual ~IPipe() {}
};

typedef PipelineComponentFactory<IPipe> PipeFactory;

/*
 * An outlet from a pipeline
 */
class IOutlet {
	/*
	 * Deliver the final product to the given file
	 *
	 * file		The name of the file to write output to. Will be created if it
	 *			does not exist.
	 * source	The input from the previous pipeline component.
	 */
	virtual void deliver(std::string &file, Coupling::Source &source) = 0;
	
	virtual ~IOutlet() {}
};

typedef PipelineComponentFactory<IOutlet> OutletFactory;


/*
 * A complete compilation pipeline
 */
class Pipeline {
private:
	IInlet *inlet;
	std::vector<IPipe *> pipes;
	IOutlet *outlet;

public:
	/*
	 * Construct a new pipeline from inlet to outlet.
	 *
	 * inlet	A string identifier for an inlet registered with InletFactory
	 * outlet	A string identifier for an outlet registered with OutletFactory
	 * Throws std::out_of_range if either inlet or outlet has not been
	 * registered.
	 */
	Pipeline(std::string inlet, std::string outlet);

	/*
	 * Install a segment of pipe between inlet and outlet. This pipe will be
	 * installed after the last installed pipe segment, if any.
	 *
	 * pipe		A string identifier for a pipe registered with PipeFactory
	 * Throws std::out_of_range if the pipe has not been registered.
	 */
	void addPipe(std::string pipe);

	/*
	 * Flow code from the source file to the destination file.
	 *
	 * srcFile	The source file, passed to the inlet
	 * dstFile	The destination file, passed to the outlet. This file is
	 *			created if it does not exist.
	 */
	void flow(std::string srcFile, std::string dstFile);

};

#endif  // _PIPELINE_HPP_
