#include "cxxtest/TestSuite.h"

#include <fstream>
#include <iostream>

#include "data/indexer/IndexerCommandCxx.h"
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
	static const bool s_trackTime = true;

	void test_index_box2d_project()
	{
#ifdef _WIN32
#ifdef NDEBUG
		processSourceFiles(
			L"Box2D",
			{
				FilePath("Box2D/Collision/b2BroadPhase.cpp"),
				FilePath("Box2D/Collision/b2CollideCircle.cpp"),
				FilePath("Box2D/Collision/b2CollideEdge.cpp"),
				FilePath("Box2D/Collision/b2CollidePolygon.cpp"),
				FilePath("Box2D/Collision/b2Collision.cpp"),
				FilePath("Box2D/Collision/b2Distance.cpp"),
				FilePath("Box2D/Collision/b2DynamicTree.cpp"),
				FilePath("Box2D/Collision/b2TimeOfImpact.cpp"),
				FilePath("Box2D/Collision/Shapes/b2ChainShape.cpp"),
				FilePath("Box2D/Collision/Shapes/b2CircleShape.cpp"),
				FilePath("Box2D/Collision/Shapes/b2EdgeShape.cpp"),
				FilePath("Box2D/Collision/Shapes/b2PolygonShape.cpp"),
				FilePath("Box2D/Common/b2BlockAllocator.cpp"),
				FilePath("Box2D/Common/b2Draw.cpp"),
				FilePath("Box2D/Common/b2Math.cpp"),
				FilePath("Box2D/Common/b2Settings.cpp"),
				FilePath("Box2D/Common/b2StackAllocator.cpp"),
				FilePath("Box2D/Common/b2Timer.cpp"),
				FilePath("Box2D/Dynamics/b2Body.cpp"),
				FilePath("Box2D/Dynamics/b2ContactManager.cpp"),
				FilePath("Box2D/Dynamics/b2Fixture.cpp"),
				FilePath("Box2D/Dynamics/b2Island.cpp"),
				FilePath("Box2D/Dynamics/b2World.cpp"),
				FilePath("Box2D/Dynamics/b2WorldCallbacks.cpp"),
				FilePath("Box2D/Dynamics/Contacts/b2ChainAndCircleContact.cpp"),
				FilePath("Box2D/Dynamics/Contacts/b2ChainAndPolygonContact.cpp"),
				FilePath("Box2D/Dynamics/Contacts/b2CircleContact.cpp"),
				FilePath("Box2D/Dynamics/Contacts/b2Contact.cpp"),
				FilePath("Box2D/Dynamics/Contacts/b2ContactSolver.cpp"),
				FilePath("Box2D/Dynamics/Contacts/b2EdgeAndCircleContact.cpp"),
				FilePath("Box2D/Dynamics/Contacts/b2EdgeAndPolygonContact.cpp"),
				FilePath("Box2D/Dynamics/Contacts/b2PolygonAndCircleContact.cpp"),
				FilePath("Box2D/Dynamics/Contacts/b2PolygonContact.cpp"),
				FilePath("Box2D/Dynamics/Joints/b2DistanceJoint.cpp"),
				FilePath("Box2D/Dynamics/Joints/b2FrictionJoint.cpp"),
				FilePath("Box2D/Dynamics/Joints/b2GearJoint.cpp"),
				FilePath("Box2D/Dynamics/Joints/b2Joint.cpp"),
				FilePath("Box2D/Dynamics/Joints/b2MotorJoint.cpp"),
				FilePath("Box2D/Dynamics/Joints/b2MouseJoint.cpp"),
				FilePath("Box2D/Dynamics/Joints/b2PrismaticJoint.cpp"),
				FilePath("Box2D/Dynamics/Joints/b2PulleyJoint.cpp"),
				FilePath("Box2D/Dynamics/Joints/b2RevoluteJoint.cpp"),
				FilePath("Box2D/Dynamics/Joints/b2RopeJoint.cpp"),
				FilePath("Box2D/Dynamics/Joints/b2WeldJoint.cpp"),
				FilePath("Box2D/Dynamics/Joints/b2WheelJoint.cpp"),
				FilePath("Box2D/Rope/b2Rope.cpp")
			}
		);
#endif // NDEBUG
#endif // _WIN32
	}

	void test_index_bullet3_project()
	{
#ifdef _WIN32
#ifdef NDEBUG
		processSourceFiles(
			L"Bullet3",
			{
				FilePath("Bullet3Collision/BroadPhaseCollision/b3DynamicBvh.cpp"),
				FilePath("Bullet3Collision/BroadPhaseCollision/b3DynamicBvhBroadphase.cpp"),
				FilePath("Bullet3Collision/BroadPhaseCollision/b3OverlappingPairCache.cpp"),
				FilePath("Bullet3Collision/NarrowPhaseCollision/b3ConvexUtility.cpp"),
				FilePath("Bullet3Collision/NarrowPhaseCollision/b3CpuNarrowPhase.cpp"),
				FilePath("Bullet3Common/b3AlignedAllocator.cpp"),
				FilePath("Bullet3Common/b3Logging.cpp"),
				FilePath("Bullet3Common/b3Vector3.cpp"),
				FilePath("Bullet3Geometry/b3ConvexHullComputer.cpp"),
				FilePath("Bullet3Geometry/b3GeometryUtil.cpp")
			}
		);
#endif // NDEBUG
#endif // _WIN32
	}

private:

	void processSourceFiles(const std::wstring& projectName, const std::vector<FilePath>& sourceFilePaths)
	{
		m_duration = 0;
		for (const FilePath& filePath : sourceFilePaths)
		{
			processSourceFile(projectName, filePath);
		}
		if (s_trackTime)
		{
			const FilePath projectDataRoot = FilePath(L"data/CxxIndexSampleProjectsTestSuite/" + projectName).makeAbsolute();

			std::ofstream outfile;
			outfile.open(FilePath(projectDataRoot.wstr() + L"/" + projectName + L".timing").str(), std::ios_base::app);
			outfile << TimeStamp::now().toString() << " - " << m_duration << " ms\n";
			outfile.close();
		}
	}

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
		const std::set<FilePathFilter> excludedFilters = {};
		const std::set<FilePathFilter> includedFilters = {};
		const FilePath workingDirectory(L".");

		std::shared_ptr<FileRegister> fileRegister = std::make_shared<FileRegister>(
			sourceFilePath,
			indexedPaths,
			excludedFilters
		);

		std::shared_ptr<DumpParserClient> parserClient = std::make_shared<DumpParserClient>();

		CxxParser parser(parserClient, fileRegister);

		std::shared_ptr<IndexerCommandCxx> command = std::make_shared<IndexerCommandCxx>(
			sourceFilePath,
			indexedPaths,
			excludedFilters,
			includedFilters,
			workingDirectory,
			utility::concat(std::vector<FilePath> { projectDataSrcRoot }, ApplicationSettings::getInstance()->getHeaderSearchPathsExpanded()),
			ApplicationSettings::getInstance()->getFrameworkSearchPathsExpanded(),
			std::vector<std::wstring> { L"--target=x86_64-pc-windows-msvc", L"-std=c++1z", sourceFilePath.wstr() }
		);

		TimeStamp startTime = TimeStamp::now();
		parser.buildIndex(command);
		m_duration += TimeStamp::now().deltaMS(startTime);

		return TextAccess::createFromString(utility::encodeToUtf8(parserClient->m_lines));
	}

	size_t m_duration;
};
