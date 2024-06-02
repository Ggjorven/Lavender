#include <Insight/Insight.hpp>

#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    Insight::Dll dll = Insight::Dll("../../bin/Debug-windows/Dll/Dll.dll");
    Insight::Internal::Classes& classes = dll.GetClasses();

    for (auto& [name, info] : classes.GetClasses())
    {
        Insight::DefaultVariableCollection vars = {};
        
        void* cls = dll.CreateClass(name);

        // Variables
        auto varInterfaces = dll.GetVariables(name, cls);
        vars.Add(varInterfaces);

        // Functions // Note(Jorben): A lot more manual (also on Dll side)
        typedef int (*FuncType)(void* instance, int ret);
        FuncType func = dll.GetCustomFunction<FuncType>("cFunc");
        int value = func(cls, 69);

        std::cout << "-------------------------" << std::endl;
        std::cout << "-- Class: " << name << std::endl;
        std::cout << "-------------------------" << std::endl;
        vars.PrintInfo();
        std::cout << "-------------------------" << std::endl;
        for (auto& func : info.Functions)
            std::cout << "Function by name \'" << func.Name << "\' found. Signature: " << func.Signature << std::endl;
        std::cout << "-------------------------" << std::endl;
    }

    return 0;
}