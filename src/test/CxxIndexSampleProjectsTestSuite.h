#include "cxxtest/TestSuite.h"

#include <fstream>
#include <iostream>

#include "data/indexer/IndexerCommandCxxEmpty.h"
#include "data/parser/cxx/CxxParser.h"
#include "helper/DumpParserClient.h"
#include "settings/ApplicationSettings.h"
#include "utility/file/FileRegister.h"
#include "utility/text/TextAccess.h"
#include "utility/utility.h"
#include "utility/utilityString.h"

class CxxIndexSampleProjectsTestSuite : public CxxTest::TestSuite
{
public:
	static const bool s_updateExpectedOutput = false;

	void test_index_box2d_project()
	{
#ifdef _WIN32
#ifdef NDEBUG
		processSourceFile(L"Box2D", FilePath("Box2D/Collision/b2BroadPhase.cpp"));
		processSourceFile(L"Box2D", FilePath("Box2D/Collision/b2CollideCircle.cpp"));
		processSourceFile(L"Box2D", FilePath("Box2D/Collision/b2CollideEdge.cpp"));
		processSourceFile(L"Box2D", FilePath("Box2D/Collision/b2CollidePolygon.cpp"));
		processSourceFile(L"Box2D", FilePath("Box2D/Collision/b2Collision.cpp"));
		processSourceFile(L"Box2D", FilePath("Box2D/Collision/b2Distance.cpp"));
		processSourceFile(L"Box2D", FilePath("Box2D/Collision/b2DynamicTree.cpp"));
		processSourceFile(L"Box2D", FilePath("Box2D/Collision/b2TimeOfImpact.cpp"));
		processSourceFile(L"Box2D", FilePath("Box2D/Collision/Shapes/b2ChainShape.cpp"));
		processSourceFile(L"Box2D", FilePath("Box2D/Collision/Shapes/b2CircleShape.cpp"));
		processSourceFile(L"Box2D", FilePath("Box2D/Collision/Shapes/b2EdgeShape.cpp"));
		processSourceFile(L"Box2D", FilePath("Box2D/Collision/Shapes/b2PolygonShape.cpp"));
		processSourceFile(L"Box2D", FilePath("Box2D/Common/b2BlockAllocator.cpp"));
		processSourceFile(L"Box2D", FilePath("Box2D/Common/b2Draw.cpp"));
		processSourceFile(L"Box2D", FilePath("Box2D/Common/b2Math.cpp"));
		processSourceFile(L"Box2D", FilePath("Box2D/Common/b2Settings.cpp"));
		processSourceFile(L"Box2D", FilePath("Box2D/Common/b2StackAllocator.cpp"));
		processSourceFile(L"Box2D", FilePath("Box2D/Common/b2Timer.cpp"));
		processSourceFile(L"Box2D", FilePath("Box2D/Dynamics/b2Body.cpp"));
		processSourceFile(L"Box2D", FilePath("Box2D/Dynamics/b2ContactManager.cpp"));
		processSourceFile(L"Box2D", FilePath("Box2D/Dynamics/b2Fixture.cpp"));
		processSourceFile(L"Box2D", FilePath("Box2D/Dynamics/b2Island.cpp"));
		processSourceFile(L"Box2D", FilePath("Box2D/Dynamics/b2World.cpp"));
		processSourceFile(L"Box2D", FilePath("Box2D/Dynamics/b2WorldCallbacks.cpp"));
		processSourceFile(L"Box2D", FilePath("Box2D/Dynamics/Contacts/b2ChainAndCircleContact.cpp"));
		processSourceFile(L"Box2D", FilePath("Box2D/Dynamics/Contacts/b2ChainAndPolygonContact.cpp"));
		processSourceFile(L"Box2D", FilePath("Box2D/Dynamics/Contacts/b2CircleContact.cpp"));
		processSourceFile(L"Box2D", FilePath("Box2D/Dynamics/Contacts/b2Contact.cpp"));
		processSourceFile(L"Box2D", FilePath("Box2D/Dynamics/Contacts/b2ContactSolver.cpp"));
		processSourceFile(L"Box2D", FilePath("Box2D/Dynamics/Contacts/b2EdgeAndCircleContact.cpp"));
		processSourceFile(L"Box2D", FilePath("Box2D/Dynamics/Contacts/b2EdgeAndPolygonContact.cpp"));
		processSourceFile(L"Box2D", FilePath("Box2D/Dynamics/Contacts/b2PolygonAndCircleContact.cpp"));
		processSourceFile(L"Box2D", FilePath("Box2D/Dynamics/Contacts/b2PolygonContact.cpp"));
		processSourceFile(L"Box2D", FilePath("Box2D/Dynamics/Joints/b2DistanceJoint.cpp"));
		processSourceFile(L"Box2D", FilePath("Box2D/Dynamics/Joints/b2FrictionJoint.cpp"));
		processSourceFile(L"Box2D", FilePath("Box2D/Dynamics/Joints/b2GearJoint.cpp"));
		processSourceFile(L"Box2D", FilePath("Box2D/Dynamics/Joints/b2Joint.cpp"));
		processSourceFile(L"Box2D", FilePath("Box2D/Dynamics/Joints/b2MotorJoint.cpp"));
		processSourceFile(L"Box2D", FilePath("Box2D/Dynamics/Joints/b2MouseJoint.cpp"));
		processSourceFile(L"Box2D", FilePath("Box2D/Dynamics/Joints/b2PrismaticJoint.cpp"));
		processSourceFile(L"Box2D", FilePath("Box2D/Dynamics/Joints/b2PulleyJoint.cpp"));
		processSourceFile(L"Box2D", FilePath("Box2D/Dynamics/Joints/b2RevoluteJoint.cpp"));
		processSourceFile(L"Box2D", FilePath("Box2D/Dynamics/Joints/b2RopeJoint.cpp"));
		processSourceFile(L"Box2D", FilePath("Box2D/Dynamics/Joints/b2WeldJoint.cpp"));
		processSourceFile(L"Box2D", FilePath("Box2D/Dynamics/Joints/b2WheelJoint.cpp"));
		processSourceFile(L"Box2D", FilePath("Box2D/Rope/b2Rope.cpp"));
#endif // NDEBUG
#endif // _WIN32
	}

