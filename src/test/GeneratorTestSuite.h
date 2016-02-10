#include "cxxtest/TestSuite.h"

#include "utility/text/TextAccess.h"

#include "License.h"
#include "Generator.h"

#include "botan_all.h"

class GeneratorTestSuite : public CxxTest::TestSuite
{
public:

    void test_create_Keys_with_Version_and_check()
    {
        Generator generator("v0");
        generator.generateKeys();

        std::string privateKey = generator.getPrivateKeyPEMFileAsString();
        std::string publicKey = generator.getPublicKeyPEMFileAsString();

        TS_ASSERT_EQUALS(privateKey.size(), 1886);
        TS_ASSERT_EQUALS(publicKey.size(), 451);
    }

    void test_load_private_Key_from_file()
    {
        Generator generator("v2");
        generator.setCustomPrivateKeyFile("./data/GeneratorTestSuite/private-v2.pem");
        bool ok = generator.loadPrivateKeyFromFile();

        TS_ASSERT(ok);
    }

    void test_load_private_key_from_string()
    {
        Generator generator("v0");
        bool ok = generator.loadPrivateKeyFromString(m_privateKey);

        TS_ASSERT(ok);
    }

    void test_create_license_and_validate()
    {
        Generator generator("v2");
        generator.generateKeys();

        generator.loadPrivateKeyFromString(generator.getPrivateKeyPEMFileAsString());
        License license;
        license.create("TestUser", "v2", generator.getPrivateKey());
        license.loadPublicKeyFromString(generator.getPublicKeyPEMFileAsString());

        TS_ASSERT_EQUALS(license.getOwnerLine(), "TestUser");
        TS_ASSERT_EQUALS(license.getLicenseTypeLine(), "Private License");
        TS_ASSERT_EQUALS(license.getVersion(), "v2");
        TS_ASSERT(license.isValid());
    }

private:
    std::string m_privateKey = "-----BEGIN ENCRYPTED PRIVATE KEY-----\n"
            "MIIFNTBfBgkqhkiG9w0BBQ0wUjAxBgkqhkiG9w0BBQwwJAQMWEDuADurEQ4T8kJO\n"
            "AgMDDUACASAwDAYIKoZIhvcNAgkFADAdBglghkgBZQMEASoEEGNDHYI3xozW5JBg\n"
            "pOLkhIMEggTQPwClVPQbk+AVqOAXndQqCs0Ad+eihbekCHt5zkRrYl6JbLq9fvfs\n"
            "kGgn1j7xtFZflcZimlLaao8pLWybJfa3xGSJnlYrJsGVMlmKJw/ee3fZG7y4/Tgo\n"
            "FytAIP51He4YAZ+4fOXyOoAT0+k2DBekZHeMADX7eXVpCLQXuoQpECvf2IPd/de4\n"
            "QKS12FRTd7Hkq7DwYs5D7aBizftcsB6FDqwKvvrheglMHfMt3/ChV8u3F8Zuqnh5\n"
            "S1NVl2b8geAnYWLEfN/V8SDu00heVHq+z2CIQrvonsTwN1c9ppgMC7/udcr4GEKU\n"
            "kqn/2h1sVRdlCxHf9p2nKRdOUXbSgQi16dKnEim3j3QCLVeNeX5Z6Tkb5pJA397i\n"
            "mbL0ihdUvbVcPMG9Rh8NONY+MkE4NKye+NHzWjsd+egaeA79qy/Xd4cJRG7GFy68\n"
            "pBWhf/xNpkhXJNV+O/KLszZG6f/UNDPJHovTp47aetD2/EqasvuYkPpapfYuqiZh\n"
            "3vtNlgL1uNIN/TjoSaut5+ifJonJnQBrMQdWLTN6I8/lZFfxZB4jLkHffLS/zTA+\n"
            "tPLxBcfZiec9dJDmuJ+P55D/3E+p6CL++yw15cgBNPFHrvfatikHvQXkRJl03S6Q\n"
            "FvrBmbjWKwT+iSZAvJrdkCeFO53CYA3tJ23GtG9pxBHP+VFyJSBu7MukjcCEMSbU\n"
            "HduNcsIIo8/p/X/CXYxJGYIfbrqz0n4z0QkZvCtyJoexoqqjTGclWoN3//j8lenL\n"
            "SbrPCwLEn1coGr6rx97LHDBjdR1WouWeGkQOyMF/dC70p4QyE3Ru0cbhsjAWeo7P\n"
            "HpGgQHJllJBFM9OkymRD32NnTCtr5Vu6epfiAsi16rfg1j1H9f0hnPbfULx+Q2WM\n"
            "sCzuUbKUcjRNXOhwDxQXGVrMXEjRzJKmqy2kpMFX/v38iKfqIx+pHMOs+/+ESwwp\n"
            "frKZ2mUwa7DdDv6fZwPUPtZk/6HyqiDoJcoiN05IxdPt6SXGPsseQWcwTdiHhDX2\n"
            "3YsTJK0O/CAf/ouqWQiZMAifTZtalTArTvTtlTE7TpAT3I0OmwNG/nI1v/orHBbZ\n"
            "M7sa9mlBDMzE4rTHIYMOttNKLxkXRChjgO2JfFDLxscSMw8wsrbGTbKh7VGFgg7Q\n"
            "Tj6KAGPiU7WbaXiIhO4adgYp3Bpbo15BhhO5bdaYzxBjOhYw1ShLEgBOC7MsOt3N\n"
            "8oChFJB1Gls5+RU6Ef27az+RE6uSumToT1HKgYDYX1TN68WWjT7Pa0lUMtZ4Bv1f\n"
            "rXMH8igPXg62kzh/hjnEtl8y2+ySAs7RnZ0vUywaKcI0Tu5tqJSLtXSfJKAPmmzE\n"
            "QYsg70kpufXccznTYvLSbM99FIbC6s+cSkJ43Ku4dwbEal+9FfqK639i2X9hdsLc\n"
            "bYXIb0Fguo2IbWruBeSIc8lukOKCOVjanVZhfOCev8ye/Wz+SS8zbEixWZMzMCzC\n"
            "if4dF/CUHnJK32IVlqKr9XGEe8ympqwMqN0eOdJn8IB+BQGCcaF6805D3R/OG4FU\n"
            "hedhIvg1cODtoi8ri41Pz7o1x9Ia0zZr7oxhQuSgHiTfRfFN0xhCFZ+sIfGNpgUs\n"
            "54IZGQlI0nQHOK8h2NI0wp3uo+fDEjjVIjQxINOSBa5awSReBpBUW2I=\n"
            "-----END ENCRYPTED PRIVATE KEY-----";
    std::string m_publicKey = "-----BEGIN PUBLIC KEY-----\n"
            "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAugYah7LOU0ssdSnyDA8h\n"
            "nOeW2lwE+NmZGxIKnqZKYNePY7Tg0c1pI0lfgJ2WYtxbubDFNDYk6bJF6mFg3jjN\n"
            "gCdocj6pyyibIISbRst+gl/1FwI8vbIkfkJBoZtftO5mKVBbO5mmrxm32fDQs1vo\n"
            "zgVxcWx3LXW87pzdQQYRACdkLSxPd+ADs+KNv6UxlOEvucDaenX3ckl3HdcWruL8\n"
            "xdYoM7z/C+PRShSGvY3wB7Y6A5IcdBmzsTR6xayCmTzzW83dmFzjCX5DSOJLHxq7\n"
            "+Fs26xZU83P1boX7eg4TjMViTxJwsCCW/2wfsZAoF0cSYCxhrkSdMHroH7Edz1PM\n"
            "fwIDAQAB\n"
            "-----END PUBLIC KEY-----\n";
};
