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
	static const bool s_updateExpectedOutput = false;

	void test_index_box2d_project()
	{
#ifdef _WIN32
		processSourceFile("Box2D", FilePath("Box2D/Collision/b2BroadPhase.cpp"));
		processSourceFile("Box2D", FilePath("Box2D/Collision/b2CollideCircle.cpp"));
		processSourceFile("Box2D", FilePath("Box2D/Collision/b2CollideEdge.cpp"));
		processSourceFile("Box2D", FilePath("Box2D/Collision/b2CollidePolygon.cpp"));
		processSourceFile("Box2D", FilePath("Box2D/Collision/b2Collision.cpp"));
		processSourceFile("Box2D", FilePath("Box2D/Collision/b2Distance.cpp"));
		processSourceFile("Box2D", FilePath("Box2D/Collision/b2DynamicTree.cpp"));
		processSourceFile("Box2D", FilePath("Box2D/Collision/b2TimeOfImpact.cpp"));
		processSourceFile("Box2D", FilePath("Box2D/Collision/Shapes/b2ChainShape.cpp"));
		processSourceFile("Box2D", FilePath("Box2D/Collision/Shapes/b2CircleShape.cpp"));
		processSourceFile("Box2D", FilePath("Box2D/Collision/Shapes/b2EdgeShape.cpp"));
		processSourceFile("Box2D", FilePath("Box2D/Collision/Shapes/b2PolygonShape.cpp"));
		processSourceFile("Box2D", FilePath("Box2D/Common/b2BlockAllocator.cpp"));
		processSourceFile("Box2D", FilePath("Box2D/Common/b2Draw.cpp"));
		processSourceFile("Box2D", FilePath("Box2D/Common/b2Math.cpp"));
		processSourceFile("Box2D", FilePath("Box2D/Common/b2Settings.cpp"));
		processSourceFile("Box2D", FilePath("Box2D/Common/b2StackAllocator.cpp"));
		processSourceFile("Box2D", FilePath("Box2D/Common/b2Timer.cpp"));
		processSourceFile("Box2D", FilePath("Box2D/Dynamics/b2Body.cpp"));
		processSourceFile("Box2D", FilePath("Box2D/Dynamics/b2ContactManager.cpp"));
		processSourceFile("Box2D", FilePath("Box2D/Dynamics/b2Fixture.cpp"));
		processSourceFile("Box2D", FilePath("Box2D/Dynamics/b2Island.cpp"));
		processSourceFile("Box2D", FilePath("Box2D/Dynamics/b2World.cpp"));
		processSourceFile("Box2D", FilePath("Box2D/Dynamics/b2WorldCallbacks.cpp"));
		processSourceFile("Box2D", FilePath("Box2D/Dynamics/Contacts/b2ChainAndCircleContact.cpp"));
		processSourceFile("Box2D", FilePath("Box2D/Dynamics/Contacts/b2ChainAndPolygonContact.cpp"));
		processSourceFile("Box2D", FilePath("Box2D/Dynamics/Contacts/b2CircleContact.cpp"));
		processSourceFile("Box2D", FilePath("Box2D/Dynamics/Contacts/b2Contact.cpp"));
		processSourceFile("Box2D", FilePath("Box2D/Dynamics/Contacts/b2ContactSolver.cpp"));
		processSourceFile("Box2D", FilePath("Box2D/Dynamics/Contacts/b2EdgeAndCircleContact.cpp"));
		processSourceFile("Box2D", FilePath("Box2D/Dynamics/Contacts/b2EdgeAndPolygonContact.cpp"));
		processSourceFile("Box2D", FilePath("Box2D/Dynamics/Contacts/b2PolygonAndCircleContact.cpp"));
		processSourceFile("Box2D", FilePath("Box2D/Dynamics/Contacts/b2PolygonContact.cpp"));
		processSourceFile("Box2D", FilePath("Box2D/Dynamics/Joints/b2DistanceJoint.cpp"));
		processSourceFile("Box2D", FilePath("Box2D/Dynamics/Joints/b2FrictionJoint.cpp"));
		processSourceFile("Box2D", FilePath("Box2D/Dynamics/Joints/b2GearJoint.cpp"));
		processSourceFile("Box2D", FilePath("Box2D/Dynamics/Joints/b2Joint.cpp"));
		processSourceFile("Box2D", FilePath("Box2D/Dynamics/Joints/b2MotorJoint.cpp"));
		processSourceFile("Box2D", FilePath("Box2D/Dynamics/Joints/b2MouseJoint.cpp"));
		processSourceFile("Box2D", FilePath("Box2D/Dynamics/Joints/b2PrismaticJoint.cpp"));
		processSourceFile("Box2D", FilePath("Box2D/Dynamics/Joints/b2PulleyJoint.cpp"));
		processSourceFile("Box2D", FilePath("Box2D/Dynamics/Joints/b2RevoluteJoint.cpp"));
		processSourceFile("Box2D", FilePath("Box2D/Dynamics/Joints/b2RopeJoint.cpp"));
		processSourceFile("Box2D", FilePath("Box2D/Dynamics/Joints/b2WeldJoint.cpp"));
		processSourceFile("Box2D", FilePath("Box2D/Dynamics/Joints/b2WheelJoint.cpp"));
		processSourceFile("Box2D", FilePath("Box2D/Rope/b2Rope.cpp"));
#endif
	}

	void test_index_bullet3_project()
	{
#ifdef _WIN32
		processSourceFile("Bullet3", FilePath("Bullet3Collision/BroadPhaseCollision/b3DynamicBvh.cpp"));
		processSourceFile("Bullet3", FilePath("Bullet3Collision/BroadPhaseCollision/b3DynamicBvhBroadphase.cpp"));
		processSourceFile("Bullet3", FilePath("Bullet3Collision/BroadPhaseCollision/b3OverlappingPairCache.cpp"));
		processSourceFile("Bullet3", FilePath("Bullet3Collision/NarrowPhaseCollision/b3ConvexUtility.cpp"));
		processSourceFile("Bullet3", FilePath("Bullet3Collision/NarrowPhaseCollision/b3CpuNarrowPhase.cpp"));
		processSourceFile("Bullet3", FilePath("Bullet3Common/b3AlignedAllocator.cpp"));
		processSourceFile("Bullet3", FilePath("Bullet3Common/b3Logging.cpp"));
		processSourceFile("Bullet3", FilePath("Bullet3Common/b3Vector3.cpp"));
		processSourceFile("Bullet3", FilePath("Bullet3Geometry/b3ConvexHullComputer.cpp"));
		processSourceFile("Bullet3", FilePath("Bullet3Geometry/b3GeometryUtil.cpp"));
#endif
	}

private:
	void processSourceFile(const std::string& projectName, const FilePath& sourceFilePath)
	{
		const FilePath projectDataRoot = FilePath("data/CxxIndexSampleProjectsTestSuite/" + projectName).absolute();
		const FilePath projectDataSrcRoot = projectDataRoot.concat(FilePath("src"));
		const FilePath projectDataExpectedOutputRoot = projectDataRoot.concat(FilePath("expected_output"));

		std::shared_ptr<TextAccess> output = parseCode(projectDataSrcRoot.concat(sourceFilePath), projectDataSrcRoot);

		FilePath expectedOutputFilePath = projectDataExpectedOutputRoot.concat(FilePath(utility::replace(sourceFilePath.withoutExtension().str() + ".txt", "/", "_")));
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