	void test_index_bullet3_project()
	{
#ifdef _WIN32
#ifdef NDEBUG
		processSourceFile(L"Bullet3", FilePath("Bullet3Collision/BroadPhaseCollision/b3DynamicBvh.cpp"));
		processSourceFile(L"Bullet3", FilePath("Bullet3Collision/BroadPhaseCollision/b3DynamicBvhBroadphase.cpp"));
		processSourceFile(L"Bullet3", FilePath("Bullet3Collision/BroadPhaseCollision/b3OverlappingPairCache.cpp"));
		processSourceFile(L"Bullet3", FilePath("Bullet3Collision/NarrowPhaseCollision/b3ConvexUtility.cpp"));
		processSourceFile(L"Bullet3", FilePath("Bullet3Collision/NarrowPhaseCollision/b3CpuNarrowPhase.cpp"));
		processSourceFile(L"Bullet3", FilePath("Bullet3Common/b3AlignedAllocator.cpp"));
		processSourceFile(L"Bullet3", FilePath("Bullet3Common/b3Logging.cpp"));
		processSourceFile(L"Bullet3", FilePath("Bullet3Common/b3Vector3.cpp"));
		processSourceFile(L"Bullet3", FilePath("Bullet3Geometry/b3ConvexHullComputer.cpp"));
		processSourceFile(L"Bullet3", FilePath("Bullet3Geometry/b3GeometryUtil.cpp"));
#endif // NDEBUG
#endif // _WIN32
	}

private:
	void processSourceFile(const std::wstring& projectName, const FilePath& sourceFilePath)
	{
		const FilePath projectDataRoot = FilePath(L"data/CxxIndexSampleProjectsTestSuite/" + projectName).makeAbsolute();
		const FilePath projectDataSrcRoot = projectDataRoot.getConcatenated(L"src");
		const FilePath projectDataExpectedOutputRoot = projectDataRoot.getConcatenated(L"expected_output");

		std::shared_ptr<TextAccess> output = parseCode(projectDataSrcRoot.getConcatenated(sourceFilePath), projectDataSrcRoot);

		FilePath expectedOutputFilePath = projectDataExpectedOutputRoot.getConcatenated(utility::replace(sourceFilePath.withoutExtension().wstr() + L".txt", L"/", L"_"));
		if (s_updateExpectedOutput || !expectedOutputFilePath.exists())
		{
			std::ofstream expectedOutputFile;
			expectedOutputFile.open(expectedOutputFilePath.str());
			expectedOutputFile << output->getText();
			expectedOutputFile.close();
		}
		else
		{
			std::shared_ptr<TextAccess> expectedOutput = TextAccess::createFromFile(expectedOutputFilePath);
			TSM_ASSERT_EQUALS(L"Output does not match the expected line count for file " + sourceFilePath.wstr() + L" in project " + projectName, expectedOutput->getLineCount(), output->getLineCount());
			if (expectedOutput->getLineCount() == output->getLineCount())
			{
				for (size_t i = 1; i <= expectedOutput->getLineCount(); i++)
				{
					TS_ASSERT_EQUALS(expectedOutput->getLine(i), output->getLine(i));
				}
			}
		}
	}

	std::shared_ptr<TextAccess> parseCode(const FilePath& sourceFilePath, const FilePath& projectDataSrcRoot)
	{
		const std::set<FilePath> indexedPaths = { projectDataSrcRoot };
		const std::set<FilePath> excludedPaths = {};
		const FilePath workingDirectory(L".");

		std::shared_ptr<FileRegister> fileRegister = std::make_shared<FileRegister>(
			FileRegisterStateData(),
			sourceFilePath,
			indexedPaths,
			excludedPaths
		);

		std::shared_ptr<DumpParserClient> parserClient = std::make_shared<DumpParserClient>();

		CxxParser parser(parserClient, fileRegister);

		std::shared_ptr<IndexerCommandCxxEmpty> command = std::make_shared<IndexerCommandCxxEmpty>(
			sourceFilePath, 
			indexedPaths,
			excludedPaths,
			workingDirectory,
			"c++1z", 
			utility::concat(std::vector<FilePath> { projectDataSrcRoot }, ApplicationSettings::getInstance()->getHeaderSearchPathsExpanded()),
			ApplicationSettings::getInstance()->getFrameworkSearchPathsExpanded(), 
			std::vector<std::string> { "--target=x86_64-pc-windows-msvc" }
		);

		parser.buildIndex(command);

		return TextAccess::createFromString(utility::encodeToUtf8(parserClient->m_lines));
	}
};