#include "cxxtest/TestSuite.h"

#include <fstream>
#include <iostream>

#include "data/indexer/IndexerCommandCxxManual.h"
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
	void test_index_box2d_project()
	{
#ifdef _WIN32
		const bool updateExpectedOutput = false;

		processSourceFile("Box2D", FilePath("Box2D/Collision/b2BroadPhase.cpp"), updateExpectedOutput);
		processSourceFile("Box2D", FilePath("Box2D/Collision/b2CollideCircle.cpp"), updateExpectedOutput);
		processSourceFile("Box2D", FilePath("Box2D/Collision/b2CollideEdge.cpp"), updateExpectedOutput);
		processSourceFile("Box2D", FilePath("Box2D/Collision/b2CollidePolygon.cpp"), updateExpectedOutput);
		processSourceFile("Box2D", FilePath("Box2D/Collision/b2Collision.cpp"), updateExpectedOutput);
		processSourceFile("Box2D", FilePath("Box2D/Collision/b2Distance.cpp"), updateExpectedOutput);
		processSourceFile("Box2D", FilePath("Box2D/Collision/b2DynamicTree.cpp"), updateExpectedOutput);
		processSourceFile("Box2D", FilePath("Box2D/Collision/b2TimeOfImpact.cpp"), updateExpectedOutput);
		processSourceFile("Box2D", FilePath("Box2D/Collision/Shapes/b2ChainShape.cpp"), updateExpectedOutput);
		processSourceFile("Box2D", FilePath("Box2D/Collision/Shapes/b2CircleShape.cpp"), updateExpectedOutput);
		processSourceFile("Box2D", FilePath("Box2D/Collision/Shapes/b2EdgeShape.cpp"), updateExpectedOutput);
		processSourceFile("Box2D", FilePath("Box2D/Collision/Shapes/b2PolygonShape.cpp"), updateExpectedOutput);
		processSourceFile("Box2D", FilePath("Box2D/Common/b2BlockAllocator.cpp"), updateExpectedOutput);
		processSourceFile("Box2D", FilePath("Box2D/Common/b2Draw.cpp"), updateExpectedOutput);
		processSourceFile("Box2D", FilePath("Box2D/Common/b2Math.cpp"), updateExpectedOutput);
		processSourceFile("Box2D", FilePath("Box2D/Common/b2Settings.cpp"), updateExpectedOutput);
		processSourceFile("Box2D", FilePath("Box2D/Common/b2StackAllocator.cpp"), updateExpectedOutput);
		processSourceFile("Box2D", FilePath("Box2D/Common/b2Timer.cpp"), updateExpectedOutput);
		processSourceFile("Box2D", FilePath("Box2D/Dynamics/b2Body.cpp"), updateExpectedOutput);
		processSourceFile("Box2D", FilePath("Box2D/Dynamics/b2ContactManager.cpp"), updateExpectedOutput);
		processSourceFile("Box2D", FilePath("Box2D/Dynamics/b2Fixture.cpp"), updateExpectedOutput);
		processSourceFile("Box2D", FilePath("Box2D/Dynamics/b2Island.cpp"), updateExpectedOutput);
		processSourceFile("Box2D", FilePath("Box2D/Dynamics/b2World.cpp"), updateExpectedOutput);
		processSourceFile("Box2D", FilePath("Box2D/Dynamics/b2WorldCallbacks.cpp"), updateExpectedOutput);
		processSourceFile("Box2D", FilePath("Box2D/Dynamics/Contacts/b2ChainAndCircleContact.cpp"), updateExpectedOutput);
		processSourceFile("Box2D", FilePath("Box2D/Dynamics/Contacts/b2ChainAndPolygonContact.cpp"), updateExpectedOutput);
		processSourceFile("Box2D", FilePath("Box2D/Dynamics/Contacts/b2CircleContact.cpp"), updateExpectedOutput);
		processSourceFile("Box2D", FilePath("Box2D/Dynamics/Contacts/b2Contact.cpp"), updateExpectedOutput);
		processSourceFile("Box2D", FilePath("Box2D/Dynamics/Contacts/b2ContactSolver.cpp"), updateExpectedOutput);
		processSourceFile("Box2D", FilePath("Box2D/Dynamics/Contacts/b2EdgeAndCircleContact.cpp"), updateExpectedOutput);
		processSourceFile("Box2D", FilePath("Box2D/Dynamics/Contacts/b2EdgeAndPolygonContact.cpp"), updateExpectedOutput);
		processSourceFile("Box2D", FilePath("Box2D/Dynamics/Contacts/b2PolygonAndCircleContact.cpp"), updateExpectedOutput);
		processSourceFile("Box2D", FilePath("Box2D/Dynamics/Contacts/b2PolygonContact.cpp"), updateExpectedOutput);
		processSourceFile("Box2D", FilePath("Box2D/Dynamics/Joints/b2DistanceJoint.cpp"), updateExpectedOutput);
		processSourceFile("Box2D", FilePath("Box2D/Dynamics/Joints/b2FrictionJoint.cpp"), updateExpectedOutput);
		processSourceFile("Box2D", FilePath("Box2D/Dynamics/Joints/b2GearJoint.cpp"), updateExpectedOutput);
		processSourceFile("Box2D", FilePath("Box2D/Dynamics/Joints/b2Joint.cpp"), updateExpectedOutput);
		processSourceFile("Box2D", FilePath("Box2D/Dynamics/Joints/b2MotorJoint.cpp"), updateExpectedOutput);
		processSourceFile("Box2D", FilePath("Box2D/Dynamics/Joints/b2MouseJoint.cpp"), updateExpectedOutput);
		processSourceFile("Box2D", FilePath("Box2D/Dynamics/Joints/b2PrismaticJoint.cpp"), updateExpectedOutput);
		processSourceFile("Box2D", FilePath("Box2D/Dynamics/Joints/b2PulleyJoint.cpp"), updateExpectedOutput);
		processSourceFile("Box2D", FilePath("Box2D/Dynamics/Joints/b2RevoluteJoint.cpp"), updateExpectedOutput);
		processSourceFile("Box2D", FilePath("Box2D/Dynamics/Joints/b2RopeJoint.cpp"), updateExpectedOutput);
		processSourceFile("Box2D", FilePath("Box2D/Dynamics/Joints/b2WeldJoint.cpp"), updateExpectedOutput);
		processSourceFile("Box2D", FilePath("Box2D/Dynamics/Joints/b2WheelJoint.cpp"), updateExpectedOutput);
		processSourceFile("Box2D", FilePath("Box2D/Rope/b2Rope.cpp"), updateExpectedOutput);
#endif
	}

	void test_index_bullet3_project()
	{
#ifdef _WIN32
		const bool updateExpectedOutput = false;

		processSourceFile("Bullet3", FilePath("Bullet3Collision/BroadPhaseCollision/b3DynamicBvh.cpp"), updateExpectedOutput);
		processSourceFile("Bullet3", FilePath("Bullet3Collision/BroadPhaseCollision/b3DynamicBvhBroadphase.cpp"), updateExpectedOutput);
		processSourceFile("Bullet3", FilePath("Bullet3Collision/BroadPhaseCollision/b3OverlappingPairCache.cpp"), updateExpectedOutput);
		processSourceFile("Bullet3", FilePath("Bullet3Collision/NarrowPhaseCollision/b3ConvexUtility.cpp"), updateExpectedOutput);
		processSourceFile("Bullet3", FilePath("Bullet3Collision/NarrowPhaseCollision/b3CpuNarrowPhase.cpp"), updateExpectedOutput);
		processSourceFile("Bullet3", FilePath("Bullet3Common/b3AlignedAllocator.cpp"), updateExpectedOutput);
		processSourceFile("Bullet3", FilePath("Bullet3Common/b3Logging.cpp"), updateExpectedOutput);
		processSourceFile("Bullet3", FilePath("Bullet3Common/b3Vector3.cpp"), updateExpectedOutput);
		processSourceFile("Bullet3", FilePath("Bullet3Geometry/b3ConvexHullComputer.cpp"), updateExpectedOutput);
		processSourceFile("Bullet3", FilePath("Bullet3Geometry/b3GeometryUtil.cpp"), updateExpectedOutput);
#endif
	}

