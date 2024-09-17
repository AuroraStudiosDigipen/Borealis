// File: RoslynCompiler.cs
using System;
using System.IO;
using System.Runtime.CompilerServices;
using Microsoft.CodeAnalysis;
using Microsoft.CodeAnalysis.CSharp;
namespace Borealis
{
    public class RoslynCompiler
    {
        public RoslynCompiler()
        {
            GameObject gameObject = new GameObject();
            gameObject.AddComponent<ExampleScript>();
        }
        /*!***********************************************************************
            \brief
                Compiles a C# code into an assembly.
            \param code
                The C# code to compile.
            \param assemblyName
                The name of the assembly to create.
        *************************************************************************/
        public byte[] CompileCode(string code, string assemblyName)
        {
            var syntaxTree = CSharpSyntaxTree.ParseText(code);
            CSharpCompilation compilation = CSharpCompilation.Create(
                assemblyName,
                new[] { syntaxTree },
                new[] { MetadataReference.CreateFromFile(typeof(object).Assembly.Location) },
                new CSharpCompilationOptions(OutputKind.DynamicallyLinkedLibrary));

            using (var dllStream = new MemoryStream())
            {
                var emitResult = compilation.Emit(dllStream);
                if (!emitResult.Success)
                {
                }
                return dllStream.ToArray();
            }

        }
    }
}
