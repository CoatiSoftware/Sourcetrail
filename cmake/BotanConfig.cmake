SET(BOTAN_URL https://github.com/randombit/botan.git/)
SET(BOTAN_VERSION 1.11.23)
SET(BOTAN_MODULES "base,ecdsa,emsa1,passhash9,rng,rsa,egd,emsa_pssr,emsa_pkcs1,x509,aes,dev_random")

SET(BOTAN_DIR $ENV{COATI_DEPEND_DIR}/botan)

find_package(Git)

if(Git_FOUND)
else()
	message(ERROR "git not found: git not in path")
endif()

if(EXISTS $ENV{COATI_DEPEND_DIR})
else()
	message(ERROR "coati dependency directory does not exist")
endif()

if(EXISTS ${BOTAN_DIR})
else()
	execute_process(
        COMMAND ${GIT_EXECUTABLE} clone ${BOTAN_URL}
        WORKING_DIRECTORY $ENV{COATI_DEPEND_DIR}        
    )
endif()

execute_process(
    COMMAND ${GIT_EXECUTABLE} checkout ${BOTAN_VERSION}
    WORKING_DIRECTORY ${BOTAN_DIR}
)

# system specific botan modules
if(UNIX)

elseif(MSVC)

endif()

execute_process(
    COMMAND  ./configure.py --gen-amalgamation --minimized-build --enable-modules=${BOTAN_MODULES}
    WORKING_DIRECTORY ${BOTAN_DIR}    
)

#linux
#./configure.py --gen-amalgamation --minimized-build --enable-modules=base,ecdsa,emsa1,passhash9,rng,rsa,egd,dev_random,emsa_pssr,emsa_pkcs1,x509,aes 
