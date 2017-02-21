#ifndef INTERPROCESS_UTILITY_H
#define INTERPROCESS_UTILITY_H

// requires a bool field 'm_initialized' to be defined
// if 'm_initialized' is false, the value '__retVal__' will be returned
// '__retVal__' is not required
#define IF_INITIALIZED(__retVal__) \
	if(m_initialized == false) \
	{ \
		LOG_ERROR_STREAM(<< "not initialized"); \
		return __retVal__; \
	} \
	else \

#endif // INTERPROCESS_UTILITY_H
