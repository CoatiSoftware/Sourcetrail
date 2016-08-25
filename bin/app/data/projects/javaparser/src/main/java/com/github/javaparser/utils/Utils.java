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

package com.github.javaparser.utils;

import java.io.IOException;
import java.io.Reader;
import java.util.ArrayList;
import java.util.Collection;
import java.util.List;

/**
 * Any kind of utility.
 *
 * @author Federico Tomassetti
 */
public class Utils {
	public static <T> List<T> ensureNotNull(List<T> list) {
		return list == null ? new ArrayList<T>() : list;
	}

	public static <E> boolean isNullOrEmpty(Collection<E> collection) {
		return collection == null || collection.isEmpty();
	}

	/**
	 * @return string with ASCII characters 10 and 13 replaced by the text "\n" and "\r".
	 */
	public static String escapeEndOfLines(String string) {
		StringBuilder escapedString = new StringBuilder();
		for (char c : string.toCharArray()) {
			switch (c) {
				case '\n':
					escapedString.append("\\n");
					break;
				case '\r':
					escapedString.append("\\r");
					break;
				default:
					escapedString.append(c);
			}
		}
		return escapedString.toString();
	}

	public static String readerToString(Reader reader) throws IOException {
		char[] arr = new char[8*1024]; // 8K at a time
		StringBuilder buf = new StringBuilder();
		int numChars;

		while ((numChars = reader.read(arr, 0, arr.length)) > 0) {
			buf.append(arr, 0, numChars);
		}

		return buf.toString();
	}

}
