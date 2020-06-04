package com.sourcetrail;

import java.util.Arrays;
import java.util.List;

public class FileContent
{
	private List<String> m_lines;

	public FileContent(String text)
	{
		m_lines = Arrays.asList(text.split("\\r?\\n"));
	}

	public Position findStartPosition(String s)
	{
		return findStartPosition(s, new Position(1, 1));
	}

	public Position findStartPosition(String s, Position from)
	{
		int lineIndex = from.line - 1;
		int startColumn = from.column - 1;
		int column = -1;
		while (lineIndex < m_lines.size())
		{
			column = m_lines.get(lineIndex).indexOf(s, startColumn);
			if (column != -1)
			{
				return new Position(lineIndex + 1, column + 1);
			}
			startColumn = 0;
			lineIndex++;
		}
		return new Position(0, 0);
	}

	public Range findRange(String s)
	{
		return findRange(s, new Position(1, 1));
	}

	public Range findRange(String s, Position from)
	{
		Position startPosition = findStartPosition(s, from);

		return new Range(
			startPosition, new Position(startPosition.line, startPosition.column + s.length() - 1));
	}
}
