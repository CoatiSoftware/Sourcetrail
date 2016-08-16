package io.coati;

import java.util.Arrays;
import java.util.List;

public class FileContent 
{
	public class Location
	{
		public int line;
		public int column;
		
		Location(int line, int column)
		{
			this.line = line;
			this.column = column;
		}
	}
	
	private List<String> m_lines;
	
	public FileContent(String text)
	{
		m_lines = Arrays.asList(text.split("\\r?\\n"));
	}
	
	public Location find(String s)
	{
		return find(s, 1, 1);
	}
	
	public Location find(String s, int fromLine, int fromColumn)
	{
		int lineIndex = fromLine - 1;
		int startColumn = fromColumn - 1;
		int column = -1;
		while (lineIndex < m_lines.size())
		{
			column = m_lines.get(lineIndex).indexOf(s, startColumn);
			if (column != -1)
			{
				return new Location(lineIndex + 1, column + 1);
			}
			startColumn = 0;
			lineIndex++;
		}
		return new Location(0, 0);
	}
}
