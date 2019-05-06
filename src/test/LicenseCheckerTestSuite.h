#include <cxxtest/TestSuite.h>

#include "License.h"
#include "LicenseChecker.h"
#include "Version.h"

class LicenseCheckerTestSuite : public CxxTest::TestSuite
{
public:

	void test_check_empty_license()
	{
		LicenseChecker::loadPublicKey();
		LicenseChecker::LicenseState state = LicenseChecker::checkLicenseString("");
		TS_ASSERT_EQUALS(state, LicenseChecker::LicenseState::EMPTY);
	}

	void test_check_malformed_license()
	{
		LicenseChecker::loadPublicKey();
		LicenseChecker::LicenseState state = LicenseChecker::checkLicenseString(
R"(-----BEGIN LICENSE-----
Product: Sourcetrail
License holder: Jane Doe
License type: Commercial License
Licensed number of users: 3
Licensed usage period: until 2021-Dec-10
-
$9$AQAKpvMMWlY7aWaMQY/AScwUo/nhqFdj+skS46v+7QHoj7X6qS+W
IErtt71WhVoqM8eki8IS+BDuO+TyLCAKhpNYQhvgykgfAx16T3ypD4A
4/DKGZbm/ZzZ3J5KD30Ko7pZfsgCnBMZu+VHSkoxtntDCzmhNySF4yo
fiDVhR9NYeWjgd0jtj1+dpBfjmQeXLgf4DqtYvgW446pUfEIIur174y
ffYziDA9VhIpgx2ZEYfFFXWNY+0esAkN389UxZ5UgQcs0LHTdEBphYO
oZJKdZdPbF+zSPwUeRej9SkCOpionrJ9lvVMo3p2pro0boeM4/bAs/p
MyEDaYFJdoazS3T8CXB0oPdEoyvYR6kOBZCw9YeW1Ud0azN8BeMx68c
gwitGhNQeI9A==
-----END LICENSE-----)"
		);

