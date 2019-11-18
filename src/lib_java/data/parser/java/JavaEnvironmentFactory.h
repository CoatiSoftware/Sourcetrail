#ifndef JAVA_ENVIRONMENT_FACTORY_H
#define JAVA_ENVIRONMENT_FACTORY_H

#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

struct JavaVM_;
typedef JavaVM_ JavaVM;

struct JNIEnv_;
typedef JNIEnv_ JNIEnv;

class JavaEnvironment;

class JavaEnvironmentFactory
{
public:
	static void createInstance(std::string classPath, std::string& errorString);
	static std::shared_ptr<JavaEnvironmentFactory> getInstance();

	~JavaEnvironmentFactory();

	std::shared_ptr<JavaEnvironment> createEnvironment();

private:
	friend class JavaEnvironment;

	static std::shared_ptr<JavaEnvironmentFactory> s_instance;
	static std::string s_classPath;

	JavaEnvironmentFactory(JavaVM* jvm);

	void registerEnvironment();
	void unregisterEnvironment();

	JavaVM* m_jvm;
	std::map<std::thread::id, std::pair<JNIEnv*, int>> m_threadIdToEnvAndUserCount;
	std::mutex m_threadIdToEnvAndUserCountMutex;
};

#endif	  // JAVA_ENVIRONMENT_FACTORY_H
