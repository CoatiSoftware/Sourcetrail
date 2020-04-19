package com.sourcetrail;

public class Range
{
	public Position begin = new Position();
	public Position end = new Position();

	public Range() {}

	public Range(int beginLine, int beginColumn, int endLine, int endColumn)
	{
		this.begin.line = beginLine;
		this.begin.column = beginColumn;
		this.end.line = endLine;
		this.end.column = endColumn;
	}

	public Range(Position begin, Position end)
	{
		this.begin.line = begin.line;
		this.begin.column = begin.column;
		this.end.line = end.line;
		this.end.column = end.column;
	}
}
