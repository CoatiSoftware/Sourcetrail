#include "catch.hpp"

#include "language_packages.h"

#if BUILD_CXX_LANGUAGE_PACKAGE

#include <fstream>

#include "ApplicationSettings.h"
#include "CxxParser.h"
#include "FileRegister.h"
#include "IndexerCommandCxx.h"
#include "IndexerStateInfo.h"
#include "ParserClientImpl.h"
#include "TestIntermediateStorage.h"
#include "TextAccess.h"
#include "TimeStamp.h"
#include "utility.h"
#include "utilityString.h"

#define REQUIRE_MESSAGE(msg, cond) do { INFO(msg); REQUIRE(cond); } while((void)0, 0)

namespace
{
	const bool updateExpectedOutput = false;
	const bool trackTime = true;
	size_t duration;

	std::shared_ptr<TextAccess> parseCode(const FilePath& sourceFilePath, const FilePath& projectDataSrcRoot)
	{
		const std::set<FilePath> indexedPaths = { projectDataSrcRoot.getCanonical() };
		const std::set<FilePathFilter> excludedFilters = {};
		const std::set<FilePathFilter> includedFilters = {};
		const FilePath workingDirectory(L".");

		std::vector<std::wstring> compilerFlags;
		utility::append(compilerFlags, IndexerCommandCxx::getCompilerFlagsForSystemHeaderSearchPaths(
			utility::concat(std::vector<FilePath> { projectDataSrcRoot }, ApplicationSettings::getInstance()->getHeaderSearchPathsExpanded())
		));
		utility::append(compilerFlags, IndexerCommandCxx::getCompilerFlagsForFrameworkSearchPaths(
			ApplicationSettings::getInstance()->getFrameworkSearchPathsExpanded()
		));
#ifdef _WIN32
		// compilerFlags.emplace_back(L"--target=x86_64-pc-windows-msvc");
#else // _WIN32
		compilerFlags.emplace_back(L"-xc++");
#endif // _WIN32
		compilerFlags.emplace_back(L"-std=c++1z");
		compilerFlags.emplace_back(sourceFilePath.wstr());

		std::shared_ptr<FileRegister> fileRegister = std::make_shared<FileRegister>(
			sourceFilePath,
			indexedPaths,
			excludedFilters
		);

		TestIntermediateStorage storage;
		CxxParser parser(std::make_shared<ParserClientImpl>(&storage), fileRegister, std::make_shared<IndexerStateInfo>());

		std::shared_ptr<IndexerCommandCxx> command = std::make_shared<IndexerCommandCxx>(
			sourceFilePath,
			indexedPaths,
			excludedFilters,
			includedFilters,
			workingDirectory,
			compilerFlags
		);

		TimeStamp startTime = TimeStamp::now();
		parser.buildIndex(command);
		duration += TimeStamp::now().deltaMS(startTime);

		storage.generateStringLists();

		return TextAccess::createFromLines(storage.m_lines);
	}

	void processSourceFile(const std::wstring& projectName, const FilePath& sourceFilePath)
	{
		const FilePath projectDataRoot = FilePath(L"data/CxxIndexSampleProjectsTestSuite/" + projectName).makeAbsolute();
		const FilePath projectDataSrcRoot = projectDataRoot.getConcatenated(L"src");
		const FilePath projectDataExpectedOutputRoot = projectDataRoot.getConcatenated(L"expected_output");

		std::shared_ptr<TextAccess> output = parseCode(projectDataSrcRoot.getConcatenated(sourceFilePath), projectDataSrcRoot);

		FilePath expectedOutputFilePath = projectDataExpectedOutputRoot.getConcatenated(utility::replace(sourceFilePath.withoutExtension().wstr() + L".txt", L"/", L"_"));
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
			REQUIRE_MESSAGE(("Output does not match the expected line count for file " + sourceFilePath.str() + " in project " + utility::encodeToUtf8(projectName)).c_str(), expectedOutput->getLineCount() == output->getLineCount());
			if (expectedOutput->getLineCount() == output->getLineCount())
			{
				for (size_t i = 1; i <= expectedOutput->getLineCount(); i++)
				{
					REQUIRE(expectedOutput->getLine(i) == output->getLine(i));
				}
			}
		}
	}

	void processSourceFiles(const std::wstring& projectName, const std::vector<FilePath>& sourceFilePaths)
	{
		duration = 0;
		for (const FilePath& filePath : sourceFilePaths)
		{
			processSourceFile(projectName, filePath);
		}
		if (trackTime)
		{
			const FilePath projectDataRoot = FilePath(L"data/CxxIndexSampleProjectsTestSuite/" + projectName).makeAbsolute();

			std::ofstream outfile;
			outfile.open(FilePath(projectDataRoot.wstr() + L"/" + projectName + L".timing").str(), std::ios_base::app);
			outfile << TimeStamp::now().toString() << " - " << duration << " ms\n";
			outfile.close();
		}
	}
}

TEST_CASE("index tictactoe project")
{
#ifdef NDEBUG
	processSourceFiles(
		L"TicTacToe",
		{
			FilePath("artificial_player.cpp"),
			FilePath("field.cpp"),
			FilePath("player.cpp"),
			FilePath("tictactoe.cpp"),
			FilePath("human_player.cpp"),
			FilePath("main.cpp"),
		}
	);
#endif // NDEBUG
}

TEST_CASE("index box2d project")
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

TEST_CASE("index bullet3 project")
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

#endif // BUILD_CXX_LANGUAGE_PACKAGE