private:
	void processSourceFile(const std::string& projectName, const FilePath& sourceFilePath, const bool updateExpectedOutput)
	{
		const FilePath projectDataRoot = FilePath("data/CxxIndexSampleProjectsTestSuite/" + projectName);
		const FilePath projectDataSrcRoot = projectDataRoot.concat(FilePath("src"));
		const FilePath projectDataExpectedOutputRoot = projectDataRoot.concat(FilePath("expected_output"));

		std::shared_ptr<TextAccess> output = parseCode(projectDataSrcRoot.concat(sourceFilePath), projectDataSrcRoot);

		FilePath expectedOutputFilePath = projectDataExpectedOutputRoot.concat(FilePath(utility::replace(sourceFilePath.withoutExtension().str() + ".txt", "/", "_")));
		if (updateExpectedOutput || !expectedOutputFilePath.exists())
		{
			std::ofstream expectedOutputFile;
			expectedOutputFile.open(expectedOutputFilePath.str());
			expectedOutputFile << output->getText();
			expectedOutputFile.close();
		}
		else
		{
			std::shared_ptr<TextAccess> expectedOutput = TextAccess::createFromFile(expectedOutputFilePath);
			TSM_ASSERT_EQUALS("Output does not match the expected line count for file " + sourceFilePath.str() + " in project " + projectName, expectedOutput->getLineCount(), output->getLineCount());
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
		std::set<FilePath> indexedPaths = { projectDataSrcRoot };
		std::set<FilePath> excludedPaths = { };

		std::shared_ptr<FileRegister> fileRegister = std::make_shared<FileRegister>(
			FileRegisterStateData(),
			sourceFilePath,
			indexedPaths,
			excludedPaths
		);

		std::shared_ptr<DumpParserClient> parserClient = std::make_shared<DumpParserClient>();

		CxxParser parser(parserClient, fileRegister);

		std::shared_ptr<IndexerCommandCxxManual> command = std::make_shared<IndexerCommandCxxManual>(
			sourceFilePath, 
			indexedPaths,
			excludedPaths,
			"c++1z", 
			utility::concat(std::vector<FilePath> { projectDataSrcRoot }, ApplicationSettings::getInstance()->getHeaderSearchPathsExpanded()),
			ApplicationSettings::getInstance()->getFrameworkSearchPathsExpanded(), 
			std::vector<std::string> { "--target=x86_64-pc-windows-msvc" },
			true
		);

		parser.buildIndex(command);

		return TextAccess::createFromString(parserClient->m_lines);
	}
};