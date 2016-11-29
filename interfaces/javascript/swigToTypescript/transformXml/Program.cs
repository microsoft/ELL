using System;
using System.IO;
using System.Xml;
using System.Xml.XPath;
using System.Xml.Xsl;

public class Program
{
    /// <summary>
    /// Transforms an XML file via an XSLT script
    /// </summary>
    public static void Main(string[] args)
    {
        if (args.Length != 3)
        {
            Console.WriteLine("usage: {0} [xml file] [xsl file] [output file]", System.AppDomain.CurrentDomain.FriendlyName);
            return;
        }
        var settings = new XsltSettings(enableDocumentFunction: false, enableScript: true);
        var xslt = new XslCompiledTransform();
        xslt.Load(args[1], settings, new XmlUrlResolver());
        xslt.Transform(args[0], args[2]);
    }
}

