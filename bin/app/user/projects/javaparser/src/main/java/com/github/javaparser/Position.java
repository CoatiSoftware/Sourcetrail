/*
 * Copyright (C) 2007-2010 Júlio Vilmar Gesser.
 * Copyright (C) 2011, 2013-2016 The JavaParser Team.
 *
 * This file is part of JavaParser.
 * 
 * JavaParser can be used either under the terms of
 * a) the GNU Lesser General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * b) the terms of the Apache License 
 *
 * You should have received a copy of both licenses in LICENCE.LGPL and
 * LICENCE.APACHE. Please refer to those files for details.
 *
 * JavaParser is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 */

package com.github.javaparser;

import com.github.javaparser.ast.Node;

/**
 * A position in a source file. Lines and columns start counting at 1.
 */
public class Position implements Comparable<Position> {
	public final int line;
	public final int column;

	public static final Position ABSOLUTE_START = new Position(Node.ABSOLUTE_BEGIN_LINE, -1);
	public static final Position ABSOLUTE_END = new Position(Node.ABSOLUTE_END_LINE, -1);

	/**
	 * The first position in the file
	 */
	public static final Position HOME = new Position(1, 1);
	public static final Position UNKNOWN = new Position(0, 0);

	public Position(int line, int column) {
		if (line < Node.ABSOLUTE_END_LINE) {
			throw new IllegalArgumentException("Can't position at line " + line);
		}
		if (column < -1) {
			throw new IllegalArgumentException("Can't position at column " + column);
		}
		this.line = line;
		this.column = column;
	}

	/**
	 * Convenient factory method.
	 */
	public static Position pos(int line, int column) {
		return new Position(line, column);
	}

	public Position withColumn(int column) {
		return new Position(this.line, column);
	}

	public Position withLine(int line) {
		return new Position(line, this.column);
	}

	/**
	 * Check if the position is usable. Does not know what it is pointing at, so it can't check if the position is after the end of the source.
	 */
	public boolean valid() {
		return line > 0 && column > 0;
	}

	public boolean invalid() {
		return !valid();
	}

	public Position orIfInvalid(Position anotherPosition) {
		if (valid()) {
			return this;
		}
		return anotherPosition;
	}
	
	public boolean isAfter(Position position) {
		if (position.line == Node.ABSOLUTE_BEGIN_LINE) return true;
		if (line > position.line) {
			return true;
		} else if (line == position.line) {
			return column > position.column;
		}
		return false;

	}

	public boolean isBefore(Position position) {
		if (position.line == Node.ABSOLUTE_END_LINE) return true;
		if (line < position.line) {
			return true;
		} else if (line == position.line) {
			return column < position.column;
		}
		return false;
	}

	@Override
	public int compareTo(Position o) {
		if (isBefore(o)) {
			return -1;
		}
		if (isAfter(o)) {
			return 1;
		}
		return 0;
	}

	@Override
	public boolean equals(Object o) {
		if (this == o) return true;
		if (o == null || getClass() != o.getClass()) return false;

		Position position = (Position) o;

		return line == position.line && column == position.column;
	}

	@Override
	public int hashCode() {
		return 31 * line + column;
	}

	@Override
	public String toString() {
		return "(line " + line + ",col " + column + ")";
	}
}
