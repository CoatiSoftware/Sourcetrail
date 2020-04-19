#include "JavaEnvironmentFactory.h"

#include <cstdlib>

#include <jni.h>

#include "ApplicationSettings.h"
#include "JavaEnvironment.h"
#include "logging.h"
#include "utilityLibrary.h"
#include "utilityWindows.h"

void JavaEnvironmentFactory::createInstance(std::string classPath, std::string& errorString)
{
	if (s_instance)
	{
		if (classPath == s_classPath)
		{
			return;
		}
		else
		{
			LOG_ERROR("java classpath cannot be changed!");
			// todo: implement destroying the old factory instance and create a new one.
			// may be not so easy... there can only be one java env per process (which can never be
			// destroyed) -.-
			return;
		}
	}

	std::function<jint(JavaVM**, void**, void*)> createInstanceFunction;
	const FilePath javaPath = ApplicationSettings::getInstance()->getJavaPath();

	if (javaPath.empty())
	{
		errorString = "No Java Path provided in preferences.";
		return;
	}

	createInstanceFunction = utility::loadFunctionFromLibrary<jint, JavaVM**, void**, void*>(
		javaPath, "JNI_CreateJavaVM", errorString);

	if (!createInstanceFunction && !errorString.empty())
	{
		return;
	}

	s_classPath = classPath;

	const int optionCount = 2;

	JavaVM* jvm = nullptr;	  // Pointer to the JVM (Java Virtual Machine)
	JNIEnv* env = nullptr;	  // Pointer to native interface

	JavaVMInitArgs vm_args;									  // Initialization arguments
	JavaVMOption* options = new JavaVMOption[optionCount];	  // JVM invocation options
	std::string classPathOption = "-Djava.class.path=" + classPath;
	options[0].optionString = const_cast<char*>(classPathOption.c_str());
	options[1].optionString = const_cast<char*>("-Xms64m");

	// use these options to enable profiling in VisualVM
	// options[2].optionString = const_cast<char*>("-Dcom.sun.management.jmxremote");
	// options[3].optionString = const_cast<char*>("-Dcom.sun.management.jmxremote.port=9010");
	// options[4].optionString =
	// const_cast<char*>("-Dcom.sun.management.jmxremote.local.only=false"); options[5].optionString
	// = const_cast<char*>("-Dcom.sun.management.jmxremote.authenticate=false");
	// options[6].optionString = const_cast<char*>("-Dcom.sun.management.jmxremote.ssl=false");

	vm_args.version = JNI_VERSION_1_8;
	vm_args.nOptions = optionCount;
	vm_args.options = options;
	vm_args.ignoreUnrecognized = false;	   // invalid options make the JVM init fail

	jint rc = createInstanceFunction(&jvm, (void**)&env, &vm_args);

	delete[] options;

	if (rc != JNI_OK)
	{
		if (rc == JNI_EVERSION)
		{
			errorString = "JVM is oudated and doesn't meet requirements";
		}
		else if (rc == JNI_ENOMEM)
		{
			errorString = "not enough memory for JVM";
		}
		else if (rc == JNI_EINVAL)
		{
			errorString = "invalid argument for launching JVM";
		}
		else if (rc == JNI_EEXIST)
		{
			errorString = "the process can only launch one JVM an not more";
		}
		else
		{
			errorString = "could not create the JVM instance (error code " + std::to_string(rc) +
				")";
		}
	}
	else
	{
		jvm->DetachCurrentThread();
		s_instance = std::shared_ptr<JavaEnvironmentFactory>(new JavaEnvironmentFactory(jvm));
	}
}

std::shared_ptr<JavaEnvironmentFactory> JavaEnvironmentFactory::getInstance()
{
	return s_instance;
}

JavaEnvironmentFactory::~JavaEnvironmentFactory()
{
	// todo: what if there are threads running using the jvm?? log something!
	m_jvm->DestroyJavaVM();
}

std::shared_ptr<JavaEnvironment> JavaEnvironmentFactory::createEnvironment()
{
	std::thread::id currentThreadId = std::this_thread::get_id();

	JNIEnv* env;

	{
		std::lock_guard<std::mutex> lock(m_threadIdToEnvAndUserCountMutex);

		std::map<std::thread::id, std::pair<JNIEnv*, int>>::const_iterator it =
			m_threadIdToEnvAndUserCount.find(currentThreadId);
		if (it != m_threadIdToEnvAndUserCount.end())
		{
			env = it->second.first;
		}
		else
		{
			m_jvm->AttachCurrentThread((void**)&env, NULL);
			m_threadIdToEnvAndUserCount.insert(
				std::make_pair(currentThreadId, std::make_pair(env, 0)));
		}
	}

	return std::shared_ptr<JavaEnvironment>(new JavaEnvironment(m_jvm, env));
}

std::shared_ptr<JavaEnvironmentFactory> JavaEnvironmentFactory::s_instance;

std::string JavaEnvironmentFactory::s_classPath;

JavaEnvironmentFactory::JavaEnvironmentFactory(JavaVM* jvm): m_jvm(jvm) {}

void JavaEnvironmentFactory::registerEnvironment()
{
	std::thread::id currentThreadId = std::this_thread::get_id();
	{
		std::lock_guard<std::mutex> lock(m_threadIdToEnvAndUserCountMutex);
		std::map<std::thread::id, std::pair<JNIEnv*, int>>::iterator it =
			m_threadIdToEnvAndUserCount.find(currentThreadId);
		if (it != m_threadIdToEnvAndUserCount.end())
		{
			it->second.second++;
		}
		else
		{
			LOG_ERROR("something went horribly wrong while registering a java environment");
		}
	}
}

void JavaEnvironmentFactory::unregisterEnvironment()
{
	std::thread::id currentThreadId = std::this_thread::get_id();
	{
		std::lock_guard<std::mutex> lock(m_threadIdToEnvAndUserCountMutex);
		std::map<std::thread::id, std::pair<JNIEnv*, int>>::iterator it =
			m_threadIdToEnvAndUserCount.find(currentThreadId);
		if (it != m_threadIdToEnvAndUserCount.end())
		{
			it->second.second--;
			if (it->second.second == 0)
			{	 // TODO: currently this happens quite often. do something about that.
				m_jvm->DetachCurrentThread();
				m_threadIdToEnvAndUserCount.erase(it);
			}
		}
		else
		{
			LOG_ERROR("something went horribly wrong while unregistering a java environment");
		}
	}
}
