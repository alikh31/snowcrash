//
//  test-ParametersParser.cc
//  snowcrash
//
//  Created by Zdenek Nemec on 9/1/13.
//  Copyright (c) 2013 Apiary Inc. All rights reserved.
//

#include <iterator>
#include "catch.hpp"
#include "Fixture.h"
#include "ParametersParser.h"

using namespace snowcrash;
using namespace snowcrashtest;

MarkdownBlock::Stack snowcrashtest::CanonicalParametersFixture()
{
    //R"(
    //+ Parameters
    //
    //  <see CanonicalParameterDefinitionFixture()>
    //)";
    
    MarkdownBlock::Stack markdown;
    // Parameters BEGIN
    markdown.push_back(MarkdownBlock(ListBlockBeginType, SourceData(), 0, SourceDataBlock()));
    markdown.push_back(MarkdownBlock(ListItemBlockBeginType, SourceData(), 0, SourceDataBlock()));
    markdown.push_back(MarkdownBlock(ParagraphBlockType, "Parameters", 0, MakeSourceDataBlock(0, 1)));
    
    // Inject parameter definiton
    MarkdownBlock::Stack parameterDefinition = CanonicalParameterDefinitionFixture();
    markdown.insert(markdown.end(), parameterDefinition.begin(), parameterDefinition.end());
    
    parameterDefinition[2].content = "limit";
    markdown.insert(markdown.end(), parameterDefinition.begin(), parameterDefinition.end());
    
    // Parameters END
    markdown.push_back(MarkdownBlock(ListItemBlockEndType, SourceData(), 0, MakeSourceDataBlock(15, 1)));
    markdown.push_back(MarkdownBlock(ListBlockEndType, SourceData(), 0,MakeSourceDataBlock(16, 1)));
    
    return markdown;
}

TEST_CASE("Parameters block classifier", "[parameters][classifier]")
{
    MarkdownBlock::Stack markdown = CanonicalParametersFixture();
    
    REQUIRE(markdown.size() == 57);
    
    BlockIterator cur = markdown.begin();
    
    // ListBlockBeginType
    REQUIRE(ClassifyBlock<ParameterCollection>(cur, markdown.end(), UndefinedSection) == ParametersSection);
    REQUIRE(ClassifyBlock<ParameterCollection>(cur, markdown.end(), ParametersSection) == UndefinedSection);
    
    ++cur; // ListItemBlockBeginType
    REQUIRE(ClassifyBlock<ParameterCollection>(cur, markdown.end(), UndefinedSection) == ParametersSection);
    REQUIRE(ClassifyBlock<ParameterCollection>(cur, markdown.end(), ParametersSection) == UndefinedSection);
    
    ++cur; // ParagraphBlockType
    REQUIRE(ClassifyBlock<ParameterCollection>(cur, markdown.end(), UndefinedSection) == UndefinedSection);
    REQUIRE(ClassifyBlock<ParameterCollection>(cur, markdown.end(), ParametersSection) == UndefinedSection);
    
    ++cur; // ListBlockBeginType
    REQUIRE(ClassifyBlock<ParameterCollection>(cur, markdown.end(), UndefinedSection) == UndefinedSection);
    REQUIRE(ClassifyBlock<ParameterCollection>(cur, markdown.end(), ParametersSection) == ParameterDefinitionSection);
}

TEST_CASE("Parse canonical parameters", "[parameters]")
{
    MarkdownBlock::Stack markdown = CanonicalParametersFixture();
    ParameterCollection parameters;
    BlueprintParserCore parser(0, SourceDataFixture, Blueprint());
    ParseSectionResult result = ParametersParser::Parse(markdown.begin(), markdown.end(), parser, parameters);
    
    REQUIRE(result.first.error.code == Error::OK);
    REQUIRE(result.first.warnings.empty());
    
    const MarkdownBlock::Stack &blocks = markdown;
    REQUIRE(std::distance(blocks.begin(), result.second) == 57);
    
    REQUIRE(parameters.size() == 2);

    REQUIRE(parameters[0].name == "id");
    REQUIRE(parameters[0].description == "2");
    
    REQUIRE(parameters[1].name == "limit");
    REQUIRE(parameters[1].description == "2");
}

