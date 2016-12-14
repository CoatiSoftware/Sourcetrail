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

import java.io.*;

import com.github.javaparser.ast.CompilationUnit;
import com.github.javaparser.ast.ImportDeclaration;
import com.github.javaparser.ast.body.BodyDeclaration;
import com.github.javaparser.ast.expr.AnnotationExpr;
import com.github.javaparser.ast.expr.Expression;
import com.github.javaparser.ast.stmt.BlockStmt;
import com.github.javaparser.ast.stmt.Statement;

import java.nio.charset.Charset;
import java.util.List;

import static com.github.javaparser.utils.Utils.readerToString;

/**
 * Parse Java source code and creates Abstract Syntax Tree classes.
 *
 * @author Júlio Vilmar Gesser
 */
public final class JavaParser {
    private static final CommentsInserter commentsInserter = new CommentsInserter();
    private static final Charset UTF8 = Charset.forName("utf-8");
    
    private ASTParser astParser = null;
    private Provider provider = null;

    /**
     * Instantiate the parser. Note that parsing can also be done with the static methods on this class.
     * Creating an instance will reduce setup time between parsing files.
     */
    public JavaParser() {
        // No specific constructors for this class.
    }

    public JavaParser setSource(Provider provider) {
        this.provider=provider;
        if(astParser==null) {
            astParser = new ASTParser(provider);
        }else{
            astParser.ReInit(provider);
        }
        return this;
    }

    public JavaParser setSource(Reader reader) {
        return setSource(new StreamProvider(reader));
    }

    public JavaParser setSource(InputStream input) throws IOException {
        return setSource(new StreamProvider(input));
    }

    public JavaParser setSource(InputStream input, Charset encoding) throws IOException {
        return setSource(new StreamProvider(input, encoding.name()));
    }

    public JavaParser setSource(File file) throws IOException {
        return setSource(new FileInputStream(file));
    }

    public JavaParser setSource(File file, Charset encoding) throws IOException {
        return setSource(new FileInputStream(file), encoding);
    }

    public JavaParser setSource(String source) {
        return setSource(new StringReader(source));
    }

    /**
     * Return the list of tokens that have been encountered while parsing code
     * using this parser.
     *
     * @return a list of tokens
     */
    public List<Token> getTokens() {
        checkSourceSet();
        return astParser.getTokens();
    }

    private void checkSourceSet() {
        if (astParser == null || provider == null) {
            throw new IllegalStateException("Please setSource() before doing any parsing.");
        }
    }

    /**
     * Parses the Java code and returns a {@link CompilationUnit} that
     * represents it.
     *
     * @return CompilationUnit representing the Java source code
     * @throws ParseException
     *             if the source code has parser errors
     */
    public CompilationUnit parse() throws ParseException {
        try {
            checkSourceSet();
            return astParser.CompilationUnit();
        } finally {
            closeProvider();
        }
    }

    private void closeProvider() {
        try {
            checkSourceSet();
            provider.close();
        } catch (IOException e) {
            // Since we're done parsing and have our result, we don't care about any errors.
        }
    }

    /**
     * Parses the Java block and returns a {@link BlockStmt} that represents it.
     *
     * @return BlockStmt representing the Java block
     * @throws ParseException
     *             if the source code has parser errors
     */
    public BlockStmt parseBlock() throws ParseException {
        try{
            checkSourceSet();
            return astParser.Block();
        } finally {
            closeProvider();
        }
    }

    /**
     * Parses the Java block and returns a {@link List} of {@link Statement}s
     * that represents it.
     *
     * @return Statement representing the Java statement
     * @throws ParseException
     *             if the source code has parser errors
     */
    public List<Statement> parseStatements() throws ParseException {
        try {
            checkSourceSet();
            return astParser.Statements();
        } finally {
            closeProvider();
        }
    }

