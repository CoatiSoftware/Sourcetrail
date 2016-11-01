#ifndef JAVA_ENVIRONMENT_H
#define JAVA_ENVIRONMENT_H

#include <string>
#include <vector>

struct JavaVM_;
typedef JavaVM_ JavaVM;

struct JNIEnv_;
typedef JNIEnv_ JNIEnv;

class _jclass;
typedef _jclass *jclass;

class _jstring;
typedef _jstring *jstring;

struct _jmethodID;
typedef struct _jmethodID *jmethodID;

class JavaEnvironmentFactory;

class JavaEnvironment
{
public:
	struct NativeMethod
	{
		std::string name;
		std::string signature;
		void *function;
	};

	~JavaEnvironment();
	bool callStaticVoidMethod(std::string className, std::string methodName, int arg1, std::string arg2, std::string arg3, std::string arg4, int arg5);
	bool callStaticMethod(std::string className, std::string methodName, std::string& ret, std::string arg1);

	std::string toStdString(jstring s);
	jstring toJString(std::string s);

	void registerNativeMethods(std::string className, std::vector<NativeMethod> methods);
private:
	friend class JavaEnvironmentFactory;

	JavaEnvironment(JavaVM* jvm, JNIEnv* env);
	jclass getJavaClass(const std::string& className);
	jmethodID getJavaStaticMethod(const std::string& className, const std::string& methodName, const std::string& methodSignature);
	jmethodID getJavaStaticMethod(jclass javaClass, const std::string& methodName, const std::string& methodSignature);

	JavaVM* m_jvm;
	JNIEnv* m_env;
};

#endif // JAVA_ENVIRONMENT_H
