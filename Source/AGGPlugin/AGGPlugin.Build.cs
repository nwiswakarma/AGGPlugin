////////////////////////////////////////////////////////////////////////////////
//
// MIT License
// 
// Copyright (c) 2018-2019 Nuraga Wiswakarma
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
////////////////////////////////////////////////////////////////////////////////
//

using System.IO;
using System.Collections;
using UnrealBuildTool;

public class AGGPlugin: ModuleRules
{
    public AGGPlugin(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
                "Engine"
            });

        PrivateIncludePaths.Add("AGGPlugin/Private");

        string ThirdPartyPath = Path.Combine(ModuleDirectory, "../../ThirdParty");

        // -- AGG include and lib path

        string AGGPath = Path.Combine(ThirdPartyPath, "AGG");
        string AGGInclude = Path.Combine(AGGPath, "include");
        string AGGLib = Path.Combine(AGGPath, "lib");

        PublicIncludePaths.Add(Path.GetFullPath(AGGInclude));
        PublicLibraryPaths.Add(Path.GetFullPath(AGGLib));

        PublicAdditionalLibraries.Add("AGG.lib");
    }
}