    /**
     * Parses the Java statement and returns a {@link Statement} that represents
     * it.
     *
     * @return Statement representing the Java statement
     * @throws ParseException
     *             if the source code has parser errors
     */
    public Statement parseStatement() throws ParseException {
        try {
            checkSourceSet();
            return astParser.Statement();
        } finally {
            closeProvider();
        }
    }

    /**
     * Parses the Java import and returns a {@link ImportDeclaration} that
     * represents it.
     *
     * @return ImportDeclaration representing the Java import declaration
     * @throws ParseException
     *             if the source code has parser errors
     */
    public ImportDeclaration parseImport() throws ParseException {
        try {
            checkSourceSet();
            return astParser.ImportDeclaration();
        } finally {
            closeProvider();
        }
    }

    /**
     * Parses the Java expression and returns a {@link Expression} that
     * represents it.
     *
     * @return Expression representing the Java expression
     * @throws ParseException
     *             if the source code has parser errors
     */
    public Expression parseExpression() throws ParseException {
        try {
            checkSourceSet();
            return astParser.Expression();
        } finally {
            closeProvider();
        }
    }

    /**
     * Parses the Java annotation and returns a {@link AnnotationExpr} that
     * represents it.
     *
     * @return AnnotationExpr representing the Java annotation
     * @throws ParseException
     *             if the source code has parser errors
     */
    public AnnotationExpr parseAnnotation() throws ParseException {
        try {
            checkSourceSet();
            return astParser.Annotation();
        } finally {
            closeProvider();
        }
    }

    /**
     * Parses the Java body declaration(e.g fields or methods) and returns a
     * {@link BodyDeclaration} that represents it.
     *
     * @return BodyDeclaration representing the Java body
     * @throws ParseException
     *             if the source code has parser errors
     */
    public BodyDeclaration<?> parseBodyDeclaration() throws ParseException {
        try {
            checkSourceSet();
            return astParser.AnnotationBodyDeclaration();
        } finally {
            closeProvider();
        }
    }

    /**
     * Parses a Java class body declaration(e.g fields or methods) and returns a
     * {@link BodyDeclaration} that represents it.
     *
     * @return BodyDeclaration representing the Java class body
     * @throws ParseException
     *             if the source code has parser errors
     */
    public BodyDeclaration<?> parseClassBodyDeclaration() throws ParseException {
        try {
            checkSourceSet();
            return astParser.ClassOrInterfaceBodyDeclaration(false);
        } finally {
            closeProvider();
        }
    }

    /**
     * Parses a Java interface body declaration(e.g fields or methods) and returns a
     * {@link BodyDeclaration} that represents it.
     *
     * @return BodyDeclaration representing the Java interface body
     * @throws ParseException
     *             if the source code has parser errors
     */
    public BodyDeclaration<?> parseInterfaceBodyDeclaration() throws ParseException {
        try {
            checkSourceSet();
            return astParser.ClassOrInterfaceBodyDeclaration(true);
        } finally {
            closeProvider();
        }
    }
    
    public static boolean getDoNotConsiderAnnotationsAsNodeStartForCodeAttribution() {
        return commentsInserter.getDoNotConsiderAnnotationsAsNodeStartForCodeAttribution();
    }

    public static void setDoNotConsiderAnnotationsAsNodeStartForCodeAttribution(boolean newValue) {
        commentsInserter.setDoNotConsiderAnnotationsAsNodeStartForCodeAttribution(newValue);
    }

    public static boolean getDoNotAssignCommentsPreceedingEmptyLines() {
        return commentsInserter.getDoNotAssignCommentsPreceedingEmptyLines();
    }

    public static void setDoNotAssignCommentsPreceedingEmptyLines(boolean newValue) {
        commentsInserter.setDoNotAssignCommentsPreceedingEmptyLines(newValue);
    }

    public static CompilationUnit parse(final InputStream in,
                                        final Charset encoding) throws ParseException {
        return parse(in,encoding,true);
    }

