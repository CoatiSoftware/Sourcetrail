#include "JavaEnvironment.h"

#include <jni.h>

#include "JavaEnvironmentFactory.h"
#include "logging.h"

JavaEnvironment::~JavaEnvironment()
{
	JavaEnvironmentFactory::getInstance()->unregisterEnvironment();
}

bool JavaEnvironment::callStaticVoidMethod(std::string className, std::string methodName)
{
	jclass javaClass = getJavaClass(className);
	jmethodID javaMethodId = getJavaStaticMethod(javaClass, methodName, "()V");
	if (javaMethodId != nullptr)
	{
		m_env->CallStaticVoidMethod(javaClass, javaMethodId);
		return true;
	}
	return false;
}

bool JavaEnvironment::callStaticVoidMethod(
	std::string className,
	std::string methodName,
	const std::string& arg1,
	const std::string& arg2,
	const std::string& arg3)
{
	jclass javaClass = getJavaClass(className);
	jmethodID javaMethodId = getJavaStaticMethod(
		javaClass, methodName, "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
	if (javaMethodId != nullptr)
	{
		jstring jarg1 = m_env->NewStringUTF(arg1.c_str());
		jstring jarg2 = m_env->NewStringUTF(arg2.c_str());
		jstring jarg3 = m_env->NewStringUTF(arg3.c_str());
		m_env->CallStaticVoidMethod(javaClass, javaMethodId, jarg1, jarg2, jarg3);
		return true;
	}
	return false;
}

bool JavaEnvironment::callStaticVoidMethod(
	std::string className,
	std::string methodName,
	int arg1,
	std::string arg2,
	std::string arg3,
	std::string arg4,
	std::string arg5,
	int arg6)
{
	jclass javaClass = getJavaClass(className);
	jmethodID javaMethodId = getJavaStaticMethod(
		javaClass,
		methodName,
		"(ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I)V");
	if (javaMethodId != nullptr)
	{
		jint jarg1 = arg1;
		jstring jarg2 = m_env->NewStringUTF(arg2.c_str());
		jstring jarg3 = m_env->NewStringUTF(arg3.c_str());
		jstring jarg4 = m_env->NewStringUTF(arg4.c_str());
		jstring jarg5 = m_env->NewStringUTF(arg5.c_str());
		jint jarg6 = arg6;
		m_env->CallStaticVoidMethod(
			javaClass, javaMethodId, jarg1, jarg2, jarg3, jarg4, jarg5, jarg6);
		return true;
	}
	return false;
}

bool JavaEnvironment::callStaticStringMethod(
	std::string className, std::string methodName, std::string& ret, const std::string& arg1)
{
	jclass javaClass = getJavaClass(className);
	jmethodID javaMethodId = getJavaStaticMethod(
		javaClass, methodName, "(Ljava/lang/String;)Ljava/lang/String;");
	if (javaMethodId != nullptr)
	{
		jstring jarg1 = m_env->NewStringUTF(arg1.c_str());
		jstring jret = (jstring)m_env->CallStaticObjectMethod(javaClass, javaMethodId, jarg1);
		if (jret)
		{
			const char* buffer = m_env->GetStringUTFChars(jret, JNI_FALSE);
			ret = std::string(buffer);
			m_env->ReleaseStringUTFChars(jret, buffer);
			return true;
		}
	}
	return false;
}

bool JavaEnvironment::callStaticStringMethod(
	std::string className,
	std::string methodName,
	std::string& ret,
	const std::string& arg1,
	const std::string& arg2)
{
	jclass javaClass = getJavaClass(className);
	jmethodID javaMethodId = getJavaStaticMethod(
		javaClass, methodName, "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;");
	if (javaMethodId != nullptr)
	{
		jstring jarg1 = m_env->NewStringUTF(arg1.c_str());
		jstring jarg2 = m_env->NewStringUTF(arg2.c_str());
		jstring jret = (jstring)m_env->CallStaticObjectMethod(javaClass, javaMethodId, jarg1, jarg2);
		if (jret)
		{
			const char* buffer = m_env->GetStringUTFChars(jret, JNI_FALSE);
			ret = std::string(buffer);
			m_env->ReleaseStringUTFChars(jret, buffer);
			return true;
		}
	}
	return false;
}

std::string JavaEnvironment::toStdString(jstring s)
{
	const char* nativeString = m_env->GetStringUTFChars(s, 0);
	std::string ret = nativeString;
	m_env->ReleaseStringUTFChars(s, nativeString);
	return ret;
}

jstring JavaEnvironment::toJString(std::string s)
{
	return m_env->NewStringUTF(s.c_str());
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
		if (m_env->RegisterNatives(javaClass, jniMethods, static_cast<jint>(methods.size())) < 0)
		{
			LOG_ERROR("RegisterNatives failed");
		}
	}
	else
	{
		LOG_ERROR("class \"" + className + "\" not found while registering native methods");
	}

	delete[] jniMethods;
}

JavaEnvironment::JavaEnvironment(JavaVM* jvm, JNIEnv* env): m_jvm(jvm), m_env(env)
{
	JavaEnvironmentFactory::getInstance()->registerEnvironment();
}

jclass JavaEnvironment::getJavaClass(const std::string& className)
{
	jclass javaClass = m_env->FindClass(className.c_str());
	if (javaClass == nullptr)
	{
		LOG_ERROR("class " + className + " not found in JVM environment");
		jthrowable exc = m_env->ExceptionOccurred();
		if (exc)
		{
			m_env->ExceptionDescribe();
			m_env->ExceptionClear();
		}
	}
	return javaClass;
}

jmethodID JavaEnvironment::getJavaStaticMethod(
	const std::string& className, const std::string& methodName, const std::string& methodSignature)
{
	return getJavaStaticMethod(getJavaClass(className), methodName, methodSignature);
}

jmethodID JavaEnvironment::getJavaStaticMethod(
	jclass javaClass, const std::string& methodName, const std::string& methodSignature)
{
	if (javaClass != nullptr)
	{
		jmethodID javaMethodId = m_env->GetStaticMethodID(
			javaClass, methodName.c_str(), methodSignature.c_str());
		if (javaMethodId == nullptr)
		{
			LOG_ERROR("method " + methodName + methodSignature + " not found in JVM environment");
		}
		return javaMethodId;
	}
	return nullptr;
}
