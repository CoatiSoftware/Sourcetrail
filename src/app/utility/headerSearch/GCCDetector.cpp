#include "utility/headerSearch/GCCDetector.h"
#include "utility/headerSearch/StandardHeaderDetection.h"

GCCDetector::GCCDetector()
{

}

GCCDetector::~GCCDetector()
{
}

static StandardHeaderDetection::Add<GCCDetector> gcc("gcc");
static StandardHeaderDetection::Add<GCCDetector> clang("clang");
static StandardHeaderDetection::Add<GCCDetector> fakecomplier("fakecompiler");