    /**
     * Parses the Java code contained in the {@link InputStream} and returns a
     * {@link CompilationUnit} that represents it.
     *
     * @param in
     *            {@link InputStream} containing Java source code
     * @param encoding
     *            encoding of the source code
     * @return CompilationUnit representing the Java source code
     * @throws ParseException
     *             if the source code has parser errors
     */
    public static CompilationUnit parse(final InputStream in,
                                        Charset encoding,
                                        boolean considerComments) throws ParseException {
        try {
            try (InputStreamReader inputStreamReader = new InputStreamReader(in, encoding)) {
                return parse(inputStreamReader, considerComments);
            }
        } catch (IOException ioe) {
            throw new ParseException(ioe.getMessage());
        }
    }

    /**
     * Parses the Java code contained in the {@link InputStream} and returns a
     * {@link CompilationUnit} that represents it.<br>
     * Note: Uses UTF-8 encoding
     * 
     * @param in
     *            {@link InputStream} containing Java source code
     * @return CompilationUnit representing the Java source code
     * @throws ParseException
     *             if the source code has parser errors
     */
    public static CompilationUnit parse(final InputStream in)
            throws ParseException {
        return parse(in, UTF8, true);
    }

    public static CompilationUnit parse(final File file, final Charset encoding)
            throws ParseException, IOException {
        return parse(file,encoding,true);
    }

    /**
     * Parses the Java code contained in a {@link File} and returns a
     * {@link CompilationUnit} that represents it.
     *
     * @param file
     *            {@link File} containing Java source code
     * @param encoding
     *            encoding of the source code
     * @return CompilationUnit representing the Java source code
     * @throws ParseException
     *             if the source code has parser errors
     */
    public static CompilationUnit parse(final File file, final Charset encoding, boolean considerComments)
            throws ParseException {
        try {
            try (FileInputStream in = new FileInputStream(file)) {
                return parse(in, encoding, considerComments);
            }
        } catch (IOException ioe) {
            throw new ParseException(ioe.getMessage());
        }
    }

    /**
     * Parses the Java code contained in a {@link File} and returns a
     * {@link CompilationUnit} that represents it.<br>
     * Note: Uses UTF-8 encoding
     * 
     *
     * @param file
     *            {@link File} containing Java source code
     * @return CompilationUnit representing the Java source code
     * @throws ParseException
     *             if the source code has parser errors
     * @throws IOException
     */
    public static CompilationUnit parse(final File file) throws ParseException,
            IOException {
        return parse(file, UTF8, true);
    }

    public static CompilationUnit parse(final Reader reader)
            throws ParseException {
        return parse(reader, true);
    }
    
    public static CompilationUnit parse(final Reader reader, boolean considerComments)
            throws ParseException {
        try {
            String comments = readerToString(reader);
            CompilationUnit cu = new JavaParser().setSource(comments).parse();
            if (considerComments){
                commentsInserter.insertComments(cu, comments);
            }
            return cu;
        } catch (IOException ioe){
            throw new ParseException(ioe.getMessage());
        }
    }

    /**
     * Parses the Java code contained in code and returns a
     * {@link CompilationUnit} that represents it.
     *
     * @param code Java source code
     * @param considerComments parse or ignore comments
     * @return CompilationUnit representing the Java source code
     * @throws ParseException if the source code has parser errors
     */
    public static CompilationUnit parse(String code, boolean considerComments) throws ParseException {
        return parse(new StringReader(code), considerComments);
    }

    /**
     * Parses the Java code contained in code and returns a
     * {@link CompilationUnit} that represents it.
     *
     * @param code Java source code
     * @return CompilationUnit representing the Java source code
     * @throws ParseException if the source code has parser errors
     */
    public static CompilationUnit parse(String code) throws ParseException {
        return parse(code, true);
    }

