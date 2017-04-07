using System;

namespace CoatiSoftware.SourcetrailPlugin.Utility
{
    class StringUtility
    {
        static public Tuple<int, int> FindFirstRange(string text, string startTag, string endTag)
        {
            if(startTag.Length > 0 && endTag.Length > 0 && (startTag.Length + endTag.Length) < text.Length)
            {
                int start = text.IndexOf(startTag);
                int end = text.IndexOf(endTag);

                // just check wheter both, start and end, are valid and the end tag occurs after the start tag
                if(start > -1 && end > -1 && end > start)
                {
                    return new Tuple<int, int>(start, end);
                }
            }

            return null;
        }

        static public int GetMatchingCharsFromStart(string a, string b)
        {
            int matchingChars = 0;

            if (a != string.Empty)
            {
                a = a.ToLower();
            }
            else
            {
                return matchingChars;
            }

            if (b != string.Empty)
            {
                b = b.ToLower();
            }
            else
            {
                return matchingChars;
            }

            for (int i = 0; i < Math.Min(a.Length, b.Length); i++)
            {
                if (!char.Equals(a[i], b[i]))
                {
                    break;
                }
                else
                {
                    matchingChars++;
                }
            }

            return matchingChars;
        }
    }
}
