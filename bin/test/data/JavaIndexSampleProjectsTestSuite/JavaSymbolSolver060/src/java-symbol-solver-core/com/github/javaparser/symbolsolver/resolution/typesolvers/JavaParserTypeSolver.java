/*
 * Copyright 2016 Federico Tomassetti
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.github.javaparser.symbolsolver.resolution.typesolvers;

import com.github.javaparser.JavaParser;
import com.github.javaparser.ast.CompilationUnit;
import com.github.javaparser.symbolsolver.javaparser.Navigator;
import com.github.javaparser.symbolsolver.javaparsermodel.JavaParserFacade;
import com.github.javaparser.symbolsolver.model.declarations.ReferenceTypeDeclaration;
import com.github.javaparser.symbolsolver.model.resolution.SymbolReference;
import com.github.javaparser.symbolsolver.model.resolution.TypeSolver;

import java.io.File;
import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Optional;

/**
 * @author Federico Tomassetti
 */
public class JavaParserTypeSolver implements TypeSolver {

    private File srcDir;

    private TypeSolver parent;

    private Map<String, CompilationUnit> parsedFiles = new HashMap<String, CompilationUnit>();
    private Map<String, List<CompilationUnit>> parsedDirectories = new HashMap<>();
    private Map<String, ReferenceTypeDeclaration> foundTypes=new HashMap<>();

    public JavaParserTypeSolver(File srcDir) {
        this.srcDir = srcDir;
    }

    @Override
    public String toString() {
        return "JavaParserTypeSolver{" +
                "srcDir=" + srcDir +
                ", parent=" + parent +
                '}';
    }

    @Override
    public TypeSolver getParent() {
        return parent;
    }

    @Override
    public void setParent(TypeSolver parent) {
        this.parent = parent;
    }


    private CompilationUnit parse(File srcFile) throws FileNotFoundException {
        if (!parsedFiles.containsKey(srcFile.getAbsolutePath())) {
            parsedFiles.put(srcFile.getAbsolutePath(), JavaParser.parse(srcFile));
        }
        return parsedFiles.get(srcFile.getAbsolutePath());
    }

    private List<CompilationUnit> parseDirectory(File srcDirectory) throws FileNotFoundException {
        if (!parsedDirectories.containsKey(srcDirectory.getAbsolutePath())) {
            List<CompilationUnit> units = new ArrayList<>();
            File[] files = srcDirectory.listFiles();
            if (files == null) throw new FileNotFoundException(srcDirectory.getAbsolutePath());
            for (File file : files) {
                if (file.getName().toLowerCase().endsWith(".java")) {
                    units.add(parse(file));
                }
            }
            parsedDirectories.put(srcDirectory.getAbsolutePath(), units);
        }

        return parsedDirectories.get(srcDirectory.getAbsolutePath());
    }

    @Override
    public SymbolReference<ReferenceTypeDeclaration> tryToSolveType(String name) {
        if (!srcDir.exists() || !srcDir.isDirectory()) {
            throw new IllegalStateException("SrcDir does not exist or is not a directory: " + srcDir.getAbsolutePath());
        }

        // TODO support enums
        // TODO support interfaces
        if (foundTypes.containsKey(name))
        	return SymbolReference.solved(foundTypes.get(name));

        SymbolReference<ReferenceTypeDeclaration> result = tryToSolveTypeUncached(name);
        if (result.isSolved()) {
            foundTypes.put(name, result.getCorrespondingDeclaration());
        }

        return result;
    }

    private SymbolReference<ReferenceTypeDeclaration> tryToSolveTypeUncached(String name) {
        String[] nameElements = name.split("\\.");

        for (int i = nameElements.length; i > 0; i--) {
            String filePath = srcDir.getAbsolutePath();
            for (int j = 0; j < i; j++) {
                filePath += "/" + nameElements[j];
            }
            filePath += ".java";

            String typeName = "";
            for (int j = i - 1; j < nameElements.length; j++) {
                if (j != i - 1) {
                    typeName += ".";
                }
                typeName += nameElements[j];
            }

            File srcFile = new File(filePath);
            try {
                CompilationUnit compilationUnit = parse(srcFile);
                Optional<com.github.javaparser.ast.body.TypeDeclaration<?>> astTypeDeclaration = Navigator.findType(compilationUnit, typeName);
                if (astTypeDeclaration.isPresent()) {
                    return SymbolReference.solved(JavaParserFacade.get(this).getTypeDeclaration(astTypeDeclaration.get()));
                }
            } catch (FileNotFoundException e) {
                // Ignore
            }

            try {
                List<CompilationUnit> compilationUnits = parseDirectory(srcFile.getParentFile());
                for (CompilationUnit compilationUnit : compilationUnits) {
                    Optional<com.github.javaparser.ast.body.TypeDeclaration<?>> astTypeDeclaration = Navigator.findType(compilationUnit, typeName);
                    if (astTypeDeclaration.isPresent()) {
                        return SymbolReference.solved(JavaParserFacade.get(this).getTypeDeclaration(astTypeDeclaration.get()));
                    }
                }
            } catch (FileNotFoundException e) {
                // Ignore
            }
        }

        return SymbolReference.unsolved(ReferenceTypeDeclaration.class);
    }

}