		TS_ASSERT_EQUALS(state, LicenseChecker::LicenseState::MALFORMED);
	}

	void test_check_invalid_license()
	{
		LicenseChecker::loadPublicKey();
		LicenseChecker::LicenseState state = LicenseChecker::checkLicenseString(
R"(-----BEGIN LICENSE-----
Product: Sourcetrail
License holder: Jane Doe
License type: Commercial License
Licensed number of users: 3
Licensed product version: unlimited
Licensed usage period: until 2021-Dec-10
-
$9$AQAKpvMMWlY7aWaMQY/AScwUo/nhqFdj+skS46v+7QHoj7X6qS+W
IErtt71WhVoqM8eki8IS+BDuO+TyLCAKhpNYQhvgykgfAx16T3ypD4A
4/DKGZbm/ZzZ3J5KD30Ko7pZfsgCnBMZu+VHSkoxtntDCzmhNySF4yo
fiDVhR9NYeWjgd0jtj1+dpBfjmQeXLgf4DqtYvgW446pUfEIIur174y
ffYziDA9VhIpgx2ZEYfFFXWNY+0esAkN389UxZ5UgQcs0LHTdEBphYO
oZJKdZdPbF+zSPwUeRej9SkCOpionrJ9lvVMo3p2pro0boeM4/bAs/p
MyEDaYFJdoazS3T8CXB0oPdEoyvYR6kOBZCw9YeW1Ud0azN8BeMx68C
gwitGhNQeI9A==
-----END LICENSE-----)"
		);

		TS_ASSERT_EQUALS(state, LicenseChecker::LicenseState::INVALID);
	}

	void test_check_license_without_start_end()
	{
		LicenseChecker::loadPublicKey();
		LicenseChecker::LicenseState state = LicenseChecker::checkLicenseString(
R"(
Product: Sourcetrail
License holder: Jane Doe
License type: Commercial License
Licensed number of users: 3
Licensed product version: unlimited
Licensed usage period: until 2021-Dec-10
-
$9$AQAKpvMMWlY7aWaMQY/AScwUo/nhqFdj+skS46v+7QHoj7X6qS+W
IErtt71WhVoqM8eki8IS+BDuO+TyLCAKhpNYQhvgykgfAx16T3ypD4A
4/DKGZbm/ZzZ3J5KD30Ko7pZfsgCnBMZu+VHSkoxtntDCzmhNySF4yo
fiDVhR9NYeWjgd0jtj1+dpBfjmQeXLgf4DqtYvgW446pUfEIIur174y
ffYziDA9VhIpgx2ZEYfFFXWNY+0esAkN389UxZ5UgQcs0LHTdEBphYO
oZJKdZdPbF+zSPwUeRej9SkCOpionrJ9lvVMo3p2pro0boeM4/bAs/p
MyEDaYFJdoazS3T8CXB0oPdEoyvYR6kOBZCw9YeW1Ud0azN8BeMx68c
gwitGhNQeI9A==
)"
		);

		TS_ASSERT_EQUALS(state, LicenseChecker::LicenseState::VALID);
	}

	void test_license_encoding()
	{
		LicenseChecker::loadPublicKey();
		LicenseChecker::setCurrentLicenseString(
R"(-----BEGIN LICENSE-----
Product: Sourcetrail
License holder: Jane Doe
License type: Commercial License
Licensed number of users: 3
Licensed product version: unlimited
Licensed usage period: until 2021-Dec-10
-
$9$AQAKpvMMWlY7aWaMQY/AScwUo/nhqFdj+skS46v+7QHoj7X6qS+W
IErtt71WhVoqM8eki8IS+BDuO+TyLCAKhpNYQhvgykgfAx16T3ypD4A
4/DKGZbm/ZzZ3J5KD30Ko7pZfsgCnBMZu+VHSkoxtntDCzmhNySF4yo
fiDVhR9NYeWjgd0jtj1+dpBfjmQeXLgf4DqtYvgW446pUfEIIur174y
ffYziDA9VhIpgx2ZEYfFFXWNY+0esAkN389UxZ5UgQcs0LHTdEBphYO
oZJKdZdPbF+zSPwUeRej9SkCOpionrJ9lvVMo3p2pro0boeM4/bAs/p
MyEDaYFJdoazS3T8CXB0oPdEoyvYR6kOBZCw9YeW1Ud0azN8BeMx68c
gwitGhNQeI9A==
-----END LICENSE-----)"
		);

		LicenseChecker::LicenseState state = LicenseChecker::checkCurrentLicense();
		TS_ASSERT_EQUALS(state, LicenseChecker::LicenseState::VALID);

		LicenseChecker::setEncodeKey("test_key");
		std::string encodedLicenseString = LicenseChecker::getCurrentLicenseStringEncoded();

		LicenseChecker::setEncodeKey("test_key_new");
		state = LicenseChecker::setCurrentLicenseStringEncoded(encodedLicenseString);

		TS_ASSERT_EQUALS(state, LicenseChecker::LicenseState::MALFORMED);

		LicenseChecker::setEncodeKey("test_key");
		state = LicenseChecker::setCurrentLicenseStringEncoded(encodedLicenseString);

		TS_ASSERT_EQUALS(state, LicenseChecker::LicenseState::VALID);
	}

	void test_check_version_license()
	{
		License license;
		bool ok = license.loadFromString(
R"(-----BEGIN LICENSE-----
Product: Sourcetrail
License holder: Jane Doe
License type: Commercial License
Licensed number of users: 1
Licensed product version: up to 2020.2
Licensed usage period: unlimited
-
$9$AQAKrGHpbkd9I+FVXZ8YtKK7rJePuPqJcWwaLoNYze14fRNiIBQV
QcelksnNcsztdUVCSp1MPkEbuWHkhR74m5qJ3wECnMekfVmP3GhbvwL
cqAEyrpR4PrZCRM2oxEgcRVk6QrPM9pHYPaPHe8sV74odOGQtHUrEW6
N8ZNL0NMLzSxurD/hXdDLdgxya9UptYIJ6c2Bbu33xJ87Ofs07pptc0
4VzBXJYG5iKIbmQAnrU6KOBL5iIZ11/sM8VbkTw+nSySI4yr3/f8BWe
7PhDAmhvDTX9aMMfpaXXeL3PyC/023FwyAqimB1vzoo28Pm1p1tN9mF
l7Ci+UazTnVY34DLVoXYUaDVlt8t8VYq05Z7f6sfcGtqpthuevGatM0
e6G8hqqTc+CQ==
-----END LICENSE-----)"
		);

		Version version = Version::getApplicationVersion();
		Version::setApplicationVersion(Version(2020, 2, 11, "asdfasdf"));

		TS_ASSERT(ok);
		TS_ASSERT_EQUALS(license.getUser(), "Jane Doe");
		TS_ASSERT_EQUALS(license.getType(), "Commercial License");
		TS_ASSERT_EQUALS(license.getNumberOfUsers(), 1);
		TS_ASSERT_EQUALS(license.getTimeLeft(), -2);
		TS_ASSERT(license.isComplete());
		TS_ASSERT(!license.isEmpty());
		TS_ASSERT(!license.isExpired());
		TS_ASSERT(!license.isTestLicense());

		LicenseChecker::loadPublicKey();
		TS_ASSERT_EQUALS(LicenseChecker::checkLicense(license), LicenseChecker::LicenseState::VALID);

		Version::setApplicationVersion(version);
	}

	void test_check_version_license_expired()
	{
		License license;
		bool ok = license.loadFromString(
R"(-----BEGIN LICENSE-----
Product: Sourcetrail
License holder: Jane Doe
License type: Commercial License
Licensed number of users: 1
Licensed product version: up to 2018.2
Licensed usage period: unlimited
-
$9$AQAKshj0tavIV6xR/dL9nXxaxib80g8Kxho0CGpvdypb4xujP5nw
mIQi3TiVrBTCszJLONCdnhyzotj1R370JPHWa/FFbGqABhexI5PGdIb
3Pa1EqQP9otCJZvDhQTxkm2Ev3zFEgDsA8njBQjr09Blcbt6F9F5zpJ
Ld75PRPLgvVmrxJfwOOg0tYQ/WrKzmm68gcF7XUHS1N7QDSrxjkqihu
xvHd6PoQSmX8pbNpYbkNF6N1T7rR5B8o7BnCc1PEw13J6k8CsomyQ4Q
qg2qAz0IOXk+r7ZagNvR9XedqcytIu6e0u48ujpEUv+1TSj7VBrOq1a
5La45aDoFn2XgdX/mlJh4RPeTEtg6u06PHf4pHFWiOXyKcvvAWuy7GT
YeDldWiy3iwA==
-----END LICENSE-----)"
		);

		Version version = Version::getApplicationVersion();
		Version::setApplicationVersion(Version(2018, 3, 11, "asdfasdf"));

		TS_ASSERT(ok);
		TS_ASSERT_EQUALS(license.getUser(), "Jane Doe");
		TS_ASSERT_EQUALS(license.getType(), "Commercial License");
		TS_ASSERT_EQUALS(license.getNumberOfUsers(), 1);
		TS_ASSERT_EQUALS(license.getTimeLeft(), -2);
		TS_ASSERT(license.isComplete());
		TS_ASSERT(!license.isEmpty());
		TS_ASSERT(license.isExpired());
		TS_ASSERT(!license.isTestLicense());

		LicenseChecker::loadPublicKey();
		TS_ASSERT_EQUALS(LicenseChecker::checkLicense(license), LicenseChecker::LicenseState::EXPIRED);

		Version::setApplicationVersion(version);
	}

	void test_check_period_license()
	{
		License license;
		bool ok = license.loadFromString(
R"(-----BEGIN LICENSE-----
Product: Sourcetrail
License holder: Jane Doe
License type: Commercial License
Licensed number of users: 3
Licensed product version: unlimited
Licensed usage period: until 2021-Dec-10
-
$9$AQAKpvMMWlY7aWaMQY/AScwUo/nhqFdj+skS46v+7QHoj7X6qS+W
IErtt71WhVoqM8eki8IS+BDuO+TyLCAKhpNYQhvgykgfAx16T3ypD4A
4/DKGZbm/ZzZ3J5KD30Ko7pZfsgCnBMZu+VHSkoxtntDCzmhNySF4yo
fiDVhR9NYeWjgd0jtj1+dpBfjmQeXLgf4DqtYvgW446pUfEIIur174y
ffYziDA9VhIpgx2ZEYfFFXWNY+0esAkN389UxZ5UgQcs0LHTdEBphYO
oZJKdZdPbF+zSPwUeRej9SkCOpionrJ9lvVMo3p2pro0boeM4/bAs/p
MyEDaYFJdoazS3T8CXB0oPdEoyvYR6kOBZCw9YeW1Ud0azN8BeMx68c
gwitGhNQeI9A==
-----END LICENSE-----)"
		);

		TS_ASSERT(ok);
		TS_ASSERT_EQUALS(license.getUser(), "Jane Doe");
		TS_ASSERT_EQUALS(license.getType(), "Commercial License");
		TS_ASSERT_EQUALS(license.getNumberOfUsers(), 3);
		TS_ASSERT(license.getTimeLeft() >= 0);
		TS_ASSERT(license.isComplete());
		TS_ASSERT(!license.isEmpty());
		TS_ASSERT(!license.isExpired());
		TS_ASSERT(!license.isTestLicense());

		LicenseChecker::loadPublicKey();
		TS_ASSERT_EQUALS(LicenseChecker::checkLicense(license), LicenseChecker::LicenseState::VALID);
	}

	void test_check_period_license_expired()
	{
		License license;
		bool ok = license.loadFromString(
R"(-----BEGIN LICENSE-----
Product: Sourcetrail
License holder: Jane Doe
License type: Commercial License
Licensed number of users: 3
Licensed product version: unlimited
Licensed usage period: until 2018-Dec-06
-
$9$AQAKkgTBy1koMv8CusfbUVWyMQtbTc67dSMTv+96bnVWSQdc/Bti
O8300TO2SWEVuyMo/BQkmXTnEFWLtwnAPqtAJXLR4oDTqlHUNs8Ur5q
iL6dMVsiyp41EHrzuof1/u+pgX03kCyIgD6TAn42En3+2pgo1zhDxnx
vaBxQTUcUW8+kJyNdSlJ6E2DgwGX2uXmf8MF8nsJtwHhOe/3qyV6fm0
PtXmWwjesNX7qGA8L+pcZKsB6AdC4dEJ71iFr8u/O1i8ab0v5xorJM3
zJ40jE0kcucpag5sW43ch0eX6PntXSwXuFfaQ7gTUH6qYTvxqIFUZGW
r0TrPuhiyxHS80ki5QApv9AShXlt3LRM41SJSCuv71Z30kGIVV6Itoo
+rtjllrh9TBA==
-----END LICENSE-----)"
		);

		TS_ASSERT(ok);
		TS_ASSERT_EQUALS(license.getUser(), "Jane Doe");
		TS_ASSERT_EQUALS(license.getType(), "Commercial License");
		TS_ASSERT_EQUALS(license.getNumberOfUsers(), 3);
		TS_ASSERT_EQUALS(license.getTimeLeft(), -1);
		TS_ASSERT(license.isComplete());
		TS_ASSERT(!license.isEmpty());
		TS_ASSERT(license.isExpired());
		TS_ASSERT(!license.isTestLicense());

		LicenseChecker::loadPublicKey();
		TS_ASSERT_EQUALS(LicenseChecker::checkLicense(license), LicenseChecker::LicenseState::EXPIRED);
	}

	void test_check_site_license()
	{
		License license;
		bool ok = license.loadFromString(
R"(-----BEGIN LICENSE-----
Product: Sourcetrail
License holder: Jane Doe
License type: Commercial License
Licensed number of users: unlimited
Licensed product version: unlimited
Licensed usage period: until 2021-Dec-10
-
$9$AQAKM0lWahp0xKiqVObgsjtpg9LNh+knuPSDbbv/6LuJqbZYA2F2
ibdICkHLfDTIiUjqXdmLbNWbavWwcrtOnzw7zxExIyCcJPXrQxUjmRU
kvVe+mPlN+4Re1jFBmCY18X13fb9JSShOjWGEnpCR/o6Fq1EKoOoYg1
NyWoVDy+Jv/Tr0Zr1/xFyhQJMw3dr0LiCrmYG0+J9P4lndI7MI4QG3z
0e625imrkJVDJsRUk3V9YXu8XxJTpSfc8tZjolzxCpaAKNzzCQeghdh
hwAEZEqUd3+ULN6lY36laCzRSMOts6506wMMhv4wzT65h8RRrrgJJjw
lnOALOwf+YcHvMpHWBBL18utvvxn1dWUdywBXPfmJnncglP7Cg+/B6q
gPxec/V8S5mA==
-----END LICENSE-----)"
		);

		TS_ASSERT(ok);
		TS_ASSERT_EQUALS(license.getUser(), "Jane Doe");
		TS_ASSERT_EQUALS(license.getType(), "Commercial License");
		TS_ASSERT_EQUALS(license.getNumberOfUsers(), 0);
		TS_ASSERT(license.getTimeLeft() >= 0);
		TS_ASSERT(license.isComplete());
		TS_ASSERT(!license.isEmpty());
		TS_ASSERT(!license.isExpired());
		TS_ASSERT(!license.isTestLicense());

		LicenseChecker::loadPublicKey();
		TS_ASSERT_EQUALS(LicenseChecker::checkLicense(license), LicenseChecker::LicenseState::VALID);
	}

	void test_check_site_license_expired()
	{
		License license;
		bool ok = license.loadFromString(
R"(-----BEGIN LICENSE-----
Product: Sourcetrail
License holder: Jane Doe
License type: Commercial License
Licensed number of users: unlimited
Licensed product version: unlimited
Licensed usage period: until 2018-Dec-08
-
$9$AQAKi2K2OESpqYwzOE6EPg5ltiYYNV3FYzGT9O/mf8fp8+ovGL5T
aJjHkXuWYsZ71yG4SH7DKrtSKc3nSHT6eBKpxkGeQ/50KMIqbFHUNWm
qsv7mzFeKeoA7e9Hf2X/RfWqBmHTV8eIhaE4qqMKF5DY+fG9Y1qlHEs
oeKJCM6BaxYiPZjG4sMKG2K/6AkT4TbNaHjrBktlE1l4ad8mW1xBXn5
p6ph0waObobOioNZINvLuRrIModAnfjCFVdKtL+HzM8Kg/2Tx7ZPCQV
4Tvj6Jm61OiXAdgZcIoL1DYsZWqIb3HFIJQw7Q+IGsKFgh64oNwh/WP
LZi4XroeDeOJnhX5+sfr0WZoV2BKCsNCsvKNHTtEG9ZGtKSgp8ktgBt
XCElsg55RjkQ==
-----END LICENSE-----)"
		);

		TS_ASSERT(ok);
		TS_ASSERT_EQUALS(license.getUser(), "Jane Doe");
		TS_ASSERT_EQUALS(license.getType(), "Commercial License");
		TS_ASSERT_EQUALS(license.getNumberOfUsers(), 0);
		TS_ASSERT_EQUALS(license.getTimeLeft(), -1);
		TS_ASSERT(license.isComplete());
		TS_ASSERT(!license.isEmpty());
		TS_ASSERT(license.isExpired());
		TS_ASSERT(!license.isTestLicense());

		LicenseChecker::loadPublicKey();
		TS_ASSERT_EQUALS(LicenseChecker::checkLicense(license), LicenseChecker::LicenseState::EXPIRED);
	}

	void test_check_test_license()
	{
		License license;
		bool ok = license.loadFromString(
R"(-----BEGIN LICENSE-----
Product: Sourcetrail
License holder: Jane Doe
License type: Test License
Licensed number of users: unlimited
Licensed product version: unlimited
Licensed usage period: until 2021-Dec-10
-
$9$AQAKrvzvSMvN038BfcgVNT8rN6/X4pFbYKpU0gMSeH62+Vsd55MC
E6rIwc+vMlHlLzHZsn6+YpSsPX2Hs31JUpkRTAb2X2ln0SZU2mZUL/V
SD4trO/jCUoTaC7FbaXD6F1A8DWuJos706g/MaNWyXc0tUnd3N6HSnZ
UEX0HHQXaKatZh5imexO5NncQtdhObU9PFo0UhXTxQ9wFxrKFzgyIIl
EQqQq4ExHzMz41wDD/476lf43W8O6tfbSRaO2aSlALy/3IqnrZBjllm
girrswPR5tAq8JCL8hxenkajWK7plnlg4tUuvpvnj/fif/2Km5v7Xls
+MDZZ7EC7hRX2UUHmIWAnCaiv25k/NHfu1GBQ19mS8mWYnRmxXOccAO
qCavX9fMG6pQ==
-----END LICENSE-----)"
		);

		TS_ASSERT(ok);
		TS_ASSERT_EQUALS(license.getUser(), "Jane Doe");
		TS_ASSERT_EQUALS(license.getType(), "Test License");
		TS_ASSERT_EQUALS(license.getNumberOfUsers(), 0);
		TS_ASSERT(license.getTimeLeft() >= 0);
		TS_ASSERT(license.isComplete());
		TS_ASSERT(!license.isEmpty());
		TS_ASSERT(!license.isExpired());
		TS_ASSERT(license.isTestLicense());

		LicenseChecker::loadPublicKey();
		TS_ASSERT_EQUALS(LicenseChecker::checkLicense(license), LicenseChecker::LicenseState::VALID);
	}

	void test_check_test_license_expired()
	{
		License license;
		bool ok = license.loadFromString(
R"(-----BEGIN LICENSE-----
Product: Sourcetrail
License holder: Jane Doe
License type: Test License
Licensed number of users: unlimited
Licensed product version: unlimited
Licensed usage period: until 2018-Dec-06
-
$9$AQAKML9uOsAwGbhR/GdF+idZBjBwcvYuNvym5yLvTweDEOs70IHy
NQBPgvq3TOUPtmKc3DE1lHLB7Fe2/8VNwcg1hkncle9fKh44WGuNIRX
dFPJ40Hl+be8ekrjyKwz1x9fb85mkzheiDQUEC8QOHUUdJJoETgp3Il
dcLDe/UL7Sq3/GpPDH+SaThMM5y+3tluf7yO/4qqRuzbV68YW/DTHEL
47/LvlW1iAypllpeJD7igpCzUEiRYa2m2pSmpOiS0FoXaZ0jhr0LNXm
C6/zLqHZ4xRnUmhDlmTf1mfybi86xOiPeZFfvSVnv5HPtf5Nsk2wGkW
pwim1/hpoCxGJ4hiN9a3cbgnjtqG7yTbfMIiBAxM4JcHt2Dzhs3+r6d
gabQRg1nODDw==
-----END LICENSE-----)"
		);

		TS_ASSERT(ok);
		TS_ASSERT_EQUALS(license.getUser(), "Jane Doe");
		TS_ASSERT_EQUALS(license.getType(), "Test License");
		TS_ASSERT_EQUALS(license.getNumberOfUsers(), 0);
		TS_ASSERT_EQUALS(license.getTimeLeft(), -1);
		TS_ASSERT(license.isComplete());
		TS_ASSERT(!license.isEmpty());
		TS_ASSERT(license.isExpired());
		TS_ASSERT(license.isTestLicense());

		LicenseChecker::loadPublicKey();
		TS_ASSERT_EQUALS(LicenseChecker::checkLicense(license), LicenseChecker::LicenseState::EXPIRED);
	}

	void test_check_lifelong_license()
	{
		License license;
		bool ok = license.loadFromString(
R"(-----BEGIN LICENSE-----
Product: Sourcetrail
License holder: Jane Doe
License type: Commercial License
Licensed number of users: 2
Licensed product version: unlimited
Licensed usage period: unlimited
-
$9$AQAKIZaofFkJU73SLfokexujrBlm792b2tF+oQksC40P+Z8vUtgT
hesYyYrRxFXK2OALm7zLkdeJYNwC60+oJx0wA19D6PLQo09d5Tf3hHx
0pkdV0J+2NwidLoJX6ZHWB2KQxg1Vg0yvczZs4uzNLLd6JRdBubN3XF
0hO9m/sYLMRzeh5sn7WJjV2M/BR1bGmgmY0itW0UXw+hYGZaViiDo1f
7erXFcZq5TJwhYM447TjoWa+KDjPd/hCohO8SKKtJ33qWWuTfJNPL5q
7iqyQemOnBOtW+nXHmg/89F+YXoyGeICxinnFs1kmqMey94s/08OUcB
GSlQ649gX2BuuACmHR/JW/0SY4ik/DgkesVHuRWXD8VTq8cOBMCKjOe
YHCqvUCFHVMA==
-----END LICENSE-----)"
		);

		TS_ASSERT(ok);
		TS_ASSERT_EQUALS(license.getUser(), "Jane Doe");
		TS_ASSERT_EQUALS(license.getType(), "Commercial License");
		TS_ASSERT_EQUALS(license.getNumberOfUsers(), 2);
		TS_ASSERT_EQUALS(license.getTimeLeft(), -2);
		TS_ASSERT(license.isComplete());
		TS_ASSERT(!license.isEmpty());
		TS_ASSERT(!license.isExpired());
		TS_ASSERT(!license.isTestLicense());

		LicenseChecker::loadPublicKey();
		TS_ASSERT_EQUALS(LicenseChecker::checkLicense(license), LicenseChecker::LicenseState::VALID);
	}

	void test_check_long_name()
	{
		License license;
		bool ok = license.loadFromString(
R"(-----BEGIN LICENSE-----
Product: Sourcetrail
License holder: Janestanasolinaria Doestopolstiminikovasotanibanastovic
License type: Commercial License
Licensed number of users: 2
Licensed product version: up to 2023.4
Licensed usage period: unlimited
-
$9$AQAKD2gin+JhXldRL5AkVpoTgNHibszdfWpKSgm+p5zC5wCYiGWd
Vtd8nzkj4Gys3liNW5K1Yu/bfgcoOwjBOh6ZpNXCs9MQaNtfHTNeNXA
6vKwBCyrKruO1Ez1y+63pYN+erVtrxIpy6bTxuSIuD115j++V7sDE6f
oMCTYVAtMoPBxkIDrMB/dRDVu+ZPffAtPl9qA/1Okl697UpFLIC5kOP
YoJmSgwXC0k0lOvF7OyzR95oKUjQifdWgnEH5kpPJ3DqcKsAvSh0bQj
Rmk10RSZo1OmjgYEssKlD04BZMWsgKEeNHVAkovY6mxR4160wOuELxP
Y8odKqEzRcdV/ZWysqZEGPpdeBIqeBd4ZVgN6kEDZqpGktGZL0LThEB
Muf9wa0CwXuw==
-----END LICENSE-----)"
		);

		TS_ASSERT(ok);
		TS_ASSERT_EQUALS(license.getUser(), "Janestanasolinaria Doestopolstiminikovasotanibanastovic");
		TS_ASSERT_EQUALS(license.getType(), "Commercial License");
		TS_ASSERT_EQUALS(license.getNumberOfUsers(), 2);
		TS_ASSERT_EQUALS(license.getTimeLeft(), -2);
		TS_ASSERT(license.isComplete());
		TS_ASSERT(!license.isEmpty());
		TS_ASSERT(!license.isExpired());
		TS_ASSERT(!license.isTestLicense());

		LicenseChecker::loadPublicKey();
		TS_ASSERT_EQUALS(LicenseChecker::checkLicense(license), LicenseChecker::LicenseState::VALID);
	}

	void test_check_old_format_license()
	{
		License license;
		bool ok = license.loadFromString(
R"(-----BEGIN LICENSE-----
Product: Sourcetrail
Licensed to: Jane Doe
License type: Commercial License (5 users)
Valid up to version: 2020.2
-
$9$AQAKDowaE3jU5qows5JQrqt6iEfM+BKO0uCgN8lYCIAy/qUzfDsI
S8pZMKD9fBNP0QHK1RNAaiwD8qszcnksDHcURUelnCBk2fpAd4YdR2a
IU6CBwkPWAT92K7KE5OfWV2h2odlSTirPb99WqWYnEngibVeBA4cGZD
k+fSpsPz3Dwg66dNUKajf+snii5wE9qa/2XYhV3gLm3Utls2jhpom31
9Ri0uP/5b8Zx8ZWtlT5BDoS8LvVBsOtBe788DSmmF+7hA8xLel+VRvt
mNzs/MWbKg+m0JrIdwaqhDlx+Fpc+rlhb0uWTJxKO2ruRkLcXkZ4ZbS
fnOHl2FZZgsRjnT0AbI9hHNf3clXuJsu85ymGa9ZESBvKRmYWvjpdcs
0yeCOCcxABmw==
-----END LICENSE-----)"
		);

		Version version = Version::getApplicationVersion();
		Version::setApplicationVersion(Version(2019, 1, 11, "asdfasdf"));

		TS_ASSERT(ok);
		TS_ASSERT_EQUALS(license.getUser(), "Jane Doe");
		TS_ASSERT_EQUALS(license.getType(), "Commercial License");
		TS_ASSERT_EQUALS(license.getNumberOfUsers(), 5);
		TS_ASSERT_EQUALS(license.getTimeLeft(), -2);
		TS_ASSERT(license.isComplete());
		TS_ASSERT(!license.isEmpty());
		TS_ASSERT(!license.isExpired());
		TS_ASSERT(!license.isTestLicense());

		LicenseChecker::loadPublicKey();
		TS_ASSERT_EQUALS(LicenseChecker::checkLicense(license), LicenseChecker::LicenseState::VALID);

		Version::setApplicationVersion(version);
	}

	void test_check_old_format_license_expired()
	{
		License license;
		bool ok = license.loadFromString(
R"(-----BEGIN LICENSE-----
Product: Sourcetrail
Licensed to: Jane Doe
License type: Commercial License (3 users)
Valid up to version: 2018.1
-
$9$AQAK2j5OkBlxfT/3y0XTbMUu/tRchYJxfD1dHgSFXJMXeFPN9h1N
RVewaFTRWub12WkRSq4rHaEt6MKZCtO3ntj8bGJxuvH4ggridXBoJjP
nkHGPMlUvrdbhs/O7uaWvVXf+n7YrZPxeBBdHiXEUI2AaNbig5FzCeS
Gt31yDkLjSkNwdR4zJGoAB2zWfhxFLIzBXv6z+rToBy71hzMbRHnUX2
B8zkBAt12QVCZszbV1XJ8LqOhxkGKdo6RsieBc5s2HJB16IFf0gcxkL
2FRzN6wMlTqs4K4pk8+kxwzB+Ywm+griLgzZo1jbg0JUm4Ou9nppaht
m4mgTH3FBGtob6Spi7H8hGNGgrT1gRmnHLyomwPfKXKuO/snvK3CZCB
yBAHSwjidKqg==
-----END LICENSE-----)"
		);

		Version version = Version::getApplicationVersion();
		Version::setApplicationVersion(Version(2019, 1, 11, "asdfasdf"));

		TS_ASSERT(ok);
		TS_ASSERT_EQUALS(license.getUser(), "Jane Doe");
		TS_ASSERT_EQUALS(license.getType(), "Commercial License");
		TS_ASSERT_EQUALS(license.getNumberOfUsers(), 3);
		TS_ASSERT_EQUALS(license.getTimeLeft(), -2);
		TS_ASSERT(license.isComplete());
		TS_ASSERT(!license.isEmpty());
		TS_ASSERT(license.isExpired());
		TS_ASSERT(!license.isTestLicense());

		LicenseChecker::loadPublicKey();
		TS_ASSERT_EQUALS(LicenseChecker::checkLicense(license), LicenseChecker::LicenseState::EXPIRED);

		Version::setApplicationVersion(version);
	}

	void test_check_old_format_test_license()
	{
		License license;
		bool ok = license.loadFromString(
R"(-----BEGIN LICENSE-----
Product: Sourcetrail
Licensed to: Jane Doe
License type: Test License (unlimited users)
Valid until: 2022-Jan-30
-
$9$AQAK6jsDjs5kS109CigXYuu89ixUkcRA6BAVOy4PN89wfuFDBY5v
fVaBI7FcM967hoAIM6RHO928Ba8Ct21eZtTwRmvuYpXC4xmKxw9zxEH
eAcxbDrTY+qZJLM9b3cClCHQK+PN+zjOtcEUxm/Yi05hbRv6F5Hq6P+
jv18gIfk2Jz+RnS5ev7ajENop7INhnTex6QW7B3G9W5GD89mzlNhSmQ
aKaqGeUyBEFDwBVdHDo7NlxBCbW5b/o0+xiOHsaMESINkFz7B7tPwtv
JDWHHJYC6gPoQ3E7EtsjoTsUSiNvVuxy2yTozFkCiNfrul9gHUD0Kt8
p/L7p61+0LrU2qRJmJ3j5DHfYK/nc7uIBVrT1juAxYkAtpSSd17NUZW
Yqov3eYdCVEw==
-----END LICENSE-----)"
		);

		TS_ASSERT(ok);
		TS_ASSERT_EQUALS(license.getUser(), "Jane Doe");
		TS_ASSERT_EQUALS(license.getType(), "Test License");
		TS_ASSERT_EQUALS(license.getNumberOfUsers(), 0);
		TS_ASSERT(license.getTimeLeft() >= 0);
		TS_ASSERT(license.isComplete());
		TS_ASSERT(!license.isEmpty());
		TS_ASSERT(!license.isExpired());
		TS_ASSERT(license.isTestLicense());

		LicenseChecker::loadPublicKey();
		TS_ASSERT_EQUALS(LicenseChecker::checkLicense(license), LicenseChecker::LicenseState::VALID);
	}

	void test_check_old_format_test_license_expired()
	{
		License license;
		bool ok = license.loadFromString(
R"(-----BEGIN LICENSE-----
Product: Sourcetrail
Licensed to: Jane Doe
License type: Test License (unlimited users)
Valid until: 2019-May-04
-
$9$AQAKTC9+XnNMcOoQSd3f+hMUd6zIKvtPVoMMYEC54gAWRSP3UE6r
k9wzoOr91V6HrisqtEprRyaWplxina6feelNv2XeNlFGoTpEa/3b94K
vIJFxViTRP80KPsC463Ef0a9eW6ZQiWJ+eDDUUokKtxikimb+lw3Dkd
e5Q+i6eGmarrrTi6OxEnMuNTSYS8ZpvzjhdEkvksKY+AAjQa44uwoFE
5Bda1BpZOZS7tFq9Sr+dkerAL+SDaHgx8grNuWMxb1YhZvROk7YguNL
jexviF8On4aw2R5LAjXmRDw/E1pzA/JRYfw7rLRz72/8gL034yCRUwu
HjIOkNTpWRBlXCbBZrDfXpjZrfYHLsYOZMmjwTo7I/OHrzUR0cxV6Qh
tgkMf1OQTpsw==
-----END LICENSE-----)"
		);

		TS_ASSERT(ok);
		TS_ASSERT_EQUALS(license.getUser(), "Jane Doe");
		TS_ASSERT_EQUALS(license.getType(), "Test License");
		TS_ASSERT_EQUALS(license.getNumberOfUsers(), 0);
		TS_ASSERT_EQUALS(license.getTimeLeft(), -1);
		TS_ASSERT(license.isComplete());
		TS_ASSERT(!license.isEmpty());
		TS_ASSERT(license.isExpired());
		TS_ASSERT(license.isTestLicense());

		LicenseChecker::loadPublicKey();
		TS_ASSERT_EQUALS(LicenseChecker::checkLicense(license), LicenseChecker::LicenseState::EXPIRED);
	}

	void test_check_one_hundred_year_testimonial_license()
	{
		License license;
		bool ok = license.loadFromString(
R"(-----BEGIN LICENSE-----
Product: Sourcetrail
Licensed to: Jane Doe
License type: Commercial License (1 Seat)
Valid up to version: 2117.2
-
$9$AQAKhOnW2QeCuuNbHLpUN7CT7DE/FRuOvEOEcHDNOn7yGs0CUIMS
Ox9zCpz0xH/XsNmL6NTyS3P4DOm9snvaJZM90ttr3nxy4Dv3IRWVXvg
OsvwbkYYC+N2WUKJ+ahk8zU7Uz179tdX2FLbce6/xcOi8Utv3thWWWs
EbV62Ja3DS7Qt9Oq7ui+9PrpvPmsc81yK5g993vOsZkPuOuoE8KNX1m
Dhl9UxaBZfmfQVtiAWx1vP8zCJ4nnANgfAAKPBXfYyuvf8/+GDC7CLo
9e5e0swhcMt0z0DfKdwR59sQPZnXSZyouW5GzTUi2E8nw5IStxi3/GX
z3TR0g8ERzP07KrzdYklfYv+cxtlev/H869p0USn5kPBwW8+ZFX+5Id
8CVzujNAz//w==
-----END LICENSE-----)"
		);

		Version version = Version::getApplicationVersion();
		Version::setApplicationVersion(Version(2019, 1, 11, "asdfasdf"));

		TS_ASSERT(ok);
		TS_ASSERT_EQUALS(license.getUser(), "Jane Doe");
		TS_ASSERT_EQUALS(license.getType(), "Commercial License");
		TS_ASSERT_EQUALS(license.getNumberOfUsers(), 1);
		TS_ASSERT_EQUALS(license.getTimeLeft(), -2);
		TS_ASSERT(license.isComplete());
		TS_ASSERT(!license.isEmpty());
		TS_ASSERT(!license.isExpired());
		TS_ASSERT(!license.isTestLicense());

		LicenseChecker::loadPublicKey();
		TS_ASSERT_EQUALS(LicenseChecker::checkLicense(license), LicenseChecker::LicenseState::VALID);

		Version::setApplicationVersion(version);
	}
};
