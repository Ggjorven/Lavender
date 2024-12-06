#include <Insight/Insight.hpp>

class Test
{
public:
    Test() = default;
    virtual ~Test() = default;

    [[ InsightAttr::ExportMemFunc(public, Test, Func, int, int ret) ]]
    int Func(int ret) const
    {
        return ret;
    }

private:
    [[ InsightAttr::ExportField(private, Test, int, abc) ]]
    int abc = 0;

} InsightClass(Test);

InsightExportField(private, Test, int, abc);

INSIGHT_FUNC_EXPORT int cFunc(Test* instance, int ret)
{
    return instance->Func(ret);
}
