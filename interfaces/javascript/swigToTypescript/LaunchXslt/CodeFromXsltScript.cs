using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace LaunchXslt
{
    class XsltScripts
    {
        public static void TestMe()
        {
            var x = TranslateType("r.q(const).utilities::CommandLineParser");
            var y = TranslateType("utilities::StlIterator&lt;(std::vector&lt;(layers::Coordinate)>::const_iterator,layers::Coordinate)>");
            var z = TranslateType("r.std::vector&lt;(double)>");
        }


        public static string GetTemplateParameter(string s)
        {
            // example input: utilities::StlIterator&lt;(std::vector&lt;(layers::Coordinate)>::const_iterator,layers::Coordinate)>

            var afterLastColon = s.LastIndexOf("::") + 2;
            return s.Substring(afterLastColon, s.LastIndexOf(")") - afterLastColon);
        }


        public static string TryRegexpPrefix(string regexp, string encodedType)
        {
            var nsRx = new System.Text.RegularExpressions.Regex(regexp);
            var m = nsRx.Match(encodedType);
            if (m.Success) return m.Groups[1].Value;
            return null;
        }


        public static string TranslateType(string encodedType)
        {
            var ltIndex = encodedType.IndexOf("&lt;");
            if (ltIndex >= 0) // it's a template instance
            {
                var typeName = TranslateType(encodedType.Substring(0, ltIndex));
                var gtIndex = encodedType.LastIndexOf(")>");
                var parms = encodedType.Substring(ltIndex + 5, gtIndex - ltIndex - 5).Split(',').Select(s=>TranslateType(s));
                return typeName + "<" + String.Join(",", parms) + ">";
            }

            var rest =
                TryRegexpPrefix(@"r\.q\(const\)\.(.*)", encodedType) ??
                TryRegexpPrefix(@"r\.(.*)", encodedType) ??
                TryRegexpPrefix("^[A-Za-z0-9]+::(.*)", encodedType);
            if (rest != null) return TranslateType(rest);

            var ident = TryRegexpPrefix("^([A-Za-z0-9_]+)$", encodedType);
            if (ident != null) return ident;

            switch (encodedType)
            {
                case "void": return "void";
                case "bool": return "boolean";
                case "double": return "double";
                case "uint64_t": return "number";
                case "size_t": return "number";
                case "string": return "string";
            }
            return "any /*" + encodedType + "*/";
        }
    }
}