TEST_CASE("Parse description parameter only", "[parameters]")
{
    //# GET /1
    //
    //+ Parameters
    //
    //    + param1
    //
    //      A
    
    MarkdownBlock::Stack markdown;
    markdown.push_back(MarkdownBlock(ListBlockBeginType, SourceData(), 0, SourceDataBlock()));
    markdown.push_back(MarkdownBlock(ListItemBlockBeginType, SourceData(), 0, SourceDataBlock()));
    markdown.push_back(MarkdownBlock(ParagraphBlockType, "Parameters", 0, MakeSourceDataBlock(0, 1)));
    markdown.push_back(MarkdownBlock(ListBlockBeginType, SourceData(), 0, SourceDataBlock()));
    markdown.push_back(MarkdownBlock(ListItemBlockBeginType, SourceData(), 0, SourceDataBlock()));
    markdown.push_back(MarkdownBlock(ParagraphBlockType, "Param1", 0, MakeSourceDataBlock(1, 1)));
    markdown.push_back(MarkdownBlock(ParagraphBlockType, "A", 0, MakeSourceDataBlock(2, 1)));
    markdown.push_back(MarkdownBlock(ListItemBlockEndType, SourceData(), 0, MakeSourceDataBlock(3, 1)));
    markdown.push_back(MarkdownBlock(ListBlockEndType, SourceData(), 0, MakeSourceDataBlock(4, 1)));
    markdown.push_back(MarkdownBlock(ListItemBlockEndType, SourceData(), 0, MakeSourceDataBlock(5, 1)));
    markdown.push_back(MarkdownBlock(ListBlockEndType, SourceData(), 0, MakeSourceDataBlock(6, 1)));
    
    
    ParameterCollection parameters;
    BlueprintParserCore parser(0, SourceDataFixture, Blueprint());
    ParseSectionResult result = ParametersParser::Parse(markdown.begin(), markdown.end(), parser, parameters);
    
    REQUIRE(result.first.error.code == Error::OK);
    REQUIRE(result.first.warnings.empty());
    
    const MarkdownBlock::Stack &blocks = markdown;
    REQUIRE(std::distance(blocks.begin(), result.second) == 11);
    
    REQUIRE(parameters.size() == 1);
    
    REQUIRE(parameters[0].name == "Param1");
    REQUIRE(parameters[0].description == "2");
    REQUIRE(parameters[0].required == true);
    REQUIRE(parameters[0].type.empty());
    REQUIRE(parameters[0].defaultValue.empty());
    REQUIRE(parameters[0].exampleValue.empty());
    REQUIRE(parameters[0].values.empty());
}