    /**
     * Parses the Java block contained in a {@link String} and returns a
     * {@link BlockStmt} that represents it.
     *
     * @param blockStatement
     *            {@link String} containing Java block code
     * @return BlockStmt representing the Java block
     * @throws ParseException
     *             if the source code has parser errors
     */
    public static BlockStmt parseBlock(final String blockStatement)
            throws ParseException {
        return new JavaParser().setSource(blockStatement).parseBlock();
    }

    /**
     * Parses the Java statement contained in a {@link String} and returns a
     * {@link Statement} that represents it.
     *
     * @param statement
     *            {@link String} containing Java statement code
     * @return Statement representing the Java statement
     * @throws ParseException
     *             if the source code has parser errors
     */
    public static Statement parseStatement(final String statement) throws ParseException {
        return new JavaParser().setSource(statement).parseStatement();
    }

    /**
     * Parses the Java statements contained in a {@link String} and returns a
     * list of {@link Statement} that represents it.
     *
     * @param statements
     *            {@link String} containing Java statements
     * @return list of Statement representing the Java statement
     * @throws ParseException
     *             if the source code has parser errors
     */
    public static List<Statement> parseStatements(final String statements) throws ParseException {
        return new JavaParser().setSource(statements).parseStatements();
    }

    /**
     * Parses the Java import contained in a {@link String} and returns a
     * {@link ImportDeclaration} that represents it.
     *
     * @param importDeclaration
     *            {@link String} containing Java import code
     * @return ImportDeclaration representing the Java import declaration
     * @throws ParseException
     *             if the source code has parser errors
     */
    public static ImportDeclaration parseImport(final String importDeclaration) throws ParseException {
        return new JavaParser().setSource(importDeclaration).parseImport();
    }

    /**
     * Parses the Java expression contained in a {@link String} and returns a
     * {@link Expression} that represents it.
     *
     * @param expression
     *            {@link String} containing Java expression
     * @return Expression representing the Java expression
     * @throws ParseException
     *             if the source code has parser errors
     */
    public static Expression parseExpression(final String expression) throws ParseException {
        return new JavaParser().setSource(expression).parseExpression();
    }

    /**
     * Parses the Java annotation contained in a {@link String} and returns a
     * {@link AnnotationExpr} that represents it.
     *
     * @param annotation
     *            {@link String} containing Java annotation
     * @return AnnotationExpr representing the Java annotation
     * @throws ParseException
     *             if the source code has parser errors
     */
    public static AnnotationExpr parseAnnotation(final String annotation) throws ParseException {
        return new JavaParser().setSource(annotation).parseAnnotation();
    }

    /**
     * Parses the Java body declaration(e.g fields or methods) contained in a
     * {@link String} and returns a {@link BodyDeclaration} that represents it.
     *
     * @param body
     *            {@link String} containing Java body declaration
     * @return BodyDeclaration representing the Java annotation
     * @throws ParseException
     *             if the source code has parser errors
     */
    public static BodyDeclaration<?> parseBodyDeclaration(final String body) throws ParseException {
        return new JavaParser().setSource(body).parseBodyDeclaration();
    }

    /**
     * Parses a Java class body declaration(e.g fields or methods) and returns a
     * {@link BodyDeclaration} that represents it.
     *
     * @param body the body of a class
     * @return BodyDeclaration representing the Java class body
     * @throws ParseException
     *             if the source code has parser errors
     */
    public static BodyDeclaration<?> parseClassBodyDeclaration(String body) throws ParseException {
        return new JavaParser().setSource(body).parseClassBodyDeclaration();
    }

    /**
     * Parses a Java interface body declaration(e.g fields or methods) and returns a
     * {@link BodyDeclaration} that represents it.
     *
     * @param body the body of an interface
     * @return BodyDeclaration representing the Java interface body
     * @throws ParseException
     *             if the source code has parser errors
     */
    public static BodyDeclaration parseInterfaceBodyDeclaration(String body) throws ParseException {
        return new JavaParser().setSource(body).parseInterfaceBodyDeclaration();
    }
}
