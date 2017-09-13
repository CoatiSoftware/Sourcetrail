#include "cxxtest/TestSuite.h"

#include <fstream>
#include <iostream>

#include "settings/ApplicationSettings.h"


#include "utility/text/TextAccess.h"
#include "utility/utility.h"
#include "utility/utilityString.h"

#include "data/indexer/IndexerCommandCxxManual.h"
#include "data/parser/cxx/CxxParser.h"

#include "helper/TestFileRegister.h"
#include "helper/TestParserClient.h"

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
	class DumpParserClient : public ParserClient
	{
	public:
		DumpParserClient()
			: m_dump("")
		{
		}

		virtual Id recordSymbol(
			const NameHierarchy& symbolName, SymbolKind symbolKind,
			AccessKind access, DefinitionKind definitionKind)
		{
			m_dump += symbolKindToString(symbolKind) + " " + addAccessPrefix(symbolName.getQualifiedNameWithSignature(), access) + "\n";
			return 0;
		}

		virtual Id recordSymbol(
			const NameHierarchy& symbolName, SymbolKind symbolKind,
			const ParseLocation& location,
			AccessKind access, DefinitionKind definitionKind)
		{
			m_dump += symbolKindToString(symbolKind) + " " + addLocationSuffix(addAccessPrefix(symbolName.getQualifiedNameWithSignature(), access) + " [" + location.filePath.fileName(), location) + "]\n";
			return 0;
		}

		virtual Id recordSymbol(
			const NameHierarchy& symbolName, SymbolKind symbolKind,
			const ParseLocation& location, const ParseLocation& scopeLocation,
			AccessKind access, DefinitionKind definitionKind)
		{
			m_dump += symbolKindToString(symbolKind) + " " + addLocationSuffix(addAccessPrefix(symbolName.getQualifiedNameWithSignature(), access) + " [" + location.filePath.fileName(), location, scopeLocation) + "]\n";
			return 0;
		}

		void recordReference(
			ReferenceKind referenceKind, const NameHierarchy& referencedName, const NameHierarchy& contextName,
			const ParseLocation& location)
		{
			std::string contextNameString = contextName.getQualifiedNameWithSignature();
			if (FilePath(contextNameString).exists())
			{
				contextNameString = FilePath(contextNameString).fileName();
			}
			m_dump += referenceKindToString(referenceKind) + " " + addLocationSuffix(contextNameString + " -> " + referencedName.getQualifiedNameWithSignature() + " [" + location.filePath.fileName(), location) + "]\n";
		}

		virtual void onError(const ParseLocation& location, const std::string& message, const std::string& commandline,
			bool fatal, bool indexed)
		{
			m_dump += "ERROR: " + addLocationSuffix(message + " [" + location.filePath.fileName(), location) + "]\n";
		}

		virtual void onLocalSymbolParsed(const std::string& name, const ParseLocation& location)
		{
			m_dump += "LOCAL_SYMBOL: " + addLocationSuffix(name + " [" + location.filePath.fileName(), location) + "]\n";
		}

		virtual void onFileParsed(const FileInfo& fileInfo)
		{
			m_dump += "FILE: " + fileInfo.path.fileName() + "\n";
		}

		virtual void onCommentParsed(const ParseLocation& location)
		{
			m_dump += "COMMENT: " + addLocationSuffix("comment [" + location.filePath.fileName(), location) + "]\n";
		}

		std::string m_dump;

	private:
		std::string symbolKindToString(SymbolKind symbolKind) const
		{
			switch (symbolKind)
			{
			case SYMBOL_BUILTIN_TYPE:
				return "SYMBOL_BUILTIN_TYPE";
			case SYMBOL_CLASS:
				return "SYMBOL_CLASS";
			case SYMBOL_ENUM:
				return "SYMBOL_ENUM";
			case SYMBOL_ENUM_CONSTANT:
				return "SYMBOL_ENUM_CONSTANT";
			case SYMBOL_FIELD:
				return "SYMBOL_FIELD";
			case SYMBOL_FUNCTION:
				return "SYMBOL_FUNCTION";
			case SYMBOL_GLOBAL_VARIABLE:
				return "SYMBOL_GLOBAL_VARIABLE";
			case SYMBOL_INTERFACE:
				return "SYMBOL_INTERFACE";
			case SYMBOL_MACRO:
				return "SYMBOL_MACRO";
			case SYMBOL_METHOD:
				return "SYMBOL_METHOD";
			case SYMBOL_NAMESPACE:
				return "SYMBOL_NAMESPACE";
			case SYMBOL_PACKAGE:
				return "SYMBOL_PACKAGE";
			case SYMBOL_STRUCT:
				return "SYMBOL_STRUCT";
			case SYMBOL_TEMPLATE_PARAMETER:
				return "SYMBOL_TEMPLATE_PARAMETER";
			case SYMBOL_TYPEDEF:
				return "SYMBOL_TYPEDEF";
			case SYMBOL_TYPE_PARAMETER:
				return "SYMBOL_TYPE_PARAMETER";
			case SYMBOL_UNION:
				return "SYMBOL_UNION";
			default:
				break;
			}
			return "SYMBOL_NON_INDEXED";
		}

		std::string referenceKindToString(ReferenceKind referenceKind) const
		{
			switch (referenceKind)
			{
			case REFERENCE_TYPE_USAGE:
				return "REFERENCE_TYPE_USAGE";
			case REFERENCE_USAGE:
				return "REFERENCE_USAGE";
			case REFERENCE_CALL:
				return "REFERENCE_CALL";
			case REFERENCE_INHERITANCE:
				return "REFERENCE_INHERITANCE";
			case REFERENCE_OVERRIDE:
				return "REFERENCE_OVERRIDE";
			case REFERENCE_TEMPLATE_ARGUMENT:
				return "REFERENCE_TEMPLATE_ARGUMENT";
			case REFERENCE_TYPE_ARGUMENT:
				return "REFERENCE_TYPE_ARGUMENT";
			case REFERENCE_TEMPLATE_DEFAULT_ARGUMENT:
				return "REFERENCE_TEMPLATE_DEFAULT_ARGUMENT";
			case REFERENCE_TEMPLATE_SPECIALIZATION:
				return "REFERENCE_TEMPLATE_SPECIALIZATION";
			case REFERENCE_TEMPLATE_MEMBER_SPECIALIZATION:
				return "REFERENCE_TEMPLATE_MEMBER_SPECIALIZATION";
			case REFERENCE_INCLUDE:
				return "REFERENCE_INCLUDE";
			case REFERENCE_IMPORT:
				return "REFERENCE_IMPORT";
			case REFERENCE_MACRO_USAGE:
				return "REFERENCE_MACRO_USAGE";
			default:
				break;
			}
			return "REFERENCE_UNDEFINED";
		}
	};

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

		return TextAccess::createFromString(parserClient->m_dump);
	}
};