TEST_CASE("Parse multiple parameters", "[parameters][now]")
{
    //+ Parameters
    //
    //    + param1
    //
    //        A
    //
    //        + Optional
    //
    //
    //    + param2
    //
    //        B
    //
    //        + Example: `B-2`
    //
    //    + param3
    
    MarkdownBlock::Stack markdown;
    markdown.push_back(MarkdownBlock(ListBlockBeginType, SourceData(), 0, SourceDataBlock()));
    markdown.push_back(MarkdownBlock(ListItemBlockBeginType, SourceData(), 0, SourceDataBlock()));
    markdown.push_back(MarkdownBlock(ParagraphBlockType, "Parameters", 0, MakeSourceDataBlock(0, 1)));
    markdown.push_back(MarkdownBlock(ListBlockBeginType, SourceData(), 0, SourceDataBlock()));
    
    // Param 1
    markdown.push_back(MarkdownBlock(ListItemBlockBeginType, SourceData(), 0, SourceDataBlock()));
    markdown.push_back(MarkdownBlock(ParagraphBlockType, "Param1", 0, MakeSourceDataBlock(1, 1)));
    markdown.push_back(MarkdownBlock(ParagraphBlockType, "A", 0, MakeSourceDataBlock(2, 1)));

    // Optional
    markdown.push_back(MarkdownBlock(ListBlockBeginType, SourceData(), 0, SourceDataBlock()));
    markdown.push_back(MarkdownBlock(ListItemBlockBeginType, SourceData(), 0, SourceDataBlock()));
    markdown.push_back(MarkdownBlock(ListItemBlockEndType, "Optional\n", 0, MakeSourceDataBlock(3, 1)));
    markdown.push_back(MarkdownBlock(ListBlockEndType, SourceData(), 0, MakeSourceDataBlock(4, 1)));
    
    markdown.push_back(MarkdownBlock(ListItemBlockEndType, SourceData(), 0, MakeSourceDataBlock(5, 1)));
    
    // Param 2
    markdown.push_back(MarkdownBlock(ListItemBlockBeginType, SourceData(), 0, SourceDataBlock()));
    markdown.push_back(MarkdownBlock(ParagraphBlockType, "Param2", 0, MakeSourceDataBlock(6, 1)));
    markdown.push_back(MarkdownBlock(ParagraphBlockType, "B", 0, MakeSourceDataBlock(7, 1)));
    
    markdown.push_back(MarkdownBlock(ListBlockBeginType, SourceData(), 0, SourceDataBlock()));
    markdown.push_back(MarkdownBlock(ListItemBlockBeginType, SourceData(), 0, SourceDataBlock()));
    markdown.push_back(MarkdownBlock(ListItemBlockEndType, "Example: `B-2`\n", 0, MakeSourceDataBlock(8, 1)));
    markdown.push_back(MarkdownBlock(ListBlockEndType, SourceData(), 0, MakeSourceDataBlock(9, 1)));
    
    markdown.push_back(MarkdownBlock(ListItemBlockEndType, SourceData(), 0, MakeSourceDataBlock(10, 1)));
    
    // Param 2
    markdown.push_back(MarkdownBlock(ListItemBlockBeginType, SourceData(), 0, SourceDataBlock()));
    markdown.push_back(MarkdownBlock(ParagraphBlockType, "Param3", 0, MakeSourceDataBlock(11, 1)));
    
    markdown.push_back(MarkdownBlock(ListItemBlockEndType, SourceData(), 0, MakeSourceDataBlock(12, 1)));
    markdown.push_back(MarkdownBlock(ListBlockEndType, SourceData(), 0, MakeSourceDataBlock(13, 1)));
    markdown.push_back(MarkdownBlock(ListItemBlockEndType, SourceData(), 0, MakeSourceDataBlock(14, 1)));
    markdown.push_back(MarkdownBlock(ListBlockEndType, SourceData(), 0, MakeSourceDataBlock(15, 1)));
    
    ParameterCollection parameters;
    BlueprintParserCore parser(0, SourceDataFixture, Blueprint());
    ParseSectionResult result = ParametersParser::Parse(markdown.begin(), markdown.end(), parser, parameters);
    
    REQUIRE(result.first.error.code == Error::OK);
    REQUIRE(result.first.warnings.empty());
    
    const MarkdownBlock::Stack &blocks = markdown;
    REQUIRE(std::distance(blocks.begin(), result.second) == 26);
    
    REQUIRE(parameters.size() == 3);
    
    REQUIRE(parameters[0].name == "Param1");
    REQUIRE(parameters[0].description == "2");
    REQUIRE(parameters[0].required == false);
    REQUIRE(parameters[0].type.empty());
    REQUIRE(parameters[0].defaultValue.empty());
    REQUIRE(parameters[0].exampleValue.empty());
    REQUIRE(parameters[0].values.empty());
    
    REQUIRE(parameters[1].name == "Param2");
    REQUIRE(parameters[1].description == "7");
    REQUIRE(parameters[1].required == true);
    REQUIRE(parameters[1].type.empty());
    REQUIRE(parameters[1].defaultValue.empty());
    REQUIRE(parameters[1].exampleValue == "B-2");
    REQUIRE(parameters[1].values.empty());
    
    REQUIRE(parameters[2].name == "Param3");
    REQUIRE(parameters[2].description.empty());
    REQUIRE(parameters[2].required == true);
    REQUIRE(parameters[2].type.empty());
    REQUIRE(parameters[2].defaultValue.empty());
    REQUIRE(parameters[2].exampleValue.empty());
    REQUIRE(parameters[2].values.empty());
    
}



