// File: RoslynCompiler.cs
using System.Collections.Generic;
using System.IO;
using System.Linq;
using Microsoft.CodeAnalysis;
using Microsoft.CodeAnalysis.CSharp;
namespace Borealis
{
    public class RoslynCompiler
    {
        public RoslynCompiler()
        {
            Debug.Log("Created compiler");
        }
        /*!***********************************************************************
            \brief
                Compiles a C# code into an assembly.
            \param code
                The C# code to compile.
            \param assemblyName
                The name of the assembly to create.
        *************************************************************************/
        public byte[] CompileCode(IEnumerable<string> filePaths, string assemblyName, out bool success)
        {
            var syntaxTrees = filePaths.Select(filePath =>
            {
                string code = File.ReadAllText(filePath);
                return CSharpSyntaxTree.ParseText(code);
            }).ToList();

            // Create the CSharpCompilation with multiple syntax trees
            var compilation = CSharpCompilation.Create(
                assemblyName,
                syntaxTrees,
                new[] {
                    MetadataReference.CreateFromFile(typeof(object).Assembly.Location),
                    MetadataReference.CreateFromFile(typeof(System.Linq.Enumerable).Assembly.Location),
                    MetadataReference.CreateFromFile(typeof(List<>).Assembly.Location),

                    MetadataReference.CreateFromFile("resources/Scripts/Core/BorealisScriptCore.dll")
                },
                new CSharpCompilationOptions(OutputKind.DynamicallyLinkedLibrary));


            using (var dllStream = new MemoryStream())
            {
                // Compile the syntax trees into the DLL
                var emitResult = compilation.Emit(dllStream);
                if (!emitResult.Success)
                {
                    foreach (var diagnostic in emitResult.Diagnostics)
                    {
                        if (diagnostic.Severity == DiagnosticSeverity.Info)
                        {
                            Debug.Log(diagnostic.ToString());
                        }
                        else if (diagnostic.Severity == DiagnosticSeverity.Warning)
                        {
                            Debug.LogWarning(diagnostic.ToString());
                        }
                        else if (diagnostic.Severity == DiagnosticSeverity.Error)
                        {
                            Debug.LogError(diagnostic.ToString());
                        }

                    }
                    success = false;
                    return null;
                }
                success = true;
                return dllStream.ToArray();
            }
        }

    }
}
