#include "data/parser/java/JavaEnvironment.h"

#include <jni.h>

#include "utility/logging/logging.h"
#include "data/parser/java/JavaEnvironmentFactory.h"

JavaEnvironment::~JavaEnvironment()
{
	JavaEnvironmentFactory::getInstance()->unregisterEnvironment();
}

bool JavaEnvironment::callStaticVoidMethod(std::string className, std::string methodName, int arg1, std::string arg2, std::string arg3, std::string arg4)
{
	jclass javaClass = m_env->FindClass(className.c_str());
	if(javaClass == nullptr)
	{
		LOG_ERROR("class " + className + " not found in JVM environment");
		jthrowable exc = m_env->ExceptionOccurred();
		if(exc)
		{
			m_env->ExceptionDescribe();
			m_env->ExceptionClear();
		}
	}
	else
	{
		jmethodID javaMethodId = m_env->GetStaticMethodID(javaClass, methodName.c_str(), "(ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
		if(javaMethodId == nullptr)
		{
			LOG_ERROR("method void " + methodName + "(int, String, String, String) not found in JVM environment");
		}
		else
		{
			jint jarg1 = arg1;
			jstring jarg2 = m_env->NewStringUTF(arg2.c_str());
			jstring jarg3 = m_env->NewStringUTF(arg3.c_str());
			jstring jarg4 = m_env->NewStringUTF(arg4.c_str());
			m_env->CallStaticVoidMethod(javaClass, javaMethodId, jarg1, jarg2, jarg3, jarg4);
			return true;
		}
	}
	return false;
}

std::string JavaEnvironment::toStdString(jstring s)
{
	const char *nativeString = m_env->GetStringUTFChars(s, 0);
	std::string ret = nativeString;
	m_env->ReleaseStringUTFChars(s, nativeString);
	return ret;
}

jstring JavaEnvironment::toJString(std::string s)
{
	return m_env->NewStringUTF(s.c_str());
}

JavaEnvironment::JavaEnvironment(JavaVM* jvm, JNIEnv* env)
	: m_jvm(jvm)
	, m_env(env)
{
	JavaEnvironmentFactory::getInstance()->registerEnvironment();
}

void JavaEnvironment::registerNativeMethods(std::string className, std::vector<NativeMethod> methods)
{
	JNINativeMethod* jniMethods = new JNINativeMethod[methods.size()];

	for (size_t i = 0; i < methods.size(); i++)
	{
		jniMethods[i].name = const_cast<char*>(methods[i].name.c_str());
		jniMethods[i].signature = const_cast<char*>(methods[i].signature.c_str());
		jniMethods[i].fnPtr = methods[i].function;
	}

	jclass javaClass = m_env->FindClass(className.c_str());
	if (javaClass)
	{
		if (m_env->RegisterNatives(javaClass, jniMethods, methods.size()) < 0)
		{
			LOG_ERROR("RegisterNatives failed");
		}
	}
	else
	{
		LOG_ERROR("class \"" + className + "\" not found while registering native methods");
	}

	delete [] jniMethods;
}

